;******************************************************************************
;* SETJMP - 16 BIT STATE -  v2.54                                             *
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
	.state16

	.global	$setjmp
	.global $_setjmp

$setjmp: .asmfunc stack_usage(40)
$_setjmp:
	STMIA	A1!, {V1, V2, V3, V4}
	MOV	A2, V5
	MOV	A3, V6
	MOV	A4, V7
	MOV	V1, V8
	MOV	V2, SP
	MOV	V3, LR
	STMIA	A1!, {A2, A3, A4, V1, V2, V3}
	SUB	A1, #40
	LDMIA	A1!, {V1, V2, V3, V4}
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
	.state16

	.global	$longjmp

$longjmp: .asmfunc stack_usage(24)
	ADD	A1, #16
	LDMIA	A1!, {A3, A4, V1, V2, V3, V4}
	MOV	V5, A3
	MOV	V6, A4
	MOV	V7, V1
	MOV	V8, V2
	MOV	SP, V3
	MOV     LR, V4
	SUB	A1, #40
	LDMIA	A1!, {V1, V2, V3, V4}
	CMP	A2, #0
	BNE	_ct_
	MOV	A2, #1
_ct_:	MOV	A1, A2
	MOV	PC, LR
	.endasmfunc

	.end
