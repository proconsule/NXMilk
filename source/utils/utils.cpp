#include "utils.h"

extern "C"{
#include <switch/services/lbl.h>
};
#include <switch.h>

namespace Utility{
	
	const std::string WHITESPACE = " \n\r\t\f\v";

	bool startWith(const std::string &str, const std::string &value, bool sensitive) {
		if (sensitive) {
			return str.compare(0, value.length(), value) == 0;
		} else {
			std::string str_low = toLower(str);
			std::string val_low = toLower(value);
			return str_low.compare(0, val_low.length(), val_low) == 0;
		}
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
	
	std::string humanSize(size_t bytes)
	{
		std::vector<std::string>suffix = {"B", "KB", "MB", "GB", "TB"};
	
		int i = 0;
		 double dblBytes = bytes;

		if (bytes > 1024) {
			for (i = 0; (bytes / 1024) > 0 && i<(int)suffix.size()-1; i++, bytes /= 1024)
				dblBytes = bytes / 1024.0;
			}

		char output[200];
		sprintf(output, "%.02lf %s", dblBytes, suffix[i].c_str());
		std::string ret = output;
		return ret;
	}
	
	std::string ltrim(const std::string &s)
	{
		size_t start = s.find_first_not_of(WHITESPACE);
		return (start == std::string::npos) ? "" : s.substr(start);
	}
 
	std::string rtrim(const std::string &s)
	{
		size_t end = s.find_last_not_of(WHITESPACE);
		return (end == std::string::npos) ? "" : s.substr(0, end + 1);
	}
 
	std::string trim(const std::string &s) {
		return rtrim(ltrim(s));
	}


	std::string removeLastSlash(const std::string &string) {

		std::string str = string;
		size_t pos = str.find_last_of('/');
		if (pos == str.length() - 1) {
			str.erase(str.length() - 1);
		}
		return str;
	}
	
	bool isArchiveExtension(std::string path){
		if(Utility::endsWith(path,".zip",false) || Utility::endsWith(path,".rar",false) || Utility::endsWith(path,".tar",false) || Utility::endsWith(path,".gz",false)){
			return true;
		}
											
		return false;
	}
	
	
	bool BackLightOff(){
		int rc = lblInitialize();
		if (R_FAILED(rc))
                return false;
		lblSwitchBacklightOff(1ul);
		lblExit();
		return true;
		
		
	}
	
	bool BackLightOn(){
		int rc = lblInitialize();
		if (R_FAILED(rc))
                return false;
		lblSwitchBacklightOn(1ul);
		lblExit();
		return true;
		
		
	}
	
	
	 
	
	
}