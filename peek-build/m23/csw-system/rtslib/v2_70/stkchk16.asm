;******************************************************************************
;* STKCHK.ASM  - STACK OVERFLOW CHECK -  v2.54                                *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************
 
;****************************************************************************
;* C$STKCHK - CHECK THE STACK FOR OVERFLOW TO MANAGE THE STACK DYNAMICALLY
;*            16 BIS
;****************************************************************************
;*
;*   o FRAME SIZE IS IN V1            
;****************************************************************************
	.state16

        .global C$STKCHK
        .global C$STKCHK_LEAF

C$STKCHK: .asmfunc stack_usage(4)
C$STKCHK_LEAF:
        PUSH      {V2}


        LDR       V2, c_stack 		; Load the top of the stack address
	ADD	  V2, V2, V1		; Add frame size to it
	ADD	  V2, #32		; Add the maximum register save size 

        CMP       V2, SP	
        BLT       L2			; Return if stack does not overflow

        LDR       V1, c_stkchkc 	; Load the stkchk called flag
        LDR       V2, [V1, #0]
        CMP       V2, #0		; If flag is set, we already detected 
        BNE       L2			; overflow, just exit

        MOV       V2, #1		; Set the stkchk called flag
        STR       V2, [V1, #0]	
        BL        $handle_stkovf 	; Call the user defined function

	
        MOV       R0, #0
        BL        $exit			; Call the exit routine

L2:
	POP	   {V2}
        BX	  LR
	.endasmfunc

;******************************************************************************
;* CONSTANT TABLE                                                             *
;******************************************************************************
           .align  4
c_stkchkc: .field          _stkchk_called,32
           .align  4
c_stack:   .field          __stack,32

;******************************************************************************
;* UNDEFINED EXTERNAL REFERENCES                                              *
;******************************************************************************
        .global $handle_stkovf
        .global $exit
        .global __stack
	.global _stkchk_called
