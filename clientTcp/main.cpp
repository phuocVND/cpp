#include "tcpClient.h"

int main() {
    TcpClient client("127.0.0.1", 8080);
    if (!client.connectToServer()) {
        return 1;
    }

    // Nhận message đầu tiên từ server
    client.receiveMessage();

    // Gửi message về lại cho server
    client.sendMessage("Hello from client!");

    while (client.isConnected()) {
        // if(!client.isConnected()){
        //     client.disconnect();
        //     break;
        // }
        // ✅ Nhận array từ server
        int arr[5];
        client.receiveArray(arr, 5);

        // ✅ Nhận struct từ server
        MyData data;
        client.receiveStruct(data);

        // ✅ Gửi lại array cho server
        int client_arr[5] = {10, 20, 30, 40, 50};
        client.sendArray(client_arr, 5);

        // ✅ Gửi lại struct cho server
        MyData mydata_to_send = {7, 77.77f, "ClientStruct"};
        client.sendStruct(mydata_to_send);
    }
    return 0;
}
