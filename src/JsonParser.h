#pragma once
#ifndef _JSONPARSER_H
#define _JSONPARSER_H

#include "Homonumeric.h"
#include "Lexer.h"
#include <vector>

namespace Json {
    template <typename Tree_Type>
    class ITreeFactory {
        public:
            typedef std::unique_ptr<Tree_Type>
            ptr_t;

            virtual ~ITreeFactory() = default;

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

            virtual ptr_t NewNumeric(
                Homonumeric
            ) = 0;
    };

    class Parsing {
        public:
            static int Escape(Lexer &);
            static int NextToken(Lexer &);
    };

    template <typename Tree_Type>
    class Parser {
        public:
            typedef typename std::unique_ptr<Tree_Type>
            tree_t;

            typedef typename ITreeFactory<Tree_Type>::ptr_t
            ptr_t;
        private:
            std::unique_ptr<ITreeFactory<Tree_Type>>
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
                std::unique_ptr<ITreeFactory<Tree_Type>> factory,
                Lexer * lexer
            ):  _factory(std::move(factory)),
                _lexer(lexer),
                _token(0) {}

            ptr_t GetTree();

            static ptr_t Tree(
                std::unique_ptr<ITreeFactory<Tree_Type>> factory,
                Lexer * lexer
            );

            template <typename Factory_Type>
            static ptr_t Tree(
                Lexer * lexer
            );

            template <template <class> class Factory_Type>
            static ptr_t Tree(
                Lexer * lexer
            );
    };
};

template <typename T>
int
Json::Parser<T>::NextToken() {
    return _token = Json::Parsing::NextToken(*_lexer);
}

template <typename T>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::GetTree() {
    NextToken();

    if (_token != '{')
        return Error();

    return ParseObject();
}

template <typename T>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::ParseObject() {
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
        _factory->NewObject(
            std::move(keys),
            std::move(values)
        )
    );
}

template <typename T>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::ParseList() {
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

    return std::move(
        _factory->NewList(std::move(values))
    );
}

template <typename T>
void
Json::Parser<T>::ParsePair(
    std::string & key,
    Json::Parser<T>::ptr_t & value
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

template <typename T>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::ParseValue() {
    NextToken();

    if (_token == '[')
        return ParseList();

    if (_token == '{')
        return ParseObject();

    return ParsePrimitive();
}

template <typename T>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::ParsePrimitive() {
    switch ((Token)_token) {
        case Token::WORD:
            return ParseKeyword();
        case Token::STRING:
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

template <typename T>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::ParseNumber(bool negative) {
    int factor = negative ? -1 : 1;

    switch ((Token)_token) {
        case Token::INTEGER:
            return std::move(
                _factory->NewNumeric(
                    Homonumeric::Integer(factor * _lexer->Integer())
                )
            );
        case Token::FLOAT:
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

template <typename T>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::ParseKeyword() {
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

    return std::move(
        _factory->NewNumeric(
            Homonumeric::Boolean((bool)(value - 1))
        )
    );
}

template <typename T>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::Error() {
    // TODO: Log error
    return nullptr;
}

template <typename T>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::Tree(
    std::unique_ptr<ITreeFactory<T>> factory,
    Lexer * lexer
) {
    return Parser(factory, lexer).GetTree();
}

template <typename T>
template <typename F>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::Tree(
    Lexer * lexer
) {
    return Parser<T>(
        std::make_unique<F>(),
        lexer
    )
    .GetTree();
}

template <typename T>
template <template <class> class F>
typename Json::Parser<T>::ptr_t
Json::Parser<T>::Tree(
    Lexer * lexer
) {
    return Parser<T>(
        std::make_unique<F<T>>(),
        lexer
    )
    .GetTree();
}

#endif
