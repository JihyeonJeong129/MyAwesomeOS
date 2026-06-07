#include "Types.h"

void kPrintString(int iX, int iY, const char* pcString);
BOOL kInitializeKernel64Area(void);

void main(void) {
    
    kPrintString(0, 3, "C Language Kernel Started~!!!");

    //Initialize kernel area for IA-32e mode
    kInitializeKernel64Area();
    kPrintString(0, 4, "IA-32e Kernel Area Initialized Successfully~!!!");
    
    while (1);
}

void kPrintString(int iX, int iY, const char* pcString) {
    CHARACTER* pstScreen = (CHARACTER*) 0xB8000;
    int i;

    pstScreen += (iY * 80) + iX;

    for (i = 0; pcString[i] != 0; i++) {
        pstScreen[i].bCharacter = pcString[i];
    }
}

BOOL kInitializeKernel64Area(void) {
    DWORD* pdwCurrentAddress = (DWORD*) 0x100000;
    DWORD* pdwLastAddress = (DWORD*) 0x600000;

    while (pdwCurrentAddress < pdwLastAddress) {
        *pdwCurrentAddress = 0x00;
        
        if(*pdwCurrentAddress != 0x00)
            return FALSE;
        
        pdwCurrentAddress++;
    }

    return TRUE;
}