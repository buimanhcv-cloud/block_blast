#include "Board.h"

#include <cmath>

namespace {
const sf::Color EMPTY_CELL_A(62, 37, 56);
const sf::Color EMPTY_CELL_B(67, 40, 60);
constexpr float CELL_INSET = 1.f;
constexpr float CELL_DRAW = static_cast<float>(Board::CELL_SIZE) - 2.f;
}

Board::Board() {
    reset();

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            cells[i][j].setSize(sf::Vector2f(CELL_DRAW, CELL_DRAW));
            cells[i][j].setPosition(
                i * CELL_SIZE + OFFSET_X + CELL_INSET,
                j * CELL_SIZE + OFFSET_Y + CELL_INSET);
            cells[i][j].setOutlineThickness(0.f);
        }
    }
}

Board::~Board() {}

void Board::reset() {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            grid[i][j] = 0;
            cellColors[i][j] = sf::Color::Transparent;
        }
    }
}

void Board::render(sf::RenderWindow& window) {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (grid[i][j] == 1) {
                cells[i][j].setFillColor(cellColors[i][j]);
            } else {
                const bool evenCell = ((i + j) % 2) == 0;
                cells[i][j].setFillColor(evenCell ? EMPTY_CELL_A : EMPTY_CELL_B);
            }
            window.draw(cells[i][j]);
        }
    }
}

void Board::update() {}

bool Board::canPlaceBlock(const Block& block, sf::Vector2i position) const {
    const auto& shape = block.getShape();
    for (size_t y = 0; y < shape.size(); ++y) {
        for (size_t x = 0; x < shape[y].size(); ++x) {
            if (shape[y][x]) {
                const int boardX = position.x + static_cast<int>(x);
                const int boardY = position.y + static_cast<int>(y);
                if (boardX < 0 || boardX >= SIZE || boardY < 0 || boardY >= SIZE || grid[boardX][boardY] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}

void Board::placeBlock(const Block& block, sf::Vector2i position) {
    const auto& shape = block.getShape();
    const sf::Color color = block.getColor();
    for (size_t y = 0; y < shape.size(); ++y) {
        for (size_t x = 0; x < shape[y].size(); ++x) {
            if (shape[y][x]) {
                const int bx = position.x + static_cast<int>(x);
                const int by = position.y + static_cast<int>(y);
                grid[bx][by] = 1;
                cellColors[bx][by] = color;
            }
        }
    }
}

int Board::clearFullLines() {
    int cleared = 0;

    bool fullRows[SIZE] = {};
    bool fullCols[SIZE] = {};

    for (int row = 0; row < SIZE; ++row) {
        fullRows[row] = isRowFull(row);
        if (fullRows[row]) {
            ++cleared;
        }
    }
    for (int col = 0; col < SIZE; ++col) {
        fullCols[col] = isColFull(col);
        if (fullCols[col]) {
            ++cleared;
        }
    }

    for (int row = 0; row < SIZE; ++row) {
        if (fullRows[row]) {
            clearRow(row);
        }
    }
    for (int col = 0; col < SIZE; ++col) {
        if (fullCols[col]) {
            clearCol(col);
        }
    }

    return cleared;
}

bool Board::isEmpty() const {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (grid[i][j] != 0) {
                return false;
            }
        }
    }
    return true;
}

bool Board::canPlaceAnyBlock(const Block& block) const {
    for (int boardX = 0; boardX < SIZE; ++boardX) {
        for (int boardY = 0; boardY < SIZE; ++boardY) {
            if (canPlaceBlock(block, sf::Vector2i(boardX, boardY))) {
                return true;
            }
        }
    }
    return false;
}

sf::Vector2i Board::getBoardPosition(sf::Vector2f screenPos) const {
    const int x = static_cast<int>(std::floor((screenPos.x - OFFSET_X) / CELL_SIZE));
    const int y = static_cast<int>(std::floor((screenPos.y - OFFSET_Y) / CELL_SIZE));
    return sf::Vector2i(x, y);
}

int Board::getCell(int x, int y) const {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        return -1;
    }

    return grid[x][y];
}

void Board::renderGhostCells(sf::RenderWindow& window, const Block& block, sf::Vector2i position) const {
    const auto& shape = block.getShape();
    const sf::Color fill = block.getColor();
    sf::Color ghostFill = fill;
    ghostFill.a = 100;

    for (size_t y = 0; y < shape.size(); ++y) {
        for (size_t x = 0; x < shape[y].size(); ++x) {
            if (!shape[y][x]) continue;

            const int boardX = position.x + static_cast<int>(x);
            const int boardY = position.y + static_cast<int>(y);

            if (boardX < 0 || boardX >= SIZE || boardY < 0 || boardY >= SIZE) {
                continue;
            }

            if (grid[boardX][boardY] == 1) {
                continue;
            }

            const float cellX = OFFSET_X + boardX * CELL_SIZE + CELL_INSET;
            const float cellY = OFFSET_Y + boardY * CELL_SIZE + CELL_INSET;

            sf::RectangleShape ghostCell(sf::Vector2f(CELL_DRAW, CELL_DRAW));
            ghostCell.setPosition(cellX, cellY);
            ghostCell.setFillColor(ghostFill);
            ghostCell.setOutlineThickness(0.f);

            window.draw(ghostCell);
        }
    }
}

bool Board::isRowFull(int row) {
    for (int x = 0; x < SIZE; ++x) {
        if (grid[x][row] == 0) {
            return false;
        }
    }
    return true;
}

bool Board::isColFull(int col) {
    for (int y = 0; y < SIZE; ++y) {
        if (grid[col][y] == 0) {
            return false;
        }
    }
    return true;
}

void Board::clearRow(int row) {
    for (int x = 0; x < SIZE; ++x) {
        grid[x][row] = 0;
        cellColors[x][row] = sf::Color::Transparent;
    }
}

void Board::clearCol(int col) {
    for (int y = 0; y < SIZE; ++y) {
        grid[col][y] = 0;
        cellColors[col][y] = sf::Color::Transparent;
    }
}
