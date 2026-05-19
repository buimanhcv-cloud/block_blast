#ifndef GAMENETWORKMANAGER_H
#define GAMENETWORKMANAGER_H

#include "NetworkClient.h"
#include <memory>
#include <functional>
#include <chrono>

struct GameStateSync {
    float opponentScore;
    int opponentBoardState[64]; // 8x8 board
    float timeRemainingP1;
    float timeRemainingP2;
};

struct OpponentMove {
    int boardX;
    int boardY;
    int blockType;
};

class GameNetworkManager {
public:
    GameNetworkManager();
    ~GameNetworkManager();

    bool connectToServer(const std::string& serverAddr, uint16_t port, const std::string& playerName);
    void disconnect();
    bool isConnected() const;

    void sendPlayerMove(int boardX, int boardY, int blockType);
    void sendScoreUpdate(float score, int linesCleared);
    void sendGameState(const int boardState[64], float score);
    void updateTimer(float timeRemaining);

    // Callbacks for received messages
    using OpponentMoveCallback = std::function<void(const OpponentMove&)>;
    using OpponentScoreCallback = std::function<void(float)>;
    using GameStateCallback = std::function<void(const GameStateSync&)>;
    using TimeUpdateCallback = std::function<void(float, float)>;
    using GameOverCallback = std::function<void(bool, const std::string&)>; // isWinner, reason

    void setOpponentMoveCallback(OpponentMoveCallback cb);
    void setOpponentScoreCallback(OpponentScoreCallback cb);
    void setGameStateCallback(GameStateCallback cb);
    void setTimeUpdateCallback(TimeUpdateCallback cb);
    void setGameOverCallback(GameOverCallback cb);

    void update(); // Called each frame to process queued messages

private:
    std::unique_ptr<NetworkClient> client;
    std::string localPlayerName;

    OpponentMoveCallback onOpponentMove;
    OpponentScoreCallback onOpponentScore;
    GameStateCallback onGameState;
    TimeUpdateCallback onTimeUpdate;
    GameOverCallback onGameOver;

    void handleMessage(const Message& msg);
    
    std::vector<uint8_t> buildPayload(const std::string& str);
    std::string extractString(const uint8_t* data, size_t& offset, size_t maxLen);
};

#endif // GAMENETWORKMANAGER_H
