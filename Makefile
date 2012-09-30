all:
	make -C emulator all
	make -C assembler all

clean:
	make -C emulator clean
	make -C assembler clean

