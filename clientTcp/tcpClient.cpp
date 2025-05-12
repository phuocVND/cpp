#include "tcpClient.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <csignal>
#include <errno.h>

TcpClient::TcpClient(const std::string& server_ip, int server_port)
    : sockfd(-1), ip(server_ip), port(server_port) {
    signal(SIGPIPE, SIG_IGN); // tránh crash khi server ngắt kết nối

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed\n";
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);
}

TcpClient::~TcpClient() {
    if (sockfd >= 0) {
        close(sockfd);
    }
}

bool TcpClient::connectToServer() {
    if (sockfd >= 0) {
        close(sockfd);  // đóng socket cũ nếu có
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);  // 🔁 tạo socket mới mỗi lần thử
    if (sockfd < 0) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << "\n";
        close(sockfd);
        sockfd = -1;
        return false;
    }

    std::cout << "Connected to server\n";
    return true;
}

bool TcpClient::isConnected() const {
    return sockfd >= 0;
}

void TcpClient::disconnect() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
        std::cout << "Disconnected from server.\n";
    }
}

void TcpClient::sendMessage(const std::string& message) {
    if (sockfd < 0) return;

    ssize_t sent = send(sockfd, message.c_str(), message.size(), 0);
    if (sent == -1) {
        std::cerr << "Send failed: " << strerror(errno) << "\n";
        disconnect();
    }
}

void TcpClient::receiveMessage() {
    if (sockfd < 0) return;

    char buffer[1024];
    ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    } else if (bytes_received == 0) {
        std::cout << "Server closed connection\n";
        disconnect();
    } else {
        std::cerr << "Receive failed: " << strerror(errno) << "\n";
        disconnect();
    }
}

void TcpClient::sendArray(short* arr, size_t size) {
    if (sockfd < 0) return;

    ssize_t sent = send(sockfd, arr, size * sizeof(short), 0);
    if (sent == -1) {
        std::cerr << "Send array failed: " << strerror(errno) << "\n";
        disconnect();
    }
}

void TcpClient::receiveArray(short* arr, size_t size) {
    if (sockfd < 0) return;

    ssize_t bytes_received = recv(sockfd, arr, size * sizeof(short), 0);
    if (bytes_received > 0) {
        std::cout << "Array received:\n";
        for (size_t i = 0; i < size; ++i) {
            std::cout << "arr[" << i << "] = " << arr[i] << "\n";
        }
    } else if (bytes_received == 0) {
        std::cout << "Server closed connection while receiving array\n";
        disconnect();
    } else {
        std::cerr << "Receive array failed: " << strerror(errno) << "\n";
        disconnect();
    }
}

void TcpClient::sendStruct(const MyData& data) {
    if (sockfd < 0) return;

    ssize_t sent = send(sockfd, &data, sizeof(data), 0);
    if (sent == -1) {
        std::cerr << "Send struct failed: " << strerror(errno) << "\n";
        disconnect();
    }
}

void TcpClient::receiveStruct(MyData& data) {
    if (sockfd < 0) return;

    ssize_t bytes_received = recv(sockfd, &data, sizeof(data), 0);
    if (bytes_received > 0) {
        std::cout << "Struct received:\n";
        std::cout << "ID: " << data.id << "\n";
        std::cout << "Value: " << data.value << "\n";
        std::cout << "Name: " << data.name << "\n";
    } else if (bytes_received == 0) {
        std::cout << "Server closed connection while receiving struct\n";
        disconnect();
    } else {
        std::cerr << "Receive struct failed: " << strerror(errno) << "\n";
        disconnect();
    }
}
