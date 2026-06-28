#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Console.h"
#include "Utility.h"
#include "Task.h"
#include "Descriptor.h"

void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode) {
    
    char vcBuffer[3] = {0, };

    vcBuffer[0] = '0' + iVectorNumber / 10;
    vcBuffer[1] = '0' + iVectorNumber % 10;

    //kPrintString(0, 0, "========================================");
    //kPrintString(0, 1, "===Common Exception Handler Execute~!!==");
    //kPrintString(0, 2, "====Interrupt or Exception Occur~!!!====");
    //kPrintString(0, 3, "             Vector: ");
    //kPrintString(21, 3, vcBuffer);
    //kPrintString(0, 4, "========================================");
    //kPrintString(0, 5, "                                        ");

    while (1);
}

void kCommonInterruptHandler(int iVectorNumber) {
    char vcBuffer[] = "[INT:  , ]";

    static int g_iCommonInterruptCount = 0;

    vcBuffer[5] = '0' + iVectorNumber / 10;
    vcBuffer[6] = '0' + iVectorNumber % 10;
    vcBuffer[8] = '0' + g_iCommonInterruptCount;

    g_iCommonInterruptCount = (g_iCommonInterruptCount + 1) % 10;

    //kPrintString(0, 0, "========================================");
    //kPrintString(0, 1, "===Common Interrupt Handler Execute~!!==");
    //kPrintString(0, 2, "====Interrupt or Exception Occur~!!!====");
    //kPrintString(13, 3, vcBuffer);
    //kPrintString(0, 4, "========================================");
    //kPrintString(0, 5, "                                        ");

    // Send EOI to PIC
    kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);
}

void kKeyboardHandler(int iVectorNumber) {
    char vcBuffer[] = "[INT:  , ]";

    static int g_iKeyboardInterruptCount = 0;

    BYTE bTemp;

    vcBuffer[5] = '0' + iVectorNumber / 10;
    vcBuffer[6] = '0' + iVectorNumber % 10;
    vcBuffer[8] = '0' + g_iKeyboardInterruptCount;

    g_iKeyboardInterruptCount = (g_iKeyboardInterruptCount + 1) % 10;

    //kPrintString(0, 0, "=========================================");
    //kPrintString(0, 1, "==Keyboard Interrupt Handler Execute~!!==");
    //kPrintString(0, 2, "====Interrupt or Exception Occur~!!!=====");
    //kPrintString(13, 3, vcBuffer);
    //kPrintString(0, 4, "=========================================");
    //kPrintString(0, 5, "                                        ");

    // Read the scan code from the keyboard controller and 
    // put it into the queue.
    if (kIsOutputBufferFull() == TRUE) {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue(bTemp);
    }

    // Send EOI to PIC
    kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);
}

void kTimerHandler(int iVectorNumber) {
    
    char vcBuffer[] = "[INT:  , ]";

    static int g_iTimerInterruptCount = 0;

    vcBuffer[5] = '0' + iVectorNumber / 10;
    vcBuffer[6] = '0' + iVectorNumber % 10;
    vcBuffer[8] = '0' + g_iTimerInterruptCount;

    g_iTimerInterruptCount = (g_iTimerInterruptCount + 1) % 10;

    //kPrintString(0, 0, "=========================================");
    //kPrintString(0, 1, "==Keyboard Interrupt Handler Execute~!!==");
    //kPrintString(0, 2, "====Interrupt or Exception Occur~!!!=====");
    //kPrintString(13, 3, vcBuffer);
    //kPrintString(0, 4, "=========================================");
    //kPrintString(0, 5, "                                        ");

    // Send EOI to PIC
    kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);

    // Increase the tick count
    g_qwTickCount++;
        
    // Decrease the processor time
    kDecreaseProcessorTime();

    // If the processor time is over, schedule the next task
    if (kIsProcessorTimeExpired() == TRUE) {
        kScheduleInInterrupt();
    }

}
