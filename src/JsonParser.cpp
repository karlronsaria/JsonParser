#include "JsonParser.h"

int Json::Parser::Escape(Lexer & lexer) {
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

int Json::Parser::NextToken(Lexer & lexer) {
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

int Json::Parser::NextToken() {
    return _token = Json::Parser::NextToken(*_lexer);
}

Json::tree_t Json::Parser::GetTree() {
    NextToken();

    if (_token != '{')
        return Error();

    return ParseObject();
}

Json::tree_t Json::Parser::ParseObject() {
    // TODO: Consider adding a kill condition at the
    //   start of each Parse function.
    std::vector<std::string> keys;
    std::vector<Json::tree_t> values;
    std::string key;
    Json::tree_t value;

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
        std::make_unique<Object>(
            std::move(keys),
            std::move(values)
        )
    );
}

Json::tree_t Json::Parser::ParseList() {
    auto values = std::vector<Json::tree_t>();

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

    return std::move(std::make_unique<List>(std::move(values)));
}

void Json::Parser::ParsePair(
    std::string & key,
    Json::tree_t & value
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

Json::tree_t Json::Parser::ParseValue() {
    NextToken();

    if (_token == '[')
        return ParseList();

    if (_token == '{')
        return ParseObject();

    return ParsePrimitive();
}

Json::tree_t Json::Parser::ParsePrimitive() {
    switch ((Token)_token) {
        case Token::WORD:
            return ParseKeyword();
        case Token::STRING:
            return std::move(std::make_unique<String>(_lexer->String()));
        default:
            if ((char)_token == '-') {
                NextToken();
                return ParseNumber(true);
            }
    }

    return ParseNumber(false);
}

Json::tree_t Json::Parser::ParseNumber(bool negative) {
    int factor = negative ? -1 : 1;

    switch ((Token)_token) {
        case Token::INTEGER:
            return std::move(std::make_unique<Numeric>(
                Homonumeric::Integer(factor * _lexer->Integer())
            ));
        case Token::FLOAT:
            return std::move(std::make_unique<Numeric>(
                Homonumeric::Float(factor * _lexer->Float())
            ));
        default:
            break;
    }

    return Error();
}

Json::tree_t Json::Parser::ParseKeyword() {
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

    return std::move(std::make_unique<Numeric>(
        Homonumeric::Boolean((bool)(value - 1))
    ));
}

Json::tree_t Json::Parser::Error() {
    // TODO: Log error
    return nullptr;
}

Json::tree_t Json::Parser::Tree(Lexer * lexer) {
    return Parser(lexer).GetTree();
}