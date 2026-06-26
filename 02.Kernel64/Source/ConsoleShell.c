#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"

// Define the command table
SHELLCOMMANDENTRY gs_vstCommandTable[] = {
    {"help", "Show Help", kHelp},
    {"clear", "Clear Screen", kClearShell},
    {"totalram", "Show Total RAM Size", kShowTotalRAMSize},
    {"strtodhtest", "String to Decimal/Hex Test", kStringToDecimalHexTest},
    {"shutdown", "Shutdown And Reboot", kShutdown},
};


// shell Main function
void kStartConsoleShell(void) {
    char vcCommandBuffer[CONSOLESHELL_MAXCOMMANDBUFFERCOUNT];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;

    // Print the prompt message
    kPrintf(CONSOLESHELL_PROMPTMESSAGE);

    while(1)
    {
        // Get the key from the keyboard
        bKey = kGetCh();

        // Handle backspace
        if (bKey == KEY_BACKSPACE) {
            if (iCommandBufferIndex > 0) {
                iCommandBufferIndex--;
                kGetCursor(&iCursorX, &iCursorY);
                kSetCursor(iCursorX - 1, iCursorY);
                kPrintf(" ");
                kSetCursor(iCursorX - 1, iCursorY);
            }
        }

        // Handle enter key
        else if (bKey == KEY_ENTER) {
            kPrintf("\n");
            
            if(iCommandBufferIndex > 0) {
                vcCommandBuffer[iCommandBufferIndex] = '\0';
                kExecuteCommand(vcCommandBuffer);
            }

            iCommandBufferIndex = 0;
            kPrintf("%s", CONSOLESHELL_PROMPTMESSAGE);
            kMemSet(vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT);
        }

        else if (bKey == KEY_LSHIFT || bKey == KEY_RSHIFT ||
                 bKey == KEY_CAPSLOCK || bKey == KEY_NUMLOCK ||
                 bKey == KEY_SCROLLLOCK) {
            // Ignore these keys
            ;
        }

        // Handle other keys
        else {

            if (bKey == KEY_TAB) {
                bKey = ' ';
            }

            if (iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT - 1) {
                vcCommandBuffer[iCommandBufferIndex++] = bKey;
                kPrintf("%c", bKey);
            }
        }
    }
}


// Execute the command
void kExecuteCommand(const char* pcCommandBuffer) {
    int i;
    int iSpaceIndex;
    int iCommandBufferLength;
    int iCommandLength;
    int iCommandCount;

    iCommandBufferLength = kStrLen(pcCommandBuffer);

    for (iSpaceIndex = 0; iSpaceIndex < iCommandBufferLength; iSpaceIndex++) {
        if (pcCommandBuffer[iSpaceIndex] == ' ') {
            break;
        }
    }

    iCommandLength = iSpaceIndex;
    iCommandCount = sizeof(gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY);

    for (i = 0; i < iCommandCount; i++) {
        iCommandLength = kStrLen(gs_vstCommandTable[i].pcCommand);

        if ((iCommandLength == iSpaceIndex) &&
            (kMemCmp(gs_vstCommandTable[i].pcCommand, pcCommandBuffer, 
                iSpaceIndex) == 0)) {
            gs_vstCommandTable[i].pfFunction(pcCommandBuffer + iSpaceIndex + 1);
            break;
        }
    }

    if (i >= iCommandCount) {
        kPrintf("'%s' is not found.\n", pcCommandBuffer);
    }
}

// Initialize the parameter list
void kInitializeParameter(PARAMETERLIST* pstList, const char* pcParameter) {
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen(pcParameter);
    pstList->iCurrentPosition = 0;
}

// Return parameter contests and length
int kGetNextParameter(PARAMETERLIST* pstList, char* pcParameter) {
    int i;
    int iLength;

    if (pstList->iLength <= 0 || pstList->iCurrentPosition >= pstList->iLength) {
        return 0;
    }

    for (i = pstList->iCurrentPosition; i < pstList->iLength; i++) {
        if (pstList->pcBuffer[i] == ' ') {
            break;
        }
    }

    iLength = i - pstList->iCurrentPosition;

    kMemCpy(pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, iLength);
    pcParameter[iLength] = '\0';

    pstList->iCurrentPosition += (iLength + 1);

    return iLength;
}

// Show help for all commands
void kHelp(const char* pcParameter) {
    int i;
    int iCommandCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;

    kPrintf("=====================================================\n");
    kPrintf("        MyAwesomeOS Shell Help (Version 0.1)\n");
    kPrintf("=====================================================\n");

    iCommandCount = sizeof(gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY);

    for (i = 0; i < iCommandCount; i++) {
        iLength = kStrLen(gs_vstCommandTable[i].pcCommand);

        if (iLength > iMaxCommandLength) {
            iMaxCommandLength = iLength;
        }
    }

    for (i = 0; i < iCommandCount; i++) {
        kPrintf("%s", gs_vstCommandTable[i].pcCommand);
        kGetCursor(&iCursorX, &iCursorY);

        kSetCursor(iMaxCommandLength, iCursorY);
        kPrintf("  -  %s\n", gs_vstCommandTable[i].pcHelp);
    }
}

// Clear the console screen
void kClearShell(const char* pcParameter) {
    kClearConsole();
    kSetCursor(0, 1);
}

// Show total RAM size
void kShowTotalRAMSize(const char* pcParameter) {
    kPrintf("Total RAM Size = %d MB\n", kGetTotalRAMSize());
}

// Test string to decimal and hex conversion
void kStringToDecimalHexTest(const char* pcParameter) {
    char vcParameter[100];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;

    // Initialize the parameter list
    kInitializeParameter(&stList, pcParameter);

    while (1) {
        iLength = kGetNextParameter(&stList, vcParameter);

        if (iLength == 0) {
            break;
        }

        kPrintf("Param %d = '%s', Length = %d, ", iCount + 1, vcParameter, iLength);

        if (kMemCmp(vcParameter, "0x", 2) == 0) {
            lValue = kAToI(vcParameter + 2, 16);
            kPrintf("HEX Value = %q\n", lValue);
        } else {
            lValue = kAToI(vcParameter, 10);
            kPrintf("Decimal Value = %d\n", lValue);
        }

        iCount++;
    }
}

// Reboot the system
void kShutdown(const char* pcParameter) {
    kPrintf("System Shutdown Start...\n");
    kPrintf("Press Any Key To Reboot PC...");
    kGetCh();
    kReboot();
}



