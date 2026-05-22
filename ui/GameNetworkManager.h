#ifndef GAMENETWORKMANAGER_H
#define GAMENETWORKMANAGER_H

#include "NetworkClient.h"
#include "board/Board.h"
#include "block/Block.h"
#include <string>
#include <functional>
#include <array>
#include <memory>

// ── Online game modes ─────────────────────────────────────────────────────────
enum class OnlineMode { REALTIME = 0, TURN_BASED = 1 };

// ── Time presets (seconds) ────────────────────────────────────────────────────
enum class TimePreset { FAST = 60, NORMAL = 180, LONG = 300 };

// ── Shared game state received from server ────────────────────────────────────
struct OnlineGameState {
    int  myScore      = 0;
    int  opponentScore= 0;
    int  myCombo      = 0;
    int  opponentCombo= 0;
    int  myTimeMs     = 0;   // remaining time in ms
    int  opponentTimeMs = 0;
    bool myTurn       = false;  // turn-based only
    bool gameOver     = false;
    int  winnerId     = -1;     // 0 or 1
    int  myPlayerId   = -1;     // assigned by server

    // Opponent board snapshot (8×8)
    int opponentGrid[Board::SIZE][Board::SIZE] = {};
};

// ── Callbacks fired on main thread via pollEvents() ──────────────────────────
struct NetworkCallbacks {
    std::function<void(int playerId)>          onRoomJoined;
    std::function<void(OnlineMode, int timeSec)> onGameStart;
    std::function<void(const OnlineGameState&)> onStateUpdate;
    std::function<void(int winnerId)>           onGameOver;
    std::function<void()>                       onYourTurn;
    std::function<void()>                       onDisconnected;
};

// ── High-level manager: wraps NetworkClient, parses packets ──────────────────
class GameNetworkManager {
public:
    GameNetworkManager();
    ~GameNetworkManager();

    bool connect(const std::string& host, unsigned short port);
    void disconnect();
    bool isConnected() const;

    // Join or create a room
    void joinRoom(const std::string& roomId, OnlineMode mode, TimePreset timePreset);
    void sendReady();

    // Call after placing a block
    void sendMove(int boardX, int boardY,
                  const std::vector<std::vector<int>>& shape);

    // Call every frame from main thread — fires callbacks
    void pollEvents(const NetworkCallbacks& callbacks);

    const OnlineGameState& getState() const { return state_; }

private:
    void handlePacket(const NetPacket& pkt, const NetworkCallbacks& cb);

    NetworkClient   client_;
    OnlineGameState state_;
    bool            wasConnected_ = false;
};

#endif // GAMENETWORKMANAGER_H
