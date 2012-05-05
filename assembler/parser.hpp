#pragma once

#include <string>
#include <cstdint>

#include "statement.hpp"
#include "error_handler.hpp"
#include "expression_parser.hpp"

namespace dcpu { namespace parser {
	class parser {
	protected:
		lexer::token_iterator current, end;
		error_handler_ptr error_handler;
		ast::statement_list &statements;

		boost::optional<ast::statement> parse_label(const lexer::token&);
		boost::optional<ast::statement> parse_instruction(const lexer::token&);
		ast::optional_argument parse_argument(const lexer::token&, ast::argument_position);
		ast::optional_argument parse_indirect_stack_argument(const lexer::token&, ast::argument_position);
		ast::optional_argument parse_mnemonic_stack_argument(const lexer::token&, ast::argument_position);
		ast::expression parse_expression(const lexer::token&, bool, bool);

		lexer::token& next_token();
		void move_back();
		void advance_until(std::function<bool (const lexer::token&)>);
	public:
		parser(lexer::lexer &lexer, ast::statement_list &statements);

		void parse();
	};

} }
