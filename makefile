.PHONY: all BootLoader Kernel32 clean

all: Disk.img

BootLoader: 00.BootLoader/BootLoader.bin

00.BootLoader/BootLoader.bin: 00.BootLoader/BootLoader.asm
	@echo
	@echo "Building BootLoader..."
	@echo

	make -C 00.BootLoader

	@echo
	@echo "BootLoader build complete."
	@echo

Kernel32: 01.Kernel32/VirtualOS.bin

01.Kernel32/VirtualOS.bin: 01.Kernel32/Source/VirtualOS.asm
	@echo
	@echo "Building Kernel..."
	@echo

	make -C 01.Kernel32

	@echo
	@echo "Kernel build complete."
	@echo

Disk.img: 00.BootLoader/BootLoader.bin 01.Kernel32/VirtualOS.bin
	@echo
	@echo "Creating Disk.img..."
	@echo

	cat 00.BootLoader/BootLoader.bin 01.Kernel32/VirtualOS.bin > Disk.img
	truncate -s 1474560 Disk.img	

	@echo
	@echo "Disk.img creation complete."
	@echo

clean:
	@echo
	@echo "Cleaning up..."
	make -C 00.BootLoader clean
	make -C 01.Kernel32 clean
	rm -f Disk.img
	@echo
	@echo "Cleanup complete."
	@echo
