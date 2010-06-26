;******************************************************************************
;* FD_CMP16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************
 
;****************************************************************************
;* FD$CMP - COMPARE TWO IEEE 754 FORMAT DOUBLE PRECISION FLOATING POINT 
;*          NUMBERS, SETTING THE STATUS ON THE RESULT.
;*
;****************************************************************************
;*
;*   o INPUT OP1 IS IN r0:r1
;*   o INPUT OP2 IS IN r2:r3
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

         .global FD$CMP

tmp	.set	r4
tmp2	.set	r5

FD$CMP: .asmfunc stack_usage(8)
	PUSH	{r4, r5}		;

	CMP	r0, r2			; HANDLE THE CASE OF BOTH INPUTS EQUAL 
	BNE	$1			;
	CMP	r1, r3			;
	BEQ	exit			;

$1:	MOV	tmp, r1			;
	ORR	tmp, r3			; HANDLE THE CASE OF BOTH INPUTS BEING
	BNE	$2			; ZERO
	MOV	tmp, r0			;
	ORR	tmp, r2			;
	LSL	tmp, tmp, #1		;
	CMP	tmp, #0x0		;
	BEQ	exit			;

$2:	LSL	tmp, r0, #1		; TEST OP1 == 0
	ORR	tmp, r1			; 
	BNE	$3			;

	CMP	tmp, r2			; OP1 == 0, DISREGARD SIGN OF OP1
	POP	{r4, r5}		; WHEN COMPUTING RESULT STATUS
	MOV	pc, lr			;

$3:	LSL	tmp, r0, #1		; CHECK INPUT #1 FOR INFINITY
	ASR	tmp, tmp, #21		;
	ADD	tmp, tmp, #1		;
	BNE	$4			;
	LSL	tmp, r2, #1		; CHECK INPUT #2 FOR INFINITY
	ASR	tmp, tmp, #21		;
	ADD	tmp, tmp, #1		;
	BNE	$4			;

	LSR	tmp, r2, #31		; HANDLE THE CASE OF BOTH INPUTS
	LSR	tmp2, r0, #31		; BEING INFINITE
	SUB	tmp, tmp, tmp2		;
        POP	{r4, r5}		;
	MOV	pc, lr			;

$4:	CMP	r2, #0			; CHECK OP2'S SIGN
	BMI	$6			;

	CMP	r0, #0			; OP2 IS POSITIVE.  IF OP1 IS NEGATIVE,
	BMI	exit			; THE RESULT IS OP1
	CMP	r0, r2			; ELSE THE RESULT IS OP1 - OP2
        BNE	exit			;
	MOV	tmp, r1			; MAKE SURE WE SET THE N BIT CORRECTLY
	EOR	tmp, r3			;
	BMI	$5			;
	CMP	r1, r3			;
	POP	{r4, r5}		;
	MOV	pc, lr			;
$5:	CMP	r3, #0			;
	POP	{r4, r5}		;
	MOV	pc, lr			;

$6:	CMP	r0, #0			; OP2 IS NEGATIVE.  IF OP1 IS POSITIVE,
	BPL	exit			; THE RESULT IS OP1
	CMP	r2, r0			; ELSE THE RESULT IS OP2 - OP1
	BNE	exit			;
	MOV	tmp, r1			; MAKE SURE WE SET THE N BIT CORRECTLY
	EOR	tmp, r3			;
	BMI	$7			;
	CMP	r3, r1			;
	POP	{r4, r5}		;
	MOV	pc, lr			;
$7:	CMP	r1, #0			;
exit:	POP	{r4, r5}		;
	MOV	pc, lr			;

	.endasmfunc

	.end
