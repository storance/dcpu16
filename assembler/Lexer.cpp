#include "Lexer.hpp"

#include <string>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

using namespace std;

namespace dcpu { namespace lexer {
    Lexer::Lexer(Iterator current, Iterator end, const string &sourceName)
        : _current(current), _end(end), _sourceName(sourceName), _line(1), _column(0) {}

    void Lexer::parse() {
        while (true) {
            TokenPtr token = nextToken();
            tokens.push_back(move(token));

            if (token->isEOI()) {
                break;
            }
        }
    }

    TokenPtr Lexer::nextToken() {
        skipWhitespaceAndComments();

        if (_current == _end) {
            return TokenPtr(new Token(makeLocation(), TokenType::END_OF_INPUT, "end of file"));
        }

        char c = nextChar();
        Location start = makeLocation();

        if (c == '+' && consumeNextCharIf('+')) {
            return TokenPtr(new Token(start, TokenType::INCREMENT, "++"));
        } else if (c == '-' && consumeNextCharIf('-')) {
            return TokenPtr(new Token(start, TokenType::DECREMENT, "--"));
        } else if (c == '<' && consumeNextCharIf('<')) {
            return TokenPtr(new Token(start, TokenType::SHIFT_LEFT, "<<"));
        } else if (c == '>' && consumeNextCharIf('>')) {
            return TokenPtr(new Token(start, TokenType::SHIFT_RIGHT, ">>"));
        } else if (isAllowedIdentifierFirstChar(c)) {
            return TokenPtr(new Token(start, TokenType::IDENTIFIER, appendWhile(c, &Lexer::isAllowedIdentifierChar)));
        } else if (isdigit(c)) {
            return parseNumber(start, appendWhile(c, &Lexer::isAllowedIdentifierChar));
        } else if (c == '\n') {
            nextLine();

            return TokenPtr(new Token(start, TokenType::NEWLINE, "newline"));
        }

        return TokenPtr(new Token(start, TokenType::CHARACTER, c));
    }

    string Lexer::appendWhile(char initial, function<bool (char)> predicate) {
    	string content;

    	if (initial > 0) content += initial;

    	while (_current != _end) {
    		char c = nextChar();
    		if (!predicate(c)) {
    			moveBack();
    			break;
    		}

    		content += c;
    	}

    	return content;
    }

    Location Lexer::makeLocation() {
        return Location(_sourceName, _line, _column);
    }

    char Lexer::nextChar() {
        char c = *_current++;
        ++_column;

        return c;
    }

    bool Lexer::consumeNextCharIf(char c) {
    	if (nextChar() == c) {
    		return true;
    	}

    	moveBack();
    	return false;
    }

    void Lexer::moveBack() {
        --_current;
        --_column;
    }

    void Lexer::nextLine() {
        _line++;
        _column = 0;
    }

    bool Lexer::isWhitespace(char c) {
    	return isspace(c) && c != '\n';
    }

    bool Lexer::isAllowedIdentifierChar(char c) {
        return isalnum(c) || c == '_' || c == '?' || c == '.' || c == '$' || c == '#' || c == '@';
    }

    bool Lexer::isAllowedIdentifierFirstChar(char c) {
        return isalpha(c) || c == '_' || c == '?' || c == '.';
    }

    void Lexer::skipWhitespaceAndComments() {
        bool inComment = false;

        for (; _current != _end; _current++, _column++) {
            if (!inComment && *_current == ';') {
                inComment = true;
            } else if (!inComment && !isWhitespace(*_current)) {
                break;
            } else if (inComment && *_current == '\n') {
                break;
            }
        }
    }

    TokenPtr Lexer::parseNumber(Location start, const string &value) {
        string unprefixedValue;

        uint8_t base = 10;
        if (boost::istarts_with(value, "0x")) {
            base = 16;
            unprefixedValue = value.substr(2);
        } else if (boost::istarts_with(value, "0o")) {
            base = 8;
            unprefixedValue = value.substr(2);
        } else if (boost::istarts_with(value, "0b")) {
            base = 2;
            unprefixedValue = value.substr(2);
        } else {
            unprefixedValue = value;
        }

        // prefix without an actual value
        if (unprefixedValue.length() == 0) {
            return TokenPtr(new InvalidIntegerToken(start, value, base));
        }

        size_t pos;
        try {
            unsigned long parsedValue = std::stoul(unprefixedValue, &pos, base);
            if (pos != unprefixedValue.size()) {
                return TokenPtr(new InvalidIntegerToken(start, value, base));
            }

            if (parsedValue > UINT32_MAX) {
                return TokenPtr(new IntegerToken(start, value, UINT32_MAX, true));
            }

            return TokenPtr(new IntegerToken(start, value, (uint32_t)parsedValue, false));
        } catch (invalid_argument &ia) {
            return TokenPtr(new InvalidIntegerToken(start, value, base));
        } catch (out_of_range &oor) {
            return TokenPtr(new IntegerToken(start, value, UINT32_MAX, true));
        }
    }
}}
