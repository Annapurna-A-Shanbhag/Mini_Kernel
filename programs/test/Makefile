FILES= ./test.o

all:$(FILES)
	ld -m elf_i386 -T linker.ld -o test.bin test.o --oformat=binary
	
./test.o: ./test.asm
	nasm -f elf32 -o test.o test.asm

clean:
	rm ./test.bin