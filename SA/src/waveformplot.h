#ifndef WAVEFORMPLOT_H
#define WAVEFORMPLOT_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>
#include <QMenu>
#include <QAction>

class WaveformPlot : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformPlot(int channelIndex, QWidget *parent = nullptr);
    ~WaveformPlot();
    
    // 缩放模式枚举
    enum ScaleMode {
        FreeScale,     // 自由缩放（同时缩放X和Y轴）
        HorizontalScale, // 仅水平缩放（X轴）
        VerticalScale    // 仅垂直缩放（Y轴）
    };

    void addDataPoint(double x, double y);
    void clearData();
    void pause();
    void resume();
    bool isPaused() const;
    void setTimeWindow(double timeWindow); // 设置时间窗口大小
    void setScaleMode(ScaleMode mode); // 设置缩放模式

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void togglePause();
    void updateTimerTimeout();

private:
    int m_channelIndex;
    QVector<double> m_xData;
    QVector<double> m_yData;
    bool m_paused;
    QPushButton *m_pauseButton;
    QVBoxLayout *m_mainLayout;
    
    // 缩放和平移相关变量
    bool m_isDragging;
    bool m_isSelecting; // 表示是否正在进行框选
    QPoint m_lastMousePos;
    QPoint m_selectionStart; // 框选起始点
    QRect m_selectionRect; // 框选矩形区域
    double m_xScaleFactor; // X轴缩放因子
    double m_yScaleFactor; // Y轴缩放因子
    double m_translateX;
    double m_translateY;
    ScaleMode m_currentScaleMode; // 当前缩放模式
    QMenu *m_contextMenu; // 右键菜单
    
    // 绘图区域相关
    QRect m_plotRect;
    double m_xMin, m_xMax, m_yMin, m_yMax;
    double m_timeWindow; // 时间窗口大小（秒），用于示波器滚动显示
    
    // 性能优化相关变量
    QTimer *m_updateTimer;
    bool m_needsUpdate;
    int m_dataPointsSinceLastUpdate;
    static const int UPDATE_INTERVAL = 30; // 毫秒
    static const int DATA_POINTS_PER_UPDATE = 10; // 每批次更新的数据点数量
    
    void updatePlotLimits();
    QPointF dataToScreen(const QPointF &dataPoint) const;
    QPointF screenToData(const QPointF &screenPoint) const;
};

#endif // WAVEFORMPLOT_H
