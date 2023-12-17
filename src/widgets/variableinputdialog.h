#pragma once

#include <QDialog>
#include <QLineEdit>

class VariableInputDialog : public QDialog
{

public:
    QList<QLineEdit *> fields;

    QStringList getResultList()
    {
        QStringList list;

        for(const auto &edit : fields)
            list << edit->text();

        return list;
    };



    VariableInputDialog(QWidget* parent = nullptr, uint32_t count = 1);
};
