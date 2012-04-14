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

void assertInvalidNumber(token_type token, std::string expectedValue, int expectedBase) {
    EXPECT_EQ(TokenType::INVALID_NUMBER, token->type);

    InvalidNumberToken *unknown = (InvalidNumberToken*)token.get();
    EXPECT_EQ(expectedValue, unknown->value);
    EXPECT_EQ(expectedBase, unknown->base);
}

void assertOverflowNumber(token_type token, std::string expectedValue) {
    EXPECT_EQ(TokenType::OVERFLOW_NUMBER, token->type);

    OverflowNumberToken *overflow = (OverflowNumberToken*)token.get();
    EXPECT_EQ(expectedValue, overflow->rawValue);
    EXPECT_EQ(0xffffffff, overflow->value);
}

void assertUnknown(token_type token, std::string expectedValue) {
    EXPECT_EQ(TokenType::UNKNOWN, token->type);

    UnknownToken *unknown = (UnknownToken*)token.get();
    EXPECT_EQ(expectedValue, unknown->value);
}

void assertEOI(token_type token) {
    EXPECT_EQ(TokenType::END_OF_INPUT, token->type);
}

void assertLeftBracket(token_type token) {
    EXPECT_EQ(TokenType::LBRACKET, token->type);
}
void assertRightBracket(token_type token) {
    EXPECT_EQ(TokenType::RBRACKET, token->type);
}

void assertComma(token_type token) {
    EXPECT_EQ(TokenType::COMMA, token->type);
}

void assertColon(token_type token) {
    EXPECT_EQ(TokenType::COLON, token->type);
}

void assertPlus(token_type token) {
    EXPECT_EQ(TokenType::PLUS, token->type);
}

void assertNewline(token_type token) {
    EXPECT_EQ(TokenType::NEWLINE, token->type);
}

vector<token_type> run_parser(string input, size_t expectedTokens) {
	lexer_type lexer(input.begin(), input.end(), "<LexerTest>");
    vector<token_type> tokens = lexer.parse();

	EXPECT_EQ(expectedTokens, tokens.size());

	return tokens;
}

TEST(LexerTest, Identifier) {
    vector<token_type> tokens = run_parser("_a1_?.$#@", 2);

    assertIdentifier(tokens[0], "_a1_?.$#@");
    assertEOI(tokens[1]);

    tokens = run_parser(".aaa111", 2);

    assertIdentifier(tokens[0], ".aaa111");
    assertEOI(tokens[1]);

    tokens = run_parser("?aaa111", 2);

    assertIdentifier(tokens[0], "?aaa111");
    assertEOI(tokens[1]);

    tokens = run_parser("aaa111", 2);

    assertIdentifier(tokens[0], "aaa111");
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

TEST(LexerTest, InvalidNumber) {
    vector<token_type> tokens = run_parser("100a3", 2);

    assertInvalidNumber(tokens[0], "100a3", 10);
    assertEOI(tokens[1]);

    tokens = run_parser("0X100Z3", 2);

    assertInvalidNumber(tokens[0], "0X100Z3", 16);
    assertEOI(tokens[1]);

    tokens = run_parser("0o10093", 2);

    assertInvalidNumber(tokens[0], "0o10093", 8);
    assertEOI(tokens[1]);

    tokens = run_parser("0o100a3", 2);

    assertInvalidNumber(tokens[0], "0o100a3", 8);
    assertEOI(tokens[1]);

    tokens = run_parser("0b1113", 2);

    assertInvalidNumber(tokens[0], "0b1113", 2);
    assertEOI(tokens[1]);

    tokens = run_parser("0B111a", 2);

    assertInvalidNumber(tokens[0], "0B111a", 2);
    assertEOI(tokens[1]);
}

TEST(LexerTest, OverflowNumber) {
    vector<token_type> tokens = run_parser("4294967296", 2);

    assertOverflowNumber(tokens[0], "4294967296");
    assertEOI(tokens[1]);
}

TEST(LexerTest, Operator) {
    vector<token_type> tokens = run_parser("++", 2);

    EXPECT_EQ(TokenType::INCREMENT, tokens[0]->type);
    assertEOI(tokens[1]);

    tokens = run_parser("--", 2);

    EXPECT_EQ(TokenType::DECREMENT, tokens[0]->type);
    assertEOI(tokens[1]);

    tokens = run_parser("+", 2);

    EXPECT_EQ(TokenType::PLUS, tokens[0]->type);
    assertEOI(tokens[1]);

    tokens = run_parser("-", 2);

    EXPECT_EQ(TokenType::MINUS, tokens[0]->type);
    assertEOI(tokens[1]);

    tokens = run_parser("+++", 2);

    assertUnknown(tokens[0], "+++");
    assertEOI(tokens[1]);
}

TEST(LexerTest, Newline) {
	vector<token_type> tokens = run_parser("\n", 2);

    assertNewline(tokens[0]);
    assertEOI(tokens[1]);

    tokens = run_parser("\r\n", 2);

    assertNewline(tokens[0]);
    assertEOI(tokens[1]);
}

TEST(LexerTest, MiscTokens) {
	vector<token_type> tokens = run_parser(":", 2);

    assertColon(tokens[0]);
    assertEOI(tokens[1]);

    tokens = run_parser("@", 2);

    EXPECT_EQ(TokenType::AT, tokens[0]->type);
    assertEOI(tokens[1]);

    tokens = run_parser(",", 2);

    assertComma(tokens[0]);
    assertEOI(tokens[1]);

	tokens = run_parser("$", 2);

    EXPECT_EQ(TokenType::DOLLAR, tokens[0]->type);
    assertEOI(tokens[1]);

    tokens = run_parser("[", 2);

    assertLeftBracket(tokens[0]);
    assertEOI(tokens[1]);

    tokens = run_parser("]", 2);

    assertRightBracket(tokens[0]);
    assertEOI(tokens[1]);

    tokens = run_parser("(", 2);

    EXPECT_EQ(TokenType::LPAREN, tokens[0]->type);
    assertEOI(tokens[1]);

    tokens = run_parser(")", 2);

    EXPECT_EQ(TokenType::RPAREN, tokens[0]->type);
    assertEOI(tokens[1]);
}

TEST(LexerTest, SkipLogic) {
	vector<token_type> tokens = run_parser(" ;comment\n\t1 + 3", 5);

	assertNewline(tokens[0]);
	assertNumber(tokens[1], 1);
	assertPlus(tokens[2]);
	assertNumber(tokens[3], 3);
    assertEOI(tokens[4]);
}

TEST(LexerTest, SimpleExpression) {
	vector<token_type> tokens = run_parser("set A, b\nset [J], 0x400\nlabel: JSR label+4\n", 20);

	int index = 0;
	assertIdentifier(tokens[index++], "set");
	assertIdentifier(tokens[index++], "A");
	assertComma(tokens[index++]);
	assertIdentifier(tokens[index++], "b");
	assertNewline(tokens[index++]);

	assertIdentifier(tokens[index++], "set");
	assertLeftBracket(tokens[index++]);
	assertIdentifier(tokens[index++], "J");
	assertRightBracket(tokens[index++]);
	assertComma(tokens[index++]);
	assertNumber(tokens[index++], 0x400);
	assertNewline(tokens[index++]);

	assertIdentifier(tokens[index++], "label");
	assertColon(tokens[index++]);
	assertIdentifier(tokens[index++], "JSR");
	assertIdentifier(tokens[index++], "label");
	assertPlus(tokens[index++]);
	assertNumber(tokens[index++], 4);
	assertNewline(tokens[index++]);

	assertEOI(tokens[index++]);
}
