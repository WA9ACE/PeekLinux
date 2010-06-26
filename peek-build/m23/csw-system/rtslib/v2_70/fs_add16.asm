;******************************************************************************
;* FS_ADD16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;*****************************************************************************
;* FS$ADD/FS$SUB - ADD/SUBTRACT TWO IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	           POINT NUMBERS.
;*****************************************************************************
;*
;*   o INPUT OP1 IS IN r0
;*   o INPUT OP2 IS IN r1
;*   o RESULT IS RETURNED IN r0
;*   o FOR SUBTRACTION, INPUT OP2 IN r1 IS KILLED 
;*
;*   o SUBTRACTION, OP1 - OP2, IS IMPLEMENTED WITH ADDITION, OP1 + (-OP2)
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY (0x7f800000/ff800000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*
;*   o IF OPERATION INVOLVES INFINITY AS AN INPUT, THE FOLLOWING SUMMARIZES
;*     THE RESULT:
;*                   +----------+----------+----------+ 
;*         ADDITION  + OP2 !INF | OP2 -INF + OP2 +INF +
;*        +----------+==========+==========+==========+ 
;*        + OP1 !INF +    -     |   -INF   +   +INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 -INF +   -INF   |   -INF   +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 +INF +   +INF   |   +INF   +   +INF   +
;*        +----------+----------+----------+----------+
;*
;*                   +----------+----------+----------+ 
;*       SUBTRACTION + OP2 !INF | OP2 -INF + OP2 +INF +
;*        +----------+==========+==========+==========+ 
;*        + OP1 !INF +    -     |   +INF   +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 -INF +   -INF   |   -INF   +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 +INF +   +INF   |   +INF   +   +INF   +
;*        +----------+----------+----------+----------+
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

	.global FS$ADD
	.global FS$SUB


m0	.set	r2
e0	.set	r3
m1	.set	r4
e1	.set	r5
shift	.set	r6
tmp	.set	r7

FS$SUB:	.asmfunc stack_usage(24)
	PUSH	{r2-r7}			;
	MOV	tmp, #1			;
	LSL	tmp, tmp, #31		;
	EOR	r1, tmp			; NEGATE INPUT #2
	B	$1			;

FS$ADD:	PUSH	{r2-r7}			;
	MOV	tmp, #1			;
	LSL	tmp, tmp, #31		;

$1:	LSL	m1, r1, #8		; PUT INPUT #2 MANTISSA IN m1
	LSL	e1, r1, #1		; PUT INPUT #2 EXPONENT IN e1
	LSR	e1, e1, #24		;
	BNE	$2			;

	CMP	m1, #0			; IF DENORMALIZED NUMBER (m0 != 0 AND
	BNE	unfl			; e1 == 0), THEN UNDERFLOW
	POP	{r2-r7}			;  ELSE IT IS ZERO SO RETURN INPUT #1
	MOV	pc, lr			;

$2:	ORR	m1, tmp			; SET IMPLIED ONE IN MANTISSA

	CMP	e1, #0xFF		; IF e1 == 0xFF, THEN OVERFLOW
	BEQ	ovfl1			;

	LSR	m1, m1, #2		; ADJUST THE MANTISSA
	CMP	r1, #0			; IF INPUT #2 IS NEGATIVE,
	BPL	$3			;
	NEG	m1, m1 			;  THEN NEGATE THE MANTISSA

$3:	LSL	m0, r0, #8		; PUT INPUT #1 MANTISSA IN m0
	LSL	e0, r0, #1		; PUT INPUT #1 EXPONENT IN e0
	LSR	e0, e0, #24		;
	BNE	$4			;

	CMP	m0, #0			; IF A DENORMALIZED NUMBER 
	BNE	unfl			; (m0 != 0 AND e0 == 0), THEN UNDERFLOW
	MOV	r0, r1			;  ELSE IT IS ZERO SO RETURN INPUT #2
	POP	{r2-r7}			; 
	MOV	pc, lr			;

$4:	ORR	m0, tmp			; SET IMPLIED ONE IN MANTISSA

	CMP	e0, #0xFF		; IF e0 == 0xFF, THEN OVERFLOW
	BEQ	ovfl0			;

	LSR	m0, m0, #2		; ADJUST THE MANTISSA
	CMP	r0, #0			; IF INPUT #1 IS NEGATIVE,
	BPL	$5			;
	NEG	m0, m0			;  THEN NEGATE THE MANTISSA
	

$5:	SUB	shift, e0, e1		; GET THE SHIFT AMOUNT
	BPL	$6			;
	MOV	tmp, m0			; IF THE SHIFT AMOUNT IS NEGATIVE, THEN
	MOV	m0, m1			;  SWAP THE TWO MANTISSA SO THAT m0
	MOV	m1, tmp			;  CONTAINS THE LARGER VALUE,
	NEG	shift, shift		;  AND NEGATE THE SHIFT AMOUNT,
	MOV	e0, e1			;  AND ENSURE THE LARGER EXP. IS IN e0

$6:	CMP	shift, #30		; IF THE SECOND MANTISSA IS SIGNIFICANT,
	BPL	$7			;
	ASR	m1, shift		;  ADD IT TO THE FIRST MANTISSA
	ADD	m0, m1			;

$7:	CMP	m0, #0x0		; IF THE RESULT IS ZERO, 
	BEQ	unfl 			;  THEN UNDERFLOW
	BPL	$8			;
	NEG	m0, m0			; IF THE RESULT IS NEGATIVE, THEN
	MOV	tmp, #0x1		;  NEGATE THE RESULT AND
	B	loop			;

$8:	MOV	tmp, #0x0		; NOTE THE SIGN

loop:	SUB	e0, #1			; NORMALIZE THE RESULTING MANTISSA
	LSL	m0, m0, #1		; ADJUSTING THE EXPONENT AS NECESSARY
	BPL	loop			;

	ADD	m0, #0x80		; ROUND THE MANTISSA TO THE NEAREST
	BCC	$9			;
	ADD	e0, e0, #1		; ADJUST EXPONENT IF AN OVERFLOW OCCURS
	B	ct			;

$9:	LSL	m0, m0, #1		; REMOVE THE IMPLIED ONE

ct:	ADD	e0, #2			; NORMALIZE THE EXPONENT
	BLE	unfl			; CHECK FOR UNDERFLOW
	CMP	e0, #0xFF		;
	BCS	ovfl			; CHECK FOR OVERFLOW

	LSR	r0, m0, #9		; REPACK THE MANTISSA INTO r0
	LSL	e0, e0, #23		;
	ORR	r0, e0			; REPACK THE EXPONENT INTO r0
	LSL	tmp, tmp, #31		;
	ORR	r0, tmp			; REPACK THE SIGN INTO r0

	POP	{r2-r7}			;
	MOV	pc, lr			;
	
unfl:	MOV	r0, #0			; UNDERFLOW
	POP	{r2-r7}			;
	MOV	pc, lr			;

					; OVERFLOW
ovfl1:  MOV	r0, r1			; SIGN BIT in R1
ovfl0:	LSR	tmp, r0, #31 		; ISOLATE SIGN BIT
ovfl:	LSL	tmp, tmp, #31		; 
	MOV	r0, #0ffh		; EXPONENT = Emax = 255
	LSL	r0, r0, #23		; SHIFT EXPONENT INTO PLACE
	ORR	r0, tmp			; COMBINE WITH SIGN BIT
	POP	{r2-r7}			;
	MOV	pc, lr			;

	.endasmfunc

	.end
