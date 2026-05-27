all: BootLoader Disk.img

BootLoader: 
	@echo
	@echo "Building BootLoader..."
	@echo

	make -C 00.BootLoader

	@echo
	@echo "BootLoader build complete."
	@echo

Disk.img: 00.BootLoader/BootLoader.bin
	@echo
	@echo "Creating Disk.img..."
	@echo

	cp 00.BootLoader/BootLoader.bin Disk.img

	@echo
	@echo "Disk.img creation complete."
	@echo

clean:
	@echo
	@echo "Cleaning up..."
	make -C 00.BootLoader clean
	rm -f Disk.img
	@echo
	@echo "Cleanup complete."
	@echo