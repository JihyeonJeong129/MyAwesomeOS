[BITS 64]

SECTION .text

global kInPortByte, kOutPortByte

; Read 1 byte from port
kInPortByte:
    push rdx

    mov rdx, rdi
    mov rax, 0

    in al, dx

    pop rdx

    ret


;Write 1byte to port
kOutPortByte:
    push rdx
    push rax

    mov rdx, rdi
    mov rax, rsi

    out dx, al

    pop rax
    pop rdx

    ret


global kLoadGDTR, kLoadTR, kLoadIDTR

; Set GDT Table at GDTR Registers
kLoadGDTR:
    lgdt [rdi]
    ret

; Set TSS Segment Descriptor at TR Registers
kLoadTR:
    ltr di
    ret

; Set IDT Table at IDTR Registers
kLoadIDTR:
    lidt [rdi]
    ret


global kGenerateDivideError

kGenerateDivideError:
    mov rax, 1
    xor rdx, rdx
    xor rcx, rcx
    div rcx
    ret



global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS

kEnableInterrupt:
    sti
    ret

kDisableInterrupt:
    cli
    ret

kReadRFLAGS:
    pushfq
    pop rax
    ret


global kReadTSC

kReadTSC:
    push rdx
    
    rdtsc

    shl rdx, 32
    or rax, rdx

    pop rdx
    ret


global kSwitchContext

; Macro which save context and change selector
%macro KSAVECONTEXT 0
    push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov ax, ds
    push rax

    mov ax, es
    push rax

    push fs
    push gs
%endmacro

; Macro which recover context
%macro KLOADCONTEXT 0
    pop gs
    pop fs
    
    pop rax
    mov es, ax
    
    pop rax
    mov ds, ax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
%endmacro

; Save current context to "Current Context" 
; and Recover context from "Next Tesk"
kSwitchContext:
    push rbp
    mov rbp, rsp

    pushfq
    cmp rdi, 0 ; Check Current Context is NULL
    je .LoadContext
    popfq

    ; Save Current Context
    push rax

    mov ax, ss
    mov qword[rdi + (23*8)], rax

    mov rax, rbp
    add rax, 16
    mov qword[rdi + (22*8)], rax

    pushfq
    pop rax
    mov qword[rdi + (21*8)], rax

    mov ax, cs
    mov qword[rdi + (20*8)], rax

    mov rax, qword[rbp+8]
    mov qword[rdi + (19*8)], rax

    pop rax
    pop rbp

    add rdi, (19*8)
    mov rsp, rdi
    sub rdi, (19*8)

    KSAVECONTEXT

.LoadContext:
    mov rsp, rsi

    ; Recover Registers from Context
    KLOADCONTEXT
    iretq
