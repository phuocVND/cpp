#include "tcpclient.h"
#include <iostream>
#include <unistd.h>
#include <cstring>

// Constructor
TCPClient::TCPClient() : sock(0) {
    memset(&serverAddr, 0, sizeof(serverAddr));  // Đảm bảo serverAddr được khởi tạo
}

// Destructor
TCPClient::~TCPClient() {
    closeConnection();  // Đảm bảo đóng kết nối khi đối tượng TCPClient bị hủy
}

// Kết nối đến server
bool TCPClient::connectToServer(const std::string &serverAddress, int port) {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return false;
    }

    serverAddr.sin_family = AF_INET;  // Giao thức IPv4
    serverAddr.sin_port = htons(port);  // Chuyển cổng sang định dạng mạng

    // Chuyển địa chỉ IP từ chuỗi sang định dạng mạng (bằng inet_pton)
    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported!" << std::endl;
        return false;
    }

    // Kết nối đến server
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        return false;
    }

    std::cout << "Connected to server at " << serverAddress << ":" << port << std::endl;
    return true;
}

// Gửi thông điệp đến server
void TCPClient::sendMessage(const std::string &message) {
    send(sock, message.c_str(), message.length(), 0);
    std::cout << "Message sent to server: " << message << std::endl;
}

// Nhận thông điệp từ server
std::string TCPClient::receiveMessage() {
    char buffer[1024] = {0};
    int valread = read(sock, buffer, sizeof(buffer) - 1);
    if (valread >= 0) {
        buffer[valread] = '\0';
        return std::string(buffer);
    } else {
        std::cerr << "Failed to read from server!" << std::endl;
        return "";
    }
}

// Đóng kết nối
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
