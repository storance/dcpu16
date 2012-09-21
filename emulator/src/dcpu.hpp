#pragma once

#include <cstdint>
#include <vector>
#include <queue>
#include <memory>

namespace dcpu { namespace emulator {
	enum class registers : std::uint8_t {
		A, B, C, X, Y, Z, I, J, SP, PC, EX, IA
	};

	class dcpu;
	class hardware_device;

	class dcpu_stack {
		dcpu &cpu;
	public:
		dcpu_stack(dcpu &cpu);

		void push(std::uint16_t value);
		std::uint16_t &push();
		std::uint16_t &pop();
		std::uint16_t &peek();
		std::uint16_t &pick(uint16_t offset);
	};

	class dcpu_registers {
		dcpu &cpu;
		std::uint16_t &get(registers reg);
	public:
		std::uint16_t a, b, c, x, y, z, i, j, sp, pc, ex, ia;

		dcpu_registers(dcpu &cpu);

		std::uint16_t &operator[] (registers reg);
		std::uint16_t &indirect(registers reg, uint16_t offset=0);
	};

	class dcpu_interrupt_handler {
		enum { QUEUE_MAX_SIZE = 256 };

		dcpu &cpu;
		bool queue_enabled;
		std::queue<uint16_t> queue;

		void trigger(uint16_t message);
	public:
		dcpu_interrupt_handler(dcpu &cpu);

		void disable_queue();
		void enable_queue();

		bool is_queue_enabled();

		void send(uint16_t message);
	};

	class dcpu_hardware_manager {
		enum { MAX_DEVICES = 65535 };

		dcpu &cpu;
		std::vector<std::shared_ptr<hardware_device>> hardware;
	public:
		dcpu_hardware_manager(dcpu &cpu);

		uint16_t get_count();
		void query(uint16_t index);
		uint16_t interrupt(uint16_t index);

		void register_device(std::shared_ptr<hardware_device> device);
	};
	

	class dcpu {
		enum { TOTAL_MEMORY=65536 };

		bool skip_next;
		bool on_fire;
		std::uint64_t cycles;

		void add_cycles(bool simulate_cpu_speed);
	public:
		std::uint16_t memory[TOTAL_MEMORY];
		dcpu_stack stack;
		dcpu_registers registers;
		dcpu_interrupt_handler interrupt_handler;
		dcpu_hardware_manager hardware_manager;

		dcpu();

		std::uint16_t get_next_word();
		bool is_interrupt_queue_enabled();
		bool is_skip_next();

		void skip_next_instruction();

		void catch_fire();
		bool is_on_fire();
	};

	
}}
