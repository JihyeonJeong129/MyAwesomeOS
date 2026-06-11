[BITS 32]

global kReadCPUID, kSwitchAndExcute64bitKernel

SECTION .text

;;;;;;;;;;;;;;;;;;;;;;;;;
; Return CPUID
; PARAM : DWORD dwEAX, DWORD *pdwEAX, *pdwEBX, *pdwECX, *pdEDX
;;;;;;;;;;;;;;;;;;;;;;;;;

kReadCPUID:
    push ebp
    mov ebp, esp
    push eax
    push ebx
    push ecx
    push edx
    push esi

    ; Excute CPUID Instruction (using EAX)
    mov eax, dword[ebp + 8]
    cpuid

    ; saving return value
    ;pdwEAX
    mov esi, dword[ebp + 12]
    mov dword[esi], eax

    ;pdwEBX
    mov esi, dword[ebp + 16]
    mov dword[esi], ebx

    ;pdwECX
    mov esi, dword[ebp + 20]
    mov dword[esi], ecx

    ;pdwEDX
    mov esi, dword[ebp + 24]
    mov dword[esi], edx

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;
; Switch IA-32e mode and Excute 64-bit Kernel
; PARAM : None
;;;;;;;;;;;;;;;;;;;;;;;;;
kSwitchAndExcute64bitKernel:
    ;set PAE bit to 1 (reg CR4)
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax

    ;PML4 Table Address and Cache enable (reg CR3)
    mov eax, 0x100000
    mov cr3, eax

    ;Enable IA-32e mode (set IA32_EFER.LME to 1)
    mov ecx, 0xc0000080
    rdmsr

    or eax, 0x0100
    wrmsr

    ; Enable cache and paging
    ; reg CR0 NW(bit 29) = 0, CD(bit 30) = 0, PG(bit 31) = 1
    mov eax, cr0
    or eax, 0xE0000000
    xor eax, 0x60000000
    mov cr0, eax

    jmp 0x08:0x200000

    jmp $
