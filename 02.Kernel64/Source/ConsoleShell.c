#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "AssemblyUtility.h"
#include "RTC.h"
#include "Task.h"
#include "Synchronization.h"

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
    {"createtask", "Create Test Task ex)createtask 1(type) 10(count)", kCreateTestTask},
    {"changepriority", "Change Task Priority ex)changepriority 1(ID) 2(priority)", kChangeTaskPriority},
    {"showtask", "Show Task List", kShowTaskList},
    {"killtask", "End Task ex)killtask 1(ID) or 0xffffffff(All Task)", kKillTask},
    {"cpuload", "Show Processor Load", kCPULoad},
    {"testmutex", "Test Mutex Function", kTestMutex},
    {"testthread", "Test Thread And Process Function", kTestThread},
    {"testpie", "Test PIE Calculation", kTestPIE},
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
static void kHelp(const char* pcParameter) {
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
static void kClearShell(const char* pcParameter) {
    kClearConsole();
    kSetCursor(0, 1);
}

// Show total RAM size
static void kShowTotalRAMSize(const char* pcParameter) {
    kPrintf("Total RAM Size = %d MB\n", kGetTotalRAMSize());
}

// Test string to decimal and hex conversion
static void kStringToDecimalHexTest(const char* pcParameter) {
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
static void kShutdown(const char* pcParameter) {
    kPrintf("System Shutdown Start...\n");
    kPrintf("Press Any Key To Reboot PC...");
    kGetCh();
    kReboot();
}

static void kSetTimer(const char* pcParameter) {
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
static void kWaitUsingPITCommand(const char* pcParameter) {
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
static void kReadTimeStampCounter(const char* pcParameter) {
    QWORD qwTSC;

    qwTSC = kReadTSC();
    kPrintf("Time Stamp Counter Value = %q\n", qwTSC);
}

// Measure Processor Speed
static void kMeasureProcessorSpeed(const char* pcParameter) {
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
static void kShowDateAndTime(const char* pcParameter) {

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
static void kTestTask(void) {
    int i = 0;

    while (1) {
        kPrintf("[%d] This message is from kTestTask. Press any key to switch "
            "kConsoleShell.\n", i++);
        
        kGetCh();

        kSwitchContext(&(gs_vstTask[1].stContext), &(gs_vstTask[0].stContext));
    }
}

// Create Test Task and Excute Multitasking
static void kCreateTestTask(const char* pcParameter) {
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
               if(kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0,
                    (QWORD)kTestTask1) == NULL) {
                   break;
               }
            }
            kPrintf("Task1 %d Created\n", i);
            break;
            
        case 2:
        default:
            for(i=0; i<iCount; i++) {
                if(kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0,
                    (QWORD)kTestTask2) == NULL) {
                    break;
                }
            }
            
            kPrintf("Task2 %d Created\n", i);
            break;
    }
}

// Task 1
static void kTestTask1(void)
{
    BYTE bData;
    int i = 0, iX = 0, iY = 0, iMargin, j;
    CHARACTER* pstScreen = (CHARACTER*) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;


    pstRunningTask = kGetRunningTask();
    iMargin = (pstRunningTask->qwID & 0xFFFFFFFF) % 10;

    for (j = 0; j < 20000; j++)
    {
        switch (i)
        {
        case 0:
            iX++;
            if (iX >= (CONSOLE_WIDTH - iMargin))
            {
                i = 1;
            }
            break;

        case 1:
            iY++;
            if (iY >= (CONSOLE_HEIGHT - iMargin))
            {
                i = 2;
            }
            break;

        case 2:
            iX--;
            if (iX < iMargin)
            {
                i = 3;
            }
            break;

        case 3:
            iY--;
            if (iX < iMargin)
            {
                i = 0;
            }
            break;
        }

        pstScreen[iY * CONSOLE_WIDTH + iX].bCharacter = bData;
        pstScreen[iY * CONSOLE_WIDTH + iX].bAttribute = bData & 0x0F;
        bData++;

        // kSchedule();
    }

    kExitTask();
}


// Task 2
static void kTestTask2(void)
{
    int i = 0, iOffset;
    CHARACTER* pstScreen = (CHARACTER*) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    char vcData[4] = { '-', '\\', '|', '/' };

    pstRunningTask = kGetRunningTask();
    // Reserve the last row for task activity indicators.  Shell output uses
    // only the rows above it, so scrolling cannot move these indicators.
    iOffset = (CONSOLE_WIDTH * (CONSOLE_HEIGHT - 1)) +
        (((pstRunningTask->qwID & 0xFFFFFFFF) * 2) % CONSOLE_WIDTH);

    while (1)
    {
        pstScreen[iOffset].bCharacter = vcData[i % 4];

        pstScreen[iOffset].bAttribute = (iOffset % 15) + 1;
        i++;

        // kSchedule();
    }
}

// Change Task Priority
static void kChangeTaskPriority(const char* pcParameter) {
    PARAMETERLIST stList;
    char vcID[30];
    char vcPriority[30];
    BYTE bPriority;
    QWORD qwID;

    kInitializeParameter(&stList, pcParameter);
    kGetNextParameter(&stList, vcID);
    kGetNextParameter(&stList, vcPriority);

    if (kMemCmp(vcID, "0x", 2) == 0) {
        qwID = kAToI(vcID + 2, 16);
    } 
    
    else {
        qwID = kAToI(vcID, 10);
    }

    bPriority = kAToI(vcPriority, 10);

    kPrintf("Change Task Priority ID [0x%q] to Priority [%d]\n", qwID, bPriority);

    if (kChangePriority(qwID, bPriority) == TRUE) {
        kPrintf("Change Task Priority Success\n");
    } 
    
    else {
        kPrintf("Change Task Priority Fail\n");
    }
}

// Print Task information
static void kShowTaskList(const char* pcParameter) {
    TCB* pstTask;
    int i = 0;
    int iCount = 0;

    kPrintf("=========== Task Total Count [%d] ===========\n", kGetReadyTaskCount());

    for(i=0; i < TASK_MAXCOUNT; i++) {
        pstTask = kGetTCBInTCBPool(i);
        if(pstTask->qwID >> 32 != 0)
        {
            if ((iCount != 0) && (iCount % 10 == 0)) {
                kPrintf("Press any key to continue... ('q' is exit) : ");
                if (kGetCh() == 'q') {
                    kPrintf("\n");
                    break;
                }
                kPrintf("\n");
            }

            kPrintf("[%d] Task ID[0x%q], Priority[%d], Flags[0x%q], Thread[%d]\n", 1+iCount++,
                pstTask->qwID, GETPRIORITY(pstTask->qwFlags), pstTask->qwFlags,
                list_count(&pstTask->stChildThreadList));
            kPrintf("       Parent PID[0x%q], Memory Address[0x%q], Size[0x%q]\n",
                            pstTask->qwParentProcessID, pstTask->pvMemoryAddress,
                            pstTask->qwMemorySize);
        }
    }
}


static BOOL kParseTaskID(const char* pcBuffer, QWORD* pqwTaskID) {
    QWORD qwValue;
    QWORD qwDigit;
    QWORD qwMaxValue;
    int i;
    int iRadix;

    qwValue = 0;
    qwMaxValue = (QWORD)-1;
    i = 0;
    iRadix = 10;

    if((pcBuffer[0] == '0') &&
       ((pcBuffer[1] == 'x') || (pcBuffer[1] == 'X'))) {
        i = 2;
        iRadix = 16;
    }

    if(pcBuffer[i] == '\0') {
        return FALSE;
    }

    for( ; pcBuffer[i] != '\0'; i++) {
        if((pcBuffer[i] >= '0') && (pcBuffer[i] <= '9')) {
            qwDigit = pcBuffer[i] - '0';
        }
        else if((iRadix == 16) && (pcBuffer[i] >= 'A') &&
                (pcBuffer[i] <= 'F')) {
            qwDigit = pcBuffer[i] - 'A' + 10;
        }
        else if((iRadix == 16) && (pcBuffer[i] >= 'a') &&
                (pcBuffer[i] <= 'f')) {
            qwDigit = pcBuffer[i] - 'a' + 10;
        }
        else {
            return FALSE;
        }

        if(qwDigit >= (QWORD)iRadix) {
            return FALSE;
        }

        if(qwValue > ((qwMaxValue - qwDigit) / iRadix)) {
            return FALSE;
        }

        qwValue = (qwValue * iRadix) + qwDigit;
    }

    *pqwTaskID = qwValue;
    return TRUE;
}


// End child threads first so their result can be printed before the prompt.
static BOOL kEndTaskFromConsoleShell(TCB* pstTask) {
    LISTHEAD* pstThreadLink;
    TCB* pstChildThread;
    QWORD qwTaskID;
    QWORD qwChildThreadID;

    qwTaskID = pstTask->qwID;

    if(pstTask->qwFlags & TASK_FLAGS_PROCESS) {
        while(list_empty(&pstTask->stChildThreadList) == FALSE) {
            pstThreadLink = pstTask->stChildThreadList.pstNext;
            pstChildThread = list_entry(pstThreadLink, TCB, stThreadLink);
            qwChildThreadID = pstChildThread->qwID;

            if(kEndTask(qwChildThreadID) == FALSE) {
                kPrintf("Child Thread ID [0x%q] End Fail\n", qwChildThreadID);
                return FALSE;
            }

            while(kIsTaskExist(qwChildThreadID) == TRUE) {
                kSchedule();
            }

            kPrintf("Child Thread ID [0x%q] End Success\n", qwChildThreadID);
        }
    }

    if(kEndTask(qwTaskID) == FALSE) {
        return FALSE;
    }

    while(kIsTaskExist(qwTaskID) == TRUE) {
        kSchedule();
    }

    return TRUE;
}


// Terminate Task
static void kKillTask(const char* pcParameter) {
    PARAMETERLIST stList;
    char vcID[30];
    QWORD qwID;
    TCB* pstTCB;
    BOOL bProcess;

    kInitializeParameter(&stList, pcParameter);
    if((kGetNextParameter(&stList, vcID) == 0) ||
       (kParseTaskID(vcID, &qwID) == FALSE)) {
        kPrintf("Usage: killtask TaskID | 0xFFFFFFFF\n");
        return;
    }

    if (qwID != 0xFFFFFFFF) {

        pstTCB = kGetTCBInTCBPool(GETTCBOFFSET(qwID));
        if ((pstTCB != NULL) && (pstTCB->qwID >> 32 != 0) &&
            ((pstTCB->qwFlags & TASK_FLAGS_SYSTEM) == 0)) {
            qwID = pstTCB->qwID;
            bProcess = ((pstTCB->qwFlags & TASK_FLAGS_PROCESS) != 0);

            kPrintf("Kill Task ID [0x%q]%s", qwID,
                    (bProcess == TRUE) ? "\n" : " ");

            if(kEndTaskFromConsoleShell(pstTCB) == TRUE) {
                kPrintf("Kill Task Success\n");
            }

            else {
                kPrintf("Kill Task Fail\n");
            }
        }
        else{
            kPrintf("Task does not exist or task is system task\n");
        }
    } 
    else {
        for (int i = 0; i < TASK_MAXCOUNT; i++) {
            TCB* pstTask = kGetTCBInTCBPool(i);
            if ((pstTask->qwID >> 32 != 0) && ((pstTask->qwFlags & TASK_FLAGS_SYSTEM) == 0x00)) {
                qwID = pstTask->qwID;
                bProcess = ((pstTask->qwFlags & TASK_FLAGS_PROCESS) != 0);

                kPrintf("Kill Task ID [0x%q]%s", qwID,
                        (bProcess == TRUE) ? "\n" : " ");
                if(kEndTaskFromConsoleShell(pstTask) == TRUE) {
                    kPrintf("Kill Task Success\n");
                } 
                
                else {
                    kPrintf("Kill Task Fail\n");
                }
            }
        }
    }
}


// Show CPU Load
static void kCPULoad(const char* pcParameter) {
    kPrintf("Processor Load: %d%%\n",kGetProcessorLoad());
}


static MUTEX gs_stMutex;
static volatile QWORD gs_qwAdder;

static void kPrintNumberTask(void) {
    int i;
    int j;
    QWORD qwTickCount;

    qwTickCount = kGetTickCount();
    while (kGetTickCount() - qwTickCount < 50) {
        kSchedule();
    }

    for (i = 0; i < 5; i++) {
        kLock(&gs_stMutex);
        kPrintf("Task ID [0x%q] Value [%d]\n", kGetRunningTask()->qwID, gs_qwAdder);
        gs_qwAdder += 1;
        kUnlock(&gs_stMutex);

        for (j = 0; j < 30000; j++) {
            ;
        }
    }

    qwTickCount = kGetTickCount();
    while (kGetTickCount() - qwTickCount < 1000) {
        kSchedule();
    }

    kExitTask();
}

static void kTestMutex(const char* pcParameter) {
    int i;

    kInitializeMutex(&gs_stMutex);
    gs_qwAdder = 1;

    for (i = 0; i < 3; i++) {
        kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, (QWORD)kPrintNumberTask);
    }

    kPrintf("Wait Util %d Task End...\n", i);
    kGetCh();
}

static void kCreateThreadTask(void) {
    int i;
    for (i = 0; i < 3; i++) {
        kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, (QWORD)kTestTask2);
    }

    while (1) {
        kSleep(1);
    }
}

static void kTestThread(const char* pcParameterBuffer) {
    TCB* pstProcess;

    (void)pcParameterBuffer;

    pstProcess = kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_PROCESS, (void *)0xEEEEEEEE, 0x1000,
                            (QWORD)kCreateThreadTask);

    if(pstProcess == NULL) {
        kPrintf("Process Create Fail\n");
    }
    else {
        kPrintf("Process Create Success: [0x%q]\n", pstProcess->qwID);
    }
}

static volatile QWORD gs_qwRandomValue = 0;

QWORD kRandom(void) {
    gs_qwRandomValue =
        (gs_qwRandomValue * 412153 + 5571031) >> 16;

    return gs_qwRandomValue;
}

static void kFPUTestTask(void) {
    double dValue1;
    double dValue2;
    TCB* pstRunningTask;
    QWORD qwCount = 0;
    QWORD qwRandomValue;
    int i;
    int iOffset;
    char vcData[4] = {'-', '\\', '|', '/'};
    CHARACTER* pstScreen = (CHARACTER*) CONSOLE_VIDEOMEMORYADDRESS;

    pstRunningTask = kGetRunningTask();

    iOffset = (pstRunningTask->qwID & 0xFFFFFFFF) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - 
        (iOffset % (CONSOLE_WIDTH - CONSOLE_HEIGHT));

    while(1){
        dValue1 = 1;
        dValue2 = 1;

        for(i=0; i<10; i++){
            qwRandomValue = kRandom();
            dValue1 *= (double) qwRandomValue;
            dValue2 *= (double) qwRandomValue;

            kSleep(1);

            qwRandomValue = kRandom();
            dValue1 /= (double) qwRandomValue;
            dValue2 /= (double) qwRandomValue;
        }

        if(dValue1 != dValue2){
            kPrintf("Value is Not same. [%f] != [%f]\n, dValue1, dValue2");
            break;
        }

        qwCount++;

        pstScreen[iOffset].bCharacter = vcData[qwCount % 4];
        pstScreen[iOffset].bAttribute = (iOffset % 15) + 1;
    }
}

static void kTestPIE(const char* pcParameterBuffer){
    double dResult;
    int i;

    kPrintf("PIE Calculation Test\n");
    kPrintf("Result: 355 / 113 = ");
    dResult = (double) 355 / 113;
    kPrintf("%d.%d%d\n", (QWORD)dResult, ((QWORD)(dResult * 10) % 10),
            ((QWORD)(dResult * 100) % 10));

    for(i=0; i<100; i++){
        kCreateTask(TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, (QWORD)kFPUTestTask);
    }
}
