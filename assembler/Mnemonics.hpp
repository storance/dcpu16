#pragma once

#include <string>
#include <cstdint>

namespace dcpu {
	enum class Register : std::uint8_t {
		A, B, C, X, Y, Z, I, J, SP, PC, EX
	};

	enum class Opcode : std::uint16_t {
		// Basic
		SET=0x1,
		ADD,
		SUB,
		MUL,
		MLI,
		DIV,
		DVI,
		MOD,
		MDI,
		AND,
		BOR,
		XOR,
		SHR,
		ASR,
		SHL,
		IFB,
		IFC,
		IFE,
		IFN,
		IFG,
		IFA,
		IFL,
		IFU,
		ADX=0x1a,
		SBX,
		STI=0x1e,
		STD,
		// Non-basic 
		JSR=0x1 << 5,
		HCF=0x07 << 5,
		INT=0x08 << 5,
		IAG=0x09 << 5,
		IAS=0x0a << 5,
		RFI=0x0b << 5,
		IAQ=0x0c << 5,
		HWN=0x10 << 5,
		HWQ=0x11 << 5,
		HWI=0x12 << 5,
		// Extensions
		JMP=0x400,
	};

	class OpcodeDefinition {
	public:
		Opcode opcode;
		std::uint8_t args;

		OpcodeDefinition(Opcode opcode, std::uint8_t args);
	};

	struct RegisterDefinition {
		Register _register;
		bool indirectable;

		RegisterDefinition(Register reg, bool indirectable);
	};

	RegisterDefinition* lookupRegister(const std::string &mnemonic);
	OpcodeDefinition* lookupOpcode(const std::string &mnemonic);

	std::ostream& operator<< (std::ostream& stream, Opcode);
	std::ostream& operator<< (std::ostream& stream, Register);
}