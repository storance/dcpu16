#include "mnemonics.hpp"

#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::algorithm;

namespace dcpu {
	/*************************************************************************
	 *
	 * opcode_definition
	 *
	 *************************************************************************/

	map<string, opcode_definition> opcode_definition::definitions = {
		{"set", opcode_definition(opcodes::SET, 2)},
		{"add", opcode_definition(opcodes::ADD, 2)},
		{"sub", opcode_definition(opcodes::SUB, 2)},
		{"mul", opcode_definition(opcodes::MUL, 2)},
		{"mli", opcode_definition(opcodes::MLI, 2)},
		{"div", opcode_definition(opcodes::DIV, 2)},
		{"dvi", opcode_definition(opcodes::DVI, 2)},
		{"mod", opcode_definition(opcodes::MOD, 2)},
		{"mdi", opcode_definition(opcodes::MDI, 2)},
		{"and", opcode_definition(opcodes::AND, 2)},
		{"bor", opcode_definition(opcodes::BOR, 2)},
		{"xor", opcode_definition(opcodes::XOR, 2)},
		{"shr", opcode_definition(opcodes::SHR, 2)},
		{"asr", opcode_definition(opcodes::ASR, 2)},
		{"shl", opcode_definition(opcodes::SHL, 2)},
		{"ifb", opcode_definition(opcodes::IFB, 2)},
		{"ifc", opcode_definition(opcodes::IFC, 2)},
		{"ife", opcode_definition(opcodes::IFE, 2)},
		{"ifn", opcode_definition(opcodes::IFN, 2)},
		{"ifg", opcode_definition(opcodes::IFG, 2)},
		{"ifa", opcode_definition(opcodes::IFA, 2)},
		{"ifl", opcode_definition(opcodes::IFL, 2)},
		{"ifu", opcode_definition(opcodes::IFU, 2)},
		{"adx", opcode_definition(opcodes::ADX, 2)},
		{"sbx", opcode_definition(opcodes::SBX, 2)},
		{"sti", opcode_definition(opcodes::STI, 2)},
		{"std", opcode_definition(opcodes::STD, 2)},
		{"jsr", opcode_definition(opcodes::JSR, 1)},
		{"hcf", opcode_definition(opcodes::HCF, 1)},
		{"int", opcode_definition(opcodes::INT, 1)},
		{"iag", opcode_definition(opcodes::IAG, 1)},
		{"ias", opcode_definition(opcodes::IAS, 1)},
		{"rfi", opcode_definition(opcodes::RFI, 1)},
		{"iaq", opcode_definition(opcodes::IAQ, 1)},
		{"hwn", opcode_definition(opcodes::HWN, 1)},
		{"hwq", opcode_definition(opcodes::HWQ, 1)},
		{"hwi", opcode_definition(opcodes::HWI, 1)},
		{"jmp", opcode_definition(opcodes::JMP, 1)}
	};

	opcode_definition::opcode_definition(opcodes opcode, uint8_t args)
			: opcode(opcode), args(args) {}

	const opcode_definition* opcode_definition::lookup(const string &mnemonic) {
		auto it = definitions.find(to_lower_copy(mnemonic));
		if (it == definitions.end()) {
			return nullptr;
		}

		return &it->second;
	}

	/*************************************************************************
	 *
	 * register_definition
	 *
	 *************************************************************************/

	map<string, register_definition> register_definition::definitions = {
		{"a",  register_definition(registers::A, true)},
		{"b",  register_definition(registers::B, true)},
		{"c",  register_definition(registers::C, true)},
		{"x",  register_definition(registers::X, true)},
		{"y",  register_definition(registers::Y, true)},
		{"z",  register_definition(registers::Z, true)},
		{"i",  register_definition(registers::I, true)},
		{"j",  register_definition(registers::J, true)},
		{"sp", register_definition(registers::SP, true)},
		{"pc", register_definition(registers::PC, false)},
		{"ex", register_definition(registers::EX, false)}
	};

	register_definition::register_definition(registers reg, bool indirectable)
		: _register(reg), indirectable(indirectable) {}

	const register_definition* register_definition::lookup(const string &mnemonic) {
		auto it = definitions.find(to_lower_copy(mnemonic));
		if (it == definitions.end()) {
			return nullptr;
		}

		return &it->second;
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
}
