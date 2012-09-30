#pragma once

#include <cstdint>
#include <vector>
#include <queue>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <atomic>

namespace dcpu { namespace emulator {
	enum class registers : uint8_t {
		A, B, C, X, Y, Z, I, J, SP, PC, EX, IA
	};

	class Dcpu;
	class HardwareDevice;

	class DcpuStack {
		Dcpu &cpu;
	public:
		DcpuStack(Dcpu &cpu);

		void push(uint16_t value);
		uint16_t &push();
		uint16_t &pop();
		uint16_t &peek();
		uint16_t &pick(uint16_t offset);
	};

	class DcpuRegisters {
		Dcpu &cpu;
		uint16_t &get(registers reg);
	public:
		uint16_t a, b, c, x, y, z, i, j, sp, pc, ex, ia;

		DcpuRegisters(Dcpu &cpu);

		uint16_t &operator[] (registers reg);
		uint16_t &indirect(registers reg, uint16_t offset=0);
		void clear();
	};

	class DcpuInterrupts {
		enum { QUEUE_MAX_SIZE = 256 };

		Dcpu &cpu;
		bool queueEnabled;
		std::queue<uint16_t> queue;

		void trigger(uint16_t message);
	public:
		DcpuInterrupts(Dcpu &cpu);

		void disableQueue();
		void enableQueue();

		bool isQueueEnabled();

		void send(uint16_t message);
	};

	class MaxHardwareDevicesException : public std::runtime_error {
	public:
		MaxHardwareDevicesException();
	};

	class DcpuHardwareManager {
		enum { MAX_DEVICES = 65535 };

		Dcpu &cpu;
		std::vector<std::shared_ptr<HardwareDevice>> hardware;
	public:
		DcpuHardwareManager(Dcpu &cpu);

		uint16_t getCount();
		void query(uint16_t index);
		uint16_t interrupt(uint16_t index);
        void tickAll();

		void registerDevice(std::shared_ptr<HardwareDevice> device);
	};

	class Dcpu {
		bool skipNext;
		bool onFire;
		uint64_t cycles;

		void addCycles(uint16_t cyclesAmount, bool simulateCpuSpeed);
	public:
		enum { TOTAL_MEMORY=65536, FREQUENCY=100000 };

		uint16_t memory[TOTAL_MEMORY];
		DcpuStack stack;
		DcpuRegisters registers;
		DcpuInterrupts interrupts;
		DcpuHardwareManager hardwareManager;

		Dcpu();

		uint16_t getCycles();
		uint16_t getNextWord();
		bool isOnFire();
		bool isSkipNext();

		void catchFire();
		void skipNextInstruction();

		void tick();
		void load(const char *filename);
		void dump(std::ostream& out) const;
		void clear();
	};

	std::ostream &operator<<(std::ostream &stream, registers reg);
}}
