;******************************************************************************
;* MEMSET32.ASM  - 32 BIT STATE -  v2.54                                      *
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
	.state32

	.global _memset

_memset: .asmfunc stack_usage(8)
	STMFD	SP!, {R0, LR}		; save R0 also since original dst
					; address is returned.

	TST	R0, #3			; check for word alignment
	BEQ	_word_aligned

	CMP	R2, #0			; set bytes until there are no more
					; to set or until address is aligned
_unaligned_loop:
	STRHIB	R1, [R0], #1
	SUBHIS	R2, R2, #1
	TSTHI	R0, #3
	BNE	_unaligned_loop
          
	CMP	R2, #0			; return early if no more bytes
	LDMEQFD	SP!, {R0, PC}		; to set.

_word_aligned:
	AND	R1, R1, #255		; be safe since prototype has value as
					; as an int rather than unsigned char

	ORR	R1, R1, R1, LSL	#8      ; replicate byte in 2nd byte of
					; register

	CMP	R2,#4			; are at least 4 bytes being set
	BCC	_memset3

	ORR	R1, R1, R1, LSL	#16	; replicate byte in upper 2 bytes
					; of register. note that each of
					; the bottom 2 bytes already contain 
					; the byte value from above.

	CMP	R2,#8			; are at least 8 bytes being set
	BCC	_memset7
        
	MOV	LR,R1			; copy bits into another register so
					; 8 bytes at a time can be copied.
					; use LR since it is already being
					; saved/restored.

	CMP	R2,#16			; are at least 16 bytes being set
	BCC	_memset15

	STMFD	SP!, {R4}		; save regs needed by 16 byte copies

	MOV	R4, R1			; copy bits into 2 other registers so
	MOV	R12, R1			; 16 bytes at a time can be copied

	SUB	R3, R2, #15		; set up loop count
	AND	R2, R2, #15		; determine number of bytes to set
					; after setting 16 byte blocks

_memset16_loop:				; set blocks of 16 bytes
	STMIA	R0!, {R1, R4, R12, LR}
	SUBS	R3, R3, #16
	BHI	_memset16_loop

	LDMFD	SP!, {R4}		; resotre regs used by 16 byte copies

_memset15:				; may still be as many as 15 bytes to 
					; set. the address in R0 is guaranteed
					; to be word aligned here.

	TST	R2, #8			; are at least 8 bytes being set
	STMNEIA	R0!, {R1, LR}


_memset7:				; may still be as many as 7 bytes to 
					; set. the address in R0 is guaranteed
					; to be word aligned here.

	TST	R2, #4			; are at least 4 bytes being set
	STRNE	R1, [R0], #4

_memset3:				; may still be as many as 3 bytes to 
					; set. the address in R0 is guaranteed
					; to be word aligned here.

	TST	R2, #2			; are there at least 2 more bytes to 
	STRNEH	R1, [R0], #2		; set.  the address in R0 is guaranteed
					; to be half-word aligned here.
	
	TST	R2, #1			; is there one remaining byte to set
	STRNEB	R1, [R0]


	LDMFD     SP!, {R0, PC}		; restore regs and return

	.endasmfunc

	.end
