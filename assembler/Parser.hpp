#pragma once

#include <string>
#include <cstdint>

#include "AbstractSyntaxTree.hpp"
#include "Lexer.hpp"

namespace dcpu { namespace parser {
	typedef std::list<lexer::token_type>::const_iterator token_iterator;

	class Parser {
	protected:
		std::list<lexer::token_type> tokens;
		token_iterator current;
		uint32_t totalErrors, totalWarnings;

		void error(Location, std::string);
		void warning(Location, std::string);

		Statement nextStatement();

		lexer::token_type nextToken();
	public:
		std::list<ast::Statement> statements;

		Parser(std::list<lexer::token_type> tokens);

		void parse();
	};

} }
