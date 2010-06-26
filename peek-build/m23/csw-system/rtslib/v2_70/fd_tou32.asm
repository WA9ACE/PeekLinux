;******************************************************************************
;* FD_TOU32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD_TOU - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;* 	    POINT NUMBER TO A 32 BIT UNSIGNED INTEGER
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
	.state32

	.global	FD_TOU, FD_TOI

ope	.set	lr

FD_TOU:	.asmfunc stack_usage(4)
	STMFD   sp!, {lr}               ; SAVE CONTEXT
	CMP	r0, #0			; CHECK FOR A NEGATIVE VALUE
	BPL     pos
        BL      FD_TOI                  ; CAST TO INT
        LDMFD   sp!, {pc}               ;

pos:	MOV	ope, r0, LSL #1		; PUT EXPONENT IN ope
	MOV	ope, ope, LSR #21	;

	SUB	ope, ope, #0x300	; ADJUST FOR EXPONENT BIAS AND
	SUBS	ope, ope, #0xFF		; CHECK FOR UNDERFLOW
	MOVMI	r0, #0			; IF UNDERFLOW, RETURN ZERO
	LDMMIFD sp!, {pc}		;

	RSBS	ope, ope, #0x1F		; CHECK FOR OVERFLOW
	BCC	ovfl			; IF OVERFLOW, RETURN 0xFFFFFFFF

	MOV	r0, r0, LSL #11		; PUT HI MANTISSA IN r0
	ORR	r0, r0, #0x80000000	; SET IMPLIED ONE IN HI MANTISSA

	MOV	r0, r0, LSR ope		; COMPUTE THE INTEGER VALUE FROM HI 
	CMP	ope, #11		; HALF OF THE MANTISSA.  IF THE LOW
	ADDCC	ope, ope, #21		; HALF OF THE MANTISSA IS SIGNIFICANT,
	ORRCC	r0, r0, r1, LSR ope	; INCLUDE IT ALSO.

	LDMFD 	sp!, {pc}		;

ovfl:	MOV	r0, #0x0		; IF OVERFLOW, RETURN 0XFFFFFFFF
	SUB	r0, r0, #0x1		;
	LDMFD	sp!, {pc}		;

	.endasmfunc

	.end
