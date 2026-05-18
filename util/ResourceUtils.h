#ifndef RESOURCEUTILS_H
#define RESOURCEUTILS_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>

bool loadGameFont(sf::Font& font);
bool loadOptionalSoundBuffer(sf::SoundBuffer& buffer, const std::string& assetPath);

#endif // RESOURCEUTILS_H
