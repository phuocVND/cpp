#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <unistd.h>

struct MyData {
    int id;
    float value;
    char name[50];
};

class TcpClient {
public:
    TcpClient(const std::string& server_ip, int server_port);
    ~TcpClient();
    bool isConnected() const;
    void disconnect();


    bool connectToServer();

    void sendMessage(const std::string& message);
    void receiveMessage();

    void sendArray(int* arr, size_t size);
    void receiveArray(int* arr, size_t size);

    void sendStruct(const MyData& data);
    void receiveStruct(MyData& data);

private:
    int sockfd;
    std::string ip;
    int port;
    struct sockaddr_in server_addr;
};

#endif // TCPCLIENT_H
