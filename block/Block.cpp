#include "Block.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>

// 19 block shapes covering all standard Block Puzzle pieces
static const std::vector<std::vector<std::vector<int>>> ALL_SHAPES = {
    // Single
    {{1}},
    // Dominoes
    {{1, 1}},
    {{1}, {1}},
    // Triominoes
    {{1, 1, 1}},
    {{1}, {1}, {1}},
    {{1, 1}, {1, 0}},
    {{1, 1}, {0, 1}},
    {{1, 0}, {1, 1}},
    {{0, 1}, {1, 1}},
    // Tetrominoes
    {{1, 1, 1, 1}},
    {{1}, {1}, {1}, {1}},
    {{1, 1}, {1, 1}},
    {{1, 1, 1}, {1, 0, 0}},
    {{1, 1, 1}, {0, 0, 1}},
    {{1, 0, 0}, {1, 1, 1}},
    {{0, 0, 1}, {1, 1, 1}},
    {{1, 1, 1}, {0, 1, 0}},
    // Pentominoes
    {{1, 1, 1}, {1, 0, 0}, {1, 0, 0}},
    {{1, 1, 1}, {0, 0, 1}, {0, 0, 1}},
};

// Color palette for each shape index
static const std::vector<sf::Color> SHAPE_COLORS = {
    sf::Color(132, 193, 255),  // single - light blue
    sf::Color(246, 173, 230),  // domino h - pink
    sf::Color(246, 173, 230),  // domino v - pink
    sf::Color(92, 212, 255),   // tri h - cyan
    sf::Color(92, 212, 255),   // tri v - cyan
    sf::Color(235, 102, 140),  // tri L1 - rose
    sf::Color(235, 102, 140),  // tri L2 - rose
    sf::Color(235, 102, 140),  // tri L3 - rose
    sf::Color(235, 102, 140),  // tri L4 - rose
    sf::Color(255, 206, 91),   // I4 h - yellow
    sf::Color(255, 206, 91),   // I4 v - yellow
    sf::Color(180, 130, 255),  // O - purple
    sf::Color(255, 140, 90),   // L1 - orange
    sf::Color(255, 140, 90),   // L2 - orange
    sf::Color(255, 140, 90),   // L3 - orange
    sf::Color(255, 140, 90),   // L4 - orange
    sf::Color(100, 230, 180),  // T - teal
    sf::Color(255, 180, 80),   // P1 - amber
    sf::Color(255, 180, 80),   // P2 - amber
};

static int g_shapeIndex = -1; // track which shape was chosen for color lookup

Block::Block() {
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }
    g_shapeIndex = std::rand() % static_cast<int>(ALL_SHAPES.size());
    shape = ALL_SHAPES[g_shapeIndex];
    blockColor = SHAPE_COLORS[g_shapeIndex];
    buildCells();
    setPosition(sf::Vector2f(0.f, 0.f));
}

Block::Block(std::initializer_list<std::vector<int>> shapeList)
    : shape(shapeList) {
    blockColor = chooseBlockColor();
    buildCells();
    setPosition(sf::Vector2f(0.f, 0.f));
}

Block::Block(const std::vector<std::vector<int>>& shape) : shape(shape) {
    blockColor = chooseBlockColor();
    buildCells();
    setPosition(sf::Vector2f(0.f, 0.f));
}

void Block::buildCells() {
    cells.clear();
    for (size_t y = 0; y < shape.size(); ++y) {
        for (size_t x = 0; x < shape[y].size(); ++x) {
            if (!shape[y][x]) continue;
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 6.f, CELL_SIZE - 6.f));
            cell.setFillColor(blockColor);
            cell.setOutlineColor(sf::Color(255, 255, 255, 150));
            cell.setOutlineThickness(2.f);
            cells.push_back(cell);
        }
    }
}

Block::~Block() {}

void Block::setPosition(sf::Vector2f position) {
    this->position = position;
    std::size_t cellIndex = 0;
    for (size_t y = 0; y < shape.size(); ++y) {
        for (size_t x = 0; x < shape[y].size(); ++x) {
            if (!shape[y][x]) continue;
            cells[cellIndex].setPosition(
                position.x + static_cast<float>(x) * CELL_SIZE + 3.f,
                position.y + static_cast<float>(y) * CELL_SIZE + 3.f);
            ++cellIndex;
        }
    }
}

sf::Vector2f Block::getPosition() const { return position; }

void Block::render(sf::RenderWindow& window) const {
    for (const auto& cell : cells) window.draw(cell);
}

void Block::renderPreview(sf::RenderWindow& window, sf::Vector2f center, float scale) const {
    const float cs = CELL_SIZE * scale;
    const float cellDraw = cs - 4.f;
    const float cols = static_cast<float>(shape.empty() ? 0 : shape[0].size());
    const float rows = static_cast<float>(shape.size());
    const float totalW = cols * cs;
    const float totalH = rows * cs;
    const float startX = center.x - totalW / 2.f;
    const float startY = center.y - totalH / 2.f;

    for (size_t y = 0; y < shape.size(); ++y) {
        for (size_t x = 0; x < shape[y].size(); ++x) {
            if (!shape[y][x]) continue;
            sf::RectangleShape cell(sf::Vector2f(cellDraw, cellDraw));
            cell.setPosition(startX + x * cs + 2.f, startY + y * cs + 2.f);
            cell.setFillColor(blockColor);
            cell.setOutlineColor(sf::Color(255, 255, 255, 120));
            cell.setOutlineThickness(1.5f);
            window.draw(cell);
        }
    }
}

void Block::update() {}

// Fix: pixel-accurate hit test per filled cell for the actual block geometry
bool Block::contains(sf::Vector2f point) const {
    for (const auto& cell : cells) {
        if (cell.getGlobalBounds().contains(point)) return true;
    }
    return false;
}

bool Block::containsPreview(sf::Vector2f point, sf::Vector2f center, float scale) const {
    const float cs = CELL_SIZE * scale;
    const float cellDraw = cs - 4.f;
    const float cols = static_cast<float>(shape.empty() ? 0 : shape[0].size());
    const float rows = static_cast<float>(shape.size());
    const float totalW = cols * cs;
    const float totalH = rows * cs;
    const float startX = center.x - totalW / 2.f;
    const float startY = center.y - totalH / 2.f;

    for (size_t y = 0; y < shape.size(); ++y) {
        for (size_t x = 0; x < shape[y].size(); ++x) {
            if (!shape[y][x]) continue;
            sf::FloatRect rect(startX + static_cast<float>(x) * cs + 2.f,
                               startY + static_cast<float>(y) * cs + 2.f,
                               cellDraw, cellDraw);
            if (rect.contains(point)) return true;
        }
    }
    return false;
}

const std::vector<std::vector<int>>& Block::getShape() const { return shape; }

sf::Vector2f Block::getSize() const {
    float width = 0.f;
    float height = static_cast<float>(shape.size()) * CELL_SIZE;
    for (const auto& row : shape)
        width = std::max(width, static_cast<float>(row.size()) * CELL_SIZE);
    return sf::Vector2f(width, height);
}

sf::Color Block::chooseBlockColor() const {
    // fallback for constructor-from-shape
    static const std::vector<sf::Color> colors = {
        sf::Color(92,212,255), sf::Color(235,102,140), sf::Color(255,206,91),
        sf::Color(180,130,255), sf::Color(100,230,180), sf::Color(255,140,90),
    };
    size_t h = 0;
    for (const auto& row : shape)
        for (int v : row) h = h * 31 + v;
    return colors[h % colors.size()];
}
