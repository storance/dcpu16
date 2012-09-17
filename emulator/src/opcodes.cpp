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
		case set_opcode::OPCODE:
			return unique_ptr<opcode>(new set_opcode(cpu, move(arg_a), move(arg_b)));
		case add_opcode::OPCODE:
			return unique_ptr<opcode>(new add_opcode(cpu, move(arg_a), move(arg_b)));
		case sub_opcode::OPCODE:
			return unique_ptr<opcode>(new sub_opcode(cpu, move(arg_a), move(arg_b)));
		default:
			// throw not supported
            break;
		}
	}

	unique_ptr<opcode> opcode::parse_special(dcpu &cpu, uint16_t instruction) {
		uint8_t o = (instruction >> 5) & 0x1f;
		uint8_t a = (instruction >> 10) & 0x3f;

		unique_ptr<argument> arg_a = argument::parse(cpu, move(a), true);
		
		switch (o) {
		default:
			// throw not supported
            break;
		}
	}

	opcode::opcode(dcpu &cpu, unique_ptr<argument> &&a, std::unique_ptr<argument> &&b) : cpu(cpu), a(move(a)), b(move(b)) {

	}

	opcode::opcode(dcpu &cpu, unique_ptr<argument> &&a) : cpu(cpu), a(move(a)), b() {

	}

	opcode::~opcode() {

	}

	set_opcode::set_opcode(dcpu &cpu, unique_ptr<argument> &&a, std::unique_ptr<argument> &&b) : opcode(cpu, move(a), move(b)) {

	}

	uint16_t set_opcode::execute() {
		b->set(a->get());
	}

	add_opcode::add_opcode(dcpu &cpu, unique_ptr<argument> &&a, std::unique_ptr<argument> &&b) : opcode(cpu, move(a), move(b)) {
		
	}

	uint16_t add_opcode::execute() {
		uint32_t result = a->get() + b->get();
		b->set(result);

		cpu.ex = result >> 16;
	}

	sub_opcode::sub_opcode(dcpu &cpu, unique_ptr<argument> &&a, std::unique_ptr<argument> &&b) : opcode(cpu, move(a), move(b)) {
		
	}

	uint16_t sub_opcode::execute() {
		uint32_t result = b->get() - a->get();
		b->set(result);

		cpu.ex = result >> 16;
	}
}}
