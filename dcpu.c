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
	assert(result->type <= ARG_LITERAL_END);

	switch (result->type) {
	case ARG_A: case ARG_B:
	case ARG_C: case ARG_X:
	case ARG_Y: case ARG_Z:
	case ARG_I: case ARG_J:
		result->location = dcpu_get_register(cpu, result->type);
		break;
	case ARG_DEREF_A: case ARG_DEREF_B:
	case ARG_DEREF_C: case ARG_DEREF_X:
	case ARG_DEREF_Y: case ARG_DEREF_Z:
	case ARG_DEREF_I: case ARG_DEREF_J:
		{
			uint8_t register_code = result->type - ARG_DEREF_A;
			word_t register_value = dcpu_read_register(cpu, register_code);
			result->location = dcpu_get_memory(cpu, register_value);	
		}
		break;
	case ARG_DEREF_NEXT_WORD_A: case ARG_DEREF_NEXT_WORD_B:
	case ARG_DEREF_NEXT_WORD_C: case ARG_DEREF_NEXT_WORD_X:
	case ARG_DEREF_NEXT_WORD_Y: case ARG_DEREF_NEXT_WORD_Z:
	case ARG_DEREF_NEXT_WORD_I: case ARG_DEREF_NEXT_WORD_J:
		{
			uint8_t register_code = result->type - ARG_DEREF_NEXT_WORD_A;
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
	case ARG_SP:
		result->location = &cpu->sp;
		break;
	case ARG_PC:
		result->location = &cpu->pc;
		break;
	case ARG_O:
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
		result->value = result->type - ARG_LITERAL_START;
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
	if (arg >= ARG_DEREF_NEXT_WORD_A 
		&& arg <= ARG_DEREF_NEXT_WORD_J) return 1;

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

void dcpu_initialize_argument(argument_t *arg, uint8_t type) {
	arg->type = type;
	arg->size = dcpu_get_argument_size(type);
	arg->value = 0;
	arg->location = NULL;
}

uint8_t dcpu_process_instruction(dcpu_t *cpu, word_t instruction) {
	opcode_handler_t *handler = cpu->basic + (instruction & 0x000f);

	argument_t a, b; 
	dcpu_initialize_argument(&a, (instruction >> 4) & 0x003f);
	dcpu_initialize_argument(&b, (instruction >> 10) & 0x003f);

	if (handler->resolve_args) {
		dcpu_resolve_argument(cpu, &a);
		dcpu_resolve_argument(cpu, &b);
	}

	return handler->handler(cpu, &a, &b);
}

bool dcpu_execute(dcpu_t *cpu, size_t program_size) {
	while (cpu->pc < program_size) {
		word_t instruction = dcpu_read_next_word(cpu);
		uint8_t result = dcpu_process_instruction(cpu, instruction);
		if (result == S_HALT) return true;
		if (result == S_INVALID_OPCODE) return false;
	}
}

void dcpu_dump(dcpu_t *cpu) {
	printf("Cycles: %lu\n", cpu->cycles);
	printf("======= Registers =======\n");
	printf("A: %04x  B: %04x  C: %04x  X: %04x  Y: %04x  Z: %04x\n",
		cpu->registers[0], cpu->registers[1], cpu->registers[2],
		cpu->registers[3], cpu->registers[4], cpu->registers[5]);
	printf("I: %04x  J: %04x SP: %04x PC: %04x  O: %04x\n",
		cpu->registers[6], cpu->registers[7], cpu->sp, cpu->pc, cpu->o);

	printf("======= Memory =======\n");
	for (int i = 0; i < TOTAL_MEMORY; i += 8) {
		if (!cpu->memory[i+0] && !cpu->memory[i+1] && !cpu->memory[i+2]
			&& !cpu->memory[i+3] && !cpu->memory[i+4] && !cpu->memory[i+5]
			&& !cpu->memory[i+6] && !cpu->memory[i+7]) {
			continue;
		} 

		printf("%04x: %04x %04x %04x %04x %04x %04x %04x %04x\n", i,
			cpu->memory[i+0], cpu->memory[i+1], cpu->memory[i+2],
			cpu->memory[i+3], cpu->memory[i+4], cpu->memory[i+5],
			cpu->memory[i+6], cpu->memory[i+7]);
	}
}

bool dcpu_load(dcpu_t *cpu, const char* filename, size_t *size) {
	cpu->pc = 0;

	FILE* f = fopen(filename, "r");
	if (!f) {
		return false;
	}

	size_t read = fread(cpu->memory, sizeof(word_t), TOTAL_MEMORY, f);
	if (size) {
		*size = read;
	}
	fclose(f);

	return true;
}
