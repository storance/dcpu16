#include "dcpu.hpp"

using namespace std;

namespace dcpu { namespace emulator {
	uint16_t dcpu::read_register(enum registers reg) {
		return this->registers[static_cast<uint8_t>(reg)];
	}

	void dcpu::write_register(enum registers reg, uint16_t value) {
		this->registers[static_cast<uint8_t>(reg)] = value;
	}

	void dcpu::read_memory(uint16_t address, uint16_t *buf, uint16_t buflen) {
		if (buflen == 0) {
			return;
		} else if (buflen < 0) {

		} else if (buflen + address > TOTAL_MEMORY) {

		}

		std::copy(memory+address, memory+address+buflen, buf);
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
		cycles++;
		return memory[pc++];
	}

	void dcpu::add_cycles(uint16_t cycles) {
		if (cycles < 0) {

		}

		this->cycles += cycles;
	}

}}
