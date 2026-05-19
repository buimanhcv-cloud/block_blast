#include "NetworkClient.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
    #pragma warning(disable: 4996)
#endif

NetworkClient::NetworkClient()
    : socket(INVALID_SOCKET), connected(false), receivingActive(false) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
    }
#endif
}

NetworkClient::~NetworkClient() {
    stopReceiveThread();
    disconnect();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool NetworkClient::connect(const std::string& serverAddress, uint16_t port) {
    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        closesocket(socket);
        return false;
    }

    if (::connect(socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed\n";
        closesocket(socket);
        socket = INVALID_SOCKET;
        return false;
    }

    connected = true;
    return true;
}

void NetworkClient::disconnect() {
    stopReceiveThread();
    if (socket != INVALID_SOCKET) {
        closesocket(socket);
        socket = INVALID_SOCKET;
    }
    connected = false;
}

bool NetworkClient::isConnected() const {
    return connected.load();
}

void NetworkClient::sendMessage(const Message& msg) {
    std::lock_guard<std::mutex> lock(sendMutex);
    sendQueue.push(msg);
}

void NetworkClient::setMessageCallback(MessageCallback cb) {
    messageCallback = cb;
}

void NetworkClient::startReceiveThread() {
    if (receivingActive.load()) return;
    receivingActive = true;
    receiveThread = std::thread(&NetworkClient::receiveThreadFunc, this);
}

void NetworkClient::stopReceiveThread() {
    receivingActive = false;
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}

bool NetworkClient::sendRaw(const uint8_t* data, size_t length) {
    if (!connected.load()) return false;
    
    size_t sent = 0;
    while (sent < length) {
        int result = ::send(socket, (const char*)(data + sent), (int)(length - sent), 0);
        if (result == SOCKET_ERROR) {
            connected = false;
            return false;
        }
        sent += result;
    }
    return true;
}

bool NetworkClient::receiveRaw(uint8_t* buffer, size_t length) {
    if (!connected.load()) return false;
    
    size_t received = 0;
    while (received < length) {
        int result = ::recv(socket, (char*)(buffer + received), (int)(length - received), 0);
        if (result <= 0) {
            connected = false;
            return false;
        }
        received += result;
    }
    return true;
}

void NetworkClient::receiveThreadFunc() {
    while (receivingActive.load() && connected.load()) {
        uint8_t lengthBuf[4];
        if (!receiveRaw(lengthBuf, 4)) {
            continue;
        }

        uint32_t msgLength = (lengthBuf[0] << 24) | (lengthBuf[1] << 16) |
                              (lengthBuf[2] << 8) | lengthBuf[3];

        if (msgLength > 1000000) { // Safety check
            connected = false;
            break;
        }

        std::vector<uint8_t> msgData(msgLength);
        if (!receiveRaw(msgData.data(), msgLength)) {
            continue;
        }

        Message msg = deserializeMessage(msgData);
        if (messageCallback) {
            messageCallback(msg);
        }
    }
}

std::vector<uint8_t> NetworkClient::serializeMessage(const Message& msg) {
    std::vector<uint8_t> data;
    data.push_back(static_cast<uint8_t>(msg.type));
    data.insert(data.end(), msg.payload.begin(), msg.payload.end());
    return data;
}

Message NetworkClient::deserializeMessage(const std::vector<uint8_t>& data) {
    Message msg;
    if (data.empty()) return msg;
    
    msg.type = static_cast<MessageType>(data[0]);
    if (data.size() > 1) {
        msg.payload.insert(msg.payload.end(), data.begin() + 1, data.end());
    }
    return msg;
}
