#include "Argument.hpp"

using namespace std;

namespace dcpu { namespace ast {
	Argument::Argument(const Location& location) : _location(location) {}

	Argument::~Argument() {}

	StackArgument::StackArgument(const Location& location, StackOperation operation) 
		: Argument(location), _operation(operation) {}

	IndirectArgument::IndirectArgument(ExpressionPtr& expr)
		: Argument(expr->_location), _expr(move(expr)) {}

	IndirectArgument::IndirectArgument(ExpressionPtr&& expr)
		: Argument(expr->_location), _expr(move(expr)) {}

	ExpressionArgument::ExpressionArgument(ExpressionPtr& expr)
		: Argument(expr->_location), _expr(move(expr)) {}

	ExpressionArgument::ExpressionArgument(ExpressionPtr&& expr)
		: Argument(expr->_location), _expr(move(expr)) {}
}}