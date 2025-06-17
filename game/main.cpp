#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <thread>
#include "food.h"
#include "snakehandle.h"
#include "tcpclient.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Food *myFood = new Food;

    SnakeHandle *mySnake = new SnakeHandle;

    TCPClient *tcpClient = new TCPClient();

    tcpClient->setFood(myFood);
    tcpClient->setSnake(mySnake);

    std::atomic<bool> running{true};

    // Tạo thread để chạy TCP giao tiếp
    std::thread tcpThread([&]() {
        if (tcpClient->connectToServer("127.0.0.1", 8080)) {
            qDebug() << "Connected to server (std::thread)";
            tcpClient->sendMessage("Hello from client!");

            while (running) {
                std::string serverResponse = tcpClient->receiveMessage();
                if(serverResponse != ""){
                    qDebug() << "Received: " << QString::fromStdString(serverResponse);
                    switch (serverResponse[0]) {
                    case 'u':
                        mySnake->up();
                        break;
                    case 'd':
                        mySnake->down();
                        break;
                    case 'r':
                        mySnake->right();
                        break;
                    case 'l':
                        mySnake->left();
                        break;
                    default:
                        break;
                    }
                }
            }
        } else {
            qDebug() << "Failed to connect to server";
        }
    });

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

    // delete tcpClient;

    return app.exec();
}
