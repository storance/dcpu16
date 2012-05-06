#include <iostream>
#include <list>
#include <gtest/gtest.h>

#include <parser.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::parser;
using namespace dcpu::lexer;

void run_parser(const string &content, int expected_statements, statement_list &statements) {
	lexer::lexer lex(content, "<Test>");
	lex.parse();

	statements.clear();
	parser::parser _parser(lex, statements);
	_parser.parse();

	ASSERT_EQ(expected_statements, statements.size());
}

TEST(ParserTest, InstructionTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	statement_list statements;

	ASSERT_NO_FATAL_FAILURE(run_parser("SET A, B\nADD A, B\nSUB A, B\nMUL A, B\nMLI A, B\nDIV A, B\nDVI A, B\nMOD A, B\n"
		"MDI A, B\nAND A, B\nBOR A, B\nXOR A, B\nSHR A, B\nASR A, B\nSHL A, B\nSTI A, B\nSTD A, B\nIFB A, B\nIFC A, B\n"
		"IFE A, B\nIFN A, B\nIFG A, B\nIFA A, B\nIFL A, B\nIFU A, B\nADX A, B\nSBX A, B\nJSR B\nHCF B\nINT B\nIAG B\n"
		"IAS B\nRFI B\n IAQ B\nHWN B\nHWQ B\nHWI B\nJMP B\n", 38, statements));

	auto it = statements.begin();

	argument arg_a(expression_argument(location, argument_position::A, evaluated_expression(location, registers::B),
			false, false));
	argument arg_b(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A),
			false, false));
	optional_argument no_arg;

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::ADD, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SUB, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::MUL, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::MLI, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::DIV, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::DVI, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::MOD, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::MDI, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::AND, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::BOR, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::XOR, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SHR, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::ASR, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SHL, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::STI, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::STD, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFB, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFC, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFE, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFN, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFG, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFA, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFL, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IFU, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::ADX, arg_a, arg_b)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SBX, arg_a, arg_b)));


	EXPECT_EQ(*it++, statement(instruction(location, opcodes::JSR, arg_a, no_arg)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::HCF, arg_a, no_arg)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::INT, arg_a, no_arg)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IAG, arg_a, no_arg)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IAS, arg_a, no_arg)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::RFI, arg_a, no_arg)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::IAQ, arg_a, no_arg)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::HWN, arg_a, no_arg)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::HWQ, arg_a, no_arg)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::HWI, arg_a, no_arg)));
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::JMP, arg_a, no_arg)));
}

TEST(ParserTest, LabelTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	statement_list statements;

	ASSERT_NO_FATAL_FAILURE(run_parser("label1:\n:..@label2\n.label3: SET A, B\n:label4 SET A, B", 6, statements));

	auto it = statements.begin();

	EXPECT_EQ(*it++, statement(label(location, "label1", label_type::Global)));

	EXPECT_EQ(*it++, statement(label(location, "..@label2", label_type::GlobalNoAttach)));

	EXPECT_EQ(*it++, statement(label(location, ".label3", label_type::Local)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::B),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(label(location, "label4", label_type::Global)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::B),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A),
				false, false))
	)));
}

TEST(ParserTest, DataTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	statement_list statements;

	ASSERT_NO_FATAL_FAILURE(run_parser("dat \"hello, world!\"\n", 1, statements));

	auto it = statements.begin();
	EXPECT_EQ(*it++, statement(data(location, {'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'})));

	ASSERT_NO_FATAL_FAILURE(run_parser(".dw 0x40, 42, 0\n", 1, statements));

	it = statements.begin();
	EXPECT_EQ(*it++, statement(data(location, {0x40, 42, 0})));
}

TEST(ParserTest, RegisterTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	statement_list statements;

	ASSERT_NO_FATAL_FAILURE(run_parser("SET A, A\nSET B, B\nSET C, C\nSET X, X\nSET Y, Y\n"
		"SET Z, Z\nSET I, I\nSET J, J\nSET PC, PC\nSET SP, SP\nSET EX, EX\n", 11, statements));

	auto it = statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::A),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::B),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::B),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::C),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::C),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::X),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::X),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::Y),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::Y),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::Z),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::Z),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::I),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::I),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::J),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::J),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::PC),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::PC),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::SP),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::SP),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::EX),
				false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::EX),
				false, false))
	)));
}

TEST(ParserTest, SimpleExpressionTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	statement_list statements;

	ASSERT_NO_FATAL_FAILURE(run_parser("set 4 * 2, 1 + 2", 1, statements));
	auto it = statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, 3), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, 8), false, false))
	)));
}

TEST(ParserTest, IndirectionTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	statement_list statements;

	ASSERT_NO_FATAL_FAILURE(run_parser("set [A], [1 * 2]", 1, statements));
	auto it = statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, 2), true, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A),
				true, false))
	)));

	ASSERT_NO_FATAL_FAILURE(run_parser("set [B - 4], [5 + J]", 1, statements));
	it = statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::J, 5),
				true, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::B, -4),
				true, false))
	)));
}

TEST(ParserTest, LabelReferencesTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	statement_list statements;

	ASSERT_NO_FATAL_FAILURE(run_parser("label: set label, [label * 2]\n:a SET a, $a", 4, statements));
	auto it = statements.begin();

	EXPECT_EQ(*it++, statement(label(location, "label", label_type::Global)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A,
				binary_operation(location, binary_operator::MULTIPLY,
						symbol_operand(location, "label"), literal_operand(location, 2)),
				true, false)
		), argument(expression_argument(location, argument_position::B, symbol_operand(location, "label"),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(label(location, "a", label_type::Global)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, symbol_operand(location, "a"), false, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A),
				false, false))
	)));
}

TEST(ParserTest, StackArgumentsTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);
	statement_list statements;

	ASSERT_NO_FATAL_FAILURE(run_parser("set A, [SP]\nset B, [SP++]\nset [--SP] , C", 3, statements));
	auto it = statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::SP),
				true, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(stack_argument(location, argument_position::A, stack_operation::POP)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::B),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::C),
				false, false)),
		argument(stack_argument(location, argument_position::A, stack_operation::PUSH))
	)));

	ASSERT_NO_FATAL_FAILURE(run_parser("set A, PEEK\nset B, POP\nset PUSH , C\nset PICK 5, PICK 1 + 2", 4, statements));

	it = statements.begin();
	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(stack_argument(location, argument_position::A, stack_operation::PEEK)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::A),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(stack_argument(location, argument_position::A, stack_operation::POP)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::B),
				false, false))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::C),
				false, false)),
		argument(stack_argument(location, argument_position::A, stack_operation::PUSH))
	)));

	EXPECT_EQ(*it++, statement(instruction(location, opcodes::SET,
		argument(expression_argument(location, argument_position::A, evaluated_expression(location, registers::SP, 3),
				true, false)),
		argument(expression_argument(location, argument_position::B, evaluated_expression(location, registers::SP, 5),
				true, false))
	)));
}
