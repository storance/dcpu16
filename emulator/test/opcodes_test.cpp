#include <gtest/gtest.h>
#include <dcpu.hpp>
#include <hardware.hpp>
#include <argument.hpp>
#include <opcodes.hpp>
#include <tuple>
#include <functional>

#include "utils/test_hardware.hpp"

using namespace std;

using dcpu::emulator::hardware_device;
using dcpu::emulator::argument;
using dcpu::emulator::literal_argument;
using dcpu::emulator::register_argument;
using dcpu::emulator::registers;


#define EXECUTE_BASIC_OPCODE(op, cpu, a, b) { \
dcpu::emulator::op ## _opcode opcode(cpu, a, b); \
opcode.execute(); \
}

#define EXECUTE_SPECIAL_OPCODE(op, cpu, a) { \
dcpu::emulator::op ## _opcode opcode(cpu, a); \
opcode.execute(); \
}

static unique_ptr<argument> create_register_arg(dcpu::emulator::dcpu &cpu, uint16_t initial_value) {
	cpu.registers.a = initial_value;

	return unique_ptr<argument>(new register_argument(cpu, registers::A));
}

static unique_ptr<argument> create_literal_arg(uint16_t value) {
	return unique_ptr<argument>(new literal_argument(value));
}

TEST(OpcodesTest, Set) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0);
	unique_ptr<argument> a = create_literal_arg(17);

	EXECUTE_BASIC_OPCODE(set, cpu, a, b)

	EXPECT_EQ(17, cpu.registers.a);
}

TEST(OpcodesTest, Add) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 22);
	unique_ptr<argument> a = create_literal_arg(18);

	EXECUTE_BASIC_OPCODE(add, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(40, cpu.registers.a);
}


TEST(OpcodesTest, AddWithOverflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 40000);
	unique_ptr<argument> a = create_literal_arg(30000);

	EXECUTE_BASIC_OPCODE(add, cpu, a, b)

	EXPECT_EQ(1, cpu.registers.ex);
	EXPECT_EQ(4464, cpu.registers.a);
}

TEST(OpcodesTest, Sub) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 22);
	unique_ptr<argument> a = create_literal_arg(18);

	EXECUTE_BASIC_OPCODE(sub, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(4, cpu.registers.a);
}


TEST(OpcodesTest, SubWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0);
	unique_ptr<argument> a = create_literal_arg(1);

	EXECUTE_BASIC_OPCODE(sub, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.registers.ex);
	EXPECT_EQ(0xffff, cpu.registers.a);
}

TEST(OpcodesTest, Mul) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 2);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(mul, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(4, cpu.registers.a);
}

TEST(OpcodesTest, MulWithOverflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0x8000);
	unique_ptr<argument> a = create_literal_arg(0x8000);

	EXECUTE_BASIC_OPCODE(mul, cpu, a, b)

	EXPECT_EQ(0x4000, cpu.registers.ex);
	EXPECT_EQ(0, cpu.registers.a);
}

TEST(OpcodesTest, Mli) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, -2);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.registers.ex);
	EXPECT_EQ((uint16_t)-4, cpu.registers.a);
}

TEST(OpcodesTest, MliWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0xfffe);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.registers.ex);
	EXPECT_EQ(0xfffc, cpu.registers.a);
}

TEST(OpcodesTest, MliWithOverflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0x4000);
	unique_ptr<argument> a = create_literal_arg(0x10);

	EXECUTE_BASIC_OPCODE(mli, cpu, a, b)

	EXPECT_EQ(4, cpu.registers.ex);
	EXPECT_EQ(0, cpu.registers.a);
}

TEST(OpcodesTest, Div) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 80);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(div, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(40, cpu.registers.a);
}

TEST(OpcodesTest, DivByZero) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 80);
	unique_ptr<argument> a = create_literal_arg(0);

	EXECUTE_BASIC_OPCODE(div, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(0, cpu.registers.a);
}

TEST(OpcodesTest, DivWithFractionalPart) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0xffff);
	unique_ptr<argument> a = create_literal_arg(0xd);

	EXECUTE_BASIC_OPCODE(div, cpu, a, b)

	EXPECT_EQ(0x2762, cpu.registers.ex);
	EXPECT_EQ(0x13b1, cpu.registers.a);
}

TEST(OpcodesTest, Dvi) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, -80);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(dvi, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ((uint16_t)-40, cpu.registers.a);
}

TEST(OpcodesTest, DviByZero) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 80);
	unique_ptr<argument> a = create_literal_arg(0);

	EXECUTE_BASIC_OPCODE(dvi, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(0, cpu.registers.a);
}

TEST(OpcodesTest, DviWithFractionalPart) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0x8001);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(dvi, cpu, a, b)

	EXPECT_EQ(0x8000, cpu.registers.ex);
	EXPECT_EQ(0xc000, cpu.registers.a);
}

TEST(OpcodesTest, Mod) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 21);
	unique_ptr<argument> a = create_literal_arg(16);

	EXECUTE_BASIC_OPCODE(mod, cpu, a, b)

	EXPECT_EQ(5, cpu.registers.a);
}

TEST(OpcodesTest, Mdi) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, -7);
	unique_ptr<argument> a = create_literal_arg(16);

	EXECUTE_BASIC_OPCODE(mdi, cpu, a, b)

	EXPECT_EQ((uint16_t)-7, cpu.registers.a);
}

TEST(OpcodesTest, And) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0xf0f0);
	unique_ptr<argument> a = create_literal_arg(0x7031);

	EXECUTE_BASIC_OPCODE(and, cpu, a, b)

	EXPECT_EQ(0x7030, cpu.registers.a);
}

TEST(OpcodesTest, Bor) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0xf0f0);
	unique_ptr<argument> a = create_literal_arg(0x7a0f);

	EXECUTE_BASIC_OPCODE(bor, cpu, a, b)

	EXPECT_EQ(0xfaff, cpu.registers.a);
}

TEST(OpcodesTest, Xor) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0xff0f);
	unique_ptr<argument> a = create_literal_arg(0xf0f0);

	EXECUTE_BASIC_OPCODE(xor, cpu, a, b)

	EXPECT_EQ(0x0fff, cpu.registers.a);
}

TEST(OpcodesTest, Shr) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 8);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(shr, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(2, cpu.registers.a);
}

TEST(OpcodesTest, ShrWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 7);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(shr, cpu, a, b)

	EXPECT_EQ(0xc000, cpu.registers.ex);
	EXPECT_EQ(1, cpu.registers.a);
}

TEST(OpcodesTest, Asr) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, -8);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(asr, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ((uint16_t)-2, cpu.registers.a);
}

TEST(OpcodesTest, AsrWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, -7);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(asr, cpu, a, b)

	EXPECT_EQ(0x4000, cpu.registers.ex);
	EXPECT_EQ((uint16_t)-2, cpu.registers.a);
}

TEST(OpcodesTest, Shl) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 8);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(shl, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(32, cpu.registers.a);
}

TEST(OpcodesTest, ShlWithOverflow) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> b = create_register_arg(cpu, 0x100f);
	unique_ptr<argument> a = create_literal_arg(4);

	EXECUTE_BASIC_OPCODE(shl, cpu, a, b)

	EXPECT_EQ(0x0001, cpu.registers.ex);
	EXPECT_EQ(0x00f0, cpu.registers.a);
}

TEST(OpcodesTest, Ifb) {
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

TEST(OpcodesTest, Ifc) {
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

TEST(OpcodesTest, Ife) {
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

TEST(OpcodesTest, Ifn) {
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


TEST(OpcodesTest, Ifg) {
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

TEST(OpcodesTest, Ifa) {
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

TEST(OpcodesTest, Ifl) {
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

TEST(OpcodesTest, Ifu) {
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

TEST(OpcodesTest, Adx) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.ex = 10;
	unique_ptr<argument> b = create_register_arg(cpu, 22);
	unique_ptr<argument> a = create_literal_arg(18);

	EXECUTE_BASIC_OPCODE(adx, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(50, cpu.registers.a);
}


TEST(OpcodesTest, AdxWithOverflow) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.ex = 3;
	unique_ptr<argument> b = create_register_arg(cpu, 65035);
	unique_ptr<argument> a = create_literal_arg(500);

	EXECUTE_BASIC_OPCODE(adx, cpu, a, b)

	EXPECT_EQ(1, cpu.registers.ex);
	EXPECT_EQ(2, cpu.registers.a);
}

TEST(OpcodesTest, Sbx) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.ex = 4;
	unique_ptr<argument> b = create_register_arg(cpu, 22);
	unique_ptr<argument> a = create_literal_arg(18);

	EXECUTE_BASIC_OPCODE(sbx, cpu, a, b)

	EXPECT_EQ(0, cpu.registers.ex);
	EXPECT_EQ(8, cpu.registers.a);
}


TEST(OpcodesTest, SbxWithUnderflow) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.ex = 1;
	unique_ptr<argument> b = create_register_arg(cpu, 0);
	unique_ptr<argument> a = create_literal_arg(2);

	EXECUTE_BASIC_OPCODE(sbx, cpu, a, b)

	EXPECT_EQ(0xffff, cpu.registers.ex);
	EXPECT_EQ(0xffff, cpu.registers.a);
}

TEST(OpcodesTest, Sti) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.i = 42;
	cpu.registers.j = 1;
	unique_ptr<argument> b = create_register_arg(cpu, 0);
	unique_ptr<argument> a = create_literal_arg(0xff);

	EXECUTE_BASIC_OPCODE(sti, cpu, a, b)

	EXPECT_EQ(0xff, cpu.registers.a);
	EXPECT_EQ(43, cpu.registers.i);
	EXPECT_EQ(2, cpu.registers.j);
}

TEST(OpcodesTest, Std) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.i = 42;
	cpu.registers.j = 1;
	unique_ptr<argument> b = create_register_arg(cpu, 0);
	unique_ptr<argument> a = create_literal_arg(0xff);

	EXECUTE_BASIC_OPCODE(std, cpu, a, b)

	EXPECT_EQ(0xff, cpu.registers.a);
	EXPECT_EQ(41, cpu.registers.i);
	EXPECT_EQ(0, cpu.registers.j);
}

TEST(OpcodesTest, Jsr) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.pc = 0x001c;
	unique_ptr<argument> a = create_literal_arg(0x00ff);

	EXECUTE_SPECIAL_OPCODE(jsr, cpu, a)

	EXPECT_EQ(0x00ff, cpu.registers.pc);
	EXPECT_EQ(0x001c, cpu.stack.peek());
}

TEST(OpcodesTest, Hcf) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> a = create_literal_arg(0x00ff);

	EXECUTE_SPECIAL_OPCODE(hcf, cpu, a)

	EXPECT_TRUE(cpu.is_on_fire());
}

TEST(OpcodesTest, Int) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.a = 42;
	cpu.registers.pc = 0x001c;
	cpu.registers.ia = 0x00a3;
	unique_ptr<argument> a = create_literal_arg(0x00ff);

	EXECUTE_SPECIAL_OPCODE(int, cpu, a)

	EXPECT_TRUE(cpu.interrupt_handler.is_queue_enabled());
	EXPECT_EQ(0x00a3, cpu.registers.pc);
	EXPECT_EQ(0x00ff, cpu.registers.a);
	EXPECT_EQ(42, cpu.stack.pick(0));
	EXPECT_EQ(0x001c, cpu.stack.pick(1));
}

TEST(OpcodesTest, IntWithNoHandler) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.pc = 0x001c;
	unique_ptr<argument> a = create_literal_arg(0x00ff);

	EXECUTE_SPECIAL_OPCODE(int, cpu, a)

	EXPECT_EQ(0x001c, cpu.registers.pc);
	EXPECT_EQ(0, cpu.registers.sp);
}

TEST(OpcodesTest, Iag) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.ia = 42;
	unique_ptr<argument> a = create_register_arg(cpu, 0);

	EXECUTE_SPECIAL_OPCODE(iag, cpu, a)

	EXPECT_EQ(42, cpu.registers.a);
	EXPECT_EQ(42, cpu.registers.ia);
}

TEST(OpcodesTest, Ias) {
	dcpu::emulator::dcpu cpu;

	cpu.registers.ia = 42;
	unique_ptr<argument> a = create_register_arg(cpu, 0xff);

	EXECUTE_SPECIAL_OPCODE(ias, cpu, a)

	EXPECT_EQ(0xff, cpu.registers.a);
	EXPECT_EQ(0xff, cpu.registers.ia);
}

TEST(OpcodesTest, Iaq) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> a = create_literal_arg(1);

	EXECUTE_SPECIAL_OPCODE(iaq, cpu, a)
	EXPECT_TRUE(cpu.interrupt_handler.is_queue_enabled());

	a = create_literal_arg(0);

	EXECUTE_SPECIAL_OPCODE(iaq, cpu, a)
	EXPECT_FALSE(cpu.interrupt_handler.is_queue_enabled());
}

TEST(OpcodesTest, Hwn) {
	dcpu::emulator::dcpu cpu;

	cpu.hardware_manager.register_device(make_shared<test_hardware>(cpu));

	unique_ptr<argument> a = create_register_arg(cpu, 30);

	EXECUTE_SPECIAL_OPCODE(hwn, cpu, a)
	
	EXPECT_EQ(1, cpu.registers.a);
}

TEST(OpcodesTest, HwnWithNoHardware) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<argument> a = create_register_arg(cpu, 30);

	EXECUTE_SPECIAL_OPCODE(hwn, cpu, a)

	EXPECT_EQ(0, cpu.registers.a);
}

TEST(OpcodesTest, Hwq) {
	dcpu::emulator::dcpu cpu;

	cpu.hardware_manager.register_device(make_shared<test_hardware>(cpu));

	unique_ptr<argument> a = create_literal_arg(0);

	EXECUTE_SPECIAL_OPCODE(hwq, cpu, a)
	
	EXPECT_EQ(0x0304, cpu.registers.a);
	EXPECT_EQ(0x0102, cpu.registers.b);
	EXPECT_EQ(0x0003, cpu.registers.c);
	EXPECT_EQ(0x3c4d, cpu.registers.x);
	EXPECT_EQ(0x1a2b, cpu.registers.y);
}

TEST(OpcodesTest, HwqOutOfBounds) {
	dcpu::emulator::dcpu cpu;

	cpu.hardware_manager.register_device(make_shared<test_hardware>(cpu));

	unique_ptr<argument> a = create_literal_arg(1);

	EXECUTE_SPECIAL_OPCODE(hwq, cpu, a)
	
	EXPECT_EQ(0, cpu.registers.a);
	EXPECT_EQ(0, cpu.registers.b);
	EXPECT_EQ(0, cpu.registers.c);
	EXPECT_EQ(0, cpu.registers.x);
	EXPECT_EQ(0, cpu.registers.y);
}

TEST(OpcodesTest, Hwi) {
	dcpu::emulator::dcpu cpu;

	auto device = make_shared<test_hardware>(cpu);
	cpu.hardware_manager.register_device(device);

	unique_ptr<argument> a = create_literal_arg(0);

	EXECUTE_SPECIAL_OPCODE(hwi, cpu, a)
	
	EXPECT_TRUE(device->interrupt_called);
}

TEST(OpcodesTest, HwiOutOfBounds) {
	dcpu::emulator::dcpu cpu;

	auto device = make_shared<test_hardware>(cpu);
	cpu.hardware_manager.register_device(device);

	unique_ptr<argument> a = create_literal_arg(1);

	EXECUTE_SPECIAL_OPCODE(hwi, cpu, a)
	
	// dummy hardware 
	EXPECT_FALSE(device->interrupt_called);
	EXPECT_EQ(0, cpu.registers.z);
}