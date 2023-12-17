#include <type_traits>
#include "confighandler.h"
#include "KeyValue.h"
#include "enums.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

bool CConfigHandler::IsBinaryWCConfig(const char* data) {
    auto header = reinterpret_cast<const CSHeader*>(data);

    return std::string(header->csSignature, 31).contains("Worldcraft Command Sequences\r\n\x1a");
}

int SourceSpecialToCocompilerSpecial(int special)
{
    switch (special) {
        case SRC_SPECIAL_CHANGE_DIRECTORY:
            return CC_SPECIAL_CHANGE_DIRECTORY;
        case SRC_SPECIAL_COPY_FILE:
            return CC_SPECIAL_COPY_FILE;
        case SRC_SPECIAL_DELETE_FILE:
            return CC_SPECIAL_DELETE_FILE;
        case SRC_SPECIAL_RENAME_FILE:
            return CC_SPECIAL_RENAME_FILE;
        case SRC_SPECIAL_GENERATE_GRID_NAV:
            return CC_SPECIAL_GENERATE_GRID_NAV;
        case SRC_SPECIAL_COPY_FILE_IF_EXISTS: //strata specific.
            return CC_SPECIAL_COPY_FILE_IF_EXISTS;
        default:
            return SPECIAL_EXEC;
    }
}


int CocompilerSpecialToSourceSpecial(int special)
{
    switch (special) {
        case CC_SPECIAL_CHANGE_DIRECTORY:
            return SRC_SPECIAL_CHANGE_DIRECTORY;
        case SRC_SPECIAL_COPY_FILE:
            return CC_SPECIAL_COPY_FILE;
        case CC_SPECIAL_DELETE_FILE:
            return SRC_SPECIAL_DELETE_FILE;
        case CC_SPECIAL_RENAME_FILE:
            return SRC_SPECIAL_RENAME_FILE;
        case CC_SPECIAL_GENERATE_GRID_NAV:
            return SRC_SPECIAL_GENERATE_GRID_NAV;
        case CC_SPECIAL_COPY_FILE_IF_EXISTS:   //Strata specific.
            return SRC_SPECIAL_COPY_FILE_IF_EXISTS;
        default:
            return SPECIAL_EXEC;
    }
}


QJsonDocument CConfigHandler::ConvertBinaryToJSON(const char* data) {

    QJsonDocument document;
    QJsonObject configuration;

    auto header = reinterpret_cast<const CSHeader*>(data);
    data += sizeof(CSHeader);

    for(int i = 0; i < header->csSequenceCount; i++)
    {
        auto sequence = reinterpret_cast<const CSSequence *>(data);

        data += sizeof(CSSequence);

        QJsonArray commands;
        for(int j = 0; j < sequence->csCommandCount; j++)
        {
            QJsonObject jsonCommand;
            auto command = reinterpret_cast<const CSCommand*>(data);
            jsonCommand.insert("enabled", command->csIsEnabled);
            jsonCommand.insert("command", command->csExecutable);
            jsonCommand.insert("parameters", command->csArguments);
            jsonCommand.insert("ensure",command->csEnsureCheck);
            jsonCommand.insert("special", SourceSpecialToCocompilerSpecial(command->csSpecial));
            jsonCommand.insert("ensured", command->csEnsureCheck);
            jsonCommand.insert("ensure_file", command->csEnsureFile);
            jsonCommand.insert("no_wait", command->csNoWait);

            commands.push_back(jsonCommand);
            data += sizeof (CSCommand);
        }

        configuration.insert(sequence->csName, commands);

    }

    document.setObject(configuration);

    return document;
}

QJsonDocument CConfigHandler::ConvertTextToJSON(const char* data)
{
    auto config = KeyValueRoot();
    auto err = config.Parse(reinterpret_cast<const char *>(data));

    if(err != KeyValueErrorCode::NO_ERROR)
        return {};

    QJsonDocument jsonConfiguration{};
    QJsonObject jsonConfigObject{};

    if(auto &sequences = config.Get("Command Sequences"); sequences.IsValid())
    {
        for(int i = 0; i < sequences.ChildCount(); i++)
        {
            auto &sequence = sequences.Get(i);

            QJsonArray jsonSequenceArray{};
            for(int j = 0; j < sequence.ChildCount(); j++)
            {
                QJsonObject jsonSequenceObject{};

                auto &configParams = sequence.Get(j);

                jsonSequenceObject.insert("enabled", configParams.Get("enabled").Value().string[0] == '1');
                jsonSequenceObject.insert("command", configParams.Get("run").Value().string);
                jsonSequenceObject.insert("parameters",configParams.Get("params").Value().string);
                jsonSequenceObject.insert("ensured", configParams.Get("ensure_check").Value().string[0] == '1');
                jsonSequenceObject.insert("special", SourceSpecialToCocompilerSpecial(std::stoi(configParams.Get("special_cmd").Value().string)));
                jsonSequenceObject.insert("ensure_file", configParams.Get("ensure_fn").Value().string);
                jsonSequenceObject.insert("no_wait", false); //Strata does away with it, I do not, so false by default it is.
                jsonSequenceArray.push_back(jsonSequenceObject);
            }

            jsonConfigObject.insert(sequence.Key().string, jsonSequenceArray);
        }
        jsonConfiguration.setObject(jsonConfigObject);
    }

    return jsonConfiguration;
}

QByteArray CConfigHandler::ExportToWCConfig(const QJsonDocument & object, bool kvConfig)
{
    return {};
}
