#pragma once

#include <string>
#include <functional>
#include <cstdint>

#include "lexer.hpp"
#include "statement.hpp"
#include "expression_parser.hpp"

namespace dcpu { namespace assembler {
	class parser {
	protected:
		token_iterator current, end;
		log& logger;
		statement_list &statements;
		uint32_t instructions_found, labels_found;

		boost::optional<statement> parse_label(const token&);
		boost::optional<statement> parse_instruction(const token&);
		boost::optional<statement> parse_directive(const token& );
		data_directive parse_data_word(const token&);
		data_directive parse_data_byte(const token&);
		void parse_data(const token& current_token, std::vector<std::uint16_t> &output, bool packed);
		org_directive parse_org(const token&);
		equ_directive parse_equ(const token&);
		fill_directive parse_fill(const token&);
		align_directive parse_align(const token&);
		optional_argument parse_argument(const token&, argument_position);
		optional_argument parse_indirect_argument(const token&, argument_position);
		optional_argument parse_stack_argument(const token&, argument_position);
		expression parse_expression(const token&, uint32_t flags);

		token& next_token();
		void move_back();
		void advance_until(std::function<bool (const token&)>);
	public:
		parser(lexer &lexer, statement_list &statements);

		void parse();
	};

} }
