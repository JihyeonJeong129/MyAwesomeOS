all: BootLoader.bin

BootLoader.bin: BootLoader.asm
	nasm -f bin -o BootLoader.bin BootLoader.asm

clean:
	rm -f BootLoader.bin
