#ifndef ONLINEGAMESELECTSCREEN_H
#define ONLINEGAMESELECTSCREEN_H

#include <SFML/Graphics.hpp>
#include <string>
#include "ui/GameNetworkManager.h"

// ── Result of the select screen ───────────────────────────────────────────────
enum class SelectResult { NONE, CONNECT, BACK };

// ── Lobby / connection screen before the online game starts ──────────────────
class OnlineGameSelectScreen {
public:
    OnlineGameSelectScreen();
    ~OnlineGameSelectScreen();

    void render(sf::RenderWindow& window);
    SelectResult handleEvent(const sf::Event& event);

    // Settings chosen by the player
    std::string  getRoomId()     const { return roomId_; }
    OnlineMode   getMode()       const { return mode_; }
    TimePreset   getTimePreset() const { return timePreset_; }
    
    std::string  getHost() const {
        size_t colonPos = host_.find(':');
        if (colonPos != std::string::npos) {
            return host_.substr(0, colonPos);
        }
        return host_;
    }

    unsigned short getPort() const {
        size_t colonPos = host_.find(':');
        if (colonPos != std::string::npos) {
            try {
                return static_cast<unsigned short>(std::stoi(host_.substr(colonPos + 1)));
            } catch (...) {
                // fall back to default
            }
        }
        return port_;
    }

    // Status text shown while connecting / waiting
    void setStatus(const std::string& msg) { statusMsg_ = msg; }

private:
    void drawButton(sf::RenderWindow& window, const sf::RectangleShape& btn,
                    const sf::Text& label);
    bool isClicked(const sf::RectangleShape& btn, sf::Vector2f mouse) const;
    void rebuildRoomText();

    sf::Font font_;

    // Mode toggle
    OnlineMode   mode_       = OnlineMode::REALTIME;
    TimePreset   timePreset_ = TimePreset::NORMAL;

    // Room ID (simple 4-char code)
    std::string roomId_      = "AAAA";

    // Server address (editable in UI)
    std::string    host_        = "weight-trustless.gl.at.ply.gg";
    unsigned short port_        = 33412;
    bool           editingHost_ = false;  // true = host field focused

    std::string statusMsg_;

    // UI elements
    sf::RectangleShape bgPanel_;

    sf::RectangleShape modeRealtimeBtn_;
    sf::RectangleShape modeTurnBtn_;

    sf::RectangleShape time1mBtn_;
    sf::RectangleShape time3mBtn_;
    sf::RectangleShape time5mBtn_;

    sf::RectangleShape connectBtn_;
    sf::RectangleShape backBtn_;

    sf::Text titleText_;
    sf::Text modeLabel_;
    sf::Text timeLabel_;
    sf::Text roomLabel_;
    sf::Text roomIdText_;
    sf::Text statusText_;

    sf::Text modeRealtimeTxt_;
    sf::Text modeTurnTxt_;
    sf::Text time1mTxt_;
    sf::Text time3mTxt_;
    sf::Text time5mTxt_;
    sf::Text connectTxt_;
    sf::Text backTxt_;

    // Host input field
    sf::RectangleShape hostInputBox_;
    sf::Text           hostInputTxt_;
    sf::Text           hostLabel_;
};

#endif // ONLINEGAMESELECTSCREEN_H
