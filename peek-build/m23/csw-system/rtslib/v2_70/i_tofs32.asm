;******************************************************************************
;* I_TOFS32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* I_TOFS - CONVERT A 32 BIT SIGNED INTEGER TO AN IEEE 754 FORMAT
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

	.global I_TOFS


m0	.set	r1
e0	.set	lr

I_TOFS:	.asmfunc stack_usage(8)
	CMP	r0, #0			; 
	MOVEQ	pc, lr			; IF ZERO, RETURN ZERO

	STMFD	sp!, {m0, lr}		; SAVE CONTEXT

	MOVPL	m0, r0			;
	RSBMIS	m0, r0, #0		; IF NEGATIVE, NEGATE IT

	MOV	e0, #0x9E		; SET THE EXPONENT FIELD

	BMI	$1			;		
loop:	MOVS	m0, m0, LSL #1		; NORMALIZE THE MANTISSA
	SUB	e0, e0, #1		; ADJUSTING THE EXPONENT, ACCORDINGLY
	BPL	loop			;

$1:	ADDS	m0, m0, #0x00000080	; ADD 1/2 TO ROUND
	ADDCS	e0, e0, #0x1		; AND ADJUST THE EXPONENT ACCORDINGLY

	MOVCC	m0, m0, LSL #1		; MASK IMPLIED 1 OUT OF THE MANTISSA
	MOV	m0, m0, LSR #9		; PACK THE MANTISSA
	ORR	m0, m0, e0, LSL #23	; PACK THE EXPONENT
	CMP	r0, #0			; IF THE INPUT WAS NEGATIVE
	ORRMI	r0, m0, #0x80000000	;  THEN SET THE SIGN FIELD
	MOVPL	r0, m0			;

	LDMFD 	sp!, {m0, pc}		; RESTORE CONTEXT

	.endasmfunc

	.end
