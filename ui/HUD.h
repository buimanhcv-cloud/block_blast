#ifndef HUD_H
#define HUD_H

#include <SFML/Graphics.hpp>

class HUD {
public:
    HUD();
    ~HUD();

    void render(sf::RenderWindow& window);
    void updateScore(int score);
    void setVersusMode(bool enabled);
    void setActivePlayer(int playerIndex);
    bool isMenuClicked(sf::Vector2f mousePos) const;

private:
    sf::Font font;
    bool versusMode;

    sf::RectangleShape menuButton;
    sf::RectangleShape scorePanel;
    sf::RectangleShape turnPanel;
    sf::RectangleShape playerIndicator;

    sf::Text menuText;
    sf::Text scoreLabel;
    sf::Text scoreValueText;
    sf::Text turnText;
};

#endif // HUD_H
