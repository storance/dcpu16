all: emulator disassembler

emulator: dcpu.o emulator.o
	gcc dcpu.o emulator.o -o emulator

disassembler: dcpu.o disassembler.o
	gcc dcpu.o disassembler.o -o disassembler

dcpu.o: dcpu.c dcpu.h
	gcc -std=c99 -c dcpu.c

emulator.o: dcpu.h emulator.c
	gcc -std=c99 -c emulator.c

disassembler.o: dcpu.h disassembler.c
	gcc -std=c99 -c disassembler.c

main.o: main.c
	gcc -std=c99 -c main.c

clean:
	rm *.o
	rm emulator
	rm disassembler

