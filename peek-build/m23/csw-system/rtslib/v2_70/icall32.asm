;******************************************************************************
;* ICALL32.ASM  - 32 BIT STATE -  v2.54                                       *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* IND_CALL - PERFORM INDIRECT CALL, SUPPORTING DUAL STATE INTERWORKING
;*
;****************************************************************************
;*
;*   o ADDRESS OF CALLED FUNCTION IS IN r4
;*   o r4 GETS DESTROYED
;*
;****************************************************************************
	.state32

	.global IND_CALL

IND_CALL: .asmfunc
	TST	r4, #1			; TEST STATE OF DESTINATION
	BNE	ep_16	                ;
	BX	r4                      ; INDIRECT CALL TO 32BIS ENTRY POINT

ep_16:	MOV	ip, r4			; FREE r4 BY STORING ITS VALUE IN ip
	MOV	r4, lr			; STORE RETURN ADDRESS IN r4
	ADD	lr, pc, #1		; SETUP NEW RETURN ADDRESS
	BX	ip			; INDIRECT CALL TO 16BIS ENTRY POINT
	.state16
	BX	r4			; RETURN
	NOP				;
	.state32

	.endasmfunc

	.end

