#pragma once

#include <string>
#include <cstdint>
#include <map>
#include <boost/optional/optional_fwd.hpp>

namespace dcpu {
	enum class registers : std::uint8_t {
		A, B, C, X, Y, Z, I, J, SP, PC, EX
	};

	enum class opcodes : std::uint16_t {
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

	enum class directives : std::uint8_t {
		INCLUDE,
		INCBIN,
		ALIGN,
		DW,
		DB,
		EQU,
		ORG,
		FILL
	};

	enum class stack_operation : std::uint8_t {
		PUSH, POP, PEEK, PICK
	};

	class instruction_definition {
		static std::map<std::string, instruction_definition> definitions;

		instruction_definition(opcodes opcode, std::uint8_t args);
	public:
		opcodes opcode;
		std::uint8_t args;

		static boost::optional<instruction_definition> lookup(const std::string &mnemonic);
	};

	class register_definition {
		static std::map<std::string, register_definition> definitions;

		register_definition(registers reg, bool indirectable);
	public:
		registers _register;
		bool indirectable;

		static boost::optional<register_definition> lookup(const std::string &mnemonic);
	};

	boost::optional<directives> lookup_directive(const std::string &mnemonic);
	boost::optional<stack_operation> lookup_stack_operation(const std::string &mnemonic);

	std::ostream& operator<< (std::ostream& stream, opcodes);
	std::ostream& operator<< (std::ostream& stream, registers);
	std::ostream& operator<< (std::ostream& stream, directives);
	std::ostream& operator<< (std::ostream& stream, stack_operation operation);
}
