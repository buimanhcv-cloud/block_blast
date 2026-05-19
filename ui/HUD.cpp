#include "HUD.h"
#include "util/ResourceUtils.h"

HUD::HUD() : versusMode(false) {
    loadGameFont(font);

    menuButton.setSize(sf::Vector2f(72.f, 52.f));
    menuButton.setPosition(448.f, 24.f);
    menuButton.setFillColor(sf::Color(83, 54, 70, 210));
    menuButton.setOutlineColor(sf::Color(123, 85, 103));
    menuButton.setOutlineThickness(2.f);

    scorePanel.setSize(sf::Vector2f(200.f, 100.f));
    scorePanel.setPosition(170.f, 100.f);
    scorePanel.setFillColor(sf::Color(83, 54, 70, 150));
    scorePanel.setOutlineColor(sf::Color(123, 85, 103));
    scorePanel.setOutlineThickness(2.f);

    turnPanel.setSize(sf::Vector2f(240.f, 72.f));
    turnPanel.setPosition(150.f, 114.f);
    turnPanel.setFillColor(sf::Color(83, 54, 70, 180));
    turnPanel.setOutlineColor(sf::Color(123, 85, 103));
    turnPanel.setOutlineThickness(2.f);

    playerIndicator.setSize(sf::Vector2f(28.f, 28.f));
    playerIndicator.setPosition(162.f, 136.f);
    playerIndicator.setOutlineThickness(0.f);

    menuText.setFont(font);
    menuText.setString(utf8Text("MENU"));
    menuText.setCharacterSize(14);
    menuText.setFillColor(sf::Color::White);
    menuText.setPosition(462.f, 38.f);

    scoreLabel.setFont(font);
    scoreLabel.setString(utf8Text("SCORE"));
    scoreLabel.setCharacterSize(14);
    scoreLabel.setPosition(200.f, 108.f);
    scoreLabel.setFillColor(sf::Color(200, 170, 185));

    scoreValueText.setFont(font);
    scoreValueText.setString(utf8Text("0"));
    scoreValueText.setCharacterSize(52);
    scoreValueText.setPosition(270.f, 118.f);
    scoreValueText.setFillColor(sf::Color::White);

    turnText.setFont(font);
    turnText.setCharacterSize(28);
    turnText.setFillColor(sf::Color::White);
    turnText.setPosition(200.f, 128.f);
}

HUD::~HUD() {}

void HUD::render(sf::RenderWindow& window) {
    window.draw(menuButton);
    window.draw(menuText);

    if (versusMode) {
        window.draw(turnPanel);
        window.draw(playerIndicator);
        window.draw(turnText);
    } else {
        window.draw(scorePanel);
        window.draw(scoreLabel);
        window.draw(scoreValueText);
    }
}

void HUD::updateScore(int score) {
    scoreValueText.setString(utf8Text(std::to_string(score)));
    const sf::FloatRect b = scoreValueText.getLocalBounds();
    scoreValueText.setPosition(270.f - b.width / 2.f, 120.f);
}

void HUD::setVersusMode(bool enabled) {
    versusMode = enabled;
}

void HUD::setActivePlayer(int playerIndex) {
    const bool playerOne = playerIndex == 0;
    turnText.setString(utf8Text(playerOne ? "Lượt: Người 1" : "Lượt: Người 2"));
    const sf::FloatRect tb = turnText.getLocalBounds();
    turnText.setPosition(270.f - tb.width / 2.f, 128.f);

    playerIndicator.setFillColor(
        playerOne ? sf::Color(92, 212, 255) : sf::Color(255, 140, 90));
}

bool HUD::isMenuClicked(sf::Vector2f mousePos) const {
    return menuButton.getGlobalBounds().contains(mousePos);
}
