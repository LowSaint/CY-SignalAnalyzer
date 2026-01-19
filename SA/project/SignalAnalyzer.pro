QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SignalAnalyzer
TEMPLATE = app

SOURCES += ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/udpreceiver.cpp \
    ../src/waveformplot.cpp \
    ../src/waveformwindow.cpp \
    ../src/workthread.cpp

HEADERS  += ../src/mainwindow.h \
    ../src/udpreceiver.h \
    ../src/waveformplot.h \
    ../src/waveformwindow.h \
    ../src/workthread.h

FORMS    += ../src/mainwindow.ui

INCLUDEPATH += ../src
