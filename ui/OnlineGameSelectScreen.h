#ifndef ONLINEGAMESELECTSCREEN_H
#define ONLINEGAMESELECTSCREEN_H

#include <SFML/Graphics.hpp>
#include "ui/OnlineGameMode.h"

class OnlineGameSelectScreen {
public:
    enum class SelectState {
        GAME_TYPE,      // Choose Realtime or Turn-based
        TIME_MODE,      // Choose 1m, 3m, 5m
        ENTERING_IP,    // Input server IP
        CONNECTING,     // Connecting to server
        WAITING,        // Waiting for opponent
        PLAYING
    };

    OnlineGameSelectScreen();
    ~OnlineGameSelectScreen();

    void handleEvent(const sf::Event& event);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);

    OnlineGameType getGameType() const { return selectedGameType; }
    TimeMode getTimeMode() const { return selectedTimeMode; }
    std::string getServerIP() const { return serverIP; }
    bool isConfirmed() const { return confirmed; }
    void resetSelection();

    SelectState getState() const { return currentState; }
    void setState(SelectState state) { currentState = state; }

private:
    SelectState currentState;
    OnlineGameType selectedGameType;
    TimeMode selectedTimeMode;
    std::string serverIP;
    std::string playerName;
    bool confirmed;

    int gameTypeSelection;  // 0: Realtime, 1: Turn-based
    int timeModeSelection;  // 0: 1m, 1: 3m, 2: 5m
    int cursorPos;

    sf::Font font;
    float elapsedTime;

    void loadFont();
    void drawGameTypeMenu(sf::RenderWindow& window);
    void drawTimeModeMenu(sf::RenderWindow& window);
    void drawIPInputMenu(sf::RenderWindow& window);
    void drawWaitingScreen(sf::RenderWindow& window);
};

#endif // ONLINEGAMESELECTSCREEN_H
