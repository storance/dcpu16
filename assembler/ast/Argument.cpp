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
		: location(location), position(position) {}

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
		: Argument(location, position), operation(operation) {}

	string StackArgument::str() const {
		return (boost::format("%s") % operation).str();
	}

	bool StackArgument::isNextWordRequired() const {
		return false;
	}

	CompileResult StackArgument::compile() const {
		switch (operation) {
		case StackOperation::PUSH:
		case StackOperation::POP:
			return CompileResult(0x18, boost::none);
		case StackOperation::PEEK:
			return CompileResult(0x19, boost::none);
		default:
			throw logic_error(boost::str(boost::format("%s") % operation));
		}
	}

	bool StackArgument::operator==(const Argument& other) const {
		const StackArgument *otherStack = dynamic_cast<const StackArgument*>(&other);

		if (!otherStack) return false;

		return position == otherStack->position && operation == otherStack->operation;
	}

	/*************************************************************************
	 *
	 * IndirectArgument
	 *
	 *************************************************************************/

	IndirectArgument::IndirectArgument(ArgumentPosition position, ExpressionPtr& expr)
		: Argument(expr->location, position), expr(move(expr)) {}

	IndirectArgument::IndirectArgument(ArgumentPosition position, ExpressionPtr&& expr)
		: Argument(expr->location, position), expr(move(expr)) {}

	string IndirectArgument::str() const {
		return (boost::format("[%s]") % expr).str();
	}

	bool IndirectArgument::isNextWordRequired() const {
		return expr->isNextWordRequired(CompileFlags(position, true, false));
	}

	CompileResult IndirectArgument::compile() const {
		return expr->compile(CompileFlags(position, false, false));
	}

	bool IndirectArgument::operator==(const Argument& other) const {
		const IndirectArgument *otherIndirect = dynamic_cast<const IndirectArgument*>(&other);

		if (!otherIndirect) return false;

		return position == otherIndirect->position && expr == otherIndirect->expr;
	}

	/*************************************************************************
	 *
	 * ExpressionArgument
	 *
	 *************************************************************************/

	ExpressionArgument::ExpressionArgument(ArgumentPosition position, ExpressionPtr& expr)
		: Argument(expr->location, position), expr(move(expr)) {}

	ExpressionArgument::ExpressionArgument(ArgumentPosition position, ExpressionPtr&& expr)
		: Argument(expr->location, position), expr(move(expr)) {}

	string ExpressionArgument::str() const {
		return expr->str();
	}

	bool ExpressionArgument::isNextWordRequired() const {
		return expr->isNextWordRequired(CompileFlags(position, false, false));
	}

	CompileResult ExpressionArgument::compile() const {
		return expr->compile(CompileFlags(position, false, false));
	}

	bool ExpressionArgument::operator==(const Argument& other) const {
		const ExpressionArgument *otherExpr = dynamic_cast<const ExpressionArgument*>(&other);

		if (!otherExpr) return false;

		return position == otherExpr->position && expr == otherExpr->expr;
	}

	/*************************************************************************
	 *
	 * Operators
	 *
	 *************************************************************************/

	bool operator== (const ArgumentPtr& left, const ArgumentPtr& right) {
		if (left && right) {
			return *left == *right;
		} else {
			return !left == !right;
		}
	}

	std::ostream& operator<< (std::ostream& stream, const ArgumentPtr &argument) {
		return stream << argument->str();
	}
}}