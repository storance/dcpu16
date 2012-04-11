all: assembler

assembler: target/assembler.o
	g++ target/assembler.o -o assembler

target/argument.o: target assembler.hpp argument.cpp argument.hpp
	g++ -c argument.cpp -o target/argument.o

target/opcode.o: target assembler.hpp argument.hpp opcode.hpp opcode.cpp
	g++ -c -I../../boost/boost_1_49_0 opcode.cpp -o target/opcode.o

target/assembler.o: target assembler.hpp assembler.cpp ast.hpp statement.hpp skipper.hpp
	g++ -c -I../../boost/boost_1_49_0 assembler.cpp -o target/assembler.o

target:
	mkdir target

clean:
	rm -Rf target
	rm assembler