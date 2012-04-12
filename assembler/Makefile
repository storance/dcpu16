all: assembler

assembler: target/assembler.o target/compiler.o
	g++ target/assembler.o target/compiler.o -o assembler

target/compiler.o: target ast.hpp compiler.hpp compiler.cpp
	g++ -std=c++0x -I../../boost/boost_1_49_0 -c compiler.cpp -o target/compiler.o

target/assembler.o: target assembler.hpp assembler.cpp ast.hpp statement.hpp skipper.hpp
	g++ -std=c++0x -I../../boost/boost_1_49_0 -c assembler.cpp -o target/assembler.o

target/opcode.o: target assembler.hpp argument.hpp opcode.hpp opcode.cpp
	g++ -c -I../../boost/boost_1_49_0 opcode.cpp -o target/opcode.o

target:
	mkdir target

clean:
	rm -Rf target
	rm assembler