#include <tuple>
#include <iostream>
#include <fstream>
#include <boost/variant.hpp>

#include "compiler.hpp"
#include "../common.hpp"

using namespace std;

class CalculateArgumentSize : public boost::static_visitor<uint8_t> 
{
public:
    uint8_t operator()(const ast::register_argument &reg) const {
        return 0;
    }

    uint8_t operator()(const ast::literal_argument &l) const {
    	return l.value <= MAX_SHORT_LITERAL ? 0 : 1;
    }
};

typedef boost::optional<word_t> next_word;
typedef std::tuple<uint8_t, next_word> encoded_argument;

class EncodeArgument : public boost::static_visitor<encoded_argument> {
public:
	encoded_argument operator()(const ast::register_argument &r) const {
		switch(r.type) {
		REGISTER_CASES(registers::A)
			if (r.is_ptr) {
				return encoded_argument(arguments::PTR_A + r.type, next_word());
			} else {
				return encoded_argument(arguments::A + r.type, next_word());
			}
		case registers::SP:
			if (r.is_ptr) {
				return encoded_argument(arguments::PEEK, next_word());
			} else {
				return encoded_argument(arguments::SP, next_word());
			}
		case registers::PC:
			return encoded_argument(arguments::PC, next_word());
		case registers::O:
			return encoded_argument(arguments::O, next_word());
		}

		return encoded_argument(0, next_word());
    }

    encoded_argument operator()(const ast::literal_argument &l) const {
    	if (l.is_ptr) {
    		return encoded_argument(arguments::PTR_NEXT_WORD, next_word(l.value));
    	} else {
	    	if (l.value <= MAX_SHORT_LITERAL) {
	    		return encoded_argument(l.value + arguments::LITERAL_START, next_word());
	    	} else {
	    		return encoded_argument(arguments::NEXT_WORD, next_word(l.value));
	    	}
	    }
    }

    encoded_argument operator()(const ast::stack_argument &s) const {
    	switch (s.type) {
    	case ast::STACK_PEEK:
    		return encoded_argument(arguments::PEEK, next_word());
    	case ast::STACK_PUSH:
    		return encoded_argument(arguments::PUSH, next_word());
    	case ast::STACK_POP:
    		return encoded_argument(arguments::POP, next_word());
    	}
    }
};

class EncodeInstruction : public boost::static_visitor<void> {
private:
	vector<word_t> &encoded_words;
	EncodeArgument arg_encoder;

	void encode_basic_opcode(uint8_t opcode, const ast::argument &a, const ast::argument &b) {
		encoded_argument encoded_a = boost::apply_visitor(arg_encoder, a);
		encoded_argument encoded_b = boost::apply_visitor(arg_encoder, b);

		word_t instruction = opcode;
		instruction |= get<0>(encoded_a) << 4;
		instruction |= get<0>(encoded_b) << 10;

		encoded_words.push_back(instruction);
		if (get<1>(encoded_a)) {
			encoded_words.push_back(*get<1>(encoded_a));
		}

		if (get<1>(encoded_b)) {
			encoded_words.push_back(*get<1>(encoded_b));
		}
	}

	void encode_non_basic_opcode(uint8_t opcode, const ast::argument &a) {
		encoded_argument encoded_a = boost::apply_visitor(arg_encoder, a);

		word_t instruction = (opcode << 4) | (get<0>(encoded_a) << 10);
		encoded_words.push_back(instruction);

		if (get<1>(encoded_a)) {
			encoded_words.push_back(*get<1>(encoded_a));
		}
	}
public:
	EncodeInstruction(vector<word_t> &encoded_words) 
		: encoded_words(encoded_words) {}

	void operator()(const ast::instruction &i) {
        switch (i.opcode) {
        case ast::SET:
        case ast::ADD:
        case ast::SUB:
        case ast::MUL:
        case ast::DIV:
        case ast::MOD:
        case ast::SHL:
        case ast::SHR:
        case ast::AND:
		case ast::BOR:
		case ast::XOR:
		case ast::IFE:
		case ast::IFN:
		case ast::IFG:
		case ast::IFB:
			encode_basic_opcode((uint8_t)i.opcode, i.a, *i.b);
			break;
		case ast::JSR:
			{
				uint8_t nbOpcode = i.opcode - ast::JSR + 1;
				encode_non_basic_opcode(nbOpcode, i.a);
			}
			break;
		case ast::JMP:
			{
				ast::argument real_a(ast::register_argument(false, registers::PC));

				encode_basic_opcode(ast::SET, real_a, i.a);
			}
		case ast::PUSH:
			{
				ast::argument real_a(ast::stack_argument(ast::STACK_PUSH));

				encode_basic_opcode(ast::SET, real_a, i.a);
			}
		case ast::POP:
			{
				ast::argument arg_b(ast::stack_argument(ast::STACK_POP));

				encode_basic_opcode(ast::SET, i.a, arg_b);
			}
			break;
        }
    }
    
    void operator()(const ast::label &l) {
        
    }
};

void Compiler::compile(const list<ast::statement> & syntaxTree) {
	vector<word_t> encoded_words;

	EncodeInstruction encoder(encoded_words);

	for (ast::statement stmt : syntaxTree) {
		boost::apply_visitor(encoder, stmt);
	}

	ofstream file("a.bin");
	for (word_t instruction : encoded_words) {
		uint8_t b1 = (instruction >> 8) & 0xff;
		uint8_t b2 = instruction & 0xff;

		file.put(b1).put(b2);
	}

	file.close();
}