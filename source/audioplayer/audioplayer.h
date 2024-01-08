#ifndef NXMP_AUDIOPLAYER_H
#define NXMP_AUDIOPLAYER_H

#include <string>
#include <mutex>

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
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
	
	size_t mempool_size;
	void* mempool_ptr;
	void* tmpdata_ptr;
	size_t max_samples_datasize;
	//AudioDriverWaveBuf wavebuf[2] = {0};
	SwrContext *resampler;
	int audId = -1;
	int vidId = -1;
	projectm* projectMHandle{nullptr};
	bool running = false;
	bool exit = false;
	long currentpos = 0;
	long duration = 0;
	int seek = 0;
	bool pause = false;
	int audiodriver;
	std::mutex lock_mutex;
}nxmpaudioctx_struct;



class CAudioPlayer{
public:
	CAudioPlayer(int _audiodriver,int visW = 1280,int visH = 720);
	~CAudioPlayer();
	static void CAudioPlayerCallback(void* userData, unsigned char* stream, int len);
	bool LoadFile(std::string filename);
	void Play();
	void Pause();
	void Stop();
	void Seek(int seconds);
	
	bool Running();
	void DrawProjectM();
	
	bool device_ready = false;
	bool fileloaded = false;
	
	unsigned int getSampleRate();
	std::string getFormat();
	std::string getCodec();
	long getDuration();
	long getPosition();
	
	void NextVisPreset();
	void PrevVisPreset();
	
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
	
	
	
};







#endif