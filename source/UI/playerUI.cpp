#include "appwindows.h"

#include "playerOSD.h"

extern CIniParser *configini;

namespace Windows {
	
	
	double shortControlsHide = 0.0f;
	float shorttextfadealpha = 1.0f;
	
	double imgControlsHide = 0.0f;
	float textfadealpha = 1.0f;
	double titleCycle = 0.0f;
	int titleCyclePos = 0;

	
	
	
	
	void PlayerWindow(){
		SetupPlayerWindow();
		if (ImGui::Begin("##playerwindow", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0,1.0,1.0,0.0));
		
		ImGuiContext& g = *GImGui;
		if(g.Time - imgControlsHide < configini->getAudioPlayerConfig().interfacehidesec){
			if(g.Time - imgControlsHide > configini->getAudioPlayerConfig().interfacehidesec-2 ){
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
					titlestring = "Title: " + audioplayer->getID3().title;
				}
			}
			if(titleCyclePos == 2){
				if(!audioplayer->getID3().title.empty()){
					titlestring = "Artist: " + audioplayer->getID3().artist;
				}
			}
			if(titleCyclePos == 3){
				if(!audioplayer->getID3().title.empty()){
					titlestring =  "Album: " + audioplayer->getID3().album;
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
		ImVec2 bitratesize = ImGui::CalcTextSize(bitratestr.c_str());
		ImVec2 sampleratesize = ImGui::CalcTextSize(sampleratestr.c_str());
		
		float maxsize = codecsize.x+20.0f;
		
		if(fileformatsize.x>maxsize)maxsize=fileformatsize.x+20.0f;
		if(bitratesize.x>maxsize)maxsize=bitratesize.x+20.0f;
		if(sampleratesize.x>maxsize)maxsize=sampleratesize.x+20.0f;
		
		
		
		
		draw_list->AddRectFilled(ImVec2(10, 60.0f), ImVec2(maxsize, 60.0f*multiplyRes+10.0f+((codecsize.y+10.0f)*4.0f)), backcolor, 3.f, ImDrawFlags_RoundCornersAll );
		ImVec2 formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes);
		//std::string bitratestr = std::string("Bitrate: ") + std::to_string(audioplayer->getBitRate()/1000) + std::string(" kbps");
		draw_list->AddText(formattextpos, ImColor(mytextcol),codecstr.c_str());
		
		formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes+codecsize.y+10.0f);
		//std::string sampleratestr = std::string("Sample Rate: ") + std::to_string(audioplayer->getSampleRate()) + std::string(" Hz");
		draw_list->AddText(formattextpos, ImColor(mytextcol),bitratestr.c_str());
		formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes+codecsize.y+10.0f+bitratesize.y+10.0f);
		draw_list->AddText(formattextpos, ImColor(mytextcol),sampleratestr.c_str());
		
		formattextpos = ImVec2(15.0f*multiplyRes,65.0f*multiplyRes+codecsize.y+10.0f+bitratesize.y+10.0f+fileformatsize.y+10.0f);
		draw_list->AddText(formattextpos, ImColor(mytextcol),fileformatstr.c_str());
		
		ImVec2 image_pnew = ImVec2(1280.0f*multiplyRes-210.0f*multiplyRes,65.0f*multiplyRes*multiplyRes); 
		ImVec2 image_p1 = ImVec2(1280.0f*multiplyRes-10.0f*multiplyRes,275.0f*multiplyRes);;
		
		
		
		
		ImVec4 testtextcol = ImVec4(1.0f,1.0f,1.0f,1.0f);
		ImVec4 testbackcol = ImVec4(0.0f,0.0f,0.0f,1.0f);
		playerOSD::pjm_playlistitem_render(draw_list,audioplayer->getCurrentPlaylistItem(),testtextcol,testbackcol);
		
		
		playerOSD::playerProgressBar(draw_list,audioplayer->getPosition()/1000000.0f,audioplayer->getDuration()/1000000.0f,textfadealpha);
		
		
		
		
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0,0.0,0.0,0.0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0,0.0,0.0,0.0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered , ImVec4(0.0,0.0,0.0,0.0));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0,0.0,0.0,0.0));
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, ImVec4(1.0,0.0,0.0,1.0));
		
		
		ImVec2 playpausepos1 = ImVec2((1280.0f*multiplyRes-60.0f*multiplyRes)/2.0f,720.0f*multiplyRes-75.0f*multiplyRes);
		ImVec2 playpausepos2 = ImVec2((1280.0f*multiplyRes+60.0f*multiplyRes)/2.0f,720.0f*multiplyRes-15.0f*multiplyRes);
		
		ImVec2 playpausepos1_n = ImVec2(playpausepos1.x,playpausepos1.y+2.0f);
		ImVec2 playpausepos2_n = ImVec2(playpausepos2.x,playpausepos2.y+2.0f);
		
		
		
		
		ImVec2 ffpos1 = playpausepos1;
		ffpos1.x = ffpos1.x + 80.0f*multiplyRes + 10.0f*multiplyRes;
		ffpos1.y = ffpos1.y + 10.0f*multiplyRes;
		
		ImVec2 ffpos2 = ffpos1;
		ffpos2.x = ffpos2.x+40.0f*multiplyRes;
		ffpos2.y = ffpos2.y+40.0f*multiplyRes;
		
		ImVec2 ffpos1_n = ImVec2(ffpos1.x,ffpos1.y+2.0f);
		ImVec2 ffpos2_n = ImVec2(ffpos2.x,ffpos2.y+2.0f);
		
		
		ImVec2 rewpos1 = playpausepos1;
		rewpos1.x = rewpos1.x - 60.0f*multiplyRes - 10.0f*multiplyRes;
		rewpos1.y = rewpos1.y + 10.0f*multiplyRes;
		
		ImVec2 rewpos2 = rewpos1;
		rewpos2.x = rewpos2.x+40.0f*multiplyRes;
		rewpos2.y = rewpos2.y+40.0f*multiplyRes;
		
		ImVec2 rewpos1_n = ImVec2(rewpos1.x,rewpos1.y+2.0f);
		ImVec2 rewpos2_n = ImVec2(rewpos2.x,rewpos2.y+2.0f);
		
		
		ImGui::SetCursorPos(playpausepos1);
		
		if (ImGui::Selectable("##PlayPause", false ,0,ImVec2(60*multiplyRes, 60*multiplyRes))){
				audioplayer->Pause();
		}
		bool isItemHovered = ImGui::IsItemHovered();
		
		ImVec4 myplaybutcol = ImVec4(0.8f,0.8f,0.8f,textfadealpha);
		ImVec4 myplaybutcolHover = ImVec4(1.0f,1.0f,1.0f,textfadealpha);
		
		
		
		
		if(audioplayer->isPaused()){
			draw_list->AddImage((void*)(intptr_t)imgloader->icons.PauseIcon.id,playpausepos1_n,playpausepos2_n,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,0.0f,0.0f,mytextcol.w)));
			draw_list->AddImage((void*)(intptr_t)imgloader->icons.PauseIcon.id,playpausepos1,playpausepos2,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(isItemHovered?myplaybutcolHover:myplaybutcol));
			
		}else{
			draw_list->AddImage((void*)(intptr_t)imgloader->icons.PlayIcon.id,playpausepos1_n,playpausepos2_n,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,0.0f,0.0f,mytextcol.w)));
			draw_list->AddImage((void*)(intptr_t)imgloader->icons.PlayIcon.id,playpausepos1,playpausepos2,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(isItemHovered?myplaybutcolHover:myplaybutcol));
			
		}
		
		
		
		ImGui::SetCursorPos(ImVec2(ffpos1.x,ffpos1.y-10.0f*multiplyRes));
		if (ImGui::Selectable("##FFbut", false ,0,ImVec2(60*multiplyRes, 60*multiplyRes))){
				audioplayer->Seek(5.0f);
		}
		
		isItemHovered = ImGui::IsItemHovered();
		
		
		draw_list->AddImage((void*)(intptr_t)imgloader->icons.FFIcon.id,ffpos1_n,ffpos2_n,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,0.0f,0.0f,mytextcol.w)));
		draw_list->AddImage((void*)(intptr_t)imgloader->icons.FFIcon.id,ffpos1,ffpos2,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(isItemHovered?myplaybutcolHover:myplaybutcol));
		
		ImGui::SetCursorPos(ImVec2(rewpos1.x,rewpos1.y-10.0f*multiplyRes));
		if (ImGui::Selectable("##Rewbut", false ,0,ImVec2(60*multiplyRes, 60*multiplyRes))){
				audioplayer->Seek(-5.0f);
		}
		
		isItemHovered = ImGui::IsItemHovered();
		
		draw_list->AddImage((void*)(intptr_t)imgloader->icons.RewIcon.id,rewpos1_n,rewpos2_n,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,0.0f,0.0f,mytextcol.w)));
		draw_list->AddImage((void*)(intptr_t)imgloader->icons.RewIcon.id,rewpos1,rewpos2,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(isItemHovered?myplaybutcolHover:myplaybutcol));
				


		
		ImVec2 prevpos1 = rewpos1;
		prevpos1.x = prevpos1.x - 60.0f*multiplyRes - 10.0f*multiplyRes;
		prevpos1.y = playpausepos1.y + 10.0f*multiplyRes;
		
		ImVec2 prevpos2 = prevpos1;
		prevpos2.x = prevpos2.x+40.0f*multiplyRes;
		prevpos2.y = prevpos2.y+40.0f*multiplyRes;
		
		ImVec2 prevpos1_n = ImVec2(prevpos1.x,prevpos1.y+2.0f);
		ImVec2 prevpos2_n = ImVec2(prevpos2.x,prevpos2.y+2.0f);


		ImGui::SetCursorPos(ImVec2(prevpos1.x,prevpos1.y-10.0f*multiplyRes));
		if (ImGui::Selectable("##Prevbut", false ,0,ImVec2(60*multiplyRes, 60*multiplyRes))){
				
		}
		
		isItemHovered = ImGui::IsItemHovered();
		
		draw_list->AddImage((void*)(intptr_t)imgloader->icons.PrevIcon.id,prevpos1_n,prevpos2_n,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,0.0f,0.0f,mytextcol.w)));
		draw_list->AddImage((void*)(intptr_t)imgloader->icons.PrevIcon.id,prevpos1,prevpos2,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(isItemHovered?myplaybutcolHover:myplaybutcol));
		
		
		ImVec2 nextpos1 = ffpos1;
		nextpos1.x = nextpos1.x + 60.0f*multiplyRes + 10.0f*multiplyRes;
		nextpos1.y = playpausepos1.y + 10.0f*multiplyRes;
		
		ImVec2 nextpos2 = nextpos1;
		nextpos2.x = nextpos2.x+40.0f*multiplyRes;
		nextpos2.y = nextpos2.y+40.0f*multiplyRes;
		
		ImVec2 nextpos1_n = ImVec2(nextpos1.x,nextpos1.y+2.0f);
		ImVec2 nextpos2_n = ImVec2(nextpos2.x,nextpos2.y+2.0f);


		ImGui::SetCursorPos(ImVec2(nextpos1.x,nextpos1.y-10.0f*multiplyRes));
		if (ImGui::Selectable("##Nextbut", false ,0,ImVec2(60*multiplyRes, 60*multiplyRes))){
				
		}
		
		isItemHovered = ImGui::IsItemHovered();
		
		draw_list->AddImage((void*)(intptr_t)imgloader->icons.NextIcon.id,nextpos1_n,nextpos2_n,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,0.0f,0.0f,mytextcol.w)));
		draw_list->AddImage((void*)(intptr_t)imgloader->icons.NextIcon.id,nextpos1,nextpos2,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(isItemHovered?myplaybutcolHover:myplaybutcol));
		
		
		
		
		ImGui::PopStyleColor(5);
		
		
		if(audioplayer->haveAlbumArt){
			draw_list->AddImage((void*)(intptr_t)audioplayer->AlbumArtTexture.id,image_pnew,image_p1,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(mytextcol));
		}
		
		ImGui::PopStyleColor();
		
		ImGui::SetWindowFocus();
		}
		ExitPlayerWindow();
		
	}
	
	
	void UserActivity(){
		ImGuiContext& g = *GImGui;
		imgControlsHide = g.Time;
		titleCycle = g.Time;
		textfadealpha = 1.0f;
		titleCyclePos = 0;
		playerOSD::pjm_playlistitem_show(10.0f);
	}
	
	void VisPlaylistActivity(){
		playerOSD::pjm_playlistitem_show(4.0f);
	}
	
}