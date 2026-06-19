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