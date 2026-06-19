#include "ImageMakerUtil.h"

int main(int argc, char* argv[]) {
    int iSourceFd;
    int iTargetFd;
    int iBootLoaderSize;
    int iKernel32SectorCount;
    int iKernel64SectorCount;
    int iSourceSize;

    if (argc != 5) {
        fprintf(stderr, "[ERROR] ImageMaker BootLoader.bin Kernel32.bin Kernel64.bin Disk.img\n");
        exit(-1);
    }

    iTargetFd = open(argv[4], O_RDWR | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE);
    if (iTargetFd == -1) {
        fprintf(stderr, "[ERROR] Disk.img File Open Failed.\n");
        exit(-1);
    }


    printf("[INFO] Disk.img File Open Success.\n");

    // Copy bootloader to image file.
    iSourceFd = open(argv[1], O_RDONLY | O_BINARY);
    if (iSourceFd == -1) {
        fprintf(stderr, "[ERROR] %s File Open Failed.\n", argv[1]);
        exit(-1);
    }


    iSourceSize = CopyFile(iSourceFd, iTargetFd);
    close(iSourceFd);

    iBootLoaderSize = AdjustInSectorSize(iTargetFd, iSourceSize);
    printf("[INFO] %s Size = [%d] Byte, Sector = [%d]\n", argv[1], 
        iSourceSize, iBootLoaderSize);

    
    // Copy 32bit mode kernel to image file.
    printf("[INFO] Copy protected mode kernel to image file.\n");
    
    iSourceFd = open(argv[2], O_RDONLY | O_BINARY);
    if (iSourceFd == -1) {
        fprintf(stderr, "[ERROR] %s File Open Failed.\n", argv[2]);
        exit(-1);
    }

    iSourceSize = CopyFile(iSourceFd, iTargetFd);
    close(iSourceFd);

    // Fill 0x00 to the remaining space of sector.
    iKernel32SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
    printf("[INFO] %s Size = [%d] Byte, Sector = [%d]\n", argv[2], 
        iSourceSize, iKernel32SectorCount);

    // Copy IA-32e mode kernel to image file.
    printf("[INFO] Copy IA-32e mode kernel to image file.\n");

    iSourceFd = open(argv[3], O_RDONLY | O_BINARY);
    if (iSourceFd == -1) {
        fprintf(stderr, "[ERROR] %s File Open Failed.\n", argv[3]);
        exit(-1);
    }

    iSourceSize = CopyFile(iSourceFd, iTargetFd);
    close(iSourceFd);

    iKernel64SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
    printf("[INFO] %s Size = [%d] Byte, Sector = [%d]\n", argv[3],
        iSourceSize, iKernel64SectorCount);
        
    // Write kernel information to image file.
    printf("[INFO] Write kernel information to image file.\n");
    WriteKernelInformation(iTargetFd, iKernel32SectorCount + iKernel64SectorCount, iKernel32SectorCount);
    printf("[INFO] Image file create complete.\n");

    close(iTargetFd);
    return 0;
}
