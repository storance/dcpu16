#pragma once

#include <list>
#include "ast.hpp"
#include "skipper.hpp"

namespace grammar {
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	template <typename Iterator>
	struct statement : qi::grammar<Iterator, std::list<ast::statement>(), skipper<Iterator> > {
		typedef skipper<Iterator> skipper_type;

	    statement() : statement::base_type(statement_list) {
	    	skipper_type skipper_;


	    	registers.add
	    		("a", registers::A)
	    		("b", registers::B)
	    		("c", registers::C)
	    		("x", registers::X)
	    		("y", registers::Y)
	    		("z", registers::Z)
	    		("i", registers::I)
	    		("j", registers::J);

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

			statement_list = +statement_;

	    	statement_ = instruction | qi::eol;

	    	instruction = (
	    			(qi::no_case[opcodes_two_arg] > argument > ',' > argument)
	    			| (qi::no_case[opcodes_one_arg] > argument)
	    		)
	    		> (qi::eol | qi::eoi);

	    	argument = register_argument 
	    		| register_ptr_argument 
	    		| literal_argument
	    		| literal_ptr_argument;

	    	register_argument = qi::no_case[registers];

	    	register_ptr_argument = '[' >> qi::no_case[registers] >> ']';

	    	literal_argument = literal_value;

	    	literal_ptr_argument = '[' >> literal_value >> ']';

	    	literal_value =  ("0b" > qi::bin)
	    			| ("0x" > qi::hex)
	    			| ("0o" > qi::oct)
	    			| qi::uint_;

    	}

    	qi::rule<Iterator, std::list<ast::statement>(), skipper_type > statement_list;
	    qi::rule<Iterator, ast::statement(), skipper_type > statement_;
	    qi::rule<Iterator, ast::instruction(), skipper_type > instruction;
	    qi::rule<Iterator, ast::argument(), skipper_type > argument;
	    qi::rule<Iterator, ast::register_argument(), skipper_type > register_argument;
	    qi::rule<Iterator, ast::register_ptr_argument(), skipper_type > register_ptr_argument;
	    qi::rule<Iterator, ast::literal_argument(), skipper_type > literal_argument;
	    qi::rule<Iterator, ast::literal_ptr_argument(), skipper_type > literal_ptr_argument;
	    qi::rule<Iterator, uint32_t(), skipper_type > literal_value;
	    qi::symbols<char, registers::register_type> registers;
	    qi::symbols<char, ast::opcode_type> opcodes_one_arg, opcodes_two_arg;
	};

}