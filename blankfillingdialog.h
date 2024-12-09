#ifndef BLANKFILLINGDIALOG_H
#define BLANKFILLINGDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>

class BlankFillingDialog : public QDialog {
    Q_OBJECT

public:
    explicit BlankFillingDialog(const QString &question, QWidget *parent = nullptr);
    QString getUserInput() const;

private slots:
    void onConfirm();

private:
    QString userInput;
    QLineEdit *inputField;
};

#endif // BLANKFILLINGDIALOG_H
