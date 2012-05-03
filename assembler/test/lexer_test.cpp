#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

#include <lexer.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::lexer;

shared_ptr<lexer::lexer> run_lexer(const string &content, error_handler_ptr error_handler =
		make_shared<dcpu::error_handler>()) {
	shared_ptr<lexer::lexer> lexer = make_shared<lexer::lexer>(content, "<Test>", error_handler);
	lexer->parse();

	return lexer;
}

TEST(lexerTest, IdentifierStartsUnderscore) {
	shared_ptr<lexer::lexer> lexer = run_lexer("_a1_?.$#@");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("_a1_?.$#@", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, IdentifierStartsPeriod) {
	shared_ptr<lexer::lexer> lexer = run_lexer(".aaa111");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ(".aaa111", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, IdentifierStartsQuestion) {
	shared_ptr<lexer::lexer> lexer = run_lexer("?aaa111");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("?aaa111", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, IdentifierStartsLetter) {
	shared_ptr<lexer::lexer> lexer = run_lexer("aaa111");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("aaa111", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, DecimalNumber) {
	shared_ptr<lexer::lexer> lexer = run_lexer("100");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(100, it++->value);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, HexNumberLowercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0xff");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(0xff, it++->value);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, HexNumberUppercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0X1D");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(0x1d, it++->value);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, BinaryNumberLowercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0b1011");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(0b1011, it++->value);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, BinaryNumberUppercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0B10001011");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(0b10001011, it++->value);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, OctalNumberLowercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0o32");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(032, it++->value);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, OctalNumberUppercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0O27");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(027, it++->value);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, InvalidDecimalNumber) {
	shared_ptr<lexer::lexer> lexer = run_lexer("100a3");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("100a3", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, InvalidHexNumber) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0X100Z3");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0X100Z3", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, OctalWithInvalidNumber) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0o10093");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0o10093", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, OctalWithInvalidLetter) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0o100a3");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0o100a3", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, BinaryWithInvalidNumber) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0b1113");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0b1113", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, BinaryWithInvalidLetter) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0B111a");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0B111a", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, OverflowNumber) {
	stringstream out;
	shared_ptr<lexer::lexer> lexer = run_lexer("4294967296", make_shared<error_handler>(out));
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_TRUE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(UINT32_MAX, it++->value);

	EXPECT_TRUE(it->is_eoi());

	EXPECT_EQ("<Test>:1:1: warning: integer '4294967296' overflows 32-bit intermediary storage\n", out.str());
}

TEST(lexerTest, DecimalNumberAtUint32Max) {
	shared_ptr<lexer::lexer> lexer = run_lexer("4294967295");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(UINT32_MAX, it++->value);

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, Increment) {
	shared_ptr<lexer::lexer> lexer = run_lexer("++");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_increment());

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, Decrement) {
	shared_ptr<lexer::lexer> lexer = run_lexer("--");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_decrement());
	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, ShiftLeft) {
	shared_ptr<lexer::lexer> lexer = run_lexer("<<");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_shift_left());

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, ShiftRight) {
	shared_ptr<lexer::lexer> lexer = run_lexer(">>");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_shift_right());

	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, Newline) {
	shared_ptr<lexer::lexer> lexer = run_lexer("\n");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_newline());
	
	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, SingleCharacters) {
	shared_ptr<lexer::lexer> lexer = run_lexer("@");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_character('@'));
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer(",");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_character(','));
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("$");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_character('$'));
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("[");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_character('['));
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("]");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_character(']'));
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("(");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_character('('));
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer(")");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_character(')'));
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("+");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_character('+'));
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("-");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_character('-'));
	EXPECT_TRUE(it->is_eoi());
}

TEST(lexerTest, CharacterLiterals) {
	shared_ptr<lexer::lexer> lexer = run_lexer("'a'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ('a', it++->value);	
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("'\\x4'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(0x4, it++->value);
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("'\\X42'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(0x42, it++->value);
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("'\\n'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ('\n', it++->value);
	EXPECT_TRUE(it->is_eoi());
}

location_ptr makeLocation(uint32_t line, uint32_t column) {
	return make_shared<location>("<Test>", line, column);
}

TEST(lexerTest, MultipleTokens) {
	shared_ptr<lexer::lexer> lexer = run_lexer("set A, b\n  set [J], 0x400\n;a test comment\nlabel: JSR label+4\n");
	ASSERT_EQ(21, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	// Line 1
	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("set", it->content);
	EXPECT_EQ(makeLocation(1, 1), it++->location);

	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("A", it->content);
	EXPECT_EQ(makeLocation(1, 5), it++->location);

	EXPECT_TRUE(it->is_character(','));
	EXPECT_EQ(makeLocation(1, 6), it++->location);

	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("b", it->content);
	EXPECT_EQ(makeLocation(1, 8), it++->location);

	EXPECT_TRUE(it->is_newline());
	EXPECT_EQ(makeLocation(1, 9), it++->location);

	// Line 2
	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("set", it->content);
	EXPECT_EQ(makeLocation(2, 3), it++->location);

	EXPECT_TRUE(it->is_character('['));
	EXPECT_EQ(makeLocation(2, 7), it++->location);

	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("J", it->content);
	EXPECT_EQ(makeLocation(2, 8), it++->location);

	EXPECT_TRUE(it->is_character(']'));
	EXPECT_EQ(makeLocation(2, 9), it++->location);

	EXPECT_TRUE(it->is_character(','));
	EXPECT_EQ(makeLocation(2, 10), it++->location);

	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(0x400, it->value);
	EXPECT_EQ(makeLocation(2, 12), it++->location);

	EXPECT_TRUE(it->is_newline());
	EXPECT_EQ(makeLocation(2, 17), it++->location);

	// Line 3
	EXPECT_TRUE(it->is_newline());
	EXPECT_EQ(makeLocation(3, 16), it++->location);

	// Line 4
	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("label", it->content);
	EXPECT_EQ(makeLocation(4, 1), it++->location);

	EXPECT_TRUE(it->is_character(':'));
	EXPECT_EQ(makeLocation(4, 6), it++->location);

	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("JSR", it->content);
	EXPECT_EQ(makeLocation(4, 8), it++->location);

	EXPECT_TRUE(it->is_identifier());
	EXPECT_EQ("label", it->content);
	EXPECT_EQ(makeLocation(4, 12), it++->location);

	EXPECT_TRUE(it->is_character('+'));
	EXPECT_EQ(makeLocation(4, 17), it++->location);

	EXPECT_TRUE(it->is_integer());
	EXPECT_EQ(4, it->value);
	EXPECT_EQ(makeLocation(4, 18), it++->location);

	EXPECT_TRUE(it->is_newline());
	EXPECT_EQ(makeLocation(4, 19), it++->location);

	// Line 5
	EXPECT_TRUE(it->is_eoi());
	EXPECT_EQ(makeLocation(5, 0), it->location);
}

