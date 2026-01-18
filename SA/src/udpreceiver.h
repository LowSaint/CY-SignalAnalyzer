#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QString>
#include <QByteArray>
#include <QTimer>

class UdpReceiver : public QObject
{
    Q_OBJECT
public:
    explicit UdpReceiver(QObject *parent = nullptr);
    ~UdpReceiver();

    bool startReceiving(const QString &localIp, int port);
    void stopReceiving();

signals:
    void packetReceived(int count);
    void packetDataBatch(const QList<QByteArray> &dataList);

private slots:
    void onReadyRead();
    void processPacketBatch();

private:
    QUdpSocket *udpSocket;
    QString localIp;
    int port;
    int packetCount;
    
    // 批量处理相关变量
    QList<QByteArray> m_packetBuffer;
    QTimer *m_batchTimer;
    static const int BATCH_SIZE = 50; // 每批次处理的数据包数量，增加到50个
    static const int BATCH_INTERVAL = 20; // 批处理间隔（毫秒），减少到20ms
    static const int MAX_BUFFER_SIZE = 5000; // 最大缓冲区大小，增加到5000个数据包
};

#endif // UDPRECEIVER_H
