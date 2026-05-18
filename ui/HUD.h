#ifndef HUD_H
#define HUD_H

#include <SFML/Graphics.hpp>

class HUD {
public:
    HUD();
    ~HUD();

    void render(sf::RenderWindow& window);
    void updateScore(int score, int highScore);

private:
    sf::Font font;
    int currentScore;
    int currentHighScore;

    sf::RectangleShape topBadge;
    sf::RectangleShape settingsButton;
    sf::RectangleShape scorePanel;
    sf::RectangleShape trayPanel;
    sf::RectangleShape traySlotLeft;
    sf::RectangleShape traySlotCenter;
    sf::RectangleShape traySlotRight;

    sf::Text crownText;
    sf::Text settingsText;
    sf::Text scoreLabel;
    sf::Text scoreValueText;
    sf::Text highScoreLabel;
    sf::Text highScoreText;
};

#endif // HUD_H
