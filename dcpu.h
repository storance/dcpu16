#pragma once

#define ARG_REGISTER_A 0x00
#define ARG_REGISTER_B 0x01
#define ARG_REGISTER_C 0x02
#define ARG_REGISTER_X 0x03
#define ARG_REGISTER_Y 0x04
#define ARG_REGISTER_Z 0x05
#define ARG_REGISTER_I 0x06
#define ARG_REGISTER_J 0x07
#define ARG_DEREF_REGISTER_A 0x08
#define ARG_DEREF_REGISTER_B 0x09
#define ARG_DEREF_REGISTER_C 0x0a
#define ARG_DEREF_REGISTER_X 0x0b
#define ARG_DEREF_REGISTER_Y 0x0c
#define ARG_DEREF_REGISTER_Z 0x0d
#define ARG_DEREF_REGISTER_I 0x0e
#define ARG_DEREF_REGISTER_J 0x0f
#define ARG_DEREF_NEXT_WORD_REGISTER_A 0x10
#define ARG_DEREF_NEXT_WORD_REGISTER_B 0x11
#define ARG_DEREF_NEXT_WORD_REGISTER_C 0x12
#define ARG_DEREF_NEXT_WORD_REGISTER_X 0x13
#define ARG_DEREF_NEXT_WORD_REGISTER_Y 0x14
#define ARG_DEREF_NEXT_WORD_REGISTER_Z 0x15
#define ARG_DEREF_NEXT_WORD_REGISTER_I 0x16
#define ARG_DEREF_NEXT_WORD_REGISTER_J 0x17
#define ARG_POP 0x18
#define ARG_PEEK 0x19
#define ARG_PUSH 0x1a
#define ARG_REGISTER_SP 0x1b
#define ARG_REGISTER_PC 0x1c
#define ARG_REGISTER_O 0x1d
#define ARG_DEREF_NEXT_WORD 0x1e
#define ARG_NEXT_WORD 0x1f
#define ARG_LITERAL_START 0x20
#define ARG_LITERAL_END 0x3f

#define OP_NON_BASIC 0
#define OP_SET 1
#define OP_ADD 2
#define OP_SUB 3
#define OP_MUL 4
#define OP_DIV 5
#define OP_MOD 6
#define OP_SHL 7
#define OP_SHR 8
#define OP_AND 9
#define OP_BOR 10
#define OP_XOR 11
#define OP_IFE 12
#define OP_IFN 13
#define OP_IFG 14
#define OP_IFB 15

#define OP_JSR 1

#define TOTAL_BASIC_OPCODES 16
#define TOTAL_NON_BASIC_OPCODES 2
#define TOTAL_REGISTERS 8
#define TOTAL_MEMORY 65536

typedef uint16_t word_t;

struct dcpu_s;

typedef struct argument_s {
	uint8_t arg_value;

	word_t value;
	word_t *location;
} argument_t;

typedef void (*op_handler_fn)(struct dcpu_s*, argument_t*, argument_t*);

typedef struct {
	op_handler_fn handler;
	bool resolve_args;
} opcode_handler_t;

typedef struct dcpu_s {
	word_t memory[TOTAL_MEMORY];
	word_t registers[TOTAL_REGISTERS];
	word_t pc;
	word_t sp;
	word_t o;
	unsigned long cycles;

	opcode_handler_t basic[TOTAL_BASIC_OPCODES];
	opcode_handler_t non_basic[TOTAL_NON_BASIC_OPCODES];
} dcpu_t;

word_t *dcpu_get_register(dcpu_t*, uint8_t);
word_t *dcpu_get_memory(dcpu_t*, word_t);
word_t dcpu_read_memory(dcpu_t*, word_t);
word_t dcpu_read_next_word(dcpu_t*);
word_t dcpu_read_register(dcpu_t*, uint8_t);

void dcpu_resolve_argument(dcpu_t *, argument_t*);

void dcpu_skip_next(dcpu_t*);
void dcpu_execute(dcpu_t*);

void resolve_argument(dcpu_t*, uint8_t, argument_t*);