;******************************************************************************
;* FS_TOLL32.ASM  - 32 BIT STATE -  v2.54                                     *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS_TOLL - CONVERT AN IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;*           POINT NUMBER TO A 64 BIT SIGNED INTEGER
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
	.state32

	.global FS_TOLL

	.if .TMS470_LITTLE

o_hi	.set r1
o_lo	.set r0

	.else

o_hi	.set r0
o_lo	.set r1

	.endif

sign	.set	r2
tmp     .set    r3
e0	.set	lr

FS_TOLL: .asmfunc stack_usage(12)
        STMFD	sp!, {r2, r3, lr}

	MOV	e0, r0, LSL #1		; PUT EXPONENT IN e0
	MOV	e0, e0, LSR #24		;

	SUBS	e0, e0, #0x7F		; CHECK FOR UNDERFLOW
	MOVMI	r0, #0			; IF UNDERFLOW, RETURN ZERO
	MOVMI	r1, #0			; 
	LDMMIFD sp!, {r2, r3, pc}	;

	RSBS	e0, e0, #0x3F		; CHECK FOR OVERFLOW
	BLS	ovfl			; IF OVERFLOW, RETURN INFINITY

	MOV	sign, r0		; SAVE THE SIGN
	MOV	r0, r0, LSL #8		; PUT MANTISSA IN m0
	ORR	r0, r0, #0x80000000	; SET IMPLIED ONE IN MANTISSA

	MOV	r1, #0			;
        ; COMPUTE THE INTEGER VALUE
        CMP     e0, #0x20               ; IF e0 GREATER OR EQUAL TO 32, PERFORM
        MOVCS   r1, r0                  ; RIGHT SHIFT IN TWO STEPS.
        MOVCS   r0, #0                  ;    r0:r1 >>= 32 
        SUBCS   e0, e0, #0x20           ;    and 
        MOV     r1, r1, LSR e0          ;    r0:r1 >>= e0 - 32
        RSBCC   tmp, e0, #0x20          ;
        ORRCC   r1, r1, r0, LSL tmp     ;
        MOVCC   r0, r0, LSR e0          ; 

        CMP     sign, #0                ; IF THE INPUT IS NEGATIVE,
	BPL	return			;

        RSBS    r1, r1, #0              ;  THEN NEGATE THE RESULT
        RSC     r0, r0, #0              ;

return:
	; IN LITTLE ENDIAN MODE THE OUTPUT LONG LONG VALUE SHOULD BE IN R1:R0.
	; SO SWAP THE REGISTER VALUES BEFORE RETURN.

	.if .TMS470_LITTLE
	MOV	lr, r0			;
	MOV	r0, r1			;
	MOV	r1, lr			;
	.endif

	LDMFD 	sp!, {r2, r3, pc}	;

ovfl:	CMP	r0, #0			; IF OVERFLOW, RETURN INFINITY
	MOV	o_hi, #0x80000000	; CHECK THE SIGN OF THE INPUT
        MOV     o_lo, #0                ;
	LDMMIFD sp!, {r2, r3, pc}	;
	SUBS 	o_lo, o_lo, #0x1	; AND ADJUST INFINITY ACCORDINGLY
	SBC  	o_hi, o_hi, #0  	; 
	LDMFD   sp!, {r2, r3, pc}	;

	.endasmfunc
	.end
