#include "AbstractSyntaxTree.hpp"

#include <boost/algorithm/string/predicate.hpp>

using namespace std;

namespace dcpu { namespace ast {

Label::Label(string name) : name(name) {
	if (boost::starts_with(name, "..@")) {
		type = LabelType::GLOBAL_NO_LOCAL;
	} else if (boost::starts_with(name, ".")) {
		type = LabelType::LOCAL;
	} else {
		type = LabelType::GLOBAL;
	}
}

LiteralArgument::LiteralArgument(uint32_t value, bool forceLongMode)
	: value(value), forceLongMode(forceLongMode) {}

Instruction::Instruction(Opcode opcode, Argument a)
	: opcode(opcode), a(a), b() {}

Instruction::Instruction(Opcode opcode, Argument a, Argument b)
	: opcode(opcode), a(a), b(b) {}

} }
