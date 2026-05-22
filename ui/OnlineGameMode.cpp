#include "OnlineGameMode.h"
#include "core/GraphicsConfig.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>

// ── Layout constants ──────────────────────────────────────────────────────────
static const float W = 540.f;
static const float H = 960.f;

// Opponent mini-board (centered between board and tray)
static constexpr float OPP_X    = 222.f;
static constexpr float OPP_Y    = 640.f;
static constexpr int   OPP_CELL = 12;

// Tray centres (bottom strip)
static const sf::Vector2f TRAY_CENTERS[3] = {
    {115.f, 820.f},
    {270.f, 820.f},
    {425.f, 820.f},
};

// ── Colour palette ────────────────────────────────────────────────────────────
static const sf::Color COL_BG          (12,  8, 32);          // deep navy bg
static const sf::Color COL_PANEL       (18, 14, 48, 230);     // panel fill
static const sf::Color COL_BORDER_BLUE (60, 120, 255, 200);   // blue border
static const sf::Color COL_BORDER_RED  (220, 60, 80, 200);    // red border
static const sf::Color COL_CYAN        (0, 230, 255);         // my score / clock
static const sf::Color COL_ORANGE      (255, 160, 40);        // opp score / clock
static const sf::Color COL_LABEL       (160, 170, 220);       // dim labels
static const sf::Color COL_GREEN_TURN  (60, 220, 120);        // your turn
static const sf::Color COL_RED_TURN    (220, 60, 80);         // opp turn
static const sf::Color COL_TRAY_SLOT   (28, 18, 58, 210);     // tray slot fill
static const sf::Color COL_TRAY_BORDER (80, 100, 220, 180);   // tray slot border

// ── Constructor ───────────────────────────────────────────────────────────────

OnlineGameMode::OnlineGameMode(sf::RenderWindow& window, GameNetworkManager& net)
    : window_(window), net_(net) {

    loadGameFont(font_);

    if (loadOptionalSoundBuffer(placeBuffer_,    "assets/place.wav"))    placeSound_.setBuffer(placeBuffer_);
    if (loadOptionalSoundBuffer(clearBuffer_,    "assets/clear.wav"))    clearSound_.setBuffer(clearBuffer_);
    if (loadOptionalSoundBuffer(comboBuffer_,    "assets/combo.wav"))    comboSound_.setBuffer(comboBuffer_);
    if (loadOptionalSoundBuffer(gameOverBuffer_, "assets/gameover.wav")) gameOverSound_.setBuffer(gameOverBuffer_);

    myBoard_ = std::make_unique<Board>();

    for (int i = 0; i < TRAY_SIZE; ++i)
        trayPositions_[i] = TRAY_CENTERS[i];

    // ── Opponent board background ─────────────────────────────────────────────
    oppBoardBg_.setSize({static_cast<float>(Board::SIZE * OPP_CELL + 10),
                         static_cast<float>(Board::SIZE * OPP_CELL + 10)});
    oppBoardBg_.setPosition(OPP_X - 5.f, OPP_Y - 5.f);
    oppBoardBg_.setFillColor(sf::Color(20, 10, 40, 240));
    oppBoardBg_.setOutlineColor(COL_BORDER_RED);
    oppBoardBg_.setOutlineThickness(2.f);

    // ── My clock panel (top-left) ─────────────────────────────────────────────
    myClockBg_.setSize({140.f, 48.f});
    myClockBg_.setPosition(8.f, 56.f);
    myClockBg_.setFillColor(sf::Color(10, 30, 60, 220));
    myClockBg_.setOutlineColor(COL_BORDER_BLUE);
    myClockBg_.setOutlineThickness(1.5f);

    // ── Opponent clock panel (top-right) ────────────────────────────────
    oppClockBg_.setSize({140.f, 48.f});
    oppClockBg_.setPosition(392.f, 56.f);
    oppClockBg_.setFillColor(sf::Color(50, 10, 20, 220));
    oppClockBg_.setOutlineColor(COL_BORDER_RED);
    oppClockBg_.setOutlineThickness(1.5f);

    // ── Turn indicator bar ────────────────────────────────────────────────────
    turnIndicator_.setSize({524.f, 5.f});
    turnIndicator_.setPosition(8.f, 110.f);

    // ── Back button ───────────────────────────────────────────────────────────
    backBtn_.setSize({90.f, 36.f});
    backBtn_.setPosition(8.f, 8.f);
    backBtn_.setFillColor(sf::Color(18, 14, 48, 220));
    backBtn_.setOutlineColor(COL_BORDER_BLUE);
    backBtn_.setOutlineThickness(1.5f);

    backBtnTxt_.setFont(font_);
    backBtnTxt_.setString("< MENU");
    backBtnTxt_.setCharacterSize(15);
    backBtnTxt_.setFillColor(sf::Color(160, 190, 255));
    sf::FloatRect tb = backBtnTxt_.getLocalBounds();
    sf::FloatRect bb = backBtn_.getGlobalBounds();
    backBtnTxt_.setPosition(
        bb.left + (bb.width  - tb.width)  / 2.f - tb.left,
        bb.top  + (bb.height - tb.height) / 2.f - tb.top);
}

OnlineGameMode::~OnlineGameMode() {}

// ── Start ─────────────────────────────────────────────────────────────────────

void OnlineGameMode::startGame(OnlineMode mode, int timeLimitSec) {
    mode_           = mode;
    timeLimitSec_   = timeLimitSec;
    myTimeMs_       = timeLimitSec * 1000;
    opponentTimeMs_ = timeLimitSec * 1000;
    gameOver_       = false;
    winnerId_       = -1;
    myScore_        = 0;
    opponentScore_  = 0;
    myCombo_        = 0;
    myTurn_         = (mode_ == OnlineMode::REALTIME);

    myBoard_->reset();
    std::memset(opponentGrid_, 0, sizeof(opponentGrid_));
    refillTray();
    draggingIndex_ = -1;
}

// ── Network callbacks ─────────────────────────────────────────────────────────

void OnlineGameMode::onStateUpdate(const OnlineGameState& state) {
    myScore_        = state.myScore;
    opponentScore_  = state.opponentScore;
    myCombo_        = state.myCombo;
    myTimeMs_       = state.myTimeMs;
    opponentTimeMs_ = state.opponentTimeMs;
    std::memcpy(opponentGrid_, state.opponentGrid, sizeof(opponentGrid_));
}

void OnlineGameMode::onYourTurn() {
    myTurn_ = true;
}

void OnlineGameMode::onGameOver(int winnerId) {
    gameOver_ = true;
    winnerId_ = winnerId;
    playSound(gameOverSound_);
}

// ── Input ─────────────────────────────────────────────────────────────────────

bool OnlineGameMode::isBackClicked(sf::Vector2f p) const {
    return backBtn_.getGlobalBounds().contains(p);
}

void OnlineGameMode::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        if (isBackClicked(mouse)) { wantsBack_ = true; return; }
    }

    if (gameOver_) return;

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));
        for (int i = 0; i < TRAY_SIZE; ++i) {
            if (!trayBlocks_[i]) continue;
            if (trayBlocks_[i]->containsPreview(mouse, trayPositions_[i])) {
                draggingIndex_ = i;
                dragOffset_ = sf::Vector2i(
                    event.mouseButton.x - static_cast<int>(trayBlocks_[i]->getPosition().x),
                    event.mouseButton.y - static_cast<int>(trayBlocks_[i]->getPosition().y));
                break;
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased &&
             event.mouseButton.button == sf::Mouse::Left) {
        if (draggingIndex_ >= 0 && trayBlocks_[draggingIndex_]) {
            if (canPlaceNow()) {
                sf::Vector2i boardPos =
                    myBoard_->getBoardPosition(trayBlocks_[draggingIndex_]->getPosition());
                if (myBoard_->canPlaceBlock(*trayBlocks_[draggingIndex_], boardPos))
                    placeBlock(draggingIndex_, boardPos);
                else
                    snapBack(draggingIndex_);
            } else {
                snapBack(draggingIndex_);
            }
        }
        draggingIndex_ = -1;
    }
    else if (event.type == sf::Event::MouseMoved &&
             draggingIndex_ >= 0 && trayBlocks_[draggingIndex_]) {
        trayBlocks_[draggingIndex_]->setPosition(sf::Vector2f(
            static_cast<float>(event.mouseMove.x - dragOffset_.x),
            static_cast<float>(event.mouseMove.y - dragOffset_.y)));
    }
}

// ── Update ────────────────────────────────────────────────────────────────────

void OnlineGameMode::update(float /*dt*/) {
    if (gameOver_) return;
    myBoard_->update();
}

// ── Render ────────────────────────────────────────────────────────────────────

void OnlineGameMode::render() {
    window_.clear(COL_BG);

    // My board (Board uses its own OFFSET_X/Y)
    myBoard_->render(window_);

    // Ghost preview while dragging
    if (draggingIndex_ >= 0 && trayBlocks_[draggingIndex_] && canPlaceNow()) {
        sf::Vector2i boardPos =
            myBoard_->getBoardPosition(trayBlocks_[draggingIndex_]->getPosition());
        if (myBoard_->canPlaceBlock(*trayBlocks_[draggingIndex_], boardPos))
            myBoard_->renderGhostCells(window_, *trayBlocks_[draggingIndex_], boardPos);
    }

    drawTray();
    drawOpponentBoard();
    drawClocks();
    drawScores();

    if (mode_ == OnlineMode::TURN_BASED)
        drawTurnIndicator();

    window_.draw(backBtn_);
    window_.draw(backBtnTxt_);

    // Dragged block on top
    if (draggingIndex_ >= 0 && trayBlocks_[draggingIndex_])
        trayBlocks_[draggingIndex_]->render(window_);

    // ── Waiting overlay ───────────────────────────────────────────────────────
    if (!gameStarted_) {
        sf::RectangleShape overlay({W, H});
        overlay.setFillColor(sf::Color(0, 0, 0, 190));
        window_.draw(overlay);

        // Pulsing panel
        sf::RectangleShape panel({340.f, 120.f});
        panel.setPosition((W - 340.f) / 2.f, H / 2.f - 70.f);
        panel.setFillColor(sf::Color(18, 14, 48, 240));
        panel.setOutlineColor(COL_BORDER_BLUE);
        panel.setOutlineThickness(2.f);
        window_.draw(panel);

        sf::Text waiting;
        waiting.setFont(font_);
        waiting.setString("Waiting for opponent...");
        waiting.setCharacterSize(24);
        waiting.setFillColor(COL_CYAN);
        waiting.setStyle(sf::Text::Bold);
        sf::FloatRect b = waiting.getLocalBounds();
        waiting.setPosition((W - b.width) / 2.f - b.left, H / 2.f - 56.f);
        window_.draw(waiting);

        sf::Text hint;
        hint.setFont(font_);
        hint.setString("Share your Room ID with a friend");
        hint.setCharacterSize(15);
        hint.setFillColor(COL_LABEL);
        sf::FloatRect bh = hint.getLocalBounds();
        hint.setPosition((W - bh.width) / 2.f - bh.left, H / 2.f - 10.f);
        window_.draw(hint);

        sf::Text hint2;
        hint2.setFont(font_);
        hint2.setString("Press ESC or < MENU to cancel");
        hint2.setCharacterSize(13);
        hint2.setFillColor(sf::Color(120, 130, 160));
        sf::FloatRect bh2 = hint2.getLocalBounds();
        hint2.setPosition((W - bh2.width) / 2.f - bh2.left, H / 2.f + 22.f);
        window_.draw(hint2);
    }

    // ── Game over overlay ─────────────────────────────────────────────────────
    if (gameOver_) {
        sf::RectangleShape overlay({W, H});
        overlay.setFillColor(sf::Color(0, 0, 0, 170));
        window_.draw(overlay);

        sf::RectangleShape panel({360.f, 200.f});
        panel.setPosition((W - 360.f) / 2.f, H / 2.f - 110.f);
        panel.setFillColor(sf::Color(18, 14, 48, 250));
        panel.setOutlineColor(COL_BORDER_BLUE);
        panel.setOutlineThickness(2.5f);
        window_.draw(panel);

        const OnlineGameState& st = net_.getState();
        std::string resultStr;
        sf::Color   resultColor;
        if (winnerId_ == st.myPlayerId) {
            resultStr   = "YOU WIN!";
            resultColor = sf::Color(60, 255, 140);
        } else if (winnerId_ == -1) {
            resultStr   = "DRAW";
            resultColor = sf::Color(255, 210, 60);
        } else {
            resultStr   = "YOU LOSE";
            resultColor = sf::Color(255, 80, 80);
        }

        sf::Text result;
        result.setFont(font_);
        result.setString(resultStr);
        result.setCharacterSize(52);
        result.setStyle(sf::Text::Bold);
        result.setFillColor(resultColor);
        sf::FloatRect rb = result.getLocalBounds();
        result.setPosition((W - rb.width) / 2.f - rb.left, H / 2.f - 100.f);
        window_.draw(result);

        // Score summary
        sf::Text scores;
        scores.setFont(font_);
        scores.setString("You: " + std::to_string(myScore_) +
                         "   Opp: " + std::to_string(opponentScore_));
        scores.setCharacterSize(20);
        scores.setFillColor(COL_LABEL);
        sf::FloatRect sc = scores.getLocalBounds();
        scores.setPosition((W - sc.width) / 2.f - sc.left, H / 2.f - 20.f);
        window_.draw(scores);

        sf::Text sub;
        sub.setFont(font_);
        sub.setString("Press ESC or < MENU to return");
        sub.setCharacterSize(16);
        sub.setFillColor(sf::Color(160, 170, 200));
        sf::FloatRect sb = sub.getLocalBounds();
        sub.setPosition((W - sb.width) / 2.f - sb.left, H / 2.f + 30.f);
        window_.draw(sub);
    }

    window_.display();
}

// ── Private helpers ───────────────────────────────────────────────────────────

bool OnlineGameMode::canPlaceNow() const {
    if (!gameStarted_) return false;
    if (mode_ == OnlineMode::REALTIME) return true;
    return myTurn_;
}

void OnlineGameMode::snapBack(int idx) {
    if (!trayBlocks_[idx]) return;
    const sf::Vector2f sz = trayBlocks_[idx]->getSize();
    trayBlocks_[idx]->setPosition(
        trayPositions_[idx] - sf::Vector2f(sz.x / 2.f, sz.y / 2.f));
}

void OnlineGameMode::placeBlock(int trayIdx, sf::Vector2i boardPos) {
    myBoard_->placeBlock(*trayBlocks_[trayIdx], boardPos);
    playSound(placeSound_);

    net_.sendMove(boardPos.x, boardPos.y, trayBlocks_[trayIdx]->getShape());

    checkForClears();
    trayBlocks_[trayIdx].reset();

    if (mode_ == OnlineMode::TURN_BASED)
        myTurn_ = false;

    bool allEmpty = true;
    for (int i = 0; i < TRAY_SIZE; ++i)
        if (trayBlocks_[i]) { allEmpty = false; break; }
    if (allEmpty) refillTray();

    if (!anyBlockCanBePlaced())
        gameOver_ = true;
}

void OnlineGameMode::refillTray() {
    for (int i = 0; i < TRAY_SIZE; ++i) {
        trayBlocks_[i] = std::make_unique<Block>();
        const sf::Vector2f sz = trayBlocks_[i]->getSize();
        trayBlocks_[i]->setPosition(
            trayPositions_[i] - sf::Vector2f(sz.x / 2.f, sz.y / 2.f));
    }
}

bool OnlineGameMode::anyBlockCanBePlaced() const {
    for (int i = 0; i < TRAY_SIZE; ++i) {
        if (!trayBlocks_[i]) continue;
        if (myBoard_->canPlaceAnyBlock(*trayBlocks_[i])) return true;
    }
    return false;
}

void OnlineGameMode::checkForClears() {
    int lines = myBoard_->clearFullLines();
    if (lines > 0) {
        myScore_ += lines * 10 * (myCombo_ + 1);
        myCombo_++;
        playSound(clearSound_);
        if (myCombo_ > 1) playSound(comboSound_);
    } else {
        myCombo_ = 0;
    }
}

void OnlineGameMode::playSound(sf::Sound& sound) {
    if (sound.getBuffer()) sound.play();
}

// ── Draw helpers ──────────────────────────────────────────────────────────────

static std::string fmtTime(int ms) {
    if (ms < 0) ms = 0;
    int s = ms / 1000;
    std::ostringstream o;
    o << s / 60 << ":" << std::setw(2) << std::setfill('0') << s % 60;
    return o.str();
}

void OnlineGameMode::drawClocks() {
    // ── My clock ──────────────────────────────────────────────────────────────
    window_.draw(myClockBg_);

    sf::Text myLbl;
    myLbl.setFont(font_); myLbl.setCharacterSize(11);
    myLbl.setFillColor(COL_LABEL); myLbl.setStyle(sf::Text::Bold);
    myLbl.setString("MY TIME");
    myLbl.setPosition(14.f, 60.f);
    window_.draw(myLbl);

    sf::Text myTime;
    myTime.setFont(font_); myTime.setCharacterSize(22);
    myTime.setFillColor(COL_CYAN); myTime.setStyle(sf::Text::Bold);
    myTime.setString(fmtTime(myTimeMs_));
    myTime.setPosition(14.f, 72.f);
    window_.draw(myTime);

    // ── Opponent clock ────────────────────────────────────────────────────────
    window_.draw(oppClockBg_);

    sf::Text oppLbl;
    oppLbl.setFont(font_); oppLbl.setCharacterSize(11);
    oppLbl.setFillColor(COL_LABEL); oppLbl.setStyle(sf::Text::Bold);
    oppLbl.setString("OPP TIME");
    oppLbl.setPosition(398.f, 60.f);
    window_.draw(oppLbl);

    sf::Text oppTime;
    oppTime.setFont(font_); oppTime.setCharacterSize(22);
    oppTime.setFillColor(COL_ORANGE); oppTime.setStyle(sf::Text::Bold);
    oppTime.setString(fmtTime(opponentTimeMs_));
    oppTime.setPosition(398.f, 72.f);
    window_.draw(oppTime);
}

void OnlineGameMode::drawScores() {
    // My score (right of my clock)
    sf::Text myScoreLbl;
    myScoreLbl.setFont(font_); myScoreLbl.setCharacterSize(11);
    myScoreLbl.setFillColor(COL_LABEL); myScoreLbl.setStyle(sf::Text::Bold);
    myScoreLbl.setString("YOU");
    myScoreLbl.setPosition(160.f, 60.f);
    window_.draw(myScoreLbl);

    sf::Text myScoreVal;
    myScoreVal.setFont(font_); myScoreVal.setCharacterSize(22);
    myScoreVal.setFillColor(COL_CYAN); myScoreVal.setStyle(sf::Text::Bold);
    myScoreVal.setString(std::to_string(myScore_));
    myScoreVal.setPosition(160.f, 72.f);
    window_.draw(myScoreVal);

    // Opponent score (left of opponent clock)
    sf::Text oppScoreLbl;
    oppScoreLbl.setFont(font_); oppScoreLbl.setCharacterSize(11);
    oppScoreLbl.setFillColor(COL_LABEL); oppScoreLbl.setStyle(sf::Text::Bold);
    oppScoreLbl.setString("OPP");
    oppScoreLbl.setPosition(300.f, 60.f);
    window_.draw(oppScoreLbl);

    sf::Text oppScoreVal;
    oppScoreVal.setFont(font_); oppScoreVal.setCharacterSize(22);
    oppScoreVal.setFillColor(COL_ORANGE); oppScoreVal.setStyle(sf::Text::Bold);
    oppScoreVal.setString(std::to_string(opponentScore_));
    oppScoreVal.setPosition(300.f, 72.f);
    window_.draw(oppScoreVal);
}

void OnlineGameMode::drawTray() {
    // Tray background panel
    sf::RectangleShape trayBg({450.f, 138.f});
    trayBg.setPosition(45.f, 750.f);
    trayBg.setFillColor(sf::Color(20, 15, 50, 190));
    trayBg.setOutlineColor(sf::Color(60, 80, 180, 160));
    trayBg.setOutlineThickness(2.f);
    window_.draw(trayBg);

    // Tray slots
    for (int i = 0; i < TRAY_SIZE; ++i) {
        sf::RectangleShape slot({112.f, 100.f});
        slot.setPosition(TRAY_CENTERS[i].x - 56.f, TRAY_CENTERS[i].y - 50.f);
        slot.setFillColor(COL_TRAY_SLOT);
        slot.setOutlineColor(COL_TRAY_BORDER);
        slot.setOutlineThickness(1.5f);
        window_.draw(slot);
    }

    for (int i = 0; i < TRAY_SIZE; ++i) {
        if (!trayBlocks_[i] || i == draggingIndex_) continue;
        trayBlocks_[i]->renderPreview(window_, trayPositions_[i]);
    }
}

void OnlineGameMode::drawOpponentBoard() {
    window_.draw(oppBoardBg_);

    for (int y = 0; y < Board::SIZE; ++y) {
        for (int x = 0; x < Board::SIZE; ++x) {
            sf::RectangleShape cell(sf::Vector2f(OPP_CELL - 2.f, OPP_CELL - 2.f));
            cell.setPosition(OPP_X + x * OPP_CELL + 1.f,
                             OPP_Y + y * OPP_CELL + 1.f);
            if (opponentGrid_[x][y])
                cell.setFillColor(sf::Color(220, 80, 80));
            else
                cell.setFillColor(sf::Color(35, 20, 50));
            window_.draw(cell);
        }
    }

    sf::Text lbl;
    lbl.setFont(font_); lbl.setCharacterSize(11);
    lbl.setFillColor(COL_LABEL); lbl.setStyle(sf::Text::Bold);
    lbl.setString("OPPONENT BOARD");
    lbl.setPosition(OPP_X - 6.f, OPP_Y - 20.f);
    window_.draw(lbl);
}

void OnlineGameMode::drawTurnIndicator() {
    turnIndicator_.setFillColor(myTurn_ ? COL_GREEN_TURN : COL_RED_TURN);
    window_.draw(turnIndicator_);

    sf::Text txt;
    txt.setFont(font_); txt.setCharacterSize(13);
    txt.setFillColor(myTurn_ ? COL_GREEN_TURN : COL_RED_TURN);
    txt.setStyle(sf::Text::Bold);
    txt.setString(myTurn_ ? "YOUR TURN" : "OPPONENT'S TURN");
    txt.setPosition(8.f, 118.f);
    window_.draw(txt);
}
