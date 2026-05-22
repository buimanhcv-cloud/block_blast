#include "NetworkClient.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <ctime>

// ── Helpers ───────────────────────────────────────────────────────────────────

static uint16_t readU16LE(const uint8_t* p) {
    return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

// Build a game-level frame: [type 1B][len 2B LE][payload NB]
static std::vector<uint8_t> buildGameFrame(const NetPacket& pkt) {
    std::vector<uint8_t> f;
    f.reserve(3 + pkt.payload.size());
    f.push_back(static_cast<uint8_t>(pkt.type));
    uint16_t len = static_cast<uint16_t>(pkt.payload.size());
    f.push_back(len & 0xFF);
    f.push_back((len >> 8) & 0xFF);
    f.insert(f.end(), pkt.payload.begin(), pkt.payload.end());
    return f;
}

// Simple base64 encode (for WS handshake key)
static std::string base64Encode(const uint8_t* data, size_t len) {
    static const char* T =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    for (size_t i = 0; i < len; i += 3) {
        uint32_t v = (uint32_t)data[i] << 16;
        if (i+1 < len) v |= (uint32_t)data[i+1] << 8;
        if (i+2 < len) v |= data[i+2];
        out += T[(v >> 18) & 63];
        out += T[(v >> 12) & 63];
        out += (i+1 < len) ? T[(v >> 6) & 63] : '=';
        out += (i+2 < len) ? T[v & 63]        : '=';
    }
    return out;
}

// ── Constructor / Destructor ──────────────────────────────────────────────────

NetworkClient::NetworkClient() { std::srand(static_cast<unsigned>(std::time(nullptr))); }
NetworkClient::~NetworkClient() { disconnect(); }

// ── Connect + WebSocket handshake ─────────────────────────────────────────────

bool NetworkClient::connect(const std::string& host, unsigned short port) {
    disconnect();

    socket_.setBlocking(true);
    if (socket_.connect(host, port, sf::seconds(8.f)) != sf::Socket::Done) {
        std::cerr << "[Net] TCP connect failed: " << host << ":" << port << "\n";
        return false;
    }

    if (!wsHandshake(host, port)) {
        socket_.disconnect();
        return false;
    }

    connected_ = true;
    running_   = true;
    recvThread_ = std::thread(&NetworkClient::receiveLoop, this);
    sendThread_ = std::thread(&NetworkClient::sendLoop,    this);
    std::cout << "[Net] WebSocket connected to " << host << ":" << port << "\n";
    return true;
}

bool NetworkClient::wsHandshake(const std::string& host, unsigned short port) {
    // Generate random 16-byte key
    uint8_t keyBytes[16];
    for (auto& b : keyBytes) b = static_cast<uint8_t>(std::rand() & 0xFF);
    std::string wsKey = base64Encode(keyBytes, 16);

    // Send HTTP Upgrade request
    std::ostringstream req;
    req << "GET / HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "Upgrade: websocket\r\n"
        << "Connection: Upgrade\r\n"
        << "Sec-WebSocket-Key: " << wsKey << "\r\n"
        << "Sec-WebSocket-Version: 13\r\n"
        << "\r\n";

    std::string reqStr = req.str();
    std::size_t sent = 0, total = 0;
    while (total < reqStr.size()) {
        auto st = socket_.send(reqStr.data() + total, reqStr.size() - total, sent);
        if (st != sf::Socket::Done && st != sf::Socket::Partial) {
            std::cerr << "[Net] WS handshake send failed\n";
            return false;
        }
        total += sent;
    }

    // Read HTTP response until "\r\n\r\n"
    std::string response;
    response.reserve(512);
    uint8_t ch;
    std::size_t got;
    while (response.size() < 4096) {
        auto st = socket_.receive(&ch, 1, got);
        if (st != sf::Socket::Done || got == 0) {
            std::cerr << "[Net] WS handshake recv failed\n";
            return false;
        }
        response += static_cast<char>(ch);
        if (response.size() >= 4 &&
            response.substr(response.size() - 4) == "\r\n\r\n")
            break;
    }

    if (response.find("101") == std::string::npos) {
        std::cerr << "[Net] WS handshake rejected:\n" << response.substr(0, 200) << "\n";
        return false;
    }
    std::cout << "[Net] WebSocket handshake OK\n";
    return true;
}

void NetworkClient::disconnect() {
    bool wasRunning = running_.exchange(false);
    connected_ = false;
    sendCv_.notify_all();
    socket_.disconnect();
    if (wasRunning) {
        if (recvThread_.joinable()) recvThread_.join();
        if (sendThread_.joinable()) sendThread_.join();
    }
}

bool NetworkClient::isConnected() const { return connected_; }

// ── WebSocket frame send (client → server, masked) ────────────────────────────

bool NetworkClient::wsSendBinary(const std::vector<uint8_t>& data) {
    // Build WS frame: FIN=1, opcode=2 (binary), MASK=1
    std::vector<uint8_t> frame;
    frame.push_back(0x82); // FIN + binary opcode

    uint64_t len = data.size();
    if (len < 126) {
        frame.push_back(0x80 | static_cast<uint8_t>(len));
    } else if (len < 65536) {
        frame.push_back(0x80 | 126);
        frame.push_back((len >> 8) & 0xFF);
        frame.push_back(len & 0xFF);
    } else {
        frame.push_back(0x80 | 127);
        for (int i = 7; i >= 0; --i)
            frame.push_back((len >> (i * 8)) & 0xFF);
    }

    // 4-byte masking key
    uint8_t mask[4];
    for (auto& m : mask) m = static_cast<uint8_t>(std::rand() & 0xFF);
    frame.insert(frame.end(), mask, mask + 4);

    // Masked payload
    for (size_t i = 0; i < data.size(); ++i)
        frame.push_back(data[i] ^ mask[i % 4]);

    std::size_t total = 0, sent = 0;
    while (total < frame.size() && running_) {
        auto st = socket_.send(frame.data() + total, frame.size() - total, sent);
        if (st == sf::Socket::Done || st == sf::Socket::Partial) total += sent;
        else return false;
    }
    return true;
}

// ── WebSocket frame receive (server → client, unmasked) ──────────────────────

bool NetworkClient::recvExact(uint8_t* buf, std::size_t n) {
    std::size_t total = 0;
    while (total < n && running_) {
        std::size_t got = 0;
        auto st = socket_.receive(buf + total, n - total, got);
        if (st == sf::Socket::Done) {
            total += got;
        } else if (st == sf::Socket::NotReady) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } else {
            return false;
        }
    }
    return total == n;
}

bool NetworkClient::wsRecvFrame(std::vector<uint8_t>& out) {
    uint8_t hdr[2];
    if (!recvExact(hdr, 2)) return false;

    // bool fin    = (hdr[0] & 0x80) != 0;
    uint8_t opcode = hdr[0] & 0x0F;
    bool    masked = (hdr[1] & 0x80) != 0;
    uint64_t len   = hdr[1] & 0x7F;

    if (len == 126) {
        uint8_t ext[2];
        if (!recvExact(ext, 2)) return false;
        len = (static_cast<uint64_t>(ext[0]) << 8) | ext[1];
    } else if (len == 127) {
        uint8_t ext[8];
        if (!recvExact(ext, 8)) return false;
        len = 0;
        for (int i = 0; i < 8; ++i) len = (len << 8) | ext[i];
    }

    uint8_t maskKey[4] = {};
    if (masked) {
        if (!recvExact(maskKey, 4)) return false;
    }

    out.resize(static_cast<size_t>(len));
    if (len > 0 && !recvExact(out.data(), static_cast<size_t>(len))) return false;

    if (masked)
        for (size_t i = 0; i < out.size(); ++i)
            out[i] ^= maskKey[i % 4];

    // Handle control frames
    if (opcode == 0x8) return false;  // close
    if (opcode == 0x9) {              // ping → pong
        uint8_t pong[2] = {0x8A, 0x00};
        std::size_t s; socket_.send(pong, 2, s);
        out.clear();
    }
    return true;
}

// ── Send helpers (non-blocking, enqueue) ─────────────────────────────────────

void NetworkClient::enqueue(NetPacket pkt) {
    if (!connected_) return;
    { std::lock_guard<std::mutex> lk(sendMutex_); outQueue_.push(std::move(pkt)); }
    sendCv_.notify_one();
}

bool NetworkClient::sendJoin(const std::string& roomId, int mode, int timeLimitSec) {
    NetPacket pkt; pkt.type = MsgType::JOIN;
    pkt.payload.push_back(static_cast<uint8_t>(mode));
    pkt.payload.push_back(static_cast<uint8_t>(timeLimitSec & 0xFF));
    pkt.payload.push_back(static_cast<uint8_t>((timeLimitSec >> 8) & 0xFF));
    pkt.payload.push_back(static_cast<uint8_t>(roomId.size()));
    for (char c : roomId) pkt.payload.push_back(static_cast<uint8_t>(c));
    enqueue(std::move(pkt)); return true;
}

bool NetworkClient::sendMove(int bx, int by,
                             const std::vector<std::vector<int>>& shape) {
    NetPacket pkt; pkt.type = MsgType::MOVE;
    pkt.payload.push_back(static_cast<uint8_t>(bx));
    pkt.payload.push_back(static_cast<uint8_t>(by));
    uint8_t rows = static_cast<uint8_t>(shape.size());
    uint8_t cols = rows > 0 ? static_cast<uint8_t>(shape[0].size()) : 0;
    pkt.payload.push_back(rows); pkt.payload.push_back(cols);
    for (auto& row : shape)
        for (int cell : row)
            pkt.payload.push_back(cell ? 1 : 0);
    enqueue(std::move(pkt)); return true;
}

bool NetworkClient::sendReady() {
    NetPacket pkt; pkt.type = MsgType::READY;
    enqueue(std::move(pkt)); return true;
}

bool NetworkClient::sendPing() {
    NetPacket pkt; pkt.type = MsgType::PING;
    enqueue(std::move(pkt)); return true;
}

// ── Send loop ─────────────────────────────────────────────────────────────────

void NetworkClient::sendLoop() {
    try {
        while (running_) {
            NetPacket pkt;
            {
                std::unique_lock<std::mutex> lk(sendMutex_);
                sendCv_.wait(lk, [this]{ return !outQueue_.empty() || !running_; });
                if (!running_ && outQueue_.empty()) break;
                pkt = std::move(outQueue_.front());
                outQueue_.pop();
            }
            auto frame = buildGameFrame(pkt);
            if (!wsSendBinary(frame)) {
                connected_ = false;
                std::cerr << "[Net] Send error\n";
                break;
            }
        }
    } catch (...) { std::cerr << "[Net] sendLoop exception\n"; }
    std::cout << "[Net] Send loop ended\n";
}

// ── Receive loop ──────────────────────────────────────────────────────────────

void NetworkClient::receiveLoop() {
    try {
        std::vector<uint8_t> wsPayload;
        std::vector<uint8_t> pending; // leftover bytes across WS frames

        while (running_) {
            wsPayload.clear();
            if (!wsRecvFrame(wsPayload)) break;
            if (wsPayload.empty()) continue; // control frame (ping/pong)

            // Append to pending buffer and parse game packets
            pending.insert(pending.end(), wsPayload.begin(), wsPayload.end());

            while (pending.size() >= 3) {
                uint16_t payloadLen = readU16LE(pending.data() + 1);
                size_t   totalLen   = 3 + payloadLen;
                if (pending.size() < totalLen) break;

                NetPacket pkt;
                pkt.type    = static_cast<MsgType>(pending[0]);
                pkt.payload = std::vector<uint8_t>(
                    pending.begin() + 3,
                    pending.begin() + totalLen);
                pending.erase(pending.begin(), pending.begin() + totalLen);

                std::lock_guard<std::mutex> lk(recvMutex_);
                inQueue_.push(std::move(pkt));
            }
        }
    } catch (...) { std::cerr << "[Net] receiveLoop exception\n"; }

    connected_ = false;
    std::cout << "[Net] Receive loop ended\n";
}

// ── Poll (main thread) ────────────────────────────────────────────────────────

bool NetworkClient::pollPacket(NetPacket& out) {
    std::lock_guard<std::mutex> lk(recvMutex_);
    if (inQueue_.empty()) return false;
    out = std::move(inQueue_.front());
    inQueue_.pop();
    return true;
}
