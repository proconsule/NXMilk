#include "playerOSD.h"

#include <iomanip>
#include "audioplayer.h"

extern float multiplyRes;
extern CAudioPlayer * audioplayer;


double relseek = 0.0f;
bool mouseoverPos = false;

namespace playerOSD {
	
	double playlistitem_timer = 0.0f;
	double playlistitem_showtime = 0.0f;
	float playlistitem_alpha = 0.0f;
	float playlistitembackground_alpha = 0.0f;
	
	void pjm_playlistitem_show(double _showtime){
		ImGuiContext& g = *GImGui;
		playlistitem_showtime = _showtime;
		playlistitem_timer = g.Time;
		playlistitem_alpha = 1.0f;
		playlistitembackground_alpha = 0.4f;
	}
	
	std::string formatTimeShort(double seconds) {

		int h((int) seconds / 3600);
		int min((int) seconds / 60 - h * 60);
		int sec((int) seconds - (h * 60 + min) * 60);

		std::ostringstream oss;
		if (h > 0) {
			oss << std::setfill('0') << std::setw(2) << h << ":";
		}
		//if (min > 0) {
			oss << std::setfill('0') << std::setw(2) << min << ":";
		//}
		oss << std::setfill('0') << std::setw(2) << sec;

		return oss.str();
	}
	
	void playerProgressBar(ImDrawList* draw_list,int currentsec,int totoalsec,float alphavalue){
		ImVec2 barsize = ImVec2(1280.0f*multiplyRes-20.0f*multiplyRes,30.0f*multiplyRes);
		float xpos = (currentsec*barsize.x*multiplyRes)/totoalsec;
		
		
		ImVec2 curpostextsize = ImGui::CalcTextSize(formatTimeShort(currentsec).c_str());
		ImVec2 totalsectextsize = ImGui::CalcTextSize(formatTimeShort(totoalsec).c_str());
		
		
		ImVec4 backbackcolor = ImVec4(1.0f,1.0f,1.0f,0.5f*alphavalue);
		ImVec4 backbackcolorneg = ImVec4(0.0f,0.0f,0.0f,0.5f*alphavalue);
		ImVec4 poscolor = ImVec4(1.0f,1.0f,1.0f,1.0f*alphavalue);
		ImVec4 poscolorneg = ImVec4(0.0f,0.0f,0.0f,1.0f*alphavalue);
		
		
		
		
		ImVec2 backp1 = ImVec2(10.0f,720.0f*multiplyRes-120.0f*multiplyRes);
		ImVec2 backp2 = ImVec2(backp1.x+barsize.x,backp1.y+barsize.y);
		
		
		ImVec2 mousePositionAbsolute = ImGui::GetMousePos();
		
		
		
		
		ImVec2 posp1 = ImVec2(backp1.x+xpos,backp1.y);
		ImVec2 posp2 = ImVec2(posp1.x+30.0f*multiplyRes,backp2.y);
		
		
		
		
		
		if(mouseoverPos){
			
			posp1 = ImVec2(mousePositionAbsolute.x-15.0f,backp1.y);
			posp2 = ImVec2(posp1.x+30.0f*multiplyRes,backp2.y);
			
			
			posp1.x=posp1.x-5.0f;
			posp1.y=posp1.y-5.0f;
			posp2.x=posp2.x+5.0f;
			posp2.y=posp2.y+5.0f;
			relseek = posp1.x;
			
		}
		
		
		if(ImGui::IsMouseDown(ImGuiMouseButton_Left)){
			if(mousePositionAbsolute.x>=posp1.x && mousePositionAbsolute.x<=posp2.x){
				if(mousePositionAbsolute.y>=posp1.y && mousePositionAbsolute.y<=posp2.y){
					
					mouseoverPos = true;
				}
			}
		}else if(mouseoverPos){
			mouseoverPos = false;
			int dstsec =  (relseek*totoalsec)/(1280.0f*multiplyRes);
			audioplayer->SeekAbs(dstsec);
			relseek = 0.0f;
		}
		
		ImVec2 posptextcur = ImVec2(backp1.x +10.0f*multiplyRes,backp1.y-curpostextsize.y-10.0f*multiplyRes);
		
		ImVec2 posptexttotal = ImVec2(backp2.x - totalsectextsize.x - 10.0f*multiplyRes,backp1.y-totalsectextsize.y-10.0f*multiplyRes);
		
		
		draw_list->AddRectFilled(backp1, backp2, ImColor(backbackcolorneg), 3.f, ImDrawFlags_RoundCornersAll );
		draw_list->AddRect(backp1, backp2, ImColor(backbackcolor), 3.f, ImDrawFlags_RoundCornersAll );
		
		
		ImVec2 postextcurbackp1 = posptextcur;
		postextcurbackp1.x = postextcurbackp1.x-5.0f;
		postextcurbackp1.y = postextcurbackp1.y-5.0f;
		
		ImVec2 postextcurbackp2 = posptextcur;
		postextcurbackp2.x = postextcurbackp2.x + curpostextsize.x +5.0f;
		postextcurbackp2.y = postextcurbackp2.y + curpostextsize.y +5.0f;
		
		ImVec2 postexttotalbackp1 = posptexttotal;
		postexttotalbackp1.x = postexttotalbackp1.x-5.0f;
		postexttotalbackp1.y = postexttotalbackp1.y-5.0f;
		
		ImVec2 postexttotalbackp2 = posptexttotal;
		postexttotalbackp2.x = postexttotalbackp2.x + curpostextsize.x +5.0f;
		postexttotalbackp2.y = postexttotalbackp2.y + curpostextsize.y +5.0f;
		
		
		draw_list->AddRectFilled(postextcurbackp1, postextcurbackp2, ImColor(backbackcolorneg), 3.f, ImDrawFlags_RoundCornersAll );
		draw_list->AddRect(postextcurbackp1, postextcurbackp2, ImColor(backbackcolor), 3.f, ImDrawFlags_RoundCornersAll );
		
		draw_list->AddText(posptextcur,ImColor(poscolor),formatTimeShort(currentsec).c_str());
		
		draw_list->AddRectFilled(postexttotalbackp1, postexttotalbackp2, ImColor(backbackcolorneg), 3.f, ImDrawFlags_RoundCornersAll );
		draw_list->AddRect(postexttotalbackp1, postexttotalbackp2, ImColor(backbackcolor), 3.f, ImDrawFlags_RoundCornersAll );
		
		draw_list->AddText(posptexttotal,ImColor(poscolor),formatTimeShort(totoalsec).c_str());
		
		
		draw_list->AddRectFilled(posp1,posp2,ImColor(poscolor), 3.f, ImDrawFlags_RoundCornersAll);
		draw_list->AddRect(posp1,posp2,ImColor(poscolorneg), 3.f, ImDrawFlags_RoundCornersAll);
		
	}
	
	
	
	void pjm_playlistitem_render(ImDrawList* draw_list,std::string itemText,ImVec4 textcol,ImVec4 backcol){
		if(playlistitem_alpha>0.0f){
			textcol.w = playlistitem_alpha;
			backcol.w = playlistitembackground_alpha;
			ImGuiContext& g = *GImGui;
			if(g.Time - playlistitem_timer < playlistitem_showtime){
				if(g.Time - playlistitem_timer > playlistitem_showtime-2.0f ){
					playlistitem_alpha = playlistitem_alpha-0.016;
					playlistitembackground_alpha = 0.4f*playlistitem_alpha;
				}
			
			}
			
			ImVec2 playlistitemsize = ImGui::CalcTextSize(itemText.c_str());
			ImVec2 formattextpos = ImVec2((1280.0f*multiplyRes-playlistitemsize.x)/2.0f,720.0f*multiplyRes-200.0f*multiplyRes);
			
			draw_list->AddRectFilled(ImVec2(10, formattextpos.y-5.0f*multiplyRes), ImVec2(1280.0f*multiplyRes-10.0f, formattextpos.y+playlistitemsize.y+5.0f*multiplyRes), ImColor(backcol), 3.f, ImDrawFlags_RoundCornersAll );
			
			draw_list->AddText(formattextpos, ImColor(textcol),itemText.c_str());
			
		}
	}
	
	
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
		
		ImVec4 bordercolor = ImVec4(0.0f,0.0f,0.0f,_color.w);
		
		ImVec2 p1 = ImVec2(pos.x,pos.y-size/2.0f);
		ImVec2 p2 = ImVec2(pos.x,pos.y+size/2.0f);
		ImVec2 p3 = ImVec2(pos.x+size,pos.y);
		
		draw_list->AddTriangleFilled(p1,p2,p3,ImGui::ColorConvertFloat4ToU32(_color));
		draw_list->AddTriangle(p1,p2,p3,ImGui::ColorConvertFloat4ToU32(bordercolor));
	}
	
	void DrawPauseIcon(ImDrawList* draw_list,ImVec2 pos,float size,ImVec4 _color){
		
		ImVec4 bordercolor = ImVec4(0.0f,0.0f,0.0f,_color.w);
		
		ImVec2 p1 = ImVec2(pos.x,pos.y-size/2.0f);
		ImVec2 p2 = ImVec2(pos.x+size/3.0f,pos.y+size/2.0f);
		
		ImVec2 p3 = ImVec2(pos.x+(size/3.0f)*2.0,pos.y-size/2.0f);
		ImVec2 p4 = ImVec2(pos.x+size,pos.y+size/2.0f);
		
		draw_list->AddRectFilled(p1,p2,ImGui::ColorConvertFloat4ToU32(_color));
		draw_list->AddRect(p1,p2,ImGui::ColorConvertFloat4ToU32(bordercolor));
		
		draw_list->AddRectFilled(p3,p4,ImGui::ColorConvertFloat4ToU32(_color));
		draw_list->AddRect(p3,p4,ImGui::ColorConvertFloat4ToU32(bordercolor));
		
	}
	
}