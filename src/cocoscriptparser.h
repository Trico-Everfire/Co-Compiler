

#include <QString>
#include <QVector>
#include <QMap>
#include <any>
#include "enums.h"

class CCocoScript {

    class CCocoTokenizer
    {
    public:
        struct Range
        {
            int start;
            int end;
        };

        struct Token
        {
            TokenType type;
            Range range;
            std::string_view string;
            int line;
            ParseError associatedError;
        };

    public:
        QVector<Token> tokenList;
        ParseError tokenizerStatus = ParseError::UNINITIALISED;
    public:
        CCocoTokenizer();
        bool TokenizeFile( const QString &path );
        bool TokenizeFile( const QByteArray &fileContents );
    };

    class CCocoParser {

        struct CCocoFunction;

        struct CCocoScope
        {
            QMap<QString, QString> variables;
            QMap<QString, QVector<std::function<std::any>>> functions;
        };

        struct CCocoFunction
        {
            QVector<QString> parameters;
        };

//        QMap<QString, CCocoFunction> globalFunctions;
//        CCocoScope* masterScope;

        CCocoParser(const QVector<CCocoTokenizer::Token> & tokenList);

    };

};

