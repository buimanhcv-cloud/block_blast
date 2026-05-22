#ifndef ONLINEGAMEMODE_H
#define ONLINEGAMEMODE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <memory>

#include "board/Board.h"
#include "block/Block.h"
#include "ui/GameNetworkManager.h"
#include "util/ResourceUtils.h"

class OnlineGameMode {
public:
    OnlineGameMode(sf::RenderWindow& window, GameNetworkManager& net);
    ~OnlineGameMode();

    void startGame(OnlineMode mode, int timeLimitSec);

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render();

    bool isGameOver()   const { return gameOver_; }
    int  getWinnerId()  const { return winnerId_; }
    bool wantsBackToMenu() const { return wantsBack_; }

    // Called by GameNetworkManager callbacks
    void onStateUpdate(const OnlineGameState& state);
    void onYourTurn();
    void onGameOver(int winnerId);

    // Called when server confirms game started
    void markGameStarted() { gameStarted_ = true; }

private:
    static const int TRAY_SIZE = 3;

    void drawOpponentBoard();
    void drawClocks();
    void drawScores();
    void drawTray();
    void drawTurnIndicator();

    bool canPlaceNow() const;
    bool isBackClicked(sf::Vector2f p) const;
    void placeBlock(int trayIdx, sf::Vector2i boardPos);
    void snapBack(int idx);
    void refillTray();
    bool anyBlockCanBePlaced() const;
    void checkForClears();
    void playSound(sf::Sound& sound);

    sf::RenderWindow&   window_;
    GameNetworkManager& net_;

    OnlineMode mode_        = OnlineMode::REALTIME;
    int  timeLimitSec_      = 180;
    bool gameOver_          = false;
    bool gameStarted_       = false;
    bool wantsBack_         = false;
    int  winnerId_          = -1;
    bool myTurn_            = false;

    std::unique_ptr<Board> myBoard_;
    std::array<std::unique_ptr<Block>, TRAY_SIZE> trayBlocks_;
    sf::Vector2f trayPositions_[TRAY_SIZE];
    int          draggingIndex_ = -1;
    sf::Vector2i dragOffset_;

    int myScore_        = 0;
    int opponentScore_  = 0;
    int myCombo_        = 0;
    int myTimeMs_       = 0;
    int opponentTimeMs_ = 0;

    int opponentGrid_[Board::SIZE][Board::SIZE] = {};

    sf::Font font_;

    sf::SoundBuffer placeBuffer_, clearBuffer_, comboBuffer_, gameOverBuffer_;
    sf::Sound       placeSound_,  clearSound_,  comboSound_,  gameOverSound_;

    sf::RectangleShape oppBoardBg_;
    sf::RectangleShape myClockBg_;
    sf::RectangleShape oppClockBg_;
    sf::RectangleShape turnIndicator_;
    sf::RectangleShape backBtn_;
    sf::Text           backBtnTxt_;
};

#endif // ONLINEGAMEMODE_H
