;******************************************************************************
;* FS_TOU16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS$TOU - CONVERT AN IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	    POINT NUMBER TO A 32 BIT UNSIGNED INTEGER
;****************************************************************************
;*
;*   o INPUT OP IS IN r0
;*   o RESULT IS RETURNED IN r0
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS 0xFFFFFFFF
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
;*   o NEGATIVE VALUE RETURNS ZERO
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

	.global FS$TOU, FS$TOI

e0      .set    r1
tmp     .set    r2

FS$TOU: .asmfunc stack_usage(8)
	PUSH	{r1, r2, lr}		; SAVE CONTEXT

	CMP	r0, #0			; CHECK FOR A NEGATIVE VALUE
        BPL     pos
        BL      FS$TOI                  ; CAST TO INT
        POP     {r1, r2, pc}

pos:    LSL     e0, r0, #1              ; PUT EXPONENT IN e0
        LSR     e0, e0, #24             ;
 
        SUB     e0, #0x7F               ; CHECK FOR UNDERFLOW
        BCC     unfl 	                ; IF UNDERFLOW, RETURN ZERO
 
        MOV     tmp, #0x1F              ;
        SUB     e0, tmp, e0             ;
        BCC     ovfl    	        ; CHECK FOR OVERFLOW
 
	LSL     r0, r0, #8              ; PUT MANTISSA IN r0
        MOV     tmp, #1                 ;
        LSL     tmp, tmp, #31           ;
        ORR     r0, tmp                 ; SET IMPLIED ONE IN MANTISSA
 
	LSR	r0, e0			; COMPUTE THE INTEGER VALUE
	POP	{r1, r2, pc}		;

unfl:   MOV     r0, #0                  ; UNDERFLOW
        POP     {r1, r2, pc}		;
 
ovfl:	MOV	r0, #0x0		; IF OVERFLOW, RETURN INFINITY
	SUB	r0, r0, #0x1		;
        POP     {r1, r2, pc}		;

	.endasmfunc

	.end
