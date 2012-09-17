#pragma once

#include <cstdint>
#include <vector>

namespace dcpu { namespace emulator {

	enum { TOTAL_REGISTERS=11, TOTAL_MEMORY=65536 };

	enum class registers : std::uint8_t {
		A, B, C, X, Y, Z, I, J, SP, PC, EX
	};

	class hardware_device;

	class dcpu {
	private:
		std::uint64_t cycles;
		std::vector<hardware_device> registered_hardware;

	public:
		std::uint16_t memory[TOTAL_MEMORY];
		union {
			std::uint16_t registers[TOTAL_REGISTERS];
			struct {
				std::uint16_t a, b, c, x, y, z, i, j, sp, pc, ex;
			};
		};

		std::uint16_t read_register(enum registers reg);
		void write_register(enum registers reg, std::uint16_t value);

		void read_memory(std::uint16_t address, std::uint16_t *buf, std::uint16_t buflen);
		void write_memory(std::uint16_t address, std::uint16_t *src, std::uint16_t srclen);

		void push(std::uint16_t value);
		std::uint16_t pop();
		std::uint16_t peek();

		std::uint16_t get_next_word();

		void add_cycles(std::uint16_t cycles);
	};

}}
