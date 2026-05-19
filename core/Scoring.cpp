#include "Scoring.h"

#include <algorithm>

namespace Scoring {

int simultaneousMultiplier(int linesCleared) {
    if (linesCleared <= 0) return 0;
    if (linesCleared == 1) return 1;
    if (linesCleared == 2) return 2;
    if (linesCleared == 3) return 3;
    return 4;
}

int pointsForClear(int linesCleared, bool fullBoardClear) {
    if (linesCleared <= 0) return 0;

    const int mult = simultaneousMultiplier(linesCleared);
    int points = BASE_PER_LINE * linesCleared * mult;

    if (fullBoardClear) {
        points *= FULL_BOARD_MULTIPLIER;
    }

    return points;
}

std::string rankTitle(int score) {
    if (score >= 5000) return "Master";
    if (score >= 3000) return "Expert";
    if (score >= 1500) return "Pro";
    if (score >= 800) return "Skilled";
    if (score >= 400) return "Rising";
    if (score >= 150) return "Rookie";
    return "Beginner";
}

} // namespace Scoring
