#include <gtest/gtest.h>
#include <iostream>
#include <tuple>

#include <opcodes.hpp>

using namespace std;
using dcpu::emulator::opcode;

class OpcodesParserTest: public ::testing::TestWithParam<tuple<uint16_t, string>> {
public:
	void SetUp() {
		auto data = GetParam();
		instruction = get<0>(data);
		expected_string = get<1>(data);
	}
protected:
	uint16_t instruction;
	string expected_string;
};

TEST_P(OpcodesParserTest, Parse) {
	dcpu::emulator::dcpu cpu;

	unique_ptr<opcode> op = opcode::parse(cpu, instruction);
	EXPECT_EQ(expected_string, op->to_str());
}

INSTANTIATE_TEST_CASE_P(All, OpcodesParserTest, ::testing::Values(
	make_tuple((uint16_t)0x0001, string("set A, A")),
	make_tuple((uint16_t)0x0002, string("add A, A")),
	make_tuple((uint16_t)0x0003, string("sub A, A")),
	make_tuple((uint16_t)0x0004, string("mul A, A")),
	make_tuple((uint16_t)0x0005, string("mli A, A")),
	make_tuple((uint16_t)0x0006, string("div A, A")),
	make_tuple((uint16_t)0x0007, string("dvi A, A")),
	make_tuple((uint16_t)0x0008, string("mod A, A")),
	make_tuple((uint16_t)0x0009, string("mdi A, A")),
	make_tuple((uint16_t)0x000a, string("and A, A")),
	make_tuple((uint16_t)0x000b, string("bor A, A")),
	make_tuple((uint16_t)0x000c, string("xor A, A")),
	make_tuple((uint16_t)0x000d, string("shr A, A")),
	make_tuple((uint16_t)0x000e, string("asr A, A")),
	make_tuple((uint16_t)0x000f, string("shl A, A")),
	make_tuple((uint16_t)0xfd50, string("ifb [C], 30")),
	make_tuple((uint16_t)0xf971, string("ifc [X], 29")),
	make_tuple((uint16_t)0x0012, string("ife A, A")),
	make_tuple((uint16_t)0x0013, string("ifn A, A")),
	make_tuple((uint16_t)0x0014, string("ifg A, A")),
	make_tuple((uint16_t)0x0015, string("ifa A, A")),
	make_tuple((uint16_t)0x0016, string("ifl A, A")),
	make_tuple((uint16_t)0x0017, string("ifu A, A")),
	make_tuple((uint16_t)0x001a, string("adx A, A")),
	make_tuple((uint16_t)0x001b, string("sbx A, A")),
	make_tuple((uint16_t)0x001e, string("sti A, A")),
	make_tuple((uint16_t)0x001f, string("std A, A")),
	make_tuple((uint16_t)0x0020, string("jsr A")),
	make_tuple((uint16_t)0x00e0, string("hcf A")),
	make_tuple((uint16_t)0x0100, string("int A")),
	make_tuple((uint16_t)0x0120, string("iag A")),
	make_tuple((uint16_t)0x0140, string("ias A")),
	make_tuple((uint16_t)0x0160, string("rfi A")),
	make_tuple((uint16_t)0x0180, string("iaq A")),
	make_tuple((uint16_t)0x0200, string("hwn A")),
	make_tuple((uint16_t)0x0220, string("hwq A")),
	make_tuple((uint16_t)0x0240, string("hwi A"))

));
