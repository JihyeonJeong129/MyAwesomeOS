#include "Types.h"
#include "Keyboard.h"

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

    kPrintString(0, 11, "Switch To IA-32e Mode Success~!!");
    kPrintString(0, 12, "IA-32e C Language Kernel Start....................[PASS]");
    kPrintString(0, 13, "Keyboard Activate.................................[    ]");

    // Activate the keyboard
    if (kActivateKeyboard() == TRUE)
    {
        kPrintString(51, 13, "PASS");
        kChangeKeyboardLED(FALSE, FALSE, FALSE);
    }
    else
    {
        kPrintString(51, 13, "FAIL");
        while (1)
        {
            ;
        }
    }

    while (1)
    {
        if (kIsOutputBufferFull() == TRUE)
        {
            bTemp = kGetKeyboardScanCode();

            if (kConvertScanCodeToASCIICode(bTemp, &vcTemp[0], &bFlag) == TRUE)
            {
                if (bFlag & KEY_FLAGS_DOWN)
                {
                    kPrintString(i++, 14, vcTemp);
                }
            }
        }
    }
}