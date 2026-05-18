#include "Menu.h"
#include "core/GraphicsConfig.h"
#include "util/ResourceUtils.h"

Menu::Menu() {
    loadGameFont(font);

    background.setSize(sf::Vector2f(
        static_cast<float>(GraphicsConfig::WINDOW_WIDTH),
        static_cast<float>(GraphicsConfig::WINDOW_HEIGHT)));
    background.setFillColor(sf::Color(37, 119, 224));

    logoPanel.setSize(sf::Vector2f(390.f, 220.f));
    logoPanel.setPosition(75.f, 120.f);
    logoPanel.setFillColor(sf::Color::Transparent);

    classicButton.setSize(sf::Vector2f(360.f, 84.f));
    classicButton.setPosition(90.f, 680.f);
    classicButton.setFillColor(sf::Color(43, 214, 190));
    classicButton.setOutlineColor(sf::Color(19, 153, 134));
    classicButton.setOutlineThickness(3.f);

    startButton = classicButton;

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

    // Menu title
    titleText.setFont(font);
    titleText.setString("BLOCK");
    titleText.setCharacterSize(82);
    titleText.setPosition(115.f, 150.f);
    titleText.setFillColor(sf::Color(255, 189, 51));

    subtitleText.setFont(font);
    subtitleText.setString("BLAST");
    subtitleText.setCharacterSize(78);
    subtitleText.setPosition(125.f, 235.f);
    subtitleText.setFillColor(sf::Color(65, 234, 255));

    classicText.setFont(font);
    classicText.setString("Play Classic");
    classicText.setCharacterSize(36);
    classicText.setPosition(160.f, 700.f);
    classicText.setFillColor(sf::Color::White);

    // Game over texts
    gameOverText.setFont(font);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(50);
    gameOverText.setPosition(145.f, 290.f);
    gameOverText.setFillColor(sf::Color(255, 240, 248));

    scoreText.setFont(font);
    scoreText.setCharacterSize(34);
    scoreText.setFillColor(sf::Color(255, 246, 250));

    highScoreLabelText.setFont(font);
    highScoreLabelText.setString("Best:");
    highScoreLabelText.setCharacterSize(22);
    highScoreLabelText.setFillColor(sf::Color(200, 170, 185));

    highScoreText.setFont(font);
    highScoreText.setCharacterSize(28);
    highScoreText.setFillColor(sf::Color(255, 220, 80));

    newBestText.setFont(font);
    newBestText.setString("NEW BEST!");
    newBestText.setCharacterSize(22);
    newBestText.setFillColor(sf::Color(255, 220, 80));
    newBestText.setPosition(210.f, 435.f);

    restartText.setFont(font);
    restartText.setString("Play Again");
    restartText.setCharacterSize(32);
    restartText.setPosition(195.f, 512.f);
    restartText.setFillColor(sf::Color::White);
}

Menu::~Menu() {}

void Menu::render(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(logoPanel);
    window.draw(classicButton);
    window.draw(titleText);
    window.draw(subtitleText);
    window.draw(classicText);
}

void Menu::renderGameOver(sf::RenderWindow& window, int score, int highScore) {
    window.draw(overlay);
    window.draw(gameOverPanel);
    window.draw(gameOverText);

    // Score
    scoreText.setString("Score: " + std::to_string(score));
    const sf::FloatRect sb = scoreText.getLocalBounds();
    scoreText.setPosition(270.f - sb.width / 2.f, 358.f);
    window.draw(scoreText);

    // Best score
    highScoreText.setString(std::to_string(highScore));
    highScoreLabelText.setPosition(160.f, 405.f);
    highScoreText.setPosition(220.f, 402.f);
    window.draw(highScoreLabelText);
    window.draw(highScoreText);

    // "NEW BEST" badge if applicable
    if (score > 0 && score >= highScore) {
        window.draw(newBestText);
    }

    window.draw(restartButton);
    window.draw(restartText);
}

bool Menu::isStartClicked(sf::Vector2f mousePos) {
    return startButton.getGlobalBounds().contains(mousePos);
}

bool Menu::isRestartClicked(sf::Vector2f mousePos) {
    return restartButton.getGlobalBounds().contains(mousePos);
}
