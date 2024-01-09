#ifndef NXMILK_WINDOWS_H
#define NXMILK_WINDOWS_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "nxmp-gfx.h"

#include "fsbrowser.h"
#include "audioplayer.h"
#include "imgloader.h"

extern CFSBrowser * fsbrowser;
extern CAudioPlayer * audioplayer;
extern CImgLoader *imgloader;

extern std::vector<std::string> audioextendions;

namespace Windows {
	
extern float multiplyRes;

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
	
	void PlayerWindow();
	
	void UserActivity();
}


#endif