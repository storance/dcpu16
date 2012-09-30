#include <cstring>
#include <stdexcept>
#include <fstream>
#include <iomanip>
#include <boost/format.hpp>

#include "dcpu.hpp"
#include "hardware.hpp"
#include "opcodes.hpp"

using namespace std;
using boost::format;
using boost::str;

namespace dcpu { namespace emulator {
	/*************************************************************************
     *
     * Dcpu
     *
     *************************************************************************/

	Dcpu::Dcpu() : skipNext(false), onFire(false), cycles(0), stack(*this), registers(*this),
			interrupts(*this), hardwareManager(*this) {
		memset(memory, 0, TOTAL_MEMORY * sizeof(uint16_t));
	}

	uint16_t Dcpu::getCycles() {
		return cycles;
	}

	uint16_t Dcpu::getNextWord() {
		return memory[registers.pc++];
	}

	bool Dcpu::isSkipNext() {
		return skipNext;
	}

	void Dcpu::skipNextInstruction() {
		skipNext = true;
	}

	void Dcpu::catchFire() {
		onFire = true;
	}

	bool Dcpu::isOnFire() {
		return onFire;
	}

	void Dcpu::tick() {
		auto instruction = Opcode::parse(*this, getNextWord());

		if (skipNext) {
			if (!instruction->isConditional()) {
				skipNext = false;
			}

			cycles += 1;
		} else {
			cycles += instruction->execute();
		}
	}

	void Dcpu::clear() {
		cycles = 0;
		onFire = false;
		skipNext = false;
		registers.clear();
		memset(memory, 0, TOTAL_MEMORY * sizeof(uint16_t));
	}

	void Dcpu::load(const char *filename) {
	    clear();

	    ifstream file(filename);
	    if (!file) {
	        throw runtime_error(str(format("Failed to open the file %s: %s") % filename % strerror(errno)));
	    }

	    size_t index = 0;
	    while (file && index <= TOTAL_MEMORY) {
	        uint8_t b1 = file.get();
	        if (file.eof()) {
	        	break;
	        }

	        uint8_t b2 = file.get();
	        if (!file.good()) {
	            throw runtime_error(str(format("Failed to read the next word from the file %s: %s") % filename 
	                	% strerror(errno)));
	        }

	        memory[index++] = (b1 << 8) | b2;
	    }
	    file.close();
	}

	void Dcpu::dump(ostream& out) const {
		out << "Cycles: " << dec << cycles << endl
			<< "======= Registers =======" << endl
			<< hex << setfill('0') << "A: " << setw(4) << registers.a
			<< "  B: " << setw(4) << registers.b
			<< "  C: " << setw(4) << registers.c
			<< "  X: " << setw(4) << registers.x
			<< "  Y: " << setw(4) << registers.y
			<< "  Z: " << setw(4) << registers.z << endl
			<< "I: " << setw(4) << registers.i
			<< "  J: " << setw(4) << registers.j
			<< " SP: " << setw(4) << registers.sp
			<< " PC: " << setw(4) << registers.pc
			<< " EX: " << setw(4) << registers.ex << endl
			<< "======= Memory =======" << endl;
		for (int i = 0; i < TOTAL_MEMORY; i += 8) {
			if (!memory[i+0] && !memory[i+1] && !memory[i+2]
				&& !memory[i+3] && !memory[i+4] && !memory[i+5]
				&& !memory[i+6] && !memory[i+7]) {
				continue;
			}

			out << setw(4) << i << ": "
				<< setw(4) << memory[i] << " "
				<< setw(4) << memory[i+1] << " "
				<< setw(4) << memory[i+2] << " "
				<< setw(4) << memory[i+3] << " "
				<< setw(4) << memory[i+4] << " "
				<< setw(4) << memory[i+5] << " "
				<< setw(4) << memory[i+6] << " "
				<< setw(4) << memory[i+7] << " " << endl;
		}
	}

	void Dcpu::addCycles(uint16_t cyclesAmount, bool simulateCpuSpeed) {
		cycles += cyclesAmount;

		if (simulateCpuSpeed) {
			
		}
	}

	/*************************************************************************
     *
     * DcpuRegisters
     *
     *************************************************************************/

	DcpuRegisters::DcpuRegisters(Dcpu &cpu) : cpu(cpu), a(0), b(0), c(0), x(0), y(0), z(0), i(0), j(0), sp(0),
			pc(0), ex(0), ia(0) {
	}

	void DcpuRegisters::clear() {
		a = b = c = x = y = z = i = j = sp = pc = ex = ia = 0;
	}

	uint16_t &DcpuRegisters::indirect(registers reg, uint16_t offset) {
		return cpu.memory[get(reg) + offset];
	}

	uint16_t &DcpuRegisters::operator[] (registers reg) {
		return get(reg);
	}

	uint16_t &DcpuRegisters::get(registers reg) {
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
		case registers::PC:
			return pc;
		case registers::SP:
			return sp;
		case registers::EX:
			return ex;
		case registers::IA:
			return ia;
		}
	}

	/*************************************************************************
     *
     * DcpuStack
     *
     *************************************************************************/

	DcpuStack::DcpuStack(Dcpu &cpu) : cpu(cpu) {

	}

	void DcpuStack::push(uint16_t value) {
		cpu.memory[--cpu.registers.sp] = value;
	}

	uint16_t &DcpuStack::push() {
		return cpu.memory[--cpu.registers.sp];
	}

	uint16_t &DcpuStack::pop() {
		return cpu.memory[cpu.registers.sp++];
	}

	uint16_t &DcpuStack::peek() {
		return cpu.memory[cpu.registers.sp];
	}

	uint16_t &DcpuStack::pick(uint16_t offset) {
		return cpu.memory[cpu.registers.sp + offset];
	}

	/*************************************************************************
     *
     * DcpuInterrupts
     *
     *************************************************************************/

	DcpuInterrupts::DcpuInterrupts(Dcpu &cpu) : cpu(cpu), queueEnabled(false), queue() {

	}

	void DcpuInterrupts::trigger(uint16_t message) {
		// TODO: use mutex
		if (cpu.registers.ia == 0) {
			return;
		}

		enableQueue();
		cpu.stack.push(cpu.registers.pc);
		cpu.stack.push(cpu.registers.a);

		cpu.registers.pc = cpu.registers.ia;
		cpu.registers.a = message;
	}

	void DcpuInterrupts::disableQueue() {
		queueEnabled = false;
	}

	void DcpuInterrupts::enableQueue() {
		queueEnabled = true;
	}

	bool DcpuInterrupts::isQueueEnabled() {
		return queueEnabled;
	}

	void DcpuInterrupts::send(uint16_t message) {
		if (cpu.registers.ia == 0) {
			return;
		}

		if (queueEnabled) {
			if (queue.size() >= QUEUE_MAX_SIZE) {
				cpu.catchFire();
				return;
			}

			queue.push(message);
		} else {
			trigger(message);
		}
	}

	/*************************************************************************
     *
     * MaxHardwareDevicesException
     *
     *************************************************************************/

	MaxHardwareDevicesException::MaxHardwareDevicesException() 
		: runtime_error("The maximum amount of hardware devices (65,535) has already been registered") {

	}

	/*************************************************************************
     *
     * DcpuHardwareManager
     *
     *************************************************************************/

	DcpuHardwareManager::DcpuHardwareManager(Dcpu &cpu) : cpu(cpu), hardware() {

	}

	uint16_t DcpuHardwareManager::getCount() {
		return hardware.size();
	}

	void DcpuHardwareManager::query(uint16_t index) {
		if (index >= hardware.size()) {
			return;
		}

		shared_ptr<HardwareDevice> device = hardware[index];

		cpu.registers.a = device->getHardwareId() & 0xffff;
		cpu.registers.b = (device->getHardwareId() >> 16) & 0xffff;
		cpu.registers.c = device->getVersion();
		cpu.registers.x = device->getManufacturerId() & 0xffff;
		cpu.registers.y = (device->getManufacturerId() >> 16) & 0xffff;
	}
	
	uint16_t DcpuHardwareManager::interrupt(uint16_t index) {
		if (index >= hardware.size()) {
			return 0;
		}

		shared_ptr<HardwareDevice> device = hardware[index];
		return device->interrupt();
	}

	void DcpuHardwareManager::registerDevice(shared_ptr<HardwareDevice> device) {
		if (hardware.size() >= MAX_DEVICES) {
			throw MaxHardwareDevicesException();
		}

		hardware.push_back(device);
	}

    
	void DcpuHardwareManager::tickAll() {
		for(auto &device : hardware) {
			device->tick();
		}
	}

	ostream &operator<<(std::ostream &stream, registers reg) {
		switch (reg) {
		case registers::A:
			return stream << "A";
		case registers::B:
			return stream << "B";
		case registers::C:
			return stream << "C";
		case registers::X:
			return stream << "X";
		case registers::Y:
			return stream << "Y";
		case registers::Z:
			return stream << "Z";
		case registers::I:
			return stream << "I";
		case registers::J:
			return stream << "J";
		case registers::PC:
			return stream << "PC";
		case registers::SP:
			return stream << "SP";
		case registers::EX:
			return stream << "EX";
		case registers::IA:
			return stream << "IA";
		default:
			return stream << "<Unknown Register>";
		}
	}
}}
