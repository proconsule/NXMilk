#include "appwindows.h"

#include "playerOSD.h"



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
		
		/*
		DrawPlayIcon(draw_list,ImVec2(10.0f,720.0f*multiplyRes-100.0f*multiplyRes),40.0f,mytextcol);
		DrawPauseIcon(draw_list,ImVec2(100.0f,720.0f*multiplyRes-100.0f*multiplyRes),40.0f,mytextcol);
		DrawFFIcon(draw_list,ImVec2(200.0f,720.0f*multiplyRes-100.0f*multiplyRes),40.0f,mytextcol);
		*/
		
		/*
		
		ImVec2 playlistitemsize = ImGui::CalcTextSize(audioplayer->getCurrentPlaylistItem().c_str());
		formattextpos = ImVec2((1280.0f*multiplyRes-playlistitemsize.x)/2.0f,720.0f*multiplyRes-40.0f*multiplyRes);
		
		
		draw_list->AddRectFilled(ImVec2(10, formattextpos.y-5.0f*multiplyRes), ImVec2(1280.0f*multiplyRes-10.0f, formattextpos.y+playlistitemsize.y+5.0f*multiplyRes), backcolor, 3.f, ImDrawFlags_RoundCornersAll );
		
		draw_list->AddText(formattextpos, ImColor(mytextcol),audioplayer->getCurrentPlaylistItem().c_str());
		*/
		
		
		ImVec4 testtextcol = ImVec4(1.0f,1.0f,1.0f,1.0f);
		ImVec4 testbackcol = ImVec4(0.0f,0.0f,0.0f,1.0f);
		playerOSD::pjm_playlistitem_render(draw_list,audioplayer->getCurrentPlaylistItem(),testtextcol,testbackcol);
		
		
		playerOSD::playerProgressBar(draw_list,audioplayer->getPosition()/1000000.0f,audioplayer->getDuration()/1000000.0f,textfadealpha);
		
		
		ImVec2 playpausepos1 = ImVec2((1280.0f*multiplyRes-60.0f*multiplyRes)/2.0f,720.0f*multiplyRes-75.0f*multiplyRes);
		ImVec2 playpausepos2 = ImVec2((1280.0f*multiplyRes+60.0f*multiplyRes)/2.0f,720.0f*multiplyRes-15.0f*multiplyRes);
		
		ImVec2 playpausepos1_n = ImVec2(playpausepos1.x,playpausepos1.y+2.0f);
		ImVec2 playpausepos2_n = ImVec2(playpausepos2.x,playpausepos2.y+2.0f);
		
		
		
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0,0.0,0.0,0.0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0,0.0,0.0,0.0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered , ImVec4(0.0,0.0,0.0,0.0));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0,0.0,0.0,0.0));
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, ImVec4(1.0,0.0,0.0,1.0));
		ImGui::SetCursorPos(playpausepos1);
		
		if (ImGui::Selectable("##PlayPause", false ,0,ImVec2(60*multiplyRes, 60*multiplyRes))){
				audioplayer->Pause();
		}
		ImGui::SetCursorPos(playpausepos1);
		if(audioplayer->isPaused()){
			draw_list->AddImage((void*)(intptr_t)imgloader->icons.PauseIcon.id,playpausepos1_n,playpausepos2_n,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,0.0f,0.0f,mytextcol.w)));
			draw_list->AddImage((void*)(intptr_t)imgloader->icons.PauseIcon.id,playpausepos1,playpausepos2,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(mytextcol));
			
		}else{
			draw_list->AddImage((void*)(intptr_t)imgloader->icons.PlayIcon.id,playpausepos1_n,playpausepos2_n,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,0.0f,0.0f,mytextcol.w)));
			draw_list->AddImage((void*)(intptr_t)imgloader->icons.PlayIcon.id,playpausepos1,playpausepos2,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(mytextcol));
			
		}
		
		unsigned int playpausetextureid = 0;
		
		if(audioplayer->isPaused()){
			playpausetextureid = imgloader->icons.PauseIcon.id;
			//draw_list->AddImage((void*)(intptr_t)imgloader->icons.PauseIcon.id,playpausepos1,playpausepos2,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(mytextcol));
			
		}else{
			playpausetextureid = imgloader->icons.PlayIcon.id;
			
			//draw_list->AddImage((void*)(intptr_t)imgloader->icons.PlayIcon.id,playpausepos1,playpausepos2,ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::ColorConvertFloat4ToU32(mytextcol));
		}
		/*
		if(ImGui::ImageButton("##playpausebut",(void*)(intptr_t)playpausetextureid,ImVec2(60.0f,60.0f),ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),ImVec4(0.0f,0.0f,0.0f,0.0f),mytextcol)){
				audioplayer->Pause();
		}
		*/
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