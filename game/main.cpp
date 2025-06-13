#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "food.h"
#include "snakehandle.h"
#include "tcpclient.h"  // Thêm header của lớp TCPClient

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Food *myFood = new Food;

    SnakeHandle *mySnake = new SnakeHandle;

    TCPClient *tcpClient = new TCPClient();

    tcpClient->setFood(myFood);
    tcpClient->setSnake(mySnake);

    if (tcpClient->connectToServer("127.0.0.1", 8080)) {
        qDebug() << "Connected server";
        while(true){
            tcpClient->sendMessage("Hello from client!");
            std::string serverResponse = tcpClient->receiveMessage();
            qDebug() << "Received from server: " << serverResponse.c_str();
        }
    }

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
