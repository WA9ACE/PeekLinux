;******************************************************************************
;* FD_DIV16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD$DIV - DIVIDES TWO IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;* 	    POINT NUMBERS.
;****************************************************************************
;*
;*   o DIVIDEND INPUT OP1 IS IN r0:r1
;*   o DIVISOR INPUT OP2 IS IN r2:r3
;*   o QUOTIENT IS RETURNED IN r0:r1
;*   o INPUT OP2 IN r2:r3 IS PRESERVED
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY 
;*       (0x7ff00000:00000000) or (0xfff00000:00000000) 
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*   o DIVIDE BY ZERO RETURNS ZERO
;*
;*   o IF THE OPERATION INVOLVES INFINITY AS AN INPUT, UNLESS THE OTHER INPUT
;*     IS ZERO, THE RESULT IS INFINITY WITH THE SIGN DETERMINED IN THE USUAL
;*     FASHION.
;*
;****************************************************************************
;*
;* +--------------------------------------------------------------+
;* | DOUBLE PRECISION FLOATING POINT FORMAT                       |
;* |   64-bit representation                                      |
;* |   31 30      20 19                  0                        |
;* |   +-+----------+---------------------+                       |
;* |   |S|     E    |        M1           |                       |
;* |   +-+----------+---------------------+                       |
;* |                                                              |
;* |   31                                0                        |
;* |   +----------------------------------+                       |
;* |   |             M2                   |                       |
;* |   +----------------------------------+                       |
;* |                                                              |
;* |   <S>  SIGN FIELD    :          0 - POSITIVE VALUE           |
;* |                                 1 - NEGATIVE VALUE           |
;* |                                                              |
;* |   <E>  EXPONENT FIELD: 0000000000 - ZERO IFF M == 0          |
;* |            0000000001..1111111110 - EXPONENT VALUE(1023 BIAS)|
;* |                        1111111111 - INFINITY                 |
;* |                                                              |
;* |   <M1:M2>  MANTISSA FIELDS:                                  |
;* |                          FRACTIONAL MAGNITUDE WITH IMPLIED 1 |
;* +--------------------------------------------------------------+
;*
;****************************************************************************
	.state16

	.global	FD$DIV

res1	.set	r0	; OVERLOADED WITH INPUT #1
op1e	.set	r1	; OVERLOADED WITH INPUT #1
op1m1	.set	r2	; OVERLOADED WITH INPUT #2
op1m2	.set	r3	; OVERLOADED WITH INPUT #2
tmp	.set	r4
op2m1	.set	r5
op2m2	.set	r6
op2e	.set	r7
res2	.set	r7	; OVERLOADED WITH op2e


FD$DIV:	.asmfunc	stack_usage(28)
	PUSH	{r2-r7}			;

	MOV	tmp, r0			; SET THE SIGN OF THE RESULT
        EOR     tmp, r2                 ;
	LSR	tmp, tmp, #31		;
	LSL	tmp, tmp, #31		;
	PUSH	{tmp}			;

	LSL	op2m1, r2, #12		; BUILD INPUT #2 MANTISSA
	LSR	op2m1, op2m1, #12	;
	LSL	op2m2, r3, #1		;
	ADC	op2m1, op2m1		;
	LSL	op2e, r2, #1		; BUILD INPUT #2 EXPONENT
	LSR	op2e, op2e, #21		;
	BEQ	unfl			;

	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #21		; SET IMPLIED ONE IN MANTISSA
	ORR	op2m1, tmp	 	;

	MOV	tmp, #0x7		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	CMP	op2e, tmp		; IF op2e == 0x7FF, THEN OVERFLOW
	BEQ	ovfl			;

	LSL	op1m1, r0, #12		; BUILD INPUT #1 MANTISSA
	LSR	op1m1, op1m1, #12	;
	LSL	op1m2, r1, #1		;
	ADC	op1m1, op1m1		;
	LSL	op1e, r0, #1		; BUILD INPUT #1 EXPONENT
	LSR	op1e, op1e, #21		;
	BEQ	unfl			;

	MOV	tmp, #0x1		; SET IMPLIED ONE IN MANTISSA
	LSL	tmp, tmp, #21		;
	ORR	op1m1, tmp	 	;

	MOV	tmp, #0x7		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	CMP	op1e, tmp		; IF op1e == 0x7FF, THEN OVERFLOW
	BEQ	ovfl			;

        SUB     op1e, op1e, op2e        ; SUBTRACT EXPONENTS
 
        ; DIVIDE THE MANTISAE:  op1m / op2m => res1:res2
        MOV     res1, #0x0              ; INITIALIZE THE QUOTIENT
	MOV	res2, #0x0		;
        MOV     tmp, #54                ; INITIALIZE THE SHIFT COUNTER
 
fdivb:  CMP     op1m1, op2m1            ; IF DIVIDEND IS LARGER THAN DIVISOR,
	BNE	$1			;
	CMP	op1m2, op2m2		;
$1:	BCC	$2			;
        ADC     res2, res2              ;  SHIFT A 1 INTO THE QUOTIENT
	ADC	res1, res1		;
	SUB	op1m2, op1m2, op2m2	;  AND SUBTRACT THE DIVISOR FROM
	SBC	op1m1, op2m1		;  THE DIVIDEND
	B	$3			;
$2:	ADC     res2, res2		; ELSE, SHIFT A 0 INTO THE QUOTIENT
	ADC	res1, res1		;
$3:	SUB	tmp, tmp, #1		; UPDATE THE SHIFT COUNTER
	BEQ	fdive			; EXIT IF OUT OF SHIFTS
	
	CMP	op1m1, #0		; EXIT IF NO DIVIDEND LEFT
	BNE	$4			;
	CMP	op1m2, #0		;
	BEQ	fdive			;
$4:	LSL	op1m2, op1m2, #1	; IF DIVIDEND STILL THERE,
	ADC	op1m1, op1m1		;   UPDATE DIVIDEND AND
	B	fdivb			;   CONTINUE
 
fdive:  CMP	tmp, #32		; ADJUST THE QUOTIENT AS NECESSARY
	BCS	low			;
	CMP	tmp, #22		;
	BCC	high			;

mid:	MOV	res1, res2		;
	LSL	res2, tmp		;
	MOV	op2m2, #32		;
	SUB	op2m2, op2m2, tmp	;
	LSR	res1, op2m2		;
	B	cont			;

low:	SUB	tmp, #32		;
	MOV	res1, res2		;
	LSL	res1, tmp		;
	MOV	res2, #0		;
	B	cont			;

high:	LSL	res1, tmp		;
	MOV	op2m2, #32		;
	SUB	op2m2, op2m2, tmp	;
	MOV	op2m1, res2		;
	LSR	op2m1, op2m2		;
	ORR	res1, op2m1		;
	LSL	res2, tmp		;

cont:   MOV	tmp, #0x1		;
	LSL	tmp, tmp, #21		;
	TST     res1, tmp               ; ALIGN THE QUOTIENT
	BNE	$5			;
	LSL	res2, res2, #1		;
	ADC	res1, res1		;
        SUB     op1e, op1e, #0x1        ;
 
$5:     MOV	tmp, #0x0		;
	ADD     res2, res2, #0x00000001 ; 1/2 ADJUST FOR ROUNDING
	ADC	res1, tmp		;
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #22		;
	TST	res1, tmp		;
	BEQ	$6			;
        ADD     op1e, op1e, #0x1        ;
	BIC	res1, tmp		;
	B	$7			;
$6:	LSR	tmp, tmp, #1		;
	BIC	res1, tmp		;

$7:	MOV	tmp, #0x3		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	ADD     op1e, op1e, tmp         ; ADJUST FOR BIAS
	BLE	unfl			; AND CHECK FOR UNDERFLOW

	ADD	tmp, tmp, tmp		;
        CMP     op1e, tmp	        ; AND CHECK FOR EXPONENT OVERFLOW 
	BHI	ovfl			;

	LSL	tmp, res1, #31		;
	LSR	res1, res1, #1		; REPACK THE QUOTIENT INTO r0:r1
	LSL	op1e, op1e, #20		;
        ORR     res1, op1e              ; REPACK THE EXPONENT INTO r0
	LSR	r1, res2, #1		;
	ORR	r1, tmp			;
	POP	{tmp}			;
        ORR     res1, tmp               ; REPACK THE SIGN INTO r0
 
	POP	{r2-r7}		        ;
	MOV	pc, lr			;

ovfl:	POP	{r0}			; RETURN OVERFLOW VALUE, WHICH IS
	MOV	r1, #0x0		; BASED ON THE SIGN OF THE RESULT
	LSR	r0, r0, #31
	LSL	r0, r0, #8
	MOV	r0, #0xFF
	LSL	r0, r0, #3
	ADD	r0, r0, #7
	LSL	r0, r0, #20
        POP     {r2-r7}		        ;
	MOV	pc, lr			;

unfl:   MOV     r0, #0                  ; UNDERFLOW, SO RETURN ZERO
	MOV	r1, #0			;
	ADD	sp, #4			;
        POP     {r2-r7}			;
	MOV	pc, lr			;

	.endasmfunc

        .end
