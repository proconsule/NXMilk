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
		if(MyUSBMount != nullptr)MyUSBMount->usbMscTestDevices();
		if (ImGui::Begin("USB Devices", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			
			
			static int selected = -1;
			if(MyUSBMount != nullptr){	
				std::vector<usb_devices> thislist = MyUSBMount->mounted_devs;
				for (unsigned int n = 0; n < thislist.size(); n++){
					std::string itemid = "##" + std::to_string(n);
					if (ImGui::Selectable(itemid.c_str(), selected == n,0, ImVec2(0, 60))){
						std::string mountpath = thislist[n].mount_point + std::string("/");
						MyUSBMount->setBasePath(mountpath);
						menuitem.state = MENU_STATE_FILEBROWSER;
						fsbrowser->DirList(mountpath,false,audioextensions);
					}
					float currstartYpos = ImGui::GetCursorPosY();
						ImGui::SameLine();
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10.0f);
						ImGui::Image((void*)(intptr_t)imgloader->icons.UsbTexture.id, ImVec2(40,40));
						ImGui::SameLine();
						ImGui::SetCursorPosY(currstartYpos);
						float currstartXpos = ImGui::GetCursorPosX();
						std::string devname = Utility::trim(thislist[n].name)  + std::string(" -> ") + thislist[n].mount_point;
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()-60.0f);
						ImGui::Text("%s",devname.c_str());
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()-30.0f);
						ImGui::SetCursorPosX(currstartXpos);
						ImGui::Text("%s %s",thislist[n].fstype.c_str(),Utility::humanSize(thislist[n].capacity).c_str());
						ImGui::SetCursorPosY(currstartYpos);
					//ImGui::SameLine();
					
					//ImGui::Text("%s %s",MyUSBMount->mounted_devs[n].fstype.c_str(),humanSize(MyUSBMount->mounted_devs[n].capacity).c_str());
				}
			}
		
		
		
		Windows::ExitMainWindow();
		}
		
		
	}
	
	
	void HomeWindow() {
        Windows::SetupWindow();
		
		char apptitlechar[256] = {0};
		//sprintf(apptitlechar,"NXMilk");
		sprintf(apptitlechar,"NXMilk v%d.%d.%d",VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO);
		
		if (ImGui::Begin(apptitlechar, nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 5});
			
			if (ImGui::BeginTable("##table1", 2)){
				ImGui::TableSetupColumn("icon", ImGuiTableColumnFlags_WidthFixed, ((80.0f+ImGui::GetStyle().FramePadding.x) *multiplyRes -2 * ImGui::GetStyle().ItemSpacing.x)); 
				ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch); 
				
				static int selected = -1;	
				std::vector<std::string> menustrings = {"SD Card","USB","Network","Exit"};
				for (unsigned int n = 0; n <  menustrings.size(); n++){
					ImGui::TableNextRow();
					std::string itemid = "##" + std::to_string(n);
					ImGui::TableSetColumnIndex(0);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    				
					//if(n == MM_UPNP)ImGui::BeginDisabled();
					
					
					if(n == 0){
						ImGui::Image((void*)(intptr_t)imgloader->icons.SdCardTexture.id, ImVec2(50,50));
					}
					else if(n == 1){
						ImGui::Image((void*)(intptr_t)imgloader->icons.UsbTexture.id, ImVec2(50,50));
					}
					else if(n == 2){
						ImGui::Image((void*)(intptr_t)imgloader->icons.NetworkTexture.id, ImVec2(50,50));
					}
					else if(n == 3){
						ImGui::Image((void*)(intptr_t)imgloader->icons.ExitTexture.id, ImVec2(50,50));
					}
					 
					ImGui::SameLine();
					ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
							
					if (ImGui::Selectable(itemid.c_str(), selected == n,selectable_flags,ImVec2(1280*multiplyRes,50*multiplyRes))){
						if(n == 0){
							menuitem.state = MENU_STATE_FILEBROWSER;
							fsbrowser = new CFSBrowser(configini->getStartPath());
							fsbrowser->DirList(configini->getStartPath(),true,audioextensions);
							
						}
						if(n == 1){
							if(MyUSBMount==nullptr){
								MyUSBMount=new USBMounter();
							}
							menuitem.state = MENU_STATE_USB;
							//fsbrowser = new CFSBrowser(MyUSBMount->getBasePath());
							//fsbrowser->DirList(fsbrowser->getCurrentPath(),true,audioextensions);
							
						}
						if(n == 2){
							menuitem.state = MENU_STATE_NETWORKSEL;
													
						}
						if(n == 3){
							
							menuitem.state = MENU_STATE_EXIT;
							
							
							//
						}
						


					}
							
															
					ImGui::TableSetColumnIndex(1);
					
					ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + ((50*multiplyRes) - ImGui::GetFont()->FontSize) / 2});
					
					
					
					
					
					//ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + ((50*multiplyRes) - ImGui::GetFont()->FontSize) / 2});
					
					ImGui::Text("%s",menustrings[n].c_str());
					
					ImGui::PopStyleVar();
					
					//if(n == MM_UPNP)ImGui::EndDisabled();
					
				}
				
				ImGui::EndTable();
				ImGui::PopStyleVar();
			}	
		}
		
		Windows::ExitMainWindow();
		
	}
	

	void MainMenuWindow() {
		Windows::SetupMainWindow();
	
		//std::vector<std::string> thislist;
		//char apptitlechar[256] = {0};
		//sprintf(apptitlechar,"NXMilk");
		//sprintf(apptitlechar,"NXMilk v%d.%d.%d",VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO);
		if (ImGui::Begin(fsbrowser->getCurrentPath().c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar )) {
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
							if(Utility::endsWith(thislist[n].filename.c_str(),".wav",false)){
								ImGui::Image((void*)(intptr_t)imgloader->icons.Wav_Icon.id, ImVec2(30*multiplyRes,30*multiplyRes));
							
							}else if(Utility::endsWith(thislist[n].filename.c_str(),".ogg",false)){
								ImGui::Image((void*)(intptr_t)imgloader->icons.Ogg_Icon.id, ImVec2(30*multiplyRes,30*multiplyRes));
							
							}else if(Utility::endsWith(thislist[n].filename.c_str(),".mp3",false)){
								ImGui::Image((void*)(intptr_t)imgloader->icons.Mp3_Icon.id, ImVec2(30*multiplyRes,30*multiplyRes));
							
							}else if(Utility::endsWith(thislist[n].filename.c_str(),".flac",false)){
								ImGui::Image((void*)(intptr_t)imgloader->icons.Flac_Icon.id, ImVec2(30*multiplyRes,30*multiplyRes));
							
							}else{
								ImGui::Image((void*)(intptr_t)imgloader->icons.FileTexture.id, ImVec2(30*multiplyRes,30*multiplyRes));
							}
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