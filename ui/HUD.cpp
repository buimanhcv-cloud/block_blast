#include "HUD.h"
#include "util/ResourceUtils.h"

// ── Modern Gradient HUD ───────────────────────────────────────────────────────
// Colour palette:
//   Background panels : deep navy  (15, 12, 40, 210)
//   Panel border      : electric blue (60, 120, 255)
//   Score value       : bright cyan (0, 230, 255)
//   Best value        : gold  (255, 210, 60)
//   Labels            : soft lavender (160, 170, 220)
//   Tray panel        : dark purple (20, 15, 50, 180)
//   Tray slot         : midnight (30, 20, 60, 200) + blue border

HUD::HUD() : currentScore(0), currentHighScore(0) {
    loadGameFont(font);

    // ── Top badge (score + best, top-left) ───────────────────────────────────
    topBadge.setSize(sf::Vector2f(210.f, 70.f));
    topBadge.setPosition(16.f, 16.f);
    topBadge.setFillColor(sf::Color(15, 12, 40, 220));
    topBadge.setOutlineColor(sf::Color(60, 120, 255, 200));
    topBadge.setOutlineThickness(2.f);

    // ── Settings button (top-right) ───────────────────────────────────────────
    settingsButton.setSize(sf::Vector2f(52.f, 52.f));
    settingsButton.setPosition(472.f, 20.f);
    settingsButton.setFillColor(sf::Color(15, 12, 40, 220));
    settingsButton.setOutlineColor(sf::Color(60, 120, 255, 180));
    settingsButton.setOutlineThickness(2.f);

    // ── Score panel (centre, below board header) ──────────────────────────────
    scorePanel.setSize(sf::Vector2f(220.f, 90.f));
    scorePanel.setPosition(160.f, 96.f);
    scorePanel.setFillColor(sf::Color(10, 8, 35, 200));
    scorePanel.setOutlineColor(sf::Color(0, 180, 255, 160));
    scorePanel.setOutlineThickness(2.f);

    // ── Tray background ───────────────────────────────────────────────────────
    trayPanel.setSize(sf::Vector2f(450.f, 138.f));
    trayPanel.setPosition(45.f, 698.f);
    trayPanel.setFillColor(sf::Color(20, 15, 50, 190));
    trayPanel.setOutlineColor(sf::Color(60, 80, 180, 160));
    trayPanel.setOutlineThickness(2.f);

    // ── Three tray slots ──────────────────────────────────────────────────────
    traySlotLeft.setSize(sf::Vector2f(112.f, 100.f));
    traySlotLeft.setPosition(58.f, 716.f);
    traySlotLeft.setFillColor(sf::Color(30, 20, 60, 210));
    traySlotLeft.setOutlineColor(sf::Color(80, 100, 220, 180));
    traySlotLeft.setOutlineThickness(1.5f);

    traySlotCenter = traySlotLeft;
    traySlotCenter.setPosition(214.f, 716.f);

    traySlotRight = traySlotLeft;
    traySlotRight.setPosition(370.f, 716.f);

    // ── Labels ────────────────────────────────────────────────────────────────
    crownText.setFont(font);
    crownText.setString("SCORE");
    crownText.setCharacterSize(13);
    crownText.setPosition(28.f, 22.f);
    crownText.setFillColor(sf::Color(160, 170, 220));
    crownText.setStyle(sf::Text::Bold);

    settingsText.setFont(font);
    settingsText.setString("SET");
    settingsText.setCharacterSize(13);
    settingsText.setPosition(482.f, 36.f);
    settingsText.setFillColor(sf::Color(160, 170, 220));

    // Score value (large, top-left badge)
    scoreLabel.setFont(font);
    scoreLabel.setString("SCORE");
    scoreLabel.setCharacterSize(13);
    scoreLabel.setPosition(178.f, 102.f);
    scoreLabel.setFillColor(sf::Color(160, 170, 220));
    scoreLabel.setStyle(sf::Text::Bold);

    scoreValueText.setFont(font);
    scoreValueText.setString("0");
    scoreValueText.setCharacterSize(48);
    scoreValueText.setPosition(270.f, 112.f);
    scoreValueText.setFillColor(sf::Color(0, 230, 255));
    scoreValueText.setStyle(sf::Text::Bold);

    // Best score (top-left badge, second line)
    highScoreLabel.setFont(font);
    highScoreLabel.setString("BEST");
    highScoreLabel.setCharacterSize(13);
    highScoreLabel.setPosition(28.f, 48.f);
    highScoreLabel.setFillColor(sf::Color(160, 170, 220));
    highScoreLabel.setStyle(sf::Text::Bold);

    highScoreText.setFont(font);
    highScoreText.setString("0");
    highScoreText.setCharacterSize(22);
    highScoreText.setPosition(28.f, 62.f);
    highScoreText.setFillColor(sf::Color(255, 210, 60));
    highScoreText.setStyle(sf::Text::Bold);
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
    currentScore     = score;
    currentHighScore = highScore;

    scoreValueText.setString(std::to_string(currentScore));
    const sf::FloatRect b = scoreValueText.getLocalBounds();
    // Centre inside scorePanel (x=160, w=220 → centre at 270)
    scoreValueText.setPosition(270.f - b.width / 2.f, 114.f);

    highScoreText.setString(std::to_string(currentHighScore));
}
