#include <gtest/gtest.h>
#include <dcpu.hpp>
#include <hardware.hpp>
#include <argument.hpp>
#include <opcodes.hpp>

using namespace std;

using dcpu::emulator::argument;
using dcpu::emulator::writable_argument;
using dcpu::emulator::literal_argument;

#define EXECUTE_OPCODE(op, cpu, a, b) dcpu::emulator::op ## _opcode opcode(cpu, a, b); \
opcode.execute();

TEST(Opcodes, Set) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 0;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(17));

	EXECUTE_OPCODE(set, cpu, a, b)

	EXPECT_EQ(17, result);
}

TEST(Opcodes, Add) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 22;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(18));

	EXECUTE_OPCODE(add, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(40, result);
}


TEST(Opcodes, AddWithOverflow) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 40000;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(30000));

	EXECUTE_OPCODE(add, cpu, a, b)

	EXPECT_EQ(1, cpu.ex);
	EXPECT_EQ(4464, result);
}

TEST(Opcodes, Sub) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 22;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(18));

	EXECUTE_OPCODE(sub, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(4, result);
}


TEST(Opcodes, SubWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 0;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(1));

	EXECUTE_OPCODE(sub, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.ex);
	EXPECT_EQ(0xffff, result);
}

TEST(Opcodes, Mul) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 2;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(2));

	EXECUTE_OPCODE(mul, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(4, result);
}

TEST(Opcodes, MulWithOverflow) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 0x8000;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(0x8000));

	EXECUTE_OPCODE(mul, cpu, a, b)

	EXPECT_EQ(0x4000, cpu.ex);
	EXPECT_EQ(0, result);
}

TEST(Opcodes, Mli) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = -2;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(2));

	EXECUTE_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.ex);
	EXPECT_EQ((uint16_t)-4, result);
}

TEST(Opcodes, MliWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 0xfffe;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(2));

	EXECUTE_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.ex);
	EXPECT_EQ(0xfffc, result);
}

TEST(Opcodes, MliWithOverflow) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 0x4000;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(0x10));

	EXECUTE_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(4, cpu.ex);
	EXPECT_EQ(0, result);
}