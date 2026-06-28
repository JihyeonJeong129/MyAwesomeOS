#include "Task.h"
#include "Descriptor.h"
#include "Utility.h"

// Setup TCB
void kSetupTask(TCB* pstTCB, QWORD qwFlags, QWORD qwID, QWORD qwEntryPointAddress, 
    void* pvStackAddress, QWORD qwStackSize) {
    
    // Initialize context
    kMemSet(&pstTCB->stContext.vqRegister, 0, sizeof(pstTCB->stContext.vqRegister));

    // Set the instruction pointer to the entry point address
    // Set stack pointer to the top of the stack
    pstTCB->stContext.vqRegister[TASK_RSP_OFFSET] = (QWORD)pvStackAddress + qwStackSize; 

    // Set base pointer to the top of the stack
    pstTCB->stContext.vqRegister[TASK_RBP_OFFSET] = (QWORD)pvStackAddress + qwStackSize;
    
    // Set segment registers to kernel segments
    pstTCB->stContext.vqRegister[TASK_CS_OFFSET] = GDT_KERNELCODESEGMENT;
    pstTCB->stContext.vqRegister[TASK_DS_OFFSET] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[TASK_ES_OFFSET] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[TASK_FS_OFFSET] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[TASK_GS_OFFSET] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[TASK_SS_OFFSET] = GDT_KERNELDATASEGMENT;

    // Set RIP and Interrupt flag
    pstTCB->stContext.vqRegister[TASK_RIP_OFFSET] = qwEntryPointAddress;

    pstTCB->stContext.vqRegister[TASK_RFLAGS_OFFSET] |= 0x0200;
    
    // Set task flags and ID
    pstTCB->qwFlags = qwFlags;
    pstTCB->qwID = qwID;

    // Store stack information
    pstTCB->pvStackAddress = pvStackAddress;
    pstTCB->qwStackSize = qwStackSize;
}
