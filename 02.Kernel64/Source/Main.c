#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "AssemblyUtility.h"
#include "PIC.h"
#include "Console.h"
#include "ConsoleShell.h"
#include "Utility.h"
#include "Task.h"
#include "PIT.h"

void kPrintString(int iX, int iY, const char* pcString)
{
    CHARACTER* pstScreen = (CHARACTER*) 0xB8000;
    int i;

    pstScreen += (iY * 80) + iX;

    for (i = 0; pcString[i] != 0; i++)
    {
        pstScreen[i].bCharacter = pcString[i];
    }
}

void Main(void)
{
    char vcTemp[2] = {0, };
    BYTE bFlag;
    BYTE bTemp;
    int i = 0;
    KEYDATA stData;

    kPrintf("Switch To IA-32e Mode Success~!!\n");
    kPrintf("IA-32e C Language Kernel Start....................[PASS]\n");
    
    kPrintString(0, 13, "GDT Initialize And Switch For IA-32e Mode ........[    ]");
    kInitializeGDTTablesAndTSS();
    kLoadGDTR(GDTR_STARTADDRESS);
    kPrintString(51, 13, "PASS");

    kPrintString(0, 14, "TSS Segment Load..................................[    ]");
    kLoadTR(GDT_TSSSEGMENT);
    kPrintString(51, 14, "PASS");

    kPrintString(0, 15, "IDT Initialize....................................[    ]");
    kInitializeIDTTables();
    kLoadIDTR(IDTR_STARTADDRESS);
    kPrintString(51, 15, "PASS");

    kSetCursor(0, 16);
    kPrintf("Total RAM Size check.............. [                   ]");
    kCheckTotalRAMSize();
    kSetCursor(36, 16);
    kPrintf("PASS, Size = %d MB\n", kGetTotalRAMSize());

    kPrintf("TCB Pool And Scheduler Initialize.................[    ]");
    kInitializeScheduler();
    kSetCursor(51, 17);
    kPrintf("PASS\n");
    kInitializePIT(MSTOCOUNT(1), 1);

    kPrintf("Keyboard Activate and Queue Initialize............[    ]");

    // Activate the keyboard
    if (kInitializeKeyboard() == TRUE)
    {
        kSetCursor(51, 18);
        kPrintf("PASS\n");
        kChangeKeyboardLED(FALSE, FALSE, FALSE);
    }
    else
    {
        kSetCursor(51, 18);
        kPrintf("FAIL\n");
        while (1)
        {
            ;
        }
    }

    kPrintf("PIC Controller And Interrupt Initialize...........[    ]");
    kInitializePIC();
    kMaskPICInterrupt(0);
    kEnableInterrupt();
    kSetCursor(51, 19);
    kPrintf("PASS\n");

    kCreateTask(TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD | TASK_FLAGS_SYSTEM | TASK_FLAGS_IDLE,
                0, 0, (QWORD) kIdleTask);

    kStartConsoleShell();
}
