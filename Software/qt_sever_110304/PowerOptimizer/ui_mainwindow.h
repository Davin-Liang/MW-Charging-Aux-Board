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
#include <QtWidgets/QListWidget>
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
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *lineEdit_local_ip;
    QLabel *label_4;
    QLineEdit *lineEdit_local_port;
    QPushButton *pushButton_connect;
    QPushButton *pushButton_disconnect;
    QLabel *label_5;
    QLabel *label_status;
    QTabWidget *tabWidget;
    QWidget *tab_3;
    QGroupBox *groupBox_2;
    QPushButton *pushButton_find_optimal;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_11;
    QLineEdit *lineEdit_square_step_2;
    QLabel *label_8;
    QLineEdit *lineEdit_square_step;
    QLabel *label_7;
    QLineEdit *lineEdit_circle_radius;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_16;
    QComboBox *comboBox_traj_type;
    QLabel *label_10;
    QLineEdit *lineEdit_max_voltage;
    QLabel *label_9;
    QLineEdit *lineEdit_voltage_step;
    QLabel *label_26;
    QLineEdit *lineEdit_initial_voltage;
    QGroupBox *groupBox;
    QPushButton *pushButton_motor_control;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_13;
    QLineEdit *lineEdit_motor_x;
    QLabel *label_14;
    QLineEdit *lineEdit_motor_y;
    QLabel *label_15;
    QLineEdit *lineEdit_motor_speed;
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
    QGroupBox *groupBox_7;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_18;
    QLineEdit *lineEdit_read_currentchannel;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_19;
    QLineEdit *lineEdit_read_currentvoltage;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_20;
    QLineEdit *lineEdit_read_currentpower;
    QWidget *tab_file_read;
    QWidget *layoutWidget2;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButton_filenamedisplay;
    QPushButton *pushButton_read_file;
    QListWidget *listWidget_files;
    QLabel *label_selected_file;
    QTextEdit *textEdit_file_content;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(656, 546);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        groupBox_connection = new QGroupBox(centralwidget);
        groupBox_connection->setObjectName(QString::fromUtf8("groupBox_connection"));
        groupBox_connection->setGeometry(QRect(10, 20, 621, 81));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font1.setPointSize(12);
        groupBox_connection->setFont(font1);
        groupBox_connection->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        horizontalLayout = new QHBoxLayout(groupBox_connection);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(groupBox_connection);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font1);

        horizontalLayout->addWidget(label_2);

        lineEdit_local_ip = new QLineEdit(groupBox_connection);
        lineEdit_local_ip->setObjectName(QString::fromUtf8("lineEdit_local_ip"));

        horizontalLayout->addWidget(lineEdit_local_ip);

        label_4 = new QLabel(groupBox_connection);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font1);

        horizontalLayout->addWidget(label_4);

        lineEdit_local_port = new QLineEdit(groupBox_connection);
        lineEdit_local_port->setObjectName(QString::fromUtf8("lineEdit_local_port"));

        horizontalLayout->addWidget(lineEdit_local_port);

        pushButton_connect = new QPushButton(groupBox_connection);
        pushButton_connect->setObjectName(QString::fromUtf8("pushButton_connect"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font2.setPointSize(14);
        pushButton_connect->setFont(font2);

        horizontalLayout->addWidget(pushButton_connect);

        pushButton_disconnect = new QPushButton(groupBox_connection);
        pushButton_disconnect->setObjectName(QString::fromUtf8("pushButton_disconnect"));
        pushButton_disconnect->setEnabled(false);
        pushButton_disconnect->setFont(font2);

        horizontalLayout->addWidget(pushButton_disconnect);

        label_5 = new QLabel(groupBox_connection);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font1);

        horizontalLayout->addWidget(label_5);

        label_status = new QLabel(groupBox_connection);
        label_status->setObjectName(QString::fromUtf8("label_status"));
        label_status->setFont(font1);
        label_status->setTextFormat(Qt::TextFormat::PlainText);

        horizontalLayout->addWidget(label_status);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 100, 631, 441));
        tabWidget->setFont(font1);
        tabWidget->setIconSize(QSize(20, 20));
        tabWidget->setDocumentMode(false);
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        groupBox_2 = new QGroupBox(tab_3);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 70, 601, 111));
        groupBox_2->setFont(font1);
        pushButton_find_optimal = new QPushButton(groupBox_2);
        pushButton_find_optimal->setObjectName(QString::fromUtf8("pushButton_find_optimal"));
        pushButton_find_optimal->setGeometry(QRect(470, 60, 121, 31));
        pushButton_find_optimal->setFont(font1);
        layoutWidget = new QWidget(groupBox_2);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 60, 441, 31));
        horizontalLayout_6 = new QHBoxLayout(layoutWidget);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(layoutWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_6->addWidget(label_11);

        lineEdit_square_step_2 = new QLineEdit(layoutWidget);
        lineEdit_square_step_2->setObjectName(QString::fromUtf8("lineEdit_square_step_2"));

        horizontalLayout_6->addWidget(lineEdit_square_step_2);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_6->addWidget(label_8);

        lineEdit_square_step = new QLineEdit(layoutWidget);
        lineEdit_square_step->setObjectName(QString::fromUtf8("lineEdit_square_step"));

        horizontalLayout_6->addWidget(lineEdit_square_step);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_6->addWidget(label_7);

        lineEdit_circle_radius = new QLineEdit(layoutWidget);
        lineEdit_circle_radius->setObjectName(QString::fromUtf8("lineEdit_circle_radius"));

        horizontalLayout_6->addWidget(lineEdit_circle_radius);

        widget = new QWidget(groupBox_2);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 20, 581, 31));
        horizontalLayout_5 = new QHBoxLayout(widget);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        label_16 = new QLabel(widget);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        horizontalLayout_5->addWidget(label_16);

        comboBox_traj_type = new QComboBox(widget);
        comboBox_traj_type->setObjectName(QString::fromUtf8("comboBox_traj_type"));

        horizontalLayout_5->addWidget(comboBox_traj_type);

        label_10 = new QLabel(widget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_5->addWidget(label_10);

        lineEdit_max_voltage = new QLineEdit(widget);
        lineEdit_max_voltage->setObjectName(QString::fromUtf8("lineEdit_max_voltage"));

        horizontalLayout_5->addWidget(lineEdit_max_voltage);

        label_9 = new QLabel(widget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_5->addWidget(label_9);

        lineEdit_voltage_step = new QLineEdit(widget);
        lineEdit_voltage_step->setObjectName(QString::fromUtf8("lineEdit_voltage_step"));

        horizontalLayout_5->addWidget(lineEdit_voltage_step);

        label_26 = new QLabel(widget);
        label_26->setObjectName(QString::fromUtf8("label_26"));

        horizontalLayout_5->addWidget(label_26);

        lineEdit_initial_voltage = new QLineEdit(widget);
        lineEdit_initial_voltage->setObjectName(QString::fromUtf8("lineEdit_initial_voltage"));

        horizontalLayout_5->addWidget(lineEdit_initial_voltage);

        groupBox = new QGroupBox(tab_3);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 601, 61));
        groupBox->setFont(font1);
        pushButton_motor_control = new QPushButton(groupBox);
        pushButton_motor_control->setObjectName(QString::fromUtf8("pushButton_motor_control"));
        pushButton_motor_control->setGeometry(QRect(470, 20, 121, 31));
        pushButton_motor_control->setFont(font1);
        layoutWidget1 = new QWidget(groupBox);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(10, 20, 451, 31));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_13 = new QLabel(layoutWidget1);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_4->addWidget(label_13);

        lineEdit_motor_x = new QLineEdit(layoutWidget1);
        lineEdit_motor_x->setObjectName(QString::fromUtf8("lineEdit_motor_x"));

        horizontalLayout_4->addWidget(lineEdit_motor_x);

        label_14 = new QLabel(layoutWidget1);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_4->addWidget(label_14);

        lineEdit_motor_y = new QLineEdit(layoutWidget1);
        lineEdit_motor_y->setObjectName(QString::fromUtf8("lineEdit_motor_y"));

        horizontalLayout_4->addWidget(lineEdit_motor_y);

        label_15 = new QLabel(layoutWidget1);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        horizontalLayout_4->addWidget(label_15);

        lineEdit_motor_speed = new QLineEdit(layoutWidget1);
        lineEdit_motor_speed->setObjectName(QString::fromUtf8("lineEdit_motor_speed"));

        horizontalLayout_4->addWidget(lineEdit_motor_speed);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        groupBox_5 = new QGroupBox(tab_4);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(10, 10, 171, 131));
        groupBox_5->setFont(font1);
        verticalLayout_7 = new QVBoxLayout(groupBox_5);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        label_6 = new QLabel(groupBox_5);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFont(font1);

        horizontalLayout_12->addWidget(label_6);

        lineEdit_read_motor_x = new QLineEdit(groupBox_5);
        lineEdit_read_motor_x->setObjectName(QString::fromUtf8("lineEdit_read_motor_x"));

        horizontalLayout_12->addWidget(lineEdit_read_motor_x);


        verticalLayout_6->addLayout(horizontalLayout_12);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setFont(font1);

        horizontalLayout_13->addWidget(label_12);

        lineEdit_read_motor_y = new QLineEdit(groupBox_5);
        lineEdit_read_motor_y->setObjectName(QString::fromUtf8("lineEdit_read_motor_y"));

        horizontalLayout_13->addWidget(lineEdit_read_motor_y);


        verticalLayout_6->addLayout(horizontalLayout_13);

        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName(QString::fromUtf8("horizontalLayout_19"));
        label_17 = new QLabel(groupBox_5);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        QFont font3;
        font3.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font3.setPointSize(12);
        font3.setBold(false);
        label_17->setFont(font3);

        horizontalLayout_19->addWidget(label_17);

        lineEdit_read_motor_speed = new QLineEdit(groupBox_5);
        lineEdit_read_motor_speed->setObjectName(QString::fromUtf8("lineEdit_read_motor_speed"));

        horizontalLayout_19->addWidget(lineEdit_read_motor_speed);


        verticalLayout_6->addLayout(horizontalLayout_19);


        verticalLayout_7->addLayout(verticalLayout_6);

        groupBox_chart = new QGroupBox(tab_4);
        groupBox_chart->setObjectName(QString::fromUtf8("groupBox_chart"));
        groupBox_chart->setGeometry(QRect(180, 10, 431, 281));
        groupBox_chart->setFont(font1);
        verticalLayout_9 = new QVBoxLayout(groupBox_chart);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        widget_chart = new QWidget(groupBox_chart);
        widget_chart->setObjectName(QString::fromUtf8("widget_chart"));

        verticalLayout_9->addWidget(widget_chart);

        groupBox_6 = new QGroupBox(tab_4);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 290, 601, 111));
        groupBox_6->setFont(font1);
        verticalLayout_3 = new QVBoxLayout(groupBox_6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        label_21 = new QLabel(groupBox_6);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setFont(font1);

        horizontalLayout_15->addWidget(label_21);

        lineEdit_read_channel1voltagemax = new QLineEdit(groupBox_6);
        lineEdit_read_channel1voltagemax->setObjectName(QString::fromUtf8("lineEdit_read_channel1voltagemax"));

        horizontalLayout_15->addWidget(lineEdit_read_channel1voltagemax);

        label_22 = new QLabel(groupBox_6);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setFont(font1);

        horizontalLayout_15->addWidget(label_22);

        lineEdit_read_channel2voltagemax = new QLineEdit(groupBox_6);
        lineEdit_read_channel2voltagemax->setObjectName(QString::fromUtf8("lineEdit_read_channel2voltagemax"));

        horizontalLayout_15->addWidget(lineEdit_read_channel2voltagemax);


        verticalLayout_3->addLayout(horizontalLayout_15);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        label_23 = new QLabel(groupBox_6);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setFont(font1);

        horizontalLayout_16->addWidget(label_23);

        lineEdit_read_channel3voltagemax = new QLineEdit(groupBox_6);
        lineEdit_read_channel3voltagemax->setObjectName(QString::fromUtf8("lineEdit_read_channel3voltagemax"));

        horizontalLayout_16->addWidget(lineEdit_read_channel3voltagemax);

        label_24 = new QLabel(groupBox_6);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        label_24->setFont(font1);

        horizontalLayout_16->addWidget(label_24);

        lineEdit_read_channel4voltagemax = new QLineEdit(groupBox_6);
        lineEdit_read_channel4voltagemax->setObjectName(QString::fromUtf8("lineEdit_read_channel4voltagemax"));

        horizontalLayout_16->addWidget(lineEdit_read_channel4voltagemax);


        verticalLayout_3->addLayout(horizontalLayout_16);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        label_25 = new QLabel(groupBox_6);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setFont(font1);

        horizontalLayout_17->addWidget(label_25);

        lineEdit_read_powermax = new QLineEdit(groupBox_6);
        lineEdit_read_powermax->setObjectName(QString::fromUtf8("lineEdit_read_powermax"));

        horizontalLayout_17->addWidget(lineEdit_read_powermax);


        verticalLayout_3->addLayout(horizontalLayout_17);

        groupBox_7 = new QGroupBox(tab_4);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setGeometry(QRect(10, 140, 171, 151));
        groupBox_7->setFont(font1);
        verticalLayout_2 = new QVBoxLayout(groupBox_7);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_18 = new QLabel(groupBox_7);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setFont(font1);

        horizontalLayout_2->addWidget(label_18);

        lineEdit_read_currentchannel = new QLineEdit(groupBox_7);
        lineEdit_read_currentchannel->setObjectName(QString::fromUtf8("lineEdit_read_currentchannel"));

        horizontalLayout_2->addWidget(lineEdit_read_currentchannel);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_19 = new QLabel(groupBox_7);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setFont(font1);

        horizontalLayout_3->addWidget(label_19);

        lineEdit_read_currentvoltage = new QLineEdit(groupBox_7);
        lineEdit_read_currentvoltage->setObjectName(QString::fromUtf8("lineEdit_read_currentvoltage"));

        horizontalLayout_3->addWidget(lineEdit_read_currentvoltage);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        label_20 = new QLabel(groupBox_7);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setFont(font1);

        horizontalLayout_14->addWidget(label_20);

        lineEdit_read_currentpower = new QLineEdit(groupBox_7);
        lineEdit_read_currentpower->setObjectName(QString::fromUtf8("lineEdit_read_currentpower"));

        horizontalLayout_14->addWidget(lineEdit_read_currentpower);


        verticalLayout->addLayout(horizontalLayout_14);


        verticalLayout_2->addLayout(verticalLayout);

        tabWidget->addTab(tab_4, QString());
        tab_file_read = new QWidget();
        tab_file_read->setObjectName(QString::fromUtf8("tab_file_read"));
        layoutWidget2 = new QWidget(tab_file_read);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(20, 10, 581, 391));
        verticalLayout_4 = new QVBoxLayout(layoutWidget2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        pushButton_filenamedisplay = new QPushButton(layoutWidget2);
        pushButton_filenamedisplay->setObjectName(QString::fromUtf8("pushButton_filenamedisplay"));
        pushButton_filenamedisplay->setFont(font3);

        horizontalLayout_7->addWidget(pushButton_filenamedisplay);

        pushButton_read_file = new QPushButton(layoutWidget2);
        pushButton_read_file->setObjectName(QString::fromUtf8("pushButton_read_file"));

        horizontalLayout_7->addWidget(pushButton_read_file);


        verticalLayout_4->addLayout(horizontalLayout_7);

        listWidget_files = new QListWidget(layoutWidget2);
        listWidget_files->setObjectName(QString::fromUtf8("listWidget_files"));

        verticalLayout_4->addWidget(listWidget_files);

        label_selected_file = new QLabel(layoutWidget2);
        label_selected_file->setObjectName(QString::fromUtf8("label_selected_file"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_selected_file->sizePolicy().hasHeightForWidth());
        label_selected_file->setSizePolicy(sizePolicy);

        verticalLayout_4->addWidget(label_selected_file);

        textEdit_file_content = new QTextEdit(layoutWidget2);
        textEdit_file_content->setObjectName(QString::fromUtf8("textEdit_file_content"));
        textEdit_file_content->setReadOnly(true);
        textEdit_file_content->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse);

        verticalLayout_4->addWidget(textEdit_file_content);

        tabWidget->addTab(tab_file_read, QString());
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox_connection->setTitle(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\350\256\276\347\275\256", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241IP", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250\347\253\257\345\217\243", nullptr));
        pushButton_connect->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\350\256\276\345\244\207", nullptr));
        pushButton_disconnect->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        label_status->setText(QCoreApplication::translate("MainWindow", "\346\234\252\350\277\236\346\216\245", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\345\212\237\347\216\207\345\257\273\344\274\230\346\214\207\344\273\244\350\256\276\347\275\256", nullptr));
        pushButton_find_optimal->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\345\257\273\344\274\230\346\214\207\344\273\244", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\346\226\271\345\275\242\350\276\271\351\225\277", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\346\226\271\345\275\242\346\255\245\351\225\277", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\345\234\206\345\275\242\345\215\212\345\276\204", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "\350\275\250\350\277\271\350\256\276\345\256\232", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\346\234\200\345\244\247\347\224\265\345\216\213", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\347\224\265\345\216\213\346\255\245\351\225\277", nullptr));
        label_26->setText(QCoreApplication::translate("MainWindow", "\345\210\235\345\247\213\347\224\265\345\216\213", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\346\216\247\345\210\266\346\214\207\344\273\244\350\256\276\347\275\256", nullptr));
        pushButton_motor_control->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\347\224\265\346\234\272\346\214\207\344\273\244", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "X\345\235\220\346\240\207", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "Y\345\235\220\346\240\207", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("MainWindow", "\346\216\247\345\210\266\346\214\207\344\273\244\350\256\276\347\275\256", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\345\275\223\345\211\215\347\212\266\346\200\201\350\257\273\345\217\226", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "x\345\235\220\346\240\207", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "y\345\235\220\346\240\207", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246", nullptr));
        groupBox_chart->setTitle(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\344\275\215\347\275\256\350\275\250\350\277\271\345\233\276", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "\345\257\273\344\274\230\347\273\223\346\236\234\346\230\276\347\244\272", nullptr));
        label_21->setText(QCoreApplication::translate("MainWindow", "Channel1_Vmax", nullptr));
        label_22->setText(QCoreApplication::translate("MainWindow", "Channel2_Vmax", nullptr));
        label_23->setText(QCoreApplication::translate("MainWindow", "Channel3_Vmax", nullptr));
        label_24->setText(QCoreApplication::translate("MainWindow", "Channel4_Vmax", nullptr));
        label_25->setText(QCoreApplication::translate("MainWindow", "Pmax", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MainWindow", "\345\257\273\344\274\230\350\277\207\347\250\213\346\225\260\346\215\256\346\230\276\347\244\272", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\351\200\232\351\201\223", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\347\224\265\345\216\213", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\345\212\237\347\216\207", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\347\233\221\346\216\247", nullptr));
        pushButton_filenamedisplay->setText(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266\346\265\217\350\247\210", nullptr));
        pushButton_read_file->setText(QCoreApplication::translate("MainWindow", "\345\206\205\345\256\271\346\237\245\347\234\213", nullptr));
        label_selected_file->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\346\234\252\351\200\211\344\270\255", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_file_read), QCoreApplication::translate("MainWindow", "\345\255\230\345\202\250\346\226\207\344\273\266\350\257\273\345\217\226", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
