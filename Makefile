all: emulator disassembler

emulator: main.o dcpu.o run_opcodes.o
	gcc main.o dcpu.o run_opcodes.o -o emulator

disassembler: main.o dcpu.o print_opcodes.o
	gcc main.o dcpu.o print_opcodes.o -o disassembler

dcpu.o: dcpu.c dcpu.h opcodes.h
	gcc -std=c99 -g -c dcpu.c

run_opcodes.o: opcodes.h run_opcodes.c
	gcc -std=c99 -g -c run_opcodes.c

print_opcodes.o: opcodes.h print_opcodes.c
	gcc -std=c99 -g -c print_opcodes.c

main.o: main.c
	gcc -std=c99 -g -c main.c

clean:
	rm *.o
	rm emulator
	rm disassembler

