#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
using namespace std;
#define PORT 1234
#define SERVER_IP "0.0.0.0"
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Tạo socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    std::string message = "Hello, server! This is a test message."
                            "Hello, server! This is a test message."
                            "Hello, server! This is a test message.";
                            
    int sData = message.length();
    sData = htonl(sData);
    send(sock, &sData, sizeof(sData), 0);

    send(sock, message.c_str(), message.length(), 0);
    std::cout << "Sent message: " << message << std::endl;

    int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("recv failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';  // Đảm bảo chuỗi kết thúc đúng
    std::cout << "Received from server: " << buffer << std::endl;

    close(sock);
    return 0;
}
