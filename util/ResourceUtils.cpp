#include "ResourceUtils.h"

#include <array>
#include <iostream>

namespace {
const std::array<const char*, 5> kFontCandidates = {
    "assets/arial.ttf",
    "assets/Arial.ttf",
    "C:/Windows/Fonts/segoeui.ttf",
    "C:/Windows/Fonts/arial.ttf",
    "C:/Windows/Fonts/tahoma.ttf",
};
}

sf::String utf8Text(const std::string& text) {
    if (text.empty()) {
        return sf::String();
    }
    return sf::String::fromUtf8(text.begin(), text.end());
}

sf::String utf8Text(const char* text) {
    if (text == nullptr || text[0] == '\0') {
        return sf::String();
    }
    const char* end = text;
    while (*end != '\0') {
        ++end;
    }
    return sf::String::fromUtf8(text, end);
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
