#include <cstring>
#include <stdexcept>
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
     * dcpu
     *
     *************************************************************************/

	dcpu::dcpu() : skip_next(false), on_fire(false), cycles(0), stack(*this), registers(*this),
			interrupt_handler(*this), hardware_manager(*this) {
		memset(memory, 0, TOTAL_MEMORY * sizeof(uint16_t));
	}

	uint16_t dcpu::get_next_word() {
		return memory[registers.pc++];
	}

	bool dcpu::is_skip_next() {
		return skip_next;
	}

	void dcpu::skip_next_instruction() {
		skip_next = true;
	}

	void dcpu::catch_fire() {
		on_fire = true;
	}

	bool dcpu::is_on_fire() {
		return on_fire;
	}

	void dcpu::run() {
		while (!on_fire) {
			uint16_t cycles = 0;
			unique_ptr<opcode> instruction = opcode::parse(*this, get_next_word());

			if (skip_next) {
				if (!instruction->is_conditional()) {
					skip_next = false;
				}

				cycles = 1;
			} else {
				cycles = instruction->execute();
			}

			add_cycles(cycles, true);
		}
	}

	void dcpu::dump(ostream& out) const {
		out << "Cycles: " << cycles << endl
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

	void dcpu::add_cycles(uint16_t cycles_amount, bool simulate_cpu_speed) {
		cycles += cycles_amount;

		if (simulate_cpu_speed) {
			
		}
	}

	/*************************************************************************
     *
     * dcpu_registers
     *
     *************************************************************************/

	dcpu_registers::dcpu_registers(dcpu &cpu) : cpu(cpu), a(0), b(0), c(0), x(0), y(0), z(0), i(0), j(0), sp(0),
			pc(0), ex(0), ia(0) {

	}

	uint16_t &dcpu_registers::indirect(registers reg, uint16_t offset) {
		return cpu.memory[get(reg) + offset];
	}

	uint16_t &dcpu_registers::operator[] (registers reg) {
		return get(reg);
	}

	uint16_t &dcpu_registers::get(registers reg) {
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
     * dcpu_stack
     *
     *************************************************************************/

	dcpu_stack::dcpu_stack(dcpu &cpu) : cpu(cpu) {

	}

	void dcpu_stack::push(uint16_t value) {
		cpu.memory[--cpu.registers.sp] = value;
	}

	uint16_t &dcpu_stack::push() {
		return cpu.memory[--cpu.registers.sp];
	}

	uint16_t &dcpu_stack::pop() {
		return cpu.memory[cpu.registers.sp++];
	}

	uint16_t &dcpu_stack::peek() {
		return cpu.memory[cpu.registers.sp];
	}

	uint16_t &dcpu_stack::pick(uint16_t offset) {
		return cpu.memory[cpu.registers.sp + offset];
	}

	/*************************************************************************
     *
     * dcpu_interrupt_handler
     *
     *************************************************************************/

	dcpu_interrupt_handler::dcpu_interrupt_handler(dcpu &cpu) : cpu(cpu), queue_enabled(false), queue() {

	}

	void dcpu_interrupt_handler::trigger(uint16_t message) {
		// TODO: use mutex
		if (cpu.registers.ia == 0) {
			return;
		}

		enable_queue();
		cpu.stack.push(cpu.registers.pc);
		cpu.stack.push(cpu.registers.a);

		cpu.registers.pc = cpu.registers.ia;
		cpu.registers.a = message;
	}

	void dcpu_interrupt_handler::disable_queue() {
		queue_enabled = false;
	}

	void dcpu_interrupt_handler::enable_queue() {
		queue_enabled = true;
	}

	bool dcpu_interrupt_handler::is_queue_enabled() {
		return queue_enabled;
	}

	void dcpu_interrupt_handler::send(uint16_t message) {
		if (cpu.registers.ia == 0) {
			return;
		}

		if (queue_enabled) {
			if (queue.size() >= QUEUE_MAX_SIZE) {
				cpu.catch_fire();
				return;
			}

			queue.push(message);
		} else {
			trigger(message);
		}
	}

	/*************************************************************************
     *
     * max_hardware_devices
     *
     *************************************************************************/

	max_hardware_devices::max_hardware_devices() 
		: runtime_error("The maximum number of hardware devices (65,535) have already been registered") {

	}

	/*************************************************************************
     *
     * dcpu_hardware_manager
     *
     *************************************************************************/

	dcpu_hardware_manager::dcpu_hardware_manager(dcpu &cpu) : cpu(cpu), hardware() {

	}

	uint16_t dcpu_hardware_manager::get_count() {
		return hardware.size();
	}

	void dcpu_hardware_manager::query(uint16_t index) {
		if (index >= hardware.size()) {
			return;
		}

		shared_ptr<hardware_device> device = hardware[index];

		cpu.registers.a = device->get_hardware_id() & 0xffff;
		cpu.registers.b = (device->get_hardware_id() >> 16) & 0xffff;
		cpu.registers.c = device->get_version();
		cpu.registers.x = device->get_manufacturer_id() & 0xffff;
		cpu.registers.y = (device->get_manufacturer_id() >> 16) & 0xffff;
	}
	
	uint16_t dcpu_hardware_manager::interrupt(uint16_t index) {
		if (index >= hardware.size()) {
			return 0;
		}

		shared_ptr<hardware_device> device = hardware[index];
		return device->interrupt();
	}

	void dcpu_hardware_manager::register_device(shared_ptr<hardware_device> device) {
		if (hardware.size() >= MAX_DEVICES) {
			throw max_hardware_devices();
		}

		hardware.push_back(device);
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
