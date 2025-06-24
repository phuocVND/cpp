#include "tcpserver.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>
#include <algorithm> // Cho std::max và std::min hoặc qBound

TcpServer::TcpServer(QObject *parent)
    : QObject(parent),
    m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &TcpServer::handleNewConnection);
}

void TcpServer::startServer(quint16 port)
{
    if (!m_server->listen(QHostAddress::Any, port)) {
        qCritical() << "Không thể khởi động server:" << m_server->errorString();
        return;
    }
    qInfo() << "Server đang lắng nghe trên port" << port;
}

void TcpServer::sendParameters(int p1, int p2, int p3, int p4, int p5, int p6)
{
    if (m_clients.isEmpty()) {
        qWarning() << "Không có client nào kết nối để gửi dữ liệu!";
        return;
    }

    QByteArray dataBlock;
    QDataStream out(&dataBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    // Ghi 6 giá trị vào luồng, đảm bảo chúng nằm trong khoảng 0-255
    out << static_cast<uint8_t>(qBound(0, p1, 255))
        << static_cast<uint8_t>(qBound(0, p2, 255))
        << static_cast<uint8_t>(qBound(0, p3, 255))
        << static_cast<uint8_t>(qBound(0, p4, 255))
        << static_cast<uint8_t>(qBound(0, p5, 255))
        << static_cast<uint8_t>(qBound(0, p6, 255));

    for (QTcpSocket *client : m_clients) {
        client->write(dataBlock);
    }

    qInfo() << "Đã gửi các giá trị:" << p1 << p2 << p3 << p4 << p5 << p6;
}

void TcpServer::handleNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *clientSocket = m_server->nextPendingConnection();
        qInfo() << "Client mới đã kết nối:" << clientSocket->peerAddress().toString();
        m_clients.append(clientSocket);
        connect(clientSocket, &QTcpSocket::disconnected, this, &TcpServer::clientDisconnected);
    }
}

void TcpServer::clientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket) return;

    qInfo() << "Client đã ngắt kết nối:" << clientSocket->peerAddress().toString();
    m_clients.removeAll(clientSocket);
    clientSocket->deleteLater();
}
