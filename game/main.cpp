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

    if (tcpClient->connectToServer("127.0.0.1", 8080)) {

        qDebug() << "Connected to server (std::thread)";

        // tcpClient->sendMessage("Hello from client!");

        // std::string serverResponse = tcpClient->receiveMessage();
        // qDebug() << "Received: " << QString::fromStdString(serverResponse);


        while (running) {
            std::string message = "555\n";
            char buffer[1024] = {0};

            tcpClient->sendValue(sizeof(message));
            tcpClient->sendMessage(message);

            uint64_t sizeData = tcpClient->receiveValue();
            std::string serverResponse = tcpClient->receiveMessage(sizeData);
            qDebug() << "Received: " << QString::fromStdString(serverResponse);


            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    } else {
        qDebug() << "Failed to connect to server";
    }

    delete tcpClient;

    return app.exec();
}
