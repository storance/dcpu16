#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "dcpu.h"

#define ARG_BUF_SIZE 32

static FILE *output = NULL;

static char* NAMES[TOTAL_REGISTERS] = {
	"A", "B", "C", "X",
	"Y", "Z", "I", "J"
};

void format_argument(dcpu_t *cpu, argument_t *arg, char *buffer,
	size_t bufsize) {
	switch (arg->type) {
	case ARG_A: case ARG_B: case ARG_C:
	case ARG_X: case ARG_Y: case ARG_Z:
	case ARG_I: case ARG_J:
		snprintf(buffer, bufsize, "%s", NAMES[arg->type]);
		break;
	case ARG_DEREF_A: case ARG_DEREF_B:
	case ARG_DEREF_C: case ARG_DEREF_X:
	case ARG_DEREF_Y: case ARG_DEREF_Z:
	case ARG_DEREF_I: case ARG_DEREF_J:
		{
			uint8_t register_code = arg->type - ARG_DEREF_A;
			snprintf(buffer, bufsize, "[%s]", NAMES[register_code]);
		}
		break;
	case ARG_DEREF_NEXT_WORD_A: case ARG_DEREF_NEXT_WORD_B:
	case ARG_DEREF_NEXT_WORD_C: case ARG_DEREF_NEXT_WORD_X:
	case ARG_DEREF_NEXT_WORD_Y: case ARG_DEREF_NEXT_WORD_Z:
	case ARG_DEREF_NEXT_WORD_I: case ARG_DEREF_NEXT_WORD_J:
		{
			uint8_t register_code = arg->type - ARG_DEREF_NEXT_WORD_A;
			word_t next_word = dcpu_read_next_word(cpu);
			snprintf(buffer, bufsize, "[0x%04x + %s]", next_word,
				NAMES[register_code]);
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
	case ARG_SP:
		snprintf(buffer, bufsize, "SP");
		break;
	case ARG_PC:
		snprintf(buffer, bufsize, "PC");
		break;
	case ARG_O:
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
			arg->value = dcpu_read_next_word(cpu);
			snprintf(buffer, bufsize, "0x%04x", arg->value);
		}
		break;
	default:
		{
			arg->value = arg->type - ARG_LITERAL_START;
			snprintf(buffer, bufsize, "0x%04x", arg->value);
		}
		break;
	}
}

void print_opcode(dcpu_t *cpu, const char *mnemonic, argument_t *a,
	argument_t *b) {
	
	char bufA[ARG_BUF_SIZE], bufB[ARG_BUF_SIZE];
	format_argument(cpu, a, bufA, ARG_BUF_SIZE);
	format_argument(cpu, b, bufB, ARG_BUF_SIZE);

	fprintf(output, "%s %s, %s\n", mnemonic, bufA, bufB);
}

uint8_t handle_non_basic(dcpu_t *cpu, argument_t* a, argument_t* b) {
	uint8_t opcode = a->type;

	if (opcode >= TOTAL_NON_BASIC_OPCODES) {
		fprintf(stderr, "Unknown Non-basic opcode 0x%x\n", opcode);
		return S_INVALID_OPCODE;
	}

	opcode_handler_t *handler = cpu->non_basic + opcode;
	return handler->handler(cpu, b, NULL);
}

uint8_t handle_set(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "SET", a, b);
	return S_CONTINUE;
}

uint8_t handle_add(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "ADD", a, b);
	return S_CONTINUE;
}

uint8_t handle_sub(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "SUB", a, b);
	return S_CONTINUE;
}

uint8_t handle_mul(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "MUL", a, b);
	return S_CONTINUE;
}

uint8_t handle_div(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "DIV", a, b);
	return S_CONTINUE;
}

uint8_t handle_mod(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "MOD", a, b);
	return S_CONTINUE;
}

uint8_t handle_shl(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "SHL", a, b);
	return S_CONTINUE;
}

uint8_t handle_shr(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "SHR", a, b);
	return S_CONTINUE;
}

uint8_t handle_and(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "AND", a, b);
	return S_CONTINUE;
}

uint8_t handle_or(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "BOR", a, b);
	return S_CONTINUE;
}

uint8_t handle_xor(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "XOR", a, b);
	return S_CONTINUE;
}

uint8_t handle_ife(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "IFE", a, b);
	return S_CONTINUE;
}

uint8_t handle_ifn(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "IFN", a, b);
	return S_CONTINUE;
}

uint8_t handle_ifg(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "IFG", a, b);
	return S_CONTINUE;
}

uint8_t handle_ifb(dcpu_t *cpu, argument_t* a, argument_t* b) {
	print_opcode(cpu, "IFB", a, b);
	return S_CONTINUE;
}

uint8_t handle_jsr(dcpu_t *cpu, argument_t* a, argument_t* b) {
	char buf[ARG_BUF_SIZE];
	format_argument(cpu, a, buf, ARG_BUF_SIZE);

	fprintf(output, "JSR %s\n", buf);

	return S_CONTINUE;
}

uint8_t handle_reserved(dcpu_t *cpu, argument_t* a, argument_t* b){
	fprintf(stderr, "Non Basic Opcode 0x0 is reserved.\n");

	return S_INVALID_OPCODE;
}

void initialize_opcode(opcode_handler_t *handler, op_handler_fn handelr_fn) {
	handler->handler = handelr_fn;
	handler->resolve_args = false;
}

void initialize_handlers(dcpu_t* cpu) {
	initialize_opcode(cpu->basic + OP_NON_BASIC, &handle_non_basic);
	initialize_opcode(cpu->basic + OP_SET, &handle_set);
	initialize_opcode(cpu->basic + OP_ADD, &handle_add);
	initialize_opcode(cpu->basic + OP_SUB, &handle_sub);
	initialize_opcode(cpu->basic + OP_MUL, &handle_mul);
	initialize_opcode(cpu->basic + OP_DIV, &handle_div);
	initialize_opcode(cpu->basic + OP_MOD, &handle_mod);
	initialize_opcode(cpu->basic + OP_SHL, &handle_shl);
	initialize_opcode(cpu->basic + OP_SHR, &handle_shr);
	initialize_opcode(cpu->basic + OP_AND, &handle_and);
	initialize_opcode(cpu->basic + OP_BOR, &handle_or);
	initialize_opcode(cpu->basic + OP_XOR, &handle_xor);
	initialize_opcode(cpu->basic + OP_IFE, &handle_ife);
	initialize_opcode(cpu->basic + OP_IFN, &handle_ifn);
	initialize_opcode(cpu->basic + OP_IFG, &handle_ifg);
	initialize_opcode(cpu->basic + OP_IFB, &handle_ifb);

	initialize_opcode(cpu->non_basic + 0, &handle_reserved);
	initialize_opcode(cpu->non_basic + OP_JSR, &handle_jsr);
}

int main(int argc, char **argv) {
	dcpu_t cpu = {0};
	initialize_handlers(&cpu);

	if (argc < 2) {
		printf("Usage: %s </path/to/dcpu/program> [</path/to/output>]\n",
			argv[0]);
		return 1;
	}

	if (argc >= 3) {
		output = fopen(argv[2], "w");
		if (!output) {
			printf("Unable to open %s for write: %s\n", argv[2],
				strerror(errno));
			return 1;
		}
	} else {
		output = stdout;
	}

	size_t size;
	if (!dcpu_load(&cpu, argv[1], &size)) {
		printf("Unable to load program %s\n", argv[1]);
		return 1;
	}	

	dcpu_execute(&cpu, size);
}
