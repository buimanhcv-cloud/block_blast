#include <SFML/Graphics.hpp>
#include <filesystem>
#include "GameLauncher.h"

const unsigned int WINDOW_WIDTH = 600;
const unsigned int WINDOW_HEIGHT = 400;
const sf::Color BG_COLOR(40, 40, 40);
const sf::Color BUTTON_COLOR(70, 130, 180);
const sf::Color BUTTON_HOVER_COLOR(100, 160, 210);
const sf::Color TEXT_COLOR(255, 255, 255);

struct Button {
    sf::RectangleShape shape;
    std::string text;
    bool hovered = false;

    Button(float x, float y, float width, float height, const std::string& txt)
        : text(txt) {
        shape.setPosition(x, y);
        shape.setSize({width, height});
        shape.setFillColor(BUTTON_COLOR);
    }

    bool isMouseOver(sf::Vector2f mousePos) const {
        return shape.getGlobalBounds().contains(mousePos);
    }

    void update(sf::Vector2f mousePos) {
        hovered = isMouseOver(mousePos);
        shape.setFillColor(hovered ? BUTTON_HOVER_COLOR : BUTTON_COLOR);
    }

    bool isClicked(sf::Event event, sf::Vector2f mousePos) const {
        return event.type == sf::Event::MouseButtonPressed &&
               event.mouseButton.button == sf::Mouse::Left &&
               isMouseOver(mousePos);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                           "Block Puzzle Game Launcher");
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontLoaded = font.loadFromFile("assets/arial.ttf");

    GameLauncher launcher("BlockPuzzleGame.exe");

    // Title
    sf::Text titleText;
    titleText.setString("Block Puzzle Game");
    titleText.setCharacterSize(48);
    titleText.setFillColor(TEXT_COLOR);
    if (fontLoaded) titleText.setFont(font);
    titleText.setPosition(WINDOW_WIDTH / 2 - 150, 50);

    // Version
    sf::Text versionText;
    versionText.setString("Version 1.0");
    versionText.setCharacterSize(16);
    versionText.setFillColor(sf::Color(180, 180, 180));
    if (fontLoaded) versionText.setFont(font);
    versionText.setPosition(WINDOW_WIDTH / 2 - 40, 120);

    // Play Button
    Button playButton(WINDOW_WIDTH / 2 - 75, 180, 150, 50, "Play");

    // Status Text
    sf::Text statusText;
    statusText.setCharacterSize(14);
    statusText.setFillColor(sf::Color(200, 200, 200));
    if (fontLoaded) statusText.setFont(font);
    statusText.setPosition(20, WINDOW_HEIGHT - 40);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        playButton.update(mousePos);

        // Check for play button click
        if (playButton.isClicked(event, mousePos)) {
            launcher.launchGame();
        }

        launcher.update();

        // Update status text
        if (launcher.isGameRunning()) {
            statusText.setString("Game is running...");
            statusText.setFillColor(sf::Color::Green);
        } else {
            statusText.setString("Click 'Play' to start the game");
            statusText.setFillColor(sf::Color(200, 200, 200));
        }

        // Render
        window.clear(BG_COLOR);

        window.draw(titleText);
        window.draw(versionText);
        window.draw(playButton.shape);

        // Draw button text
        sf::Text buttonText;
        buttonText.setString(playButton.text);
        buttonText.setCharacterSize(24);
        buttonText.setFillColor(TEXT_COLOR);
        if (fontLoaded) buttonText.setFont(font);
        buttonText.setPosition(playButton.shape.getPosition().x + 35,
                              playButton.shape.getPosition().y + 8);
        window.draw(buttonText);

        window.draw(statusText);
        window.display();
    }

    return 0;
}
