QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SignalAnalyzer
TEMPLATE = app

SOURCES += ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/udpreceiver.cpp

HEADERS  += ../src/mainwindow.h \
    ../src/udpreceiver.h

FORMS    += ../src/mainwindow.ui

INCLUDEPATH += ../src
