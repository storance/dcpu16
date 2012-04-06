#define REGISTERS 8
#define MEMORY_SIZE 65536

#define ARG_REG_START 0
#define ARG_REG_END 0x07
#define ARG_REF_REG_START 0x08
#define ARG_REF_REG_END 0x0f
#define ARG_REF_NW_REG_START 0x10
#define ARG_REF_NW_REG_END 0x17
#define ARG_POP 0x18
#define ARG_PEEK 0x19
#define ARG_PUSH 0x1a
#define ARG_SP 0x1b
#define ARG_PC 0x1c
#define ARG_O 0x1d
#define ARG_REF_NEXT_WORD 0x1e
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
#define NUM_BASIC_OPCODES 16
#define NUM_NON_BASIC_OPCODES 4096 

#define OP_JSR 1

typedef uint16_t word_t;

struct dcpu_s;

typedef void (*op_handler)(struct dcpu_s*, word_t*, bool, word_t);
typedef void (*nb_op_handler)(struct dcpu_s*, word_t*, bool);

typedef struct dcpu_s {
	word_t registers[REGISTERS];
	word_t pc;
	word_t sp;
	word_t o;
	bool skipNext;
	word_t memory[MEMORY_SIZE];
	unsigned long cycles;

	op_handler basic[NUM_BASIC_OPCODES];
	nb_op_handler non_basic[NUM_NON_BASIC_OPCODES];
} dcpu_t;

typedef void (*op_handler)(dcpu_t*, word_t*, bool, word_t);
typedef void (*nb_op_handler)(dcpu_t*, word_t*, bool);

