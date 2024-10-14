FILES=./build/ipc_client.o ./build/ipc_server.o
INCLUDES= -I../stdlib/src
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
all: ${FILES}
	i686-elf-gcc -g -T ./linker.ld -o ./client.elf -ffreestanding -O0 -nostdlib -fpic -g ./build/ipc_client.o ../stdlib/stdlib.elf
	i686-elf-gcc -g -T ./linker.ld -o ./server.elf -ffreestanding -O0 -nostdlib -fpic -g ./build/ipc_server.o ../stdlib/stdlib.elf


./build/ipc_client.o: ./src/ipc_client.c
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./src/ipc_client.c -o ./build/ipc_client.o

./build/ipc_server.o: ./src/ipc_server.c
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./src/ipc_server.c -o ./build/ipc_server.o


clean:
	rm -rf ${FILES}
	rm ./client.elf
	rm ./server.elf