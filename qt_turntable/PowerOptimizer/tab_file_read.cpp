#include "tab_file_read.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFileInfoList>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

TabFileRead::TabFileRead(MainWindow *mw_)
    : QObject(mw_), mw(mw_)
{
}

TabFileRead::~TabFileRead()
{
}

void TabFileRead::setupConnections()
{
    if (!mw) return;
    connect(mw->ui->pushButton_filenamedisplay, &QPushButton::clicked,
            this, &TabFileRead::on_pushButton_filenamedisplay_clicked, Qt::UniqueConnection);
    connect(mw->ui->pushButton_read_file, &QPushButton::clicked,
            this, &TabFileRead::on_pushButton_read_file_clicked, Qt::UniqueConnection);
    connect(mw->ui->listWidget_files, &QListWidget::itemSelectionChanged,
            this, &TabFileRead::on_listWidget_files_itemSelectionChanged, Qt::UniqueConnection);

    // 双击直接查看文件内容
    connect(mw->ui->listWidget_files, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem *item) {
                displayFileContent(item->data(Qt::UserRole).toString());
            });

    // 初始化表格样式
    setupTableWidget();
}

/**
 * @brief 初始化文件内容显示表格，包括样式、不可编辑性与行列调整策略
 */
void TabFileRead::setupTableWidget()
{
    // 设置表格属性
    mw->ui->tableWidget_file_content->setEditTriggers(QAbstractItemView::NoEditTriggers); // 不可编辑
    mw->ui->tableWidget_file_content->setSelectionBehavior(QAbstractItemView::SelectRows); // 选择整行
    mw->ui->tableWidget_file_content->setSelectionMode(QAbstractItemView::SingleSelection); // 单选
    mw->ui->tableWidget_file_content->setAlternatingRowColors(true); // 交替行颜色
    // 启用排序
    // mw->ui->tableWidget_file_content->setSortingEnabled(true);

    // 设置表头属性
    QHeaderView *horizontalHeader = mw->ui->tableWidget_file_content->horizontalHeader();
    horizontalHeader->setSectionResizeMode(QHeaderView::Stretch); // 自动拉伸列宽
    horizontalHeader->setStretchLastSection(true); // 最后一列自动拉伸

    QHeaderView *verticalHeader = mw->ui->tableWidget_file_content->verticalHeader();
    verticalHeader->setVisible(true); // 显示行号

    // 设置表格样式
    mw->ui->tableWidget_file_content->setStyleSheet(
        "QTableWidget {"
        "    gridline-color: #d0d0d0;"
        "    background-color: white;"
        "}"
        "QTableWidget::item {"
        "    padding: 5px;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #0078d7;"
        "    color: white;"
        "}"
        );
}

/**
 * @brief 扫描当前目录中的 CSV 数据文件并更新列表显示
 *
 * 会清空现有文件列表，扫描工作目录下所有 .csv 文件，
 * 以修改时间排序，并更新 listWidget_files。
 */
void TabFileRead::scanDataFiles()
{
    // 清空当前文件列表
    mw->ui->listWidget_files->clear();
    m_currentSelectedFile.clear();
    mw->ui->label_selected_file->setText("当前选中文件：无");

    // 获取当前工作目录
    QDir currentDir(QDir::currentPath());

    // 设置文件过滤器，查找CSV文件
    QStringList filters;
    filters << "*.csv";
    currentDir.setNameFilters(filters);

    // 获取文件列表并按修改时间排序（最新的在前）
    QFileInfoList fileList = currentDir.entryInfoList(QDir::Files, QDir::Time);

    if (fileList.isEmpty()) {
        qDebug() << "未找到任何CSV数据文件";

        // 添加提示项
        QListWidgetItem *item = new QListWidgetItem("未找到CSV数据文件", mw->ui->listWidget_files);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable); // 设置为不可选择
        return;
    }

    // 添加到列表控件
    for (const QFileInfo &fileInfo : fileList) {
        // 只显示文件名，不显示完整路径
        QString fileName = fileInfo.fileName();

        // 确保文件名以.csv结尾，防止显示其他类型的文件
        if (fileName.toLower().endsWith(".csv")) {
            QString displayText = QString("%1 (大小: %2字节)")
                                      .arg(fileName)
                                      .arg(fileInfo.size());

            QListWidgetItem *item = new QListWidgetItem(displayText, mw->ui->listWidget_files);
            item->setData(Qt::UserRole, fileInfo.absoluteFilePath()); // 保存完整路径

            qDebug() << "添加文件到列表:" << fileName << "完整路径:" << fileInfo.absoluteFilePath();
        }
    }

    qDebug() << "找到" << fileList.size() << "个数据文件";
}
/**
 * @brief 处理文件列表选中项变化事件
 *
 * 记录当前选中文件，并更新右侧文件名称显示。
 */
void TabFileRead::on_listWidget_files_itemSelectionChanged()
{
    QList<QListWidgetItem*> selectedItems = mw->ui->listWidget_files->selectedItems();

    if (selectedItems.isEmpty()) {
        m_currentSelectedFile.clear();
        mw->ui->label_selected_file->setText("当前选中文件：无");
        return;
    }

    QListWidgetItem *selectedItem = selectedItems.first();
    m_currentSelectedFile = selectedItem->data(Qt::UserRole).toString();

    QString displayText = QString("当前选中文件：%1").arg(QFileInfo(m_currentSelectedFile).fileName());
    mw->ui->label_selected_file->setText(displayText);

    qDebug() << "选中文件：" << m_currentSelectedFile;
}
/**
 * @brief 处理文件列表选中项变化事件
 *
 * 记录当前选中文件，并更新右侧文件名称显示。
 */
void TabFileRead::displayFileContent(const QString &filePath)
{
    if (filePath.isEmpty()) {
        QMessageBox::warning(mw, "警告", "请先选择要查看的文件");
        return;
    }

    loadCsvToTable(filePath);
}
/**
 * @brief 将 CSV 文件加载到 tableWidget_file_content 中显示
 * @param filePath 目标 CSV 文件路径
 *
 * 支持简单的 CSV 格式解析，不解析引号内部逗号。
 */
void TabFileRead::loadCsvToTable(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        QMessageBox::critical(mw, "错误", QString("文件不存在: %1").arg(filePath));
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(mw, "错误", QString("无法打开文件: %1").arg(filePath));
        return;
    }

    // 清空表格
    mw->ui->tableWidget_file_content->clear();
    mw->ui->tableWidget_file_content->setRowCount(0);
    mw->ui->tableWidget_file_content->setColumnCount(0);

    QTextStream in(&file);
    QStringList lines;

    // 读取所有行
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.trimmed().isEmpty()) {
            lines.append(line);
        }
    }
    file.close();

    if (lines.isEmpty()) {
        qDebug() << "文件为空:" << filePath;
        QMessageBox::information(mw, "提示", "选中的文件为空");
        return;
    }

    // 解析CSV内容
    QStringList headers;
    QList<QStringList> data;

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        // 简单的CSV解析（处理逗号分隔，但不处理引号内的逗号）
        QStringList fields = line.split(',');
        //QStringList fields = parseCsvLine(line);

        // 移除每个字段的空白字符
        for (int j = 0; j < fields.size(); ++j) {
            fields[j] = fields[j].trimmed();
        }

        if (i == 0) {
            // 第一行作为表头
            headers = fields;
        } else {
            // 数据行
            data.append(fields);
        }
    }

    // 设置表格列数和表头
    mw->ui->tableWidget_file_content->setColumnCount(headers.size());
    mw->ui->tableWidget_file_content->setHorizontalHeaderLabels(headers);
    // 设置行数
    mw->ui->tableWidget_file_content->setRowCount(data.size());

    // 填充数据
    for (int row = 0; row < data.size(); ++row) {
        const QStringList &rowData = data[row];

        for (int col = 0; col < rowData.size() && col < headers.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);

            // 尝试将数值列右对齐
            bool isNumber;
            rowData[col].toDouble(&isNumber);
            if (isNumber) {
                item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            } else {
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            }

            mw->ui->tableWidget_file_content->setItem(row, col, item);
        }
    }

    // 调整列宽
    mw->ui->tableWidget_file_content->resizeColumnsToContents();

    // 显示文件信息
    QString info = QString("文件: %1 | 行数: %2 | 列数: %3")
                       .arg(QFileInfo(filePath).fileName())
                       .arg(data.size())
                       .arg(headers.size());

    qDebug() << "已加载CSV文件到表格:" << filePath << "行数:" << data.size() << "列数:" << headers.size();
}
/**
 * @brief “显示文件存储情况”按钮点击处理
 */
void TabFileRead::on_pushButton_filenamedisplay_clicked()
{
    qDebug() << "开始扫描数据文件...";
    scanDataFiles();
}
/**
 * @brief “查看文件内容”按钮点击处理
 */
void TabFileRead::on_pushButton_read_file_clicked()
{
    if (m_currentSelectedFile.isEmpty()) {
        QMessageBox::warning(mw, "警告", "请先选择要查看的文件");
        return;
    }

    qDebug() << "查看文件内容：" << m_currentSelectedFile;
    displayFileContent(m_currentSelectedFile);
}
