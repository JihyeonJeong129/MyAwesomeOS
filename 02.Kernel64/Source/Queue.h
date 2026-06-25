#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "Types.h"

#pragma pack(push, 1)

typedef struct kQueueManagerStruct {
    int iDataSize;
    int iMaxDataCount;

    void* pvQueueArray;
    int iPutIndex;
    int iGetIndex;

    BOOL bLastOperationPut;

} Queue;

#pragma pack(pop)

// Functions
void kInitializeQueue(Queue* pstQueue, void* pvQueueBuffer, 
    int iMaxDataCount, int iDataSize);
BOOL kIsQueueFull(const Queue* pstQueue);
BOOL kIsQueueEmpty(const Queue* pstQueue);
BOOL kPutQueue(Queue* pstQueue, const void* pvData);
BOOL kGetQueue(Queue* pstQueue, void* pvData);

#endif // __QUEUE_H__
