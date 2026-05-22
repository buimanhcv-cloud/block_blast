#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>

enum class MenuAction { NONE, SOLO, VS_AI, ONLINE, QUIT };

class Menu {
public:
    Menu();
    ~Menu();

    void render(sf::RenderWindow& window);
    void renderGameOver(sf::RenderWindow& window, int score, int highScore);
    void renderPause(sf::RenderWindow& window);

    MenuAction handleClick(sf::Vector2f mousePos);
    bool isRestartClicked(sf::Vector2f mousePos);
    bool isBackToMenuClicked(sf::Vector2f mousePos);

private:
    void makeButton(sf::RectangleShape& btn, sf::Text& txt,
                    float x, float y, float w, float h,
                    const std::string& label, unsigned int fontSize,
                    sf::Color fill, sf::Color textColor = sf::Color::White);
    void drawButton(sf::RenderWindow& window,
                    const sf::RectangleShape& btn, const sf::Text& txt);

    sf::Font font;

    // Background
    sf::RectangleShape background;

    // Title
    sf::Text titleLine1;
    sf::Text titleLine2;

    // Menu buttons
    sf::RectangleShape soloBtn;   sf::Text soloTxt;
    sf::RectangleShape vsAiBtn;   sf::Text vsAiTxt;
    sf::RectangleShape onlineBtn; sf::Text onlineTxt;
    sf::RectangleShape quitBtn;   sf::Text quitTxt;

    // Game over
    sf::RectangleShape overlay;
    sf::RectangleShape gameOverPanel;
    sf::RectangleShape restartBtn;   sf::Text restartTxt;
    sf::RectangleShape backMenuBtn;  sf::Text backMenuTxt;
    sf::Text gameOverTitle;
    sf::Text scoreText;
    sf::Text highScoreText;
    sf::Text newBestText;
};

#endif // MENU_H
