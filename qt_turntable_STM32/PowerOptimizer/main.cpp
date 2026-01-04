#include "mainwindow.h"
#include "logindialog.h"
#include <QApplication>
#include <QStyleFactory>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置应用程序信息
    QApplication::setApplicationName("各类设备控制中心");
    QApplication::setApplicationVersion("2.0");
    QApplication::setOrganizationName("YourCompany");

    // 设置现代风格
    a.setStyle(QStyleFactory::create("Fusion"));

    // 显示登录对话框
    // LoginDialog loginDialog;
    // if (loginDialog.exec() != QDialog::Accepted) {
    //     return 0; // 用户取消登录，退出程序
    // }

    MainWindow w;
    w.show();

    return a.exec();
}
