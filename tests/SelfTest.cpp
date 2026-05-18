#include "SelfTest.h"
#include "board/Board.h"
#include "block/Block.h"
#include <iostream>

static int passed = 0;
static int failed = 0;

static void check(bool cond, const char* name) {
    if (cond) { std::cout << "[PASS] " << name << "\n"; ++passed; }
    else       { std::cout << "[FAIL] " << name << "\n"; ++failed; }
}

int runSelfTests() {
    std::cout << "=== Running self-tests ===\n";

    Board board;

    // 1. Empty board: can place a 1x1 block at any valid position
    Block single({{1}});
    check(board.canPlaceBlock(single, {0, 0}), "Place 1x1 at (0,0)");
    check(board.canPlaceBlock(single, {7, 7}), "Place 1x1 at (7,7)");
    check(!board.canPlaceBlock(single, {8, 0}), "Reject 1x1 out of bounds (8,0)");
    check(!board.canPlaceBlock(single, {0, 8}), "Reject 1x1 out of bounds (0,8)");

    // 2. Overlap detection
    board.placeBlock(single, {3, 3});
    check(!board.canPlaceBlock(single, {3, 3}), "Reject overlap at (3,3)");
    check(board.canPlaceBlock(single, {3, 4}), "Allow adjacent (3,4)");

    // 3. Full row clear
    Block hbar({{1,1,1,1,1,1,1,1}});
    board.placeBlock(hbar, {0, 5});
    int cleared = board.clearFullLines();
    check(cleared >= 1, "Clear full row");

    // 4. canPlaceAnyBlock on empty board
    board.reset();
    Block big({{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}});
    check(board.canPlaceAnyBlock(big), "4x4 fits on empty board");

    // 5. canPlaceAnyBlock when board is almost full
    // Fill everything except one corner
    for (int x = 0; x < 8; ++x)
        for (int y = 0; y < 8; ++y)
            if (!(x == 7 && y == 7))
                board.placeBlock(single, {x, y});
    check(!board.canPlaceAnyBlock(big), "4x4 doesn't fit on nearly-full board");
    check(board.canPlaceAnyBlock(single), "1x1 still fits at last cell");

    std::cout << "\n=== Results: " << passed << " passed, " << failed << " failed ===\n";
    return failed > 0 ? 1 : 0;
}
