#include "HUD.h"
#include "util/ResourceUtils.h"

HUD::HUD() : currentScore(0), currentHighScore(0) {
    loadGameFont(font);

    // Top-left badge (score area)
    topBadge.setSize(sf::Vector2f(200.f, 56.f));
    topBadge.setPosition(20.f, 20.f);
    topBadge.setFillColor(sf::Color(83, 54, 70, 210));
    topBadge.setOutlineColor(sf::Color(123, 85, 103));
    topBadge.setOutlineThickness(2.f);

    // Settings button top-right
    settingsButton.setSize(sf::Vector2f(52.f, 52.f));
    settingsButton.setPosition(468.f, 24.f);
    settingsButton.setFillColor(sf::Color(83, 54, 70, 210));
    settingsButton.setOutlineColor(sf::Color(123, 85, 103));
    settingsButton.setOutlineThickness(2.f);

    // Score panel (centre)
    scorePanel.setSize(sf::Vector2f(200.f, 100.f));
    scorePanel.setPosition(170.f, 100.f);
    scorePanel.setFillColor(sf::Color(83, 54, 70, 150));
    scorePanel.setOutlineColor(sf::Color(123, 85, 103));
    scorePanel.setOutlineThickness(2.f);

    // Tray background
    trayPanel.setSize(sf::Vector2f(450.f, 134.f));
    trayPanel.setPosition(45.f, 700.f);
    trayPanel.setFillColor(sf::Color(101, 67, 85, 110));
    trayPanel.setOutlineColor(sf::Color(86, 56, 73));
    trayPanel.setOutlineThickness(2.f);

    // Three tray slots
    traySlotLeft.setSize(sf::Vector2f(110.f, 98.f));
    traySlotLeft.setPosition(60.f, 718.f);
    traySlotLeft.setFillColor(sf::Color(82, 51, 68, 180));
    traySlotLeft.setOutlineColor(sf::Color(115, 76, 95));
    traySlotLeft.setOutlineThickness(2.f);

    traySlotCenter = traySlotLeft;
    traySlotCenter.setPosition(215.f, 718.f);

    traySlotRight = traySlotLeft;
    traySlotRight.setPosition(370.f, 718.f);

    // Crown label
    crownText.setFont(font);
    crownText.setString("SCORE");
    crownText.setCharacterSize(14);
    crownText.setPosition(34.f, 28.f);
    crownText.setFillColor(sf::Color(200, 170, 185));

    settingsText.setFont(font);
    settingsText.setString("SET");
    settingsText.setCharacterSize(14);
    settingsText.setPosition(479.f, 38.f);
    settingsText.setFillColor(sf::Color::White);

    // Score value (large centre)
    scoreLabel.setFont(font);
    scoreLabel.setString("SCORE");
    scoreLabel.setCharacterSize(14);
    scoreLabel.setPosition(200.f, 108.f);
    scoreLabel.setFillColor(sf::Color(200, 170, 185));

    scoreValueText.setFont(font);
    scoreValueText.setString("0");
    scoreValueText.setCharacterSize(52);
    scoreValueText.setPosition(270.f, 118.f);
    scoreValueText.setFillColor(sf::Color::White);

    // High score
    highScoreLabel.setFont(font);
    highScoreLabel.setString("BEST");
    highScoreLabel.setCharacterSize(14);
    highScoreLabel.setPosition(34.f, 52.f);
    highScoreLabel.setFillColor(sf::Color(200, 170, 185));

    highScoreText.setFont(font);
    highScoreText.setString("0");
    highScoreText.setCharacterSize(22);
    highScoreText.setPosition(34.f, 66.f);
    highScoreText.setFillColor(sf::Color(255, 220, 80));
}

HUD::~HUD() {}

void HUD::render(sf::RenderWindow& window) {
    window.draw(topBadge);
    window.draw(settingsButton);
    window.draw(scorePanel);
    window.draw(trayPanel);
    window.draw(traySlotLeft);
    window.draw(traySlotCenter);
    window.draw(traySlotRight);
    window.draw(crownText);
    window.draw(settingsText);
    window.draw(scoreLabel);
    window.draw(scoreValueText);
    window.draw(highScoreLabel);
    window.draw(highScoreText);
}

void HUD::updateScore(int score, int highScore) {
    currentScore = score;
    currentHighScore = highScore;

    scoreValueText.setString(std::to_string(currentScore));
    const sf::FloatRect b = scoreValueText.getLocalBounds();
    scoreValueText.setPosition(270.f - b.width / 2.f, 120.f);

    highScoreText.setString(std::to_string(currentHighScore));
}
