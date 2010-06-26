;******************************************************************************
;* FS_ADD32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;*****************************************************************************
;* FS_ADD/FS_SUB - ADD/SUBTRACT TWO IEEE 754 FORMAT SINGLE PRECISION FLOATING 
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
	.state32

	.global FS_ADD
	.global FS_SUB


m0	.set	r2
e0	.set	r3
m1	.set	r4
e1	.set	r5
shift	.set	r6
tmp	.set	lr

FS_SUB:	.asmfunc stack_usage(24)
	EOR	r1, r1, #0x80000000	; NEGATE INPUT #2

FS_ADD:
	STMFD	sp!, {r2-r6, lr}

	MOV	m1, r1, LSL #8		; PUT INPUT #2 MANTISSA IN m1
	MOV	e1, r1, LSL #1		; PUT INPUT #2 EXPONENT IN e1
	MOVS	e1, e1, LSR #24		;
	BNE	$1			;

	CMP	m1, #0			; IF DENORMALIZED NUMBER (m0 != 0 AND
	MOVNE	r0, #0			; e1 == 0), THEN UNDERFLOW
	LDMFD	sp!, {r2-r6, pc}	; ELSE IT IS ZERO SO RETURN INPUT #1

$1:	ORR	m1, m1, #0x80000000	; SET IMPLIED ONE IN MANTISSA

	CMP	e1, #0xFF		; IF e1 == 0xFF, THEN OVERFLOW
	
	BEQ	ovfl1			;

	MOV	m1, m1, LSR #2		; ADJUST THE MANTISSA
	CMP	r1, #0			; IF INPUT #2 IS NEGATIVE,
	RSBMI	m1, m1, #0		;  THEN NEGATE THE MANTISSA
	
	MOV	m0, r0, LSL #8		; PUT INPUT #1 MANTISSA IN m0
	MOV	e0, r0, LSL #1		; PUT INPUT #1 EXPONENT IN e0
	MOVS	e0, e0, LSR #24		;
	BNE	$2			;

	CMP	m0, #0			; IF DENORMALIZED NUMBER (m0 != 0 AND
	MOVNE	r0, #0			; e0 == 0), THEN UNDERFLOW
	MOVEQ	r0, r1			; ELSE IT IS ZERO SO RETURN INPUT #2
	LDMFD	sp!, {r2-r6, pc}	;

$2:	ORR	m0, m0, #0x80000000	; SET IMPLIED ONE IN MANTISSA

	CMP	e0, #0xFF		; IF e0 == 0xFF, THEN OVERFLOW
	BEQ	ovfl0			;

	MOV	m0, m0, LSR #2		; ADJUST THE MANTISSA
	CMP	r0, #0			; IF INPUT #1 IS NEGATIVE,
	RSBMI	m0, m0, #0		;  THEN NEGATE THE MANTISSA
	
	SUBS	shift, e0, e1		; GET THE SHIFT AMOUNT
	MOVMI	tmp, m0			; IF THE SHIFT AMOUNT IS NEGATIVE, THEN
	MOVMI	m0, m1			;  SWAP THE TWO MANTISSA SO THAT m0
	MOVMI	m1, tmp			;  CONTAINS THE LARGER VALUE,
	RSBMI	shift, shift, #0	;  AND NEGATE THE SHIFT AMOUNT,
	MOVMI	e0, e1			;  AND ENSURE THE LARGER EXP. IS IN e0

	CMP	shift, #30		; IF THE 2nd MANTISSA IS SIGNIFICANT,
	ADDMI	m0, m0, m1, ASR shift	; ADD IT TO THE FIRST MANTISSA
	CMP	m0, #0x0		;
	MOVEQ	r0, #0			; IF THE RESULT IS ZERO, 
	LDMEQFD	sp!, {r2-r6, pc}	;  THEN UNDERFLOW
	RSBMI	m0, m0, #0x0		; IF THE RESULT IS NEGATIVE, THEN
	MOVMI	tmp, #0x1		;  NEGATE THE RESULT AND
	MOVPL	tmp, #0x0		; NOTE THE SIGN

loop:	MOVS	m0, m0, LSL #1		; NORMALIZE THE RESULTING MANTISSA
	SUB	e0, e0, #1		; ADJUSTING THE EXPONENT AS NECESSARY
	BPL	loop			;

	ADDS	m0, m0, #0x80		; ROUND THE MANTISSA TO THE NEAREST
	ADDCS	e0, e0, #1		; ADJUST EXPONENT IF AN OVERFLOW OCCURS
	MOVCC	m0, m0, LSL #1		; REMOVE THE IMPLIED ONE

	ADDS	e0, e0, #2		; NORMALIZE THE EXPONENT
	MOVLE	r0, #0			; CHECK FOR UNDERFLOW
	LDMLEFD	sp!, {r2-r6, pc}	;
	CMP	e0, #0xFF		; CHECK FOR OVERFLOW
	BCS	ovfl			;

	MOV	r0, m0, LSR #9		; REPACK THE MANTISSA INTO r0
	ORR	r0, r0, e0, LSL #23	; REPACK THE EXPONENT INTO r0
	ORR	r0, r0, tmp, LSL #31	; REPACK THE SIGN INTO r0

	LDMFD	sp!, {r2-r6, pc}	;
	
ovfl1:	MOV	r0, r1			; OVERFLOW
ovfl0:  MOV	tmp, r0, LSR #31	;
ovfl:	MOV	tmp, tmp, LSL #31	; ISOLATE SIGN BIT
	MOV	r0, #0xFF		; SET UP Emax EXPONENT
	MOV	r0, r0, LSL #23
	ORR	r0, r0, tmp		; COMBINE SIGN AND EXPONENT
	LDMFD	sp!, {r2-r6, pc}	;

	.endasmfunc

	.end
