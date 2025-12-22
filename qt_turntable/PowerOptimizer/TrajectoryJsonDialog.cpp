// TrajectoryJsonDialog.cpp
#include "TrajectoryJsonDialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>

TrajectoryJsonDialog::TrajectoryJsonDialog(
    const QString &jsonTemplate, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("参考轨迹配置（JSON）");
    resize(700, 500);

    editor_ = new QPlainTextEdit(this);
    editor_->setPlainText(jsonTemplate);
    editor_->setFont(QFont("Monospace"));
    editor_->setWordWrapMode(QTextOption::NoWrap);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(editor_);
    layout->addWidget(buttons);
}

QString TrajectoryJsonDialog::jsonText() const
{
    return editor_->toPlainText();
}
