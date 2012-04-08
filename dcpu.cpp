#include <inttypes.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <iomanip>

#include "dcpu.h"

using namespace std;

static word_t literals[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
	0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
	0x1e, 0x1f
};

void OpcodeArgument::resolve(DCPU &cpu) {
	switch (type) {
	case arg_a: case arg_b: case arg_c: case arg_x:
	case arg_y: case arg_z: case arg_i: case arg_j:
		value = cpu.getRegisterPtr(type);
		break;
	case arg_ptr_a: case arg_ptr_b: case arg_ptr_c: case arg_ptr_x:
	case arg_ptr_y: case arg_ptr_z: case arg_ptr_i: case arg_ptr_j:
		{
			uint8_t regType = type - arg_ptr_a;
			word_t regValue = cpu.getRegisterValue(regType);

			value = cpu.getMemoryPtr(regValue);
		}
		break;
	case arg_ptr_offset_a: case arg_ptr_offset_b: case arg_ptr_offset_c:
	case arg_ptr_offset_x: case arg_ptr_offset_y: case arg_ptr_offset_z:
	case arg_ptr_offset_i: case arg_ptr_offset_j:
		{
			uint8_t regType = type - arg_ptr_offset_a;
			word_t regValue = cpu.getRegisterValue(regType);
			word_t nextWord = cpu.getNextWord();

			value = cpu.getMemoryPtr(regValue + nextWord);
		}
		break;
	case arg_pop:
		value = cpu.popStack();
		break;
	case arg_peek:
		value = cpu.peekStack();
		break;
	case arg_push:
		value = cpu.pushStack();
		break;
	case arg_sp:
		value = &cpu.sp;
		break;
	case arg_pc:
		value = &cpu.pc;
		break;
	case arg_o:
		value = &cpu.o;
		break;
	case arg_ptr_next_word:
		{
			word_t nextWord = cpu.getNextWord();
			value = cpu.getMemoryPtr(nextWord);
		}
		break;
	case arg_next_word:
		value = cpu.getNextWordPtr();
		break;
	default:
		if (type > 0x3f) {
			throw logic_error("Invalid argument literal");
		}
		value = &literals[type - arg_literal];
		break;
	}
}

bool OpcodeArgument::isNextWordUsed() const {
	return (type >= arg_ptr_offset_a && type <= arg_ptr_offset_j)
		|| type == arg_ptr_next_word || type == arg_next_word;
}

Opcode::Opcode(word_t instruction) {

	opcode = (OpcodeType)(instruction & 0x000f);
	a.setType((instruction >> 4) & 0x003f);
	b.setType((instruction >> 10) & 0x003f);

	if (opcode == op_non_basic) {
		size = 1 + b.isNextWordUsed();
	} else {
		size = 1 + a.isNextWordUsed() + b.isNextWordUsed();
	}
}

DCPU::DCPU() {

}

word_t DCPU::getRegisterValue(uint8_t type) const {
	return registers[type];
}

word_t *DCPU::getRegisterPtr(uint8_t type) {
	return registers + type;
}

word_t DCPU::getMemoryValue(word_t address) const {
	return memory[address];
}

word_t *DCPU::getMemoryPtr(word_t address) {
	return memory + address;
}

word_t DCPU::getNextWord() {
	return *getNextWordPtr();
}

word_t *DCPU::getNextWordPtr() {
	addCycles();
	return getMemoryPtr(pc++);	
}

word_t *DCPU::popStack() {
	return getMemoryPtr(sp++);
}

word_t *DCPU::peekStack() {
	return getMemoryPtr(sp);
}

word_t *DCPU::pushStack() {
	return getMemoryPtr(--sp);
}

void DCPU::pushStack(word_t value) {
	word_t *stackHead = pushStack();
	*stackHead = value;
}

uint32_t DCPU::getCycles() {
	return cycles;
}

void DCPU::addCycles(word_t amount) {
	cycles += amount;
}

void DCPU::dump(ostream& out) const {
	out << "Cycles: " << cycles << endl
		<< "======= Registers =======" << endl
		<< hex << setfill('0') << "A: " << setw(4) << registers[0]
		<< "  B: " << setw(4) << registers[1]
		<< "  C: " << setw(4) << registers[2]
		<< "  X: " << setw(4) << registers[3]
		<< "  Y: " << setw(4) << registers[4]
		<< "  Z: " << setw(4) << registers[5] << endl
		<< "I: " << setw(4) << registers[6]
		<< "  J: " << setw(4) << registers[7]
		<< " SP: " << setw(4) << sp
		<< " PC: " << setw(4) << pc
		<< "  O: " << setw(4) << o << endl
		<< "======= Memory =======" << endl;
	for (int i = 0; i < TOTAL_MEMORY; i += 8) {
		if (!memory[i+0] && !memory[i+1] && !memory[i+2]
			&& !memory[i+3] && !memory[i+4] && !memory[i+5]
			&& !memory[i+6] && !memory[i+7]) {
			continue;
		}

		out << setw(4) << i << ": "
			<< setw(4) << memory[i] << " "
			<< setw(4) << memory[i+1] << " "
			<< setw(4) << memory[i+2] << " "
			<< setw(4) << memory[i+3] << " "
			<< setw(4) << memory[i+4] << " "
			<< setw(4) << memory[i+5] << " "
			<< setw(4) << memory[i+6] << " "
			<< setw(4) << memory[i+7] << " " << endl;
	}
}

size_t DCPU::load(const char *filename) {
	pc = 0;

	ifstream file;
	
	file.open(filename);
	if (!file) {
		throw runtime_error("Failed to load program");
	}

	file.read((char*)memory, TOTAL_MEMORY * sizeof(word_t));
	size_t size =  file.gcount() / 2;
	file.close();

	return size;
}

void DCPU::execute(OpcodeHandler* handler) {
	while (!halted) {
		Opcode opcode(getNextWord());
		handler->execute(*this, opcode);
	}
}

void DCPU::skipNext() {
	Opcode opcode(getMemoryValue(pc));

	pc += opcode.getSize();
	addCycles(1);
}

void DCPU::halt() {
	halted = true;
}
