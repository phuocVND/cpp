#include <iostream>

#include "tcpClient.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    TcpClient client("127.0.0.1", 12345);
    client.connectToServer();

    return app.exec();
}
