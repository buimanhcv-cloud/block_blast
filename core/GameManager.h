#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <memory>
#include <thread>
#include <atomic>

#include "block/Block.h"
#include "board/Board.h"
#include "ui/HUD.h"
#include "ui/Menu.h"
#include "ui/GameNetworkManager.h"
#include "ui/OnlineGameMode.h"
#include "ui/OnlineGameSelectScreen.h"
#include "core/AIPlayer.h"

enum class GameState {
    MENU,
    PLAYING_SOLO,
    PLAYING_VS_AI,
    GAME_OVER,
    ONLINE_SELECT,
    ONLINE_PLAYING
};

class GameManager {
public:
    explicit GameManager(sf::RenderWindow& window);
    ~GameManager();

    void handleEvent(const sf::Event& event);
    void update();
    void render();

private:
    static const int TRAY_SIZE = 3;

    sf::RenderWindow& window;
    GameState currentState = GameState::MENU;

    // ── Core objects ──────────────────────────────────────────────────────────
    std::unique_ptr<Board> board;
    std::unique_ptr<Menu>  menu;
    std::unique_ptr<HUD>   hud;

    // ── Tray ──────────────────────────────────────────────────────────────────
    std::array<std::unique_ptr<Block>, TRAY_SIZE> trayBlocks;
    int          draggingIndex = -1;
    sf::Vector2i dragOffset;
    sf::Vector2f trayPositions[TRAY_SIZE];

    // ── Sounds ────────────────────────────────────────────────────────────────
    sf::SoundBuffer placeBuffer, clearBuffer, comboBuffer, gameOverBuffer;
    sf::Sound       placeSound,  clearSound,  comboSound,  gameOverSound;

    // ── Score ─────────────────────────────────────────────────────────────────
    int score     = 0;
    int highScore = 0;
    int combo     = 0;

    // ── AI ────────────────────────────────────────────────────────────────────
    sf::Clock aiClock;
    float     aiThinkDelay = 1.2f;  // seconds between AI moves
    bool      aiWaiting    = false;

    // ── Online ────────────────────────────────────────────────────────────────
    GameNetworkManager                  netManager;
    std::unique_ptr<OnlineGameSelectScreen> selectScreen;
    std::unique_ptr<OnlineGameMode>         onlineGame;
    NetworkCallbacks                        netCallbacks;
    std::thread                             connectThread_;   // async connect
    std::atomic<bool>                       connecting_{false};

    // ── In-game back button ───────────────────────────────────────────────────
    sf::Font           uiFont;
    sf::RectangleShape backBtn;
    sf::Text           backBtnTxt;

    // ── Private methods ───────────────────────────────────────────────────────
    void startSolo();
    void startVsAI();
    void endGame();
    void refillTray();
    void checkForClears();
    void playSound(sf::Sound& sound);
    bool anyBlockCanBePlaced() const;
    void drawTray();
    void drawBackButton();
    bool isBackBtnClicked(sf::Vector2f p) const;

    void handlePlayingEvent(const sf::Event& event);
    void updateAI();

    void setupNetworkCallbacks();
    void enterOnlineSelect();
    void connectAndJoin();
};

#endif // GAMEMANAGER_H
