;******************************************************************************
;* FD_TOI16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD$TOI - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;*          INTO A SIGNED 32 BIT INTEGER 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0:r1
;*   o RESULT IS RETURNED IN r0
;*   o INPUT OP IN r1 IS DESTROYED
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS 0x7FFFFFFF/0x80000000, DEPENDING ON THE SIGN OF
;*     THE INPUT
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
;*   o ROUNDING MODE:  ROUND TO ZERO
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
	.state16

	.global	FD$TOI

ope	.set	r2
tmp	.set	r3
sign	.set	lr

FD$TOI:	.asmfunc stack_usage(12)
	PUSH	{r2, r3, lr}		; SAVE CONTEXT

	LSL	ope, r0, #1		; PUT EXPONENT IN ope
	LSR	ope, ope, #21		;

	MOV	tmp, #0x3		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	SUB	ope, ope, tmp		; ADJUST FOR EXPONENT BIAS AND
	BCC	unfl			; CHECK FOR UNDERFLOW

	MOV	tmp, #0x1F		;
	SUB	ope, tmp, ope		; CHECK FOR OVERFLOW
	BLS	ovfl			; IF OVERFLOW, RETURN INFINITY

	MOV	sign, r0		; SAVE SIGN
	LSL	r0, r0, #11		; PUT HI MANTISSA IN r0
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #31		;
	ORR	r0, tmp			; SET IMPLIED ONE IN HI MANTISSA

	LSR	r0, ope			; COMPUTE THE INTEGER VALUE
	CMP	ope, #11		; FROM HI HALF OF THE MANTISSA.
	BCS	$1			; IF THE LOW HALF OF THE MANTISSA IS
	ADD	ope, #21		;  SIGNIFICANT, INCLUDE IT INTO THE 
	LSR	r1, ope			;  INTEGER VALUE, ALSO.
	ORR	r0, r1			;

$1:	MOV	tmp, sign		;
	CMP	tmp, #0x0		; IF THE INPUT IS NEGATIVE,
	BPL	$2			;  THEN NEGATE THE RESULT
	NEG	r0, r0			;
$2:	POP	{r2, r3, pc}		;

unfl:   MOV     r0, #0                  ; UNDERFLOW
	POP	{r2, r3, pc}		;
 
ovfl:	MOV	tmp, r0			; IF OVERFLOW, RETURN INFINITY
	MOV	r0, #0x1		;
	LSL	r0, r0, #31		;
	CMP	tmp, #0			; CHECK THE SIGN OF THE INPUT
	BMI	$3			; AND ADJUST THE VALUE OF INFINITY
	SUB	r0, #1			; ACCORDINGLY
$3:	POP 	{r2, r3, pc}		;

	.endasmfunc

	.end
