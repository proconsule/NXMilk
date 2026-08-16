#include "audioplayer.h"

#include <malloc.h>
#include <algorithm>
#include <cctype>

#define BUFFER_COUNT 5

// Returns true if filename has a .mid / .midi extension (case insensitive)
static bool IsMidiFile(const std::string &filename){
	std::string ext;
	size_t pos = filename.find_last_of('.');
	if(pos == std::string::npos) return false;
	ext = filename.substr(pos);
	std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
	return ext == ".mid" || ext == ".midi" || ext == ".rmi" || ext == ".kar";
}

void MidiAudioThread(void *arg);

void AudrenAudioThread(void *arg);
size_t append_audio(const void *buf, size_t size);
void write_audio(const void* buf, size_t size);

static const uint8_t m_sink_channels[] = { 0, 1 };
ssize_t free_wavebuf_index();

s16* m_decoded_buffer = nullptr;
void* mempool_ptr = nullptr;
void* current_pool_ptr = nullptr;
    
AudioDriver m_driver;
AudioDriverWaveBuf m_wavebufs[BUFFER_COUNT];
AudioDriverWaveBuf* m_current_wavebuf;
Mutex m_update_lock;
    
bool m_inited_driver = false;
int m_channel_count = 0;
int m_sample_rate = 0;
int m_buffer_size = 0;
int m_samples = 0;
ssize_t m_current_size = 0;
    
size_t m_total_queued_samples = 0;

const int m_samples_per_frame = AUDREN_SAMPLES_PER_FRAME_48KHZ;
const int m_latency = 5;

static int configure_audio_filters(nxmpaudioctx_struct *audioctx, const char *afilters, int force_output_format);

static int init_filter_graph(nxmpaudioctx_struct *audioctx, AVFilterGraph **graph, AVFilterContext **src,
                             AVFilterContext **sink);
							 
static int init_filters(nxmpaudioctx_struct *audioctx,const char *filters_descr,AVFilterGraph *new_graph);

void write_audren(AVFrame *frame, CProjectMVis * pmvis);

CAudioPlayer::~CAudioPlayer(){
	
	if(nxmpaudioctx.running){
		nxmpaudioctx.exit = true;
		threadWaitForExit(&t0);
		threadClose(&t0);
	}
	


	
		if (m_decoded_buffer) {
			free(m_decoded_buffer);
			m_decoded_buffer = nullptr;
		}
		
		if (mempool_ptr) {
			free(mempool_ptr);
			mempool_ptr = nullptr;
		}
		
		if (m_inited_driver) {
			m_inited_driver = false;
			audrvVoiceStop(&m_driver, 0);
			audrvClose(&m_driver);
			audrenExit();
		}
    
		if(nxmpaudioctx.isMidi){
			if(nxmpaudioctx.fplayer){
				delete_fluid_player(nxmpaudioctx.fplayer);
				nxmpaudioctx.fplayer = nullptr;
			}
			if(nxmpaudioctx.fsynth){
				delete_fluid_synth(nxmpaudioctx.fsynth);
				nxmpaudioctx.fsynth = nullptr;
			}
			if(nxmpaudioctx.fsettings){
				delete_fluid_settings(nxmpaudioctx.fsettings);
				nxmpaudioctx.fsettings = nullptr;
			}
		} else {
			if(nxmpaudioctx.graph) {
				avfilter_graph_free(&nxmpaudioctx.graph);
				nxmpaudioctx.graph = nullptr;
			}

			if(nxmpaudioctx.audCtx) {
				avcodec_free_context(&nxmpaudioctx.audCtx);
				nxmpaudioctx.audCtx = nullptr;
			}
			
			if(nxmpaudioctx.pFormatCtx) {
				avformat_close_input(&nxmpaudioctx.pFormatCtx);
				nxmpaudioctx.pFormatCtx = nullptr;
			}
		}
	if(nxmpaudioctx.pmvis) {
        delete nxmpaudioctx.pmvis;
        nxmpaudioctx.pmvis = nullptr;
    }
	
}

CAudioPlayer::CAudioPlayer(int visW,int visH,audioplayerconfig_struct _audioconfig){
	
	audioconfig = _audioconfig;
	
	nxmpaudioctx.pmvis = new CProjectMVis(visW,visH,_audioconfig.milkpresetspath,_audioconfig.milktexturespath,_audioconfig.usebuiltinpreset);
	
		static const AudioRendererConfig arConfig =
		{
			.output_rate     = AudioRendererOutputRate_48kHz,
			.num_voices      = 24,
			.num_effects     = 0,
			.num_sinks       = 1,
			.num_mix_objs    = 1,
			.num_mix_buffers = 2,
		};
		
		m_channel_count = 2;
		m_sample_rate = 48000;
		m_buffer_size = m_latency * m_samples_per_frame * sizeof(s16);
		m_samples = m_buffer_size / m_channel_count / sizeof(s16);
		m_current_size = 0;
		
		m_decoded_buffer = (s16 *)malloc(m_channel_count * m_samples_per_frame * sizeof(s16));

		int mempool_size = (m_buffer_size * BUFFER_COUNT + (AUDREN_MEMPOOL_ALIGNMENT - 1)) &~ (AUDREN_MEMPOOL_ALIGNMENT - 1);
		mempool_ptr = memalign(AUDREN_MEMPOOL_ALIGNMENT, mempool_size);
		
		
			Result rc = audrenInitialize(&arConfig);
		if (R_FAILED(rc)) {
			printf("audrenInitialize: %x", rc);
			return;
		}
		
		rc = audrvCreate(&m_driver, &arConfig, m_channel_count);
		if (R_FAILED(rc)) {
			printf("audrvCreate: %x", rc);
			return;
		}
		
		for (int i = 0; i < BUFFER_COUNT; i++) {
			m_wavebufs[i].data_raw = mempool_ptr;
			m_wavebufs[i].size = mempool_size;
			m_wavebufs[i].start_sample_offset = i * m_samples;
			m_wavebufs[i].end_sample_offset = m_wavebufs[i].start_sample_offset + m_samples;
		}
		
		m_current_wavebuf = NULL;
		
		int mpid = audrvMemPoolAdd(&m_driver, mempool_ptr, mempool_size);
		audrvMemPoolAttach(&m_driver, mpid);
		
		audrvDeviceSinkAdd(&m_driver, AUDREN_DEFAULT_DEVICE_NAME, m_channel_count, m_sink_channels);
		
		rc = audrenStartAudioRenderer();
		if (R_FAILED(rc)) {
			printf("audrenStartAudioRenderer: %x", rc);
		}
		
		audrvVoiceInit(&m_driver, 0, m_channel_count, PcmFormat_Int16, m_sample_rate);
		audrvVoiceSetDestinationMix(&m_driver, 0, AUDREN_FINAL_MIX_ID);
		
		for (int i = 0; i < m_channel_count; i++) {
			for (int j = 0; j < m_channel_count; j++) {
				audrvVoiceSetMixFactor(&m_driver, 0, i == j ? 1.0f : 0.0f, i, j);
			}
		}
		
		audrvVoiceStart(&m_driver, 0);
		m_inited_driver = true;
		device_ready = true;	
		
	
}

void CAudioPlayer::ClearID3(){
	id3tags.title = "";
	id3tags.artist = "";
	id3tags.album = "";
	if(haveAlbumArt)glDeleteTextures(1, &AlbumArtTexture.id);		
	haveAlbumArt = false;
}

bool CAudioPlayer::LoadFile(std::string filename){
	if(fileloaded){
		Stop();

		if(nxmpaudioctx.isMidi){
			if(nxmpaudioctx.fplayer){ delete_fluid_player(nxmpaudioctx.fplayer); nxmpaudioctx.fplayer = nullptr; }
			if(nxmpaudioctx.fsynth){ delete_fluid_synth(nxmpaudioctx.fsynth); nxmpaudioctx.fsynth = nullptr; }
			if(nxmpaudioctx.fsettings){ delete_fluid_settings(nxmpaudioctx.fsettings); nxmpaudioctx.fsettings = nullptr; }
		} else {
			avfilter_graph_free(&nxmpaudioctx.graph);
			avcodec_free_context(&nxmpaudioctx.audCtx);
			avformat_close_input(&nxmpaudioctx.pFormatCtx);
			avformat_free_context(nxmpaudioctx.pFormatCtx);
			avcodec_free_context(&nxmpaudioctx.audCtx);
		}

		fileloaded=false;
		ClearID3();
		
		
	}
	
	nxmpaudioctx.isMidi = IsMidiFile(filename);

	if(nxmpaudioctx.isMidi){

		nxmpaudioctx.fsettings = new_fluid_settings();
		fluid_settings_setnum(nxmpaudioctx.fsettings, "synth.sample-rate", 48000.0);
		fluid_settings_setint(nxmpaudioctx.fsettings, "synth.audio-channels", 1);
		fluid_settings_setint(nxmpaudioctx.fsettings, "synth.audio-groups", 1);
		fluid_settings_setint(nxmpaudioctx.fsettings, "player.reset-synth", 0);

		nxmpaudioctx.fsynth = new_fluid_synth(nxmpaudioctx.fsettings);
		if(!nxmpaudioctx.fsynth){
			printf("fluidsynth: cannot create synth\n");
			delete_fluid_settings(nxmpaudioctx.fsettings);
			nxmpaudioctx.fsettings = nullptr;
			return false;
		}

		nxmpaudioctx.fsfontId = fluid_synth_sfload(nxmpaudioctx.fsynth, audioconfig.soundfontpath.c_str(), 1);
		if(nxmpaudioctx.fsfontId == FLUID_FAILED){
			printf("fluidsynth: cannot load soundfont %s\n", audioconfig.soundfontpath.c_str());
			delete_fluid_synth(nxmpaudioctx.fsynth);
			nxmpaudioctx.fsynth = nullptr;
			delete_fluid_settings(nxmpaudioctx.fsettings);
			nxmpaudioctx.fsettings = nullptr;
			return false;
		}

		nxmpaudioctx.fplayer = new_fluid_player(nxmpaudioctx.fsynth);
		if(!nxmpaudioctx.fplayer || fluid_player_add(nxmpaudioctx.fplayer, filename.c_str()) != FLUID_OK){
			printf("fluidsynth: cannot open midi file %s\n", filename.c_str());
			if(nxmpaudioctx.fplayer){ delete_fluid_player(nxmpaudioctx.fplayer); nxmpaudioctx.fplayer = nullptr; }
			delete_fluid_synth(nxmpaudioctx.fsynth);
			nxmpaudioctx.fsynth = nullptr;
			delete_fluid_settings(nxmpaudioctx.fsettings);
			nxmpaudioctx.fsettings = nullptr;
			return false;
		}

		nxmpaudioctx.midiSamplesWritten = 0;
		nxmpaudioctx.currentpos = 0;
		bitrate = 0;
		nxmpaudioctx.duration = 0;

		fluid_player_play(nxmpaudioctx.fplayer);

		fileloaded = true;
		loadedfilename = filename.substr(filename.find_last_of("/")+1);
		return true;
	}
	
	nxmpaudioctx.pFormatCtx = avformat_alloc_context();
	int ret = avformat_open_input(&nxmpaudioctx.pFormatCtx, std::string(std::string("file:")+filename).c_str(), NULL, NULL);
	
	if (ret < 0) {
		char err[1024] = { 0 };
		int nRet = av_strerror(ret, err, 1024);
        printf("Cannot open %s errno %s\n", filename.c_str(),err);
        
        return false;
    }
	
	ret = avformat_find_stream_info(nxmpaudioctx.pFormatCtx, NULL);
	if (ret < 0) {
        avformat_close_input(&nxmpaudioctx.pFormatCtx);
		avformat_free_context(nxmpaudioctx.pFormatCtx);
		printf("avformat_find_stream_info error %d\n",ret);
        return false;
    }
	
	//av_dump_format(nxmpaudioctx.pFormatCtx, 0, std::string(std::string("file:")+filename).c_str(), 0);
	
	
	AVDictionary* dict = nxmpaudioctx.pFormatCtx->metadata;
         // In Ogg, the internal comment metadata headers are attached to a single content stream.
         // By convention, it's the first logical bitstream occuring.
	if (!dict && nxmpaudioctx.pFormatCtx->nb_streams > 0) {
		dict = nxmpaudioctx.pFormatCtx->streams[0]->metadata;
	}
	
	
	
	AVDictionaryEntry *t = NULL;
	
	if ((t = av_dict_get(dict, "title", NULL, 0))) {
		id3tags.title = t->value;		
		t = NULL;
	}
	if ((t = av_dict_get(dict, "artist", NULL, 0))) {
		id3tags.artist = t->value;
		t = NULL;		
	}
	if ((t = av_dict_get(dict, "album", NULL, 0))) {
		id3tags.album = t->value;
		t = NULL;
	}
	
	
	
	bool foundAudio = false;
	bool foundVideo = false;
	for (unsigned int i = 0; i < nxmpaudioctx.pFormatCtx->nb_streams; i++) {
        AVCodecParameters *localparam = nxmpaudioctx.pFormatCtx->streams[i]->codecpar;
        
		AVDictionaryEntry *t = NULL;
			
		
		AVCodec *localcodec = avcodec_find_decoder(localparam->codec_id);
        if (localparam->codec_type == AVMEDIA_TYPE_AUDIO && !foundAudio) {
			
			
            nxmpaudioctx.audCodec = localcodec;
            nxmpaudioctx.audpar = localparam;
			if(nxmpaudioctx.audpar->extradata){
				
			}
            nxmpaudioctx.audId = i;
            foundAudio = true;
			
			
        }
		
		
		if (localparam->codec_type == AVMEDIA_TYPE_VIDEO && !foundVideo) {
            nxmpaudioctx.vidCodec = localcodec;
            nxmpaudioctx.vidpar = localparam;
            nxmpaudioctx.vidId = i;
			foundVideo = true;
			
        }
		
		if (nxmpaudioctx.pFormatCtx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
			AVPacket * pkt = &nxmpaudioctx.pFormatCtx->streams[i]->attached_pic;
			haveAlbumArt = true;
			AlbumArtTexture =  nxmpgfx::load_texture_from_mem(pkt->data,pkt->size);
			
		}

    }
	
	if(!foundAudio){
		printf("NO AUDIO TRACK\n");
		return false;
	}
	
	nxmpaudioctx.audCtx = avcodec_alloc_context3(nxmpaudioctx.audCodec);
    if (avcodec_parameters_to_context(nxmpaudioctx.audCtx, nxmpaudioctx.audpar) < 0) {
        avcodec_free_context(&nxmpaudioctx.audCtx);
		return false;
    }
	
	if (nxmpaudioctx.audpar->codec_id == AV_CODEC_ID_VORBIS  ){
		bitrate = nxmpaudioctx.pFormatCtx->bit_rate;
	}else if (nxmpaudioctx.audpar->codec_id == AV_CODEC_ID_FLAC  ){
		bitrate = nxmpaudioctx.pFormatCtx->bit_rate;
	} else {
		bitrate = nxmpaudioctx.audCtx->bit_rate;
	}
	
	
    if (avcodec_open2(nxmpaudioctx.audCtx, nxmpaudioctx.audCodec, NULL) < 0) {
        avcodec_free_context(&nxmpaudioctx.audCtx);
        return false;
    }
	
	
	
	
	ret = init_filter_graph(&nxmpaudioctx,&nxmpaudioctx.graph,&nxmpaudioctx.src,&nxmpaudioctx.sink);
	
	
	nxmpaudioctx.duration = (1000000 * ( nxmpaudioctx.pFormatCtx->streams[nxmpaudioctx.audId]->duration * ((float) nxmpaudioctx.pFormatCtx->streams[nxmpaudioctx.audId]->time_base.num / nxmpaudioctx.pFormatCtx->streams[nxmpaudioctx.audId]->time_base.den)));
	
	fileloaded = true;
	loadedfilename = filename.substr(filename.find_last_of("/")+1);
	return true;
}

void CAudioPlayer::Play(){
	nxmpaudioctx.pause = false;
	
	if(nxmpaudioctx.isMidi){
		threadCreate(&t0, MidiAudioThread, &nxmpaudioctx, NULL, 0x100000, 0x3B, -2);
	} else {
		threadCreate(&t0, AudrenAudioThread, &nxmpaudioctx, NULL, 0x100000, 0x3B, -2);
	}
	threadStart(&t0);
}

void CAudioPlayer::Pause(){
	nxmpaudioctx.pause = !nxmpaudioctx.pause;
}

bool CAudioPlayer::isPaused(){
	return nxmpaudioctx.pause;
}

void CAudioPlayer::Stop(){
	if(nxmpaudioctx.running){
		nxmpaudioctx.exit = true;
		threadWaitForExit(&t0);
		threadClose(&t0);
		nxmpaudioctx.exit = false;
	}
}

void CAudioPlayer::NextVisPreset(){
	spectrumvis = false;
	nxmpaudioctx.pmvis->NextVisPreset();
}
void CAudioPlayer::PrevVisPreset(){
	spectrumvis = false;
	nxmpaudioctx.pmvis->PrevVisPreset();
}

bool CAudioPlayer::ToogleVis(){
	return nxmpaudioctx.pmvis->ToogleVis();
}

bool CAudioPlayer::VisEnabled(){
	return nxmpaudioctx.pmvis->VisEnabled();
}

std::string CAudioPlayer::getCurrentPlaylistItem(){
	if(spectrumvis)return "Spectrum";
	return nxmpaudioctx.pmvis->getCurrentPlaylistItem();
}

unsigned int CAudioPlayer::getBitRate(){
	return bitrate;
}

std::string CAudioPlayer::getCodec(){
	if(nxmpaudioctx.isMidi) return "MIDI (fluidsynth)";
	return nxmpaudioctx.audCodec->long_name;
}

unsigned int CAudioPlayer::getSampleRate(){
	if(nxmpaudioctx.isMidi) return 48000;	
	return nxmpaudioctx.audCtx->sample_rate;
}

std::string CAudioPlayer::getFormat(){
	if(nxmpaudioctx.isMidi) return "Standard MIDI File";
	return nxmpaudioctx.pFormatCtx->iformat->long_name;
}

long CAudioPlayer::getDuration(){
	return nxmpaudioctx.duration;
}

nxmilk_id3tags_struct CAudioPlayer::getID3(){
	return id3tags;
}


long CAudioPlayer::getPosition(){
	return nxmpaudioctx.currentpos;
}

bool CAudioPlayer::Running(){
	return nxmpaudioctx.running;
}

std::string CAudioPlayer::getFileName(){
	return loadedfilename;
}

void CAudioPlayer::Seek(int seconds) {
    nxmpaudioctx.seek = seconds;
}

void CAudioPlayer::SeekAbs(int seconds){
	nxmpaudioctx.seekabs = seconds;
}

void AudrenAudioThread(void *arg) {
	
	nxmpaudioctx_struct *ctx = (nxmpaudioctx_struct *)arg;
	
	AVFrame *aframe;
    AVFrame *aframe_filt;
    
	AVPacket *packet;
	aframe = av_frame_alloc();
	aframe_filt = av_frame_alloc();
	packet = av_packet_alloc();
	ctx->running = true;


	while (!ctx->exit){
			
				
			int ffres = av_read_frame(ctx->pFormatCtx, packet);
			if(ffres <0)break;
			
			if(ctx->seekabs!=0){
				int64_t m_target_ts = av_rescale_q(ctx->seekabs * AV_TIME_BASE, AV_TIME_BASE_Q, ctx->pFormatCtx->streams[ctx->audId]->time_base);
				avcodec_flush_buffers(ctx->audCtx);
				av_seek_frame(ctx->pFormatCtx, packet->stream_index, m_target_ts, 0);
				ctx->seekabs = 0;
			}
			
			
            if(ctx->seek!=0){
				int seconds = ctx->currentpos/1000000+ctx->seek;
				int64_t m_target_ts = av_rescale_q(seconds * AV_TIME_BASE, AV_TIME_BASE_Q, ctx->pFormatCtx->streams[ctx->audId]->time_base);
				avcodec_flush_buffers(ctx->audCtx);
				if(ctx->seek>0){
					av_seek_frame(ctx->pFormatCtx, packet->stream_index, m_target_ts, 0);
			
				}else{
					av_seek_frame(ctx->pFormatCtx, packet->stream_index, m_target_ts, AVSEEK_FLAG_BACKWARD);
			
				}
				ctx->seek = 0;
				continue;
			}
			if (packet->stream_index == ctx->audId) {
				ctx->currentpos = (long) (1000000 * (packet->pts * ((float) ctx->pFormatCtx->streams[ctx->audId]->time_base.num / ctx->pFormatCtx->streams[ctx->audId]->time_base.den)));
				
				
				int ret =  avcodec_send_packet(ctx->audCtx, packet);
				if(ret <0){
					printf("avcodec_send_packet %d\n",ret);
					break;
				}
			
				ret = avcodec_receive_frame(ctx->audCtx, aframe);
				if(ret == AVERROR(EAGAIN)){
					continue;
				}
				if(ret <0){
					printf("avcodec_receive_frame %d\n",ret);
					break;
				}
				
				int err = av_buffersrc_add_frame_flags(ctx->src, aframe,0);
				 
				 
				 while ((err = av_buffersink_get_frame(ctx->sink, aframe_filt)) >= 0) {
					/* now do something with our filtered frame */
					
					const int bps = av_get_bytes_per_sample(aframe_filt->format);
					aframe_filt->linesize[0] = bps*2*aframe_filt->nb_samples;
					
					write_audren(aframe_filt,ctx->pmvis);
					if (err < 0) {
						fprintf(stderr, "Error processing the filtered frame:");
						break;
					}
					
				}

			}
		audrvUpdate(&m_driver);
		av_packet_unref(packet);
		while(ctx->pause && !ctx->exit){
			svcSleepThread(1000);
		}

	}
	av_packet_free(&packet);
    av_frame_free(&aframe);
	av_frame_free(&aframe_filt);
	ctx->running = false;	
	
}

void MidiAudioThread(void *arg) {

	nxmpaudioctx_struct *ctx = (nxmpaudioctx_struct *)arg;
	ctx->running = true;

	const int frames = m_samples_per_frame; // AUDREN_SAMPLES_PER_FRAME_48KHZ
	int16_t *midibuf = (int16_t *)malloc(frames * 2 * sizeof(int16_t)); // stereo interleaved

	while (!ctx->exit) {

		if (fluid_player_get_status(ctx->fplayer) == FLUID_PLAYER_DONE) {
			break;
		}
		
		if (ctx->seekabs != 0) {

			int total_ticks = fluid_player_get_total_ticks(ctx->fplayer);
			int target_tick = 0;
			if (ctx->duration > 0) {
				target_tick = (int)((double)total_ticks * ((double)(ctx->seekabs * 1000000) / (double)ctx->duration));
			}
			fluid_synth_all_sounds_off(ctx->fsynth, -1);
			fluid_player_seek(ctx->fplayer, target_tick);
			ctx->midiSamplesWritten = (long)((double)ctx->seekabs * 48000.0);
			ctx->seekabs = 0;
		}
		
		if (ctx->seek != 0) {
			int seconds = ctx->currentpos/1000000 + ctx->seek;
			if (seconds < 0) seconds = 0;
			int total_ticks = fluid_player_get_total_ticks(ctx->fplayer);
			int target_tick = 0;
			if (ctx->duration > 0) {
				target_tick = (int)((double)total_ticks * ((double)(seconds * 1000000) / (double)ctx->duration));
			}
			fluid_synth_all_sounds_off(ctx->fsynth, -1);
			fluid_player_seek(ctx->fplayer, target_tick);
			ctx->midiSamplesWritten = (long)((double)seconds * 48000.0);
			ctx->seek = 0;
		}

		fluid_synth_write_s16(ctx->fsynth, frames, midibuf, 0, 2, midibuf, 1, 2);
		
		if (ctx->pmvis->VisEnabled()) {
			ctx->pmvis->AddInt16(midibuf, frames * 2);
		}

		write_audio(midibuf, frames * 2 * sizeof(int16_t));

		ctx->midiSamplesWritten += frames;
		ctx->currentpos = (long)(1000000.0 * (double)ctx->midiSamplesWritten / 48000.0);

		audrvUpdate(&m_driver);
	
		while (ctx->pause && !ctx->exit) {
		svcSleepThread(1000);
		}
	}

	free(midibuf);
	ctx->running = false;
}

void write_audren(AVFrame *frame, CProjectMVis * pmvis){
	if(pmvis->VisEnabled()){
		unsigned int samples =  frame->linesize[0]/sizeof(int16_t)/2;
		pmvis->AddInt16(reinterpret_cast<int16_t*>(frame->data[0]),samples);
	}
	write_audio(frame->data[0],frame->linesize[0]);
}


ssize_t free_wavebuf_index() {
    for (int i = 0; i < BUFFER_COUNT; i++) {
        if (m_wavebufs[i].state == AudioDriverWaveBufState_Free ||
            m_wavebufs[i].state == AudioDriverWaveBufState_Done) {
            return i;
        }
    }
    return -1;
}

size_t append_audio(const void *buf, size_t size) {
    ssize_t index = -1;
    
    if (!m_current_wavebuf) {
        index = free_wavebuf_index();
        if (index == -1) {
            return 0;
        }
        
        m_current_wavebuf = &m_wavebufs[index];
        current_pool_ptr = mempool_ptr + (index * m_buffer_size);
        m_current_size = 0;
    }
    
    if (size > m_buffer_size - m_current_size) {
        size = m_buffer_size - m_current_size;
    }
    
    void *dstbuf = current_pool_ptr + m_current_size;
    memcpy(dstbuf, buf, size);
    armDCacheFlush(dstbuf, size);
    
    m_current_size += size;
    
    if (m_current_size == m_buffer_size) {
        audrvVoiceAddWaveBuf(&m_driver, 0, m_current_wavebuf);
        
        mutexLock(&m_update_lock);
        audrvUpdate(&m_driver);
        mutexUnlock(&m_update_lock);
        
        if (!audrvVoiceIsPlaying(&m_driver, 0)) {
            audrvVoiceStart(&m_driver, 0);
        }
        
        m_current_wavebuf = NULL;
    }
    
    return size;
}

void write_audio(const void* buf, size_t size) {
    if (!m_inited_driver) {
        printf("Audren: Call write_audio without init driver!");
        return;
    }

    audrvUpdate(&m_driver);

    size_t queued_samples = m_total_queued_samples -
        audrvVoiceGetPlayedSampleCount(&m_driver, 0);
 

    size_t written = 0;
    while (written < size) {
        written += append_audio(buf + written, size - written);

        if (written != size) {
            mutexLock(&m_update_lock);
            audrvUpdate(&m_driver);
            mutexUnlock(&m_update_lock);
            audrenWaitFrame();
        }
    }
}

void CAudioPlayer::DrawProjectM(){
	nxmpaudioctx.pmvis->Draw();
}

 
 static int init_filter_graph(nxmpaudioctx_struct *audioctx,AVFilterGraph **graph, AVFilterContext **src,
                             AVFilterContext **sink)
{
    AVFilterGraph *filter_graph;
    AVFilterContext *abuffer_ctx;
    const AVFilter  *abuffer;
    AVFilterContext *volume_ctx;
    const AVFilter  *volume;
    AVFilterContext *aformat_ctx;
    const AVFilter  *aformat;
    AVFilterContext *abuffersink_ctx;
    const AVFilter  *abuffersink;

    AVDictionary *options_dict = NULL;
    uint8_t options_str[1024];
    uint8_t ch_layout[64];

    int err;

    /* Create a new filtergraph, which will contain all the filters. */
    filter_graph = avfilter_graph_alloc();
    if (!filter_graph) {
        fprintf(stderr, "Unable to create filter graph.\n");
        return AVERROR(ENOMEM);
    }

    /* Create the abuffer filter;
     * it will be used for feeding the data into the graph. */
    abuffer = avfilter_get_by_name("abuffer");
    if (!abuffer) {
        fprintf(stderr, "Could not find the abuffer filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    abuffer_ctx = avfilter_graph_alloc_filter(filter_graph, abuffer, "src");
    if (!abuffer_ctx) {
        fprintf(stderr, "Could not allocate the abuffer instance.\n");
        return AVERROR(ENOMEM);
    }

	if (audioctx->audpar->codec_id == AV_CODEC_ID_VORBIS  ){
		
	}


    /* Set the filter options through the AVOptions API. */
    av_channel_layout_describe(&audioctx->audCtx->ch_layout, ch_layout, sizeof(ch_layout));
    av_opt_set    (abuffer_ctx, "channel_layout", ch_layout,                            AV_OPT_SEARCH_CHILDREN);
    av_opt_set    (abuffer_ctx, "sample_fmt",     av_get_sample_fmt_name(audioctx->audCtx->sample_fmt), AV_OPT_SEARCH_CHILDREN);
    av_opt_set_q  (abuffer_ctx, "time_base",      (AVRational){ 1, audioctx->audCtx->sample_rate },  AV_OPT_SEARCH_CHILDREN);
    av_opt_set_int(abuffer_ctx, "sample_rate",    audioctx->audCtx->sample_rate,                     AV_OPT_SEARCH_CHILDREN);

    /* Now initialize the filter; we pass NULL options, since we have already
     * set all the options above. */
    err = avfilter_init_str(abuffer_ctx, NULL);
    if (err < 0) {
        fprintf(stderr, "Could not initialize the abuffer filter.\n");
        return err;
    }

    /* Create the aformat filter;
     * it ensures that the output is of the format we want. */
    aformat = avfilter_get_by_name("aformat");
    if (!aformat) {
        fprintf(stderr, "Could not find the aformat filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    aformat_ctx = avfilter_graph_alloc_filter(filter_graph, aformat, "aformat");
    if (!aformat_ctx) {
        fprintf(stderr, "Could not allocate the aformat instance.\n");
        return AVERROR(ENOMEM);
    }

    /* A third way of passing the options is in a string of the form
     * key1=value1:key2=value2.... */
    snprintf(options_str, sizeof(options_str),
             "sample_fmts=%s:sample_rates=%d:channel_layouts=stereo",
             av_get_sample_fmt_name(AV_SAMPLE_FMT_S16), 48000);
    err = avfilter_init_str(aformat_ctx, options_str);
    if (err < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not initialize the aformat filter.\n");
        return err;
    }

    /* Finally create the abuffersink filter;
     * it will be used to get the filtered data out of the graph. */
    abuffersink = avfilter_get_by_name("abuffersink");
    if (!abuffersink) {
        fprintf(stderr, "Could not find the abuffersink filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    abuffersink_ctx = avfilter_graph_alloc_filter(filter_graph, abuffersink, "sink");
    if (!abuffersink_ctx) {
        fprintf(stderr, "Could not allocate the abuffersink instance.\n");
        return AVERROR(ENOMEM);
    }

    /* This filter takes no options. */
    err = avfilter_init_str(abuffersink_ctx, NULL);
    if (err < 0) {
        fprintf(stderr, "Could not initialize the abuffersink instance.\n");
        return err;
    }

    /* Connect the filters;
     * in this simple case the filters just form a linear chain. */
    //err = avfilter_link(abuffer_ctx, 0, volume_ctx, 0);
    //if (err >= 0)
    err = avfilter_link(abuffer_ctx, 0, aformat_ctx, 0);
    if (err >= 0)
        err = avfilter_link(aformat_ctx, 0, abuffersink_ctx, 0);
    if (err < 0) {
        fprintf(stderr, "Error connecting filters\n");
        return err;
    }

    /* Configure the graph. */
    err = avfilter_graph_config(filter_graph, NULL);
    if (err < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error configuring the filter graph\n");
        return err;
    }

    *graph = filter_graph;
    *src   = abuffer_ctx;
    *sink  = abuffersink_ctx;

    return 0;
}