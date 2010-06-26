;******************************************************************************
;            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION           
;                                                                             
;   Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only 
;   Unauthorized reproduction and/or distribution is strictly prohibited.  This 
;   product  is  protected  under  copyright  law  and  trade  secret law as an 
;   unpublished work.  Created 1987, (C) Copyright 1996 Texas Instruments.  All 
;   rights reserved.                                                            
;                  
;                                                           
;   Filename       	: init.asm
;
;   Description    	: Environment configuration
;
;   Project        	: drivers
;
;   Author         	: pmonteil@tif.ti.com Patrice Monteil.
;
;   Version number	: 1.4
;
;   Date and time	: 03/06/01 10:44:19
;
;   Previous delta 	: 12/19/00 14:28:47
;
;   SCCS file      	: /db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release_gprs/mod/emu_p/EMU_P_C_SAMPLE_REQ1145_BIS/drivers1/board_7/SCCS/s.init.asm
;
;   Sccs Id  (SID)       : '@(#) init.asm 1.4 03/06/01 10:44:19 '
;
; 
;*****************************************************************************

; use in int_loc_1_15.s for first initializations 


      .if BOARD = 70 | BOARD = 71
    .if CHIPSET = 15
      .if PSP_FAILSAFE = 1
CS0_MEM_REG   .word  0x0000FFF9   ; Asynch Read/write, 15-RDWST,15-WRWST, 15-WELEN
CS3_MEM_REG   .word  0x0000FFF9   ; Asynch Read/write, 4-RDWST,5-WRWST, 3-WELEN
      .else
CS0_MEM_REG   .word  0x00002031   ; Asynch Read/write, 3-RDWST,0-WRWST, 2-WELEN
CS3_MEM_REG   .word  0x00002031   ; Asynch Read/write, 3-RDWST,0-WRWST, 2-WELEN
      .endif
      .endif ; CHIPSET
     .endif  ; BOARD

CLKM_MEM_REG  .equ  0x31	;the same define INIT_CLKM_ARM_CLK = 0x1031 for InitArmAfterReset
CTL_MEM_REG   .short  0x02a   ; rhea strobe 0/1 + API access size adaptation
