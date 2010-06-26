;******************************************************************************
;* FD_TOULL32.ASM  - 32 BIT STATE -  v2.54                                    *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD_TOULL - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;*   	      POINT NUMBER TO A 64 BIT UNSIGNED INTEGER
;****************************************************************************
;*
;*   o INPUT OP IS IN r0:r1
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

	.global	FD_TOULL, FD_TOLL

ope	.set	lr
tmp     .set    r2

FD_TOULL: .asmfunc stack_usage(8)
	STMFD	sp!, {r2, lr}		; SAVE CONTEXT
        CMP	r0, #0			; CHECK FOR A NEGATIVE VALUE
	BPL     pos
        BL      FD_TOLL                 ; CAST TO INT
	LDMFD	sp!, {r2, pc}


pos:	MOV	ope, r0, LSL #1		; PUT EXPONENT IN ope
	MOV	ope, ope, LSR #21	;

	SUB	ope, ope, #0x300	; ADJUST FOR EXPONENT BIAS AND
	SUBS	ope, ope, #0xFF		; CHECK FOR UNDERFLOW
	MOVMI	r0, #0			; IF UNDERFLOW, RETURN ZERO
	MOVMI	r1, #0			; 
	LDMMIFD sp!, {r2, pc}		;

	RSBS	ope, ope, #0x3F		; CHECK FOR OVERFLOW. IF OVERFLOW, 
	BCC	ovfl			; RETURN 0xFFFFFFFF:FFFFFFFF

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

	; IN LITTLE ENDIAN MODE THE OUTPUT LONG LONG VALUE SHOULD BE IN R1:R0.
	; SO SWAP THE REGISTER VALUES BEFORE RETURN.

	.if .TMS470_LITTLE
	MOV	lr, r0			;
	MOV	r0, r1			;
	MOV	r1, lr			;
	.endif

	LDMFD 	sp!, {r2, pc}		;

ovfl:	MOV	r0, #0x0		; IF OVERFLOW, RETURN 0XFFFFFFFF
	SUB	r0, r0, #0x1		;
        MOV     r1, r0                  ;
	LDMFD	sp!, {r2, pc}		;

	.endasmfunc
	.end
