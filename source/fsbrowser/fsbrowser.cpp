#include "fsbrowser.h"

std::string toLower(const std::string &str) {
		std::string ret = str;
		std::transform(ret.begin(), ret.end(), ret.begin(),[](unsigned char c) { return std::tolower(c); });
		return ret;
	}

std::string toUpper(const std::string &str) {
		std::string ret = str;
		std::transform(ret.begin(), ret.end(), ret.begin(),[](unsigned char c) { return std::toupper(c); });
		return ret;
	}
	
std::string removeLastSlash(const std::string &string) {

    std::string str = string;
    size_t pos = str.find_last_of('/');
    if (pos == str.length() - 1) {
        str.erase(str.length() - 1);
    }
	return str;
}

bool SortNameAsc(const fsentry_struct &entryA, const fsentry_struct &entryB){
	if ((S_ISDIR(entryA.st.st_mode)) && !(S_ISDIR(entryB.st.st_mode)))
			return true;
		else if (!(S_ISDIR(entryA.st.st_mode)) && (S_ISDIR(entryB.st.st_mode)))
			return false;
		else {
			if (strcasecmp(entryA.filename.c_str(), entryB.filename.c_str()) < 0)
				return true;
					
		}
		
		return false;
}

bool endsWith(const std::string &value, const std::string &ending, bool sensitive) {
		if (ending.size() > value.size()) return false;
		if (sensitive) {
			return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
		} else {
			std::string val_low = toLower(value);
			std::string end_low = toLower(ending);
			return std::equal(end_low.rbegin(), end_low.rend(), val_low.rbegin());
		}
	}




CFSBrowser::CFSBrowser(std::string initialpath){
	currentpath = initialpath;
}

void CFSBrowser::DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions){
		
			filelist.clear();
			currentpath = path;
			
			struct dirent *ent;
			DIR *dir;

			if (!path.empty()) {
				if ((dir = opendir(path.c_str())) != nullptr) {
				
					auto *reent    = __syscall_getreent();
					auto *devoptab = devoptab_list[dir->dirData->device];	
					
					
					
					while (/*(ent = readdir(dir)) != nullptr*/ true) {
							reent->deviceData = devoptab->deviceData;
							struct stat st{0};
							if (devoptab->dirnext_r(reent, dir->dirData, dir->fileData.d_name, &st))
							break;
							
							if ((/*path == "/" ||*/ strlen(dir->fileData.d_name) == 1) && dir->fileData.d_name[0] == '.') {
								continue;
							}
							if ((/*path == "/" ||*/ strlen(dir->fileData.d_name) == 2) && dir->fileData.d_name[0] == '.' && dir->fileData.d_name[1] == '.') {
								continue;
							}
							if (!showHidden && dir->fileData.d_name[0] == '.') {
								if (strlen(dir->fileData.d_name) != 2 && dir->fileData.d_name[1] != '.') {
									continue;
								}
							}
							
							
							
							fsentry_struct fsentry;
							fsentry.filename = dir->fileData.d_name;
							memcpy(&fsentry.st,&st,sizeof(struct stat));
							
							if(path.find_first_of("/") == 0){
								FsFileSystem sdmc;
								fsOpenSdCardFileSystem(&sdmc);
								
								FsTimeStampRaw timestamp = {0};
								char safe_buf[FS_MAX_PATH];
								std::string fullpath = removeLastSlash(path) + "/" + fsentry.filename;
								strcpy(safe_buf, fullpath.c_str());
								fsFsGetFileTimeStampRaw(&sdmc, safe_buf, &timestamp);
								fsentry.st.st_atime = timestamp.accessed;
								fsentry.st.st_ctime = timestamp.created;
								fsentry.st.st_mtime = timestamp.modified;
								fsFsClose(&sdmc);
							}
							
							
							filelist.push_back(fsentry);
							
						}
						
					
						closedir(dir);
						std::sort(filelist.begin(), filelist.end(), SortNameAsc);
						filelist.erase(
							std::remove_if(filelist.begin(), filelist.end(), [extensions](const fsentry_struct &file) {
								for (auto &ext : extensions) {
									if (endsWith(file.filename, ext, false)) {
										return false;
									}
								}
								return !S_ISDIR(file.st.st_mode);
						}), filelist.end());
					}
				
			}
		
	}
	
	
std::vector<fsentry_struct> CFSBrowser::getCurrList(){
	return filelist;
}
	
	
std::string CFSBrowser::getCurrentPath(){
	return currentpath;		
}

std::string CFSBrowser::backDir(){
	std::string relpath = currentpath.substr(currentpath.find_last_of("\\/")+1);
	currentpath = currentpath.substr(0, currentpath.find_last_of("\\/"));
	if(currentpath == "")currentpath = "/";
	return relpath;
}