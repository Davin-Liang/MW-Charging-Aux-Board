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
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tab_device_connect;
    QGroupBox *group_box_stm32_connection;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *lineEdit_local_ip;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_4;
    QLineEdit *lineEdit_local_port;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *pushButton_connect;
    QPushButton *pushButton_disconnect;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_5;
    QLabel *label_status;
    QGroupBox *turntable_connection;
    QVBoxLayout *verticalLayout_13;
    QHBoxLayout *horizontalLayout_37;
    QHBoxLayout *horizontalLayout_29;
    QLabel *label_38;
    QLineEdit *lineEdit_baudrate;
    QHBoxLayout *horizontalLayout_30;
    QLabel *label_40;
    QLineEdit *lineEdit_parity;
    QHBoxLayout *horizontalLayout_35;
    QHBoxLayout *horizontalLayout_31;
    QLabel *label_42;
    QLineEdit *lineEdit_dataBit;
    QHBoxLayout *horizontalLayout_32;
    QLabel *label_43;
    QLineEdit *lineEdit_stopBit;
    QHBoxLayout *horizontalLayout_36;
    QLabel *label_44;
    QLineEdit *lineEdit_port;
    QHBoxLayout *horizontalLayout_33;
    QPushButton *pushButton_connection;
    QPushButton *pushButton_disconnection;
    QHBoxLayout *horizontalLayout_34;
    QLabel *label_37;
    QLabel *connection_status;
    QWidget *tab_stm32;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_13;
    QLineEdit *lineEdit_motor_x;
    QLabel *label_14;
    QLineEdit *lineEdit_motor_y;
    QLabel *label_15;
    QLineEdit *lineEdit_motor_speed;
    QPushButton *pushButton_motor_control;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_14;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_16;
    QComboBox *comboBox_traj_type;
    QLabel *label_10;
    QLineEdit *lineEdit_max_voltage;
    QLabel *label_9;
    QLineEdit *lineEdit_voltage_step;
    QLabel *label_26;
    QLineEdit *lineEdit_initial_voltage;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_11;
    QLineEdit *lineEdit_square_step_2;
    QLabel *label_8;
    QLineEdit *lineEdit_square_step;
    QLabel *label_7;
    QLineEdit *lineEdit_circle_radius;
    QPushButton *pushButton_find_optimal;
    QGroupBox *groupBox_7;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_18;
    QLineEdit *lineEdit_read_currentchannel;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_20;
    QLineEdit *lineEdit_read_currentpower;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_19;
    QLineEdit *lineEdit_read_currentvoltage;
    QGroupBox *groupBox_chart;
    QHBoxLayout *horizontalLayout_26;
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
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_6;
    QLineEdit *lineEdit_read_motor_x;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_12;
    QLineEdit *lineEdit_read_motor_y;
    QHBoxLayout *horizontalLayout_19;
    QLabel *label_17;
    QLineEdit *lineEdit_read_motor_speed;
    QWidget *tab_file_read;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButton_filenamedisplay;
    QPushButton *pushButton_read_file;
    QListWidget *listWidget_files;
    QLabel *label_selected_file;
    QTableWidget *tableWidget_file_content;
    QWidget *tab_turntable_control;
    QGroupBox *group_box_turntable_shape;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_turntable_img;
    QGroupBox *open_group_box_turntable_control;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_36;
    QLineEdit *line_edit_x_speed_cmd;
    QComboBox *combo_box_x_speed_cmd;
    QHBoxLayout *horizontalLayout_18;
    QLabel *label_35;
    QLineEdit *line_edit_y_speed_cmd;
    QComboBox *combo_box_y_speed_cmd;
    QHBoxLayout *horizontalLayout_27;
    QPushButton *btn_stop_x_turntable_run;
    QPushButton *btn_stop_y_turntable_run;
    QPushButton *btn_set_speed;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_10;
    QHBoxLayout *horizontalLayout_20;
    QLabel *label_33;
    QLineEdit *line_edit_x_pos;
    QPushButton *btn_set_x_pos;
    QHBoxLayout *horizontalLayout_21;
    QLabel *label_34;
    QLineEdit *line_edit_y_pos;
    QPushButton *btn_set_y_pos;
    QHBoxLayout *horizontalLayout_28;
    QPushButton *btn_x_zero;
    QPushButton *btn_y_zero;
    QGroupBox *groupBox_8;
    QHBoxLayout *horizontalLayout_22;
    QLabel *label;
    QLineEdit *line_edit_monitor_x_pos;
    QLabel *label_27;
    QLineEdit *line_edit_monitor_x_speed;
    QLabel *label_3;
    QLineEdit *line_edit_monitor_y_pos;
    QLabel *label_28;
    QLineEdit *line_edit_monitor_y_speed;
    QGroupBox *groupBox_9;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_25;
    QLabel *label_29;
    QLineEdit *line_edit_x_pos_ref;
    QLabel *label_31;
    QComboBox *combo_box_x_track_ref;
    QLabel *label_30;
    QLineEdit *line_edit_y_pos_ref;
    QLabel *label_32;
    QComboBox *combo_box_y_track_ref;
    QHBoxLayout *horizontalLayout_24;
    QPushButton *btn_set_target_pos;
    QLabel *label_45;
    QComboBox *controller_selection;
    QLabel *control_status;
    QGroupBox *groupBox_10;
    QHBoxLayout *horizontalLayout_23;
    QLabel *label_46;
    QLineEdit *line_edit_kp_parameter;
    QLabel *label_47;
    QLineEdit *line_edit_ki_parameter;
    QLabel *label_48;
    QLineEdit *line_edit_kd_parameter;
    QPushButton *btn_set_pidcontroller_parameter;
    QPushButton *btn_stop_pidcontrol;
    QGroupBox *groupBox_chart_2;
    QHBoxLayout *horizontalLayout_38;
    QWidget *turntable_position_chart;
    QLabel *label_39;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1011, 770);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 40, 991, 721));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font1.setPointSize(12);
        tabWidget->setFont(font1);
        tabWidget->setIconSize(QSize(20, 20));
        tabWidget->setDocumentMode(false);
        tab_device_connect = new QWidget();
        tab_device_connect->setObjectName(QString::fromUtf8("tab_device_connect"));
        group_box_stm32_connection = new QGroupBox(tab_device_connect);
        group_box_stm32_connection->setObjectName(QString::fromUtf8("group_box_stm32_connection"));
        group_box_stm32_connection->setGeometry(QRect(10, 10, 451, 181));
        group_box_stm32_connection->setFont(font1);
        group_box_stm32_connection->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        verticalLayout_5 = new QVBoxLayout(group_box_stm32_connection);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(group_box_stm32_connection);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font1);

        horizontalLayout->addWidget(label_2);

        lineEdit_local_ip = new QLineEdit(group_box_stm32_connection);
        lineEdit_local_ip->setObjectName(QString::fromUtf8("lineEdit_local_ip"));

        horizontalLayout->addWidget(lineEdit_local_ip);


        verticalLayout_5->addLayout(horizontalLayout);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_4 = new QLabel(group_box_stm32_connection);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font1);

        horizontalLayout_8->addWidget(label_4);

        lineEdit_local_port = new QLineEdit(group_box_stm32_connection);
        lineEdit_local_port->setObjectName(QString::fromUtf8("lineEdit_local_port"));

        horizontalLayout_8->addWidget(lineEdit_local_port);


        verticalLayout_5->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        pushButton_connect = new QPushButton(group_box_stm32_connection);
        pushButton_connect->setObjectName(QString::fromUtf8("pushButton_connect"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font2.setPointSize(14);
        pushButton_connect->setFont(font2);

        horizontalLayout_9->addWidget(pushButton_connect);

        pushButton_disconnect = new QPushButton(group_box_stm32_connection);
        pushButton_disconnect->setObjectName(QString::fromUtf8("pushButton_disconnect"));
        pushButton_disconnect->setEnabled(false);
        pushButton_disconnect->setFont(font2);

        horizontalLayout_9->addWidget(pushButton_disconnect);


        verticalLayout_5->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        label_5 = new QLabel(group_box_stm32_connection);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font1);

        horizontalLayout_10->addWidget(label_5);

        label_status = new QLabel(group_box_stm32_connection);
        label_status->setObjectName(QString::fromUtf8("label_status"));
        label_status->setFont(font1);
        label_status->setTextFormat(Qt::TextFormat::PlainText);

        horizontalLayout_10->addWidget(label_status);


        verticalLayout_5->addLayout(horizontalLayout_10);

        turntable_connection = new QGroupBox(tab_device_connect);
        turntable_connection->setObjectName(QString::fromUtf8("turntable_connection"));
        turntable_connection->setGeometry(QRect(460, 10, 521, 181));
        turntable_connection->setFont(font1);
        turntable_connection->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        verticalLayout_13 = new QVBoxLayout(turntable_connection);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        horizontalLayout_37 = new QHBoxLayout();
        horizontalLayout_37->setObjectName(QString::fromUtf8("horizontalLayout_37"));
        horizontalLayout_29 = new QHBoxLayout();
        horizontalLayout_29->setObjectName(QString::fromUtf8("horizontalLayout_29"));
        label_38 = new QLabel(turntable_connection);
        label_38->setObjectName(QString::fromUtf8("label_38"));
        label_38->setFont(font1);

        horizontalLayout_29->addWidget(label_38);

        lineEdit_baudrate = new QLineEdit(turntable_connection);
        lineEdit_baudrate->setObjectName(QString::fromUtf8("lineEdit_baudrate"));

        horizontalLayout_29->addWidget(lineEdit_baudrate);


        horizontalLayout_37->addLayout(horizontalLayout_29);

        horizontalLayout_30 = new QHBoxLayout();
        horizontalLayout_30->setObjectName(QString::fromUtf8("horizontalLayout_30"));
        label_40 = new QLabel(turntable_connection);
        label_40->setObjectName(QString::fromUtf8("label_40"));
        label_40->setFont(font1);

        horizontalLayout_30->addWidget(label_40);

        lineEdit_parity = new QLineEdit(turntable_connection);
        lineEdit_parity->setObjectName(QString::fromUtf8("lineEdit_parity"));

        horizontalLayout_30->addWidget(lineEdit_parity);


        horizontalLayout_37->addLayout(horizontalLayout_30);


        verticalLayout_13->addLayout(horizontalLayout_37);

        horizontalLayout_35 = new QHBoxLayout();
        horizontalLayout_35->setObjectName(QString::fromUtf8("horizontalLayout_35"));
        horizontalLayout_31 = new QHBoxLayout();
        horizontalLayout_31->setObjectName(QString::fromUtf8("horizontalLayout_31"));
        label_42 = new QLabel(turntable_connection);
        label_42->setObjectName(QString::fromUtf8("label_42"));
        label_42->setFont(font1);

        horizontalLayout_31->addWidget(label_42);

        lineEdit_dataBit = new QLineEdit(turntable_connection);
        lineEdit_dataBit->setObjectName(QString::fromUtf8("lineEdit_dataBit"));

        horizontalLayout_31->addWidget(lineEdit_dataBit);


        horizontalLayout_35->addLayout(horizontalLayout_31);

        horizontalLayout_32 = new QHBoxLayout();
        horizontalLayout_32->setObjectName(QString::fromUtf8("horizontalLayout_32"));
        label_43 = new QLabel(turntable_connection);
        label_43->setObjectName(QString::fromUtf8("label_43"));
        label_43->setFont(font1);

        horizontalLayout_32->addWidget(label_43);

        lineEdit_stopBit = new QLineEdit(turntable_connection);
        lineEdit_stopBit->setObjectName(QString::fromUtf8("lineEdit_stopBit"));

        horizontalLayout_32->addWidget(lineEdit_stopBit);


        horizontalLayout_35->addLayout(horizontalLayout_32);


        verticalLayout_13->addLayout(horizontalLayout_35);

        horizontalLayout_36 = new QHBoxLayout();
        horizontalLayout_36->setObjectName(QString::fromUtf8("horizontalLayout_36"));
        label_44 = new QLabel(turntable_connection);
        label_44->setObjectName(QString::fromUtf8("label_44"));
        label_44->setFont(font1);

        horizontalLayout_36->addWidget(label_44);

        lineEdit_port = new QLineEdit(turntable_connection);
        lineEdit_port->setObjectName(QString::fromUtf8("lineEdit_port"));

        horizontalLayout_36->addWidget(lineEdit_port);


        verticalLayout_13->addLayout(horizontalLayout_36);

        horizontalLayout_33 = new QHBoxLayout();
        horizontalLayout_33->setObjectName(QString::fromUtf8("horizontalLayout_33"));
        pushButton_connection = new QPushButton(turntable_connection);
        pushButton_connection->setObjectName(QString::fromUtf8("pushButton_connection"));

        horizontalLayout_33->addWidget(pushButton_connection);

        pushButton_disconnection = new QPushButton(turntable_connection);
        pushButton_disconnection->setObjectName(QString::fromUtf8("pushButton_disconnection"));

        horizontalLayout_33->addWidget(pushButton_disconnection);


        verticalLayout_13->addLayout(horizontalLayout_33);

        horizontalLayout_34 = new QHBoxLayout();
        horizontalLayout_34->setObjectName(QString::fromUtf8("horizontalLayout_34"));
        label_37 = new QLabel(turntable_connection);
        label_37->setObjectName(QString::fromUtf8("label_37"));
        label_37->setFont(font1);

        horizontalLayout_34->addWidget(label_37);

        connection_status = new QLabel(turntable_connection);
        connection_status->setObjectName(QString::fromUtf8("connection_status"));
        connection_status->setFont(font1);
        connection_status->setTextFormat(Qt::TextFormat::PlainText);

        horizontalLayout_34->addWidget(connection_status);


        verticalLayout_13->addLayout(horizontalLayout_34);

        tabWidget->addTab(tab_device_connect, QString());
        tab_stm32 = new QWidget();
        tab_stm32->setObjectName(QString::fromUtf8("tab_stm32"));
        groupBox = new QGroupBox(tab_stm32);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 981, 81));
        groupBox->setFont(font1);
        horizontalLayout_4 = new QHBoxLayout(groupBox);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_13 = new QLabel(groupBox);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_4->addWidget(label_13);

        lineEdit_motor_x = new QLineEdit(groupBox);
        lineEdit_motor_x->setObjectName(QString::fromUtf8("lineEdit_motor_x"));

        horizontalLayout_4->addWidget(lineEdit_motor_x);

        label_14 = new QLabel(groupBox);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_4->addWidget(label_14);

        lineEdit_motor_y = new QLineEdit(groupBox);
        lineEdit_motor_y->setObjectName(QString::fromUtf8("lineEdit_motor_y"));

        horizontalLayout_4->addWidget(lineEdit_motor_y);

        label_15 = new QLabel(groupBox);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        horizontalLayout_4->addWidget(label_15);

        lineEdit_motor_speed = new QLineEdit(groupBox);
        lineEdit_motor_speed->setObjectName(QString::fromUtf8("lineEdit_motor_speed"));

        horizontalLayout_4->addWidget(lineEdit_motor_speed);

        pushButton_motor_control = new QPushButton(groupBox);
        pushButton_motor_control->setObjectName(QString::fromUtf8("pushButton_motor_control"));
        pushButton_motor_control->setFont(font1);

        horizontalLayout_4->addWidget(pushButton_motor_control);

        groupBox_2 = new QGroupBox(tab_stm32);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(0, 80, 981, 121));
        groupBox_2->setFont(font1);
        verticalLayout_14 = new QVBoxLayout(groupBox_2);
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_16 = new QLabel(groupBox_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        horizontalLayout_5->addWidget(label_16);

        comboBox_traj_type = new QComboBox(groupBox_2);
        comboBox_traj_type->setObjectName(QString::fromUtf8("comboBox_traj_type"));

        horizontalLayout_5->addWidget(comboBox_traj_type);

        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_5->addWidget(label_10);

        lineEdit_max_voltage = new QLineEdit(groupBox_2);
        lineEdit_max_voltage->setObjectName(QString::fromUtf8("lineEdit_max_voltage"));

        horizontalLayout_5->addWidget(lineEdit_max_voltage);

        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_5->addWidget(label_9);

        lineEdit_voltage_step = new QLineEdit(groupBox_2);
        lineEdit_voltage_step->setObjectName(QString::fromUtf8("lineEdit_voltage_step"));

        horizontalLayout_5->addWidget(lineEdit_voltage_step);

        label_26 = new QLabel(groupBox_2);
        label_26->setObjectName(QString::fromUtf8("label_26"));

        horizontalLayout_5->addWidget(label_26);

        lineEdit_initial_voltage = new QLineEdit(groupBox_2);
        lineEdit_initial_voltage->setObjectName(QString::fromUtf8("lineEdit_initial_voltage"));

        horizontalLayout_5->addWidget(lineEdit_initial_voltage);


        verticalLayout_14->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_6->addWidget(label_11);

        lineEdit_square_step_2 = new QLineEdit(groupBox_2);
        lineEdit_square_step_2->setObjectName(QString::fromUtf8("lineEdit_square_step_2"));

        horizontalLayout_6->addWidget(lineEdit_square_step_2);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_6->addWidget(label_8);

        lineEdit_square_step = new QLineEdit(groupBox_2);
        lineEdit_square_step->setObjectName(QString::fromUtf8("lineEdit_square_step"));

        horizontalLayout_6->addWidget(lineEdit_square_step);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_6->addWidget(label_7);

        lineEdit_circle_radius = new QLineEdit(groupBox_2);
        lineEdit_circle_radius->setObjectName(QString::fromUtf8("lineEdit_circle_radius"));

        horizontalLayout_6->addWidget(lineEdit_circle_radius);

        pushButton_find_optimal = new QPushButton(groupBox_2);
        pushButton_find_optimal->setObjectName(QString::fromUtf8("pushButton_find_optimal"));
        pushButton_find_optimal->setFont(font1);

        horizontalLayout_6->addWidget(pushButton_find_optimal);


        verticalLayout_14->addLayout(horizontalLayout_6);

        groupBox_7 = new QGroupBox(tab_stm32);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setGeometry(QRect(0, 380, 341, 181));
        groupBox_7->setFont(font1);
        verticalLayout_9 = new QVBoxLayout(groupBox_7);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_18 = new QLabel(groupBox_7);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setFont(font1);

        horizontalLayout_2->addWidget(label_18);

        lineEdit_read_currentchannel = new QLineEdit(groupBox_7);
        lineEdit_read_currentchannel->setObjectName(QString::fromUtf8("lineEdit_read_currentchannel"));

        horizontalLayout_2->addWidget(lineEdit_read_currentchannel);


        verticalLayout_9->addLayout(horizontalLayout_2);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        label_20 = new QLabel(groupBox_7);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setFont(font1);

        horizontalLayout_14->addWidget(label_20);

        lineEdit_read_currentpower = new QLineEdit(groupBox_7);
        lineEdit_read_currentpower->setObjectName(QString::fromUtf8("lineEdit_read_currentpower"));

        horizontalLayout_14->addWidget(lineEdit_read_currentpower);


        verticalLayout_9->addLayout(horizontalLayout_14);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_19 = new QLabel(groupBox_7);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setFont(font1);

        horizontalLayout_3->addWidget(label_19);

        lineEdit_read_currentvoltage = new QLineEdit(groupBox_7);
        lineEdit_read_currentvoltage->setObjectName(QString::fromUtf8("lineEdit_read_currentvoltage"));

        horizontalLayout_3->addWidget(lineEdit_read_currentvoltage);


        verticalLayout_9->addLayout(horizontalLayout_3);

        groupBox_chart = new QGroupBox(tab_stm32);
        groupBox_chart->setObjectName(QString::fromUtf8("groupBox_chart"));
        groupBox_chart->setGeometry(QRect(340, 210, 641, 341));
        groupBox_chart->setFont(font1);
        horizontalLayout_26 = new QHBoxLayout(groupBox_chart);
        horizontalLayout_26->setObjectName(QString::fromUtf8("horizontalLayout_26"));
        widget_chart = new QWidget(groupBox_chart);
        widget_chart->setObjectName(QString::fromUtf8("widget_chart"));

        horizontalLayout_26->addWidget(widget_chart);

        groupBox_6 = new QGroupBox(tab_stm32);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setGeometry(QRect(0, 560, 981, 131));
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

        groupBox_5 = new QGroupBox(tab_stm32);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(0, 210, 341, 171));
        groupBox_5->setFont(font1);
        verticalLayout = new QVBoxLayout(groupBox_5);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        label_6 = new QLabel(groupBox_5);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFont(font1);

        horizontalLayout_12->addWidget(label_6);

        lineEdit_read_motor_x = new QLineEdit(groupBox_5);
        lineEdit_read_motor_x->setObjectName(QString::fromUtf8("lineEdit_read_motor_x"));

        horizontalLayout_12->addWidget(lineEdit_read_motor_x);


        verticalLayout->addLayout(horizontalLayout_12);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setFont(font1);

        horizontalLayout_13->addWidget(label_12);

        lineEdit_read_motor_y = new QLineEdit(groupBox_5);
        lineEdit_read_motor_y->setObjectName(QString::fromUtf8("lineEdit_read_motor_y"));

        horizontalLayout_13->addWidget(lineEdit_read_motor_y);


        verticalLayout->addLayout(horizontalLayout_13);

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


        verticalLayout->addLayout(horizontalLayout_19);

        tabWidget->addTab(tab_stm32, QString());
        tab_file_read = new QWidget();
        tab_file_read->setObjectName(QString::fromUtf8("tab_file_read"));
        layoutWidget = new QWidget(tab_file_read);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(22, 12, 941, 671));
        verticalLayout_4 = new QVBoxLayout(layoutWidget);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        pushButton_filenamedisplay = new QPushButton(layoutWidget);
        pushButton_filenamedisplay->setObjectName(QString::fromUtf8("pushButton_filenamedisplay"));
        pushButton_filenamedisplay->setFont(font3);

        horizontalLayout_7->addWidget(pushButton_filenamedisplay);

        pushButton_read_file = new QPushButton(layoutWidget);
        pushButton_read_file->setObjectName(QString::fromUtf8("pushButton_read_file"));

        horizontalLayout_7->addWidget(pushButton_read_file);


        verticalLayout_4->addLayout(horizontalLayout_7);

        listWidget_files = new QListWidget(layoutWidget);
        listWidget_files->setObjectName(QString::fromUtf8("listWidget_files"));

        verticalLayout_4->addWidget(listWidget_files);

        label_selected_file = new QLabel(layoutWidget);
        label_selected_file->setObjectName(QString::fromUtf8("label_selected_file"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_selected_file->sizePolicy().hasHeightForWidth());
        label_selected_file->setSizePolicy(sizePolicy);

        verticalLayout_4->addWidget(label_selected_file);

        tableWidget_file_content = new QTableWidget(layoutWidget);
        tableWidget_file_content->setObjectName(QString::fromUtf8("tableWidget_file_content"));
        tableWidget_file_content->setAlternatingRowColors(true);
        tableWidget_file_content->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        tableWidget_file_content->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        verticalLayout_4->addWidget(tableWidget_file_content);

        tabWidget->addTab(tab_file_read, QString());
        tab_turntable_control = new QWidget();
        tab_turntable_control->setObjectName(QString::fromUtf8("tab_turntable_control"));
        group_box_turntable_shape = new QGroupBox(tab_turntable_control);
        group_box_turntable_shape->setObjectName(QString::fromUtf8("group_box_turntable_shape"));
        group_box_turntable_shape->setGeometry(QRect(1, 1, 291, 191));
        verticalLayout_7 = new QVBoxLayout(group_box_turntable_shape);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        label_turntable_img = new QLabel(group_box_turntable_shape);
        label_turntable_img->setObjectName(QString::fromUtf8("label_turntable_img"));
        label_turntable_img->setPixmap(QPixmap(QString::fromUtf8(":/i18n/PowerOptimizer_zh_CN.qm")));

        verticalLayout_7->addWidget(label_turntable_img);

        open_group_box_turntable_control = new QGroupBox(tab_turntable_control);
        open_group_box_turntable_control->setObjectName(QString::fromUtf8("open_group_box_turntable_control"));
        open_group_box_turntable_control->setGeometry(QRect(0, 200, 311, 481));
        verticalLayout_2 = new QVBoxLayout(open_group_box_turntable_control);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox_3 = new QGroupBox(open_group_box_turntable_control);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setFont(font1);
        verticalLayout_6 = new QVBoxLayout(groupBox_3);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        label_36 = new QLabel(groupBox_3);
        label_36->setObjectName(QString::fromUtf8("label_36"));

        horizontalLayout_11->addWidget(label_36);

        line_edit_x_speed_cmd = new QLineEdit(groupBox_3);
        line_edit_x_speed_cmd->setObjectName(QString::fromUtf8("line_edit_x_speed_cmd"));

        horizontalLayout_11->addWidget(line_edit_x_speed_cmd);

        combo_box_x_speed_cmd = new QComboBox(groupBox_3);
        combo_box_x_speed_cmd->setObjectName(QString::fromUtf8("combo_box_x_speed_cmd"));

        horizontalLayout_11->addWidget(combo_box_x_speed_cmd);


        verticalLayout_6->addLayout(horizontalLayout_11);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        label_35 = new QLabel(groupBox_3);
        label_35->setObjectName(QString::fromUtf8("label_35"));

        horizontalLayout_18->addWidget(label_35);

        line_edit_y_speed_cmd = new QLineEdit(groupBox_3);
        line_edit_y_speed_cmd->setObjectName(QString::fromUtf8("line_edit_y_speed_cmd"));

        horizontalLayout_18->addWidget(line_edit_y_speed_cmd);

        combo_box_y_speed_cmd = new QComboBox(groupBox_3);
        combo_box_y_speed_cmd->setObjectName(QString::fromUtf8("combo_box_y_speed_cmd"));

        horizontalLayout_18->addWidget(combo_box_y_speed_cmd);


        verticalLayout_6->addLayout(horizontalLayout_18);

        horizontalLayout_27 = new QHBoxLayout();
        horizontalLayout_27->setObjectName(QString::fromUtf8("horizontalLayout_27"));
        btn_stop_x_turntable_run = new QPushButton(groupBox_3);
        btn_stop_x_turntable_run->setObjectName(QString::fromUtf8("btn_stop_x_turntable_run"));

        horizontalLayout_27->addWidget(btn_stop_x_turntable_run);

        btn_stop_y_turntable_run = new QPushButton(groupBox_3);
        btn_stop_y_turntable_run->setObjectName(QString::fromUtf8("btn_stop_y_turntable_run"));

        horizontalLayout_27->addWidget(btn_stop_y_turntable_run);


        verticalLayout_6->addLayout(horizontalLayout_27);

        btn_set_speed = new QPushButton(groupBox_3);
        btn_set_speed->setObjectName(QString::fromUtf8("btn_set_speed"));

        verticalLayout_6->addWidget(btn_set_speed);


        verticalLayout_2->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(open_group_box_turntable_control);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setFont(font1);
        verticalLayout_10 = new QVBoxLayout(groupBox_4);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setObjectName(QString::fromUtf8("horizontalLayout_20"));
        label_33 = new QLabel(groupBox_4);
        label_33->setObjectName(QString::fromUtf8("label_33"));
        label_33->setFont(font1);

        horizontalLayout_20->addWidget(label_33);

        line_edit_x_pos = new QLineEdit(groupBox_4);
        line_edit_x_pos->setObjectName(QString::fromUtf8("line_edit_x_pos"));

        horizontalLayout_20->addWidget(line_edit_x_pos);

        btn_set_x_pos = new QPushButton(groupBox_4);
        btn_set_x_pos->setObjectName(QString::fromUtf8("btn_set_x_pos"));

        horizontalLayout_20->addWidget(btn_set_x_pos);


        verticalLayout_10->addLayout(horizontalLayout_20);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setObjectName(QString::fromUtf8("horizontalLayout_21"));
        label_34 = new QLabel(groupBox_4);
        label_34->setObjectName(QString::fromUtf8("label_34"));

        horizontalLayout_21->addWidget(label_34);

        line_edit_y_pos = new QLineEdit(groupBox_4);
        line_edit_y_pos->setObjectName(QString::fromUtf8("line_edit_y_pos"));

        horizontalLayout_21->addWidget(line_edit_y_pos);

        btn_set_y_pos = new QPushButton(groupBox_4);
        btn_set_y_pos->setObjectName(QString::fromUtf8("btn_set_y_pos"));

        horizontalLayout_21->addWidget(btn_set_y_pos);


        verticalLayout_10->addLayout(horizontalLayout_21);

        horizontalLayout_28 = new QHBoxLayout();
        horizontalLayout_28->setObjectName(QString::fromUtf8("horizontalLayout_28"));
        btn_x_zero = new QPushButton(groupBox_4);
        btn_x_zero->setObjectName(QString::fromUtf8("btn_x_zero"));

        horizontalLayout_28->addWidget(btn_x_zero);

        btn_y_zero = new QPushButton(groupBox_4);
        btn_y_zero->setObjectName(QString::fromUtf8("btn_y_zero"));

        horizontalLayout_28->addWidget(btn_y_zero);


        verticalLayout_10->addLayout(horizontalLayout_28);


        verticalLayout_2->addWidget(groupBox_4);

        groupBox_8 = new QGroupBox(tab_turntable_control);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        groupBox_8->setGeometry(QRect(310, 210, 671, 71));
        horizontalLayout_22 = new QHBoxLayout(groupBox_8);
        horizontalLayout_22->setObjectName(QString::fromUtf8("horizontalLayout_22"));
        label = new QLabel(groupBox_8);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_22->addWidget(label);

        line_edit_monitor_x_pos = new QLineEdit(groupBox_8);
        line_edit_monitor_x_pos->setObjectName(QString::fromUtf8("line_edit_monitor_x_pos"));

        horizontalLayout_22->addWidget(line_edit_monitor_x_pos);

        label_27 = new QLabel(groupBox_8);
        label_27->setObjectName(QString::fromUtf8("label_27"));

        horizontalLayout_22->addWidget(label_27);

        line_edit_monitor_x_speed = new QLineEdit(groupBox_8);
        line_edit_monitor_x_speed->setObjectName(QString::fromUtf8("line_edit_monitor_x_speed"));

        horizontalLayout_22->addWidget(line_edit_monitor_x_speed);

        label_3 = new QLabel(groupBox_8);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_22->addWidget(label_3);

        line_edit_monitor_y_pos = new QLineEdit(groupBox_8);
        line_edit_monitor_y_pos->setObjectName(QString::fromUtf8("line_edit_monitor_y_pos"));

        horizontalLayout_22->addWidget(line_edit_monitor_y_pos);

        label_28 = new QLabel(groupBox_8);
        label_28->setObjectName(QString::fromUtf8("label_28"));

        horizontalLayout_22->addWidget(label_28);

        line_edit_monitor_y_speed = new QLineEdit(groupBox_8);
        line_edit_monitor_y_speed->setObjectName(QString::fromUtf8("line_edit_monitor_y_speed"));

        horizontalLayout_22->addWidget(line_edit_monitor_y_speed);

        groupBox_9 = new QGroupBox(tab_turntable_control);
        groupBox_9->setObjectName(QString::fromUtf8("groupBox_9"));
        groupBox_9->setGeometry(QRect(310, 1, 671, 141));
        verticalLayout_8 = new QVBoxLayout(groupBox_9);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        horizontalLayout_25 = new QHBoxLayout();
        horizontalLayout_25->setObjectName(QString::fromUtf8("horizontalLayout_25"));
        label_29 = new QLabel(groupBox_9);
        label_29->setObjectName(QString::fromUtf8("label_29"));

        horizontalLayout_25->addWidget(label_29);

        line_edit_x_pos_ref = new QLineEdit(groupBox_9);
        line_edit_x_pos_ref->setObjectName(QString::fromUtf8("line_edit_x_pos_ref"));

        horizontalLayout_25->addWidget(line_edit_x_pos_ref);

        label_31 = new QLabel(groupBox_9);
        label_31->setObjectName(QString::fromUtf8("label_31"));

        horizontalLayout_25->addWidget(label_31);

        combo_box_x_track_ref = new QComboBox(groupBox_9);
        combo_box_x_track_ref->setObjectName(QString::fromUtf8("combo_box_x_track_ref"));

        horizontalLayout_25->addWidget(combo_box_x_track_ref);

        label_30 = new QLabel(groupBox_9);
        label_30->setObjectName(QString::fromUtf8("label_30"));

        horizontalLayout_25->addWidget(label_30);

        line_edit_y_pos_ref = new QLineEdit(groupBox_9);
        line_edit_y_pos_ref->setObjectName(QString::fromUtf8("line_edit_y_pos_ref"));

        horizontalLayout_25->addWidget(line_edit_y_pos_ref);

        label_32 = new QLabel(groupBox_9);
        label_32->setObjectName(QString::fromUtf8("label_32"));

        horizontalLayout_25->addWidget(label_32);

        combo_box_y_track_ref = new QComboBox(groupBox_9);
        combo_box_y_track_ref->setObjectName(QString::fromUtf8("combo_box_y_track_ref"));

        horizontalLayout_25->addWidget(combo_box_y_track_ref);


        verticalLayout_8->addLayout(horizontalLayout_25);

        horizontalLayout_24 = new QHBoxLayout();
        horizontalLayout_24->setObjectName(QString::fromUtf8("horizontalLayout_24"));
        btn_set_target_pos = new QPushButton(groupBox_9);
        btn_set_target_pos->setObjectName(QString::fromUtf8("btn_set_target_pos"));

        horizontalLayout_24->addWidget(btn_set_target_pos);

        label_45 = new QLabel(groupBox_9);
        label_45->setObjectName(QString::fromUtf8("label_45"));

        horizontalLayout_24->addWidget(label_45);

        controller_selection = new QComboBox(groupBox_9);
        controller_selection->setObjectName(QString::fromUtf8("controller_selection"));

        horizontalLayout_24->addWidget(controller_selection);

        control_status = new QLabel(groupBox_9);
        control_status->setObjectName(QString::fromUtf8("control_status"));

        horizontalLayout_24->addWidget(control_status);


        verticalLayout_8->addLayout(horizontalLayout_24);

        groupBox_10 = new QGroupBox(tab_turntable_control);
        groupBox_10->setObjectName(QString::fromUtf8("groupBox_10"));
        groupBox_10->setGeometry(QRect(310, 140, 671, 71));
        horizontalLayout_23 = new QHBoxLayout(groupBox_10);
        horizontalLayout_23->setObjectName(QString::fromUtf8("horizontalLayout_23"));
        label_46 = new QLabel(groupBox_10);
        label_46->setObjectName(QString::fromUtf8("label_46"));

        horizontalLayout_23->addWidget(label_46);

        line_edit_kp_parameter = new QLineEdit(groupBox_10);
        line_edit_kp_parameter->setObjectName(QString::fromUtf8("line_edit_kp_parameter"));

        horizontalLayout_23->addWidget(line_edit_kp_parameter);

        label_47 = new QLabel(groupBox_10);
        label_47->setObjectName(QString::fromUtf8("label_47"));

        horizontalLayout_23->addWidget(label_47);

        line_edit_ki_parameter = new QLineEdit(groupBox_10);
        line_edit_ki_parameter->setObjectName(QString::fromUtf8("line_edit_ki_parameter"));

        horizontalLayout_23->addWidget(line_edit_ki_parameter);

        label_48 = new QLabel(groupBox_10);
        label_48->setObjectName(QString::fromUtf8("label_48"));

        horizontalLayout_23->addWidget(label_48);

        line_edit_kd_parameter = new QLineEdit(groupBox_10);
        line_edit_kd_parameter->setObjectName(QString::fromUtf8("line_edit_kd_parameter"));

        horizontalLayout_23->addWidget(line_edit_kd_parameter);

        btn_set_pidcontroller_parameter = new QPushButton(groupBox_10);
        btn_set_pidcontroller_parameter->setObjectName(QString::fromUtf8("btn_set_pidcontroller_parameter"));

        horizontalLayout_23->addWidget(btn_set_pidcontroller_parameter);

        btn_stop_pidcontrol = new QPushButton(groupBox_10);
        btn_stop_pidcontrol->setObjectName(QString::fromUtf8("btn_stop_pidcontrol"));

        horizontalLayout_23->addWidget(btn_stop_pidcontrol);

        groupBox_chart_2 = new QGroupBox(tab_turntable_control);
        groupBox_chart_2->setObjectName(QString::fromUtf8("groupBox_chart_2"));
        groupBox_chart_2->setGeometry(QRect(310, 280, 671, 401));
        groupBox_chart_2->setFont(font1);
        horizontalLayout_38 = new QHBoxLayout(groupBox_chart_2);
        horizontalLayout_38->setObjectName(QString::fromUtf8("horizontalLayout_38"));
        turntable_position_chart = new QWidget(groupBox_chart_2);
        turntable_position_chart->setObjectName(QString::fromUtf8("turntable_position_chart"));

        horizontalLayout_38->addWidget(turntable_position_chart);

        tabWidget->addTab(tab_turntable_control, QString());
        label_39 = new QLabel(centralwidget);
        label_39->setObjectName(QString::fromUtf8("label_39"));
        label_39->setGeometry(QRect(20, 10, 191, 21));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Times New Roman"));
        font4.setPointSize(16);
        label_39->setFont(font4);
        label_39->setAlignment(Qt::AlignmentFlag::AlignCenter);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        group_box_stm32_connection->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\351\207\207\351\233\206\347\263\273\347\273\237STM32\344\273\245\345\244\252\347\275\221\350\277\236\346\216\245", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241IP", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250\347\253\257\345\217\243", nullptr));
        pushButton_connect->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\350\256\276\345\244\207", nullptr));
        pushButton_disconnect->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        label_status->setText(QCoreApplication::translate("MainWindow", "\346\234\252\350\277\236\346\216\245", nullptr));
        turntable_connection->setTitle(QCoreApplication::translate("MainWindow", "\345\244\251\347\272\277\350\275\254\345\217\260\350\277\236\346\216\245", nullptr));
        label_38->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207", nullptr));
        label_40->setText(QCoreApplication::translate("MainWindow", "\346\243\200\351\252\214\344\275\215", nullptr));
        label_42->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\344\275\215", nullptr));
        label_43->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\344\275\215", nullptr));
        label_44->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\350\256\276\345\244\207\350\267\257\345\276\204", nullptr));
        pushButton_connection->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\350\275\254\345\217\260", nullptr));
        pushButton_disconnection->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        label_37->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        connection_status->setText(QCoreApplication::translate("MainWindow", "\346\234\252\350\277\236\346\216\245", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_device_connect), QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\350\277\236\346\216\245", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\346\216\247\345\210\266\346\214\207\344\273\244\350\256\276\347\275\256", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "X\345\235\220\346\240\207", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "Y\345\235\220\346\240\207", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246", nullptr));
        pushButton_motor_control->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\347\224\265\346\234\272\346\214\207\344\273\244", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\345\212\237\347\216\207\345\257\273\344\274\230\346\214\207\344\273\244\350\256\276\347\275\256", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "\350\275\250\350\277\271\350\256\276\345\256\232", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\346\234\200\345\244\247\347\224\265\345\216\213", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\347\224\265\345\216\213\346\255\245\351\225\277", nullptr));
        label_26->setText(QCoreApplication::translate("MainWindow", "\345\210\235\345\247\213\347\224\265\345\216\213", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\346\226\271\345\275\242\350\276\271\351\225\277", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\346\226\271\345\275\242\346\255\245\351\225\277", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\345\234\206\345\275\242\345\215\212\345\276\204", nullptr));
        pushButton_find_optimal->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\345\257\273\344\274\230\346\214\207\344\273\244", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MainWindow", "\345\257\273\344\274\230\350\277\207\347\250\213\346\225\260\346\215\256\346\230\276\347\244\272", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\351\200\232\351\201\223", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\345\212\237\347\216\207", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\347\224\265\345\216\213", nullptr));
        groupBox_chart->setTitle(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\344\275\215\347\275\256\350\275\250\350\277\271\345\233\276", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "\345\257\273\344\274\230\347\273\223\346\236\234\346\230\276\347\244\272", nullptr));
        label_21->setText(QCoreApplication::translate("MainWindow", "Channel1_Vmax", nullptr));
        label_22->setText(QCoreApplication::translate("MainWindow", "Channel2_Vmax", nullptr));
        label_23->setText(QCoreApplication::translate("MainWindow", "Channel3_Vmax", nullptr));
        label_24->setText(QCoreApplication::translate("MainWindow", "Channel4_Vmax", nullptr));
        label_25->setText(QCoreApplication::translate("MainWindow", "Pmax", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\345\275\223\345\211\215\347\212\266\346\200\201\350\257\273\345\217\226", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "x\345\235\220\346\240\207", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "y\345\235\220\346\240\207", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_stm32), QCoreApplication::translate("MainWindow", "\346\216\245\346\224\266STM32\346\225\260\346\215\256", nullptr));
        pushButton_filenamedisplay->setText(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266\346\265\217\350\247\210", nullptr));
        pushButton_read_file->setText(QCoreApplication::translate("MainWindow", "\345\206\205\345\256\271\346\237\245\347\234\213", nullptr));
        label_selected_file->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\346\234\252\351\200\211\344\270\255", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_file_read), QCoreApplication::translate("MainWindow", "\345\255\230\345\202\250\346\226\207\344\273\266\350\257\273\345\217\226", nullptr));
        group_box_turntable_shape->setTitle(QCoreApplication::translate("MainWindow", "\350\275\254\345\217\260\345\236\213\345\217\267", nullptr));
        label_turntable_img->setText(QString());
        open_group_box_turntable_control->setTitle(QCoreApplication::translate("MainWindow", "\345\274\200\347\216\257\346\265\213\350\257\225", nullptr));
        groupBox_3->setTitle(QString());
        label_36->setText(QCoreApplication::translate("MainWindow", "X\350\275\264\351\200\237\345\272\246", nullptr));
        label_35->setText(QCoreApplication::translate("MainWindow", "Y\350\275\264\351\200\237\345\272\246", nullptr));
        btn_stop_x_turntable_run->setText(QCoreApplication::translate("MainWindow", "x\350\275\264\345\201\234\346\255\242", nullptr));
        btn_stop_y_turntable_run->setText(QCoreApplication::translate("MainWindow", "y\350\275\264\345\201\234\346\255\242", nullptr));
        btn_set_speed->setText(QCoreApplication::translate("MainWindow", "\350\256\276\345\256\232\351\200\237\345\272\246", nullptr));
        groupBox_4->setTitle(QString());
        label_33->setText(QCoreApplication::translate("MainWindow", "X\350\275\264\344\275\215\347\275\256", nullptr));
        btn_set_x_pos->setText(QCoreApplication::translate("MainWindow", "\344\275\215\347\275\256\350\256\276\345\256\232", nullptr));
        label_34->setText(QCoreApplication::translate("MainWindow", "Y\350\275\264\344\275\215\347\275\256", nullptr));
        btn_set_y_pos->setText(QCoreApplication::translate("MainWindow", "\344\275\215\347\275\256\350\256\276\345\256\232", nullptr));
        btn_x_zero->setText(QCoreApplication::translate("MainWindow", "X\350\275\264\346\270\205\351\233\266", nullptr));
        btn_y_zero->setText(QCoreApplication::translate("MainWindow", "Y\350\275\264\346\270\205\351\233\266", nullptr));
        groupBox_8->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\347\233\221\346\216\247\345\214\272\345\237\237", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "X\350\275\264\344\275\215\347\275\256", nullptr));
        label_27->setText(QCoreApplication::translate("MainWindow", "X\350\275\264\351\200\237\345\272\246", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Y\350\275\264\344\275\215\347\275\256", nullptr));
        label_28->setText(QCoreApplication::translate("MainWindow", "Y\350\275\264\351\200\237\345\272\246", nullptr));
        groupBox_9->setTitle(QCoreApplication::translate("MainWindow", "\345\217\202\350\200\203\344\275\215\347\275\256\350\256\276\345\256\232", nullptr));
        label_29->setText(QCoreApplication::translate("MainWindow", "X\350\275\264\344\275\215\347\275\256", nullptr));
        label_31->setText(QCoreApplication::translate("MainWindow", "\345\217\202\350\200\203\350\275\250\350\277\271", nullptr));
        label_30->setText(QCoreApplication::translate("MainWindow", "Y\350\275\264\344\275\215\347\275\256", nullptr));
        label_32->setText(QCoreApplication::translate("MainWindow", "\345\217\202\350\200\203\350\275\250\350\277\271", nullptr));
        btn_set_target_pos->setText(QCoreApplication::translate("MainWindow", "\350\256\276\345\256\232\347\233\256\346\240\207\344\275\215\347\275\256", nullptr));
        label_45->setText(QCoreApplication::translate("MainWindow", "\346\216\247\345\210\266\347\256\227\346\263\225\350\256\276\347\275\256", nullptr));
        control_status->setText(QCoreApplication::translate("MainWindow", "\346\234\252\350\277\233\345\205\245\351\227\255\347\216\257\346\265\213\350\257\225", nullptr));
        groupBox_10->setTitle(QCoreApplication::translate("MainWindow", "PID\347\256\227\345\217\202\346\225\260\350\256\276\345\256\232", nullptr));
        label_46->setText(QCoreApplication::translate("MainWindow", "KP", nullptr));
        label_47->setText(QCoreApplication::translate("MainWindow", "KI", nullptr));
        label_48->setText(QCoreApplication::translate("MainWindow", "KD", nullptr));
        btn_set_pidcontroller_parameter->setText(QCoreApplication::translate("MainWindow", "\345\217\202\346\225\260\350\256\276\345\256\232", nullptr));
        btn_stop_pidcontrol->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\351\227\255\347\216\257", nullptr));
        groupBox_chart_2->setTitle(QCoreApplication::translate("MainWindow", "\350\275\254\345\217\260\344\275\215\347\275\256\350\275\250\350\277\271\345\233\276", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_turntable_control), QCoreApplication::translate("MainWindow", "\345\244\251\347\272\277\350\275\254\345\217\260\346\216\247\345\210\266", nullptr));
        label_39->setText(QCoreApplication::translate("MainWindow", "\345\220\204\347\261\273\350\256\276\345\244\207\346\216\247\345\210\266\344\270\255\345\277\203", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
