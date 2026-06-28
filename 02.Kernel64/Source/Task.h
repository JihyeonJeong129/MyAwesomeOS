#ifndef __TASK_H__
#define __TASK_H__

#include "Types.h"

#include "list.h"

#define TASK_REGISTERS_COUNT 24
#define TASK_REGISTERS_SIZE  8

// Registers Offset
#define TASK_GS_OFFSET 0
#define TASK_FS_OFFSET 1
#define TASK_ES_OFFSET 2
#define TASK_DS_OFFSET 3
#define TASK_R15_OFFSET 4
#define TASK_R14_OFFSET 5
#define TASK_R13_OFFSET 6
#define TASK_R12_OFFSET 7
#define TASK_R11_OFFSET 8
#define TASK_R10_OFFSET 9
#define TASK_R9_OFFSET  10
#define TASK_R8_OFFSET  11
#define TASK_RSI_OFFSET 12
#define TASK_RDI_OFFSET 13
#define TASK_RDX_OFFSET 14
#define TASK_RCX_OFFSET 15
#define TASK_RBX_OFFSET 16
#define TASK_RAX_OFFSET 17
#define TASK_RBP_OFFSET 18
#define TASK_RIP_OFFSET 19
#define TASK_CS_OFFSET 20
#define TASK_RFLAGS_OFFSET 21
#define TASK_RSP_OFFSET 22
#define TASK_SS_OFFSET 23

// Task pool Address
#define TASK_TCBPOOLADDRESS 0x800000
#define TASK_MAXCOUNT 1024

// Task Pool and Stack size
#define TASK_STACKPOOLADDRESS (TASK_TCBPOOLADDRESS + (sizeof(TCB) * TASK_MAXCOUNT))
#define TASK_STACKSIZE 8192

// Invalid ID
#define TASK_INVALIDID 0xFFFFFFFFFFFFFFFF

// Maximum processor time for a task (ms)
#define TASK_PROCESSORTIME 5


#pragma pack(push, 1)

typedef struct kContextStruct {
    QWORD vqRegister[TASK_REGISTERS_COUNT];
} CONTEXT;


typedef struct kTaskControlBlockStruct {
    LISTHEAD stLink;
    
    CONTEXT stContext;

    QWORD qwFlags;
    QWORD qwID;

    void* pvStackAddress;
    QWORD qwStackSize;
} TCB;


typedef struct kTCBPoolManagerStruct {
    
    TCB* pstStartAddress;
    
    int iMaxCount;
    int iUseCount;
    int iAllocatedCount;
} TCBPOOLMANAGER;

typedef struct kSchedulerStruct {
    TCB* pstRunningTask;

    int iProcessorTime;

    LISTHEAD stReadyList;
} SCHEDULER;


#pragma pack(pop)


// Functions
void kSetupTask(TCB* pstTCB, QWORD qwFlags, QWORD qwID, QWORD qwEntryPointAddress, 
    void* pvStackAddress, QWORD qwStackSize);


// For Task Pool and Task
void kInitializeTCBPool(void);
TCB* kAllocateTCB(void);
void kFreeTCB(QWORD qwID);
TCB* kCreateTask(QWORD qwFlags, QWORD qwEntryPointAddress);

// For Scheduler
void kInitializeScheduler(void);
void kSetRunningTask(TCB* pstTask);
TCB* kGetRunningTask(void);
TCB* kGetNextTaskToRun(void);
void kAddTaskToReadyList(TCB* pstTask);
void kSchedule(void);
BOOL kScheduleInInterrupt(void);
void kDecreaseProcessorTime(void);
BOOL kIsProcessorTimeExpired(void);


#endif // __TASK_H__
