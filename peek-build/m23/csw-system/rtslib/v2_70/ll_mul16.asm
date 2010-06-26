;******************************************************************************
;* LL_MUL16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* LL$MUL - MULTIPLY TWO 64 BIT SIGNED LONG LONG NUMBERS
;****************************************************************************
;*
;*   o INPUT OP1 IS IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o INPUT OP2 IS IN r2:r3 (r3:r2 IF LITTLE ENDIAN)
;*   o RESULT IS RETURNED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o INPUT OP2 IN r2:r3 IS NOT DESTROYED
;*
;*   o THE UPPER 64 BITS OF THE 64 X 64 MULTIPLICATION RESULT IS IGNORED.
;*
;****************************************************************************
	.state16

	.global	LL$MUL

	.if .TMS470_LITTLE

l1_hi	.set r1
l1_lo	.set r0
l2_hi	.set r3
l2_lo	.set r2

	.else

l1_hi	.set r0
l1_lo	.set r1
l2_hi	.set r2
l2_lo	.set r3

	.endif

LL$MUL:	.asmfunc stack_usage(12)
	PUSH	{r4-r5, lr}		;

	NOP				; 
	BX	pc			; CHANGE TO 32 BIT STATE FOR MULTIPLY
	NOP				;

	.state32

        ; r4:r5 = l1_hi:l1_lo * l2_hi:l2_lo = 
	;	LO32(l1_hi*l2_lo):0 + LO32(l1_lo*l2_hi):0 + ALL64(l1_lo*l2_lo)
        MUL     r4, l2_lo, l1_hi        ;  r4 = LO32(l1_hi*l2_lo)
        MLA     r4, l2_hi, l1_lo, r4    ;  r4 = LO32(l1_hi*l2_lo) + 
					;		LO32(l1_lo*l2_hi)
        MOV     r5, #0                  ;  r4:r5 = r4:0
        UMLAL   r5, r4, l2_lo, l1_lo    ;  r4:r5 = ALL64(l1_lo*l2_lo) + r4:0
        MOV     l1_hi, r4               ;  RETURN THE RESULT IN l1_hi:l1_lo 
        MOV     l1_lo, r5               ;

	ADD	lr, pc, #0x1		; CHANGE BACK TO 16 BIT STATE
	BX	lr			;

	.state16

        POP     {r4-r5, pc}             ;

	.endasmfunc
        .end


