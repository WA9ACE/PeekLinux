;***************************************************************
;* TMS470 ANSI C/C++ Codegen                     Version 1.22e *
;* Date/Time created: Fri Apr 25 14:16:29 2003                 *
;***************************************************************
	.state16
;	ac470 -i/db/tms470/sun4/tools/470cgt1.22e -i../common/ -i/db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/customer/custx/release/mod/NUCLEUS/nucleus_arm_source/ -i../../cust0/ -i../../include/ -dCHIPSET=12 -dANALOG=3 -dBOARD=43 -dLONG_JUMP=3 -mt -me /db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release_gprs/mod/emu_p/EMU_P_CALPLUS_ESAMPLE_PAGE_MODE_TEST/tools/ptool/BACKUP_C/sys_wait_clkm.c sys_wait_clkm.if 
	.file	"/db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release_gprs/mod/emu_p/EMU_P_CALPLUS_ESAMPLE_PAGE_MODE_TEST/tools/ptool/BACKUP_C/sys_wait_clkm.c"
	.file	"sys_types.h"
	.sym	_SYS_BOOL,0,12,13,8
	.sym	_SYS_UWORD8,0,12,13,8
	.sym	_SYS_WORD8,0,2,13,8
	.sym	_SYS_UWORD16,0,13,13,16
	.sym	_SYS_WORD16,0,3,13,16
	.sym	_SYS_UWORD32,0,15,13,32
	.sym	_SYS_WORD32,0,5,13,32
	.sym	_SYS_FUNC,0,144,13,32
	.file	"clkm.h"
	.file	"/db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release_gprs/mod/emu_p/EMU_P_CALPLUS_ESAMPLE_PAGE_MODE_TEST/tools/ptool/BACKUP_C/sys_wait_clkm.c"
	.sect	".text:v$0"
	.clink
	.global	_wait_ARM_cycles
	.sym	_wait_ARM_cycles,_wait_ARM_cycles,160,2,0
	.func
	.state32

;***************************************************************
;* FUNCTION VENEER: _wait_ARM_cycles                           *
;***************************************************************
_wait_ARM_cycles:
	 LDR       ip, [pc]
	 BX        ip
	.field     $wait_ARM_cycles+1, 32
	.state16
	.endfunc

	.sect	".loop"
	.global	$wait_ARM_cycles
	.sym	$wait_ARM_cycles,$wait_ARM_cycles,32,2,0
	.func	56

;***************************************************************
;* FUNCTION DEF: $wait_ARM_cycles                              *
;***************************************************************
$wait_ARM_cycles:
        ADD       SP, #-4
;* A1    assigned to _cpt_loop
	.sym	_cpt_loop,0,15,17,32
	.sym	_cpt_loop,0,15,1,32
	.line	2
        STR       A1, [SP, #0]
	.line	6
 CMP       A1, #0
	.line	7
 BEQ       END_FUNCTION
	.line	9
LOOP_LINE:        
	.line	10
 SUB       A1, A1, #1
	.line	11
 CMP       A1, #0
	.line	12
 BNE       LOOP_LINE
	.line	14
END_FUNCTION:        
	.line	15
        ADD       SP, #4
        BX        LR
	.endfunc	70,000000000h,4


;***************************************************************
;* UNDEFINED EXTERNAL REFERENCES                               *
;***************************************************************
