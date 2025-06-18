#pragma once

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "food.h"
#include "snakehandle.h"

class TCPClient {
public:
    TCPClient();
    ~TCPClient();

    bool connectToServer(const std::string &serverAddress, int port);

    void sendMessage(const std::string &message);

    void sendValue(const uint64_t &value);
    uint64_t receiveValue();
    std::string receiveMessage(uint64_t size);

    void closeConnection();

    void setFood(Food *myFood);

    void setSnake(SnakeHandle *mySnake);

    bool sendIntArray(const int* data, int count);

private:

    Food *m_myFood;

    SnakeHandle *m_mySnake;

    int sock;

    struct sockaddr_in serverAddr;
};
