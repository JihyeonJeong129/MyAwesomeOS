#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString(int iX, int iY, const char* pcString);
BOOL kInitializeKernel64Area(void);
BOOL kIsMemoryEnough(void);

void main(void) {

    DWORD i;
    DWORD dwEAX, dwEBX, dwECX, dwEDX;
    char vcVendorString[13] = {0, };
    
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

    // Build page tables for IA-32e mode
    kPrintString(0, 7, "IA-32e Page Tables Initialized ...................[    ]");
    kInitializePageTables();
    kPrintString(51, 7, "Pass");

    //Read CPU Vendor String
    kPrintString(0, 8, "Processor Vendor String Read .....................[    ]");
    kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    *(DWORD*) vcVendorString = dwEBX;
    *((DWORD*) vcVendorString + 1) = dwEDX;
    *((DWORD*) vcVendorString + 2) = dwECX;
    kPrintString(51, 8, "Pass");
    kPrintString(0, 9, "Processor Vendor .........................[            ]");
    kPrintString(43, 9, vcVendorString);    

    //Check support for 64bit mode
    kPrintString(0, 10, "64bit Mode Support Check .........................[    ]");
    kReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    if ((dwEDX & (1 << 29)) == 0) {
        kPrintString(51, 10, "Fail");
        kPrintString(0, 11, "This processor does not support 64bit mode.");
        while (1);
    }
    else {
        kPrintString(51, 10, "Pass");
        kPrintString(0, 11, "This processor supports 64bit mode.");
    }

    //switch to IA-32e mode and execute 64bit kernel
    kPrintString(0, 12, "Switch to IA-32e Mode.");
    //kSwitchAndExecute64bitKernel();
    
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