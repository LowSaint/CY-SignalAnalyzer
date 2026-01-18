#include "waveformwindow.h"

WaveformWindow::WaveformWindow(QWidget *parent) : QMainWindow(parent)
{
    // 设置窗口标题
    this->setWindowTitle("CY Signal Analyzer - Waveforms");
    
    // 创建中央部件
    m_centralWidget = new QWidget(this);
    this->setCentralWidget(m_centralWidget);
    
    // 创建网格布局，2行4列
    m_plotLayout = new QGridLayout(m_centralWidget);
    
    // 创建8个波形绘图对象
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        WaveformPlot *plot = new WaveformPlot(i, this);
        m_waveformPlots.append(plot);
        
        int row = i / 4;
        int col = i % 4;
        m_plotLayout->addWidget(plot, row, col);
    }
    
    // 设置窗口大小和最小尺寸
    this->resize(1200, 800);
    this->setMinimumSize(800, 600);
}

WaveformWindow::~WaveformWindow()
{
    // 清理波形绘图对象
    foreach (WaveformPlot *plot, m_waveformPlots) {
        delete plot;
    }
}

QVector<WaveformPlot*> WaveformWindow::getWaveformPlots() const
{
    return m_waveformPlots;
}

void WaveformWindow::clearAllData()
{
    // 清空所有波形绘图数据
    foreach (WaveformPlot *plot, m_waveformPlots) {
        plot->clearData();
    }
}
