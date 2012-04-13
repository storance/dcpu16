#pragma once

#include <list>
#include "ast.hpp"
#include "error_handler.hpp"
#include "argument.hpp"

namespace grammar {
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	using qi::labels::_1;
	using qi::labels::_2;
	using qi::labels::_3;
	using qi::labels::_4;
	using qi::on_error;
	using qi::on_success;
	using qi::fail;
	using boost::phoenix::function;

	template <typename Iterator>
	struct Statement : qi::grammar<Iterator, std::list<ast::statement>(), skipper<Iterator> > {
		typedef skipper<Iterator> skipper_type;

		Statement(ErrorHandler<Iterator>& errorHandler) : Statement::base_type(start), argument(errorHandler) {
			opcodes_two_arg.add
				("set", ast::SET)
				("add", ast::ADD)
				("sub", ast::SUB)
				("mul", ast::MUL)
				("div", ast::DIV)
				("mod", ast::MOD)
				("shl", ast::SHL)
				("shr", ast::SHR)
				("and", ast::AND)
				("bor", ast::BOR)
				("or",  ast::BOR)
				("xor", ast::XOR)
				("ife", ast::IFE)
				("ifn", ast::IFN)
				("ifg", ast::IFG)
				("ifb", ast::IFB);

			opcodes_one_arg.add
				("jsr", ast::JSR)
				("jmp", ast::JMP)
				("push", ast::PUSH)
				("pop", ast::POP);

			start = (+_statement);

			_statement = instruction | label_decl | qi::eol;

			instruction = ((qi::no_case[opcodes_two_arg] > argument > ',' > argument)
				| (qi::no_case[opcodes_one_arg] > argument)) > (qi::eol | qi::eoi);

			label = qi::raw[qi::lexeme[(ascii::alpha | '_' |'?' |  '.') 
				>> *(ascii::alnum | '_' | '?' | '$' | '#' | '@' | '~' | '.')]];

			label_ref = ('$' > label) | (label - argument.all_registers);
			label_decl = (':' > label) | (label >> ':');

			BOOST_SPIRIT_DEBUG_NODES(
				(start)
            	(instruction)
            	(_statement)
            	(label_decl)
        	);

			on_error<fail>(start, function<ErrorHandler<Iterator> >(errorHandler)("Error! Expecting ", _4, _3));
		}

		Argument<Iterator> argument;

		qi::rule<Iterator, std::list<ast::statement>(), skipper_type> start;
		qi::rule<Iterator, ast::statement(), skipper_type> _statement;
		qi::rule<Iterator, ast::instruction(), skipper_type> instruction;
		qi::rule<Iterator, std::string(), skipper_type> label, label_ref;
		qi::rule<Iterator, ast::label(), skipper_type> label_decl;
		
		qi::symbols<char, ast::opcode_type> opcodes_one_arg, opcodes_two_arg;
	};

}