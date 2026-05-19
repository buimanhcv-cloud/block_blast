#ifndef SCORING_H
#define SCORING_H

#include <string>

namespace Scoring {

// Điểm cơ bản mỗi hàng/cột xóa trong một lượt
constexpr int BASE_PER_LINE = 10;

// Hệ số khi xóa nhiều hàng/cột cùng lúc (1→×1, 2→×2, 3→×3, 4+→×4)
int simultaneousMultiplier(int linesCleared);

// Hệ số thưởng khi xóa sạch toàn bộ bảng sau lượt xóa
constexpr int FULL_BOARD_MULTIPLIER = 5;

int pointsForClear(int linesCleared, bool fullBoardClear);
std::string rankTitle(int score);

} // namespace Scoring

#endif // SCORING_H
