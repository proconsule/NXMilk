#include "utils.h"

namespace Utility{

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

	
}