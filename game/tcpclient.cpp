#include "tcpclient.h"
#include <iostream>
#include <unistd.h>
#include <cstring>

TCPClient::TCPClient() : sock(0) {
    memset(&serverAddr, 0, sizeof(serverAddr));
}

TCPClient::~TCPClient() {
    closeConnection();
}

bool TCPClient::connectToServer(const std::string &serverAddress, int port) {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
    std::cout << "Message sent to server: " << msgWithNewline << std::endl;
}

void TCPClient::sendValue(const uint64_t &value) {
    uint64_t networkValue = htonll(value); // chuyển về big-endian để đảm bảo tương thích mạng
    int bytesSent = send(sock, &networkValue, sizeof(networkValue), 0);

    if (bytesSent != sizeof(networkValue)) {
        perror("Lỗi khi gửi dữ liệu (sendValue)");
    } else {
        std::cout << "Sent int (binary) to server: " << value << std::endl;
    }
}

uint64_t TCPClient::receiveValue() {
    uint64_t networkValue;
    ssize_t bytesReceived = recv(sock, &networkValue, sizeof(networkValue), MSG_WAITALL);

    if (bytesReceived != sizeof(networkValue)) {
        perror("Lỗi khi nhận dữ liệu (receiveValue)");
        return 0;  // hoặc throw exception nếu bạn dùng
    }

    uint64_t hostValue = ntohll(networkValue);  // Chuyển ngược lại từ network byte order
    std::cout << "Received int (binary) from server: " << hostValue << std::endl;
    return hostValue;
}

bool TCPClient::sendIntArray(const int* data, int count) {

    int byteSize = count * sizeof(int);
    std::cout << byteSize << std::endl;
    int sent = send(sock, data, byteSize, 0);

    // Kiểm tra xem số byte đã gửi có đúng với kích thước không
    return sent == byteSize;
}

std::string TCPClient::receiveMessage(uint64_t size) {
    char buffer[size];
    int valread = recv(sock, buffer, sizeof(buffer), 0);

    if (valread > 0) {
        buffer[valread] = '\0';  // đảm bảo kết thúc chuỗi
        return std::string(buffer);
    } else if (valread == 0) {
        std::cerr << "Server đóng kết nối." << std::endl;
        close(sock);
        return "";
    } else {
        perror("recv lỗi");
        return "";
    }
}

void TCPClient::closeConnection() {
    if (sock != 0) {
        close(sock);
        std::cout << "Connection closed!" << std::endl;
    }
}

void TCPClient::setFood(Food *myFood){
    m_myFood = myFood;
}

void TCPClient::setSnake(SnakeHandle *mySnake){
    m_mySnake = mySnake;
}
