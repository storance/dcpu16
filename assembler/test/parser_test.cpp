#include <iostream>
#include <list>
#include <gtest/gtest.h>

#include <parser.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::parser;
using namespace dcpu::lexer;

void run_parser(const string &content, int expected_statements, shared_ptr<parser::parser> &_parser) {
	lexer::lexer lex(content, "<Test>");
	lex.parse();

	_parser = make_shared<parser::parser>(lex);
	_parser->parse();

	ASSERT_EQ(expected_statements, _parser->statements.size());
}

TEST(ParserTest, InstructionTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	std::shared_ptr<parser::parser> _parser;

	ASSERT_NO_FATAL_FAILURE(run_parser("SET A, B\nADD A, B\nSUB A, B\nMUL A, B\nMLI A, B\nDIV A, B\nDVI A, B\nMOD A, B\n"
		"MDI A, B\nAND A, B\nBOR A, B\nXOR A, B\nSHR A, B\nASR A, B\nSHL A, B\nSTI A, B\nSTD A, B\nIFB A, B\nIFC A, B\n"
		"IFE A, B\nIFN A, B\nIFG A, B\nIFA A, B\nIFL A, B\nIFU A, B\nADX A, B\nSBX A, B\nJSR A\nHCF A\nINT A\nIAG A\n"
		"IAS A\nRFI A\n IAQ A\nHWN A\nHWQ A\nHWI A\nJMP A\n", 38, _parser));

	auto it = _parser->statements.begin();

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::ADD,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SUB,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::MUL,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::MLI,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::DIV,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::DVI,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::MOD,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::MDI,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::AND,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::BOR,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::XOR,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SHR,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::ASR,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SHL,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::STI,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::STD,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFB,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFC,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFE,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFN,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFG,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFA,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFL,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFU,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::ADX,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SBX,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::JSR,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::HCF,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::INT,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IAG,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IAS,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::RFI,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IAQ,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::HWN,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::HWQ,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::HWI,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::JMP,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false),
		optional_argument()
	)));
}

TEST(ParserTest, LabelTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	std::shared_ptr<parser::parser> _parser;

	ASSERT_NO_FATAL_FAILURE(run_parser("label1:\n:..@label2\n.label3: SET A, B\n:label4 SET A, B", 6, _parser));

	auto it = _parser->statements.begin();

	EXPECT_EQ(*it++, statement(label(location, "label1", label_type::Global)));

	EXPECT_EQ(*it++, statement(label(location, "..@label2", label_type::GlobalNoAttach)));

	EXPECT_EQ(*it++, statement(label(location, ".label3", label_type::Local)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(label(location, "label4", label_type::Global)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));
}

TEST(ParserTest, RegisterTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	std::shared_ptr<parser::parser> _parser;

	ASSERT_NO_FATAL_FAILURE(run_parser("SET A, A\nSET B, B\nSET C, C\nSET X, X\nSET Y, Y\n"
		"SET Z, Z\nSET I, I\nSET J, J\nSET PC, PC\nSET SP, SP\nSET EX, EX\n", 11, _parser));

	auto it = _parser->statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::A), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::B), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::B), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::C), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::C), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::X), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::X), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::Y), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::Y), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::Z), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::Z), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::I), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::I), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::J), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::J), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::PC), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::PC), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::SP), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::SP), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::EX), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::EX), false, false))
	)));
}

TEST(ParserTest, SimpleExpressionTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	std::shared_ptr<parser::parser> _parser;

	ASSERT_NO_FATAL_FAILURE(run_parser("set 4 * 2, 1 + 2", 1, _parser));
	auto it = _parser->statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, 3), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, 8), false, false))
	)));
}

TEST(ParserTest, IndirectionTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	std::shared_ptr<parser::parser> _parser;

	ASSERT_NO_FATAL_FAILURE(run_parser("set [A], [1 * 2]", 1, _parser));
	auto it = _parser->statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, 2), true, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), true, false))
	)));

	ASSERT_NO_FATAL_FAILURE(run_parser("set [B - 4], [5 + J]", 1, _parser));
	it = _parser->statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::J, 5), true, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::B, -4), true, false))
	)));
}

TEST(ParserTest, LabelReferencesTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	std::shared_ptr<parser::parser> _parser;

	ASSERT_NO_FATAL_FAILURE(run_parser("label: set label, [label * 2]\n:a SET a, $a", 4, _parser));
	auto it = _parser->statements.begin();

	EXPECT_EQ(*it++, statement(label(location, "label", label_type::Global)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, binary_operation(location, binary_operator::MULTIPLY,
			symbol_operand(location, "label"), literal_operand(location, 2)), true, false
		)),
		argument(expression_argument(location, argument_position::B, symbol_operand(location, "label"), false, false))
	)));

	EXPECT_EQ(*it++, statement(label(location, "a", label_type::Global)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, symbol_operand(location, "a"), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));
}

TEST(ParserTest, StackArgumentsTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	std::shared_ptr<parser::parser> _parser;

	ASSERT_NO_FATAL_FAILURE(run_parser("set A, [SP]\nset B, [SP++]\nset [--SP] , C", 3, _parser));
	auto it = _parser->statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(stack_argument(location, argument_position::A, stack_operation::PEEK)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(stack_argument(location, argument_position::A, stack_operation::POP)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::B), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::C), false, false)),
		argument(stack_argument(location, argument_position::A, stack_operation::PUSH))
	)));

	ASSERT_NO_FATAL_FAILURE(run_parser("set A, PEEK\nset B, POP\nset PUSH , C\nset PICK 5, PICK 1 + 2", 4, _parser));

	it = _parser->statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(stack_argument(location, argument_position::A, stack_operation::PEEK)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(stack_argument(location, argument_position::A, stack_operation::POP)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::B), false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::C), false, false)),
		argument(stack_argument(location, argument_position::A, stack_operation::PUSH))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::SP, 3), true, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::SP, 5), true, false))
	)));
}
