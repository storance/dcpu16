#include "RegisterDefinition.hpp"

#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;

namespace dcpu { namespace ast {
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

	RegisterDefinition::RegisterDefinition(Register reg, bool indirectable)
		: _register(reg), _indirectable(indirectable) {}

	RegisterDefinition* lookupRegister(const string &registerName) {
		auto it = registers.find(boost::algorithm::to_lower_copy(registerName));
		if (it == registers.end()) {
			return nullptr;
		}

		return &it->second;
	}
}}