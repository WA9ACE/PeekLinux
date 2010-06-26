;******************************************************************************
;* FD_TOS16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD$TOFS - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;* 	     POINT NUMBER TO 754 FORMAT SINGLE PRECISION FLOATING 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0:r1
;*   o RESULT IS RETURNED IN r0
;*   o r1 IS DESTROYED
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY (0x7F800000/FF800000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*
;****************************************************************************
;*
;* +------------------------------------------------------------------+
;* | DOUBLE PRECISION FLOATING POINT FORMAT                           |
;* |   64-bit representation                                          |
;* |   31 30      20 19                  0                            |
;* |   +-+----------+---------------------+                           |
;* |   |S|     E    |        M1           |                           |
;* |   +-+----------+---------------------+                           |
;* |                                                                  |
;* |   31                                0                            |
;* |   +----------------------------------+                           |
;* |   |             M2                   |                           |
;* |   +----------------------------------+                           |
;* |                                                                  |
;* |   <S>  SIGN FIELD    :          0 - POSITIVE VALUE               |
;* |                                 1 - NEGATIVE VALUE               |
;* |                                                                  |
;* |   <E>  EXPONENT FIELD: 0000000000 - ZERO IFF M == 0              |
;* |            0000000001..1111111110 - EXPONENT VALUE(1023 BIAS)    |
;* |                        1111111111 - INFINITY                     |
;* |                                                                  |
;* |   <M1:M2>  MANTISSA FIELDS:  FRACTIONAL MAGNITUDE WITH IMPLIED 1 |
;* +------------------------------------------------------------------+
;*
;****************************************************************************
;*
;* +--------------------------------------------------------------+
;* | SINGLE PRECISION FLOATING POINT FORMAT                       |
;* |   32-bit representation                                      |
;* |   31 30    23 22                    0                        |
;* |   +-+--------+-----------------------+                       |
;* |   |S|    E   |           M           +                       |
;* |   +-+--------+-----------------------+                       |
;* |                                                              |
;* |   <S>  SIGN FIELD    :        0 - POSITIVE VALUE             |
;* |                               1 - NEGATIVE VALUE             |
;* |                                                              |
;* |   <E>  EXPONENT FIELD:       00 - ZERO IFF M == 0            |
;* |                         01...FE - EXPONENT VALUE (127 BIAS)  |
;* |                              FF - INFINITY                   |
;* |                                                              |
;* |   <M>  MANTISSA FIELD:  FRACTIONAL MAGNITUDE WITH IMPLIED 1  |
;* +--------------------------------------------------------------+
;*
;****************************************************************************
	.state16

	.global	FD$TOFS

tmp	.set	r2

FD$TOFS: .asmfunc stack_usage(4)
	PUSH	{r2}			; SAVE CONTEXT

	LSR	r1, r1, #20		; SETUP MANTISSA INTO r1
	LSL	tmp, r0, #12		;
	ORR	r1, tmp			;

	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #8		;
	ADD	r1, r1, tmp		; ADD 1/2 FOR ROUNDING
	BCC	$1			;
	LSR	r1, r1, #1		; IF OVERFLOW, ADJUST MANTISSA AND
	LSL	tmp, tmp, #12		; EXPONENT
	ADD	r0, r0, tmp		;

$1:	LSR	r1, r1, #9		; ALIGN MANTISSA
	CMP	r0, #0			; AND ADD SIGN
	BPL	$2			;
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #31		;
	ORR	r1, tmp			;

$2:	LSL	r0, r0, #1		; SETUP EXPONENT
	LSR	r0, r0, #21		;
	BEQ	unfl			; CHECK FOR UNDERFLOW / ZERO

	MOV	tmp, #0x38		;
	LSL	tmp, tmp, #4		;
	SUB	r0, r0, tmp		; ADJUST FOR THE BIAS

	CMP	r0, #0xFF		; CHECK FOR OVERFLOW
	BCS	ovfl			;

	LSL	r0, r0, #23		;
	ORR	r0, r1			; ADD EXPONENT INTO RESULT
	POP	{r2}			;
	MOV	pc, lr			;
	
unfl:	POP	{r2}			; IF UNDERFLOW, RETURN 0
	MOV	pc, lr			;

ovfl:	LSR	r1, r1, #31		; OVERFLOW, RETURN +/- INF
	LSL	r1, r1, #31
	MOV	r0, #0xFF
	LSL	r0, r0, #23
	ORR	r0, r1			;
	POP	{r2}			;
	MOV	pc, lr			;

	.endasmfunc

	.end
