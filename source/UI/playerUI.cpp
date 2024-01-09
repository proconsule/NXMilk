#include "appwindows.h"


namespace Windows {
	
	
	double imgControlsHide = 0.0f;
	float textfadealpha = 1.0f;
	
	
	void PlayerWindow(){
		SetupPlayerWindow();
		if (ImGui::Begin("##playerwindow", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0,1.0,1.0,0.0));
		
		ImGuiContext& g = *GImGui;
		if(g.Time - imgControlsHide < 5){
			if(g.Time - imgControlsHide > 3 ){
				textfadealpha = textfadealpha-0.016;
			}
		
		}
		
		
		ImVec4 mytextcol = ImVec4(1.0f,1.0f,1.0f,textfadealpha);
		float backfade = textfadealpha*0.4f;
		ImColor backcolor(0.0f, 0.0f, 0.0f, backfade);
		
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		
		
		ImVec2 titlesize = ImGui::CalcTextSize(audioplayer->getFileName().c_str());
		ImVec2 titlepos = ImVec2((1280.0f*multiplyRes-titlesize.x)/2.0,15.0f*multiplyRes);
		
		draw_list->AddRectFilled(ImVec2(10, 10.0f), ImVec2(1280.0f*multiplyRes-10.0f, 10.0f+titlesize.y + 10.0f), backcolor, 3.f, ImDrawFlags_RoundCornersAll );
		draw_list->AddText(titlepos, ImColor(mytextcol),audioplayer->getFileName().c_str());
		
		std::string codecstr = std::string("Codec: ") + audioplayer->getCodec();
		std::string bitratestr = std::string("Bitrate: ") + std::to_string(audioplayer->getBitRate()/1000) + std::string(" kbps");
		std::string sampleratestr = std::string("Sample Rate: ") + std::to_string(audioplayer->getSampleRate()) + std::string(" Hz");
		ImVec2 codecsize = ImGui::CalcTextSize(codecstr.c_str());
		
		float maxsize = codecsize.x+20.0f;
		
		
		
		draw_list->AddRectFilled(ImVec2(10, 60.0f), ImVec2(maxsize, 200.0f), backcolor, 3.f, ImDrawFlags_RoundCornersAll );
		ImVec2 formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes);
		//std::string bitratestr = std::string("Bitrate: ") + std::to_string(audioplayer->getBitRate()/1000) + std::string(" kbps");
		draw_list->AddText(formattextpos, ImColor(mytextcol),codecstr.c_str());
		
		formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes+codecsize.y+10.0f);
		//std::string sampleratestr = std::string("Sample Rate: ") + std::to_string(audioplayer->getSampleRate()) + std::string(" Hz");
		draw_list->AddText(formattextpos, ImColor(mytextcol),bitratestr.c_str());
		ImVec2 bitratesize = ImGui::CalcTextSize(bitratestr.c_str());
		formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes+codecsize.y+10.0f+bitratesize.y+10.0f);
		draw_list->AddText(formattextpos, ImColor(mytextcol),sampleratestr.c_str());
		
		
		
		ImGui::PopStyleColor();
		
		}
		ExitPlayerWindow();
		
	}
	
	
	void UserActivity(){
		ImGuiContext& g = *GImGui;
		imgControlsHide = g.Time;
		textfadealpha = 1.0f;
	}
	
}