#ifndef NXMP_PLAYEROSD_H
#define NXMP_PLAYEROSD_H

#include <string>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

namespace playerOSD {
	
	
	void pjm_playlistitem_show(double _showtime);
	void pjm_playlistitem_render(ImDrawList* draw_list,std::string itemText,ImVec4 textcol,ImVec4 backcol);
	
	void playerProgressBar(ImDrawList* draw_list,int currentsec,int totoalsec,float alphavalue);
	
	
	void DrawFFIcon(ImDrawList* draw_list,ImVec2 pos,float size,ImVec4 _color);
		
	void DrawPlayIcon(ImDrawList* draw_list,ImVec2 pos,float size,ImVec4 _color);
		
	void DrawPauseIcon(ImDrawList* draw_list,ImVec2 pos,float size,ImVec4 _color);
	
};



#endif