;******************************************************************************
;* MEMCPY32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************
 
;****************************************************************************
;* C_MEMCPY - COPY CHARACTERS FROM SOURCE TO DEST
;*
;****************************************************************************
;*
;*   o DESTINATION LOCATION IS IN r0
;*   o SOURCE LOCATION IS IN r1
;*   o NUMBER OF CHARACTERS TO BE COPIED IS IN r2
;****************************************************************************
        .state32
 
        .global C_MEMCPY
C_MEMCPY: .asmfunc stack_usage(12)
	CMP	r2, #0			; CHECK FOR n == 0
	BXEQ	lr			;

	STMFD	sp!, {r0, lr}		; SAVE RETURN VALUE AND ADDRESS

	TST	r1, #0x3		; CHECK ADDRESS ALIGNMENT
	BNE	_unaln			; IF NOT WORD ALIGNED, HANDLE SPECIALLY
	TST	r0, #0x3		;
	BNE	_saln			;

_aln:	CMP	r2, #16			; CHECK FOR n >= 16
	BCC	_l16			;

	STMFD	sp!, {r4}		;
	SUB	r2, r2, #16		;
_c16:	LDMIA	r1!, {r3, r4, r12, lr}	; COPY 16 BYTES
	STMIA	r0!, {r3, r4, r12, lr}	;
	SUBS	r2, r2, #16		;
	BCS	_c16			;
	LDMFD	sp!, {r4}		;
	ADDS	r2, r2, #16		; RETURN IF DONE
	LDMEQFD	sp!, {r0, pc}		;
	
_l16:	ANDS	r3, r2, #0xC		;
	BEQ	_cp1			;
	BICS	r2, r2, #0xC		;
	ADR	r12, _4line - 16	;
	ADD	pc, r12, r3, LSL #2	;

_4line:	LDR	r3, [r1], #4		; COPY 4 BYTES
	STR	r3, [r0], #4		;
	LDMEQFD	sp!, {r0, pc} 		; CHECK FOR n == 0
	B	_cp1			;

	LDMIA	r1!, {r3, r12}		; COPY 8 BYTES
	STMIA	r0!, {r3, r12}		;
	LDMEQFD	sp!, {r0, pc} 		; CHECK FOR n == 0
	B	_cp1			;

	LDMIA	r1!, {r3, r12, lr}	; COPY 12 BYTES
	STMIA	r0!, {r3, r12, lr}	;
	LDMEQFD	sp!, {r0, pc} 		; CHECK FOR n == 0

_cp1:	SUBS	r2, r2, #1		;
	ADRNE	r3, _1line - 4		; SETUP TO COPY 1 - 3 BYTES...
	ADDNE	pc, r3, r2, LSL #4	;

_1line:	LDRB	r3, [r1], #1		; COPY 1 BYTE
	STRB	r3, [r0], #1		;
	LDMFD	sp!, {r0, pc}		;

	LDRH	r3, [r1], #2		; COPY 2 BYTES
	STRH	r3, [r0], #2		;
	LDMFD	sp!, {r0, pc}		;
	NOP				;

	LDRH	r3, [r1], #2		; COPY 3 BYTES
	STRH	r3, [r0], #2		;
	LDRB	r3, [r1], #1		;
	STRB	r3, [r0], #1		;
	LDMFD	sp!, {r0, pc}		;

_unaln:	LDRB	r3, [r1], #1		; THE ADDRESSES ARE NOT WORD ALIGNED.
	STRB	r3, [r0], #1		; COPY BYTES UNTIL THE SOURCE IS
	SUBS	r2, r2, #1		; WORD ALIGNED OR THE COPY SIZE
	LDMEQFD	sp!, {r0, pc}		; BECOMES ZERO
	TST	r1, #0x3		;
	BNE	_unaln			;

_saln:	TST	r0, #0x1		; IF THE ADDRESSES ARE OFF BY 1 BYTE
	BNE	_off1			; JUST BYTE COPY

	TST	r0, #0x2		; IF THE ADDRESSES ARE NOW WORD ALIGNED
	BEQ	_aln			; GO COPY.  ELSE THEY ARE OFF BY 2, SO
					; GO SHORT WORD COPY

_off2:	SUBS	r2, r2, #4		; COPY 2 BYTES AT A TIME...
	BCC	_c1h			;
_c2:	LDR	r3, [r1], #4		; START BY COPYING CHUNKS OF 4,
	.if	.TMS470_BIG
	STRH	r3, [r0, #2]		;
	MOV	r3, r3, LSR #16		;
	STRH	r3, [r0], #4		;
	.else
	STRH	r3, [r0], #4		;
	MOV	r3, r3, LSR #16		;
	STRH	r3, [r0, #-2]		;
	.endif
	SUBS	r2, r2, #4		;
	BCS	_c2			;
	CMN	r2, #4			;
	LDMEQFD	sp!, {r0, pc}		;

_c1h:	ADDS	r2, r2, #2		; THEN COPY THE ODD BYTES.
	LDRCSH	r3, [r1], #2		;
	STRCSH	r3, [r0], #2		;
	SUBCS	r2, r2, #2		;
	ADDS	r2, r2, #1		;
	LDRCSB	r3, [r1], #1		;
	STRCSB	r3, [r0], #1		;
	LDMFD	sp!, {r0, pc}		;

_off1:	SUBS	r2, r2, #4		; COPY 1 BYTE AT A TIME...
	BCC	_c1b			;
_c1:	LDR	r3, [r1], #4		; START BY COPYING CHUNKS OF 4,
	.if	.TMS470_BIG
	STRB	r3, [r0, #3]		;
	MOV	r3, r3, LSR #8		;
	STRB	r3, [r0, #2]		;
	MOV	r3, r3, LSR #8		;
	STRB	r3, [r0, #1]		;
	MOV	r3, r3, LSR #8		;
	STRB	r3, [r0], #4		;
	.else
	STRB	r3, [r0], #4		;
	MOV	r3, r3, LSR #8		;
	STRB	r3, [r0, #-3]		;
	MOV	r3, r3, LSR #8		;
	STRB	r3, [r0, #-2]		;
	MOV	r3, r3, LSR #8		;
	STRB	r3, [r0, #-1]		;
	.endif
	SUBS	r2, r2, #4		;
	BCS	_c1			;

_c1b:	ADDS	r2, r2, #4		; THEN COPY THE ODD BYTES.
	LDMEQFD	sp!, {r0, pc}		;
_lp1:	LDRB	r3, [r1], #1		;
	STRB	r3, [r0], #1		;
	SUBS	r2, r2, #1		;
	BNE	_lp1			;
	LDMFD	sp!, {r0, pc}		;

	.endasmfunc

	.end




