#include "../Lexer.hpp"

#include <gtest/gtest.h>

using namespace std;

typedef string::const_iterator iterator_type;
typedef Lexer<iterator_type> lexer_type;
typedef Lexer<iterator_type>::token_type token_type;

void assertIdentifier(token_type token, string expectedName) {
    EXPECT_EQ(TokenType::IDENTIFIER, token->type);

    IdentifierToken *identifier = (IdentifierToken*)token.get();
    EXPECT_EQ(expectedName, identifier->name);
}

void assertNumber(token_type token, uint32_t expectedValue) {
    EXPECT_EQ(TokenType::NUMBER, token->type);

    NumberToken *number = (NumberToken*)token.get();
    EXPECT_EQ(expectedValue, number->value);
}

void assertEOI(token_type token) {
    EXPECT_EQ(TokenType::END_OF_INPUT, token->type);
}

vector<token_type> run_parser(string input, size_t expectedTokens) {
	lexer_type lexer(input.begin(), input.end(), "<LexerTest>");
    vector<token_type> tokens = lexer.parse();

	EXPECT_EQ(expectedTokens, tokens.size());

	return tokens;
}

TEST(LexerTest, Identifier) {
    vector<token_type> tokens = run_parser("_abc111", 2);

    assertIdentifier(tokens[0], "_abc111");
    assertEOI(tokens[1]);
}

TEST(LexerTest, DecimalNumber) {
    vector<token_type> tokens = run_parser("100", 2);

    assertNumber(tokens[0], 100);
    assertEOI(tokens[1]);
}

TEST(LexerTest, HexNumber) {
    vector<token_type> tokens = run_parser("0xff", 2);

    assertNumber(tokens[0], 255);
    assertEOI(tokens[1]);

    tokens = run_parser("0X1D", 2);

    assertNumber(tokens[0], 29);
    assertEOI(tokens[1]);
}

TEST(LexerTest, BinaryNumber) {
    vector<token_type> tokens = run_parser("0b1011", 2);

    assertNumber(tokens[0], 11);
    assertEOI(tokens[1]);

    tokens = run_parser("0B10001011", 2);

    assertNumber(tokens[0], 139);
    assertEOI(tokens[1]);
}

TEST(LexerTest, OctalNumber) {
    vector<token_type> tokens = run_parser("0o32", 2);

    assertNumber(tokens[0], 26);
    assertEOI(tokens[1]);

    tokens = run_parser("0O27", 2);

    assertNumber(tokens[0], 23);
    assertEOI(tokens[1]);
}
