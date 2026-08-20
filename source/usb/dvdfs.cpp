#include "dvdfs.h"

DVDMounter::DVDMounter(){
	
	testusbdvd = new CUSBDVD(true,true);
}


std::string DVDMounter::getBasePath(){
	return basepath;
}

void DVDMounter::setBasePath(std::string _basepath){
	basepath = _basepath;
}

DVDMounter::~DVDMounter(){
	if(testusbdvd){ 
		delete testusbdvd;
		testusbdvd = nullptr;
	}
	
}
