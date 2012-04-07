#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>

#include "dcpu.h"

word_t *dcpu_get_register(dcpu_t *cpu, uint8_t register_code) {
	return cpu->registers + register_code;
}

word_t dcpu_read_register(dcpu_t *cpu, uint8_t register_code) {
	return *dcpu_get_register(cpu, register_code);
}

word_t *dcpu_get_memory(dcpu_t *cpu, word_t address) {
	return cpu->memory + address;
}

word_t dcpu_read_memory(dcpu_t *cpu, word_t address) {
	return *dcpu_get_memory(cpu, address);
}

word_t dcpu_read_next_word(dcpu_t *cpu) {
	cpu->cycles++;

	return dcpu_read_memory(cpu, cpu->pc++);
}

void dcpu_resolve_argument(dcpu_t *cpu, argument_t *result) {
	assert(result);
	assert(result->arg_value <= ARG_LITERAL_END);

	switch (result->arg_value) {
	case ARG_REGISTER_A: case ARG_REGISTER_B:
	case ARG_REGISTER_C: case ARG_REGISTER_X:
	case ARG_REGISTER_Y: case ARG_REGISTER_Z:
	case ARG_REGISTER_I: case ARG_REGISTER_J:
		result->location = dcpu_get_register(cpu, result->arg_value);
		break;
	case ARG_DEREF_REGISTER_A: case ARG_DEREF_REGISTER_B:
	case ARG_DEREF_REGISTER_C:case ARG_DEREF_REGISTER_X:
	case ARG_DEREF_REGISTER_Y: case ARG_DEREF_REGISTER_Z:
	case ARG_DEREF_REGISTER_I: case ARG_DEREF_REGISTER_J:
		{
			uint8_t register_code = result->arg_value - ARG_DEREF_REGISTER_A;
			word_t register_value = dcpu_read_register(cpu, register_code);
			result->location = dcpu_get_memory(cpu, register_value);	
		}
		break;
	case ARG_DEREF_NEXT_WORD_REGISTER_A: case ARG_DEREF_NEXT_WORD_REGISTER_B:
	case ARG_DEREF_NEXT_WORD_REGISTER_C: case ARG_DEREF_NEXT_WORD_REGISTER_X:
	case ARG_DEREF_NEXT_WORD_REGISTER_Y: case ARG_DEREF_NEXT_WORD_REGISTER_Z:
	case ARG_DEREF_NEXT_WORD_REGISTER_I: case ARG_DEREF_NEXT_WORD_REGISTER_J:
		{
			uint8_t register_code = result->arg_value - ARG_DEREF_NEXT_WORD_REGISTER_A;
			word_t register_value = dcpu_read_register(cpu, register_code);
			word_t next_word = dcpu_read_next_word(cpu);
			result->location = dcpu_get_memory(cpu, next_word + register_value);
		}
		break;
	case ARG_POP:
		result->location = dcpu_get_memory(cpu, cpu->sp++);
		break;
	case ARG_PEEK:
		result->location = dcpu_get_memory(cpu, cpu->sp);
		break;
	case ARG_PUSH:
		result->location = dcpu_get_memory(cpu, --cpu->sp);
		break;
	case ARG_REGISTER_SP:
		result->location = &cpu->sp;
		break;
	case ARG_REGISTER_PC:
		result->location = &cpu->pc;
		break;
	case ARG_REGISTER_O:
		result->location = &cpu->o;
		break;
	case ARG_DEREF_NEXT_WORD:
		{
			word_t next_word = dcpu_read_next_word(cpu);
			result->location = dcpu_get_memory(cpu, next_word);
		}
		break;
	case ARG_NEXT_WORD:
		result->value = dcpu_read_next_word(cpu);
		result->location = NULL;
		break;
	default:
		result->value = result->arg_value - ARG_LITERAL_START;
		result->location = NULL;
		break;
	}

	if (result->location) {
		result->value = *result->location;
	}
}

uint8_t dcpu_get_argument_size(uint8_t arg) {
	if (arg == ARG_DEREF_NEXT_WORD) return 1;
	if (arg == ARG_NEXT_WORD) return 1;
	if (arg >= ARG_DEREF_NEXT_WORD_REGISTER_A 
		&& arg <= ARG_DEREF_NEXT_WORD_REGISTER_J) return 1;

	return 0;
}

uint8_t dcpu_get_instruction_size(word_t instruction) {
	uint8_t opcode = instruction & 0x0000f;
	uint8_t a = (instruction >> 4) & 0x003f;
	uint8_t b = (instruction >> 10) & 0x003f;

	if (opcode == 0) {
		return 1 + dcpu_get_argument_size(b);
	} else {
		return 1 + dcpu_get_argument_size(a) + dcpu_get_argument_size(b);
	}
}

void dcpu_skip_next(dcpu_t *cpu) {
	word_t instruction = dcpu_read_next_word(cpu);

	cpu->pc += dcpu_get_instruction_size(instruction) - 1;
}

void dcpu_process_instruction(dcpu_t *cpu, word_t instruction) {
	opcode_handler_t *handler = cpu->basic + (instruction & 0x000f);
	argument_t a = {.arg_value = (instruction >> 4) & 0x003f, .value = 0, .location = NULL};
	argument_t b = {.arg_value = (instruction >> 10) & 0x003f, .value = 0, .location = NULL};

	if (handler->resolve_args) {
		dcpu_resolve_argument(cpu, &a);
		dcpu_resolve_argument(cpu, &b);
	}

	handler->handler(cpu, &a, &b);
}

void dcpu_execute(dcpu_t *cpu) {
	while (true) {
		word_t instruction = dcpu_read_next_word(cpu);
		if (instruction == 0) {
			break;
		}

		dcpu_process_instruction(cpu, instruction);
	}
}