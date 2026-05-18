#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

class GameLauncher {
public:
    GameLauncher(const std::string& gameExePath);
    ~GameLauncher() = default;

    bool launchGame();
    bool isGameRunning() const;
    void update();

private:
    std::string gameExePath;
    bool gameRunning = false;
    int lastProcessId = 0;

    bool fileExists(const std::string& filename) const;
};
