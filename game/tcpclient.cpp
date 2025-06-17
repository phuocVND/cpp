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
    send(sock, message.c_str(), message.length(), 0);
    std::cout << "Message sent to server: " << message << std::endl;
}

std::string TCPClient::receiveMessage() {
    char buffer[1024] = {0};
    int valread = recv(sock, buffer, sizeof(buffer), 0);

    if (valread > 0) {
        buffer[valread] = '\0';  // đảm bảo kết thúc chuỗi
        return std::string(buffer);
    } else if (valread == 0) {
        std::cerr << "Server đóng kết nối." << std::endl;
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
