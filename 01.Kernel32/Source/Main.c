#include "Types.h"

void kPrintString(int iX, int iY, const char* pcString);
BOOL kInitializeKernel64Area(void);
BOOL kIsMemoryEnough(void);

void main(void) {
    
    kPrintString(0, 3, "C Language Kernel Started.........................[Pass]");

    //Check minimum memory size
    kPrintString(0, 4, "Minimum Memory Size Check.........................[    ]");
    if(kIsMemoryEnough() == FALSE)
    {
        kPrintString(51, 4, "Fail");
        kPrintString(0, 5, "Not Enough Memory. Requires Over 64MB Memory......[Fail]");
        while (1);
    }

    else{
        kPrintString(51, 4, "Pass");
        kPrintString(0, 5, "Memory Size OK. OS Requires Over 64MB Memory......[Pass]");
    }

    //Initialize kernel area for IA-32e mode
    kPrintString(0, 6, "IA-32e Kernel Area Initialized ...................[    ]");
    kInitializeKernel64Area();
    kPrintString(51, 6, "Pass");

    
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

BOOL kIsMemoryEnough(void) {
    DWORD* pdwCurrentAddress = (DWORD*) 0x100000;
    DWORD* pdwLastAddress = (DWORD*) 0x4000000;

    while (pdwCurrentAddress < pdwLastAddress) {
        *pdwCurrentAddress = 0x12345678;

        if (*pdwCurrentAddress != 0x12345678)
            return FALSE;

        pdwCurrentAddress += (0x100000 / 4); // 1MB unit
    }

    return TRUE;
}