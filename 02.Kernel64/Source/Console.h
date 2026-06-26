#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "Types.h"

#define CONSOLE_BACKGROUND_COLOR_BLACK         0X00
#define CONSOLE_BACKGROUND_COLOR_BLUE          0X10
#define CONSOLE_BACKGROUND_COLOR_GREEN         0X20
#define CONSOLE_BACKGROUND_COLOR_CYAN          0X30
#define CONSOLE_BACKGROUND_COLOR_RED           0X40
#define CONSOLE_BACKGROUND_COLOR_MAGENTA       0X50
#define CONSOLE_BACKGROUND_COLOR_BROWN         0X60
#define CONSOLE_BACKGROUND_COLOR_WHITE         0X70
#define CONSOLE_BACKGROUND_COLOR_BLINK         0X80

#define CONSOLE_FOREGROUND_COLOR_DARK_BLACK         0X00
#define CONSOLE_FOREGROUND_COLOR_DARK_BLUE          0X01
#define CONSOLE_FOREGROUND_COLOR_DARK_GREEN         0X02
#define CONSOLE_FOREGROUND_COLOR_DARK_CYAN          0X03
#define CONSOLE_FOREGROUND_COLOR_DARK_RED           0X04
#define CONSOLE_FOREGROUND_COLOR_DARK_MAGENTA       0X05
#define CONSOLE_FOREGROUND_COLOR_DARK_BROWN         0X06
#define CONSOLE_FOREGROUND_COLOR_DARK_WHITE         0X07

#define CONSOLE_FOREGROUND_COLOR_BRIGHT_BLACK       0X08
#define CONSOLE_FOREGROUND_COLOR_BRIGHT_BLUE        0X09
#define CONSOLE_FOREGROUND_COLOR_BRIGHT_GREEN       0X0A
#define CONSOLE_FOREGROUND_COLOR_BRIGHT_CYAN        0X0B
#define CONSOLE_FOREGROUND_COLOR_BRIGHT_RED         0X0C
#define CONSOLE_FOREGROUND_COLOR_BRIGHT_MAGENTA     0X0D
#define CONSOLE_FOREGROUND_COLOR_BRIGHT_YELLOW      0X0E
#define CONSOLE_FOREGROUND_COLOR_BRIGHT_WHITE       0X0F

#define CONSOLE_DEFAULT_TEXT_COLOR \
(CONSOLE_BACKGROUND_COLOR_BLACK | CONSOLE_FOREGROUND_COLOR_BRIGHT_GREEN)


// Console's width, height, and video memory address
#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25
#define CONSOLE_VIDEOMEMORYADDRESS 0XB8000

// Video Controller's I/O port addresses
#define VGA_PORT_INDEX 0X3D4
#define VGA_PORT_DATA 0X3D5
#define VGA_INDEX_UPPERCURSOR 0X0E
#define VGA_INDEX_LOWERCURSOR 0X0F

#pragma pack(push, 1)

typedef struct kConsoleStruct {
    int iCurrentPrintOffset;
} CONSOLEMANAGER;

#pragma pack(pop)

// Functions
void kInitializeConsole(int iX, int iY);
void kSetCursor(int iX, int iY);
void kGetCursor(int* piX, int* piY);
void kPrintf(const char* pcFormatString, ...);
int kConsolePrintString(const char* pcBuffer);
void kClearConsole(void);
BYTE kGetCh(void);
void kPrintStringXY(int iX, int iY, const char* pcString);

#endif /*__CONSOLE_H__*/


