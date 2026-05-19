#ifndef RESOURCEUTILS_H
#define RESOURCEUTILS_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/String.hpp>
#include <string>

bool loadGameFont(sf::Font& font);
sf::String utf8Text(const char* text);
sf::String utf8Text(const std::string& text);
bool loadOptionalSoundBuffer(sf::SoundBuffer& buffer, const std::string& assetPath);

#endif // RESOURCEUTILS_H
