#include <QDialogButtonBox>
#include <QLabel>
#include <QFormLayout>
#include "variableinputdialog.h"

VariableInputDialog::VariableInputDialog(QWidget *parent, uint32_t count) : QDialog(parent)
{
    auto form = new QFormLayout(this);

    form->addRow(new QLabel("Variable:"));

    for(int i = 0; i < count; ++i) {
        QLineEdit *lineEdit = new QLineEdit(this);
        QString label = QString(count == 1 ? "value" : count == 2 ? i == 0 ? "key" : "value" : "value %1" );
        if(count > 2)
            label = label.arg(i + 1);
        form->addRow(label, lineEdit);

        fields << lineEdit;
    }

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                          Qt::Horizontal, this);
    form->addRow(buttonBox);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}