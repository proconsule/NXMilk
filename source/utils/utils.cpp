#include "utils.h"

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

	
}