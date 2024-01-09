#include <stdio.h>

#include <errno.h>
#include <signal.h>



#include <sys/time.h>

#include <string>
#include <iterator>
#include <utility>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include <switch.h>

#include "appwindows.h"

#include "audioplayer.h"
#include "nxmp-gfx.h"
#include "imgloader.h"

#define is_bit_set(val, bit_no) (((val) >> (bit_no)) & 1)



GLuint WIDTH = 1280;
GLuint HEIGHT = 720;
GLFWwindow *window;

CAudioPlayer * audioplayer;
CImgLoader * imgloader;

std::vector<std::string> audioextendions = {".mp3",".flac"};

extern u32 __nx_applet_exit_mode;


int
main(int argc, const char* const* argv) {
	
	
	appletLockExit();
	
	SocketInitConfig cfg = *(socketGetDefaultInitConfig());
	cfg.bsd_service_type = BsdServiceType_System;
	socketInitialize(&cfg);
    
	nxlinkStdio();
	romfsInit();


	
	appletInitializeGamePlayRecording();
    appletSetWirelessPriorityMode(AppletWirelessPriorityMode_OptimizedForWlan);
	
	extern u32 __nx_applet_type;
    auto saved_applet_type = std::exchange(__nx_applet_type, AppletType_SystemApplet );

    nvInitialize();
    __nx_applet_type = saved_applet_type;
	

	int ret = 0;

	if (R_FAILED(ret = nifmInitialize(NifmServiceType_User))) {
		return ret;
	}

	if (R_FAILED(ret = plInitialize(PlServiceType_User ))) {
		
		return ret;
	}
	
	
	nxmpgfx::Init_Backend(false,true);
	nxmpgfx::Init_Backend_Splash(false);
	
	
	
	nxmpgfx::updateSplash(25);
	nxmpgfx::updateSplash(50);
	
	
	
	
	
	
	nxmpgfx::Init_ImGui(false);
	nxmpgfx::SetColorTheme(0);
	
	nxmpgfx::UniFontLoader(false);
	
	imgloader = new CImgLoader("romfs:");

	audioplayer = new CAudioPlayer(0);
	
	fsbrowser = new CFSBrowser("");
	


	fsbrowser->DirList("/switch/nxmp",false,audioextendions);

/*
	bool fileloaded = audioplayer->LoadFile("/switch/nxmp/test.mp3");

	if(fileloaded){
		printf("File Loaded\n");
	}
	printf("Codec: %s\n",audioplayer->getCodec().c_str());
	printf("Samplerate: %d\n",audioplayer->getSampleRate());
	printf("Duration: %ld\n",audioplayer->getDuration());
	audioplayer->Play();
*/

#ifdef __SWITCH__	

	appletSetMediaPlaybackState(true);

	uint32_t counter = 0;

	while(appletMainLoop() && !nxmpgfx::WindowShouldClose())
    {
		
		
		uint64_t event_ret = nxmpgfx::Process_UI_Events(std::chrono::system_clock::now());
		if(event_ret)Windows::UserActivity();
		
		if(is_bit_set(event_ret,nxmpgfx::BUT_A)){
			audioplayer->Pause();
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_Y)){
			audioplayer->ViewSpectrum();
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_B)){
			if(audioplayer->Running()){
				audioplayer->Stop();
			}else{
				std::string newpath = fsbrowser->backDir();
				fsbrowser->DirList(fsbrowser->getCurrentPath(),false,audioextendions);
			}
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_PLUS)){
			break;
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_R)){
			audioplayer->Seek(5);
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_L)){
			audioplayer->Seek(-5);
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_ZL)){
			audioplayer->PrevVisPreset();
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_ZR)){
			audioplayer->NextVisPreset();
			//int plsidx = projectm_playlist_play_next(_playlist, true);
			//printf("PLS ITEM: %s\n",projectm_playlist_item(_playlist,plsidx));
		}
		
		
		nxmpgfx::NewFrame();
        ImGui::NewFrame();
		
		if(!audioplayer->Running()){
			Windows::MainMenuWindow();
		}
		if(audioplayer->Running()){
			Windows::PlayerWindow();
		}
		
		nxmpgfx::Render_PreMPV();
		
		
		if(audioplayer->Running()){
			audioplayer->DrawProjectM();
		}
		
		nxmpgfx::Render_PostMPV();
		
		
		counter++;
    }

    
#endif
	delete audioplayer;
	delete imgloader;
	
	glfwDestroyWindow(window);
	glfwTerminate();
	
	appletSetMediaPlaybackState(false);
	
	plExit();
	romfsExit();
	socketExit();
	appletUnlockExit();
	
	__nx_applet_exit_mode = 1;
    return 0;
		
}