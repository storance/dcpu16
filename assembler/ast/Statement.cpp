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
	Statement::Statement(const Location& location) : location(location) {}

	Statement::~Statement() {}

	void Statement::evaluateExpressions(SymbolTablePtr& table, ErrorHandlerPtr &errorHandler) {

	}
	
	bool Statement::compress(SymbolTablePtr& table) {
		return false;
	}
	
	void Statement::compile(vector<uint16_t> &output) {

	}

	StatementPtr Statement::label(const lexer::Location &location, const string& name) {
		return StatementPtr(new Label(location, name));
	}

	StatementPtr Statement::label(const lexer::Location &location, const std::string &name, LabelType type) {
		return StatementPtr(new Label(location, name, type));
	}

	StatementPtr Statement::instruction(const Location &location, Opcode opcode, ArgumentPtr& a, ArgumentPtr& b) {
		return StatementPtr(new Instruction(location, opcode, a, b));
	}

	StatementPtr Statement::instruction(const Location &location, Opcode opcode, ArgumentPtr &&a, ArgumentPtr &&b) {
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
		: Statement(location), name(name) {
		if (boost::starts_with(name, "..@")) {
			type = LabelType::GlobalNoAttach;
		} else if (boost::starts_with(name, ".")) {
			type = LabelType::Local;
		} else {
			type = LabelType::Global;
		}
	}

	Label::Label(const lexer::Location &location, const std::string &name, LabelType type)
		: Statement(location), type(type), name(name) {}

	string Label::str() const {
		return (boost::format("%s:") % name).str();
	}

	bool Label::operator==(const Statement &other) const {
		const Label *otherLabel = dynamic_cast<const Label*>(&other);

		return name == otherLabel->name;
	}

	void Label::buildSymbolTable(SymbolTablePtr& table, uint16Ptr &position) const {
		table->add(*this, *position);
	}

	/*************************************************************************
	 *
	 * Instruction
	 *
	 *************************************************************************/

	Instruction::Instruction(const Location &location, Opcode opcode, ArgumentPtr& a, ArgumentPtr& b)
		: Statement(location), opcode(opcode), a(move(a)), b(move(b)) {}

	string Instruction::str() const {
		if ((!b && !a) || (b && !a)) {
			return "";
		}

		if (b) {
			return (boost::format("%s %s, %s") % opcode % b % a).str();	
		} else {
			return (boost::format("%s %s") % opcode % a).str();	
		}
	}

	void Instruction::buildSymbolTable(SymbolTablePtr& table, uint16Ptr &position) const {
		++*position;

		if (b && b->isNextWordRequired()) {
			++*position;
		}

		if (a && a->isNextWordRequired()) {
			++*position;
		}
	}

	void Instruction::evaluateExpressions(SymbolTablePtr& table, ErrorHandlerPtr &errorHandler) {

	}
	
	bool Instruction::compress(SymbolTablePtr& table) {
		return false;
	}

	void Instruction::compile(vector<uint16_t> &output, Opcode opcode, ArgumentPtr &a, ArgumentPtr &b) {
		uint16_t instruction = static_cast<uint16_t>(opcode);

		boost::optional<uint16_t> aNextWord, bNextWord;
		if (a) {
			CompileResult result = a->compile();
			instruction |= (get<0>(result) & 0x3f) << 10;
			aNextWord = get<1>(result);
		}

		if (b) {
			CompileResult result = b->compile();
			instruction |= (get<0>(result) & 0x1f) << 5;
			bNextWord = get<1>(result);
		}

		output.push_back(instruction);
		if (aNextWord) {
			output.push_back(*aNextWord);
		}

		if (bNextWord) {
			output.push_back(*bNextWord);
		}
	}
	
	void Instruction::compile(vector<uint16_t> &output) {
		if (opcode == Opcode::JMP) {
			auto jmpB = Argument::expression(
				ArgumentPosition::B,
				Expression::evaluatedRegister(location, Register::PC)
			);

			compile(output, Opcode::SET, a, jmpB);
		} else {
			compile(output, opcode, a, b);
		}
	}

	bool Instruction::operator==(const Statement &other) const {
		const Instruction *otherInstruction = dynamic_cast<const Instruction*>(&other);

		return opcode == otherInstruction->opcode
			&& a == otherInstruction->a
			&& b == otherInstruction-> b;
	}

	/*************************************************************************
	 *
	 * Operators
	 *
	 *************************************************************************/
	bool operator== (const StatementPtr& left, const StatementPtr& right) {
		if (left && right) {
			return *left == *right;
		} else {
			return !left == !right;
		}
	}

	ostream& operator<< (ostream& stream, const StatementPtr &stmt) {
		return stream << stmt->str();
	}
}}