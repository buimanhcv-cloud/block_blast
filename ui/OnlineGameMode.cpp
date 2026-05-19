#include "OnlineGameMode.h"
#include <iostream>
#include <cstring>

OnlineGameMode::OnlineGameMode(OnlineGameType type, TimeMode timeMode)
    : gameType(type), timeMode(timeMode), myTimeRemaining(static_cast<float>(timeMode)),
      opponentTimeRemaining(static_cast<float>(timeMode)), opponentScore(0.0f),
      isActive(false), isCurrentTurn(true) {
    networkMgr = std::make_unique<GameNetworkManager>();
    std::memset(opponentBoardState, 0, sizeof(opponentBoardState));
}

OnlineGameMode::~OnlineGameMode() {
    disconnect();
}

bool OnlineGameMode::joinGame(const std::string& serverAddr, uint16_t port, const std::string& playerName) {
    if (!networkMgr->connectToServer(serverAddr, port, playerName)) {
        std::cerr << "Failed to connect to online game\n";
        return false;
    }

    // Set up callbacks
    networkMgr->setOpponentMoveCallback([this](const OpponentMove& move) {
        this->onOpponentMove(move);
    });

    networkMgr->setOpponentScoreCallback([this](float score) {
        this->onOpponentScore(score);
    });

    networkMgr->setGameStateCallback([this](const GameStateSync& state) {
        this->onGameState(state);
    });

    networkMgr->setTimeUpdateCallback([this](float timeP1, float timeP2) {
        this->onTimeUpdate(timeP1, timeP2);
    });

    networkMgr->setGameOverCallback([this](bool isWinner, const std::string& reason) {
        this->onGameOver(isWinner, reason);
    });

    isActive = true;
    myTimeRemaining = static_cast<float>(timeMode);
    opponentTimeRemaining = static_cast<float>(timeMode);

    return true;
}

void OnlineGameMode::disconnect() {
    if (networkMgr) {
        networkMgr->disconnect();
    }
    isActive = false;
}

bool OnlineGameMode::isGameActive() const {
    return isActive && networkMgr->isConnected();
}

bool OnlineGameMode::isMyTurn() const {
    if (gameType == OnlineGameType::REALTIME) {
        return true; // Always can play in realtime mode
    }
    return isCurrentTurn;
}

void OnlineGameMode::updateLocal(float deltaTime) {
    if (!isActive) return;

    networkMgr->update();
    updateTimer(deltaTime);

    // Send periodic time updates
    static float timeSinceLastUpdate = 0.0f;
    timeSinceLastUpdate += deltaTime;
    if (timeSinceLastUpdate >= 0.5f) {
        networkMgr->updateTimer(myTimeRemaining);
        timeSinceLastUpdate = 0.0f;
    }
}

void OnlineGameMode::notifyPlayerMove(int boardX, int boardY, int blockType) {
    if (!isActive) return;
    networkMgr->sendPlayerMove(boardX, boardY, blockType);
}

void OnlineGameMode::notifyScoreUpdate(float score, int linesCleared) {
    if (!isActive) return;
    networkMgr->sendScoreUpdate(score, linesCleared);
}

void OnlineGameMode::notifyGameState(const int boardState[64], float score) {
    if (!isActive) return;
    networkMgr->sendGameState(boardState, score);
}

float OnlineGameMode::getOpponentScore() const {
    return opponentScore;
}

const int* OnlineGameMode::getOpponentBoardState() const {
    return opponentBoardState;
}

float OnlineGameMode::getMyTimeRemaining() const {
    return myTimeRemaining;
}

float OnlineGameMode::getOpponentTimeRemaining() const {
    return opponentTimeRemaining;
}

void OnlineGameMode::onOpponentMove(const OpponentMove& move) {
    std::cout << "Opponent moved at (" << move.boardX << ", " << move.boardY << ")\n";
}

void OnlineGameMode::onOpponentScore(float score) {
    opponentScore = score;
}

void OnlineGameMode::onGameState(const GameStateSync& state) {
    opponentScore = state.opponentScore;
    std::memcpy(opponentBoardState, state.opponentBoardState, sizeof(opponentBoardState));
}

void OnlineGameMode::onTimeUpdate(float timeP1, float timeP2) {
    myTimeRemaining = timeP1;
    opponentTimeRemaining = timeP2;
}

void OnlineGameMode::onGameOver(bool isWinner, const std::string& reason) {
    std::cout << (isWinner ? "You won!" : "You lost!") << " Reason: " << reason << "\n";
    isActive = false;
}

void OnlineGameMode::updateTimer(float deltaTime) {
    if (gameType == OnlineGameType::REALTIME) {
        return; // No time limit in realtime
    }

    // Turn-based: only subtract time when it's your turn
    if (isCurrentTurn) {
        myTimeRemaining -= deltaTime;
        if (myTimeRemaining < 0.0f) {
            myTimeRemaining = 0.0f;
            isActive = false;
        }
    }
}

void OnlineGameMode::render(sf::RenderWindow& window) {
    if (!isActive) return;

    renderOpponentBoard(window);
    renderTimer(window);
}

void OnlineGameMode::renderOpponentBoard(sf::RenderWindow& window) {
    // This will be integrated with the main game's render system
    // For now, this is a placeholder
}

void OnlineGameMode::renderTimer(sf::RenderWindow& window) {
    // This will display the chess-clock style timer
    // To be integrated with main HUD
}
