#include "Information.h"
#include "Requests.h"

bool IsThisVersion(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor) {
	OSVERSIONINFOEXW vi = {};
	vi.dwOSVersionInfoSize = sizeof(vi);
	vi.dwMajorVersion = wMajorVersion;
	vi.dwMinorVersion = wMinorVersion;
	vi.wServicePackMajor = wServicePackMajor;
	DWORDLONG const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
			VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL),
			VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	return VerifyVersionInfoW(&vi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

std::string Information::GetOS() {
	if (IsThisVersion(10, 0, 0)) {
		return "Windows 10 or later";
	}
	else if ((6, 3, 0)) {
		return "Windows 8.1";
	}
	else if (IsThisVersion(6, 2, 0)) {
		return "Windows 8/10";
	}
	else if (IsThisVersion(6, 1, 0)) {
		return "Windows 7";
	}
	else if (IsThisVersion(6, 0, 0)) {
		return "Windows Vista";
	}
	else if (IsThisVersion(5, 1, 0)) {
		return "Windows XP";
	}
	else if (IsThisVersion(5, 0, 0)) {
		return "Windows 2000";
	}
	else {
		return "Unknown OS";
	}
}

std::string Information::GetIP() {
	return Requests::GetRequest("api.ipify.org", "4B4DB4B3");
}

std::string Information::GetPCName() {
	char buff[512] = { 0 };
	DWORD len = sizeof(buff) - 1;
	GetUserNameA(buff, &len);
	return buff;
}

std::string Information::GetProcessorBrand() {
	int CPUInfo[4] = { -1 };

	__cpuidex(CPUInfo, 0x80000000, 0);
	unsigned int index = CPUInfo[0];

	char ProcessorBrand[0x40] = {0};

	for (unsigned int i = 0x80000000; i <= index; ++i)
	{
		__cpuidex(CPUInfo, i, 0);

		switch (i) {
		case 0x80000002:
			memcpy(ProcessorBrand, CPUInfo, sizeof(CPUInfo));
			break;

		case 0x80000003:
			memcpy(ProcessorBrand + 16, CPUInfo, sizeof(CPUInfo));
			break;

		case 0x80000004:
			memcpy(ProcessorBrand + 32, CPUInfo, sizeof(CPUInfo));
			break;

		default:
			break;
		}
	}

	return ProcessorBrand;
}