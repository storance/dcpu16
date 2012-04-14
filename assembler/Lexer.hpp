#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Token.hpp"

template <typename Iterator>
class Lexer {
public:
    typedef std::shared_ptr<Token> token_type;
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

    token_type nextToken();
    Location makeLocation();

    bool isHexDigit(char);
    bool isAllowedIdentifierChar(char);
    bool isAllowedIdentifierFirstChar(char);

    token_type parseNumber(std::string value);
public:
    Lexer(Iterator current, Iterator end, std::string sourceName);

    std::vector<token_type> parse();
};

#include "Lexer.cpp"
