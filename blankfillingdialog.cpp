#include "BlankFillingDialog.h"

BlankFillingDialog::BlankFillingDialog(const QString &question, QWidget *parent)
    : QDialog(parent), userInput("") {//初始化
    // Set up the layout
    QVBoxLayout *layout = new QVBoxLayout(this);//垂直布局

    // Add question label
    QLabel *questionLabel = new QLabel(question, this);
    layout->addWidget(questionLabel);

    // Add input field
    inputField = new QLineEdit(this);
    layout->addWidget(inputField);

    // Add confirm button
    QPushButton *confirmButton = new QPushButton("确认", this);
    layout->addWidget(confirmButton);

    // Connect confirm button click signal to onConfirm slot
    connect(confirmButton, &QPushButton::clicked, this, &BlankFillingDialog::onConfirm);
}

void BlankFillingDialog::onConfirm() {
    userInput = inputField->text();
    accept();
}

QString BlankFillingDialog::getUserInput() const {
    return userInput;
}
