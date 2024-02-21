#ifndef NXMP_AUDIOPLAYER_H
#define NXMP_AUDIOPLAYER_H

#include <string>
#include <mutex>

#include <SDL2/SDL.h>

#include "nxmp-gfx.h"
#include "stb_image.h"

#include "iniParser.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>

#include "libavutil/channel_layout.h"
#include "libavutil/md5.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "libavutil/samplefmt.h"

#include "libavutil/avstring.h"
#include "libavutil/channel_layout.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/fifo.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include "libavutil/bprint.h"

#ifdef __cplusplus
}
#endif

#include <switch.h>

//#include "projectM-4/projectM.h"
#include "projectm-vis.h" 

#define HAVE_SDL

typedef struct {
	AVFormatContext *pFormatCtx;
	AVCodecContext *audCtx;
	AVCodec *audCodec;
	AVCodecParameters *audpar;
	AVCodec *vidCodec;
	AVCodecParameters *vidpar;
#ifdef HAVE_SDL
	SDL_AudioDeviceID auddev;
#endif
	//AudioDriver drv;
	
	CProjectMVis * pmvis;
	
	
	AVFilterGraph *graph;
    AVFilterContext *src, *sink;
	
	size_t mempool_size;
	void* mempool_ptr;
	void* tmpdata_ptr;
	size_t max_samples_datasize;
	//AudioDriverWaveBuf wavebuf[2] = {0};
	SwrContext *resampler = nullptr;
	int audId = -1;
	int vidId = -1;
	projectm* projectMHandle{nullptr};
	bool running = false;
	bool exit = false;
	long currentpos = 0;
	long duration = 0;
	int seek = 0;
	int seekabs = 0;
	bool pause = false;
	int audiodriver;
	std::mutex lock_mutex;
}nxmpaudioctx_struct;


typedef struct{
	std::string album;
	std::string artist;
	std::string title;
	
	
}nxmilk_id3tags_struct;


class CAudioPlayer{
public:
	CAudioPlayer(int _audiodriver,int visW,int visH,audioplayerconfig_struct _audioconfig);
	~CAudioPlayer();
	bool LoadFile(std::string filename);
	void Play();
	void Pause();
	void Stop();
	void Seek(int seconds);
	void SeekAbs(int seconds);
	
	bool Running();
	bool isPaused();
	void DrawProjectM();
	
	bool device_ready = false;
	bool fileloaded = false;
	
	unsigned int getSampleRate();
	unsigned int getBitRate();
	std::string getFormat();
	std::string getCodec();
	std::string getFileName();
	nxmilk_id3tags_struct getID3();
	void ClearID3();
	long getDuration();
	long getPosition();
	
	void NextVisPreset();
	void PrevVisPreset();
	std::string getCurrentPlaylistItem();
	void ToogleVis();
	bool VisEnabled();
	
	
	
	
	bool haveAlbumArt = false;
	Tex AlbumArtTexture;
	
private:
	//SDL_AudioDeviceID auddev;
#ifdef HAVE_SDL
	SDL_AudioSpec want, have;
#endif
	int audiodriver = 0;
	nxmpaudioctx_struct nxmpaudioctx;
	
	//AVFormatContext *pFormatCtx;
    //int vidId = -1, audId = -1;
    double fpsrendering = 0.0;
    //AVCodecContext *vidCtx, *audCtx;
    //AVCodec *vidCodec, *audCodec;
    //AVCodecParameters *vidpar, *audpar;
	Thread t0;
	std::string loadedfilename;
	
	bool spectrumvis = false;
	
	nxmilk_id3tags_struct id3tags;


	unsigned int bitrate;
	
	audioplayerconfig_struct audioconfig;
	
	
};







#endif