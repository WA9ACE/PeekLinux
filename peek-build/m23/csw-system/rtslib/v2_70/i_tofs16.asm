;******************************************************************************
;* I_TOFS16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* I$TOFS - CONVERT A 32 BIT SIGNED INTEGER TO AN IEEE 754 FORMAT 
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
	.state16

	.global I$TOFS


m0	.set	r1
e0	.set	r2

I$TOFS:	.asmfunc stack_usage(8)
	PUSH	{r1, r2}		; SAVE CONTEXT

	MOV	e0, #0x9E		; SET THE EXPONENT FIELD

	CMP	r0, #0			; IF ZERO, RETURN ZERO
	BMI	$1			;
	BNE	$2			;
	POP	{r1, r2}		;
	MOV	pc, lr			; 

$1:	NEG	m0, r0			; IF NEGATIVE, NEGATE IT
	BMI	cont			;
	B	loop			;
$2:	MOV	m0, r0			;

loop:	SUB	e0, #1			; NORMALIZE THE MANTISSA
	LSL	m0, m0, #1		; ADJUSTING THE EXPONENT, ACCORDINGLY
	BPL	loop			;

cont:	ADD	m0, #0x80		; ADD 1/2 TO ROUND
	BCC	$3			;
	ADD	e0, #0x1		; AND ADJUST THE EXPONENT ACCORDINGLY
	B	$4			;

$3:	LSL	m0, m0, #1		; MASK IMPLIED 1 OUT OF THE MANTISSA

$4:	LSR	m0, m0, #9		; PACK THE MANTISSA
	LSL	e0, e0, #23		;
	ORR	m0, e0			; PACK THE EXPONENT
	CMP	r0, #0			; IF THE INPUT WAS NEGATIVE
	BPL	$5			;
	MOV	e0, #1			;
	LSL	e0, e0, #31		;
	ORR	m0, e0			;  THEN SET THE SIGN FIELD
$5:	MOV	r0, m0			;
	POP 	{r1, r2}		; RESTORE CONTEXT
	MOV	pc, lr			;

	.endasmfunc

	.end
