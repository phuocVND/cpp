#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QList>
#include <cstdint>

class QTcpServer;
class QTcpSocket;

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);

public slots:
    // Slot để khởi động server
    void startServer(quint16 port);

    // Slot để nhận giá trị từ QML và gửi đi
    void sendParameters(int p1, int p2, int p3, int p4, int p5, int p6);

private slots:
    void handleNewConnection();
    void clientDisconnected();

private:
    QTcpServer *m_server;
    QList<QTcpSocket *> m_clients;
};

#endif // TCPSERVER_H
