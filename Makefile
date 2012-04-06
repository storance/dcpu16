all: emulator

emulator: emulator.o
	gcc emulator.o -o emulator

emulator.o: emulator.c emulator.h
	gcc -std=c99 -g -c emulator.c

clean:
	rm *.o
	rm emulator

