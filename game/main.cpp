#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>
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


void handlerTcp(Food *myFood, SnakeHandle *mySnake , TCPClient *tcpClient, Data *dataSend){
    std::atomic<bool> running{true};

        while (running) {
            std::string message = "555\n";
            char buffer[1024] = {0};

            tcpClient->sendValue(sizeof(message));

            tcpClient->sendMessage(message);

            uint64_t sizeData = tcpClient->receiveValue();
            std::string serverResponse = tcpClient->receiveMessage(sizeData);
            // qDebug() << "Received: " << QString::fromStdString(serverResponse);

            bool check = myFood->xFood() == mySnake->xSnake() && myFood->yFood() == mySnake->ySnake();
            std::string sendStr = std::to_string(dataSend->xFood = myFood->xFood()) + "," +
                                  std::to_string(dataSend->yFood = myFood->yFood()) + "," +
                                  std::to_string(dataSend->xHead = mySnake->xSnake()) + "," +
                                  std::to_string(dataSend->yHead = mySnake->ySnake())+ "," +
                                  std::to_string(check) + "," +
                                  std::to_string(mySnake->last_action);

            tcpClient->sendValue(sizeof(sendStr));
            tcpClient->sendMessage(sendStr);

            // 2. Nhận phản hồi từ server
            sizeData = tcpClient->receiveValue();
            serverResponse = tcpClient->receiveMessage(sizeData);

            // 3. Xử lý lệnh
            if (!serverResponse.empty()) {
                char dir = serverResponse[0];
                QMetaObject::invokeMethod(mySnake, [=]() {
                    mySnake->handleDirection(dir);
                }, Qt::QueuedConnection);
            }
            if(check == true){
                myFood->randomizePosition();
                mySnake->randomizePosition();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
}


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

    if (tcpClient->connectToServer("127.0.0.1", 8080)) {

        qDebug() << "Connected to server (std::thread)";
        std::thread tcpThread([=]() {
            handlerTcp(myFood, mySnake, tcpClient, dataSend);
        });

        tcpThread.detach();
    }

    // delete tcpClient;

    return app.exec();
}
