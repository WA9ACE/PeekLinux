;******************************************************************************
;* FD_CMP32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************
 
;****************************************************************************
;* FD_CMP - COMPARE TWO IEEE 754 FORMAT DOUBLE PRECISION FLOATING POINT 
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
	.state32

	.global FD_CMP

tmp	.set	lr

FD_CMP: .asmfunc stack_usage(4)
	STMFD	sp!, {lr}

	CMP	r0, r2			; HANDLE THE CASE OF BOTH INPUTS EQUAL 
	BNE	$1			;
	CMP	r1, r3			;
        LDMEQFD sp!, {pc}	        ;

$1:	ORRS	tmp, r1, r3		; HANDLE THE CASE OF BOTH INPUTS BEING
	BNE	$2			; ZERO
	ORR	tmp, r0, r2		;
	MOV	tmp, tmp, LSL #1	;
	CMP	tmp, #0x0		;
        LDMEQFD sp!, {pc}               ;

$2:	ORRS	tmp, r1, r0, LSL #1	; TEST OP1 == 0
	BNE	$3			;

	CMP	tmp, r2			; OP1 == 0, DISREGARD SIGN OF OP1
	LDMFD   sp!, {pc} 	        ; WHEN COMPUTING RESULT SIGN

$3:	MOV	tmp, r0, LSL #1		; CHECK INPUT #1 FOR INFINITY
	MOV	tmp, tmp, ASR #21	;
	ADDS	tmp, tmp, #1		;
	BNE	$4			;
	MOV	tmp, r2, LSL #1		; CHECK INPUT #2 FOR INFINITY
	MOV	tmp, tmp, ASR #21	;
	ADDS	tmp, tmp, #1		;
	BNE	$4			;

	MOV	tmp, r2, LSR #31	; HANDLE THE CASE OF BOTH INPUTS
	SUBS	tmp, tmp, r0, LSR #31	; BEING INFINITE
        LDMFD   sp!, {pc}               ;

$4:	CMP	r2, #0			; CHECK OP2'S SIGN
	BMI	$6			;

	CMP	r0, #0			; OP2 IS POSITIVE.  IF OP1 IS NEGATIVE,
        LDMMIFD sp!, {pc}           	; THE RESULT IS OP1,
	CMP	r0, r2			; ELSE IT IS OP1 - OP2
        LDMNEFD sp!, {pc}       	;
	EORS	tmp, r1, r3		; MAKE SURE WE SET THE N BIT CORRECTLY
	BMI	$5			;
	CMP	r1, r3			;
        LDMFD   sp!, {pc} 	        ;
$5:	CMP	r3, #0			;
        LDMFD   sp!, {pc} 	        ;

$6:	CMP	r0, #0			; OP2 IS NEGATIVE.  IF OP1 IS POSITIVE,
        LDMPLFD sp!, {pc}           	; THE RESULT IS OP1
	CMP	r2, r0			; ELSE IT IS OP2 - OP1
        LDMNEFD sp!, {pc}       	;
	EORS	tmp, r1, r3		; MAKE SURE WE SET THE N BIT CORRECTLY
	BMI	$7			;
	CMP	r3, r1			;
        LDMFD   sp!, {pc} 	        ;
$7:	CMP	r1, #0			;
        LDMFD   sp!, {pc} 	        ;

	.endasmfunc

	.end

