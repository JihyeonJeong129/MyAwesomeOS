#include "Task.h"
#include "Descriptor.h"
#include "Utility.h"
#include "AssemblyUtility.h"
#include "Synchronization.h"
#include "Console.h"

static SCHEDULER gs_stScheduler;
static TCBPOOLMANAGER gs_stTCBPoolManager;
static TCB* kGetProcessByThread(TCB* pstThread);

// Initialize TCB Pool
static void kInitializeTCBPool(void) {
    int i;

    kMemSet(&gs_stTCBPoolManager, 0, sizeof(gs_stTCBPoolManager));

    gs_stTCBPoolManager.pstStartAddress = (TCB*)TASK_TCBPOOLADDRESS;
    kMemSet((void*)TASK_TCBPOOLADDRESS, 0, sizeof(TCB) * TASK_MAXCOUNT);

    for (i = 0; i < TASK_MAXCOUNT; i++) {
        gs_stTCBPoolManager.pstStartAddress[i].qwID = i;
        INIT_LIST_HEAD(&gs_stTCBPoolManager.pstStartAddress[i].stLink);
        INIT_LIST_HEAD(&gs_stTCBPoolManager.pstStartAddress[i].stThreadLink);
        INIT_LIST_HEAD(&gs_stTCBPoolManager.pstStartAddress[i].stChildThreadList);
    }

    gs_stTCBPoolManager.iMaxCount = TASK_MAXCOUNT;
    gs_stTCBPoolManager.iAllocatedCount = 1;
}

// Allocate TCB
static TCB* kAllocateTCB(void) {
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
static void kFreeTCB(QWORD qwID) {
    int i;

    i = GETTCBOFFSET(qwID);

    kMemSet(&gs_stTCBPoolManager.pstStartAddress[i].stContext, 0, sizeof(CONTEXT));
    INIT_LIST_HEAD(&gs_stTCBPoolManager.pstStartAddress[i].stLink);
    INIT_LIST_HEAD(&gs_stTCBPoolManager.pstStartAddress[i].stThreadLink);
    INIT_LIST_HEAD(&gs_stTCBPoolManager.pstStartAddress[i].stChildThreadList);

    gs_stTCBPoolManager.pstStartAddress[i].qwID = i;
    gs_stTCBPoolManager.iUseCount--;
}

// Generate Task
TCB* kCreateTask(QWORD qwFlags, void* pvMemoryAddress, QWORD qwMemorySize,
                 QWORD qwEntryPointAddress) {
    TCB* pstTask, * pstProcess;
    void* pvStackAddress;
    BOOL bPreviousFlag;


    bPreviousFlag = kLockForSystemData();
    pstTask = kAllocateTCB();
    if (pstTask == NULL) {
        kUnlockForSystemData(bPreviousFlag);
        return NULL;
    }

    INIT_LIST_HEAD(&pstTask->stThreadLink);
    INIT_LIST_HEAD(&pstTask->stChildThreadList);

    // Search process or thread which includes the task to be created
    pstProcess = kGetProcessByThread(kGetRunningTask());

    // Case not exist process or thread
    if(pstProcess == NULL){
        kFreeTCB(pstTask->qwID);
        kUnlockForSystemData(bPreviousFlag);
        return NULL;
    }

    // Case creating a thread
    if(qwFlags & TASK_FLAGS_THREAD) {
        pstTask->qwParentProcessID = pstProcess->qwID;
        pstTask->pvMemoryAddress = pstProcess->pvMemoryAddress;
        pstTask->qwMemorySize = pstProcess->qwMemorySize;

        list_add_tail(&pstTask->stThreadLink, &pstProcess->stChildThreadList);
    }

    else{
        pstTask->qwParentProcessID = pstProcess->qwID;
        pstTask->pvMemoryAddress = pvMemoryAddress;
        pstTask->qwMemorySize = qwMemorySize;
    }

    pvStackAddress = (void*)((QWORD)TASK_STACKPOOLADDRESS + (TASK_STACKSIZE * 
        GETTCBOFFSET(pstTask->qwID)));

    pstTask->bFPUUsed = FALSE;
    
    kSetupTask(pstTask, qwFlags, pstTask->qwID, qwEntryPointAddress, pvStackAddress, TASK_STACKSIZE);
    kAddTaskToReadyList(pstTask);
    kUnlockForSystemData(bPreviousFlag);

    return pstTask;
}

// Setup TCB
static void kSetupTask(TCB* pstTCB, QWORD qwFlags, QWORD qwID, QWORD qwEntryPointAddress, 
    void* pvStackAddress, QWORD qwStackSize) {
    
    // Initialize context
    kMemSet(&pstTCB->stContext.vqRegister, 0, sizeof(pstTCB->stContext.vqRegister));

    pstTCB->stContext.vqRegister[TASK_RSP_OFFSET] = (QWORD)pvStackAddress + qwStackSize - 8;
    pstTCB->stContext.vqRegister[TASK_RBP_OFFSET] = (QWORD)pvStackAddress + qwStackSize - 8;

    *(QWORD*)((QWORD)pvStackAddress + qwStackSize - 8) = (QWORD)kExitTask;

    INIT_LIST_HEAD(&pstTCB->stLink);

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

    int i;
    TCB* pstTask;

    kMemSet(&gs_stScheduler, 0, sizeof(gs_stScheduler));
    
    kInitializeTCBPool();

    for (i = 0; i < TASK_READYLIST_MAXCOUNT; i++) {
        INIT_LIST_HEAD(&gs_stScheduler.vstReadyList[i]);
        gs_stScheduler.viExecutionCount[i] = 0;
    }

    INIT_LIST_HEAD(&gs_stScheduler.stWaitList);

    // Allocate TCB for setting init process which is the booting task
    pstTask = kAllocateTCB();
    gs_stScheduler.pstRunningTask = pstTask;
    pstTask->qwFlags = TASK_FLAGS_HIGHEST | TASK_FLAGS_PROCESS | TASK_FLAGS_SYSTEM;
    pstTask->qwParentProcessID = pstTask->qwID;
    pstTask->pvMemoryAddress = (void*)0x100000;
    pstTask->qwMemorySize = 0x500000;
    pstTask->pvStackAddress = (void*)0x600000;
    pstTask->qwStackSize = 0x100000;
    INIT_LIST_HEAD(&pstTask->stThreadLink);
    INIT_LIST_HEAD(&pstTask->stChildThreadList);

    // Initialize the processor time and load
    gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;
    gs_stScheduler.qwProcessorLoad = 0;
    gs_stScheduler.qwSpendProcessorTimeInIdleTask = 0;

    gs_stScheduler.qwLastFPUUsedTaskID = TASK_INVALIDID;
}

// Set the currently running task
void kSetRunningTask(TCB* pstTask) {
    BOOL bPreviousFlag;

    bPreviousFlag = kLockForSystemData();
    gs_stScheduler.pstRunningTask = pstTask;
    kUnlockForSystemData(bPreviousFlag);
}

// Return the currently running task
TCB* kGetRunningTask(void) {
    
    BOOL bPreviousFlag;
    TCB* pstRunningTask;
    
    bPreviousFlag = kLockForSystemData();
    pstRunningTask = gs_stScheduler.pstRunningTask;
    kUnlockForSystemData(bPreviousFlag);

    return pstRunningTask;
}

// Return the next task to run
static TCB* kGetNextTaskToRun(void) {

    TCB* pstTargetTask = NULL;
    LISTHEAD* pstLink;
    int iTaskCount, i, j;

    for(j=0; j<2; j++){
        
        for (i = 0; i < TASK_READYLIST_MAXCOUNT; i++) {
            
            iTaskCount = list_count(&gs_stScheduler.vstReadyList[i]);
            
            if (gs_stScheduler.viExecutionCount[i] < iTaskCount) {
                pstLink = list_del_header(&gs_stScheduler.vstReadyList[i]);
                pstTargetTask = list_entry(pstLink, TCB, stLink);
                gs_stScheduler.viExecutionCount[i]++;
                break;
            }

            else{
                gs_stScheduler.viExecutionCount[i] = 0;
            }
        }

        if(pstTargetTask != NULL){
            break;
        }
    }

    return pstTargetTask;
}


// Insert a task into the ready list
static BOOL kAddTaskToReadyList(TCB* pstTask) {
    BYTE bPriority;
    bPriority = GETPRIORITY(pstTask->qwFlags);

    if(bPriority >= TASK_READYLIST_MAXCOUNT){
        return FALSE;
    }

    list_add_tail(&pstTask->stLink, &gs_stScheduler.vstReadyList[bPriority]);
    return TRUE;
}

// Remove a task from the ready list
static TCB* kRemoveTaskFromReadyList(QWORD qwTaskID) {
    TCB* pstTargetTask;
    TCB* pstCurrentTask;
    LISTHEAD* pstLink;
    LISTHEAD* pstNextLink;
    BYTE bPriority;

    if (GETTCBOFFSET(qwTaskID) >= TASK_MAXCOUNT) {
        return NULL;
    }

    // Find task TCB by ID
    pstTargetTask = &gs_stTCBPoolManager.pstStartAddress[GETTCBOFFSET(qwTaskID)];
    if (pstTargetTask->qwID != qwTaskID) {
        return NULL;
    }

    bPriority = GETPRIORITY(pstTargetTask->qwFlags);

    list_for_each_safe(pstLink, pstNextLink, &gs_stScheduler.vstReadyList[bPriority]) {
        pstCurrentTask = list_entry(pstLink, TCB, stLink);

        if (pstCurrentTask == pstTargetTask) {
            list_del(pstLink);
            return pstCurrentTask;
        }
    }

    return NULL;
}


// Change the priority of a task
BOOL kChangePriority(QWORD qwTaskID, BYTE bPriority) {
    TCB* pstTargetTask;
    BOOL bPreviousFlag;

    if (bPriority >= TASK_READYLIST_MAXCOUNT) {
        return FALSE;
    }

    bPreviousFlag = kLockForSystemData();
   
    pstTargetTask = gs_stScheduler.pstRunningTask;


    if (pstTargetTask->qwID == qwTaskID) {
        SETPRIORITY(pstTargetTask->qwFlags, bPriority);
    }

    else {
        pstTargetTask = kRemoveTaskFromReadyList(qwTaskID);
        if (pstTargetTask == NULL) {
            
            pstTargetTask = kGetTCBInTCBPool(GETTCBOFFSET(qwTaskID));
            
            if (pstTargetTask != NULL) {
                SETPRIORITY(pstTargetTask->qwFlags, bPriority);
            }
        }

        else{
            SETPRIORITY(pstTargetTask->qwFlags, bPriority);
            kAddTaskToReadyList(pstTargetTask);
        }
    }

    kUnlockForSystemData(bPreviousFlag);
    return TRUE;
}


// Switch to the other task
void kSchedule(void) {
    TCB* pstRunningTask, * pstNextTask;
    BOOL bPreviousFlag;

    if (kGetReadyTaskCount() < 1) {
        return;
    }

    // Disable interrupts
    bPreviousFlag = kLockForSystemData(); 

    pstNextTask = kGetNextTaskToRun();

    if (pstNextTask == NULL) {
        kUnlockForSystemData(bPreviousFlag);
        return;
    }

    // Edit the currently running task's context and switch to the next task
    pstRunningTask = gs_stScheduler.pstRunningTask;
    gs_stScheduler.pstRunningTask = pstNextTask;

    if ((pstRunningTask->qwFlags & TASK_FLAGS_IDLE) == TASK_FLAGS_IDLE) {
        gs_stScheduler.qwSpendProcessorTimeInIdleTask += 
            TASK_PROCESSORTIME - gs_stScheduler.iProcessorTime;
    }

    if(gs_stScheduler.qwLastFPUUsedTaskID != pstNextTask->qwID){
        kSetTS();
    }

    else{
        kClearTS();
    }

    // IF set the end flag, insert the task into the wait list
    // and do not add it to the ready list
    if ((pstRunningTask->qwFlags & TASK_FLAGS_ENDTASK) == TASK_FLAGS_ENDTASK) {
        list_add_tail(&pstRunningTask->stLink, &gs_stScheduler.stWaitList);
        kSwitchContext(NULL, &pstNextTask->stContext);
    }

    else {
        kAddTaskToReadyList(pstRunningTask);
        kSwitchContext(&pstRunningTask->stContext, &pstNextTask->stContext);
    }

    // Update the processor time
    gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;

    kUnlockForSystemData(bPreviousFlag);
}

// When occurring a timer interrupt, switch to the other task
BOOL kScheduleInInterrupt(void) {
    TCB* pstRunningTask, * pstNextTask;
    char* pcContextAddress;
    BOOL bPreviousFlag;

    bPreviousFlag = kLockForSystemData();
    pstNextTask = kGetNextTaskToRun();

    if (pstNextTask == NULL) {
        kUnlockForSystemData(bPreviousFlag);
        return FALSE;
    }

    // Switch to the next task
    pcContextAddress = (char*) IST_STARTADDRESS + IST_SIZE - sizeof(CONTEXT);

    pstRunningTask = gs_stScheduler.pstRunningTask;
    gs_stScheduler.pstRunningTask = pstNextTask;

    // Increase the processor time if switch from the idle task
    if ((pstRunningTask->qwFlags & TASK_FLAGS_IDLE) == TASK_FLAGS_IDLE) {
        gs_stScheduler.qwSpendProcessorTimeInIdleTask += TASK_PROCESSORTIME;
    }

    // If the end flag is set, insert the task into the wait list
    if ((pstRunningTask->qwFlags & TASK_FLAGS_ENDTASK) == TASK_FLAGS_ENDTASK) {
        list_add_tail(&pstRunningTask->stLink, &gs_stScheduler.stWaitList);
    }

    else {
        kMemCpy(&pstRunningTask->stContext, pcContextAddress, sizeof(CONTEXT));
        kAddTaskToReadyList(pstRunningTask);
    }

    kUnlockForSystemData(bPreviousFlag);

    if(gs_stScheduler.qwLastFPUUsedTaskID != pstNextTask->qwID){
        kSetTS();
    }

    else{
        kClearTS();
    }

    kMemCpy(pcContextAddress, &pstNextTask->stContext, sizeof(CONTEXT));

    // Update the processor time
    gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;

    return TRUE;
}

// Stop the currently running task
BOOL kEndTask(QWORD qwTaskID) {
    TCB* pstTargetTask;
    BYTE bPriority;
    BOOL bPreviousFlag;

    bPreviousFlag = kLockForSystemData();

    if (GETTCBOFFSET(qwTaskID) >= TASK_MAXCOUNT) {
        return FALSE;
    }

    pstTargetTask = gs_stScheduler.pstRunningTask;

    if (pstTargetTask->qwID == qwTaskID) {
        if ((pstTargetTask->qwFlags & TASK_FLAGS_IDLE) == TASK_FLAGS_IDLE) {
            return FALSE;
        }

        pstTargetTask->qwFlags |= TASK_FLAGS_ENDTASK;
        SETPRIORITY(pstTargetTask->qwFlags, TASK_FLAGS_WAIT);

        kUnlockForSystemData(bPreviousFlag);

        kSchedule();

        while(1);
    }

    // If the task is not the currently running task, remove it from the ready list
    // and linked it to the wait list
    else{
        pstTargetTask = kRemoveTaskFromReadyList(qwTaskID);
        if (pstTargetTask == NULL) {
            pstTargetTask = kGetTCBInTCBPool(GETTCBOFFSET(qwTaskID));
            if ((pstTargetTask != NULL) && (pstTargetTask->qwID == qwTaskID)) {
                if ((pstTargetTask->qwFlags & TASK_FLAGS_IDLE) == TASK_FLAGS_IDLE) {
                    return FALSE;
                }

                pstTargetTask->qwFlags |= TASK_FLAGS_ENDTASK;
                SETPRIORITY(pstTargetTask->qwFlags, TASK_FLAGS_WAIT);
                kUnlockForSystemData(bPreviousFlag);
                return TRUE;
            }

            return FALSE;
        }

        if ((pstTargetTask->qwFlags & TASK_FLAGS_IDLE) == TASK_FLAGS_IDLE) {
            kAddTaskToReadyList(pstTargetTask);
            kUnlockForSystemData(bPreviousFlag);
            return FALSE;
        }

        pstTargetTask->qwFlags |= TASK_FLAGS_ENDTASK;
        SETPRIORITY(pstTargetTask->qwFlags, TASK_FLAGS_WAIT);
        list_add_tail(&pstTargetTask->stLink, &gs_stScheduler.stWaitList);
    }

    kUnlockForSystemData(bPreviousFlag);
    return TRUE;
}

// Terminate task
void kExitTask(void) {
    kEndTask(gs_stScheduler.pstRunningTask->qwID);
}


// Return the number of tasks in the ready list
int kGetReadyTaskCount(void) {
    int i, iCount = 0;
    BOOL bPreviousFlag;

    bPreviousFlag = kLockForSystemData();
    for (i = 0; i < TASK_READYLIST_MAXCOUNT; i++) {
        iCount += list_count(&gs_stScheduler.vstReadyList[i]);
    }
    kUnlockForSystemData(bPreviousFlag);

    return iCount;
}

int kGetTaskCount(void) {
    int iCount;
    BOOL bPreviousFlag;

    iCount = kGetReadyTaskCount();

    bPreviousFlag = kLockForSystemData();
    iCount += list_count(&gs_stScheduler.stWaitList);
    iCount++; // Add the currently running task
    kUnlockForSystemData(bPreviousFlag);

    return iCount;
}


// Return the TCB in the TCB pool by offset
TCB* kGetTCBInTCBPool(int iOffset) {
    if (iOffset < 0 || iOffset >= TASK_MAXCOUNT) {
        return NULL;
    }

    return &gs_stTCBPoolManager.pstStartAddress[iOffset];
}


// Return is task exist
BOOL kIsTaskExist(QWORD qwID) {
    TCB* pstTargetTask;

    pstTargetTask = kGetTCBInTCBPool(GETTCBOFFSET(qwID));

    if (pstTargetTask == NULL) {
        return FALSE;
    }

    if (pstTargetTask->qwID != qwID) {
        return FALSE;
    }

    return TRUE;
}


// Return the processor utilization
QWORD kGetProcessorLoad(void) {
    return gs_stScheduler.qwProcessorLoad;
}

// Return process which includes the thread
static TCB* kGetProcessByThread(TCB* pstThread) {
    TCB* pstProcess;

    if(pstThread->qwFlags & TASK_FLAGS_PROCESS){
        return pstThread;
    }

    pstProcess = kGetTCBInTCBPool(GETTCBOFFSET(pstThread->qwParentProcessID));

    if(pstProcess == NULL || pstProcess->qwID != pstThread->qwParentProcessID){
        return NULL;
    }

    return pstProcess;
}

// Idle task
// Remove the task from the wait list which has the end flag set and free the TCB
void kIdleTask(void) {
    TCB* pstTask, * pstChildThread, * pstProcess;
    QWORD qwLastSpendTickInIdleTask;
    QWORD qwLastMeasureTickCount;
    QWORD qwCurrentMeasureTickCount;
    QWORD qwCurrentSpendTickInIdleTask;

    BOOL bPreviousFlag;
    int i, iCount;
    LISTHEAD* pstThreadLink;

    qwLastSpendTickInIdleTask = gs_stScheduler.qwSpendProcessorTimeInIdleTask;
    qwLastMeasureTickCount = kGetTickCount();

    while(1)
    {
        // Save Current State
        qwCurrentMeasureTickCount = kGetTickCount();
        qwCurrentSpendTickInIdleTask = gs_stScheduler.qwSpendProcessorTimeInIdleTask;

        // Calculate the processor utilization
        if (qwCurrentMeasureTickCount - qwLastMeasureTickCount == 0) {
            gs_stScheduler.qwProcessorLoad = 0;
        }

        else {
            gs_stScheduler.qwProcessorLoad = 100 - 
                ((qwCurrentSpendTickInIdleTask - qwLastSpendTickInIdleTask) * 100) / 
                (qwCurrentMeasureTickCount - qwLastMeasureTickCount);
        }

        qwLastMeasureTickCount = qwCurrentMeasureTickCount;
        qwLastSpendTickInIdleTask = qwCurrentSpendTickInIdleTask;

        kHaltProcessorByLoad();

        if (kGetReadyTaskCount() >= 0) {

            while(1) {
                bPreviousFlag = kLockForSystemData();
                LISTHEAD* pstLink = list_del_header(&gs_stScheduler.stWaitList);
                if (pstLink == NULL) {
                    kUnlockForSystemData(bPreviousFlag);
                    break;
                }

                pstTask = list_entry(pstLink, TCB, stLink);

                if(pstTask->qwFlags & TASK_FLAGS_PROCESS){
                    iCount = list_count(&pstTask->stChildThreadList);
                    for(i=0; i<iCount; i++){
                        pstThreadLink = list_del_header(&pstTask->stChildThreadList);
                        if(pstThreadLink == NULL){
                            break;
                        }

                        pstChildThread = list_entry(pstThreadLink, TCB, stThreadLink);
                        list_add_tail(&pstChildThread->stThreadLink,
                                &pstTask->stChildThreadList);

                        kEndTask(pstChildThread->qwID);
                    }

                    if(list_empty(&pstTask->stChildThreadList) == FALSE){
                        list_add_tail(&pstTask->stLink, &gs_stScheduler.stWaitList);

                        kUnlockForSystemData(bPreviousFlag);
                        continue;
                    }
                }

                else if(pstTask->qwFlags & TASK_FLAGS_THREAD){
                    pstProcess = kGetProcessByThread(pstTask);
                    if(pstProcess != NULL){
                        list_del(&pstTask->stThreadLink);
                    }
                }

                kFreeTCB(pstTask->qwID);
                kUnlockForSystemData(bPreviousFlag);
            }
        }
        
        kSchedule();        
    }
}

void kHaltProcessorByLoad(void) {
    if (gs_stScheduler.qwProcessorLoad < 40) {
        kHlt();
        kHlt();
        kHlt();
    }

    else if (gs_stScheduler.qwProcessorLoad < 80) {
        kHlt();
        kHlt();
    }

    else if (gs_stScheduler.qwProcessorLoad < 95) {
        kHlt();
    }
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

QWORD kGetLastFPUUsedTaskID(void){
    return gs_stScheduler.qwLastFPUUsedTaskID;
}

void kSetLastFPUUsedTaskID(QWORD qwTaskID){
    gs_stScheduler.qwLastFPUUsedTaskID = qwTaskID;
}