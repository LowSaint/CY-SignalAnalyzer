#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QString>
#include <QByteArray>

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
    void packetData(const QByteArray &data);

private slots:
    void onReadyRead();

private:
    QUdpSocket *udpSocket;
    QString localIp;
    int port;
    int packetCount;
};

#endif // UDPRECEIVER_H
