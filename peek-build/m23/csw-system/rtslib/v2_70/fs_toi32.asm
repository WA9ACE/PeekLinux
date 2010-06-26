;******************************************************************************
;* FS_TOI32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS_TOI - CONVERT AN IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;*          POINT NUMBER TO A 32 BIT SIGNED INTEGER
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
	.state32

	.global FS_TOI


m0	.set	r1
e0	.set	lr

FS_TOI:	.asmfunc stack_usage(8)
	STMFD	sp!, {m0, lr}

	MOV	e0, r0, LSL #1		; PUT EXPONENT IN e0
	MOV	e0, e0, LSR #24		;

	SUBS	e0, e0, #0x7F		; CHECK FOR UNDERFLOW
	MOVMI	r0, #0			; IF UNDERFLOW, RETURN ZERO
	LDMMIFD sp!, {m0, pc}		;

	RSBS	e0, e0, #0x1F		; CHECK FOR OVERFLOW
	BLS	ovfl			; IF OVERFLOW, RETURN INFINITY

	MOV	m0, r0, LSL #8		; PUT MANTISSA IN m0
	ORR	m0, m0, #0x80000000	; SET IMPLIED ONE IN MANTISSA

	MOV	m0, m0, LSR e0		; COMPUTE THE INTEGER VALUE << 1
 
        CMP     r0, #0                  ; IF THE INPUT IS NEGATIVE,
        RSBMI   r0, m0, #0              ;  THEN NEGATE THE RESULT
        MOVPL   r0, m0                  ;

	LDMFD 	sp!, {m0, pc}		;

ovfl:	CMP	r0, #0			; IF OVERFLOW, RETURN INFINITY
	MOV	r0, #0x80000000		; CHECK THE SIGN OF THE INPUT
	SUBPL	r0, r0, #0x1		; AND ADJUST THE VALUE OF INFINITY
	LDMFD	sp!, {m0, pc}		; ACCORDINGLY

	.endasmfunc

	.end
