#pragma once

#include <string>
#include <cstdint>

#include "Common.hpp"

namespace dcpu { namespace ast {
	class OpcodeDefinition {
	public:
		Opcode _opcode;
		std::uint8_t _args;

		OpcodeDefinition(Opcode opcode, std::uint8_t args);
	};

	OpcodeDefinition* lookupOpcode(const std::string&);
}}