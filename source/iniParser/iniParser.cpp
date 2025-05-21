#include "iniParser.h"

std::vector<std::string> split_extension (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

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
		
		if(inidata["AudioPlayer"].has("usebuiltinpreset")){
			audioplayerconfig.usebuiltinpreset = string_to_bool(inidata.get("AudioPlayer").get("usebuiltinpreset"));
		}
		if(inidata["AudioPlayer"].has("enabled_extensions")){
			std::string enabled_extensions_string = inidata.get("AudioPlayer").get("enabled_extensions");
			config_enabled_extensions = split_extension(enabled_extensions_string,',');
		}
		
		
		for (auto const& it2 : inidata){
			
			auto const& key = it2.first;
			if(key.compare("AudioPlayer")){
				//printf("Network Share %s\r\n",key.c_str());
				
				if(inidata[key].has("server")){
					networkstruct_v2 tmpnet;
					tmpnet.name = key.c_str();
					tmpnet.server = inidata.get(key).get("server");
					tmpnet.type = inidata.get(key).get("type");
					if(inidata[key].has("username")){
						tmpnet.username = inidata.get(key).get("username");
						if(inidata[key].has("password")){
							tmpnet.password = inidata.get(key).get("password");
						}
					}
					
					if(inidata[key].has("pubkeypath")){
						tmpnet.pubkeypath = inidata.get(key).get("pubkeypath");
					}
					if(inidata[key].has("privkeypath")){
						tmpnet.privkeypath = inidata.get(key).get("privkeypath");
					}
					if(inidata[key].has("passphrase")){
						tmpnet.passphrase = inidata.get(key).get("passphrase");
					}
					if(inidata[key].has("port")){
						tmpnet.port = string_to_int(inidata.get(key).get("port"));
					}
					
					if(inidata[key].has("path")){
						tmpnet.path = inidata.get(key).get("path");
					}
					
					networks_v2.push_back(tmpnet);
				}
			
			}
		}
		
	
	}
}

std::string CIniParser::getStartPath(){
	return startpath;
}

audioplayerconfig_struct CIniParser::getAudioPlayerConfig(){
	return audioplayerconfig;
}

std::vector<std::string> CIniParser::getConfigExtensions(){
	
	return config_enabled_extensions;
}

std::vector<networkstruct_v2> CIniParser::getNetworks(){
	return networks_v2;
}