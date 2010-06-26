;******************************************************************************
;* FS_CMP16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS$CMP - COMPARE TWO IEEE 754 FORMAT SINGLE PRECISION FLOATING POINT 
;*          NUMBERS, SETTING THE STATUS ON THE RESULT.
;*
;****************************************************************************
;*
;*   o INPUT OP1 IS IN r0
;*   o INPUT OP2 IS IN r1
;*   o INPUTS ARE NOT DESTROYED
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o +0 == -0
;*
;*   o IF COMPUTING THE RESULT INVOLVES INFINITIES, THE FOLLOWING TABLE
;*     SUMMARIZES THE EFFECTIVE RESULT
;*                   +----------+----------+----------+ 
;*                   + OP2 !INF | OP2 -INF + OP2 +INF +
;*        +----------+==========+==========+==========+ 
;*        + OP1 !INF +    -     |   +INF   +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 -INF +   -INF   |    -0    +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 +INF +   +INF   |   +INF   +    +0    +
;*        +----------+----------+----------+----------+
;*
;*   o THE RESULT OF THE COMPARE IS COMPUTED USING INTEGER SUBRACT IF THE
;*     SIGN OF THE INPUTS IS THE SAME.  THE TABLE SUMMARIZES THE 
;*     IMPLEMENTATION.
;*                 +-----------+-----------+ 
;*                 +   OP2 +   |   OP2 -   +
;*        +--------+===========+===========+ 
;*        + OP1 +  + OP1 - OP2 |    OP1    + 
;*        +--------+-----------+-----------+ 
;*        + OP1 -  +    OP1    | OP2 - OP1 + 
;*        +--------+-----------+-----------+ 
;*
;*   o THE FOLLOWING STATUS BITS ARE SET ON THE RESULT
;*     o Z=1,N=0,V=0  IFF 0
;*     o Z=0,N=1,V=0  IFF -
;*     o Z=0,N=0,V=0  IFF +
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

	.global FS$CMP

tmp	.set	r2
tmp2	.set	r3

FS$CMP:	.asmfunc stack_usage(8)
	PUSH	{r2, r3}		;

	CMP	r0, r1			; HANDLE THE CASE OF BOTH INPUTS EQUAL 
	BEQ	exit			;

	MOV	tmp, r0			;
	ORR	tmp, r1			; HANDLE THE CASE OF BOTH INPUTS BEING
	LSL	tmp, tmp, #1		; ZERO
	CMP	tmp, #0x0		;
	BEQ	exit			;

	MOV	tmp, r0			; TEST OP1 == 0
	LSL	tmp, tmp, #1		; 
	BNE	$2			;

	CMP	tmp, r1			; OP1 == 0, DISREGARD SIGN OF OP1
	POP	{r2, r3}		; WHEN COMPUTING RESULT STATUS
	MOV	pc, lr			;

$2	LSL	tmp, r0, #1		;
	LSR	tmp, tmp, #24	 	; CHECK INPUT #1 FOR INFINITY
	CMP	tmp, #0xFF		;
	BNE	$1			;
	LSL	tmp, r1, #1		; CHECK INPUT #2 FOR INFINITY
	LSR	tmp, tmp, #24		;
	CMP	tmp, #0xFF		;
	BNE	$1			;

	LSR	tmp, r1, #31		; HANDLE THE CASE OF BOTH INPUTS
	LSR	tmp2, r0, #31		; BEING INFINITE
	SUB	tmp, tmp, tmp2		;
	POP	{r2, r3}		;
	MOV	pc, lr			;

$1:	CMP	r1, #0			; CHECK OP2'S SIGN
	BMI	op2_m			;

	CMP	r0, #0			; OP2 IS POSITIVE.
	BMI	exit			; IF OP1 IS POSITIVE,
	CMP	r0, r1 			;  THE RESULT IS OP1 - OP2, 
exit:	POP	{r2, r3}		;  ELSE OP1
	MOV	pc, lr			;

op2_m:	CMP	r0, #0			; OP2 IS NEGATIVE.
	BPL	exit			; IF OP1 IS NEGATIVE,
	CMP	r1, r0			;  THE RESULT IS OP2 - OP1,
	POP	{r2, r3}		;  ELSE OP1
	MOV	pc, lr			;

	.endasmfunc

	.end
