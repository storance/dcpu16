#include "../Lexer.hpp"

#include <vector>
#include <gtest/gtest.h>

using namespace std;
using namespace dcpu;
using namespace dcpu::lexer;

typedef string::const_iterator iterator_type;
typedef Lexer<iterator_type, std::vector<token_type>> lexer_type;

#define assertIdentifier(token, expectedName) { \
	token_type t = token; \
    ASSERT_EQ(TokenType::IDENTIFIER, t->type); \
    IdentifierToken *identifier = (IdentifierToken*)t.get(); \
    EXPECT_EQ(expectedName, identifier->name); \
}

#define assertNumber(token, expectedValue) { \
	token_type t = token; \
    ASSERT_EQ(TokenType::NUMBER, t->type); \
    NumberToken *number = (NumberToken*)t.get(); \
    EXPECT_EQ(expectedValue, number->value); \
}

#define assertInvalidNumber(token, expectedValue, expectedBase) { \
	token_type t = token; \
    ASSERT_EQ(TokenType::INVALID_NUMBER, t->type); \
    InvalidNumberToken *unknown = (InvalidNumberToken*)t.get(); \
    EXPECT_EQ(expectedValue, unknown->value); \
    EXPECT_EQ(expectedBase, unknown->base); \
}

#define assertOverflowNumber(token, expectedValue) { \
	token_type t = token; \
    ASSERT_EQ(TokenType::OVERFLOW_NUMBER, t->type); \
    OverflowNumberToken *overflow = (OverflowNumberToken*)t.get(); \
    EXPECT_EQ(expectedValue, overflow->rawValue); \
    EXPECT_EQ(0xffffffff, overflow->value); \
}

#define assertUnknown(token, expectedValue) { \
	token_type t = token; \
    ASSERT_EQ(TokenType::UNKNOWN, t->type); \
    UnknownToken *unknown = (UnknownToken*)t.get(); \
    EXPECT_EQ(expectedValue, unknown->value); \
}

#define assertEOI(token) ASSERT_EQ(TokenType::END_OF_INPUT, token->type)

#define assertLeftBracket(token) ASSERT_EQ(TokenType::LBRACKET, token->type)

#define assertRightBracket(token) ASSERT_EQ(TokenType::RBRACKET, token->type)

#define assertLeftParenthesis(token) ASSERT_EQ(TokenType::LPAREN, token->type)

#define assertRightParenthesis(token) ASSERT_EQ(TokenType::RPAREN, token->type)

#define assertComma(token) ASSERT_EQ(TokenType::COMMA, token->type)

#define assertColon(token) ASSERT_EQ(TokenType::COLON, token->type)

#define assertDollar(token) ASSERT_EQ(TokenType::DOLLAR, token->type)

#define assertAt(token) ASSERT_EQ(TokenType::AT, token->type)

#define assertPlus(token) ASSERT_EQ(TokenType::PLUS, token->type)

#define assertMinus(token) ASSERT_EQ(TokenType::MINUS, token->type)

#define assertIncrement(token) ASSERT_EQ(TokenType::INCREMENT, token->type)

#define assertDecrement(token) ASSERT_EQ(TokenType::DECREMENT, token->type)

#define assertNewline(token) ASSERT_EQ(TokenType::NEWLINE, token->type)

#define assertLocation(token, lineNum, columnNum) { \
	token_type t = token; \
	EXPECT_EQ("<LexerTest>", t->location.sourceName); \
	EXPECT_EQ(lineNum, t->location.line); \
	EXPECT_EQ(columnNum, t->location.column); \
}

void runParser(string input, size_t expectedTokens, vector<token_type> &tokens) {
	lexer_type lexer(input.begin(), input.end(), "<LexerTest>");
    lexer.parse();

    tokens = lexer.getTokens();

    ASSERT_EQ(expectedTokens, tokens.size());
}

TEST(LexerTest, Identifier) {
    vector<token_type> tokens;

    ASSERT_NO_FATAL_FAILURE(runParser("_a1_?.$#@", 2, tokens));

    assertIdentifier(tokens[0], "_a1_?.$#@");
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser(".aaa111", 2, tokens));

    assertIdentifier(tokens[0], ".aaa111");
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("?aaa111", 2, tokens));

    assertIdentifier(tokens[0], "?aaa111");
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("aaa111", 2, tokens));

    assertIdentifier(tokens[0], "aaa111");
    assertEOI(tokens[1]);
}

TEST(LexerTest, DecimalNumber) {
    vector<token_type> tokens;

    ASSERT_NO_FATAL_FAILURE(runParser("100", 2, tokens));

    assertNumber(tokens[0], 100);
    assertEOI(tokens[1]);
}

TEST(LexerTest, HexNumber) {
    vector<token_type> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0xff", 2, tokens));

    assertNumber(tokens[0], 255);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("0X1D", 2, tokens));

    assertNumber(tokens[0], 29);
    assertEOI(tokens[1]);
}

TEST(LexerTest, BinaryNumber) {
    vector<token_type> tokens;

    ASSERT_NO_FATAL_FAILURE(runParser("0b1011", 2, tokens));

    assertNumber(tokens[0], 11);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("0B10001011", 2, tokens));

    assertNumber(tokens[0], 139);
    assertEOI(tokens[1]);
}

TEST(LexerTest, OctalNumber) {
    vector<token_type> tokens;

    ASSERT_NO_FATAL_FAILURE(runParser("0o32", 2, tokens));

    assertNumber(tokens[0], 26);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("0O27", 2, tokens));

    assertNumber(tokens[0], 23);
    assertEOI(tokens[1]);
}

TEST(LexerTest, InvalidNumber) {
    vector<token_type> tokens;

    ASSERT_NO_FATAL_FAILURE(runParser("100a3", 2, tokens));

    assertInvalidNumber(tokens[0], "100a3", 10);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("0X100Z3", 2, tokens));

    assertInvalidNumber(tokens[0], "0X100Z3", 16);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("0o10093", 2, tokens));

    assertInvalidNumber(tokens[0], "0o10093", 8);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("0o100a3", 2, tokens));

    assertInvalidNumber(tokens[0], "0o100a3", 8);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("0b1113", 2, tokens));

    assertInvalidNumber(tokens[0], "0b1113", 2);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("0B111a", 2, tokens));

    assertInvalidNumber(tokens[0], "0B111a", 2);
    assertEOI(tokens[1]);
}

TEST(LexerTest, OverflowNumber) {
    vector<token_type> tokens;

    ASSERT_NO_FATAL_FAILURE(runParser("4294967296", 2, tokens));

    assertOverflowNumber(tokens[0], "4294967296");
    assertEOI(tokens[1]);
}

TEST(LexerTest, Operator) {
    vector<token_type> tokens;

    ASSERT_NO_FATAL_FAILURE(runParser("++", 2, tokens));

    assertIncrement(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("--", 2, tokens));

    assertDecrement(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("+", 2, tokens));

    assertPlus(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("-", 2, tokens));

    assertMinus(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("+++", 2, tokens));

    assertUnknown(tokens[0], "+++");
    assertEOI(tokens[1]);
}

TEST(LexerTest, Newline) {
	vector<token_type> tokens;

	ASSERT_NO_FATAL_FAILURE(runParser("\n", 2, tokens));

    assertNewline(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("\r\n", 2, tokens));

    assertNewline(tokens[0]);
    assertEOI(tokens[1]);
}

TEST(LexerTest, MiscTokens) {
	vector<token_type> tokens;

	ASSERT_NO_FATAL_FAILURE(runParser(":", 2, tokens));

    assertColon(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("@", 2, tokens));

    assertAt(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser(",", 2, tokens));

    assertComma(tokens[0]);
    assertEOI(tokens[1]);

	ASSERT_NO_FATAL_FAILURE(runParser("$", 2, tokens));

    assertDollar(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("[", 2, tokens));

    assertLeftBracket(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("]", 2, tokens));

    assertRightBracket(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser("(", 2, tokens));

    assertLeftParenthesis(tokens[0]);
    assertEOI(tokens[1]);

    ASSERT_NO_FATAL_FAILURE(runParser(")", 2, tokens));

    assertRightParenthesis(tokens[0]);
    assertEOI(tokens[1]);
}

TEST(LexerTest, SkipLogic) {
	vector<token_type> tokens;

	ASSERT_NO_FATAL_FAILURE(runParser(" ;comment\n\t1 + 3", 5, tokens));

	assertNewline(tokens[0]);
	assertNumber(tokens[1], 1);
	assertPlus(tokens[2]);
	assertNumber(tokens[3], 3);
    assertEOI(tokens[4]);
}

TEST(LexerTest, SimpleExpression) {
	vector<token_type> tokens;

	ASSERT_NO_FATAL_FAILURE(runParser("set A, b\nset [J], 0x400\nlabel: JSR label+4\n", 20, tokens));

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

TEST(LexerTest, Location) {
	vector<token_type> tokens;

	ASSERT_NO_FATAL_FAILURE(runParser("set A, b\n  set [J], 0x400\n;a test comment\nSET I, 1\n", 19, tokens));

	int index = 0;
	assertIdentifier(tokens[index], "set");
	assertLocation(tokens[index++], 1, 1);

	assertIdentifier(tokens[index], "A");
	assertLocation(tokens[index++], 1, 5);

	assertComma(tokens[index]);
	assertLocation(tokens[index++], 1, 6);

	assertIdentifier(tokens[index], "b");
	assertLocation(tokens[index++], 1, 8);

	assertNewline(tokens[index]);
	assertLocation(tokens[index++], 1, 9);

	assertIdentifier(tokens[index], "set");
	assertLocation(tokens[index++], 2, 3);

	assertLeftBracket(tokens[index]);
	assertLocation(tokens[index++], 2, 7);

	assertIdentifier(tokens[index], "J");
	assertLocation(tokens[index++], 2, 8);

	assertRightBracket(tokens[index]);
	assertLocation(tokens[index++], 2, 9);

	assertComma(tokens[index]);
	assertLocation(tokens[index++], 2, 10);

	assertNumber(tokens[index], 0x400);
	assertLocation(tokens[index++], 2, 12);

	assertNewline(tokens[index]);
	assertLocation(tokens[index++], 2, 17);

	assertNewline(tokens[index]);
	assertLocation(tokens[index++], 3, 16);

	assertIdentifier(tokens[index], "SET");
	assertLocation(tokens[index++], 4, 1);

	assertIdentifier(tokens[index], "I");
	assertLocation(tokens[index++], 4, 5);

	assertComma(tokens[index]);
	assertLocation(tokens[index++], 4, 6);

	assertNumber(tokens[index], 1);
	assertLocation(tokens[index++], 4, 8);

	assertNewline(tokens[index]);
	assertLocation(tokens[index++], 4, 9);

	assertEOI(tokens[index]);
	assertLocation(tokens[index++], 5, 0);
}
