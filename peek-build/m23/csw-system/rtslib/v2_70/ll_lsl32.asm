;******************************************************************************
;* LL_LSL32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* LL_LSL - LEFT SHIFT A 64 BIT LONG LONG NUMBER BY 1 to 64 bits
;****************************************************************************
;*
;*   o INPUT OP1 IS IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o INPUT OP2 IS IN r2   
;*   o RESULT IS RETURNED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o INPUT OP2 IN r2 IS NOT DESTROYED
;*
;****************************************************************************
	.state32

	.global	LL_LSL

	.if .TMS470_LITTLE

hi	.set r1
lo	.set r0

	.else

hi	.set r0
lo	.set r1

	.endif

shft	.set r2

LL_LSL:	.asmfunc stack_usage(8)
        STMFD   SP!, {r3, lr}           ;

	SUBS	r3, shft, #32		;
	MOVCS	hi, lo, LSL r3		;
	MOVCS	lo, #0			;
	RSBCC   r3, r3, #0		;
	MOVCC	hi, hi, LSL shft	;
	ORRCC	hi, hi, lo, LSR r3	;
	MOVCC	lo, lo, LSL shft	;

        LDMFD   SP!, {r3, pc}           ;

	.endasmfunc
        .end


