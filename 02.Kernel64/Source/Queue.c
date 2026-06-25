#include "Queue.h"

// Initialize the queue
void kInitializeQueue(Queue* pstQueue, void* pvQueueBuffer, 
    int iMaxDataCount, int iDataSize) {
    pstQueue->iDataSize = iDataSize;
    pstQueue->iMaxDataCount = iMaxDataCount;
    pstQueue->pvQueueArray = pvQueueBuffer;
    pstQueue->iPutIndex = 0;
    pstQueue->iGetIndex = 0;
    pstQueue->bLastOperationPut = FALSE;
}

// Return TRUE if the queue is full, otherwise return FALSE
BOOL kIsQueueFull(const Queue* pstQueue) {
    if (pstQueue->iPutIndex == pstQueue->iGetIndex && 
        pstQueue->bLastOperationPut == TRUE) {
        return TRUE;
    }
    return FALSE;
}

// Return TRUE if the queue is empty, otherwise return FALSE
BOOL kIsQueueEmpty(const Queue* pstQueue) {
    if (pstQueue->iPutIndex == pstQueue->iGetIndex && 
        pstQueue->bLastOperationPut == FALSE) {
        return TRUE;
    }
    return FALSE;
}

// Put data into the queue
BOOL kPutQueue(Queue* pstQueue, const void* pvData) {
    if (kIsQueueFull(pstQueue) == TRUE) {
        return FALSE;
    }

    kMemCpy((char*)pstQueue->pvQueueArray + (pstQueue->iPutIndex * pstQueue->iDataSize), 
        pvData, pstQueue->iDataSize);
    pstQueue->iPutIndex = (pstQueue->iPutIndex + 1) % pstQueue->iMaxDataCount;
    pstQueue->bLastOperationPut = TRUE;
    return TRUE;
}

// Get data from the queue
BOOL kGetQueue(Queue* pstQueue, void* pvData) {
    if (kIsQueueEmpty(pstQueue) == TRUE) {
        return FALSE;
    }

    kMemCpy(pvData, (char*)pstQueue->pvQueueArray + (pstQueue->iGetIndex * 
        pstQueue->iDataSize), pstQueue->iDataSize);
    pstQueue->iGetIndex = (pstQueue->iGetIndex + 1) % pstQueue->iMaxDataCount;
    pstQueue->bLastOperationPut = FALSE;
    return TRUE;
}