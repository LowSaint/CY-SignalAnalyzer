#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <QThread>
#include <QObject>
#include <QList>
#include <QByteArray>

class UdpReceiver;

class WorkThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkThread(QObject *parent = nullptr);
    ~WorkThread();

    void startWork(const QString &localIp, int port);
    void stopWork();

signals:
    void packetReceived(int count);
    void packetDataBatch(const QList<QByteArray> &dataList);

private:
    UdpReceiver *m_udpReceiver;
    QString m_localIp;
    int m_port;

protected:
    void run() override;
};

#endif // WORKTHREAD_H
