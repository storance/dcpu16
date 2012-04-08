#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <getopt.h>

#include "dcpu.h"

using namespace std;

const int TOTAL_BASIC_OPCODES=16;
const int TOTAL_NON_BASIC_OPCODES=2;

static const char *register_names[TOTAL_REGISTERS] = {
	"A", "B", "C", "X",
	"Y", "Z", "I", "J"
};

static const char* basic_opcode_names[TOTAL_BASIC_OPCODES] = {
	NULL,
	"SET",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"MOD",
	"SHL",
	"SHR",
	"AND",
	"BOR",
	"XOR",
	"IFE",
	"IFN",
	"IFG",
	"IFB"
};

static const char* non_basic_opcode_names[TOTAL_NON_BASIC_OPCODES] = {
	NULL,
	"JSR"
};

class PrintOpcodeHandler : public OpcodeHandler {
	ostream &out;
	size_t size;
public:
	PrintOpcodeHandler(ostream &aOut, size_t aSize) :
		out (aOut), size(aSize) {}

	virtual void execute(DCPU &cpu, Opcode& opcode) {
		if (cpu.pc >= size) {
			cpu.halt();
			return;
		}

		OpcodeType opcodeType = opcode.getOpcodeType();

		if (opcodeType == 0) {
			executeNonBasic(cpu, opcode);
		} else {
			out << basic_opcode_names[opcodeType] << " ";
			printArgument(cpu, opcode.getA());
			out << ", ";
			printArgument(cpu, opcode.getB());
			out << endl;
		}
	}
private:
	void printArgument(DCPU &cpu, OpcodeArgument &arg) {
		uint8_t type = arg.getType();

		switch (type) {
		case arg_a: case arg_b: case arg_c: case arg_x:
		case arg_y: case arg_z: case arg_i: case arg_j:
			out << register_names[type];
			break;
		case arg_ptr_a: case arg_ptr_b: case arg_ptr_c: case arg_ptr_x:
		case arg_ptr_y: case arg_ptr_z: case arg_ptr_i: case arg_ptr_j:
			out << "[" << register_names[type - arg_ptr_a] << "]";
			break;
		case arg_ptr_offset_a: case arg_ptr_offset_b: case arg_ptr_offset_c:
		case arg_ptr_offset_x: case arg_ptr_offset_y: case arg_ptr_offset_z:
		case arg_ptr_offset_i: case arg_ptr_offset_j:
			out << "[" << cpu.getNextWord() << " + " 
				<< register_names[type - arg_ptr_offset_a] << "]";
			break;
		case arg_pop: out << "POP"; break;
		case arg_peek: out << "PEEK"; break;
		case arg_push: out << "PUSH"; break;
		case arg_sp: out << "SP"; break;
		case arg_pc: out << "PC"; break;
		case arg_o: out << "O"; break;
		case arg_ptr_next_word: out << "[" << cpu.getNextWord() << "]"; break;
		case arg_next_word: out << cpu.getNextWord(); break;
		default: out << (type - arg_literal); break;
		}
	}

	void executeNonBasic(DCPU &cpu, Opcode &opcode) {
		uint8_t nbOpcode = opcode.getOpcodeType();
		OpcodeArgument &a = opcode.getB();

		if (nbOpcode == 0 || nbOpcode > op_jsr) {
			word_t pc = cpu.pc - opcode.getSize();

			stringstream buf;
			buf << hex << setfill('0') << setw(4) << pc 
				<< ": Invalid Non-basic Opcode 0x"
				<< setw(2) << (word_t)nbOpcode;
			throw logic_error(buf.str().c_str());
		}

		out << non_basic_opcode_names[nbOpcode] << " ";
		printArgument(cpu, a);
		out << endl;
	}
};

int main(int argc, char **argv) {
	DCPU cpu;
	
	uint8_t base = 16;
	char *inputFile = NULL;
	char *outputFile = NULL;

	option longOptions[] = {
		{"decimal", no_argument, 0, 'd'},
		{"octal", no_argument, 0, 'c'},
		{"hex", no_argument, 0, 'h'},
		{"output", required_argument, 0, 'o'},
		{0, 0, 0, 0}
	};

	int c, optionIndex;
	while ((c = getopt_long(argc, argv, "cdho:", longOptions,
		&optionIndex)) != -1) {

		switch (c) {
		case 'c':
			base = 8;
			break;
		case 'd':
			base = 10;
			break;
		case 'h':
			base = 16;
			break;
		case 'o':
			outputFile = optarg;
			break;
		case '?':
			break;
		default:
			return 1;
		}
	}

	if (optind < argc) {
		inputFile = argv[optind++];
	} else {
		cerr << "Missing file to disassemble." << endl;
		return 1;
	}

	ostream *out;
	ofstream outFile;
	if (outputFile) {
		outFile.exceptions( ofstream::failbit | ofstream::badbit );
		outFile.open(outputFile);
		if (!outFile) {
			cerr << "Failed to open " << outputFile << " for write" << endl;
			return 1;
		}

		out = &outFile;
	} else {
		out = &cout;
	}

	(*out) << setbase(base) << showbase;

	try {
		size_t size = cpu.load(inputFile);

		PrintOpcodeHandler handler(*out, size);
		cpu.execute(&handler);
	} catch (exception &e) {
		cerr << "Error: " << e.what() << endl;
		return 1;
	}
}

