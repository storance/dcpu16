#include <iostream>
#include <list>
#include <gtest/gtest.h>
#include <boost/variant.hpp>
#include <boost/format.hpp>

#include <statement.hpp>
#include <compiler.hpp>

using namespace std;
using namespace dcpu::assembler;

static dcpu::assembler::log logger;
static location_ptr test_location = make_shared<location>("<Test>", 1, 1);

TEST(ArgumentCompile, RegisterA) {
	argument arg(expression_argument(test_location, argument_position::B,
			evaluated_expression(test_location, registers::A), false, false));
	EXPECT_EQ(compile_result(0x0), compile(logger, arg));
}

TEST(ArgumentCompile, RegisterB) {
	argument arg(expression_argument(test_location, argument_position::B,
			evaluated_expression(test_location, registers::B), false, false));
	EXPECT_EQ(compile_result(0x1), compile(logger, arg));
}

TEST(ArgumentCompile, RegisterC) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::C), false, false));
	EXPECT_EQ(compile_result(0x2), compile(logger, arg));
}

TEST(ArgumentCompile, RegisterX) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::X), false, false));
	EXPECT_EQ(compile_result(0x3), compile(logger, arg));
}

TEST(ArgumentCompile, RegisterY) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::Y), false, false));
	EXPECT_EQ(compile_result(0x4), compile(logger, arg));
}

TEST(ArgumentCompile, RegisterZ) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::Z), false, false));
	EXPECT_EQ(compile_result(0x5), compile(logger, arg));
}

TEST(ArgumentCompile, RegisterI) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::I), false, false));
	EXPECT_EQ(compile_result(0x6), compile(logger, arg));
}

TEST(ArgumentCompile, RegisterJ) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::J), false, false));
	EXPECT_EQ(compile_result(0x7), compile(logger, arg));
}

TEST(ArgumentCompile, RegisterSP) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::SP), false, false));
	EXPECT_EQ(compile_result(0x1b), compile(logger, arg));
}

TEST(ArgumentCompile, RegisterPC) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::PC), false, false));
	EXPECT_EQ(compile_result(0x1c), compile(logger, arg));
}

TEST(ArgumentCompile, RegisterEX) {
	argument arg(expression_argument(test_location, argument_position::B,
				evaluated_expression(test_location, registers::EX), false, false));
	EXPECT_EQ(compile_result(0x1d), compile(logger, arg));
}

TEST(ArgumentCompile, ShortFormLiteral) {
	argument arg1(expression_argument(test_location, argument_position::A, evaluated_expression(test_location, -1),
			false, false));
	EXPECT_EQ(compile_result(0x20), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::A, evaluated_expression(test_location, 0),
			false, false));
	EXPECT_EQ(compile_result(0x21), compile(logger, arg2));

	argument arg3(expression_argument(test_location, argument_position::A, evaluated_expression(test_location, 30),
			false, false));
	EXPECT_EQ(compile_result(0x3f), compile(logger, arg3));
}

TEST(ArgumentCompile, LongFormLiteral) {
	argument arg1(expression_argument(test_location, argument_position::B, evaluated_expression(test_location, -1),
			false, false));
	EXPECT_EQ(compile_result(0x1f, -1), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::B, evaluated_expression(test_location, 0),
			false, false));
	EXPECT_EQ(compile_result(0x1f, 0), compile(logger, arg2));

	argument arg3(expression_argument(test_location, argument_position::B, evaluated_expression(test_location, 30),
			false, false));
	EXPECT_EQ(compile_result(0x1f, 30), compile(logger, arg3));

	argument arg4(expression_argument(test_location, argument_position::A, evaluated_expression(test_location, -2),
			false, false));
	EXPECT_EQ(compile_result(0x1f, -2), compile(logger, arg4));

	argument arg5(expression_argument(test_location, argument_position::A, evaluated_expression(test_location, 31),
			false, false));
	EXPECT_EQ(compile_result(0x1f, 31), compile(logger, arg5));
}

TEST(ArgumentCompile, Stack) {
	argument arg1 = argument(stack_argument(test_location, argument_position::B, stack_operation::PUSH));
	EXPECT_EQ(compile_result(0x18), compile(logger, arg1));

	argument arg2 = argument(stack_argument(test_location, argument_position::A, stack_operation::POP));
	EXPECT_EQ(compile_result(0x18), compile(logger, arg2));

	argument arg3 = argument(stack_argument(test_location, argument_position::A, stack_operation::PEEK));
	EXPECT_EQ(compile_result(0x19), compile(logger, arg3));

	argument arg4 = argument(stack_argument(test_location, argument_position::B, stack_operation::PEEK));
	EXPECT_EQ(compile_result(0x19), compile(logger, arg4));
}

TEST(ArgumentCompile, IndirectRegisterA) {
	argument arg1(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::A), true, false));
	EXPECT_EQ(compile_result(0x8), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::A, 0x40), true, false));
	EXPECT_EQ(compile_result(0x10, 0x40), compile(logger, arg2));
}

TEST(ArgumentCompile, IndirectRegisterB) {
	argument arg1(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::B), true, false));
	EXPECT_EQ(compile_result(0x9), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::B, 0x40), true, false));
	EXPECT_EQ(compile_result(0x11, 0x40), compile(logger, arg2));
}

TEST(ArgumentCompile, IndirectRegisterC) {
	argument arg1(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::C), true, false));
	EXPECT_EQ(compile_result(0xa), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::C, 0x40), true, false));
	EXPECT_EQ(compile_result(0x12, 0x40), compile(logger, arg2));
}

TEST(ArgumentCompile, IndirectRegisterX) {
	argument arg1(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::X), true, false));
	EXPECT_EQ(compile_result(0xb), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::X, 0x40), true, false));
	EXPECT_EQ(compile_result(0x13, 0x40), compile(logger, arg2));
}

TEST(ArgumentCompile, IndirectRegisterY) {
	argument arg1(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::Y), true, false));
	EXPECT_EQ(compile_result(0xc), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::Y, 0x40), true, false));
	EXPECT_EQ(compile_result(0x14, 0x40), compile(logger, arg2));
}

TEST(ArgumentCompile, IndirectRegisterZ) {
	argument arg1(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::Z), true, false));
	EXPECT_EQ(compile_result(0xd), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::Z, 0x40), true, false));
	EXPECT_EQ(compile_result(0x15, 0x40), compile(logger, arg2));
}

TEST(ArgumentCompile, IndirectRegisterI) {
	argument arg1(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::I), true, false));
	EXPECT_EQ(compile_result(0xe), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::I, 0x40), true, false));
	EXPECT_EQ(compile_result(0x16, 0x40), compile(logger, arg2));
}

TEST(ArgumentCompile, IndirectRegisterJ) {
	argument arg1(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::J), true, false));
	EXPECT_EQ(compile_result(0xf), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::J, 0x40), true, false));
	EXPECT_EQ(compile_result(0x17, 0x40), compile(logger, arg2));
}

TEST(ArgumentCompile, IndirectRegisterSP) {
	argument arg1(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::SP), true, false));
	EXPECT_EQ(compile_result(0x19), compile(logger, arg1));

	argument arg2(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
			registers::SP, 0x40), true, false));
	EXPECT_EQ(compile_result(0x1a, 0x40), compile(logger, arg2));
}

TEST(InstructionCompile, SET) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::SET,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0401, output[0]);
}

TEST(InstructionCompile, ADD) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::ADD,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0402, output[0]);
}

TEST(InstructionCompile, SUB) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::SUB,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0403, output[0]);
}

TEST(InstructionCompile, MUL) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::MUL,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0404, output[0]);
}

TEST(InstructionCompile, MLI) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::MLI,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0405, output[0]);
}

TEST(InstructionCompile, DIV) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::DIV,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0406, output[0]);
}

TEST(InstructionCompile, DVI) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::DVI,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0407, output[0]);
}

TEST(InstructionCompile, MOD) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::MOD,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0408, output[0]);
}

TEST(InstructionCompile, MDI) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::MDI,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0409, output[0]);
}

TEST(InstructionCompile, AND) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::AND,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040a, output[0]);
}

TEST(InstructionCompile, BOR) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::BOR,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040b, output[0]);
}

TEST(InstructionCompile, XOR) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::XOR,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040c, output[0]);
}

TEST(InstructionCompile, SHR) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::SHR,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040d, output[0]);
}

TEST(InstructionCompile, ASR) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::ASR,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040e, output[0]);
}

TEST(InstructionCompile, SHL) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::SHL,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040f, output[0]);
}

TEST(InstructionCompile, IFB) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IFB,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0410, output[0]);
}

TEST(InstructionCompile, IFC) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IFC,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0411, output[0]);
}

TEST(InstructionCompile, IFE) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IFE,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0412, output[0]);
}

TEST(InstructionCompile, IFN) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IFN,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0413, output[0]);
}

TEST(InstructionCompile, IFG) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IFG,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0414, output[0]);
}

TEST(InstructionCompile, IFA) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IFA,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0415, output[0]);
}

TEST(InstructionCompile, IFL) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IFL,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0416, output[0]);
}

TEST(InstructionCompile, IFU) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IFU,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0417, output[0]);
}

TEST(InstructionCompile, ADX) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::ADX,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041a, output[0]);
}

TEST(InstructionCompile, SBX) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::SBX,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041b, output[0]);
}

TEST(InstructionCompile, STI) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::STI,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041e, output[0]);
}

TEST(InstructionCompile, STD) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::STD,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::B), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041f, output[0]);
}

TEST(InstructionCompile, JSR) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::JSR,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0020, output[0]);
}

TEST(InstructionCompile, HCF) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::HCF,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x00e0, output[0]);
}

TEST(InstructionCompile, INT) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::INT,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0100, output[0]);
}

TEST(InstructionCompile, IAG) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IAG,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0120, output[0]);
}

TEST(InstructionCompile, IAS) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IAS,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0140, output[0]);
}

TEST(InstructionCompile, RFI) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::RFI,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0160, output[0]);
}

TEST(InstructionCompile, IAQ) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::IAQ,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0180, output[0]);
}

TEST(InstructionCompile, HWN) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::HWN,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0200, output[0]);
}

TEST(InstructionCompile, HWQ) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::HWQ,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0220, output[0]);
}

TEST(InstructionCompile, HWI) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::HWI,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0240, output[0]);
}

TEST(InstructionCompile, JMP) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::JMP,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::A), false, false)), boost::none));
	compile(logger, output, _instruction);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0381, output[0]);
}


TEST(InstructionCompile, NextWordArgA) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::SET,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::SP, 5), true, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location,
				registers::A), false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(2, output.size());
	EXPECT_EQ(0x6801, output[0]);
	EXPECT_EQ(5, output[1]);
}

TEST(InstructionCompile, NextWordArgB) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::SET,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::SP), false, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location, 16),
				false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(2, output.size());
	EXPECT_EQ(0x6fe1, output[0]);
	EXPECT_EQ(16, output[1]);
}

TEST(InstructionCompile, NextWordBoth) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::SET,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location,
				registers::SP, 5), true, false)),
		argument(expression_argument(test_location, argument_position::B, evaluated_expression(test_location, 16),
				false, false))
	));
	compile(logger, output, _instruction);

	ASSERT_EQ(3, output.size());
	EXPECT_EQ(0x6be1, output[0]);
	EXPECT_EQ(5, output[1]);
	EXPECT_EQ(16, output[2]);
}


TEST(InstructionCompile, JsrNextWordArgA) {
	vector<uint16_t> output;

	statement _instruction(instruction(test_location, opcodes::JSR,
		argument(expression_argument(test_location, argument_position::A, evaluated_expression(test_location, 400),
				false, false)), boost::none
	));

	compile(logger, output, _instruction);
	
	ASSERT_EQ(2, output.size());
	EXPECT_EQ(0x7c20, output[0]);
	EXPECT_EQ(400, output[1]);
}

TEST(DirectiveCompile, Data) {
	vector<uint16_t> output;

	vector<uint16_t> expected_data = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd','!', 0};
	statement data(data_directive(test_location, expected_data));

	compile(logger, output, data);
	ASSERT_EQ(13, output.size());
	EXPECT_EQ(expected_data, output);
}

TEST(DirectiveCompile, Fill) {
	vector<uint16_t> output;

	statement fill(fill_directive(test_location,
			evaluated_expression(test_location, 25),
			evaluated_expression(test_location, 0x32)));

	compile(logger, output, fill);
	ASSERT_EQ(25, output.size());
	for (size_t i = 0; i < output.size(); i++) {
		SCOPED_TRACE(str(boost::format("Word %d") % i));
		EXPECT_EQ(0x32, output[i]);
	}
}
