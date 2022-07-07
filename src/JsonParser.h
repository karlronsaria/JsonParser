#pragma once
#ifndef _JSONPARSER_H
#define _JSONPARSER_H

#include "Lexer.h"
#include "JsonTree.h"

namespace Json {
    template <typename TreeT>
    class ITreeFactory {
        public:
            typedef std::unique_ptr<TreeT>
            ptr_t;

            virtual ITreeFactory() = default;

            virtual ptr_t NewObject(
                std::vector<std::string> keys,
                std::vector<ptr_t> values
            ) = 0;

            virtual ptr_t NewList(
                std::vector<ptr_t> values
            ) = 0;

            virtual ptr_t NewString(
                const std::string &
            ) = 0;

            virtual ptr_t Numeric(
                Homonumeric
            ) = 0;
    };

    class Parsing {
        public:
            static int Escape(Lexer &);
            static int NextToken(Lexer &);
    };

    template <class TreeT, template <class> class FactoryT>
    class Parser {
        public:
            typedef ITreeFactory<TreeT>::ptr_t ptr_t;
        private:
            std::unique_ptr<ITreeFactory<TreeT>>
            _factory;

            Lexer * _lexer;
            int _token;
        protected:
            int NextToken();
            ptr_t ParseObject();
            ptr_t ParseList();
            void ParsePair(std::string &, ptr_t &);
            ptr_t ParseValue();
            ptr_t ParsePrimitive();
            ptr_t ParseNumber(bool);
            ptr_t ParseKeyword();
            ptr_t Error();
        public:
            Parser(
                Lexer * lexer
            ):  _factory(std::make_unique<FactoryT<TreeT>>),
                _lexer(lexer),
                _token(0) {}

            ptr_t GetTree();
            static ptr_t Tree(Lexer * lexer);
    };
};

template <class T, template <class> class F>
int
Json::Parser<T, F>::NextToken() {
    return _token = Json::Parsing::NextToken(*_lexer);
}

template <class T, template <class> class F>
Json::Parser<T, F>::ptr_t
Json::Parser<T, F>::GetTree() {
    NextToken();

    if (_token != '{')
        return Error();

    return ParseObject();
}

template <class T, template <class> class F>
Json::Parser<T, F>::ptr_t
Json::Parser<T, F>::ParseObject() {
    // TODO: Consider adding a kill condition at the
    //   start of each Parse function.
    std::vector<std::string> keys;
    std::vector<tree_t> values;
    std::string key;
    tree_t value;

    do {
        ParsePair(key, value);

        if (_token == Token::ERROR)
            return Error();

        keys.push_back(std::move(key));
        values.push_back(std::move(value));
        NextToken();
    }
    while (_token == ',');

    if (_token != '}')
        return Error();

    return std::move(
        // TODO: TREE POINTER 1
        _factory->NewObject(
            std::move(keys),
            std::move(values)
        )
    );
}

template <class T, template <class> class F>
Json::Parser<T, F>::ptr_t
Json::Parser<T, F>::ParseList() {
    auto values = std::vector<tree_t>();

    do {
        auto value = ParseValue();

        if (_token == Token::ERROR)
            return Error();

        values.push_back(std::move(value));
        NextToken();
    }
    while (_token == ',');

    if (_token != ']')
        return Error();

    // TODO: TREE POINTER 2
    return std::move(
        _factory->NewList(std::move(values));
    );
}

template <class T, template <class> class F>
void
Json::Parser<T, F>::ParsePair(
    std::string & key,
    Json::Parser<T, F>::ptr_t & value
) {
    NextToken();

    if ((Token)_token != Token::STRING) {
        value = Error();
        return;
    }

    key = _lexer->String();
    NextToken();

    if ((char)_token != ':') {
        value = Error();
        return;
    }

    value = std::move(ParseValue());
}

template <class T, template <class> class F>
Json::Parser<T, F>::ptr_t
Json::Parser<T, F>::ParseValue() {
    NextToken();

    if (_token == '[')
        return ParseList();

    if (_token == '{')
        return ParseObject();

    return ParsePrimitive();
}

template <class T, template <class> class F>
Json::Parser<T, F>::ptr_t
Json::Parser<T, F>::ParsePrimitive() {
    switch ((Token)_token) {
        case Token::WORD:
            return ParseKeyword();
        case Token::STRING:
            // TODO: TREE POINTER 3
            return std::move(
                _factory->NewString(_lexer->String())
            );
        default:
            if ((char)_token == '-') {
                NextToken();
                return ParseNumber(true);
            }
    }

    return ParseNumber(false);
}

template <class T, template <class> class F>
Json::Parser<T, F>::ptr_t
Json::Parser<T, F>::ParseNumber(bool negative) {
    int factor = negative ? -1 : 1;

    switch ((Token)_token) {
        case Token::INTEGER:
            // TODO: TREE POINTER 4
            return std::move(
                _factory->NewNumeric(
                    Homonumeric::Integer(factor * _lexer->Integer())
                )
            );
        case Token::FLOAT:
            // TODO: TREE POINTER 5
            return std::move(
                _factory->NewNumeric(
                    Homonumeric::Float(factor * _lexer->Float())
                )
            );
        default:
            break;
    }

    return Error();
}

template <class T, template <class> class F>
Json::Parser<T, F>::ptr_t
Json::Parser<T, F>::ParseKeyword() {
    std::string keyword = _lexer->String();

    if (keyword == "null")
        return nullptr;

    int value = 0;

    if (keyword == "false")
        value = 1;

    if (keyword == "true")
        value = 2;

    if (!value)
        return Error();

    // TODO: TREE POINTER 6
    return std::move(
        _factory->NewNumeric(
            Homonumeric::Boolean((bool)(value - 1))
        )
    );
}

template <class T, template <class> class F>
Json::Parser<T, F>::ptr_t
Json::Parser<T, F>::Error() {
    // TODO: Log error
    return nullptr;
}

template <class T, template <class> class F>
Json::Parser<T, F>::ptr_t
Json::Parser<T, F>::Tree(Lexer * lexer) {
    return Parser(lexer).GetTree();
}

#endif