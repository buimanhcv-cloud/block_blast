#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <memory>

#include "block/Block.h"
#include "board/Board.h"
#include "ui/HUD.h"
#include "ui/DuelSelectScreen.h"
#include "ui/Menu.h"
#include "ui/OnlineGameMode.h"

enum class GameMode { Classic, Versus, Online };
enum class GameState { MENU, DUEL_SELECT, ONLINE_SELECT, PLAYING, GAME_OVER, VERSUS_OVER, ONLINE_OVER };

class GameManager {
public:
    GameManager(sf::RenderWindow& window);
    ~GameManager();

    void handleEvent(const sf::Event& event);
    void update();
    void render();

private:
    static const int TRAY_SIZE = 3;

    sf::RenderWindow& window;
    GameMode gameMode;
    GameState currentState;
    int activePlayer;
    int versusWinner;

    std::unique_ptr<Board> board;
    std::unique_ptr<Menu> menu;
    std::unique_ptr<DuelSelectScreen> duelSelect;
    std::unique_ptr<HUD> hud;
    std::unique_ptr<OnlineGameMode> onlineMode;

    std::array<std::array<std::unique_ptr<Block>, TRAY_SIZE>, 2> playerTrays;
    int draggingIndex;
    sf::Vector2i dragOffset;
    sf::Vector2f trayPositions[TRAY_SIZE];

    sf::SoundBuffer placeBuffer, clearBuffer, comboBuffer, gameOverBuffer;
    sf::Sound placeSound, clearSound, comboSound, gameOverSound;

    int score;
    int highScore;
    bool newBestThisRun;

    std::array<std::unique_ptr<Block>, TRAY_SIZE>& activeTray();
    const std::array<std::unique_ptr<Block>, TRAY_SIZE>& activeTray() const;

    void startClassicGame();
    void startVersusGame();
    void startOnlineGame(OnlineGameType type, TimeMode timeMode);
    void returnToMenu();
    void endClassicGame();
    void endVersusGame(int winnerPlayer);
    void endOnlineGame();
    void refillTrayFor(int player);
    void spawnBlockInSlot(int player, int slot);
    void checkForClears();
    void switchVersusTurn();
    void playSound(sf::Sound& sound);
    bool anyBlockCanBePlacedFor(int player) const;
    void drawTray();
    void handlePlayingInput(const sf::Event& event);
};

#endif // GAMEMANAGER_H
