.PHONY: all BootLoader 01.Kernel32 02.Kernel64 clean

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


01.Kernel32:
	@echo
	@echo "Building Kernel..."
	@echo

	make -C 01.Kernel32

	@echo
	@echo "Kernel build complete."
	@echo

02.Kernel64:
	@echo
	@echo "Building Kernel64..."
	@echo

	make -C 02.Kernel64

	@echo
	@echo "Kernel64 build complete."
	@echo

ImageMaker: 04.Utility/00.ImageMaker/ImageMaker.c 04.Utility/00.ImageMaker/ImageMakerUtil.c 04.Utility/00.ImageMaker/ImageMakerUtil.h
	gcc -o $@ 04.Utility/00.ImageMaker/ImageMaker.c 04.Utility/00.ImageMaker/ImageMakerUtil.c

Disk.img: ImageMaker 00.BootLoader/BootLoader.bin 01.Kernel32 02.Kernel64
	@echo
	@echo "Creating Disk.img..."
	@echo

	./ImageMaker 00.BootLoader/BootLoader.bin 01.Kernel32/Kernel32.bin 02.Kernel64/Kernel64.bin Disk.img

	@echo
	@echo "Disk.img creation complete."
	@echo

clean:
	@echo
	@echo "Cleaning up..."
	make -C 00.BootLoader clean
	make -C 01.Kernel32 clean
	make -C 02.Kernel64 clean
	rm -f Disk.img
	@echo
	@echo "Cleanup complete."
	@echo
