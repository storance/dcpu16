#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <getopt.h>

#include <boost/format.hpp>

#include "dcpu.hpp"

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

		uint8_t opcodeType = opcode.getType();

		if (opcode.isNonBasic()) {
			if (opcodeType == opcodes::RESERVED || opcodeType > opcodes::JSR) {
				throw logic_error(str(boost::format("%04x: Invalid Non-basic "
					"Opcode %#x") % opcode.getLocation() % (word_t)opcodeType));
			}

			out << non_basic_opcode_names[opcodeType] << " ";
			printArgument(cpu, opcode.getA());
			out << endl;
		} else {
			out << basic_opcode_names[opcodeType] << " ";
			printArgument(cpu, opcode.getA());
			out << ", ";
			printArgument(cpu, opcode.getB());
			out << endl;
		}
	}
private:
	void printArgument(DCPU &cpu, OpcodeArgument *arg) {
		uint8_t type = arg->getType();

		switch (type) {
		REGISTER_CASES(arguments::A)
			out << register_names[type];
			break;
		REGISTER_CASES(arguments::PTR_A)
			out << "[" << register_names[type - arguments::PTR_A] << "]";
			break;
		REGISTER_CASES(arguments::PTR_OFFSET_A)
			out << "[" << cpu.getNextWord() << " + " 
				<< register_names[type - arguments::PTR_OFFSET_A] << "]";
			break;
		case arguments::POP: out << "POP"; break;
		case arguments::PEEK: out << "PEEK"; break;
		case arguments::PUSH: out << "PUSH"; break;
		case arguments::SP: out << "SP"; break;
		case arguments::PC: out << "PC"; break;
		case arguments::O: out << "O"; break;
		case arguments::PTR_NEXT_WORD:
			out << "[" << cpu.getNextWord() << "]";
			break;
		case arguments::NEXT_WORD: out << cpu.getNextWord(); break;
		default: out << (type - arguments::LITERAL_START); break;
		}
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

