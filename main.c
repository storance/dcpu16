#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "dcpu.h"
#include "opcodes.h"

bool load_program(dcpu_t *cpu, const char* filename) {
	cpu->pc = 0;

	FILE* f = fopen(filename, "r");
	if (!f) {
		return false;
	}

	fread(cpu->memory, sizeof(word_t), TOTAL_MEMORY, f);
	fclose(f);

	return true;
}

int main(int argc, char **argv) {
	dcpu_t cpu = {0};
	initialize_handlers(&cpu);

	if (argc < 2) {
		printf("Usage: %s </path/to/dcpu/program>\n", argv[0]);
		return 1;
	}

	if (!load_program(&cpu, argv[1])) {
		printf("Unable to load program %s\n", argv[1]);
		return 1;
	}	

	dcpu_execute(&cpu);
	dump_cpu(&cpu);
}