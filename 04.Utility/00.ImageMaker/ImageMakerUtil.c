#include "ImageMakerUtil.h"

int CopyFile(int iSourceFd, int iTargetFd) {
    int iSourceSize;
    char vcBuffer[BYTE_OF_SECTOR_SIZE];
    int iRead;
    int iWrite;

    iSourceSize = 0;

    while (1) {
        iRead = read(iSourceFd, vcBuffer, BYTE_OF_SECTOR_SIZE);
        if (iRead == -1) {
            fprintf(stderr, "[ERROR] File Read Failed.\n");
            exit(-1);
        }

        if (iRead == 0) {
            break;
        }

        iWrite = write(iTargetFd, vcBuffer, iRead);
        if (iWrite == -1) {
            fprintf(stderr, "[ERROR] File Write Failed.\n");
            exit(-1);
        }

        iSourceSize += iRead;
    }

    return iSourceSize;
}

int AdjustInSectorSize(int iFd, int iSourceSize) {
    int i;
    int iAdjustSizeToSector;
    int cCh;
    int iSectorCount;

    iAdjustSizeToSector = iSourceSize % BYTE_OF_SECTOR_SIZE;
    cCh = 0x00;

    if (iAdjustSizeToSector != 0) {
        iAdjustSizeToSector = BYTE_OF_SECTOR_SIZE - iAdjustSizeToSector;

        printf("[INFO] Fill 0x00 to the remaining space of sector.\n");
        printf("[INFO] Adjust size to sector by filling [%d] Byte.\n", iAdjustSizeToSector);

        for (i = 0; i < iAdjustSizeToSector; i++) {
            write(iFd, &cCh, 1);
        }
    } else {
        printf("[INFO] No need to fill 0x00 to the remaining space of sector.\n");
    }

    iSectorCount = (iSourceSize + iAdjustSizeToSector) / BYTE_OF_SECTOR_SIZE;

    return iSectorCount;
}

void WriteKernelInformation(int iTargetFd, int iTotalKernelSectorCount, int iKernel32SectorCount) {
    unsigned short usData;
    long lPosition;

    lPosition = lseek(iTargetFd, 5, SEEK_SET);
    if (lPosition == -1) {
        fprintf(stderr, "[ERROR] lseek Failed.\n");
        exit(-1);
    }

    usData = (unsigned short)iTotalKernelSectorCount;
    write(iTargetFd, &usData, 2);

    usData = (unsigned short)iKernel32SectorCount;
    write(iTargetFd, &usData, 2);

    printf("[INFO] Total sector count except bootloader = [%d] Sector.\n", iTotalKernelSectorCount);
    printf("[INFO] Protected mode kernel sector count = [%d] Sector.\n", iKernel32SectorCount);
}
