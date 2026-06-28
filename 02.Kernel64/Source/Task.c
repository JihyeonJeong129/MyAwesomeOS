#include "Task.h"
#include "Descriptor.h"
#include "Utility.h"
#include "AssemblyUtility.h"

static SCHEDULER gs_stScheduler;
static TCBPOOLMANAGER gs_stTCBPoolManager;

// Initialize TCB Pool
void kInitializeTCBPool(void) {
    int i;

    kMemSet(&gs_stTCBPoolManager, 0, sizeof(gs_stTCBPoolManager));

    gs_stTCBPoolManager.pstStartAddress = (TCB*)TASK_TCBPOOLADDRESS;
    kMemSet((void*)TASK_TCBPOOLADDRESS, 0, sizeof(TCB) * TASK_MAXCOUNT);

    for (i = 0; i < TASK_MAXCOUNT; i++) {
        gs_stTCBPoolManager.pstStartAddress[i].qwID = i;
        INIT_LIST_HEAD(&gs_stTCBPoolManager.pstStartAddress[i].stLink);
    }

    gs_stTCBPoolManager.iMaxCount = TASK_MAXCOUNT;
    gs_stTCBPoolManager.iAllocatedCount = 1;
}

// Allocate TCB
TCB* kAllocateTCB(void) {
    TCB* pstEmptyTCB;
    int i;

    if (gs_stTCBPoolManager.iUseCount == gs_stTCBPoolManager.iMaxCount) {
        return NULL;
    }

    for (i = 0; i < gs_stTCBPoolManager.iMaxCount; i++) {
        if (gs_stTCBPoolManager.pstStartAddress[i].qwID >> 32 == 0) {
            pstEmptyTCB = &gs_stTCBPoolManager.pstStartAddress[i];
            break;
        }
    }

    pstEmptyTCB->qwID = ((QWORD)gs_stTCBPoolManager.iAllocatedCount << 32) | i;
    gs_stTCBPoolManager.iAllocatedCount++;
    gs_stTCBPoolManager.iUseCount++;

    if (gs_stTCBPoolManager.iAllocatedCount == 0) {
        gs_stTCBPoolManager.iAllocatedCount = 1;
    }

    return pstEmptyTCB;
}

// Free TCB
void kFreeTCB(QWORD qwID) {
    int i;

    i = qwID & 0xFFFFFFFF;

    kMemSet(&gs_stTCBPoolManager.pstStartAddress[i].stContext, 0, sizeof(CONTEXT));
    INIT_LIST_HEAD(&gs_stTCBPoolManager.pstStartAddress[i].stLink);

    gs_stTCBPoolManager.pstStartAddress[i].qwID = i;
    gs_stTCBPoolManager.iUseCount--;
}

// Generate Task
TCB* kCreateTask(QWORD qwFlags, QWORD qwEntryPointAddress) {
    TCB* pstTask;
    void* pvStackAddress;

    pstTask = kAllocateTCB();
    if (pstTask == NULL) {
        return NULL;
    }

    pvStackAddress = (void*)((QWORD)TASK_STACKPOOLADDRESS + (TASK_STACKSIZE * 
        (pstTask->qwID & 0xFFFFFFFF)));

    kSetupTask(pstTask, qwFlags, pstTask->qwID, qwEntryPointAddress, pvStackAddress, TASK_STACKSIZE);

    kAddTaskToReadyList(pstTask);

    return pstTask;
}

// Setup TCB
void kSetupTask(TCB* pstTCB, QWORD qwFlags, QWORD qwID, QWORD qwEntryPointAddress, 
    void* pvStackAddress, QWORD qwStackSize) {
    
    // Initialize context
    kMemSet(&pstTCB->stContext.vqRegister, 0, sizeof(pstTCB->stContext.vqRegister));

    INIT_LIST_HEAD(&pstTCB->stLink);

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


// For Scheduler
void kInitializeScheduler(void) {

    kInitializeTCBPool();

    INIT_LIST_HEAD(&gs_stScheduler.stReadyList);

    gs_stScheduler.pstRunningTask = kAllocateTCB();
    gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;
}

// Set the currently running task
void kSetRunningTask(TCB* pstTask) {
    gs_stScheduler.pstRunningTask = pstTask;
}

// Return the currently running task
TCB* kGetRunningTask(void) {
    return gs_stScheduler.pstRunningTask;
}

// Return the next task to run
TCB* kGetNextTaskToRun(void) {

    LISTHEAD* pstLink;

    pstLink = list_del_header(&gs_stScheduler.stReadyList);

    if (pstLink == NULL) {
        return NULL;
    }

    return list_entry(pstLink, TCB, stLink);
}


// Insert a task into the ready list
void kAddTaskToReadyList(TCB* pstTask) {
    list_add_tail(&pstTask->stLink, &gs_stScheduler.stReadyList);
}

// Switch to the other task
void kSchedule(void) {
    TCB* pstRunningTask, * pstNextTask;
    BOOL bPreviousFlag;

    if (list_empty(&gs_stScheduler.stReadyList) == TRUE) {
        return;
    }

    // Disable interrupts
    bPreviousFlag = kSetInterruptFlag(FALSE);

    pstNextTask = kGetNextTaskToRun();

    if (pstNextTask == NULL) {
        kSetInterruptFlag(bPreviousFlag);
        return;
    }

    pstRunningTask = gs_stScheduler.pstRunningTask;
    kAddTaskToReadyList(pstRunningTask);

    // Switch to the next task
    gs_stScheduler.pstRunningTask = pstNextTask;
    kSwitchContext(&pstRunningTask->stContext, &pstNextTask->stContext);

    // Update the processor time
    gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;

    kSetInterruptFlag(bPreviousFlag);
}

// When occurring a timer interrupt, switch to the other task
BOOL kScheduleInInterrupt(void) {
    TCB* pstRunningTask, * pstNextTask;
    char* pcContextAddress;

    pstNextTask = kGetNextTaskToRun();

    if (pstNextTask == NULL) {
        return FALSE;
    }

    // Switch to the next task
    pcContextAddress = (char*) IST_STARTADDRESS + IST_SIZE - sizeof(CONTEXT);

    pstRunningTask = gs_stScheduler.pstRunningTask;
    kMemCpy(&pstRunningTask->stContext, pcContextAddress, sizeof(CONTEXT));
    kAddTaskToReadyList(pstRunningTask);

    gs_stScheduler.pstRunningTask = pstNextTask;
    kMemCpy(pcContextAddress, &pstNextTask->stContext, sizeof(CONTEXT));

    // Update the processor time
    gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;

    return TRUE;
}

// Decrease the processor time 
void kDecreaseProcessorTime(void) {
    
    if (gs_stScheduler.iProcessorTime > 0) {
        gs_stScheduler.iProcessorTime--;
    }
}

// Return if processor has remaining time
BOOL kIsProcessorTimeExpired(void) {
    if (gs_stScheduler.iProcessorTime > 0) {
        return FALSE;
    }
    return TRUE;
}

