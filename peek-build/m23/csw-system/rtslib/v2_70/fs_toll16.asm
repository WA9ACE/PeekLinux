;******************************************************************************
;* FS_TOLL.ASM  - 16 BIT STATE -  v2.54                                       *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS$TOLL - CONVERT AN IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	     POINT NUMBER TO A 64 BIT SIGNED INTEGER
;****************************************************************************
;*
;*   o INPUT OP IS IN r0
;*   o RESULT IS RETURNED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS 0x7FFFFFFF:FFFFFFFF/80000000:00000000, DEPENDING ON 
;*     THE SIGN OF THE INPUT
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o ROUNDING MODE:  ROUND TO ZERO
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

	.global FS$TOLL

	.if .TMS470_LITTLE

o_hi	.set r1
o_lo	.set r0

	.else

o_hi	.set r0
o_lo	.set r1

	.endif

e0	.set	r2
tmp	.set	r3
sign	.set	lr

FS$TOLL: .asmfunc stack_usage(12)
        PUSH	{r2, r3, lr}		;

	LSL	e0, r0, #1		; PUT EXPONENT IN e0
	LSR	e0, e0, #24		;

	SUB	e0, #0x7F		; CHECK FOR UNDERFLOW
	BMI	unfl			; IF UNDERFLOW, RETURN ZERO

	MOV	tmp, #0x3F		;
	SUB	e0, tmp, e0		;
	BLS	ovfl			; CHECK FOR OVERFLOW

	MOV	sign, r0		; SAVE SIGN
	LSL	r0, r0, #8		; PUT MANTISSA IN r0
	MOV	tmp, #1			;
	LSL	tmp, tmp, #31		;
	ORR	r0, tmp			; SET IMPLIED ONE IN MANTISSA

        MOV     tmp, e0                 ; COMPUTE THE INTEGER VALUE BY RIGHT 
        SUB     tmp, #32                ; SHIFTING THE MANTISSA BY THE e0
        BCC     $1                      ; 
        MOV     r1, r0                  ; IF e0 >= 32
        MOV     r0, #0                  ;    r0:r1 = 0:((r0 >> (e0 - 32))
	LSR	r1, tmp		        ; 
	B       adj_sign                ;
$1:
        MOV     r1, r0                  ; IF e0 < 32
        LSR     r0, e0                  ;    r0:r1 = (r0:0) >> e0
        NEG     tmp, tmp                ;
        LSL     r1, tmp                 ;

adj_sign:
	MOV	tmp, sign		;
	CMP	tmp, #0			;
	BPL	$2			; IF THE INPUT IS NEGATIVE,
        MOV     tmp, #0                 ;
	NEG	r1, r1			;  THEN NEGATE THE RESULT AND RETURN
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

	POP 	{r2, r3, pc}		;

unfl:	MOV	r0, #0			; UNDERFLOW
        MOV     r1, #0                  ;
	POP 	{r2, r3, pc}		;

ovfl:   MOV	tmp, r0			; OVERFLOW
	MOV	o_lo, #0		;
	MOV	o_hi, #0x1		;
	LSL	o_hi, o_hi, #31		;
	CMP	tmp, #0			; IF INPUT IS NEGATIVE, RETURN
	BMI	$3			; 0x80000000:00000000
        MOV     tmp, #0                 ;
	SUB	o_lo, o_lo, #1          ; ELSE RETURN 0x7FFFFFFF:FFFFFFFF
        SBC     o_hi, tmp               ;
$3:	POP 	{r2, r3, pc}		;

	.endasmfunc
	.end
