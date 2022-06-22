#pragma once
#ifndef _JSONPARSER_H
#define _JSONPARSER_H

#include "lexer.h"

class JsonLexer {
    public:
        static int Escape(Lexer &);
        static int NextToken(Lexer &);
};

class JsonParser;

class IJsonParserVisitor {
    public:
        virtual ~IJsonParserVisitor() = default;
        virtual void Push() = 0;
        virtual void Pop() = 0;
        virtual bool ForJson(Lexer &) = 0;
        virtual bool ForObject(Lexer &) = 0;
        virtual bool ForList(Lexer &) = 0;
        virtual bool ForPair(Lexer &) = 0;
        virtual bool ForValue(Lexer &) = 0;
        virtual bool ForPrimitive(Lexer &) = 0;
        virtual bool ForNumber(Lexer &) = 0;
        virtual bool ForKeyWord(Lexer &) = 0;
        virtual bool ForInteger(Lexer &) = 0;
        virtual bool ForFloat(Lexer &) = 0;
        virtual bool ForString(Lexer &) = 0;
        virtual bool ForError(Lexer &) = 0;
        virtual bool ForKey(Lexer &) = 0;
};

class JsonParser {
    public:
        static int ParseJson(Lexer &, IJsonParserVisitor *);
        static int ParseObject(Lexer &, IJsonParserVisitor *);
        static int ParseList(Lexer &, IJsonParserVisitor *);
        static int ParsePair(Lexer &, IJsonParserVisitor *);
        static int ParseValue(Lexer &, IJsonParserVisitor *);
        static int ParsePrimitive(int, Lexer &, IJsonParserVisitor *);
        static int ParseNumber(int, Lexer &, IJsonParserVisitor *);
        static int ParseKey(Lexer &, IJsonParserVisitor *);

        static int Error(Lexer &, IJsonParserVisitor *);
};

#endif
