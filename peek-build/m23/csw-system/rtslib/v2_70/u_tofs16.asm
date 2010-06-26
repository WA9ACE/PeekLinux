;******************************************************************************
;* U_TOFS16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* U$TOFS - CONVERT A 32 BIT UNSIGNED INTEGER TO AN IEEE 754 FORMAT
;*	    SINGLE PRECISION FLOATING POINT NUMBER
;****************************************************************************
;*
;*   o INPUT OP IS IN R0
;*   o RESULT IS RETURNED IN R0
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

	.global U$TOFS

e0	.set	r1

U$TOFS: .asmfunc stack_usage(4)
	PUSH    {r1}		        ; SAVE CONTEXT

	MOV	e0, #0x9E		; SET THE EXPONENT FIELD

	CMP     r0, #0                  ; IF ZERO, RETURN ZERO
	BMI	cont			;
        BNE     loop			;
	POP	{r1}			;
        MOV     pc, lr                  ;
 
loop:   SUB     e0, #1                  ; NORMALIZE THE MANTISSA
        LSL     r0, r0, #1              ; ADJUSTING THE EXPONENT, ACCORDINGLY
        BPL     loop                    ;

cont:	ADD     r0, #0x80               ; ADD 1/2 TO ROUND
        BCC     $1			;
        ADD     e0, #0x1                ; AND ADJUST THE EXPONENT ACCORDINGLY
        B       $2			;
 
$1:	LSL     r0, r0, #1              ; MASK IMPLIED 1 OUT OF THE MANTISSA
 
$2:	LSR     r0, r0, #9              ; PACK THE MANTISSA
        LSL     e0, e0, #23             ;
        ORR     r0, e0                  ; PACK THE EXPONENT

        POP     {r1}	 	        ; RESTORE CONTEXT
	MOV	pc, lr			;

	.endasmfunc
	.end
