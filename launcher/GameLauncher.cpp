#include "GameLauncher.h"
#include <filesystem>
#include <windows.h>
#include <cstdlib>
#include <iostream>

GameLauncher::GameLauncher(const std::string& exePath)
    : gameExePath(exePath) {
}

bool GameLauncher::fileExists(const std::string& filename) const {
    return std::filesystem::exists(filename);
}

bool GameLauncher::launchGame() {
    if (!fileExists(gameExePath)) {
        std::cerr << "Error: Game executable not found at: " << gameExePath << std::endl;
        return false;
    }

    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};
    si.cb = sizeof(si);

    char commandLine[MAX_PATH];
    strcpy_s(commandLine, MAX_PATH, gameExePath.c_str());

    if (!CreateProcessA(
        nullptr,
        commandLine,
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi)) {

        std::cerr << "Error: Failed to launch game. Error code: " << GetLastError() << std::endl;
        return false;
    }

    lastProcessId = pi.dwProcessId;
    gameRunning = true;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

bool GameLauncher::isGameRunning() const {
    return gameRunning;
}

void GameLauncher::update() {
    if (gameRunning && lastProcessId != 0) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, lastProcessId);

        if (hProcess == nullptr) {
            gameRunning = false;
            return;
        }

        DWORD exitCode = 0;
        if (GetExitCodeProcess(hProcess, &exitCode)) {
            if (exitCode != STILL_ACTIVE) {
                gameRunning = false;
            }
        }

        CloseHandle(hProcess);
    }
}
