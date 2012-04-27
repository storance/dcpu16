#include "Statement.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "../SymbolTable.hpp"

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

	string Label::str() const {
		return (boost::format("%s:") % _name).str();
	}

	void Label::buildSymbolTable(SymbolTable& table, std::uint16_t &position) const {
		table.add(*this, position);
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

	string Instruction::str() const {
		if ((!_b && !_a) || (_b && !_a)) {
			return "";
		}

		if (_b) {
			return (boost::format("%s %s, %s") % ast::str(_opcode) % ast::str(_b) % ast::str(_a)).str();	
		} else {
			return (boost::format("%s %s") % ast::str(_opcode) % ast::str(_a)).str();	
		}
	}

	void Instruction::buildSymbolTable(SymbolTable& table, std::uint16_t &position) const {
		++position;

		if (_b && _b->isNextWordRequired()) {
			++position;
		}

		if (_a && _a->isNextWordRequired()) {
			++position;
		}
	}

	string str(const StatementPtr & stmt) {
		return stmt->str();
	}
}}