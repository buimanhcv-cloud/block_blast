#include "ResourceUtils.h"

#include <array>
#include <iostream>

namespace {
const std::array<const char*, 4> kFontCandidates = {
    "assets/arial.ttf",
    "assets/Arial.ttf",
    "C:/Windows/Fonts/arial.ttf",
    "C:/Windows/Fonts/segoeui.ttf",
};
}

bool loadGameFont(sf::Font& font) {
    for (const char* candidate : kFontCandidates) {
        if (font.loadFromFile(candidate)) {
            return true;
        }
    }

    std::cerr << "Warning: no usable font found. UI text may be invisible." << std::endl;
    return false;
}

bool loadOptionalSoundBuffer(sf::SoundBuffer& buffer, const std::string& assetPath) {
    if (buffer.loadFromFile(assetPath)) {
        return true;
    }

    std::cerr << "Warning: could not load sound '" << assetPath << "'." << std::endl;
    return false;
}
