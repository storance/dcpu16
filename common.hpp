#pragma once

typedef uint16_t word_t;

enum {
	TOTAL_GENERAL_PURPOSE_REGISTERS = 8,
	TOTAL_REGISTERS = 11,
	TOTAL_MEMORY = 0x10000,
	MAX_SHORT_LITERAL = 0x1f
};

enum class Register {
	A, B, C, X, Y, Z, I, J, SP, PC, O
};

namespace arguments {
	enum {
		A,
		B,
		C,
		X,
		Y,
		Z,
		I,
		J,
		PTR_A,
		PTR_B,
		PTR_C,
		PTR_X,
		PTR_Y,
		PTR_Z,
		PTR_I,
		PTR_J,
		PTR_OFFSET_A,
		PTR_OFFSET_B,
		PTR_OFFSET_C,
		PTR_OFFSET_X,
		PTR_OFFSET_Y,
		PTR_OFFSET_Z,
		PTR_OFFSET_I,
		PTR_OFFSET_J,
		POP,
		PEEK,
		PUSH,
		SP,
		PC,
		O,
		PTR_NEXT_WORD,
		NEXT_WORD,
		LITERAL_START,
		LITERAL_END=0x3f
	};
}

namespace opcodes {
	enum {
		NON_BASIC,
		SET,
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		SHL,
		SHR,
		AND,
		BOR,
		XOR,
		IFE,
		IFN,
		IFG,
		IFB
	};

	enum {
		RESERVED,
		JSR
	};
}

#define REGISTER_CASES(start) case start: \
case start+1: \
case start+2: \
case start+3: \
case start+4: \
case start+5: \
case start+6: \
case start+7:
