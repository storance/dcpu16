#pragma once

#include "common.hpp"

class DCPU;
class Opcode;

class OpcodeArgument {
private:
	Opcode *parent;
	uint8_t type;
	word_t *value;
public:
	OpcodeArgument(Opcode *aParent, uint8_t arg);

	const Opcode *getParent() const;
	uint8_t getType() const;

	bool isLValue() const;
	bool isNextWordUsed() const;
	void resolve(DCPU &cpu);

	word_t getValue() const;
	OpcodeArgument &operator=(word_t value);

};

class Opcode {
private:
	word_t location;
	uint8_t size;
	uint8_t opcode;
	bool nonBasic;
	OpcodeArgument *a;
	OpcodeArgument *b;
public:
	Opcode(word_t, word_t);
	virtual ~Opcode();

	bool isNonBasic() const;
	word_t getLocation() const;
	uint8_t getSize() const;
	uint8_t getType() const;
	OpcodeArgument *getA();
	OpcodeArgument *getB();
};
