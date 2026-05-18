#ifndef GRAPHICSCONFIG_H
#define GRAPHICSCONFIG_H

#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace GraphicsConfig {
inline constexpr unsigned int WINDOW_WIDTH = 540;
inline constexpr unsigned int WINDOW_HEIGHT = 960;
inline constexpr unsigned int FRAME_RATE_LIMIT = 60;
inline constexpr unsigned int ANTIALIASING_LEVEL = 4;
inline constexpr bool ENABLE_VERTICAL_SYNC = true;
inline constexpr const char* WINDOW_TITLE = "Block Puzzle Game";

inline sf::VideoMode windowedMode() {
    return sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT);
}

inline sf::ContextSettings contextSettings() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = ANTIALIASING_LEVEL;
    return settings;
}
} // namespace GraphicsConfig

#endif // GRAPHICSCONFIG_H
