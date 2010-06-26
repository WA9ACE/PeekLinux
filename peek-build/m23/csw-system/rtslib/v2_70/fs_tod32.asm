;******************************************************************************
;* FS_TOD32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS_TOFD - CONVERT AN IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	     POINT NUMBER TO 754 FORMAT DOUBLE PRECISION FLOATING 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0
;*   o RESULT IS RETURNED IN r0:r1
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY 
;*       (0x7ff00000:00000000) or (0xfff00000:00000000) 
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
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

	.global	FS_TOFD

e0	.set	lr

FS_TOFD: .asmfunc stack_usage(4)
	STMFD	sp!, {lr}		; SAVE CONTEXT

	MOVS	e0, r0, LSL #1		; SETUP EXPONENT IN e0
	MOVS	e0, e0, LSR #24		;
	BEQ	unfl			; CHECK FOR UNDERFLOW / ZERO

	CMP	e0, #0xFF		; CHECK FOR INFINITY
	BEQ	ovfl			; AND RETURN OVERFLOW

	ADD	e0, e0, #0x380		; ADJUST FOR THE BIAS

	MOV	r1, r0, LSR #31		; ENCODE SIGN WITH EXPONENT
	ORR	e0, e0, r1, LSL #11	;

	MOV	r1, r0, LSL #29		; SETUP LOW PART OF MANTISSA
	MOV	r0, r0, LSL #9		; AND SETUP HIGH PART OF MANTISSA
	MOV	r0, r0, LSR #12		;
	ORR	r0, r0, e0, LSL #20	; OR SIGN AND EXPONENT INTO RESULT
	LDMFD	sp!, {pc}		;
	
unfl:	MOV	r0, #0x0		; IF UNDERFLOW, RETURN 0
	MOV	r1, #0x0		;
	LDMFD	sp!, {pc}		;

ovfl:	MOV	r1, #0			; IF OVERFLOW, RETURN +/- INFINITY
	MOV	r0, r0, LSR #23		; ISOLATE SIGN AND EXPONENT
	MOV	r0, r0, LSL #3
	ADD	r0, r0, #7
	MOV	r0, r0, LSL #20
	LDMFD	sp!, {pc}		;

	.endasmfunc

	.end
