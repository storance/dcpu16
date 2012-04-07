#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>


#include "dcpu.h"
#include "opcodes.h"

#define ARG_BUF_SIZE 32

static char* REGISTER_NAMES[TOTAL_REGISTERS] = {"A", "B", "C", "X", "Y", "Z", "I", "J"};

void format_argument(dcpu_t *cpu, const argument_t *arg, char *buffer, size_t bufsize) {
	switch (arg->arg_value) {
	case ARG_REGISTER_A: case ARG_REGISTER_B: case ARG_REGISTER_C:
	case ARG_REGISTER_X: case ARG_REGISTER_Y: case ARG_REGISTER_Z:
	case ARG_REGISTER_I: case ARG_REGISTER_J:
		snprintf(buffer, bufsize, "%s", REGISTER_NAMES[arg->arg_value]);
		break;
	case ARG_DEREF_REGISTER_A: case ARG_DEREF_REGISTER_B:
	case ARG_DEREF_REGISTER_C: case ARG_DEREF_REGISTER_X:
	case ARG_DEREF_REGISTER_Y: case ARG_DEREF_REGISTER_Z:
	case ARG_DEREF_REGISTER_I: case ARG_DEREF_REGISTER_J:
		{
			uint8_t register_code = arg->arg_value - ARG_DEREF_REGISTER_A;
			snprintf(buffer, bufsize, "[%s]", REGISTER_NAMES[register_code]);
		}
		break;
	case ARG_DEREF_NEXT_WORD_REGISTER_A: case ARG_DEREF_NEXT_WORD_REGISTER_B:
	case ARG_DEREF_NEXT_WORD_REGISTER_C: case ARG_DEREF_NEXT_WORD_REGISTER_X:
	case ARG_DEREF_NEXT_WORD_REGISTER_Y: case ARG_DEREF_NEXT_WORD_REGISTER_Z:
	case ARG_DEREF_NEXT_WORD_REGISTER_I: case ARG_DEREF_NEXT_WORD_REGISTER_J:
		{
			uint8_t register_code = arg->arg_value - ARG_DEREF_NEXT_WORD_REGISTER_A;
			word_t next_word = dcpu_read_next_word(cpu);
			snprintf(buffer, bufsize, "[0x%04x + %s]", next_word,
				REGISTER_NAMES[register_code]);
		}
		break;
	case ARG_POP:
		snprintf(buffer, bufsize, "POP");
		break;
	case ARG_PEEK:
		snprintf(buffer, bufsize, "PEEK");
		break;
	case ARG_PUSH:
		snprintf(buffer, bufsize, "PUSH");
		break;
	case ARG_REGISTER_SP:
		snprintf(buffer, bufsize, "SP");
		break;
	case ARG_REGISTER_PC:
		snprintf(buffer, bufsize, "PC");
		break;
	case ARG_REGISTER_O:
		snprintf(buffer, bufsize, "O");
		break;
	case ARG_DEREF_NEXT_WORD:
		{
			word_t next_word = dcpu_read_next_word(cpu);
			snprintf(buffer, bufsize, "[0x%04x]", next_word);
		}
		break;
	case ARG_NEXT_WORD:
		{
			word_t next_word = dcpu_read_next_word(cpu);
			snprintf(buffer, bufsize, "0x%04x", next_word);
		}
		break;
	default:
		{
			word_t literal = arg->arg_value - ARG_LITERAL_START;
			snprintf(buffer, bufsize, "0x%04x", literal);
		}
		break;
	}
}

void print_opcode(dcpu_t *cpu, const char *mnemonic, argument_t *a, argument_t *b) {
	
	char bufA[ARG_BUF_SIZE], bufB[ARG_BUF_SIZE];
	format_argument(cpu, a, bufA, ARG_BUF_SIZE);
	format_argument(cpu, b, bufB, ARG_BUF_SIZE);

	printf("%s %s, %s\n", mnemonic, bufA, bufB);
}

void handle_non_basic_opcode(dcpu_t *cpu, argument_t* a, argument_t* b) {
	uint8_t opcode = a->arg_value;

	if (opcode >= TOTAL_NON_BASIC_OPCODES) {
		printf(";Reserved non-basic opcode 0x%x\n", opcode);
		return;
	}

	opcode_handler_t *handler = cpu->non_basic + opcode;
	handler->handler(cpu, b, NULL);
}

void handle_set(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "SET", a, b);
}

void handle_add(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "ADD", a, b);
}

void handle_sub(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "SUB", a, b);
}

void handle_mul(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "MUL", a, b);
}

void handle_div(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "DIV", a, b);
}

void handle_mod(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "MOD", a, b);
}

void handle_shl(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "SHL", a, b);
}

void handle_shr(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "SHR", a, b);
}

void handle_and(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "AND", a, b);
}

void handle_or(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "BOR", a, b);
}

void handle_xor(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "XOR", a, b);
}

void handle_ife(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "IFE", a, b);
}

void handle_ifn(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "IFN", a, b);
}

void handle_ifg(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "IFG", a, b);
}

void handle_ifb(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "IFB", a, b);
}

void handle_jsr(dcpu_t *cpu, argument_t* a, argument_t* b) {
	char buf[ARG_BUF_SIZE];
	format_argument(cpu, a, buf, ARG_BUF_SIZE);

	printf("JSR %s\n", buf);
}

void handle_reserved(dcpu_t *cpu, argument_t* a, argument_t* b){
	printf(";Reserved non-basic opcode 0x0\n");
}

void initialize_opcode(opcode_handler_t *handler, op_handler_fn handelr_fn, bool resolve_args) {
	handler->handler = handelr_fn;
	handler->resolve_args = resolve_args;
}

void initialize_handlers(dcpu_t* cpu) {
	initialize_opcode(cpu->basic + OP_NON_BASIC, &handle_non_basic_opcode, false);
	initialize_opcode(cpu->basic + OP_SET, &handle_set, false);
	initialize_opcode(cpu->basic + OP_ADD, &handle_add, false);
	initialize_opcode(cpu->basic + OP_SUB, &handle_sub, false);
	initialize_opcode(cpu->basic + OP_MUL, &handle_mul, false);
	initialize_opcode(cpu->basic + OP_DIV, &handle_div, false);
	initialize_opcode(cpu->basic + OP_MOD, &handle_mod, false);
	initialize_opcode(cpu->basic + OP_SHL, &handle_shl, false);
	initialize_opcode(cpu->basic + OP_SHR, &handle_shr, false);
	initialize_opcode(cpu->basic + OP_AND, &handle_and, false);
	initialize_opcode(cpu->basic + OP_BOR, &handle_or,  false);
	initialize_opcode(cpu->basic + OP_XOR, &handle_xor, false);
	initialize_opcode(cpu->basic + OP_IFE, &handle_ife, false);
	initialize_opcode(cpu->basic + OP_IFN, &handle_ifn, false);
	initialize_opcode(cpu->basic + OP_IFG, &handle_ifg, false);
	initialize_opcode(cpu->basic + OP_IFB, &handle_ifb, false);

	initialize_opcode(cpu->non_basic + 0, &handle_reserved, false);
	initialize_opcode(cpu->non_basic + OP_JSR, &handle_jsr, false);
}

void dump_cpu(dcpu_t *cpu) {

}
