#ifndef TRAJECTORYJSONDIALOG__H
#define TRAJECTORYJSONDIALOG__H
#include <QDialog>
#include <QPlainTextEdit>

class TrajectoryJsonDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TrajectoryJsonDialog(const QString &jsonTemplate,
                                  QWidget *parent = nullptr);

    QString jsonText() const;

private:
    QPlainTextEdit *editor_;
};

#endif // TRAJECTORYJSONDIALOG__H
