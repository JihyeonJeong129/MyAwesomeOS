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

// Ready list execution count
#define TASK_READYLIST_MAXCOUNT 5

// Priority levels
#define TASK_FLAGS_HIGHEST 0x00
#define TASK_FLAGS_HIGH 0x01
#define TASK_FLAGS_MEDIUM 0x02
#define TASK_FLAGS_LOW 0x03
#define TASK_FLAGS_LOWEST 0x04
#define TASK_FLAGS_WAIT 0xFF

// Task Flags
#define TASK_FLAGS_ENDTASK 0x8000000000000000
#define TASK_FLAGS_SYSTEM 0x4000000000000000
#define TASK_FLAGS_PROCESS 0x2000000000000000
#define TASK_FLAGS_THREAD 0x1000000000000000
#define TASK_FLAGS_IDLE 0x0800000000000000

#define GETPRIORITY(x) ((x) & 0xFF)
#define SETPRIORITY(x, priority) ((x) = ((x) & 0xFFFFFFFFFFFFFF00) | (priority))
#define GETTCBOFFSET(x) ((x) & 0xFFFFFFFF)

#pragma pack(push, 1)

typedef struct kContextStruct {
    QWORD vqRegister[TASK_REGISTERS_COUNT];
} CONTEXT;


typedef struct kTaskControlBlockStruct {
    LISTHEAD stLink;
    
    void* pvMemoryAddress;
    QWORD qwMemorySize;

    LISTHEAD stThreadLink;

    QWORD qwParentProcessID;

    QWORD vqwFPUContext[512/8];

    LISTHEAD stChildThreadList;

    QWORD qwFlags;
    QWORD qwID;

    CONTEXT stContext;

    void* pvStackAddress;
    QWORD qwStackSize;

    BOOL bFPUUsed;

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

    LISTHEAD vstReadyList[TASK_READYLIST_MAXCOUNT];

    LISTHEAD stWaitList;

    int viExecutionCount[TASK_READYLIST_MAXCOUNT];

    QWORD qwProcessorLoad;

    QWORD qwSpendProcessorTimeInIdleTask;

    QWORD qwLastFPUUsedTaskID;
} SCHEDULER;


#pragma pack(pop)


// Functions
static void kSetupTask(TCB* pstTCB, QWORD qwFlags, QWORD qwID, QWORD qwEntryPointAddress, 
    void* pvStackAddress, QWORD qwStackSize);


// For Task Pool and Task
static void kInitializeTCBPool(void);
static TCB* kAllocateTCB(void);
static void kFreeTCB(QWORD qwID);
TCB* kCreateTask(QWORD qwFlags, void* pvMemoryAddress, QWORD qwMemorySize,
                 QWORD qwEntryPointAddress);
// For Scheduler
void kInitializeScheduler(void);
void kSetRunningTask(TCB* pstTask);
TCB* kGetRunningTask(void);
static TCB* kGetNextTaskToRun(void);
static BOOL kAddTaskToReadyList(TCB* pstTask);
void kSchedule(void);
BOOL kScheduleInInterrupt(void);
void kDecreaseProcessorTime(void);
BOOL kIsProcessorTimeExpired(void);

static TCB* kRemoveTaskFromReadyList(QWORD qwTaskID);
BOOL kChangePriority(QWORD qwTaskID, BYTE bPriority);
BOOL kEndTask(QWORD qwTaskID);
void kExitTask(void);
int kGetReadyTaskCount(void);
int kGetTaskCount(void);
TCB* kGetTCBInTCBPool(int iOffset);
BOOL kIsTaskExist(QWORD qwID);
QWORD kGetProcessorLoad(void);

// For Idle Task
void kIdleTask(void);
void kHaltProcessorByLoad(void);

// For FPU
QWORD kGetLastFPUUsedTaskID(void);
void kSetLastFPUUsedTaskID(QWORD qwTaskID);

#endif // __TASK_H__
