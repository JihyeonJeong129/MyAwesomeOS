#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "AssemblyUtility.h"
#include "RTC.h"

// Define the command table
SHELLCOMMANDENTRY gs_vstCommandTable[] = {
    {"help", "Show Help", kHelp},
    {"clear", "Clear Screen", kClearShell},
    {"totalram", "Show Total RAM Size", kShowTotalRAMSize},
    {"strtodhtest", "String to Decimal/Hex Test", kStringToDecimalHexTest},
    {"shutdown", "Shutdown And Reboot", kShutdown},
    {"settimer", "Set PIT Controller Counter0 to Desired Value", kSetTimer},
    {"wait", "Wait Using PIT Controller", kWaitUsingPITCommand},
    {"rdtsc", "Read Time Stamp Counter", kReadTimeStampCounter},
    {"cpuspeed", "Measure Processor Speed", kMeasureProcessorSpeed},
    {"showtime", "Show Date And Time", kShowDateAndTime},
    {"createtask", "Create Test Task ex)createtask 1(type) 10(count)", kCreateTestTask}
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

void kSetTimer(const char* pcParameter) {
    char vcParameter[100];
    PARAMETERLIST stList;
    int iLength;
    long lValue;
    BOOL bPeriodic;

    kInitializeParameter(&stList, pcParameter);

    iLength = kGetNextParameter(&stList, vcParameter);

    if (iLength == 0) {
        kPrintf("ex) settimer 100(ms)\n");
        return;
    }

    lValue = kAToI(vcParameter, 10);

    if(kGetNextParameter(&stList, vcParameter) == 0) {
        kPrintf("ex) settimer 100(ms)\n");
        return;
    }

    bPeriodic = kAToI(vcParameter, 10);

    kInitializePIT(MSTOCOUNT(lValue), bPeriodic);
    kPrintf("Time %d ms, Periodic Mode = %d\n", lValue, bPeriodic);
}

// Wait using PIT
void kWaitUsingPITCommand(const char* pcParameter) {
    char vcParameter[100];
    PARAMETERLIST stList;
    int iLength;
    long lMillisecond;
    int i;

    kInitializeParameter(&stList, pcParameter);

    if (kGetNextParameter(&stList, vcParameter) == 0)
    {
        kPrintf("ex) wait 100(ms)\n");
        return;
    }

    lMillisecond = kAToI(vcParameter, 10);
    kPrintf("Wait %d ms...", lMillisecond);

    for (i = 0; i < lMillisecond / 30; i++) {
        kWaitUsingPIT(MSTOCOUNT(30));
    }

    kWaitUsingPIT(MSTOCOUNT(lMillisecond % 30));
    kEnableInterrupt();

    kPrintf("%d sleep complete\n", lMillisecond);

    kInitializePIT(MSTOCOUNT(1), TRUE);
}
 
// Read Time Stamp Counter
void kReadTimeStampCounter(const char* pcParameter) {
    QWORD qwTSC;

    qwTSC = kReadTSC();
    kPrintf("Time Stamp Counter Value = %q\n", qwTSC);
}

// Measure Processor Speed
void kMeasureProcessorSpeed(const char* pcParameter) {
    int i;
    QWORD qwLastTSC, qwTotalTSC = 0;

    kPrintf("Now Measuring.");

    kDisableInterrupt();

    for (i = 0; i < 200; i++) {
        qwLastTSC = kReadTSC();
        kWaitUsingPIT(MSTOCOUNT(50));
        qwTotalTSC += kReadTSC() - qwLastTSC;

        kPrintf(".");
    }

    kInitializePIT(MSTOCOUNT(1), TRUE);

    kEnableInterrupt();
    kPrintf("\nCPU Speed = %d MHz\n", qwTotalTSC / 10000000);
}

// Show Date and Time
void kShowDateAndTime(const char* pcParameter) {

    BYTE bSecond, bMinute, bHour;
    BYTE bDayOfWeek, bDayOfMonth, bMonth;
    WORD wYear;

    // Read the current date and time from CMOS
    kReadRTCTime(&bHour, &bMinute, &bSecond);
    kReadRTCDate(&wYear, &bMonth, &bDayOfMonth, &bDayOfWeek);

    kPrintf("Date: %d/%d/%d\n", wYear, bMonth, bDayOfMonth);
    kPrintf("Time: %d:%d:%d\n", bHour, bMinute, bSecond);
}

// Define TCB and stack for test task
static TCB gs_vstTask[2] = {0, };
static QWORD gs_vstStack[1024] = {0, };

// Create Test Task
void kTestTask(void) {
    int i = 0;

    while (1) {
        kPrintf("[%d] This message is from kTestTask. Press any key to switch "
            "kConsoleShell.\n", i++);
        
        kGetCh();

        kSwitchContext(&(gs_vstTask[1].stContext), &(gs_vstTask[0].stContext));
    }
}

// Create Test Task and Excute Multitasking
void kCreateTestTask(const char* pcParameter) {
    PARAMETERLIST stList;
    char vcType[30];
    char vcCount[30];
    int i = 0;
    int iCount;
    int iClearStartOffset;
    int iClearCount;
    BOOL bPreviousInterruptFlag;
    CHARACTER* pstScreen = (CHARACTER*)CONSOLE_VIDEOMEMORYADDRESS;

    kInitializeParameter(&stList, pcParameter);
    kGetNextParameter(&stList, vcType);
    kGetNextParameter(&stList, vcCount);
    iCount = kAToI(vcCount, 10);

    switch(kAToI(vcType, 10)) {
        case 1:
            for(i=0; i<iCount; i++) {
               if(kCreateTask(0, (QWORD)kTestTask1) == NULL) {
                   kPrintf("Task Create Fail\n");
                   return;
               }
            }
            break;
            
        case 2:
        default:
            bPreviousInterruptFlag = kSetInterruptFlag(FALSE);

            iClearCount = iCount * 2;
            if (iClearCount > CONSOLE_WIDTH * CONSOLE_HEIGHT) {
                iClearCount = CONSOLE_WIDTH * CONSOLE_HEIGHT;
            }

            iClearStartOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - iClearCount;
            for (i = iClearStartOffset; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; i++) {
                pstScreen[i].bCharacter = ' ';
                pstScreen[i].bAttribute = CONSOLE_DEFAULT_TEXT_COLOR;
            }

            for(i=0; i<iCount; i++) {
               if(kCreateTask(0, (QWORD)kTestTask2) == NULL) {
                   kPrintf("Task Create Fail\n");
                   kSetInterruptFlag(bPreviousInterruptFlag);
                   return;
               }
            }

            kSetInterruptFlag(bPreviousInterruptFlag);
            break;
    }
}

// Task 1
void kTestTask1(void) {
    BYTE bData;
    int i = 0;
    int iX = 0, iY = 0;
    int iMargin;
    int j;

    CHARACTER* pstScreen = (CHARACTER*)CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;

    pstRunningTask = kGetRunningTask();
    iMargin = (pstRunningTask->qwID  & 0xFFFFFFFF) % 10;
    iX = iMargin;
    iY = iMargin;
    bData = 'A' + iMargin;

    for (j = iMargin; j < CONSOLE_WIDTH - iMargin; j++) {
        pstScreen[iMargin * CONSOLE_WIDTH + j].bCharacter = ' ';
        pstScreen[iMargin * CONSOLE_WIDTH + j].bAttribute = CONSOLE_DEFAULT_TEXT_COLOR;
        pstScreen[(CONSOLE_HEIGHT - 1 - iMargin) * CONSOLE_WIDTH + j].bCharacter = ' ';
        pstScreen[(CONSOLE_HEIGHT - 1 - iMargin) * CONSOLE_WIDTH + j].bAttribute =
            CONSOLE_DEFAULT_TEXT_COLOR;
    }

    for (j = iMargin; j < CONSOLE_HEIGHT - iMargin; j++) {
        pstScreen[j * CONSOLE_WIDTH + iMargin].bCharacter = ' ';
        pstScreen[j * CONSOLE_WIDTH + iMargin].bAttribute = CONSOLE_DEFAULT_TEXT_COLOR;
        pstScreen[j * CONSOLE_WIDTH + CONSOLE_WIDTH - 1 - iMargin].bCharacter = ' ';
        pstScreen[j * CONSOLE_WIDTH + CONSOLE_WIDTH - 1 - iMargin].bAttribute =
            CONSOLE_DEFAULT_TEXT_COLOR;
    }

    while (1) {
        
        switch (i)
        {
        case 0:
            iX++;
            if (iX >= (CONSOLE_WIDTH - 1 - iMargin)) {
                i=1;
            }
            break;

            case 1:
                iY++;
                if (iY >= (CONSOLE_HEIGHT - 1 - iMargin)) {
                    i=2;
            }
            break;

            case 2:
                iX--;
                if (iX <= iMargin) {
                    i=3;
            }
            break;

            case 3:
                iY--;
                if (iY <= iMargin) {
                    i=0;
            }
            break;
        }

        pstScreen[iY * CONSOLE_WIDTH + iX].bCharacter = bData;
        pstScreen[iY * CONSOLE_WIDTH + iX].bAttribute = bData & 0x0F;
        bData++;

        kSchedule();
    }
}


// Task 2
void kTestTask2(void) {
    int i = 0;
    int iOffset;
    CHARACTER* pstScreen = (CHARACTER*)CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    char vcData[4] = {'-', '\\', '|', '/'};

    pstRunningTask = kGetRunningTask();
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - 1 -
        (((pstRunningTask->qwID & 0xFFFFFFFF) * 2) %
        (CONSOLE_WIDTH * CONSOLE_HEIGHT));

    pstScreen[iOffset].bCharacter = ' ';
    pstScreen[iOffset].bAttribute = CONSOLE_DEFAULT_TEXT_COLOR;

    while (1) {
        pstScreen[iOffset].bCharacter = vcData[i % 4];
        pstScreen[iOffset].bAttribute = (iOffset % 15) + 1;
        i++;

        kSchedule();
    }
}
