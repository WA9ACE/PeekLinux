;******************************************************************************
;* U_DIV16.ASM  - 16 BIT STATE -  v2.54                                       *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

	.if $$isdefed("__small_divide__")
;****************************************************************************
;* U$DIV/U$MOD - DIVIDE TWO UNSIGNED 32 BIT NUMBERS - SMALL CODE SIZE VERSION
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
;*
;****************************************************************************
	.state16

	.global U$DIV
	.global U$MOD

dvs     .set    r2              ; WORK COPY OF THE DIVISOR (SHIFTED)
quo     .set    r3              ; WORK COPY OF THE QUOTIENT
max     .set    r4              ; MAX DIVIDEND FOR DIVISOR SHIFT AMOUNT

U$DIV:	.asmfunc stack_usage(16)
U$MOD:
        PUSH    {r2-r4, lr}             ; SAVE CONTEXT
 
        MOV     dvs, r1                 ; CHECK FOR DIVISION BY ZERO
        BEQ     _div_by_zero_           ;

        MOV     quo, #0                 ; INITIALIZE THE QUOTIENT

        MOV     max, #1                 ;
        LSL     max, max, #31           ;
        CMP     r0, max                 ; SETUP THE MAX DIVIDEND WHEN
        BCS     _udvs_loop_             ; CALCULATING THE DIVISOR SHIFT
        MOV     max, r0                 ; AMOUNT

_udvs_loop_:
        CMP     max, dvs                ;
        BLS     _udiv_                  ;
        LSL     dvs, dvs, #1            ; CALCULATE THE MAXIMUM DIVISOR
        B       _udvs_loop_             ; SHIFT AMOUNT

_udiv_:
        CMP     r0, dvs                 ; IF DIVIDEND IS LARGER THAN DIVISOR,
        ADC     quo, quo                ;  SHIFT A 1 INTO THE QUOTIENT, ELSE 0
        CMP     r0, dvs                 ; IF DIVIDEND IS LARGER THAN DIVISOR,
        BCC     _cc1_                   ;  SUBTRACT THE DIVISOR,
        SUB     r0, r0, dvs             ; 
_cc1_:  CMP     r1, dvs                 ; IF THERE IS SHIFTED DIVISOR, THEN
        BCS     _cc2_                   ;
        LSR     dvs, dvs, #1            ; CONTINUE THE LOOP.
        B       _udiv_                  ; 
_cc2_:  MOV     r1, quo                 ; ELSE DONE. PLACE THE QUOTIENT
        POP     {r2-r4, pc}             ; IT ITS RIGHT PLACE.

_div_by_zero_:
        MOV     r0, #0                  ; DIVIDE BY ZERO RETURNS ZERO
        POP     {r2-r4, pc}             ;

	.endasmfunc

	.endif
        .end
