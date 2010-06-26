;******************************************************************************
;* I_DIV16.ASM  - 16 BIT STATE -  V1.16                                       *
;* Copyright (c) 1995-1997 Texas Instruments Incorporated                     *
;******************************************************************************

	.if $$isdefed("__small_divide__")
;****************************************************************************
;* I$DIV/I$MOD - DIVIDE TWO SIGNED 32 BIT NUMBERS.
;*
;****************************************************************************
;*
;*   o DIVIDEND IS IN r0
;*   o DIVISOR IS IN r1
;*
;*   o QUOTIENT IS PLACED IN r1
;*   o REMAINDER IS PLACED IN r0
;*
;*   o DIVIDE BY ZERO RETURNS ZERO
;*   o SIGN OF REMAINDER IS THE SIGN OF THE DIVIDEND
;*
;****************************************************************************
	.state16

	.global I$DIV
	.global I$MOD
	
dvs     .set    r2              ; WORK COPY OF THE DIVISOR (SHIFTED)
quo     .set    r3              ; WORK COPY OF THE QUOTIENT
negs    .set    r4              ; SAVED COPY OF THE SIGNS

I$DIV:	.asmfunc stack_usage(16)
I$MOD:
        PUSH    {r2-r4, lr}             ; SAVE CONTEXT

        CMP     r1, #0                  ; CHECK FOR DIVISION BY ZERO
        BEQ     _div_by_zero_           ;

        LSR     negs, r1, #1            ; MOVE THE SIGN OF THE DIVISOR TO negs
        CMP     r1, #0                  ;
        BPL     _ct1_                   ;
        NEG     r1, r1                  ; ABSOLUTE SIGN OF THE DIVISOR
_ct1_:  ASR     dvs, r0, #1             ;
        EOR     negs, dvs               ; MOVE THE SIGN OF THE DIVIDEND TO negs
        CMP     r0, #0                  ;
        BPL     _ct2_                   ;
        NEG     r0, r0                  ; ABSOLUTE SIGN OF THE DIVIDEND

_ct2_:  MOV     quo, #0                 ; INITIALIZE THE QUOTIENT
        MOV     dvs, r1                 ; INITIALIZE THE DIVISOR (SHIFTED)

_sdvs_loop_:
        CMP     r0, dvs                 ; 
        BLS     _sdiv_                  ;
        LSL     dvs, dvs, #1            ; CALCULATE THE MAXIMUM DIVISOR
        B       _sdvs_loop_             ; SHIFT AMOUNT

_sdiv_:
        CMP     r0, dvs                 ; IF DIVIDEND IS LARGER THAN DIVISOR,
        ADC     quo, quo                ;  SHIFT A 1 INTO THE QUOTIENT, ELSE 0
        CMP     r0, dvs                 ; IF DIVIDEND IS LARGER THAN DIVISOR,
        BCC     _cc1_                   ;  SUBTRACT THE DIVISOR,
        SUB     r0, r0, dvs             ; 
_cc1_:  CMP     r1, dvs                 ; IF THERE IS SHIFTED DIVISOR, THEN
        BCS     _cc2_                   ;
        LSR     dvs, dvs, #1            ; CONTINUE THE LOOP.
        B       _sdiv_                  ; 
_cc2_:  MOV     r1, quo                 ; ELSE DONE. PLACE THE QUOTIENT
                                        ; IT ITS RIGHT PLACE.
        CMP     negs, #0                ;
        BPL     _ct3_                   ;
        NEG     r0, r0                  ; SET THE SIGN OF THE REMAINDER AND
_ct3_:  LSL     negs, negs, #1          ;
        BPL     _ct4_                   ;
        NEG     r1, r1                  ; SET THE SIGN OF THE QUOTIENT.
_ct4_:  POP     {r2-r4, pc}             ;

_div_by_zero_:
        MOV     r0, #0                  ; DIVIDE BY ZERO RETURNS ZERO
        POP     {r2-r4, pc}             ;

	.endasmfunc

	.endif

        .end
