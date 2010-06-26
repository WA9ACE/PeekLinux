;******************************************************************************
;* FS_TOI.ASM  - 16 BIT STATE -  v2.54                                        *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS$TOI - CONVERT AN IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	    POINT NUMBER TO A 32 BIT SIGNED INTEGER
;****************************************************************************
;*
;*   o INPUT OP IS IN r0
;*   o RESULT IS RETURNED IN r0
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS 0x7FFFFFFF/80000000, DEPENDING ON THE SIGN OF THE
;*     INPUT
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
;*   o ROUNDING MODE:  ROUND TO ZERO
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

	.global FS$TOI


e0	.set	r1
tmp	.set	r2
sign	.set	lr

FS$TOI:	.asmfunc stack_usage(12)
	PUSH	{r1, r2, lr}		;

	LSL	e0, r0, #1		; PUT EXPONENT IN e0
	LSR	e0, e0, #24		;

	SUB	e0, #0x7F		; CHECK FOR UNDERFLOW
	BMI	unfl			; IF UNDERFLOW, RETURN ZERO

	MOV	tmp, #0x1F		;
	SUB	e0, tmp, e0		;
	BLS	ovfl			; CHECK FOR OVERFLOW

	MOV	sign, r0		; SAVE SIGN
	LSL	r0, r0, #8		; PUT MANTISSA IN r0
	MOV	tmp, #1			;
	LSL	tmp, tmp, #31		;
	ORR	r0, tmp			; SET IMPLIED ONE IN MANTISSA

	LSR	r0, e0			; COMPUTE THE INTEGER VALUE

	MOV	tmp, sign		;
	CMP	tmp, #0			;
	BPL	$1			; IF THE INPUT IS NEGATIVE,
	NEG	r0, r0			;  THEN NEGATE THE RESULT AND RETURN
$1:	POP 	{r1, r2, pc}		;

unfl:	MOV	r0, #0			; UNDERFLOW
	POP 	{r1, r2, pc}		;

ovfl:   MOV	tmp, r0			; OVERFLOW
	MOV	r0, #0x1		;
	LSL	r0, r0, #31		;
	CMP	tmp, #0			; IF INPUT IS NEGATIVE, RETURN
	BMI	$2			; 0x80000000
	SUB	r0, r0, #1
$2:	POP 	{r1, r2, pc}		;

	.endasmfunc

	.end
