;******************************************************************************
;* FD_MUL32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD_MUL - MULTIPLY TWO IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;* 	    POINT NUMBERS.
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
****************************************************************************
	.state32

	.global	FD_MUL

op1m1	.set	r2	; OVERLOADED WITH INPUT #2
op1m2	.set	r3	; OVERLOADED WITH INPUT #2
op1e	.set	r4
op2m1	.set	r5
op2m2	.set	r6
op2e	.set	r7
sign	.set	r8
tmp	.set	lr

FD_MUL: .asmfunc stack_usage(32)
	STMFD	sp!, {r2-r8, lr}
 
        EORS    sign, r0, r2            ; SET THE SIGN OF THE RESULT
        MOVMI   sign, #0x80000000       ;
        MOVPL   sign, #0x00000000       ;
 	
	MOV	op2m1, r2, LSL #11		; BUILD INPUT #2 MANTISSA
	ORR	op2m1, op2m1, r3, LSR #21	;
	MOV	op2m2, r3, LSL #11		;
	MOV	op2e, r2, LSL #1		; BUILD INPUT #2 EXPONENT
	MOVS	op2e, op2e, LSR #21		;
        ORRNE   op2m1, op2m1,#0x80000000	; SET IMPLIED 1 IN MANTISSA
        MOVEQ   r0, #0                  	; IF op2e == 0, ELSE UNDERFLOW
	MOVEQ	r1, #0				;
        LDMEQFD sp!, {r2-r8, pc}        	;

	MOV	tmp, #0x700		;
	ADD	tmp, tmp, #0xFF		;
        CMP     op2e, tmp               ; IF op2e == 0x7FF, THEN OVERFLOW
	BEQ	ovfl			;

	MOV	op1m1, r0, LSL #11		; BUILD INPUT #1 MANTISSA
	ORR	op1m1, op1m1, r1, LSR #21	;
	MOV	op1m2, r1, LSL #11		;
	MOV	op1e, r0, LSL #1		; BUILD INPUT #1 EXPONENT
	MOVS	op1e, op1e, LSR #21		;
        ORRNE   op1m1, op1m1,#0x80000000	; SET IMPLIED 1 IN MANTISSA
        MOVEQ   r0, #0                  	; IF op1e == 0, ELSE UNDERFLOW
	MOVEQ	r1, #0				;
        LDMEQFD sp!, {r2-r8, pc}        	;

        CMP     op1e, tmp               ; IF op1e == 0x7FF, THEN OVERFLOW
	BEQ	ovfl			;

        ADD     op1e, op1e, op2e        ; ADD EXPONENTS

	; MULTIPLY THE MANTISAE: op1m * op2m => r0:r1
	UMULL	r1, r0, op1m1, op2m1       ; op1m1*op2m1 => r0 : r1 : 0   : 0
	UMULL	tmp, op2e, op1m1, op2m2	   ; op1m1*op2m2 =>  0 :op2e:tmp  : 0
	UMULL	op2m2, op1m1, op1m2, op2m2 ; op1m2*op2m2 =>  0 :  0 :op1m1:op2m2
	ADDS	tmp, tmp, op1m1		   ;                 0 :  0 :tmp  :0
	ADCS	op2e, op2e, #0		   ;                 0 :op2e: 0   : 0
	ADC	r0, r0, #0		   ;                r0 :  0 : 0   : 0
	UMULL	op2m2, op1m1, op1m2, op2m1 ; op1m2*op2m1 =>  0 :op1m1:op2m2: 0
	ADDS	tmp, tmp, op2m2		   ;                 0 :  0 : tmp : 0
	ADCS	op2e, op2e, #0		   ;                 0 :op2e: 0   : 0
	ADC	r0, r0, #0	 	   ;                r0 :  0 : 0   : 0
	ADDS	op2e, op2e, op1m1	   ;                 0 :op2e: 0   : 0
	ADC	r0, r0, #0		   ;                r0 :  0 : 0   : 0
	ADDS	r1, r1, op2e		   ;                 0 : r1 : 0   : 0
	ADCS	r0, r0, #0		   ;                r0 :  0 : 0   : 0

	MOVMI	tmp, #0x00000400	; SETUP 1/2 CONSTANT FOR ROUNDING
	MOVPL	tmp, #0x00000200	;

        ADDS    r1, r1, tmp             ; ROUND TO NEAREST
	ADCS	r0, r0, #0		;

	ADDMI	op1e, op1e, #0x1	; ALIGN THE RESULT
	BMI	$1			;
	MOVS	r1, r1, LSL #1		;
	ADC	r0, r0, r0		;

$1:	MOV	op2e, #0x300		;
	ADD	op2e, op2e, #0xFF	;
	SUBS    op1e, op1e, op2e        ; ADJUST FOR BIAS
        MOVLE   r0, #0x0                ; AND CHECK FOR UNDERFLOW
	MOVLE	r1, #0x0		;
        LDMLEFD sp!, {r2-r8, pc}        ;
 
	ADD	op2e, op2e, #0x400	; AND CHECK FOR EXPONENT OVERFLOW
        CMP     op1e, op2e	        ;
	BCS	ovfl			;

	MOV	r1, r1, LSR #11		; REPACK LOW PART OF RESULT INTO r1
	ORR	r1, r1, r0, LSL #21	;
	MOV	r0, r0, LSL #1		; REPACK HIGH PART OF RESULT INTO r0
	MOV	r0, r0, LSR #12		;
        ORR     r0, r0, op1e, LSL #20   ; REPACK THE EXPONENT INTO r0
        ORR     r0, r0, sign            ; REPACK THE SIGN INTO r0
 
        LDMFD   sp!, {r2-r8, pc}        ;
 

ovfl:	MOV	r1, #0			; IF OVERFLOW, RETURN +/- INFINITY
	MOV	r0, #0xFF
	MOV	r0, r0, LSL #3
	ADD	r0, r0, #7
	MOV	r0, r0, LSL #20
	ORR	r0, r0, sign
        LDMFD   sp!, {r2-r8, pc}        ;
 
	.endasmfunc

        .end
