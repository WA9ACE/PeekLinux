;******************************************************************************
;* ICALL16.ASM  - 16 BIT STATE -  v2.54                                       *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* IND$CALL - PERFORM INDIRECT CALL, SUPPORTING DUAL STATE INTERWORKING
;*
;****************************************************************************
;*
;*   o ADDRESS OF CALLED FUNCTION IS IN r4
;*   o r4 GETS DESTROYED
;*
;****************************************************************************
	.state16

	.global IND$CALL

IND$CALL: .asmfunc
	MOV	ip, r4			; SAVE OFF DESTINATION ADDRESS
	LSR	r4, r4, #1		; TEST STATE OF DESTINATION
	BCC	ep_32                   ;
	BX	ip                      ; INDIRECT CALL TO 16BIS ENTRY POINT
	NOP				;

ep_32:	MOV	r4, lr			; STORE RETURN ADDRESS IN r4
	MOV	lr, pc			; SETUP NEW RETURN ADDRESS
	BX	ip			; INDIRECT CALL TO 32BIS ENTRY POINT
	.state32
	BX	r4			; RETURN
	.state16

	.endasmfunc

	.end

