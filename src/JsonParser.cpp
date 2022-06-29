#include "JsonParser.h"

int JsonParser::Escape(Lexer & lexer) {
    if (!lexer.NextChar())
        return Token::ERROR;

    char character = lexer.CurrentChar();

    if (character != '\\')
        return character;

    if (!lexer.NextChar())
        return Token::ERROR;

    switch (lexer.CurrentChar()) {
        case 'a': return '\a';
        case 'b': return '\b';
        case 'e': return '\e';
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case 'v': return '\v';
        case '\\': return '\\';
        case '\'': return '\'';
        case '"': return '\"';
        case '?': return '\?';
        default: return Token::ERROR;
    }
}

int JsonParser::NextToken(Lexer & lexer) {
    if (!lexer.IgnoreWhiteSpace())
        return Token::END;

    char currentChar = lexer.CurrentChar();

    if (isalpha(currentChar) || currentChar == '_')
        return lexer.LexWord();

    if (isdigit(currentChar))
        return lexer.LexNumber();

    if (currentChar == '"') {
        lexer.NextChar();
        return lexer.LexString('"', Escape);
    }

    if (currentChar == '\'') {
        lexer.NextChar();
        return lexer.LexCharacter('\'', Escape);
    }

    int temp = (int)currentChar;
    lexer.NextChar();
    return temp;
}

int JsonParser::NextToken() {
    return _token = JsonParser::NextToken(*_lexer);
}

jsonptr_t JsonParser::GetTree() {
    NextToken();

    if (_token != '{')
        return Error();

    return ParseObject();
}

jsonptr_t JsonParser::ParseObject() {
    // TODO: Consider adding a kill condition at the
    //   start of each Parse function.
    auto pairs = std::vector<jsonptr_t>();

    do {
        auto pair = ParsePair();

        if (_token == Token::ERROR)
            return Error();

        pairs.push_back(std::move(pair));
        NextToken();
    }
    while (_token == ',');

    if (_token != '}')
        return Error();

    return std::move(std::make_unique<JsonObject>(std::move(pairs)));
}

jsonptr_t JsonParser::ParseList() {
    auto values = std::vector<jsonptr_t>();

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

    return std::move(std::make_unique<JsonList>(std::move(values)));
}

jsonptr_t JsonParser::ParsePair() {
    NextToken();

    if ((Token)_token != Token::STRING)
        return Error();

    std::string key = _lexer->String();
    NextToken();

    if ((char)_token != ':')
        return Error();

    return std::move(
        std::make_unique<JsonPair>(
            key,
            std::move(ParseValue())
        )
    );
}

jsonptr_t JsonParser::ParseValue() {
    NextToken();

    if (_token == '[')
        return ParseList();

    if (_token == '{')
        return ParseObject();

    return ParsePrimitive();
}

jsonptr_t JsonParser::ParsePrimitive() {
    switch ((Token)_token) {
        case Token::WORD:
            return ParseKeyword();
        case Token::STRING:
            return std::move(std::make_unique<JsonString>(_lexer->String()));
        default:
            if ((char)_token == '-') {
                NextToken();
                return ParseNumber(true);
            }
    }

    return ParseNumber(false);
}

jsonptr_t JsonParser::ParseNumber(bool negative) {
    int factor = negative ? -1 : 1;

    switch ((Token)_token) {
        case Token::INTEGER:
            return std::move(std::make_unique<JsonNumeric>(
                Homonumeric::Integer(factor * _lexer->Integer())
            ));
        case Token::FLOAT:
            return std::move(std::make_unique<JsonNumeric>(
                Homonumeric::Float(factor * _lexer->Float())
            ));
        default:
            break;
    }

    return Error();
}

jsonptr_t JsonParser::ParseKeyword() {
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

    return std::move(std::make_unique<JsonNumeric>(
        Homonumeric::Boolean((bool)(value - 1))
    ));
}

jsonptr_t JsonParser::Error() {
    // TODO: Log error
    return nullptr;
}

jsonptr_t JsonParser::Tree(Lexer * lexer) {
    return JsonParser(lexer).GetTree();
}