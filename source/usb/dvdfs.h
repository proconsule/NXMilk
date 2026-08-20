#ifndef NXMP_DVDFS_H
#define NXMP_DVDFS_H

#include <cstdlib>
#include <string>
#include <vector>
#include <threads.h>

#include <usbdvd.h>


class DVDMounter{
public:
	DVDMounter();
	~DVDMounter();
	
	void setBasePath(std::string _basepath);
	std::string getBasePath();

	
	CUSBDVD *testusbdvd = nullptr;

	std::string basepath = "";
	
};

#endif
