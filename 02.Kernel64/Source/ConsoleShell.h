#ifndef __CONSOLESHELL_H__
#define __CONSOLESHELL_H__

#include "Types.h"

#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT 300
#define CONSOLESHELL_PROMPTMESSAGE "MyAwesomeOS> "

typedef void (*CommandFunction)(const char* pcParameter);

#pragma pack(push, 1)

typedef struct kShellCommandEntryStruct {
    char* pcCommand;
    char* pcHelp;
    CommandFunction pfFunction;
} SHELLCOMMANDENTRY;

#pragma pack(pop)

typedef struct kParameterListStruct {
    const char* pcBuffer;
    int iLength;
    int iCurrentPosition;
} PARAMETERLIST;

#pragma pack(push, 1)

// Functions
void kStartConsoleShell(void);
void kExecuteCommand(const char* pcCommandBuffer);
void kInitializeParameter(PARAMETERLIST* pstList, const char* pcParameter);
int kGetNextParameter(PARAMETERLIST* pstList, char* pcParameter);

static void kHelp(const char* pcParameter);
static void kClearShell(const char* pcParameter);
static void kShowTotalRAMSize(const char* pcParameter);
static void kStringToDecimalHexTest(const char* pcParameter);
static void kShutdown(const char* pcParameter);

static void kSetTimer(const char* pcParameter);
static void kWaitUsingPITCommand(const char* pcParameter);
static void kReadTimeStampCounter(const char* pcParameter);
static void kMeasureProcessorSpeed(const char* pcParameter);
static void kShowDateAndTime(const char* pcParameter);

static void kTestTask1(void);
static void kTestTask2(void);
static void kCreateTestTask(const char* pcParameter);

static void kChangeTaskPriority(const char* pcParameter);
static void kShowTaskList(const char* pcParameter);
static void kKillTask(const char* pcParameter);
static void kCPULoad(const char* pcParameter);

static void kTestMutex(const char* pcParameter);

static void kCreateThreadTask(void);
static void kTestThread(const char* pcParameterBuffer);

static void kTestPIE(const char* pcParameterBuffer);

#endif /*__CONSOLESHELL_H__*/
