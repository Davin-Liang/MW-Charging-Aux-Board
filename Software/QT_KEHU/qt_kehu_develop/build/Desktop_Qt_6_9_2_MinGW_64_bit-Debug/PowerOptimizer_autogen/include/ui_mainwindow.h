/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *groupBox_connection;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButton_connect;
    QPushButton *pushButton_disconnect;
    QLabel *label_5;
    QLabel *label_status;
    QWidget *widget1;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit_local_ip;
    QLabel *label_4;
    QLineEdit *lineEdit_local_port;
    QWidget *widget2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit_remote_ip;
    QLabel *label_3;
    QLineEdit *lineEdit_remote_port;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(623, 510);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        groupBox_connection = new QGroupBox(centralwidget);
        groupBox_connection->setObjectName("groupBox_connection");
        groupBox_connection->setGeometry(QRect(60, 230, 481, 181));
        widget = new QWidget(groupBox_connection);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(30, 110, 250, 25));
        horizontalLayout_3 = new QHBoxLayout(widget);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        pushButton_connect = new QPushButton(widget);
        pushButton_connect->setObjectName("pushButton_connect");
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        pushButton_connect->setFont(font);

        horizontalLayout_3->addWidget(pushButton_connect);

        pushButton_disconnect = new QPushButton(widget);
        pushButton_disconnect->setObjectName("pushButton_disconnect");
        pushButton_disconnect->setEnabled(false);
        pushButton_disconnect->setFont(font);

        horizontalLayout_3->addWidget(pushButton_disconnect);

        label_5 = new QLabel(widget);
        label_5->setObjectName("label_5");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        font1.setPointSize(12);
        label_5->setFont(font1);

        horizontalLayout_3->addWidget(label_5);

        label_status = new QLabel(widget);
        label_status->setObjectName("label_status");
        label_status->setFont(font1);

        horizontalLayout_3->addWidget(label_status);

        widget1 = new QWidget(groupBox_connection);
        widget1->setObjectName("widget1");
        widget1->setGeometry(QRect(30, 70, 430, 23));
        horizontalLayout_2 = new QHBoxLayout(widget1);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(widget1);
        label_2->setObjectName("label_2");
        label_2->setFont(font1);

        horizontalLayout_2->addWidget(label_2);

        lineEdit_local_ip = new QLineEdit(widget1);
        lineEdit_local_ip->setObjectName("lineEdit_local_ip");

        horizontalLayout_2->addWidget(lineEdit_local_ip);

        label_4 = new QLabel(widget1);
        label_4->setObjectName("label_4");
        label_4->setFont(font1);

        horizontalLayout_2->addWidget(label_4);

        lineEdit_local_port = new QLineEdit(widget1);
        lineEdit_local_port->setObjectName("lineEdit_local_port");

        horizontalLayout_2->addWidget(lineEdit_local_port);

        widget2 = new QWidget(groupBox_connection);
        widget2->setObjectName("widget2");
        widget2->setGeometry(QRect(31, 31, 430, 23));
        horizontalLayout = new QHBoxLayout(widget2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget2);
        label->setObjectName("label");
        label->setFont(font1);

        horizontalLayout->addWidget(label);

        lineEdit_remote_ip = new QLineEdit(widget2);
        lineEdit_remote_ip->setObjectName("lineEdit_remote_ip");

        horizontalLayout->addWidget(lineEdit_remote_ip);

        label_3 = new QLabel(widget2);
        label_3->setObjectName("label_3");
        label_3->setFont(font1);

        horizontalLayout->addWidget(label_3);

        lineEdit_remote_port = new QLineEdit(widget2);
        lineEdit_remote_port->setObjectName("lineEdit_remote_port");

        horizontalLayout->addWidget(lineEdit_remote_port);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 623, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox_connection->setTitle(QString());
        pushButton_connect->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\350\256\276\345\244\207", nullptr));
        pushButton_disconnect->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        label_status->setText(QCoreApplication::translate("MainWindow", "\346\234\252\350\277\236\346\216\245", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260IP\345\234\260\345\235\200", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260\347\253\257\345\217\243", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\350\277\234\347\250\213IP\345\234\260\345\235\200", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\350\277\234\347\250\213\347\253\257\345\217\243", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
