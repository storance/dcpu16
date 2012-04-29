#include "Argument.hpp"

#include <stdexcept>
#include <boost/format.hpp>

#include "../SymbolTable.hpp"

using namespace std;
using namespace dcpu::lexer;

namespace dcpu { namespace ast {
	/*************************************************************************
	 *
	 * Argument
	 *
	 *************************************************************************/
	Argument::Argument(const Location& location, const ArgumentFlags& flags) 
		: flags(flags), location(location) {}

	void Argument::resolveLabels(SymbolTablePtr& table, ErrorHandlerPtr &errorHandler, uint16_t position) {

	}

	bool Argument::compress(SymbolTablePtr& table, uint16_t pc) {
		return false;
	}

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
	return ArgumentPtr(new ExpressionArgument(position, expr, true, false));
	}

	ArgumentPtr Argument::expression(ArgumentPosition position, ExpressionPtr &&expr) {
		return ArgumentPtr(new ExpressionArgument(position, expr, false, false));
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
		: Argument(location, ArgumentFlags(position, true, false)), operation(operation) {}

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

		return flags == otherStack->flags && operation == otherStack->operation;
	}

	/*************************************************************************
	 *
	 * ExpressionArgument
	 *
	 *************************************************************************/

	ExpressionArgument::ExpressionArgument(ArgumentPosition position, ExpressionPtr& expr, bool indirect,
		bool forceNextWord) : Argument(expr->location, ArgumentFlags(position, indirect, forceNextWord)),
		expr(move(expr)), nextWordRequired(true) {}

	string ExpressionArgument::str() const {
		if (flags.isIndirection()) {
			return boost::str(boost::format("[%s]") % expr);
		} else {
			return boost::str(boost::format("%s") % expr);
		}
	}

	void ExpressionArgument::resolveLabels(SymbolTablePtr& table, ErrorHandlerPtr &errorHandler, uint16_t pc) {
		if (!expr->isEvaluated()) {
			expr->resolveLabels(table, errorHandler);

			if (!expr->isEvaluatable()) {
				return;
			}

			nextWordRequired = expr->evaluate()->isNextWordRequired(flags);

			if (!nextWordRequired) {
				table->decrementAfter(pc);
			}
		}
	}

	bool ExpressionArgument::compress(SymbolTablePtr& table, uint16_t pc) {
		if (!expr->isEvaluated() && expr->isEvaluatable()) {
			if (nextWordRequired && !expr->evaluate()->isNextWordRequired(flags)) {
				nextWordRequired = false;
				table->decrementAfter(pc);

				return true;
			}
		}

		return false;
	}

	bool ExpressionArgument::isNextWordRequired() const {
		return expr->isNextWordRequired(flags);
	}

	CompileResult ExpressionArgument::compile() const {
		if (!expr->isEvaluated()) {
			return expr->evaluate()->compile(flags);
		}
		return expr->compile(flags);
	}

	bool ExpressionArgument::operator==(const Argument& other) const {
		const ExpressionArgument *otherExpr = dynamic_cast<const ExpressionArgument*>(&other);

		if (!otherExpr) return false;

		return flags == otherExpr->flags && expr == otherExpr->expr;
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