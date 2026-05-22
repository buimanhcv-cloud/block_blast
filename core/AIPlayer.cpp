#include "AIPlayer.h"
#include <limits>

// ── Score a placement ─────────────────────────────────────────────────────────
// Simulate placing the block and count lines cleared + penalise height.
int AIPlayer::scorePlacement(const Board& board,
                              const Block& block,
                              sf::Vector2i pos) {
    // Build a temporary grid copy
    int grid[Board::SIZE][Board::SIZE] = {};
    for (int x = 0; x < Board::SIZE; ++x)
        for (int y = 0; y < Board::SIZE; ++y)
            grid[x][y] = board.getCell(x, y);

    // Place block
    const auto& shape = block.getShape();
    for (int r = 0; r < (int)shape.size(); ++r)
        for (int c = 0; c < (int)shape[r].size(); ++c)
            if (shape[r][c])
                grid[pos.x + c][pos.y + r] = 1;

    // Count full rows and cols
    int linesCleared = 0;
    for (int row = 0; row < Board::SIZE; ++row) {
        bool full = true;
        for (int x = 0; x < Board::SIZE; ++x)
            if (!grid[x][row]) { full = false; break; }
        if (full) linesCleared++;
    }
    for (int col = 0; col < Board::SIZE; ++col) {
        bool full = true;
        for (int y = 0; y < Board::SIZE; ++y)
            if (!grid[col][y]) { full = false; break; }
        if (full) linesCleared++;
    }

    // Count total filled cells (lower = better board state)
    int filled = 0;
    for (int x = 0; x < Board::SIZE; ++x)
        for (int y = 0; y < Board::SIZE; ++y)
            if (grid[x][y]) filled++;

    // Score: lines cleared worth a lot, filled cells penalised
    return linesCleared * 100 - filled;
}

// ── Find best move ────────────────────────────────────────────────────────────
AIPlayer::Move AIPlayer::findBestMove(
    const Board& board,
    const std::array<std::unique_ptr<Block>, 3>& tray) {

    Move best;
    int bestScore = std::numeric_limits<int>::min();

    for (int t = 0; t < 3; ++t) {
        if (!tray[t]) continue;
        const Block& block = *tray[t];

        for (int bx = 0; bx < Board::SIZE; ++bx) {
            for (int by = 0; by < Board::SIZE; ++by) {
                sf::Vector2i pos(bx, by);
                // canPlaceBlock is const-correct
                if (const_cast<Board&>(board).canPlaceBlock(block, pos)) {
                    int score = scorePlacement(board, block, pos);
                    if (score > bestScore) {
                        bestScore = score;
                        best = {t, pos, true};
                    }
                }
            }
        }
    }
    return best;
}
