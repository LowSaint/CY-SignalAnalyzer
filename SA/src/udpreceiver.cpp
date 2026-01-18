#include "udpreceiver.h"

// 定义静态常量
const int UdpReceiver::BATCH_SIZE;
const int UdpReceiver::BATCH_INTERVAL;
const int UdpReceiver::MAX_BUFFER_SIZE;
#include <QTimer>
#include <QList>

UdpReceiver::UdpReceiver(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
    packetCount = 0;

    // 初始化批量处理定时器
    m_batchTimer = new QTimer(this);
    m_batchTimer->setInterval(BATCH_INTERVAL);
    m_batchTimer->setSingleShot(false);

    connect(udpSocket, &QUdpSocket::readyRead, this, &UdpReceiver::onReadyRead);
    connect(m_batchTimer, &QTimer::timeout, this, &UdpReceiver::processPacketBatch);
}

UdpReceiver::~UdpReceiver()
{
    stopReceiving();
    delete udpSocket;
    delete m_batchTimer;
}

bool UdpReceiver::startReceiving(const QString &localIp, int port)
{
    this->localIp = localIp;
    this->port = port;
    packetCount = 0;
    m_packetBuffer.clear();

    // 绑定到指定IP和端口
    if (udpSocket->bind(QHostAddress(localIp), port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        // 启动批量处理定时器
        m_batchTimer->start();
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
    // 停止批量处理定时器
    m_batchTimer->stop();
    // 处理剩余的数据包
    if (!m_packetBuffer.isEmpty()) {
        processPacketBatch();
    }
}

void UdpReceiver::onReadyRead()
{
    // 处理所有可用的UDP数据包，将它们添加到缓冲区
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress senderAddress;
        quint16 senderPort;

        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);

        // 将数据包添加到缓冲区，限制缓冲区大小
        if (m_packetBuffer.size() < MAX_BUFFER_SIZE) {
            m_packetBuffer.append(datagram);
            packetCount++;
        } else {
            // 缓冲区已满，丢弃旧的数据包
            m_packetBuffer.removeFirst();
            m_packetBuffer.append(datagram);
            packetCount++;
        }
    }
}

void UdpReceiver::processPacketBatch()
{
    if (m_packetBuffer.isEmpty()) {
        return;
    }

    // 发送当前数据包数量
    emit packetReceived(packetCount);

    // 批量处理数据包
    QList<QByteArray> batchData;
    int processCount = qMin(m_packetBuffer.size(), BATCH_SIZE);
    
    for (int i = 0; i < processCount; i++) {
        batchData.append(m_packetBuffer.takeFirst());
    }
    
    // 发送批量数据包
    emit packetDataBatch(batchData);
}
