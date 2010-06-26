;******************************************************************************
;* U_TOFD32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* U_TOFD - CONVERT AN UNSIGNED 32 BIT INTEGER INTO AN IEEE 754 FORMAT
;*          DOUBLE PRECISION FLOATING POINT                 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0
;*   o RESULT IS RETURNED IN r0:r1
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

	.global U_TOFD

U_TOFD:	.asmfunc
	CMP	r0, #0			; IF ZERO, RETURN ZERO
	MOVEQ	r1, #0			;
	MOVEQ	pc, lr			;

	MOV	r1, r0			; MOVE INPUT INTO r1
	MOV	r0, #0x1F		; SETUP THE EXPONENT
	ADD	r0, r0, #0x00000400	;

loop:	MOVS	r1, r1, LSL #1		; NORMALIZE THE MANTISSA
	SUB	r0, r0, #0x1		; ADJUSTING THE EXPONENT, ACCORDINGLY
	BCC	loop			;

	MOV	r0, r0, LSL #20		; SETUP HIGH HALF OF RESULT
	ORR	r0, r0, r1, LSR #12	;
	MOV	r1, r1, LSL #20		; SETUP LOW HALF OF RESULT

	MOV	pc, lr			;

	.endasmfunc
	.end
