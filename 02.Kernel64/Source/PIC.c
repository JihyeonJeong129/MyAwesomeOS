#include "PIC.h"

// Initialize the PIC
void kInitializePIC(void) {
    // Initialize the Master PIC
    kOutPortByte(PIC_MASTER_PORT1, 0x11);
    kOutPortByte(PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR);
    kOutPortByte(PIC_MASTER_PORT2, 0x04);
    kOutPortByte(PIC_MASTER_PORT2, 0x01);

    // Initialize the Slave PIC
    kOutPortByte(PIC_SLAVE_PORT1, 0x11);
    kOutPortByte(PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR + 8);
    kOutPortByte(PIC_SLAVE_PORT2, 0x02);
    kOutPortByte(PIC_SLAVE_PORT2, 0x01);
}

// Mask PIC Interrupts
void kMaskPICInterrupt(WORD wIRQBitmask) {
    // Mask the Master PIC
    kOutPortByte(PIC_MASTER_PORT2, (BYTE) wIRQBitmask);

    // Mask the Slave PIC
    kOutPortByte(PIC_SLAVE_PORT2, (BYTE) (wIRQBitmask >> 8));
}

// Send EOI to PIC
void kSendEOIToPIC(int iIRQNumber) {
    // Send EOI to the Master PIC
    kOutPortByte(PIC_MASTER_PORT1, 0x20);

    // If the IRQ number is for the Slave PIC, send EOI to the Slave PIC
    if (iIRQNumber >= 8) {
        kOutPortByte(PIC_SLAVE_PORT1, 0x20);
    }
}