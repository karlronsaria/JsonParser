#pragma once
#ifndef _JSONPARSER_H
#define _JSONPARSER_H

#include "Lexer.h"
#include "JsonTree.h"

class JsonParser {
    public:
        static int Escape(Lexer &);
        static int NextToken(Lexer &);
    private:
        Lexer * _lexer;
        int _token;
    protected:
        int NextToken();
        jsonptr_t ParseObject();
        jsonptr_t ParseList();
        jsonptr_t ParsePair();
        jsonptr_t ParseValue();
        jsonptr_t ParsePrimitive();
        jsonptr_t ParseNumber(bool);
        jsonptr_t ParseKeyword();
        jsonptr_t Error();
    public:
        JsonParser(
            Lexer * lexer
        ):  _lexer(lexer),
            _token(0) {}

        jsonptr_t GetTree();
        static jsonptr_t Tree(Lexer * lexer);
};

#endif
