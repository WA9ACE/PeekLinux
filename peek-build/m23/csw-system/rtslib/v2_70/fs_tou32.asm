;******************************************************************************
;* FS_TOU32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS_TOU - CONVERT AN IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	    POINT NUMBER TO A 32 BIT UNSIGNED INTEGER
;****************************************************************************
;*
;*   o INPUT OP IS IN r0
;*   o RESULT IS RETURNED IN r0
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

	.global FS_TOU, FS_TOI

e0	.set	lr


FS_TOU:	STMFD   sp!, {lr}               ; SAVE CONTEXT
	CMP	r0, #0			; CHECK FOR A NEGATIVE VALUE
	BPL     pos
        BL      FS_TOI                  ; CAST TO INT
        LDMFD   sp!, {pc}               ;

pos:	MOV	e0, r0, LSL #1		; PUT EXPONENT IN e0
	MOV	e0, e0, LSR #24		;

	SUBS	e0, e0, #0x7F		; CHECK FOR UNDERFLOW
	MOVCC	r0, #0			; IF UNDERFLOW, RETURN ZERO
	LDMCCFD sp!, {pc}		;

	RSBS	e0, e0, #0x1F		; CHECK FOR OVERFLOW
	BCC	ovfl			; IF OVERFLOW, RETURN 0xFFFFFFFF

	MOV	r0, r0, LSL #8		; PUT MANTISSA IN r0
	ORR	r0, r0, #0x80000000	; SET IMPLIED ONE IN MANTISSA

        MOV     r0, r0, LSR e0          ; COMPUTE THE INTEGER VALUE

	LDMFD	sp!, {pc}		;

ovfl:	MOV	r0, #0x0		; IF OVERFLOW, RETURN 0xFFFFFFFF
	SUB	r0, r0, #0x1		;
	LDMFD	sp!, {pc}		;

	.end
