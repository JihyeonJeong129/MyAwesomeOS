#include "PIT.h"
#include "AssemblyUtility.h"

// Initialize PIT
void kInitializePIT(WORD wCount, BOOL bPeriodic) {
    
    kOutPortByte(PIT_MODE_COMMAND_PORT, PIT_COUNTER0_ONCE); // Latch the current count value
    
    // Set PIT to periodic mode if bPeriodic is TRUE, otherwise set to one-shot mode
    if (bPeriodic) {
        // Set PIT to periodic mode
        kOutPortByte(PIT_MODE_COMMAND_PORT, PIT_COUNTER0_PERIODIC);
    }
    
    // Set the count value for PIT
    kOutPortByte(PIT_COUNTER0_PORT, wCount);
    kOutPortByte(PIT_COUNTER0_PORT, wCount >> 8);
}

// Read the current count value of PIT counter 0
WORD kReadCounter0(void) {

    BYTE bHighByte, bLowByte;
    WORD wTemp = 0;

    // Latch the current count value
    kOutPortByte(PIT_MODE_COMMAND_PORT, PIT_COUNTER0_LATCH);

    // Read the low byte first, then the high byte
    bLowByte = kInPortByte(PIT_COUNTER0_PORT);
    bHighByte = kInPortByte(PIT_COUNTER0_PORT);

    wTemp = (bHighByte << 8) | bLowByte;

    return wTemp;
}

// Wait for a specified count using PIT
void kWaitUsingPIT(WORD wCount) {

    WORD wCurrentCount0;
    WORD wLastCount0;

    kInitializePIT(0, TRUE);

    wLastCount0 = kReadCounter0();

    while (1) {
        wCurrentCount0 = kReadCounter0();

        // Check if the count has decreased by the specified amount
        if ((wLastCount0 - wCurrentCount0) >= wCount) {
            break;
        }
    }
}
