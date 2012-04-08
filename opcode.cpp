#include <inttypes.h>
#include <string>
#include <exception>
#include <stdexcept>

#include <boost/format.hpp>

#include "opcode.hpp"
#include "dcpu.hpp"

using namespace std;

static word_t literals[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
	0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
	0x1e, 0x1f
};

OpcodeArgument::OpcodeArgument(Opcode *aParent, uint8_t arg) 
	: parent(aParent), type(arg), value(NULL) {

}

const Opcode *OpcodeArgument::getParent() const {
	return parent;
}

uint8_t OpcodeArgument::getType() const {
	return type;
}

word_t OpcodeArgument::getValue() const {
	if (value) {
		return *value;
	}

	return 0;
}

OpcodeArgument &OpcodeArgument::operator=(word_t value) {
	if (isLValue()) {
		*this->value = value;
	}

	return *this;
}

bool OpcodeArgument::isLValue() const {
	return value && type < arguments::NEXT_WORD;
}

void OpcodeArgument::resolve(DCPU &cpu) {
	switch (type) {
	REGISTER_CASES(arguments::A)
		value = cpu.getRegisterPtr(type);
		break;
	REGISTER_CASES(arguments::PTR_A)
		{
			uint8_t regType = type - arguments::PTR_A;

			value = cpu.getMemoryPtr(cpu.getRegisterValue(regType));
		}
		break;
	REGISTER_CASES(arguments::PTR_OFFSET_A)
		{
			uint8_t regType = type - arguments::PTR_OFFSET_A;

			value = cpu.getMemoryPtr(cpu.getRegisterValue(regType) 
				+ cpu.getNextWord());
		}
		break;
	case arguments::POP:
		value = cpu.popStack();
		break;
	case arguments::PEEK:
		value = cpu.peekStack();
		break;
	case arguments::PUSH:
		value = cpu.pushStack();
		break;
	case arguments::SP:
		value = &cpu.sp;
		break;
	case arguments::PC:
		value = &cpu.pc;
		break;
	case arguments::O:
		value = &cpu.o;
		break;
	case arguments::PTR_NEXT_WORD:
		value = cpu.getMemoryPtr(cpu.getNextWord());
		break;
	case arguments::NEXT_WORD:
		value = cpu.getNextWordPtr();
		break;
	default:
		if (type > arguments::LITERAL_END) {
			throw logic_error(str(boost::format("%04x: Invalid argument type %#x") 
				% (parent->getLocation()) % type));
		}
		value = &literals[type - arguments::LITERAL_START];
		break;
	}
}

bool OpcodeArgument::isNextWordUsed() const {
	return (type >= arguments::PTR_OFFSET_A && type <= arguments::PTR_OFFSET_J)
		|| type == arguments::PTR_NEXT_WORD || type == arguments::NEXT_WORD;
}

Opcode::Opcode(word_t instruction, word_t location) {
	this->location = location;
	this->opcode = instruction & 0x000f;
	uint8_t a_type = (instruction >> 4) & 0x003f;
	uint8_t b_type = (instruction >> 10) & 0x003f;

	if (this->opcode == opcodes::NON_BASIC) {
		nonBasic = true;
		opcode = a_type;

		a = new OpcodeArgument(this, b_type);
		b = NULL;
		
		size = 1 + a->isNextWordUsed();
	} else {
		nonBasic = false;

		a = new OpcodeArgument(this, a_type);
		b = new OpcodeArgument(this, b_type);

		size = 1 + a->isNextWordUsed() + b->isNextWordUsed();
	}
}

Opcode::~Opcode() {
	if (a) {
		delete a;
	}

	if (b) {
		delete b;
	}
}

bool Opcode::isNonBasic() const {
	return nonBasic;
}

word_t Opcode::getLocation() const {
	return location;
}

uint8_t Opcode::getSize() const {
	return size;
}

uint8_t Opcode::getType() const {
	return opcode;
}

OpcodeArgument *Opcode::getA() {
	return a;
}

OpcodeArgument *Opcode::getB() {
	return b;
}