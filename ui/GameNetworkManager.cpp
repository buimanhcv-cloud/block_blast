#include "GameNetworkManager.h"
#include <iostream>
#include <cstring>

GameNetworkManager::GameNetworkManager() {}
GameNetworkManager::~GameNetworkManager() { disconnect(); }

bool GameNetworkManager::connect(const std::string& host, unsigned short port) {
    return client_.connect(host, port);
}

void GameNetworkManager::disconnect() {
    client_.disconnect();
}

bool GameNetworkManager::isConnected() const {
    return client_.isConnected();
}

void GameNetworkManager::joinRoom(const std::string& roomId,
                                  OnlineMode mode, TimePreset timePreset) {
    client_.sendJoin(roomId,
                     static_cast<int>(mode),
                     static_cast<int>(timePreset));
}

void GameNetworkManager::sendReady() {
    client_.sendReady();
}

void GameNetworkManager::sendMove(int boardX, int boardY,
                                  const std::vector<std::vector<int>>& shape) {
    client_.sendMove(boardX, boardY, shape);
}

// ── Packet parsing ────────────────────────────────────────────────────────────

void GameNetworkManager::pollEvents(const NetworkCallbacks& callbacks) {
    NetPacket pkt;
    while (client_.pollPacket(pkt)) {
        handlePacket(pkt, callbacks);
    }
    // Only fire disconnect if we were previously connected and now lost it
    if (wasConnected_ && !client_.isConnected()) {
        wasConnected_ = false;
        if (callbacks.onDisconnected)
            callbacks.onDisconnected();
    }
    if (client_.isConnected()) wasConnected_ = true;
}

void GameNetworkManager::handlePacket(const NetPacket& pkt,
                                      const NetworkCallbacks& cb) {
    const auto& p = pkt.payload;

    switch (pkt.type) {

    case MsgType::ROOM_JOINED:
        // payload: [1 playerId]
        if (p.size() >= 1) {
            state_.myPlayerId = p[0];
            if (cb.onRoomJoined) cb.onRoomJoined(state_.myPlayerId);
        }
        break;

    case MsgType::GAME_START:
        // payload: [1 mode][2 timeLimitSec LE]
        if (p.size() >= 3) {
            OnlineMode mode = static_cast<OnlineMode>(p[0]);
            int timeSec = static_cast<int>(p[1]) | (static_cast<int>(p[2]) << 8);
            state_.myTimeMs       = timeSec * 1000;
            state_.opponentTimeMs = timeSec * 1000;
            if (cb.onGameStart) cb.onGameStart(mode, timeSec);
        }
        break;

    case MsgType::GAME_STATE:
        // payload: [64 bytes opponent grid row-major]
        if (p.size() >= Board::SIZE * Board::SIZE) {
            for (int y = 0; y < Board::SIZE; ++y)
                for (int x = 0; x < Board::SIZE; ++x)
                    state_.opponentGrid[x][y] = p[y * Board::SIZE + x];
            if (cb.onStateUpdate) cb.onStateUpdate(state_);
        }
        break;

    case MsgType::SCORE_UPDATE:
        // payload: [1 playerId][4 score LE][1 combo]
        if (p.size() >= 6) {
            int pid   = p[0];
            int score = p[1] | (p[2] << 8) | (p[3] << 16) | (p[4] << 24);
            int combo = p[5];
            if (pid == state_.myPlayerId) {
                state_.myScore = score;
                state_.myCombo = combo;
            } else {
                state_.opponentScore = score;
                state_.opponentCombo = combo;
            }
            if (cb.onStateUpdate) cb.onStateUpdate(state_);
        }
        break;

    case MsgType::TIME_UPDATE:
        // payload: [4 p0TimeMs LE][4 p1TimeMs LE]
        if (p.size() >= 8) {
            int t0 = p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
            int t1 = p[4] | (p[5]<<8) | (p[6]<<16) | (p[7]<<24);
            if (state_.myPlayerId == 0) {
                state_.myTimeMs       = t0;
                state_.opponentTimeMs = t1;
            } else {
                state_.myTimeMs       = t1;
                state_.opponentTimeMs = t0;
            }
            if (cb.onStateUpdate) cb.onStateUpdate(state_);
        }
        break;

    case MsgType::YOUR_TURN:
        state_.myTurn = true;
        if (cb.onYourTurn) cb.onYourTurn();
        break;

    case MsgType::GAME_OVER:
        // payload: [1 winnerId]
        if (p.size() >= 1) {
            state_.gameOver = true;
            state_.winnerId = p[0];
            if (cb.onGameOver) cb.onGameOver(state_.winnerId);
        }
        break;

    case MsgType::PONG:
        break;

    case MsgType::ERROR_MSG:
        std::cerr << "[Net] Server error\n";
        break;

    default:
        std::cerr << "[Net] Unknown packet type: "
                  << static_cast<int>(pkt.type) << "\n";
        break;
    }
}
