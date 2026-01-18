#ifndef WAVEFORMWINDOW_H
#define WAVEFORMWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include "waveformplot.h"

class WaveformWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WaveformWindow(QWidget *parent = nullptr);
    ~WaveformWindow();

    QVector<WaveformPlot*> getWaveformPlots() const;
    void clearAllData();

private:
    QWidget *m_centralWidget;
    QGridLayout *m_plotLayout;
    QVector<WaveformPlot*> m_waveformPlots;
    static const int NUM_CHANNELS = 8;
};

#endif // WAVEFORMWINDOW_H
