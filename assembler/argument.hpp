#pragma once

#include <list>
#include "ast.hpp"
#include "error_handler.hpp"
#include "skipper.hpp"

namespace grammar {
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	template <typename Iterator>
	struct Argument : qi::grammar<Iterator, ast::argument(), skipper<Iterator> > {
		typedef skipper<Iterator> skipper_type;

		Argument(ErrorHandler<Iterator>& errorHandler) : Argument::base_type(start) {
			registers.add
				("a", registers::A)
				("b", registers::B)
				("c", registers::C)
				("x", registers::X)
				("y", registers::Y)
				("z", registers::Z)
				("i", registers::I)
				("j", registers::J);

			stack_pointer.add("sp", registers::SP);
			program_counter.add("pc", registers::PC);
			overflow.add("o", registers::O);

			stack_type.add
				("peek", ast::STACK_PEEK)
				("push", ast::STACK_PUSH)
				("pop",  ast::STACK_POP);

			all_registers = qi::no_case[registers | program_counter | stack_pointer | overflow];
			addressable_registers = qi::no_case[registers];

			start = _register | _literal | _stack;

			literal_value = qi::no_case[("0b" > qi::bin) | ("0x" > qi::hex) | ("0o" > qi::oct) | qi::uint_];

			_register = (qi::attr(false) >> all_registers >> qi::attr(0))
				| ('[' >> qi::attr(true) >> addressable_registers >> qi::attr(0) >> ']')
				| ('[' >> qi::attr(true) >> addressable_registers >> '+' >> literal_value >> ']')
				| register_ptr_offset;

			register_ptr_offset = '[' >> literal_value >> '+' >> addressable_registers  >> qi::attr(true) >> ']';

			_literal = (qi::attr(false) >> literal_value) | ('[' >> qi::attr(true) >> literal_value >> ']');

			_stack = qi::no_case[stack_type
				| (qi::omit[qi::lit('[') >> stack_pointer >> ']'] >> qi::attr(ast::STACK_PEEK))
				| (qi::omit[qi::lit('[') >> "--" >> stack_pointer >> ']'] >> qi::attr(ast::STACK_PUSH))
				| (qi::omit[qi::lit('[') >> stack_pointer >> "++" >> ']'] >> qi::attr(ast::STACK_POP))
			];
		}

		qi::rule<Iterator, ast::argument(), skipper_type> start;

		qi::rule<Iterator, ast::register_argument(), skipper_type> _register;
		qi::rule<Iterator, ast::literal_argument(), skipper_type> _literal;
		qi::rule<Iterator, ast::stack_argument(), skipper_type> _stack;
		qi::rule<Iterator, reverse_register_argument(), skipper_type> register_ptr_offset;
		qi::rule<Iterator, uint32_t(), skipper_type > literal_value;
		qi::rule<Iterator, registers::register_type(), skipper_type> all_registers, addressable_registers;

		qi::symbols<char, registers::register_type> registers, program_counter, stack_pointer, overflow;
		qi::symbols<char, ast::stack_type> stack_type;
	};
}