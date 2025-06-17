#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <thread>
#include "food.h"
#include "snakehandle.h"
#include "tcpclient.h"
struct Data{
    int xFood = 0;
    int yFood = 0;
    int xHead = 0;
    int yHead = 0;
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    Data *dataSend = new Data;

    Food *myFood = new Food;

    SnakeHandle *mySnake = new SnakeHandle;

    TCPClient *tcpClient = new TCPClient();

    tcpClient->setFood(myFood);
    tcpClient->setSnake(mySnake);

    engine.rootContext()->setContextProperty("Food", myFood);
    engine.rootContext()->setContextProperty("Snake", mySnake);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("game", "Main");

    myFood->randomizePosition();
    mySnake->randomizePosition();

    std::atomic<bool> running{true};

    // Tạo thread để chạy TCP giao tiếp
    std::thread tcpThread([&]() {
        if (tcpClient->connectToServer("127.0.0.1", 8080)) {
            qDebug() << "Connected to server (std::thread)";
            tcpClient->sendMessage("Hello from client!\n");
            std::string serverResponse = tcpClient->receiveMessage();
            qDebug() << "Received: " << QString::fromStdString(serverResponse);
            while (running) {
                // 1. Gửi dữ liệu đi
                bool check = myFood->xFood() == mySnake->xSnake() && myFood->yFood() == mySnake->ySnake();
                std::string sendStr = std::to_string(dataSend->xFood = myFood->xFood()) + "," +
                                      std::to_string(dataSend->yFood = myFood->yFood()) + "," +
                                      std::to_string(dataSend->xHead = mySnake->xSnake()) + "," +
                                      std::to_string(dataSend->yHead = mySnake->ySnake())+ "," + std::to_string(check);

                tcpClient->sendMessage(sendStr + "\n");

                // 2. Nhận phản hồi từ server
                std::string serverResponse = tcpClient->receiveMessage(); // chờ server

                // 3. Xử lý lệnh
                if (!serverResponse.empty()) {
                    qDebug() << "Received: " << QString::fromStdString(serverResponse);
                    switch (serverResponse[0]) {
                    case 'u': mySnake->up(); break;
                    case 'd': mySnake->down(); break;
                    case 'l': mySnake->left(); break;
                    case 'r': mySnake->right(); break;
                    default: break;
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(50)); // giúp ổn định vòng lặp
            }
        } else {
            qDebug() << "Failed to connect to server";
        }
    });

    delete tcpClient;

    return app.exec();
}
