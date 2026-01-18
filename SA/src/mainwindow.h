#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QTimer>
#include <QStringList>
#include "udpreceiver.h"
#include "waveformwindow.h"

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
    void displayPacketDataBatch(const QList<QByteArray> &dataList);
    void updateUITimer();

private:
    Ui::MainWindow *ui;
    UdpReceiver *udpReceiver;
    WaveformWindow *m_waveformWindow;
    QList<QNetworkInterface> networkInterfaces;
    QString selectedIpAddress;
    
    // UI更新优化相关变量
    QTimer *uiUpdateTimer;
    int currentPacketCount;
    QStringList packetDataBuffer;
    quint32 lastFrameNumber; // 保存上一帧的帧号，用于丢帧检测
    double lastSampleTime; // 保存上一个采样点的时间，用于确保时间连续性
    static const int MAX_BUFFER_SIZE = 100;
    static const int UPDATE_INTERVAL = 100; // 毫秒
    static constexpr double SAMPLING_RATE = 3.2e6; // 采样率：3.2Msps
    static constexpr double TIME_INTERVAL = 1.0 / SAMPLING_RATE; // 采样间隔（秒）
    
    // 波形绘图相关常量
    static const int NUM_CHANNELS = 8;
    static const int NUM_SAMPLES = 64;
};

#endif // MAINWINDOW_H
