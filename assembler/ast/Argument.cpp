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

	ArgumentPtr Argument::stack(const Location& location, ArgumentPosition position, StackOperation operation) {
		return ArgumentPtr(new StackArgument(location, position, operation));
	}

	ArgumentPtr Argument::stackPop(const Location& location, ArgumentPosition position) {
		return stack(location, position, StackOperation::POP);
	}

	ArgumentPtr Argument::stackPush(const Location& location, ArgumentPosition position) {
		return stack(location, position, StackOperation::PUSH);
	}

	ArgumentPtr Argument::stackPeek(const Location& location, ArgumentPosition position) {
		return stack(location, position, StackOperation::PEEK);
	}

	ArgumentPtr Argument::indirect(ArgumentPosition position, ExpressionPtr &&expr) {
	return ArgumentPtr(new IndirectArgument(position, expr));
	}

	ArgumentPtr Argument::expression(ArgumentPosition position, ExpressionPtr &&expr) {
		return ArgumentPtr(new ExpressionArgument(position, expr));
	}

	ArgumentPtr Argument::null() {
		return ArgumentPtr(nullptr);
	}

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
		switch (_operation) {
		case StackOperation::PUSH:
		case StackOperation::POP:
			return 0x18;
		case StackOperation::PEEK:
			return 0x19;
		default:
			throw new logic_error(ast::str(_operation));
		}
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

	string str(const ArgumentPtr &argument) {
		return argument->str();
	}
}}