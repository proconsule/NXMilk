#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"


#include "smb2fs.h"
#include "sshfs.h"
#include "nfsfs.h"


namespace Windows {
	
	int netwinselected = -1;
	
	
	bool urlcheck_isconnected = false;
	
	std::string newnetaddress = "";
	std::string newentryname = "New Share";
	
	
    void NetworkWindow() {
        Windows::SetupWindow();
		
        if (ImGui::Begin("Network Source Selection", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
			//GUI::cloktimeText(ImVec2((1180.0f*multiplyRes)-ImGui::CalcTextSize(nxmpstats->currentTime).x-(10.0*multiplyRes),2.0f*multiplyRes),true,nxmpstats->currentTime);
			//GUI::newbatteryIcon(ImVec2(1180.0f*multiplyRes,2.0f*multiplyRes),true,batteryPercent,40,20,true);
			
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 5});
			
			
		
			if(true){
				ImGui::BeginChild("##tablecontainer",ImVec2(total_w,total_h-45*multiplyRes));
				ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
				if (ImGui::BeginTable("Network Source Menu",2)){
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    				
					ImGui::TableSetupColumn("icon", ImGuiTableColumnFlags_WidthFixed, ((80.0f+ImGui::GetStyle().FramePadding.x) *multiplyRes -2 * ImGui::GetStyle().ItemSpacing.x)); 
					ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch); 
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					
					
					
					unsigned int n = 0;
					int selected = 0;
					
					for(unsigned int z=0;z<configini->getNetworks().size();z++){
						
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if(configini->getNetworks()[z].type == "ftp"){
							ImGui::Image((void*)(intptr_t)imgloader->icons.FTPTexture.id, ImVec2(50,50));
						}
						if(configini->getNetworks()[z].type == "http"){
							ImGui::Image((void*)(intptr_t)imgloader->icons.HTTPTexture.id, ImVec2(50,50));
						}
						if(configini->getNetworks()[z].type == "sftp"){
							ImGui::Image((void*)(intptr_t)imgloader->icons.SFTPTexture.id, ImVec2(50,50));
						}
						if(configini->getNetworks()[z].type == "smb"){
							ImGui::Image((void*)(intptr_t)imgloader->icons.SMBTexture.id, ImVec2(50,50));
						}
						if(configini->getNetworks()[z].type == "nfs"){
							ImGui::Image((void*)(intptr_t)imgloader->icons.NFSTexture.id, ImVec2(50,50));
						}
						ImGui::SameLine();
					
						std::string itemid = "##itemid" + std::to_string(z+n);
						if (ImGui::Selectable(itemid.c_str(), selected == z+n+1,selectable_flags,ImVec2(1280*multiplyRes,50*multiplyRes))){
							fsbrowser = new CFSBrowser(configini->getNetworks()[z]);
							fsbrowser->DirList(fsbrowser->getCurrentPath(),false,audioextensions);	
							menuitem.state = MENU_STATE_FILEBROWSER;
						
						}
						
						if(ImGui::IsItemHovered()){
							netwinselected = z+n;
						}
						ImGui::TableSetColumnIndex(1);
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + ((50*multiplyRes) - ImGui::GetFont()->FontSize) / 2});
						ImGui::Text(std::string(configini->getNetworks()[z].name + " @ " +configini->getNetworks()[z].server).c_str());
						if(ImGui::IsItemHovered()){
								netwinselected = z+n;
						}
						
					}
					
					
					ImGui::PopStyleVar();			
					
					ImGui::EndTable();
					ImGui::SetWindowFocus();
					
				}
				ImGui::EndChild();
				
				ImGui::BeginChild("##helpchild",ImVec2(total_w,ImGui::GetContentRegionAvail().y));
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				
				ImGui::Dummy(ImVec2(0,5));
				ImVec2 startpos =  ImGui::GetCursorScreenPos();
				ImGui::Dummy(ImVec2(0,5));
				ImGui::Text(FONT_DPADUP_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(FONT_DPADDOWN_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text("Navigation");
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				ImGui::Text(FONT_A_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text("Select");
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				ImGui::Text(FONT_Y_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text("Home");
				
				window->DrawList->AddLine(startpos,ImVec2(startpos.x+1280*multiplyRes,startpos.y) , ImGui::GetColorU32(ImVec4(1.0f,1.0f,1.0f,1.0f)), 1.0f);
				
				ImGui::EndChild();
			}
			
			ImGui::PopStyleVar();
		}
		Windows::ExitWindow();
	}
}	


