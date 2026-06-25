#include "Utility.h"
#include "AssemblyUtility.h"

void kMemSet(void* pvDestination, BYTE bData, int iSize){
    int i;
    for(i = 0; i < iSize; i++){
        ((BYTE*) pvDestination)[i] = bData;
    }
}

int kMemCpy(void* pvDestination, const void* pvSource, int iSize){
    int i;
    for(i = 0; i < iSize; i++){
        ((BYTE*) pvDestination)[i] = ((BYTE*) pvSource)[i];
    }
    return iSize;
}

int kMemCmp(const void* pvDestination, const void* pvSource, int iSize){
    int i;
    for(i = 0; i < iSize; i++){
        if(((BYTE*) pvDestination)[i] != ((BYTE*) pvSource)[i]){
            return ((BYTE*) pvDestination)[i] - ((BYTE*) pvSource)[i];
        }
    }
    return 0;
}

BOOL kSetInterruptFlag(BOOL bEnableInterrupt){
    QWORD qwRFLAGS;

    qwRFLAGS = kReadRFLAGS();

    if(bEnableInterrupt == TRUE){
        kEnableInterrupt();
    }
    
    else{
        kDisableInterrupt();
    }



    if(qwRFLAGS & 0x0200){
        return TRUE;
    }
    
    else{
        return FALSE;
    }
}