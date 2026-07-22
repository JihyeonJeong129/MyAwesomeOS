#include "Synchronization.h"
#include "AssemblyUtility.h"
#include "Utility.h"
#include "Task.h"

// Function to lock data which using global system
BOOL kLockForSystemData(void)
{
    return kSetInterruptFlag(FALSE);
}

// Function to unlock data which using global system
void kUnlockForSystemData(BOOL bInterruptFlag)
{
    kSetInterruptFlag(bInterruptFlag);
}

// Initialize mutex
void kInitializeMutex(MUTEX* pstMutex)
{
    pstMutex->bLockFlag = FALSE;
    pstMutex->qwTaskID = TASK_INVALIDID;
    pstMutex->dwLockCount = 0;
}   

// Lock function for data which is used between tasks
void kLock(MUTEX* pstMutex)
{
    if(kTestAndSet(&(pstMutex->bLockFlag), 0, 1) == FALSE)
    {
        if(pstMutex->qwTaskID == kGetRunningTask()->qwID)
        {
            pstMutex->dwLockCount++;
            return;
        }

        while(kTestAndSet(&(pstMutex->bLockFlag), 0, 1) == FALSE)
        {
            kSchedule();
        }
    }

    pstMutex->dwLockCount = 1;
    pstMutex->qwTaskID = kGetRunningTask()->qwID;
}

// Unlock function for data which is used between tasks
void kUnlock(MUTEX* pstMutex)
{
    if((pstMutex->qwTaskID != kGetRunningTask()->qwID) || 
        (pstMutex->dwLockCount == 0))
    {
        return;
    }

    if(pstMutex->dwLockCount > 1)
    {
        pstMutex->dwLockCount--;
        return;
    }

    pstMutex->qwTaskID = TASK_INVALIDID;
    pstMutex->dwLockCount = 0;
    pstMutex->bLockFlag = FALSE;
}
