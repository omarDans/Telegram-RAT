#include "shell.h"


std::string shell::CreateShellProcess(PROCESS_INFORMATION& pi, HANDLE& hInputWrite, HANDLE& hOutputRead) {
    printf("Ejecutando CreateShellProcess\n");
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    DWORD read;
    std::string output;
    char buffer[1024];

    printf("variables raras\n");

    // Crear las tuberías para la entrada y salida
    HANDLE hInputRead, hOutputWrite;
    printf("CreatePipe\n");
    if (!CreatePipe(&hInputRead, &hInputWrite, &sa, 0) || !CreatePipe(&hOutputRead, &hOutputWrite, &sa, 0)) {
        return "";
    }

    // Configurar las tuberías para que no hereden los manejadores
    SetHandleInformation(hInputWrite, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hOutputRead, HANDLE_FLAG_INHERIT, 0);

    // Configuración del proceso
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hInputRead;
    si.hStdOutput = hOutputWrite;
    si.hStdError = hOutputWrite;

    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    // Iniciar el proceso de shell
    printf("CreateProcess\n");
    if (!CreateProcessA(NULL, (LPSTR)"cmd.exe", NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        return "";
    }
    Sleep(2000);
    printf("A topeee\n");

    // Cerrar los manejadores que no se usan
    while (true) {
        if (!ReadFile(hOutputRead, buffer, sizeof(buffer) - 1, &read, NULL) || read == 0) {
            break;
        }
        buffer[read] = '\0';
        output += buffer;
        if (read < sizeof(buffer) - 1) {
            break;
        }
    }
    CloseHandle(hInputRead);
    CloseHandle(hOutputWrite);

    printf("Saliendo de while\n");
    return output;
}

std::string shell::ExecuteCommand(HANDLE& hInputWrite, HANDLE& hOutputRead, const std::string& command) {
    DWORD written, read, available;
    std::string output;
    char buffer[1024];

    std::string cmd = command + "\n";
    WriteFile(hInputWrite, cmd.c_str(), cmd.length(), &written, NULL);

    while (true) {
        if (!PeekNamedPipe(hOutputRead, NULL, 0, NULL, &available, NULL)) {
            break;
        }

        if (available > 0) {
            if (!ReadFile(hOutputRead, buffer, sizeof(buffer) - 1, &read, NULL)) {
                break;
            }
            buffer[read] = '\0';
            output += buffer;
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        if (output.length() > cmd.length()) {
            break;
        }
    }
    if (command.compare("exit") == 0) {
        hInputWrite = NULL;
        hOutputRead = NULL;
    }

    return output;
}

void shell::clearstdout(HANDLE& hOutputRead) {
    DWORD available, read;
    char buffer[1024];
    while (true) {
        if (!PeekNamedPipe(hOutputRead, NULL, 0, NULL, &available, NULL)) {
            break;
        }
        if (available > 0) {
            if (!ReadFile(hOutputRead, buffer, sizeof(buffer), &read, NULL)) {
                break;
            }
        }
        else {
            break;
        }
    }
    return;
}