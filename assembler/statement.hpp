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


	    	general_purpose_registers.add
	    		("a", registers::A)
	    		("b", registers::B)
	    		("c", registers::C)
	    		("x", registers::X)
	    		("y", registers::Y)
	    		("z", registers::Z)
	    		("i", registers::I)
	    		("j", registers::J);

	    	sp_register.add("sp", registers::SP);
	    	pc_register.add("pc", registers::PC);
	    	o_register.add("o", registers::O);

	    	stack_type.add
	    		("peek", ast::STACK_PEEK)
	    		("push", ast::STACK_PUSH)
	    		("pop",  ast::STACK_POP);

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

	    	all_registers = qi::no_case[
	    		general_purpose_registers 
	    		| pc_register 
	    		| sp_register 
	    		| o_register
    		];

	    	addressable_registers = qi::no_case[
	    		general_purpose_registers 
	    		| sp_register
	    	];

	    	offset_addressable_registers = qi::no_case[
	    		general_purpose_registers
	    	];

	    	literal_value =  qi::no_case[
	    		("0b" > qi::bin)
	    		| ("0x" > qi::hex)
	    		| ("0o" > qi::oct)
	    		| qi::uint_
	    	];

			statement_list = +statement_;

	    	statement_ = instruction | qi::eol;

	    	instruction = (
	    			(qi::no_case[opcodes_two_arg] > argument > ',' > argument)
	    			| (qi::no_case[opcodes_one_arg] > argument)
	    		) > (qi::eol | qi::eoi);

	    	argument = register_argument 
	    		| literal_argument
	    		| stack_argument;

	    	register_argument = (qi::attr(false) >> all_registers)
	    		| ('[' >> qi::attr(true) >> addressable_registers >> ']');

	    	literal_argument = (qi::attr(false) >> literal_value) 
	    		| ('[' >> qi::attr(true) >> literal_value >> ']');

	    	stack_argument = qi::no_case[
	    		stack_type
	    		| (qi::omit['[' >> sp_register >> ']'] 
	    			>> qi::attr(ast::STACK_PEEK))
	    		| (qi::omit[qi::lit('[') >> "--" >> sp_register >> ']'] 
    				>> qi::attr(ast::STACK_PUSH))
	    		| (qi::omit['[' >> sp_register >> "++" >> ']'] 
	    			>> qi::attr(ast::STACK_POP))
	    	];
    	}

    	qi::rule<Iterator, std::list<ast::statement>(), skipper_type > statement_list;
	    qi::rule<Iterator, ast::statement(), skipper_type > statement_;
	    qi::rule<Iterator, ast::instruction(), skipper_type > instruction;
	    
	    qi::rule<Iterator, ast::argument(), skipper_type > argument;
	    qi::rule<Iterator, ast::register_argument(), skipper_type > register_argument;
	    qi::rule<Iterator, ast::literal_argument(), skipper_type > literal_argument;
	    qi::rule<Iterator, ast::stack_argument(), skipper_type > stack_argument;

	    qi::rule<Iterator, uint32_t(), skipper_type > literal_value;
	    qi::rule<Iterator, registers::register_type(), skipper_type > all_registers,
	    	addressable_registers, offset_addressable_registers;
	    qi::symbols<char, registers::register_type> general_purpose_registers,
	    	pc_register, sp_register, o_register;
	    qi::symbols<char, ast::opcode_type> opcodes_one_arg, opcodes_two_arg;
	    qi::symbols<char, ast::stack_type> stack_type;
	};

}