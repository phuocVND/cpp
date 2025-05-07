#include "tcpServer.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
void send_array(int client_socket, int* arr, size_t size) {
    ssize_t bytes_sent = send(client_socket, arr, size * sizeof(int), 0);
    if (bytes_sent == -1) {
        std::cerr << "Error sending array to client." << std::endl;
    } else {
        std::cout << "Array sent successfully!" << std::endl;
    }
}

void recv_array(int client_socket, int* arr, size_t size) {
    ssize_t bytes_received = recv(client_socket, arr, size * sizeof(int), 0);
    if (bytes_received > 0) {
        std::cout << "Array received successfully!" << std::endl;
        // Hiển thị giá trị mảng nhận được
        for (size_t i = 0; i < size; ++i) {
            std::cout << "arr[" << i << "] = " << arr[i] << std::endl;
        }
    } else if (bytes_received == 0) {
        std::cout << "Client disconnected!" << std::endl;
    } else {
        std::cerr << "Error receiving array from client." << std::endl;
    }
}

struct MyData {
    int id;
    float value;
    char name[50];
};


void send_struct(int client_socket, const MyData& data) {
    ssize_t bytes_sent = send(client_socket, &data, sizeof(data), 0);
    if (bytes_sent == -1) {
        std::cerr << "Error sending struct to client." << std::endl;
    } else {
        std::cout << "Struct sent successfully!" << std::endl;
    }
}

void recv_struct(int client_socket, MyData& data) {
    ssize_t bytes_received = recv(client_socket, &data, sizeof(data), 0);
    if (bytes_received > 0) {
        std::cout << "Struct received successfully!" << std::endl;
        // Hiển thị các giá trị của struct nhận được
        std::cout << "ID: " << data.id << std::endl;
        std::cout << "Value: " << data.value << std::endl;
        std::cout << "Name: " << data.name << std::endl;
    } else if (bytes_received == 0) {
        std::cout << "Client disconnected!" << std::endl;
    } else {
        std::cerr << "Error receiving struct from client." << std::endl;
    }
}
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

    // Nhận chuỗi từ client
    char buffer[1024];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        std::cout << "Received from client: " << buffer << std::endl;
    } else if (bytes_received == 0) {
        std::cout << "Client disconnected!" << std::endl;
        close(client_socket);
        return;
    } else {
        std::cerr << "Error receiving data from client." << std::endl;
        close(client_socket);
        return;
    }
    while(true) {
        // Gửi mảng sang client
        int arr[5] = {1, 2, 3, 4, 5};
        send_array(client_socket, arr, 5);

        // Gửi struct sang client
        MyData data_to_send = {1, 42.42f, "DataStruct"};
        send_struct(client_socket, data_to_send);

        // ✅ Nhận mảng từ client
        int client_arr[5];
        recv_array(client_socket, client_arr, 5);

        // ✅ Nhận struct từ client
        MyData client_data;
        recv_struct(client_socket, client_data);
    }

    // Đóng kết nối
    // close(client_socket);
}




