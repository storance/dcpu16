#include <inttypes.h>
#include <cassert>

#include "argument.hpp"

using namespace std;

/*==================================================================
 *
 * Argument
 * 
 *==================================================================*/

Argument::Argument(location_t location, bool nextWordRequired) 
	: location(location), nextWordRequired(nextWordRequired),
	labelsResolved(false) {

}

word_t Argument::getNextWord() {
	assert(labelsResolved);

	return 0;
}

bool Argument::isNextWordRequired() {
	return nextWordRequired;
}

void Argument::resolveLabels(SymbolTable &table) {
	labelsResolved = true;
}

/*==================================================================
 *
 * RegisterArgument
 * 
 *==================================================================*/


RegisterArgument::RegisterArgument(location_t location, bool nextWordRequired,
	uint8_t registerType) : Argument (location, nextWordRequired),
	registerType(registerType) {

}

RegisterArgument::RegisterArgument(location_t location, uint8_t registerType) 
	: Argument(location, false), registerType(registerType) {

}

uint8_t RegisterArgument::encode() {
	assert(labelsResolved);

	switch (registerType) {
	case registers::A:
		return arguments::A;
	case registers::B:
		return arguments::B;
	case registers::C:
		return arguments::C;
	case registers::X:
		return arguments::X;
	case registers::Y:
		return arguments::Y;
	case registers::Z:
		return arguments::Z;
	case registers::I:
		return arguments::I;
	case registers::J:
		return arguments::J;
	case registers::SP:
		return arguments::SP;
	case registers::PC:
		return arguments::PC;
	case registers::O:
		return arguments::O;
	default:
		// throw exception
		break;
	}
}

/*==================================================================
 *
 * RegisterPtrArgument
 * 
 *==================================================================*/


RegisterPtrArgument::RegisterPtrArgument(location_t location, uint8_t registerType)
	: RegisterArgument(location, registerType) {

}

uint8_t RegisterPtrArgument::encode() {
	assert(labelsResolved);

	switch (registerType) {
	case registers::A:
		return arguments::PTR_A;
	case registers::B:
		return arguments::PTR_B;
	case registers::C:
		return arguments::PTR_C;
	case registers::X:
		return arguments::PTR_X;
	case registers::Y:
		return arguments::PTR_Y;
	case registers::Z:
		return arguments::PTR_Z;
	case registers::I:
		return arguments::PTR_I;
	case registers::J:
		return arguments::PTR_J;
	case registers::SP:
		return arguments::PEEK;
	default:
		// throw exception
		break;
	}
}

/*==================================================================
 *
 * RegisterOffsetPtrArgument
 * 
 *==================================================================*/

RegisterOffsetPtrArgument::RegisterOffsetPtrArgument(location_t location,
	uint8_t registerType, word_t offset) : RegisterArgument(location, true,
	registerType), offset(offset) {

}

uint8_t RegisterOffsetPtrArgument::encode() {
	assert(labelsResolved);

	switch (registerType) {
	case registers::A:
		return arguments::PTR_OFFSET_A;
	case registers::B:
		return arguments::PTR_OFFSET_B;
	case registers::C:
		return arguments::PTR_OFFSET_C;
	case registers::X:
		return arguments::PTR_OFFSET_X;
	case registers::Y:
		return arguments::PTR_OFFSET_Y;
	case registers::Z:
		return arguments::PTR_OFFSET_Z;
	case registers::I:
		return arguments::PTR_OFFSET_I;
	case registers::J:
		return arguments::PTR_OFFSET_J;
	default:
		// throw exception
		break;
	}
}

word_t RegisterOffsetPtrArgument::getNextWord() {
	assert(labelsResolved);

	return offset;
}

/*==================================================================
 *
 * RegisterLabelPtrArgument
 * 
 *==================================================================*/

RegisterLabelPtrArgument::RegisterLabelPtrArgument(location_t location,
	uint8_t registerType, word_t offset, const std::string &label) 
	: RegisterOffsetPtrArgument(location, registerType, offset), label(label) {

}

void RegisterLabelPtrArgument::resolveLabels(SymbolTable &table) {
	word_t *labelOffset = table.lookup(label);
	if (labelOffset == NULL) {
		// error!
	}

	offset += *labelOffset;
}

/*==================================================================
 *
 * PopStackArgument
 * 
 *==================================================================*/

PopStackArgument::PopStackArgument(location_t location) : Argument(location, false) {

}

uint8_t PopStackArgument::encode() {
	assert(labelsResolved);

	return arguments::POP;
}

/*==================================================================
 *
 * PushStackArgument
 * 
 *==================================================================*/

PushStackArgument::PushStackArgument(location_t location) : Argument(location, false) {

}

uint8_t PushStackArgument::encode() {
	assert(labelsResolved);

	return arguments::PUSH;
}

/*==================================================================
 *
 * LiteralArgument
 * 
 *==================================================================*/

LiteralArgument::LiteralArgument(location_t location, bool nextWordRequired,
	word_t literal) : Argument(location, nextWordRequired), literal(literal) {

}

LiteralArgument::LiteralArgument(location_t location, word_t literal) 
	: Argument(location, literal <= MAX_SHORT_LITERAL), literal(literal) {

}

word_t LiteralArgument::getNextWord() {
	assert(labelsResolved);

	return literal;
}

uint8_t LiteralArgument::encode() {
	assert(labelsResolved);

	if (!nextWordRequired) {
		return arguments::LITERAL_START + literal;
	} else {
		return arguments::NEXT_WORD;
	}
}

/*==================================================================
 *
 * LiteralPtrArgument
 * 
 *==================================================================*/

LiteralPtrArgument::LiteralPtrArgument(location_t location, word_t literal)
	: LiteralArgument(location, true, literal) {

}

uint8_t LiteralPtrArgument::encode() {
	assert(labelsResolved);
	
	return arguments::PTR_NEXT_WORD;
}

/*==================================================================
 *
 * LabelArgument
 * 
 *==================================================================*/

LabelArgument::LabelArgument(location_t location, const std::string &label,
	word_t extra) : LiteralArgument(location, true, extra), label(label) {

}

void LabelArgument::resolveLabels(SymbolTable &table) {
	word_t *labelOffset = table.lookup(label);
	if (labelOffset == NULL) {
		// error!
	}

	literal += *labelOffset;
}

/*==================================================================
 *
 * LabelPtrArgument
 * 
 *==================================================================*/

LabelPtrArgument::LabelPtrArgument(location_t location, const std::string &label,
	word_t extra) : LiteralPtrArgument(location, extra), label(label) {

}

void LabelPtrArgument::resolveLabels(SymbolTable &table) {
	word_t *labelOffset = table.lookup(label);
	if (labelOffset == NULL) {
		// error!
	}

	literal += *labelOffset;
}
