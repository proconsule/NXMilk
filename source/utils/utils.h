
#include <string>
#include <iostream>    
#include <sstream>    
#include <iomanip>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

namespace Utility{
	
	bool endsWith(const std::string &value, const std::string &ending, bool sensitive);
	bool startWith(const std::string &str, const std::string &value, bool sensitive);
	std::string toLower(const std::string &str);
	std::string toUpper(const std::string &str);
	std::string humanSize(size_t bytes);
	std::string ltrim(const std::string &s);
	std::string rtrim(const std::string &s);
	std::string trim(const std::string &s);
	std::string removeLastSlash(const std::string &string);
	bool isArchiveExtension(std::string path);
	
	bool BackLightOff();
	bool BackLightOn();
}


