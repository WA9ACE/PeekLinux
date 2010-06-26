;******************************************************************************
;* FD_MUL16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD$MUL - MULTIPLY TWO IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;*          POINT NUMBERS.
;****************************************************************************
;*
;*   o INPUT OP1 IS IN r0:r1
;*   o INPUT OP2 IS IN r2:r3
;*   o RESULT IS RETURNED IN r0:r1
;*   o INPUT OP2 IN r2:r3 IS NOT DESTROYED
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY 
;*       (0x7ff00000:00000000) or (0xfff00000:00000000) 
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*
;*   o IF THE OPERATION INVOLVES INFINITY AS AN INPUT, UNLESS THE OTHER INPUT
;*     IS ZERO, THE RESULT IS INFINITY WITH THE SIGN DETERMINED IN THE USUAL
;*     FASHION.
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

	.global	FD$MUL

res1	.set	r0	; OVERLOADED WITH INPUT #1
op1e	.set	r1	; OVERLOADED WITH INPUT #1
op1m1	.set	r2	; OVERLOADED WITH INPUT #2
op1m2	.set	r3	; OVERLOADED WITH INPUT #2
tmp	.set	r4
op2m1	.set	r5
op2m2	.set	r6
op2e	.set	r7
res2	.set	r7	; OVERLOADED WITH op2e


FD$MUL:	.asmfunc	stack_usage(32)
	PUSH	{r2-r7, lr}		;

	MOV	tmp, r0			; SET THE SIGN OF THE RESULT
        EOR     tmp, r2                 ;
	LSR	tmp, tmp, #31		;
	LSL	tmp, tmp, #31		;
	PUSH	{tmp}			;

	LSL	op2m1, r2, #11		; BUILD INPUT #2 MANTISSA
	LSR	tmp, r3, #21		;
	ORR	op2m1, tmp		;
	LSL	op2m2, r3, #11		;
	LSL	op2e, r2, #1		; BUILD INPUT #2 EXPONENT
	LSR	op2e, op2e, #21		;
	BEQ	unfl			;

	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #31		; SET IMPLIED ONE IN MANTISSA
	ORR	op2m1, tmp	 	;

	MOV	tmp, #0x7		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	CMP	op2e, tmp		; IF op2e == 0x7FF, THEN OVERFLOW
	BEQ	ovfl			;

	LSL	op1m1, r0, #11		; BUILD INPUT #1 MANTISSA
	LSR	tmp, r1, #21		;
	ORR	op1m1, tmp		;
	LSL	op1m2, r1, #11		;
	LSL	op1e, r0, #1		; BUILD INPUT #1 EXPONENT
	LSR	op1e, op1e, #21		;
	BEQ	unfl			;

	MOV	tmp, #0x1		; SET IMPLIED ONE IN MANTISSA
	LSL	tmp, tmp, #31		;
	ORR	op1m1, tmp	 	;

	MOV	tmp, #0x7		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	CMP	op1e, tmp		; IF op1e == 0x7FF, THEN OVERFLOW
	BEQ	ovfl			;

        ADD     op1e, op1e, op2e        ; ADD EXPONENTS

	NOP				; 
	BX	pc			; CHANGE TO 32 BIT STATE FOR MULTIPLY
	NOP				;

	.state32

	; MULTIPLY THE MANTISAE: op1m * op2m => res1:res2
	UMULL	res2, res1, op1m1, op2m1   ; op1m1*op2m1 =>res1:res2: 0   : 0
	UMULL	tmp, op1m1, op2m2, op1m1   ; op1m1*op2m2 =>  0 :op1m1:tmp : 0
	UMULL	lr, op2m2, op1m2, op2m2    ; op1m2*op2m2 =>  0 :  0 :op2m2: lr
	UMULL	lr, op2m1, op1m2, op2m1    ; op1m2*op2m1 =>  0 :op2m1: lr : 0
	ADDS	op2m2, op2m2, lr	   ;                 0 :  0 :op2m2: 0

	MOV	op1m2, #0x0		   ;
	ADD	lr, pc, #0x1		   ; CHANGE BACK TO 16 BIT STATE
	BX	lr			   ;

	.state16

	ADC	op2m1, op1m2		   ;                 0 :op2m1: 0  : 0
	ADC	res1, op1m2		   ;               res1:  0 : 0   : 0
	ADD	tmp, op2m2		   ;                 0 :  0 : tmp : 0
	ADC	op2m1, op1m2		   ;                 0 :op2m1: 0  : 0
	ADC	res1, op1m2	 	   ;               res1:  0 : 0   : 0

	MOV	tmp, #0x80		   ; SETUP 1/2 CONSTANT FOR ROUNDING
	LSL	tmp, tmp, #2    	   ;

	ADD	op1m1, op2m1		   ;                 0 :op1m1: 0  : 0
	ADC	res1, op1m2		   ;               res1:  0 : 0   : 0
	ADD	res2, op1m1		   ;                 0 :res2: 0   : 0
	ADC	res1, op1m2		   ;               res1:  0 : 0   : 0

	BPL	$1			; IF THE MANTISSA WILL NOT NEED
	LSL	tmp, tmp, #0x1		; ALIGNMENT, ADJUST 1/2 CONSTANT.

$1:	ADD	res2, res2, tmp		; ADD 1/2 FOR ROUNDING
	ADC	res1, op1m2		;

	BMI	$2			; ALIGN THE RESULT
	LSL	res2, res2, #1		;
	ADC	res1, res1		;
	B	$3			;

$2:	ADD	op1e, op1e, #0x1	; 

$3:	MOV	tmp, #0x3		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	SUB     op1e, op1e, tmp         ; ADJUST FOR BIAS
	BLE	unfl			; AND CHECK FOR UNDERFLOW

	ADD	tmp, tmp, tmp		;
        CMP     op1e, tmp	        ; AND CHECK FOR EXPONENT OVERFLOW 
	BHI	ovfl			;

	LSL	op1m1, op1e, #20	; ALIGN EXPONENT INTO op1m1
	LSR	r1, res2, #11		; REPACK LOW PART OF RESULT
	LSL	tmp, res1, #21		;
	ORR	r1, tmp			;
	LSL	r0, res1, #1		; REPACK HIGH PART OF RESULT
	LSR	r0, r0, #12		;
        ORR     r0, op1m1               ; REPACK THE EXPONENT INTO r0
	POP	{tmp}			;
        ORR     r0, tmp                 ; REPACK THE SIGN INTO r0
 
	POP	{r2-r7, pc}	        ;

ovfl:   MOV     r1, #0                  ; IF OVERFLOW, RETURN +/- INFINITY
	MOV	r0, #0xFF
	LSL	r0, r0, #3
	ADD	r0, r0, #7
	LSL	r0, r0, #20
	POP	{tmp}			;
        ORR     r0, tmp                 ;
        POP     {r2-r7, pc}	        ;

unfl:   MOV     r0, #0                  ; UNDERFLOW, SO RETURN ZERO
	MOV	r1, #0			;
	ADD	sp, #4			;
        POP     {r2-r7, pc}             ;
 
	.endasmfunc

        .end

