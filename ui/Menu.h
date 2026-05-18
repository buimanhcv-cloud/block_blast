#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>

class Menu {
public:
    Menu();
    ~Menu();

    void render(sf::RenderWindow& window);
    void renderGameOver(sf::RenderWindow& window, int score, int highScore);
    bool isStartClicked(sf::Vector2f mousePos);
    bool isRestartClicked(sf::Vector2f mousePos);

private:
    sf::Font font;
    sf::RectangleShape background;
    sf::RectangleShape logoPanel;
    sf::RectangleShape classicButton;
    sf::RectangleShape startButton;
    sf::RectangleShape overlay;
    sf::RectangleShape gameOverPanel;
    sf::RectangleShape restartButton;

    sf::Text titleText;
    sf::Text subtitleText;
    sf::Text classicText;
    sf::Text gameOverText;
    sf::Text restartText;
    sf::Text scoreText;
    sf::Text highScoreText;
    sf::Text highScoreLabelText;
    sf::Text newBestText;
};

#endif // MENU_H
