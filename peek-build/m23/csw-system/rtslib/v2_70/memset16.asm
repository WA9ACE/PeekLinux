;******************************************************************************
;* MEMSET16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* memset - INITIALIZE MEMORY WITH VALUE.
;* 
;*	C Prototype   : void *memset(void *s, int c, size_t n);
;*      C++ Prototype : void *std::memset(void *s, int c, std::size_t n);
;*
;****************************************************************************
;*
;*   o DESTINATION LOCATION IS IN r0
;*   o INITIALIZATION VALUE IS IN r1
;*   o NUMBER OF BYTES TO INITIALIZE IS IN r2
;*
;*   o ORIGINAL DESTINATION LOCATION RETURNED IN r0
;****************************************************************************
	.state16

	.global $memset

$memset: .asmfunc
	MOV	R12, R0			; save R0 also since original dst
					; address is returned.

	LSL	R3, R0, #30
	BEQ	_word_aligned

_unaligned_loop				; set bytes until there are no more
	CMP	R2, #0			; to set or until address is aligned
        BEQ	_done
	STRB	R1, [R0]
	ADD	R0, #1
	SUB	R2, #1
	LSL	R3, R0, #30
	BNE	_unaligned_loop
          
_word_aligned:
	CMP	R2,#4			; are at least 4 bytes being set
	BCC	_memset3

	LSL	R1, R1, #24		; be safe since prototype has value as
	LSR	R1, R1, #24		; as an int rather than unsigned char

	LSL	R3, R1, #8		; replicate byte in all 4 bytes of
	ORR	R1, R3			; register
	LSL	R3, R1, #16
	ORR	R1, R3

	CMP	R2,#8			; are at least 8 bytes being set
	BCC	_memset7
        
	MOV	R3,R1			; copy bits into another register so
					; 8 bytes at a time can be copied.

	CMP	R2,#16			; are at least 16 bytes being set
	BCC	_memset15

	PUSH	{R4, R5, R6}		; save regs needed by 16 byte copies

	MOV	R4, #15

	SUB	R6, R2, R4		; set up loop count
	AND	R2, R4			; determine number of bytes to set
					; after setting 16 byte blocks


	MOV	R4, R1			; copy bits into 2 other registers so
	MOV	R5, R1			; 16 bytes at a time can be copied
	

_memset16_loop:				; set blocks of 16 bytes
	STMIA	R0!, {R1, R3, R4, R5}
	SUB	R6, #16
	BHI	_memset16_loop

	POP	{R4, R5, R6}		; resotre regs used by 16 byte copies

_memset15:				; may still be as many as 15 bytes to 
					; set. the address in R0 is guaranteed
					; to be word aligned here.

	CMP	R2, #8
	BCC	_memset7	
	STMIA	R0!, {R1, R3}		; are at least 8 bytes being set
	SUB	R2, #8

_memset7:				; may still be as many as 7 bytes to 
					; set. the address in R0 is guaranteed
					; to be word aligned here.

	CMP	R2,#4			; are at least 4 bytes being set
	BCC	_memset3
	STMIA	R0!, {R1}
	SUB	R2, #4

_memset3:				; may still be as many as 3 bytes to 
					; set. the address in R0 is guaranteed
					; to be word aligned here.

	CMP	R2, #0
	BEQ	_done			; if 0 bytes left to set then done

	SUB	R2, #1
_memset1_loop:
	STRB	R1, [R0, R2]
	SUB	R2, #1
	BCS	_memset1_loop


_done:					; all done, restore regs and return
	MOV	R0, R12
	MOV	PC, LR

	.endasmfunc

	.end
