;******************************************************************************
;* FS_TOD16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FS$TOFD - CONVERT AN IEEE 754 FORMAT SINGLE PRECISION FLOATING 
;* 	     POINT NUMBER TO 754 FORMAT DOUBLE PRECISION FLOATING 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0
;*   o RESULT IS RETURNED IN r0:r1
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY 
;*       (0x7ff00000:00000000) or (0xfff00000:00000000) 
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
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
;*
;* +--------------------------------------------------------------+
;* | SINGLE PRECISION FLOATING POINT FORMAT                       |
;* |   32-bit representation                                      |
;* |   31 30    23 22                    0                        |
;* |   +-+--------+-----------------------+                       |
;* |   |S|    E   |           M           +                       |
;* |   +-+--------+-----------------------+                       |
;* |                                                              |
;* |   <S>  SIGN FIELD    :        0 - POSITIVE VALUE             |
;* |                               1 - NEGATIVE VALUE             |
;* |                                                              |
;* |   <E>  EXPONENT FIELD:       00 - ZERO IFF M == 0            |
;* |                         01...FE - EXPONENT VALUE (127 BIAS)  |
;* |                              FF - INFINITY                   |
;* |                                                              |
;* |   <M>  MANTISSA FIELD:  FRACTIONAL MAGNITUDE WITH IMPLIED 1  |
;* +--------------------------------------------------------------+
;*
;****************************************************************************
	.state16

	.global	FS$TOFD

e0	.set	r3

FS$TOFD: .asmfunc stack_usage(4)
	PUSH	{r3}			; SAVE CONTEXT

	LSL	e0, r0, #1		; SETUP EXPONENT IN e0
	LSR	e0, e0, #24		;
	BEQ	unfl			; CHECK FOR UNDERFLOW / ZERO

	CMP	e0, #0xFF		; CHECK FOR INFINITY
	BEQ	ovfl			; AND RETURN OVERFLOW

	MOV	r1, #0x7		; SETUP #0x380 CONSTANT
	LSL	r1, r1, #7		;
	ADD	e0, e0, r1		; ADJUST FOR THE BIAS

	LSR	r1, r0, #31		; ENCODE SIGN WITH EXPONENT
	LSL	r1, r1, #11		;
	ORR	e0, r1			;

	LSL	r1, r0, #29		; SETUP LOW PART OF MANTISSA
	LSL	r0, r0, #9		; AND SETUP HIGH PART OF MANTISSA
	LSR	r0, r0, #12		;
	LSL	e0, e0, #20		;
	ORR	r0, e0			; OR SIGN AND EXPONENT INTO RESULT
	POP	{r3}			;
	MOV	pc, lr			;
	
unfl:	MOV	r0, #0x0		; IF UNDERFLOW, RETURN 0
	MOV	r1, #0x0		;
	POP	{r3}			;
	MOV	pc, lr			;

ovfl:	MOV	r1, #0			; IF OVERFLOW, RETURN +/- INFINITY
        LSR	r0, r0, #23		; ISOLATE EXP AND SIGN
	LSL	r0, r0, #3		; FILL IN REST OF Emax
	ADD	r0, r0, #7		;
        LSL	r0, r0, #20		; SHIFT EXP AND SIGN INTO PLACE
	POP	{r3}			;
	MOV	pc, lr			;

	.endasmfunc

	.end
