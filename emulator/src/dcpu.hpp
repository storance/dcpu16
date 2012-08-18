#pragma once

namespace dcpu { namespace emulator {

	enum { TOTAL_MEMORY=65536 };

	enum class registers {
		A,
		B,
		C,
		X,
		Y,
		Z,
		I,
		J,
		SP,
		PC,
		EX
	};

	class dcpu {
	private:
		uint64_t cycles;

		uint16_t memory[TOTAL_MEMORY];
		uint16_t a, b, c, x, y, z, i, j, sp, pc, ex;
	public:
		uint16_t read_register(registers reg);
		void write_register(registers reg, uint16_t value);

		uint16_t read_memory(uint16_t address);
		void read_memory(uint16_t address, uint16_t *buf, uint16_t buflen);
		void write_memory(uint16_t address, uint16_t value);
		void write_memory(uint16_t address, uint16_t *src, uint16_t srclen);

		void push(uint16_t value);
		uint16_t pop();
		uint16_t peek();

		uint16_t get_next_word();

		void add_cycles(uint16_t cycles);
	};

}}
