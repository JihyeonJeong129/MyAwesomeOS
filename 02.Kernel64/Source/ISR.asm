[BITS 64]

SECTION .text

extern kCommonExceptionHandler, kCommonInterruptHandler, kKeyboardHandler
extern kTimerHandler

global kISRDivideError, kISRDebug, kISRNMI, kISRBreakPoint, kISROverflow
global kISRBoundRangeExceeded, kISRInvalidOpcode, kISRDeviceNotAvailable
global kISRDoubleFault, kISRCoprocessorSegmentOverrun, kISRInvalidTSS
global kISRSegmentNotPresent, kISRStackSegmentFault, kISRGeneralProtection
global kISRPageFault, kISR15, kISRFPUError, kISRAlignmentCheck, kISRMackineCheck
global kISRSIMDError, kISRETCInterrupt, kISRETCException
global kISRNotUsed1, kISRNotUsed2, kISRMachineCheck

global kISRTimer, kISRKeyboard, kISRSlavePIC, kISRSerial1, kISRSerial2
global kISRParallel1, kISRParallel2, kISRFloppy, kISRRTC, kISRReserved
global kISRISRNotUsed1, kISRISRNotUsed2, kISRMouse, kISRCoprocessor
global kISRHDD1, kISRHDD2, kISRTCInterrupt

; Macro which save context and change selector
%macro KSAVECONTEXT 0
    push rbp
    mov rbp, rsp
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

    ;change Segment selector
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
%endmacro

; Macro which load context
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

;;;;;;;;;;;;;;;;;;;;;;;;;
; Exception Handler
;;;;;;;;;;;;;;;;;;;;;;;;;

; #0 Divide Error ISR
kISRDivideError:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 0
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #1 Debug ISR
kISRDebug:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 1
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #2 NMI ISR
kISRNMI:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 2
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #3 BreakPoint ISR
kISRBreakPoint:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 3
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #4 Overflow ISR
kISROverflow:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 4
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #5 Bound Range Exceeded ISR
kISRBoundRangeExceeded:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 5
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #6 Invalid Opcode ISR
kISRInvalidOpcode:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 6
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #7 Device Not Available ISR
kISRDeviceNotAvailable:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 7
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #8 Double Fault ISR
kISRDoubleFault:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 8
    mov rsi, qword[rbp + 8]
    call kCommonExceptionHandler

    KLOADCONTEXT
    add rsp, 8
    iretq

; #9 Coprocessor Segment Overrun ISR
kISRCoprocessorSegmentOverrun:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 9
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #10 Invalid TSS ISR
kISRInvalidTSS:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 10
    mov rsi, qword[rbp + 8]
    call kCommonExceptionHandler

    KLOADCONTEXT
    add rsp, 8
    iretq

; #11 Segment Not Present ISR
kISRSegmentNotPresent:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 11
    mov rsi, qword[rbp + 8]
    call kCommonExceptionHandler

    KLOADCONTEXT
    add rsp, 8
    iretq

; #12 Stack Segment Fault ISR
kISRStackSegmentFault:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 12
    mov rsi, qword[rbp + 8]
    call kISRStackSegmentFault

    KLOADCONTEXT
    add rsp, 8
    iretq

; #13 General Protection ISR
kISRGeneralProtection:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 13
    mov rsi, qword[rbp + 8]
    call kCommonExceptionHandler

    KLOADCONTEXT
    add rsp, 8
    iretq

; #14 Page Fault ISR
kISRPageFault:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 14
    mov rsi, qword[rbp + 8]
    call kCommonExceptionHandler

    KLOADCONTEXT
    add rsp, 8
    iretq

; #15 Reserved ISR
kISR15:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 15
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #16 FPU Error ISR
kISRFPUError:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 16
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #17 Alignment Check ISR
kISRAlignmentCheck:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 17
    mov rsi, qword[rbp + 8]
    call kCommonExceptionHandler

    KLOADCONTEXT
    add rsp, 8
    iretq

; #18 Machine Check ISR
kISRMachineCheck:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 18
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #19 SIMD Floating Point Exception ISR
kISRSIMDError:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 19
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #20 ~ 31 Reserved ISR
kISRETCException:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 20
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq


;;;;;;;;;;;;;;;;;;;;;;;;;
; Interrupt Handler
;;;;;;;;;;;;;;;;;;;;;;;;;

; #32 Timer ISR
kISRTimer:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 32
    call kTimerHandler

    KLOADCONTEXT
    iretq

; #33 Keyboard ISR
kISRKeyboard:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 33
    call kKeyboardHandler

    KLOADCONTEXT
    iretq

; #34 slave PIC ISR
kISRSlavePIC:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 34
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #35 Serial Port2 ISR
kISRSerial2:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 35
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #36 Serial Port1 ISR
kISRSerial1:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 36
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #37 Parallel Port2 ISR
kISRParallel2:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 37
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #38 Flopy disk Controller ISR
kISRFloppy:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 38
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #39 Parallel Port1 ISR
kISRParallel1:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 39
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #40 RTC ISR
kISRRTC:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 40
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #41 Reserved Interrupt ISR
kISRReserved:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 41
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #42 Not Used - 1 ISR
kISRNotUsed1:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 42
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #43 Not Used - 2 ISR
kISRNotUsed2:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 43
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #44 Mouse ISR
kISRMouse:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 44
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #45 Coprocessor ISR
kISRCoprocessor:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 45
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #46 HDD1 ISR
kISRHDD1:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 46
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #47 HDD2 ISR
kISRHDD2:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 47
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq

; #48 ISR for all other interrupts
kISRETCInterrupt:
    KSAVECONTEXT

    ;call handler and insert exception num to handler
    mov rdi, 48
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq
