#ifndef DUELSELECTSCREEN_H
#define DUELSELECTSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>

class DuelSelectScreen {
public:
    DuelSelectScreen();
    ~DuelSelectScreen();

    void render(sf::RenderWindow& window);
    bool handleClick(sf::Vector2f mousePos);
    bool isBackClicked(sf::Vector2f mousePos) const;
    int selectedMilestoneId() const;

private:
    struct MilestoneOption {
        int id;
        sf::RectangleShape button;
        sf::Text label;
    };

    sf::Font font;
    sf::RectangleShape background;
    sf::RectangleShape panel;
    sf::RectangleShape backButton;
    sf::Text backText;
    sf::Text titleText;
    sf::Text blitzHeaderText;
    sf::Text fastHeaderText;
    sf::Text hourHeaderText;
    sf::Text dayHeaderText;

    std::vector<MilestoneOption> options;
    int selectedId;

    void addOption(int id, const char* label, float x, float y, float width, float height,
                   const sf::Color& fill, const sf::Color& outline);
};

#endif // DUELSELECTSCREEN_H
