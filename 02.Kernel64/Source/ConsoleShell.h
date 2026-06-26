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

void kHelp(const char* pcParameter);
void kClearShell(const char* pcParameter);
void kShowTotalRAMSize(const char* pcParameter);
void kStringToDecimalHexTest(const char* pcParameter);
void kShutdown(const char* pcParameter);

#endif /*__CONSOLESHELL_H__*/