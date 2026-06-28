#ifndef __TASK_H__
#define __TASK_H__

#include "Types.h"

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

#pragma pack(push, 1)

typedef struct kContextStruct {
    QWORD vqRegister[TASK_REGISTERS_COUNT];
} CONTEXT;

typedef struct kTaskControlBlockStruct {
    CONTEXT stContext;

    QWORD qwFlags;
    QWORD qwID;

    void* pvStackAddress;
    QWORD qwStackSize;
} TCB;

#pragma pack(pop)


// Functions
void kSetupTask(TCB* pstTCB, QWORD qwFlags, QWORD qwID, QWORD qwEntryPointAddress, 
    void* pvStackAddress, QWORD qwStackSize);

#endif // __TASK_H__