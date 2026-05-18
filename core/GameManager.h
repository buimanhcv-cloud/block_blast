#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <memory>

#include "block/Block.h"
#include "board/Board.h"
#include "ui/HUD.h"
#include "ui/Menu.h"

enum class GameState { MENU, PLAYING, GAME_OVER };

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
    GameState currentState;
    std::unique_ptr<Board> board;
    std::unique_ptr<Menu> menu;
    std::unique_ptr<HUD> hud;

    // Tray: 3 blocks at a time
    std::array<std::unique_ptr<Block>, TRAY_SIZE> trayBlocks;
    int draggingIndex;        // which tray slot is being dragged (-1 = none)
    sf::Vector2i dragOffset;
    sf::Vector2f trayPositions[TRAY_SIZE]; // home positions in tray

    sf::SoundBuffer placeBuffer, clearBuffer, comboBuffer, gameOverBuffer;
    sf::Sound placeSound, clearSound, comboSound, gameOverSound;

    int score;
    int highScore;
    int combo;

    void startGame();
    void endGame();
    void refillTray();
    void checkForClears();
    void playSound(sf::Sound& sound);
    bool anyBlockCanBePlaced() const;
    void drawTray();
};

#endif // GAMEMANAGER_H
