#include <iostream>
#include <list>
#include <gtest/gtest.h>

#include "Utils.hpp"
#include "../Parser.hpp"
#include "../SymbolTable.hpp"

using namespace std;
using namespace std::placeholders;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::parser;
using namespace dcpu::lexer;

TEST(ParserTest, InstructionTest) {
	Location location("<Test>", 1, 1);
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("SET A, B\nADD A, B\nSUB A, B\nMUL A, B\nMLI A, B\nDIV A, B\nDVI A, B\nMOD A, B\n"
		"MDI A, B\nAND A, B\nBOR A, B\nXOR A, B\nSHR A, B\nASR A, B\nSHL A, B\nSTI A, B\nSTD A, B\nIFB A, B\nIFC A, B\n"
		"IFE A, B\nIFN A, B\nIFG A, B\nIFA A, B\nIFL A, B\nIFU A, B\nADX A, B\nSBX A, B\nJSR A\nHCF A\nINT A\nIAG A\n"
		"IAS A\nRFI A\n IAQ A\nHWN A\nHWQ A\nHWI A\nJMP A\n", 38, parser));

	auto it = parser->statements.begin();

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::ADD,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SUB,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::MUL,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::MLI,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::DIV,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::DVI,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::MOD,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::MDI,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::AND,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::BOR,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::XOR,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SHR,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::ASR,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SHL,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::STI,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::STD,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IFB,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IFC,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IFE,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IFN,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IFG,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IFA,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IFL,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IFU,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::ADX,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SBX,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::JSR,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::HCF,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::INT,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IAG,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IAS,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::RFI,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::IAQ,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::HWN,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::HWQ,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::HWI,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::JMP,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null()
	));
}

TEST(ParserTest, LabelTest) {
	Location location("<Test>", 1, 1);
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("label1 :\n:label2\nlabel3: SET A, B\n: label4 SET A, B", 6, parser));

	auto it = parser->statements.begin();

	EXPECT_EQ(*it++, Statement::label(location, "label1", LabelType::Global));

	EXPECT_EQ(*it++, Statement::label(location, "label2", LabelType::Global));

	EXPECT_EQ(*it++, Statement::label(location, "label3", LabelType::Global));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::label(location, "label4", LabelType::Global));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	ASSERT_TRUE(parser->symbolTable->lookup("label1"));
	EXPECT_EQ(0, *parser->symbolTable->lookup("label1"));

	ASSERT_TRUE(parser->symbolTable->lookup("label2"));
	EXPECT_EQ(0, *parser->symbolTable->lookup("label2"));

	ASSERT_TRUE(parser->symbolTable->lookup("label3"));
	EXPECT_EQ(0, *parser->symbolTable->lookup("label3"));

	ASSERT_TRUE(parser->symbolTable->lookup("label4"));
	EXPECT_EQ(1, *parser->symbolTable->lookup("label4"));
}

TEST(ParserTest, RegisterTest) {
	Location location("<Test>", 1, 1);
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("SET A, A\nSET B, B\nSET C, C\nSET X, X\nSET Y, Y\n"
		"SET Z, Z\nSET I, I\nSET J, J\nSET PC, PC\nSET SP, SP\nSET EX, EX\n", 11, parser));

	auto it = parser->statements.begin();
	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::B))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::C)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::C))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::X)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::X))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::Y)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::Y))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::Z)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::Z))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::I)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::I))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::J)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::J))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::PC)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::PC))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::SP)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::SP))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::EX)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::EX))
	));
}

TEST(ParserTest, SimpleExpressionTest) {
	Location location("<Test>", 1, 1);
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("set 4 * 2, 1 + 2", 1, parser));
	auto it = parser->statements.begin();
	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedLiteral(location, 3)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedLiteral(location, 8))
	));
}

TEST(ParserTest, IndirectionTest) {
	Location location("<Test>", 1, 1);
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("set [A], [1 * 2]", 1, parser));
	auto it = parser->statements.begin();
	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::indirect(ArgumentPosition::A, Expression::evaluatedLiteral(location, 2)),
		Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	ASSERT_NO_FATAL_FAILURE(runParser("set [B - 4], [5 + J]", 1, parser));
	it = parser->statements.begin();
	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::indirect(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::J, 5)),
		Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::B, -4))
	));
}

TEST(ParserTest, LabelReferencesTest) {
	Location location("<Test>", 1, 1);
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("label: set label, [label * 2]\n:a SET a, $a", 4, parser));
	auto it = parser->statements.begin();

	EXPECT_EQ(*it++, Statement::label(location, "label", LabelType::Global));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::indirect(ArgumentPosition::A, Expression::binaryOperation(location, BinaryOperator::MULTIPLY,
			Expression::labelOperand(location, "label"),
			Expression::literalOperand(location, 2))
		),
		Argument::expression(ArgumentPosition::B, Expression::labelOperand(location, "label"))
	));

	EXPECT_EQ(*it++, Statement::label(location, "a", LabelType::Global));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::labelOperand(location, "a")),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));
}

TEST(ParserTest, StackArgumentsTest) {
	Location location("<Test>", 1, 1);
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("set A, [SP]\nset B, [SP++]\nset [--SP] , C", 3, parser));
	auto it = parser->statements.begin();
	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::stackPeek(location, ArgumentPosition::A),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::stackPop(location, ArgumentPosition::A),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::B))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::C)),
		Argument::stackPush(location, ArgumentPosition::B)
	));

	ASSERT_NO_FATAL_FAILURE(runParser("set A, PEEK\nset B, POP\nset PUSH , C\nset PICK 5, PICK 1 + 2", 4, parser));

	it = parser->statements.begin();
	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::stackPeek(location, ArgumentPosition::A),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::stackPop(location, ArgumentPosition::A),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::B))
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::C)),
		Argument::stackPush(location, ArgumentPosition::B)
	));

	EXPECT_EQ(*it++, Statement::instruction(location, Opcode::SET,
		Argument::indirect(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::SP, 3)),
		Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::SP, 5))
	));
}
