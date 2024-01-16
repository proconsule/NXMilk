#include "iniParser.h"

bool string_to_bool(std::string boolstring){
	if(boolstring == "true"){
		return true;
	}
	if(boolstring == "false"){
		return false;
	}
	return false;
}

int string_to_int(std::string intstring){
	return stoi(intstring);
}

std::string bool_to_string(bool value){
	return value ? std::string("true"):std::string("false");
}

std::string int_to_string(int value){
	return std::to_string(value);
}

std::string float_to_string(float value){
	std::stringstream ss1;
	ss1 << value;
	std::string str1 = ss1.str();
	return str1;
}

float string_to_float(std::string floatstring){
	return std::stof(floatstring);
}


CIniParser::CIniParser(std::string _filename){
	filename = _filename;
	fileref = new mINI::INIFile(filename);
	ReadConfig();
}

CIniParser::~CIniParser(){
	
	
}

void CIniParser::ReadConfig(){
	inidata.clear();
	opened =  fileref->read(inidata);
	
	if(opened){
		if(inidata["AudioPlayer"].has("startpath")){
			startpath = inidata.get("AudioPlayer").get("startpath");
		}
		if(inidata["AudioPlayer"].has("milkpresetspath")){
			audioplayerconfig.milkpresetspath = inidata.get("AudioPlayer").get("milkpresetspath");
		}
		if(inidata["AudioPlayer"].has("milktexturespath")){
			audioplayerconfig.milktexturespath = inidata.get("AudioPlayer").get("milktexturespath");
		}
		if(inidata["AudioPlayer"].has("interfacehidesec")){
			audioplayerconfig.interfacehidesec = string_to_int(inidata.get("AudioPlayer").get("interfacehidesec"));
		}
		
		if(inidata["AudioPlayer"].has("usebuitinpresets")){
			audioplayerconfig.usebuitinpresets = string_to_bool(inidata.get("AudioPlayer").get("usebuitinpresets"));
		}
	
	}
}

std::string CIniParser::getStartPath(){
	return startpath;
}

audioplayerconfig_struct CIniParser::getAudioPlayerConfig(){
	return audioplayerconfig;
}