all: emulator disassembler

emulator: target/dcpu.o target/emulator.o target/opcode.o
	g++ target/dcpu.o target/emulator.o target/opcode.o -o emulator

disassembler: target/dcpu.o target/disassembler.o target/opcode.o
	g++ target/dcpu.o target/disassembler.o target/opcode.o -o disassembler

target/dcpu.o: dcpu.cpp dcpu.hpp opcode.hpp common.hpp target
	g++ -c dcpu.cpp -o target/dcpu.o

target/opcode.o: opcode.cpp opcode.hpp dcpu.hpp common.hpp target
	g++ -c opcode.cpp -o target/opcode.o

target/emulator.o: dcpu.hpp emulator.cpp common.hpp target 
	g++ -c emulator.cpp -o target/emulator.o

target/disassembler.o: dcpu.hpp disassembler.cpp common.hpp target
	g++ -c disassembler.cpp -o target/disassembler.o

target:
	mkdir target

clean:
	rm -Rf target
	rm emulator
	rm disassembler

