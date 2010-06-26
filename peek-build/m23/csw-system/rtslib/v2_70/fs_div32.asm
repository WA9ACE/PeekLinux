;******************************************************************************
;* FS_DIV32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS_DIV - DIVIDES TWO IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	    POINT NUMBERS.
;****************************************************************************
;*
;*   o DIVIDEND INPUT OP1 IS IN r0
;*   o DIVISOR INPUT OP2 IS IN r1
;*   o QUOTIENT IS RETURNED IN r0
;*   o INPUT OP2 IN r1 IS DESTROYED
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY (0x7f800000/ff800000)
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*   o DIVIDE BY ZERO RETURNS ZERO
;*
;*   o IF THE OPERATION INVOLVES INFINITY AS AN INPUT, UNLESS THE OTHER INPUT
;*     IS ZERO, THE RESULT IS INFINITY WITH THE SIGN DETERMINED IN THE USUAL
;*     FASHION.
;*
;****************************************************************************
;*
;* +--------------------------------------------------------------+
;* | SINGLE PRECISION FLOATING POINT FORMAT                       |
;* |								  |
;* |   31 30    23 22                    0			  |
;* |   +-+--------+-----------------------+			  |
;* |   |S|    E   |           M           +			  |
;* |   +-+--------+-----------------------+			  |
;* |								  |
;* |   <S>  SIGN FIELD    :        0 - POSITIVE VALUE		  |
;* |			           1 - NEGATIVE VALUE		  |
;* |								  |
;* |   <E>  EXPONENT FIELD:       00 - ZERO IFF M == 0		  |
;* |			     01...FE - EXPONENT VALUE (127 BIAS)  |
;* |				  FF - INFINITY			  |
;* |								  |
;* |   <M>  MANTISSA FIELD:  FRACTIONAL MAGNITUDE WITH IMPLIED 1  |
;* +--------------------------------------------------------------+
;*
;****************************************************************************
	.state32

	.global FS_DIV


in1	.set	r0
in2	.set	r1

m1	.set	r2
e1	.set	r3
m2	.set	r4
e2	.set	r5
sign	.set	r6
tmp	.set	lr

FS_DIV:	.asmfunc stack_usage(24)
	STMFD	sp!, {r2-r6, lr}

	EORS	sign, in1, in2		; SET THE SIGN OF THE RESULT
	MOVMI	sign, #0x80000000	;
	MOVPL	sign, #0x00000000	;

	MOV	m1, in1, LSL #8		; PUT INPUT #1 MANTISSA IN m1
	MOV	e1, in1, LSL #1		; PUT INPUT #1 EXPONENT IN e1
	MOVS	e1, e1, LSR #24		;
	ORRNE	m1, m1, #0x80000000	; SET IMPLIED ONE IN MANTISSA IF e1 != 0

	MOVEQ	in1, #0			; IF e1 == 0, THEN UNDERFLOW
	LDMEQFD	sp!, {r2-r6, pc}	;

	CMP	e1, #0xFF		; IF e1 == 0xFF, THEN OVERFLOW
	BEQ	ovfl			;

	MOV	m2, in2, LSL #8		; PUT INPUT #2 MANTISSA IN m2
	MOV	e2, in2, LSL #1		; PUT INPUT #2 EXPONENT IN e2
	MOVS	e2, e2, LSR #24		;
	ORRNE	m2, m2, #0x80000000	; SET IMPLIED ONE IN MANTISSA IF e2 != 0

	MOVEQ	in1, #0			; IF e2 == 0, THEN UNDERFLOW
	LDMEQFD	sp!, {r2-r6, pc}	;

	CMP	e2, #0xFF		; IF e2 == 0xFF, THEN OVERFLOW
	BEQ	ovfl			;

	SUB	e1, e1, e2		; SUBTRACT EXPONENTS

	; DIVIDE THE MANTISAE:  m1 / m2 => in1
	MOV	in1, #0x0		; INITIALIZE THE QUOTIENT
  	MOV	tmp, #32		; INITIALIZE THE SHIFT COUNTER
        MOV     m2, m2, LSR #1          ; SETUP THE DIVISOR
        MOV     m1, m1, LSR #1          ; SETUP THE DIVIDEND
	
_fdiv_:
	CMP	m1, m2			; IF DIVIDEND IS LARGER THAN DIVISOR,
	ADC	in1, in1, in1		; SHIFT A 1 INTO THE QUOTIENT, ELSE 0
	SUBCS	m1, m1, m2		; IF DIVIDEND LARGER, SUBTRACT DIVISOR
	SUBS	tmp, tmp, #0x1		; UPDATE THE SHIFT COUNTER
	BEQ	_flb1_			; EXIT IF OUT OF SHIFTS
	CMP	m1, #0x0		; IF DIVIDEND STILL THERE,
	MOVNE	m1, m1, LSL #1		;  UPDATE DIVIDEND
	BNE	_fdiv_			;  CONTINUE

	MOV	in1, in1, LSL tmp	; ADJUST THE MANTISSA AS NECESSARY

_flb1_:	TST	in1, #0x80000000	; ALIGN THE MANTISSA
	MOVEQ	in1, in1, LSL #1	;
	SUBEQ	e1, e1, #0x1		;

	ADDS	in1, in1, #0x00000080	; 1/2 ADJUST FOR ROUNDING
	ADDCS	e1, e1, #0x1		;
	MOVCC	in1, in1, LSL #1	;

_flb2_:	ADDS	e1, e1, #0x7F		; ADJUST FOR BIAS
	MOVLE	in1, #0			; AND CHECK FOR UNDERFLOW
	LDMLEFD	sp!, {r2-r6, pc}	;

	CMP	e1, #0x000000FF		; AND CHECK FOR EXPONENT OVERFLOW
	BCS	ovfl			;

	MOV	in1, in1, LSR #0x9	; REPACK THE MANTISSA
	ORR	in1, in1, e1, LSL #23	; REPACK THE EXPONENT INTO in1
	ORR	in1, in1, sign		; REPACK THE SIGN INTO in1
		
	LDMFD	sp!, {r2-r6, pc}	;

ovfl:	MOV	tmp, #0xFF		; OVERFLOW, RETURN +/- INFINITY
	MOV	tmp, tmp, LSL #23	;
	ORR	in1, sign, tmp		;
	LDMFD	sp!, {r2-r6, pc}	;

	.endasmfunc

	.end
