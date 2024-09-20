#pragma once

#ifndef UTILS_H
#define UTILS_H
#include "common.h"

namespace Utilities {
	std::vector<std::string> split(std::string str, char delim);
	std::string to_utf8(const std::string& str);
	bool copyfile(const char* SRC, const char* DST);
	char* getFolder(const char* name);
	bool isUserAdmin();
}

#endif