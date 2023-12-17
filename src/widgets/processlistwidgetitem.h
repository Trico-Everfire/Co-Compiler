#pragma once

#include <QListWidgetItem>
#include <QListWidget>
#include "../enums.h"

class Q_WIDGETS_EXPORT CProcessListWidgetItem : public QListWidgetItem {

    QString m_Command{};
    QString m_Arguments{};
    int m_special{};
    bool m_no_wait{};

public:

    explicit CProcessListWidgetItem(QListWidget *listview = nullptr, int type = Type) : QListWidgetItem(listview, type) {this->setCheckState(Qt::Checked);};
    explicit CProcessListWidgetItem(const QString &command, const QString &arguments, int special, bool no_wait,
                                    QListWidget *listview = nullptr, int type = Type) : QListWidgetItem(command + " " + arguments, listview, type) {
        m_Command = command;
        m_Arguments = arguments;
        m_no_wait = no_wait;
        changeSpecial(special);
        this->setCheckState(Qt::Checked);
    };
    explicit CProcessListWidgetItem(const QIcon &icon, const QString &command, const QString &arguments, int special,
                                    bool no_wait, QListWidget *listview = nullptr, int type = Type) : QListWidgetItem(icon, command + " " + arguments, listview, type){
        m_Command = command;
        m_Arguments = arguments;
        m_no_wait = no_wait;
        changeSpecial(special);
        this->setCheckState(Qt::Checked);
    };
    CProcessListWidgetItem(const QListWidgetItem &other) : QListWidgetItem(other) { this->setCheckState(Qt::Checked); };

    void changeCommand(const QString& command)
    {
        m_Command = command;
        this->setText(m_Command + " " + m_Arguments);
    }

    void changeArguments(const QString& arguments)
    {
        m_Arguments = arguments;
        this->setText(m_Command + " " + m_Arguments);
    }

    void changeSpecial(int special)
    {
        this->m_special = special;
        if(special != SPECIAL_EXEC)
        {
            switch(special)
            {
                case CC_SPECIAL_CHANGE_DIRECTORY:
                    changeCommand("Change Directory");
                    break;
                case CC_SPECIAL_COPY_FILE:
                    changeCommand("Copy File");
                    break;
                case CC_SPECIAL_DELETE_FILE:
                    changeCommand("Delete File");
                    break;
                case CC_SPECIAL_RENAME_FILE:
                    changeCommand("Rename File");
                    break;
                case CC_SPECIAL_GENERATE_GRID_NAV:
                    changeCommand("Generate Grid Nav");
                    break;
                case CC_SPECIAL_COPY_FILE_IF_EXISTS:
                    changeCommand("Copy File If Exists");
                    break;
            }
        }
    }

    void changeNoWait(bool no_wait)
    {
        m_no_wait = no_wait;
    }

    QString getCommand()
    {
        return m_Command;
    }

    QString getArguments()
    {
        return m_Arguments;
    }

    int getSpecial()
    {
        return m_special;
    }

    bool getNoWait()
    {
        return m_no_wait;
    }

private:


};

