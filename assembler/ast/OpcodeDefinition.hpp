#pragma once

#include <string>
#include <cstdint>

namespace dcpu { namespace ast {
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
		AND,
		BOR,
		XOR,
		SHR,
		ASR,
		SHL,
		STI,
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
		// Non-basic 
		JSR=0x1 << 5,
		HCF=0x07 << 5,
		INT=0x08 << 5,
		IAG=0x09 << 5,
		IAS=0x0a << 5,
		HWN=0x10 << 5,
		HWQ=0x11 << 5,
		HWI=0x12 << 5,
		// Extensions
		JMP=0x400,
	};

	class OpcodeDefinition {
	public:
		Opcode _opcode;
		std::uint8_t _args;

		OpcodeDefinition(Opcode opcode, std::uint8_t args);
	};

	OpcodeDefinition* lookupOpcode(const std::string&);
	std::string str(Opcode);
}}