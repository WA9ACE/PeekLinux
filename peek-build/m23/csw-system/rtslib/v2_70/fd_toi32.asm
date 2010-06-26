;******************************************************************************
;* FD_TOI32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD_TOI - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
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
	.state32

	.global	FD_TOI

opm	.set	r2
ope	.set	lr

FD_TOI:	.asmfunc stack_usage(8)
	STMFD	sp!, {r2, lr}		; SAVE CONTEXT

	MOV	ope, r0, LSL #1		; PUT EXPONENT IN ope
	MOV	ope, ope, LSR #21	;

	SUB	ope, ope, #0x300	; ADJUST FOR EXPONENT BIAS AND
	SUBS	ope, ope, #0xFF		; CHECK FOR UNDERFLOW
	MOVMI	r0, #0			; IF UNDERFLOW, RETURN ZERO
	LDMMIFD sp!, {r2, pc}		;

	RSBS	ope, ope, #0x1F		; CHECK FOR OVERFLOW
	BLS	ovfl			; IF OVERFLOW, RETURN INFINITY

	MOV	opm, r0, LSL #11	; PUT HI MANTISSA IN opm
	ORR	opm, opm, #0x80000000	; SET IMPLIED ONE IN HI MANTISSA

	MOV	opm, opm, LSR ope	; COMPUTE THE INTEGER VALUE
	CMP	ope, #11		; FROM HI HALF OF THE MANTISSA.
	ADDCC	ope, ope, #21		; IF THE LOW HALF OF THE MANTISSA IS
	ORRCC	opm, opm, r1, LSR ope	;  SIGNIFICANT, INCLUDE IT ALSO

	CMP	r0, #0			; IF THE INPUT IS NEGATIVE,
	RSBMI	r0, opm, #0		;  THEN NEGATE THE RESULT
	MOVPL	r0, opm			;

	LDMFD 	sp!, {r2, pc}		;

ovfl:	CMP	r0, #0			; IF OVERFLOW, RETURN INFINITY
	MOV	r0, #0x80000000		; CHECK THE SIGN OF THE INPUT
	SUBPL	r0, r0, #0x1		; AND ADJUST INFINITY ACCORDINGLY
	LDMFD	sp!, {r2, pc}		;

	.endasmfunc

	.end
