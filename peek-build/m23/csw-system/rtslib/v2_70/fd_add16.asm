;******************************************************************************
;* FS_ADD16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;******************************************************************************
;* FD_ADD/FD_SUB - ADD / SUBTRACT TWO IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;*   	           POINT NUMBERS.
;******************************************************************************
;*
;*   o INPUT OP1 IS IN r0:r1
;*   o INPUT OP2 IS IN r2:r3
;*   o RESULT IS RETURNED IN r0:r1
;*   o INPUT OP2 IN r2:r3 IS PRESERVED
;*
;*   o SUBTRACTION, OP1 - OP2, IS IMPLEMENTED WITH ADDITION, OP1 + (-OP2)
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY 
;*       (0x7ff00000:00000000) or (0xfff00000:00000000) 
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*
;*   o IF OPERATION INVOLVES INFINITY AS AN INPUT, THE FOLLOWING SUMMARIZES
;*     THE RESULT:
;*                   +----------+----------+----------+ 
;*         ADDITION  + OP2 !INF | OP2 -INF + OP2 +INF +
;*        +----------+==========+==========+==========+ 
;*        + OP1 !INF +    -     |   -INF   +   +INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 -INF +   -INF   |   -INF   +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 +INF +   +INF   |   +INF   +   +INF   +
;*        +----------+----------+----------+----------+
;*
;*                   +----------+----------+----------+ 
;*       SUBTRACTION + OP2 !INF | OP2 -INF + OP2 +INF +
;*        +----------+==========+==========+==========+ 
;*        + OP1 !INF +    -     |   +INF   +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 -INF +   -INF   |   -INF   +   -INF   +
;*        +----------+----------+----------+----------+ 
;*        + OP1 +INF +   +INF   |   +INF   +   +INF   +
;*        +----------+----------+----------+----------+
;*
;****************************************************************************
*
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

	.global	FD$ADD
	.global	FD$SUB

shift	.set	r0	; OVERLOADED WITH INPUT #1
op1e	.set	r1	; OVERLOADED WITH INPUT #1
op1m1	.set	r2	; OVERLOADED WITH INPUT #2
op1m2	.set	r3	; OVERLOADED WITH INPUT #2
tmp	.set	r4
op2m1	.set	r5
op2m2	.set	r6
op2e	.set	r7


FD$SUB:	.asmfunc stack_usage(24)

	PUSH	{r2-r7}			;
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #31		;
	EOR	r2, tmp			; NEGATE INPUT #2
	B	ct			;

FD$ADD:	PUSH	{r2-r7}			;
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #31		;

ct:	PUSH	{r2}			; SAVE MODIFIED INPUT #2

	LSL	op2m1, r2, #12		; BUILD INPUT #2 MANTISSA
	LSR	op2m1, op2m1, #3	;
	LSR	op2m2, r3, #23		;
	ORR	op2m1, op2m2		;
	LSL	op2m2, r3, #9		;
	LSL	op2e, r2, #1		; BUILD INPUT #2 EXPONENT
	LSR	op2e, op2e, #21		;
	BNE	$1			;

	MOV	tmp, op2m1		; IF DENORMALIZED NUMBER (op2m != 0 AND
        ORR     tmp, op2m2		; op2e == 0), THEN UNDERFLOW
	BNE	unfl			;
	ADD	sp, #4			;
        POP	{r2-r7}		        ; ELSE IT IS ZERO SO RETURN INPUT #1
	MOV	pc, lr			;

$1:	LSR	tmp, tmp, #2		; SET IMPLIED ONE IN MANTISSA
	ORR	op2m1, tmp	 	;

	MOV	tmp, #0x7		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	CMP	op2e, tmp		; IF op2e == 0x7FF, THEN OVERFLOW
	BNE	$2			;

	MOV	r1, #0x0		;
	LSR	r0, r2, #20		; PICK UP SIGN AND EXP FROM R2
	LSL	r0, r0, #20
	ADD	sp, #4			;
        POP     {r2-r7}		        ;
	MOV	pc, lr			;

$2:     CMP     r2, #0                  ;
	BPL	$3			; IF INPUT #2 IS NEGATIVE,
	MOV	tmp, #0			;  THEN NEGATE THE MANTISSA
        SUB     op2m2, tmp, op2m2	;
        SBC     tmp, op2m1	        ;
	MOV	op2m1, tmp		;

$3:	LSL	op1m1, r0, #12		; BUILD INPUT #1 MANTISSA
	LSR	op1m1, op1m1, #3	;
	LSR	op1m2, r1, #23		;
	ORR	op1m1, op1m2		;
	LSL	op1m2, r1, #9		;
	LSL	op1e, r0, #1		; BUILD INPUT #1 EXPONENT
	LSR	op1e, op1e, #21		;
	BNE	$4			;

	MOV	tmp, op1m1		; IF DENORMALIZED NUMBER (op1m != 0 AND
        ORR     tmp, op1m2		; op1e == 0), THEN UNDERFLOW
	BNE	unfl			;
	POP	{r0, r2, r3}		; ELSE IT IS ZERO SO RETURN INPUT #2
	MOV 	r1, r3			;
        POP     {r4-r7}  	        ;
	MOV	pc, lr			;

$4:	MOV	tmp, #0x1		; SET IMPLIED ONE IN MANTISSA
	LSL	tmp, tmp, #29		;
	ORR	op1m1, tmp	 	;

	MOV	tmp, #0x7		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	CMP	op1e, tmp		; IF op1e == 0x7FF, THEN OVERFLOW
	BNE	$5

	MOV	r1, #0x0		;
	LSR	r0, r0, #20		; ISOLATE SIGN AND EXP IN R0
	LSL	r0, r0, #20
	ADD	sp, #4			;
        POP	{r2-r7}		        ;
	MOV	pc, lr			;

$5:     CMP     r0, #0                  ;
	BPL	$6			; IF INPUT #1 IS NEGATIVE,
	MOV	tmp, #0			;  THEN NEGATE THE MANTISSA
	SUB	op1m2, tmp, op1m2	;
        SBC     tmp, op1m1	        ;
	MOV	op1m1, tmp		;

$6:     SUB     shift, op1e, op2e       ; GET THE SHIFT AMOUNT
	BPL	norm			;
        MOV     tmp, op1m1              ; IF THE SHIFT AMOUNT IS NEGATIVE, THEN
        MOV     op1m1, op2m1            ;  SWAP THE TWO MANTISSA SO THAT op1m
	MOV  	op2m1, tmp		;  CONTAINS THE LARGER VALUE,
	MOV	tmp, op1m2		;
	MOV	op1m2, op2m2		;
	MOV	op2m2, tmp		;
	MOV	tmp, #0			;
        SUB     shift, tmp, shift       ;  AND NEGATE THE SHIFT AMOUNT,
        MOV     op1e, op2e              ;  AND ENSURE THE LARGER EXP. IS IN op1e

norm:   CMP     shift, #54              ; IF THE SECOND MANTISSA IS SIGNIFICANT,
	BPL	no_add			;

	CMP	shift, #0		; ADJUST THE SECOND MANTISSA, BASED
	BEQ	no_sft			; UPON ITS EXPONENT.

sloop:	LSL	tmp, op2m1, #31		; NORMALIZE THE MANTISSA
	ASR	op2m1, op2m1, #1	;
	LSR	op2m2, op2m2, #1	;
	ORR	op2m2, tmp		;
	SUB	shift, shift, #1	;
	BNE	sloop			;

no_sft:	ADD	op1m2, op1m2, op2m2	; ADD IT TO THE FIRST MANTISSA
	ADC	op1m1, op2m1		;

no_add: MOV	tmp, op1m1		;
	ORR     tmp, op1m2		;
	BEQ	unfl			;

        MOV	tmp, #0x0               ;
	CMP	op1m1, #0		; IF THE RESULT IS POSITIVE, NOTE SIGN
	BPL	nloop			;
        MOV     tmp, #0x1               ; IF THE RESULT IS NEGATIVE, THEN
	MOV	shift, #0		;  NOTE THE SIGN AND
        SUB	op1m2, shift, op1m2     ;  NEGATE THE RESULT
	SBC	shift, op1m1		;
	MOV	op1m1, shift		;
 
nloop:  SUB     op1e, op1e, #1          ; NORMALIZE THE RESULTING MANTISSA
	LSL     op1m2, op1m2, #1 	; ADJUSTING THE EXPONENT AS NECESSARY
	ADC	op1m1, op1m1		; 
        BPL     nloop                   ;
 
	MOV	op2e, #0x0		;
	MOV	shift, #0x1		;
	LSL	shift, shift, #10	;
        ADD     op1m2, op1m2, shift     ; ROUND THE MANTISSA TO THE NEAREST
	ADC	op1m1, op2e		;
	BCC	$7			;
        ADD     op1e, op1e, #1          ; ADJUST EXPONENT IF AN OVERFLOW OCCURS
	B	$8			;
$7:     LSL     op1m2, op1m2, #1 	; REMOVE THE IMPLIED ONE
	ADC	op1m1, op1m1		;
 
$8:     ADD     op1e, op1e, #2          ; NORMALIZE THE EXPONENT
	BLE	unfl			; CHECK FOR UNDERFLOW
	MOV	shift, #0x7		;
	LSL	shift, shift, #8	;
	ADD	shift, #0xFF		;
        CMP     op1e, shift             ; CHECK FOR OVERFLOW
	BCC	$9			;

	POP	{r0, r2, r3}		; OVERFLOW
	MOV	r1, #0x0		;
	LSR	r0, r0, #31
	LSL	r0, r0, #8
	ADD	r0, #0xFF
	LSL	r0, r0, #3
	ADD	r0, r0, #7
	LSL	r0, r0, #20
        POP     {r4-r7}	                ;
	MOV	pc, lr			;
 
$9:	LSR	r0, op1m1, #12		; REPACK THE MANTISSA INTO r0:r1
	LSL	op1e, op1e, #20		;
        ORR     r0, op1e		; REPACK THE EXPONENT INTO r0
	LSL	tmp, tmp, #31		;
        ORR     r0, tmp 		; REPACK THE SIGN INTO r0
        LSL	r1, op1m1, #20		;
	LSR	op1m2, op1m2, #12	;
	ORR	r1, op1m2		;

	ADD	sp, #4			;
        POP	{r2-r7}	        	;
	MOV	pc, lr			;

unfl:   MOV     r0, #0                  ; UNDERFLOW, SO RETURN ZERO
	MOV	r1, #0			;
	ADD	sp, #4			;
        POP     {r2-r7}			;
	MOV	pc, lr			;

	.endasmfunc

        .end
