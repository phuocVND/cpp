#include "tcpServer.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

TcpServer::TcpServer(const std::string& ip, short port) {
    // Tạo socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Cấu hình địa chỉ server
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);  // Chuyển đổi cổng sang dạng network byte order
    server_address.sin_addr.s_addr = inet_addr(ip.c_str());  // Địa chỉ IP của server

    // Gắn socket với địa chỉ server
    if (bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Bind failed" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Bắt đầu lắng nghe kết nối từ client
    if (listen(server_fd, 3) == -1) {
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on " << ip << ":" << port << std::endl;
}

void TcpServer::start_accept() {
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    // Chấp nhận kết nối từ client
    client_socket = accept(server_fd, (struct sockaddr*)&client_address, &client_len);
    if (client_socket == -1) {
        std::cerr << "Accept failed" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Thông báo kết nối thành công
    std::cout << "Client connected!" << std::endl;

    // Xử lý client
    handle_client(client_socket);
}

void TcpServer::handle_client(int client_socket) {
    // Gửi thông điệp đến client
    const char* message = "Hello from TCP server!";
    send(client_socket, message, strlen(message), 0);

    // Nhận dữ liệu từ client
    char buffer[1024];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        std::cout << "Received from client: " << buffer << std::endl;
    }   else if (bytes_received == 0) {
        // Client đã đóng kết nối
        std::cout << "Client disconnected!" << std::endl;
    } else {
        // Lỗi khi nhận dữ liệu
        std::cerr << "Error receiving data from client." << std::endl;
    }

    // Đóng kết nối với client
    close(client_socket);
}
