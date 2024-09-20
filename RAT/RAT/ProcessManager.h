#pragma once
#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H
#include "common.h"

namespace ProcessManager {
	std::string ProcessList();
	bool InjectShell(DWORD pid, std::string shell);
	DWORD PidByName(std::string name);
}

#endif