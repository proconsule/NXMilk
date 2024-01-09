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
					FsFileSystem sdmc;
					fsOpenSdCardFileSystem(&sdmc);
					while ((ent = readdir(dir)) != nullptr) {
						if ((path == "/" || strlen(ent->d_name) == 1) && ent->d_name[0] == '.') {
							continue;
						}
						if ((path == "/" || strlen(ent->d_name) == 2) && ent->d_name[0] == '.' && ent->d_name[1] == '.') {
							continue;
						}
						if (!showHidden && ent->d_name[0] == '.') {
							if (strlen(ent->d_name) != 2 && ent->d_name[1] != '.') {
								continue;
							}
						}


						fsentry_struct fsentry;
						
						fsentry.filename = ent->d_name;
						std::string fpath = removeLastSlash(path) + "/" + fsentry.filename;
						
						FsTimeStampRaw timestamp = {0};
						char safe_buf[FS_MAX_PATH];
						strcpy(safe_buf, fpath.c_str());
						fsFsGetFileTimeStampRaw(&sdmc, safe_buf, &timestamp);
						
						
						
						struct stat st{0};
						if (stat(fpath.c_str(), &st) == 0) {
							//memcpy(&fsentry.st,&st,sizeof(struct stat));
							fsentry.st.st_size =  st.st_size;
							fsentry.st.st_mode = st.st_mode;
							fsentry.st.st_ctime = timestamp.created;
							fsentry.st.st_mtime = timestamp.modified;
							fsentry.st.st_atime = timestamp.accessed;
						}
						
						//filelist.push_back(fsentry);
						
						
						
						if(!S_ISDIR(st.st_mode)){
							bool isMediafile = false;
							for (auto &ext : extensions) {
								if (endsWith(fsentry.filename, ext, false)) {
									isMediafile = true;
								}
							}
							if(isMediafile){
								
								filelist.push_back(fsentry);
							}
						}else if(S_ISDIR(st.st_mode)){
							filelist.push_back(fsentry);
						}
						

						/*

						FS::FileEntry file;
						file.name = ent->d_name;
						file.dbread = -1;
						file.path = FS::removeLastSlash(path) + "/" + file.name;
						
						FsTimeStampRaw timestamp = {0};
						char safe_buf[FS_MAX_PATH];
						strcpy(safe_buf, file.path.c_str());
						fsFsGetFileTimeStampRaw(&sdmc, safe_buf, &timestamp);
						
						
						
						struct stat st{0};
						if (stat(file.path.c_str(), &st) == 0) {
							file.size = (size_t) st.st_size;
							file.type = S_ISDIR(st.st_mode) ? FS::FileEntryType::Directory : FS::FileEntryType::File;
							file.is_valid = 1;
							file.created = timestamp.created;
							file.modified = timestamp.modified;
							file.accessed = timestamp.accessed;
						}
						
						
						if(file.type == FS::FileEntryType::File){
							bool isMediafile = false;
							for (auto &ext : extensions) {
								if (Utility::endsWith(file.name, ext, false)) {
									isMediafile = true;
								}
							}
							if(isMediafile){
								if(Utility::isImageExtension(file.name)){
									file.mediatype = FS::FileMediaType::Image;
									currentimagelist.push_back(file);
								}
								if(Utility::isArchiveExtension(file.name)){
									file.mediatype = FS::FileMediaType::Archive;
								}
								currentlist.push_back(file);
							}
						}else if(file.type == FS::FileEntryType::Directory){
							currentlist.push_back(file);
						}
						*/
					}
					fsFsClose(&sdmc);
					closedir(dir);
					std::sort(filelist.begin(), filelist.end(), SortNameAsc);
					
					/*
					
					if(sortOrder == FS::FS_NAME_ASCENDINGORDER){
						std::sort(currentlist.begin(), currentlist.end(), FS::SortNameAsc);
					}
					if(sortOrder == FS::FS_NAME_DESCENDINGORDER){
						std::sort(currentlist.begin(), currentlist.end(), FS::SortNameDesc);
					}
					
					if(sortOrder == FS::FS_DATE_ASCENDINGORDER){
						std::sort(currentlist.begin(), currentlist.end(), FS::SortDateAsc);
					}
					if(sortOrder == FS::FS_DATE_DESCENDINGORDER){
						std::sort(currentlist.begin(), currentlist.end(), FS::SortDateDesc);
					}
					
					if(sortOrder == FS::FS_SIZE_ASCENDINGORDER){
						std::sort(currentlist.begin(), currentlist.end(), FS::SortSizeAsc);
					}
					if(sortOrder == FS::FS_SIZE_DESCENDINGORDER){
						std::sort(currentlist.begin(), currentlist.end(), FS::SortSizeDesc);
					}
					*/
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