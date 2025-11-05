#include "logindialog.h"
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    // 设置管理员文件路径
    adminFilePath = QDir::currentPath() + "/admins.json";

    setupUI();
    applyStyles();
    loadAdmins();

    // 设置对话框属性
    setWindowTitle("管理员登录 - 数据采集系统");
    setFixedSize(450, 350);
    setModal(true);

    // 居中显示
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    // 创建堆叠窗口
    stackedWidget = new QStackedWidget(this);

    // 创建登录和注册界面
    loginWidget = new QWidget();
    registerWidget = new QWidget();

    setupLoginUI();
    setupRegisterUI();

    // 将两个界面添加到堆叠窗口
    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(registerWidget);

    // 设置主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);
}

void LoginDialog::setupLoginUI()
{
    // 创建控件
    titleLabel = new QLabel("数据采集系统可视化");
    titleLabel->setAlignment(Qt::AlignCenter);


    usernameLabel = new QLabel("用户名:");
    passwordLabel = new QLabel("密码:");

    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("请输入用户名");
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("请输入密码");

    loginButton = new QPushButton("登录");
    cancelButton = new QPushButton("取消");
    addAdminButton = new QPushButton("添加管理员账号");

    // 设置布局
    QVBoxLayout *loginLayout = new QVBoxLayout(loginWidget);

    // 标题
    loginLayout->addWidget(titleLabel);
    loginLayout->addSpacing(30);

    // 用户名输入
    QHBoxLayout *usernameLayout = new QHBoxLayout();
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(usernameEdit);
    loginLayout->addLayout(usernameLayout);

    loginLayout->addSpacing(15);

    // 密码输入
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordEdit);
    loginLayout->addLayout(passwordLayout);

    loginLayout->addSpacing(30);

    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    loginLayout->addLayout(buttonLayout);

    loginLayout->addSpacing(15);

    // 添加管理员按钮
    QHBoxLayout *addAdminLayout = new QHBoxLayout();
    addAdminLayout->addStretch();
    addAdminLayout->addWidget(addAdminButton);
    addAdminLayout->addStretch();
    loginLayout->addLayout(addAdminLayout);

    // 连接信号槽
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::onCancelClicked);
    connect(addAdminButton, &QPushButton::clicked, this, &LoginDialog::onAddAdminClicked);
}

void LoginDialog::setupRegisterUI()
{
    // 创建控件
    registerTitleLabel = new QLabel("添加管理员账号");
    registerTitleLabel->setAlignment(Qt::AlignCenter);


    newUsernameLabel = new QLabel("用户名:");
    newPasswordLabel = new QLabel("密码:");
    confirmPasswordLabel = new QLabel("确认密码:");

    newUsernameEdit = new QLineEdit();
    newUsernameEdit->setPlaceholderText("请输入新用户名");
    newPasswordEdit = new QLineEdit();
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    newPasswordEdit->setPlaceholderText("请输入密码");
    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setPlaceholderText("请再次输入密码");

    registerButton = new QPushButton("注册");
    backButton = new QPushButton("返回登录");

    // 设置布局
    QVBoxLayout *registerLayout = new QVBoxLayout(registerWidget);

    // 标题
    registerLayout->addWidget(registerTitleLabel);
    registerLayout->addSpacing(30);

    // 用户名输入
    QHBoxLayout *usernameLayout = new QHBoxLayout();
    usernameLayout->addWidget(newUsernameLabel);
    usernameLayout->addWidget(newUsernameEdit);
    registerLayout->addLayout(usernameLayout);

    registerLayout->addSpacing(15);

    // 密码输入
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    passwordLayout->addWidget(newPasswordLabel);
    passwordLayout->addWidget(newPasswordEdit);
    registerLayout->addLayout(passwordLayout);

    registerLayout->addSpacing(15);

    // 确认密码
    QHBoxLayout *confirmLayout = new QHBoxLayout();
    confirmLayout->addWidget(confirmPasswordLabel);
    confirmLayout->addWidget(confirmPasswordEdit);
    registerLayout->addLayout(confirmLayout);

    registerLayout->addSpacing(30);

    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(registerButton);
    buttonLayout->addWidget(backButton);
    registerLayout->addLayout(buttonLayout);

    // 连接信号槽
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
    connect(backButton, &QPushButton::clicked, this, &LoginDialog::onBackToLoginClicked);
}

void LoginDialog::applyStyles()
{
    // 在设置样式表之前先设置对象名
    titleLabel->setObjectName("mainTitle");
    registerTitleLabel->setObjectName("mainTitle");
    cancelButton->setObjectName("cancelButton");
    addAdminButton->setObjectName("addAdminButton");

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
            font-size: 14px;
        }

        QLabel#mainTitle {
            font-size: 20px;
            font-weight: bold;
            color: white;
            padding: 10px;
            background: transparent;
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

        QPushButton#cancelButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                       stop:0 #f44336, stop:1 #d32f2f);
        }

        QPushButton#cancelButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                       stop:0 #d32f2f, stop:1 #f44336);
        }

        QPushButton#addAdminButton {
            background: transparent;
            border: 1px solid rgba(255, 255, 255, 0.5);
            color: white;
        }

        QPushButton#addAdminButton:hover {
            background: rgba(255, 255, 255, 0.1);
        }
    )");
}
void LoginDialog::loadAdmins()
{
    QFile file(adminFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        // 如果文件不存在，创建默认管理员
        QJsonObject defaultAdmin;
        defaultAdmin["username"] = "admin";
        defaultAdmin["password"] = "admin123";

        adminArray.append(defaultAdmin);
        saveAdmins();
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isArray()) {
        adminArray = doc.array();
    } else {
        // 如果文件内容不是数组，创建默认管理员
        QJsonObject defaultAdmin;
        defaultAdmin["username"] = "admin";
        defaultAdmin["password"] = "admin123";

        adminArray.append(defaultAdmin);
        saveAdmins();
    }
}

void LoginDialog::saveAdmins()
{
    QFile file(adminFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "错误", "无法保存管理员数据！");
        return;
    }

    QJsonDocument doc(adminArray);
    file.write(doc.toJson());
    file.close();
}

bool LoginDialog::validateAdmin(const QString &username, const QString &password)
{
    for (const QJsonValue &value : adminArray) {
        QJsonObject admin = value.toObject();
        if (admin["username"].toString() == username &&
            admin["password"].toString() == password) {
            return true;
        }
    }
    return false;
}

void LoginDialog::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "登录失败", "用户名和密码不能为空！");
        return;
    }

    // 验证管理员账号
    if (validateAdmin(username, password)) {
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

void LoginDialog::onAddAdminClicked()
{
    // 切换到注册界面
    stackedWidget->setCurrentWidget(registerWidget);

    // 清空注册表单
    newUsernameEdit->clear();
    newPasswordEdit->clear();
    confirmPasswordEdit->clear();
    newUsernameEdit->setFocus();
}

void LoginDialog::onRegisterClicked()
{
    QString username = newUsernameEdit->text().trimmed();
    QString password = newPasswordEdit->text().trimmed();
    QString confirmPassword = confirmPasswordEdit->text().trimmed();

    // 验证输入
    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "注册失败", "所有字段都必须填写！");
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "注册失败", "两次输入的密码不一致！");
        newPasswordEdit->clear();
        confirmPasswordEdit->clear();
        newPasswordEdit->setFocus();
        return;
    }

    if (password.length() < 6) {
        QMessageBox::warning(this, "注册失败", "密码长度至少需要6位！");
        return;
    }

    // 检查用户名是否已存在
    for (const QJsonValue &value : adminArray) {
        QJsonObject admin = value.toObject();
        if (admin["username"].toString() == username) {
            QMessageBox::warning(this, "注册失败", "该用户名已存在！");
            newUsernameEdit->clear();
            newUsernameEdit->setFocus();
            return;
        }
    }

    // 添加新管理员
    QJsonObject newAdmin;
    newAdmin["username"] = username;
    newAdmin["password"] = password;
    adminArray.append(newAdmin);
    saveAdmins();

    QMessageBox::information(this, "注册成功", "管理员账号添加成功！");

    // 返回登录界面
    onBackToLoginClicked();
}

void LoginDialog::onBackToLoginClicked()
{
    // 切换到登录界面
    stackedWidget->setCurrentWidget(loginWidget);

    // 清空登录表单
    usernameEdit->clear();
    passwordEdit->clear();
    usernameEdit->setFocus();
}
