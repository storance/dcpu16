#pragma once

#include <string>
#include <cstdint>

namespace dcpu { namespace ast {
	enum class Register : std::uint8_t {
		A, B, C, X, Y, Z, I, J, SP, PC, EX
	};

	struct RegisterDefinition {
		Register _register;
		bool _indirectable;

		RegisterDefinition(Register reg, bool indirectable);
	};

	RegisterDefinition* lookupRegister(const std::string&);
	std::string str(Register);
}}