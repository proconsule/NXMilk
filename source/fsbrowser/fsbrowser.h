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
#include "iniParser.h"

#include "sshfs.h"
#include "smb2fs.h"
#include "nfsfs.h"
#include "libarchivefs.h"
#include "cuebinfs.h"
#include "iso9660fs.h"

typedef struct{
	std::string filename;
	struct stat st;
}fsentry_struct;


class CFSBrowser{
public:
	CFSBrowser(std::string initialpath,std::string _title);
	CFSBrowser(networkstruct_v2 netconfdata);
	~CFSBrowser();
	void DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<fsentry_struct> getCurrList();
	std::string getCurrentPath();
	std::string backDir();
	
	int FindFileIDX(std::string _filename);
	std::string getNextFile(std::string _filename);
	std::string getPrevFile(std::string _filename);
	int currfileidx = 0;
	
	void OpeCueFile(std::string _path);
	void OpeISO9660File(std::string _path);
	void OpenArchive(std::string _path);
	void CloseFilesMount();
	std::string title = "";
	bool filemount = false;
	
private:
	std::vector<fsentry_struct> filelist;
	std::vector<fsentry_struct> audiofilelist;
	std::string currentpath;
	std::string basepath;
	
	CSSHFS *SSHFS = nullptr;
	CSMB2FS *SMB2FS = nullptr;
	CNFSFS *NFSFS = nullptr;
	CCUEBINFS *CUEBINFS = nullptr;
	CARCHFS *ARCHFS = nullptr;
	CISO9660FS *ISO9660FS = nullptr;
	
	
	
	
	bool connected = false;
	
	
	std::string oldtitle = "";
	std::string oldmount = "";
	
	
};


#endif