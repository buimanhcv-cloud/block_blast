#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
    #pragma comment(lib, "Mswsock.lib")
    #pragma comment(lib, "AdvApi32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
    typedef int SOCKET;
#endif

enum class MessageType : uint8_t {
    HANDSHAKE = 0x01,
    JOIN_GAME = 0x02,
    MOVE = 0x03,
    SCORE_UPDATE = 0x04,
    GAME_STATE = 0x05,
    TIME_UPDATE = 0x06,
    GAME_OVER = 0x07,
    OPPONENT_DISCONNECT = 0x08,
    PING = 0x09,
    PONG = 0x0A,
    ERROR_MSG = 0xFF
};

struct Message {
    MessageType type;
    std::vector<uint8_t> payload;
};

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    bool connect(const std::string& serverAddress, uint16_t port);
    void disconnect();
    bool isConnected() const;

    void sendMessage(const Message& msg);
    using MessageCallback = std::function<void(const Message&)>;
    void setMessageCallback(MessageCallback cb);

    void startReceiveThread();
    void stopReceiveThread();

private:
    SOCKET socket;
    std::atomic<bool> connected;
    std::atomic<bool> receivingActive;
    std::thread receiveThread;

    std::queue<Message> sendQueue;
    std::mutex sendMutex;
    std::mutex receiveMutex;

    MessageCallback messageCallback;

    bool sendRaw(const uint8_t* data, size_t length);
    bool receiveRaw(uint8_t* buffer, size_t length);
    void receiveThreadFunc();

    std::vector<uint8_t> serializeMessage(const Message& msg);
    Message deserializeMessage(const std::vector<uint8_t>& data);
};

#endif // NETWORKCLIENT_H
