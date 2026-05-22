#include "GameManager.h"
#include "core/GraphicsConfig.h"
#include "util/ResourceUtils.h"
#include <iostream>
#include <thread>
#include <atomic>

// Tray slot centres
static const sf::Vector2f TRAY_CENTERS[3] = {
    {115.f, 767.f},
    {GraphicsConfig::WINDOW_WIDTH / 2.f, 767.f},
    {425.f, 767.f},
};

// ── Constructor ───────────────────────────────────────────────────────────────
GameManager::GameManager(sf::RenderWindow& window)
    : window(window) {

    for (int i = 0; i < TRAY_SIZE; ++i)
        trayPositions[i] = TRAY_CENTERS[i];

    if (loadOptionalSoundBuffer(placeBuffer,    "assets/place.wav"))    placeSound.setBuffer(placeBuffer);
    if (loadOptionalSoundBuffer(clearBuffer,    "assets/clear.wav"))    clearSound.setBuffer(clearBuffer);
    if (loadOptionalSoundBuffer(comboBuffer,    "assets/combo.wav"))    comboSound.setBuffer(comboBuffer);
    if (loadOptionalSoundBuffer(gameOverBuffer, "assets/gameover.wav")) gameOverSound.setBuffer(gameOverBuffer);

    board = std::make_unique<Board>();
    menu  = std::make_unique<Menu>();
    hud   = std::make_unique<HUD>();

    // ── In-game back button (top-left) ────────────────────────────────────────
    loadGameFont(uiFont);
    backBtn.setSize({90.f, 36.f});
    backBtn.setPosition(8.f, 8.f);
    backBtn.setFillColor(sf::Color(60, 40, 60, 200));
    backBtn.setOutlineColor(sf::Color(160, 100, 160));
    backBtn.setOutlineThickness(1.5f);

    backBtnTxt.setFont(uiFont);
    backBtnTxt.setString("< MENU");
    backBtnTxt.setCharacterSize(16);
    backBtnTxt.setFillColor(sf::Color(220, 180, 220));
    sf::FloatRect tb = backBtnTxt.getLocalBounds();
    sf::FloatRect bb = backBtn.getGlobalBounds();
    backBtnTxt.setPosition(
        bb.left + (bb.width  - tb.width)  / 2.f - tb.left,
        bb.top  + (bb.height - tb.height) / 2.f - tb.top);

    setupNetworkCallbacks();
}

GameManager::~GameManager() {}

// ── Network callbacks ─────────────────────────────────────────────────────────
void GameManager::setupNetworkCallbacks() {
    netCallbacks.onRoomJoined = [this](int pid) {
        // Create the online game view immediately so player sees "Waiting..." screen
        if (!onlineGame)
            onlineGame = std::make_unique<OnlineGameMode>(window, netManager);
        currentState = GameState::ONLINE_PLAYING;
        if (selectScreen)
            selectScreen->setStatus("Joined as Player " + std::to_string(pid + 1) +
                                    ". Waiting for opponent...");
    };
    netCallbacks.onGameStart = [this](OnlineMode mode, int timeSec) {
        if (!onlineGame)
            onlineGame = std::make_unique<OnlineGameMode>(window, netManager);
        onlineGame->startGame(mode, timeSec);
        onlineGame->markGameStarted();
        currentState = GameState::ONLINE_PLAYING;
    };
    netCallbacks.onStateUpdate = [this](const OnlineGameState& state) {
        if (onlineGame) onlineGame->onStateUpdate(state);
    };
    netCallbacks.onGameOver = [this](int winnerId) {
        if (onlineGame) onlineGame->onGameOver(winnerId);
    };
    netCallbacks.onYourTurn = [this]() {
        if (onlineGame) onlineGame->onYourTurn();
    };
    netCallbacks.onDisconnected = [this]() {
        if (currentState == GameState::ONLINE_PLAYING ||
            currentState == GameState::ONLINE_SELECT) {
            currentState = GameState::MENU;
        }
    };
}

// ── Event handling ────────────────────────────────────────────────────────────
void GameManager::handleEvent(const sf::Event& event) {

    // ── Online select ─────────────────────────────────────────────────────────
    if (currentState == GameState::ONLINE_SELECT) {
        if (!selectScreen) return;
        SelectResult r = selectScreen->handleEvent(event);
        if (r == SelectResult::BACK) {
            netManager.disconnect();
            currentState = GameState::MENU;
        } else if (r == SelectResult::CONNECT) {
            connectAndJoin();
        }
        return;
    }

    // ── Online playing ────────────────────────────────────────────────────────
    if (currentState == GameState::ONLINE_PLAYING) {
        if (onlineGame) onlineGame->handleEvent(event);
        // Back to menu via ESC or in-game back button
        bool wantsBack = (event.type == sf::Event::KeyPressed &&
                          event.key.code == sf::Keyboard::Escape);
        if (!wantsBack && onlineGame) wantsBack = onlineGame->wantsBackToMenu();
        if (wantsBack) {
            netManager.disconnect();
            onlineGame.reset();
            currentState = GameState::MENU;
        }
        return;
    }

    // ── Main menu ─────────────────────────────────────────────────────────────
    if (currentState == GameState::MENU) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                               static_cast<float>(event.mouseButton.y));
            MenuAction action = menu->handleClick(mouse);
            switch (action) {
                case MenuAction::SOLO:   startSolo();        break;
                case MenuAction::VS_AI:  startVsAI();        break;
                case MenuAction::ONLINE: enterOnlineSelect(); break;
                case MenuAction::QUIT:   window.close();     break;
                default: break;
            }
        }
        return;
    }

    // ── Game over ─────────────────────────────────────────────────────────────
    if (currentState == GameState::GAME_OVER) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                               static_cast<float>(event.mouseButton.y));
            if (menu->isRestartClicked(mouse)) {
                // Restart same mode
                if (currentState == GameState::GAME_OVER) startSolo();
            } else if (menu->isBackToMenuClicked(mouse)) {
                currentState = GameState::MENU;
            }
        }
        return;
    }

    // ── Playing (solo or vs AI) ───────────────────────────────────────────────
    if (currentState == GameState::PLAYING_SOLO ||
        currentState == GameState::PLAYING_VS_AI) {

        // Back button
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                               static_cast<float>(event.mouseButton.y));
            if (isBackBtnClicked(mouse)) {
                currentState = GameState::MENU;
                return;
            }
        }
        handlePlayingEvent(event);
    }
}

void GameManager::handlePlayingEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
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
            const sf::Vector2i boardPos =
                board->getBoardPosition(trayBlocks[draggingIndex]->getPosition());

            if (board->canPlaceBlock(*trayBlocks[draggingIndex], boardPos)) {
                board->placeBlock(*trayBlocks[draggingIndex], boardPos);
                playSound(placeSound);
                checkForClears();
                trayBlocks[draggingIndex].reset();

                bool allEmpty = true;
                for (int i = 0; i < TRAY_SIZE; ++i)
                    if (trayBlocks[i]) { allEmpty = false; break; }
                if (allEmpty) refillTray();

                if (!anyBlockCanBePlaced()) endGame();
                else if (currentState == GameState::PLAYING_VS_AI) {
                    // Trigger AI after player places
                    aiWaiting = true;
                    aiClock.restart();
                }
            } else {
                // Snap back
                const sf::Vector2f sz = trayBlocks[draggingIndex]->getSize();
                trayBlocks[draggingIndex]->setPosition(
                    trayPositions[draggingIndex] -
                    sf::Vector2f(sz.x / 2.f, sz.y / 2.f));
            }
            draggingIndex = -1;
        }
    }
    else if (event.type == sf::Event::MouseMoved &&
             draggingIndex >= 0 && trayBlocks[draggingIndex]) {
        trayBlocks[draggingIndex]->setPosition(sf::Vector2f(
            static_cast<float>(event.mouseMove.x - dragOffset.x),
            static_cast<float>(event.mouseMove.y - dragOffset.y)));
    }
}

// ── Update ────────────────────────────────────────────────────────────────────
void GameManager::update() {
    if (currentState == GameState::PLAYING_SOLO ||
        currentState == GameState::PLAYING_VS_AI) {
        board->update();
        hud->updateScore(score, highScore);
        if (currentState == GameState::PLAYING_VS_AI)
            updateAI();
    }

    if (currentState == GameState::ONLINE_SELECT ||
        currentState == GameState::ONLINE_PLAYING) {
        netManager.pollEvents(netCallbacks);
    }
    if (currentState == GameState::ONLINE_PLAYING && onlineGame)
        onlineGame->update(1.f / 60.f);
}

void GameManager::updateAI() {
    if (!aiWaiting) return;
    if (aiClock.getElapsedTime().asSeconds() < aiThinkDelay) return;
    aiWaiting = false;

    AIPlayer::Move move = AIPlayer::findBestMove(*board, trayBlocks);
    if (!move.valid) {
        endGame();
        return;
    }

    board->placeBlock(*trayBlocks[move.trayIndex], move.boardPos);
    playSound(placeSound);
    checkForClears();
    trayBlocks[move.trayIndex].reset();

    bool allEmpty = true;
    for (int i = 0; i < TRAY_SIZE; ++i)
        if (trayBlocks[i]) { allEmpty = false; break; }
    if (allEmpty) refillTray();

    if (!anyBlockCanBePlaced()) endGame();
}

// ── Render ────────────────────────────────────────────────────────────────────
void GameManager::render() {
    if (currentState == GameState::ONLINE_SELECT) {
        if (selectScreen) selectScreen->render(window);
        return;
    }
    if (currentState == GameState::ONLINE_PLAYING) {
        if (onlineGame) onlineGame->render();
        return;
    }

    if (currentState == GameState::MENU) {
        window.clear(sf::Color(28, 28, 52));
        menu->render(window);
        window.display();
        return;
    }

    window.clear(sf::Color(12, 8, 32));   // modern dark navy

    if (currentState == GameState::PLAYING_SOLO ||
        currentState == GameState::PLAYING_VS_AI) {

        board->render(window);

        // Ghost preview
        if (draggingIndex >= 0 && trayBlocks[draggingIndex]) {
            const sf::Vector2i boardPos =
                board->getBoardPosition(trayBlocks[draggingIndex]->getPosition());
            if (board->canPlaceBlock(*trayBlocks[draggingIndex], boardPos))
                board->renderGhostCells(window, *trayBlocks[draggingIndex], boardPos);
        }

        drawTray();
        hud->render(window);

        // VS AI label
        if (currentState == GameState::PLAYING_VS_AI) {
            sf::Text aiLabel;
            aiLabel.setFont(uiFont);
            aiLabel.setString("VS AI");
            aiLabel.setCharacterSize(14);
            aiLabel.setFillColor(sf::Color(255, 180, 80));
            aiLabel.setPosition(110.f, 18.f);
            window.draw(aiLabel);
        }

        drawBackButton();

        if (draggingIndex >= 0 && trayBlocks[draggingIndex])
            trayBlocks[draggingIndex]->render(window);

    } else if (currentState == GameState::GAME_OVER) {
        board->render(window);
        drawTray();
        hud->render(window);
        menu->renderGameOver(window, score, highScore);
    }

    window.display();
}

// ── Game flow ─────────────────────────────────────────────────────────────────
void GameManager::startSolo() {
    currentState = GameState::PLAYING_SOLO;
    score = 0; combo = 0;
    board->reset();
    refillTray();
    draggingIndex = -1;
}

void GameManager::startVsAI() {
    currentState = GameState::PLAYING_VS_AI;
    score = 0; combo = 0;
    aiWaiting = false;
    board->reset();
    refillTray();
    draggingIndex = -1;
}

void GameManager::endGame() {
    if (score > highScore) highScore = score;
    currentState = GameState::GAME_OVER;
    playSound(gameOverSound);
}

void GameManager::refillTray() {
    for (int i = 0; i < TRAY_SIZE; ++i) {
        trayBlocks[i] = std::make_unique<Block>();
        const sf::Vector2f sz = trayBlocks[i]->getSize();
        trayBlocks[i]->setPosition(
            trayPositions[i] - sf::Vector2f(sz.x / 2.f, sz.y / 2.f));
    }
}

void GameManager::drawTray() {
    for (int i = 0; i < TRAY_SIZE; ++i) {
        if (!trayBlocks[i] || i == draggingIndex) continue;
        trayBlocks[i]->renderPreview(window, trayPositions[i]);
    }
}

void GameManager::checkForClears() {
    const int lines = board->clearFullLines();
    if (lines > 0) {
        score += lines * 10 * (combo + 1);
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

void GameManager::drawBackButton() {
    window.draw(backBtn);
    window.draw(backBtnTxt);
}

bool GameManager::isBackBtnClicked(sf::Vector2f p) const {
    return backBtn.getGlobalBounds().contains(p);
}

// ── Online ────────────────────────────────────────────────────────────────────
void GameManager::enterOnlineSelect() {
    selectScreen = std::make_unique<OnlineGameSelectScreen>();
    currentState = GameState::ONLINE_SELECT;
}

void GameManager::connectAndJoin() {
    if (!selectScreen) return;
    if (connecting_) return;   // already trying

    const std::string host   = selectScreen->getHost();
    const unsigned short port = selectScreen->getPort();
    const std::string roomId  = selectScreen->getRoomId();
    const OnlineMode  mode    = selectScreen->getMode();
    const TimePreset  preset  = selectScreen->getTimePreset();

    selectScreen->setStatus("Connecting to " + host + ":" + std::to_string(port) + "...");
    connecting_ = true;

    // Join previous connect thread if any
    if (connectThread_.joinable()) connectThread_.join();

    // Connect on background thread — never blocks main thread
    connectThread_ = std::thread([this, host, port, roomId, mode, preset]() {
        bool ok = netManager.connect(host, port);
        // Marshal result back via a flag; main thread picks it up in update()
        if (!ok) {
            connecting_ = false;
            // Post failure message via selectScreen if still alive
            if (selectScreen)
                selectScreen->setStatus("Connection failed. Is the server running?");
            return;
        }
        // Send JOIN immediately from this thread (enqueue is thread-safe)
        netManager.joinRoom(roomId, mode, preset);
        connecting_ = false;
    });
}
