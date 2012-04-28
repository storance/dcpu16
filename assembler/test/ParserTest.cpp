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

typedef function<void (ArgumentPtr&)> ArgumentFunc;
typedef function<void (ExpressionPtr&)> ExpressionFunc;


void assertLabel(StatementIterator &it, const string &expectedName) {
	auto& statement = *it++;

	Label* label = dynamic_cast<Label*>(statement.get());
	ASSERT_TRUE(label != nullptr);
	EXPECT_EQ(expectedName, label->_name);
}

void assertInstruction(StatementIterator &it, Opcode opcode, ArgumentFunc verifyA) {
	auto& statement = *it++;

	Instruction* instruction = dynamic_cast<ast::Instruction*>(statement.get());
	ASSERT_TRUE(instruction != nullptr);
	EXPECT_EQ(opcode, instruction->_opcode);

	{
		SCOPED_TRACE("Argument b"); 
		EXPECT_FALSE(instruction->_b);
	}
	{
		SCOPED_TRACE("Argument a"); 
		verifyA(instruction->_a);
	}
}

void assertInstruction(StatementIterator &it, Opcode opcode, ArgumentFunc verifyB, ArgumentFunc verifyA) {
	auto& statement = *it++;

	Instruction* instruction = dynamic_cast<ast::Instruction*>(statement.get());
	ASSERT_TRUE(instruction != nullptr);
	EXPECT_EQ(opcode, instruction->_opcode);

	{
		SCOPED_TRACE("Argument b"); 
		verifyB(instruction->_b);
	}
	{
		SCOPED_TRACE("Argument a"); 
		verifyA(instruction->_a);
	}
}

ArgumentFunc assertArgumentIsExpression(ExpressionFunc assertFunc) {
	return [=] (ArgumentPtr& arg) {
		ExpressionArgument *exprArg = dynamic_cast<ExpressionArgument*>(arg.get());
		ASSERT_TRUE(exprArg != nullptr);
		{
			SCOPED_TRACE("Verify Expression Argument");
			assertFunc(exprArg->_expr);
		}
	};
}

ArgumentFunc assertArgumentIsIndirect(ExpressionFunc assertFunc) {
	return [=] (ArgumentPtr& arg) {
		IndirectArgument *exprArg = dynamic_cast<IndirectArgument*>(arg.get());

		ASSERT_TRUE(exprArg != nullptr);
		{
			SCOPED_TRACE("Verify Indirect Argument");
			assertFunc(exprArg->_expr);
		}
	};
}

ArgumentFunc assertArgumentIsStack(StackOperation operation) {
	return [=] (ArgumentPtr& arg) {
		StackArgument *stackArg = dynamic_cast<StackArgument*>(arg.get());

		ASSERT_TRUE(stackArg != nullptr);
		EXPECT_EQ(operation, stackArg->_operation);
	};
}

ArgumentFunc assertArgumentIsNull() {
	return [=] (ArgumentPtr& arg) {
		EXPECT_FALSE(arg);
	};
}

TEST(ParserTest, InstructionTest) {
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("SET A, B\nADD A, B\nSUB A, B\nMUL A, B\nMLI A, B\nDIV A, B\nDVI A, B\nMOD A, B\n"
		"AND A, B\nBOR A, B\nXOR A, B\nSHR A, B\nASR A, B\nSHL A, B\nSTI A, B\nIFB A, B\nIFC A, B\nIFE A, B\nIFN A, B\n"
		"IFG A, B\nIFA A, B\nIFL A, B\nIFU A, B\nADX A, B\nSBX A, B\nJSR A\nHCF A\nINT A\nIAG A\nIAS A\nHWN A\nHWQ A\n"
		"HWI A\nJMP A\nMDI A, B\nSTD A, B\nRFI A\n IAQ A\n", 38, parser));

	auto it = parser->statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::ADD,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertInstruction(it, Opcode::SUB,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 4"); 
		assertInstruction(it, Opcode::MUL,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 5"); 
		assertInstruction(it, Opcode::MLI,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 6"); 
		assertInstruction(it, Opcode::DIV,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 7"); 
		assertInstruction(it, Opcode::DVI,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 8"); 
		assertInstruction(it, Opcode::MOD,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 9"); 
		assertInstruction(it, Opcode::AND,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 10"); 
		assertInstruction(it, Opcode::BOR,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 11"); 
		assertInstruction(it, Opcode::XOR,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 12"); 
		assertInstruction(it, Opcode::SHR,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 13"); 
		assertInstruction(it, Opcode::ASR,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 14"); 
		assertInstruction(it, Opcode::SHL,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 15"); 
		assertInstruction(it, Opcode::STI,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 16"); 
		assertInstruction(it, Opcode::IFB,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 16"); 
		assertInstruction(it, Opcode::IFC,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 18"); 
		assertInstruction(it, Opcode::IFE,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 19"); 
		assertInstruction(it, Opcode::IFN,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 20"); 
		assertInstruction(it, Opcode::IFG,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 21"); 
		assertInstruction(it, Opcode::IFA,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 22"); 
		assertInstruction(it, Opcode::IFL,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}


	{
		SCOPED_TRACE("Statement: 23"); 
		assertInstruction(it, Opcode::IFU,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 24"); 
		assertInstruction(it, Opcode::ADX,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 25"); 
		assertInstruction(it, Opcode::SBX,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 26"); 
		assertInstruction(it, Opcode::JSR,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 27"); 
		assertInstruction(it, Opcode::HCF,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 28"); 
		assertInstruction(it, Opcode::INT,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 29"); 
		assertInstruction(it, Opcode::IAG,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 30"); 
		assertInstruction(it, Opcode::IAS,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 31"); 
		assertInstruction(it, Opcode::HWN,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 32"); 
		assertInstruction(it, Opcode::HWQ,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 33"); 
		assertInstruction(it, Opcode::HWI,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 34"); 
		assertInstruction(it, Opcode::JMP,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 35"); 
		assertInstruction(it, Opcode::MDI,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 36"); 
		assertInstruction(it, Opcode::STD,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 37"); 
		assertInstruction(it, Opcode::RFI,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 38"); 
		assertInstruction(it, Opcode::IAQ,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}
}

TEST(ParserTest, LabelTest) {
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("label1 :\n:label2\nlabel3: SET A, B\n: label4 SET A, B", 6, parser));

	auto it = parser->statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertLabel(it, "label1");
		EXPECT_EQ(0, *parser->symbolTable->lookup("label1"));
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertLabel(it, "label2");
		EXPECT_EQ(0, *parser->symbolTable->lookup("label2"));
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertLabel(it, "label3");
		EXPECT_EQ(0, *parser->symbolTable->lookup("label3"));
	}

	{
		SCOPED_TRACE("Statement: 4"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 5"); 
		assertLabel(it, "label4");
		EXPECT_EQ(1, *parser->symbolTable->lookup("label4"));
	}

	{
		SCOPED_TRACE("Statement: 6"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}
}

TEST(ParserTest, RegisterTest) {
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("SET A, A\nSET B, B\nSET C, C\nSET X, X\nSET Y, Y\n"
		"SET Z, Z\nSET I, I\nSET J, J\nSET PC, PC\nSET SP, SP\nSET EX, EX\n", 11, parser));

	auto it = parser->statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)));
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::C)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::C)));
	}

	{
		SCOPED_TRACE("Statement: 4"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::X)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::X)));
	}

	{
		SCOPED_TRACE("Statement: 5"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::Y)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::Y)));
	}

	{
		SCOPED_TRACE("Statement: 6"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::Z)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::Z)));
	}

	{
		SCOPED_TRACE("Statement: 7"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::I)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::I)));
	}

	{
		SCOPED_TRACE("Statement: 8"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::J)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::J)));
	}

	{
		SCOPED_TRACE("Statement: 9"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::PC)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::PC)));
	}

	{
		SCOPED_TRACE("Statement: 10"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::SP)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::SP)));
	}

	{
		SCOPED_TRACE("Statement: 11"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::EX)),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::EX)));
	}
}

TEST(ParserTest, SimpleExpressionTest) {
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("set 4 * 2, 1 + 2", 1, parser));
	auto it = parser->statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedLiteral(8)),
			assertArgumentIsExpression(assertIsEvaluatedLiteral(3)));
	}
}

TEST(ParserTest, IndirectionTest) {
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("set [A], [1 * 2]", 1, parser));
	auto it = parser->statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsIndirect(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsIndirect(assertIsEvaluatedLiteral(2))
		);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("set [B - 4], [5 + J]", 1, parser));
	it = parser->statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsIndirect(
				assertIsEvaluatedRegister(Register::B, true, -4)),
			assertArgumentIsIndirect(
				assertIsEvaluatedRegister(Register::J, true, 5))
		);
	}
}

TEST(ParserTest, LabelReferencesTest) {
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("label: set label, [label * 2]\n:a SET a, $a", 4, parser));
	auto it = parser->statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertLabel(it, "label");
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsLabelRef("label")),
			assertArgumentIsIndirect(
				assertIsBinaryOperation(BinaryOperator::MULTIPLY,
					assertIsLabelRef("label"),
					assertIsLiteral(2)
				)
			)
		);
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertLabel(it, "a");
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsExpression(assertIsLabelRef("a")));
	}
}

TEST(ParserTest, StackArgumentsTest) {
	std::shared_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("set A, [SP]\nset B, [SP++]\nset [--SP] , C", 3, parser));
	auto it = parser->statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsStack(StackOperation::PEEK));
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)),
			assertArgumentIsStack(StackOperation::POP));
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsStack(StackOperation::PUSH),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::C)));
	}

	ASSERT_NO_FATAL_FAILURE(runParser("set A, PEEK\nset B, POP\nset PUSH , C\nset PICK 5, PICK 1 + 2", 4, parser));
	it = parser->statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::A)),
			assertArgumentIsStack(StackOperation::PEEK));
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::B)),
			assertArgumentIsStack(StackOperation::POP));
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsStack(StackOperation::PUSH),
			assertArgumentIsExpression(assertIsEvaluatedRegister(Register::C)));
	}

	{
		SCOPED_TRACE("Statement: 4"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsIndirect(assertIsEvaluatedRegister(Register::SP, true, 5)),
			assertArgumentIsIndirect(assertIsEvaluatedRegister(Register::SP, true, 3))
		);
	}
}
