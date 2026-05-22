#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <SFML/Network.hpp>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>
#include <condition_variable>

// ── Binary protocol message types ─────────────────────────────────────────────
enum class MsgType : uint8_t {
    JOIN        = 0x01,
    MOVE        = 0x02,
    READY       = 0x03,
    PING        = 0x04,
    ROOM_JOINED = 0x10,
    GAME_START  = 0x11,
    GAME_STATE  = 0x12,
    SCORE_UPDATE= 0x13,
    TIME_UPDATE = 0x14,
    YOUR_TURN   = 0x15,
    GAME_OVER   = 0x16,
    PONG        = 0x17,
    ERROR_MSG   = 0x18,
};

struct NetPacket {
    MsgType              type;
    std::vector<uint8_t> payload;
};

// ── WebSocket client over sf::TcpSocket ──────────────────────────────────────
// Implements RFC 6455 WebSocket framing (binary frames, client masking).
// Game packets are wrapped inside WS frames; the wire format per game packet
// is still: [1 byte type][2 bytes payload length LE][N bytes payload].
//
// Public API is identical to the old TCP client — no changes needed upstream.
class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    // Blocking connect + WS handshake (call from background thread)
    bool connect(const std::string& host, unsigned short port);
    void disconnect();
    bool isConnected() const;

    // Non-blocking sends (enqueue → send thread)
    bool sendJoin(const std::string& roomId, int mode, int timeLimitSec);
    bool sendMove(int boardX, int boardY,
                  const std::vector<std::vector<int>>& shape);
    bool sendReady();
    bool sendPing();

    // Poll on main thread (once per frame)
    bool pollPacket(NetPacket& out);

private:
    void enqueue(NetPacket pkt);
    void receiveLoop();
    void sendLoop();

    // WebSocket helpers
    bool     wsHandshake(const std::string& host, unsigned short port);
    bool     wsSendBinary(const std::vector<uint8_t>& data);
    bool     wsRecvFrame(std::vector<uint8_t>& out);  // blocks until 1 frame
    bool     recvExact(uint8_t* buf, std::size_t n);

    sf::TcpSocket              socket_;
    std::atomic<bool>          connected_{false};
    std::atomic<bool>          running_{false};

    std::thread                recvThread_;
    std::mutex                 recvMutex_;
    std::queue<NetPacket>      inQueue_;

    std::thread                sendThread_;
    std::mutex                 sendMutex_;
    std::condition_variable    sendCv_;
    std::queue<NetPacket>      outQueue_;
};

#endif // NETWORKCLIENT_H
