;******************************************************************************
;* FS_TOULL32.ASM  - 32 BIT STATE -  v2.54                                    *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS_TOULL - CONVERT AN IEEE 754 FORMAT SINGLE PRECISION FLOATING 
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
	.state32

	.global FS_TOULL, FS_TOLL

tmp     .set    r2
e0	.set	lr

FS_TOULL: .asmfunc stack_usage(8)
	STMFD	sp!, {r2, lr}		; SAVE CONTEXT
        CMP	r0, #0			; CHECK FOR A NEGATIVE VALUE
	BPL     pos
        BL      FS_TOLL                 ; CAST TO INT
	LDMFD	sp!, {r2, pc}

pos:	MOV	e0, r0, LSL #1		; PUT EXPONENT IN e0
	MOV	e0, e0, LSR #24		;

	SUBS	e0, e0, #0x7F		; CHECK FOR UNDERFLOW
	MOVCC	r0, #0			; IF UNDERFLOW, RETURN ZERO
	LDMCCFD sp!, {r2, pc}		;

	RSBS	e0, e0, #0x3F		; CHECK FOR OVERFLOW
	BCC	ovfl			; IF OVERFLOW, RETURN 0xFFFFFFFF

	MOV	r0, r0, LSL #8		; PUT MANTISSA IN r0
	ORR	r0, r0, #0x80000000	; SET IMPLIED ONE IN MANTISSA

        ; COMPUTE THE INTEGER VALUE
        CMP     e0, #0x20               ; IF e0 GREATER OR EQUAL TO 32, PERFORM
        MOVCS   r1, r0                  ; RIGHT SHIFT IN TWO STEPS.
        MOVCS   r0, #0                  ;    r0:r1 >>= 32 
        SUBCS   e0, e0, #0x20           ;    and 
        MOV     r1, r1, LSR e0          ;    r0:r1 >>= e0 - 32
        RSBCC   tmp, e0, #0x20          ;
        ORRCC   r1, r1, r0, LSL tmp     ;
        MOVCC   r0, r0, LSR e0          ; 

	; IN LITTLE ENDIAN MODE THE OUTPUT LONG LONG VALUE SHOULD BE IN R1:R0.
	; SO SWAP THE REGISTER VALUES BEFORE RETURN.

	.if .TMS470_LITTLE
	MOV	lr, r0			;
	MOV	r0, r1			;
	MOV	r1, lr			;
	.endif

	LDMFD	sp!, {r2, pc}		;

ovfl:	MOV	r0, #0x0		; IF OVERFLOW, RETURN 
	SUB	r0, r0, #0x1		; 0xFFFFFFFF:FFFFFFFF
        MOV     r1, r0                  ;
	LDMFD	sp!, {r2, pc}		;

	.endasmfunc
	.end
