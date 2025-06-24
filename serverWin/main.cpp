#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

// Thêm include cho lớp server của chúng ta
#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Đăng ký lớp C++ TcpServer để có thể sử dụng trong QML
    qmlRegisterType<TcpServer>("com.company.tcp", 1, 0, "TcpServer");

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("serverWin", "Main");

    return app.exec();
}
