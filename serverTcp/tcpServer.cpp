#include "tcpServer.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <cerrno>

// Hàm gửi mảng
void send_array(int client_socket, int* arr, size_t size) {
    ssize_t bytes_sent = send(client_socket, arr, size * sizeof(int), 0);
    if (bytes_sent == -1) {
        std::cerr << "Error sending array: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Array sent successfully!" << std::endl;
    }
}

// Hàm nhận mảng, trả về true nếu thành công, false nếu client ngắt kết nối hoặc lỗi
bool recv_array(int client_socket, int* arr, size_t size) {
    ssize_t bytes_received = recv(client_socket, arr, size * sizeof(int), 0);
    if (bytes_received > 0) {
        std::cout << "Array received successfully!" << std::endl;
        for (size_t i = 0; i < size; ++i) {
            std::cout << "arr[" << i << "] = " << arr[i] << std::endl;
        }
        return true;
    } else if (bytes_received == 0) {
        std::cout << "Client disconnected!" << std::endl;
        return false;
    } else {
        std::cerr << "Error receiving array: " << strerror(errno) << std::endl;
        return false;
    }
}

// Hàm gửi struct
void send_struct(int client_socket, const MyData& data) {
    ssize_t bytes_sent = send(client_socket, &data, sizeof(data), 0);
    if (bytes_sent == -1) {
        std::cerr << "Error sending struct: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Struct sent successfully!" << std::endl;
    }
}

// Hàm nhận struct, trả về true nếu thành công, false nếu client ngắt kết nối hoặc lỗi
bool recv_struct(int client_socket, MyData& data) {
    ssize_t bytes_received = recv(client_socket, &data, sizeof(data), 0);
    if (bytes_received > 0) {
        std::cout << "Struct received successfully!" << std::endl;
        std::cout << "ID: " << data.id << std::endl;
        std::cout << "Value: " << data.value << std::endl;
        std::cout << "Name: " << data.name << std::endl;
        return true;
    } else if (bytes_received == 0) {
        std::cout << "Client disconnected!" << std::endl;
        return false;
    } else {
        std::cerr << "Error receiving struct: " << strerror(errno) << std::endl;
        return false;
    }
}

TcpServer::TcpServer(const std::string& ip, short port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip.c_str());

    if (bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) == -1) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on " << ip << ":" << port << std::endl;
}

void TcpServer::start_accept() {
    while (true) {
        int client_socket;
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);

        client_socket = accept(server_fd, (struct sockaddr*)&client_address, &client_len);
        if (client_socket == -1) {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }

        std::cout << "Client connected!" << std::endl;

        std::thread t([this, client_socket]() {
            handle_client(client_socket);
            close(client_socket); // Đóng socket trong luồng
        });
        t.detach();
    }
}

void TcpServer::handle_client(int client_socket) {
    // Gửi thông điệp chào
    const char* message = "Hello from TCP server!";
    if (send(client_socket, message, strlen(message), 0) == -1) {
        std::cerr << "Error sending welcome message: " << strerror(errno) << std::endl;
        return;
    }

    // Nhận chuỗi từ client
    char buffer[1024];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        std::cout << "Client disconnected or error: " << (bytes_received == 0 ? "disconnected" : strerror(errno)) << std::endl;
        return;
    }
    buffer[bytes_received] = '\0';
    std::cout << "Received from client: " << buffer << std::endl;

    // Vòng lặp gửi/nhận dữ liệu
    while (true) {
        // Gửi mảng
        int arr[5] = {1, 2, 3, 4, 5};
        send_array(client_socket, arr, 5);

        // Gửi struct
        MyData data_to_send = {1, 42.42f, "DataStruct"};
        send_struct(client_socket, data_to_send);

        // Nhận mảng từ client
        int client_arr[5];
        if (!recv_array(client_socket, client_arr, 5)) {
            break; // Thoát nếu client ngắt kết nối hoặc lỗi
        }

        // Nhận struct từ client
        MyData client_data;
        if (!recv_struct(client_socket, client_data)) {
            break; // Thoát nếu client ngắt kết nối hoặc lỗi
        }
    }

    std::cout << "Closing client connection" << std::endl;
}