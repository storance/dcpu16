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

	void Statement::evaluateExpressions(SymbolTable& table, ErrorHandler &errorHandler) {

	}
	
	bool Statement::compress(SymbolTable& table) {
		return false;
	}
	
	void Statement::compile(vector<uint16_t> &output) {

	}

	StatementPtr Statement::label(const lexer::Location& location, const std::string& name) {
		return StatementPtr(new Label(location, name));
	}

	StatementPtr Statement::instruction(const lexer::Location& location, Opcode opcode, ArgumentPtr& a, ArgumentPtr& b) {
		return StatementPtr(new Instruction(location, opcode, a, b));
	}

	StatementPtr Statement::null() {
		return StatementPtr();
	}

	/*************************************************************************
	 *
	 * Label
	 *
	 *************************************************************************/

	Label::Label(const Location &location, const string &name)
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

	void Label::buildSymbolTable(SymbolTable& table, uint16_t &position) const {
		table.add(*this, position);
	}

	/*************************************************************************
	 *
	 * Instruction
	 *
	 *************************************************************************/

	Instruction::Instruction(const Location &location, Opcode opcode, ArgumentPtr& a, ArgumentPtr& b)
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

	void Instruction::buildSymbolTable(SymbolTable& table, uint16_t &position) const {
		++position;

		if (_b && _b->isNextWordRequired()) {
			++position;
		}

		if (_a && _a->isNextWordRequired()) {
			++position;
		}
	}

	void Instruction::evaluateExpressions(SymbolTable& table, ErrorHandler &errorHandler) {

	}
	
	bool Instruction::compress(SymbolTable& table) {
		return false;
	}
	
	void Instruction::compile(vector<uint16_t> &output) {
		// Allows the arg compile to push their next word to output while we're still building the instruction
		output.push_back(0);
		uint16_t &instruction = output.back();
		if (_opcode == Opcode::JMP) {
			instruction = static_cast<uint16_t>(Opcode::SET) | (0x1c << 5);
		} else {
			instruction = static_cast<uint16_t>(_opcode);
		}

		if (_a) {
			instruction |= (_a->compile(output) & 0x3f) << 10;
		}

		if (_b) {
			instruction |= (_b->compile(output) & 0x1f) << 5;
		}
	}

	string str(const StatementPtr & stmt) {
		return stmt->str();
	}
}}