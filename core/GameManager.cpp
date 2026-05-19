#include "GameManager.h"

#include "core/GraphicsConfig.h"
#include "core/Scoring.h"
#include "util/ResourceUtils.h"
#include "util/ScoreStorage.h"

static const sf::Vector2f TRAY_CENTERS[3] = {
    {115.f, 767.f},
    {GraphicsConfig::WINDOW_WIDTH / 2.f, 767.f},
    {425.f, 767.f},
};

static const sf::Color PLAYER_COLORS[2] = {
    sf::Color(92, 212, 255),
    sf::Color(255, 140, 90),
};

GameManager::GameManager(sf::RenderWindow& window)
    : window(window),
      gameMode(GameMode::Classic),
      currentState(GameState::MENU),
      activePlayer(0),
      versusWinner(-1),
      draggingIndex(-1),
      score(0),
      highScore(loadHighScore()),
      newBestThisRun(false) {
    for (int i = 0; i < TRAY_SIZE; ++i) {
        trayPositions[i] = TRAY_CENTERS[i];
    }

    if (loadOptionalSoundBuffer(placeBuffer, "assets/place.wav")) {
        placeSound.setBuffer(placeBuffer);
    }
    if (loadOptionalSoundBuffer(clearBuffer, "assets/clear.wav")) {
        clearSound.setBuffer(clearBuffer);
    }
    if (loadOptionalSoundBuffer(comboBuffer, "assets/combo.wav")) {
        comboSound.setBuffer(comboBuffer);
    }
    if (loadOptionalSoundBuffer(gameOverBuffer, "assets/gameover.wav")) {
        gameOverSound.setBuffer(gameOverBuffer);
    }

    board = std::make_unique<Board>();
    menu = std::make_unique<Menu>();
    duelSelect = std::make_unique<DuelSelectScreen>();
    hud = std::make_unique<HUD>();
}

GameManager::~GameManager() {}

std::array<std::unique_ptr<Block>, GameManager::TRAY_SIZE>& GameManager::activeTray() {
    return playerTrays[activePlayer];
}

const std::array<std::unique_ptr<Block>, GameManager::TRAY_SIZE>& GameManager::activeTray() const {
    return playerTrays[activePlayer];
}

void GameManager::handleEvent(const sf::Event& event) {
    if (currentState == GameState::MENU) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            const sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                                    static_cast<float>(event.mouseButton.y));
            if (menu->isClassicClicked(mouse)) {
                startClassicGame();
            } else if (menu->isVersusClicked(mouse)) {
                startVersusGame();
            } else if (menu->isDuelMilestonesClicked(mouse)) {
                currentState = GameState::DUEL_SELECT;
            } else if (menu->isOnlineClicked(mouse)) {
                currentState = GameState::ONLINE_SELECT;
            }
        }
        return;
    }

    if (currentState == GameState::ONLINE_SELECT) {
        // Online game mode will be handled separately
        // For now, stub implementation
        return;
    }

    if (currentState == GameState::DUEL_SELECT) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            const sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                                    static_cast<float>(event.mouseButton.y));
            if (duelSelect->isBackClicked(mouse)) {
                currentState = GameState::MENU;
            } else {
                duelSelect->handleClick(mouse);
            }
        }
        return;
    }

    if (currentState == GameState::GAME_OVER) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            if (menu->isRestartClicked(
                    sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
                startClassicGame();
            }
        }
        return;
    }

    if (currentState == GameState::VERSUS_OVER) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            if (menu->isRestartClicked(
                    sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
                startVersusGame();
            }
        }
        return;
    }

    if (currentState == GameState::PLAYING) {
        handlePlayingInput(event);
    }
}

void GameManager::handlePlayingInput(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        const sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                                static_cast<float>(event.mouseButton.y));

        if (hud->isMenuClicked(mouse)) {
            returnToMenu();
            return;
        }

        auto& tray = activeTray();
        for (int i = 0; i < TRAY_SIZE; ++i) {
            if (!tray[i]) {
                continue;
            }
            if (tray[i]->containsPreview(mouse, trayPositions[i])) {
                draggingIndex = i;
                dragOffset = sf::Vector2i(
                    event.mouseButton.x - static_cast<int>(tray[i]->getPosition().x),
                    event.mouseButton.y - static_cast<int>(tray[i]->getPosition().y));
                break;
            }
        }
    } else if (event.type == sf::Event::MouseButtonReleased &&
               event.mouseButton.button == sf::Mouse::Left) {
        if (draggingIndex < 0) {
            return;
        }

        auto& tray = activeTray();
        if (!tray[draggingIndex]) {
            draggingIndex = -1;
            return;
        }

        const sf::Vector2i boardPos = board->getBoardPosition(tray[draggingIndex]->getPosition());

        if (board->canPlaceBlock(*tray[draggingIndex], boardPos)) {
            const sf::Vector2f snappedPos(
                Board::OFFSET_X + static_cast<float>(boardPos.x) * Board::CELL_SIZE,
                Board::OFFSET_Y + static_cast<float>(boardPos.y) * Board::CELL_SIZE);
            tray[draggingIndex]->setPosition(snappedPos);

            board->placeBlock(*tray[draggingIndex], boardPos);
            playSound(placeSound);
            checkForClears();

            const int usedSlot = draggingIndex;
            const int currentPlayer = activePlayer;
            spawnBlockInSlot(currentPlayer, usedSlot);

            if (gameMode == GameMode::Versus) {
                switchVersusTurn();
            } else if (!anyBlockCanBePlacedFor(0)) {
                endClassicGame();
            }
        } else {
            const sf::Vector2f& home = trayPositions[draggingIndex];
            const sf::Vector2f sz = tray[draggingIndex]->getSize();
            tray[draggingIndex]->setPosition(home - sf::Vector2f(sz.x / 2.f, sz.y / 2.f));
        }

        draggingIndex = -1;
    } else if (event.type == sf::Event::MouseMoved && draggingIndex >= 0) {
        auto& tray = activeTray();
        if (tray[draggingIndex]) {
            tray[draggingIndex]->setPosition(sf::Vector2f(
                static_cast<float>(event.mouseMove.x - dragOffset.x),
                static_cast<float>(event.mouseMove.y - dragOffset.y)));
        }
    }
}

void GameManager::update() {
    if (currentState != GameState::PLAYING) {
        return;
    }

    board->update();

    if (gameMode == GameMode::Versus) {
        hud->setActivePlayer(activePlayer);
    } else {
        hud->updateScore(score);
    }
}

void GameManager::render() {
    window.clear(currentState == GameState::MENU ? sf::Color(37, 119, 224)
                                               : sf::Color(115, 82, 98));

    if (currentState == GameState::MENU) {
        menu->render(window);
    } else if (currentState == GameState::DUEL_SELECT) {
        duelSelect->render(window);
    } else if (currentState == GameState::ONLINE_SELECT) {
        // Placeholder for online select screen
        sf::RectangleShape placeholder(sf::Vector2f(GraphicsConfig::WINDOW_WIDTH, GraphicsConfig::WINDOW_HEIGHT));
        placeholder.setFillColor(sf::Color(50, 50, 100));
        window.draw(placeholder);
    } else if (currentState == GameState::PLAYING) {
        board->render(window);

        auto& tray = activeTray();
        if (draggingIndex >= 0 && tray[draggingIndex]) {
            const sf::Vector2f blockPos = tray[draggingIndex]->getPosition();
            const sf::Vector2i boardPos = board->getBoardPosition(blockPos);
            if (board->canPlaceBlock(*tray[draggingIndex], boardPos)) {
                board->renderGhostCells(window, *tray[draggingIndex], boardPos);
            }
        }

        drawTray();
        hud->render(window);

        if (draggingIndex >= 0 && tray[draggingIndex]) {
            tray[draggingIndex]->render(window);
        }
    } else if (currentState == GameState::GAME_OVER) {
        board->render(window);
        menu->renderGameOver(window, score, highScore, Scoring::rankTitle(score), newBestThisRun);
    } else if (currentState == GameState::VERSUS_OVER) {
        board->render(window);
        menu->renderVersusEnd(window, versusWinner);
    }

    window.display();
}

void GameManager::startClassicGame() {
    gameMode = GameMode::Classic;
    currentState = GameState::PLAYING;
    activePlayer = 0;
    score = 0;
    newBestThisRun = false;
    draggingIndex = -1;
    hud->setVersusMode(false);
    board->reset();
    for (auto& tray : playerTrays) {
        for (auto& block : tray) {
            block.reset();
        }
    }
    refillTrayFor(0);
}

void GameManager::startVersusGame() {
    gameMode = GameMode::Versus;
    currentState = GameState::PLAYING;
    activePlayer = 0;
    versusWinner = -1;
    draggingIndex = -1;
    hud->setVersusMode(true);
    hud->setActivePlayer(0);
    board->reset();
    for (auto& tray : playerTrays) {
        for (auto& block : tray) {
            block.reset();
        }
    }
    refillTrayFor(0);
    refillTrayFor(1);

    if (!anyBlockCanBePlacedFor(0)) {
        endVersusGame(1);
    }
}

void GameManager::startOnlineGame(OnlineGameType type, TimeMode timeMode) {
    gameMode = GameMode::Online;
    currentState = GameState::PLAYING;
    activePlayer = 0;
    draggingIndex = -1;
    hud->setVersusMode(false);
    board->reset();
    for (auto& tray : playerTrays) {
        for (auto& block : tray) {
            block.reset();
        }
    }
    
    onlineMode = std::make_unique<OnlineGameMode>(type, timeMode);
    refillTrayFor(0);
}

void GameManager::returnToMenu() {
    currentState = GameState::MENU;
    draggingIndex = -1;
    hud->setVersusMode(false);
    for (auto& tray : playerTrays) {
        for (auto& block : tray) {
            block.reset();
        }
    }
}

void GameManager::endClassicGame() {
    newBestThisRun = score > highScore;
    if (newBestThisRun) {
        highScore = score;
        saveHighScore(highScore);
    }
    currentState = GameState::GAME_OVER;
    playSound(gameOverSound);
}

void GameManager::endVersusGame(int winnerPlayer) {
    versusWinner = winnerPlayer;
    currentState = GameState::VERSUS_OVER;
    playSound(gameOverSound);
}

void GameManager::endOnlineGame() {
    if (onlineMode) {
        onlineMode->disconnect();
        onlineMode.reset();
    }
    currentState = GameState::ONLINE_OVER;
    playSound(gameOverSound);
}

void GameManager::spawnBlockInSlot(int player, int slot) {
    auto& tray = playerTrays[player];
    tray[slot] = std::make_unique<Block>();
    if (gameMode == GameMode::Versus) {
        tray[slot]->setColor(PLAYER_COLORS[player]);
    }
    const sf::Vector2f fullSize = tray[slot]->getSize();
    tray[slot]->setPosition(
        trayPositions[slot] - sf::Vector2f(fullSize.x / 2.f, fullSize.y / 2.f));
}

void GameManager::refillTrayFor(int player) {
    for (int i = 0; i < TRAY_SIZE; ++i) {
        spawnBlockInSlot(player, i);
    }
}

void GameManager::drawTray() {
    const auto& tray = activeTray();
    for (int i = 0; i < TRAY_SIZE; ++i) {
        if (!tray[i]) {
            continue;
        }
        if (i == draggingIndex) {
            continue;
        }
        tray[i]->renderPreview(window, trayPositions[i]);
    }
}

void GameManager::checkForClears() {
    const int linesCleared = board->clearFullLines();
    if (linesCleared <= 0) {
        return;
    }

    playSound(clearSound);

    if (gameMode == GameMode::Classic) {
        const bool fullBoard = board->isEmpty();
        score += Scoring::pointsForClear(linesCleared, fullBoard);
        if (linesCleared >= 2 || fullBoard) {
            playSound(comboSound);
        }
    }
}

void GameManager::switchVersusTurn() {
    const int previousPlayer = activePlayer;
    activePlayer = 1 - activePlayer;

    if (!anyBlockCanBePlacedFor(activePlayer)) {
        endVersusGame(previousPlayer);
    }
}

bool GameManager::anyBlockCanBePlacedFor(int player) const {
    for (int i = 0; i < TRAY_SIZE; ++i) {
        if (!playerTrays[player][i]) {
            continue;
        }
        if (board->canPlaceAnyBlock(*playerTrays[player][i])) {
            return true;
        }
    }
    return false;
}

void GameManager::playSound(sf::Sound& sound) {
    if (sound.getBuffer()) {
        sound.play();
    }
}
