#include "Types.h"
#include "Keyboard.h"
#include "AssemblyUtility.h"

//////////////////////////////////////
//  Function about Keyboard controller
//  and Keyboard control
//////////////////////////////////////

// Returns TRUE if the output buffer has data; otherwise returns FALSE.
BOOL kIsOutputBufferFull(void)
{
    // If bit 0 of the status register is set, the output buffer has data.
    if (kInPortByte(0x64) & 0x01)
    {
        return TRUE;
    }
    return FALSE;
}

// Returns TRUE if the input buffer is full; otherwise returns FALSE.
BOOL kIsInputBufferFull(void)
{
    // If bit 1 of the status register is set, the input buffer is full.
    if (kInPortByte(0x64) & 0x02)
    {
        return TRUE;
    }
    return FALSE;
}

// Activate Keyboard and return TRUE if successful; otherwise returns FALSE.
BOOL kActivateKeyboard(void)
{    
    int i;
    int j;

    // Send the command to activate the keyboard.
    kOutPortByte(0x64, 0xAE);

    for (i = 0; i < 0xFFFF; i++)
    {
        // Check if the input buffer is full.
        if (kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    kOutPortByte(0x60, 0xF4);

    // Wait for the ACK response from the keyboard.
    for (i = 0; i < 100; i++)
    {
        for (j = 0; j < 0xFFFF; j++)
        {
            if (kIsOutputBufferFull() == TRUE)
            {
                break;
            }
        }
        
        // If the data is ACK (0xFA), return TRUE.
        if (kInPortByte(0x60) == 0xFA)
        {
            return TRUE;
        }
    }

    return FALSE;
}

// Returns the scan code from the output buffer.
BYTE kGetKeyboardScanCode(void)
{
    // Wait until the output buffer has data.
    while (kIsOutputBufferFull() == FALSE)
    {
        ;
    }

    // Read the scan code from the output buffer.
    return kInPortByte(0x60);
}


// Change the keyboard LED status and return 
//TRUE if successful; otherwise returns FALSE.
BYTE kChangeKeyboardLED(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn)
{
    int i;
    int j;
    
    for (i = 0; i < 0xFFFF; i++)
    {
        // Check if the input buffer is full.
        if (kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    // Send the command to change the LED status.
    kOutPortByte(0x60, 0xED);

    for (i = 0; i < 0xFFFF; i++)
    {
        // Check if the input buffer is full.
        if (kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    // Wait for the ACK response from the keyboard.
    for (i = 0; i < 100; i++)
    {
        for (j = 0; j < 0xFFFF; j++)
        {
            if (kIsOutputBufferFull() == TRUE)
            {
                break;
            }
        }

        if (kInPortByte(0x60) == 0xFA)
        {
            break;
        }
    }

    // If the ACK response is not received, return FALSE.
    if (i >= 100)
    {
        return FALSE;
    }

    // Send the LED status to the keyboard.
    kOutPortByte(0x60, (bCapsLockOn << 2) | (bNumLockOn << 1) | bScrollLockOn);

    for (i = 0; i<0xFFFF; i++)
    {
        // Check if the input buffer is full.
        if (kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    // Wait for the ACK response from the keyboard.
    for (i = 0; i < 100; i++)
    {
        for (j = 0; j < 0xFFFF; j++)
        {
            if (kIsOutputBufferFull() == TRUE)
            {
                break;
            }
        }

        if (kInPortByte(0x60) == 0xFA)
        {
            return TRUE;
        }
    }

    return FALSE;
}


// Activate the A20 gate and return TRUE if successful; otherwise returns FALSE.
void kEnableA20Gate(void)
{
    int i;
    BYTE bOutputPortData;

    kOutPortByte(0x64, 0xD0);
    
    for (i = 0; i < 0xFFFF; i++)
    {
        // Check if the output buffer is full.
        if (kIsOutputBufferFull() == TRUE)
        {
            break;
        }
    }

    bOutputPortData = kInPortByte(0x60) | 0x01;

    for (i = 0; i < 0xFFFF; i++)
    {
        // Check if the input buffer is full.
        if (kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    kOutPortByte(0x64, 0xD1);
    kOutPortByte(0x60, bOutputPortData);
}

// Reset Processor and return TRUE if successful; otherwise returns FALSE.
void kReboot(void)
{
    int i;

    // Wait until the input buffer is empty.
    for (i = 0; i < 0xFFFF; i++)
    {
        if (kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    // Send the command to reset the processor.
    kOutPortByte(0x64, 0xD1);
    kOutPortByte(0x60, 0x00);

    // Wait for the processor to reset.
    while (1)
    {
        ;
    }
}


//////////////////////////////////////
//  Function about Translation between
//  scan code and ASCII code
//////////////////////////////////////

// Keyboard manager structure
static KEYBOARDMANAGER gs_stKeyboardManager = { 0, };

// Table for converting scan codes to ASCII codes
static KEYMAPPINGENTRY gs_vstKeyMappingTable[KEY_MAPPINGTABLEMAXCOUNT] =
{
    { KEY_NONE, KEY_NONE },
    { KEY_ESC, KEY_ESC },
    { '1', '!' },
    { '2', '@' },
    { '3', '#' },
    { '4', '$' },
    { '5', '%' },
    { '6', '^' },
    { '7', '&' },
    { '8', '*' },
    { '9', '(' },
    { '0', ')' },
    { '-', '_' },
    { '=', '+' },
    { KEY_BACKSPACE, KEY_BACKSPACE },
    { KEY_TAB, KEY_TAB },
    { 'q', 'Q' },
    { 'w', 'W' },
    { 'e', 'E' },
    { 'r', 'R' },
    { 't', 'T' },
    { 'y', 'Y' },
    { 'u', 'U' },
    { 'i', 'I' },
    { 'o', 'O' },
    { 'p', 'P' },
    { '[', '{' },
    { ']', '}' },
    { KEY_ENTER, KEY_ENTER },
    { KEY_CTRL, KEY_CTRL },
    { 'a', 'A' },
    { 's', 'S' },
    { 'd', 'D' },
    { 'f', 'F' },
    { 'g', 'G' },
    { 'h', 'H' },
    { 'j', 'J' },
    { 'k', 'K' },
    { 'l', 'L' },
    { ';', ':' },
    { '\'', '"' },
    { '`', '~' },
    { KEY_LSHIFT, KEY_LSHIFT },
    { '\\', '|' },
    { 'z', 'Z' },
    { 'x', 'X' },
    { 'c', 'C' },
    { 'v', 'V' },
    { 'b', 'B' },
    { 'n', 'N' },
    { 'm', 'M' },
    { ',', '<' },
    { '.', '>' },
    { '/', '?' },
    { KEY_RSHIFT, KEY_RSHIFT },
    { '*', '*' },
    { KEY_LALT, KEY_LALT },
    { ' ', ' ' },
    { KEY_CAPSLOCK, KEY_CAPSLOCK },
    { KEY_F1, KEY_F1 },
    { KEY_F2, KEY_F2 },
    { KEY_F3, KEY_F3 },
    { KEY_F4, KEY_F4 },
    { KEY_F5, KEY_F5 },
    { KEY_F6, KEY_F6 },
    { KEY_F7, KEY_F7 },
    { KEY_F8, KEY_F8 },
    { KEY_F9, KEY_F9 },
    { KEY_F10, KEY_F10 },
    { KEY_NUMLOCK, KEY_NUMLOCK },
    { KEY_SCROLLLOCK, KEY_SCROLLLOCK },
    { '7', '7' },
    { '8', '8' },
    { '9', '9' },
    { '-', '-' },
    { '4', '4' },
    { '5', '5' },
    { '6', '6' },
    { '+', '+' },
    { '1', '1' },
    { '2', '2' },
    { '3', '3' },
    { '0', '0' },
    { '.', '.' },
    { KEY_NONE, KEY_NONE },
    { KEY_NONE, KEY_NONE },
    { KEY_NONE, KEY_NONE },
    { KEY_F11, KEY_F11 },
    { KEY_F12, KEY_F12 }
};


// check if the scan code corresponds to an alphabet key (A-Z) 
// and return TRUE if it does; otherwise returns FALSE.
BOOL kIsAlphabetScanCode(BYTE bScanCode)
{
    // Check if the scan code is for an alphabet key (A-Z).
    if (('a' <= gs_vstKeyMappingTable[bScanCode].bNormalCode && gs_vstKeyMappingTable[bScanCode].bNormalCode <= 'z') ||
        ('A' <= gs_vstKeyMappingTable[bScanCode].bNormalCode && gs_vstKeyMappingTable[bScanCode].bNormalCode <= 'Z'))
    {
        return TRUE;
    }
    return FALSE;
}

// Check if the scan code corresponds to a number key (0-9)
// or a symbol key (e.g., !, @, #, etc.) 
// return TRUE if it does; otherwise returns FALSE.
BOOL kIsNumberOrSymbolScanCode(BYTE bScanCode)
{
    // Check if the scan code is for a number key (0-9).
    if ('0' <= gs_vstKeyMappingTable[bScanCode].bNormalCode && gs_vstKeyMappingTable[bScanCode].bNormalCode <= '9' ||
        (gs_vstKeyMappingTable[bScanCode].bNormalCode >= ' ' && gs_vstKeyMappingTable[bScanCode].bNormalCode <= '/' ) ||
        (gs_vstKeyMappingTable[bScanCode].bNormalCode >= ':' && gs_vstKeyMappingTable[bScanCode].bNormalCode <= '@' ) ||
        (gs_vstKeyMappingTable[bScanCode].bNormalCode >= '[' && gs_vstKeyMappingTable[bScanCode].bNormalCode <= '`' ) ||
        (gs_vstKeyMappingTable[bScanCode].bNormalCode >= '{' && gs_vstKeyMappingTable[bScanCode].bNormalCode <= '~' ))
    {
        return TRUE;
    }
    return FALSE;
}

// Check if the scan code corresponds to a number key (0-9) on the numeric keypad
// and return TRUE if it does; otherwise returns FALSE.
BOOL kIsNumberPadScanCode(BYTE bScanCode)
{
    // Check if the scan code is for a number key (0-9) on the numeric keypad.
    if (gs_vstKeyMappingTable[bScanCode].bNormalCode >= '0' && gs_vstKeyMappingTable[bScanCode].bNormalCode <= '9')
    {
        return TRUE;
    }
    return FALSE;
}

// Check if the scan code corresponds to an extended key 
// and return TRUE if it does; otherwise returns FALSE.
BOOL kIsUseCombinedCode(BYTE bScanCode)
{
    BYTE bDownScanCode;
    BOOL bUseCombinedKey = FALSE;

    bDownScanCode = bScanCode & 0x7F;

    // Check if the scan code is for an alphabet key (A-Z).
    if (kIsAlphabetScanCode(bDownScanCode) == TRUE)
    {
        if((gs_stKeyboardManager.bShiftDown ^ gs_stKeyboardManager.bCapsLockOn) == TRUE)
        {
            bUseCombinedKey = TRUE;
        }
        else
        {
            bUseCombinedKey = FALSE;
        }
    }
    
    // Check if the scan code is for a num and symbol key.
    else if (kIsNumberOrSymbolScanCode(bDownScanCode) == TRUE)
    {
        if(gs_stKeyboardManager.bShiftDown == TRUE)
        {
            bUseCombinedKey = TRUE;
        }
        else
        {
            bUseCombinedKey = FALSE;
        }
    }

    // check if the scan code is for a number key (0-9) on the numeric keypad.
    else if (kIsNumberPadScanCode(bDownScanCode) == TRUE && 
        gs_stKeyboardManager.bExtendedCodeIn == FALSE)
    {
        if(gs_stKeyboardManager.bNumLockOn == TRUE)
        {
            bUseCombinedKey = TRUE;
        }
        else
        {
            bUseCombinedKey = FALSE;
        }
    }

    return bUseCombinedKey;
}

// Renew the status of the modifier keys (Shift, Caps Lock, Num Lock, Scroll Lock)
// and synchroize the keyboard LED status.
void kUpdateCombinationKeyStatusAndLED(BYTE bScanCode)
{
    BOOL bDown;
    BYTE bDownScanCode;
    BOOL bLEDStatusChanged = FALSE;

    // Check if the scan code is for key release.
    if (bScanCode & 0x80)
    {
        bDown = FALSE;
        bDownScanCode = bScanCode & 0x7F;
    }
    else
    {
        bDown = TRUE;
        bDownScanCode = bScanCode;
    }

    // update the status of the modifier keys (Shift, Caps Lock, Num Lock, Scroll Lock).
    if ((bDownScanCode == 42) || (bDownScanCode == 54))
    {
        gs_stKeyboardManager.bShiftDown = bDown;
    }

    else if (bDownScanCode == 58 && bDown == TRUE)
    {
        gs_stKeyboardManager.bCapsLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }
    else if (bDownScanCode == 69 && bDown == TRUE)
    {
        gs_stKeyboardManager.bNumLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }
    else if (bDownScanCode == 70 && bDown == TRUE)
    {
        gs_stKeyboardManager.bScrollLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }

    if (bLEDStatusChanged == TRUE)
    {
        kChangeKeyboardLED(gs_stKeyboardManager.bCapsLockOn, gs_stKeyboardManager.bNumLockOn, gs_stKeyboardManager.bScrollLockOn);
    }
}

// Convert the scan code to an ASCII code and return it.
BOOL kConvertScanCodeToASCIICode(BYTE bScanCode, BYTE* pbASCIICode, BOOL* pbFlags)
{
    BOOL bUseCombinedKey;

    if (gs_stKeyboardManager.iSkipCountForPause > 0)
    {
        gs_stKeyboardManager.iSkipCountForPause--;
        return FALSE;
    }

    // Pause key is special because it sends multiple scan codes for a single key press.
    if (bScanCode == 0xE1)
    {
        *pbASCIICode = KEY_PAUSE;
        *pbFlags = KEY_FLAGS_DOWN;
        gs_stKeyboardManager.iSkipCountForPause = KEY_SKIPCOUNTFORPAUSE;
        return TRUE;
    }

    else if (bScanCode == 0xE0)
    {
        gs_stKeyboardManager.bExtendedCodeIn = TRUE;
        return FALSE;
    }

    bUseCombinedKey = kIsUseCombinedCode(bScanCode);

    if (bUseCombinedKey == TRUE)
    {
        *pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bCombinedCode;
    }
    else
    {
        *pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bNormalCode;
    }

    // Set extended key flag if the scan code is for an extended key.
    if(gs_stKeyboardManager.bExtendedCodeIn == TRUE)
    {
        *pbFlags = KEY_FLAGS_EXTENDEDKEY;
        gs_stKeyboardManager.bExtendedCodeIn = FALSE;
    }
    else
    {
        *pbFlags = 0;
    }

    // Set key down flag if the scan code is for a key press; otherwise, clear it.
    if ((bScanCode & 0x80) == 0)
    {        
        *pbFlags |= KEY_FLAGS_DOWN;
    }

    // Refresh the status of the modifier keys
    kUpdateCombinationKeyStatusAndLED(bScanCode);

    return TRUE;
}
    
