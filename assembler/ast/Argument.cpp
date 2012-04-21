#include "Argument.hpp"

using namespace std;

namespace dcpu { namespace ast {
	Argument::Argument(const Location& location) : _location(location) {}

	Argument::~Argument() {}

	StackArgument::StackArgument(const Location& location, StackOperation operation) 
		: Argument(location), _operation(operation) {}

	IndirectArgument::IndirectArgument(shared_ptr<Expression> expr)
		: Argument(expr->_location), _expr(expr) {}

	ExpressionArgument::ExpressionArgument(shared_ptr<Expression> expr)
		: Argument(expr->_location), _expr(expr) {}
}}