#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "block/Block.h"

class Board {
public:
    static const int SIZE = 8;
    static const int CELL_SIZE = 50;
    static constexpr float OFFSET_X = 70.f;
    static constexpr float OFFSET_Y = 220.f;

    Board();
    ~Board();

    void reset();
    void render(sf::RenderWindow& window);
    void update();

    bool canPlaceBlock(const Block& block, sf::Vector2i position);
    void placeBlock(const Block& block, sf::Vector2i position);
    int clearFullLines();
    bool canPlaceAnyBlock(const Block& block) const;
    sf::Vector2i getBoardPosition(sf::Vector2f screenPos) const;
    int getCell(int x, int y) const;
    void renderGhostCells(sf::RenderWindow& window, const Block& block, sf::Vector2i position) const;

private:
    int grid[SIZE][SIZE];
    sf::RectangleShape cells[SIZE][SIZE];
    sf::RectangleShape boardBackground;
    sf::RectangleShape boardBorder;

    bool isRowFull(int row);
    bool isColFull(int col);
    void clearRow(int row);
    void clearCol(int col);
};

#endif // BOARD_H
