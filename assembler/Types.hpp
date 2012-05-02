#pragma once

#include <list>
#include <memory>
#include <tuple>
#include <cstdint>

#include <boost/optional.hpp>

namespace dcpu {

	// forward declarations
	class SymbolTable;
	class ErrorHandler;

	namespace lexer {
		class Token;
		class Location;
	}

	namespace ast {
		class Statement;
		class Argument;
		class Expression;
	}

	// typedefs
	typedef std::shared_ptr<ErrorHandler> ErrorHandlerPtr;
	typedef std::shared_ptr<SymbolTable> SymbolTablePtr;
	typedef std::shared_ptr<std::uint16_t> uint16Ptr;

	typedef std::unique_ptr<ast::Statement> StatementPtr;
	typedef std::unique_ptr<ast::Argument> ArgumentPtr;
	typedef std::unique_ptr<ast::Expression> ExpressionPtr;

	typedef std::list<StatementPtr> StatementList;
	typedef StatementList::iterator StatementIterator;

	typedef std::list<Token> TokenList;
	typedef TokenList::iterator TokenIterator;
	typedef std::shared_ptr<Location> LocationPtr;

	typedef std::tuple<std::uint8_t, boost::optional<std::uint16_t>> CompileResult;
}