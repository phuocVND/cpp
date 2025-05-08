#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
struct MyData {
    int id;
    float value;
    char name[50];
};
class TcpServer {
public:
    // Khởi tạo server với địa chỉ IP và cổng
    TcpServer(const std::string& ip, short port);

    // Bắt đầu lắng nghe kết nối
    void start_accept();

private:
    // Phương thức xử lý client sau khi kết nối
    void handle_client(int client_socket);

    int server_fd;  // Socket cho server
    struct sockaddr_in server_address;  // Địa chỉ của server
};

#endif // TCPSERVER_H
