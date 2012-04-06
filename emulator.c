#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "emulator.h"

static word_t literals[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
	0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
	0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

word_t *get_register(dcpu_t *cpu, uint8_t reg) {
	return cpu->registers + reg;
}

word_t *get_memory(dcpu_t *cpu, word_t offset) {
	return cpu->memory + offset;
}

word_t *get_next_word(dcpu_t *cpu) {
	cpu->cycles++;

	return get_memory(cpu, cpu->pc++);
}

word_t *pop_stack(dcpu_t *cpu) {
	return get_memory(cpu, cpu->sp++);
}

word_t *push_stack(dcpu_t *cpu) {
	return get_memory(cpu, --cpu->sp);
}

word_t *peek_stack(dcpu_t *cpu) {
	return get_memory(cpu, cpu->sp);
}

word_t* get_argument(dcpu_t *cpu, word_t code, bool *lvalue) {
	if (lvalue) {
		*lvalue = true;
	}

	if (code >= ARG_REG_START && code <= ARG_REG_END) {
		return get_register(cpu, code);
	} else if (code >= ARG_REF_REG_START && code <= ARG_REF_REG_END) {
		word_t reg_value = *get_register(cpu, code - ARG_REF_REG_START);
		
		return get_memory(cpu, reg_value);
	} else if (code >= ARG_REF_NW_REG_START && code <= ARG_REF_NW_REG_END) {
		word_t reg_value = *get_register(cpu, code - ARG_REF_NW_REG_START);
		word_t next_word = *get_next_word(cpu);
		
		return get_memory(cpu, next_word + reg_value);
	} else if (code == ARG_POP) {
		return pop_stack(cpu);
	} else if (code == ARG_PEEK) {
		return peek_stack(cpu);
	} else if (code == ARG_PUSH) {
		return push_stack(cpu);
	} else if (code == ARG_SP) {
		return &cpu->sp;
	} else if (code == ARG_PC) {
		return &cpu->pc;
	} else if (code == ARG_O) {
		return &cpu->o;
	} else if (code == ARG_REF_NEXT_WORD) {
		word_t next_word = *get_next_word(cpu);

		return get_memory(cpu, next_word);
	} else if (code == ARG_NEXT_WORD) {
		if (lvalue) {
			*lvalue = false;
		}

		return get_next_word(cpu);
	} else if (code >= ARG_LITERAL_START && code <= ARG_LITERAL_END) {
		if (lvalue) {
			*lvalue = false;
		}

		return literals + (code - ARG_LITERAL_START);
	} else {
		if (lvalue) {
			*lvalue = false;
		}

		return NULL;
	}
}

void handle_non_basic_opcode(dcpu_t *cpu, word_t* a, bool lvalue, word_t opcode) {
	nb_op_handler handler = cpu->non_basic[opcode];
	
	printf("Non-basic opcode: %04x\n", opcode);
	if (handler) {
		handler(cpu, a, lvalue);
	}
}

void handle_set(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	if (!lvalue) return;

	*a = b;
}

void handle_add(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	cpu->cycles++;

	word_t result = *a + b;
	cpu->o = result < *a || result < b;

	if (lvalue) *a = result;
}

void handle_sub(dcpu_t *cpu, word_t *a, bool lvalue, word_t b) {
	cpu->cycles++;

	word_t result = *a = b;
	cpu->o = result > *a ? 0xffff : 0;

	if (lvalue) *a = result;
}

void handle_mul(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	cpu->cycles++;
	word_t result = *a * b;

	cpu->o = (result >> 16) & 0xffff;
	if (lvalue) *a = result;
}

void handle_div(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	cpu->cycles += 2;

	word_t result;
	if (b == 0) {
		cpu->o = 0;
		result = 0;
	} else {
		uint32_t result_nocarry = (*a << 16) / b;
		cpu->o = result_nocarry & 0xffff;
		result = (word_t)(result_nocarry >> 16);
	}

	if (lvalue) *a = result;
}

void handle_mod(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	cpu->cycles += 2;
	if (!lvalue) return;

	if (b == 0) {
		*a = 0;
	} else {
		*a = *a % b;
	}
}

void handle_shl(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	cpu->cycles++;

	word_t result = *a << b;
	cpu->o = (result >> 16) & 0xffff;

	if (lvalue) *a = result;
}

void handle_shr(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	cpu->cycles++;

	uint32_t result_nocarry = (*a << 16) >> b; 
	cpu->o = result_nocarry & 0xffff;

	if (lvalue) *a = (word_t)(result_nocarry >> 16);
}

void handle_and(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	if (!lvalue) return;
	*a = *a & b;
}

void handle_or(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	if (!lvalue) return;
	*a = *a | b;
}

void handle_xor(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	if (!lvalue) return;
	*a = *a ^ b;
}

void handle_ife(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	cpu->cycles++;
	cpu->skipNext = *a != b;
}

void handle_ifn(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	cpu->cycles++;
	cpu->skipNext = *a == b;
}

void handle_ifg(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	cpu->cycles++;
	cpu->skipNext = *a <= b;
}

void handle_ifb(dcpu_t *cpu, word_t* a, bool lvalue, word_t b) {
	cpu->cycles++;
	cpu->skipNext = (*a & b) == 0;
}

void handle_jsr(dcpu_t *cpu, word_t* a, bool lvalue) {
	cpu->cycles++;

	word_t *stack = push_stack(cpu);
	*stack = cpu->pc;

	printf("PC: %04x a: %04x\n", cpu->pc, *a);
	cpu->pc = *a;
}

bool is_argument_use_next_word(word_t arg) {
	return arg >= ARG_REF_NW_REG_START && arg <= ARG_REF_NW_REG_END ||
		arg == ARG_REF_NEXT_WORD || arg == ARG_NEXT_WORD;
}

uint8_t calc_argument_size(word_t opcode, word_t arg_a, word_t arg_b) {
	return (opcode > 0 ? is_argument_use_next_word(arg_a) : 0) + 
		is_argument_use_next_word(arg_b);
}

void process_instruction(dcpu_t *cpu, word_t instruction) {
	word_t opcode = instruction & 0x000f;
	word_t arg_a = (instruction >> 4) & 0x003f;
	word_t arg_b = (instruction >> 10) & 0x003f;

	if (cpu->skipNext) {
		cpu->pc += calc_argument_size(opcode, arg_a, arg_b);
		return;
	}

	printf("%04x (%04x, %04x)", opcode, arg_a, arg_b);
	if (opcode == 0) {
		bool lvalue;
		word_t *arg = get_argument(cpu, arg_b, &lvalue);
		word_t nb_opcode = arg_a;

		printf(" [%04x]\n", *arg);

		handle_non_basic_opcode(cpu, arg, lvalue, nb_opcode);
	} else {
		bool lvalue;
		word_t *a = get_argument(cpu, arg_a, &lvalue);
		word_t *b = get_argument(cpu, arg_b, NULL);

		if (!a || !b) {
			return;
		}
		
		printf(" [%04x %04x]\n", *a, *b);
		cpu->basic[opcode](cpu, a, lvalue, *b);
	}
}

void init_dcpu(dcpu_t *cpu) {
	cpu->basic[OP_SET] = &handle_set;
	cpu->basic[OP_ADD] = &handle_add;
	cpu->basic[OP_SUB] = &handle_sub;
	cpu->basic[OP_MUL] = &handle_mul;
	cpu->basic[OP_DIV] = &handle_div;
	cpu->basic[OP_MOD] = &handle_mod;
	cpu->basic[OP_SHL] = &handle_shl;
	cpu->basic[OP_SHR] = &handle_shr;
	cpu->basic[OP_AND] = &handle_and;
	cpu->basic[OP_BOR] = &handle_or;
	cpu->basic[OP_XOR] = &handle_xor;
	cpu->basic[OP_IFE] = &handle_ife;
	cpu->basic[OP_IFN] = &handle_ifn;
	cpu->basic[OP_IFG] = &handle_ifg;
	cpu->basic[OP_IFB] = &handle_ifb;

	cpu->non_basic[OP_JSR] = &handle_jsr;
}

void run_program(dcpu_t *cpu, word_t size) {
	while (cpu->pc <= size) {
		word_t instruction = *get_next_word(cpu);
		process_instruction(cpu, instruction);
	}
}

bool load_program(dcpu_t *cpu, const char* filename, word_t *size) {
	cpu->pc = 0;

	FILE* f = fopen(filename, "r");
	if (!f) {
		return false;
	}

	*size = fread(cpu->memory, sizeof(word_t), MEMORY_SIZE, f);
	fclose(f);

	return true;
}

bool is_line_empty(word_t *memory, int line) {
	for (int i = 0; i < 8; i++) {
		if (memory[line+i] != 0) {
			return false;
		}
	}

	return true;
}

void dump(dcpu_t *cpu) {
	printf("Cycles: %lu\n", cpu->cycles);
	printf("======= Registers =======\n");
	printf("A: %04x  B: %04x  C: %04x  X: %04x  Y: %04x  Z: %04x\n",
		cpu->registers[0], cpu->registers[1], cpu->registers[2],
		cpu->registers[3], cpu->registers[4], cpu->registers[5]);
	printf("I: %04x  J: %04x SP: %04x PC: %04x  O: %04x\n",
		cpu->registers[6], cpu->registers[7], cpu->sp, cpu->pc, cpu->o);

	printf("======= Memory =======\n");
	bool last_line_empty = false;
	for (int i = 0; i < MEMORY_SIZE; i += 8) {
		if (is_line_empty(cpu->memory, i)) {
			if (!last_line_empty) {
				printf ("                     ...\n");
			}
			last_line_empty = true;
		} else {
			last_line_empty = false;
			printf("%04x: %04x %04x %04x %04x %04x %04x %04x %04x\n", i,
				cpu->memory[i+0], cpu->memory[i+1], cpu->memory[i+2],
				cpu->memory[i+3], cpu->memory[i+4], cpu->memory[i+5],
				cpu->memory[i+6], cpu->memory[i+7]);
		}
	}
}

int main(int argc, char **argv) {
	dcpu_t cpu = {0};
	init_dcpu(&cpu);

	if (argc < 2) {
		printf("Usage: emulator </path/to/dcpu/program>\n");
		return 1;
	}

	word_t size;
	if (!load_program(&cpu, argv[1], &size)) {
		printf("Unable to load program %s\n", argv[1]);
		return 1;
	}	

	run_program(&cpu, size);
	dump(&cpu);
}
