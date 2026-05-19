#ifndef ONLINEGAMEMODE_H
#define ONLINEGAMEMODE_H

#include "GameNetworkManager.h"
#include <memory>
#include <SFML/Graphics.hpp>

enum class OnlineGameType {
    REALTIME,    // Both play simultaneously
    TURN_BASED   // Chess-clock style: 1m, 3m, or 5m per player
};

enum class TimeMode {
    FAST = 60,      // 1 minute
    NORMAL = 180,   // 3 minutes
    LONG = 300      // 5 minutes
};

class OnlineGameMode {
public:
    OnlineGameMode(OnlineGameType type, TimeMode timeMode);
    ~OnlineGameMode();

    bool joinGame(const std::string& serverAddr, uint16_t port, const std::string& playerName);
    void disconnect();

    bool isGameActive() const;
    bool isMyTurn() const;

    void updateLocal(float deltaTime);
    void render(sf::RenderWindow& window);

    // Called from main game to sync state
    void notifyPlayerMove(int boardX, int boardY, int blockType);
    void notifyScoreUpdate(float score, int linesCleared);
    void notifyGameState(const int boardState[64], float score);

    // Get opponent's current state for rendering
    float getOpponentScore() const;
    const int* getOpponentBoardState() const;
    float getMyTimeRemaining() const;
    float getOpponentTimeRemaining() const;

private:
    OnlineGameType gameType;
    TimeMode timeMode;
    std::unique_ptr<GameNetworkManager> networkMgr;

    float myTimeRemaining;
    float opponentTimeRemaining;
    float opponentScore;
    int opponentBoardState[64];

    bool isActive;
    bool isCurrentTurn; // For turn-based mode

    void onOpponentMove(const OpponentMove& move);
    void onOpponentScore(float score);
    void onGameState(const GameStateSync& state);
    void onTimeUpdate(float timeP1, float timeP2);
    void onGameOver(bool isWinner, const std::string& reason);

    void updateTimer(float deltaTime);
    void renderOpponentBoard(sf::RenderWindow& window);
    void renderTimer(sf::RenderWindow& window);
};

#endif // ONLINEGAMEMODE_H
