#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "dcpu.h"

void store_result(argument_t* a, word_t result) {
	if (a->location) {
		*a->location = result;
	}
}

uint8_t handle_non_basic(dcpu_t *cpu, argument_t* a, argument_t* b) {
	uint8_t opcode = a->type;

	if (opcode >= TOTAL_NON_BASIC_OPCODES) {
		fprintf(stderr, "Unknown Non Basic Opcode 0x%x\n", opcode);
		return S_INVALID_OPCODE;
	}

	opcode_handler_t *handler = cpu->non_basic + opcode;
	
	if (handler->resolve_args) {
		dcpu_resolve_argument(cpu, b);
	}

	return handler->handler(cpu, b, NULL);
}

bool is_set_halt_instruction(dcpu_t* cpu, argument_t* a, argument_t* b) {
	if (a->type == ARG_PC) {
		word_t old_pc = cpu->pc - 1 - a->size - b->size;
		return (b->type == ARG_NEXT_WORD && b->value == old_pc)
			|| (b->type >= ARG_LITERAL_START && b->value == old_pc);
	}

	return false;
}

uint8_t handle_set(dcpu_t *cpu, argument_t* a, argument_t* b) {
	if (is_set_halt_instruction(cpu, a, b)) {
		return S_HALT;
	}

	store_result(a, b->value);

	return S_CONTINUE;
}

uint8_t handle_add(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	word_t result = a->value + b->value;
	cpu->o = result < a->value || result < b->value;

	store_result(a, result);

	return S_CONTINUE;
}

uint8_t handle_sub(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	word_t result = a->value - b->value;
	cpu->o = result > a->value ? 0xffff : 0;

	store_result(a, result);

	return S_CONTINUE;
}

uint8_t handle_mul(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	word_t result = a->value * b->value;
	cpu->o = (result >> 16) & 0xffff;

	store_result(a, result);

	return S_CONTINUE;
}

uint8_t handle_div(dcpu_t *cpu, argument_t* a, argument_t* b) {
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

	return S_CONTINUE;
}

uint8_t handle_mod(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles += 2;

	word_t result;
	if (b->value == 0) {
		result = 0;
	} else {
		result = a->value % b->value;
	}

	store_result(a, result);

	return S_CONTINUE;
}

uint8_t handle_shl(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	word_t result = a->value << b->value;
	cpu->o = (result >> 16) & 0xffff;

	store_result(a, result);

	return S_CONTINUE;
}

uint8_t handle_shr(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	uint32_t result_nocarry = (a->value << 16) >> b->value; 
	cpu->o = result_nocarry & 0xffff;

	store_result(a, (word_t)(result_nocarry >> 16));

	return S_CONTINUE;
}

uint8_t handle_and(dcpu_t *cpu, argument_t* a, argument_t* b) {
	store_result(a, a->value & b->value);

	return S_CONTINUE;
}

uint8_t handle_bor(dcpu_t *cpu, argument_t* a, argument_t* b) {
	store_result(a, a->value | b->value);

	return S_CONTINUE;
}

uint8_t handle_xor(dcpu_t *cpu, argument_t* a, argument_t* b) {
	store_result(a, a->value ^ b->value);

	return S_CONTINUE;
}

uint8_t handle_ife(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;
	if (a->value != b->value) dcpu_skip_next(cpu);

	return S_CONTINUE;
}

uint8_t handle_ifn(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;
	if (a->value == b->value) dcpu_skip_next(cpu);

	return S_CONTINUE;
}

uint8_t handle_ifg(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;
	if (a->value <= b->value) dcpu_skip_next(cpu);

	return S_CONTINUE;
}

uint8_t handle_ifb(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;
	if ((a->value & b->value) == 0) dcpu_skip_next(cpu);

	return S_CONTINUE;
}

uint8_t handle_jsr(dcpu_t *cpu, argument_t* a, argument_t* b) {
	cpu->cycles++;

	word_t *stack = dcpu_get_memory(cpu, --cpu->sp);

	*stack = cpu->pc;
	cpu->pc = a->value;

	return S_CONTINUE;
}

uint8_t handle_reserved(dcpu_t *cpu, argument_t* a, argument_t* b){
	fprintf(stderr, "Non Basic Opcode 0x0 is reserved.\n");
	return S_INVALID_OPCODE;
}

void initialize_opcode(opcode_handler_t *handler, op_handler_fn handelr_fn, 
	bool resolve_args) {
	handler->handler = handelr_fn;
	handler->resolve_args = resolve_args;
}

void initialize_handlers(dcpu_t* cpu) {
	initialize_opcode(cpu->basic + OP_NON_BASIC, &handle_non_basic, false);
	initialize_opcode(cpu->basic + OP_SET, &handle_set, true);
	initialize_opcode(cpu->basic + OP_ADD, &handle_add, true);
	initialize_opcode(cpu->basic + OP_SUB, &handle_sub, true);
	initialize_opcode(cpu->basic + OP_MUL, &handle_mul, true);
	initialize_opcode(cpu->basic + OP_DIV, &handle_div, true);
	initialize_opcode(cpu->basic + OP_MOD, &handle_mod, true);
	initialize_opcode(cpu->basic + OP_SHL, &handle_shl, true);
	initialize_opcode(cpu->basic + OP_SHR, &handle_shr, true);
	initialize_opcode(cpu->basic + OP_AND, &handle_and, true);
	initialize_opcode(cpu->basic + OP_BOR, &handle_bor, true);
	initialize_opcode(cpu->basic + OP_XOR, &handle_xor, true);
	initialize_opcode(cpu->basic + OP_IFE, &handle_ife, true);
	initialize_opcode(cpu->basic + OP_IFN, &handle_ifn, true);
	initialize_opcode(cpu->basic + OP_IFG, &handle_ifg, true);
	initialize_opcode(cpu->basic + OP_IFB, &handle_ifb, true);

	initialize_opcode(cpu->non_basic + 0, &handle_reserved, true);
	initialize_opcode(cpu->non_basic + OP_JSR, &handle_jsr, true);
}

int main(int argc, char **argv) {
	dcpu_t cpu = {0};
	initialize_handlers(&cpu);

	if (argc < 2) {
		printf("Usage: %s </path/to/dcpu/program>\n", argv[0]);
		return 1;
	}

	if (!dcpu_load(&cpu, argv[1], NULL)) {
		printf("Unable to load program %s\n", argv[1]);
		return 1;
	}	

	dcpu_execute(&cpu, TOTAL_MEMORY);
	dcpu_dump(&cpu);
}
