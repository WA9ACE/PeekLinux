;******************************************************************************
;* FD_TOLL32.ASM  - 32 BIT STATE -  v2.54                                     *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD_TOLL - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;*           INTO A SIGNED 64 BIT INTEGER 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0:r1
;*   o RESULT IS RETURNED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS 0x7FFFFFFF:FFFFFFFF/0x80000000:00000000, DEPENDING 
;*     ON THE SIGN OF THE INPUT
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o ROUNDING MODE:  ROUND TO ZERO
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

	.global	FD_TOLL

	.if .TMS470_LITTLE

o_hi	.set r1
o_lo	.set r0

	.else

o_hi	.set r0
o_lo	.set r1

	.endif

ope	.set	lr
sign	.set	r2
tmp     .set    r3

FD_TOLL: .asmfunc stack_usage(12)
        STMFD	sp!, {r2, r3, lr}	; SAVE CONTEXT

	MOV	ope, r0, LSL #1		; PUT EXPONENT IN ope
	MOV	ope, ope, LSR #21	;

	SUB	ope, ope, #0x300	; ADJUST FOR EXPONENT BIAS AND
	SUBS	ope, ope, #0xFF		; CHECK FOR UNDERFLOW
	MOVMI	r0, #0			; IF UNDERFLOW, RETURN ZERO
	MOVMI	r1, #0			; IF UNDERFLOW, RETURN ZERO
	LDMMIFD sp!, {r2, r3, pc}		;

	RSBS	ope, ope, #0x3F		; CHECK FOR OVERFLOW
	BLS	ovfl			; IF OVERFLOW, RETURN INFINITY

        MOV     sign, r0                ;
	MOV	r0, r0, LSL #11		; SHIFT THE EXPONENT (11 BITS) 
        ORR     r0, r0, r1, LSR #21     ; OUT OF THE MANTISSA R0:R1 
        MOV     r1, r1, LSL #11         ; 
	ORR	r0, r0, #0x80000000	; SET IMPLIED ONE IN HI MANTISSA 

        ; ADJUST THE VALUE IN R0:R1 FOR THE EXPONENT BY RIGHT SHIFTING 
        ; EXPONENT NUMBER OF BITS OUT OF R0:R1.
        SUBS    tmp, ope, #0x20         ; IF (EXP >= 32)
        MOVCS   r1, r0, LSR tmp         ;    R1 = MNT.HI >> EXP - 32
        MOVCS   r0, #0                  ;    R0 = 0 

        RSBCC   tmp, ope, #0x20         ; IF (EXP < 32)
        MOVCC   r1, r1, LSR ope         ; R0:R1 = R0:R1 >> EXP
        MOVCC   tmp, r0, LSL tmp        ;
        ORRCC   r1, r1, tmp             ;
        MOVCC   r0, r0, LSR ope         ;

	CMP	sign, #0		; IF THE INPUT IS NEGATIVE,
        BPL     return                  ;
	RSBS 	r1, r1, #0		;  THEN NEGATE THE RESULT
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
	LDMFD sp!, {r2, r3, pc}	        ;

	.endasmfunc
	.end
