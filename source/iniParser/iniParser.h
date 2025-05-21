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

struct networkstruct_v2{
	std::string name = "";
	std::string server = "";
	std::string type = "";
	std::string username = "";
	std::string password = "";
	std::string path = "";
	std::string pubkeypath = "";
	std::string privkeypath = "";
	std::string passphrase = "";
	int port = 0;
};




class CIniParser{
public:
	CIniParser(std::string _filename);
	~CIniParser();
	void ReadConfig();
	
	std::string getStartPath();
	audioplayerconfig_struct getAudioPlayerConfig();
	std::vector<std::string> getConfigExtensions();
	
	std::vector<networkstruct_v2> getNetworks();
	
protected:
	std::string filename;
	std::string startpath = "/switch/NXMilk";
	audioplayerconfig_struct audioplayerconfig;
	
	bool opened = false;
	
	std::vector<std::string> config_enabled_extensions;
	std::vector<networkstruct_v2> networks_v2;
	
	mINI::INIFile *fileref;
	mINI::INIStructure inidata;
	
};


#endif