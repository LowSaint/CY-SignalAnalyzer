#include "waveformplot.h"
#include <QWheelEvent>
#include <QDebug>
#include <cmath>

WaveformPlot::WaveformPlot(int channelIndex, QWidget *parent)
    : QWidget(parent),
      m_channelIndex(channelIndex),
      m_paused(false),
      m_isDragging(false),
      m_isSelecting(false),
      m_xScaleFactor(1.0),
      m_yScaleFactor(1.0),
      m_translateX(0.0),
      m_translateY(0.0),
      m_xMin(0.0),
      m_xMax(64.0),
      m_yMin(-100.0),
      m_yMax(100.0),
      m_timeWindow(64.0), // 默认时间窗口为64秒
      m_needsUpdate(false),
      m_dataPointsSinceLastUpdate(0)
{
    // 创建布局和暂停按钮
    m_mainLayout = new QVBoxLayout(this);
    
    // 设置绘图区域（初始设置，会在paintEvent中更新）
    m_plotRect = QRect(20, 20, width() - 40, height() - 60);
    
    // 创建暂停/继续按钮
    m_pauseButton = new QPushButton(QString("通道 %1: 暂停").arg(channelIndex + 1), this);
    m_mainLayout->addStretch(); // 添加伸缩项，将按钮推到下方
    m_mainLayout->addWidget(m_pauseButton);
    
    // 连接信号槽
    connect(m_pauseButton, &QPushButton::clicked, this, &WaveformPlot::togglePause);
    
    // 创建更新定时器
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(UPDATE_INTERVAL);
    m_updateTimer->start();
    connect(m_updateTimer, &QTimer::timeout, this, &WaveformPlot::updateTimerTimeout);
    
    // 初始化缩放模式
    m_currentScaleMode = FreeScale;
    
    // 创建右键菜单
    m_contextMenu = new QMenu(this);
    QAction *freeScaleAction = m_contextMenu->addAction("自由缩放");
    QAction *horizontalScaleAction = m_contextMenu->addAction("水平缩放");
    QAction *verticalScaleAction = m_contextMenu->addAction("垂直缩放");
    
    // 连接菜单动作
    connect(freeScaleAction, &QAction::triggered, this, [this]() { setScaleMode(FreeScale); });
    connect(horizontalScaleAction, &QAction::triggered, this, [this]() { setScaleMode(HorizontalScale); });
    connect(verticalScaleAction, &QAction::triggered, this, [this]() { setScaleMode(VerticalScale); });
    
    // 设置窗口属性
    setMinimumSize(400, 200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

WaveformPlot::~WaveformPlot()
{
}

void WaveformPlot::addDataPoint(double x, double y)
{
    if (!m_paused) {
        // 添加数据点
        m_xData.append(x);
        m_yData.append(y);
        
        m_needsUpdate = true;
        m_dataPointsSinceLastUpdate++;
        
        // 每50毫秒更新一次绘图，而不是每64个点
        if (m_needsUpdate && m_dataPointsSinceLastUpdate >= 10) {
            // 使用update()而不是repaint()，让Qt优化绘图
            update();
            m_needsUpdate = false;
            m_dataPointsSinceLastUpdate = 0;
        }
    }
}

void WaveformPlot::clearData()
{
    m_xData.clear();
    m_yData.clear();
    update();
}

void WaveformPlot::updateTimerTimeout()
{
    // 定时更新绘图，确保即使数据点不足也能及时显示
    if (m_needsUpdate) {
        updatePlotLimits();
        update();
        m_needsUpdate = false;
        m_dataPointsSinceLastUpdate = 0;
    }
}

void WaveformPlot::pause()
{
    m_paused = true;
    m_pauseButton->setText(QString("通道 %1: 继续").arg(m_channelIndex + 1));
}

void WaveformPlot::resume()
{
    m_paused = false;
    m_pauseButton->setText(QString("通道 %1: 暂停").arg(m_channelIndex + 1));
}

bool WaveformPlot::isPaused() const
{
    return m_paused;
}

void WaveformPlot::setScaleMode(ScaleMode mode)
{
    m_currentScaleMode = mode;
    
    // 可以添加不同缩放模式的视觉反馈
    QString scaleModeText;
    switch (mode) {
    case FreeScale:
        scaleModeText = "自由缩放";
        break;
    case HorizontalScale:
        scaleModeText = "水平缩放";
        break;
    case VerticalScale:
        scaleModeText = "垂直缩放";
        break;
    }
    
    // 可以在状态栏或工具提示中显示当前缩放模式
    // QToolTip::showText(mapToGlobal(QPoint(0, 0)), QString("当前缩放模式: %1").arg(scaleModeText), this);
}

void WaveformPlot::togglePause()
{
    if (m_paused) {
        resume();
    } else {
        pause();
    }
}

void WaveformPlot::updatePlotLimits()
{
    if (m_xData.isEmpty() || m_yData.isEmpty()) {
        return;
    }
    
    // Y轴范围根据实际数据动态调整
    m_yMin = *std::min_element(m_yData.begin(), m_yData.end());
    m_yMax = *std::max_element(m_yData.begin(), m_yData.end());
    
    // 添加一些边距
    double yRange = m_yMax - m_yMin;
    
    m_yMin -= yRange * 0.05;
    m_yMax += yRange * 0.05;
    
    if (yRange == 0.0) {
        m_yMin -= 1.0;
        m_yMax += 1.0;
    }
}

void WaveformPlot::setTimeWindow(double timeWindow)
{
    // 设置时间窗口大小（仅用于初始显示范围，不再限制历史数据）
    m_timeWindow = timeWindow;
    
    // 如果只有少量数据，调整显示范围
    if (m_xData.size() > 0) {
        double latestX = m_xData.last();
        
        // 如果数据量不足一个时间窗口，扩展显示范围
        if (latestX - m_xData.first() < m_timeWindow) {
            m_xMax = m_xData.first() + m_timeWindow;
        }
    }
    
    update();
}

QPointF WaveformPlot::dataToScreen(const QPointF &dataPoint) const
{
    double xScale = m_plotRect.width() / (m_xMax - m_xMin);
    double yScale = m_plotRect.height() / (m_yMax - m_yMin);
    
    int screenX = m_plotRect.left() + static_cast<int>((dataPoint.x() - m_xMin) * xScale * m_xScaleFactor + m_translateX);
    int screenY = m_plotRect.bottom() - static_cast<int>((dataPoint.y() - m_yMin) * yScale * m_yScaleFactor + m_translateY);
    
    return QPointF(screenX, screenY);
}

QPointF WaveformPlot::screenToData(const QPointF &screenPoint) const
{
    double xScale = m_plotRect.width() / (m_xMax - m_xMin);
    double yScale = m_plotRect.height() / (m_yMax - m_yMin);
    
    double dataX = (screenPoint.x() - m_plotRect.left() - m_translateX) / (xScale * m_xScaleFactor) + m_xMin;
    double dataY = (m_plotRect.bottom() - screenPoint.y() - m_translateY) / (yScale * m_yScaleFactor) + m_yMin;
    
    return QPointF(dataX, dataY);
}

void WaveformPlot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    // 保持只显示1秒的数据
    if (m_xData.size() > 1) {
        // 计算当前显示范围
        double currentRange = m_xData.last() - m_xData.first();
        
        // 如果超过1秒，移除最旧的数据点，保持1秒窗口
        if (currentRange > 1.0) {
            // 计算需要移除的数据点数量
            double removeTime = currentRange - 1.0;
            int removeCount = static_cast<int>(removeTime / (m_xData[1] - m_xData[0]));
            
            // 移除多余的数据点
            for (int i = 0; i < removeCount && m_xData.size() > 1; ++i) {
                m_xData.removeFirst();
                m_yData.removeFirst();
            }
        }
        
        // 更新X轴范围
        m_xMin = m_xData.first();
        m_xMax = m_xData.last();
    } else if (!m_xData.isEmpty()) {
        // 只有一个数据点
        m_xMin = m_xData.first();
        m_xMax = m_xData.first() + 1.0; // 初始显示1秒的数据
    }
    
    // 更新Y轴范围
    updatePlotLimits();
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false); // 关闭抗锯齿以提高性能
    
    // 更新绘图区域，为下方的按钮留出空间
    int buttonHeight = m_pauseButton->height();
    m_plotRect = QRect(20, 20, width() - 40, height() - buttonHeight - 40);
    
    // 绘制背景
    painter.fillRect(rect(), Qt::white);
    
    // 绘制坐标轴
    painter.setPen(QPen(Qt::black, 1));
    
    // X轴
    painter.drawLine(m_plotRect.left(), m_plotRect.bottom(), m_plotRect.right(), m_plotRect.bottom());
    
    // Y轴
    painter.drawLine(m_plotRect.left(), m_plotRect.top(), m_plotRect.left(), m_plotRect.bottom());
    
    // 绘制刻度和标签
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    
    // X轴刻度和标签
    int xTickCount = 8;
    for (int i = 0; i <= xTickCount; ++i) {
        double x = m_xMin + (m_xMax - m_xMin) * i / xTickCount;
        QPointF screenPos = dataToScreen(QPointF(x, m_yMin));
        
        // 绘制刻度线
        painter.drawLine(screenPos.x(), screenPos.y(), screenPos.x(), screenPos.y() - 5);
        
        // 绘制标签
        painter.drawText(screenPos.x() - 10, screenPos.y() + 15, QString::number(x, 'f', 1));
    }
    
    // Y轴刻度和标签
    int yTickCount = 6;
    for (int i = 0; i <= yTickCount; ++i) {
        double y = m_yMin + (m_yMax - m_yMin) * i / yTickCount;
        QPointF screenPos = dataToScreen(QPointF(m_xMin, y));
        
        // 绘制刻度线
        painter.drawLine(screenPos.x(), screenPos.y(), screenPos.x() + 5, screenPos.y());
        
        // 绘制标签
        painter.drawText(screenPos.x() - 40, screenPos.y() + 4, QString::number(y, 'f', 1));
    }
    
    // 绘制波形
    if (m_xData.size() > 1 && m_yData.size() > 1) {
        painter.setPen(QPen(Qt::blue, 1.0)); // 细线以提高性能
        
        // 只绘制可见区域内的波形
        int startIdx = 0;
        int endIdx = m_xData.size() - 1;
        
        // 找到可见区域的起始和结束索引
        while (startIdx < endIdx && m_xData[startIdx] < m_xMin) {
            startIdx++;
        }
        while (endIdx > startIdx && m_xData[endIdx] > m_xMax) {
            endIdx--;
        }
        
        // 绘制可见的波形段
        for (int i = startIdx; i < endIdx; ++i) {
            QPointF startPoint = dataToScreen(QPointF(m_xData[i], m_yData[i]));
            QPointF endPoint = dataToScreen(QPointF(m_xData[i + 1], m_yData[i + 1]));
            
            // 检查点是否在绘图区域内
            if (m_plotRect.intersects(QRect(startPoint.toPoint(), endPoint.toPoint()))) {
                painter.drawLine(startPoint, endPoint);
            }
        }
    }
    
    // 绘制通道名称
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(m_plotRect.left() + 10, m_plotRect.top() - 10, QString("通道 %1").arg(m_channelIndex + 1));
    
    // 绘制框选矩形
    if (m_isSelecting && !m_selectionRect.isNull()) {
        painter.setPen(QPen(Qt::red, 1, Qt::DashLine));
        painter.setBrush(QBrush(QColor(255, 0, 0, 50))); // 半透明红色
        painter.drawRect(m_selectionRect);
    }
}

void WaveformPlot::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging) {
        // 处理平移
        int deltaX = event->pos().x() - m_lastMousePos.x();
        int deltaY = event->pos().y() - m_lastMousePos.y();
        
        m_translateX += deltaX;
        m_translateY += deltaY;
        
        m_lastMousePos = event->pos();
        update();
    } else if (m_isSelecting) {
        // 处理框选
        m_selectionRect = QRect(m_selectionStart, event->pos()).normalized();
        update();
    } else {
        // 显示鼠标悬停时的坐标
        if (m_plotRect.contains(event->pos())) {
            QPointF dataPos = screenToData(event->pos());
            QString tooltipText = QString("X: %1\nY: %2").arg(dataPos.x(), 0, 'f', 2).arg(dataPos.y(), 0, 'f', 2);
            QToolTip::showText(event->globalPos(), tooltipText, this);
        } else {
            QToolTip::hideText();
        }
    }
}

void WaveformPlot::wheelEvent(QWheelEvent *event)
{
    // 鼠标滚轮用于平移操作
    double panFactor = 0.1;
    int delta = event->delta();
    
    if (event->modifiers() == Qt::ControlModifier) {
        // 按住Ctrl键时上下平移
        if (delta > 0) {
            // 向上平移
            m_translateY += m_plotRect.height() * panFactor;
        } else {
            // 向下平移
            m_translateY -= m_plotRect.height() * panFactor;
        }
    } else {
        // 默认左右平移
        if (delta > 0) {
            // 向左平移
            m_translateX += m_plotRect.width() * panFactor;
        } else {
            // 向右平移
            m_translateX -= m_plotRect.width() * panFactor;
        }
    }
    
    update();
}

void WaveformPlot::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_plotRect.contains(event->pos())) {
        // 开始框选
        m_isSelecting = true;
        m_selectionStart = event->pos();
        m_selectionRect = QRect();
        setCursor(Qt::CrossCursor);
    } else if (event->button() == Qt::RightButton && m_plotRect.contains(event->pos())) {
        // 显示上下文菜单
        m_contextMenu->exec(event->globalPos());
    }
}

void WaveformPlot::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isSelecting) {
        // 完成框选
        m_isSelecting = false;
        setCursor(Qt::ArrowCursor);
        
        // 如果框选矩形有效，执行缩放
        if (!m_selectionRect.isNull() && m_selectionRect.width() > 5 && m_selectionRect.height() > 5) {
            // 获取框选区域的左上角和右下角的实际坐标
            QPointF topLeft = screenToData(m_selectionRect.topLeft());
            QPointF bottomRight = screenToData(m_selectionRect.bottomRight());
            
            // 根据当前缩放模式更新绘图范围
            switch (m_currentScaleMode) {
            case FreeScale:
                // 自由缩放：同时更新X和Y轴的范围
                m_xMin = topLeft.x();
                m_xMax = bottomRight.x();
                m_yMin = topLeft.y();
                m_yMax = bottomRight.y();
                break;
            case HorizontalScale:
                // 水平缩放：只更新X轴的范围，保持Y轴范围不变
                m_xMin = topLeft.x();
                m_xMax = bottomRight.x();
                break;
            case VerticalScale:
                // 垂直缩放：只更新Y轴的范围，保持X轴范围不变
                m_yMin = topLeft.y();
                m_yMax = bottomRight.y();
                break;
            }
            
            // 重置缩放和平移
            m_xScaleFactor = 1.0;
            m_yScaleFactor = 1.0;
            m_translateX = 0.0;
            m_translateY = 0.0;
            
            update();
        }
        
        m_selectionRect = QRect();
    }
}

void WaveformPlot::mouseDoubleClickEvent(QMouseEvent *event)
{
    // 重置缩放和平移
    m_xScaleFactor = 1.0;
    m_yScaleFactor = 1.0;
    m_translateX = 0.0;
    m_translateY = 0.0;
    update();
}
