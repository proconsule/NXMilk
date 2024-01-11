#include "appwindows.h"


namespace Windows {
	
	
	double imgControlsHide = 0.0f;
	float textfadealpha = 1.0f;
	double titleCycle = 0.0f;
	int titleCyclePos = 0;

	
	void DrawFFIcon(ImDrawList* draw_list,ImVec2 pos,float size,ImVec4 _color){
		
		ImVec2 p1 = ImVec2(pos.x,pos.y-size/2.0f);
		ImVec2 p2 = ImVec2(pos.x,pos.y+size/2.0f);
		ImVec2 p3 = ImVec2(pos.x+((size/3.0f)*2.0),pos.y);
		
		
		ImVec2 p4 = ImVec2(pos.x+(size/3.0f),pos.y-size/2.0f);
		ImVec2 p5 = ImVec2(pos.x+(size/3.0f),pos.y+size/2.0f);
		ImVec2 p6 = ImVec2(pos.x+size,pos.y);
		
		
		draw_list->AddTriangleFilled(p1,p2,p3,ImGui::ColorConvertFloat4ToU32(_color));
		draw_list->AddTriangleFilled(p4,p5,p6,ImGui::ColorConvertFloat4ToU32(_color));
	}
	
	void DrawPlayIcon(ImDrawList* draw_list,ImVec2 pos,float size,ImVec4 _color){
		
		ImVec2 p1 = ImVec2(pos.x,pos.y-size/2.0f);
		ImVec2 p2 = ImVec2(pos.x,pos.y+size/2.0f);
		ImVec2 p3 = ImVec2(pos.x+size,pos.y);
		
		draw_list->AddTriangleFilled(p1,p2,p3,ImGui::ColorConvertFloat4ToU32(_color));
	}
	
	void DrawPauseIcon(ImDrawList* draw_list,ImVec2 pos,float size,ImVec4 _color){
		
		ImVec2 p1 = ImVec2(pos.x,pos.y-size/2.0f);
		ImVec2 p2 = ImVec2(pos.x+size/3.0f,pos.y+size/2.0f);
		
		ImVec2 p3 = ImVec2(pos.x+(size/3.0f)*2.0,pos.y-size/2.0f);
		ImVec2 p4 = ImVec2(pos.x+size,pos.y+size/2.0f);
		
		draw_list->AddRectFilled(p1,p2,ImGui::ColorConvertFloat4ToU32(_color));
		draw_list->AddRectFilled(p3,p4,ImGui::ColorConvertFloat4ToU32(_color));
		
	}
	
	
	void PlayerWindow(){
		SetupPlayerWindow();
		if (ImGui::Begin("##playerwindow", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0,1.0,1.0,0.0));
		
		ImGuiContext& g = *GImGui;
		if(g.Time - imgControlsHide < 10){
			if(g.Time - imgControlsHide > 8 ){
				textfadealpha = textfadealpha-0.016;
			}
		
		}
		
		
		
		
		
		
		ImVec4 mytextcol = ImVec4(1.0f,1.0f,1.0f,textfadealpha);
		float backfade = textfadealpha*0.4f;
		ImColor backcolor(0.0f, 0.0f, 0.0f, backfade);
		
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		
		
		std::string titlestring = audioplayer->getFileName();
		
		if(titleCyclePos == 0){
				titlestring = audioplayer->getFileName();
			}
			if(titleCyclePos == 1){
				if(!audioplayer->getID3().title.empty()){
					titlestring = audioplayer->getID3().title;
				}
			}
			if(titleCyclePos == 2){
				if(!audioplayer->getID3().title.empty()){
					titlestring = audioplayer->getID3().artist;
				}
			}
			if(titleCyclePos == 3){
				if(!audioplayer->getID3().title.empty()){
					titlestring = audioplayer->getID3().album;
				}
			}
		
		
		if(g.Time - titleCycle > 2.0){
			titleCycle = g.Time;
			titleCyclePos=titleCyclePos+1;
			if(titleCyclePos>3)titleCyclePos = 0;
			
			
			
		}
		
		/*
		if(!audioplayer->getID3().title.empty()){
			titlestring = audioplayer->getID3().artist + "-" + audioplayer->getID3().title;
		}
		*/
		
		ImVec2 titlesize = ImGui::CalcTextSize(titlestring.c_str());
		ImVec2 titlepos = ImVec2((1280.0f*multiplyRes-titlesize.x)/2.0,15.0f*multiplyRes);
		
		draw_list->AddRectFilled(ImVec2(10, 10.0f), ImVec2(1280.0f*multiplyRes-10.0f, 10.0f+titlesize.y + 10.0f), backcolor, 3.f, ImDrawFlags_RoundCornersAll );
		
		
		draw_list->AddText(titlepos, ImColor(mytextcol),titlestring.c_str());
		
		std::string codecstr = std::string("Codec: ") + audioplayer->getCodec();
		std::string bitratestr = std::string("Bitrate: ") + std::to_string(audioplayer->getBitRate()/1000) + std::string(" kbps");
		std::string sampleratestr = std::string("Sample Rate: ") + std::to_string(audioplayer->getSampleRate()) + std::string(" Hz");
		std::string fileformatstr = std::string("Format: ") + audioplayer->getFormat();
		
		ImVec2 codecsize = ImGui::CalcTextSize(codecstr.c_str());
		ImVec2 fileformatsize = ImGui::CalcTextSize(fileformatstr.c_str());
		
		float maxsize = codecsize.x+20.0f;
		
		if(fileformatsize.x>codecsize.x)maxsize=fileformatsize.x+20.0f;
		
		
		
		
		
		draw_list->AddRectFilled(ImVec2(10, 60.0f), ImVec2(maxsize, 60.0f*multiplyRes+10.0f+((codecsize.y+10.0f)*4.0f)), backcolor, 3.f, ImDrawFlags_RoundCornersAll );
		ImVec2 formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes);
		//std::string bitratestr = std::string("Bitrate: ") + std::to_string(audioplayer->getBitRate()/1000) + std::string(" kbps");
		draw_list->AddText(formattextpos, ImColor(mytextcol),codecstr.c_str());
		
		formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes+codecsize.y+10.0f);
		//std::string sampleratestr = std::string("Sample Rate: ") + std::to_string(audioplayer->getSampleRate()) + std::string(" Hz");
		draw_list->AddText(formattextpos, ImColor(mytextcol),bitratestr.c_str());
		ImVec2 bitratesize = ImGui::CalcTextSize(bitratestr.c_str());
		formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes+codecsize.y+10.0f+bitratesize.y+10.0f);
		draw_list->AddText(formattextpos, ImColor(mytextcol),sampleratestr.c_str());
		
		formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes+codecsize.y+10.0f+bitratesize.y+10.0f+fileformatsize.y+10.0f);
		draw_list->AddText(formattextpos, ImColor(mytextcol),fileformatstr.c_str());
		
		ImVec2 image_pnew = ImVec2(1280.0f*multiplyRes-210.0f*multiplyRes,65.0f*multiplyRes*multiplyRes); 
		ImVec2 image_p1 = ImVec2(1280.0f*multiplyRes-10.0f*multiplyRes,275.0f*multiplyRes);;
		
		
		DrawPlayIcon(draw_list,ImVec2(10.0f,720.0f*multiplyRes-100.0f*multiplyRes),40.0f,mytextcol);
		DrawPauseIcon(draw_list,ImVec2(100.0f,720.0f*multiplyRes-100.0f*multiplyRes),40.0f,mytextcol);
		DrawFFIcon(draw_list,ImVec2(200.0f,720.0f*multiplyRes-100.0f*multiplyRes),40.0f,mytextcol);
		
		
		
		if(audioplayer->haveAlbumArt){
			draw_list->AddImage((void*)(intptr_t)audioplayer->AlbumArtTexture.id,image_pnew,image_p1,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(mytextcol));
		}
		
		ImGui::PopStyleColor();
		
		}
		ExitPlayerWindow();
		
	}
	
	
	void UserActivity(){
		ImGuiContext& g = *GImGui;
		imgControlsHide = g.Time;
		titleCycle = g.Time;
		textfadealpha = 1.0f;
		titleCyclePos = 0;
	}
	
}