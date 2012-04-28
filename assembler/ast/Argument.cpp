#include "Argument.hpp"

#include <stdexcept>
#include <boost/format.hpp>

using namespace std;
using namespace dcpu::lexer;

namespace dcpu { namespace ast {
	/*************************************************************************
	 *
	 * Argument
	 *
	 *************************************************************************/
	Argument::Argument(const Location& location, ArgumentPosition position) 
		: _location(location), _position(position) {}

	Argument::~Argument() {}

	/*************************************************************************
	 *
	 * StackArgument
	 *
	 *************************************************************************/

	StackArgument::StackArgument(const Location& location, ArgumentPosition position, StackOperation operation) 
		: Argument(location, position), _operation(operation) {}

	string StackArgument::str() const {
		return ast::str(_operation);
	}

	bool StackArgument::isNextWordRequired() const {
		return false;
	}

	uint8_t StackArgument::compile(std::vector<std::uint16_t> &output) {
		if (_operation == StackOperation::PUSH || _operation == StackOperation::POP) {
			return 0x18;
		} else if (_operation == StackOperation::PEEK) {
			return 0x19;
		}

		throw new logic_error(boost::str(boost::format("Unknown StackOperation %d") % static_cast<int>(_operation)));
	}

	/*************************************************************************
	 *
	 * IndirectArgument
	 *
	 *************************************************************************/

	IndirectArgument::IndirectArgument(ArgumentPosition position, ExpressionPtr& expr)
		: Argument(expr->_location, position), _expr(move(expr)) {}

	IndirectArgument::IndirectArgument(ArgumentPosition position, ExpressionPtr&& expr)
		: Argument(expr->_location, position), _expr(move(expr)) {}

	string IndirectArgument::str() const {
		return (boost::format("[%s]") % ast::str(_expr)).str();
	}

	bool IndirectArgument::isNextWordRequired() const {
		return _expr->isNextWordRequired(_position, false);
	}

	uint8_t IndirectArgument::compile(std::vector<std::uint16_t> &output) {
		return _expr->compile(output, _position, true, false);
	}

	/*************************************************************************
	 *
	 * ExpressionArgument
	 *
	 *************************************************************************/

	ExpressionArgument::ExpressionArgument(ArgumentPosition position, ExpressionPtr& expr)
		: Argument(expr->_location, position), _expr(move(expr)) {}

	ExpressionArgument::ExpressionArgument(ArgumentPosition position, ExpressionPtr&& expr)
		: Argument(expr->_location, position), _expr(move(expr)) {}

	string ExpressionArgument::str() const {
		return ast::str(_expr);
	}

	bool ExpressionArgument::isNextWordRequired() const {
		return _expr->isNextWordRequired(_position, false);
	}

	uint8_t ExpressionArgument::compile(std::vector<std::uint16_t> &output) {
		return _expr->compile(output, _position, false, false);
	}

	/*************************************************************************
	 *
	 * Pretty Printers
	 *
	 *************************************************************************/

	string str(ArgumentPosition position) {
		switch (position) {
		case ArgumentPosition::A:
			return "a";
		case ArgumentPosition::B:
			return "b";
		default:
			return "<Unknown ArgumentPosition>";
		}
	}

	string str(StackOperation operation) {
		switch (operation) {
		case StackOperation::PUSH:
			return "PUSH";
		case StackOperation::POP:
			return "POP";
		case StackOperation::PEEK:
			return "PEEK";
		default:
			return "<Unknown StackOperation>";
		}
	}

	string str(const ArgumentPtr &argument) {
		return argument->str();
	}
}}