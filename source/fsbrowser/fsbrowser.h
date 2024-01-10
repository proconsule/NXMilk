#ifndef NXMILK_FSBROWSER_H
#define NXMILK_FSBROWSER_H

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/dirent.h>
#include <sys/iosupport.h>
#include <vector>
#include <algorithm>

#include <switch.h>


typedef struct{
	std::string filename;
	struct stat st{0};
	time_t mod;
}fsentry_struct;


class CFSBrowser{
public:
	CFSBrowser(std::string initialpath);
	void DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<fsentry_struct> getCurrList();
	std::string getCurrentPath();
	std::string backDir();
	
private:
	std::vector<fsentry_struct> filelist;
	std::string currentpath;
	
};


#endif