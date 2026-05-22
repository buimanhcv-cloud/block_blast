#include "OnlineGameSelectScreen.h"
#include "util/ResourceUtils.h"
#include <sstream>

// ── Layout helpers ────────────────────────────────────────────────────────────
static sf::RectangleShape makeBtn(float x, float y, float w, float h,
                                   sf::Color fill = sf::Color(60, 80, 120)) {
    sf::RectangleShape btn({w, h});
    btn.setPosition(x, y);
    btn.setFillColor(fill);
    btn.setOutlineThickness(2.f);
    btn.setOutlineColor(sf::Color(100, 140, 200));
    return btn;
}

static sf::Text makeText(const sf::Font& font, const std::string& str,
                          unsigned int size, sf::Color color = sf::Color::White) {
    sf::Text t;
    t.setFont(font);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(color);
    return t;
}

static void centreIn(sf::Text& text, const sf::RectangleShape& btn) {
    sf::FloatRect tb = text.getLocalBounds();
    sf::FloatRect bb = btn.getGlobalBounds();
    text.setPosition(bb.left + (bb.width  - tb.width)  / 2.f - tb.left,
                     bb.top  + (bb.height - tb.height) / 2.f - tb.top);
}

// ── Constructor ───────────────────────────────────────────────────────────────

OnlineGameSelectScreen::OnlineGameSelectScreen() {
    loadGameFont(font_);

    bgPanel_ = makeBtn(20.f, 60.f, 500.f, 820.f, sf::Color(20, 20, 40));
    bgPanel_.setOutlineColor(sf::Color(80, 100, 160));

    // Title
    titleText_ = makeText(font_, "ONLINE MULTIPLAYER", 28, sf::Color(100, 200, 255));
    titleText_.setPosition(60.f, 80.f);

    // Mode label + buttons
    modeLabel_ = makeText(font_, "Mode:", 18, sf::Color(180, 180, 180));
    modeLabel_.setPosition(40.f, 160.f);

    modeRealtimeBtn_ = makeBtn(40.f,  190.f, 200.f, 44.f);
    modeTurnBtn_     = makeBtn(260.f, 190.f, 200.f, 44.f);

    modeRealtimeTxt_ = makeText(font_, "Realtime", 16);
    modeTurnTxt_     = makeText(font_, "Turn-Based", 16);
    centreIn(modeRealtimeTxt_, modeRealtimeBtn_);
    centreIn(modeTurnTxt_,     modeTurnBtn_);

    // Time label + buttons
    timeLabel_ = makeText(font_, "Time Limit:", 18, sf::Color(180, 180, 180));
    timeLabel_.setPosition(40.f, 260.f);

    time1mBtn_ = makeBtn(40.f,  290.f, 130.f, 44.f);
    time3mBtn_ = makeBtn(190.f, 290.f, 130.f, 44.f);
    time5mBtn_ = makeBtn(340.f, 290.f, 130.f, 44.f);

    time1mTxt_ = makeText(font_, "1 min",  16);
    time3mTxt_ = makeText(font_, "3 min",  16);
    time5mTxt_ = makeText(font_, "5 min",  16);
    centreIn(time1mTxt_, time1mBtn_);
    centreIn(time3mTxt_, time3mBtn_);
    centreIn(time5mTxt_, time5mBtn_);

    // Room ID
    roomLabel_  = makeText(font_, "Room ID:", 18, sf::Color(180, 180, 180));
    roomLabel_.setPosition(40.f, 370.f);
    roomIdText_ = makeText(font_, roomId_, 28, sf::Color(255, 220, 80));
    roomIdText_.setPosition(40.f, 400.f);

    // Status
    statusText_ = makeText(font_, "", 15, sf::Color(180, 220, 180));
    statusText_.setPosition(40.f, 700.f);

    // Connect / Back
    connectBtn_ = makeBtn(40.f,  750.f, 200.f, 52.f, sf::Color(30, 120, 60));
    backBtn_    = makeBtn(280.f, 750.f, 200.f, 52.f, sf::Color(100, 40, 40));
    connectTxt_ = makeText(font_, "CONNECT", 20);
    backTxt_    = makeText(font_, "BACK",    20);
    centreIn(connectTxt_, connectBtn_);
    centreIn(backTxt_,    backBtn_);

    // Host input field
    hostLabel_ = makeText(font_, "Server IP:", 18, sf::Color(180, 180, 180));
    hostLabel_.setPosition(40.f, 490.f);

    hostInputBox_ = makeBtn(40.f, 520.f, 380.f, 40.f, sf::Color(15, 20, 45));
    hostInputBox_.setOutlineColor(sf::Color(80, 100, 160));

    hostInputTxt_ = makeText(font_, host_, 16, sf::Color(200, 230, 255));
    hostInputTxt_.setPosition(50.f, 530.f);
}

OnlineGameSelectScreen::~OnlineGameSelectScreen() {}

// ── Render ────────────────────────────────────────────────────────────────────

void OnlineGameSelectScreen::render(sf::RenderWindow& window) {
    window.clear(sf::Color(15, 15, 30));
    window.draw(bgPanel_);
    window.draw(titleText_);

    // Mode buttons (highlight selected)
    modeRealtimeBtn_.setFillColor(mode_ == OnlineMode::REALTIME
        ? sf::Color(40, 120, 200) : sf::Color(60, 80, 120));
    modeTurnBtn_.setFillColor(mode_ == OnlineMode::TURN_BASED
        ? sf::Color(40, 120, 200) : sf::Color(60, 80, 120));

    window.draw(modeLabel_);
    drawButton(window, modeRealtimeBtn_, modeRealtimeTxt_);
    drawButton(window, modeTurnBtn_,     modeTurnTxt_);

    // Time buttons (highlight selected)
    time1mBtn_.setFillColor(timePreset_ == TimePreset::FAST   ? sf::Color(40,120,200) : sf::Color(60,80,120));
    time3mBtn_.setFillColor(timePreset_ == TimePreset::NORMAL ? sf::Color(40,120,200) : sf::Color(60,80,120));
    time5mBtn_.setFillColor(timePreset_ == TimePreset::LONG   ? sf::Color(40,120,200) : sf::Color(60,80,120));

    window.draw(timeLabel_);
    drawButton(window, time1mBtn_, time1mTxt_);
    drawButton(window, time3mBtn_, time3mTxt_);
    drawButton(window, time5mBtn_, time5mTxt_);

    // Room ID
    window.draw(roomLabel_);
    window.draw(roomIdText_);

    // Server IP input
    hostInputBox_.setOutlineColor(editingHost_
        ? sf::Color(0, 200, 255) : sf::Color(80, 100, 160));
    window.draw(hostLabel_);
    window.draw(hostInputBox_);
    hostInputTxt_.setString(host_ + (editingHost_ ? "|" : ""));
    window.draw(hostInputTxt_);

    // Instructions
    sf::Text hint = makeText(font_,
        "Tap Server IP to edit  |  Same Room ID = same match", 13,
        sf::Color(120, 120, 140));
    hint.setPosition(40.f, 570.f);
    window.draw(hint);

    sf::Text hint2 = makeText(font_,
        "Use ngrok or Railway to play over internet", 13,
        sf::Color(100, 120, 100));
    hint2.setPosition(40.f, 592.f);
    window.draw(hint2);

    // Status
    statusText_.setString(statusMsg_);
    window.draw(statusText_);

    drawButton(window, connectBtn_, connectTxt_);
    drawButton(window, backBtn_,    backTxt_);

    window.display();
}

// ── Events ────────────────────────────────────────────────────────────────────

SelectResult OnlineGameSelectScreen::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
                           static_cast<float>(event.mouseButton.y));

        if (isClicked(modeRealtimeBtn_, mouse)) mode_ = OnlineMode::REALTIME;
        if (isClicked(modeTurnBtn_,     mouse)) mode_ = OnlineMode::TURN_BASED;
        if (isClicked(time1mBtn_,       mouse)) timePreset_ = TimePreset::FAST;
        if (isClicked(time3mBtn_,       mouse)) timePreset_ = TimePreset::NORMAL;
        if (isClicked(time5mBtn_,       mouse)) timePreset_ = TimePreset::LONG;

        if (isClicked(connectBtn_, mouse)) return SelectResult::CONNECT;
        if (isClicked(backBtn_,    mouse)) return SelectResult::BACK;

        // Toggle host input focus
        editingHost_ = isClicked(hostInputBox_, mouse);
    }

    // Keyboard input
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::BackSpace) {
            if (editingHost_ && !host_.empty()) {
                host_.pop_back();
            } else if (!editingHost_ && !roomId_.empty()) {
                roomId_.pop_back();
                rebuildRoomText();
            }
        }
        // Tab switches focus between host and room fields
        if (event.key.code == sf::Keyboard::Tab) {
            editingHost_ = !editingHost_;
        }
    }
    if (event.type == sf::Event::TextEntered) {
        char c = static_cast<char>(event.text.unicode);
        if (editingHost_) {
            // Allow IP chars: 0-9, dot, letters (for hostnames like ngrok)
            if (host_.size() < 50 && (
                (c >= '0' && c <= '9') || c == '.' || c == ':' || c == '-' ||
                (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
                host_ += c;
            }
        } else {
            // Room ID: A-Z, 0-9, max 6 chars
            if (roomId_.size() < 6 &&
                ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                 (c >= '0' && c <= '9'))) {
                roomId_ += static_cast<char>(std::toupper(c));
                rebuildRoomText();
            }
        }
    }

    return SelectResult::NONE;
}

// ── Private ───────────────────────────────────────────────────────────────────

void OnlineGameSelectScreen::drawButton(sf::RenderWindow& window,
                                         const sf::RectangleShape& btn,
                                         const sf::Text& label) {
    window.draw(btn);
    window.draw(label);
}

bool OnlineGameSelectScreen::isClicked(const sf::RectangleShape& btn,
                                        sf::Vector2f mouse) const {
    return btn.getGlobalBounds().contains(mouse);
}

void OnlineGameSelectScreen::rebuildRoomText() {
    roomIdText_.setString(roomId_.empty() ? "_" : roomId_);
}
