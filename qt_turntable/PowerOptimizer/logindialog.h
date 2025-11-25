#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStackedWidget>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void onLoginClicked();
    void onCancelClicked();
    void onAddAdminClicked();
    void onRegisterClicked();
    void onBackToLoginClicked();

private:
    void setupUI();
    void setupLoginUI();
    void setupRegisterUI();
    void applyStyles();
    void loadAdmins();
    void saveAdmins();
    bool validateAdmin(const QString &username, const QString &password);

    // 堆叠窗口用于切换界面
    QStackedWidget *stackedWidget;

    // 登录界面控件
    QWidget *loginWidget;
    QLabel *titleLabel;
    QLabel *usernameLabel;
    QLabel *passwordLabel;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    QPushButton *addAdminButton;

    // 注册界面控件
    QWidget *registerWidget;
    QLabel *registerTitleLabel;
    QLabel *newUsernameLabel;
    QLabel *newPasswordLabel;
    QLabel *confirmPasswordLabel;
    QLineEdit *newUsernameEdit;
    QLineEdit *newPasswordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *registerButton;
    QPushButton *backButton;

    // 管理员数据
    QJsonArray adminArray;
    QString adminFilePath;
};

#endif // LOGINDIALOG_H
