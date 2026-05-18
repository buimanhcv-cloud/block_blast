#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

#include "core/GraphicsConfig.h"
#include "core/GameManager.h"
#include "tests/SelfTest.h"

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--self-test") == 0) {
        return runSelfTests();
    }

    sf::RenderWindow window(
        GraphicsConfig::windowedMode(),
        GraphicsConfig::WINDOW_TITLE,
        sf::Style::Titlebar | sf::Style::Close,
        GraphicsConfig::contextSettings());
    window.setVerticalSyncEnabled(GraphicsConfig::ENABLE_VERTICAL_SYNC);
    if (!GraphicsConfig::ENABLE_VERTICAL_SYNC) {
        window.setFramerateLimit(GraphicsConfig::FRAME_RATE_LIMIT);
    }

    GameManager gameManager(window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            gameManager.handleEvent(event);
        }

        gameManager.update();
        gameManager.render();
    }

    return 0;
}
