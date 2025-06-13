#pragma once

#include <string>
#include <sys/socket.h>    // Định nghĩa socket
#include <arpa/inet.h>     // Định nghĩa in_addr, sockaddr_in
#include "food.h"
#include "snakehandle.h"
// Lớp TCPClient để quản lý kết nối client
class TCPClient {
public:
    TCPClient();  // Constructor
    ~TCPClient(); // Destructor

    bool connectToServer(const std::string &serverAddress, int port);
    void sendMessage(const std::string &message);
    std::string receiveMessage();
    void closeConnection();

    void setFood(Food *myFood);
    void setSnake(SnakeHandle *mySnake);

private:

    Food *m_myFood;

    SnakeHandle *m_mySnake;

    int sock;                // Socket descriptor
    struct sockaddr_in serverAddr;  // Địa chỉ và cổng server
};
