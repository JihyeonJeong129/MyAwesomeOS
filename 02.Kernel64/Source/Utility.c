#include "Utility.h"
#include "AssemblyUtility.h"
#include <stdarg.h>

volatile QWORD g_qwTickCount = 0;

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

int kStrLen(const char* pcBuffer){
    int i;
    
    for(i = 0; pcBuffer[i] != '\0'; i++){
        ;
    }
    
    return i;
}

// Total RAM size in MB
static int gs_iTotalRAMSize = 0;

// Check total RAM size and return it in MB
// Checking Size over 64MB
void kCheckTotalRAMSize(void){
    DWORD* pdwCurrentAddress;
    DWORD dwPreviousValue;

    pdwCurrentAddress = (DWORD*) 0x4000000; // 64MB

    while(1){
        dwPreviousValue = *pdwCurrentAddress;
        *pdwCurrentAddress = 0x12345678;

        if(*pdwCurrentAddress != 0x12345678){
            break;
        }

        *pdwCurrentAddress = dwPreviousValue;
        pdwCurrentAddress += (0x100000 / 4);
    }

    gs_iTotalRAMSize = ((QWORD) pdwCurrentAddress / 0x100000);
}

QWORD kGetTotalRAMSize(void){
    return gs_iTotalRAMSize;
}


QWORD kAToI (const char* pcBuffer, int iRadix){
    QWORD qwReturn = 0;
    
    switch(iRadix){
        case 16:
            qwReturn = kHexStringToQword(pcBuffer);
            break;
        case 10:
            qwReturn = kDecimalStringToLong(pcBuffer);
            break;
        default:
            return 0;
    }

    return qwReturn;
}


// Convert decimal string to long
long kDecimalStringToLong(const char* pcBuffer){
    long lReturn = 0;
    int i;

    if(pcBuffer[0] == '-'){
        i = 1;
    }

    else{
        i = 0;
    }


    for( ; pcBuffer[i] != '\0'; i++){
        lReturn *= 10;
        lReturn += (pcBuffer[i] - '0');
    }

    if(pcBuffer[0] == '-'){
        lReturn = -lReturn;
    }

    return lReturn;
}

// Convert hex string to QWORD
QWORD kHexStringToQword(const char* pcBuffer){
    QWORD qwReturn = 0;
    int i;

    for( ; pcBuffer[i] != '\0'; i++){
        qwReturn *= 16;

        if((pcBuffer[i] >= '0') && (pcBuffer[i] <= '9')){
            qwReturn += (pcBuffer[i] - '0');
        }

        else if((pcBuffer[i] >= 'A') && (pcBuffer[i] <= 'F')){
            qwReturn += (pcBuffer[i] - 'A' + 10);
        }

        else if((pcBuffer[i] >= 'a') && (pcBuffer[i] <= 'f')){
            qwReturn += (pcBuffer[i] - 'a' + 10);
        }

        else{
            return 0;
        }
    }

    return qwReturn;
}

// itoa function to convert integer to string
int kIToA(long lValue, char* pcBuffer, int iRadix){
    int iReturn;

    switch(iRadix){
        case 16:
            iReturn = kHexToString(lValue, pcBuffer);
            break;
        case 10:
            iReturn = kDecimalToString(lValue, pcBuffer);
            break;
        default:
            pcBuffer[0] = '\0';
            break;
    }

    return iReturn;
}

// Convert decimal to string
int kDecimalToString(long lValue, char* pcBuffer){
    long i;

    if(lValue == 0){
        pcBuffer[0] = '0';
        pcBuffer[1] = '\0';
        return 1;
    }

    if(lValue < 0){
        pcBuffer[0] = '-';
        lValue = -lValue;
        i = 1;
    }
    else{
        i = 0;
    }


    for( ; lValue > 0; lValue /= 10){
        pcBuffer[i++] = '0' + (lValue % 10);
    }

    pcBuffer[i] = '\0';

    // Reverse the string
    kReverseString(pcBuffer);

    return i;
}

// Convert hex to string
int kHexToString(QWORD lValue, char* pcBuffer){
    QWORD i;
    QWORD qwCurrentValue;

    if(lValue == 0){
        pcBuffer[0] = '0';
        pcBuffer[1] = '\0';
        return 1;
    }

    for(i = 0; lValue > 0; lValue /= 16){
        qwCurrentValue = lValue % 16;

        if(qwCurrentValue < 10){
            pcBuffer[i++] = '0' + qwCurrentValue;
        }
        else{
            pcBuffer[i++] = 'A' + (qwCurrentValue - 10);
        }
    }

    pcBuffer[i] = '\0';

    // Reverse the string
    kReverseString(pcBuffer);

    return i;
}

// Reverse the string
void kReverseString(char* pcBuffer){
    int iLength = kStrLen(pcBuffer);
    int i;
    char cTemp;

    for(i = 0; i < iLength / 2; i++){
        cTemp = pcBuffer[i];
        pcBuffer[i] = pcBuffer[iLength - 1 - i];
        pcBuffer[iLength - 1 - i] = cTemp;
    }
}


// sprintf function to print formatted string
int kSPrintf(char* pcBuffer, const char* pcFormatString, ...){
    va_list ap;
    int iReturn;

    va_start(ap, pcFormatString);
    iReturn = kVSPrintf(pcBuffer, pcFormatString, ap);
    va_end(ap);

    return iReturn;
}

int kVSPrintf(char* pcBuffer, const char* pcFormatString, va_list ap){
    QWORD i, j;
    int iBufferIndex = 0;
    int iCopyLength;
    int iFormatStringLength = kStrLen(pcFormatString);
    char* pcCopyString;
    QWORD qwValue;
    int iValue;

    for(i = 0; i < iFormatStringLength; i++){
        if(pcFormatString[i] == '%'){
            
            i++;

            switch(pcFormatString[i]){
                case 's':
                    pcCopyString = (char*) va_arg(ap, char*);
                    iCopyLength = kStrLen(pcCopyString);
                    kMemCpy(pcBuffer + iBufferIndex, pcCopyString, iCopyLength);
                    iBufferIndex += iCopyLength;
                    break;

                case 'c':
                    pcBuffer[iBufferIndex++] = (char) va_arg(ap, int);
                    break;

                case 'd':
                case 'i':
                    iValue = (int) va_arg(ap, int);
                    iBufferIndex += kIToA(iValue, pcBuffer + iBufferIndex, 10);
                    break;
                    
                case 'x':
                case 'X':
                case 'q':
                case 'Q':
                    qwValue = (QWORD) va_arg(ap, QWORD);
                    iBufferIndex += kIToA(qwValue, pcBuffer + iBufferIndex, 16);
                    break;

                default:
                    pcBuffer[iBufferIndex++] = pcFormatString[i];
                    break;
            }
        }

        else{
            pcBuffer[iBufferIndex++] = pcFormatString[i];
        }
    }

    pcBuffer[iBufferIndex] = '\0';

    return iBufferIndex;
}

QWORD kGetTickCount(void){
    return g_qwTickCount;
}
