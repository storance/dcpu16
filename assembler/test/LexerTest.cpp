#include "../Lexer.hpp"

#include <iostream>
#include <vector>
#include <gtest/gtest.h>

using namespace std;
using namespace dcpu;
using namespace dcpu::lexer;

#define ASSERT_LOCATION(token, lineNum, columnNum) { \
	SCOPED_TRACE("Location"); \
	assertLocation(token, lineNum, columnNum); \
}

#define EXPECT_INTEGER(token, expectedValue, expectedOverflow) { \
	SCOPED_TRACE("Integer"); \
	expectInteger(token, expectedVaue, expectedOverflow); \
}

#define EXPECT_INVALID_INTEGER(token, expectedContent, expectedBase) { \
	SCOPED_TRACE("Invalid Integer"); \
	expectInvaidInteger(token ,expectedContent, expectedBase); \
}

void expectInteger(TokenPtr &token, uint32_t expectedValue, bool expectedOverflow) {
	IntegerToken *integerToken = asInteger(token);
	EXPECT_EQ(expectedValue, integerToken->value); 
	if (expectedOverflow)  {
		EXPECT_TRUE(integerToken->overflow);
	} else {
		EXPECT_FALSE(integerToken->overflow);
	}
}

void expectInvalidInteger(TokenPtr &ptr, const std::string &expectedContent, uint8_t expectedBase) {
	InvalidIntegerToken* integerToken = asInvalidInteger(token);
	EXPECT_EQ(expectedContent, integerToken->content);
	EXPECT_EQ(expectedBase, integerToken->base);
}

void assertLocation(TokenPtr &token, uint32_t expectedLine, uint32_t expectedColumn) {
	EXPECT_EQ("<LexerTest>", token->location.sourceName);
	EXPECT_EQ(expectedLine, token->location.line);
	EXPECT_EQ(expectedColumn, token->location.column);
}

void assertIdentifier(TokenPtr &token, const string &expectedContent) {
	ASSERT_TRUE(token->isIdentifier());
	EXPECT_EQ(expectedContent, token->content);
}

void assertEOI(TokenPtr &token) {
	ASSERT_TRUE(token->isEOI());
}

void runParser(const string &input, size_t expectedTokens, vector<TokenPtr> &tokens) {
	Lexer lexer(input.begin(), input.end(), "<LexerTest>");
    lexer.parse();

    tokens = move(lexer.tokens);

    ASSERT_EQ(expectedTokens, tokens.size());
}

TEST(LexerTest, IdentifierStartsUnderscore) {
    vector<TokenPtr> tokens;

    ASSERT_NO_FATAL_FAILURE(runParser("_a1_?.$#@", 2, tokens));

	auto it = tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertIdentifier(*it++, "_a1_?.$#@");
	}

	{
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, IdentifierStartsPeriod) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser(".aaa111", 2, tokens));

	EXPECT_TRUE(tokens[0]->isIdentifier());
	EXPECT_EQ(tokens[0]->content, ".aaa111");

	EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, IdentifierStartsQuestion) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("?aaa111", 2, tokens));

    EXPECT_TRUE(tokens[0]->isIdentifier());
	EXPECT_EQ(tokens[0]->content, "?aaa111");

	EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, IdentifierStartsLetter) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("aaa111", 2, tokens));

    EXPECT_TRUE(tokens[0]->isIdentifier());
	EXPECT_EQ(tokens[0]->content, "aaa111");

	EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, DecimalNumber) {
    vector<TokenPtr> tokens;

    ASSERT_NO_FATAL_FAILURE(runParser("100", 2, tokens));

	ASSERT_TRUE(tokens[0]->isInteger());
	EXPECT_INTEGER(tokens[0], 100, false);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, HexNumberLowercase) {
    vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0xff", 2, tokens));

	ASSERT_TRUE(tokens[0]->isInteger());
	EXPECT_INTEGER(tokens[0], 0xff, false);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, HexNumberUppercase) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0X1D", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInteger());
	EXPECT_INTEGER(tokens[0], 0x1d, false);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, BinaryNumberLowercase) {
    vector<TokenPtr> tokens;

    ASSERT_NO_FATAL_FAILURE(runParser("0b1011", 2, tokens));

	ASSERT_TRUE(tokens[0]->isInteger());
	EXPECT_INTEGER(tokens[0], 0b1011, false);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, BinaryNumberUppercase) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0B10001011", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInteger());
	EXPECT_INTEGER(tokens[0], 0b10001011, false);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, OctalNumberLowercase) {
    vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0o32", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInteger());
	EXPECT_INTEGER(tokens[0], 032, false);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, OctalNumberUppercase) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0O27", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInteger());
	EXPECT_INTEGER(tokens[0], 027, false);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, InvalidDecimalNumber) {
    vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("100a3", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInvalidInteger());
	EXPECT_INVALID_INTEGER(tokens[0], "100a3", 10);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, InvalidHexNumber) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0X100Z3", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInvalidInteger());
	EXPECT_INVALID_INTEGER(tokens[0], "0X100Z3", 16);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, OctalWithInvalidNumber) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0o10093", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInvalidInteger());
	EXPECT_INVALID_INTEGER(tokens[0], "0o10093", 8);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, OctalWithInvalidLetter) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0o100a3", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInvalidInteger());
	EXPECT_INVALID_INTEGER(tokens[0], "0o100a3", 8);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, BinaryWithInvalidNumber) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0b1113", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInvalidInteger());
	EXPECT_INVALID_INTEGER(tokens[0], "0b1113", 2);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, BinaryWithInvalidLetter) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("0B111a", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInvalidInteger());
	EXPECT_INVALID_INTEGER(tokens[0], "0B111a", 2);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, OverflowNumber) {
    vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("4294967296", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInteger());
	EXPECT_INTEGER(tokens[0], UINT32_MAX, true);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, DecimalNumberAtUint32Max) {
    vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("4294967295", 2, tokens));

    ASSERT_TRUE(tokens[0]->isInteger());
	EXPECT_INTEGER(tokens[0], UINT32_MAX, false);

    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, Increment) {
    vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("++", 2, tokens));

	EXPECT_TRUE(tokens[0]->isIncrement());
	EXPECT_EQ(tokens[0]->content, "++");

	EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, Decrement) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("--", 2, tokens));

    EXPECT_TRUE(tokens[0]->isDecrement());
	EXPECT_EQ(tokens[0]->content, "--");

	EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, ShiftLeft) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("<<", 2, tokens));

    EXPECT_TRUE(tokens[0]->isShiftLeft());
	EXPECT_EQ(tokens[0]->content, "<<");

	EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, ShiftRight) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser(">>", 2, tokens));

    EXPECT_TRUE(tokens[0]->isShiftRight());
	EXPECT_EQ(tokens[0]->content, ">>");

	EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, Newline) {
	vector<TokenPtr> tokens;
	ASSERT_NO_FATAL_FAILURE(runParser("\n", 2, tokens));

    EXPECT_TRUE(tokens[0]->isNewline());
    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, SingleCharacters) {
	vector<TokenPtr> tokens;
    ASSERT_NO_FATAL_FAILURE(runParser("@", 2, tokens));

    EXPECT_TRUE(tokens[0]->isCharacter('@'));
    EXPECT_TRUE(tokens[1]->isEOI());

    ASSERT_NO_FATAL_FAILURE(runParser(",", 2, tokens));

    EXPECT_TRUE(tokens[0]->isCharacter(','));
    EXPECT_TRUE(tokens[1]->isEOI());

	ASSERT_NO_FATAL_FAILURE(runParser("$", 2, tokens));

    EXPECT_TRUE(tokens[0]->isCharacter('$'));
    EXPECT_TRUE(tokens[1]->isEOI());

    ASSERT_NO_FATAL_FAILURE(runParser("[", 2, tokens));

    EXPECT_TRUE(tokens[0]->isCharacter('['));
    EXPECT_TRUE(tokens[1]->isEOI());

    ASSERT_NO_FATAL_FAILURE(runParser("]", 2, tokens));

    EXPECT_TRUE(tokens[0]->isCharacter(']'));
    EXPECT_TRUE(tokens[1]->isEOI());

    ASSERT_NO_FATAL_FAILURE(runParser("(", 2, tokens));

    EXPECT_TRUE(tokens[0]->isCharacter('('));
    EXPECT_TRUE(tokens[1]->isEOI());

    ASSERT_NO_FATAL_FAILURE(runParser(")", 2, tokens));

    EXPECT_TRUE(tokens[0]->isCharacter(')'));
    EXPECT_TRUE(tokens[1]->isEOI());

    ASSERT_NO_FATAL_FAILURE(runParser("+", 2, tokens));

    EXPECT_TRUE(tokens[0]->isCharacter('+'));
    EXPECT_TRUE(tokens[1]->isEOI());

    ASSERT_NO_FATAL_FAILURE(runParser("-", 2, tokens));

    EXPECT_TRUE(tokens[0]->isCharacter('-'));
    EXPECT_TRUE(tokens[1]->isEOI());
}

TEST(LexerTest, SimpleExpression) {
	vector<TokenPtr> tokens;
	ASSERT_NO_FATAL_FAILURE(runParser("set A, b\nset [J], 0x400\nlabel: JSR label+4\n", 20, tokens));

	int index = 0;
	// Line 1
	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("set", tokens[index++]->content);

	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("A", tokens[index++]->content);

	EXPECT_TRUE(tokens[index++]->isCharacter(','));

	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("b", tokens[index++]->content);

	EXPECT_TRUE(tokens[index++]->isNewline());

	// Line 2
	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("set", tokens[index++]->content);

	EXPECT_TRUE(tokens[index++]->isCharacter('['));

	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("J", tokens[index++]->content);

	EXPECT_TRUE(tokens[index++]->isCharacter(']'));

	EXPECT_TRUE(tokens[index++]->isCharacter(','));

	ASSERT_TRUE(tokens[index]->isInteger());
	EXPECT_INTEGER(tokens[index++], 0x400, false);

	EXPECT_TRUE(tokens[index++]->isNewline());

	// Line 3
	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("label", tokens[index++]->content);

	EXPECT_TRUE(tokens[index++]->isCharacter(':'));

	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("JSR", tokens[index++]->content);

	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("label", tokens[index++]->content);

	EXPECT_TRUE(tokens[index++]->isCharacter('+'));

	ASSERT_TRUE(tokens[index]->isInteger());
	EXPECT_INTEGER(tokens[index++], 4, false);

	EXPECT_TRUE(tokens[index++]->isNewline());

	EXPECT_TRUE(tokens[index++]->isEOI());
}

TEST(LexerTest, Location) {
	vector<TokenPtr> tokens;
	ASSERT_NO_FATAL_FAILURE(runParser("set A, b\n  set [J], 0x400\n;a test comment\nSET I, 1\n", 19, tokens));

	int index = 0;

	// Line 1
	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("set", tokens[index]->content);
	ASSERT_LOCATION(tokens[index++], 1, 1);

	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("A", tokens[index]->content);
	ASSERT_LOCATION(tokens[index++], 1, 5);

	EXPECT_TRUE(tokens[index]->isCharacter(','));
	ASSERT_LOCATION(tokens[index++], 1, 6);


	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("b", tokens[index]->content);
	ASSERT_LOCATION(tokens[index++], 1, 8);

	EXPECT_TRUE(tokens[index]->isNewline());
	ASSERT_LOCATION(tokens[index++], 1, 9);

	// Line 2
	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("set", tokens[index]->content);
	ASSERT_LOCATION(tokens[index++], 2, 3);

	EXPECT_TRUE(tokens[index]->isCharacter('['));
	ASSERT_LOCATION(tokens[index++], 2, 7);

	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("J", tokens[index]->content);
	ASSERT_LOCATION(tokens[index++], 2, 8);

	EXPECT_TRUE(tokens[index]->isCharacter(']'));
	ASSERT_LOCATION(tokens[index++], 2, 9);

	EXPECT_TRUE(tokens[index]->isCharacter(','));
	ASSERT_LOCATION(tokens[index++], 2, 10);

	ASSERT_TRUE(tokens[index]->isInteger());
	EXPECT_INTEGER(tokens[index], 0x400, false);
	ASSERT_LOCATION(tokens[index++], 2, 12);

	EXPECT_TRUE(tokens[index]->isNewline());
	ASSERT_LOCATION(tokens[index++], 2, 17);

	// Line 3
	EXPECT_TRUE(tokens[index]->isNewline());
	ASSERT_LOCATION(tokens[index++], 3, 16);

	// Line 4
	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("SET", tokens[index]->content);
	ASSERT_LOCATION(tokens[index++], 4, 1);

	EXPECT_TRUE(tokens[index]->isIdentifier());
	EXPECT_EQ("I", tokens[index]->content);
	ASSERT_LOCATION(tokens[index++], 4, 5);

	EXPECT_TRUE(tokens[index]->isCharacter(','));
	ASSERT_LOCATION(tokens[index++], 4, 6);

	ASSERT_TRUE(tokens[index]->isInteger());
	EXPECT_INTEGER(tokens[index], 1, false);
	ASSERT_LOCATION(tokens[index++], 4, 8);

	EXPECT_TRUE(tokens[index]->isNewline());
	ASSERT_LOCATION(tokens[index++], 4, 9);

	// Line 5
	EXPECT_TRUE(tokens[index]->isEOI());
	ASSERT_LOCATION(tokens[index++], 5, 0);
}
