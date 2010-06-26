;******************************************************************************
;* FS_DIV16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS$DIV - DIVIDES TWO IEEE 754 FORMAT SINGLE PRECISION FLOATING 
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
	.state16

	.global FS$DIV


in1	.set	r0
in2	.set	r1

m1	.set	r2
e1	.set	r3
m2	.set	r4
e2	.set	r5
sign	.set	r6
tmp	.set	r7

FS$DIV:	.asmfunc stack_usage(24)
	PUSH	{r2-r7}			;

	MOV	tmp, #1			; SETUP 0x80000000 CONSTANT
	LSL	tmp, tmp, #31		;

	MOV	sign, in1		;
	EOR	sign, in2		; SET THE SIGN OF THE RESULT
	LSR	sign, sign, #31		;
	LSL	sign, sign, #31		;

	LSL	m1, in1, #8		; PUT INPUT #1 MANTISSA IN m1
	LSL	e1, in1, #1		; PUT INPUT #1 EXPONENT IN e1
	LSR	e1, e1, #24		;
	BEQ	unfl			; IF e1 == 0, THEN UNDERFLOW
	ORR	m1, tmp			; SET IMPLIED ONE IN MANTISSA IF e1 != 0

	CMP	e1, #0xFF		; IF e1 == 0xFF, THEN OVERFLOW
	BEQ	ovfl			;

	LSL	m2, in2, #8		; PUT INPUT #2 MANTISSA IN m2
	LSL	e2, in2, #1		; PUT INPUT #2 EXPONENT IN e2
	LSR	e2, e2, #24		;
	BEQ	unfl			; IF e2 == 0, THEN UNDERFLOW
	ORR	m2, tmp			; SET IMPLIED ONE IN MANTISSA IF e2 != 0

	CMP	e2, #0xFF		; IF e2 == 0xFF, THEN OVERFLOW
	BEQ	ovfl			;

	SUB	e1, e1, e2		; SUBTRACT EXPONENTS

	; DIVIDE THE MANTISAE:  m1 / m2 => in1
	MOV	in1, #0x0		; INITIALIZE THE QUOTIENT
  	MOV	in2, #32		; INITIALIZE THE SHIFT COUNTER (IN in2)
        LSR     m2, m2, #1	        ; SETUP THE DIVISOR
        LSR     m1, m1, #1      	; SETUP THE DIVIDEND
	
_fdiv_:	CMP	m1, m2			; IF DIVIDEND IS LARGER THAN DIVISOR,
	ADC	in1, in1		; SHIFT A 1 INTO THE QUOTIENT, ELSE 0
	CMP	m1, m2			;
	BCC	$1			;
	SUB	m1, m1, m2		; IF DIVIDEND LARGER, SUBTRACT DIVISOR
$1:	SUB	in2, #0x1		; UPDATE THE SHIFT COUNTER
	BEQ	_flb1_			; EXIT IF OUT OF SHIFTS
	CMP	m1, #0x0		; IF DIVIDEND NOT STILL THERE,
	BEQ	$2			;  EXIT
	LSL	m1, m1, #1		;  ELSE UPDATE DIVIDEND
	BNE	_fdiv_			;  AND CONTINUE

$2:	LSL	in1, in2		; ADJUST THE MANTISSA AS NECESSARY

_flb1_:	TST	in1, tmp		; ALIGN THE MANTISSA
	BNE	$3			;
	LSL	in1, in1, #1		;
	SUB	e1, #0x1		;

$3:	ADD	in1, #0x80		; 1/2 ADJUST FOR ROUNDING
	BCC	$4			;
	ADD	e1, #0x1		;
	B	_flb2_;
$4:	LSL	in1, in1, #1		;

_flb2_:	ADD	e1, #0x7F		; ADJUST FOR BIAS
	BLE	unfl			; AND CHECK FOR UNDERFLOW

	CMP	e1, #0xFF		; AND CHECK FOR EXPONENT OVERFLOW
	BCS	ovfl			;

	LSR	in1, in1, #0x9		; REPACK THE MANTISSA
	LSL	e1, e1, #23		;
	ORR	in1, e1			; REPACK THE EXPONENT INTO in1
	ORR	in1, sign		; REPACK THE SIGN INTO in1
		
	POP	{r2-r7}			;
	MOV	pc, lr			;

unfl:	MOV	in1, #0			; UNDERFLOW
	POP	{r2-r7}			;
	MOV	pc, lr			;

ovfl:	MOV	in1, #0xFF		; OVERFLOW
	LSL	in1, in1, #23		;
	ORR	in1, sign		;
	POP	{r2-r7}			;
	MOV	pc, lr			;

	.endasmfunc

	.end
