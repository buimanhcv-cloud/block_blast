#include "Menu.h"
#include "core/GraphicsConfig.h"
#include "util/ResourceUtils.h"

namespace {
constexpr float SCREEN_CENTER_X = GraphicsConfig::WINDOW_WIDTH / 2.f;

void centerTextAt(sf::Text& text, float x, float y) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top);
    text.setPosition(x, y);
}

void centerTextInRect(sf::Text& text, const sf::FloatRect& area) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    text.setPosition(area.left + area.width / 2.f, area.top + area.height / 2.f);
}
} // namespace

Menu::Menu() {
    loadGameFont(font);

    background.setSize(sf::Vector2f(
        static_cast<float>(GraphicsConfig::WINDOW_WIDTH),
        static_cast<float>(GraphicsConfig::WINDOW_HEIGHT)));
    background.setFillColor(sf::Color(37, 119, 224));

    logoPanel.setSize(sf::Vector2f(390.f, 220.f));
    logoPanel.setPosition(75.f, 120.f);
    logoPanel.setFillColor(sf::Color::Transparent);

    classicButton.setSize(sf::Vector2f(360.f, 64.f));
    classicButton.setPosition(90.f, 560.f);
    classicButton.setFillColor(sf::Color(43, 214, 190));
    classicButton.setOutlineColor(sf::Color(19, 153, 134));
    classicButton.setOutlineThickness(3.f);

    versusButton.setSize(sf::Vector2f(360.f, 64.f));
    versusButton.setPosition(90.f, 640.f);

    duelMilestonesButton.setSize(sf::Vector2f(360.f, 64.f));
    duelMilestonesButton.setPosition(90.f, 720.f);
    duelMilestonesButton.setFillColor(sf::Color(120, 180, 255));
    duelMilestonesButton.setOutlineColor(sf::Color(70, 120, 200));
    duelMilestonesButton.setOutlineThickness(3.f);

    onlineButton.setSize(sf::Vector2f(360.f, 64.f));
    onlineButton.setPosition(90.f, 800.f);
    onlineButton.setFillColor(sf::Color(255, 120, 180));
    onlineButton.setOutlineColor(sf::Color(200, 70, 130));
    onlineButton.setOutlineThickness(3.f);

    versusButton.setFillColor(sf::Color(255, 140, 90));
    versusButton.setOutlineColor(sf::Color(200, 90, 40));
    versusButton.setOutlineThickness(3.f);

    overlay.setSize(sf::Vector2f(
        static_cast<float>(GraphicsConfig::WINDOW_WIDTH),
        static_cast<float>(GraphicsConfig::WINDOW_HEIGHT)));
    overlay.setFillColor(sf::Color(12, 5, 12, 180));

    gameOverPanel.setSize(sf::Vector2f(390.f, 310.f));
    gameOverPanel.setPosition(75.f, 270.f);
    gameOverPanel.setFillColor(sf::Color(111, 79, 98, 245));
    gameOverPanel.setOutlineColor(sf::Color(199, 166, 188));
    gameOverPanel.setOutlineThickness(3.f);

    restartButton.setSize(sf::Vector2f(210.f, 64.f));
    restartButton.setPosition(165.f, 498.f);
    restartButton.setFillColor(sf::Color(77, 225, 51));
    restartButton.setOutlineColor(sf::Color(40, 159, 18));
    restartButton.setOutlineThickness(3.f);

    titleText.setFont(font);
    titleText.setString(utf8Text("BLOCK"));
    titleText.setCharacterSize(82);
    titleText.setFillColor(sf::Color(255, 189, 51));
    centerTextAt(titleText, SCREEN_CENTER_X, 150.f);

    subtitleText.setFont(font);
    subtitleText.setString(utf8Text("BLAST"));
    subtitleText.setCharacterSize(78);
    subtitleText.setFillColor(sf::Color(65, 234, 255));
    centerTextAt(subtitleText, SCREEN_CENTER_X, 235.f);

    classicText.setFont(font);
    classicText.setString(utf8Text("Classic"));
    classicText.setCharacterSize(34);
    classicText.setFillColor(sf::Color::White);
    centerTextInRect(classicText, sf::FloatRect(90.f, 560.f, 360.f, 64.f));

    versusText.setFont(font);
    versusText.setString(utf8Text("1v1 Đối kháng"));
    versusText.setCharacterSize(30);
    versusText.setFillColor(sf::Color::White);
    centerTextInRect(versusText, sf::FloatRect(90.f, 640.f, 360.f, 64.f));

    duelMilestonesText.setFont(font);
    duelMilestonesText.setString(utf8Text("Mốc đối chiến"));
    duelMilestonesText.setCharacterSize(30);
    duelMilestonesText.setFillColor(sf::Color::White);
    centerTextInRect(duelMilestonesText, sf::FloatRect(90.f, 720.f, 360.f, 64.f));

    onlineText.setFont(font);
    onlineText.setString(utf8Text("Online Đối chiến"));
    onlineText.setCharacterSize(28);
    onlineText.setFillColor(sf::Color::White);
    centerTextInRect(onlineText, sf::FloatRect(90.f, 800.f, 360.f, 64.f));

    gameOverText.setFont(font);
    gameOverText.setString(utf8Text("Game Over"));
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color(255, 240, 248));
    centerTextAt(gameOverText, SCREEN_CENTER_X, 290.f);

    scoreText.setFont(font);
    scoreText.setCharacterSize(34);
    scoreText.setFillColor(sf::Color(255, 246, 250));

    highScoreLabelText.setFont(font);
    highScoreLabelText.setString(utf8Text("Best:"));
    highScoreLabelText.setCharacterSize(22);
    highScoreLabelText.setFillColor(sf::Color(200, 170, 185));

    highScoreText.setFont(font);
    highScoreText.setCharacterSize(28);
    highScoreText.setFillColor(sf::Color(255, 220, 80));

    rankText.setFont(font);
    rankText.setCharacterSize(28);
    rankText.setFillColor(sf::Color(120, 255, 200));

    newBestText.setFont(font);
    newBestText.setString(utf8Text("NEW BEST!"));
    newBestText.setCharacterSize(22);
    newBestText.setFillColor(sf::Color(255, 220, 80));
    centerTextAt(newBestText, SCREEN_CENTER_X, 435.f);

    restartText.setFont(font);
    restartText.setString(utf8Text("Chơi lại"));
    restartText.setCharacterSize(32);
    restartText.setFillColor(sf::Color::White);
    centerTextInRect(restartText, sf::FloatRect(165.f, 498.f, 210.f, 64.f));

    versusResultText.setFont(font);
    versusResultText.setCharacterSize(40);
    versusResultText.setFillColor(sf::Color(255, 240, 248));
}

Menu::~Menu() {}

void Menu::render(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(logoPanel);
    window.draw(classicButton);
    window.draw(versusButton);
    window.draw(duelMilestonesButton);
    window.draw(onlineButton);
    window.draw(titleText);
    window.draw(subtitleText);
    window.draw(classicText);
    window.draw(versusText);
    window.draw(duelMilestonesText);
    window.draw(onlineText);
}

void Menu::renderGameOver(sf::RenderWindow& window, int score, int highScore, const std::string& rank, bool newBest) {
    gameOverText.setString(utf8Text("Game Over"));
    centerTextAt(gameOverText, SCREEN_CENTER_X, 290.f);

    window.draw(overlay);
    window.draw(gameOverPanel);
    window.draw(gameOverText);

    scoreText.setString(utf8Text("Score: " + std::to_string(score)));
    centerTextAt(scoreText, SCREEN_CENTER_X, 358.f);
    window.draw(scoreText);

    rankText.setString(utf8Text(rank));
    centerTextAt(rankText, SCREEN_CENTER_X, 400.f);
    window.draw(rankText);

    highScoreText.setString(utf8Text(std::to_string(highScore)));
    centerTextAt(highScoreLabelText, SCREEN_CENTER_X - 40.f, 438.f);
    centerTextAt(highScoreText, SCREEN_CENTER_X + 40.f, 435.f);
    window.draw(highScoreLabelText);
    window.draw(highScoreText);

    if (newBest) {
        window.draw(newBestText);
    }

    restartText.setString(utf8Text("Chơi lại"));
    centerTextInRect(restartText, sf::FloatRect(165.f, 498.f, 210.f, 64.f));
    window.draw(restartButton);
    window.draw(restartText);
}

void Menu::renderVersusEnd(sf::RenderWindow& window, int winnerPlayer) {
    window.draw(overlay);
    window.draw(gameOverPanel);

    gameOverText.setString(utf8Text("Kết thúc"));
    centerTextAt(gameOverText, SCREEN_CENTER_X, 290.f);
    window.draw(gameOverText);

    const char* winnerLine = winnerPlayer == 0 ? "Người 1 thắng!" : "Người 2 thắng!";
    versusResultText.setString(utf8Text(winnerLine));
    centerTextAt(versusResultText, SCREEN_CENTER_X, 370.f);
    window.draw(versusResultText);

    restartText.setString(utf8Text("Chơi lại"));
    centerTextInRect(restartText, sf::FloatRect(165.f, 498.f, 210.f, 64.f));
    window.draw(restartButton);
    window.draw(restartText);
}

bool Menu::isClassicClicked(sf::Vector2f mousePos) const {
    return classicButton.getGlobalBounds().contains(mousePos);
}

bool Menu::isVersusClicked(sf::Vector2f mousePos) const {
    return versusButton.getGlobalBounds().contains(mousePos);
}

bool Menu::isDuelMilestonesClicked(sf::Vector2f mousePos) const {
    return duelMilestonesButton.getGlobalBounds().contains(mousePos);
}

bool Menu::isOnlineClicked(sf::Vector2f mousePos) const {
    return onlineButton.getGlobalBounds().contains(mousePos);
}

bool Menu::isRestartClicked(sf::Vector2f mousePos) const {
    return restartButton.getGlobalBounds().contains(mousePos);
}
