#include "tcpserver.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QThread>
#include <iostream>
#include <ostream>
#include <string>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    TcpServer server("127.0.0.1", 8080);

    std::thread serverThread([&server]() {
        server.start_accept();
    });
    serverThread.detach();

    std::cout << "Main thread is running." << std::endl;


    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("tcpserver", "Main");

    return app.exec();
}
