#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm> // Để dùng std::min

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define CLIENT_BUFFER_SIZE 50 // Buffer của client là 50

// --- CÁC HÀM TRỢ GIÚP VỚI DEBUG (Không thay đổi) ---

bool sendMessage(int socket_fd, const std::string& message) {
    uint32_t len = message.length();
    std::cout << "DEBUG (sendMessage): Chuẩn bị gửi thông điệp " << len << " byte." << std::endl;
    uint32_t net_len = htonl(len);

    std::cout << "DEBUG (sendMessage): ... Gửi độ dài (4 byte)..." << std::endl;
    if (send(socket_fd, &net_len, sizeof(net_len), 0) < 0) {
        perror("[-] Failed to send message length");
        return false;
    }

    std::cout << "DEBUG (sendMessage): ... Gửi nội dung..." << std::endl;
    if (send(socket_fd, message.c_str(), len, 0) < 0) {
        perror("[-] Failed to send message data");
        return false;
    }
    
    std::cout << "[+] Sent: \"" << message << "\"" << std::endl;
    return true;
}

bool receiveMessage(int socket_fd, std::string& outputMessage, int buffer_size) {
    std::cout << "DEBUG (receiveMessage): Đang chờ nhận độ dài thông điệp (4 byte)..." << std::endl;
    uint32_t net_len;
    int bytes_received = recv(socket_fd, &net_len, sizeof(net_len), 0);
    
    if (bytes_received <= 0) { if (bytes_received == 0) { std::cout << "[!] Peer disconnected gracefully." << std::endl; } else { perror("[-] Failed to receive message length"); } return false; }

    uint32_t len = ntohl(net_len);
    std::cout << "DEBUG (receiveMessage): Đã nhận độ dài. Kích thước mong đợi: " << len << " byte." << std::endl;

    outputMessage.clear();
    outputMessage.reserve(len);
    char buffer[buffer_size];
    int total_received = 0;

    std::cout << "DEBUG (receiveMessage): Bắt đầu vòng lặp nhận dữ liệu với buffer size " << buffer_size << "..." << std::endl;
    while (total_received < len) {
        int bytes_to_read = std::min((int)(len - total_received), buffer_size);
        std::cout << "DEBUG (receiveMessage): ... Vòng lặp: Sẽ đọc tối đa " << bytes_to_read << " byte." << std::endl;

        bytes_received = recv(socket_fd, buffer, bytes_to_read, 0);

        if (bytes_received <= 0) { if (bytes_received == 0) { std::cout << "[!] Peer disconnected during data transmission." << std::endl; } else { perror("[-] Failed to receive message data"); } return false; }
        
        std::cout << "DEBUG (receiveMessage): ... Vòng lặp: Thực tế nhận được " << bytes_received << " byte." << std::endl;
        std::cout << "  >> CHUNK DATA: \"" << std::string(buffer, bytes_received) << "\"" << std::endl;

        outputMessage.append(buffer, bytes_received);
        total_received += bytes_received;
    }

    std::cout << "DEBUG (receiveMessage): Đã nhận đủ. Ghép lại thành công." << std::endl;
    std::cout << "[+] Received: \"" << outputMessage << "\"" << std::endl;
    return true;
}


// --- HÀM MAIN CỦA CLIENT ---

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    std::cout << "[+] Client starting up..." << std::endl;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { return -1; }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) { return -1; }

    std::cout << "[+] Connecting to server..." << std::endl;
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { return -1; }
    
    std::cout << "[+] Connected to server." << std::endl;
    
    std::cout << "\n---------- BƯỚC 1: CLIENT GỬI ----------" << std::endl;
    
    // ======================== THAY ĐỔI Ở ĐÂY ========================
    // Tạo một tin nhắn lớn hơn 300 byte để gửi đi
    std::string base_message = "This is a data chunk from client. "; // 32 byte
    std::string message_to_server = "START_OF_CLIENT_MESSAGE | ";
    while(message_to_server.length() < 300) {
        message_to_server += base_message;
    }
    message_to_server += "END_OF_CLIENT_MESSAGE.";
    // ===============================================================

    if (sendMessage(sock, message_to_server)) {
        std::cout << "\n---------- BƯỚC 2: CLIENT NHẬN ----------" << std::endl;
        std::string server_response;
        receiveMessage(sock, server_response, CLIENT_BUFFER_SIZE);
    }
    
    close(sock);
    std::cout << "\n[+] Connection closed. Client shutting down." << std::endl;
    return 0;
}