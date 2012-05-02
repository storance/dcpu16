#include "Mnemonics.hpp"

#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::algorithm;

namespace dcpu {
	static map<string, OpcodeDefinition> opcodes = {
		{"set", OpcodeDefinition(Opcode::SET, 2)},
		{"add", OpcodeDefinition(Opcode::ADD, 2)},
		{"sub", OpcodeDefinition(Opcode::SUB, 2)},
		{"mul", OpcodeDefinition(Opcode::MUL, 2)},
		{"mli", OpcodeDefinition(Opcode::MLI, 2)},
		{"div", OpcodeDefinition(Opcode::DIV, 2)},
		{"dvi", OpcodeDefinition(Opcode::DVI, 2)},
		{"mod", OpcodeDefinition(Opcode::MOD, 2)},
		{"mdi", OpcodeDefinition(Opcode::MDI, 2)},
		{"and", OpcodeDefinition(Opcode::AND, 2)},
		{"bor", OpcodeDefinition(Opcode::BOR, 2)},
		{"xor", OpcodeDefinition(Opcode::XOR, 2)},
		{"shr", OpcodeDefinition(Opcode::SHR, 2)},
		{"asr", OpcodeDefinition(Opcode::ASR, 2)},
		{"shl", OpcodeDefinition(Opcode::SHL, 2)},
		{"ifb", OpcodeDefinition(Opcode::IFB, 2)},
		{"ifc", OpcodeDefinition(Opcode::IFC, 2)},
		{"ife", OpcodeDefinition(Opcode::IFE, 2)},
		{"ifn", OpcodeDefinition(Opcode::IFN, 2)},
		{"ifg", OpcodeDefinition(Opcode::IFG, 2)},
		{"ifa", OpcodeDefinition(Opcode::IFA, 2)},
		{"ifl", OpcodeDefinition(Opcode::IFL, 2)},
		{"ifu", OpcodeDefinition(Opcode::IFU, 2)},
		{"adx", OpcodeDefinition(Opcode::ADX, 2)},
		{"sbx", OpcodeDefinition(Opcode::SBX, 2)},
		{"sti", OpcodeDefinition(Opcode::STI, 2)},
		{"std", OpcodeDefinition(Opcode::STD, 2)},
		{"jsr", OpcodeDefinition(Opcode::JSR, 1)},
		{"hcf", OpcodeDefinition(Opcode::HCF, 1)},
		{"int", OpcodeDefinition(Opcode::INT, 1)},
		{"iag", OpcodeDefinition(Opcode::IAG, 1)},
		{"ias", OpcodeDefinition(Opcode::IAS, 1)},
		{"rfi", OpcodeDefinition(Opcode::RFI, 1)},
		{"iaq", OpcodeDefinition(Opcode::IAQ, 1)},
		{"hwn", OpcodeDefinition(Opcode::HWN, 1)},
		{"hwq", OpcodeDefinition(Opcode::HWQ, 1)},
		{"hwi", OpcodeDefinition(Opcode::HWI, 1)},
		{"jmp", OpcodeDefinition(Opcode::JMP, 1)}
	};

	static map<string, RegisterDefinition> registers = {
		{"a",  RegisterDefinition(Register::A, true)},
		{"b",  RegisterDefinition(Register::B, true)},
		{"c",  RegisterDefinition(Register::C, true)},
		{"x",  RegisterDefinition(Register::X, true)},
		{"y",  RegisterDefinition(Register::Y, true)},
		{"z",  RegisterDefinition(Register::Z, true)},
		{"i",  RegisterDefinition(Register::I, true)},
		{"j",  RegisterDefinition(Register::J, true)},
		{"sp", RegisterDefinition(Register::SP, true)},
		{"pc", RegisterDefinition(Register::PC, false)},
		{"ex", RegisterDefinition(Register::EX, false)}
	};

	OpcodeDefinition::OpcodeDefinition(Opcode opcode, uint8_t args)
		: opcode(opcode), args(args) {}

	OpcodeDefinition* lookupOpcode(const string &mnemonic) {
		auto it = opcodes.find(to_lower_copy(mnemonic));
		if (it == opcodes.end()) {
			return nullptr;
		}

		return &it->second;
	}

	RegisterDefinition::RegisterDefinition(Register reg, bool indirectable)
		: _register(reg), indirectable(indirectable) {}

	RegisterDefinition* lookupRegister(const string &mnemonic) {
		auto it = registers.find(to_lower_copy(mnemonic));
		if (it == registers.end()) {
			return nullptr;
		}

		return &it->second;
	}

	ostream& operator<< (ostream& stream, Opcode opcode) {
		switch(opcode) {
		case Opcode::SET:
			return stream << "SET";
		case Opcode::ADD:
			return stream << "ADD";
		case Opcode::SUB:
			return stream << "SUB";
		case Opcode::MUL:
			return stream << "MUL";
		case Opcode::MLI:
			return stream << "MLI";
		case Opcode::DIV:
			return stream << "DIV";
		case Opcode::DVI:
			return stream << "DVI";
		case Opcode::MOD:
			return stream << "MOD";
		case Opcode::MDI:
			return stream << "MDI";
		case Opcode::AND:
			return stream << "AND";
		case Opcode::BOR:
			return stream << "BOR";
		case Opcode::XOR:
			return stream << "XOR";
		case Opcode::SHR:
			return stream << "SHR";
		case Opcode::ASR:
			return stream << "ASR";
		case Opcode::SHL:
			return stream << "SHL";
		case Opcode::STI:
			return stream << "STI";
		case Opcode::STD:
			return stream << "STD";
		case Opcode::IFB:
			return stream << "IFB";
		case Opcode::IFC:
			return stream << "IFC";
		case Opcode::IFE:
			return stream << "IFE";
		case Opcode::IFN:
			return stream << "IFN";
		case Opcode::IFG:
			return stream << "IFG";
		case Opcode::IFA:
			return stream << "IFA";
		case Opcode::IFL:
			return stream << "IFL";
		case Opcode::IFU:
			return stream << "IFU";
		case Opcode::ADX:
			return stream << "ADX";
		case Opcode::SBX:
			return stream << "SBX";
		case Opcode::JSR:
			return stream << "JSR";
		case Opcode::HCF:
			return stream << "HCF";
		case Opcode::INT:
			return stream << "INT";
		case Opcode::IAG:
			return stream << "IAG";
		case Opcode::IAS:
			return stream << "IAS";
		case Opcode::RFI:
			return stream << "RFI";
		case Opcode::IAQ:
			return stream << "IAQ";
		case Opcode::HWN:
			return stream << "HWN";
		case Opcode::HWQ:
			return stream << "HWQ";
		case Opcode::HWI:
			return stream << "HWI";
		case Opcode::JMP:
			return stream << "JMP";
		default:
			return stream << "<Unknown Opcode " << static_cast<int>(opcode) << ">";
		}
	}

	ostream& operator<< (ostream& stream, Register _register) {
		switch (_register) {
		case Register::A:
			return stream << "A";
		case Register::B:
			return stream << "B";
		case Register::C:
			return stream << "C";
		case Register::X:
			return stream << "X";
		case Register::Y:
			return stream << "Y";
		case Register::Z:
			return stream << "Z";
		case Register::I:
			return stream << "I";
		case Register::J:
			return stream << "J";
		case Register::PC:
			return stream << "PC";
		case Register::SP:
			return stream << "SP";
		case Register::EX:
			return stream << "EX";
		default:
			return stream << "<Unknown Register " << static_cast<int>(_register) << ">";
		}
	}
}