#include "tcpClient.h"
#include <algorithm>
int main() {
    TcpClient client("0.0.0.0", 1234);
    
    while (!client.connectToServer()) {
        std::cerr << "Retrying connection in 1 second...\n";
        sleep(1);
    }

    // Nhận message đầu tiên từ server
    client.receiveMessage();

    // Gửi message về lại cho server
    client.sendMessage("Hello from client!");

    while (client.isConnected()) {

        short arr[6];
        client.receiveArray(arr, 6);
        short client_arr[6];
        std::copy(std::begin(arr), std::end(arr), std::begin(client_arr));

        client.sendArray(client_arr, 6);
    }
    return 0;
}
