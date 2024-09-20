#include "utils.h"

std::vector<std::string> Utilities::split(std::string str, char delim) {
    std::vector<std::string> params;
    if (str.find(delim) == std::string::npos) {
        return params;
    }
    std::stringstream ss(str);
    std::string word;
    while (std::getline(ss, word, delim)) {
        params.push_back(word);
    }
    return params;
}

std::string Utilities::to_utf8(const std::string& str) {
    // Get the required buffer size for the wide char string.
    int wideSize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    if (wideSize == 0) {
        return "";
    }

    // Convert to wide char string (UTF-16).
    std::vector<wchar_t> wideStr(wideSize);
    if (MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wideStr[0], wideSize) == 0) {
        return "";
    }

    // Get the required buffer size for the UTF-8 string.
    int utf8Size = WideCharToMultiByte(CP_UTF8, 0, &wideStr[0], -1, NULL, 0, NULL, NULL);
    if (utf8Size == 0) {
        return "";
    }

    // Convert to UTF-8 string.
    std::vector<char> utf8Str(utf8Size);
    if (WideCharToMultiByte(CP_UTF8, 0, &wideStr[0], -1, &utf8Str[0], utf8Size, NULL, NULL) == 0) {
        return "";
    }

    return std::string(&utf8Str[0]);
}

bool Utilities::copyfile(const char* SRC, const char* DST) {
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dst(DST, std::ios::binary);

    if (!src.is_open()) {
        printf("Error opening the source file\n");
        return false;
    }

    if (!dst.is_open()) {
        printf("Error opening the destination file\n");
        return false;
    }

    dst << src.rdbuf();

    if (!dst) {
        printf("Error copying the file\n");
        return false;
    }
    return true;
}
char* Utilities::getFolder(const char* name) {
    char* buffer = getenv(name);
    if (buffer != NULL) {
        return buffer;
    }
    else {
        return NULL;
    }
}

bool Utilities::isUserAdmin() {
    bool isElevated = false;
    HANDLE token;
    TOKEN_ELEVATION elev;
    DWORD size;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        if (GetTokenInformation(token, TokenElevation, &elev, sizeof(elev), &size)) {
            isElevated = elev.TokenIsElevated;
        }
        CloseHandle(token);
        token = NULL;
    }
    return isElevated;
}