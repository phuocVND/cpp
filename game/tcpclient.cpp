#include "tcpclient.h"
#include <iostream>
#include <cstring>
#include <vector>

// Định nghĩa htonll & ntohll nếu cần (chỉ định nghĩa nếu chưa có)
#ifndef _WIN32
#ifndef htonll
uint64_t htonll(uint64_t value) {
    static const int num = 42;
    if (*(const char*)(&num) == num) { // little endian
        return ((uint64_t)htonl(value & 0xFFFFFFFF) << 32) | htonl(value >> 32);
    } else {
        return value;
    }
}

uint64_t ntohll(uint64_t value) {
    static const int num = 42;
    if (*(const char*)(&num) == num) { // little endian
        return ((uint64_t)ntohl(value & 0xFFFFFFFF) << 32) | ntohl(value >> 32);
    } else {
        return value;
    }
}
#endif
#endif


TCPClient::TCPClient() : sock(0), m_myFood(nullptr), m_mySnake(nullptr) {
    memset(&serverAddr, 0, sizeof(serverAddr));
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
    }
#endif
}

TCPClient::~TCPClient() {
    closeConnection();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool TCPClient::connectToServer(const std::string &serverAddress, int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported!" << std::endl;
        return false;
    }

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        return false;
    }

    std::cout << "Connected to server at " << serverAddress << ":" << port << std::endl;
    return true;
}

void TCPClient::sendMessage(const std::string &message) {
    std::string msgWithNewline = message;
    if (!message.empty() && message.back() != '\n')
        msgWithNewline += '\n';

    send(sock, msgWithNewline.c_str(), msgWithNewline.length(), 0);
}

void TCPClient::sendValue(const uint64_t &value) {
    uint64_t networkValue = htonll(value);
    int bytesSent = send(sock, (const char*)&networkValue, sizeof(networkValue), 0);

    if (bytesSent != sizeof(networkValue)) {
        std::cerr << "Lỗi khi gửi dữ liệu (sendValue)" << std::endl;
    }
}

uint64_t TCPClient::receiveValue() {
    uint64_t networkValue;
    int bytesReceived = recv(sock, (char*)&networkValue, sizeof(networkValue), 0);

    if (bytesReceived != sizeof(networkValue)) {
        std::cerr << "Lỗi khi nhận dữ liệu (receiveValue)" << std::endl;
        return 0;
    }

    return ntohll(networkValue);
}

std::string TCPClient::receiveMessage(uint64_t size) {
    std::vector<char> buffer(size + 1, 0);
    int valread = recv(sock, buffer.data(), size, 0);

    if (valread > 0) {
        buffer[valread] = '\0';
        return std::string(buffer.data());
    } else if (valread == 0) {
        std::cerr << "Server đóng kết nối." << std::endl;
        CLOSE_SOCKET(sock);
        return "";
    } else {
        std::cerr << "recv lỗi" << std::endl;
        return "";
    }
}

bool TCPClient::sendIntArray(const int* data, int count) {
    int byteSize = count * sizeof(int);
    int sent = send(sock, (const char*)data, byteSize, 0);
    return sent == byteSize;
}

void TCPClient::closeConnection() {
    if (sock != 0) {
        CLOSE_SOCKET(sock);
        std::cout << "Connection closed!" << std::endl;
    }
}

void TCPClient::setFood(Food *myFood) {
    m_myFood = myFood;
}

void TCPClient::setSnake(SnakeHandle *mySnake) {
    m_mySnake = mySnake;
}
