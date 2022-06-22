#include "JsonParser.h"

int JsonLexer::Escape(Lexer & lexer) {
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

int JsonLexer::NextToken(Lexer & lexer) {
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

int JsonParser::ParseJson(Lexer & lexer, IJsonParserVisitor * visitor) {
    visitor->ForJson(lexer);
    visitor->Push();

    int token = JsonLexer::NextToken(lexer);

    if (token != '{')
        return Error(lexer, visitor);

    token = ParseObject(lexer, visitor);
    visitor->Pop();
    return token;
}

int JsonParser::ParseObject(Lexer & lexer, IJsonParserVisitor * visitor) {
    visitor->ForObject(lexer);
    visitor->Push();

    if (ParsePair(lexer, visitor) == Token::ERROR)
        return Error(lexer, visitor);

    int token = JsonLexer::NextToken(lexer);

    while (token == ',') {
        if (ParsePair(lexer, visitor) == Token::ERROR)
            return Error(lexer, visitor);

        token = JsonLexer::NextToken(lexer);
    }

    if (token != '}')
        return Error(lexer, visitor);

    visitor->Pop();
    return token;
}

int JsonParser::ParseList(Lexer & lexer, IJsonParserVisitor * visitor) {
    visitor->ForList(lexer);
    visitor->Push();

    if (ParseValue(lexer, visitor) == Token::ERROR)
        return Error(lexer, visitor);

    int token = JsonLexer::NextToken(lexer);

    while (token == ',') {
        if (ParseValue(lexer, visitor) == Token::ERROR)
            return Error(lexer, visitor);

        token = JsonLexer::NextToken(lexer);
    }

    if (token != ']')
        return Error(lexer, visitor);

    visitor->Pop();
    return token;
}

int JsonParser::ParsePair(Lexer & lexer, IJsonParserVisitor * visitor) {
    visitor->ForPair(lexer);
    visitor->Push();

    int token = JsonLexer::NextToken(lexer);

    if (ParseKey(lexer, visitor) == Token::ERROR)
        return Error(lexer, visitor);

    token = JsonLexer::NextToken(lexer);

    if ((char)token != ':')
        return Error(lexer, visitor);

    token = ParseValue(lexer, visitor);
    visitor->Pop();
    return token;
}

int JsonParser::ParseValue(Lexer & lexer, IJsonParserVisitor * visitor) {
    visitor->ForValue(lexer);
    visitor->Push();

    int token = JsonLexer::NextToken(lexer);

    if (token == '[')
        token = ParseList(lexer, visitor);
    else if (token == '{')
        token = ParseObject(lexer, visitor);
    else
        token = ParsePrimitive(token, lexer, visitor);

    visitor->Pop();
    return token;
}

int JsonParser::ParsePrimitive(int token, Lexer & lexer, IJsonParserVisitor * visitor) {
    visitor->ForPrimitive(lexer);

    switch ((Token)token) {
        case Token::WORD:
            visitor->ForKeyWord(lexer);
            break;
        case Token::INTEGER:
            visitor->ForInteger(lexer);
            break;
        case Token::FLOAT:
            visitor->ForFloat(lexer);
            break;
        case Token::STRING:
            visitor->ForString(lexer);
            break;
        default:
            if ((char)token == '-') {
                JsonLexer::NextToken(lexer);
                return ParseNumber(token, lexer, visitor);
            }

            return Error(lexer, visitor);
    }

    return token;
}

int JsonParser::ParseNumber(int token, Lexer & lexer, IJsonParserVisitor * visitor) {
    switch ((Token)token) {
        case Token::INTEGER:
            visitor->ForInteger(lexer);
            break;
        case Token::FLOAT:
            visitor->ForFloat(lexer);
            break;
        default:
            return Error(lexer, visitor);
    }

    return token;
}

int JsonParser::Error(Lexer & lexer, IJsonParserVisitor * visitor) {
    visitor->ForError(lexer);
    return Token::ERROR;
}

int JsonParser::ParseKey(Lexer & lexer, IJsonParserVisitor * visitor) {
    return visitor->ForKey(lexer)
        ? Token::STRING
        : Token::END;
}
