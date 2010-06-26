;******************************************************************************
;* FD_TOLL16.ASM  - 16 BIT STATE -  v2.54                                     *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD$TOLL - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;*           INTO A SIGNED 64 BIT INTEGER 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0:r1
;*   o RESULT IS RETURNED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS 0x7FFFFFFF:FFFFFFFF/0x80000000:00000000, DEPENDING 
;*     ON THE SIGN OF THE INPUT
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o ROUNDING MODE:  ROUND TO ZERO
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

	.global	FD$TOLL

	.if .TMS470_LITTLE

o_hi	.set r1
o_lo	.set r0

	.else

o_hi	.set r0
o_lo	.set r1

	.endif

ope	.set	r2
tmp	.set	r3
tmp2    .set    r4
sign	.set	lr

FD$TOLL: .asmfunc stack_usage(16)
        PUSH	{r2-r4, lr}		; SAVE CONTEXT

	LSL	ope, r0, #1		; PUT EXPONENT IN ope
	LSR	ope, ope, #21		;

	MOV	tmp, #0x3		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	SUB	ope, ope, tmp		; ADJUST FOR EXPONENT BIAS AND
	BCC	unfl			; CHECK FOR UNDERFLOW

	MOV	tmp, #0x3F		;
	SUB	ope, tmp, ope		; CHECK FOR OVERFLOW
	BLS	ovfl			; IF OVERFLOW, RETURN INFINITY

	MOV	sign, r0		; SAVE SIGN

	LSL	r0, r0, #11		; PUT MANTISSA IN r0:r1
        LSR     tmp, r1, #21            ;
        ORR     r0, tmp                 ;
        LSL     r1, r1, #11             ;
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #31		;
	ORR	r0, tmp			; SET IMPLIED ONE IN HI MANTISSA


        ; COMPUTE THE INTEGER VALUE
        MOV     tmp, ope                ;
        SUB     tmp, #32                ; IF ope >= 32 
        BCC     $1                      ;
        MOV     r1, r0                  ;    r0:r1 = 0:r0 >> (ope - 32)
        MOV     r0, #0                  ;
	LSR	r1, tmp		        ;
	B       adj_sign                ;
$1:
        LSR     r1, ope                 ; IF ope < 32
        NEG     tmp, tmp                ;    
        MOV     tmp2, r0                ;    r0:r1 = r0:r1 >> ope
        LSL     tmp2, tmp               ;          = r0 >> ope : 
        ORR     r1, tmp2                ;             r1 >> ope | r0 << (32-ope)
        LSR     r0, ope                 ;

adj_sign:	
        MOV	tmp, sign		;
	CMP	tmp, #0x0		; IF THE INPUT IS NEGATIVE,
	BPL	$2			;  
        MOV     tmp, #0                 ;
	NEG	r1, r1			;  THEN NEGATE THE RESULT
        SBC     tmp, r0                 ;
        MOV     r0, tmp                 ;
$2:	
	; IN LITTLE ENDIAN MODE THE OUTPUT LONG LONG VALUE SHOULD BE IN R1:R0.
	; SO SWAP THE REGISTER VALUES BEFORE RETURN.

	.if .TMS470_LITTLE
	MOV	lr, r0			;
	MOV	r0, r1			;
	MOV	r1, lr			;
	.endif

        POP	{r2-r4, pc}		;

unfl:   MOV     o_hi, #0                ; UNDERFLOW
        MOV     o_lo, #0                ;
	POP	{r2-r4, pc}		;
 
ovfl:	MOV	tmp, r0			; IF OVERFLOW, RETURN INFINITY
	MOV	o_lo, #0		;
	MOV	o_hi, #0x1		;
	LSL	o_hi, o_hi, #31		;
	CMP	tmp, #0			; CHECK THE SIGN OF THE INPUT
	BMI	$3			; AND ADJUST THE VALUE OF INFINITY
        MOV     tmp, #0                 ; ACCORDINGLY
	SUB	o_lo, #1		; 
        SBC     o_hi, tmp               ;
$3:	
        POP 	{r2-r4, pc}		;

	.endasmfunc
	.end
