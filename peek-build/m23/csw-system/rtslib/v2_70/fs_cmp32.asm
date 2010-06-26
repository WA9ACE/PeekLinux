;******************************************************************************
;* FS_CMP32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS_CMP32 - COMPARE TWO IEEE 754 FORMAT SINGLE PRECISION FLOATING POINT 
;*            NUMBERS, SETTING THE STATUS ON THE RESULT.
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
	.state32

	.global FS_CMP

tmp	.set	lr

FS_CMP:	.asmfunc stack_usage(4)
	STR	lr, [sp, #-4]!

	CMP	r0, r1			; HANDLE THE CASE OF BOTH INPUTS EQUAL 
	LDREQ	pc, [sp], #4		;

	ORR	tmp, r0, r1		; HANDLE THE CASE OF BOTH INPUTS BEING
	MOV	tmp, tmp, LSL #1	; ZERO
	CMP	tmp, #0x0		;
	LDREQ	pc, [sp], #4		;

	MOVS	tmp, r0, LSL #1		; TEST OP1 == 0
	BNE	_ct2_			; 

	CMP	tmp, r1			; OP1 == 0, DISREGARD SIGN OF OP1 
	LDR	pc, [sp], #4		; WHEN COMPUTING RESULT STATUS

_ct2_:	MOV	tmp, r0, LSL #1		;
	MOV	tmp, tmp, LSR #24 	; CHECK INPUT #1 FOR INFINITY
	CMP	tmp, #0xFF		;
	BNE	_ct1_			;
	MOV	tmp, r1, LSL #1		; CHECK INPUT #2 FOR INFINITY
	MOV	tmp, tmp, LSR #24	;
	CMP	tmp, #0xFF		;
	BNE	_ct1_			;

	MOV	tmp, r1, LSR #31	; HANDLE THE CASE OF BOTH INPUTS
	SUBS	tmp, tmp, r0, LSR #31	; BEING INFINITE
	LDR	pc, [sp], #4		;

_ct1_:	CMP	r1, #0			; CHECK OP2'S SIGN
	BMI	op2_m			;

	CMP	r0, #0			; OP2 IS POSITIVE.  IF OP1 IS POSITIVE,
	CMPPL	r0, r1 			; THE RESULT IS OP1 - OP2, ELSE OP1
	LDR	pc, [sp], #4		;

op2_m:	CMP	r0, #0			; OP2 IS NEGATIVE.  IF OP1 IS NEGATIVE,
	CMPMI	r1, r0			; THE RESULT IS OP2 - OP1, ELSE OP1
	LDR	pc, [sp], #4		;

	.endasmfunc

	.end
	

