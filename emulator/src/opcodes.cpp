#include "opcodes.hpp"

using namespace std;

namespace dcpu { namespace emulator {
	unique_ptr<opcode> opcode::parse(dcpu &cpu, uint16_t instruction) {
		if ((instruction & 0x1f) != 0) {
			return move(parse_basic(cpu, instruction));
		} else {
			return move(parse_special(cpu, instruction));
		}
	}

	unique_ptr<opcode> opcode::parse_basic(dcpu &cpu, uint16_t instruction) {
		uint8_t o = instruction & 0x1f;
		uint8_t a = (instruction >> 10) & 0x3f;
		uint8_t b = (instruction >> 5) & 0x1f;

		unique_ptr<argument> arg_a = argument::parse(cpu, a, true);
		unique_ptr<argument> arg_b = argument::parse(cpu, b, false);

		switch (o) {
		BASIC_OPCODE_CASE(set, cpu, arg_a, arg_b)
		BASIC_OPCODE_CASE(add, cpu, arg_a, arg_b)
		BASIC_OPCODE_CASE(sub, cpu, arg_a, arg_b)
		BASIC_OPCODE_CASE(mul, cpu, arg_a, arg_b)
		default:
			// throw not supported
            break;
		}
	}

	unique_ptr<opcode> opcode::parse_special(dcpu &cpu, uint16_t instruction) {
		uint8_t o = (instruction >> 5) & 0x1f;
		uint8_t a = (instruction >> 10) & 0x3f;

		unique_ptr<argument> arg_a = argument::parse(cpu, a, true);
		
		switch (o) {
		default:
			// throw not supported
            break;
		}
	}

	opcode::opcode(dcpu &cpu, unique_ptr<argument> &a, std::unique_ptr<argument> &b) : cpu(cpu), a(move(a)), b(move(b)) {

	}

	opcode::opcode(dcpu &cpu, unique_ptr<argument> &a) : cpu(cpu), a(move(a)), b() {

	}

	opcode::~opcode() {

	}

	uint16_t set_opcode::execute() {
		b->set(a->get());

		return 1;
	}

	uint16_t add_opcode::execute() {
		uint32_t result = a->get() + b->get();
		b->set(result);

		cpu.ex = result >> 16;

		return 2;
	}

	uint16_t sub_opcode::execute() {
		uint32_t result = b->get() - a->get();
		b->set(result);

		cpu.ex = result >> 16;

		return 2;
	}

	uint16_t mul_opcode::execute() {
		uint32_t result = b->get() * a->get();
		b->set(result);

		cpu.ex = (result >> 16) & 0xffff;

		return 2;
	}

	uint16_t mli_opcode::execute() {
		int16_t signedA = a->get();
		int16_t signedB = b->get();

		int32_t result = signedA * signedB;
		b->set(result);

		cpu.ex = (result >> 16) & 0xffff;

		return 2;
	}
}}
