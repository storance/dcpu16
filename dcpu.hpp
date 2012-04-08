#pragma once

#include <iostream>

#include "common.hpp"
#include "opcode.hpp"

class OpcodeHandler {
public:
	virtual void execute(DCPU&, Opcode &)=0;
};

class DCPU {
private:
	uint32_t cycles;
	bool halted;
public:
	word_t memory[TOTAL_MEMORY];
	word_t registers[TOTAL_GENERAL_PURPOSE_REGISTERS];
	word_t pc;
	word_t sp;
	word_t o;

	DCPU();

	word_t* getRegisterPtr(uint8_t);
	word_t getRegisterValue(uint8_t) const;

	word_t* getMemoryPtr(word_t);
	word_t getMemoryValue(word_t) const;
	
	word_t getNextWord();
	word_t *getNextWordPtr();

	word_t *popStack();
	word_t *peekStack();
	word_t *pushStack();
	void pushStack(word_t value);

	void addCycles(word_t amount=1);
	uint32_t getCycles();

	size_t load(const char *filename);
	void execute(OpcodeHandler*);
	void dump(std::ostream&) const;

	void skipNext();
	void halt();
};
