;******************************************************************************
;* ULL_TOFD32.ASM  - 32 BIT STATE -  v2.54                                    *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* ULL_TOFD - CONVERT AN UNSIGNED 64 BIT INTEGER INTO AN IEEE 754 FORMAT
;*            DOUBLE PRECISION FLOATING POINT                 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o RESULT IS RETURNED IN r0:r1
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

	.global ULL_TOFD

ULL_TOFD: .asmfunc stack_usage(4)
        CMP	r0, #0			; IF ZERO, RETURN ZERO
        CMPEQ   r1, #0                  ;
	MOVEQ	pc, lr			;

        STMFD   sp!, {lr}               ;

	; IN LITTLE ENDIAN MODE THE INPUT LONG LONG VALUE IS IN R1:R0. SWAP THE
	; WORDS SO THAT WE HAVE THE LONG LONG VAULUE IN R0:R1.

	.if .TMS470_LITTLE
	MOV	lr, r0			;
	MOV	r0, r1			;
	MOV	r1, lr			;
	.endif

	MOV	lr, #0x3F		; SETUP THE EXPONENT
	ADD	lr, lr, #0x00000400	;

loop:	MOVS	r0, r0, LSL #1		; NORMALIZE THE MANTISSA
        ORR     r0, r0, r1, LSR #31     ;
        MOV     r1, r1, LSL #1          ;
	SUB	lr, lr, #0x1		; ADJUSTING THE EXPONENT, ACCORDINGLY
	BCC	loop			;

        MOV     r1, r1, LSR #12         ; SETUP LOW HALF OF MANTISSA
        ORR     r1, r1, r0, LSL #20     ;
        MOV     r0, r0, LSR #12         ; SETUP HIGH HALF OF MANTISSA
        ORR     r0, r0, lr, LSL #20     ; SETUP THE EXPONENT AND SIGN 

	LDMFD	sp!, {pc}               ;

	.endasmfunc
	.end
