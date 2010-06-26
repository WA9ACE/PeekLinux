;******************************************************************************
;* I_DIV32.ASM  - 32 BIT STATE -  v2.54                                       *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* I_DIV/I_MOD - DIVIDE TWO SIGNED 32 BIT NUMBERS.
;* I$DIV/I$MOD - 16 BIT STATE INTERFACE TO I_DIV/I_MOD.
;*  
;****************************************************************************
;*
;*   o DIVIDEND IS IN r0
;*   o DIVISOR IS IN r1
;*
;*   o QUOTIENT IS PLACED IN r1
;*   o REMAINDER IS PLACED IN r0
;*
;*   o DIVIDE BY ZERO RETURNS ZERO
;*   o SIGN OF REMAINDER IS THE SIGN OF THE DIVIDEND
;*
;****************************************************************************
	.global I_DIV
	.global I_MOD

dvs	.set	r2		; WORK COPY OF THE DIVISOR (SHIFTED)
quo	.set	r3		; WORK COPY OF THE QUOTIENT
negs	.set	lr		; SAVED COPY OF THE SIGNS

	.if !$$isdefed("__small_divide__")
	.global I$DIV
	.global I$MOD

	.align
	.state16
I$DIV:	.asmfunc 
I$MOD:	BX	pc			; CHANGE TO 32-BIT STATE
	NOP
	.endasmfunc
	.endif
	
	.state32
I_DIV:	.asmfunc stack_usage(12)
I_MOD:	STMFD	sp!, {dvs, quo, lr}	; SAVE CONTEXT
	
	MOV	negs, r1, LSR #1	; MOVE THE SIGN OF THE DIVISOR TO negs
	EORS	negs, negs, r0, ASR #1	; MOVE THE SIGN OF THE DIVIDEND TO negs
	RSBMI	r0, r0, #0		; ABSOLUTE SIGN OF THE DIVIDEND

	ANDS	quo, r1, #0x80000000	; INITIALIZE THE QUOTIENT
	RSBNE	r1, r1, #0		; ABSOLUTE SIGN OF THE DIVISOR
	MOVS	dvs, r1			; CHECK FOR DIVISION BY ZERO, AND
	BEQ	div_by_zero		; INITIALIZE THE DIVISOR (SHIFTED)

	CMP	dvs, r0,  LSR #16	; CALCULATE THE MAXIMUM DIVISOR
	MOVLS	dvs, dvs, LSL #16	; SHIFT AMOUNT WITH PSEUDO BINARY
	CMP	dvs, r0,  LSR #8	; SEARCH.
	MOVLS	dvs, dvs, LSL #8	;

	CMP	dvs, r0, LSR #1		; NOW FIND EXACTLY WHERE THE SHIFTED
	BHI	mod1			; DIVISOR SHOULD BE SO THAT WE CAN
	CMP	dvs, r0, LSR #2		; JUMP INTO THE CORRECT LOCATION
	BHI	mod2			; OF THE UNROLLED DIVIDE LOOP.
	CMP	dvs, r0, LSR #3		;
	BHI	mod3			;
	CMP	dvs, r0, LSR #4		;
	BHI	mod4			;
	CMP	dvs, r0, LSR #5		;
	BHI	mod5			;
	CMP	dvs, r0, LSR #6		;
	BHI	mod6			;
	CMP	dvs, r0, LSR #7		;
	BHI	mod7			;

divl:					; DIVIDE LOOP UNROLLED 8 TIMES
	CMP	r0, dvs, LSL #7		; IF DIVIDEND IS LARGER THAN DIVISOR,
	ADC	quo, quo, quo		; SHIFT A 1 INTO THE QUOTIENT AND 
	SUBCS	r0, r0, dvs, LSL #7	; SUBTRACT THE DIVISOR, ELSE SHIFT A 0.
	CMP	r0, dvs, LSL #6		;

mod7:	ADC	quo, quo, quo		;
	SUBCS	r0, r0, dvs, LSL #6	;
	CMP	r0, dvs, LSL #5		;

mod6:	ADC	quo, quo, quo		;
	SUBCS	r0, r0, dvs, LSL #5	;
	CMP	r0, dvs, LSL #4		;

mod5:	ADC	quo, quo, quo		;
	SUBCS	r0, r0, dvs, LSL #4	;
	CMP	r0, dvs, LSL #3		;

mod4:	ADC	quo, quo, quo		;
	SUBCS	r0, r0, dvs, LSL #3	;
	CMP	r0, dvs, LSL #2		;

mod3:	ADC	quo, quo, quo		;
	SUBCS	r0, r0, dvs, LSL #2	;
	CMP	r0, dvs, LSL #1		;

mod2:	ADC	quo, quo, quo		;
	SUBCS	r0, r0, dvs, LSL #1	;

mod1:	CMP	r0, dvs			;
	ADC	quo, quo, quo		;
	SUBCS	r0, r0, dvs		;

	CMP	r1, dvs			; IF THERE IS SHIFTED DIVISOR, THEN
	MOVCC	dvs, dvs, LSR #8	; CONTINUE THE LOOP.
	BCC	divl			;

	MOV	r1, quo			; ELSE DONE. PLACE THE QUOTIENT
	MOVS	negs, negs, LSL #1	; IT ITS RIGHT PLACE, AND
	RSBMI	r1, quo, #0		; SET THE SIGN OF THE QUOTIENT AND
	RSBCS	r0, r0, #0		; REMAINDER.
	.if !$$isdefed("__small_divide__")
	LDMFD	sp!, {dvs, quo, lr}
	BX	lr
	.else
	LDMFD	sp!, {dvs, pc}
	.endif

div_by_zero:
	MOV	r0, #0			; DIVIDE BY ZERO RETURNS ZERO
	.if !$$isdefed("__small_divide__")
	LDMFD	sp!, {dvs, quo, lr}
	BX	lr
	.else
	LDMFD	sp!, {dvs, pc}
	.endif

	.endasmfunc

	.end

