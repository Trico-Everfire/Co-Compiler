#include "mainwindow.h"
#include "widgets/processlistwidgetitem.h"
#include "widgets/console.h"
#include "KeyValue.h"
#include "confighandler.h"
#include "enums.h"
#include "widgets/variableinputdialog.h"
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QMenuBar>
#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QProcess>
#include <QTimer>
#include <QScrollBar>
#include <QElapsedTimer>
#include <QColorDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QStandardItem>
#include <QCheckBox>
#include <QSystemTrayIcon>
#include <QButtonGroup>

constexpr auto CONFIG_SAVE_FILTER = "Configuration File (cmdseq.wc cconfig.json);;Hammer Config (cmdseq.wc);;Cocompiler Config (cconfig.json);;All files (*.*)";

CMainWindow::CMainWindow() : QMainWindow(nullptr)
{

    m_pSystemTrayIcon = new QSystemTrayIcon(this);
    m_pCurrentRunningProcess = nullptr;

    auto fileBar = this->menuBar()->addMenu(tr("&File"));
    auto loadConfigAction = fileBar->addAction("&Load Config");
    auto exportConfigAction = fileBar->addAction("&Export Config");

    auto optionsBar = this->menuBar()->addMenu("&Options");
    auto pConsoleTextColorAction = optionsBar->addAction("&Set Console Text Color");
    auto pConsoleBackgroundColorAction = optionsBar->addAction("&Set Console Background Color");
    auto pEditVariablesAction = optionsBar->addAction("&Edit Global Variables");

    auto pMainWidget = new QWidget(this);

    auto pMainLayout = new QGridLayout(pMainWidget);

    auto pConfigurationLabel = new QLabel(tr("Configurations:"),pMainWidget);
    pMainLayout->addWidget(pConfigurationLabel, 0, 0, Qt::AlignTop);

    pConfigurationComboBox = new QComboBox(pMainWidget);
    pConfigurationComboBox->addItem("Default");
    pMainLayout->addWidget(pConfigurationComboBox, 1, 0, Qt::AlignTop);

    auto pRunCommandsLabel = new QLabel((tr("Run Commands:")),pMainWidget);
    pMainLayout->addWidget(pRunCommandsLabel, 2, 0, Qt::AlignTop);

    pRunCommandsList = new QListWidget(pMainWidget);
    pRunCommandsList->setDragDropMode(QListWidget::InternalMove);

    pMainLayout->addWidget(pRunCommandsList, 3, 0, 3, 1, Qt::AlignAbsolute);

    pEditButton = new QPushButton(tr("Edit"), pMainWidget);
    pMainLayout->addWidget(pEditButton, 1, 1, Qt::AlignAbsolute);

    pNewItemButton = new QPushButton(tr("New"),pMainWidget);
    pMainLayout->addWidget(pNewItemButton, 2, 1, Qt::AlignAbsolute);

    pRemoveItemButton = new QPushButton(tr("Remove"),pMainWidget);
    pMainLayout->addWidget(pRemoveItemButton, 3, 1, Qt::AlignAbsolute);

    pSaveConfigButton = new QPushButton(tr("save"),pMainWidget);
    pMainLayout->addWidget(pSaveConfigButton, 4, 1, Qt::AlignAbsolute);

    pLockSelection = new QPushButton(pMainWidget);
    pLockSelection->setIcon(QIcon(":resource/lock_icon.svg"));
    pLockSelection->setIconSize(QSize(24, 24));
    pLockSelection->setFixedSize(28,28);
    pLockSelection->setCheckable(true);
    pMainLayout->addWidget(pLockSelection, 5, 1, Qt::AlignBottom);

    pProcessGroupBox = new QGroupBox("Command Properties",pMainWidget);
    auto pProcessLayout = new QGridLayout(pProcessGroupBox);

    pSpecialComboBox = new QComboBox(pMainWidget);
    pSpecialComboBox->addItem("Executable", SPECIAL_EXEC);
    pSpecialComboBox->addItem("Copy File", CC_SPECIAL_COPY_FILE);
    pSpecialComboBox->addItem("Copy File If Exists", CC_SPECIAL_COPY_FILE_IF_EXISTS);
    pSpecialComboBox->addItem("Change Directory", CC_SPECIAL_CHANGE_DIRECTORY);
    pSpecialComboBox->addItem("Rename File", CC_SPECIAL_RENAME_FILE);
    pSpecialComboBox->addItem("Delete File", CC_SPECIAL_DELETE_FILE);
    pSpecialComboBox->addItem("Select Full Filepath As Variable", CC_SPECIAL_SET_LOCAL_VARIABLE_FULL_PATH_FILE);
    pSpecialComboBox->addItem("Select Filename As Variable", CC_SPECIAL_SET_LOCAL_VARIABLE_FILENAME);
    pSpecialComboBox->addItem("Select Directory As Variable", CC_SPECIAL_SET_LOCAL_VARIABLE_DIRECTORY);
    pSpecialComboBox->addItem("Clear Variable", CC_SPECIAL_CLEAR_LOCAL_VARIABLE);
    pSpecialComboBox->addItem("Fatal Assert", CC_SPECIAL_FATAL_ASSERT);
    pSpecialComboBox->addItem("Warning Assert", CC_SPECIAL_WARNING_ASSERT);
    pSpecialComboBox->addItem("Clear Console", CC_SPECIAL_CLEAR_CONSOLE);
    pSpecialComboBox->addItem("Log", CC_SPECIAL_LOG);

    pProcessLayout->addWidget(pSpecialComboBox, 0 ,0, Qt::AlignTop);

    auto pCommandLabel = new QLabel("Command:",pMainWidget);
    pProcessLayout->addWidget(pCommandLabel, 1 ,0, Qt::AlignTop);

    pCommandLineEdit = new QLineEdit(pMainWidget);
    pProcessLayout->addWidget(pCommandLineEdit, 2 ,0, Qt::AlignTop);

    auto pParameterLabel = new QLabel("Parameters:",pMainWidget);
    pProcessLayout->addWidget(pParameterLabel, 3 ,0, Qt::AlignTop);
    pParameterTextEdit = new QTextEdit(pMainWidget);
    pProcessLayout->addWidget(pParameterTextEdit, 4 ,0, Qt::AlignTop);

    pContinueAfterProcessCheckBox = new QCheckBox("Wait for keypress when done compiling.", pMainWidget);
    pProcessLayout->addWidget(pContinueAfterProcessCheckBox, 5 ,0, Qt::AlignTop);

    pMainLayout->addWidget(pProcessGroupBox, 1, 2, 5, 1);

    pConsoleOutput = new Console(pMainWidget);
    pConsoleOutput->setReadOnly(true);
    pMainLayout->addWidget(pConsoleOutput, 6, 0, 1, 3);

    auto buttonBox = new QDialogButtonBox(pMainWidget);

    pCompileButton = new QPushButton("Run",pMainWidget);
    pCompileButton->setFixedWidth(this->size().width() / 10);
    buttonBox->addButton(pCompileButton, QDialogButtonBox::NoRole);

    continueButton = new QPushButton("Continue",pMainWidget);
    continueButton->setDisabled(true);
    QPalette originalContinueButtonPalette = continueButton->palette();;

    connect(continueButton, &QPushButton::pressed, continueButton, [&,originalContinueButtonPalette]{
        runProcessQueue();
        continueButton->setPalette(originalContinueButtonPalette);
        continueButton->setDisabled(true);
    });

    buttonBox->addButton(continueButton, QDialogButtonBox::NoRole);

    auto pClearButton = new QPushButton("Clear", pMainWidget);
    buttonBox->addButton(pClearButton, QDialogButtonBox::NoRole);

    pTerminateButton = new QPushButton("Terminate", pMainWidget);
    pTerminateButton->setDisabled(true);
    buttonBox->addButton(pTerminateButton, QDialogButtonBox::NoRole);

    pMainLayout->addWidget(buttonBox, 7, 0, 1, 2, Qt::AlignLeft);

    auto pCloseButton = new QPushButton("Close", pMainWidget);
    pCloseButton->setFixedWidth(this->size().width() / 8);
    pMainLayout->addWidget(pCloseButton, 7, 2, Qt::AlignRight);

    this->setCentralWidget(pMainWidget);


    connect(loadConfigAction, &QAction::triggered, this, &CMainWindow::LoadConfigurationFromFile);
    connect(pCloseButton, &QPushButton::clicked, this, &CMainWindow::close);
    connect(pEditVariablesAction, &QAction::triggered, this, &CMainWindow::EditVariables);
    connect(pConsoleTextColorAction, &QAction::triggered, this, &CMainWindow::ChangeConsoleTextColor);
    connect(pConsoleBackgroundColorAction, &QAction::triggered, this, &CMainWindow::ChangeCOnsoleBackgroundColor);
    connect(pConfigurationComboBox, &QComboBox::currentTextChanged, this, &CMainWindow::ChangeConfigurations);
    connect(pRunCommandsList, &QListWidget::currentItemChanged, this, &CMainWindow::RunConfigurationChanged);
    connect(pCommandLineEdit, &QLineEdit::textEdited, this, &CMainWindow::CommandLineTextChanged);
    connect(pRunCommandsList->model(), &QAbstractItemModel::rowsMoved, this, &CMainWindow::RowsReordered);
    connect(pSpecialComboBox, &QComboBox::currentTextChanged, this, &CMainWindow::ChangeSpecialComboBox);
    connect(pParameterTextEdit, &QTextEdit::textChanged, this, &CMainWindow::ParameterTextChanged);
    connect(pContinueAfterProcessCheckBox, &QCheckBox::toggled, this, &CMainWindow::NoWaitCheckBoxChecked);
    connect(pNewItemButton, &QPushButton::clicked, this, &CMainWindow::AddNewCommandListItem);
    connect(pEditButton, &QPushButton::clicked, this, &CMainWindow::EditButtonClicked);
    connect(pRemoveItemButton, &QPushButton::clicked, this, &CMainWindow::RemoveCommandListItem);
    connect(pSaveConfigButton, &QPushButton::clicked, this, &CMainWindow::SaveConfig);
    connect(pLockSelection, &QPushButton::clicked, this, &CMainWindow::OnLockProcessesPressed);
    connect(pCompileButton,&QPushButton::clicked, this, &CMainWindow::CompileButtonPressed);
    connect(pTerminateButton, &QPushButton::clicked, this, &CMainWindow::TerminateCurrentProcess);
    connect(pClearButton, &QPushButton::clicked, pConsoleOutput, &Console::clear);

    auto jsonFile = QFile(QDir::currentPath() + "/cconfig.json");
    if(jsonFile.exists()) {
        jsonFile.open(QFile::ReadOnly);
        fillConfigurations(QJsonDocument::fromJson(jsonFile.readAll()));
        jsonFile.close();
    }

}

void CMainWindow::fillConfigurations(const QJsonDocument &doc) {
    auto json = doc.object()["sequences"].toObject();
    pConfigurationComboBox->clear();

    QPalette p = pConsoleOutput->palette();
    p.setColor(QPalette::Text, doc.object().value("console_text_color").toString());
    p.setColor(QPalette::Base, doc.object().value("console_background_color").toString());

    m_errorColor = doc.object().value("console_error_color").toString();
    m_warningColor = doc.object().value("console_warning_color").toString();

    pConsoleOutput->setPalette(p);

    auto variables = doc.object().value("variables").toObject();

    for(const auto &key : variables.keys())
    {
        this->m_Variables.insert(key, variables[key].toString());
    }

    for(const auto& key : json.keys()) {

        QJsonValue value = json.value(key);
        pConfigurationComboBox->addItem(key);
        QVector<CProcessListWidgetItem*> items;
        for(auto content : value.toArray())
        {
            auto objectContent = content.toObject();
            auto item = new CProcessListWidgetItem(objectContent["command"].toString(),
                                                   objectContent["parameters"].toString(),
                                                   objectContent["special"].toInt(), objectContent["no_wait"].toBool());

            item->setCheckState( objectContent["enabled"].toBool() ? Qt::Checked : Qt::Unchecked);
            items.push_back(item);
        }
        m_Configurations.insert(key,items);

    }

    emit pConfigurationComboBox->currentTextChanged(pConfigurationComboBox->currentText());
}

void CMainWindow::SaveConfig() {

    QJsonDocument file;
    QJsonObject fileObject;

    QJsonObject configObject;
    for(const auto& key : m_Configurations.keys())
    {
        QJsonArray sequences;

        for(int j = 0; j < m_Configurations[key].count(); j++)
        {
            auto command = m_Configurations[key][j];

            QJsonObject commandObject;
            commandObject.insert("enabled", command->checkState() == Qt::Checked);
            commandObject.insert("command", command->getCommand());
            commandObject.insert("parameters",command->getArguments());
            commandObject.insert("special",command->getSpecial());
            commandObject.insert("ensured", false);
            commandObject.insert("ensure_file", "");
            commandObject.insert("no_wait", command->getNoWait());
            sequences.push_back(commandObject);
        }

        configObject.insert(key,sequences);
    }

    fileObject.insert("sequences", configObject);

    fileObject.insert("console_text_color",pConsoleOutput->palette().color(QPalette::Text).name(QColor::HexRgb));
    fileObject.insert("console_background_color",pConsoleOutput->palette().color(QPalette::Base).name(QColor::HexRgb));
    fileObject.insert("console_error_color", m_errorColor);
    fileObject.insert("console_warning_color", m_warningColor);

    QJsonObject variables;

    for(const auto& key : m_Variables.keys())
    {
        variables.insert(key, m_Variables[key]);
    }

    fileObject.insert("variables", variables);

    file.setObject(fileObject);
    auto jsonFile = QFile(QDir::currentPath() + "/cconfig.json");
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(file.toJson());
    jsonFile.close();

    hasChanges = false;
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
    if(!this->hasChanges) {
        event->accept();
        return;
    }

    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Save Changes?",
                                                                tr("Do you wanna save your canges before exiting?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        this->SaveConfig();
        event->accept();
    } else if (resBtn == QMessageBox::Cancel) {
        event->ignore();
    } else {
        event->accept();
    }
}

void CMainWindow::runProcessQueue()
{

    if(m_QueueProcesses.isEmpty()) {
        pCompileButton->setDisabled(false);
        pTerminateButton->setDisabled(true);
        m_pSystemTrayIcon->show();
        m_pSystemTrayIcon->showMessage("Process Complete!", "The process has been completed.", style()->standardIcon(QStyle::SP_MessageBoxWarning));
        return;
    }

    auto command = m_QueueProcesses[0];
    m_QueueProcesses.pop_front();

    QRegularExpression reg(R"(\s+(?=([^"]*"[^"]*")*[^"]*$))",QRegularExpression::PatternOptions::enum_type::ExtendedPatternSyntaxOption);

    if(command->getSpecial() != SPECIAL_EXEC)
    {
        runSpecialCases(command->getArguments().split(reg).replaceInStrings("\"",""), command->getSpecial());
        return runProcessQueue();
    }

    m_pCurrentRunningProcess = new QProcess(this);

    QMap<QString, QString> replacements;
    replacements.insert(m_Variables);
    replacements.insert(m_LocalVariables);

    auto commandStr = command->getCommand();
    auto argumentsStr = command->getArguments();

    for(const auto &key : replacements.keys())
    {
        commandStr = commandStr.replace(QRegularExpression(QRegularExpression::escape(key)), replacements[key]);
        argumentsStr = argumentsStr.replace(QRegularExpression(QRegularExpression::escape(key)), replacements[key]);
    }

    qInfo() << argumentsStr;

    m_pCurrentRunningProcess->setProgram(commandStr);
    m_pCurrentRunningProcess->setArguments(argumentsStr.split(reg).replaceInStrings("\"", ""));
    m_pCurrentRunningProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_pCurrentRunningProcess->setWorkingDirectory(m_WorkDirectory);

    connect(m_pCurrentRunningProcess, &QProcess::readyReadStandardOutput, m_pCurrentRunningProcess, [&]{
        pConsoleOutput->putPlainData(m_pCurrentRunningProcess->readAll());
    });

    QElapsedTimer* timer = new QElapsedTimer();
    connect(m_pCurrentRunningProcess, &QProcess::started, m_pCurrentRunningProcess, [timer]{
        qInfo() << "started";
        timer->start();
    });
    connect(m_pCurrentRunningProcess, &QProcess::finished, m_pCurrentRunningProcess, [&, command, timer](int exitCode, QProcess::ExitStatus exitStatus){
        qDebug() << timer->elapsed() / 1000.0f;
        delete timer;

        delete m_pCurrentRunningProcess;
        m_pCurrentRunningProcess = nullptr;

        if(command->getNoWait())
            runProcessQueue();
        else
        {

            if(exitStatus == QProcess::CrashExit)
                return runProcessQueue();

            continueButton->setDisabled(false);

            QPalette pal = continueButton->palette();
            pal.setColor(QPalette::Button, QColor(Qt::green));
            pal.setColor(QPalette::ButtonText, QColor(Qt::black));
            continueButton->setAutoFillBackground(true);
            continueButton->setPalette(pal);
            continueButton->update();
        }
    });

    m_pCurrentRunningProcess->start();

    if(!m_pCurrentRunningProcess->waitForStarted())
    {
        pConsoleOutput->putPlainData(("\n\n WARNING! Unable to run " + commandStr + "\n\n").toLatin1());
        runProcessQueue();
    };

    qDebug("Done!");

}

void CMainWindow::runSpecialCases(const QStringList &arguments, int special)
{
    switch(special)
    {
        case CC_SPECIAL_SET_LOCAL_VARIABLE_DIRECTORY:
        case CC_SPECIAL_SET_LOCAL_VARIABLE_FULL_PATH_FILE:
        case CC_SPECIAL_SET_LOCAL_VARIABLE_FILENAME:
            if (arguments[0].isEmpty())
                return;

            if(m_LocalVariables.contains(arguments[0]))
            {
                if(arguments.length() >= 2 && arguments[1] == "1")
                    return;

                auto result = QMessageBox::question(this, "Local Variable exists",
                                                    "Do you wanna re-set the local variable?");

                if(result == QMessageBox::No)
                    return;

            }

            {
                auto text = special == CC_SPECIAL_SET_LOCAL_VARIABLE_DIRECTORY ? QFileDialog::getExistingDirectory(this) : QFileDialog::getOpenFileName(this);

                if (text.isEmpty())
                    return;

                this->m_LocalVariables[arguments[0]] = (special == CC_SPECIAL_SET_LOCAL_VARIABLE_FILENAME) ? text.split(
                        "/").last() : text;
            }
            return;

        case CC_SPECIAL_CLEAR_LOCAL_VARIABLE:
            if (arguments.isEmpty())
                return;

            for(const auto& arg : arguments)
                m_LocalVariables.remove(arg);

            return;

        case CC_SPECIAL_RENAME_FILE:
            if(arguments.size() != 2)
                return;

            if(!QFileInfo::exists(arguments[0]))
                return;
            {
            auto path = arguments[0].split("/");
            path.pop_back();
            path.push_back(arguments[1]);
            auto joinedPath = path.join("/");

            if(!QFileInfo(arguments[0]).isReadable() || (QFileInfo::exists(joinedPath) && QFileInfo(joinedPath).isWritable()))
                return;

            QFile::rename(arguments[0], joinedPath);

            };
            return;

        case CC_SPECIAL_COPY_FILE:
        case CC_SPECIAL_COPY_FILE_IF_EXISTS:
            // Assuring a copy exists is the job of the assert special
            // keeping track of the IF_EXISTS is just to keep strata
            // config exports happy.
            if(arguments.size() != 2)
                return;

            {

                if (!QFileInfo::exists(arguments[0]))
                    return;

                auto pathFile = arguments[1].split("/");
                if(pathFile.last().isEmpty())
                    pathFile.pop_back();

                if (QFileInfo(pathFile.join("/")).isDir())
                {
                    auto splitFileName = arguments[0].split("/").last();
                    pathFile.push_back(splitFileName);
                }

                auto joinedPathFile = pathFile.join("/");

                if (!QFileInfo(arguments[0]).isReadable() ||
                    (QFileInfo::exists(joinedPathFile) && QFileInfo(joinedPathFile).isWritable()))
                    return;

                QFile::copy(arguments[0], joinedPathFile);

            }
            return;

        case CC_SPECIAL_CHANGE_DIRECTORY:

            if (!QFileInfo::exists(arguments[0]) || !QFileInfo(arguments[0]).isDir())
                return;

            m_WorkDirectory = arguments[0];

            return;

        case CC_SPECIAL_DELETE_FILE:
            if(!QFileInfo::exists(arguments[0]) || !QFileInfo(arguments[0]).isWritable())
                return;

            QFile::remove(arguments[0]);
            return;

        case CC_SPECIAL_WARNING_ASSERT:
        case CC_SPECIAL_FATAL_ASSERT:
            if(arguments.isEmpty())
                return;

            for(const auto &arg : arguments)
                if(pConsoleOutput->toPlainText().contains(arg))
                {
                    pConsoleOutput->putHTMLData("<br><p style=\"color:" + (special == CC_SPECIAL_FATAL_ASSERT ? m_errorColor : m_warningColor).toLatin1() + "\">" + ("ASSERT FAILED! " + arg + " Found!").toLatin1() +"</p><br>");
                    if(special == CC_SPECIAL_FATAL_ASSERT)
                        this->m_QueueProcesses.clear();
                }

            return;

        case CC_SPECIAL_CLEAR_CONSOLE:
            pConsoleOutput->clear();

        default:
            return;
    }

}

void CMainWindow::TerminateCurrentProcess() {

    if(!m_pCurrentRunningProcess)
        return;

    m_QueueProcesses.clear();
    m_pCurrentRunningProcess->terminate();
    pTerminateButton->setDisabled(true);

}

void CMainWindow::ChangeConsoleTextColor()
{

    auto colorPicker = new QColorDialog(this);

    pConsoleOutput->clear();

    pConsoleOutput->putPlainData("This is a test!");

    colorPicker->setCurrentColor(pConsoleOutput->palette().color(QPalette::Text));

    connect(colorPicker, &QColorDialog::currentColorChanged, colorPicker, [&](const QColor &color){
        auto palette = pConsoleOutput->palette();
        palette.setColor(QPalette::Text, color);
        pConsoleOutput->setPalette(palette);
    });

    connect(colorPicker, &QColorDialog::accepted, colorPicker, [&]{
        this->hasChanges = true;
    });

    connect(colorPicker, &QColorDialog::finished, colorPicker, [&, colorPicker]{
        pConsoleOutput->clear();
        delete colorPicker;
    });

    colorPicker->open();
}

void CMainWindow::ChangeCOnsoleBackgroundColor()
{
        auto colorPicker = new QColorDialog(this);

        pConsoleOutput->clear();

        pConsoleOutput->putPlainData("This is a test!");

        colorPicker->setCurrentColor(pConsoleOutput->palette().color(QPalette::Base));

        connect(colorPicker, &QColorDialog::currentColorChanged, this, [&](const QColor &color){
            auto palette = pConsoleOutput->palette();
            palette.setColor(QPalette::Base, color);
            pConsoleOutput->setPalette(palette);
        });

        connect(colorPicker, &QColorDialog::accepted, colorPicker, [&]{
            this->hasChanges = true;
        });

        connect(colorPicker, &QColorDialog::finished, colorPicker, [&, colorPicker]{
            pConsoleOutput->clear();
            delete colorPicker;
        });

        colorPicker->open();
}

void CMainWindow::ChangeSpecialComboBox() {

    auto processItem = dynamic_cast<CProcessListWidgetItem*>(pRunCommandsList->currentItem());
    if(!processItem)
        return;
    processItem->changeSpecial(pSpecialComboBox->currentData().toInt());
    pRunCommandsList->currentItemChanged(pRunCommandsList->currentItem(), pRunCommandsList->currentItem());
    this->hasChanges = true;

}

void CMainWindow::RunConfigurationChanged()
{
    auto processItem = dynamic_cast<CProcessListWidgetItem*>(pRunCommandsList->currentItem());
    if(!processItem)
        return;
    pCommandLineEdit->setText(processItem->getCommand());
    pParameterTextEdit->setText(processItem->getArguments());
    pContinueAfterProcessCheckBox->setChecked(!processItem->getNoWait());
    int data = pSpecialComboBox->findData(processItem->getSpecial());
    pSpecialComboBox->setCurrentIndex(data);
    pCommandLineEdit->setDisabled(data != SPECIAL_EXEC);
}

void CMainWindow::RowsReordered()
{
    QVector<CProcessListWidgetItem*> replacementList;
    auto originalList = &m_Configurations[pConfigurationComboBox->currentText()];

    for(int i = 0; i < pRunCommandsList->count(); i++)
    {
        replacementList.push_back(reinterpret_cast<CProcessListWidgetItem*>(pRunCommandsList->item(i)));
    }

    originalList->clear();
    originalList->append(replacementList);

    this->hasChanges = true;
}

void CMainWindow::CommandLineTextChanged(const QString &text)
{
    auto processItem = dynamic_cast<CProcessListWidgetItem*>(pRunCommandsList->currentItem());
    if(!processItem)
        return;

    auto previousTest = processItem->getCommand();

    if(previousTest == text)
        return;

    processItem->changeCommand(text);
    hasChanges = true;
}

void CMainWindow::ParameterTextChanged()
{
        auto processItem = dynamic_cast<CProcessListWidgetItem*>(pRunCommandsList->currentItem());
        if(!processItem)
            return;

        auto text = pParameterTextEdit->toPlainText();
        auto previousText = processItem->getArguments();

        if(previousText == text)
            return;

        processItem->changeArguments(pParameterTextEdit->toPlainText());
        hasChanges = true;
}

void CMainWindow::NoWaitCheckBoxChecked(bool toggled) {

    auto processItem = dynamic_cast<CProcessListWidgetItem*>(pRunCommandsList->currentItem());
    if(!processItem)
        return;

    processItem->changeNoWait(!toggled);
    hasChanges = true;

}

void CMainWindow::EditButtonClicked()
{

    //TODO: Create this as it's own class
    // But make sure to
    auto editDialog = new QDialog(this);
    editDialog->setWindowTitle("Edit Configurations");
    auto editLayout = new QGridLayout(editDialog);

    auto editList = new QListWidget(editDialog);
    editLayout->addWidget(editList,0,0,5, 1);

    auto newEditButton = new QPushButton("New",editDialog);
    editLayout->addWidget(newEditButton,0,1, Qt::AlignAbsolute);

    auto RemoveEditButton = new QPushButton("Remove",editDialog);
    editLayout->addWidget(RemoveEditButton,1,1, Qt::AlignAbsolute);

    auto RenameEditButton = new QPushButton("Rename",editDialog);
    editLayout->addWidget(RenameEditButton,2,1, Qt::AlignAbsolute);

    auto CopyEditButton = new QPushButton("Copy",editDialog);
    editLayout->addWidget(CopyEditButton,3,1, Qt::AlignAbsolute);

    auto CloseEditButton = new QPushButton("Close",editDialog);
    editLayout->addWidget(CloseEditButton,4,1, Qt::AlignBottom);

    for(auto key : m_Configurations.keys())
    {
        editList->addItem(key);
    }

    editDialog->setModal(true);
    editDialog->open();

    connect(newEditButton, &QPushButton::pressed, editDialog, [&, editList, editDialog]{
        auto text = QInputDialog::getText(editDialog, tr("New Configuration"), tr("Configuration Name:"), QLineEdit::Normal);

        if(text.isEmpty())
            return;

        m_Configurations.insert(text,{});
        editList->addItem(text);
        pConfigurationComboBox->addItem(text);
        hasChanges = true;

    });

    connect(RemoveEditButton, &QPushButton::pressed, editDialog, [&, editList]{

        if (!editList->currentItem())
            return;

        m_Configurations.remove(editList->currentItem()->text());
        pConfigurationComboBox->removeItem(pConfigurationComboBox->findText(editList->currentItem()->text()));
        editList->takeItem(editList->row(editList->currentItem()));
        hasChanges = true;
    });

    connect(RenameEditButton, &QPushButton::pressed, editDialog, [&, editDialog, editList] {

        if (!editList->currentItem())
            return;

        auto text = QInputDialog::getText(editDialog, tr("Rename Configuration"), tr("Configuration Name:"), QLineEdit::Normal);

        if(text.isEmpty())
            return;

        auto vec = m_Configurations.value(editList->currentItem()->text());
        m_Configurations.remove(editList->currentItem()->text());
        m_Configurations.insert(text, vec);

        pConfigurationComboBox->setItemText(pConfigurationComboBox->findText(editList->currentItem()->text()), text);
        editList->currentItem()->setText(text);
        hasChanges = true;

    });

    connect(CloseEditButton, &QPushButton::pressed, editDialog, &QDialog::close);

}

void CMainWindow::EditVariables() {

    auto editDialog = new QDialog(this);
    editDialog->setWindowTitle("Edit Configurations");
    auto editLayout = new QGridLayout(editDialog);

    auto editList = new QListWidget(editDialog);
    editLayout->addWidget(editList,0,0,5, 1);

    auto newEditButton = new QPushButton("New",editDialog);
    editLayout->addWidget(newEditButton,0,1, Qt::AlignAbsolute);

    auto RemoveEditButton = new QPushButton("Remove",editDialog);
    editLayout->addWidget(RemoveEditButton,1,1, Qt::AlignAbsolute);

    auto EditButton = new QPushButton("Edit", editDialog);
    editLayout->addWidget(EditButton, 2, 1, Qt::AlignAbsolute);

    auto CloseEditButton = new QPushButton("Close",editDialog);
    editLayout->addWidget(CloseEditButton,4,1, Qt::AlignBottom);

    for(const auto &key : m_Variables.keys())
    {
        auto item = new CProcessListWidgetItem(key, m_Variables[key], 0, true);
        item->setData(Qt::CheckStateRole, QVariant());
        editList->addItem( item );

    }

    editDialog->setModal(true);
    editDialog->open();

    connect(newEditButton, &QPushButton::pressed, editDialog, [&, editList, editDialog]{

        auto dialog = new VariableInputDialog(editDialog, 2);

        if(dialog->exec() != QDialog::Accepted)
            return;

        auto list = dialog->getResultList();

        if(list[0].isEmpty() || list[1].isEmpty())
            return;

        this->m_Variables.insert(list[0], list[1]);
        editList->addItem(new CProcessListWidgetItem(list[0], list[1], 0, true));

        hasChanges = true;

    });

    connect(RemoveEditButton, &QPushButton::pressed, editDialog, [&, editList]{

        if (!editList->currentItem())
            return;

        auto key = reinterpret_cast<CProcessListWidgetItem*>(editList->currentItem())->getCommand();

        m_Variables.remove(key);
        delete editList->takeItem(editList->row(editList->currentItem()));
        hasChanges = true;
    });

    connect(EditButton, &QPushButton::pressed, editDialog, [&, editDialog, editList] {

        if(!editList->currentItem())
            return;

        auto item = reinterpret_cast<CProcessListWidgetItem*>(editList->currentItem());

        auto dialog = new VariableInputDialog(editDialog, 2);

        dialog->fields[0]->setText(item->getCommand());
        dialog->fields[1]->setText(item->getArguments());

        if(dialog->exec() != QDialog::Accepted)
            return;

        auto list = dialog->getResultList();

        if(list[0].isEmpty() || list[1].isEmpty())
            return;

        m_Variables.remove(item->getCommand());
        m_Variables.insert(list[0], list[1]);

        item->changeCommand(list[0]);
        item->changeArguments(list[1]);

        hasChanges = true;

    });

    connect(CloseEditButton, &QPushButton::pressed, editDialog, &QDialog::close);

}

void CMainWindow::AddNewCommandListItem() {

        if(pConfigurationComboBox->count() == 0)
            return;

        if(!pRunCommandsList->currentItem())
        {
            auto item = new CProcessListWidgetItem(pCommandLineEdit->text(), pParameterTextEdit->toPlainText(), 0, true,
                                                   pRunCommandsList);
            pRunCommandsList->addItem(item);
            pRunCommandsList->setCurrentItem(item);
            this->m_Configurations[pConfigurationComboBox->currentText()].push_back(item);
        }
        else
        {
            auto item = new CProcessListWidgetItem("", "", 0, true, pRunCommandsList);
            pRunCommandsList->addItem(item);
            pRunCommandsList->setCurrentItem(item);
            this->m_Configurations[pConfigurationComboBox->currentText()].push_back(item);
        }
        this->hasChanges = true;
}

void CMainWindow::RemoveCommandListItem()
{
    if(!pRunCommandsList->currentItem())
        return;

    Q_UNUSED(pRunCommandsList->takeItem(pRunCommandsList->currentRow()));

    if(pRunCommandsList->count() != 0)
        return;

    pParameterTextEdit->clear();
    pCommandLineEdit->clear();

    this->hasChanges = true;
}

void CMainWindow::ChangeConfigurations(const QString &s) {

    for(int i = pRunCommandsList->count(); i > 0; i--)
        pRunCommandsList->takeItem(i - 1);

    if(this->m_Configurations.find(s) == this->m_Configurations.end())
        return;

    for(auto item : this->m_Configurations[s])
    {
        pRunCommandsList->addItem(item);
    }

}

void CMainWindow::LoadConfigurationFromFile() {

        auto configurationPath = QFileDialog::getOpenFileName(this, tr("Load Config"), "", CONFIG_SAVE_FILTER, nullptr, QFileDialog::DontUseNativeDialog);
        if (configurationPath.isEmpty()) {
            return;
        }

        auto file = QFile(configurationPath);
        file.open(QFile::ReadOnly);

        auto fileContents = file.readAll();
        file.close();

        QJsonDocument jsonConfiguration;
        QJsonObject jsonConfigObject;

        if(!configurationPath.endsWith(".json")) {

            if (CConfigHandler::IsBinaryWCConfig(fileContents)) {
                jsonConfiguration = CConfigHandler::ConvertBinaryToJSON(fileContents);
            } else {
                jsonConfiguration = CConfigHandler::ConvertTextToJSON(
                        fileContents);
            }

        } else {

            jsonConfiguration = QJsonDocument::fromJson(fileContents);
        }

        fillConfigurations(jsonConfiguration);

        hasChanges = true;
}

void CMainWindow::CompileButtonPressed() {

        if(pConfigurationComboBox->count() == 0)
            return;

        pConsoleOutput->clear();
        m_WorkDirectory = QDir::currentPath();

        for(int i = 0; i < pRunCommandsList->count(); i++) {
            auto item = reinterpret_cast<CProcessListWidgetItem*>(pRunCommandsList->item(i));
            if(item->checkState() == Qt::Checked)
                m_QueueProcesses.push_back(item);
        }

        pCompileButton->setDisabled(true);
        pTerminateButton->setDisabled(false);

        runProcessQueue();
}

void CMainWindow::OnLockProcessesPressed(bool checked) {

    pRunCommandsList->setDisabled(checked);
    pProcessGroupBox->setDisabled(checked);
    pNewItemButton->setDisabled(checked);
    pRemoveItemButton->setDisabled(checked);
    pSaveConfigButton->setDisabled(checked);
    pEditButton->setDisabled(checked);
    pLockSelection->setIcon(checked ? QIcon(":resource/lock_clicked_icon.svg") : QIcon(":resource/lock_icon.svg"));

}
