#pragma once

#include <string>
#include <cstdint>

#include "Common.hpp"

namespace dcpu { namespace ast {
	struct RegisterDefinition {
		Register _register;
		bool _indirectable;

		RegisterDefinition(Register reg, bool indirectable);
	};

	RegisterDefinition* lookupRegister(const std::string&);
}}