#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "AssemblyUtility.h"
#include "PIC.h"

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

    kPrintString(0, 11, "Switch To IA-32e Mode Success~!!");
    kPrintString(0, 12, "IA-32e C Language Kernel Start....................[PASS]");
    
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

    kPrintString(0, 16, "Keyboard Activate and Queue Initialize............[    ]");

    // Activate the keyboard
    if (kInitializeKeyboard() == TRUE)
    {
        kPrintString(51, 16, "PASS");
        kChangeKeyboardLED(FALSE, FALSE, FALSE);
    }
    else
    {
        kPrintString(51, 16, "FAIL");
        while (1)
        {
            ;
        }
    }

    kPrintString(0, 17, "PIC Controller And Interrupt Initialize...........[    ]");
    kInitializePIC();
    kMaskPICInterrupt(0);
    kEnableInterrupt();
    kPrintString(51, 17, "PASS");

    while (1)
    {
        if (kGetKeyFromKeyQueue(&stData) == TRUE)
        {
            if (stData.bFlags & KEY_FLAGS_DOWN)
            {
                vcTemp[0] = stData.bASCIICode;
                kPrintString(i++, 18, vcTemp);

                if(vcTemp[0] == '0')
                {
                    // Activate Temp Exception Handler
                    kGenerateDivideError();
                }
            }
        }
    }
}
