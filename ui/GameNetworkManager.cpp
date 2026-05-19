#include "GameNetworkManager.h"
#include <cstring>
#include <iostream>

GameNetworkManager::GameNetworkManager()
    : localPlayerName("") {
    client = std::make_unique<NetworkClient>();
}

GameNetworkManager::~GameNetworkManager() {
    disconnect();
}

bool GameNetworkManager::connectToServer(const std::string& serverAddr, uint16_t port, const std::string& playerName) {
    localPlayerName = playerName;
    
    if (!client->connect(serverAddr, port)) {
        std::cerr << "Failed to connect to server\n";
        return false;
    }

    client->setMessageCallback([this](const Message& msg) {
        this->handleMessage(msg);
    });

    client->startReceiveThread();

    // Send handshake
    Message handshake;
    handshake.type = MessageType::HANDSHAKE;
    auto nameBytes = buildPayload(playerName);
    handshake.payload = nameBytes;
    client->sendMessage(handshake);

    return true;
}

void GameNetworkManager::disconnect() {
    if (client) {
        client->disconnect();
    }
}

bool GameNetworkManager::isConnected() const {
    return client && client->isConnected();
}

void GameNetworkManager::sendPlayerMove(int boardX, int boardY, int blockType) {
    Message msg;
    msg.type = MessageType::MOVE;
    msg.payload.resize(3);
    msg.payload[0] = static_cast<uint8_t>(boardX);
    msg.payload[1] = static_cast<uint8_t>(boardY);
    msg.payload[2] = static_cast<uint8_t>(blockType);
    client->sendMessage(msg);
}

void GameNetworkManager::sendScoreUpdate(float score, int linesCleared) {
    Message msg;
    msg.type = MessageType::SCORE_UPDATE;
    msg.payload.resize(5);
    
    uint32_t scoreInt = static_cast<uint32_t>(score);
    msg.payload[0] = (scoreInt >> 24) & 0xFF;
    msg.payload[1] = (scoreInt >> 16) & 0xFF;
    msg.payload[2] = (scoreInt >> 8) & 0xFF;
    msg.payload[3] = scoreInt & 0xFF;
    msg.payload[4] = static_cast<uint8_t>(linesCleared);
    
    client->sendMessage(msg);
}

void GameNetworkManager::sendGameState(const int boardState[64], float score) {
    Message msg;
    msg.type = MessageType::GAME_STATE;
    msg.payload.resize(64 + 4);
    
    for (int i = 0; i < 64; ++i) {
        msg.payload[i] = static_cast<uint8_t>(boardState[i] & 0xFF);
    }
    
    uint32_t scoreInt = static_cast<uint32_t>(score);
    msg.payload[64] = (scoreInt >> 24) & 0xFF;
    msg.payload[65] = (scoreInt >> 16) & 0xFF;
    msg.payload[66] = (scoreInt >> 8) & 0xFF;
    msg.payload[67] = scoreInt & 0xFF;
    
    client->sendMessage(msg);
}

void GameNetworkManager::updateTimer(float timeRemaining) {
    Message msg;
    msg.type = MessageType::TIME_UPDATE;
    msg.payload.resize(4);
    
    uint32_t timeInt = static_cast<uint32_t>(timeRemaining * 1000); // ms
    msg.payload[0] = (timeInt >> 24) & 0xFF;
    msg.payload[1] = (timeInt >> 16) & 0xFF;
    msg.payload[2] = (timeInt >> 8) & 0xFF;
    msg.payload[3] = timeInt & 0xFF;
    
    client->sendMessage(msg);
}

void GameNetworkManager::setOpponentMoveCallback(OpponentMoveCallback cb) {
    onOpponentMove = cb;
}

void GameNetworkManager::setOpponentScoreCallback(OpponentScoreCallback cb) {
    onOpponentScore = cb;
}

void GameNetworkManager::setGameStateCallback(GameStateCallback cb) {
    onGameState = cb;
}

void GameNetworkManager::setTimeUpdateCallback(TimeUpdateCallback cb) {
    onTimeUpdate = cb;
}

void GameNetworkManager::setGameOverCallback(GameOverCallback cb) {
    onGameOver = cb;
}

void GameNetworkManager::update() {
    // Messages are handled in receive thread callback
    // This is a placeholder for any frame-level processing if needed
}

void GameNetworkManager::handleMessage(const Message& msg) {
    switch (msg.type) {
        case MessageType::MOVE: {
            if (msg.payload.size() >= 3) {
                OpponentMove move;
                move.boardX = msg.payload[0];
                move.boardY = msg.payload[1];
                move.blockType = msg.payload[2];
                if (onOpponentMove) onOpponentMove(move);
            }
            break;
        }
        case MessageType::SCORE_UPDATE: {
            if (msg.payload.size() >= 4) {
                uint32_t scoreInt = (msg.payload[0] << 24) | (msg.payload[1] << 16) |
                                    (msg.payload[2] << 8) | msg.payload[3];
                float score = static_cast<float>(scoreInt);
                if (onOpponentScore) onOpponentScore(score);
            }
            break;
        }
        case MessageType::TIME_UPDATE: {
            if (msg.payload.size() >= 8) {
                uint32_t timeP1 = (msg.payload[0] << 24) | (msg.payload[1] << 16) |
                                  (msg.payload[2] << 8) | msg.payload[3];
                uint32_t timeP2 = (msg.payload[4] << 24) | (msg.payload[5] << 16) |
                                  (msg.payload[6] << 8) | msg.payload[7];
                if (onTimeUpdate) onTimeUpdate(timeP1 / 1000.0f, timeP2 / 1000.0f);
            }
            break;
        }
        case MessageType::GAME_OVER: {
            if (msg.payload.size() >= 1) {
                bool isWinner = msg.payload[0] != 0;
                size_t offset = 1;
                std::string reason = extractString(msg.payload.data(), offset, msg.payload.size());
                if (onGameOver) onGameOver(isWinner, reason);
            }
            break;
        }
        default:
            break;
    }
}

std::vector<uint8_t> GameNetworkManager::buildPayload(const std::string& str) {
    std::vector<uint8_t> payload(str.size());
    std::memcpy(payload.data(), str.c_str(), str.size());
    return payload;
}

std::string GameNetworkManager::extractString(const uint8_t* data, size_t& offset, size_t maxLen) {
    if (offset >= maxLen) return "";
    std::string result(reinterpret_cast<const char*>(data + offset), maxLen - offset);
    offset = maxLen;
    return result;
}
