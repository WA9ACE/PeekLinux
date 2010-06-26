;******************************************************************************
;* FS_MUL16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS$MUL - MULTIPLY TWO IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	    POINT NUMBERS.
;****************************************************************************
;*
;*   o INPUT OP1 IS IN r0
;*   o INPUT OP2 IS IN r1
;*   o RESULT IS RETURNED IN r0
;*   o INPUT OP2 IN r1 IS NOT DESTROYED
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY (0x7f800000/ff800000)
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*
;*   o IF THE OPERATION INVOLVES INFINITY AS AN INPUT, UNLESS THE OTHER INPUT
;*     IS ZERO, THE RESULT IS INFINITY WITH THE SIGN DETERMINED IN THE USUAL
;*     FASHION.
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

	.global FS$MUL


in1	.set	r0
in2	.set	r1

m1	.set	r2
e1	.set	r3
m2	.set	r4
e2	.set	r5
sign	.set	r6
tmp	.set	r7

FS$MUL:	.asmfunc stack_usage(24)
	PUSH	{r2-r7}			;

        MOV     tmp, #1                 ; SETUP 0x080000000 CONSTANT
        LSL     tmp, tmp, #31           ;
 
        MOV     sign, in1               ;
        EOR     sign, in2               ; SET THE SIGN OF THE RESULT
        BMI     $1			;
        MOV     sign, #0                ;
        B       $2			;
$1:	MOV     sign, tmp               ;
 
$2:	LSL     m1, in1, #8             ; PUT INPUT #1 MANTISSA IN m1
        LSL     e1, in1, #1             ; PUT INPUT #1 EXPONENT IN e1
        LSR     e1, e1, #24             ;
        BEQ     unfl			; IF e1 == 0, THEN UNDERFLOW
        ORR     m1, tmp                 ; SET IMPLIED ONE IN MANTISSA IF e1 != 0 
        CMP     e1, #0xFF               ; IF e1 == 0xFF, THEN OVERFLOW
        BEQ     ovfl			;

        LSL     m2, in2, #8             ; PUT INPUT #2 MANTISSA IN m2
        LSL     e2, in2, #1             ; PUT INPUT #2 EXPONENT IN e2
        LSR     e2, e2, #24             ;
        BEQ     unfl			; IF e2 == 0, THEN UNDERFLOW
        ORR     m2, tmp                 ; SET IMPLIED ONE IN MANTISSA IF e2 != 0 
        CMP     e2, #0xFF               ; IF e2 == 0xFF, THEN OVERFLOW
        BEQ     ovfl			;

	ADD	e1, e2			; ADD EXPONENTS

	BX	pc			; SWITCH TO 32 BIT STATE FOR THE
	NOP				; LONG MULTIPLY

	.state32
	UMULL	e2, m2, m1, m2		; MULTIPLY THE MANTISAE INTO m2:e2

	ADD	e2, pc, #1		; SWITCH BACK TO 16 BIT STATE
	BX	e2			;

	.state16
	CMP	m2, #0			;
	BMI	$3			;
	LSL	m2, m2, #1		; ADJUST THE EXPONENT AS NECESSARY
	SUB	e1, #0x1		; AND ADJUST FOR BIAS

$3:	ADD	m2, #0x80		; ADD 1/2 FOR ROUNDING
	BCC	$4			;
	ADD	e1, #0x1		; AND ADJUST FOR OVERFLOW
	LSR	m2, m2, #1		;

$4:	SUB	e1, #0x7E		; ADJUST THE BIAS
	BLE	unfl			; AND CHECK FOR UNDERFLOW

	CMP	e1, #0xFF		; CHECK FOR EXPONENT OVERFLOW
	BCS	ovfl			;

	BIC	m2, tmp			; REMOVE IMPLIED 1 IN MANTISSA
	LSR	in1, m2, #8		; SETUP THE MANTISSA
        LSL     e1, e1, #23             ;
	ORR	in1, e1			; REPACK THE EXPONENT INTO in1
	ORR	in1, sign		; REPACK THE SIGN INTO in1
		
        POP     {r2-r7}		     	;
	MOV	pc, lr			;

unfl:	MOV     in1, #0                 ; UNDERFLOW
        POP     {r2-r7}		     	;
	MOV	pc, lr			;
 
ovfl:	MOV	in1, #0xFF		; OVERFLOW
	LSL	in1, in1, #23		;
        ORR     in1, sign               ;
        POP     {r2-r7}		     	;
	MOV	pc, lr			;

	.endasmfunc

	.end

