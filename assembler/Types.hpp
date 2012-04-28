#pragma once

#include <list>
#include <memory>
#include <cstdint>

namespace dcpu {

	// forward declarations
	class SymbolTable;
	class ErrorHandler;

	namespace lexer {
		class Token;
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

	typedef std::unique_ptr<lexer::Token> TokenPtr;
	typedef std::list<TokenPtr> TokenList;
	typedef TokenList::iterator TokenIterator;
}