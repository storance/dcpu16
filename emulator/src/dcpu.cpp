#include "dcpu.hpp"

namespace dcpu { namespace emulator {
	uint16_t dcpu::read_register(registers reg) {
		switch (reg) {
		case registers::A:
			return a;
		case registers::B:
			return b;
		case registers::C:
			return c;
		case registers::X:
			return x;
		case registers::Y:
			return y;
		case registers::Z:
			return z;
		case registers::I:
			return i;
		case registers::J:
			return j;
		case registers::SP:
			return sp;
		case registers::PC:
			return pc;
		case registers::EX:
			return ex;
		}
	}

	void dcpu::write_register(registers reg, uint16_t value) {
		switch (reg) {
		case registers::A:
			a = value;
			break;
		case registers::B:
			b = value;
			break;
		case registers::C:
			c = value;
			break;
		case registers::X:
			x = value;
			break;
		case registers::Y:
			y = value;
			break;
		case registers::Z:
			z = value;
			break;
		case registers::I:
			i = value;
			break;
		case registers::J:
			j = value;
			break;
		case registers::SP:
			sp = value;
			break;
		case registers::PC:
			pc = value;
			break;
		case registers::EX:
			ex = value;
			break;
		}
	}

	uint16_t dcpu::read_memory(uint16_t address) {
		if (address < 0) {

		}else if (address >= TOTAL_MEMORY) {

		}

		return memory[address];
	}

	void dcpu::read_memory(uint16_t address, uint16_t *buf, uint16_t buflen) {
		if (buflen == 0) {
			return;
		} else if (buflen < 0) {

		} else if (buflen + address > TOTAL_MEMORY) {

		}

		std::copy(memory+address, memory+address+buflen, buf);
	}

	void dcpu::write_memory(uint16_t address, uint16_t value) {
		if (address < 0) {

		} else if (address >= TOTAL_MEMORY) {

		}

		memory[address] = value;
	}

	void dcpu::write_memory(uint16_t address, uint16_t *src, uint16_t srclen) {
		if (srclen == 0) {
			return;
		} else if (srclen < 0) {

		} else if (srclen + address > TOTAL_MEMORY) {

		}

		std::copy(src, src+srclen, memory+address);
	}

	void dcpu::push(uint16_t value) {
		memory[--sp] = value;
	}

	uint16_t dcpu::pop() {
		return memory[sp++];
	}

	uint16_t dcpu::peek() {
		return memory[sp];
	}

	uint16_t dcpu::get_next_word() {
		if (pc > TOTAL_MEMORY) {

		}

		cycles++;
		return memory[pc++];
	}

	void dcpu::add_cycles(uint16_t cycles) {
		if (cycles < 0) {

		}

		this->cycles += cycles;
	}

}}
