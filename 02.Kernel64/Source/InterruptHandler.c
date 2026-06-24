#include "InterruptHandler.h"
#include "PIC.h"

void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode) {
    
    char vcBuffer[3] = {0, };

    vcBuffer[0] = '0' + iVectorNumber / 10;
    vcBuffer[1] = '0' + iVectorNumber % 10;

    kPrintString(0, 0, "========================================");
    kPrintString(0, 1, "===Common Exception Handler Execute~!!==");
    kPrintString(0, 2, "====Interrupt or Exception Occur~!!!====");
    kPrintString(0, 3, "             Vector: ");
    kPrintString(21, 3, vcBuffer);
    kPrintString(0, 4, "========================================");
    kPrintString(0, 5, "                                        ");

    while (1);
}

void kCommonInterruptHandler(int iVectorNumber) {
    char vcBuffer[] = "[INT:  , ]";

    static int g_iCommonInterruptCount = 0;

    vcBuffer[5] = '0' + iVectorNumber / 10;
    vcBuffer[6] = '0' + iVectorNumber % 10;
    vcBuffer[8] = '0' + g_iCommonInterruptCount;

    g_iCommonInterruptCount = (g_iCommonInterruptCount + 1) % 10;

    kPrintString(0, 0, "========================================");
    kPrintString(0, 1, "===Common Interrupt Handler Execute~!!==");
    kPrintString(0, 2, "====Interrupt or Exception Occur~!!!====");
    kPrintString(13, 3, vcBuffer);
    kPrintString(0, 4, "========================================");
    kPrintString(0, 5, "                                        ");

    // Send EOI to PIC
    kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);
}

void kKeyboardHandler(int iVectorNumber) {
    char vcBuffer[] = "[INT:  , ]";

    static int g_iKeyboardInterruptCount = 0;

    vcBuffer[5] = '0' + iVectorNumber / 10;
    vcBuffer[6] = '0' + iVectorNumber % 10;
    vcBuffer[8] = '0' + g_iKeyboardInterruptCount;

    g_iKeyboardInterruptCount = (g_iKeyboardInterruptCount + 1) % 10;

    kPrintString(0, 0, "=========================================");
    kPrintString(0, 1, "==Keyboard Interrupt Handler Execute~!!==");
    kPrintString(0, 2, "====Interrupt or Exception Occur~!!!=====");
    kPrintString(13, 3, vcBuffer);
    kPrintString(0, 4, "=========================================");
    kPrintString(0, 5, "                                        ");

    // Send EOI to PIC
    kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);
}