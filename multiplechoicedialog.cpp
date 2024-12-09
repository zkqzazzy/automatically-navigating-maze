#include "multiplechoicedialog.h"

MultipleChoiceDialog::MultipleChoiceDialog(const QString &question, const QStringList &options, QWidget *parent)
    : QDialog(parent), userChoice("") {
    // Set up the layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Add question label
    QLabel *questionLabel = new QLabel(question, this);
    layout->addWidget(questionLabel);

    // Add radio buttons for each option
    buttonGroup = new QButtonGroup(this);
    for (const QString &option : options) {//为每个选项创建一个新的单选按钮
        QRadioButton *radioButton = new QRadioButton(option, this);
        layout->addWidget(radioButton);
        buttonGroup->addButton(radioButton);
    }

    // Add confirm button
    QPushButton *confirmButton = new QPushButton("确认", this);
    layout->addWidget(confirmButton);

    // Connect confirm button click signal to onConfirm slot
    connect(confirmButton, &QPushButton::clicked, this, &MultipleChoiceDialog::onConfirm);
}

void MultipleChoiceDialog::onConfirm() {
    QAbstractButton *checkedButton = buttonGroup->checkedButton();
    if (checkedButton) {
        userChoice = checkedButton->text();
    }
    accept();
}

QString MultipleChoiceDialog::getUserChoice() const {
    return userChoice;
}
