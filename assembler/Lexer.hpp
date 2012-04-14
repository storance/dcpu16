#pragma once

#include <string>
#include <memory>
#include <vector>

enum class TokenType {
    NUMBER,
    IDENTIFIER,
    COMMA,
    COLON,
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    NEWLINE,
    QUOTED_STRING,
    UNKNOWN,
    END_OF_INPUT
};

struct Location {
    std::string sourceName;
    std::uint32_t line;
    std::uint32_t column;

    Location(std::string, std::uint32_t, std::uint32_t);
};

class Token {
public:
    Location location;
    TokenType type;

    Token(Location, TokenType);
};

class UnknownToken : public Token {
public:
    char value;

    UnknownToken(Location, char);
};

class NumericToken : public Token {
public:
    std::uint32_t value;

    NumericToken(Location, std::uint32_t);
};

class IdentifierToken : public Token {
public:
    std::string name;

    IdentifierToken(Location, std::string);
};

template <typename Iterator>
class Lexer {
protected:
    Iterator current, end;
    std::string sourceName;
    std::uint32_t line, column;

    void skipWhitespace();
    template<typename Predicate> void skipUntil(Predicate);
    template<typename Predicate, typename Action> void processUntil(Predicate, Action);

    char nextChar();
    void moveBack();
    void nextLine();

    std::shared_ptr<Token> nextToken();
    Location makeLocation();

    bool isHexDigit(char);
    bool isAllowedIdentifierChar(char);
    bool isAllowedIdentifierFirstChar(char);

    void parseHexNumber(std::uint32_t&);
    void parseOctNumber(std::uint32_t&);
    void parseBinNumber(std::uint32_t&);
    void parseNumber(std::uint32_t&);
public:
    Lexer(Iterator current, Iterator end, std::string sourceName);

    std::vector<std::shared_ptr<Token>> parse();
};

