#include <gtest/gtest.h>
#include <tuple>
#include <dcpu.hpp>
#include <argument.hpp>
#include <iostream>

using namespace std;

using dcpu::emulator::argument;
using dcpu::emulator::literal_argument;
using dcpu::emulator::register_argument;
using dcpu::emulator::registers;

class ArgumentTester {
public:
	virtual void setup(dcpu::emulator::dcpu &cpu)=0;
	virtual void verify(dcpu::emulator::dcpu &cpu)=0;
	virtual uint16_t get_initial_value() {
		return 0x20;
	}

	virtual void print(ostream &stream) const=0;
};

class ArgumentTest : public ::testing::TestWithParam<tuple<uint8_t, bool, shared_ptr<ArgumentTester>>> {
public:
	void SetUp() {
		auto data = GetParam();
		code = get<0>(data);
		isA = get<1>(data);
		tester = get<2>(data);
	}
protected:
	uint8_t code;
	bool isA;
	shared_ptr<ArgumentTester> tester;
};

class RegisterArgument : public ArgumentTester {
	registers reg;
public:
	RegisterArgument(registers reg) : reg(reg) {}

	virtual void setup(dcpu::emulator::dcpu &cpu) {
		cpu.registers[reg] = get_initial_value();
	}

	virtual void verify(dcpu::emulator::dcpu &cpu) {
		EXPECT_EQ(0x30, cpu.registers[reg]);
	}

	virtual void print(ostream &stream) const {
		stream << "RegisterArgument(" << reg << ")";
	}
};

class RegisterIndirectArgument : public ArgumentTester {
	registers reg;
public:
	RegisterIndirectArgument(registers reg) : reg(reg) {}

	virtual void setup(dcpu::emulator::dcpu &cpu) {
		cpu.registers[reg] = 0x001f;
		cpu.memory[0x001f] = get_initial_value();
	}

	virtual void verify(dcpu::emulator::dcpu &cpu) {
		EXPECT_EQ(0x30, cpu.memory[0x001f]);
	}

	virtual void print(ostream &stream) const {
		stream << "RegisterIndirectArgument(" << reg << ")";
	}
};

class RegisterIndirectOffsetArgument : public ArgumentTester {
	registers reg;
public:
	RegisterIndirectOffsetArgument(registers reg) : reg(reg) {}

	virtual void setup(dcpu::emulator::dcpu &cpu) {
		cpu.memory[0] = 0x0010;
		cpu.registers[reg] = 0x001f;
		cpu.memory[0x002f] = get_initial_value();
	}

	virtual void verify(dcpu::emulator::dcpu &cpu) {
		EXPECT_EQ(1, cpu.registers.pc);
		EXPECT_EQ(0x30, cpu.memory[0x002f]);
	}

	virtual void print(ostream &stream) const {
		stream << "RegisterIndirectOffsetArgument(" << reg << ")";
	}
};

class StackPushArgument : public ArgumentTester {
public:
	virtual void setup(dcpu::emulator::dcpu &cpu) {
		cpu.memory[0xffff] = get_initial_value();
	}

	virtual void verify(dcpu::emulator::dcpu &cpu) {
		EXPECT_EQ(0xffff, cpu.registers.sp);
		EXPECT_EQ(0x30, cpu.memory[0xffff]);
	}

	virtual void print(ostream &stream) const {
		stream << "StackPushArgument()";
	}
};

class StackPopArgument : public ArgumentTester {
public:
	virtual void setup(dcpu::emulator::dcpu &cpu) {
		cpu.stack.push(get_initial_value());
	}

	virtual void verify(dcpu::emulator::dcpu &cpu) {
		EXPECT_EQ(0, cpu.registers.sp);
		EXPECT_EQ(0x30, cpu.memory[0xffff]);
	}

	virtual void print(ostream &stream) const {
		stream << "StackPopArgument()";
	}
};

class StackPeekArgument : public ArgumentTester {
public:
	virtual void setup(dcpu::emulator::dcpu &cpu) {
		cpu.stack.push(get_initial_value());
	}

	virtual void verify(dcpu::emulator::dcpu &cpu) {
		EXPECT_EQ(0xffff, cpu.registers.sp);
		EXPECT_EQ(0x30, cpu.memory[0xffff]);
	}

	virtual void print(ostream &stream) const {
		stream << "StackPeekArgument()";
	}
};

class StackPickArgument : public ArgumentTester {
public:
	virtual void setup(dcpu::emulator::dcpu &cpu) {
		cpu.stack.push(0x1a);
		cpu.stack.push(get_initial_value());
		cpu.stack.push(0x1b);
		cpu.stack.push(0x1c);

		cpu.memory[0] = 2;
		cpu.registers.pc = 0;
	}

	virtual void verify(dcpu::emulator::dcpu &cpu) {
		EXPECT_EQ(1, cpu.registers.pc);
		EXPECT_EQ(0xfffc, cpu.registers.sp);
		EXPECT_EQ(0x30, cpu.memory[0xfffe]);
	}

	virtual void print(ostream &stream) const {
		stream << "StackPickArgument()";
	}
};

class IndirectNextWordArgument : public ArgumentTester {
public:
	virtual void setup(dcpu::emulator::dcpu &cpu) {
		cpu.registers.pc = 0;
		cpu.memory[0] = 0x030a;
		cpu.memory[0x030a] = get_initial_value();
	}

	virtual void verify(dcpu::emulator::dcpu &cpu) {
		EXPECT_EQ(1, cpu.registers.pc);
		EXPECT_EQ(0x030a, cpu.memory[0]);
		EXPECT_EQ(0x30, cpu.memory[0x030a]);
	}

	virtual void print(ostream &stream) const {
		stream << "IndirectNextWordArgument()";
	}
};

class NextWordArgument : public ArgumentTester {
public:
	virtual void setup(dcpu::emulator::dcpu &cpu) {
		cpu.registers.pc = 0x30;
		cpu.memory[0x30] = get_initial_value();
	}

	virtual void verify(dcpu::emulator::dcpu &cpu) {
		EXPECT_EQ(0x31, cpu.registers.pc);
		EXPECT_EQ(get_initial_value(), cpu.memory[0x30]);
	}

	virtual void print(ostream &stream) const {
		stream << "NextWordArgument()";
	}
};

class LiteralArgument : public ArgumentTester {
	uint16_t literal_value;
public:
	LiteralArgument(uint16_t literal_value) : literal_value(literal_value) {}

	virtual void setup(dcpu::emulator::dcpu &cpu) {
	}

	virtual uint16_t get_initial_value() {
		return literal_value;
	}

	virtual void verify(dcpu::emulator::dcpu &cpu) {
	}

	virtual void print(ostream &stream) const {
		stream << "LiteralArgument(" << literal_value << ")";
	}
};

TEST_P(ArgumentTest, Parse) {
	dcpu::emulator::dcpu cpu;

	tester->setup(cpu);

	auto arg = argument::parse(cpu, code, isA);
	EXPECT_EQ(tester->get_initial_value(), arg->get());

	arg->set(0x30);
	tester->verify(cpu);
}

INSTANTIATE_TEST_CASE_P(All, ArgumentTest, ::testing::Values(
	make_tuple(0x00, false, make_shared<RegisterArgument>(registers::A)),
	make_tuple(0x01, false, make_shared<RegisterArgument>(registers::B)),
	make_tuple(0x02, false, make_shared<RegisterArgument>(registers::C)),
	make_tuple(0x03, false, make_shared<RegisterArgument>(registers::X)),
	make_tuple(0x04, false, make_shared<RegisterArgument>(registers::Y)),
	make_tuple(0x05, false, make_shared<RegisterArgument>(registers::Z)),
	make_tuple(0x06, false, make_shared<RegisterArgument>(registers::I)),
	make_tuple(0x07, false, make_shared<RegisterArgument>(registers::J)),
	make_tuple(0x08, false, make_shared<RegisterIndirectArgument>(registers::A)),
	make_tuple(0x09, false, make_shared<RegisterIndirectArgument>(registers::B)),
	make_tuple(0x0a, false, make_shared<RegisterIndirectArgument>(registers::C)),
	make_tuple(0x0b, false, make_shared<RegisterIndirectArgument>(registers::X)),
	make_tuple(0x0c, false, make_shared<RegisterIndirectArgument>(registers::Y)),
	make_tuple(0x0d, false, make_shared<RegisterIndirectArgument>(registers::Z)),
	make_tuple(0x0e, false, make_shared<RegisterIndirectArgument>(registers::I)),
	make_tuple(0x0f, false, make_shared<RegisterIndirectArgument>(registers::J)),
	make_tuple(0x10, false, make_shared<RegisterIndirectOffsetArgument>(registers::A)),
	make_tuple(0x11, false, make_shared<RegisterIndirectOffsetArgument>(registers::B)),
	make_tuple(0x12, false, make_shared<RegisterIndirectOffsetArgument>(registers::C)),
	make_tuple(0x13, false, make_shared<RegisterIndirectOffsetArgument>(registers::X)),
	make_tuple(0x14, false, make_shared<RegisterIndirectOffsetArgument>(registers::Y)),
	make_tuple(0x15, false, make_shared<RegisterIndirectOffsetArgument>(registers::Z)),
	make_tuple(0x16, false, make_shared<RegisterIndirectOffsetArgument>(registers::I)),
	make_tuple(0x17, false, make_shared<RegisterIndirectOffsetArgument>(registers::J)),
	make_tuple(0x18, false, make_shared<StackPushArgument>()),
	make_tuple(0x18, true,  make_shared<StackPopArgument>()),
	make_tuple(0x19, false, make_shared<StackPeekArgument>()),
	make_tuple(0x1a, false, make_shared<StackPickArgument>()),
	make_tuple(0x1b, false, make_shared<RegisterArgument>(registers::SP)),
	make_tuple(0x1c, false, make_shared<RegisterArgument>(registers::PC)),
	make_tuple(0x1d, false, make_shared<RegisterArgument>(registers::EX)),
	make_tuple(0x1e, false, make_shared<IndirectNextWordArgument>()),
	make_tuple(0x1f, false, make_shared<NextWordArgument>()),
	make_tuple(0x20, false, make_shared<LiteralArgument>(-1)),
	make_tuple(0x21, false, make_shared<LiteralArgument>(0)),
	make_tuple(0x22, false, make_shared<LiteralArgument>(1)),
	make_tuple(0x3f, false, make_shared<LiteralArgument>(30))));

ostream& operator<<(ostream &stream, const ArgumentTester &tester) {
	tester.print(stream);

	return stream;
}

ostream& operator<<(ostream &stream, const tuple<uint8_t, bool, shared_ptr<ArgumentTester>> &testData) {
	return stream << "code=" << hex << (int)get<0>(testData) << dec << ", isA=" << (get<1>(testData) ? "true" : "false") << "tester=" << *get<2>(testData);
}