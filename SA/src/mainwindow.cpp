#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("CY Signal Analyzer");

    // 初始化工作线程
    workThread = new WorkThread(this);

    // 连接信号槽
    connect(workThread, &WorkThread::packetReceived, this, &MainWindow::updatePacketCount, Qt::QueuedConnection);
    connect(workThread, &WorkThread::packetDataBatch, this, &MainWindow::displayPacketDataBatch, Qt::QueuedConnection);

    // 设置默认端口号
    ui->portLineEdit->setText("60002");

    // 加载网卡列表
    networkInterfaces = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface interface, networkInterfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            ui->networkInterfaceComboBox->addItem(interface.humanReadableName());
        }
    }

    // 初始化显示第一个网卡的IP地址
    if (!networkInterfaces.isEmpty()) {
        on_networkInterfaceComboBox_currentIndexChanged(0);
    }

    // 初始化UI更新定时器和相关变量
    uiUpdateTimer = new QTimer(this);
    uiUpdateTimer->setInterval(UPDATE_INTERVAL);
    connect(uiUpdateTimer, &QTimer::timeout, this, &MainWindow::updateUITimer);
    currentPacketCount = 0;
    lastFrameNumber = 0; // 初始化上一帧号为0
    lastSampleTime = 0.0; // 初始化上一个采样点的时间为0
    
    // 初始化波形窗口
    m_waveformWindow = new WaveformWindow(this);
    
    // 显示波形窗口
    m_waveformWindow->show();
}

MainWindow::~MainWindow()
{
    // 清理波形窗口
    delete m_waveformWindow;
    
    delete workThread;
    delete uiUpdateTimer;
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    if (ui->startButton->text() == tr("开始接收")) {
        // 获取设置的端口号
        bool ok;
        int port = ui->portLineEdit->text().toInt(&ok);
        if (!ok || port < 1 || port > 65535) {
            ui->statusBar->showMessage("端口号无效，请输入1-65535之间的整数", 3000);
            return;
        }

        // 开始接收UDP数据
        workThread->startWork(selectedIpAddress, port);
        
        // 清空显示区域
        ui->packetCountLabel->setText("已接收数据包数量: 0");
        
        // 重置计数和缓冲区
        currentPacketCount = 0;
        packetDataBuffer.clear();
        lastFrameNumber = 0; // 重置帧号计数器
        lastSampleTime = 0.0; // 重置时间计数器，确保时间轴从0开始
        
        // 清空所有波形绘图数据
        m_waveformWindow->clearAllData();
        
        // 启动UI更新定时器
        uiUpdateTimer->start();
        
        ui->startButton->setText(tr("停止接收"));
        ui->networkInterfaceComboBox->setEnabled(false);
        ui->portLineEdit->setEnabled(false);
        ui->statusBar->showMessage("开始接收UDP数据", 3000);
    } else {
        // 停止接收UDP数据
        workThread->stopWork();
        
        // 停止UI更新定时器
        uiUpdateTimer->stop();
        
        // 更新最终计数
        ui->packetCountLabel->setText(QString("已接收数据包数量: %1").arg(currentPacketCount));
        
        ui->startButton->setText(tr("开始接收"));
        ui->networkInterfaceComboBox->setEnabled(true);
        ui->portLineEdit->setEnabled(true);
        ui->statusBar->showMessage("停止接收UDP数据", 3000);
    }
}

void MainWindow::on_networkInterfaceComboBox_currentIndexChanged(int index)
{
    // 获取选中网卡的IP地址
    QNetworkInterface selectedInterface = networkInterfaces[index];
    QList<QNetworkAddressEntry> entries = selectedInterface.addressEntries();
    foreach (QNetworkAddressEntry entry, entries) {
        if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
            selectedIpAddress = entry.ip().toString();
            ui->ipAddressLabel->setText(selectedIpAddress);
            break;
        }
    }
}

void MainWindow::updatePacketCount(int count)
{
    // 只更新计数器，不直接更新UI
    currentPacketCount = count;
}

void MainWindow::displayPacketData(const QByteArray &data)
{
    // 按照协议解析UDP报文
    QString frameInfo;
    
    // 检查数据包长度是否足够（至少1096字节）
    if (data.size() < 1096) {
        frameInfo = QString("数据包长度不足：%1字节，跳过解析").arg(data.size());
        packetDataBuffer.append(frameInfo);
        return;
    }
    
    // 解析数据包头（0xAA55AA55）
    quint32 header = *reinterpret_cast<const quint32*>(data.constData());
    if (header != 0xAA55AA55) {
        frameInfo = QString("数据包头错误：0x%1，跳过解析").arg(header, 8, 16, QChar('0'));
        packetDataBuffer.append(frameInfo);
        return;
    }
    
    // 解析帧类型（固定值2或3，根据协议备注是3）
    quint32 frameType = *reinterpret_cast<const quint32*>(data.constData() + 4);
    if (frameType != 2) {
        frameInfo = QString("帧类型错误：%1，跳过解析").arg(frameType);
        packetDataBuffer.append(frameInfo);
        return;
    }
    
    // 解析总帧号（8-11字节）
    quint32 totalFrameNumber = *reinterpret_cast<const quint32*>(data.constData() + 8);
    
    // 解析分片号1（12-15字节）
    quint32 fragmentNum1 = *reinterpret_cast<const quint32*>(data.constData() + 12);
    
    // 解析分片号2（16-19字节）
    quint32 fragmentNum2 = *reinterpret_cast<const quint32*>(data.constData() + 16);
    
    // 解析图像宽度（20-23字节）
    quint32 imageWidth = *reinterpret_cast<const quint32*>(data.constData() + 20);
    
    // 解析图像高度（24-27字节）
    quint32 imageHeight = *reinterpret_cast<const quint32*>(data.constData() + 24);
    
//    // 解析校验码（1088-1091字节）
//    quint32 receivedChecksum = *reinterpret_cast<const quint32*>(data.constData() + 1088);
    
//    // 计算校验码：将校验码数据段置零后，对本包所有数据按int类型求和
//    quint32 calculatedChecksum = 0;
    
//    // 创建数据包的副本
//    QByteArray dataCopy = data;
    
//    // 将校验码位置（1088-1091字节）置零
//    memset(dataCopy.data() + 1088, 0, 4);
    
//    // 将副本视为int数组并计算总和
//    const quint32 *intData = reinterpret_cast<const quint32*>(dataCopy.constData());
//    int numInts = dataCopy.size() / sizeof(quint32);
    
//    for (int i = 0; i < numInts; ++i) {
//        calculatedChecksum += intData[i];
//    }
    
//    // 验证校验码
//    if (receivedChecksum != calculatedChecksum) {
//        frameInfo = QString("校验码错误：接收0x%1，计算0x%2，跳过解析").arg(receivedChecksum, 8, 16, QChar('0')).arg(calculatedChecksum, 8, 16, QChar('0'));
//        packetDataBuffer.append(frameInfo);
//        return;
//    }
    
    // 解析数据包尾（1092-1095字节，0x0011EEFF）
    quint32 tail = *reinterpret_cast<const quint32*>(data.constData() + 1092);
    if (tail != 0x0011EEFF) {
        frameInfo = QString("数据包尾错误：0x%1，跳过解析").arg(tail, 8, 16, QChar('0'));
        packetDataBuffer.append(frameInfo);
        return;
    }
    
    // 解析波形数据（从第64字节开始）
    const short *waveformData = reinterpret_cast<const short*>(data.constData() + 64);
    
    // 获取波形窗口中的绘图对象
    QVector<WaveformPlot*> waveformPlots = m_waveformWindow->getWaveformPlots();
    
    // 更新8个通道的波形数据，每个通道64个采样点
    for (int sampleIdx = 0; sampleIdx < NUM_SAMPLES; ++sampleIdx) {
        for (int channelIdx = 0; channelIdx < NUM_CHANNELS; ++channelIdx) {
            // 计算数据在数组中的索引
            int dataIdx = sampleIdx * NUM_CHANNELS + channelIdx;
            
            // 计算当前采样点的时间值，确保时间连续性
            double x = lastSampleTime + (sampleIdx * NUM_CHANNELS + channelIdx) * TIME_INTERVAL;
            double y = waveformData[dataIdx];
            
            // 更新对应的通道绘图
            if (channelIdx < waveformPlots.size()) {
                waveformPlots[channelIdx]->addDataPoint(x, y);
            }
        }
    }
    
    // 更新lastSampleTime为最后一个采样点的时间值
    lastSampleTime += NUM_SAMPLES * NUM_CHANNELS * TIME_INTERVAL;
    
    // 丢帧检测逻辑
    if (lastFrameNumber != 0) { // 不是第一帧
        if (totalFrameNumber != lastFrameNumber + 1) {
            // 发生丢帧
            frameInfo = QString("发生丢帧：上一帧号%1，当前帧号%2，丢失%3帧").arg(lastFrameNumber).arg(totalFrameNumber).arg(totalFrameNumber - lastFrameNumber - 1);
            packetDataBuffer.append(frameInfo);
        }
    }
    
    // 更新上一帧号
    lastFrameNumber = totalFrameNumber;
    
    // 限制缓冲区大小，防止内存溢出
    if (packetDataBuffer.size() > MAX_BUFFER_SIZE) {
        packetDataBuffer.removeFirst();
    }
}

void MainWindow::displayPacketDataBatch(const QList<QByteArray> &dataList)
{
    // 批量处理接收到的UDP数据包
    foreach (const QByteArray &data, dataList) {
        displayPacketData(data);
    }
}

void MainWindow::updateUITimer()
{
    // 更新数据包计数
    ui->packetCountLabel->setText(QString("已接收数据包数量: %1").arg(currentPacketCount));
    
    // 更新数据包数据显示
    if (!packetDataBuffer.isEmpty()) {
        // 将缓冲区数据批量添加到显示区域
        ui->packetDataTextEdit->append(packetDataBuffer.join("\n"));
        packetDataBuffer.clear();
        
        // 自动滚动到底部
        ui->packetDataTextEdit->moveCursor(QTextCursor::End);
    }
}
