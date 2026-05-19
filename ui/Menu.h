#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>

class Menu {
public:
    Menu();
    ~Menu();

    void render(sf::RenderWindow& window);
    void renderGameOver(sf::RenderWindow& window, int score, int highScore, const std::string& rank, bool newBest);
    void renderVersusEnd(sf::RenderWindow& window, int winnerPlayer);
    bool isClassicClicked(sf::Vector2f mousePos) const;
    bool isVersusClicked(sf::Vector2f mousePos) const;
    bool isDuelMilestonesClicked(sf::Vector2f mousePos) const;
    bool isOnlineClicked(sf::Vector2f mousePos) const;
    bool isRestartClicked(sf::Vector2f mousePos) const;

private:
    sf::Font font;
    sf::RectangleShape background;
    sf::RectangleShape logoPanel;
    sf::RectangleShape classicButton;
    sf::RectangleShape versusButton;
    sf::RectangleShape duelMilestonesButton;
    sf::RectangleShape onlineButton;
    sf::RectangleShape overlay;
    sf::RectangleShape gameOverPanel;
    sf::RectangleShape restartButton;

    sf::Text titleText;
    sf::Text subtitleText;
    sf::Text classicText;
    sf::Text versusText;
    sf::Text duelMilestonesText;
    sf::Text onlineText;
    sf::Text gameOverText;
    sf::Text restartText;
    sf::Text scoreText;
    sf::Text highScoreText;
    sf::Text highScoreLabelText;
    sf::Text rankText;
    sf::Text newBestText;
    sf::Text versusResultText;
};

#endif // MENU_H
