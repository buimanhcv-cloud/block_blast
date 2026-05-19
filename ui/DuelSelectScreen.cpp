#include "DuelSelectScreen.h"

#include "core/GraphicsConfig.h"
#include "util/ResourceUtils.h"

namespace {
constexpr float SCREEN_CENTER_X = GraphicsConfig::WINDOW_WIDTH / 2.f;
constexpr float BTN_W = 148.f;
constexpr float BTN_H = 48.f;
constexpr float COL_GAP = 12.f;

constexpr float GRID_LEFT = (GraphicsConfig::WINDOW_WIDTH - (3.f * BTN_W + 2.f * COL_GAP)) / 2.f;

void centerTextInRect(sf::Text& text, const sf::FloatRect& area) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    text.setPosition(area.left + area.width / 2.f, area.top + area.height / 2.f);
}

void centerTextAt(sf::Text& text, float x, float y) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top);
    text.setPosition(x, y);
}

float gridX(int column) {
    return GRID_LEFT + static_cast<float>(column) * (BTN_W + COL_GAP);
}
} // namespace

DuelSelectScreen::DuelSelectScreen() : selectedId(-1) {
    loadGameFont(font);

    background.setSize(sf::Vector2f(
        static_cast<float>(GraphicsConfig::WINDOW_WIDTH),
        static_cast<float>(GraphicsConfig::WINDOW_HEIGHT)));
    background.setFillColor(sf::Color(37, 119, 224));

    panel.setSize(sf::Vector2f(480.f, 700.f));
    panel.setPosition(30.f, 118.f);
    panel.setFillColor(sf::Color(30, 80, 160, 210));
    panel.setOutlineColor(sf::Color(80, 150, 230));
    panel.setOutlineThickness(2.f);

    backButton.setSize(sf::Vector2f(96.f, 44.f));
    backButton.setPosition(24.f, 24.f);
    backButton.setFillColor(sf::Color(83, 54, 70, 220));
    backButton.setOutlineColor(sf::Color(123, 85, 103));
    backButton.setOutlineThickness(2.f);

    backText.setFont(font);
    backText.setString(utf8Text("Quay lại"));
    backText.setCharacterSize(18);
    backText.setFillColor(sf::Color::White);
    centerTextInRect(backText, sf::FloatRect(24.f, 24.f, 96.f, 44.f));

    titleText.setFont(font);
    titleText.setString(utf8Text("Mốc đối chiến"));
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color(255, 220, 100));
    centerTextAt(titleText, SCREEN_CENTER_X, 132.f);

    blitzHeaderText.setFont(font);
    blitzHeaderText.setString(utf8Text("Block chớp"));
    blitzHeaderText.setCharacterSize(18);
    blitzHeaderText.setFillColor(sf::Color(255, 230, 120));
    centerTextAt(blitzHeaderText, SCREEN_CENTER_X, 168.f);

    fastHeaderText.setFont(font);
    fastHeaderText.setString(utf8Text("Block nhanh"));
    fastHeaderText.setCharacterSize(18);
    fastHeaderText.setFillColor(sf::Color(180, 255, 230));
    centerTextAt(fastHeaderText, SCREEN_CENTER_X, 268.f);

    hourHeaderText.setFont(font);
    hourHeaderText.setString(utf8Text("Theo giờ"));
    hourHeaderText.setCharacterSize(18);
    hourHeaderText.setFillColor(sf::Color(190, 220, 255));
    centerTextAt(hourHeaderText, SCREEN_CENTER_X, 368.f);

    dayHeaderText.setFont(font);
    dayHeaderText.setString(utf8Text("Theo ngày"));
    dayHeaderText.setCharacterSize(18);
    dayHeaderText.setFillColor(sf::Color(255, 200, 160));
    centerTextAt(dayHeaderText, SCREEN_CENTER_X, 468.f);

    const sf::Color blitzFill(255, 206, 91);
    const sf::Color blitzOutline(220, 160, 40);
    const sf::Color fastFill(43, 214, 190);
    const sf::Color fastOutline(19, 153, 134);
    const sf::Color hourFill(120, 180, 255);
    const sf::Color hourOutline(70, 120, 200);
    const sf::Color dayFill(255, 140, 90);
    const sf::Color dayOutline(200, 90, 40);

    int id = 0;
    addOption(id++, "1 phút", gridX(0), 192.f, BTN_W, BTN_H, blitzFill, blitzOutline);
    addOption(id++, "3 phút", gridX(1), 192.f, BTN_W, BTN_H, blitzFill, blitzOutline);
    addOption(id++, "5 phút", gridX(2), 192.f, BTN_W, BTN_H, blitzFill, blitzOutline);

    addOption(id++, "10 phút", gridX(0), 292.f, BTN_W, BTN_H, fastFill, fastOutline);
    addOption(id++, "15 phút", gridX(1), 292.f, BTN_W, BTN_H, fastFill, fastOutline);
    addOption(id++, "30 phút", gridX(2), 292.f, BTN_W, BTN_H, fastFill, fastOutline);

    addOption(id++, "1 giờ", gridX(0), 392.f, BTN_W, BTN_H, hourFill, hourOutline);
    addOption(id++, "3 giờ", gridX(1), 392.f, BTN_W, BTN_H, hourFill, hourOutline);
    addOption(id++, "5 giờ", gridX(2), 392.f, BTN_W, BTN_H, hourFill, hourOutline);

    addOption(id++, "1 ngày", gridX(0), 492.f, BTN_W, BTN_H, dayFill, dayOutline);
    addOption(id++, "3 ngày", gridX(1), 492.f, BTN_W, BTN_H, dayFill, dayOutline);
    addOption(id++, "7 ngày", gridX(2), 492.f, BTN_W, BTN_H, dayFill, dayOutline);
}

DuelSelectScreen::~DuelSelectScreen() {}

void DuelSelectScreen::addOption(int id, const char* label, float x, float y, float width, float height,
                                 const sf::Color& fill, const sf::Color& outline) {
    MilestoneOption opt;
    opt.id = id;
    opt.button.setSize(sf::Vector2f(width, height));
    opt.button.setPosition(x, y);
    opt.button.setFillColor(fill);
    opt.button.setOutlineColor(outline);
    opt.button.setOutlineThickness(2.f);

    opt.label.setFont(font);
    opt.label.setString(utf8Text(label));
    opt.label.setCharacterSize(18);
    opt.label.setFillColor(sf::Color::White);
    centerTextInRect(opt.label, sf::FloatRect(x, y, width, height));

    options.push_back(opt);
}

void DuelSelectScreen::render(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(panel);
    window.draw(backButton);
    window.draw(backText);
    window.draw(titleText);
    window.draw(blitzHeaderText);
    window.draw(fastHeaderText);
    window.draw(hourHeaderText);
    window.draw(dayHeaderText);

    for (const auto& opt : options) {
        if (opt.id == selectedId) {
            sf::RectangleShape highlight = opt.button;
            highlight.setOutlineColor(sf::Color::White);
            highlight.setOutlineThickness(4.f);
            window.draw(highlight);
        } else {
            window.draw(opt.button);
        }
        window.draw(opt.label);
    }
}

bool DuelSelectScreen::handleClick(sf::Vector2f mousePos) {
    if (isBackClicked(mousePos)) {
        return false;
    }

    for (const auto& opt : options) {
        if (opt.button.getGlobalBounds().contains(mousePos)) {
            selectedId = opt.id;
            return true;
        }
    }
    return false;
}

bool DuelSelectScreen::isBackClicked(sf::Vector2f mousePos) const {
    return backButton.getGlobalBounds().contains(mousePos);
}

int DuelSelectScreen::selectedMilestoneId() const {
    return selectedId;
}
