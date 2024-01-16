#include "appwindows.h"

#include <vector>
#include <iomanip>
#include "usbfs.h"

CFSBrowser * fsbrowser = nullptr;

extern USBMounter *MyUSBMount;

extern bool USBDialog;

extern std::vector<std::string> audioextensions;



namespace Windows {
	
	std::string removeLastSlash(const std::string &string) {

		std::string str = string;
		size_t pos = str.find_last_of('/');
		if (pos == str.length() - 1) {
			str.erase(str.length() - 1);
		}
		return str;
	}
	

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
	
	
	
	std::string formatTimeStamp(time_t timestamp){
		
		struct tm* timeStruct = localtime((const time_t *)&timestamp);
		char test1[36];
		strftime(test1, 36, "%d/%m/%Y %H:%M:%S", timeStruct);
		return std::string(test1);
	}

	void USBWindow() {
		Windows::SetupMainWindow();
		if (ImGui::Begin("USB Devices", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			
			
			static int selected = -1;
			if(MyUSBMount != nullptr){	
				std::vector<usb_devices> thislist = MyUSBMount->mounted_devs;
				for (unsigned int n = 0; n < thislist.size(); n++){
					std::string itemid = "##" + std::to_string(n);
					if (ImGui::Selectable(itemid.c_str(), selected == n,0, ImVec2(0, 60))){
						std::string mountpath = thislist[n].mount_point + std::string("/");
						MyUSBMount->setBasePath(mountpath);
						fsbrowser->DirList(mountpath,false,audioextensions);
					}
					ImGui::SameLine();
					
					ImGui::Text("%s %s",MyUSBMount->mounted_devs[n].fstype.c_str(),humanSize(MyUSBMount->mounted_devs[n].capacity).c_str());
				}
			}
		
		
		
		Windows::ExitMainWindow();
		}
		
		
	}

	void MainMenuWindow() {
		Windows::SetupMainWindow();
	
		//std::vector<std::string> thislist;
		char apptitlechar[256] = {0};
		//sprintf(apptitlechar,"NXMilk");
		sprintf(apptitlechar,"NXMilk v%d.%d.%d",VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO);
		if (ImGui::Begin(apptitlechar, nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar )) {
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 5});
			//ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.26f, 0.59f, 0.98f, 0.00f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, nxmpgfx::NavHover_color);
			ImGui::PushStyleColor(ImGuiCol_NavHighlight, nxmpgfx::Active_color);
			
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",fsbrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
		
		
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
								std::string openfilename = fsbrowser->getCurrentPath()+"/"+thislist[n].filename;
								printf("AAA: %s\n",openfilename.c_str());
								bool loaded = audioplayer->LoadFile(fsbrowser->getCurrentPath()+"/"+thislist[n].filename);
								if(loaded){
									audioplayer->Play();
								}
							}else{
								fsbrowser->DirList(removeLastSlash(fsbrowser->getCurrentPath()) +"/"+ thislist[n].filename,false,audioextensions);
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
						ImGui::Text("%s",formatTimeStamp(thislist[n].st.st_mtime).c_str());
						
						
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