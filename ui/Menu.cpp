#include "Menu.h"
#include "core/GraphicsConfig.h"
#include "util/ResourceUtils.h"

// ── Helper: draw gradient rectangle (fake gradient with overlays) ────────────
static void drawGradientRect(sf::RenderWindow& window, float x, float y, float w, float h,
                              sf::Color top, sf::Color bottom) {
    // Draw 2 triangles to simulate gradient
    sf::ConvexShape grad(4);
    grad.setPoint(0, sf::Vector2f(x, y));
    grad.setPoint(1, sf::Vector2f(x + w, y));
    grad.setPoint(2, sf::Vector2f(x + w, y + h));
    grad.setPoint(3, sf::Vector2f(x, y + h));
    grad.setFillColor(top);
    window.draw(grad);
    
    // Overlay for gradient effect
    sf::RectangleShape overlay(sf::Vector2f(w, h));
    overlay.setPosition(x, y);
    overlay.setFillColor(bottom);
    overlay.setFillColor(sf::Color(bottom.r, bottom.g, bottom.b, 100));
    window.draw(overlay);
}

// ── Helper: centre text inside a button ──────────────────────────────────────
static void centreText(sf::Text& text, const sf::RectangleShape& btn) {
    sf::FloatRect tb = text.getLocalBounds();
    sf::FloatRect bb = btn.getGlobalBounds();
    text.setPosition(
        bb.left + (bb.width  - tb.width)  / 2.f - tb.left,
        bb.top  + (bb.height - tb.height) / 2.f - tb.top);
}

// ── Constructor ───────────────────────────────────────────────────────────────
Menu::Menu() {
    loadGameFont(font);

    const float W = static_cast<float>(GraphicsConfig::WINDOW_WIDTH);   // 540
    const float H = static_cast<float>(GraphicsConfig::WINDOW_HEIGHT);  // 960

    // ── Background: gradient blue → purple ──────────────────────────────────
    background.setSize({W, H});
    background.setFillColor(sf::Color(20, 40, 100));  // dark blue base

    // ── Title with glow effect ──────────────────────────────────────────────
    titleLine1.setFont(font);
    titleLine1.setString("BLOCK");
    titleLine1.setCharacterSize(110);
    titleLine1.setStyle(sf::Text::Bold);
    titleLine1.setFillColor(sf::Color(100, 255, 200));  // cyan
    {
        sf::FloatRect b = titleLine1.getLocalBounds();
        titleLine1.setPosition((W - b.width) / 2.f - b.left, 60.f);
    }

    titleLine2.setFont(font);
    titleLine2.setString("BLAST");
    titleLine2.setCharacterSize(108);
    titleLine2.setStyle(sf::Text::Bold);
    titleLine2.setFillColor(sf::Color(255, 120, 200));  // magenta
    {
        sf::FloatRect b = titleLine2.getLocalBounds();
        titleLine2.setPosition((W - b.width) / 2.f - b.left, 170.f);
    }

    // ── Menu buttons (gradient + rounded effect) ───────────────────────────
    const float BW = 300.f, BH = 64.f;
    const float BX = (W - BW) / 2.f;
    const float spacing = 80.f;

    // SOLO button (teal gradient)
    soloBtn.setSize({BW, BH});
    soloBtn.setPosition(BX, 320.f);
    soloBtn.setFillColor(sf::Color(40, 200, 160));
    soloBtn.setOutlineThickness(3.f);
    soloBtn.setOutlineColor(sf::Color(100, 255, 220));

    soloTxt.setFont(font);
    soloTxt.setString("SOLO CLASSIC");
    soloTxt.setCharacterSize(28);
    soloTxt.setFillColor(sf::Color::White);
    soloTxt.setStyle(sf::Text::Bold);
    centreText(soloTxt, soloBtn);

    // VS AI button (orange gradient)
    vsAiBtn.setSize({BW, BH});
    vsAiBtn.setPosition(BX, 320.f + spacing);
    vsAiBtn.setFillColor(sf::Color(220, 140, 60));
    vsAiBtn.setOutlineThickness(3.f);
    vsAiBtn.setOutlineColor(sf::Color(255, 200, 120));

    vsAiTxt.setFont(font);
    vsAiTxt.setString("VS COMPUTER");
    vsAiTxt.setCharacterSize(28);
    vsAiTxt.setFillColor(sf::Color::White);
    vsAiTxt.setStyle(sf::Text::Bold);
    centreText(vsAiTxt, vsAiBtn);

    // ONLINE button (blue gradient)
    onlineBtn.setSize({BW, BH});
    onlineBtn.setPosition(BX, 320.f + spacing * 2.f);
    onlineBtn.setFillColor(sf::Color(80, 160, 255));
    onlineBtn.setOutlineThickness(3.f);
    onlineBtn.setOutlineColor(sf::Color(150, 200, 255));

    onlineTxt.setFont(font);
    onlineTxt.setString("ONLINE PVP");
    onlineTxt.setCharacterSize(28);
    onlineTxt.setFillColor(sf::Color::White);
    onlineTxt.setStyle(sf::Text::Bold);
    centreText(onlineTxt, onlineBtn);

    // QUIT button (red gradient)
    quitBtn.setSize({BW, BH});
    quitBtn.setPosition(BX, 320.f + spacing * 3.f);
    quitBtn.setFillColor(sf::Color(200, 80, 80));
    quitBtn.setOutlineThickness(3.f);
    quitBtn.setOutlineColor(sf::Color(255, 150, 150));

    quitTxt.setFont(font);
    quitTxt.setString("QUIT");
    quitTxt.setCharacterSize(28);
    quitTxt.setFillColor(sf::Color::White);
    quitTxt.setStyle(sf::Text::Bold);
    centreText(quitTxt, quitBtn);

    // ── Game-over overlay ─────────────────────────────────────────────────────
    overlay.setSize({W, H});
    overlay.setFillColor(sf::Color(0, 0, 0, 220));

    gameOverPanel.setSize({420.f, 400.f});
    gameOverPanel.setPosition((W - 420.f) / 2.f, 200.f);
    gameOverPanel.setFillColor(sf::Color(30, 50, 120));
    gameOverPanel.setOutlineColor(sf::Color(100, 200, 255));
    gameOverPanel.setOutlineThickness(4.f);

    gameOverTitle.setFont(font);
    gameOverTitle.setString("GAME OVER");
    gameOverTitle.setCharacterSize(56);
    gameOverTitle.setStyle(sf::Text::Bold);
    gameOverTitle.setFillColor(sf::Color(255, 100, 120));
    {
        sf::FloatRect b = gameOverTitle.getLocalBounds();
        gameOverTitle.setPosition((W - b.width) / 2.f - b.left, 220.f);
    }

    scoreText.setFont(font);
    scoreText.setCharacterSize(34);
    scoreText.setFillColor(sf::Color(100, 255, 200));
    scoreText.setStyle(sf::Text::Bold);

    highScoreText.setFont(font);
    highScoreText.setCharacterSize(26);
    highScoreText.setFillColor(sf::Color(255, 200, 100));

    newBestText.setFont(font);
    newBestText.setString("⭐ NEW BEST! ⭐");
    newBestText.setCharacterSize(24);
    newBestText.setStyle(sf::Text::Bold);
    newBestText.setFillColor(sf::Color(255, 255, 100));
    {
        sf::FloatRect b = newBestText.getLocalBounds();
        newBestText.setPosition((W - b.width) / 2.f - b.left, 420.f);
    }

    // Restart button (teal)
    restartBtn.setSize({200.f, 58.f});
    restartBtn.setPosition((W - 200.f) / 2.f, 470.f);
    restartBtn.setFillColor(sf::Color(40, 200, 160));
    restartBtn.setOutlineThickness(2.f);
    restartBtn.setOutlineColor(sf::Color(100, 255, 220));

    restartTxt.setFont(font);
    restartTxt.setString("PLAY AGAIN");
    restartTxt.setCharacterSize(24);
    restartTxt.setFillColor(sf::Color::White);
    restartTxt.setStyle(sf::Text::Bold);
    centreText(restartTxt, restartBtn);

    // Back to menu button (purple)
    backMenuBtn.setSize({200.f, 52.f});
    backMenuBtn.setPosition((W - 200.f) / 2.f, 545.f);
    backMenuBtn.setFillColor(sf::Color(120, 80, 180));
    backMenuBtn.setOutlineThickness(2.f);
    backMenuBtn.setOutlineColor(sf::Color(180, 150, 255));

    backMenuTxt.setFont(font);
    backMenuTxt.setString("MAIN MENU");
    backMenuTxt.setCharacterSize(22);
    backMenuTxt.setFillColor(sf::Color::White);
    backMenuTxt.setStyle(sf::Text::Bold);
    centreText(backMenuTxt, backMenuBtn);
}

Menu::~Menu() {}

// ── Render main menu ──────────────────────────────────────────────────────────
void Menu::render(sf::RenderWindow& window) {
    window.draw(background);
    
    // Draw title with shadow effect
    sf::Text shadow = titleLine1;
    shadow.setFillColor(sf::Color(0, 0, 0, 100));
    shadow.setPosition(titleLine1.getPosition().x + 2.f, titleLine1.getPosition().y + 2.f);
    window.draw(shadow);
    window.draw(titleLine1);

    shadow = titleLine2;
    shadow.setFillColor(sf::Color(0, 0, 0, 100));
    shadow.setPosition(titleLine2.getPosition().x + 2.f, titleLine2.getPosition().y + 2.f);
    window.draw(shadow);
    window.draw(titleLine2);

    // Draw buttons
    window.draw(soloBtn);
    window.draw(soloTxt);
    window.draw(vsAiBtn);
    window.draw(vsAiTxt);
    window.draw(onlineBtn);
    window.draw(onlineTxt);
    window.draw(quitBtn);
    window.draw(quitTxt);
}

// ── Render game-over overlay ──────────────────────────────────────────────────
void Menu::renderGameOver(sf::RenderWindow& window, int score, int highScore) {
    const float W = static_cast<float>(GraphicsConfig::WINDOW_WIDTH);

    window.draw(overlay);
    window.draw(gameOverPanel);
    window.draw(gameOverTitle);

    scoreText.setString("Score: " + std::to_string(score));
    {
        sf::FloatRect b = scoreText.getLocalBounds();
        scoreText.setPosition((W - b.width) / 2.f - b.left, 310.f);
    }
    window.draw(scoreText);

    highScoreText.setString("Best: " + std::to_string(highScore));
    {
        sf::FloatRect b = highScoreText.getLocalBounds();
        highScoreText.setPosition((W - b.width) / 2.f - b.left, 360.f);
    }
    window.draw(highScoreText);

    if (score > 0 && score >= highScore)
        window.draw(newBestText);

    window.draw(restartBtn);
    window.draw(restartTxt);
    window.draw(backMenuBtn);
    window.draw(backMenuTxt);
}

// ── Click handlers ────────────────────────────────────────────────────────────
MenuAction Menu::handleClick(sf::Vector2f p) {
    if (soloBtn.getGlobalBounds().contains(p))   return MenuAction::SOLO;
    if (vsAiBtn.getGlobalBounds().contains(p))   return MenuAction::VS_AI;
    if (onlineBtn.getGlobalBounds().contains(p)) return MenuAction::ONLINE;
    if (quitBtn.getGlobalBounds().contains(p))   return MenuAction::QUIT;
    return MenuAction::NONE;
}

bool Menu::isRestartClicked(sf::Vector2f p) {
    return restartBtn.getGlobalBounds().contains(p);
}

bool Menu::isBackToMenuClicked(sf::Vector2f p) {
    return backMenuBtn.getGlobalBounds().contains(p);
}
