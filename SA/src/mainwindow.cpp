#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("CY Signal Analyzer");

    // 初始化UDP接收器
    udpReceiver = new UdpReceiver(this);

    // 连接信号槽
    connect(udpReceiver, &UdpReceiver::packetReceived, this, &MainWindow::updatePacketCount);
    connect(udpReceiver, &UdpReceiver::packetData, this, &MainWindow::displayPacketData);

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
}

MainWindow::~MainWindow()
{
    delete udpReceiver;
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
        if (udpReceiver->startReceiving(selectedIpAddress, port)) {
            // 清空显示区域
            ui->packetCountLabel->setText("已接收数据包数量: 0");
            ui->packetDataTextEdit->clear();
            
            // 重置计数和缓冲区
            currentPacketCount = 0;
            packetDataBuffer.clear();
            
            // 启动UI更新定时器
            uiUpdateTimer->start();
            
            ui->startButton->setText(tr("停止接收"));
            ui->networkInterfaceComboBox->setEnabled(false);
            ui->portLineEdit->setEnabled(false);
            ui->statusBar->showMessage("开始接收UDP数据", 3000);
        } else {
            ui->statusBar->showMessage("绑定端口失败，请检查端口是否被占用", 3000);
        }
    } else {
        // 停止接收UDP数据
        udpReceiver->stopReceiving();
        
        // 停止UI更新定时器
        uiUpdateTimer->stop();
        
        // 更新最终计数和显示所有剩余数据
        ui->packetCountLabel->setText(QString("已接收数据包数量: %1").arg(currentPacketCount));
        if (!packetDataBuffer.isEmpty()) {
            ui->packetDataTextEdit->append(packetDataBuffer.join("\n"));
            packetDataBuffer.clear();
        }
        
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
    
    // 检查数据包长度是否足够（至少1092字节）
    if (data.size() < 1092) {
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
    
    // 解析帧类型（应该为2）
    quint32 frameType = *reinterpret_cast<const quint32*>(data.constData() + 4);
    if (frameType != 2) {
        frameInfo = QString("帧类型错误：%1，跳过解析").arg(frameType);
        packetDataBuffer.append(frameInfo);
        return;
    }
    
    // 解析总帧号（4-7字节）
    quint32 totalFrameNumber = *reinterpret_cast<const quint32*>(data.constData() + 8);
    
    // 解析数据包尾（0x0011EEFF）
    quint32 tail = *reinterpret_cast<const quint32*>(data.constData() + 1092);
    if (tail != 0x0011EEFF) {
        frameInfo = QString("数据包尾错误：0x%1，跳过解析").arg(tail, 8, 16, QChar('0'));
        packetDataBuffer.append(frameInfo);
        return;
    }
    
    // 打印帧号信息
    frameInfo = QString("帧号：%1").arg(totalFrameNumber);
    packetDataBuffer.append(frameInfo);
    
    // 限制缓冲区大小，防止内存溢出
    if (packetDataBuffer.size() > MAX_BUFFER_SIZE) {
        packetDataBuffer.removeFirst();
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
