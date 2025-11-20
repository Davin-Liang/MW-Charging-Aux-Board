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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *groupBox_connection;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit_remote_ip;
    QLabel *label_3;
    QLineEdit *lineEdit_remote_port;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit_local_ip;
    QLabel *label_4;
    QLineEdit *lineEdit_local_port;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButton_connect;
    QPushButton *pushButton_disconnect;
    QLabel *label_5;
    QLabel *label_status;
    QGroupBox *groupBox_control;
    QPushButton *pushButton_execute;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QComboBox *comboBox_cmd1;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_7;
    QComboBox *comboBox_cmd2;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_8;
    QComboBox *comboBox_cmd3;
    QWidget *layoutWidget3;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_9;
    QComboBox *comboBox_cmd4;
    QWidget *layoutWidget4;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_10;
    QComboBox *comboBox_cmd5;
    QGroupBox *groupBox_monitor;
    QWidget *layoutWidget5;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_11;
    QTextEdit *textEdit_message;
    QWidget *layoutWidget6;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_12;
    QPushButton *pushButton_read_sd;
    QTextEdit *textEdit_sd_data;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(663, 521);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        groupBox_connection = new QGroupBox(centralwidget);
        groupBox_connection->setObjectName("groupBox_connection");
        groupBox_connection->setGeometry(QRect(0, 0, 451, 131));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        font.setPointSize(14);
        groupBox_connection->setFont(font);
        groupBox_connection->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        verticalLayout = new QVBoxLayout(groupBox_connection);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(groupBox_connection);
        label->setObjectName("label");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        font1.setPointSize(12);
        label->setFont(font1);

        horizontalLayout->addWidget(label);

        lineEdit_remote_ip = new QLineEdit(groupBox_connection);
        lineEdit_remote_ip->setObjectName("lineEdit_remote_ip");

        horizontalLayout->addWidget(lineEdit_remote_ip);

        label_3 = new QLabel(groupBox_connection);
        label_3->setObjectName("label_3");
        label_3->setFont(font1);

        horizontalLayout->addWidget(label_3);

        lineEdit_remote_port = new QLineEdit(groupBox_connection);
        lineEdit_remote_port->setObjectName("lineEdit_remote_port");

        horizontalLayout->addWidget(lineEdit_remote_port);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_2 = new QLabel(groupBox_connection);
        label_2->setObjectName("label_2");
        label_2->setFont(font1);

        horizontalLayout_2->addWidget(label_2);

        lineEdit_local_ip = new QLineEdit(groupBox_connection);
        lineEdit_local_ip->setObjectName("lineEdit_local_ip");

        horizontalLayout_2->addWidget(lineEdit_local_ip);

        label_4 = new QLabel(groupBox_connection);
        label_4->setObjectName("label_4");
        label_4->setFont(font1);

        horizontalLayout_2->addWidget(label_4);

        lineEdit_local_port = new QLineEdit(groupBox_connection);
        lineEdit_local_port->setObjectName("lineEdit_local_port");

        horizontalLayout_2->addWidget(lineEdit_local_port);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        pushButton_connect = new QPushButton(groupBox_connection);
        pushButton_connect->setObjectName("pushButton_connect");
        pushButton_connect->setFont(font);

        horizontalLayout_3->addWidget(pushButton_connect);

        pushButton_disconnect = new QPushButton(groupBox_connection);
        pushButton_disconnect->setObjectName("pushButton_disconnect");
        pushButton_disconnect->setEnabled(false);
        pushButton_disconnect->setFont(font);

        horizontalLayout_3->addWidget(pushButton_disconnect);

        label_5 = new QLabel(groupBox_connection);
        label_5->setObjectName("label_5");
        label_5->setFont(font1);

        horizontalLayout_3->addWidget(label_5);

        label_status = new QLabel(groupBox_connection);
        label_status->setObjectName("label_status");
        label_status->setFont(font1);

        horizontalLayout_3->addWidget(label_status);


        verticalLayout->addLayout(horizontalLayout_3);

        groupBox_control = new QGroupBox(centralwidget);
        groupBox_control->setObjectName("groupBox_control");
        groupBox_control->setGeometry(QRect(10, 140, 231, 341));
        groupBox_control->setFont(font1);
        pushButton_execute = new QPushButton(groupBox_control);
        pushButton_execute->setObjectName("pushButton_execute");
        pushButton_execute->setGeometry(QRect(40, 280, 131, 31));
        pushButton_execute->setFont(font);
        layoutWidget = new QWidget(groupBox_control);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 30, 187, 24));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName("label_6");

        horizontalLayout_4->addWidget(label_6);

        comboBox_cmd1 = new QComboBox(layoutWidget);
        comboBox_cmd1->setObjectName("comboBox_cmd1");

        horizontalLayout_4->addWidget(comboBox_cmd1);

        layoutWidget1 = new QWidget(groupBox_control);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(20, 80, 187, 24));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(layoutWidget1);
        label_7->setObjectName("label_7");

        horizontalLayout_5->addWidget(label_7);

        comboBox_cmd2 = new QComboBox(layoutWidget1);
        comboBox_cmd2->setObjectName("comboBox_cmd2");

        horizontalLayout_5->addWidget(comboBox_cmd2);

        layoutWidget2 = new QWidget(groupBox_control);
        layoutWidget2->setObjectName("layoutWidget2");
        layoutWidget2->setGeometry(QRect(20, 130, 187, 24));
        horizontalLayout_6 = new QHBoxLayout(layoutWidget2);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_8 = new QLabel(layoutWidget2);
        label_8->setObjectName("label_8");

        horizontalLayout_6->addWidget(label_8);

        comboBox_cmd3 = new QComboBox(layoutWidget2);
        comboBox_cmd3->setObjectName("comboBox_cmd3");

        horizontalLayout_6->addWidget(comboBox_cmd3);

        layoutWidget3 = new QWidget(groupBox_control);
        layoutWidget3->setObjectName("layoutWidget3");
        layoutWidget3->setGeometry(QRect(20, 180, 187, 24));
        horizontalLayout_7 = new QHBoxLayout(layoutWidget3);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        label_9 = new QLabel(layoutWidget3);
        label_9->setObjectName("label_9");

        horizontalLayout_7->addWidget(label_9);

        comboBox_cmd4 = new QComboBox(layoutWidget3);
        comboBox_cmd4->setObjectName("comboBox_cmd4");

        horizontalLayout_7->addWidget(comboBox_cmd4);

        layoutWidget4 = new QWidget(groupBox_control);
        layoutWidget4->setObjectName("layoutWidget4");
        layoutWidget4->setGeometry(QRect(20, 230, 187, 24));
        horizontalLayout_8 = new QHBoxLayout(layoutWidget4);
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        label_10 = new QLabel(layoutWidget4);
        label_10->setObjectName("label_10");

        horizontalLayout_8->addWidget(label_10);

        comboBox_cmd5 = new QComboBox(layoutWidget4);
        comboBox_cmd5->setObjectName("comboBox_cmd5");

        horizontalLayout_8->addWidget(comboBox_cmd5);

        groupBox_monitor = new QGroupBox(centralwidget);
        groupBox_monitor->setObjectName("groupBox_monitor");
        groupBox_monitor->setGeometry(QRect(240, 140, 401, 341));
        groupBox_monitor->setFont(font);
        layoutWidget5 = new QWidget(groupBox_monitor);
        layoutWidget5->setObjectName("layoutWidget5");
        layoutWidget5->setGeometry(QRect(20, 30, 161, 291));
        verticalLayout_2 = new QVBoxLayout(layoutWidget5);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(layoutWidget5);
        label_11->setObjectName("label_11");
        label_11->setFont(font1);

        verticalLayout_2->addWidget(label_11);

        textEdit_message = new QTextEdit(layoutWidget5);
        textEdit_message->setObjectName("textEdit_message");

        verticalLayout_2->addWidget(textEdit_message);

        layoutWidget6 = new QWidget(groupBox_monitor);
        layoutWidget6->setObjectName("layoutWidget6");
        layoutWidget6->setGeometry(QRect(200, 30, 181, 291));
        verticalLayout_3 = new QVBoxLayout(layoutWidget6);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_12 = new QLabel(layoutWidget6);
        label_12->setObjectName("label_12");
        label_12->setFont(font1);

        verticalLayout_3->addWidget(label_12);

        pushButton_read_sd = new QPushButton(layoutWidget6);
        pushButton_read_sd->setObjectName("pushButton_read_sd");
        pushButton_read_sd->setFont(font);

        verticalLayout_3->addWidget(pushButton_read_sd);

        textEdit_sd_data = new QTextEdit(layoutWidget6);
        textEdit_sd_data->setObjectName("textEdit_sd_data");

        verticalLayout_3->addWidget(textEdit_sd_data);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 663, 21));
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
        groupBox_connection->setTitle(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\350\256\276\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\350\277\234\347\250\213IP\345\234\260\345\235\200", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\350\277\234\347\250\213\347\253\257\345\217\243", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260IP\345\234\260\345\235\200", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260\347\253\257\345\217\243", nullptr));
        pushButton_connect->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\350\256\276\345\244\207", nullptr));
        pushButton_disconnect->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        label_status->setText(QCoreApplication::translate("MainWindow", "\346\234\252\350\277\236\346\216\245", nullptr));
        groupBox_control->setTitle(QCoreApplication::translate("MainWindow", "\346\216\247\345\210\266\346\214\207\344\273\244\350\256\276\347\275\256", nullptr));
        pushButton_execute->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\346\216\247\345\210\266\346\214\207\344\273\244", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\350\275\250\350\277\271\350\256\276\345\256\232", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\350\275\250\350\277\271\345\215\212\345\276\204", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\350\275\250\350\277\271\346\255\245\351\225\277", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\351\200\232\351\201\223\346\234\200\345\244\247\347\224\265\345\216\213", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\347\224\265\345\216\213\345\257\273\344\274\230\346\255\245\351\225\277", nullptr));
        groupBox_monitor->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\347\233\221\346\216\247", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\346\266\210\346\201\257\346\216\245\346\224\266\346\241\206", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "SD\345\215\241\346\225\260\346\215\256", nullptr));
        pushButton_read_sd->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226SD\345\215\241\346\225\260\346\215\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
