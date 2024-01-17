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
	struct stat st;
}fsentry_struct;


class CFSBrowser{
public:
	CFSBrowser(std::string initialpath);
	void DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<fsentry_struct> getCurrList();
	std::string getCurrentPath();
	std::string backDir();
	
	int FindFileIDX(std::string _filename);
	std::string getNextFile(std::string _filename);
	std::string getPrevFile(std::string _filename);
	int currfileidx = 0;
	
private:
	std::vector<fsentry_struct> filelist;
	std::vector<fsentry_struct> audiofilelist;
	std::string currentpath;
	std::string basepath;
	
	
};


#endif