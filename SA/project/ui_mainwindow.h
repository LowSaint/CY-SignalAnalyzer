/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *networkInterfaceLabel;
    QComboBox *networkInterfaceComboBox;
    QLabel *label;
    QLabel *ipAddressLabel;
    QLabel *portLabel;
    QLineEdit *portLineEdit;
    QPushButton *startButton;
    QLabel *packetCountLabel;
    QTextEdit *packetDataTextEdit;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(600, 400);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        networkInterfaceLabel = new QLabel(centralWidget);
        networkInterfaceLabel->setObjectName(QString::fromUtf8("networkInterfaceLabel"));

        gridLayout->addWidget(networkInterfaceLabel, 0, 0, 1, 1);

        networkInterfaceComboBox = new QComboBox(centralWidget);
        networkInterfaceComboBox->setObjectName(QString::fromUtf8("networkInterfaceComboBox"));

        gridLayout->addWidget(networkInterfaceComboBox, 0, 1, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 2, 1, 1);

        ipAddressLabel = new QLabel(centralWidget);
        ipAddressLabel->setObjectName(QString::fromUtf8("ipAddressLabel"));
        ipAddressLabel->setFrameShape(QFrame::Box);
        ipAddressLabel->setFrameShadow(QFrame::Sunken);
        ipAddressLabel->setLineWidth(1);
        ipAddressLabel->setMidLineWidth(0);

        gridLayout->addWidget(ipAddressLabel, 0, 3, 1, 1);

        portLabel = new QLabel(centralWidget);
        portLabel->setObjectName(QString::fromUtf8("portLabel"));

        gridLayout->addWidget(portLabel, 1, 0, 1, 1);

        portLineEdit = new QLineEdit(centralWidget);
        portLineEdit->setObjectName(QString::fromUtf8("portLineEdit"));

        gridLayout->addWidget(portLineEdit, 1, 1, 1, 1);

        startButton = new QPushButton(centralWidget);
        startButton->setObjectName(QString::fromUtf8("startButton"));

        gridLayout->addWidget(startButton, 1, 2, 1, 2);


        verticalLayout->addLayout(gridLayout);

        packetCountLabel = new QLabel(centralWidget);
        packetCountLabel->setObjectName(QString::fromUtf8("packetCountLabel"));
        packetCountLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(packetCountLabel);

        packetDataTextEdit = new QTextEdit(centralWidget);
        packetDataTextEdit->setObjectName(QString::fromUtf8("packetDataTextEdit"));
        packetDataTextEdit->setReadOnly(true);

        verticalLayout->addWidget(packetDataTextEdit);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        networkInterfaceLabel->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\347\275\221\345\215\241:", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "IP\345\234\260\345\235\200:", nullptr));
        ipAddressLabel->setText(QCoreApplication::translate("MainWindow", "0.0.0.0", nullptr));
        portLabel->setText(QCoreApplication::translate("MainWindow", "\347\233\256\347\232\204\347\253\257\345\217\243:", nullptr));
        portLineEdit->setInputMask(QCoreApplication::translate("MainWindow", "99999", nullptr));
        portLineEdit->setText(QCoreApplication::translate("MainWindow", "60002", nullptr));
        startButton->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\346\216\245\346\224\266", nullptr));
        packetCountLabel->setText(QCoreApplication::translate("MainWindow", "\345\267\262\346\216\245\346\224\266\346\225\260\346\215\256\345\214\205\346\225\260\351\207\217: 0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
