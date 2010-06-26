;******************************************************************************
;* FD_ADD32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;*****************************************************************************
;* FD_ADD/FD_SUB - ADD / SUBTRACT TWO IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;* 	           POINT NUMBERS.
;*****************************************************************************
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
	.state32
	.global	FD_ADD
	.global FD_SUB

op1m1	.set	r4
op1m2	.set	r5
op1e	.set	r6
op2m1	.set	r7
op2m2	.set	r8
op2e	.set	r9
shift	.set	r10
tmp	.set	lr

FD_SUB: .asmfunc stack_usage(40)
	STMFD	sp!, {r2-r10, lr}	;
	EOR	r2, r2, #0x80000000	; NEGATE SECOND OPERAND
	B	_start			;

FD_ADD:
	STMFD	sp!, {r2-r10, lr}	;
	
_start:	MOV	op2m1, r2, LSL #12		; BUILD INPUT #2 MANTISSA
	MOV	op2m1, op2m1, LSR #3		;
	ORR	op2m1, op2m1, r3, LSR #23	;
	MOV	op2m2, r3, LSL #9		;
	MOV	op2e, r2, LSL #1		; BUILD INPUT #2 EXPONENT
	MOVS	op2e, op2e, LSR #21		;
	BNE	$1				;

        ORR     tmp, op2m1, op2m2	; IF DENORMALIZED NUMBER (op2m != 0 AND
        MOVNE   r0, #0                  ; op2e == 0), THEN UNDERFLOW
	MOVNE	r1, #0			;
        LDMFD   sp!, {r2-r10, pc}       ; ELSE IT IS ZERO SO RETURN INPUT #1

$1:	ORR	op2m1, op2m1, #0x20000000 	; SET IMPLIED ONE IN MANTISSA
	MOV	shift, #0x700		; INITIALIZE shift WITH 0x7FF
	ADD	shift, shift, #0xFF	;
	CMP	op2e, shift		; IF op2e == 0x7FF, THEN OVERFLOW
	BNE	$2			;
	MOV	r1, #0			;
	MOV	r0, r2, LSR #20
	MOV	r0, r0, LSL #20
        LDMFD   sp!, {r2-r10, pc}       ;

$2:     CMP     r2, #0                  ;
	BPL	$3			; IF INPUT #2 IS NEGATIVE,
        RSBS    op2m2, op2m2, #0        ;  THEN NEGATE THE MANTISSA
        RSC     op2m1, op2m1, #0        ;

$3:	MOV	op1m1, r0, LSL #12		; BUILD INPUT #1 MANTISSA
	MOV	op1m1, op1m1, LSR #3		;
	ORR	op1m1, op1m1, r1, LSR #23	;
	MOV	op1m2, r1, LSL #9		;
	MOV	op1e, r0, LSL #1		; BUILD INPUT #1 EXPONENT
	MOVS	op1e, op1e, LSR #21		;
	BNE	$4				;

        ORR     tmp, op1m1, op1m2	; IF DENORMALIZED NUMBER (op1m != 0 AND
        MOVNE   r0, #0                  ; op1e == 0), THEN UNDERFLOW
	MOVNE	r1, #0			;
        MOVEQ   r0, r2                  ; ELSE IT IS ZERO SO RETURN INPUT #2
	MOVEQ	r1, r3			;
        LDMFD   sp!, {r2-r10, pc}       ;

$4:	ORR	op1m1, op1m1, #0x20000000 	; SET IMPLIED ONE IN MANTISSA
	CMP	op1e, shift		; IF op1e == 0x7FF, THEN OVERFLOW
	BNE	$5			;
	MOV	r1, #0			;
	MOV	r0, r0, LSR #20
	MOV	r0, r0, LSL #20
        LDMFD   sp!, {r2-r10, pc}       ;

$5:     CMP     r0, #0                  ;
	BPL	$6			; IF INPUT #1 IS NEGATIVE,
        RSBS    op1m2, op1m2, #0        ;  THEN NEGATE THE MANTISSA
        RSC     op1m1, op1m1, #0        ;

$6:     SUBS    shift, op1e, op2e       ; GET THE SHIFT AMOUNT
	BPL	$7			;
        MOV     tmp, op1m1              ; IF THE SHIFT AMOUNT IS NEGATIVE, THEN
        MOV     op1m1, op2m1            ;  SWAP THE TWO MANTISSA SO THAT op1m
	MOV  	op2m1, tmp		;  CONTAINS THE LARGER VALUE,
	MOV	tmp, op1m2		;
	MOV	op1m2, op2m2		;
	MOV	op2m2, tmp		;
        RSB     shift, shift, #0        ;  AND NEGATE THE SHIFT AMOUNT,
        MOV     op1e, op2e              ;  AND ENSURE THE LARGER EXP. IS IN op1e

$7:     CMP     shift, #54              ; IF THE SECOND MANTISSA IS SIGNIFICANT,
	BPL	no_add			;

	CMP	shift, #0		; ADJUST THE SECOND MANTISSA, BASED
	BEQ	no_sft			; UPON ITS EXPONENT.

sloop:	MOVS	op2m1, op2m1, ASR #1	;
	MOV	op2m2, op2m2, RRX	;
	SUBS	shift, shift, #1	;
	BNE	sloop

no_sft:	ADDS	op1m2, op1m2, op2m2	; ADD IT TO THE FIRST MANTISSA
	ADCS	op1m1, op1m1, op2m1	;

no_add: ORRS    tmp, op1m1, op1m2	;
        MOVEQ   r0, #0                  ; IF THE RESULT IS ZERO, 
	MOVEQ	r1, #0			;
        LDMEQFD sp!, {r2-r10, pc}       ;  THEN UNDERFLOW

	CMP	op1m1, #0		;
        MOVPL   tmp, #0x0               ; IF THE RESULT IS POSITIVE, NOTE SIGN
	BPL	nloop			;
        MOV     tmp, #0x1               ; IF THE RESULT IS NEGATIVE, THEN
        RSBS    op1m2, op1m2, #0x0      ;  NOTE THE SIGN AND
	RSC	op1m1, op1m1, #0x0	;  NEGATE THE RESULT
 
nloop:  MOVS    op1m2, op1m2, LSL #1    ; NORMALIZE THE RESULTING MANTISSA
	ADCS	op1m1, op1m1, op1m1	; 
        SUB     op1e, op1e, #1          ; ADJUSTING THE EXPONENT AS NECESSARY
        BPL     nloop                   ;
 
        ADDS    op1m2, op1m2, #0x400    ; ROUND THE MANTISSA TO THE NEAREST
	ADCS	op1m1, op1m1, #0	;
        ADDCS   op1e, op1e, #1          ; ADJUST EXPONENT IF AN OVERFLOW OCCURS
	BCS	ovfl			;
        MOVS    op1m2, op1m2, LSL #1    ; REMOVE THE IMPLIED ONE
	ADC	op1m1, op1m1, op1m1	;
 
ovfl:   ADDS    op1e, op1e, #2          ; NORMALIZE THE EXPONENT
        MOVLE   r0, #0                  ; CHECK FOR UNDERFLOW
	MOVLE	r1, #0			;
        LDMLEFD sp!, {r2-r10, pc}       ;
	MOV	shift, #0x700		;
	ADD	shift, shift, #0xFF	;
        CMP     op1e, shift             ; CHECK FOR OVERFLOW
	BCC	$9
	MOV	r1, #0			;
	AND	r2, r2, #0x80000000
	MOV	r0, #0xFF
	MOV	r0, r0, LSL #3
	ADD	r0, r0, #7
	MOV	r0, r0, LSL #20
	ORR	r0, r0, r2
        LDMFD   sp!, {r2-r10, pc}       ;
 
$9:     MOV	op2m1, op1m1, LSL #20	; REPACK THE MANTISSA INTO r0:r1
	ORR	r1, op2m1, op1m2, LSR #12 ;
	MOV	r0, op1m1, LSR #12	;
        ORR     r0, r0, op1e, LSL #20   ; REPACK THE EXPONENT INTO r0
        ORR     r0, r0, tmp, LSL #31    ; REPACK THE SIGN INTO r0
 
        LDMFD   sp!, {r2-r10, pc}       ;
 
	.endasmfunc

        .end
