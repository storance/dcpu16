#pragma once

#include <vector>

#include "argument.hpp"
#include "assembler.hpp"

class Opcode  {
protected:
	location_t location;
	bool nonBasic;
	uint8_t opcode;
	Argument *a;
	Argument *b;

	Opcode(const location_t&, uint8_t, bool, Argument *, Argument*);
public:
	static Opcode *createBasic(const location_t&, uint8_t, Argument*,
		Argument*);
	static Opcode *createNonBasic(const location_t&, uint8_t, Argument*);

	uint8_t getSize();
	void resolveLabels(SymbolTable &);
	void encode(std::vector<word_t> &);
};