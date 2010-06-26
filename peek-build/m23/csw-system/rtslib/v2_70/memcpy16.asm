;******************************************************************************
;* MEMCPY16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************
 
;****************************************************************************
;* C$MEMCPY - COPY CHARACTERS FROM SOURCE TO DEST
;*
;****************************************************************************
;*
;*   o DESTINATION LOCATION IS IN r0
;*   o SOURCE LOCATION IS IN r1
;*   o NUMBER OF CHARACTERS TO BE COPIED IS IN r2
;****************************************************************************
        .state16
 
        .global C$MEMCPY
C$MEMCPY: .asmfunc
        CMP     r2, #0                  ; CHECK FOR n == 0
        BEQ     _ret2_			;
 
        MOV	r12, r0			; SAVE RETURN VALUE

	LSL	r3, r1, #30		; CHECK ADDRESS ALIGNMENT
	BNE	_unaln			; IF NOT WORD ALIGNED, HANDLE SPECIALLY
	LSL	r3, r0, #30		;
	BNE	_saln			;

_aln:	CMP	r2, #16			; ADDRESSES AND LENGTH ARE WORD ALIGNED
	BCC	_udr16			; IF n < 16, SKIP 16 BYTE COPY CODE

_ovr16:	PUSH	{r4 - r6}		; COPYING 16 BYTES OR MORE.
	SUB	r2, #16			;
_lp16:	LDMIA	r1!, {r3 - r6}		;
	STMIA	r0!, {r3 - r6}		;
	SUB	r2, #16			;
	BCS	_lp16			;
	POP	{r4 - r6}		; RESTORE THE SAVED REGISTERS AND
	ADD	r2, #16			; CONTINUE THE COPY IF THE REMAINDER
	BEQ	_ret_			; IS NONZERO.
	
_udr16: CMP	r2, #4			; IF UNDER 16, THEN COMPUTE 
	BCC	_off1			; THE COPY CODE TO EXECUTE, AND
	CMP	r2, #8			; GO THERE
	BCC	_c4			;
	CMP	r2, #12			;
	BCC	_c8			;

_c12:	LDMIA	r1!, {r3}		; COPY 12 BYTES
	STMIA	r0!, {r3}		; 
_c8:	LDMIA	r1!, {r3} 		; COPY 8 BYTES
	STMIA	r0!, {r3}		;
_c4:	LDMIA	r1!, {r3} 		; COPY 4 BYTES
	STMIA	r0!, {r3}		;

_oddsz:	LSL	r2, r2, #30		; HANDLE THE TRAILING BYTES
	BEQ	_ret_			;
	LSR	r2, r2, #30		;
	B	_lp1			;

_unaln:	LDRB	r3, [r1]		; THE ADDRESSES ARE NOT WORD ALIGNED.
	STRB	r3, [r0]		; COPY BYTES UNTIL THE SOURCE IS
	ADD	r1, r1, #1		;
	ADD	r0, r0, #1		;
	SUB	r2, r2, #1		; WORD ALIGNED OR THE COPY SIZE
	BEQ	_ret_			; BECOMES ZERO
	LSL	r3, r1, #30		;
	BNE	_unaln			;

_saln:	LSL	r3, r0, #31		; IF THE ADDRESSES ARE OFF BY 1 BYTE
	BNE	_off1			; JUST BYTE COPY

	LSL	r3, r0, #30		; IF THE ADDRESSES ARE NOW WORD ALIGNED
	BEQ	_aln			; GO COPY.  ELSE THEY ARE OFF BY 2, SO
					; GO SHORT WORD COPY

_off2:	SUB	r2, r2, #4		; COPY 2 BYTES AT A TIME...
	BCC	_oddb			;
_cp4s:	LDMIA	r1!, {r3}		; LOAD IN CHUNKS OF 4
	.if	.TMS470_BIG
	STRH	r3, [r0, #2]		;
	LSR	r3, r3, #16		;
	STRH	r3, [r0]		;
	.else
	STRH	r3, [r0]		;
	LSR	r3, r3, #16		;
	STRH	r3, [r0, #2]		;
	.endif
	ADD	r0, r0, #4		;
	SUB	r2, r2, #4		;
	BCS	_cp4s			;
	B	_oddb			;

_cp4:	LDMIA	r1!, {r3}		; COPY 1 BYTE AT A TIME, IN CHUNKS OF 4
	.if	.TMS470_BIG
	STRB	r3, [r0, #3]		;
	LSR	r3, r3, #8		;
	STRB	r3, [r0, #2]		;
	LSR	r3, r3, #8		;
	STRB	r3, [r0, #1]		;
	LSR	r3, r3, #8		;
	STRB	r3, [r0]		;
	.else
	STRB	r3, [r0]		;
	LSR	r3, r3, #8		;
	STRB	r3, [r0, #1]		;
	LSR	r3, r3, #8		;
	STRB	r3, [r0, #2]		;
	LSR	r3, r3, #8		;
	STRB	r3, [r0, #3]		;
	.endif
	ADD	r0, r0, #4		;
_off1:	SUB	r2, r2, #4		;
	BCS	_cp4			;

_oddb:	ADD	r2, r2, #4		; THEN COPY THE ODD BYTES.
	BEQ	_ret_			;
	
_lp1:	LDRB	r3, [r1]		;
	STRB	r3, [r0]		;
	ADD	r1, r1, #1		;
	ADD	r0, r0, #1		;
	SUB	r2, r2, #1		;
	BNE	_lp1			;
_ret_:	MOV	r0, r12			;
_ret2_:	MOV	pc, lr

	.endasmfunc

	.end



