//
// Created by trico on 27-11-23.
//

#include "cocoscriptparser.h"
#include <QStringView>
#include <QMap>
#include <iomanip>
#include <QStack>

//char singleTokens[] = "{}[]()=;,";

static const QMap<char, QPair<TokenType, ParseError>> singleTokens{
    {'{', {TokenType::OPEN_BRACE, ParseError::INVALID_OPEN_BRACE}},
    {'}', {TokenType::CLOSE_BRACE, ParseError::INVALID_CLOSE_BRACE}},
    {'[', {TokenType::OPEN_BRACKET, ParseError::INVALID_OPEN_BRACKET}},
    {']', {TokenType::CLOSE_BRACKET, ParseError::INVALID_CLOSE_BRACKET}},
    {'(', {TokenType::OPEN_PARENTHESIS, ParseError::INVALID_OPEN_PARENTHESIS}},
    {')', {TokenType::CLOSE_PARENTHESIS, ParseError::INVALID_CLOSE_PARENTHESIS}},
    {'=', {TokenType::EQUALS, ParseError::INVALID_EQUALS}},
    {';', {TokenType::SEMI_COLUMN, ParseError::INVALID_SEMI_COLUMN}},
    {',', {TokenType::COMMA, ParseError::INVALID_COMMA}}
};

//bool CCocoScript::CCocoTokenizer::TokenizeFile(const QString &path) {
//    return false;
//}

bool CCocoScript::CCocoTokenizer::TokenizeFile(const QByteArray &fileContents) {
    this->tokenizerStatus = ParseError::NO_ERROR;

    int ln = 1;

    for ( auto ch = fileContents.constBegin(); ch != fileContents.constEnd(); ch++ )
    {
        char c = *(ch);

        if ( c == '\t' )
            continue;

        if ( c == '\r' )
            continue;

        if ( c == '\n' )
        {
            ln++;
            continue;
        }

        if(c == '/' && ((*(std::next(ch)) == '/') || *(std::next(ch)) == '*'))
        {

                auto current = ch;

                auto cmpStr = std::string_view(current, fileContents.constEnd());

                if(cmpStr.contains("//") || cmpStr.contains("/*") )
                {
                    this->tokenizerStatus = ParseError::INVALID_COMMENT;
                    return false;
                }

                while(*ch != '*')
                    ch++;
                ch+=2;

            Token token;
            token.line = ln;

            token.type = cmpStr.contains("/*") ? TokenType::ENDLINE_COMMENT : TokenType::STANDALONE_COMMENT;

            token.associatedError = ParseError::INVALID_COMMENT;

            token.string = std::string_view(current, ch);;

            token.range = {static_cast<int>(fileContents.indexOf(current)), static_cast<int>(fileContents.indexOf(ch))};

            this->tokenList.push_back(token);
            continue;

        }

        if(c == '"')
        {
            auto current = ch;
            c = '\0';

            while(c != '"')
                c = *ch++;

            Token token;
            token.line = ln;

            token.type = TokenType::STRING;

            token.associatedError = ParseError::INVALID_STRING;

            token.string = std::string_view(current, ch);;

            token.range = {static_cast<int>(fileContents.indexOf(current)), static_cast<int>(fileContents.indexOf(ch))};

            this->tokenList.push_back(token);
            continue;
        }

        if(singleTokens.contains(c))
        {
            auto tokenInfo = singleTokens[c];

            Token token;
            token.line = ln;
            token.type = tokenInfo.first;
            token.associatedError = tokenInfo.second;
            token.string = std::string_view(ch, ch+1);
            int position = static_cast<int>(fileContents.indexOf(ch));
            token.range = {position, position + 1};
            continue;
        }

        if(c != ' ')
        {
            auto current = ch;
            while ( c != '\n' && c != ' ' && c != '\t' && c != '\r' && !singleTokens.contains(c) )
                c = *(ch++);
            Token token;
            token.line = ln;

            token.type = TokenType::STRING;

            token.associatedError = ParseError::INVALID_STRING;

            token.string = std::string_view(current, ch);;

            token.range = {static_cast<int>(fileContents.indexOf(current)), static_cast<int>(fileContents.indexOf(ch))};

            this->tokenList.push_back(token);
            continue;
        }
    }

    return true;
}

CCocoScript::CCocoParser::CCocoParser(const QVector<CCocoScript::CCocoTokenizer::Token> &tokenList)
{

    typedef QString (*cocoFunction)();

    QStack<QMap<std::string_view, cocoFunction>> stack{{}};


    //TODO: Implement functions and such.

    for(auto it = tokenList.begin(); it != tokenList.end(); it++)
    {
        if(it->type == TokenType::LITERAL)
        {
            if(it->string == "fn")
            {
                CCocoFunction function;

                it++;

                if(it->type != TokenType::LITERAL) {
                    return;
                }

                std::string_view name = it->string;

                it++;

                if(it->type != TokenType::OPEN_PARENTHESIS) {
                    return;
                }

                it++;

                while( it->type != TokenType::CLOSE_PARENTHESIS)
                {
                    if(it->type != TokenType::LITERAL) {
                        return;
                    }

                    

                }

            }


            bool found = false;

            for(auto &scope : stack)
            {
                if(scope.contains(it->string))
                {

                    it++;
                    
                    if(it->type != TokenType::EQUALS)
                    {
                        return;
                    }

                    it++;
                    
                    if(it->type == TokenType::STRING || it->type == TokenType::LITERAL)
                    {
                        
                    }
                    
                    found = true;
                    
                }
            }

            if(!found)
            {
                return;
            }

        }
    }
}
