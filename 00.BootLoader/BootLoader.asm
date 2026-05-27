[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07C0:START

;;;;;;;;;;;;;;;;;;;;;;;;;
; Floppy Disk Parameter for read OS image
;;;;;;;;;;;;;;;;;;;;;;;;;

TOTALSECTORCOUNT:   dw  1024
SECTORNUMBER:   db  0x02
HEADNUMBER:     db  0x00
TRACKNUMBER:    db  0x00

START:
    mov ax, 0x07C0
    mov ds, ax
    mov ax, 0xB800
    mov es, ax
    
    mov si, 0

.SCREENCLEARLOOP:
    mov byte[es: si], 0
    mov byte[es: si+1], 0x0A

    add si, 2

    cmp si, 80*25*2

    jb .SCREENCLEARLOOP

    mov si, 0
    mov di, 0

.MESSAGELOOP:
    mov cl, byte[MESSAGE1 + si]

    cmp cl, 0
    je .MESSAGEEND

    mov byte[es: di], cl
    mov byte[es: di+1], 0x0A

    add si, 1
    add di, 2

    cmp di, 80*25*2

    jb .MESSAGELOOP

.MESSAGEEND:

;;;;;;;;;;;;;;;;;;;;;;;;;
; Read OS Image Begin
;;;;;;;;;;;;;;;;;;;;;;;;;

        mov si, 0x1000
        mov es, si
        
        mov bx, 0x0000

        mov di, word[TOTALSECTORCOUNT]

READDATA:
    cmp di, 0
    je READEND
    sub di, 0x1

    ;;;;;;;;;;;;;;;;;;;;;;;;;
    ; BIOS Function Calling
    ;;;;;;;;;;;;;;;;;;;;;;;;;

    mov ah, 0x02 ; function num
    mov al, 0x1 ; read sector count
    mov ch, byte[TRACKNUMBER]
    mov cl, byte[SECTORNUMBER]
    mov dh, byte[HEADNUMBER]
    mov dl, 0x00

    int 0x13 ; calling interupt service

    jc HANDLEDISKERROR

    ;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Calculating track, head, sector address
    ;;;;;;;;;;;;;;;;;;;;;;;;;

    add si, 0x20
    mov es, si

    mov al, byte[SECTORNUMBER]
    add al, 0x1

    mov byte[SECTORNUMBER], al
    cmp al, 19
    jb READDATA

    xor byte[HEADNUMBER], 0x01
    mov byte[SECTORNUMBER], 0x01

    cmp byte[HEADNUMBER], 0x00
    jne READDATA

    add byte[TRACKNUMBER], 0x01
    jmp READDATA
    
READEND:
    jmp $

HANDLEDISKERROR:
    jmp $


jmp $

MESSAGE1: db 'MINT64 OS Boot Loader Start~!!', 0

times 510 - ($ - $$)    db  0x00

db 0x55
db 0xAA
