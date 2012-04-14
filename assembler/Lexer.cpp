#include "Lexer.hpp"

#include <cctype>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>

using namespace std;

Location::Location(std::string sourceName, uint32_t line, uint32_t column)
    : sourceName(sourceName), line(line), column(column) {}

Token::Token(Location location, TokenType type)
    : location(location), type(type) {}

UnknownToken::UnknownToken(Location location, char value)
    : Token(location, TokenType::UNKNOWN), value(value) {}

NumericToken::NumericToken(Location location, uint32_t value)
    : Token(location, TokenType::NUMBER), value(value) {}

IdentifierToken::IdentifierToken(Location location, std::string name)
    : Token(location, TokenType::IDENTIFIER), name(name) {}

template<typename Iterator>
Lexer<Iterator>::Lexer(Iterator current, Iterator end, string sourceName)
    : current(current), end(end), sourceName(sourceName), line(1), column(0) {

}

template<typename Iterator>
void Lexer<Iterator>::skipWhitespace() {
    skipUntil([] (char c) { return c != ' ' && c != '\t';});
}

template<typename Iterator> template<typename Predicate>
void Lexer<Iterator>::skipUntil(Predicate predicate) {
    processUntil(predicate, [] (char ch) -> void {});
}

template<typename Iterator> template<typename Predicate, typename Action>
void Lexer<Iterator>::processUntil(Predicate predicate, Action action) {
    while (current != end) {
        char c = nextChar();
        if (predicate(c)) {
            moveBack();
            break;
        }

        action(c);
    }
}

template<typename Iterator>
Location Lexer<Iterator>::makeLocation() {
    return Location(sourceName, line, column);
}

template<typename Iterator>
char Lexer<Iterator>::nextChar() {
    char c = *current;
    ++current;
    ++column;

    return c;
}

template<typename Iterator>
void Lexer<Iterator>::moveBack() {
    --current;
}

template<typename Iterator>
void Lexer<Iterator>::nextLine() {
    line++;
    column = 0;
}

template<typename Iterator>
bool Lexer<Iterator>::isHexDigit(char c) {
    return isdigit(c) || tolower(c) >= 'a' && tolower(c) <= 'f';
}

template<typename Iterator>
bool Lexer<Iterator>::isAllowedIdentifierChar(char c) {
    return isalnum(c) || c == '_' || c == '?' || c == '.' || c == '$' || c == '#' || c == '@';
}

template<typename Iterator>
bool Lexer<Iterator>::isAllowedIdentifierFirstChar(char c) {
    return isalpha(c) || c == '_' || c == '?' || c == '.' || c == '$';
}

template<typename Iterator>
void Lexer<Iterator>::parseHexNumber(std::uint32_t &value) {
    processUntil([this](char c) {
        return !this->isHexDigit(c);
    }, [&](char c) {
        value <<= 4;
        if (isdigit(c)) {
            value += c - '0';
        } else {
            value += tolower(c) - 'a' + 10;
        }
    });
}

template<typename Iterator>
void Lexer<Iterator>::parseOctNumber(std::uint32_t& value) {
    processUntil([this](char c) {
        return c < '0' || c > '7';
    }, [&](char c) {
        value = (value << 3) + (c - '0');
    });
}

template<typename Iterator>
void Lexer<Iterator>::parseBinNumber(std::uint32_t& value) {
    processUntil([this](char c) {
        return c != '0' && c != '1';
    }, [&](char c) {
        value = (value << 1) + (c - '0');
    });
}

template<typename Iterator>
void Lexer<Iterator>::parseNumber(std::uint32_t& value) {
    processUntil([this](char c) {
        return !isdigit(c);
    }, [&](char c) {
        value = (value * 10) + (c - '0');
    });
}

template<typename Iterator>
shared_ptr<Token> Lexer<Iterator>::nextToken() {
    skipWhitespace();

    if (current == end) {
        return make_shared<Token>(makeLocation(), TokenType::END_OF_INPUT);
    }
    char c = nextChar();

    if (c == ';') {
        skipUntil([](char ch) { return ch == '\n' || ch == '\r'; });

        return nextToken();
    } else if (c == ':') {
        return make_shared<Token>(makeLocation(), TokenType::COLON);
    } else if (c == ',') {
        return make_shared<Token>(makeLocation(), TokenType::COMMA);
    } else if (isAllowedIdentifierFirstChar(c)) {
        string name;
        name += c;

        processUntil([this](char ch) { return !this->isAllowedIdentifierChar(ch);}, [&] (char ch) { name += ch; });

        return make_shared<IdentifierToken>(makeLocation(), name);
    } else if (isdigit(c)) {
        uint32_t value = 0;
        if (c == '0') {
            c = nextChar();
            if (tolower(c) == 'x') {
                parseHexNumber(value);
            } else if (tolower(c) == 'o') {
                parseOctNumber(value);
            } else if (tolower(c) == 'b') {
                parseBinNumber(value);
            } else if (isdigit(c)) {
                moveBack();
                parseNumber(value);
            } else {
                moveBack();

                return make_shared<NumericToken>(makeLocation(), value);
            }
        } else {
            moveBack();
            parseNumber(value);
        }

        return make_shared<NumericToken>(makeLocation(), value);
    } else if (c == '[') {
        return make_shared<Token>(makeLocation(), TokenType::LBRACKET);
    } else if (c == ']') {
        return make_shared<Token>(makeLocation(), TokenType::RBRACKET);
    } else if (c == '\r') {
        if (nextChar() == '\n') {
            nextLine();

            return make_shared<Token>(makeLocation(), TokenType::NEWLINE);
        } else {
            moveBack();

            return make_shared<UnknownToken>(makeLocation(), c);
        }

    } else if (c == '\n') {
        nextLine();

        return make_shared<Token>(makeLocation(), TokenType::NEWLINE);
    }

    return make_shared<UnknownToken>(makeLocation(), c);
}
template<typename Iterator>
vector<shared_ptr<Token>> Lexer<Iterator>::parse() {
    vector<shared_ptr<Token>> tokens;

    while (true) {
        shared_ptr<Token> token = nextToken();
        tokens.push_back(token);

        if (token->type == TokenType::END_OF_INPUT) {
            break;
        }
    }

    return tokens;
}

typedef string::const_iterator iterator_type;

int main(int argc, char **argv) {
    ifstream in(argv[1], ios_base::in);

    if (!in) {
        cerr << "Failed to open " << argv[1] << endl;
    }

    string storage;
    in.unsetf(ios::skipws);
    copy(istream_iterator<char>(in), istream_iterator<char>(),
    	back_inserter(storage));

    iterator_type begin = storage.begin();
    iterator_type end = storage.end();

    Lexer<iterator_type> parser(begin, end, argv[1]);

    vector<shared_ptr<Token>> tokens = parser.parse();

    cout << "Total Tokens: " << tokens.size() << endl;
    for (shared_ptr<Token> token : tokens) {
        switch (token->type) {
        case TokenType::NUMBER:
            cout << "Number (" << ((NumericToken*)token.get())->value << ")" << endl;
            break;
        case TokenType::IDENTIFIER:
            cout << "Identifier (" << ((IdentifierToken*)token.get())->name << ")" << endl;
            break;
        case TokenType::COLON:
            cout << "Colon" << endl;
            break;
        case TokenType::COMMA:
            cout << "Comma" << endl;
            break;
        case TokenType::LBRACKET:
            cout << "Left Bracket" << endl;
            break;
        case TokenType::RBRACKET:
            cout << "Right Bracket" << endl;
            break;
        case TokenType::NEWLINE:
            cout << "Newline" << endl;
            break;
        case TokenType::END_OF_INPUT:
            cout << "End of Input" << endl;
            break;
        case TokenType::UNKNOWN:
            cout << "Unknown (" << ((UnknownToken*)token.get())->value << ")" << endl;
            break;
        default:
            cout << "Token (" << static_cast<int>(token->type) << ")" << endl;
            break;
        }
    }
}
