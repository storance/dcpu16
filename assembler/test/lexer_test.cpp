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

TEST(LexerTest, SymbolStartsUnderscore) {
	shared_ptr<lexer::lexer> lexer = run_lexer("_a1_?.$#@");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ("_a1_?.$#@", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, SymbolStartsPeriod) {
	shared_ptr<lexer::lexer> lexer = run_lexer(".aaa111");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ(".aaa111", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, SymbolStartsQuestion) {
	shared_ptr<lexer::lexer> lexer = run_lexer("?aaa111");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ("?aaa111", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, SymbolStartsLetter) {
	shared_ptr<lexer::lexer> lexer = run_lexer("aaa111");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ("aaa111", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, ExplicitSymbol) {
	shared_ptr<lexer::lexer> lexer = run_lexer("$pc");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ("pc", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, QuotedString) {
	shared_ptr<lexer::lexer> lexer = run_lexer("\"hello, world!\"");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_quoted_string());
	EXPECT_EQ(quote_type::DOUBLE_QUOTE, it->get_quote_type());
	EXPECT_EQ("hello, world!", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, LabelColonSuffix) {
	shared_ptr<lexer::lexer> lexer = run_lexer("_a1_?.$#@:");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_label());
	EXPECT_EQ("_a1_?.$#@", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, LabelColonPrefix) {
	shared_ptr<lexer::lexer> lexer = run_lexer(":_a1_?.$#@");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_label());
	EXPECT_EQ("_a1_?.$#@", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, RegisterTest) {
	shared_ptr<lexer::lexer> lexer = run_lexer("A B C X Y Z I J PC SP EX");
	ASSERT_EQ(12, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_register(registers::A));
	EXPECT_TRUE(it++->is_register(registers::B));
	EXPECT_TRUE(it++->is_register(registers::C));
	EXPECT_TRUE(it++->is_register(registers::X));
	EXPECT_TRUE(it++->is_register(registers::Y));
	EXPECT_TRUE(it++->is_register(registers::Z));
	EXPECT_TRUE(it++->is_register(registers::I));
	EXPECT_TRUE(it++->is_register(registers::J));
	EXPECT_TRUE(it++->is_register(registers::PC));
	EXPECT_TRUE(it++->is_register(registers::SP));
	EXPECT_TRUE(it++->is_register(registers::EX));

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, DirectiveTest) {
	shared_ptr<lexer::lexer> lexer = run_lexer(".DW .DAT DAT .DB .DP .INCLUDE .INCBIN .FILL .ALIGN .EQU .ORG");
	ASSERT_EQ(12, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_directive(directives::DW));
	EXPECT_TRUE(it++->is_directive(directives::DW));
	EXPECT_TRUE(it++->is_directive(directives::DW));
	EXPECT_TRUE(it++->is_directive(directives::DB));
	EXPECT_TRUE(it++->is_directive(directives::DB));
	EXPECT_TRUE(it++->is_directive(directives::INCLUDE));
	EXPECT_TRUE(it++->is_directive(directives::INCBIN));
	EXPECT_TRUE(it++->is_directive(directives::FILL));
	EXPECT_TRUE(it++->is_directive(directives::ALIGN));
	EXPECT_TRUE(it++->is_directive(directives::EQU));
	EXPECT_TRUE(it++->is_directive(directives::ORG));

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, InstructionTest) {
	shared_ptr<lexer::lexer> lexer = run_lexer("SET ADD SUB MUL MLI DIV DVI MOD MDI AND BOR XOR SHR ASR SHL IFB IFC "
			"IFE IFN IFG IFA IFL IFU ADX SBX STI STD JSR HCF INT IAG IAS RFI IAQ HWN HWQ HWI JMP");
	ASSERT_EQ(39, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_instruction(opcodes::SET));
	EXPECT_TRUE(it++->is_instruction(opcodes::ADD));
	EXPECT_TRUE(it++->is_instruction(opcodes::SUB));
	EXPECT_TRUE(it++->is_instruction(opcodes::MUL));
	EXPECT_TRUE(it++->is_instruction(opcodes::MLI));
	EXPECT_TRUE(it++->is_instruction(opcodes::DIV));
	EXPECT_TRUE(it++->is_instruction(opcodes::DVI));
	EXPECT_TRUE(it++->is_instruction(opcodes::MOD));
	EXPECT_TRUE(it++->is_instruction(opcodes::MDI));
	EXPECT_TRUE(it++->is_instruction(opcodes::AND));
	EXPECT_TRUE(it++->is_instruction(opcodes::BOR));
	EXPECT_TRUE(it++->is_instruction(opcodes::XOR));
	EXPECT_TRUE(it++->is_instruction(opcodes::SHR));
	EXPECT_TRUE(it++->is_instruction(opcodes::ASR));
	EXPECT_TRUE(it++->is_instruction(opcodes::SHL));
	EXPECT_TRUE(it++->is_instruction(opcodes::IFB));
	EXPECT_TRUE(it++->is_instruction(opcodes::IFC));
	EXPECT_TRUE(it++->is_instruction(opcodes::IFE));
	EXPECT_TRUE(it++->is_instruction(opcodes::IFN));
	EXPECT_TRUE(it++->is_instruction(opcodes::IFG));
	EXPECT_TRUE(it++->is_instruction(opcodes::IFA));
	EXPECT_TRUE(it++->is_instruction(opcodes::IFL));
	EXPECT_TRUE(it++->is_instruction(opcodes::IFU));
	EXPECT_TRUE(it++->is_instruction(opcodes::ADX));
	EXPECT_TRUE(it++->is_instruction(opcodes::SBX));
	EXPECT_TRUE(it++->is_instruction(opcodes::STI));
	EXPECT_TRUE(it++->is_instruction(opcodes::STD));
	EXPECT_TRUE(it++->is_instruction(opcodes::JSR));
	EXPECT_TRUE(it++->is_instruction(opcodes::HCF));
	EXPECT_TRUE(it++->is_instruction(opcodes::INT));
	EXPECT_TRUE(it++->is_instruction(opcodes::IAG));
	EXPECT_TRUE(it++->is_instruction(opcodes::IAS));
	EXPECT_TRUE(it++->is_instruction(opcodes::RFI));
	EXPECT_TRUE(it++->is_instruction(opcodes::IAQ));
	EXPECT_TRUE(it++->is_instruction(opcodes::HWN));
	EXPECT_TRUE(it++->is_instruction(opcodes::HWQ));
	EXPECT_TRUE(it++->is_instruction(opcodes::HWI));
	EXPECT_TRUE(it++->is_instruction(opcodes::JMP));

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, DecimalNumber) {
	shared_ptr<lexer::lexer> lexer = run_lexer("100");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(100, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, HexNumberLowercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0xff");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0xff, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, HexNumberUppercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0X1D");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0x1d, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, BinaryNumberLowercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0b1011");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0b1011, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, BinaryNumberUppercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0B10001011");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0b10001011, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, OctalNumberLowercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0o32");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(032, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, OctalNumberUppercase) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0O27");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(027, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, InvalidDecimalNumber) {
	shared_ptr<lexer::lexer> lexer = run_lexer("100a3");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("100a3", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, InvalidHexNumber) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0X100Z3");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0X100Z3", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, OctalWithInvalidNumber) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0o10093");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0o10093", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, OctalWithInvalidLetter) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0o100a3");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0o100a3", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, BinaryWithInvalidNumber) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0b1113");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0b1113", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, BinaryWithInvalidLetter) {
	shared_ptr<lexer::lexer> lexer = run_lexer("0B111a");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0B111a", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, OverflowNumber) {
	stringstream out;
	shared_ptr<lexer::lexer> lexer = run_lexer("4294967296", make_shared<error_handler>(out));
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_TRUE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(UINT32_MAX, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());

	EXPECT_EQ("<Test>:1:1: warning: integer '4294967296' overflows 32-bit intermediary storage\n", out.str());
}

TEST(LexerTest, DecimalNumberAtUint32Max) {
	shared_ptr<lexer::lexer> lexer = run_lexer("4294967295");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(UINT32_MAX, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, Increment) {
	shared_ptr<lexer::lexer> lexer = run_lexer("++");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_increment());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, Decrement) {
	shared_ptr<lexer::lexer> lexer = run_lexer("--");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_decrement());
	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, ShiftLeft) {
	shared_ptr<lexer::lexer> lexer = run_lexer("<<");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_shift_left());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, ShiftRight) {
	shared_ptr<lexer::lexer> lexer = run_lexer(">>");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_shift_right());

	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, Newline) {
	shared_ptr<lexer::lexer> lexer = run_lexer("\n");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->is_newline());
	
	EXPECT_TRUE(it->is_eoi());
}

TEST(LexerTest, SingleCharacters) {
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

TEST(LexerTest, CharacterLiterals) {
	shared_ptr<lexer::lexer> lexer = run_lexer("'a'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	auto it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ('a', it++->get_integer());
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("'\\x4'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0x4, it++->get_integer());
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("'\\X42'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0x42, it++->get_integer());
	EXPECT_TRUE(it->is_eoi());

	lexer = run_lexer("'\\n'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	it = lexer->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ('\n', it++->get_integer());
	EXPECT_TRUE(it->is_eoi());
}

location_ptr makeLocation(uint32_t line, uint32_t column) {
	return make_shared<location>("<Test>", line, column);
}

TEST(LexerTest, MultipleTokens) {
	shared_ptr<lexer::lexer> lexer = run_lexer("set A, b\n  set [J], 0x400\n;a test comment\nlabel: JSR label+4\n");
	ASSERT_EQ(20, lexer->tokens.size());
	EXPECT_FALSE(lexer->error_handler->has_errors());
	EXPECT_FALSE(lexer->error_handler->has_warnings());

	// Line 1
	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->is_instruction(opcodes::SET));
	EXPECT_EQ(makeLocation(1, 1), it++->location);

	EXPECT_TRUE(it->is_register(registers::A));
	EXPECT_EQ(makeLocation(1, 5), it++->location);

	EXPECT_TRUE(it->is_character(','));
	EXPECT_EQ(makeLocation(1, 6), it++->location);

	EXPECT_TRUE(it->is_register(registers::B));
	EXPECT_EQ(makeLocation(1, 8), it++->location);

	EXPECT_TRUE(it->is_newline());
	EXPECT_EQ(makeLocation(1, 9), it++->location);

	// Line 2
	EXPECT_TRUE(it->is_instruction(opcodes::SET));
	EXPECT_EQ(makeLocation(2, 3), it++->location);

	EXPECT_TRUE(it->is_character('['));
	EXPECT_EQ(makeLocation(2, 7), it++->location);

	EXPECT_TRUE(it->is_register(registers::J));
	EXPECT_EQ("J", it->content);
	EXPECT_EQ(makeLocation(2, 8), it++->location);

	EXPECT_TRUE(it->is_character(']'));
	EXPECT_EQ(makeLocation(2, 9), it++->location);

	EXPECT_TRUE(it->is_character(','));
	EXPECT_EQ(makeLocation(2, 10), it++->location);

	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0x400, it->get_integer());
	EXPECT_EQ(makeLocation(2, 12), it++->location);

	EXPECT_TRUE(it->is_newline());
	EXPECT_EQ(makeLocation(2, 17), it++->location);

	// Line 3
	EXPECT_TRUE(it->is_newline());
	EXPECT_EQ(makeLocation(3, 16), it++->location);

	// Line 4
	EXPECT_TRUE(it->is_label());
	EXPECT_EQ("label", it->content);
	EXPECT_EQ(makeLocation(4, 1), it++->location);

	EXPECT_TRUE(it->is_instruction(opcodes::JSR));
	EXPECT_EQ(makeLocation(4, 8), it++->location);

	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ("label", it->content);
	EXPECT_EQ(makeLocation(4, 12), it++->location);

	EXPECT_TRUE(it->is_character('+'));
	EXPECT_EQ(makeLocation(4, 17), it++->location);

	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(4, it->get_integer());
	EXPECT_EQ(makeLocation(4, 18), it++->location);

	EXPECT_TRUE(it->is_newline());
	EXPECT_EQ(makeLocation(4, 19), it++->location);

	// Line 5
	EXPECT_TRUE(it->is_eoi());
	EXPECT_EQ(makeLocation(5, 0), it->location);
}

