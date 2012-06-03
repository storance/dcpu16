#include "mnemonics.hpp"

#include <map>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::algorithm;

namespace dcpu { namespace assembler {
	/*************************************************************************
	 *
	 * instruction_definition
	 *
	 *************************************************************************/

	map<string, instruction_definition> instruction_definition::definitions = {
		{"set", instruction_definition(opcodes::SET, 2)},
		{"add", instruction_definition(opcodes::ADD, 2)},
		{"sub", instruction_definition(opcodes::SUB, 2)},
		{"mul", instruction_definition(opcodes::MUL, 2)},
		{"mli", instruction_definition(opcodes::MLI, 2)},
		{"div", instruction_definition(opcodes::DIV, 2)},
		{"dvi", instruction_definition(opcodes::DVI, 2)},
		{"mod", instruction_definition(opcodes::MOD, 2)},
		{"mdi", instruction_definition(opcodes::MDI, 2)},
		{"and", instruction_definition(opcodes::AND, 2)},
		{"bor", instruction_definition(opcodes::BOR, 2)},
		{"xor", instruction_definition(opcodes::XOR, 2)},
		{"shr", instruction_definition(opcodes::SHR, 2)},
		{"asr", instruction_definition(opcodes::ASR, 2)},
		{"shl", instruction_definition(opcodes::SHL, 2)},
		{"ifb", instruction_definition(opcodes::IFB, 2)},
		{"ifc", instruction_definition(opcodes::IFC, 2)},
		{"ife", instruction_definition(opcodes::IFE, 2)},
		{"ifn", instruction_definition(opcodes::IFN, 2)},
		{"ifg", instruction_definition(opcodes::IFG, 2)},
		{"ifa", instruction_definition(opcodes::IFA, 2)},
		{"ifl", instruction_definition(opcodes::IFL, 2)},
		{"ifu", instruction_definition(opcodes::IFU, 2)},
		{"adx", instruction_definition(opcodes::ADX, 2)},
		{"sbx", instruction_definition(opcodes::SBX, 2)},
		{"sti", instruction_definition(opcodes::STI, 2)},
		{"std", instruction_definition(opcodes::STD, 2)},
		{"jsr", instruction_definition(opcodes::JSR, 1)},
		{"hcf", instruction_definition(opcodes::HCF, 1)},
		{"int", instruction_definition(opcodes::INT, 1)},
		{"iag", instruction_definition(opcodes::IAG, 1)},
		{"ias", instruction_definition(opcodes::IAS, 1)},
		{"rfi", instruction_definition(opcodes::RFI, 1)},
		{"iaq", instruction_definition(opcodes::IAQ, 1)},
		{"hwn", instruction_definition(opcodes::HWN, 1)},
		{"hwq", instruction_definition(opcodes::HWQ, 1)},
		{"hwi", instruction_definition(opcodes::HWI, 1)},
		{"jmp", instruction_definition(opcodes::JMP, 1)}
	};

	instruction_definition::instruction_definition(opcodes opcode, uint8_t args)
			: opcode(opcode), args(args) {}

	boost::optional<instruction_definition> instruction_definition::lookup(const string &mnemonic) {
		auto it = definitions.find(to_lower_copy(mnemonic));
		if (it == definitions.end()) {
			return boost::none;
		}

		return it->second;
	}

	/*************************************************************************
	 *
	 * registers
	 *
	 *************************************************************************/

	boost::optional<registers> lookup_register(const std::string &mnenmonic) {
		if (iequals(mnenmonic, "a")) {
			return registers::A;
		} else if (iequals(mnenmonic, "b")) {
			return registers::B;
		}else if (iequals(mnenmonic, "c")) {
			return registers::C;
		} else if (iequals(mnenmonic, "x")) {
			return registers::X;
		} else if (iequals(mnenmonic, "y")) {
			return registers::Y;
		}else if (iequals(mnenmonic, "z")) {
			return registers::Z;
		} else if (iequals(mnenmonic, "i")) {
			return registers::I;
		} else if (iequals(mnenmonic, "j")) {
			return registers::J;
		} else if (iequals(mnenmonic, "sp")) {
			return registers::SP;
		} else if (iequals(mnenmonic, "pc")) {
			return registers::PC;
		} else if (iequals(mnenmonic, "ex")) {
			return registers::EX;
		}

		return boost::none;
	}

	/*************************************************************************
	 *
	 * directives
	 *
	 *************************************************************************/

	boost::optional<directives> lookup_directive(const std::string &mnemonic) {
		if (iequals(mnemonic, ".org")) {
			return directives::ORG;
		} else if (iequals(mnemonic, ".align")) {
			return directives::ALIGN;
		} else if (iequals(mnemonic, ".equ")) {
			return directives::EQU;
		} else if (iequals(mnemonic, ".include")) {
			return directives::INCLUDE;
		} else if (iequals(mnemonic, ".incbin")) {
			return directives::INCBIN;
		} else if (iequals(mnemonic, ".dw") || iequals(mnemonic, ".dat") || iequals(mnemonic, "dat")) {
			return directives::DW;
		} else if (iequals(mnemonic, ".db") || iequals(mnemonic, ".dp")) {
			return directives::DB;
		} else if (iequals(mnemonic, ".fill")) {
			return directives::FILL;
		}

		return boost::none;
	}

	/*************************************************************************
	 *
	 * stack_operation
	 *
	 *************************************************************************/

	boost::optional<stack_operation> lookup_stack_operation(const std::string &mnemonic) {
		if (iequals(mnemonic, "push")) {
			return stack_operation::PUSH;
		} else if (iequals(mnemonic, "pop")) {
			return stack_operation::POP;
		} else if (iequals(mnemonic, "peek")) {
			return stack_operation::PEEK;
		} else if (iequals(mnemonic, "pick")) {
			return stack_operation::PICK;
		}

		return boost::none;
	}

	/*************************************************************************
	 *
	 * Stream operators
	 *
	 *************************************************************************/

	ostream& operator<< (ostream& stream, opcodes opcode) {
		switch(opcode) {
		case opcodes::SET:
			return stream << "SET";
		case opcodes::ADD:
			return stream << "ADD";
		case opcodes::SUB:
			return stream << "SUB";
		case opcodes::MUL:
			return stream << "MUL";
		case opcodes::MLI:
			return stream << "MLI";
		case opcodes::DIV:
			return stream << "DIV";
		case opcodes::DVI:
			return stream << "DVI";
		case opcodes::MOD:
			return stream << "MOD";
		case opcodes::MDI:
			return stream << "MDI";
		case opcodes::AND:
			return stream << "AND";
		case opcodes::BOR:
			return stream << "BOR";
		case opcodes::XOR:
			return stream << "XOR";
		case opcodes::SHR:
			return stream << "SHR";
		case opcodes::ASR:
			return stream << "ASR";
		case opcodes::SHL:
			return stream << "SHL";
		case opcodes::STI:
			return stream << "STI";
		case opcodes::STD:
			return stream << "STD";
		case opcodes::IFB:
			return stream << "IFB";
		case opcodes::IFC:
			return stream << "IFC";
		case opcodes::IFE:
			return stream << "IFE";
		case opcodes::IFN:
			return stream << "IFN";
		case opcodes::IFG:
			return stream << "IFG";
		case opcodes::IFA:
			return stream << "IFA";
		case opcodes::IFL:
			return stream << "IFL";
		case opcodes::IFU:
			return stream << "IFU";
		case opcodes::ADX:
			return stream << "ADX";
		case opcodes::SBX:
			return stream << "SBX";
		case opcodes::JSR:
			return stream << "JSR";
		case opcodes::HCF:
			return stream << "HCF";
		case opcodes::INT:
			return stream << "INT";
		case opcodes::IAG:
			return stream << "IAG";
		case opcodes::IAS:
			return stream << "IAS";
		case opcodes::RFI:
			return stream << "RFI";
		case opcodes::IAQ:
			return stream << "IAQ";
		case opcodes::HWN:
			return stream << "HWN";
		case opcodes::HWQ:
			return stream << "HWQ";
		case opcodes::HWI:
			return stream << "HWI";
		case opcodes::JMP:
			return stream << "JMP";
		default:
			return stream << "<Unknown opcode " << static_cast<int>(opcode) << ">";
		}
	}

	ostream& operator<< (ostream& stream, registers _register) {
		switch (_register) {
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
		default:
			return stream << "<Unknown register " << static_cast<int>(_register) << ">";
		}
	}

	ostream& operator<< (ostream& stream, directives directive) {
		switch (directive) {
		case directives::INCLUDE:
			return stream << ".INCLUDE";
		case directives::INCBIN:
			return stream << ".INCBIN";
		case directives::FILL:
			return stream << ".FILL";
		case directives::ALIGN:
			return stream << ".ALIGN";
		case directives::DW:
			return stream << ".DW";
		case directives::DB:
			return stream << ".DB";
		case directives::EQU:
			return stream << ".EQU";
		case directives::ORG:
			return stream << ".ORG";
		default:
			return stream << "<Unknown directive " << static_cast<int>(directive) << ">";
		}
	}

	ostream& operator<< (ostream& stream, stack_operation operation) {
		switch (operation) {
		case stack_operation::PUSH:
			return stream << "PUSH";
		case stack_operation::POP:
			return stream << "POP";
		case stack_operation::PEEK:
			return stream << "PEEK";
		case stack_operation::PICK:
			return stream << "PICK";
		default:
			return stream << "<Unknown stack_operation " << static_cast<int>(operation) << ">";
		}
	}
}}
