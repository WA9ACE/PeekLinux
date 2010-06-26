;******************************************************************************
;* FD_DIV32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD_DIV - DIVIDES TWO IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
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
****************************************************************************
	.state32

	.global FD_DIV

op1m1	.set	r2	; OVERLOADED WITH INPUT #2
op1m2	.set	r3	; OVERLOADED WITH INPUT #2
op1e	.set	r4
op2m1	.set	r5
op2m2	.set	r6
op2e	.set	r7
sign	.set	r8
tmp	.set	lr

FD_DIV:	.asmfunc stack_usage(32)
	STMFD	sp!, {r2-r8, lr}
 
        EORS    sign, r0, r2            ; SET THE SIGN OF THE RESULT
        MOVMI   sign, #0x80000000       ;
        MOVPL   sign, #0x00000000       ;
 	
	MOV	op2m1, r2, LSL #12	; BUILD INPUT #2 MANTISSA
	MOV	op2m1, op2m1, LSR #12	;
	MOVS	op2m2, r3, LSL #1	;
	ADC	op2m1, op2m1, op2m1	;
	MOV	op2e, r2, LSL #1	; BUILD INPUT #2 EXPONENT
	MOVS	op2e, op2e, LSR #21	;
        ORRNE   op2m1, op2m1,#0x00200000; SET IMPLIED 1 IN MANTISSA IF op2e != 0
        MOVEQ   r0, #0                  ; IF op2e == 0, THEN UNDERFLOW
	MOVEQ	r1, #0			;
        LDMEQFD sp!, {r2-r8, pc}        ;

	MOV	tmp, #0x700		;
	ADD	tmp, tmp, #0xFF		;
        CMP     op2e, tmp               ; IF op2e == 0x7FF, THEN OVERFLOW
	BEQ	ovfl			;

	MOV	op1m1, r0, LSL #12	; BUILD INPUT #1 MANTISSA
	MOV	op1m1, op1m1, LSR #12	;
	MOVS	op1m2, r1, LSL #1	;
	ADC	op1m1, op1m1, op1m1	;
	MOV	op1e, r0, LSL #1	; BUILD INPUT #1 EXPONENT
	MOVS	op1e, op1e, LSR #21	;
        ORRNE   op1m1, op1m1,#0x00200000; SET IMPLIED 1 IN MANTISSA IF op1e != 0
        MOVEQ   r0, #0                  ; IF op1e == 0, THEN UNDERFLOW
	MOVEQ	r1, #0			;
        LDMEQFD sp!, {r2-r8, pc}        ;

        CMP     op1e, tmp               ; IF op1e == 0x7FF, THEN OVERFLOW
	BEQ	ovfl			;

        SUB     op1e, op1e, op2e        ; SUBTRACT EXPONENTS
 
        ; DIVIDE THE MANTISAE:  op1m / op2m => r0:r1
        MOV     r0, #0x0                ; INITIALIZE THE QUOTIENT
	MOV	r1, #0x0		;
        MOV     tmp, #54                ; INITIALIZE THE SHIFT COUNTER
 
fdivb:  CMP     op1m1, op2m1            ; IF DIVIDEND IS LARGER THAN DIVISOR,
	BNE	$1			;
	CMP	op1m2, op2m2		;
$1:	BCC	$2			;
        ADCS    r1, r1, r1              ;  SHIFT A 1 INTO THE QUOTIENT
	ADC	r0, r0, r0		;
	SUBS	op1m2, op1m2, op2m2	;  AND SUBTRACT THE DIVISOR FROM
	SBC	op1m1, op1m1, op2m1	;  THE DIVIDEND
	B	$3			;
$2:	ADCS    r1, r1, r1		; ELSE, SHIFT A 0 INTO THE QUOTIENT
	ADC	r0, r0, r0		;
$3:	SUBS	tmp, tmp, #1		; UPDATE THE SHIFT COUNTER
	BEQ	fdive			; EXIT IF OUT OF SHIFTS
	ORRS	op2e, op1m1, op1m2	;
	BEQ	fdive			; IF DIVIDEND STILL THERE,
	MOVS	op1m2, op1m2, LSL #1	;  UPDATE DIVIDEND
	ADC	op1m1, op1m1, op1m1	;  AND
	B	fdivb			;  CONTINUE
 
fdive:  CMP	tmp, #32		; ADJUST THE QUOTIENT AS NECESSARY
	BCS	low			;
	CMP	tmp, #22		;
	BCC	high			;

mid:	RSB	op2e, tmp, #32		;
	MOV	r0, r1, LSR op2e	;
	MOV	r1, r1, LSL tmp		;
	B	cont			;

low:	SUB	tmp, tmp, #32		;
	MOV	r0, r1, LSL tmp		;
	MOV	r1, #0			;
	B	cont			;

high:	MOV	r0, r0, LSL tmp		;
	RSB	op2e, tmp, #32		;
	ORR	r0, r0, r1, LSR op2e	;
	MOV	r1, r1, LSL tmp		;

cont:   TST     r0, #0x00200000         ; ALIGN THE QUOTIENT
	BNE	$4			;
	MOVS	r1, r1, LSL #1		;
	ADC	r0, r0, r0		;
        SUB     op1e, op1e, #0x1        ;
 
$4:     ADDS    r1, r1, #0x00000001     ; 1/2 ADJUST FOR ROUNDING
	ADC	r0, r0, #0		;
	TST	r0, #0x00400000		;
        ADDNE   op1e, op1e, #0x1        ;
	BICNE	r0, r0, #0x00400000	;
	BICEQ	r0, r0, #0x00200000	;
 
	MOV	op2e, #0x300		;
	ADD	op2e, op2e, #0xFF	;
	ADDS    op1e, op1e, op2e        ; ADJUST FOR BIAS
        MOVLE   r0, #0x0                ; AND CHECK FOR UNDERFLOW
	MOVLE	r1, #0x0		;
        LDMLEFD sp!, {r2-r6, pc}        ;
 
	ADD	op2e, op2e, #0x400	; AND CHECK FOR EXPONENT OVERFLOW
        CMP     op1e, op2e	        ;
	BCS	ovfl			;

	MOVS	r0, r0, LSR #1		; REPACK THE QUOTIENT INTO r0:r1
	MOV	r1, r1, RRX		; 
        ORR     r0, r0, op1e, LSL #20   ; REPACK THE EXPONENT INTO r0
        ORR     r0, r0, sign            ; REPACK THE SIGN INTO r0
 
        LDMFD   sp!, {r2-r8, pc}        ;
 
ovfl:	MOV	r1, #0x0		; IF OVERFLOW, RETURN +/- INFINITY
	MOV	r0, #0xFF
	MOV	r0, r0, LSL #3
	ADD	r0, r0, #7
	MOV	r0, r0, LSL #20
	ORR	r0, r0, sign
        LDMFD   sp!, {r2-r8, pc}        ;
 
	.endasmfunc

        .end
