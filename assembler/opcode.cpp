#include <inttypes.h>
#include <iostream>
#include <iomanip>

#include "opcode.hpp"

using namespace std;

Opcode::Opcode(const location_t &location, uint8_t opcode, bool nonBasic,
	Argument *a, Argument *b) : location(location), opcode(opcode),
	nonBasic(nonBasic), a(a), b(b) {

}

Opcode *Opcode::createBasic(const location_t &line, uint8_t opcode,
	Argument *a, Argument *b) {

	return new Opcode(line, opcode, false, a, b);
}

Opcode *Opcode::createNonBasic(const location_t &line, uint8_t opcode,
	Argument *a) {

	return new Opcode(line, opcode, true, a, NULL);
}

uint8_t Opcode::getSize() {
	if (nonBasic) {
		return 1 + a->isNextWordRequired();
	} else {
		return 1 + a->isNextWordRequired() + b->isNextWordRequired();
	}
}

void Opcode::resolveLabels(SymbolTable &table) {
	a->resolveLabels(table);
	if (b) {
		b->resolveLabels(table);
	}
}

void Opcode::encode(vector<word_t> &output) {
	word_t instruction = 0;

	if (nonBasic) {
		instruction = (opcode << 4) | (a->encode() << 10);
	} else {
		instruction = opcode | (a->encode() << 4) | (b->encode() << 10);
	}

	output.push_back(instruction);
	if (a->isNextWordRequired()) {
		output.push_back(a->getNextWord());
	}

	if (b && b->isNextWordRequired()) {
		output.push_back(b->getNextWord());
	}
}