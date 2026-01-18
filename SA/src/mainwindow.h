#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QTimer>
#include <QStringList>
#include "udpreceiver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_networkInterfaceComboBox_currentIndexChanged(int index);
    void updatePacketCount(int count);
    void displayPacketData(const QByteArray &data);
    void updateUITimer();

private:
    Ui::MainWindow *ui;
    UdpReceiver *udpReceiver;
    QList<QNetworkInterface> networkInterfaces;
    QString selectedIpAddress;
    
    // UI更新优化相关变量
    QTimer *uiUpdateTimer;
    int currentPacketCount;
    QStringList packetDataBuffer;
    static const int MAX_BUFFER_SIZE = 100;
    static const int UPDATE_INTERVAL = 100; // 毫秒
};

#endif // MAINWINDOW_H
