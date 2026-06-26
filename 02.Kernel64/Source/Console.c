#include <stdarg.h>
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "AssemblyUtility.h"

CONSOLEMANAGER gs_stConsoleManager = {0, };

// Initialize the console
void kInitializeConsole(int iX, int iY) {
    kMemSet( &gs_stConsoleManager, 0, sizeof(gs_stConsoleManager));
    kSetCursor(iX, iY);
}

// Set the cursor position
void kSetCursor(int iX, int iY) {
    int iOffset;
    iOffset = (iY * CONSOLE_WIDTH + iX);

    kOutPortByte(VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR);
    kOutPortByte(VGA_PORT_DATA, iOffset >> 8);
    kOutPortByte(VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR);
    kOutPortByte(VGA_PORT_DATA, iOffset & 0xFF);

    // Update the current print offset
    gs_stConsoleManager.iCurrentPrintOffset = iOffset;
}

// Get the cursor position
void kGetCursor(int* piX, int* piY) {
    *piX = gs_stConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
    *piY = gs_stConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}

void kPrintf(const char* pcFormatString, ...) {
    va_list ap;
    char vcBuffer[1024];
    int iNextPrintOffset;

    // Format the string
    va_start(ap, pcFormatString);
    kVSPrintf(vcBuffer, pcFormatString, ap);
    va_end(ap);

    // Print the formatted string and update the cursor position
    iNextPrintOffset = kConsolePrintString(vcBuffer);
    kSetCursor(iNextPrintOffset % CONSOLE_WIDTH, iNextPrintOffset / CONSOLE_WIDTH);
}

// \n, \t, and \r are handled in kConsolePrintString
int kConsolePrintString(const char* pcBuffer) {
    CHARACTER* pstScreen = (CHARACTER*)CONSOLE_VIDEOMEMORYADDRESS;
    
    int i;
    int iCurrentOffset;
    int iLength = kStrLen(pcBuffer);

    // Get the current print offset
    iCurrentOffset = gs_stConsoleManager.iCurrentPrintOffset;

    for (i = 0; i < iLength; i++) {
        switch (pcBuffer[i]) {
            case '\n':
                iCurrentOffset += (CONSOLE_WIDTH - (iCurrentOffset % CONSOLE_WIDTH));
                break;
            case '\t':
                iCurrentOffset += (8 - (iCurrentOffset % 8));
                break;
            case '\r':
                iCurrentOffset -= (iCurrentOffset % CONSOLE_WIDTH);
                break;
            default:
                pstScreen[iCurrentOffset].bCharacter = pcBuffer[i];
                pstScreen[iCurrentOffset].bAttribute = CONSOLE_DEFAULT_TEXT_COLOR;
                iCurrentOffset++;
                break;
        }

        // Handle scrolling if the offset exceeds the screen size
        if (iCurrentOffset >= (CONSOLE_WIDTH * CONSOLE_HEIGHT)) {
            kMemCpy(pstScreen, 
                pstScreen + CONSOLE_WIDTH, 
                sizeof(CHARACTER) * CONSOLE_WIDTH * (CONSOLE_HEIGHT - 1));
            
            kMemSet(pstScreen + CONSOLE_WIDTH * (CONSOLE_HEIGHT - 1), 0, 
                sizeof(CHARACTER) * CONSOLE_WIDTH);
            
             iCurrentOffset = (CONSOLE_WIDTH * (CONSOLE_HEIGHT - 1));
        }
    }

    // Update the current print offset
    gs_stConsoleManager.iCurrentPrintOffset = iCurrentOffset;

    return iCurrentOffset;
}

// Clear the console screen
void kClearConsole(void) {
    CHARACTER* pstScreen = (CHARACTER*)CONSOLE_VIDEOMEMORYADDRESS;
    int i;

    for (i = 0; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; i++) {
        pstScreen[i].bCharacter = ' ';
        pstScreen[i].bAttribute = CONSOLE_DEFAULT_TEXT_COLOR;
    }

    // Reset the current print offset and cursor position
    kSetCursor(0, 0);
}

// Implementation of getch function
BYTE kGetCh(void) {
    KEYDATA stData;
    while (1) {

        while (1) {
            if (kGetKeyFromKeyQueue(&stData) == TRUE) {
                break;
            }
        }

        if (stData.bFlags & KEY_FLAGS_DOWN) {
            return stData.bASCIICode;
        }
    }
}

// Print character to the console at X, Y position
void kPrintStringXY(int iX, int iY, const char* pcString) {
    CHARACTER* pstScreen = (CHARACTER*)CONSOLE_VIDEOMEMORYADDRESS;
    int i;

    // Calculate the starting offset based on X and Y coordinates
    int iOffset = (iY * CONSOLE_WIDTH) + iX;
    int iLength = kStrLen(pcString);

    for (i = 0; i < iLength; i++) {
        pstScreen[iOffset + i].bCharacter = pcString[i];
        pstScreen[iOffset + i].bAttribute = CONSOLE_DEFAULT_TEXT_COLOR;
    }
}
