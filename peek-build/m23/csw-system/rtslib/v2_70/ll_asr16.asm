;******************************************************************************
;* LL_ASR16.ASM  - 16 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* LL$ASR - RIGHT SHIFT A 64 BIT SIGNED LONG LONG NUMBER BY 1 to 64 bits
;****************************************************************************
;*
;*   o INPUT OP1 IS IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o INPUT OP2 IS IN r2   
;*   o RESULT IS RETURNED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o INPUT OP2 IN r2 IS NOT DESTROYED
;*
;****************************************************************************
	.state16

	.global	LL$ASR

	.if .TMS470_LITTLE

hi	.set r1
lo	.set r0

	.else

hi	.set r0
lo	.set r1

	.endif

shft	.set r2

LL$ASR:	.asmfunc stack_usage(12)
	PUSH	{r3-r4, lr}  		;
        MOV     r3, #32                 ;
        SUB     r3, r3, shft            ;
        BHI     $1                      ;
        MOV     lo, hi                  ;
        ASR     hi, hi, #31
        NEG     r3, r3                  ;
        ASR     lo, r3                  ;
        B       return                  ;
$1:      
        MOV     r4, hi                  ;
        LSR     lo, shft                ;
        LSL     r4, r3                  ;
        ORR     lo, r4                  ;
        ASR     hi, shft                ;

return:
        POP     {r3-r4, pc}             ;

	.endasmfunc
        .end


