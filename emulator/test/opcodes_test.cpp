#include <gtest/gtest.h>
#include <dcpu.hpp>
#include <hardware.hpp>
#include <argument.hpp>
#include <opcodes.hpp>
#include <tuple>
#include <functional>

#include "utils/test_hardware.hpp"

using namespace std;
using namespace dcpu::emulator;


#define EXECUTE_BASIC_OPCODE(op, cpu, a, b) { \
op ## Opcode opcode(cpu, a, b); \
opcode.execute(); \
}

#define EXECUTE_SPECIAL_OPCODE(op, cpu, a) { \
op ## Opcode opcode(cpu, a); \
opcode.execute(); \
}

static ArgumentPtr createRegisterArgument(Dcpu &cpu, uint16_t initial_value) {
	cpu.registers.a = initial_value;

	return ArgumentPtr(new RegisterArgument(cpu, registers::A));
}

static ArgumentPtr createLiteralArgument(uint16_t value) {
	return ArgumentPtr(new LiteralArgument(value));
}

TEST(OpcodesTest, Set) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0);
	ArgumentPtr a = createLiteralArgument(17);

	EXECUTE_BASIC_OPCODE(set, cpu, a, b)

	EXPECT_EQ(17, cpu.registers.a);
}

TEST(OpcodesTest, Add) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 22);
	ArgumentPtr a = createLiteralArgument(18);

	EXECUTE_BASIC_OPCODE(add, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(40, cpu.registers.a);
}


TEST(OpcodesTest, AddWithOverflow) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 40000);
	ArgumentPtr a = createLiteralArgument(30000);

	EXECUTE_BASIC_OPCODE(add, cpu, a, b)

	EXPECT_EQ(1, cpu.registers.ex);
	EXPECT_EQ(4464, cpu.registers.a);
}

TEST(OpcodesTest, Sub) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 22);
	ArgumentPtr a = createLiteralArgument(18);

	EXECUTE_BASIC_OPCODE(sub, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(4, cpu.registers.a);
}


TEST(OpcodesTest, SubWithUnderflow) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0);
	ArgumentPtr a = createLiteralArgument(1);

	EXECUTE_BASIC_OPCODE(sub, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.registers.ex);
	EXPECT_EQ(0xffff, cpu.registers.a);
}

TEST(OpcodesTest, Mul) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 2);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(mul, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(4, cpu.registers.a);
}

TEST(OpcodesTest, MulWithOverflow) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0x8000);
	ArgumentPtr a = createLiteralArgument(0x8000);

	EXECUTE_BASIC_OPCODE(mul, cpu, a, b)

	EXPECT_EQ(0x4000, cpu.registers.ex);
	EXPECT_EQ(0, cpu.registers.a);
}

TEST(OpcodesTest, Mli) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, -2);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.registers.ex);
	EXPECT_EQ((uint16_t)-4, cpu.registers.a);
}

TEST(OpcodesTest, MliWithUnderflow) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0xfffe);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.registers.ex);
	EXPECT_EQ(0xfffc, cpu.registers.a);
}

TEST(OpcodesTest, MliWithOverflow) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0x4000);
	ArgumentPtr a = createLiteralArgument(0x10);

	EXECUTE_BASIC_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(4, cpu.registers.ex);
	EXPECT_EQ(0, cpu.registers.a);
}

TEST(OpcodesTest, Div) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 80);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(div, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(40, cpu.registers.a);
}

TEST(OpcodesTest, DivByZero) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 80);
	ArgumentPtr a = createLiteralArgument(0);

	EXECUTE_BASIC_OPCODE(div, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(0, cpu.registers.a);
}

TEST(OpcodesTest, DivWithFractionalPart) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0xffff);
	ArgumentPtr a = createLiteralArgument(0xd);

	EXECUTE_BASIC_OPCODE(div, cpu, a, b)

	EXPECT_EQ(0x2762, cpu.registers.ex);
	EXPECT_EQ(0x13b1, cpu.registers.a);
}

TEST(OpcodesTest, Dvi) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, -80);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(dvi, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ((uint16_t)-40, cpu.registers.a);
}

TEST(OpcodesTest, DviByZero) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 80);
	ArgumentPtr a = createLiteralArgument(0);

	EXECUTE_BASIC_OPCODE(dvi, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(0, cpu.registers.a);
}

TEST(OpcodesTest, DviWithFractionalPart) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0x8001);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(dvi, cpu, a, b)

	EXPECT_EQ(0x8000, cpu.registers.ex);
	EXPECT_EQ(0xc000, cpu.registers.a);
}

TEST(OpcodesTest, Mod) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 21);
	ArgumentPtr a = createLiteralArgument(16);

	EXECUTE_BASIC_OPCODE(mod, cpu, a, b)

	EXPECT_EQ(5, cpu.registers.a);
}

TEST(OpcodesTest, Mdi) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, -7);
	ArgumentPtr a = createLiteralArgument(16);

	EXECUTE_BASIC_OPCODE(mdi, cpu, a, b)

	EXPECT_EQ((uint16_t)-7, cpu.registers.a);
}

TEST(OpcodesTest, And) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0xf0f0);
	ArgumentPtr a = createLiteralArgument(0x7031);

	EXECUTE_BASIC_OPCODE(and, cpu, a, b)

	EXPECT_EQ(0x7030, cpu.registers.a);
}

TEST(OpcodesTest, Bor) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0xf0f0);
	ArgumentPtr a = createLiteralArgument(0x7a0f);

	EXECUTE_BASIC_OPCODE(bor, cpu, a, b)

	EXPECT_EQ(0xfaff, cpu.registers.a);
}

TEST(OpcodesTest, Xor) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0xff0f);
	ArgumentPtr a = createLiteralArgument(0xf0f0);

	EXECUTE_BASIC_OPCODE(xor, cpu, a, b)

	EXPECT_EQ(0x0fff, cpu.registers.a);
}

TEST(OpcodesTest, Shr) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 8);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(shr, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(2, cpu.registers.a);
}

TEST(OpcodesTest, ShrWithUnderflow) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 7);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(shr, cpu, a, b)

	EXPECT_EQ(0xc000, cpu.registers.ex);
	EXPECT_EQ(1, cpu.registers.a);
}

TEST(OpcodesTest, Asr) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, -8);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(asr, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ((uint16_t)-2, cpu.registers.a);
}

TEST(OpcodesTest, AsrWithUnderflow) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, -7);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(asr, cpu, a, b)

	EXPECT_EQ(0x4000, cpu.registers.ex);
	EXPECT_EQ((uint16_t)-2, cpu.registers.a);
}

TEST(OpcodesTest, Shl) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 8);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(shl, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(32, cpu.registers.a);
}

TEST(OpcodesTest, ShlWithOverflow) {
	Dcpu cpu;

	ArgumentPtr b = createRegisterArgument(cpu, 0x100f);
	ArgumentPtr a = createLiteralArgument(4);

	EXECUTE_BASIC_OPCODE(shl, cpu, a, b)

	EXPECT_EQ(0x0001, cpu.registers.ex);
	EXPECT_EQ(0x00f0, cpu.registers.a);
}

TEST(OpcodesTest, Ifb) {
	Dcpu cpu;
	ArgumentPtr a, b;

	b = createRegisterArgument(cpu, 0x000f);
	a = createLiteralArgument(0x000f);

	EXECUTE_BASIC_OPCODE(ifb, cpu, a, b)
	EXPECT_FALSE(cpu.isSkipNext());

	b = createRegisterArgument(cpu, 0x000f);
	a = createLiteralArgument(0xfff0);

	EXECUTE_BASIC_OPCODE(ifb, cpu, a, b)
	EXPECT_TRUE(cpu.isSkipNext());
}

TEST(OpcodesTest, Ifc) {
	Dcpu cpu;
	ArgumentPtr a, b;

	b = createRegisterArgument(cpu, 0x000f);
	a = createLiteralArgument(0xfff0);

	EXECUTE_BASIC_OPCODE(ifc, cpu, a, b)
	EXPECT_FALSE(cpu.isSkipNext());

	b = createRegisterArgument(cpu, 0xfff0);
	a = createLiteralArgument(0xfff0);

	EXECUTE_BASIC_OPCODE(ifc, cpu, a, b)
	EXPECT_TRUE(cpu.isSkipNext());
}

TEST(OpcodesTest, Ife) {
	Dcpu cpu;
	ArgumentPtr a, b;

	b = createRegisterArgument(cpu, 2);
	a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(ife, cpu, a, b)
	EXPECT_FALSE(cpu.isSkipNext());

	b = createRegisterArgument(cpu, 1);
	a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(ife, cpu, a, b)
	EXPECT_TRUE(cpu.isSkipNext());
}

TEST(OpcodesTest, Ifn) {
	Dcpu cpu;
	ArgumentPtr a, b;

	b = createRegisterArgument(cpu, 1);
	a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(ifn, cpu, a, b)
	EXPECT_FALSE(cpu.isSkipNext());

	b = createRegisterArgument(cpu, 2);
	a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(ifn, cpu, a, b)
	EXPECT_TRUE(cpu.isSkipNext());
}


TEST(OpcodesTest, Ifg) {
	Dcpu cpu;
	ArgumentPtr a, b;

	b = createRegisterArgument(cpu, 3);
	a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(ifg, cpu, a, b)
	EXPECT_FALSE(cpu.isSkipNext());

	b = createRegisterArgument(cpu, 1);
	a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(ifg, cpu, a, b)
	EXPECT_TRUE(cpu.isSkipNext());
}

TEST(OpcodesTest, Ifa) {
	Dcpu cpu;
	ArgumentPtr a, b;

	b = createRegisterArgument(cpu, -2);
	a = createLiteralArgument(-3);

	EXECUTE_BASIC_OPCODE(ifa, cpu, a, b)
	EXPECT_FALSE(cpu.isSkipNext());

	b = createRegisterArgument(cpu, -3);
	a = createLiteralArgument(-2);

	EXECUTE_BASIC_OPCODE(ifa, cpu, a, b)
	EXPECT_TRUE(cpu.isSkipNext());
}

TEST(OpcodesTest, Ifl) {
	Dcpu cpu;
	ArgumentPtr a, b;

	b = createRegisterArgument(cpu, 2);
	a = createLiteralArgument(3);

	EXECUTE_BASIC_OPCODE(ifl, cpu, a, b)
	EXPECT_FALSE(cpu.isSkipNext());

	b = createRegisterArgument(cpu, 2);
	a = createLiteralArgument(1);

	EXECUTE_BASIC_OPCODE(ifl, cpu, a, b)
	EXPECT_TRUE(cpu.isSkipNext());
}

TEST(OpcodesTest, Ifu) {
	Dcpu cpu;
	ArgumentPtr a, b;

	b = createRegisterArgument(cpu, -3);
	a = createLiteralArgument(-2);

	EXECUTE_BASIC_OPCODE(ifu, cpu, a, b)
	EXPECT_FALSE(cpu.isSkipNext());

	b = createRegisterArgument(cpu, -2);
	a = createLiteralArgument(-3);

	EXECUTE_BASIC_OPCODE(ifu, cpu, a, b)
	EXPECT_TRUE(cpu.isSkipNext());
}

TEST(OpcodesTest, Adx) {
	Dcpu cpu;

	cpu.registers.ex = 10;
	ArgumentPtr b = createRegisterArgument(cpu, 22);
	ArgumentPtr a = createLiteralArgument(18);

	EXECUTE_BASIC_OPCODE(adx, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(50, cpu.registers.a);
}


TEST(OpcodesTest, AdxWithOverflow) {
	Dcpu cpu;

	cpu.registers.ex = 3;
	ArgumentPtr b = createRegisterArgument(cpu, 65035);
	ArgumentPtr a = createLiteralArgument(500);

	EXECUTE_BASIC_OPCODE(adx, cpu, a, b)

	EXPECT_EQ(1, cpu.registers.ex);
	EXPECT_EQ(2, cpu.registers.a);
}

TEST(OpcodesTest, Sbx) {
	Dcpu cpu;

	cpu.registers.ex = 4;
	ArgumentPtr b = createRegisterArgument(cpu, 22);
	ArgumentPtr a = createLiteralArgument(18);

	EXECUTE_BASIC_OPCODE(sbx, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(8, cpu.registers.a);
}


TEST(OpcodesTest, SbxWithUnderflow) {
	Dcpu cpu;

	cpu.registers.ex = 1;
	ArgumentPtr b = createRegisterArgument(cpu, 0);
	ArgumentPtr a = createLiteralArgument(2);

	EXECUTE_BASIC_OPCODE(sbx, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.registers.ex);
	EXPECT_EQ(0xffff, cpu.registers.a);
}

TEST(OpcodesTest, Sti) {
	Dcpu cpu;

	cpu.registers.i = 42;
	cpu.registers.j = 1;
	ArgumentPtr b = createRegisterArgument(cpu, 0);
	ArgumentPtr a = createLiteralArgument(0xff);

	EXECUTE_BASIC_OPCODE(sti, cpu, a, b)

	EXPECT_EQ(0xff, cpu.registers.a);
	EXPECT_EQ(43, cpu.registers.i);
	EXPECT_EQ(2, cpu.registers.j);
}

TEST(OpcodesTest, Std) {
	Dcpu cpu;

	cpu.registers.i = 42;
	cpu.registers.j = 1;
	ArgumentPtr b = createRegisterArgument(cpu, 0);
	ArgumentPtr a = createLiteralArgument(0xff);

	EXECUTE_BASIC_OPCODE(std, cpu, a, b)

	EXPECT_EQ(0xff, cpu.registers.a);
	EXPECT_EQ(41, cpu.registers.i);
	EXPECT_EQ(0, cpu.registers.j);
}

TEST(OpcodesTest, Jsr) {
	Dcpu cpu;

	cpu.registers.pc = 0x001c;
	ArgumentPtr a = createLiteralArgument(0x00ff);

	EXECUTE_SPECIAL_OPCODE(jsr, cpu, a)

	EXPECT_EQ(0x00ff, cpu.registers.pc);
	EXPECT_EQ(0x001c, cpu.stack.peek());
}

TEST(OpcodesTest, Hcf) {
	Dcpu cpu;

	ArgumentPtr a = createLiteralArgument(0x00ff);

	EXECUTE_SPECIAL_OPCODE(hcf, cpu, a)

	EXPECT_TRUE(cpu.isOnFire());
}

TEST(OpcodesTest, Int) {
	Dcpu cpu;

	cpu.registers.a = 42;
	cpu.registers.pc = 0x001c;
	cpu.registers.ia = 0x00a3;
	ArgumentPtr a = createLiteralArgument(0x00ff);

	EXECUTE_SPECIAL_OPCODE(int, cpu, a)

	EXPECT_TRUE(cpu.interrupts.isQueueEnabled());
	EXPECT_EQ(0x00a3, cpu.registers.pc);
	EXPECT_EQ(0x00ff, cpu.registers.a);
	EXPECT_EQ(42, cpu.stack.pick(0));
	EXPECT_EQ(0x001c, cpu.stack.pick(1));
}

TEST(OpcodesTest, IntWithNoHandler) {
	Dcpu cpu;

	cpu.registers.pc = 0x001c;
	ArgumentPtr a = createLiteralArgument(0x00ff);

	EXECUTE_SPECIAL_OPCODE(int, cpu, a)

	EXPECT_EQ(0x001c, cpu.registers.pc);
	EXPECT_EQ(0, cpu.registers.sp);
}

TEST(OpcodesTest, Iag) {
	Dcpu cpu;

	cpu.registers.ia = 42;
	ArgumentPtr a = createRegisterArgument(cpu, 0);

	EXECUTE_SPECIAL_OPCODE(iag, cpu, a)

	EXPECT_EQ(42, cpu.registers.a);
	EXPECT_EQ(42, cpu.registers.ia);
}

TEST(OpcodesTest, Ias) {
	Dcpu cpu;

	cpu.registers.ia = 42;
	ArgumentPtr a = createRegisterArgument(cpu, 0xff);

	EXECUTE_SPECIAL_OPCODE(ias, cpu, a)

	EXPECT_EQ(0xff, cpu.registers.a);
	EXPECT_EQ(0xff, cpu.registers.ia);
}

TEST(OpcodesTest, Iaq) {
	Dcpu cpu;

	ArgumentPtr a = createLiteralArgument(1);

	EXECUTE_SPECIAL_OPCODE(iaq, cpu, a)
	EXPECT_TRUE(cpu.interrupts.isQueueEnabled());

	a = createLiteralArgument(0);

	EXECUTE_SPECIAL_OPCODE(iaq, cpu, a)
	EXPECT_FALSE(cpu.interrupts.isQueueEnabled());
}

TEST(OpcodesTest, Hwn) {
	Dcpu cpu;

	cpu.hardwareManager.registerDevice(make_shared<TestHardware>(cpu));

	ArgumentPtr a = createRegisterArgument(cpu, 30);

	EXECUTE_SPECIAL_OPCODE(hwn, cpu, a)
	
	EXPECT_EQ(1, cpu.registers.a);
}

TEST(OpcodesTest, HwnWithNoHardware) {
	Dcpu cpu;

	ArgumentPtr a = createRegisterArgument(cpu, 30);

	EXECUTE_SPECIAL_OPCODE(hwn, cpu, a)

	EXPECT_EQ(0, cpu.registers.a);
}

TEST(OpcodesTest, Hwq) {
	Dcpu cpu;

	cpu.hardwareManager.registerDevice(make_shared<TestHardware>(cpu));

	ArgumentPtr a = createLiteralArgument(0);

	EXECUTE_SPECIAL_OPCODE(hwq, cpu, a)
	
	EXPECT_EQ(0x0304, cpu.registers.a);
	EXPECT_EQ(0x0102, cpu.registers.b);
	EXPECT_EQ(0x0003, cpu.registers.c);
	EXPECT_EQ(0x3c4d, cpu.registers.x);
	EXPECT_EQ(0x1a2b, cpu.registers.y);
}

TEST(OpcodesTest, HwqOutOfBounds) {
	Dcpu cpu;

	cpu.hardwareManager.registerDevice(make_shared<TestHardware>(cpu));

	ArgumentPtr a = createLiteralArgument(1);

	EXECUTE_SPECIAL_OPCODE(hwq, cpu, a)
	
	EXPECT_EQ(0, cpu.registers.a);
	EXPECT_EQ(0, cpu.registers.b);
	EXPECT_EQ(0, cpu.registers.c);
	EXPECT_EQ(0, cpu.registers.x);
	EXPECT_EQ(0, cpu.registers.y);
}

TEST(OpcodesTest, Hwi) {
	Dcpu cpu;

	auto device = make_shared<TestHardware>(cpu);
	cpu.hardwareManager.registerDevice(device);

	ArgumentPtr a = createLiteralArgument(0);

	EXECUTE_SPECIAL_OPCODE(hwi, cpu, a)
	
	EXPECT_TRUE(device->interruptCalled);
}

TEST(OpcodesTest, HwiOutOfBounds) {
	Dcpu cpu;

	auto device = make_shared<TestHardware>(cpu);
	cpu.hardwareManager.registerDevice(device);

	ArgumentPtr a = createLiteralArgument(1);

	EXECUTE_SPECIAL_OPCODE(hwi, cpu, a)
	
	// dummy hardware 
	EXPECT_FALSE(device->interruptCalled);
	EXPECT_EQ(0, cpu.registers.z);
}