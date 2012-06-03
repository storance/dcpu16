#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

#include <lexer.hpp>

using namespace std;
using namespace dcpu::assembler;

static dcpu::assembler::log default_logger;

shared_ptr<lexer> run_lexer(const string &content, dcpu::assembler::log &logger=default_logger) {
	shared_ptr<lexer> lex = make_shared<lexer>(content, "<Test>", logger);
	lex->parse();

	return lex;
}

TEST(Lexer, SymbolStartsUnderscore) {
	shared_ptr<lexer> lex = run_lexer("_a1_?.$#@");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ("_a1_?.$#@", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, SymbolStartsPeriod) {
	shared_ptr<lexer> lex = run_lexer(".aaa111");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ(".aaa111", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, SymbolStartsQuestion) {
	shared_ptr<lexer> lex = run_lexer("?aaa111");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ("?aaa111", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, SymbolStartsLetter) {
	shared_ptr<lexer> lex = run_lexer("aaa111");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ("aaa111", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, ExplicitSymbol) {
	shared_ptr<lexer> lex = run_lexer("$pc");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_symbol());
	EXPECT_EQ("pc", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, QuotedString) {
	shared_ptr<lexer> lex = run_lexer("\"hello, world!\"");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_quoted_string());
	EXPECT_EQ(quote_type::DOUBLE_QUOTE, it->get_quote_type());
	EXPECT_EQ("hello, world!", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, LabelColonSuffix) {
	shared_ptr<lexer> lex = run_lexer("_a1_?.$#@:");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_label());
	EXPECT_EQ("_a1_?.$#@", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, LabelColonPrefix) {
	shared_ptr<lexer> lex = run_lexer(":_a1_?.$#@");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_label());
	EXPECT_EQ("_a1_?.$#@", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, Register) {
	shared_ptr<lexer> lex = run_lexer("A B C X Y Z I J PC SP EX");
	ASSERT_EQ(12, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
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

TEST(Lexer, Directive) {
	shared_ptr<lexer> lex = run_lexer(".DW .DAT DAT .DB .DP .INCLUDE .INCBIN .FILL .ALIGN .EQU .ORG");
	ASSERT_EQ(12, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
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

TEST(Lexer, Instruction) {
	shared_ptr<lexer> lex = run_lexer("SET ADD SUB MUL MLI DIV DVI MOD MDI AND BOR XOR SHR ASR SHL IFB IFC "
			"IFE IFN IFG IFA IFL IFU ADX SBX STI STD JSR HCF INT IAG IAS RFI IAQ HWN HWQ HWI JMP");
	ASSERT_EQ(39, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
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

TEST(Lexer, StackOperation) {
	shared_ptr<lexer> lex = run_lexer("PUSH POP PEEK PICK [  SP] [  --     SP    ] [SP++]");
	ASSERT_EQ(8, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_stack_operation(stack_operation::PUSH));
	EXPECT_TRUE(it++->is_stack_operation(stack_operation::POP));
	EXPECT_TRUE(it++->is_stack_operation(stack_operation::PEEK));
	EXPECT_TRUE(it++->is_stack_operation(stack_operation::PICK));
	EXPECT_TRUE(it++->is_stack_operation(stack_operation::PEEK));
	EXPECT_TRUE(it++->is_stack_operation(stack_operation::PUSH));
	EXPECT_TRUE(it++->is_stack_operation(stack_operation::POP));

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, DecimalNumber) {
	shared_ptr<lexer> lex = run_lexer("100");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(100, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, HexNumberLowercase) {
	shared_ptr<lexer> lex = run_lexer("0xff");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0xff, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, HexNumberUppercase) {
	shared_ptr<lexer> lex = run_lexer("0X1D");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0x1d, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, BinaryNumberLowercase) {
	shared_ptr<lexer> lex = run_lexer("0b1011");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0b1011, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, BinaryNumberUppercase) {
	shared_ptr<lexer> lex = run_lexer("0B10001011");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0b10001011, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, OctalNumberLowercase) {
	shared_ptr<lexer> lex = run_lexer("0o32");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(032, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, OctalNumberUppercase) {
	shared_ptr<lexer> lex = run_lexer("0O27");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(027, it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, InvalidDecimalNumber) {
	shared_ptr<lexer> lex = run_lexer("100a3");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("100a3", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, InvalidHexNumber) {
	shared_ptr<lexer> lex = run_lexer("0X100Z3");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0X100Z3", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, OctalWithInvalidNumber) {
	shared_ptr<lexer> lex = run_lexer("0o10093");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0o10093", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, OctalWithInvalidLetter) {
	shared_ptr<lexer> lex = run_lexer("0o100a3");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0o100a3", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, BinaryWithInvalidNumber) {
	shared_ptr<lexer> lex = run_lexer("0b1113");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0b1113", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, BinaryWithInvalidLetter) {
	shared_ptr<lexer> lex = run_lexer("0B111a");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it->is_invalid_integer());
	EXPECT_EQ("0B111a", it++->content);

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, OverflowNumber) {
	stringstream out;
	log capturing_logger(out);

	shared_ptr<lexer> lex = run_lexer("4294967296", capturing_logger);
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_TRUE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(numeric_limits<uint32_t>::max(), it++->get_integer());

	EXPECT_TRUE(it->is_eoi());

	EXPECT_EQ("<Test>:1:1: warning: overflow converting '4294967296' to a 32-bit integer\n", out.str());
}

TEST(Lexer, DecimalNumberAtUint32Max) {
	shared_ptr<lexer> lex = run_lexer("4294967295");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(numeric_limits<uint32_t>::max(), it++->get_integer());

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, Operator) {
	shared_ptr<lexer> lex = run_lexer("<< >> && || == != <> <= >=");
	ASSERT_EQ(10, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_operator(operator_type::SHIFT_LEFT));
	EXPECT_TRUE(it++->is_operator(operator_type::SHIFT_RIGHT));
	EXPECT_TRUE(it++->is_operator(operator_type::AND));
	EXPECT_TRUE(it++->is_operator(operator_type::OR));
	EXPECT_TRUE(it++->is_operator(operator_type::EQ));
	EXPECT_TRUE(it++->is_operator(operator_type::NEQ));
	EXPECT_TRUE(it++->is_operator(operator_type::NEQ));
	EXPECT_TRUE(it++->is_operator(operator_type::LTE));
	EXPECT_TRUE(it++->is_operator(operator_type::GTE));

	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, Newline) {
	shared_ptr<lexer> lex = run_lexer("\n");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_newline());
	
	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, SingleCharacters) {
	shared_ptr<lexer> lex = run_lexer("@");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_character('@'));
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer(",");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_character(','));
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer("$");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_character('$'));
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer("[");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_character('['));
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer("]");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_character(']'));
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer("(");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_character('('));
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer(")");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_character(')'));
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer("+");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_character('+'));
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer("-");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	EXPECT_TRUE(it++->is_character('-'));
	EXPECT_TRUE(it->is_eoi());
}

TEST(Lexer, CharacterLiterals) {
	shared_ptr<lexer> lex = run_lexer("'a'");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	auto it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ('a', it++->get_integer());
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer("'\\x4'");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0x4, it++->get_integer());
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer("'\\X42'");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ(0x42, it++->get_integer());
	EXPECT_TRUE(it->is_eoi());

	lex = run_lexer("'\\n'");
	ASSERT_EQ(2, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	it = lex->tokens.begin();
	ASSERT_TRUE(it->is_integer());
	EXPECT_EQ('\n', it++->get_integer());
	EXPECT_TRUE(it->is_eoi());
}

location_ptr makeLocation(uint32_t line, uint32_t column) {
	return make_shared<location>("<Test>", line, column);
}

TEST(Lexer, MultipleTokens) {
	shared_ptr<lexer> lex = run_lexer("set A, b\n  set [J], 0x400\n;a test comment\nlabel: JSR label+4\n");
	ASSERT_EQ(20, lex->tokens.size());
	EXPECT_FALSE(lex->logger.has_errors());
	EXPECT_FALSE(lex->logger.has_warnings());

	// Line 1
	auto it = lex->tokens.begin();
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

