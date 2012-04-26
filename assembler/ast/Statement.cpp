#include "Statement.hpp"

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace dcpu::lexer;

namespace dcpu { namespace ast {
	/*************************************************************************
	 *
	 * Statement
	 *
	 *************************************************************************/
	Statement::Statement(const Location& location) : _location(location) {}

	Statement::~Statement() {}

	/*************************************************************************
	 *
	 * Label
	 *
	 *************************************************************************/

	Label::Label(const Location &location, const std::string &name)
		: Statement(location), _name(name) {
		if (boost::starts_with(_name, "..@")) {
			_type = LabelType::GlobalNoAttach;
		} else if (boost::starts_with(_name, ".")) {
			_type = LabelType::Local;
		} else {
			_type = LabelType::Global;
		}
	}

	/*************************************************************************
	 *
	 * Instruction
	 *
	 *************************************************************************/

	Instruction::Instruction(const Location &location, Opcode opcode, ArgumentPtr& a, ArgumentPtr& b)
		: Statement(location), _opcode(opcode), _a(move(a)), _b(move(b)) {}

	Instruction::Instruction(const Location &location, Opcode opcode, ArgumentPtr&& a, ArgumentPtr&& b)
		: Statement(location), _opcode(opcode), _a(move(a)), _b(move(b)) {}

}}