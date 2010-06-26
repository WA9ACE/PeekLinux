;******************************************************************************
;* LL_DIV16.ASM  - 16 BIT STATE -  V1.16                                      *
;* Copyright (c) 1995-1997 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* LL$DIV/LL$MOD - DIVIDE TWO SIGNED 64 BIT NUMBERS.
;*
;****************************************************************************
;*
;*   o DIVIDEND IS IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o DIVISOR IS IN r2:r3  (r3:r2 IF LITTLE ENDIAN)
;*
;*   o QUOTIENT IS PLACED IN r2:r3  (r3:r2 IF LITTLE ENDIAN)
;*   o REMAINDER IS PLACED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*
;*   o DIVIDE BY ZERO RETURNS ZERO
;*   o SIGN OF REMAINDER IS THE SIGN OF THE DIVIDEND
;*
;****************************************************************************
	.state16

	.global LL$DIV
	.global LL$MOD
    .global LL_DIV
    .global LL_MOD

LL$DIV:	
LL$MOD: .asmfunc stack_usage(4)
        PUSH {lr}
	NOP
	BX  pc		        ; Change to 32-bit state
	NOP
	.align
	.state32
	BL  LL_MOD              ; and call 32-bit DIV/MOD routine.
	ADD lr, pc, #0x1        
	BX  lr
	.state16
	POP {pc}

	.endasmfunc
	.end
