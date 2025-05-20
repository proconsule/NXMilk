
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
	
	
}


