#ifndef NXMILK_WINDOWS_H
#define NXMILK_WINDOWS_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "nxmp-gfx.h"

#include "fsbrowser.h"
#include "audioplayer.h"
#include "imgloader.h"
#include "utils.h"

extern CFSBrowser * fsbrowser;
extern CAudioPlayer * audioplayer;
extern CImgLoader *imgloader;
extern CIniParser *configini;

extern std::vector<std::string> audioextensions;

extern float multiplyRes;

enum MENU_STATES {
	MENU_STATE_HOME,
    MENU_STATE_FILEBROWSER,
	MENU_STATE_USB,
	MENU_STATE_NETWORKSEL,
	MENU_STATE_EXIT,
	MENU_STATE_GUILESS
};

typedef struct menuitem_struct{
	MENU_STATES state;
	//bool filemount = false;
	
};


extern menuitem_struct menuitem;




namespace Windows {
	


inline void SetupMainWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes), ImGuiCond_Always);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg,nxmpgfx::Window_Bg_color);
    };

inline void SetupWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg,nxmpgfx::Window_Bg_color);
    };

    inline void ExitMainWindow(void) {
        ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
        
    };
	inline void ExitWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar();
		ImGui::PopStyleColor();
    };
	
	inline void SetupPlayerWindow(void){
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f*multiplyRes), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0,0.0,0.0,0.0));
	}
	
	inline void ExitPlayerWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar();
		ImGui::PopStyleColor();
    };
	
	void MainMenuWindow();
	void HomeWindow();
	void USBWindow();
	void NetworkWindow();
	
	void PlayerWindow();
	
	void UserActivity();
	void VisPlaylistActivity();
}


#endif