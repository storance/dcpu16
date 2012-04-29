#include <iostream>
#include <list>
#include <gtest/gtest.h>

#include <boost/format.hpp>

#include <ast/Statement.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::lexer;

static Location location("<Test>", 1, 1);

ostream& operator<<(ostream& stream, const CompileResult& result) {
	uint8_t encodedValue = get<0>(result);
	boost::optional<uint16_t> nextWord = get<1>(result);

	stream << "(" << boost::format("%#02x") % (uint)encodedValue << ", ";
	if (nextWord) {
		stream << boost::format("%#04x") % *nextWord << ")";
	} else {
		stream << "None)";
	}

	return stream;
}

TEST(ArgumentCompileTest, RegisterA) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A));
	EXPECT_EQ(CompileResult(0x0, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, RegisterB) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::B));
	EXPECT_EQ(CompileResult(0x1, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, RegisterC) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::C));
	EXPECT_EQ(CompileResult(0x2, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, RegisterX) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::X));
	EXPECT_EQ(CompileResult(0x3, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, RegisterY) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::Y));
	EXPECT_EQ(CompileResult(0x4, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, RegisterZ) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::Z));
	EXPECT_EQ(CompileResult(0x5, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, RegisterI) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::I));
	EXPECT_EQ(CompileResult(0x6, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, RegisterJ) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::J));
	EXPECT_EQ(CompileResult(0x7, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, RegisterSP) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::SP));
	EXPECT_EQ(CompileResult(0x1b, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, RegisterPC) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::PC));
	EXPECT_EQ(CompileResult(0x1c, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, RegisterEX) {
	auto arg = Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::EX));
	EXPECT_EQ(CompileResult(0x1d, boost::none), arg->compile());
}

TEST(ArgumentCompileTest, ShortFormLiteral) {
	auto arg1 = Argument::expression(ArgumentPosition::A, Expression::evaluatedLiteral(location, -1));
	EXPECT_EQ(CompileResult(0x20, boost::none), arg1->compile());

	auto arg2 = Argument::expression(ArgumentPosition::A, Expression::evaluatedLiteral(location, 0));
	EXPECT_EQ(CompileResult(0x21, boost::none), arg2->compile());

	auto arg3 = Argument::expression(ArgumentPosition::A, Expression::evaluatedLiteral(location, 30));
	EXPECT_EQ(CompileResult(0x3f, boost::none), arg3->compile());
}

TEST(ArgumentCompileTest, LongFormLiteral) {
	auto arg1 = Argument::expression(ArgumentPosition::B, Expression::evaluatedLiteral(location, -1));
	EXPECT_EQ(CompileResult(0x1f, -1), arg1->compile());

	auto arg2 = Argument::expression(ArgumentPosition::B, Expression::evaluatedLiteral(location, 0));
	EXPECT_EQ(CompileResult(0x1f, 0), arg2->compile());

	auto arg3 = Argument::expression(ArgumentPosition::B, Expression::evaluatedLiteral(location, 30));
	EXPECT_EQ(CompileResult(0x1f, 30), arg3->compile());

	auto arg4 = Argument::expression(ArgumentPosition::A, Expression::evaluatedLiteral(location, -2));
	EXPECT_EQ(CompileResult(0x1f, -2), arg4->compile());

	auto arg5 = Argument::expression(ArgumentPosition::A, Expression::evaluatedLiteral(location, 31));
	EXPECT_EQ(CompileResult(0x1f, 31), arg5->compile());
}

TEST(ArgumentCompileTest, Stack) {
	auto arg1 = Argument::stackPush(location, ArgumentPosition::B);
	EXPECT_EQ(CompileResult(0x18, boost::none), arg1->compile());

	auto arg2 = Argument::stackPop(location, ArgumentPosition::A);
	EXPECT_EQ(CompileResult(0x18, boost::none), arg2->compile());

	auto arg3 = Argument::stackPeek(location, ArgumentPosition::A);
	EXPECT_EQ(CompileResult(0x19, boost::none), arg3->compile());

	auto arg4 = Argument::stackPeek(location, ArgumentPosition::B);
	EXPECT_EQ(CompileResult(0x19, boost::none), arg4->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterA) {
	auto arg1 = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A));
	EXPECT_EQ(CompileResult(0x8, boost::none), arg1->compile());

	auto arg2 = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A, 0x40));
	EXPECT_EQ(CompileResult(0x10, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterB) {
	auto arg = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::B));
	EXPECT_EQ(CompileResult(0x9, boost::none), arg->compile());

	auto arg2 = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::B, 0x40));
	EXPECT_EQ(CompileResult(0x11, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterC) {
	auto arg = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::C));
	EXPECT_EQ(CompileResult(0xa, boost::none), arg->compile());

	auto arg2 = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::C, 0x40));
	EXPECT_EQ(CompileResult(0x12, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterX) {
	auto arg = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::X));
	EXPECT_EQ(CompileResult(0xb, boost::none), arg->compile());

	auto arg2 = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::X, 0x40));
	EXPECT_EQ(CompileResult(0x13, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterY) {
	auto arg = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::Y));
	EXPECT_EQ(CompileResult(0xc, boost::none), arg->compile());

	auto arg2 = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::Y, 0x40));
	EXPECT_EQ(CompileResult(0x14, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterZ) {
	auto arg = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::Z));
	EXPECT_EQ(CompileResult(0xd, boost::none), arg->compile());

	auto arg2 = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::Z, 0x40));
	EXPECT_EQ(CompileResult(0x15, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterI) {
	auto arg = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::I));
	EXPECT_EQ(CompileResult(0xe, boost::none), arg->compile());

	auto arg2 = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::I, 0x40));
	EXPECT_EQ(CompileResult(0x16, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterJ) {
	auto arg = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::J));
	EXPECT_EQ(CompileResult(0xf, boost::none), arg->compile());

	auto arg2 = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::J, 0x40));
	EXPECT_EQ(CompileResult(0x17, 0x40), arg2->compile());
}

TEST(ArgumentCompileTest, IndirectRegisterSP) {
	auto arg = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::SP));
	EXPECT_EQ(CompileResult(0x19, boost::none), arg->compile());

	auto arg2 = Argument::indirect(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::SP, 0x40));
	EXPECT_EQ(CompileResult(0x1a, 0x40), arg2->compile());
}

TEST(InstructionCompileTest, SET) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::SET, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0401, output[0]);
}

TEST(InstructionCompileTest, ADD) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::ADD, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0402, output[0]);
}

TEST(InstructionCompileTest, SUB) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::SUB, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0403, output[0]);
}

TEST(InstructionCompileTest, MUL) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::MUL, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0404, output[0]);
}

TEST(InstructionCompileTest, MLI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::MLI, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0405, output[0]);
}

TEST(InstructionCompileTest, DIV) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::DIV, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0406, output[0]);
}

TEST(InstructionCompileTest, DVI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::DVI, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0407, output[0]);
}

TEST(InstructionCompileTest, MOD) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::MOD, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0408, output[0]);
}

TEST(InstructionCompileTest, MDI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::MDI, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0409, output[0]);
}

TEST(InstructionCompileTest, AND) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::AND, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040a, output[0]);
}

TEST(InstructionCompileTest, BOR) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::BOR, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040b, output[0]);
}

TEST(InstructionCompileTest, XOR) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::XOR, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040c, output[0]);
}

TEST(InstructionCompileTest, SHR) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::SHR, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040d, output[0]);
}

TEST(InstructionCompileTest, ASR) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::ASR, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040e, output[0]);
}

TEST(InstructionCompileTest, SHL) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::SHL, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x040f, output[0]);
}

TEST(InstructionCompileTest, IFB) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IFB, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0410, output[0]);
}

TEST(InstructionCompileTest, IFC) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IFC, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0411, output[0]);
}

TEST(InstructionCompileTest, IFE) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IFE, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0412, output[0]);
}

TEST(InstructionCompileTest, IFN) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IFN, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0413, output[0]);
}

TEST(InstructionCompileTest, IFG) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IFG, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0414, output[0]);
}

TEST(InstructionCompileTest, IFA) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IFA, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0415, output[0]);
}

TEST(InstructionCompileTest, IFL) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IFL, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0416, output[0]);
}

TEST(InstructionCompileTest, IFU) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IFU, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0417, output[0]);
}

TEST(InstructionCompileTest, ADX) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::ADX, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041a, output[0]);
}

TEST(InstructionCompileTest, SBX) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::SBX, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041b, output[0]);
}

TEST(InstructionCompileTest, STI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::STI, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041e, output[0]);
}

TEST(InstructionCompileTest, STD) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::STD, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::B)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x041f, output[0]);
}

TEST(InstructionCompileTest, JSR) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::JSR, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0020, output[0]);
}

TEST(InstructionCompileTest, HCF) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::HCF, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x00e0, output[0]);
}

TEST(InstructionCompileTest, INT) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::INT, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0100, output[0]);
}

TEST(InstructionCompileTest, IAG) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IAG, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0120, output[0]);
}

TEST(InstructionCompileTest, IAS) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IAS, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0140, output[0]);
}

TEST(InstructionCompileTest, RFI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::RFI, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0160, output[0]);
}

TEST(InstructionCompileTest, IAQ) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::IAQ, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0180, output[0]);
}

TEST(InstructionCompileTest, HWN) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::HWN, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0200, output[0]);
}

TEST(InstructionCompileTest, HWQ) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::HWQ, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0220, output[0]);
}

TEST(InstructionCompileTest, HWI) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::HWI, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0240, output[0]);
}

TEST(InstructionCompileTest, JMP) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::JMP, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::A)),
		Argument::null());
	instruction->compile(output);

	ASSERT_EQ(1, output.size());
	EXPECT_EQ(0x0381, output[0]);
}


TEST(InstructionCompileTest, NextWordArgA) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::SET, 
		Argument::indirect(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::SP, 5)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedRegister(location, Register::A)));
	instruction->compile(output);

	ASSERT_EQ(2, output.size());
	EXPECT_EQ(0x6801, output[0]);
	EXPECT_EQ(5, output[1]);
}

TEST(InstructionCompileTest, NextWordArgB) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::SET, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::SP)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedLiteral(location, 16)));
	instruction->compile(output);

	ASSERT_EQ(2, output.size());
	EXPECT_EQ(0x6fe1, output[0]);
	EXPECT_EQ(16, output[1]);
}

TEST(InstructionCompileTest, NextWordBoth) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::SET, 
		Argument::indirect(ArgumentPosition::A, Expression::evaluatedRegister(location, Register::SP, 5)),
		Argument::expression(ArgumentPosition::B, Expression::evaluatedLiteral(location, 16)));
	instruction->compile(output);

	ASSERT_EQ(3, output.size());
	EXPECT_EQ(0x6be1, output[0]);
	EXPECT_EQ(5, output[1]);
	EXPECT_EQ(16, output[2]);
}


TEST(InstructionCompileTest, JsrNextWordArgA) {
	vector<uint16_t> output;

	auto instruction = Statement::instruction(location, Opcode::JSR, 
		Argument::expression(ArgumentPosition::A, Expression::evaluatedLiteral(location, 400)),
		Argument::null());
	instruction->compile(output);
	
	ASSERT_EQ(2, output.size());
	EXPECT_EQ(0x7c20, output[0]);
	EXPECT_EQ(400, output[1]);
}