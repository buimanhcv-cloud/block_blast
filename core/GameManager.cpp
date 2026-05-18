#include "GameManager.h"

#include <iostream>
#include "core/GraphicsConfig.h"
#include "util/ResourceUtils.h"

// Tray slot centres (x, y) for 3 blocks spread across bottom panel
static const sf::Vector2f TRAY_CENTERS[3] = {
    {115.f, 767.f},
    {GraphicsConfig::WINDOW_WIDTH / 2.f, 767.f},
    {425.f, 767.f},
};

GameManager::GameManager(sf::RenderWindow& window)
    : window(window),
      currentState(GameState::MENU),
      draggingIndex(-1),
      score(0),
      highScore(0),
      combo(0) {
    for (int i = 0; i < TRAY_SIZE; ++i)
        trayPositions[i] = TRAY_CENTERS[i];

    if (loadOptionalSoundBuffer(placeBuffer,    "assets/place.wav"))    placeSound.setBuffer(placeBuffer);
    if (loadOptionalSoundBuffer(clearBuffer,    "assets/clear.wav"))    clearSound.setBuffer(clearBuffer);
    if (loadOptionalSoundBuffer(comboBuffer,    "assets/combo.wav"))    comboSound.setBuffer(comboBuffer);
    if (loadOptionalSoundBuffer(gameOverBuffer, "assets/gameover.wav")) gameOverSound.setBuffer(gameOverBuffer);

    board = std::make_unique<Board>();
    menu  = std::make_unique<Menu>();
    hud   = std::make_unique<HUD>();
}

GameManager::~GameManager() {}

// ── Event handling ─────────────────────────────────────────────────────────────

void GameManager::handleEvent(const sf::Event& event) {
    if (currentState == GameState::MENU) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            if (menu->isStartClicked(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
                startGame();
        }
        return;
    }

    if (currentState == GameState::GAME_OVER) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            if (menu->isRestartClicked(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
                startGame();
        }
        return;
    }

    // PLAYING ─────────────────────────────────────────────────────────────────
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        // Pick up whichever tray block was clicked
        for (int i = 0; i < TRAY_SIZE; ++i) {
            if (!trayBlocks[i]) continue;
            if (trayBlocks[i]->containsPreview(mouse, trayPositions[i])) {
                draggingIndex = i;
                dragOffset = sf::Vector2i(
                    event.mouseButton.x - static_cast<int>(trayBlocks[i]->getPosition().x),
                    event.mouseButton.y - static_cast<int>(trayBlocks[i]->getPosition().y));
                break;
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased &&
             event.mouseButton.button == sf::Mouse::Left) {
        if (draggingIndex >= 0 && trayBlocks[draggingIndex]) {
            const sf::Vector2i boardPos = board->getBoardPosition(trayBlocks[draggingIndex]->getPosition());

            if (board->canPlaceBlock(*trayBlocks[draggingIndex], boardPos)) {
                const sf::Vector2f snappedPos(
                    Board::OFFSET_X + static_cast<float>(boardPos.x) * Board::CELL_SIZE,
                    Board::OFFSET_Y + static_cast<float>(boardPos.y) * Board::CELL_SIZE);
                trayBlocks[draggingIndex]->setPosition(snappedPos);

                board->placeBlock(*trayBlocks[draggingIndex], boardPos);
                playSound(placeSound);
                checkForClears();
                trayBlocks[draggingIndex].reset(); // consume the block

                // Refill tray only when all 3 slots are empty
                bool allEmpty = true;
                for (int i = 0; i < TRAY_SIZE; ++i)
                    if (trayBlocks[i]) { allEmpty = false; break; }
                if (allEmpty) refillTray();

                // Check game-over against remaining tray blocks
                if (!anyBlockCanBePlaced()) endGame();
            } else {
                // Snap back to tray home position
                const sf::Vector2f& home = trayPositions[draggingIndex];
                const sf::Vector2f sz = trayBlocks[draggingIndex]->getSize();
                trayBlocks[draggingIndex]->setPosition(
                    home - sf::Vector2f(sz.x / 2.f, sz.y / 2.f));
            }
            draggingIndex = -1;
        }
    }
    else if (event.type == sf::Event::MouseMoved && draggingIndex >= 0 &&
             trayBlocks[draggingIndex]) {
        trayBlocks[draggingIndex]->setPosition(sf::Vector2f(
            static_cast<float>(event.mouseMove.x - dragOffset.x),
            static_cast<float>(event.mouseMove.y - dragOffset.y)));
    }
}

// ── Update / Render ────────────────────────────────────────────────────────────

void GameManager::update() {
    if (currentState == GameState::PLAYING) {
        board->update();
        hud->updateScore(score, highScore);
    }
}

void GameManager::render() {
    window.clear(currentState == GameState::MENU
        ? sf::Color(37, 119, 224)
        : sf::Color(115, 82, 98));

    if (currentState == GameState::MENU) {
        menu->render(window);
    } else if (currentState == GameState::PLAYING) {
        board->render(window);

        // Draw ghost preview when dragging over board
        if (draggingIndex >= 0 && trayBlocks[draggingIndex]) {
            const sf::Vector2f blockPos = trayBlocks[draggingIndex]->getPosition();
            const sf::Vector2i boardPos = board->getBoardPosition(blockPos);
            const bool canPlace = board->canPlaceBlock(*trayBlocks[draggingIndex], boardPos);

            if (canPlace) {
                board->renderGhostCells(window, *trayBlocks[draggingIndex], boardPos);
            }
        }

        drawTray();
        hud->render(window);
        // Draw the dragged block on top of everything
        if (draggingIndex >= 0 && trayBlocks[draggingIndex])
            trayBlocks[draggingIndex]->render(window);
    } else if (currentState == GameState::GAME_OVER) {
        board->render(window);
        menu->renderGameOver(window, score, highScore);
    }

    window.display();
}

// ── Private helpers ────────────────────────────────────────────────────────────

void GameManager::startGame() {
    currentState = GameState::PLAYING;
    score = 0;
    combo = 0;
    board->reset();
    refillTray();
}

void GameManager::endGame() {
    if (score > highScore) highScore = score;
    currentState = GameState::GAME_OVER;
    playSound(gameOverSound);
}

void GameManager::refillTray() {
    for (int i = 0; i < TRAY_SIZE; ++i) {
        trayBlocks[i] = std::make_unique<Block>();
        const sf::Vector2f fullSize = trayBlocks[i]->getSize();
        trayBlocks[i]->setPosition(
            trayPositions[i] - sf::Vector2f(fullSize.x / 2.f, fullSize.y / 2.f));
    }
}

void GameManager::drawTray() {
    for (int i = 0; i < TRAY_SIZE; ++i) {
        if (!trayBlocks[i]) continue;
        if (i == draggingIndex) continue; // drawn on top later
        trayBlocks[i]->renderPreview(window, trayPositions[i]);
    }
}

void GameManager::checkForClears() {
    const int linesCleared = board->clearFullLines();
    if (linesCleared > 0) {
        score += linesCleared * 10 * (combo + 1);
        combo++;
        playSound(clearSound);
        if (combo > 1) playSound(comboSound);
    } else {
        combo = 0;
    }
}

bool GameManager::anyBlockCanBePlaced() const {
    for (int i = 0; i < TRAY_SIZE; ++i) {
        if (!trayBlocks[i]) continue;
        if (board->canPlaceAnyBlock(*trayBlocks[i])) return true;
    }
    return false;
}

void GameManager::playSound(sf::Sound& sound) {
    if (sound.getBuffer()) sound.play();
}
