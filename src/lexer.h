#pragma once
#ifndef _LEXER_H
#define _LEXER_H

#include <string>
#include <map>
#include <math.h>
#include <memory>
#include "IEnumerator.h"

enum Token {
    ERROR = 0,
    END = -1,
    WORD = -2,
    INTEGER = -3,
    FLOAT = -4,
    SPACE = -5,
    STRING = -6,
    CHARACTER = -7
};

class Lexer {
    private:
        std::string _str;
        int _integer;
        float _float;
        char _character;
        char _current_char;
        std::shared_ptr<IEnumerator> _stream;
    protected:
        std::shared_ptr<IEnumerator> Enumerator();
    public:
        Lexer(std::shared_ptr<IEnumerator> &&);

        bool NextChar();
        bool IgnoreWhiteSpace();
        int LexWhiteSpace();
        int LexWord();
        int LexInteger(int & digits);
        int LexInteger();

        int LexString(char delimiter);
        int LexString(
            char delimiter,
            int (*forEachCharacter)(Lexer &)
        );
        int LexCharacter(char delimiter);
        int LexCharacter(
            char delimiter,
            int (*forEachCharacter)(Lexer &)
        );
        int LexNumber();

        int NextToken();
        int Index() const;
        bool HasNext() const;
        char CurrentChar() const;
        const std::string & Str() const;
        int Integer() const;
        float Float() const;
        char Character() const;
};

#endif