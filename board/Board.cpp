#include "Board.h"

#include <algorithm>
#include <cmath>

Board::Board() {
    reset();

    boardBackground.setSize(sf::Vector2f(SIZE * CELL_SIZE + 32.f, SIZE * CELL_SIZE + 32.f));
    boardBackground.setPosition(OFFSET_X - 16.f, OFFSET_Y - 16.f);
    boardBackground.setFillColor(sf::Color(88, 57, 74));
    boardBackground.setOutlineThickness(0.f);

    boardBorder.setSize(sf::Vector2f(SIZE * CELL_SIZE + 12.f, SIZE * CELL_SIZE + 12.f));
    boardBorder.setPosition(OFFSET_X - 6.f, OFFSET_Y - 6.f);
    boardBorder.setFillColor(sf::Color::Transparent);
    boardBorder.setOutlineColor(sf::Color(64, 34, 52));
    boardBorder.setOutlineThickness(3.f);

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            cells[i][j].setSize(sf::Vector2f(CELL_SIZE - 2.f, CELL_SIZE - 2.f));
            cells[i][j].setPosition(i * CELL_SIZE + OFFSET_X + 1.f, j * CELL_SIZE + OFFSET_Y + 1.f);
            cells[i][j].setFillColor(sf::Color(61, 34, 55));
            cells[i][j].setOutlineColor(sf::Color(48, 24, 42));
            cells[i][j].setOutlineThickness(1.f);
        }
    }
}

Board::~Board() {
}

void Board::reset() {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            grid[i][j] = 0;
        }
    }
}

void Board::render(sf::RenderWindow& window) {
    window.draw(boardBackground);
    window.draw(boardBorder);

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (grid[i][j] == 1) {
                cells[i][j].setFillColor(sf::Color(92, 212, 255));
                cells[i][j].setOutlineColor(sf::Color(214, 247, 255));
            } else {
                const bool evenCell = ((i + j) % 2) == 0;
                cells[i][j].setFillColor(evenCell ? sf::Color(62, 37, 56) : sf::Color(67, 40, 60));
                cells[i][j].setOutlineColor(sf::Color(49, 28, 43));
            }
            window.draw(cells[i][j]);
        }
    }
}

void Board::update() {
}

bool Board::canPlaceBlock(const Block& block, sf::Vector2i position) {
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
    for (size_t y = 0; y < shape.size(); ++y) {
        for (size_t x = 0; x < shape[y].size(); ++x) {
            if (shape[y][x]) {
                grid[position.x + static_cast<int>(x)][position.y + static_cast<int>(y)] = 1;
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

bool Board::canPlaceAnyBlock(const Block& block) const {
    const auto& shape = block.getShape();

    for (int boardX = 0; boardX < SIZE; ++boardX) {
        for (int boardY = 0; boardY < SIZE; ++boardY) {
            bool fits = true;

            for (size_t shapeY = 0; shapeY < shape.size() && fits; ++shapeY) {
                for (size_t shapeX = 0; shapeX < shape[shapeY].size(); ++shapeX) {
                    if (!shape[shapeY][shapeX]) {
                        continue;
                    }

                    const int x = boardX + static_cast<int>(shapeX);
                    const int y = boardY + static_cast<int>(shapeY);
                    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || grid[x][y] == 1) {
                        fits = false;
                        break;
                    }
                }
            }

            if (fits) {
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
    const sf::Color blockColor = sf::Color(100, 150, 200, 80);
    const sf::Color outlineColor = sf::Color(150, 200, 255, 150);

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

            const float cellX = OFFSET_X + boardX * CELL_SIZE + 1.f;
            const float cellY = OFFSET_Y + boardY * CELL_SIZE + 1.f;

            sf::RectangleShape ghostCell(sf::Vector2f(CELL_SIZE - 2.f, CELL_SIZE - 2.f));
            ghostCell.setPosition(cellX, cellY);
            ghostCell.setFillColor(blockColor);
            ghostCell.setOutlineColor(outlineColor);
            ghostCell.setOutlineThickness(1.5f);

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
    }
}

void Board::clearCol(int col) {
    for (int y = 0; y < SIZE; ++y) {
        grid[col][y] = 0;
    }
}
