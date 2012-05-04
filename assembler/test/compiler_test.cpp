#include <iostream>
#include <list>
#include <gtest/gtest.h>

#include <statement.hpp>
#include <compiler.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::compiler;
using namespace dcpu::ast;
using namespace dcpu::lexer;

static location_ptr test_location = make_shared<location>("<Test>", 1, 1);

TEST(ArgumentCompileTest, RegisterA) {
	argument arg(expression_argument(test_location, argument_position::B,
			evaluated_expression(test_location, registers::A), false, false));
	EXPECT_EQ(compile_result(0x0), compile(arg));
}

TEST(ArgumentCompileTest, RegisterB) {
	argument arg(expression_argument(test_location, argument_position::B,
			evaluated_expression(test_location, registers::B), false, false));
	EXPECT_EQ(compile_result(0x1), compile(arg));
}

TEST(ArgumentCompileTest, RegisterC) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::C), false, false));
	EXPECT_EQ(compile_result(0x2), compile(arg));
}

TEST(ArgumentCompileTest, RegisterX) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::X), false, false));
	EXPECT_EQ(compile_result(0x3), compile(arg));
}

TEST(ArgumentCompileTest, RegisterY) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::Y), false, false));
	EXPECT_EQ(compile_result(0x4), compile(arg));
}

TEST(ArgumentCompileTest, RegisterZ) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::Z), false, false));
	EXPECT_EQ(compile_result(0x5), compile(arg));
}

TEST(ArgumentCompileTest, RegisterI) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::I), false, false));
	EXPECT_EQ(compile_result(0x6), compile(arg));
}

TEST(ArgumentCompileTest, RegisterJ) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::J), false, false));
	EXPECT_EQ(compile_result(0x7), compile(arg));
}

TEST(ArgumentCompileTest, RegisterSP) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::SP), false, false));
	EXPECT_EQ(compile_result(0x1b), compile(arg));
}

TEST(ArgumentCompileTest, RegisterPC) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::PC), false, false));
	EXPECT_EQ(compile_result(0x1c), compile(arg));
}

TEST(ArgumentCompileTest, RegisterEX) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::EX), false, false));
	EXPECT_EQ(compile_result(0x1d), compile(arg));
}
/*
TEST(ArgumentCompileTest, ShortFormLiteral) {
	argument arg1expression_argument(argument_position::A, Expression::evaluatedLiteral(test_location, -1));
	EXPECT_EQ(compile_result(0x20), arg1->compile());

	argument arg2expression_argument(argument_position::A, Expression::evaluatedLiteral(test_location, 0));
	EXPECT_EQ(compile_result(0x21), arg2->compile());

	argument arg3expression_argument(argument_position::A, Expression::evaluatedLiteral(test_location, 30));
	EXPECT_EQ(compile_result(0x3f), arg3->compile());
}

TEST(ArgumentCompileTest, LongFormLiteral) {
	argument arg1expression_argument(argument_position::B, Expression::evaluatedLiteral(test_location, -1));
	EXPECT_EQ(compile_result(0x1f, -1), arg1->compile());

	argument arg2expression_argument(argument_position::B, Expression::evaluatedLiteral(test_location, 0));
	EXPECT_EQ(compile_result(0x1f, 0), arg2->compile());

	argument arg3expression_argument(argument_position::B, Expression::evaluatedLiteral(test_location, 30));
	EXPECT_EQ(compile_result(0x1f, 30), arg3->compile());

	argument arg4expression_argument(argument_position::A, Expression::evaluatedLiteral(test_location, -2));
	EXPECT_EQ(compile_result(0x1f, -2), arg4->compile());

	argument arg5expression_argument(argument_position::A, Expression::evaluatedLiteral(test_location, 31));
	EXPECT_EQ(compile_result(0x1f, 31), arg5->compile());
}

TEST(ArgumentCompileTest, Stack) {
	argument arg1 = Argument::stackPush(test_location, argument_position::B);
	EXPECT_EQ(compile_result(0x18), arg1->compile());

	argument arg2 = Argument::stackPop(test_location, argument_position::A);
	EXPECT_EQ(compile_result(0x18), arg2->compile());

	argument arg3 = Argument::stackPeek(test_location, argument_position::A);
	EXPECT_EQ(compile_result(0x19), arg3->compile());

	argument arg4 = Argument::stackPeek(test_location, argument_position::B);
	EXPECT_EQ(compile_result(0x19), arg4->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterA) {
	argument arg1 = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::A));
	EXPECT_EQ(compile_result(0x8), arg1->compile());

	argument arg2 = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::A, 0x40));
	EXPECT_EQ(compile_result(0x10, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterB) {
	argument arg = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::B));
	EXPECT_EQ(compile_result(0x9), compile(arg));

	argument arg2 = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::B, 0x40));
	EXPECT_EQ(compile_result(0x11, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterC) {
	argument arg = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::C));
	EXPECT_EQ(compile_result(0xa), compile(arg));

	argument arg2 = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::C, 0x40));
	EXPECT_EQ(compile_result(0x12, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterX) {
	argument arg = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::X));
	EXPECT_EQ(compile_result(0xb), compile(arg));

	argument arg2 = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::X, 0x40));
	EXPECT_EQ(compile_result(0x13, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterY) {
	argument arg = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::Y));
	EXPECT_EQ(compile_result(0xc), compile(arg));

	argument arg2 = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::Y, 0x40));
	EXPECT_EQ(compile_result(0x14, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterZ) {
	argument arg = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::Z));
	EXPECT_EQ(compile_result(0xd), compile(arg));

	argument arg2 = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::Z, 0x40));
	EXPECT_EQ(compile_result(0x15, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterI) {
	argument arg = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::I));
	EXPECT_EQ(compile_result(0xe), compile(arg));

	argument arg2 = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::I, 0x40));
	EXPECT_EQ(compile_result(0x16, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterJ) {
	argument arg = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::J));
	EXPECT_EQ(compile_result(0xf), compile(arg));

	argument arg2 = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::J, 0x40));
	EXPECT_EQ(compile_result(0x17, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterSP) {
	argument arg = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::SP));
	EXPECT_EQ(compile_result(0x19), compile(arg));

	argument arg2 = Argument::indirect(argument_position::B, Expression::evaluatedRegister(test_location, registers::SP, 0x40));
	EXPECT_EQ(compile_result(0x1a, 0x40), arg2->compile());
}

TEST(InstructionCompileTest, SET) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::SET,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0401, output[0]);
}

TEST(InstructionCompileTest, ADD) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::ADD,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0402, output[0]);
}

TEST(InstructionCompileTest, SUB) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::SUB,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0403, output[0]);
}

TEST(InstructionCompileTest, MUL) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::MUL,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0404, output[0]);
}

TEST(InstructionCompileTest, MLI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::MLI,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0405, output[0]);
}

TEST(InstructionCompileTest, DIV) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::DIV,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0406, output[0]);
}

TEST(InstructionCompileTest, DVI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::DVI,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0407, output[0]);
}

TEST(InstructionCompileTest, MOD) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::MOD,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0408, output[0]);
}

TEST(InstructionCompileTest, MDI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::MDI,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0409, output[0]);
}

TEST(InstructionCompileTest, AND) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::AND,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040a, output[0]);
}

TEST(InstructionCompileTest, BOR) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::BOR,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040b, output[0]);
}

TEST(InstructionCompileTest, XOR) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::XOR,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040c, output[0]);
}

TEST(InstructionCompileTest, SHR) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::SHR,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040d, output[0]);
}

TEST(InstructionCompileTest, ASR) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::ASR,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040e, output[0]);
}

TEST(InstructionCompileTest, SHL) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::SHL,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040f, output[0]);
}

TEST(InstructionCompileTest, IFB) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IFB,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0410, output[0]);
}

TEST(InstructionCompileTest, IFC) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IFC,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0411, output[0]);
}

TEST(InstructionCompileTest, IFE) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IFE,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0412, output[0]);
}

TEST(InstructionCompileTest, IFN) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IFN,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0413, output[0]);
}

TEST(InstructionCompileTest, IFG) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IFG,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0414, output[0]);
}

TEST(InstructionCompileTest, IFA) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IFA,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0415, output[0]);
}

TEST(InstructionCompileTest, IFL) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IFL,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0416, output[0]);
}

TEST(InstructionCompileTest, IFU) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IFU,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0417, output[0]);
}

TEST(InstructionCompileTest, ADX) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::ADX,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041a, output[0]);
}

TEST(InstructionCompileTest, SBX) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::SBX,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041b, output[0]);
}

TEST(InstructionCompileTest, STI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::STI,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041e, output[0]);
}

TEST(InstructionCompileTest, STD) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::STD,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::B)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041f, output[0]);
}

TEST(InstructionCompileTest, JSR) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::JSR,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0020, output[0]);
}

TEST(InstructionCompileTest, HCF) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::HCF,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x00e0, output[0]);
}

TEST(InstructionCompileTest, INT) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::INT,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0100, output[0]);
}

TEST(InstructionCompileTest, IAG) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IAG,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0120, output[0]);
}

TEST(InstructionCompileTest, IAS) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IAS,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0140, output[0]);
}

TEST(InstructionCompileTest, RFI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::RFI,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0160, output[0]);
}

TEST(InstructionCompileTest, IAQ) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::IAQ,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0180, output[0]);
}

TEST(InstructionCompileTest, HWN) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::HWN,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0200, output[0]);
}

TEST(InstructionCompileTest, HWQ) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::HWQ,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0220, output[0]);
}

TEST(InstructionCompileTest, HWI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::HWI,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0240, output[0]);
}

TEST(InstructionCompileTest, JMP) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::JMP,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0381, output[0]);
}


TEST(InstructionCompileTest, NextWordArgA) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::SET,
		Argument::indirect(argument_position::A, Expression::evaluatedRegister(test_location, registers::SP, 5)),
		Argument::expression(argument_position::B, Expression::evaluatedRegister(test_location, registers::A)));
	instruction->compile(output);

	ASSERT_EQ(2, output.size());
	EXPECT_EQ(0x6801, output[0]);
	EXPECT_EQ(5, output[1]);
}

TEST(InstructionCompileTest, NextWordArgB) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::SET,
		Argument::expression(argument_position::A, Expression::evaluatedRegister(test_location, registers::SP)),
		Argument::expression(argument_position::B, Expression::evaluatedLiteral(test_location, 16)));
	instruction->compile(output);

	ASSERT_EQ(2, output.size());
	EXPECT_EQ(0x6fe1, output[0]);
	EXPECT_EQ(16, output[1]);
}

TEST(InstructionCompileTest, NextWordBoth) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::SET,
		Argument::indirect(argument_position::A, Expression::evaluatedRegister(test_location, registers::SP, 5)),
		Argument::expression(argument_position::B, Expression::evaluatedLiteral(test_location, 16)));
	instruction->compile(output);

	ASSERT_EQ(3, output.size());
	EXPECT_EQ(0x6be1, output[0]);
	EXPECT_EQ(5, output[1]);
	EXPECT_EQ(16, output[2]);
}


TEST(InstructionCompileTest, JsrNextWordArgA) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(test_location, opcodes::::JSR,
		Argument::expression(argument_position::A, Expression::evaluatedLiteral(test_location, 400)),
		Argument::null());
	instruction->compile(output);
	
	ASSERT_EQ(2, output.size());
	EXPECT_EQ(0x7c20, output[0]);
	EXPECT_EQ(400, output[1]);
}
*/
