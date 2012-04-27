#include "Argument.hpp"

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
		return _expr->isNextWordRequired();
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
		return _expr->isNextWordRequired();
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