#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 1234
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Tạo socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Gắn socket vào địa chỉ và port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối
    if (listen(server_socket, 5) < 0) {
        perror("listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // Chấp nhận kết nối từ client
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket < 0) {
        perror("accept failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connected!" << std::endl;
    int sData = 0;
    recv(client_socket, &sData, sizeof(sData), 0);

    sData = htonl(sData);
    std::cout << "Received from client: " << sData << std::endl;
    int total_bytes = 0;
    char repClient[sData];

    int bytes_received = recv(client_socket, &repClient, sData, 0);
    if (bytes_received < 0) {
        perror("recv failed");
        close(client_socket);
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    repClient[bytes_received] = '\0';

    std::cout << "Received from client: " << repClient << std::endl;

    // Gửi phản hồi lại client
    const char* response = "Message received!";
    send(client_socket, response, strlen(response), 0);

    close(client_socket);
    close(server_socket);
    return 0;
}