#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "board/Board.h"
#include "block/Block.h"
#include <array>
#include <memory>

// ── Simple greedy AI: picks the placement that clears the most lines,
//    then prefers positions that keep the board least filled. ─────────────────
class AIPlayer {
public:
    struct Move {
        int trayIndex = -1;
        sf::Vector2i boardPos;
        bool valid = false;
    };

    // Find the best move given the current board and tray.
    // Returns the best Move (valid=false if no move possible).
    static Move findBestMove(
        const Board& board,
        const std::array<std::unique_ptr<Block>, 3>& tray);

private:
    // Score a placement: higher = better
    static int scorePlacement(const Board& board,
                               const Block& block,
                               sf::Vector2i pos);
};

#endif // AIPLAYER_H
