#include "appwindows.h"

#include <vector>
#include <iomanip>

CFSBrowser * fsbrowser = nullptr;

extern std::vector<std::string> audioextendions;

namespace Windows {
	float multiplyRes = 1.0f;

	std::string humanSize(size_t bytes)
	{
		std::vector<std::string>suffix = {"B", "KB", "MB", "GB", "TB"};
	
		int i = 0;
		double dblBytes = bytes;

		if (bytes > 1024) {
			for (i = 0; (bytes / 1024) > 0 && i<(int)suffix.size()-1; i++, bytes /= 1024)
				dblBytes = bytes / 1024.0;
			}

		char output[200];
		sprintf(output, "%.02lf %s", dblBytes, suffix[i].c_str());
		std::string ret = output;
		return ret;
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


	void MainMenuWindow() {
		Windows::SetupMainWindow();
	
		//std::vector<std::string> thislist;
	
		if (ImGui::Begin("NXMilk", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 5});
			//ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.26f, 0.59f, 0.98f, 0.00f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, nxmpgfx::NavHover_color);
			ImGui::PushStyleColor(ImGuiCol_NavHighlight, nxmpgfx::Active_color);
		
			if (ImGui::BeginTable("table1", 3,/*ImGuiTableFlags_RowBg|*/ImGuiTableFlags_ScrollY)){
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, (910.0f*multiplyRes -2 * ImGui::GetStyle().ItemSpacing.x)); 
					ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 125.0f*multiplyRes);
					ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed,255.f*multiplyRes);       
					ImGui::TableSetupScrollFreeze(0, 1);
					//ImGui::TableHeadersRow();
					ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Name");
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("Size");
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("Date");
					ImGuiListClipper clipper;
					std::vector<fsentry_struct> thislist = fsbrowser->getCurrList();
					//clipper.Begin(thislist.size());
					
					//while (clipper.Step())
					//{
					//	for (unsigned int n = clipper.DisplayStart; n < clipper.DisplayEnd; n++){
							
					//	}
						
					//}
					ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
					
					for (unsigned int n = 0; n < thislist.size(); n++){
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						
						if(S_ISDIR(thislist[n].st.st_mode)){
							ImGui::Image((void*)(intptr_t)imgloader->icons.FolderTexture.id, ImVec2(30*multiplyRes,30*multiplyRes));
						}else{
							ImGui::Image((void*)(intptr_t)imgloader->icons.FileTexture.id, ImVec2(30*multiplyRes,30*multiplyRes));
						
						}
						ImGui::SameLine();
						float curx = ImGui::GetCursorPosX();
						float cury = ImGui::GetCursorPosY();
						
						std::string itemid = "##" + std::to_string(n);
						if (ImGui::Selectable(itemid.c_str(), false,selectable_flags,ImVec2(0,30.0f*multiplyRes))){
							if(!S_ISDIR(thislist[n].st.st_mode)){
								bool loaded = audioplayer->LoadFile(fsbrowser->getCurrentPath()+"/"+thislist[n].filename);
								if(loaded){
									audioplayer->Play();
								}
							}else{
								fsbrowser->DirList(fsbrowser->getCurrentPath()+"/"+thislist[n].filename,false,audioextendions);
							}
							
						}
						ImGui::SameLine();
						ImGui::SetCursorPosX(curx);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()+5.0f);
						ImGui::Text("%s",thislist[n].filename.c_str());
						
						ImGui::TableSetColumnIndex(1);
						if(!S_ISDIR(thislist[n].st.st_mode)){
							ImGui::SetCursorPosY(ImGui::GetCursorPosY()+5.0f);
							ImGui::Text("%s",humanSize(thislist[n].st.st_size).c_str());
						}
						ImGui::TableSetColumnIndex(2);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()+5.0f);
						ImGui::Text("%s",formatTimeShort(thislist[n].st.st_mtime).c_str());
						
						
					}
					ImGui::SetWindowFocus();
					ImGui::EndTable();
		
			ImGui::PopStyleVar();
			ImGui::PopStyleColor(2);
			}
			Windows::ExitMainWindow();
		}
	}
}