#pragma once

#include <cstddef>
#include <vector>
#include <memory>

class QByteArray;
class QJsonDocument;

class CConfigHandler {
public:
    static bool IsBinaryWCConfig(const char* data);

    static QJsonDocument ConvertBinaryToJSON(const char* data, bool isCSGO);

    static QJsonDocument ConvertTextToJSON(const char* data);

    static std::unique_ptr<QByteArray> ExportToWCConfig(const QJsonDocument &object, bool kvConfig, bool isCSGO);

};



