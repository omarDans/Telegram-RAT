#pragma once

#ifndef SHELL_H
#define SHELL_H
#include "common.h"

namespace shell {
	std::string CreateShellProcess(PROCESS_INFORMATION& pi, HANDLE& hInputWrite, HANDLE& hOutputRead);
	std::string ExecuteCommand(HANDLE& hInputWrite, HANDLE& hOutputRead, const std::string& command);
	void clearstdout(HANDLE& hOutputRead);
}

#endif