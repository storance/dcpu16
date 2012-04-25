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

	/*************************************************************************
	 *
	 * Pretty Print
	 *
	 *************************************************************************/

	std::string str(Opcode opcode) {
		switch(opcode) {
		case Opcode::SET:
			return "SET";
		case Opcode::ADD:
			return "ADD";
		case Opcode::SUB:
			return "SUB";
		case Opcode::MUL:
			return "MUL";
		case Opcode::MLI:
			return "MLI";
		case Opcode::DIV:
			return "DIV";
		case Opcode::DVI:
			return "DVI";
		case Opcode::MOD:
			return "MOD";
		case Opcode::AND:
			return "AND";
		case Opcode::BOR:
			return "BOR";
		case Opcode::XOR:
			return "XOR";
		case Opcode::SHR:
			return "SHR";
		case Opcode::ASR:
			return "ASR";
		case Opcode::SHL:
			return "SHL";
		case Opcode::IFB:
			return "IFB";
		case Opcode::IFC:
			return "IFC";
		case Opcode::IFE:
			return "IFE";
		case Opcode::IFN:
			return "IFN";
		case Opcode::IFG:
			return "IFG";
		case Opcode::IFA:
			return "IFA";
		case Opcode::IFL:
			return "IFL";
		case Opcode::IFU:
			return "IFU";
		case Opcode::JSR:
			return "JSR";
		case Opcode::INT:
			return "INT";
		case Opcode::ING:
			return "ING";
		case Opcode::INS:
			return "INS";
		case Opcode::HWN:
			return "HWN";
		case Opcode::HWQ:
			return "HWQ";
		case Opcode::HWI:
			return "HWI";
		case Opcode::JMP:
			return "JMP";
		}

		return "<Unknown Opcode>";
	}
}}