;******************************************************************************
;* STKCHK.ASM  - STACK OVERFLOW CHECK -  v2.54                                *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************
 
;****************************************************************************
;* C_STKCHK - CHECK THE STACK FOR OVERFLOW TO MANAGE THE STACK DYNAMICALLY
;*            32 BIS
;****************************************************************************
;*
;*   o FRAME SIZE IS IN V1              
;****************************************************************************
        .state32
 
        .global C_STKCHK
        .global C_STKCHK_LEAF

C_STKCHK: .asmfunc
C_STKCHK_LEAF:
        LDR       V9, c_stack 		; Load the top of the stack address
	ADD	  V9, V9, V1  		; Add the frame size 
	ADD	  V9, V9, #32 		; Add max register save size
        CMP       V9, SP      		; Return if no stack overflow
        BLT       L1

        LDR       V1, c_stkchkc 	; _stkchk_called is set if we already 
        LDR       V9, [V1, #0]		; detected stack overflow 
        CMP       V9, #0	
        BNE       L1			; return if _stkchk_called is set

        MOV       V9, #1		; Now we have detected stack overflow
        STR       V9, [V1, #0]		; So call the user function to 
        BL        _handle_stkovf	;   handle the stack overflow

        MOV       R0, #0
        BL        _exit			; Call the exit routine
L1:
	BX	  LR
	.endasmfunc

;******************************************************************************
;* CONSTANT TABLE                                                             *
;******************************************************************************
            .align  4
c_stkchkc:  .field          _stkchk_called,32
            .align  4
c_stack:    .field          __stack,32

;******************************************************************************
;* UNDEFINED EXTERNAL REFERENCES                                              *
;******************************************************************************
        .global _handle_stkovf
        .global _exit
        .global __stack
	.global _stkchk_called

