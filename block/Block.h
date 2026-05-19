#ifndef BLOCK_H
#define BLOCK_H

#include <SFML/Graphics.hpp>
#include <vector>

class Block {
public:
    static constexpr float CELL_SIZE = 50.f;

    Block();
    Block(std::initializer_list<std::vector<int>> shapeList);
    explicit Block(const std::vector<std::vector<int>>& shape);
    ~Block();

    void setPosition(sf::Vector2f position);
    sf::Vector2f getPosition() const;
    void render(sf::RenderWindow& window) const;
    void update();
    bool contains(sf::Vector2f point) const;
    bool containsPreview(sf::Vector2f point, sf::Vector2f center, float scale = 0.55f) const;
    const std::vector<std::vector<int>>& getShape() const;
    sf::Color getColor() const;
    void setColor(sf::Color color);
    sf::Vector2f getSize() const;

    void renderPreview(sf::RenderWindow& window, sf::Vector2f center, float scale = 0.55f) const;

private:
    void buildCells();
    void buildPreviewCache();
    sf::Color chooseBlockColor() const;

    sf::Vector2f position;
    std::vector<std::vector<int>> shape;
    std::vector<sf::RectangleShape> cells;
    mutable std::vector<sf::RectangleShape> previewCells;
    mutable std::vector<sf::FloatRect> previewHitRects;
    sf::Color blockColor;
};

#endif // BLOCK_H
