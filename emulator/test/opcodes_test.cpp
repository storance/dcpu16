#include <gtest/gtest.h>
#include "dcpu.hpp"
#include "hardware.hpp"
#include "argument.hpp"
#include "opcodes.hpp"

using namespace std;

using dcpu::emulator::set_opcode;
using dcpu::emulator::add_opcode;
using dcpu::emulator::sub_opcode;
using dcpu::emulator::argument;
using dcpu::emulator::writable_argument;
using dcpu::emulator::literal_argument;

TEST(Opcodes, Set) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 0;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(17));

	set_opcode opcode(cpu, move(a), move(b));
	opcode.execute();

	EXPECT_EQ(17, result);
}

TEST(Opcodes, AddWithoutOverflow) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 22;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(18));

	add_opcode opcode(cpu, move(a), move(b));
	opcode.execute();

	EXPECT_EQ(40, result);
}


TEST(Opcodes, AddWithOverflow) {
	dcpu::emulator::dcpu cpu;

	uint16_t result = 40000;
	unique_ptr<argument> b(new writable_argument(&result));
	unique_ptr<argument> a(new literal_argument(30000));

	add_opcode opcode(cpu, move(a), move(b));
	opcode.execute();

	EXPECT_EQ(1, cpu.ex);
	EXPECT_EQ(4464, result);
}