#include <gtest/gtest.h>
#include <dcpu.hpp>
#include <hardware.hpp>
#include <argument.hpp>
#include <opcodes.hpp>

#include <iostream>

using namespace std;

using dcpu::emulator::argument;
using dcpu::emulator::literal_argument;
using dcpu::emulator::register_argument;
using dcpu::emulator::registers;


#define EXECUTE_BASIC_OPCODE(op, cpu, a, b) { \
dcpu::emulator::op ## _opcode opcode(cpu, a, b); \
opcode.execute(); \
}

static unique_ptr<argument> create_register_arg(dcpu::emulator::dcpu &cpu, uint16_t initial_value) {
	cpu.a = initial_value;

	return unique_ptr<argument>(new register_argument(cpu, registers::A));
}

static unique_ptr<argument> create_literal_arg(uint16_t value) {
	return unique_ptr<argument>(new literal_argument(value));
}

TEST(Opcodes, Set) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0);
	unique_ptr<argument> a = create_literal_arg(17);

	EXECUTE_BASIC_OPCODE(set, cpu, a, b)

	EXPECT_EQ(17, cpu.a);
}

TEST(Opcodes, Add) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 22);
	unique_ptr<argument> a = create_literal_arg(18);

	EXECUTE_BASIC_OPCODE(add, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(40, cpu.a);
}


TEST(Opcodes, AddWithOverflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 40000);
	unique_ptr<argument> a = create_literal_arg(30000);

	EXECUTE_BASIC_OPCODE(add, cpu, a, b)

	EXPECT_EQ(1, cpu.ex);
	EXPECT_EQ(4464, cpu.a);
}

TEST(Opcodes, Sub) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 22);
	unique_ptr<argument> a = create_literal_arg(18);

	EXECUTE_BASIC_OPCODE(sub, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(4, cpu.a);
}


TEST(Opcodes, SubWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0);
	unique_ptr<argument> a = create_literal_arg(1);

	EXECUTE_BASIC_OPCODE(sub, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.ex);
	EXPECT_EQ(0xffff, cpu.a);
}

TEST(Opcodes, Mul) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 2);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(mul, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(4, cpu.a);
}

TEST(Opcodes, MulWithOverflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0x8000);
	unique_ptr<argument> a = create_literal_arg(0x8000);

	EXECUTE_BASIC_OPCODE(mul, cpu, a, b)

	EXPECT_EQ(0x4000, cpu.ex);
	EXPECT_EQ(0, cpu.a);
}

TEST(Opcodes, Mli) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, -2);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.ex);
	EXPECT_EQ((uint16_t)-4, cpu.a);
}

TEST(Opcodes, MliWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0xfffe);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.ex);
	EXPECT_EQ(0xfffc, cpu.a);
}

TEST(Opcodes, MliWithOverflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0x4000);
	unique_ptr<argument> a = create_literal_arg(0x10);

	EXECUTE_BASIC_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(4, cpu.ex);
	EXPECT_EQ(0, cpu.a);
}

TEST(Opcodes, Div) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 80);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(div, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(40, cpu.a);
}

TEST(Opcodes, DivByZero) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 80);
	unique_ptr<argument> a = create_literal_arg(0);

	EXECUTE_BASIC_OPCODE(div, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(0, cpu.a);
}

TEST(Opcodes, DivWithFractionalPart) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0xffff);
	unique_ptr<argument> a = create_literal_arg(0xd);

	EXECUTE_BASIC_OPCODE(div, cpu, a, b)

	EXPECT_EQ(0x2762, cpu.ex);
	EXPECT_EQ(0x13b1, cpu.a);
}

TEST(Opcodes, Dvi) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, -80);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(dvi, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ((uint16_t)-40, cpu.a);
}

TEST(Opcodes, DviByZero) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 80);
	unique_ptr<argument> a = create_literal_arg(0);

	EXECUTE_BASIC_OPCODE(dvi, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(0, cpu.a);
}

TEST(Opcodes, DviWithFractionalPart) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0x8001);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(dvi, cpu, a, b)

	EXPECT_EQ(0x8000, cpu.ex);
	EXPECT_EQ(0xc000, cpu.a);
}

TEST(Opcodes, Mod) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 21);
	unique_ptr<argument> a = create_literal_arg(16);

	EXECUTE_BASIC_OPCODE(mod, cpu, a, b)

	EXPECT_EQ(5, cpu.a);
}

TEST(Opcodes, Mdi) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, -7);
	unique_ptr<argument> a = create_literal_arg(16);

	EXECUTE_BASIC_OPCODE(mdi, cpu, a, b)

	EXPECT_EQ((uint16_t)-7, cpu.a);
}

TEST(Opcodes, And) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0xf0f0);
	unique_ptr<argument> a = create_literal_arg(0x7031);

	EXECUTE_BASIC_OPCODE(and, cpu, a, b)

	EXPECT_EQ(0x7030, cpu.a);
}

TEST(Opcodes, Bor) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0xf0f0);
	unique_ptr<argument> a = create_literal_arg(0x7a0f);

	EXECUTE_BASIC_OPCODE(bor, cpu, a, b)

	EXPECT_EQ(0xfaff, cpu.a);
}

TEST(Opcodes, Xor) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0xff0f);
	unique_ptr<argument> a = create_literal_arg(0xf0f0);

	EXECUTE_BASIC_OPCODE(xor, cpu, a, b)

	EXPECT_EQ(0x0fff, cpu.a);
}

TEST(Opcodes, Shr) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 8);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(shr, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(2, cpu.a);
}

TEST(Opcodes, ShrWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 7);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(shr, cpu, a, b)

	EXPECT_EQ(0xc000, cpu.ex);
	EXPECT_EQ(1, cpu.a);
}

TEST(Opcodes, Asr) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, -8);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(asr, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ((uint16_t)-2, cpu.a);
}

TEST(Opcodes, AsrWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, -7);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(asr, cpu, a, b)

	EXPECT_EQ(0x4000, cpu.ex);
	EXPECT_EQ((uint16_t)-2, cpu.a);
}

TEST(Opcodes, Shl) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 8);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(shl, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(32, cpu.a);
}

TEST(Opcodes, ShlWithOverflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0x100f);
	unique_ptr<argument> a = create_literal_arg(4);

	EXECUTE_BASIC_OPCODE(shl, cpu, a, b)

	EXPECT_EQ(0x0001, cpu.ex);
	EXPECT_EQ(0x00f0, cpu.a);
}

TEST(Opcodes, Ifb) {
	dcpu::emulator::dcpu cpu;
	unique_ptr<argument> a, b;

	b = create_register_arg(cpu, 0x000f);
	a = create_literal_arg(0x000f);

	EXECUTE_BASIC_OPCODE(ifb, cpu, a, b)
	EXPECT_FALSE(cpu.is_skip_next());

	b = create_register_arg(cpu, 0x000f);
	a = create_literal_arg(0xfff0);

	EXECUTE_BASIC_OPCODE(ifb, cpu, a, b)
	EXPECT_TRUE(cpu.is_skip_next());
}

TEST(Opcodes, Ifc) {
	dcpu::emulator::dcpu cpu;
	unique_ptr<argument> a, b;

	b = create_register_arg(cpu, 0x000f);
	a = create_literal_arg(0xfff0);

	EXECUTE_BASIC_OPCODE(ifc, cpu, a, b)
	EXPECT_FALSE(cpu.is_skip_next());

	b = create_register_arg(cpu, 0xfff0);
	a = create_literal_arg(0xfff0);

	EXECUTE_BASIC_OPCODE(ifc, cpu, a, b)
	EXPECT_TRUE(cpu.is_skip_next());
}

TEST(Opcodes, Ife) {
	dcpu::emulator::dcpu cpu;
	unique_ptr<argument> a, b;

	b = create_register_arg(cpu, 2);
	a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(ife, cpu, a, b)
	EXPECT_FALSE(cpu.is_skip_next());

	b = create_register_arg(cpu, 1);
	a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(ife, cpu, a, b)
	EXPECT_TRUE(cpu.is_skip_next());
}

TEST(Opcodes, Ifn) {
	dcpu::emulator::dcpu cpu;
	unique_ptr<argument> a, b;

	b = create_register_arg(cpu, 1);
	a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(ifn, cpu, a, b)
	EXPECT_FALSE(cpu.is_skip_next());

	b = create_register_arg(cpu, 2);
	a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(ifn, cpu, a, b)
	EXPECT_TRUE(cpu.is_skip_next());
}


TEST(Opcodes, Ifg) {
	dcpu::emulator::dcpu cpu;
	unique_ptr<argument> a, b;

	b = create_register_arg(cpu, 3);
	a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(ifg, cpu, a, b)
	EXPECT_FALSE(cpu.is_skip_next());

	b = create_register_arg(cpu, 1);
	a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(ifg, cpu, a, b)
	EXPECT_TRUE(cpu.is_skip_next());
}

TEST(Opcodes, Ifa) {
	dcpu::emulator::dcpu cpu;
	unique_ptr<argument> a, b;

	b = create_register_arg(cpu, -2);
	a = create_literal_arg(-3);

	EXECUTE_BASIC_OPCODE(ifa, cpu, a, b)
	EXPECT_FALSE(cpu.is_skip_next());

	b = create_register_arg(cpu, -3);
	a = create_literal_arg(-2);

	EXECUTE_BASIC_OPCODE(ifa, cpu, a, b)
	EXPECT_TRUE(cpu.is_skip_next());
}

TEST(Opcodes, Ifl) {
	dcpu::emulator::dcpu cpu;
	unique_ptr<argument> a, b;

	b = create_register_arg(cpu, 2);
	a = create_literal_arg(3);

	EXECUTE_BASIC_OPCODE(ifl, cpu, a, b)
	EXPECT_FALSE(cpu.is_skip_next());

	b = create_register_arg(cpu, 2);
	a = create_literal_arg(1);

	EXECUTE_BASIC_OPCODE(ifl, cpu, a, b)
	EXPECT_TRUE(cpu.is_skip_next());
}

TEST(Opcodes, Ifu) {
	dcpu::emulator::dcpu cpu;
	unique_ptr<argument> a, b;

	b = create_register_arg(cpu, -3);
	a = create_literal_arg(-2);

	EXECUTE_BASIC_OPCODE(ifu, cpu, a, b)
	EXPECT_FALSE(cpu.is_skip_next());

	b = create_register_arg(cpu, -2);
	a = create_literal_arg(-3);

	EXECUTE_BASIC_OPCODE(ifu, cpu, a, b)
	EXPECT_TRUE(cpu.is_skip_next());
}

TEST(Opcodes, Adx) {
	dcpu::emulator::dcpu cpu;

	cpu.ex = 10;
	unique_ptr<argument> b = create_register_arg(cpu, 22);
	unique_ptr<argument> a = create_literal_arg(18);

	EXECUTE_BASIC_OPCODE(adx, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(50, cpu.a);
}


TEST(Opcodes, AdxWithOverflow) {
	dcpu::emulator::dcpu cpu;

	cpu.ex = 3;
	unique_ptr<argument> b = create_register_arg(cpu, 65035);
	unique_ptr<argument> a = create_literal_arg(500);

	EXECUTE_BASIC_OPCODE(adx, cpu, a, b)

	EXPECT_EQ(1, cpu.ex);
	EXPECT_EQ(2, cpu.a);
}

TEST(Opcodes, Sbx) {
	dcpu::emulator::dcpu cpu;

	cpu.ex = 4;
	unique_ptr<argument> b = create_register_arg(cpu, 22);
	unique_ptr<argument> a = create_literal_arg(18);

	EXECUTE_BASIC_OPCODE(sbx, cpu, a, b)

	EXPECT_EQ(0, cpu.ex);
	EXPECT_EQ(8, cpu.a);
}


TEST(Opcodes, SbxWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	cpu.ex = 1;
	unique_ptr<argument> b = create_register_arg(cpu, 0);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(sbx, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.ex);
	EXPECT_EQ(0xffff, cpu.a);
}

TEST(Opcodes, Sti) {
	dcpu::emulator::dcpu cpu;

	cpu.i = 42;
	cpu.j = 1;
	unique_ptr<argument> b = create_register_arg(cpu, 0);
	unique_ptr<argument> a = create_literal_arg(0xff);

	EXECUTE_BASIC_OPCODE(sti, cpu, a, b)

	EXPECT_EQ(0xff, cpu.a);
	EXPECT_EQ(43, cpu.i);
	EXPECT_EQ(2, cpu.j);
}

TEST(Opcodes, Std) {
	dcpu::emulator::dcpu cpu;

	cpu.i = 42;
	cpu.j = 1;
	unique_ptr<argument> b = create_register_arg(cpu, 0);
	unique_ptr<argument> a = create_literal_arg(0xff);

	EXECUTE_BASIC_OPCODE(std, cpu, a, b)

	EXPECT_EQ(0xff, cpu.a);
	EXPECT_EQ(41, cpu.i);
	EXPECT_EQ(0, cpu.j);
}