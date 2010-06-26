;******************************************************************************
;* FD_TOU16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD$TOU - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;*          INTO AN UNSIGNED 32 BIT INTEGER 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0:r1
;*   o RESULT IS RETURNED IN r0
;*   o INPUT OP IN r1 IS DESTROYED
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS 0xFFFFFFFF
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
;*   o NEGATIVE VALUE RETURNS ZERO
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

	.global	FD$TOU, FD$TOI

ope	.set	r2
tmp	.set	r3

FD$TOU:	.asmfunc stack_usage(8)
	PUSH	{r2, r3, lr}		; SAVE CONTEXT

	CMP	r0, #0			; CHECK FOR A NEGATIVE VALUE
        BPL     pos
        BL      FD$TOI                  ; CAST TO INT
        POP     {r2, r3, pc}


pos:	LSL	ope, r0, #1		; PUT EXPONENT IN ope
	LSR	ope, ope, #21		;

	MOV	tmp, #0x3		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	SUB	ope, ope, tmp		; ADJUST FOR EXPONENT BIAS AND
	BCC	unfl			; CHECK FOR UNDERFLOW

	MOV	tmp, #0x1F		;
	SUB	ope, tmp, ope		; CHECK FOR OVERFLOW
	BCC	ovfl			; IF OVERFLOW, RETURN 0xFFFFFFFF

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

$1:	POP	{r2, r3, pc}		;

unfl:   MOV     r0, #0                  ; UNDERFLOW
	POP	{r2, r3, pc}		;
 
ovfl:	MOV	r0, #0x0		; IF OVERFLOW, RETURN INFINITY
	SUB	r0, r0, #0x1		;
	POP	{r2, r3, pc}		;
 
	.endasmfunc

	.end
