;******************************************************************************
;* FD_TOS32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD_TOFS - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
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
	.state32

	.global	FD_TOFS

FD_TOFS: .asmfunc
	MOV	r1, r1, LSR #20		; SETUP MANTISSA INTO r1
	ORR	r1, r1, r0, LSL #12	;

	ADDS	r1, r1, #0x00000100	; ADD 1/2 FOR ROUNDING
	MOVCS	r1, r1, LSR #1		;
	ADDCS	r0, r0, #0x00100000	;

	MOV	r1, r1, LSR #9		; ALIGN MANTISSA
	CMP	r0, #0			; AND ADD SIGN
	ORRMI	r1, r1, #0x80000000	;

	MOV	r0, r0, LSL #1		; SETUP EXPONENT
	MOVS	r0, r0, LSR #21		;
	MOVEQ	pc, lr			; CHECK FOR UNDERFLOW / ZERO

	SUB	r0, r0, #0x380		; ADJUST FOR THE BIAS

	CMP	r0, #0xFF		; CHECK FOR OVERFLOW
	BCS	ovfl			;

	ORR	r0, r1, r0, LSL #23	; ADD EXPONENT INTO RESULT
	MOV	pc, lr			;
	

ovfl:	AND	r1, r1, #0x80000000	; OVERFLOW, RETURN +/- INF
	MOV	r0, #0xFF
	MOV	r0, r0, LSL #23
	ORR	r0, r0, r1
	MOV	pc, lr			;

	.endasmfunc

	.end
