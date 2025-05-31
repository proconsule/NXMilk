#include <stdio.h>

#include <errno.h>
#include <signal.h>

#include "devoptabfs_defines.h"

#include <sys/time.h>

#include <string>
#include <vector>
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
#include "iniParser.h"

#define is_bit_set(val, bit_no) (((val) >> (bit_no)) & 1)



GLuint WIDTH = 1280;
GLuint HEIGHT = 720;
GLFWwindow *window;

CAudioPlayer * audioplayer;
CImgLoader * imgloader;

CIniParser* configini;


float multiplyRes = 1.0f;
bool isHandheld = true;

std::vector<std::string> audioextensions = {".mp3",".flac",".ogg",".wav",".m4a",".cue",".rar",".zip",".tar.gz",".iso"};


std::vector<std::string> libopenmptext = {"669",".amf",".ams",".dbm",".digi",".dmf",".dsm",".far",".gdm",".ice",".imf",".it",".j2b",".m15",".mdl",".med",".mmcmp",".mms",".mo3",".mod",".mptm",".mt2",".mtm",".nst",".okt",".plm",".ppm",".psm",".pt36",".ptm",".s3m",".sfx",".sfx2",".st26",".stk",".stm",".ult",".umx",".wow",".xm",".xpk"};
std::vector<std::string> libgmefilext = {".ay",".gbs",".gym",".hes",".kss",".nfs",".nfse",".sap",".spc",".vgm",".vgz"};
std::vector<std::string> libmodplugext = {"669",".abc",".amf",".ams",".dbm",".dmf",".dsm",".far",".it",".mdl",".med",".mid",".mod",".mt2",".mtm",".okt",".psm",".ptm",".s3m",".stm",".ult",".umx",".xm",".itgz",".itr",".itz",".mdgz",".mdr",".mdz",".s3gz",".s3r",".s3z",".xmgz",".xmr",".xmz"};



extern u32 __nx_applet_exit_mode;


USBMounter *MyUSBMount = nullptr;





bool USBDialog = false;

bool BackLightToggle = true;



menuitem_struct menuitem;

bool slaveplayer=false;
std::string slaveplayer_file = "";



int
main(int argc, const char* const* argv) {
	
	
	appletLockExit();
	romfsInit();
	
	if(argc>1){
		slaveplayer=true;
		slaveplayer_file = argv[1];
	}
	
	
	
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
	
	menuitem.state = MENU_STATE_HOME;
	
	if(slaveplayer)menuitem.state = MENU_STATE_GUILESS;
	
	
	std::vector<std::string> trackermerge;
	
	std::set_union(libopenmptext.cbegin(), libopenmptext.cend(),
                   libmodplugext.cbegin(), libmodplugext.cend(),
                   std::back_inserter(trackermerge));
	
	
	audioextensions.insert(audioextensions.end(), trackermerge.begin(), trackermerge.end());
	audioextensions.insert(audioextensions.end(), libgmefilext.begin(), libgmefilext.end());
	
	configini = new CIniParser("NXMilk.ini");
	
	for(long unsigned int i=0;i<configini->getConfigExtensions().size();i++){
		audioextensions.push_back(configini->getConfigExtensions()[i]);
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

	audioplayer = new CAudioPlayer(1280.0f*multiplyRes,720.0f*multiplyRes,configini->getAudioPlayerConfig());
	
	//fsbrowser = new CFSBrowser("","");
	nxmpgfx::updateSplash(100);

	

	//fsbrowser->DirList(configini->getStartPath(),false,audioextensions);

#ifdef __SWITCH__	

	appletSetMediaPlaybackState(true);

	uint32_t counter = 0;
	
	if(slaveplayer){
		bool loaded = audioplayer->LoadFile(slaveplayer_file.c_str());
		if(loaded){
			audioplayer->Play();
		}
	}

	while(appletMainLoop() && !nxmpgfx::WindowShouldClose())
    {
		
		
		uint64_t event_ret = nxmpgfx::Process_UI_Events(std::chrono::system_clock::now());
		//if(event_ret)Windows::UserActivity();
		
		if(ImGui::IsMouseDown(ImGuiMouseButton_Left)){
			Windows::UserActivity();
		}
		
		if(is_bit_set(event_ret,nxmpgfx::BUT_DLEFT) || is_bit_set(event_ret,nxmpgfx::BUT_DRIGHT)){
			Windows::UserActivity();
		}
		
		
		if(is_bit_set(event_ret,nxmpgfx::BUT_A)){
			Windows::UserActivity();
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_X)){
			if(menuitem.state == MENU_STATE_FILEBROWSER ){
				if(fsbrowser != nullptr){
					if(fsbrowser->filemount){
						fsbrowser->CloseFilesMount();
						fsbrowser->DirList(fsbrowser->getCurrentPath(),false,audioextensions);
					}
				}
			}
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_Y)){
			if(audioplayer->Running()){
				bool retvis = audioplayer->ToogleVis();
				
			}else{
				if(menuitem.state == MENU_STATE_FILEBROWSER){
					if(fsbrowser != nullptr){
						delete fsbrowser;
						fsbrowser = nullptr;
					}
					menuitem.state = MENU_STATE_HOME;
				}
			}
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_B)){
			if(audioplayer->Running()){
				audioplayer->Stop();
			}else{
				if(menuitem.state == MENU_STATE_FILEBROWSER){
					if(fsbrowser != nullptr){
						std::string newpath = fsbrowser->backDir();
						fsbrowser->DirList(fsbrowser->getCurrentPath(),false,audioextensions);
					}
				}
			}
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_PLUS)){
			if(audioplayer->Running()){
				if(BackLightToggle){
					Utility::BackLightOff();
					BackLightToggle = false;
				}else{
					Utility::BackLightOn();
					BackLightToggle = true;
				}
			}
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_R)){
			if(audioplayer->Running()){
				audioplayer->Seek(5);
				Windows::UserActivity();
			}
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_L)){
			if(audioplayer->Running()){
				audioplayer->Seek(-5);
				Windows::UserActivity();
			}
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_ZL)){
			if(audioplayer->Running()){
				audioplayer->PrevVisPreset();
				Windows::VisPlaylistActivity();
			}
		}
		if(is_bit_set(event_ret,nxmpgfx::BUT_ZR)){
			if(audioplayer->Running()){
				audioplayer->NextVisPreset();
				Windows::VisPlaylistActivity();
			}
		}
		
		if(!audioplayer->Running() && !BackLightToggle){
			Utility::BackLightOn();
			BackLightToggle = true;
		}
		
		
		
		nxmpgfx::NewFrame();
        ImGui::NewFrame();
		
		if(!audioplayer->Running()){
			if(menuitem.state == MENU_STATE_HOME){
				Windows::HomeWindow();
			}else if(menuitem.state == MENU_STATE_FILEBROWSER){
				Windows::MainMenuWindow();
			}else if(menuitem.state == MENU_STATE_USB){
				Windows::USBWindow();
			}else if(menuitem.state == MENU_STATE_NETWORKSEL){
				Windows::NetworkWindow();
			}
		}
		if(audioplayer->Running()){
			Windows::PlayerWindow();
		}
		
		nxmpgfx::Render_PreMPV();
		
		
		if(audioplayer->Running() && audioplayer->VisEnabled()){
			audioplayer->DrawProjectM();
		}
		
		nxmpgfx::Render_PostMPV();
		
		
		counter++;
		
		if(menuitem.state == MENU_STATE_EXIT)break;
		
		if(menuitem.state == MENU_STATE_GUILESS && !audioplayer->Running())break;
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