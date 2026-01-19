#include "workthread.h"
#include "udpreceiver.h"

WorkThread::WorkThread(QObject *parent) : QThread(parent)
{
    m_udpReceiver = nullptr;
    m_port = 0;
}

WorkThread::~WorkThread()
{
    stopWork();
}

void WorkThread::startWork(const QString &localIp, int port)
{
    m_localIp = localIp;
    m_port = port;
    
    if (!isRunning()) {
        start();
    }
}

void WorkThread::stopWork()
{
    if (m_udpReceiver) {
        m_udpReceiver->stopReceiving();
        m_udpReceiver->deleteLater();
        m_udpReceiver = nullptr;
    }
    
    if (isRunning()) {
        quit();
        wait();
    }
}

void WorkThread::run()
{
    // 在工作线程中创建UdpReceiver
    m_udpReceiver = new UdpReceiver;
    
    // 连接信号槽
    connect(m_udpReceiver, &UdpReceiver::packetReceived, this, &WorkThread::packetReceived, Qt::DirectConnection);
    connect(m_udpReceiver, &UdpReceiver::packetDataBatch, this, &WorkThread::packetDataBatch, Qt::DirectConnection);
    
    // 开始接收UDP数据
    m_udpReceiver->startReceiving(m_localIp, m_port);
    
    // 启动事件循环
    exec();
}
