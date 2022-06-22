#pragma once
#ifndef _TESTLEXER_H
#define _TESTLEXER_H

#include "../src/StringEnumerator.h"
#include "../src/StreamEnumerator.h"
#include "../src/other.h"
#include "LexerDemo001.h"
#include "LexerDemo002.h"
#include "JsonParserDemo001.h"
#include <fstream>
#include <istream>
#include <sstream>
#include <memory>
#include <vector>

typedef bool (*test_function_ptr)(std::string&, std::string&);

struct Test {
    std::string name;
    test_function_ptr definition;
};

class Tests {
    private:
        static std::vector<Test> _list;
    public:
        static std::string WorkingDirectory;

        static void Init();
        static void Run(std::ostream&);

        static bool TestLexer(
            const std::string & testString,
            const std::vector<std::string> & expectedTokens,
            std::string & actual,
            std::string & expected,
            bool ignoreWhiteSpace
        );

        static bool TestLexerDemo002(
            const std::string & testString,
            const std::vector<std::string> & expectedTokens,
            std::string & actual,
            std::string & expected
        );

        static bool TestJsonParserDemo001(
            const std::string & testFilePath,
            const std::string & expectedFilePath,
            std::string & actual,
            std::string & expected
        );
};

class LexerTestVisitor: public ILexerVisitor {
    public:
        static const char STRING_DELIMITER = '"';
    private:
        std::shared_ptr<Lexer> _lexer;
        std::vector<std::string> _expected_tokens;
        std::string _actual;
        std::string _expected;
        bool _ignore_white_space;
        std::ostringstream _test_out;
        int _index;
    public:
        LexerTestVisitor(
            const std::shared_ptr<Lexer> & lexer,
            const std::vector<std::string> & expectedTokens,
            bool ignoreWhiteSpace
        );

        void ForWhiteSpace(const std::string & whiteSpace) override;
        void ForWord(const std::string & word) override;
        void ForInteger(int number) override;
        int ForPunctuation(char token) override;
        bool ForEachToken(int token) override;
        std::shared_ptr<Lexer> GetLexer() override;

        const std::string & Actual() const;
        const std::string & Expected() const;

        int Index() const;
        int ExpectedMax() const;
};

#endif