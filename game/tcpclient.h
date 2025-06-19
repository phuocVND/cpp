#pragma once

#include <string>
#include <cstdint>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSE_SOCKET closesocket
#else
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
typedef int socket_t;
#define CLOSE_SOCKET close
#endif

class Food;         // forward declarations
class SnakeHandle;

class TCPClient {
public:
    TCPClient();
    ~TCPClient();

    bool connectToServer(const std::string &serverAddress, int port);

    void sendMessage(const std::string &message);
    void sendValue(const uint64_t &value);
    uint64_t receiveValue();
    std::string receiveMessage(uint64_t size);
    bool sendIntArray(const int* data, int count);

    void closeConnection();

    void setFood(Food *myFood);
    void setSnake(SnakeHandle *mySnake);

private:
    Food *m_myFood;
    SnakeHandle *m_mySnake;
    socket_t sock;
    struct sockaddr_in serverAddr;
};
