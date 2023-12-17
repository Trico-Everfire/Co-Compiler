#pragma once

#include <cstddef>
#include <vector>

class QByteArray;
class QJsonDocument;

class CConfigHandler {

#pragma pack(1)
    struct CSCommand
    {
        bool csIsEnabled;
        char padding[3];
        int csSpecial;
        char csExecutable[260];
        char csArguments[260];
        bool csIsLongFilename;
        char padding2[3];
        bool csEnsureCheck;
        char padding3[3];
        char csEnsureFile[260];
        int csUseProcessWindow;
        bool csNoWait;
        char padding4[3];
    };

    struct CSSequence
    {
        char csName[128];
        unsigned int csCommandCount;
    };

    struct CSHeader
    {
        char csSignature[31];
        float csVersion;
        unsigned int csSequenceCount;
    };
#pragma pack()

public:
    static bool IsBinaryWCConfig(const char* data);

    static QJsonDocument ConvertBinaryToJSON(const char* data);

    static QJsonDocument ConvertTextToJSON(const char* data);

    QByteArray ExportToWCConfig(const QJsonDocument &object, bool kvConfig);
};



