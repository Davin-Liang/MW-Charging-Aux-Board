#include "logindialog.h"
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    applyStyles();

    // 设置对话框属性
    setWindowTitle("管理员登录 - 数据采集系统");
    setFixedSize(400, 300);
    setModal(true);

    // 居中显示 - 使用 Qt6 兼容的方式
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    // 连接信号槽
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::onCancelClicked);
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    // 创建控件
    titleLabel = new QLabel("数据采集系统可视化");
    titleLabel->setAlignment(Qt::AlignCenter);

    usernameLabel = new QLabel("用户名:");
    passwordLabel = new QLabel("密码:");

    usernameEdit = new QLineEdit();
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("登录");
    cancelButton = new QPushButton("取消");

    // 设置布局
    QVBoxLayout *mainLayout = new QVBoxLayout();

    // 标题
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(30);

    // 用户名输入
    QHBoxLayout *usernameLayout = new QHBoxLayout();
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(usernameEdit);
    mainLayout->addLayout(usernameLayout);

    mainLayout->addSpacing(15);

    // 密码输入
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordEdit);
    mainLayout->addLayout(passwordLayout);

    mainLayout->addSpacing(30);

    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void LoginDialog::applyStyles()
{
    // 设置现代化样式
    setStyleSheet(R"(
        QDialog {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                       stop:0 #667eea, stop:1 #764ba2);
            border-radius: 10px;
        }

        QLabel {
            color: white;
            font-family: 'Microsoft YaHei';
        }

        QLabel#title {
            font-size: 24px;
            font-weight: bold;
            color: white;
            padding: 10px;
        }

        QLineEdit {
            background-color: rgba(255, 255, 255, 0.9);
            border: 2px solid rgba(255, 255, 255, 0.3);
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 14px;
            color: #333;
            min-height: 20px;
        }

        QLineEdit:focus {
            border-color: #4CAF50;
            background-color: white;
        }

        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                       stop:0 #4CAF50, stop:1 #45a049);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
            min-width: 80px;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                       stop:0 #45a049, stop:1 #4CAF50);
        }

        QPushButton:pressed {
            background: #367c39;
        }

        QPushButton#cancel {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                       stop:0 #f44336, stop:1 #d32f2f);
        }

        QPushButton#cancel:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                       stop:0 #d32f2f, stop:1 #f44336);
        }
    )");

    // 为标题设置特殊样式
    titleLabel->setObjectName("title");
    cancelButton->setObjectName("cancel");
}

void LoginDialog::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    // 简单的登录验证（实际使用时应该更安全）
    if (username == "admin" && password == "admin123") {
        accept(); // 登录成功，关闭对话框并返回Accepted
    } else {
        QMessageBox::warning(this, "登录失败",
                             "用户名或密码错误！\n默认账号：admin\n默认密码：admin123");
        passwordEdit->clear();
        passwordEdit->setFocus();
    }
}

void LoginDialog::onCancelClicked()
{
    reject(); // 取消登录
}
