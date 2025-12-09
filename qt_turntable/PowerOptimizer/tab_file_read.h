#ifndef TAB_FILE_READ_H
#define TAB_FILE_READ_H

#include <QObject>

class MainWindow;

class TabFileRead : public QObject
{
    Q_OBJECT
public:
    explicit TabFileRead(MainWindow *mw);
    ~TabFileRead() override;

    void setupConnections();

public slots:
    void on_pushButton_filenamedisplay_clicked();  // 文件存储情况显示
    void on_pushButton_read_file_clicked();        // 文件内容查看
    void on_listWidget_files_itemSelectionChanged(); // 文件选择变化

private:
    MainWindow *mw;
    // 文件读取相关变量
    QString m_currentSelectedFile;             // 当前选中的文件路径
    // 文件读取相关函数
    void scanDataFiles();                      // 扫描数据文件
    void displayFileContent(const QString &filePath); // 显示文件内容（表格形式）
    void setupTableWidget();                   // 初始化表格控件
    void loadCsvToTable(const QString &filePath); // 加载CSV到表格

};

#endif // TAB_FILE_READ_H
