;******************************************************************************
;* LL_TOFS16.ASM  - 16 BIT STATE -  v2.54                                     *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* LL$TOFS - CONVERT A 64 BIT SIGNED INTEGER TO AN IEEE 754 FORMAT 
;*           SINGLE PRECISION FLOATING POINT NUMBER
;****************************************************************************
;*
;*   o INPUT OP IS IN R0:R1 (r1:r0 IF LITTLE ENDIAN)
;*   o RESULT IS RETURNED IN R0
;*   o THE VALUE IN R1 IS DESTROYED
;*
;*   o ROUNDING MODE:  ROUND TO NEAREST
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

	.global LL$TOFS


e0	.set	r2
tmp	.set	r3

LL$TOFS: .asmfunc stack_usage(12)
        PUSH	{r2, r3, lr}		; SAVE CONTEXT

	; IN LITTLE ENDIAN MODE THE INPUT LONG LONG VALUE IS IN R1:R0. SWAP THE
	; WORDS SO THAT WE HAVE THE LONG LONG VAULUE IN R0:R1.

	.if .TMS470_LITTLE
	MOV	tmp, r0			;
	MOV	r0, r1			;
	MOV	r1, tmp			;
	.endif

	MOV	e0, #0xBE		; SET THE EXPONENT FIELD
        MOV     lr, r0                  ; SAVE THE SIGN

	CMP	r0, #0			; IF ZERO, RETURN ZERO
	BMI	$1			;
	BNE	loop			;
        CMP     r1, #0                  ;
	BNE	loop            	;
	POP	{r2, r3, pc}		;

$1:	
        MOV     tmp, #0                 ;
        NEG     r1, r1                  ;
        SBC     tmp, r0                 ;
        MOV     r0, tmp                 ;
	BMI	cont			;

loop:   SUB     e0, #1                  ; NORMALIZE THE MANTISSA
        LSL     r0, r0, #1              ; ADJUSTING THE EXPONENT, ACCORDINGLY
        LSR     tmp, r1, #31            ;
        ORR     r0, tmp                 ;
        LSL     r1, r1, #1              ;
        CMP     r0, #0                  ;
        BPL     loop                    ;

cont:	ADD	r0, #0x80		; ADD 1/2 TO ROUND
	BCC	$2			;
	ADD	e0, #0x1		; AND ADJUST THE EXPONENT ACCORDINGLY
	B	$3			;

$2:	LSL	r0, r0, #1		; MASK IMPLIED 1 OUT OF THE MANTISSA

$3:	LSR	r0, r0, #9		; PACK THE MANTISSA
	LSL	e0, e0, #23		;
	ORR	r0, e0			; PACK THE EXPONENT
	MOV	tmp, lr			; IF THE INPUT WAS NEGATIVE
        CMP     tmp, #0                 ;
	BPL	$4			;
	MOV	e0, #1			;
	LSL	e0, e0, #31		;
	ORR	r0, e0			;  THEN SET THE SIGN FIELD
$4:	
	POP 	{r2, r3, pc}		; RESTORE CONTEXT

	.endasmfunc
	.end
