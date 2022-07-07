#pragma once
#ifndef _JSONPARSER_H
#define _JSONPARSER_H

#include "Lexer.h"
#include "JsonTree.h"

namespace Json {
    class Parser {
        public:
            static int Escape(Lexer &);
            static int NextToken(Lexer &);
        private:
            Lexer * _lexer;
            int _token;
        protected:
            int NextToken();
            tree_t ParseObject();
            tree_t ParseList();
            void ParsePair(std::string &, tree_t &);
            tree_t ParseValue();
            tree_t ParsePrimitive();
            tree_t ParseNumber(bool);
            tree_t ParseKeyword();
            tree_t Error();
        public:
            Parser(
                Lexer * lexer
            ):  _lexer(lexer),
                _token(0) {}

            tree_t GetTree();
            static tree_t Tree(Lexer * lexer);
    };
};

#endif
