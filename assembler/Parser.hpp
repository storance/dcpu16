#pragma once

#include <string>
#include <cstdint>

template <typename Iterator>
class Parser {
protected:
    Iterator current, end;
    std::string sourceFile;
    std::uint32_t line, column;

    void skipWhitespace();
    template<typename Function> void skipUntil(Function);

    void nextToken();
public:
    Parser(Iterator current, Iterator end, std::string sourceName);
};
