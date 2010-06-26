;******************************************************************************
;* I_TOFD16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* I$TOFD - CONVERT AN SIGNED 32 BIT INTEGER INTO AN IEEE 754 FORMAT
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
	.state16

	.global I$TOFD

e0	.set	r2

I$TOFD:	.asmfunc stack_usage(4)
	PUSH	{r2}			; SAVE CONTEXT

	MOV	e0, #0x4		; PRESETUP FOR EXPONENT FIELD

	CMP	r0, #0			; IF ZERO, RETURN ZERO
	BNE	$1			;
	MOV	r1, #0			;
	POP	{r2}			;
	MOV	pc, lr			;

$1:	BPL	$2			; IF NEGATIVE, ENCODE SIGN IN THE
	ADD	e0, #0x8		; EXPONENT FIELD
	NEG	r0, r0			;

$2:	LSL	e0, e0, #8		; SETUP REMAINDER OF THE EXPONENT FIELD
	ADD	e0, #0x1F		;

loop:	SUB	e0, e0, #0x1		; NORMALIZE THE MANTISSA
	LSL	r0, r0, #1		; ADJUSTING THE EXPONENT, ACCORDINGLY
	BCC	loop			;

done:	LSL	r1, r0, #20		; SETUP LOW HALF OF RESULT
	LSR	r0, r0, #12		; SETUP HIGH HALF OF RESULT
	LSL	e0, e0, #20		;
	ORR	r0, e0			;

	POP	{r2}			;
	MOV	pc, lr			;

	.endasmfunc

	.end
