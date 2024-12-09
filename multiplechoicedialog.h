#ifndef MULTIPLECHOICEDIALOG_H
#define MULTIPLECHOICEDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStringList>

class MultipleChoiceDialog : public QDialog {
    Q_OBJECT

public:
    explicit MultipleChoiceDialog(const QString &question, const QStringList &options, QWidget *parent = nullptr);
    QString getUserChoice() const;

private slots:
    void onConfirm();

private:
    QString userChoice;
    QButtonGroup *buttonGroup;
};

#endif // MULTIPLECHOICEDIALOG_H
