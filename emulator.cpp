#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <csignal>

#include "dcpu.h"

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
		if (opcode.getOpcodeType() == 0) {
			executeNonBasic(cpu, opcode);
		} else {
			OpcodeArgument &a = opcode.getA();
			OpcodeArgument &b = opcode.getB();

			a.resolve(cpu);
			b.resolve(cpu);

			switch (opcode.getOpcodeType()) {
			case op_set:
				a = *b;
				break;
			case op_add:
				{
					uint32_t result = *a + *b;
					
					cpu.o = result >> 16;
					a = (word_t)result;
				}
				break;
			case op_sub:
				{
					uint32_t result = *a - *b;
					
					cpu.o = result >> 16;
					a = (word_t)result;
				}
				break;
			case op_mul:
				{
					uint32_t result = *a * *b;
					
					cpu.o = (result >> 16) & 0xffff;
					a = (word_t)result;
				}
				break;
			case op_div:
				{
					if (*b == 0) {
						cpu.o = 0;
						a = 0;
					} else {
						uint32_t result = (*a << 16) / *b;

						cpu.o = result & 0xffff;
						a = (word_t)(result >> 16);
					} 
				}
				break;
			case op_mod:
				if (*b == 0) {
					a = 0;
				} else {
					a = *a % *b;
				}
				break;
			case op_shl:
				{
					uint32_t result = *a << *b;
					cpu.o = (result >> 16) & 0xffff;
					a = (word_t)result;
				}
				break;
			case op_shr:
				{
					uint32_t result = (*a << 16) >> *b; 
					cpu.o = result & 0xffff;
					result = (word_t)result >> 16;
				}
				break;
			case op_and:
				a = *a & *b;
				break;
			case op_bor:
				a = *a | *b;
				break;
			case op_xor:
				a = *a ^ *b;
				break;
			case op_ife:
				if (*a != *b) {
					cpu.skipNext();
				}
				break;
			case op_ifn:
				if (*a == *b) {
					cpu.skipNext();
				}
				break;
			case op_ifg:
				if (*a <= *b) {
					cpu.skipNext();
				}
				break;
			case op_ifb:
				if (*a & *b == 0) {
					cpu.skipNext();
				}
				break;
			}

			cpu.addCycles(basic_cycles_table[opcode.getOpcodeType()] - 1);	
		}
	}
private:
	void executeNonBasic(DCPU &cpu, Opcode &opcode) {
		uint8_t nbOpcode = opcode.getOpcodeType();
		OpcodeArgument &a = opcode.getB();

		a.resolve(cpu);
		switch (nbOpcode) {
		case op_jsr:
			cpu.pushStack(cpu.pc);
			cpu.pc = *a;
			break;
		default:
			{
				word_t pc = cpu.pc - opcode.getSize();

				stringstream buf;
				buf << hex << setfill('0') << setw(4) << pc 
					<< ": Invalid Opcode " << setw(2) << nbOpcode;
				throw logic_error(buf.str().c_str());
			}
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
		printf("Usage: %s </path/to/dcpu/program>\n", argv[0]);
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