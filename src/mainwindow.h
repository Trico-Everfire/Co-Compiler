#pragma once

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QDialog>
#include <QLineEdit>

class CProcessListWidgetItem;
class QPushButton;
class QListWidget;
class QComboBox;
class QCheckBox;
class QSystemTrayIcon;
class QProcess;
class Console;
class QGroupBox;
class QElapsedTimer;
class LuaSyntaxHighlighter;

class CMainWindow : public QMainWindow {

    QMap<QString, QVector<CProcessListWidgetItem*>> m_Configurations;
    QMap<QString, QString> m_Variables;
    QMap<QString, QString> m_LocalVariables;
    QMap<QString, QString> m_RuntimeVariables;
    QString m_WorkDirectory;
    QString m_errorColor = "#FF0000";
    QString m_warningColor = "#FFFF00";

    QVector<CProcessListWidgetItem*> m_QueueProcesses;

    LuaSyntaxHighlighter *m_pLuaHighlighting;
    QSystemTrayIcon* m_pSystemTrayIcon;
    Console *pConsoleOutput;
    QPushButton *pCompileButton;
    QListWidget *pRunCommandsList;
    QComboBox *pConfigurationComboBox;
    QCheckBox *pContinueAfterProcessCheckBox;
    QPushButton* continueButton;
    QPushButton *pTerminateButton;
    QComboBox *pSpecialComboBox;
    QLineEdit *pCommandLineEdit;
    QTextEdit *pParameterTextEdit;
    QGroupBox *pProcessGroupBox;
    QPushButton *pEditButton;
    QPushButton *pNewItemButton;
    QPushButton *pRemoveItemButton;
    QPushButton *pSaveConfigButton;
    QPushButton *pLockSelection;

    QProcess *m_pCurrentRunningProcess;

    QElapsedTimer *timer;

    public Q_SIGNAL:
        void closed();
private:

    bool hasChanges = false;

    void fillConfigurations(const QJsonDocument &doc);

    void emitConfigurations(QJsonDocument &doc);

    void SaveConfig();

public:
    CMainWindow();

    void closeEvent(QCloseEvent *event) override;

    void runProcessQueue();

    void runSpecialCases(const QStringList &arguments, const QString &raw_args, int special);

    void TerminateCurrentProcess();

    void ChangeConsoleTextColor();

    void ChangeConsoleBackgroundColor();

    void ChangeSpecialComboBox();

    void RunConfigurationChanged();

    void RowsReordered();

    void CommandLineTextChanged(const QString & text);

    void ParameterTextChanged();

    void NoWaitCheckBoxChecked(bool toggled);

    void EditButtonClicked();

    void EditVariables();

    void AddNewCommandListItem();

    void RemoveCommandListItem();

    void ChangeConfigurations(const QString& s);

    void LoadConfigurationFromFile();

    void ExportConfigurationToFile();

    void CompileButtonPressed();

    void OnLockProcessesPressed(bool checked);

};

