;******************************************************************************
;* U_DIV32.ASM  - 32 BIT STATE -  v2.54                                       *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* U_DIV/U_MOD - DIVIDE TWO UNSIGNED 32 BIT NUMBERS.
;* U$DIV/U$MOD - 16 BIT STATE INTERFACE TO U_DIV/U_MOD.
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
;*
;****************************************************************************
	.global U_DIV
	.global U_MOD

dvs	.set	r2		; WORK COPY OF THE DIVISOR (SHIFTED)
quo	.set	lr		; WORK COPY OF THE QUOTIENT

	.if !$$isdefed("__small_divide__")
	.global U$DIV
	.global U$MOD

	.align
	.state16
U$DIV:  .asmfunc
U$MOD:	BX	pc			; CHANGE TO 32-BIT STATE
	NOP
	.endasmfunc
	.endif
	
	.state32
U_DIV:	.asmfunc stack_usage(8)
U_MOD:	STMFD	sp!, {dvs, lr}		; SAVE CONTEXT

	MOVS	dvs, r1			; CHECK FOR DIVISION BY ZERO
	BEQ	div_by_zero		;

	MOV	quo, #0			; INITIALIZE THE QUOTIENT

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

	MOV	r1, quo			; ELSE WE ARE DONE. PLACE THE QUOTIENT
					; IN ITS RIGHT PLACE.
	.if !$$isdefed("__small_divide__")
	LDMFD	sp!, {dvs, lr}
	BX	lr
	.else
	LDMFD	sp!, {dvs, pc}
	.endif
	
div_by_zero:
	MOV	r0, #0			; DIVIDE BY ZERO RETURNS ZERO
	.if !$$isdefed("__small_divide__")
	LDMFD	sp!, {dvs, lr}
	BX	lr
	.else
	LDMFD	sp!, {dvs, pc}
	.endif

	.endasmfunc
	.end
