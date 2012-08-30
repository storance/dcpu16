#include "argument.hpp"

namespace dcpu { namespace emulator {
	std::unique_ptr<argument>&& argument::parse(dcpu &cpu, uint8_t code, bool isA) {
		if (code >= ARG_REGISTER_START && code <= ARG_REGISTER_END) {
			return new writable_argument(cpu->registers + code);
		} else if (code >= ARG_INDIRECT_REGISTER_START && code <= ARG_INDIRECT_REGISTER_END) {
			uint16_t offset = cpu->registers[code- ARG_INDIRECT_REGISTER_START];
			return new writable_argument(cpu->memory + offset);
		} else if (code >= ARG_INDIRECT_OFFSET_REGISTER_START && code <= ARG_INDIRECT_OFFSET_REGISTER_END) {
			uint16_t offset = cpu->registers[code - ARG_INDIRECT_OFFSET_REGISTER_START];
			return new writable_argument(cpu->memory + offset);
		} else if (code == ARG_PUSH_POP && isA) {
			return new writable_argument(cpu->memory + dcpu->sp++);
		} else if (code == ARG_PUSH_POP && !isA) {
			return new writable_argument(cpu->memory + --dcpu->sp);
		} else if (code == ARG_PEEK) {
			return new writable_argument(cpu->memory + dcpu->sp);
		} else if (code == ARG_PICK) {
			uint16_t offset = dcpu->sp + cpu->get_next_word();
			return new writable_argument(cpu->memory + offset);
		} else if (code == ARG_SP) {
			return new writable_argument(&cpu->sp);
		} else if (code == ARG_PC) {
			return new writable_argument(&cpu->pc);
		} else if (code == ARG_EX) {
			return new writable_argument(&cpu->ex);
		} else if (code == ARG_INDIRECT_NEXT_WORD) {
			return new writable_argument(cpu->memory + cpu->get_next_word());
		} else if (code == ARG_NEXT_WORD) {
			return new literal_argument(cpu->get_next_word());
		} else if (code >= ARG_LITERAL_START && code <= ARG_LITERAL_END && isA) {
			return new literal_argument(code - ARG_LITERAL_START - 1);
		}

		// TODO: throw exception
	}

	argument::~argument() {

	}

	writable_argument::writable_argument(uint16_t* value) : value(value) {

	}

	uint16_t writable_argument::get() {
		return *value;
	}

	void writable_argument::set(uint16_t value) {
		*this->value = value;
	}

	literal_argument::literal_argument(uint16_t value) : value(value) {

	}

	uint16_t literal_argument::get() {
		return value;
	}

	void literal_argument::set(uint16_t value) {
		// no-op
	}
}}