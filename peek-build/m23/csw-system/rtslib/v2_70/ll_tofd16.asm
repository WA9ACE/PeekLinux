;******************************************************************************
;* LL_TOFD16.ASM  - 16 BIT STATE -  v2.54                                     *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* LL$TOFD - CONVERT AN SIGNED 64 BIT INTEGER INTO AN IEEE 754 FORMAT
;*           DOUBLE PRECISION FLOATING POINT                 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
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

	.global LL$TOFD

e0	.set	r2
tmp     .set    r3
tmp2    .set    r4

LL$TOFD: .asmfunc stack_usage(16)
	CMP	r0, #0			; IF ZERO, RETURN ZERO
	BNE	$1			;
        CMP     r1, #0                  ;
        BNE     $1
	MOV	pc, lr			;

$1:	
        PUSH	{r2-r4, lr}		; SAVE CONTEXT

	; IN LITTLE ENDIAN MODE THE INPUT LONG LONG VALUE IS IN R1:R0. SWAP THE
	; WORDS SO THAT WE HAVE THE LONG LONG VAULUE IN R0:R1.

	.if .TMS470_LITTLE
	MOV	tmp, r0			;
	MOV	r0, r1			;
	MOV	r1, tmp			;
	.endif

	MOV	e0, #0x4		; PRESETUP FOR EXPONENT FIELD

        CMP     r0, #0                  ;
        BPL	$2			; IF NEGATIVE, ENCODE SIGN IN THE
	ADD	e0, #0x8		; EXPONENT FIELD

        MOV     tmp, #0                 ;
	NEG	r1, r1			; AND NEGATE THE INPUT VALUE 
        SBC     tmp, r0                 ;
        MOV     r0, tmp                 ;

$2:	
        LSL	e0, e0, #8		; SETUP REMAINDER OF THE EXPONENT FIELD
	ADD	e0, #0x3F		;

loop:	SUB	e0, e0, #0x1		; NORMALIZE THE MANTISSA
        MOV     tmp2, r0                ;
	LSL	r0, r0, #1		; ADJUSTING THE EXPONENT, ACCORDINGLY
        LSR     tmp, r1, #31            ;
        ORR     r0, tmp                 ;
        LSL     r1, r1, #1              ;
        CMP     tmp2, #0                ;
	BPL	loop			;

done:	
        LSR     r1, r1, #12             ;
        LSL	tmp, r0, #20		; SETUP LOW HALF OF RESULT
        ORR     r1, tmp                 ;
	LSR	r0, r0, #12		; SETUP HIGH HALF OF RESULT
	LSL	e0, e0, #20		;
	ORR	r0, e0			;

	POP	{r2-r4, pc}		;

	.endasmfunc
	.end
