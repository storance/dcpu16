#include "dcpu.hpp"
#include "hardware.hpp"
#include <cstring>

using namespace std;

namespace dcpu { namespace emulator {
	dcpu::dcpu() : skipNext(false), cycles(0), registered_hardware() {
		memset(memory, 0, TOTAL_MEMORY);
		memset(registers, 0, TOTAL_REGISTERS);
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

	bool dcpu::is_skip_next() {
		return skipNext;
	}

}}
