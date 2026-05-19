#include "OnlineGameSelectScreen.h"
#include "util/ResourceUtils.h"
#include <iostream>

OnlineGameSelectScreen::OnlineGameSelectScreen()
    : currentState(SelectState::GAME_TYPE), selectedGameType(OnlineGameType::REALTIME),
      selectedTimeMode(TimeMode::NORMAL), serverIP("127.0.0.1"), playerName("Player"),
      confirmed(false), gameTypeSelection(0), timeModeSelection(1), cursorPos(0),
      elapsedTime(0.0f) {
    loadFont();
}

OnlineGameSelectScreen::~OnlineGameSelectScreen() {
}

void OnlineGameSelectScreen::loadFont() {
    ResourceUtils::loadFont(font);
}

void OnlineGameSelectScreen::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (currentState) {
            case SelectState::GAME_TYPE:
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Left) {
                    gameTypeSelection = (gameTypeSelection - 1 + 2) % 2;
                } else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::Right) {
                    gameTypeSelection = (gameTypeSelection + 1) % 2;
                } else if (event.key.code == sf::Keyboard::Return) {
                    selectedGameType = (gameTypeSelection == 0) ? OnlineGameType::REALTIME : OnlineGameType::TURN_BASED;
                    if (selectedGameType == OnlineGameType::TURN_BASED) {
                        currentState = SelectState::TIME_MODE;
                    } else {
                        currentState = SelectState::ENTERING_IP;
                    }
                }
                break;

            case SelectState::TIME_MODE:
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Left) {
                    timeModeSelection = (timeModeSelection - 1 + 3) % 3;
                } else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::Right) {
                    timeModeSelection = (timeModeSelection + 1) % 3;
                } else if (event.key.code == sf::Keyboard::Return) {
                    selectedTimeMode = (timeModeSelection == 0) ? TimeMode::FAST :
                                       (timeModeSelection == 1) ? TimeMode::NORMAL : TimeMode::LONG;
                    currentState = SelectState::ENTERING_IP;
                }
                break;

            case SelectState::ENTERING_IP:
                if (event.key.code == sf::Keyboard::Return) {
                    currentState = SelectState::CONNECTING;
                    confirmed = true;
                } else if (event.key.code == sf::Keyboard::BackSpace && !serverIP.empty()) {
                    serverIP.pop_back();
                }
                break;

            case SelectState::WAITING:
                if (event.key.code == sf::Keyboard::Escape) {
                    currentState = SelectState::GAME_TYPE;
                    confirmed = false;
                }
                break;

            default:
                break;
        }
    } else if (event.type == sf::Event::TextEntered) {
        if (currentState == SelectState::ENTERING_IP) {
            char c = static_cast<char>(event.text.unicode);
            if ((c >= '0' && c <= '9') || c == '.') {
                if (serverIP.length() < 20) {
                    serverIP += c;
                }
            }
        }
    }
}

void OnlineGameSelectScreen::update(float deltaTime) {
    elapsedTime += deltaTime;
}

void OnlineGameSelectScreen::render(sf::RenderWindow& window) {
    window.clear(sf::Color(20, 20, 40));

    switch (currentState) {
        case SelectState::GAME_TYPE:
            drawGameTypeMenu(window);
            break;
        case SelectState::TIME_MODE:
            drawTimeModeMenu(window);
            break;
        case SelectState::ENTERING_IP:
            drawIPInputMenu(window);
            break;
        case SelectState::WAITING:
            drawWaitingScreen(window);
            break;
        default:
            break;
    }
}

void OnlineGameSelectScreen::drawGameTypeMenu(sf::RenderWindow& window) {
    const float centerX = window.getSize().x / 2.0f;
    const float centerY = window.getSize().y / 2.0f;

    sf::Text title("Select Game Mode", font, 40);
    title.setPosition(centerX - title.getGlobalBounds().width / 2, 50);
    title.setFillColor(sf::Color::White);
    window.draw(title);

    // Realtime option
    sf::Text realtimeText("Realtime (Both play simultaneously)", font, 24);
    realtimeText.setPosition(centerX - 150, centerY - 50);
    realtimeText.setFillColor(gameTypeSelection == 0 ? sf::Color::Yellow : sf::Color::White);
    window.draw(realtimeText);

    // Turn-based option
    sf::Text turnBasedText("Turn-based (Chess Clock)", font, 24);
    turnBasedText.setPosition(centerX - 150, centerY + 20);
    turnBasedText.setFillColor(gameTypeSelection == 1 ? sf::Color::Yellow : sf::Color::White);
    window.draw(turnBasedText);

    sf::Text helpText("UP/DOWN to select, ENTER to confirm", font, 16);
    helpText.setPosition(centerX - 200, window.getSize().y - 50);
    helpText.setFillColor(sf::Color::Cyan);
    window.draw(helpText);
}

void OnlineGameSelectScreen::drawTimeModeMenu(sf::RenderWindow& window) {
    const float centerX = window.getSize().x / 2.0f;
    const float centerY = window.getSize().y / 2.0f;

    sf::Text title("Select Time Limit", font, 40);
    title.setPosition(centerX - title.getGlobalBounds().width / 2, 50);
    title.setFillColor(sf::Color::White);
    window.draw(title);

    const char* timeModes[] = { "Fast (1 minute)", "Normal (3 minutes)", "Long (5 minutes)" };
    for (int i = 0; i < 3; ++i) {
        sf::Text modeText(timeModes[i], font, 24);
        modeText.setPosition(centerX - 120, centerY - 60 + i * 80);
        modeText.setFillColor(timeModeSelection == i ? sf::Color::Yellow : sf::Color::White);
        window.draw(modeText);
    }

    sf::Text helpText("UP/DOWN to select, ENTER to confirm", font, 16);
    helpText.setPosition(centerX - 200, window.getSize().y - 50);
    helpText.setFillColor(sf::Color::Cyan);
    window.draw(helpText);
}

void OnlineGameSelectScreen::drawIPInputMenu(sf::RenderWindow& window) {
    const float centerX = window.getSize().x / 2.0f;
    const float centerY = window.getSize().y / 2.0f;

    sf::Text title("Enter Server IP", font, 40);
    title.setPosition(centerX - title.getGlobalBounds().width / 2, 50);
    title.setFillColor(sf::Color::White);
    window.draw(title);

    sf::Text ipText("IP: " + serverIP, font, 28);
    ipText.setPosition(centerX - 150, centerY - 30);
    ipText.setFillColor(sf::Color::Yellow);
    window.draw(ipText);

    sf::Text instructionText("Default: 127.0.0.1 (local)", font, 16);
    instructionText.setPosition(centerX - 150, centerY + 20);
    instructionText.setFillColor(sf::Color::Cyan);
    window.draw(instructionText);

    sf::Text helpText("Type IP (0-9, .), ENTER to connect", font, 16);
    helpText.setPosition(centerX - 200, window.getSize().y - 50);
    helpText.setFillColor(sf::Color::Green);
    window.draw(helpText);
}

void OnlineGameSelectScreen::drawWaitingScreen(sf::RenderWindow& window) {
    const float centerX = window.getSize().x / 2.0f;
    const float centerY = window.getSize().y / 2.0f;

    sf::Text waitText("Waiting for opponent...", font, 36);
    waitText.setPosition(centerX - waitText.getGlobalBounds().width / 2, centerY - 50);
    waitText.setFillColor(sf::Color::Yellow);
    window.draw(waitText);

    // Animated dots
    int dotCount = 1 + static_cast<int>(elapsedTime * 2) % 3;
    std::string dots(dotCount, '.');
    sf::Text dotsText(dots, font, 32);
    dotsText.setPosition(centerX + 200, centerY - 50);
    dotsText.setFillColor(sf::Color::Yellow);
    window.draw(dotsText);
}

void OnlineGameSelectScreen::resetSelection() {
    currentState = SelectState::GAME_TYPE;
    gameTypeSelection = 0;
    timeModeSelection = 1;
    serverIP = "127.0.0.1";
    confirmed = false;
}
