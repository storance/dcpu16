#include <inttypes.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cerrno>
#include <exception>
#include <stdexcept>
#include <iomanip>

#include <boost/format.hpp>

#include "dcpu.hpp"

using namespace std;

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
		throw runtime_error(str(boost::format("Failed to open the file %s: %s") 
				% filename % strerror(errno)));
	}

	size_t index = 0;
	while (file) {
		uint8_t b1 = file.get();
		uint8_t b2 = file.get();
		if (file.bad()) {
			throw runtime_error(str(boost::format("Failed to read the next "
				"word from the file %s: %s") % filename 
				% strerror(errno)));
		}

		memory[index++] = (b1 << 8) | b2;
	}
	file.close();

	return index;
}

void DCPU::execute(OpcodeHandler* handler) {
	while (!halted) {
		Opcode opcode(getNextWord(), pc-1);
		handler->execute(*this, opcode);
	}
}

void DCPU::skipNext() {
	Opcode opcode(getMemoryValue(pc), pc);

	pc += opcode.getSize();
	addCycles(1);
}

void DCPU::halt() {
	halted = true;
}
