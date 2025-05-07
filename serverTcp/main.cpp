#include "tcpServer.h"
#include <string>
#include <iostream>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
class ServerThread {
    public:
        ServerThread(const std::string& ip, short port) : server(ip, port) {}
    
        // Hàm này sẽ được gọi trong thread
        static void* start_server(void* arg) {
            ServerThread* server_thread = static_cast<ServerThread*>(arg);
            while (true) {
                server_thread->server.start_accept();  // Chấp nhận kết nối từ client
            }
            return nullptr;
        }
    
        // Chạy server trong thread
        void run() {
            // Tạo và bắt đầu thread
            if (pthread_create(&thread_id, nullptr, start_server, this) != 0) {
                std::cerr << "Failed to create thread!" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cout << "Server is running in a separate thread." << std::endl;
        }
    
        ~ServerThread() {
            pthread_join(thread_id, nullptr);  // Đợi cho thread hoàn thành
        }
    
    private:
        TcpServer server;
        pthread_t thread_id;  // ID của thread
    };

int main(int argc, char *argv[])
{
    ServerThread server_thread("127.0.0.1", 8080);
    server_thread.run();  // Chạy server trong thread

    // Main thread có thể thực hiện công việc khác hoặc dừng lại
    std::cout << "Main thread is running." << std::endl;

    while (true) {
        // Bạn có thể thêm code xử lý công việc khác ở đây, hoặc chỉ cần chờ
        // server tiếp tục lắng nghe kết nối
    }

    return 0;
}
