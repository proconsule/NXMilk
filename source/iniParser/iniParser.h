#ifndef NXMILK_INIPARSER_H
#define NXMILK_INIPARSER_H

#include <string>
#include <vector>

#define MINI_CASE_SENSITIVE
#include "../mINI/src/mini/ini.h"


typedef struct{
	std::string milkpresetspath = "/switch/NXMilk/presets/milk/";
	std::string milktexturespath = "/switch/NXMilk/presets/Textures/";
	int interfacehidesec = 10;
	bool usebuiltinpreset = true;
	
	
}audioplayerconfig_struct;


class CIniParser{
public:
	CIniParser(std::string _filename);
	~CIniParser();
	void ReadConfig();
	
	std::string getStartPath();
	audioplayerconfig_struct getAudioPlayerConfig();
	
	
protected:
	std::string filename;
	std::string startpath = "/switch/NXMilk";
	audioplayerconfig_struct audioplayerconfig;
	
	bool opened = false;
	
	mINI::INIFile *fileref;
	mINI::INIStructure inidata;
	
};


#endif