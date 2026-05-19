#include "Block.h"

#include <algorithm>
#include <chrono>
#include <random>

// 36 shapes, all within 3x3 — varied but still easy to place
static const std::vector<std::vector<std::vector<int>>> ALL_SHAPES = {
    // 1 cell
    {{1}},

    // 2 cells — domino
    {{1, 1}},
    {{1}, {1}},

    // 3 cells — tromino
    {{1, 1, 1}},
    {{1}, {1}, {1}},
    {{1, 1}, {1, 0}},
    {{1, 1}, {0, 1}},
    {{1, 0}, {1, 1}},
    {{0, 1}, {1, 1}},

    // 4 cells — tetromino (fits 3x3)
    {{1, 1}, {1, 1}},
    {{1, 1, 1}, {0, 1, 0}},
    {{1, 0}, {1, 1}, {1, 0}},
    {{0, 1, 0}, {1, 1, 1}},
    {{0, 1}, {1, 1}, {0, 1}},
    {{1, 0, 0}, {1, 1, 1}},
    {{1, 1}, {1, 0}, {1, 0}},
    {{1, 1, 1}, {0, 0, 1}},
    {{0, 1}, {0, 1}, {1, 1}},
    {{0, 1, 1}, {1, 1, 0}},
    {{1, 1, 0}, {0, 1, 1}},
    {{1, 0}, {1, 1}, {0, 1}},

    // 5 cells — compact pentomino
    {{0, 1, 0}, {1, 1, 1}, {0, 1, 0}},
    {{1, 1, 1}, {1, 0, 0}, {1, 0, 0}},
    {{1, 1, 1}, {0, 0, 1}, {0, 0, 1}},
    {{1, 1, 1}, {1, 1, 0}},
    {{1, 1, 0}, {1, 1, 1}},
    {{1, 0}, {1, 1}, {0, 1}},
    {{0, 1}, {1, 1}, {1, 0}},
    {{1, 1, 0}, {0, 1, 0}, {0, 1, 1}},
    {{0, 1, 1}, {0, 1, 0}, {1, 1, 0}},

    // 6 cells — rectangles & U (easy)
    {{1, 1, 1}, {1, 1, 1}},
    {{1, 1}, {1, 1}, {1, 1}},
    {{1, 1, 1}, {1, 0, 1}},
    {{1, 0, 1}, {1, 1, 1}},
    {{1, 1, 0}, {0, 1, 1}, {0, 1, 1}},
    {{0, 1, 1}, {1, 1, 0}, {1, 1, 0}},
};

Block::Block() {
    static std::mt19937 rng(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> dist(0, static_cast<int>(ALL_SHAPES.size()) - 1);
    shape = ALL_SHAPES[dist(rng)];
    blockColor = chooseBlockColor();
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
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2.f, CELL_SIZE - 2.f));
            cell.setFillColor(blockColor);
            cell.setOutlineThickness(0.f);
            cells.push_back(cell);
        }
    }
    buildPreviewCache();
}

void Block::buildPreviewCache() {
    previewCells.clear();
    previewHitRects.clear();

    constexpr float scale = 0.55f;
    const float cs = CELL_SIZE * scale;
    const float cellDraw = cs - 2.f;
    const float cols = static_cast<float>(shape.empty() ? 0 : shape[0].size());
    const float rows = static_cast<float>(shape.size());
    const float totalW = cols * cs;
    const float totalH = rows * cs;
    const float startX = -totalW / 2.f;
    const float startY = -totalH / 2.f;

    for (size_t y = 0; y < shape.size(); ++y) {
        for (size_t x = 0; x < shape[y].size(); ++x) {
            if (!shape[y][x]) continue;

            const float localX = startX + static_cast<float>(x) * cs + 1.f;
            const float localY = startY + static_cast<float>(y) * cs + 1.f;

            sf::RectangleShape cell(sf::Vector2f(cellDraw, cellDraw));
            cell.setPosition(localX, localY);
            cell.setFillColor(blockColor);
            cell.setOutlineThickness(0.f);
            previewCells.push_back(cell);
            previewHitRects.emplace_back(localX, localY, cellDraw, cellDraw);
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
                position.x + static_cast<float>(x) * CELL_SIZE + 1.f,
                position.y + static_cast<float>(y) * CELL_SIZE + 1.f);
            ++cellIndex;
        }
    }
}

sf::Vector2f Block::getPosition() const { return position; }

void Block::render(sf::RenderWindow& window) const {
    for (const auto& cell : cells) window.draw(cell);
}

void Block::renderPreview(sf::RenderWindow& window, sf::Vector2f center, float /*scale*/) const {
    for (size_t i = 0; i < previewCells.size(); ++i) {
        const sf::FloatRect& local = previewHitRects[i];
        previewCells[i].setPosition(center.x + local.left, center.y + local.top);
        window.draw(previewCells[i]);
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

bool Block::containsPreview(sf::Vector2f point, sf::Vector2f center, float /*scale*/) const {
    for (const auto& local : previewHitRects) {
        const sf::FloatRect world(
            center.x + local.left, center.y + local.top, local.width, local.height);
        if (world.contains(point)) {
            return true;
        }
    }
    return false;
}

const std::vector<std::vector<int>>& Block::getShape() const { return shape; }

sf::Color Block::getColor() const { return blockColor; }

void Block::setColor(sf::Color color) {
    blockColor = color;
    for (auto& cell : cells) {
        cell.setFillColor(blockColor);
    }
    buildPreviewCache();
}

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
        sf::Color(132, 193, 255), sf::Color(246, 173, 230), sf::Color(92, 212, 255),
        sf::Color(235, 102, 140), sf::Color(255, 206, 91), sf::Color(180, 130, 255),
        sf::Color(255, 140, 90), sf::Color(100, 230, 180), sf::Color(255, 180, 80),
    };
    size_t h = 0;
    for (const auto& row : shape)
        for (int v : row) h = h * 31 + v;
    return colors[h % colors.size()];
}
