;******************************************************************************
;* ULL_DIV16.ASM  - 16 BIT STATE -  V1.16                                     *
;* Copyright (c) 1995-1997 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* ULL$DIV/ULL$MOD - DIVIDE TWO UNSIGNED 64 BIT NUMBERS.
;*
;****************************************************************************
;*
;*   o DIVIDEND IS IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*   o DIVISOR IS IN r2:r3 (r3:r2 IF LITTLE ENDIAN)
;*
;*   o QUOTIENT IS PLACED IN r2:r3  (r3:r2 IF LITTLE ENDIAN)
;*   o REMAINDER IS PLACED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*
;*   o DIVIDE BY ZERO RETURNS ZERO
;*
;****************************************************************************
	.state16

	.global ULL$DIV
	.global ULL$MOD
    .global ULL_DIV
    .global ULL_MOD

ULL$DIV: .asmfunc stack_usage(4)
ULL$MOD:
        PUSH {lr}
	NOP
	BX  pc		        ; Change to 32-bit state
	NOP
	.align
	.state32
	BL  ULL_MOD             ; and call 32-bit DIV/MOD routine.
	ADD lr, pc, #0x1
	BX  lr                  ; Change the state back to 16-bit
	.state16
	POP {pc}

	.endasmfunc
	.end
