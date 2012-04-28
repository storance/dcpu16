#include "OpcodeDefinition.hpp"

#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;

namespace dcpu { namespace ast {
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

	OpcodeDefinition::OpcodeDefinition(Opcode opcode, std::uint8_t args)
		: _opcode(opcode), _args(args) {}

	OpcodeDefinition* lookupOpcode(const string &opcodeName) {
		auto it = opcodes.find(boost::algorithm::to_lower_copy(opcodeName));
		if (it == opcodes.end()) {
			return nullptr;
		}

		return &it->second;
	}
}}