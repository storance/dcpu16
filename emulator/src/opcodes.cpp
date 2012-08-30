#include "opcodes.hpp"

using namespace std;

amespace dcpu { namespace emulator {
	unique_ptr<opcode> &&opcode::parse(dcpu &cpu, uint16_t instruction) {
		if (instruction & 0x1f != 0) {
			return move(parse_basic(instruction));
		} else {
			return move(parse_special(instruction));
		}
	}

	unique_ptr<opcode>&& parse_basic(dcpu &cpu, uint16_t instruction) {
		uint8_t o = instruction & 0x1f;
		uint8_t a = (instruction >> 10) & 0x3f;
		uint8_t b = (instruction >> 5) & 0x1f;

		unique_ptr<argument> arg_a = argument::parse(cpu, a, true);
		unique_ptr<argument> arg_b = argument::parse(cpu, b, false);

		switch (o) {
		case set_opcode::OPCODE:
			return unique_ptr<opcode>(new set_opcode(cpu, arg_a, arg_b));
			break;
		case add_opcode::OPCODE:
			return unique_ptr<opcode>(new add_opcode(cpu, arg_a, arg_b));
			break;
		default:
			// throw not supported
		}
	}

	unique_ptr<opcode>&& parse_special(dcpu &cpu, uint16_t instruction) {
		uint8_t o = (instruction >> 5) & 0x1f;
		uint8_t a = (instruction >> 10) & 0x3f;

		unique_ptr arg_a = argument::parse(cpu, a, true);
		
		switch (o) {
		default:
			// throw not supported
		}
	}

	opcode::opcode(dcpu &cpu, unique_ptr<argument> &&a, std::unique_ptr<argument> &&b) : cpu(cpu), a(a), b(b) {

	}

	opcode::opcode(dcpu &cpu, unique_ptr<argument> &&a) : cpu(cpu), a(a), b() {

	}

	opcode::~opcode() {

	}

	uint16_t set_opcode::execute() {
		a.set(b.get());
	}

	uint16_t add_opcode::execute() {
		uint32_t result = a.get() + b.get();
		a.set(result);

		cpu.write_register(registers::EX, result >> 16);
	}
}}