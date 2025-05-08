#include "tcpServer.h"
#include <string>
#include <iostream>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    TcpServer server("127.0.0.1", 8080);

    // Bắt đầu lắng nghe kết nối từ client
    server.start_accept();
    std::cout << "Main thread is running." << std::endl;

    while (true) {
        // Bạn có thể thêm code xử lý công việc khác ở đây, hoặc chỉ cần chờ
        // server tiếp tục lắng nghe kết nối
    }

    return 0;
}
