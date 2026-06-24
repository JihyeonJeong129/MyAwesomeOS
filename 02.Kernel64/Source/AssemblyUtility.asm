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



global kEnableInterrupt, kDisableInterrupt, kReadFLAGS

kEnableInterrupt:
    sti
    ret

kDisableInterrupt:
    cli
    ret

kReadFLAGS:
    pushfq
    pop rax
