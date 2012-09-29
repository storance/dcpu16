all:
	make -C emulator all
	make -C assembler all

clean:
	rm -Rf target
	rm emulator
	rm disassembler

