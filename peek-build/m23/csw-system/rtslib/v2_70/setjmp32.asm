;******************************************************************************
;* SETJMP - 32 BIT STATE -  v2.54                                             *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;*   setjmp
;*
;*     C syntax  : int setjmp(jmp_buf env)
;*
;*     Function  : Save callers current environment for a subsequent
;*                 call to longjmp.  Return 0.
;*
;*     The context save area is organized as follows:
;*
;*       env -->  .long  V1
;*                .long  V2
;*                .long  V3
;*                .long  V4
;*                .long  V5
;*                .long  V6
;*                .long  V7
;*                .long  V8
;*                .long  SP
;*                .long  LR
;*
;****************************************************************************
;*
;*  NOTE : ANSI specifies that "setjmp.h" declare "setjmp" as a macro. 
;*         In our implementation, the setjmp macro calls a function "_setjmp".
;*         However, since the user may not include "setjmp.h", we provide
;*         two entry-points to this function.
;*
;****************************************************************************
	.state32

	.global _setjmp
	.global __setjmp

_setjmp: .asmfunc stack_usage(40)
__setjmp:
	STMIA	A1!, {V1 - V8, SP, LR}
	MOV     A1,#0
	MOV	PC, LR
	.endasmfunc


;****************************************************************************
;*   longjmp
;*
;*     C syntax  : void longjmp(jmp_buf env, int val)
;*
;*     Function  : Restore the context contained in the jump buffer.
;*                 This causes an apparent "2nd return" from the
;*                 setjmp invocation which built the "env" buffer.
;*                 This return appears to return "returnvalue".
;*                 NOTE: This function may not return 0.
;****************************************************************************
	.state32

 	.global	_longjmp

_longjmp: .asmfunc stack_usage(40)
	LDMIA	A1!, {V1 - V8, SP, LR}
	CMP	A2, #0
	MOVEQ	A1, #1
	MOVNE	A1, A2
	BX	LR

	.end
	.endasmfunc
