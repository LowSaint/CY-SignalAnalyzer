#include "udpreceiver.h"

UdpReceiver::UdpReceiver(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
    packetCount = 0;

    connect(udpSocket, &QUdpSocket::readyRead, this, &UdpReceiver::onReadyRead);
}

UdpReceiver::~UdpReceiver()
{
    stopReceiving();
    delete udpSocket;
}

bool UdpReceiver::startReceiving(const QString &localIp, int port)
{
    this->localIp = localIp;
    this->port = port;
    packetCount = 0;

    // 绑定到指定IP和端口
    if (udpSocket->bind(QHostAddress(localIp), port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        return true;
    } else {
        return false;
    }
}

void UdpReceiver::stopReceiving()
{
    if (udpSocket->state() == QUdpSocket::BoundState) {
        udpSocket->close();
    }
}

void UdpReceiver::onReadyRead()
{
    // 处理所有可用的UDP数据包
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress senderAddress;
        quint16 senderPort;

        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);

        // 已经绑定到指定端口，接收到的数据包都是发送到该端口的
        // 不再需要检查源端口，直接处理所有接收到的UDP包
        packetCount++;
        emit packetReceived(packetCount);
        emit packetData(datagram);
    }
}
