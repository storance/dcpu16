#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include "dcpu.h"
#include "opcodes.h"

void store_result(argument_t* a, word_t result) {
	if (a->location) {
		*a->location = result;
	}
}

void handle_non_basic_opcode(dcpu_t *cpu, argument_t* a, argument_t* b) {
	uint8_t opcode = a->arg_value;

	if (opcode >= TOTAL_NON_BASIC_OPCODES) {
		// TODO: handle error
		return;
	}

	opcode_handler_t *handler = cpu->non_basic + opcode;
	
	if (handler->resolve_args) {
		dcpu_resolve_argument(cpu, b);
	}

	handler->handler(cpu, b, NULL);
}

void handle_set(dcpu_t *cpu, argument_t* a, argument_t* b) {
	store_result(a, b->value);
}

void handle_add(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	word_t result = a->value + b->value;
	cpu->o = result < a->value || result < b->value;

	store_result(a, result);
}

void handle_sub(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	word_t result = a->value - b->value;
	cpu->o = result > a->value ? 0xffff : 0;

	store_result(a, result);
}

void handle_mul(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	word_t result = a->value * b->value;
	cpu->o = (result >> 16) & 0xffff;

	store_result(a, result);
}

void handle_div(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles += 2;

	word_t result;
	if (b->value == 0) {
		cpu->o = 0;
		result = 0;
	} else {
		uint32_t result_nocarry = (a->value << 16) / b->value;
		cpu->o = result_nocarry & 0xffff;
		result = (word_t)(result_nocarry >> 16);
	}

	store_result(a, result);
}

void handle_mod(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles += 2;

	word_t result;
	if (b->value == 0) {
		result = 0;
	} else {
		result = a->value % b->value;
	}

	store_result(a, result);
}

void handle_shl(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	word_t result = a->value << b->value;
	cpu->o = (result >> 16) & 0xffff;

	store_result(a, result);
}

void handle_shr(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	uint32_t result_nocarry = (a->value << 16) >> b->value; 
	cpu->o = result_nocarry & 0xffff;

	store_result(a, (word_t)(result_nocarry >> 16));
}

void handle_and(dcpu_t *cpu, argument_t* a, argument_t* b) {
	store_result(a, a->value & b->value);
}

void handle_or(dcpu_t *cpu, argument_t* a, argument_t* b) {
	store_result(a, a->value | b->value);
}

void handle_xor(dcpu_t *cpu, argument_t* a, argument_t* b) {
	store_result(a, a->value ^ b->value);
}

void handle_ife(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;
	if (a->value != b->value) dcpu_skip_next(cpu);
}

void handle_ifn(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;
	if (a->value == b->value) dcpu_skip_next(cpu);
}

void handle_ifg(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;
	if (a->value <= b->value) dcpu_skip_next(cpu);
}

void handle_ifb(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;
	if ((a->value & b->value) == 0) dcpu_skip_next(cpu);
}

void handle_jsr(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	word_t *stack = dcpu_get_memory(cpu, --cpu->sp);

	*stack = cpu->pc;
	cpu->pc = a->value;
}

void handle_reserved(dcpu_t *cpu, argument_t* a, argument_t* b){
	// TODO: handle error
}

void initialize_opcode(opcode_handler_t *handler, op_handler_fn handelr_fn, bool resolve_args) {
	handler->handler = handelr_fn;
	handler->resolve_args = resolve_args;
}

void initialize_handlers(dcpu_t* cpu) {
	initialize_opcode(cpu->basic + OP_NON_BASIC, &handle_non_basic_opcode, false);
	initialize_opcode(cpu->basic + OP_SET, &handle_set, true);
	initialize_opcode(cpu->basic + OP_ADD, &handle_add, true);
	initialize_opcode(cpu->basic + OP_SUB, &handle_sub, true);
	initialize_opcode(cpu->basic + OP_MUL, &handle_mul, true);
	initialize_opcode(cpu->basic + OP_DIV, &handle_div, true);
	initialize_opcode(cpu->basic + OP_MOD, &handle_mod, true);
	initialize_opcode(cpu->basic + OP_SHL, &handle_shl, true);
	initialize_opcode(cpu->basic + OP_SHR, &handle_shr, true);
	initialize_opcode(cpu->basic + OP_AND, &handle_and, true);
	initialize_opcode(cpu->basic + OP_BOR, &handle_or, true);
	initialize_opcode(cpu->basic + OP_XOR, &handle_xor, true);
	initialize_opcode(cpu->basic + OP_IFE, &handle_ife, true);
	initialize_opcode(cpu->basic + OP_IFN, &handle_ifn, true);
	initialize_opcode(cpu->basic + OP_IFG, &handle_ifg, true);
	initialize_opcode(cpu->basic + OP_IFB, &handle_ifb, true);

	initialize_opcode(cpu->non_basic + 0, &handle_reserved, true);
	initialize_opcode(cpu->non_basic + OP_JSR, &handle_jsr, true);
}

bool is_line_empty(word_t *memory, int line) {
	for (int i = 0; i < 8; i++) {
		if (memory[line+i] != 0) {
			return false;
		}
	}

	return true;
}

void dump_cpu(dcpu_t *cpu) {
	printf("Cycles: %lu\n", cpu->cycles);
	printf("======= Registers =======\n");
	printf("A: %04x  B: %04x  C: %04x  X: %04x  Y: %04x  Z: %04x\n",
		cpu->registers[0], cpu->registers[1], cpu->registers[2],
		cpu->registers[3], cpu->registers[4], cpu->registers[5]);
	printf("I: %04x  J: %04x SP: %04x PC: %04x  O: %04x\n",
		cpu->registers[6], cpu->registers[7], cpu->sp, cpu->pc, cpu->o);

	printf("======= Memory =======\n");
	bool last_line_empty = false;
	for (int i = 0; i < TOTAL_MEMORY; i += 8) {
		if (is_line_empty(cpu->memory, i)) {
			continue;
		}

		printf("%04x: %04x %04x %04x %04x %04x %04x %04x %04x\n", i,
			cpu->memory[i+0], cpu->memory[i+1], cpu->memory[i+2],
			cpu->memory[i+3], cpu->memory[i+4], cpu->memory[i+5],
			cpu->memory[i+6], cpu->memory[i+7]);
	}
}
