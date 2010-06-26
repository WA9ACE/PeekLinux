;******************************************************************************
;* FS_TOULL16.ASM  - 16 BIT STATE -  v2.54                                    *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS$TOULL - CONVERT AN IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	      POINT NUMBER TO A 64 BIT UNSIGNED INTEGER
;****************************************************************************
;*
;*   o INPUT OP IS IN r0
;*   o RESULT IS RETURNED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS 0xFFFFFFFF:FFFFFFFF
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
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

	.global FS$TOULL, FS$TOLL

e0      .set    r2
tmp     .set    r3

FS$TOULL: .asmfunc stack_usage(12)
	PUSH	{r2-r3, lr}		; SAVE CONTEXT

	CMP	r0, #0			; CHECK FOR A NEGATIVE VALUE
        BPL     pos
        BL      FS$TOLL                 ; CAST TO INT
        POP     {r2, r3, pc}

pos:    LSL     e0, r0, #1              ; PUT EXPONENT IN e0
        LSR     e0, e0, #24             ;
 
        SUB     e0, #0x7F               ; CHECK FOR UNDERFLOW
        BCC     unfl 	                ; IF UNDERFLOW, RETURN ZERO
 
        MOV     tmp, #0x3F              ;
        SUB     e0, tmp, e0             ;
        BCC     ovfl    	        ; CHECK FOR OVERFLOW
 
	LSL     r0, r0, #8              ; PUT MANTISSA IN r0
        MOV     tmp, #1                 ;
        LSL     tmp, tmp, #31           ;
        ORR     r0, tmp                 ; SET IMPLIED ONE IN MANTISSA
 
        MOV     tmp, e0                 ;
        SUB     tmp, #32                ;
        BCC     $1                      ;
        MOV     r1, r0                  ;
        MOV     r0, #0                  ;
	LSR	r1, tmp		        ; COMPUTE THE INTEGER VALUE
	B	return
$1:
        MOV     r1, r0                  ;
        LSR     r0, e0                  ;
        NEG     tmp, tmp                ;
        LSL     r1, tmp                 ;
return:
	; IN LITTLE ENDIAN MODE THE OUTPUT LONG LONG VALUE SHOULD BE IN R1:R0.
	; SO SWAP THE REGISTER VALUES BEFORE RETURN.

	.if .TMS470_LITTLE
	MOV	lr, r0			;
	MOV	r0, r1			;
	MOV	r1, lr			;
	.endif

	POP	{r2-r3, pc}		;

unfl:   MOV     r0, #0                  ; UNDERFLOW
        MOV     r1, #0
        POP     {r2-r3, pc}		;
 
ovfl:	MOV	r0, #0x0		; IF OVERFLOW, RETURN INFINITY
	SUB	r0, r0, #0x1		;
        MOV     r1, r0                  ; 
        POP     {r2-r3, pc}		;

	.endasmfunc
	.end
