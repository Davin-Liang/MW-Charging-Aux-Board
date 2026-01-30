/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
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
    QTabWidget *tabWidget;
    QWidget *tab_3;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_20;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *lineEdit_motor_x;
    QLabel *label_13;
    QHBoxLayout *horizontalLayout_5;
    QLineEdit *lineEdit_motor_y;
    QLabel *label_14;
    QHBoxLayout *horizontalLayout_6;
    QLineEdit *lineEdit_motor_speed;
    QLabel *label_15;
    QPushButton *pushButton_motor_control;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_7;
    QComboBox *comboBox_traj_type;
    QLabel *label_16;
    QHBoxLayout *horizontalLayout_8;
    QLineEdit *lineEdit_square_step;
    QLabel *label_8;
    QHBoxLayout *horizontalLayout_9;
    QLineEdit *lineEdit_circle_radius;
    QLabel *label_7;
    QHBoxLayout *horizontalLayout_10;
    QLineEdit *lineEdit_max_voltage;
    QLabel *label_10;
    QHBoxLayout *horizontalLayout_11;
    QLineEdit *lineEdit_voltage_step;
    QLabel *label_9;
    QPushButton *pushButton_find_optimal;
    QWidget *tab_4;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_7;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_6;
    QLineEdit *lineEdit_read_motor_x;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_12;
    QLineEdit *lineEdit_read_motor_y;
    QHBoxLayout *horizontalLayout_19;
    QLabel *label_17;
    QLineEdit *lineEdit_read_motor_speed;
    QGroupBox *groupBox_chart;
    QVBoxLayout *verticalLayout_9;
    QWidget *widget_chart;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_7;
    QHBoxLayout *horizontalLayout_18;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_18;
    QLineEdit *lineEdit_read_currentchannel;
    QLabel *label_19;
    QLineEdit *lineEdit_read_currentvoltage;
    QLabel *label_20;
    QLineEdit *lineEdit_read_currentpower;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_21;
    QLineEdit *lineEdit_read_channel1voltagemax;
    QLabel *label_22;
    QLineEdit *lineEdit_read_channel2voltagemax;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_23;
    QLineEdit *lineEdit_read_channel3voltagemax;
    QLabel *label_24;
    QLineEdit *lineEdit_read_channel4voltagemax;
    QHBoxLayout *horizontalLayout_17;
    QLabel *label_25;
    QLineEdit *lineEdit_read_powermax;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_8;
    QTextEdit *textEdit_message;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(973, 573);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        groupBox_connection = new QGroupBox(centralwidget);
        groupBox_connection->setObjectName(QString::fromUtf8("groupBox_connection"));
        groupBox_connection->setGeometry(QRect(20, 10, 811, 131));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font.setPointSize(14);
        groupBox_connection->setFont(font);
        groupBox_connection->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        verticalLayout = new QVBoxLayout(groupBox_connection);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(groupBox_connection);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font1.setPointSize(12);
        label->setFont(font1);

        horizontalLayout->addWidget(label);

        lineEdit_remote_ip = new QLineEdit(groupBox_connection);
        lineEdit_remote_ip->setObjectName(QString::fromUtf8("lineEdit_remote_ip"));

        horizontalLayout->addWidget(lineEdit_remote_ip);

        label_3 = new QLabel(groupBox_connection);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font1);

        horizontalLayout->addWidget(label_3);

        lineEdit_remote_port = new QLineEdit(groupBox_connection);
        lineEdit_remote_port->setObjectName(QString::fromUtf8("lineEdit_remote_port"));

        horizontalLayout->addWidget(lineEdit_remote_port);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(groupBox_connection);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font1);

        horizontalLayout_2->addWidget(label_2);

        lineEdit_local_ip = new QLineEdit(groupBox_connection);
        lineEdit_local_ip->setObjectName(QString::fromUtf8("lineEdit_local_ip"));

        horizontalLayout_2->addWidget(lineEdit_local_ip);

        label_4 = new QLabel(groupBox_connection);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font1);

        horizontalLayout_2->addWidget(label_4);

        lineEdit_local_port = new QLineEdit(groupBox_connection);
        lineEdit_local_port->setObjectName(QString::fromUtf8("lineEdit_local_port"));

        horizontalLayout_2->addWidget(lineEdit_local_port);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pushButton_connect = new QPushButton(groupBox_connection);
        pushButton_connect->setObjectName(QString::fromUtf8("pushButton_connect"));
        pushButton_connect->setFont(font);

        horizontalLayout_3->addWidget(pushButton_connect);

        pushButton_disconnect = new QPushButton(groupBox_connection);
        pushButton_disconnect->setObjectName(QString::fromUtf8("pushButton_disconnect"));
        pushButton_disconnect->setEnabled(false);
        pushButton_disconnect->setFont(font);

        horizontalLayout_3->addWidget(pushButton_disconnect);

        label_5 = new QLabel(groupBox_connection);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font1);

        horizontalLayout_3->addWidget(label_5);

        label_status = new QLabel(groupBox_connection);
        label_status->setObjectName(QString::fromUtf8("label_status"));
        label_status->setFont(font1);
        label_status->setTextFormat(Qt::TextFormat::PlainText);

        horizontalLayout_3->addWidget(label_status);


        verticalLayout->addLayout(horizontalLayout_3);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 140, 821, 421));
        tabWidget->setIconSize(QSize(20, 20));
        tabWidget->setDocumentMode(false);
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        layoutWidget = new QWidget(tab_3);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 801, 381));
        horizontalLayout_20 = new QHBoxLayout(layoutWidget);
        horizontalLayout_20->setObjectName(QString::fromUtf8("horizontalLayout_20"));
        horizontalLayout_20->setContentsMargins(0, 0, 0, 0);
        groupBox = new QGroupBox(layoutWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setFont(font);
        verticalLayout_5 = new QVBoxLayout(groupBox);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        lineEdit_motor_x = new QLineEdit(groupBox);
        lineEdit_motor_x->setObjectName(QString::fromUtf8("lineEdit_motor_x"));

        horizontalLayout_4->addWidget(lineEdit_motor_x, 0, Qt::AlignmentFlag::AlignLeft);

        label_13 = new QLabel(groupBox);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_4->addWidget(label_13);


        verticalLayout_5->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        lineEdit_motor_y = new QLineEdit(groupBox);
        lineEdit_motor_y->setObjectName(QString::fromUtf8("lineEdit_motor_y"));

        horizontalLayout_5->addWidget(lineEdit_motor_y);

        label_14 = new QLabel(groupBox);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_5->addWidget(label_14);


        verticalLayout_5->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        lineEdit_motor_speed = new QLineEdit(groupBox);
        lineEdit_motor_speed->setObjectName(QString::fromUtf8("lineEdit_motor_speed"));

        horizontalLayout_6->addWidget(lineEdit_motor_speed);

        label_15 = new QLabel(groupBox);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        horizontalLayout_6->addWidget(label_15);


        verticalLayout_5->addLayout(horizontalLayout_6);

        pushButton_motor_control = new QPushButton(groupBox);
        pushButton_motor_control->setObjectName(QString::fromUtf8("pushButton_motor_control"));
        pushButton_motor_control->setFont(font);

        verticalLayout_5->addWidget(pushButton_motor_control);


        horizontalLayout_20->addWidget(groupBox);

        groupBox_2 = new QGroupBox(layoutWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setFont(font);
        verticalLayout_4 = new QVBoxLayout(groupBox_2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        comboBox_traj_type = new QComboBox(groupBox_2);
        comboBox_traj_type->setObjectName(QString::fromUtf8("comboBox_traj_type"));

        horizontalLayout_7->addWidget(comboBox_traj_type);

        label_16 = new QLabel(groupBox_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        horizontalLayout_7->addWidget(label_16);


        verticalLayout_4->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        lineEdit_square_step = new QLineEdit(groupBox_2);
        lineEdit_square_step->setObjectName(QString::fromUtf8("lineEdit_square_step"));

        horizontalLayout_8->addWidget(lineEdit_square_step);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_8->addWidget(label_8);


        verticalLayout_4->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        lineEdit_circle_radius = new QLineEdit(groupBox_2);
        lineEdit_circle_radius->setObjectName(QString::fromUtf8("lineEdit_circle_radius"));

        horizontalLayout_9->addWidget(lineEdit_circle_radius);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_9->addWidget(label_7);


        verticalLayout_4->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        lineEdit_max_voltage = new QLineEdit(groupBox_2);
        lineEdit_max_voltage->setObjectName(QString::fromUtf8("lineEdit_max_voltage"));

        horizontalLayout_10->addWidget(lineEdit_max_voltage);

        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_10->addWidget(label_10);


        verticalLayout_4->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        lineEdit_voltage_step = new QLineEdit(groupBox_2);
        lineEdit_voltage_step->setObjectName(QString::fromUtf8("lineEdit_voltage_step"));

        horizontalLayout_11->addWidget(lineEdit_voltage_step);

        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_11->addWidget(label_9);


        verticalLayout_4->addLayout(horizontalLayout_11);

        pushButton_find_optimal = new QPushButton(groupBox_2);
        pushButton_find_optimal->setObjectName(QString::fromUtf8("pushButton_find_optimal"));
        pushButton_find_optimal->setFont(font);

        verticalLayout_4->addWidget(pushButton_find_optimal);


        horizontalLayout_20->addWidget(groupBox_2);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        groupBox_5 = new QGroupBox(tab_4);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(20, 20, 271, 171));
        groupBox_5->setFont(font);
        verticalLayout_7 = new QVBoxLayout(groupBox_5);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        label_6 = new QLabel(groupBox_5);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFont(font);

        horizontalLayout_12->addWidget(label_6);

        lineEdit_read_motor_x = new QLineEdit(groupBox_5);
        lineEdit_read_motor_x->setObjectName(QString::fromUtf8("lineEdit_read_motor_x"));

        horizontalLayout_12->addWidget(lineEdit_read_motor_x);


        verticalLayout_6->addLayout(horizontalLayout_12);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setFont(font);

        horizontalLayout_13->addWidget(label_12);

        lineEdit_read_motor_y = new QLineEdit(groupBox_5);
        lineEdit_read_motor_y->setObjectName(QString::fromUtf8("lineEdit_read_motor_y"));

        horizontalLayout_13->addWidget(lineEdit_read_motor_y);


        verticalLayout_6->addLayout(horizontalLayout_13);

        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName(QString::fromUtf8("horizontalLayout_19"));
        label_17 = new QLabel(groupBox_5);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setFont(font);

        horizontalLayout_19->addWidget(label_17);

        lineEdit_read_motor_speed = new QLineEdit(groupBox_5);
        lineEdit_read_motor_speed->setObjectName(QString::fromUtf8("lineEdit_read_motor_speed"));

        horizontalLayout_19->addWidget(lineEdit_read_motor_speed);


        verticalLayout_6->addLayout(horizontalLayout_19);


        verticalLayout_7->addLayout(verticalLayout_6);

        groupBox_chart = new QGroupBox(tab_4);
        groupBox_chart->setObjectName(QString::fromUtf8("groupBox_chart"));
        groupBox_chart->setGeometry(QRect(300, 20, 501, 171));
        groupBox_chart->setFont(font);
        verticalLayout_9 = new QVBoxLayout(groupBox_chart);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        widget_chart = new QWidget(groupBox_chart);
        widget_chart->setObjectName(QString::fromUtf8("widget_chart"));

        verticalLayout_9->addWidget(widget_chart);

        widget = new QWidget(tab_4);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 200, 801, 181));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        groupBox_7 = new QGroupBox(widget);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setFont(font);
        horizontalLayout_18 = new QHBoxLayout(groupBox_7);
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        label_18 = new QLabel(groupBox_7);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setFont(font);

        horizontalLayout_14->addWidget(label_18);

        lineEdit_read_currentchannel = new QLineEdit(groupBox_7);
        lineEdit_read_currentchannel->setObjectName(QString::fromUtf8("lineEdit_read_currentchannel"));

        horizontalLayout_14->addWidget(lineEdit_read_currentchannel);

        label_19 = new QLabel(groupBox_7);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setFont(font);

        horizontalLayout_14->addWidget(label_19);

        lineEdit_read_currentvoltage = new QLineEdit(groupBox_7);
        lineEdit_read_currentvoltage->setObjectName(QString::fromUtf8("lineEdit_read_currentvoltage"));

        horizontalLayout_14->addWidget(lineEdit_read_currentvoltage);

        label_20 = new QLabel(groupBox_7);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setFont(font);

        horizontalLayout_14->addWidget(label_20);

        lineEdit_read_currentpower = new QLineEdit(groupBox_7);
        lineEdit_read_currentpower->setObjectName(QString::fromUtf8("lineEdit_read_currentpower"));

        horizontalLayout_14->addWidget(lineEdit_read_currentpower);


        horizontalLayout_18->addLayout(horizontalLayout_14);


        verticalLayout_2->addWidget(groupBox_7);

        groupBox_6 = new QGroupBox(widget);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setFont(font);
        verticalLayout_3 = new QVBoxLayout(groupBox_6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        label_21 = new QLabel(groupBox_6);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setFont(font);

        horizontalLayout_15->addWidget(label_21);

        lineEdit_read_channel1voltagemax = new QLineEdit(groupBox_6);
        lineEdit_read_channel1voltagemax->setObjectName(QString::fromUtf8("lineEdit_read_channel1voltagemax"));

        horizontalLayout_15->addWidget(lineEdit_read_channel1voltagemax);

        label_22 = new QLabel(groupBox_6);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setFont(font);

        horizontalLayout_15->addWidget(label_22);

        lineEdit_read_channel2voltagemax = new QLineEdit(groupBox_6);
        lineEdit_read_channel2voltagemax->setObjectName(QString::fromUtf8("lineEdit_read_channel2voltagemax"));

        horizontalLayout_15->addWidget(lineEdit_read_channel2voltagemax);


        verticalLayout_3->addLayout(horizontalLayout_15);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        label_23 = new QLabel(groupBox_6);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setFont(font);

        horizontalLayout_16->addWidget(label_23);

        lineEdit_read_channel3voltagemax = new QLineEdit(groupBox_6);
        lineEdit_read_channel3voltagemax->setObjectName(QString::fromUtf8("lineEdit_read_channel3voltagemax"));

        horizontalLayout_16->addWidget(lineEdit_read_channel3voltagemax);

        label_24 = new QLabel(groupBox_6);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        label_24->setFont(font);

        horizontalLayout_16->addWidget(label_24);

        lineEdit_read_channel4voltagemax = new QLineEdit(groupBox_6);
        lineEdit_read_channel4voltagemax->setObjectName(QString::fromUtf8("lineEdit_read_channel4voltagemax"));

        horizontalLayout_16->addWidget(lineEdit_read_channel4voltagemax);


        verticalLayout_3->addLayout(horizontalLayout_16);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        label_25 = new QLabel(groupBox_6);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setFont(font);

        horizontalLayout_17->addWidget(label_25);

        lineEdit_read_powermax = new QLineEdit(groupBox_6);
        lineEdit_read_powermax->setObjectName(QString::fromUtf8("lineEdit_read_powermax"));

        horizontalLayout_17->addWidget(lineEdit_read_powermax);


        verticalLayout_3->addLayout(horizontalLayout_17);


        verticalLayout_2->addWidget(groupBox_6);

        tabWidget->addTab(tab_4, QString());
        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(830, 20, 131, 541));
        groupBox_3->setFont(font);
        verticalLayout_8 = new QVBoxLayout(groupBox_3);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        textEdit_message = new QTextEdit(groupBox_3);
        textEdit_message->setObjectName(QString::fromUtf8("textEdit_message"));

        verticalLayout_8->addWidget(textEdit_message);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


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
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\346\216\247\345\210\266\346\214\207\344\273\244\350\256\276\347\275\256", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "X\345\235\220\346\240\207", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "Y\345\235\220\346\240\207", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246", nullptr));
        pushButton_motor_control->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\347\224\265\346\234\272\346\214\207\344\273\244", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\345\212\237\347\216\207\345\257\273\344\274\230\346\214\207\344\273\244\350\256\276\347\275\256", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "\350\275\250\350\277\271\350\256\276\345\256\232", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\346\226\271\345\275\242\346\255\245\351\225\277", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\345\234\206\345\275\242\345\215\212\345\276\204", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\346\234\200\345\244\247\347\224\265\345\216\213", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\347\224\265\345\216\213\346\255\245\351\225\277", nullptr));
        pushButton_find_optimal->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\345\257\273\344\274\230\346\214\207\344\273\244", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("MainWindow", "\346\216\247\345\210\266\346\214\207\344\273\244\350\256\276\347\275\256", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\345\275\223\345\211\215\347\212\266\346\200\201\350\257\273\345\217\226", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "x\345\235\220\346\240\207", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "y\345\235\220\346\240\207", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246", nullptr));
        groupBox_chart->setTitle(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\344\275\215\347\275\256\350\275\250\350\277\271\345\233\276", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MainWindow", "\345\257\273\344\274\230\350\277\207\347\250\213\346\225\260\346\215\256\346\230\276\347\244\272", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\351\200\232\351\201\223", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\347\224\265\345\216\213", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\345\212\237\347\216\207", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "\345\257\273\344\274\230\347\273\223\346\236\234\346\230\276\347\244\272", nullptr));
        label_21->setText(QCoreApplication::translate("MainWindow", "Channel1_Vmax", nullptr));
        label_22->setText(QCoreApplication::translate("MainWindow", "Channel2_Vmax", nullptr));
        label_23->setText(QCoreApplication::translate("MainWindow", "Channel3_Vmax", nullptr));
        label_24->setText(QCoreApplication::translate("MainWindow", "Channel4_Vmax", nullptr));
        label_25->setText(QCoreApplication::translate("MainWindow", "Pmax", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\347\233\221\346\216\247", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "\346\266\210\346\201\257\346\216\245\346\224\266\346\241\206", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
