#include "ScoreStorage.h"

#include <fstream>

namespace {
const char* HIGH_SCORE_FILE = "highscore.dat";
}

int loadHighScore() {
    std::ifstream in(HIGH_SCORE_FILE);
    int value = 0;
    if (in >> value && value >= 0) {
        return value;
    }
    return 0;
}

void saveHighScore(int score) {
    if (score < 0) return;
    std::ofstream out(HIGH_SCORE_FILE, std::ios::trunc);
    if (out) {
        out << score;
    }
}
