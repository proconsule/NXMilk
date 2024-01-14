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

#include "usbfs.h"

#define is_bit_set(val, bit_no) (((val) >> (bit_no)) & 1)



GLuint WIDTH = 1280;
GLuint HEIGHT = 720;
GLFWwindow *window;

CAudioPlayer * audioplayer;
CImgLoader * imgloader;

float multiplyRes = 1.0f;
bool isHandheld = true;


std::vector<std::string> audioextensions = {".mp3",".flac",".ogg"};

extern u32 __nx_applet_exit_mode;


USBMounter *MyUSBMount = nullptr;

bool USBDialog = false;


int
main(int argc, const char* const* argv) {
	
	
	appletLockExit();
	romfsInit();
	
	
	AppletOperationMode stus=appletGetOperationMode();
	if (stus == AppletOperationMode_Handheld) {
		isHandheld=true;
		multiplyRes = 1.0f;
	}
	if (stus == AppletOperationMode_Console) {
		isHandheld=false;
		multiplyRes = 1.5f;
	}
	
	
	AppletType at = appletGetAppletType();
    if ( at != AppletType_Application && at != AppletType_SystemApplication) {
		
		
		nxmpgfx::Init_Backend(!isHandheld,true);
		nxmpgfx::Init_Backend_AppletMode(!isHandheld);
		nxmpgfx::loopAppletMode();
		
		nxmpgfx::Destroy_Backend();
		romfsExit();
		appletUnlockExit();
		return 0;
	}
	
	
	
	SocketInitConfig cfg = *(socketGetDefaultInitConfig());
	cfg.bsd_service_type = BsdServiceType_System;
	socketInitialize(&cfg);
    
	nxlinkStdio();
	


	
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
	nxmpgfx::updateSplash(50);
	
	
	
	
	
	
	nxmpgfx::Init_ImGui(false);
	nxmpgfx::SetColorTheme(0);
	nxmpgfx::setEnableTouch(true);
	
	nxmpgfx::UniFontLoader(false);
	
	imgloader = new CImgLoader("romfs:");

	audioplayer = new CAudioPlayer(1);
	
	fsbrowser = new CFSBrowser("");
	nxmpgfx::updateSplash(100);


	fsbrowser->DirList("/switch/nxmp",false,audioextensions);

#ifdef __SWITCH__	

	appletSetMediaPlaybackState(true);

	uint32_t counter = 0;

	while(appletMainLoop() && !nxmpgfx::WindowShouldClose())
    {
		
		
		uint64_t event_ret = nxmpgfx::Process_UI_Events(std::chrono::system_clock::now());
		//if(event_ret)Windows::UserActivity();
		
		if(ImGui::IsMouseDown(ImGuiMouseButton_Left)){
			Windows::UserActivity();
		}
		
		
		if(is_bit_set(event_ret,nxmpgfx::BUT_A)){
			Windows::UserActivity();
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_Y)){
			if(audioplayer->Running()){
				audioplayer->ViewSpectrum();
			}else{
				if(MyUSBMount == nullptr){
					MyUSBMount = new USBMounter();
					
				}
				USBDialog = !USBDialog;
				if(!USBDialog){
					fsbrowser->DirList("/switch/nxmp",false,audioextensions);
					MyUSBMount->setBasePath("");
				}
			}
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_B)){
			if(audioplayer->Running()){
				audioplayer->Stop();
			}else{
				std::string newpath = fsbrowser->backDir();
				fsbrowser->DirList(fsbrowser->getCurrentPath(),false,audioextensions);
			}
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_PLUS)){
			break;
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_R)){
			audioplayer->Seek(5);
			Windows::UserActivity();
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_L)){
			audioplayer->Seek(-5);
			Windows::UserActivity();
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_ZL)){
			audioplayer->PrevVisPreset();
			Windows::VisPlaylistActivity();
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_ZR)){
			audioplayer->NextVisPreset();
			Windows::VisPlaylistActivity();
		}
		
		
		nxmpgfx::NewFrame();
        ImGui::NewFrame();
		
		if(!audioplayer->Running()){
			if(USBDialog && MyUSBMount->getBasePath() == ""){
				Windows::USBWindow();
			}else{
				Windows::MainMenuWindow();
			}		
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
	
	if(MyUSBMount!=nullptr)delete MyUSBMount;
	
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