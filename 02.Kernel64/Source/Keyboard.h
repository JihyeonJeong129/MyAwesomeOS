#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "Types.h"

// Macro for special keys

// Count of scan codes to skip for Pause key
#define KEY_SKIPCOUNTFORPAUSE 2

// Flags for key events
#define KEY_FLAGS_UP 0x00
#define KEY_FLAGS_DOWN 0x01
#define KEY_FLAGS_EXTENDEDKEY 0x02

// Structure for mapping scan codes to ASCII codes
#define KEY_MAPPINGTABLEMAXCOUNT 89

#define KEY_NONE 0
#define KEY_ENTER '\n'
#define KEY_TAB '\t'
#define KEY_ESC 0x1B
#define KEY_BACKSPACE 0x08
#define KEY_CTRL 0x81
#define KEY_LSHIFT 0x82
#define KEY_RSHIFT 0x83
#define KEY_PRINTSCREEN 0x84
#define KEY_LALT 0x85
#define KEY_CAPSLOCK 0x86
#define KEY_F1 0x87
#define KEY_F2 0x88
#define KEY_F3 0x89
#define KEY_F4 0x8A
#define KEY_F5 0x8B
#define KEY_F6 0x8C
#define KEY_F7 0x8D
#define KEY_F8 0x8E
#define KEY_F9 0x8F
#define KEY_F10 0x90
#define KEY_NUMLOCK 0x91
#define KEY_SCROLLLOCK 0x92
#define KEY_HOME 0x93
#define KEY_UP 0x94
#define KEY_PAGEUP 0x95
#define KEY_LEFT 0x96
#define KEY_CENTER 0x97
#define KEY_RIGHT 0x98
#define KEY_END 0x99
#define KEY_DOWN 0x9A
#define KEY_PAGEDOWN 0x9B
#define KEY_INS 0x9C
#define KEY_DEL 0x9D
#define KEY_F11 0x9E
#define KEY_F12 0x9F
#define KEY_PAUSE 0xA0

// Structure for key mapping
#pragma pack(push, 1)

typedef struct kKeyMappingEntryStruct
{
    BYTE bNormalCode;      // ASCII code for normal key press
    BYTE bCombinedCode;    // ASCII code for combined key press (e.g., Shift + key)
} KEYMAPPINGENTRY;

#pragma pack(pop)

typedef struct kKeyboardManagerStruct
{
    BOOL bShiftDown;               // TRUE if Shift key is pressed
    BOOL bCapsLockOn;              // TRUE if Caps Lock is on
    BOOL bNumLockOn;               // TRUE if Num Lock is on
    BOOL bScrollLockOn;            // TRUE if Scroll Lock is on
    
    BOOL bExtendedCodeIn;          // TRUE if an extended scan code is being processed
    int iSkipCountForPause;        // Count of scan codes to skip for Pause key
} KEYBOARDMANAGER;

// Function prototypes for keyboard management
BOOL kIsOutputBufferFull(void);
BOOL kIsInputBufferFull(void);
BOOL kActivateKeyboard(void);
BYTE kGetKeyboardScanCode(void);
BOOL kChangeKeyboardLED(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn);
void kEnableA20Gate(void);
void kReboot(void);
BOOL kIsAlphabetScanCode(BYTE bScanCode);
BOOL kIsNumberOrSymbolScanCode(BYTE bScanCode);
BOOL kIsNumberPadScanCode(BYTE bScanCode);
BOOL kIsUseCombinedCode(BYTE bScanCode);
void kUpdateCombinationKeyStatusAndLED(BYTE bScanCode);
BOOL kConvertScanCodeToASCIICode(BYTE bScanCode, BYTE* pbASCIICode, BOOL* pbFlags);

#endif /* __KEYBOARD_H__ */
