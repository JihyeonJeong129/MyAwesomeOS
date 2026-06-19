[BITS 64]           

SECTION .text       

extern Main

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Code Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
	mov ax, 0x10        
	mov ds, ax         
	mov es, ax          
	mov fs, ax          
	mov gs, ax          
	
	mov ss, ax          
		mov rsp, 0x700000
		mov rbp, 0x700000
	
	call Main           
	
	jmp $
