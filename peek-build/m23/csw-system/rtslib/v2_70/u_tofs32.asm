;******************************************************************************
;* U_TOFS32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* U_TOFS - CONVERT A 32 BIT UNSIGNED INTEGER TO AN IEEE 754 FORMAT
;*          SINGLE PRECISION FLOATING POINT NUMBER
;****************************************************************************
;*
;*   o INPUT OP IS IN R0
;*   o RESULT IS RETURNED IN R0
;*
;*   o ROUNDING MODE:  ROUND TO NEAREST
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

	.global U_TOFS

e0	.set	lr

U_TOFS:	.asmfunc stack_usage(4)
	CMP	r0, #0			; 
	MOVEQ	pc, lr			; IF ZERO, RETURN ZERO

	STR	lr, [sp, #-4]!		; SAVE CONTEXT ON STACK

	MOV	e0, #0x9E		; SET THE EXPONENT FIELD

	BMI	cont			;
loop:	MOVS	r0, r0, LSL #1		; NORMALIZE THE MANTISSA
	SUB	e0, e0, #1		; ADJUSTING THE EXPONENT, ACCORDINGLY
	BPL	loop			;

cont:	ADDS	r0, r0, #0x00000080	; ADD 1/2 TO ROUND
	ADDCS	e0, e0, #0x1		; AND ADJUST THE EXPONENT ACCORDINGLY

	MOVCC	r0, r0, LSL #1		; MASK THE IMPLIED ONE IN THE MANTISSA
	MOV	r0, r0, LSR #9		; PACK THE MANTISSA
	ORR	r0, r0, e0, LSL #23	; PACK THE EXPONENT

	LDR	pc, [sp], #4		; RESTORE CONTEXT

	.endasmfunc
	.end
