#include <inttypes.h>
#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
#include <csignal>
#include <iomanip>

#include <boost/format.hpp>

#include "dcpu.hpp"

using namespace std;

static uint8_t basic_cycles_table[16] = {
	0, 1, 2, 2,
	2, 3, 3, 2,
	2, 1, 1, 1,
	2, 2, 2, 2
};

static uint8_t non_basic_cycles_table[2] = {
	0, 2
};

class EmulatorOpcodeHandler : public OpcodeHandler {
public:
	virtual void execute(DCPU &cpu, Opcode& opcode) {
		if (opcode.isNonBasic()) {
			executeNonBasic(cpu, opcode);
		} else {
			OpcodeArgument *a = opcode.getA();
			OpcodeArgument *b = opcode.getB();

			a->resolve(cpu);
			b->resolve(cpu);

			uint8_t opcodeType = opcode.getType();
			switch (opcodeType) {
			case opcodes::SET:
				*a = b->getValue();
				break;
			case opcodes::ADD:
				{
					uint32_t result = a->getValue() + b->getValue();
					
					cpu.o = result >> 16;
					*a = (word_t)result;
				}
				break;
			case opcodes::SUB:
				{
					uint32_t result = a->getValue() - b->getValue();
					
					cpu.o = result >> 16;
					*a = (word_t)result;
				}
				break;
			case opcodes::MUL:
				{
					uint32_t result = a->getValue() * b->getValue();
					
					cpu.o = (result >> 16) & 0xffff;
					*a = (word_t)result;
				}
				break;
			case opcodes::DIV:
				{
					if (b->getValue() == 0) {
						cpu.o = 0;
						*a = 0;
					} else {
						uint32_t result = (a->getValue() << 16) / b->getValue();

						cpu.o = result & 0xffff;
						*a = (word_t)(result >> 16);
					} 
				}
				break;
			case opcodes::MOD:
				if (b->getValue() == 0) {
					*a = 0;
				} else {
					*a = a->getValue() % b->getValue();
				}
				break;
			case opcodes::SHL:
				{
					uint32_t result = a->getValue() << b->getValue();
					cpu.o = (result >> 16) & 0xffff;
					*a = (word_t)result;
				}
				break;
			case opcodes::SHR:
				{
					uint32_t result = (a->getValue() << 16) >> b->getValue(); 
					cpu.o = result & 0xffff;
					result = (word_t)result >> 16;
				}
				break;
			case opcodes::AND:
				*a = a->getValue() & b->getValue();
				break;
			case opcodes::BOR:
				*a = a->getValue() | b->getValue();
				break;
			case opcodes::XOR:
				*a = a->getValue() ^ b->getValue();
				break;
			case opcodes::IFE:
				if (a->getValue() != b->getValue()) {
					cpu.skipNext();
				}
				break;
			case opcodes::IFN:
				if (a->getValue() == b->getValue()) {
					cpu.skipNext();
				}
				break;
			case opcodes::IFG:
				if (a->getValue() <= b->getValue()) {
					cpu.skipNext();
				}
				break;
			case opcodes::IFB:
				if (a->getValue() & b->getValue() == 0) {
					cpu.skipNext();
				}
				break;
			}

			cpu.addCycles(basic_cycles_table[opcodeType] - 1);	
		}
	}
private:
	void executeNonBasic(DCPU &cpu, Opcode &opcode) {
		uint8_t nbOpcode = opcode.getType();
		OpcodeArgument *a = opcode.getA();

		a->resolve(cpu);

		switch (nbOpcode) {
		case opcodes::JSR:
			cpu.pushStack(cpu.pc);
			cpu.pc = a->getValue();
			break;
		default:
			throw logic_error(str(boost::format("%04x: Invalid Non-basic "
				"Opcode %#x") % opcode.getLocation() % (word_t)nbOpcode));
			break;
		}

		cpu.addCycles(non_basic_cycles_table[nbOpcode] - 1);
	}
};

static DCPU cpu;

void signal_handler(int signal) {
	cpu.halt();
}

int main(int argc, char **argv) {
	
	EmulatorOpcodeHandler handler;

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " </path/to/dcpu/program>" << endl;
		return 1;
	}

	signal(SIGINT, &signal_handler);
	try {
		cpu.load(argv[1]);
		cpu.execute(&handler);
		cpu.dump(cout);
	} catch (exception &e) {
		cerr << "Error: " << e.what() << endl;

		return 1;
	}
}