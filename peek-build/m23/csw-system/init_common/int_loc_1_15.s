;            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION           
;                                                                             
;   Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only 
;   Unauthorized reproduction and/or distribution is strictly prohibited.  This 
;   product  is  protected  under  copyright  law  and  trade  secret law as an 
;   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All 
;   rights reserved.                                                            
;                  
;                                                           
;   Filename       	: int.s
;
;   Description    	: Nucleus initialization
;
;   Project        	: Drivers
;
;   Author         	: proussel@ti.com  Patrick Roussel.
;
;   Version number	: 1.3
;
;   Date and time	: 07/23/98 15:36:07
;
;   Previous delta 	: 07/23/98 15:36:06
;
;   SCCS file      	: /db/gsm_asp/db_ht96/dsp_0/gsw/rel_0/mcu_l1/release1.5/mod/emu/EMU_MCMP/eva3_drivers/source/SCCS/s.int.s
;
;   Sccs Id  (SID)      : '@(#) int.s 1.3 07/23/98 15:36:07 '
;/*************************************************************************/
;/*                                                                       */
;/*     Copyright (c) 1993 - 1996 Accelerated Technology, Inc.            */
;/*                                                                       */
;/* PROPRIETARY RIGHTS of Accelerated Technology are involved in the      */
;/* subject matter of this material.  All manufacturing, reproduction,    */
;/* use, and sales rights pertaining to this subject matter are governed  */
;/* by the license agreement.  The recipient of this software implicitly  */
;/* accepts the terms of the license.                                     */
;/*                                                                       */
;/*************************************************************************/
;
;/*************************************************************************/
;/*                                                                       */
;/* FILE NAME                                            VERSION          */
;/*                                                                       */
;/*      int.s                                       PLUS/THUMB/T 1.3     */
;/*                                                                       */
;/* COMPONENT                                                             */
;/*                                                                       */
;/*      IN - Initialization                                              */
;/*                                                                       */
;/* DESCRIPTION                                                           */
;/*                                                                       */
;/*      This file contains the target processor dependent initialization */
;/*      routines and data.                                               */
;/*                                                                       */
;/* AUTHOR                                                                */
;/*                                                                       */
;/*      Barry Sellew, Accelerated Technology, Inc.                       */
;/*                                                                       */
;/* DATA STRUCTURES                                                       */
;/*                                                                       */
;/*      INT_Vectors                         Interrupt vector table       */
;/*                                                                       */
;/* FUNCTIONS                                                             */
;/*                                                                       */
;/*      INT_Initialize                      Target initialization        */
;/*      INT_Vectors_Loaded                  Returns a NU_TRUE if all the */
;/*                                            default vectors are loaded */
;/*      INT_Setup_Vector                    Sets up an actual vector     */
;/*                                                                       */
;/* DEPENDENCIES                                                          */
;/*                                                                       */
;/*      nucleus.h                           System constants             */ 
;/*                                                                       */ 
;/* HISTORY                                                               */
;/*                                                                       */
;/*         NAME            DATE                    REMARKS               */
;/*                                                                       */
;/*      B. Sellew       01-19-1996      Created initial version 1.0      */
;/*	     B. Sellew	     01-22-1996	     Verified version 1.0	  	      */
;/*	     B. Sellew	     03-14-1996	     Modified to use the ROM          */
;/*				                          initialization method,          */
;/*	                                      resulting in version 1.1        */
;/*      B. Sellew       03-14-1996      Verified version 1.1             */
;/*      B. Sellew       02-06-1997      Created version 1.3              */
;/*	     B. Sellew	     02-06-1997	     Verified version 1.3	  	      */
;/*      M. Manning      06-02-1997      Added support for FIQ            */
;/*                                       interrupts. Bumped to 1.4       */
;/*      M. Manning      06-03-1997      Verified version 1.4             */
;/*                                                                       */
;/*************************************************************************/
;
;
;/* Define constants used in low-level initialization.  */
;
;

  .if LONG_JUMP = 3
        .global IND_CALL
        .global _f_load_int_mem
        .global _ResetVector
        
; Initialization for variable S_D_Mem
; NEW COMPILER MANAGEMENT
; S_D_Mem is a UWORD32, and not anymore UWORD8
; See mem_load.c
;
      	.sect	".cinit"
      	.align	4
      	.field  	4,32
      	.field  	_S_D_Mem+0,32
      	.field  	0,32		; _S_D_Mem @ 0

      	.sect	".text"
        .global _S_D_Mem
_S_D_Mem: .usect  "S_D_Mem",4,4
        .sym    _S_D_Mem,_S_D_Mem,14,2,32        ; For debug only

; Initialization for variable E_D_Mem
; NEW COMPILER MANAGEMENT
; S_D_Mem is a UWORD32, and not anymore UWORD8
; See mem_load.c
;
        
      	.sect	".cinit"
      	.align	4
      	.field  	4,32
      	.field  	_E_D_Mem+0,32
      	.field  	0,32		; _E_D_Mem @ 0

      	.sect	".text"
        .global _E_D_Mem
_E_D_Mem: .usect  "E_D_Mem",4,4
        .sym    _E_D_Mem,_E_D_Mem,14,2,32        ; For debug only

  	.endif	; LONGJUMP = 3
  
        .global _f_load_int_mem
        .global _ResetVector
  
LOCKOUT         .equ     00C0h              ; Interrupt lockout value
LOCK_MSK        .equ     00C0h              ; Interrupt lockout mask value
MODE_MASK       .equ     001Fh              ; Processor Mode Mask
SUP_MODE        .equ     0013h              ; Supervisor Mode (SVC)
IRQ_MODE        .equ     0012h              ; Interrupt Mode (IRQ)
FIQ_MODE        .equ     0011h              ; Fast Interrupt Mode (FIQ)
  .if OP_L1_STANDALONE = 0
ABORT_MODE      .equ     0017h              ; Abort Interrupt Mode
UNDEF_MODE      .equ     001Bh              ; Undefined Interrupt Mode (should not happen)
  .endif	; OP_L1_STANDALONE =0
IRQ_STACK_SIZE  .equ     128                ; Number of bytes in IRQ stack (must be align(8))
                                            ;   Note that the IRQ interrupt,
                                            ;   by default, is managed by 
                                            ;   Nucleus PLUS.  Only several
                                            ;   words are actually used.  The 
                                            ;   system stack is what will 
                                            ;   actually be used for Nuclues
                                            ;   PLUS managed IRQ interrupts.
FIQ_STACK_SIZE  .equ     512                ; Number of bytes in FIQ stack. (must be align(8))
                                            ;   This value is application 
                                            ;   specific.  By default, Nucleus
                                            ;   does not manage FIQ interrupts
                                            ;   and furthermore, leaves them
  .if PSP_STANDALONE = 1                                             ;   enabled virtually all the time.
SYSTEM_SIZE     .equ     2048               ; Define the system stack size (must be align(8))
  .else
SYSTEM_SIZE     .equ     1024               ; Define the system stack size (must be align(8))
  .endif
TIMER_SIZE      .equ     1024               ; Define timer HISR stack size (must be align(8))
TIMER_PRIORITY  .equ     2                  ; Timer HISR priority (values from
                                            ;   0 to 2, where 0 is highest)
  .if OP_L1_STANDALONE = 1
; The user must check the demo.map file after the first link to decide what value
; should be assigned to OTHER_SECTIONS, or the user can modify demo.cmd to arrange
; the sections correctly instead of using OTHER_SECTIONS.
OTHER_SECTIONS  .equ     700h               ; other sections after bss.
  .endif	; OP_L1_STANDALONE = 1

;
;/* End of low-level initialization constants.  */
;
;
;/* Define the initialization flag that indicates whether or not all of the
;   default vectors have been loaded during initialization.  */
;
;INT    INT_Loaded_Flag;

        .def    _INT_Loaded_Flag
        .bss    _INT_Loaded_Flag, 4, 4
;
;/* Define the vector table  */
;

        .sect ".start"
     .if SECURITY = 2 | CHIPSET = 15 ; For Locosto Always _INT_Initialize
            .ref    _INT_Initialize

_ResetVector:
	    B	_INT_Initialize
      .else		      

        .ref    _INT_Bootloader_Start

_ResetVector:
        B	_INT_Bootloader_Start
     .endif		; SECURITY      

        .sect ".indint"
         
        .def  _IndirectVectorTable
_IndirectVectorTable:
        LDR   PC, [PC, #0x14]
        LDR   PC, [PC, #0x14]
        LDR   PC, [PC, #0x14]
        LDR   PC, [PC, #0x14]
        LDR   PC, [PC, #0x14]
        LDR   PC, [PC, #0x14]
        LDR   PC, [PC, #0x14]

        .word    INT_Undef_Inst
        .word    INT_Swi
        .word    INT_Abort_Prefetch
        .word    INT_Abort_Data
        .word    INT_Reserved
  .if OP_L1_STANDALONE = 0
    .if TI_NUC_MONITOR = 1
        .word    _INT_IRQ
    .else
        .word    INT_IRQ
    .endif
  .else
        .word    INT_IRQ
  .endif
  .if OP_L1_STANDALONE = 0
    .if TI_PROFILER = 1
        .word    _INT_FIQ
    .else
        .word    INT_FIQ
    .endif
  .else
        .word    INT_FIQ
  .endif
;
;	.text
;
;	.ref	cinit

	.sect       ".inttext"
	.global     cinit       ; Linker symbol for C variable init.
       .if (TOOL_CHOICE > 1)

        .global     pinit       ; Linker symbol for C++ constructor table
       .endif
; Address definitions in the section where they are used.

;
;/* Define the global system stack variable.  This is setup by the 
;   initialization routine.  */
;
;
;       extern VOID            *TCD_System_Stack;

        .ref  	_TCD_System_Stack
        .ref    _TCT_System_Limit
;
;
;/* Define the global data structures that need to be initialized by this
;   routine.  These structures are used to define the system timer management
;   HISR.  */
;   
;	extern VOID     *TMD_HISR_Stack_Ptr;
;	extern UNSIGNED  TMD_HISR_Stack_Size;
;	extern INT       TMD_HISR_Priority;

;
        .ref  	_TMD_HISR_Stack_Ptr
        .ref  	_TMD_HISR_Stack_Size
        .ref  	_TMD_HISR_Priority
;
;/* Define extern function references.  */
;
;	VOID   INC_Initialize(VOID *first_available_memory);
;	VOID   TCT_Interrupt_Context_Save(VOID);
;	VOID   TCT_Interrupt_Context_Restore(VOID);
;	VOID   TCC_Dispatch_LISR(INT vector_number);
;	VOID   TMT_Timer_Interrupt(void);
;
        .ref  	_INC_Initialize
        .ref  	_TCT_Interrupt_Context_Save
        .ref  	_TCT_Interrupt_Context_Restore
        .ref  	_TCC_Dispatch_LISR
        .ref  	_TMT_Timer_Interrupt
        ;/* Application f_inth_irq_handler, f_inth_fiq_handler */
        .ref  	_f_inth_irq_handler
        .ref  	_f_inth_fiq_handler
;
; /* Reference pointers defined by the linker */
;
	.ref	.bss
	.ref	end

  .if OP_L1_STANDALONE = 0
;
;/* Define indirect branching labels for the vector table  */
;
        .def    INT_Undef_Inst
INT_Undef_Inst
        B       arm_undefined               ; Undefined
;
        .def    INT_Swi
INT_Swi
        B      arm_swi                     ; Software Generated
;
        .def    INT_Abort_Prefetch
INT_Abort_Prefetch
        B      arm_abort_prefetch          ; Abort Prefetch
;
        .def    INT_Abort_Data
INT_Abort_Data
        B       arm_abort_data              ; Abort Data
;
        .def    INT_Reserved
INT_Reserved
        B       arm_reserved                ; Reserved
;
        .def    Vect_IRQ
Vect_IRQ
    .if TI_NUC_MONITOR = 1
        B       Vect_IRQ
    .else
        B       INT_IRQ
    .endif
;
        .def    Vect_FIQ
Vect_FIQ
    .if TI_PROFILER = 1
        B       Vect_FIQ
    .else
        B       INT_FIQ
    .endif
;
     .if PSP_STANDALONE =1
	.def INT_Small_Sleep
INT_Small_Sleep
	.ref TCT_Schedule_Loop
	B TCT_Schedule_Loop
     .endif
	
  .endif ;   OP_L1_STANDALONE = 0

;
;/*************************************************************************/
;/*                                                                       */
;/* FUNCTION                                                              */
;/*                                                                       */
;/*      INT_Initialize                                                   */
;/*                                                                       */
;/* DESCRIPTION                                                           */
;/*                                                                       */
;/*      This function sets up the global system stack variable and       */
;/*      transfers control to the target independent initialization       */
;/*      function INC_Initialize.  Responsibilities of this function      */
;/*      include the following:                                           */
;/*                                                                       */
;/*             - Setup necessary processor/system control registers      */
;/*             - Initialize the vector table                             */
;/*             - Setup the system stack pointers                         */
;/*             - Setup the timer interrupt                               */
;/*             - Calculate the timer HISR stack and priority             */
;/*             - Calculate the first available memory address            */
;/*             - Transfer control to INC_Initialize to initialize all of */
;/*               the system components.                                  */
;/*                                                                       */
;/* AUTHOR                                                                */
;/*                                                                       */
;/*      Barry Sellew, Accelerated Technology, Inc.                       */
;/*                                                                       */
;/* CALLED BY                                                             */
;/*                                                                       */
;/*      none  					 			  */
;/*                                                                       */
;/* CALLS                                                                 */
;/*                                                                       */
;/*      INC_Initialize                      Common initialization        */
;/*                                                                       */
;/* INPUTS                                                                */
;/*                                                                       */
;/*      None                                                             */
;/*                                                                       */
;/* OUTPUTS                                                               */
;/*                                                                       */
;/*      None                                                             */
;/*                                                                       */
;/* HISTORY                                                               */
;/*                                                                       */
;/*         NAME            DATE                    REMARKS               */
;/*                                                                       */
;/*      B. Sellew       01-19-1996      Created initial version 1.0      */
;/*	 B. Sellew	 01-22-1996	 Verified version 1.0	  	  */
;/*                                                                       */
;/*************************************************************************/
;VOID    INT_Initialize(void)
;{
	.def	_c_int00
_c_int00

	.include "init.asm"
addrCS0	      	.word	 0xfffffb04		    ; CS0 address space


  ;.if OP_L1_STANDALONE = 0
    .if BOARD = 70 | BOARD = 71
MPU_CONF_GPIO_39_REG .word   0xFFFEF19C   ; CONF_GPIO_39 Register
A22_ENABLE_VALUE     .short  0x0001
    .endif
   ;.endif ;   OP_L1_STANDALONE = 0

CNTL_ARM_CLK_REG  .word   0xFFFFFD00   ; CNTL_ARM_CLK register address
DPLL_CNTRL_REG    .word   0xFFFF9800   ; DPLL control register address
EXTRA_CONTROL_REG .word   0xFFFFFB10   ; Extra Control register CONF address
MPU_CTL_REG       .word   0xFFFFFF08   ; MPU_CTL register address   

CNTL_ARM_CLK_RST  .short  0x1081	   ; Initialization of CNTL_ARM_CLK register
                                       ; Use DPLL, Divide by 1
DPLL_CONTROL_RST  .short  0x2002       ; Configure DPLL in default state
DISABLE_DU_MASK   .short  0x0800       ; Mask to Disable the DU module
MPU_CTL_RST       .short  0x0000       ; Reset value of MPU_CTL register - All protections disabled

DPLL_CONTROL_104MHZ  .short 0x2402  
DPLL_CONTROL_104MHZ_ENABLE  .short 0x2412  
ROM_MEMSET  .word 0x08079c60    ; ROM code address for memset function
ROM_MEMCPY  .word 0x0807b5e4     ;ROM code address for memcpy function

DRP_SCRIPT_START_REG  .word   0xFFFF0500  ; 
DRP_ABORT_SCRIPT      .byte   0x00        ;
DRP_IDLE_SCRIPT       .byte   0x8F        ;
DRP_OCP_SYSCONFIG_REG .word   0xFFFF0010  ;
DRP_SW_RESET          .byte   0x02        ;

MPU_TIMER1_CNTL_TIMER .word 0xFFFE3800
MPU_TIMER1_LOAD_TIMER.word 0xFFFE3802
MPU_TIMER1_READ_TIMER .word 0xFFFE3804


MPU_TIMER1_LOAD_VALUE .word 0xffff
MPU_TIMER1_CTRL_VALUE .short 0x3d
MPU_TIMER1_CTRL_VALUE_OFF .short 0x1d



c_cinit	.long	cinit

       .if (TOOL_CHOICE > 1)
c_pinit .long   pinit
       .endif
        .def  	_INT_Initialize
_INT_Initialize

;
;  Configuration of ARM clock and DPLL frequency
;
  
   ;OMAPS82614 we will lock PLL at 104MHz to speed up memory init. We assume at this stage that no critical peripheral/DSP
  ; accesses are made until Init_Target() and so do not need to reset dpll afterwards

  ;  Configure DPLL register with 104MHz value
  ;
	ldr	r1,DPLL_CNTRL_REG     ; Load address of DPLL register in R1
	ldrh	r2,DPLL_CONTROL_104MHZ   ; Load DPLL init value in R2
	strh	r2,[r1]               ; Store DPLL init value in DPLL register

  ; Now enable
	ldrh	r2,DPLL_CONTROL_104MHZ_ENABLE   ; Load DPLL enable value in R2
	strh	r2,[r1]               ; Store DPLL enable value in DPLL register


;
;  Wait that DPLL goes in Lock at 104MHz mode
;
Wait_DPLL_Lock
  ldrh   r2,[r1]               ; Load DPLL register
  and   r2,r2,#1              ; Perform a mask on bit 0
  cmp   r2,#0                 ; Compare DPLL lock bit
  beq   Wait_DPLL_Lock      ; Wait Lock mode (i.e. bit[0]='1')
; OMAPS82614 end

  	;
	;  Configure CNTL_ARM_CLK register with reset value: DPLL is used to
  	;  generate ARM clock with division factor of 1.
  	;
	
	ldr	r1,CNTL_ARM_CLK_REG  ; Load address of CNTL_ARM_CLK register in R1
	ldrh	r2,CNTL_ARM_CLK_RST  ; Load CNTL_ARM_CLK reset value in R2
	strh	r2,[r1]              ; Store CNTL_ARM_CLK reset value in CNTL_ARM_CLK register
 	.if VIRTIO!=1 
	;
	;  Disable all MPU protections
		;
	ldr   r1,MPU_CTL_REG       ; Load address of MPU_CTL register
	ldrh  r2,MPU_CTL_RST       ; Load reset value of MPU_CTL register
	strh  r2,[r1]              ; Store reset value of MPU_CTL register

  	ldr  r1, DRP_SCRIPT_START_REG
  	ldrb r2, DRP_ABORT_SCRIPT
  	strb r2, [r1]

  	ldr  r1, DRP_OCP_SYSCONFIG_REG
  	ldrb r2, DRP_SW_RESET
  	strb r2, [r1]

	.endif


   ldr     r1,MPU_TIMER1_LOAD_TIMER       ; Load address of MPU_CTL register
   ldrh    r2,MPU_TIMER1_LOAD_VALUE        ; Load reset value of MPU_CTL register
   strh    r2,[r1]              ; Store reset value of MPU_CTL register

   ldr     r1,MPU_TIMER1_CNTL_TIMER       ; Load address of MPU_CTL register
   ldrh    r2,MPU_TIMER1_CTRL_VALUE        ; Load reset value of MPU_CTL register
   strh    r2,[r1]              ; Store reset value of MPU_CTL register
;
;  Wait-state configuration of external and internal memories
;

	ldr	  r1,addrCS0		

	ldr	r2,CS0_MEM_REG	; pSRAM initialization
	str	r2,[r1]		; CS0

	ldr	r2,CS3_MEM_REG	; NOR flash initialization
	str	r2,[r1,#0x0C]	; CS3 


  ;.if OP_L1_STANDALONE = 0
    .if BOARD = 70 | BOARD = 71
    ; On I-Sample board A22 must be enabled to access full 128 Mbit Memory
       ldr     r1,MPU_CONF_GPIO_39_REG
       ldrh    r2,[r1]
       ldr     r0,A22_ENABLE_VALUE
       strh    r0,[r1]
    .endif ; BOARD 70 | 71
  ;.endif ;   OP_L1_STANDALONE = 0

;
;    /* Insure that the processor is in supervisor mode.  */
;
        MRS     a1,CPSR                     ; Pickup current CPSR
        BIC     a1,a1,#MODE_MASK            ; Clear the mode bits
        ORR     a1,a1,#SUP_MODE             ; Set the supervisor mode bits
        ORR     a1,a1,#LOCKOUT              ; Insure IRQ and FIQ interrupts are
                                            ;   locked out
        MSR     CPSR,a1                     ; Setup the new CPSR

;
; NEW COMPILER MANAGEMENT
; REWORK OF .bss INITIALIZATION - start
; Creation of INT_memset and INT_memcpy, respectively identical to memset and 
; memcpy from the rts library of compiler V2.51.
; They are used to make the initialization of the .bss section and the load 
; of the internal ram code not dependent to the 32-bit alignment.
; The old code used for the initialization and the load used a loop with
; 4-byte increment, assuming the 32-bit alignment of the .bss section.
; This alignment is not true with compiler V2.51.
; This change applies whatever the compiler version.
;
;    /* Clear the un-initialized global and static C data areas.  */
;       Initialize the system stack pointer a first time to allow use of memset function
;       which needs stack.
;       The system stack pointers will be fully initialized after having cleared
;       the BSS area.  */
;        
  .if OP_L1_STANDALONE = 1

        LDR     a1,BSS_End   	            ; Pickup the ending address of BSS
; MQ added the following two lines to avoid messing the last few sections.
        MOV     a2,#OTHER_SECTIONS          ; jump over other sections behind
        ADD     a1,a1,a2                    ; BSS_END.
  .else
        LDR     a1,StackSegment             ; Pickup the begining address from .cmd file
                                            ;   (is aligned on 8 byte boundary)
  .endif ;   OP_L1_STANDALONE
        MOV     a2,#SYSTEM_SIZE             ; Pickup system stack size
        SUB     a2,a2,#4                    ; Subtract one word for first addr
        ADD     a3,a1,a2                    ; Build start of system stack area
  .if OP_L1_STANDALONE = 1
        BIC     a3,a3,#3                    ; Insure word aligment of stack
  .endif
        MOV     sp,a3                       ; Setup initial stack pointer


        STMFD   sp!,{a1-a4}               ; Save a1-a4 registers to stack

        LDR     a1,BSS_Start              ; Pickup the start of the BSS area
        LDR     a3,BSS_End                ; Pickup the end of the BSS area
        SUB     a3,a3,a1                  ; Calculate size of the BSS area
        MOV     a2,#0                     ; Clear value in a2

        BL      _INT_memset                   ; Clear the BSS area using memset function


  .if LONG_JUMP = 3                ; Visual Linker
        LDR     a1,BSS_IntMem_Start              ; Pickup the start of the BSS area
        LDR     a3,BSS_IntMem_End         ; Pickup the end of the BSS area
        SUB     a3,a3,a1                  ; Calculate size of the BSS area
        MOV     a2,#0                     ; Clear value in a2

        BL      _INT_memset                   ; Clear the BSS area using memset function

  .endif

        LDMFD   sp!,{a1-a4}               ; Restore a1-a4 registers from stack

; NEW COMPILER MANAGEMENT
; REWORK OF .bss INITIALIZATION - end  

  
;
;    /* Setup the vectors loaded flag to indicate to other routines in the 
;       system whether or not all of the default vectors have been loaded. 
;       If INT_Loaded_Flag is 1, all of the default vectors have been loaded.
;       Otherwise, if INT_Loaded_Flag is 0, registering an LISR cause the
;       default vector to be loaded.  In the THUMB this variable is always
;       set to 1.  All vectors must be setup by this function.  */
;    INT_Loaded_Flag =  0;
;
        MOV     a1,#1                       ; All vectors are assumed loaded
        LDR     a2,Loaded_Flag   	        ; Build address of loaded flag  
        STR     a1,[a2,#0]                  ; Initialize loaded flag

  .if (GSM_IDLE_RAM=0) | (BOARD=35 & OP_L1_STANDALONE=0)
;
  .if OP_L1_STANDALONE = 1
;    /* Initialize the system stack pointers.  This is done after the BSS is
;       clear because the TCD_System_Stack pointer is a BSS variable!  It is
;       assumed that available memory starts immediately after the end of the
;       BSS section.  */
;
        LDR     a1,BSS_End   	            ; Pickup the ending address of BSS
; MQ added the following two lines to avoid messing the last few sections.
        MOV     a2,#OTHER_SECTIONS          ; jump over other sections behind
        ADD     a1,a1,a2                    ; BSS_END.

  .else

;    /* Initialize the system stack pointers.  This is done after the BSS is
;       cleared because the TCD_System_Stack pointer is a BSS variable!  It is
;       assumed that the .cmd file is written to direct where these stacks should
;       be allocated and to align them on double word boundaries.
;
        LDR     a1,StackSegment             ; Pickup the begining address from .cmd file
                                            ;   (is aligned on 8 byte boundary)
  .endif ;   OP_L1_STANDALONE

        MOV     a2,#SYSTEM_SIZE             ; Pickup system stack size
        SUB     a2,a2,#4                    ; Subtract one word for first addr
        ADD     a3,a1,a2                    ; Build start of system stack area
  .if OP_L1_STANDALONE = 1
        BIC     a3,a3,#3                    ; Insure word aligment of stack
  .endif
        MOV     v7,a1                       ; Setup initial stack limit
        LDR     a4,System_Limit             ; Pickup system stack limit address
        STR     v7,[a4, #0]                 ; Save stack limit
        MOV     sp,a3                       ; Setup initial stack pointer
        LDR     a4,System_Stack             ; Pickup system stack address
        STR     sp,[a4, #0]                 ; Save stack pointer
        MOV     a2,#IRQ_STACK_SIZE          ; Pickup IRQ stack size in bytes
        ADD     a3,a3,a2                    ; Allocate IRQ stack area
  .if OP_L1_STANDALONE = 1
        BIC     a3,a3,#3                    ; Insure word alignment
  .endif
        MRS     a1,CPSR                     ; Pickup current CPSR
        BIC     a1,a1,#MODE_MASK            ; Clear the mode bits
        ORR     a1,a1,#IRQ_MODE             ; Set the IRQ mode bits
        MSR     CPSR,a1                     ; Move to IRQ mode
        MOV     sp,a3                       ; Setup IRQ stack pointer
        MOV     a2,#FIQ_STACK_SIZE          ; Pickup FIQ stack size in bytes
        ADD     a3,a3,a2                    ; Allocate FIQ stack area
  .if OP_L1_STANDALONE = 1
        BIC     a3,a3,#3                    ; Insure word alignment
  .endif
        MRS     a1,CPSR                     ; Pickup current CPSR
        BIC     a1,a1,#MODE_MASK            ; Clear the mode bits
        ORR     a1,a1,#FIQ_MODE             ; Set the FIQ mode bits
        MSR     CPSR,a1                     ; Move to the FIQ mode
        MOV     sp,a3                       ; Setup FIQ stack pointer

  .if OP_L1_STANDALONE = 0
        MRS     a1,CPSR                     ; Pickup current CPSR
        BIC     a1,a1,#MODE_MASK            ; Clear the mode bits
        ORR     a1,a1,#ABORT_MODE           ; Set the Abort mode bits
        MSR     CPSR,a1                     ; Move to the Abort mode
        LDR     sp,Exception_Stack          ; Setup Abort stack pointer

        MRS     a1,CPSR                     ; Pickup current CPSR
        BIC     a1,a1,#MODE_MASK            ; Clear the mode bits
        ORR     a1,a1,#UNDEF_MODE           ; Set the Undefined mode bits
        MSR     CPSR,a1                     ; Move to the Undefined mode
        LDR     sp,Exception_Stack          ; Setup Undefined stack pointer
                                            ;   (should never be used)
  .endif

; go to Supervisor Mode
        MRS     a1,CPSR                     ; Pickup current CPSR
        BIC     a1,a1,#MODE_MASK            ; Clear mode bits
        ORR     a1,a1,#SUP_MODE             ; Set the supervisor mode bits
        MSR     CPSR,a1                     ; All interrupt stacks are setup,
                                            ;   return to supervisor mode
;
;    /* Define the global data structures that need to be initialized by this
;       routine.  These structures are used to define the system timer 
;       management HISR.  */
;    TMD_HISR_Stack_Ptr =        (VOID *) a3;
;    TMD_HISR_Stack_Size =       TIMER_SIZE;
;    TMD_HISR_Priority =         TIMER_PRIORITY;
;
; TMD_HISR_Stack_Ptr points at the top (the lowest address) of the allocated
; area. The Timer HISR (called "SYSTEM H") and its related stack will be created
; in TMI_Initialize(). The current stack pointer will be set at the bottom (the
; lowest address) of the expected area.

        LDR     a4,HISR_Stack_Ptr  	        ; Pickup variable's address
        ADD     a3,a3,#4                    ; Increment to next available word
        STR     a3,[a4, #0]                 ; Setup timer HISR stack pointer
        MOV     a2,#TIMER_SIZE              ; Pickup the timer HISR stack size
        BIC     a2,a2,#3                    ; Insure word alignment
        ADD     a3,a3,a2                    ; Allocate the timer HISR stack 
                                            ;   from available memory
        LDR     a4,HISR_Stack_Size    	    ; Pickup variable's address
        STR     a2,[a4, #0]                 ; Setup timer HISR stack size
        MOV     a2,#TIMER_PRIORITY          ; Pickup timer HISR priority (0-2)
        LDR     a4,HISR_Priority            ; Pickup variable's address
        STR     a2,[a4, #0]                 ; Setup timer HISR priority

  .elseif (GSM_IDLE_RAM=1) & (OP_L1_STANDALONE=1)
        ;
        ;  Setup of IRQ stack pointer
        ;

        LDR     a1, irq_stack_buf_size      ; Get address of variable containing stack size in a1
        LDR     a1, [a1, #0]                ; Get content of variable
        SUB     a1, a1, #4                  ; Substract 4 to get last available address of stack
        LDR     a2, irq_stack_buf           ; Get IRQ stack buffer address in a2
        ADD     a2, a2, a1                  ; Add a1 and a2 to get last address of stack
        MRS     a1,CPSR                     ; Pickup current CPSR
        BIC     a1,a1,#MODE_MASK            ; Clear the mode bits
        ORR     a1,a1,#IRQ_MODE             ; Set the IRQ mode bits
        MSR     CPSR,a1                     ; Move to IRQ mode
        MOV     sp,a2                       ; Setup IRQ stack pointer

        ;
        ;  Setup of FIQ stack pointer
        ;
        LDR     a1, fiq_stack_buf_size      ; Get FIQ stack size in a1
        LDR     a1, [a1, #0]                ; Get content of variable
        SUB     a1, a1, #4                  ; Substract 4 to get last available address of stack
        LDR     a2, fiq_stack_buf           ; Get FIQ stack buffer address in a2
        ADD     a2, a2, a1                  ; Add a1 and a2 to get last address of stack
        MRS     a1,CPSR                     ; Pickup current CPSR
        BIC     a1,a1,#MODE_MASK            ; Clear the mode bits
        ORR     a1,a1,#FIQ_MODE             ; Set the FIQ mode bits
        MSR     CPSR,a1                     ; Move to FIQ mode
        MOV     sp,a2                       ; Setup FIQ stack pointer

        ;
        ;  Go back in supervisor mode
        ;
        MRS     a1,CPSR                     ; Pickup current CPSR
        BIC     a1,a1,#MODE_MASK            ; Clear mode bits
        ORR     a1,a1,#SUP_MODE             ; Set the supervisor mode bits
        MSR     CPSR,a1                     ; All interrupt stacks are setup,
                                            ;   return to supervisor mode
        ;
        ;  Setup of SVC stack pointer
        ;
        LDR     a1, svc_stack_buf_size      ; Get address of variable containing stack size in a1
        LDR     a1, [a1, #0]                ; Get content of variable
        SUB     a1, a1, #4                  ; Substract 4 to get last available address of stack
        LDR     a2, svc_stack_buf           ; Get SVC stack buffer address in a2
        ADD     a2, a2, a1                  ; Add a1 and a2 to get last address of stack
        MOV     sp,a2                       ; Setup SVC stack pointer

        LDR     a2, svc_stack_buf           ; Get SVC stack buffer address in a2
        LDR     a4,System_Limit             ; Pickup system stack limit address
        STR     a2,[a4, #0]                 ; Save stack limit

        LDR     a4,System_Stack             ; Pickup system stack address
        STR     sp,[a4, #0]                 ; Save stack pointer

        ;
        ;  Setup of HISR TIMER stack pointer
        ;
        LDR     a4,HISR_Stack_Ptr  	    ; Pickup variable's address
        LDR     a2,timer_hisr_stack_buf  	    ; Pickup variable's address
        STR     a2,[a4, #0]                 ; Setup timer HISR stack pointer

        LDR     a4,HISR_Stack_Size    	    ; Pickup variable's address
        LDR     a2,timer_hisr_stack_buf_size  	    ; Pickup variable's address
        LDR     a2, [a2, #0]
        STR     a2,[a4, #0]                 ; Setup timer HISR stack size

        MOV     a2,#TIMER_PRIORITY          ; Pickup timer HISR priority (0-2)
        LDR     a4,HISR_Priority            ; Pickup variable's address
        STR     a2,[a4, #0]                 ; Setup timer HISR priority

  .endif ; GSM_IDLE_RAM

      ; This sequence must be always done in order to download the interrupt
      ; vector remapping
        MOV     V1, a3                      ; Save a3 register
        BL      _f_load_int_mem             ; Download FLASH to Internal RAM
        MOV     a3, V1                      ; Restore a3 register

  .if OP_L1_STANDALONE = 0
; We now fill up the System, IRQ, FIQ and System Timer HISR stacks with 0xFE for
; checking the status of the stacks later. 
; inputs:
;     a3 still has the bottom of all four stacks and is aligned. 
; algorithm:
;     We start from the top of all four stacks (*System_Limit), which is
;     necessarily aligned.  We store 0xFEFEFEFE until we have filled the
;     bottom of the fourth stack
; outputs:
;     memory has 0xFE on all four stacks: System, FIQ, IRQ and System Timer HISR
;     a3 still has the bottom of all four stacks

        LDR     a2,System_Limit             ; pickup system stack limit address
        LDR     a1,[a2]                     ; a1 = StackSegment
        MOV     a4,#0FEh                    ; use this and the next 7 instructons to set a4 = 0xFEFEFEFE
        STRB    a4,[a1, #0]
        STRB    a4,[a1, #1]
        STRB    a4,[a1, #2]
        STRB    a4,[a1, #3]
        LDR     a4,[a1],#4                  ; stored first word, move to second

fill_stack:
        STR     a4,[a1],#4                  ; store a word and increment by four
        CMP     a1,a3                       ; is this the last address?
        BLT     fill_stack                  ; if not, loop back

  .endif ;   OP_L1_STANDALONE = 0
;
;       Perform auto-initialization.  if cinit is -1, then there is none.

;  ------- OP_L1_STANDALONE = 1
;       Do not modify a3 (r2) which points to first_available_memory
;  ------- OP_L1_STANDALONE = 1
;
        LDR     r0, c_cinit
        CMN     r0, #1
        BLNE    _auto_init
       .if (TOOL_CHOICE > 1)
        
;*------------------------------------------------------
;* CALL INITIALIZATION ROUTINES FOR CONSTRUCTORS. IF
;* PINIT IS -1, THEN THERE ARE NONE.
;*------------------------------------------------------
	LDR	r5, c_pinit
	CMN	r5, #1
	BEQ	_application_
	B	_c1_
_loop_:	BL	IND_CALL
_c1_:	LDR	r4, [r5], #4
	CMP	r4, #0
	BNE	_loop_

        

_application_:
        .endif
;
;     /* Call INC_Initialize with a pointer to the first available memory 
;        address after the compiler's global data.  This memory may be used
;        by the application.  */
;     INC_Initialize(first_available_memory);
;
        MOV     a1,a3                       ; Pass the first available memory
	B       _INC_Initialize		        ; to high-level initialization
;}
;
 
;
;/*************************************************************************/
;/*                                                                       */
;/* FUNCTION                                                              */
;/*                                                                       */
;/*      INT_Vectors_Loaded                                               */
;/*                                                                       */
;/* DESCRIPTION                                                           */
;/*                                                                       */
;/*      This function returns the flag that indicates whether or not     */
;/*      all the default vectors have been loaded.  If it is false,       */
;/*      each LISR register also loads the ISR shell into the actual      */
;/*      vector table.                                                    */
;/*                                                                       */
;/* AUTHOR                                                                */
;/*                                                                       */
;/*      Barry Sellew, Accelerated Technology, Inc.                       */
;/*                                                                       */
;/* CALLED BY                                                             */
;/*                                                                       */
;/*      TCC_Register_LISR                   Register LISR for vector     */
;/*                                                                       */
;/* CALLS                                                                 */
;/*                                                                       */
;/*      None                                                             */
;/*                                                                       */
;/* INPUTS                                                                */
;/*                                                                       */
;/*      None                                                             */
;/*                                                                       */
;/* OUTPUTS                                                               */
;/*                                                                       */
;/*      None                                                             */
;/*                                                                       */
;/* HISTORY                                                               */
;/*                                                                       */
;/*         NAME            DATE                    REMARKS               */
;/*                                                                       */
;/*      B. Sellew       01-19-1996      Created initial version 1.0      */
;/*	 B. Sellew	 01-22-1996	 Verified version 1.0	  	  */
;/*                                                                       */
;/*************************************************************************/
;INT    INT_Vectors_Loaded(void)
;{
	.def	$INT_Vectors_Loaded
$INT_Vectors_Loaded			    ; Dual-state interworking veneer
	.state16
	BX	pc
	NOP
	.state32
	B	_INT_Vectors_Loaded
;
        .def    _INT_Vectors_Loaded
_INT_Vectors_Loaded
;
;    /* Just return the loaded vectors flag.  */
;    return(INT_Loaded_Flag);
;
        MOV     a1,#1                       ; Always return TRUE since there 
                                            ;   are really only two normal 
                                            ;   vectors IRQ & FIQ
        BX      lr                          ; Return to caller
;}
;
;
;/*************************************************************************/
;/*                                                                       */
;/* FUNCTION                                                              */
;/*                                                                       */
;/*      INT_Setup_Vector                                                 */
;/*                                                                       */
;/* DESCRIPTION                                                           */
;/*                                                                       */
;/*      This function sets up the specified vector with the new vector   */
;/*      value.  The previous vector value is returned to the caller.     */
;/*                                                                       */
;/* AUTHOR                                                                */
;/*                                                                       */
;/*      Barry Sellew, Accelerated Technology, Inc.                       */
;/*                                                                       */
;/* CALLED BY                                                             */
;/*                                                                       */
;/*      Application                                                      */
;/*      TCC_Register_LISR                   Register LISR for vector     */
;/*                                                                       */
;/* CALLS                                                                 */
;/*                                                                       */
;/*      None                                                             */
;/*                                                                       */
;/* INPUTS                                                                */
;/*                                                                       */
;/*      vector                              Vector number to setup       */
;/*      new                                 Pointer to new assembly      */
;/*                                            language ISR               */
;/*                                                                       */
;/* OUTPUTS                                                               */
;/*                                                                       */
;/*      old vector contents                                              */
;/*                                                                       */
;/* HISTORY                                                               */
;/*                                                                       */
;/*         NAME            DATE                    REMARKS               */
;/*                                                                       */
;/*      B. Sellew       01-19-1996      Created initial version 1.0      */
;/*	 B. Sellew	 01-22-1996	 Verified version 1.0	  	  */
;/*                                                                       */
;/*************************************************************************/
;VOID  *INT_Setup_Vector(INT vector, VOID *new)
;{
	.def	$INT_Setup_Vector
$INT_Setup_Vector			    ; Dual-state interworking veneer
	.state16
	BX	pc
	NOP
	.state32
	B	_INT_Setup_Vector
;
        .def    _INT_Setup_Vector
_INT_Setup_Vector
;
;
;     VOID    *old_vector;                        /* Old interrupt vector      */
;     VOID   **vector_table;                      /* Pointer to vector table   */
;
;    /* Calculate the starting address of the actual vector table.  */
;    vector_table =  (VOID **) 0;
;
;    /* Pickup the old interrupt vector.  */
;    old_vector =  vector_table[vector];
;    
;    /* Setup the new interrupt vector.  */
;    vector_table[vector] =  new;
;    
;    /* Return the old interrupt vector.  */
;    return(old_vector);

        MOV     a1,#0                       ; This routine is not applicable to
                                            ;   THUMB, return a NULL pointer
        BX      lr                          ; Return to caller
;}
;
;
;
;
;/*************************************************************************/
;/*                                                                       */
;/* FUNCTIONS                                                             */
;/*                                                                       */
;/*      INT_EnableIRQ, INT_DisableIRQ                                    */
;/*                                                                       */
;/* DESCRIPTION                                                           */
;/*                                                                       */
;/*      This function enable/disable IRQ/FIQ in current mode             */
;/*                                                                       */
;/*************************************************************************/
;
        .if (GSM_IDLE_RAM=1) & (OP_L1_STANDALONE=1)
        	.sect	".intram"
            .align 4
        .endif

	.align 4
        .global $INT_EnableIRQ
$INT_EnableIRQ:
	.state16
        BX      pc
	nop

	.state32
        MRS     a1, CPSR                ; read current PSR
        BIC     a1,a1,#MODE_MASK        ; remove all mode bits
        ORR     a1,a1,#IRQ_MODE         ; retrieve desired mode
        MSR     CPSR,a1                 ; IRQ mode

        MRS     a1, CPSR                ; read current PSR
        BIC     a1,a1,#LOCKOUT          ; interrupt lockout value
        MSR     CPSR,a1                 ; Lockout interrupts

        BIC     a1,a1,#MODE_MASK        ; remove all mode bits
        ORR     a1,a1,#SUP_MODE         
        MSR     CPSR,a1                 ; Lockout interrupts

	add	a1, pc, #1		; back to Thumb mode 
	bx	a1

	.state16
        BX      lr                      ; Return to caller

;
;
        .if (GSM_IDLE_RAM=1) & (OP_L1_STANDALONE=1)
            .align 4
        .endif

	.align 4
        .global $INT_DisableIRQ
$INT_DisableIRQ:
	.state16
        BX      pc
	nop

	.state32
        MRS     a1, CPSR                ; read current PSR
        BIC     a1,a1,#MODE_MASK        ; remove all mode bits
        ORR     a1,a1,#IRQ_MODE         ; retrieve desired mode
        MSR     CPSR,a1                 ; IRQ mode

        MRS     a1, CPSR                ; read current PSR
        ORR     a1,a1,#LOCKOUT          ; Build interrupt lockout value
        MSR     CPSR,a1                 ; Lockout interrupts

        BIC     a1,a1,#MODE_MASK        ; remove all mode bits
        ORR     a1,a1,#SUP_MODE         
        MSR     CPSR,a1                 ; Lockout interrupts

	add	a1, pc, #1		; back to Thumb mode 
	bx	a1

	.state16
         BX      lr                      ; Return to caller

        .if (GSM_IDLE_RAM=1) & (OP_L1_STANDALONE=1)
        	.sect       ".inttext"
            .align 4
        .endif

;
;
;/*************************************************************************/
;/*                                                                       */
;/* FUNCTION                                                              */
;/*                                                                       */
;/*      INT_Retrieve_Shell                                               */
;/*                                                                       */
;/* DESCRIPTION                                                           */
;/*                                                                       */
;/*      This function retrieves the pointer to the shell interrupt       */
;/*      service routine.  The shell interrupt service routine calls      */
;/*      the LISR dispatch routine.                                       */
;/*                                                                       */
;/* AUTHOR                                                                */
;/*                                                                       */
;/*      Barry Sellew, Accelerated Technology, Inc.                       */
;/*                                                                       */
;/* CALLED BY                                                             */
;/*                                                                       */
;/*      TCC_Register_LISR                   Register LISR for vector     */
;/*                                                                       */
;/* CALLS                                                                 */
;/*                                                                       */
;/*      None                                                             */
;/*                                                                       */
;/* INPUTS                                                                */
;/*                                                                       */
;/*      vector                              Vector number to setup       */
;/*                                                                       */
;/* OUTPUTS                                                               */
;/*                                                                       */
;/*      shell pointer                                                    */
;/*                                                                       */
;/* HISTORY                                                               */
;/*                                                                       */
;/*         NAME            DATE                    REMARKS               */
;/*                                                                       */
;/*      B. Sellew       01-19-1996      Created initial version 1.0      */
;/*	 B. Sellew	 01-22-1996	 Verified version 1.0	  	  */
;/*                                                                       */
;/*************************************************************************/
;VOID  *INT_Retrieve_Shell(INT vector)
;{
	.def	$INT_Retrieve_Shell
$INT_Retrieve_Shell			    ; Dual-state interworking veneer
	.state16
	BX	pc
	NOP
	.state32
	B	_INT_Retrieve_Shell
;
        .def    _INT_Retrieve_Shell
_INT_Retrieve_Shell
;
;    /* Return the LISR Shell interrupt routine.  */
;    return(INT_Vectors[vector]);
;
        MOV     a1,#0                       ; This routine is not applicable to
                                            ;   THUMB, return a NULL pointer
        BX      lr                          ; Return to caller
;}
;
;
;
;/* The following section contains default interrupt handlers.  */
;
  .if OP_L1_STANDALONE = 0
    .if TI_NUC_MONITOR = 1
; define a new section to be mapped independently
        .sect ".irqtext"
             
        .def    _INT_IRQ
		.global _INT_IRQ
_INT_IRQ
    .else
        .def    INT_IRQ
INT_IRQ
    .endif ;    TI_NUC_MONITOR
  .else ;   OP_L1_STANDALONE

        .def    INT_Undef_Inst
INT_Undef_Inst
        B       INT_Undef_Inst              ; Undefined Instruction
;
        .def    INT_Swi
INT_Swi
        B       INT_Swi                ; Software Generated
;
        .def    INT_Abort_Prefetch
INT_Abort_Prefetch
        B       INT_Abort_Prefetch          ; Abort Prefetch
;
        .def    INT_Abort_Data
INT_Abort_Data
        B       INT_Abort_Data              ; Abort Data
;
        .def    INT_Reserved
INT_Reserved
        B       INT_Reserved                ; Reserved
;
        .if (GSM_IDLE_RAM=1) & (OP_L1_STANDALONE=1)
        	.sect	".intram"
            .align 4
        .endif
        .def    INT_IRQ
INT_IRQ
  .endif ;   OP_L1_STANDALONE

;
;    /* Call Prepare for IRQ interrupt processing by calling 
;       TCT_Interrupt_Context_Save.  */
        STMDB   sp!,{r0-r5}                 ; Save a1-a4 on temporary IRQ stack

;BUG correction 1st part -------------------
;It looks like there is an issue with ARM7 IRQ masking in the CPSR register
;which leads to crashes in Nucleus+ scheduler.
;Basically the code below  (correct as LOCKOUT = 0xC0) is used in many places by N+ but do not 
;prevent from having an interrupt after the execution of the third line (I mean execution, not 
;fetch).
;        MRS     a1,CPSR                     ; Pickup current CPSR
;        ORR     a1,a1,#LOCKOUT              ; Build interrupt lockout value
;        MSR     CPSR,a1                     ; Lockout interrupts
;       * IRQ INTERRUPT ! *
;       Next instructions...
;
;SW workaround:
;When a task is interrupted at this point an interrupted context is stored on its task and will 
;be resumed later on at the next instruction but to make a long story short it leads to some 
;problem as the OS does not expect to be interrupted there.
;Further testing tends to show that the CPSR *seems* to be loaded with the proper masking value 
;but that the IRQ is still triggered (has been hardwarewise requested during the instruction 
;exectution by the ARM7 core?)
        MRS     a1,spsr                     ; check for the IRQ bug:
        TST     a1,#080h                    ; if the I - flag is set,
        BNE     IRQBUG                      ; then postpone execution of this IRQ
;Bug correction 1st part end ---------------
    
        SUB     r4,lr,#4                    ; Save IRQ's lr (return address)
        BL      _TCT_Interrupt_Context_Save ; Call context save routine


  .if OP_L1_STANDALONE = 0 
    .if TI_NUC_MONITOR = 1
; Log the IRQ call entry
        .global _ti_nuc_monitor_LISR_log
        BL      _ti_nuc_monitor_LISR_log		; Call the LISR Log function.
    .endif

    ; WCP Profiler - log ISR entry
    .if WCP_PROF = 1
        .global _prf_LogIsrEntry

        LDR      a1,SRC_IRQ_BIN_REG
        LDRH     a1,[a1]
        AND      a1,a1,#SRC_IRQ_BIN_MASK
        BL _prf_LogIsrEntry
    .endif ;; WCP_PROF = 1
  .endif ;; OP_L1_STANDALONE = 0

;
;    /* On actuall hardware, a register must be examined to see what the 
;       IRQ interrupt was caused from.   For default processing, the 
;       timer is the only IRQ interrupt source.  It is assumed that further
;       timer interrupts are disabled upon this call.  */
;
        BL      _f_inth_irq_handler 			; Call  int. irq service routine

  .if OP_L1_STANDALONE = 0
    .if TI_NUC_MONITOR = 1
; Log the IRQ exit
        .global _ti_nuc_monitor_LISR_log_end
        BL      _ti_nuc_monitor_LISR_log_end		; Call the LISR end function.
    .endif

    ; WCP Profiler - log ISR exit
    .if WCP_PROF = 1
        .global _prf_LogIsrExit
        BL _prf_LogIsrExit
    .endif ;; WCP_PROF = 1
  .endif ;; OP_L1_STANDALONE = 0

;
;    /* IRQ interrupt processing is complete.  Restore context- Never 
;       returns!  */
        B       _TCT_Interrupt_Context_Restore

;BUG correction 2nd part  ------------------
IRQBUG: LDMFD  sp!,{r0-r5}                  ; return from interrup. Fix for locosto reset problem. We have pushed 6 regs and popping 4. 
        SUBS   pc,r14,#4
;BUG correction 2nd part end  --------------

;
  .if OP_L1_STANDALONE = 0
    .if TI_NUC_MONITOR = 1
      .sect ".inttext"
    .endif
  .endif
;

  .if OP_L1_STANDALONE = 0
    .if TI_PROFILER = 1
; define a new section to be mapped independently
        .sect ".fiqtext"
             
        .def    _INT_FIQ
		.global _INT_FIQ
_INT_FIQ
    .else
        .def    INT_FIQ
INT_FIQ
    .endif ;   TI_PROFILER
  .else ;   OP_L1_STANDALONE = 1
        .if (GSM_IDLE_RAM=1) & (OP_L1_STANDALONE=1)
            .align 4
        .endif
        .def    INT_FIQ
INT_FIQ
  .endif

  .if OP_L1_STANDALONE = 0
    .if TI_PROFILER = 1
; Warning : 
; This code has been added for profiliing purpose.
; It removes all other FIQ. 
        .global    _ti_profiler_handler
; Timing profiler using FIQ - Handle FIQ directly here
        STMFD    sp!,{R0-R4, LR}            ; Save R0-R4 and LR on FIQ stack

        MOV     R0, LR                      ; Retrieve link register in R0
        BL      _ti_profiler_handler        ; Store into buffer
        ;/* Application f_inth_irq_handler, f_inth_fiq_handler */
        BL      _f_inth_fiq_handler                 ; Call the FIQ ISR
        LDMFD   sp!,{R0-R4, LR}            ; Restore R0-R4 and LR from FIQ stack
        SUBS    PC, LR, #4                 ; return from FIQ
    .else

;
;    /* Call Prepare for FIQ interrupt processing by calling 
;       TCT_Interrupt_Context_Save.  */
        STMDB   sp!,{r0-r5}                 ; Save r0-r5 on temporary FIQ stack
      .if WCP_PROF = 1
        ;; Disable OS scheduler if the source of FIQ is WCP profiler sampling timer
        LDR      a1,SRC_FIQ_BIN_REG
        LDRH     a1,[a1]
        AND      a1,a1,#SRC_FIQ_BIN_MASK
        CMP      a1,#MC_TIMER_FIQ_ID
        BEQ      prf_fiq
      .endif ; WCP_PROF = 1
        SUB     r4,lr,#4                    ; Save FIQ's lr (return address)        
        BL      _TCT_Interrupt_Context_Save ; Call context save routine
;
;    /* On actuall hardware, a register must be examined to see what the 
;       FIQ interrupt was caused from.   For default processing, the 
;       test is the only FIQ interrupt source.  */
;
;    /* Replace this with a call to your own ISR */
        BL      _f_inth_fiq_handler 			; Call  int. fiq service routine

;
;    /* FIQ interrupt processing is complete.  Restore context- Never 
;       returns!  */
        B       _TCT_Interrupt_Context_Restore

     .if WCP_PROF = 1
prf_fiq        ;Timing profiler using FIQ - handle FIQ directly here
        .global  _prf_LogPCSample
        STMFD    sp!,{lr}                   ; Store return addres on FIQ stack
        SUB      a1,lr,#4                   ; Retrieve link register in R0
        BL       _prf_LogPCSample           ; Store it into ring buffer
        BL       _f_inth_fiq_handler        ; Call  FIQ service routine
        LDMFD    sp!,{lr}
        LDMFD    sp!,{r0-r5}                ; Restore R0-R5 and LR from FIQ stack
        SUBS     pc,lr,#4                   ; and return from FIQ

SRC_FIQ_BIN_REG	  .word   0xFFFFFA12        ; Indicates interrupt number that requested GSM-MPU action
SRC_FIQ_BIN_MASK  .equ    0x1F              ; Valid bits for SRC_FIQ_BIN_REG
MC_TIMER_FIQ_ID   .equ    0x01              ; FIQ number used by Monte Carlo profiler 
SRC_IRQ_BIN_REG	  .word   0xFFFFFA10        ; Indicates interrupt number that requested GSM-MPU action
SRC_IRQ_BIN_MASK  .equ    0x1F              ; Valid bits for SRC_FIQ_BIN_REG
      .endif ; WCP_PROF = 1


    .endif ;   TI_PROFILER
  .else

;
;    /* Call Prepare for FIQ interrupt processing by calling 
;       TCT_Interrupt_Context_Save.  */
        STMDB   sp!,{r0-r5}                 ; Save a1-a4 on temporary FIQ stack
        SUB     r4,lr,#4                    ; Save FIQ's lr (return address)
        BL      _TCT_Interrupt_Context_Save ; Call context save routine
;
;    /* On actual hardware, a register must be examined to see what the 
;       FIQ interrupt was caused from.   For default processing, the 
;       test is the only FIQ interrupt source.  */
;
;    /* Replace this with a call to your own ISR */
        BL      _f_inth_fiq_handler 			; Call  int. fiq service routine
;
;    /* FIQ interrupt processing is complete.  Restore context- Never 
;       returns!  */
        B       _TCT_Interrupt_Context_Restore   

        .if (GSM_IDLE_RAM=1) & (OP_L1_STANDALONE=1)
        	.sect       ".inttext"
            .align 4
        .endif

  .endif ;   OP_L1_STANDALONE


  .if OP_L1_STANDALONE = 0
    .if TI_PROFILER = 1
      .sect ".inttext"
    .endif
  .endif

;***************************************************************
;* CONSTANT TABLE                                              *
;***************************************************************


  .if OP_L1_STANDALONE = 0

; internal/external RAM
    	.if L1_GPRS = 1
RAM_SIZE       .equ    0x400000         ; size (in bytes) of external RAM 
RAM_LOW        .equ    0x400000        ; first address of external RAM
     .else	; GSM ONLY
RAM_SIZE       .equ    0x50000          ; size (in bytes) of internal RAM 
RAM_LOW        .equ    0x8000000         ; first address of internal RAM
      .endif

RAM_HIGH       .equ    RAM_LOW + RAM_SIZE ; first address after internal/external RAM
               

	.global exception_stack        ; top address of SVC mode stack

	.global _xdump_buffer          ; first address of state data

	.global stack_segment		   ; address of the top of the system stack

;
; /* Define exception functions */
;


XDUMP_STACK_SIZE .equ   20

; layout of xdump buffer:
; struct xdump_s {
;     long registers[16] // svc mode registers
;     long cpsr          // svc mode CPSR
;     long exception     // magic word + index of vector taken
;     long stack[20]     // bottom 20 words of usr mode stack
; }

arm_undefined:
        stmfd   r13!,{r11,r12}		; store r12 for Xdump_buffer pointer, r11 for index
		mov     r11,#1
		b       save_regs
		
arm_swi:
        stmfd   r13!,{r11,r12}		; store r12 for Xdump_buffer pointer, r11 for index
		mov     r11,#2
		b       save_regs
        
arm_abort_prefetch:
        stmfd   r13!,{r11,r12}		; store r12 for Xdump_buffer pointer, r11 for index
		mov     r11,#3
		b       save_regs
		
        
arm_abort_data:
        stmfd   r13!,{r11,r12}		; store r12 for Xdump_buffer pointer, r11 for index
		mov     r11,#4
		b       save_regs

arm_reserved:
        ldr     r13,Exception_Stack     ; should never happen, but mode is unknown at this point
        stmfd   r13!,{r11,r12}		; store r12 for Xdump_buffer pointer, r11 for index
		mov     r11,#5
		b       save_regs
        
save_regs: 

;.if $$isdefed("ENABLE_DAR")
  .if DAR_STATE=1 | DAR_STATE=2
    .ref    _dar_exception  
 
        ldr     r12,Xdump_buffer
        str     r14,[r12,#4*15] ; save r14_abt (original PC) into r15 slot

        stmia   r12,{r0-r10}    ; save unbanked registers (except r11 and r12)
        ldmfd   r13!,{r0,r1}    ; get original r11 and r12
        str     r0,[r12,#4*11]  ; save original r11
        str     r1,[r12,#4*12]  ; save original r12
        mrs     r0,spsr         ; get original psr
        str     r0,[r12,#4*16]  ; save original cpsr

        mrs     r1,cpsr         ; save mode psr
        bic     r2,r1,#0x1f     ; psr with mode bits cleared
        and     r0,r0,#0x1f     ; get original mode bits
        add     r0,r0,r2

        msr     cpsr,r0		  ; move to pre-exception mode
        str     r13,[r12,#4*13] ; save original SP
        str     r14,[r12,#4*14] ; save original LR
        msr     cpsr,r1 	  ; restore mode psr

        ; r11 has original index
        orr     r10,r11,#0xDE<<24; r10 = 0xDEAD0000 + index of vector taken
        orr     r10,r10,#0xAD<<16
        str     r10,[r12,#4*17] ; save magic + index


        mov     r0,r11          ; put index into 1st argument
        b       _dar_exception
 .else
  b       #0x00000000 ; Branch to reset vector - This has to be modified by 
 .endif
 
  .if DAR_STATE=1 | DAR_STATE=2
	.global $exception		       ; export function

$exception: ; Veneer function
        .ref    _exception
		.state16
		adr     r0,_exception
		bx      r0
		.align
		.state32
		.def    _exception
_exception:
        ldr     r12,Xdump_buffer ; redundant unless _exception is called
        ldr     r11,[r12,#4*13]  ; get svc mode r13
        add     r12,r12,#4*18    ; base of stack buffer

        ; check if svc r13(sp) is within internal/external RAM. It *could* be invalid.
        ; we boldly assume stack is only within internal RAM except for GPRS build on 
		; Calypso chipset : stack is within external RAM
    
              
stack_range:              
        ldmfd   r11!,{r0-r9}    ; copy ten stack words..
        stmia   r12!,{r0-r9}
        ldmfd   r11!,{r0-r9}    ; copy ten stack words..
        stmia   r12!,{r0-r9}

nostack:
STACKS       .equ    SYSTEM_SIZE + IRQ_STACK_SIZE +	FIQ_STACK_SIZE + TIMER_SIZE
	.ref _dar_reset
        ; we're finished saving all state. Now execute C code for more flexibility.
        ; set up a stack for this C call

        LDR     a1,StackSegment             ; Pickup the begining address from .cmd file
                                            ;    (is aligned on 8 byte boundary)
        MOV     a2,#STACKS					; Pickup all stacks size
        ADD     a2,a2,#0x80                 ; Add 128 to get past all used data
        ADD     a3,a1,a2                    
        MOV     sp,a3                       ; Setup exception stack pointer
        b _dar_reset
 .endif
  .endif ;   OP_L1_STANDALONE = 0

;
;    /* Define all the global addresses used in this section */
;

BSS_Start
	.word	.bss
;
BSS_End
	.word	end
;
  .if (GSM_IDLE_RAM=1) & (OP_L1_STANDALONE=1)

    .ref   _irq_stack ; Stack buffer (already 32-bits aligned)
	.ref   _irq_stack_size ; Size of stack pointer in 32-bits word
    .ref   _fiq_stack ; Stack buffer (already 32-bits aligned)
	.ref   _fiq_stack_size ; Size of stack pointer in 32-bits word
    .ref   _svc_stack ; Stack buffer (already 32-bits aligned)
	.ref   _svc_stack_size ; Size of stack pointer in 32-bits word
    .ref   _timer_hisr_stack ; Stack buffer (already 32-bits aligned)
	.ref   _timer_hisr_stack_size ; Size of stack pointer in 32-bits word


irq_stack_buf .word _irq_stack
irq_stack_buf_size .word _irq_stack_size
fiq_stack_buf .word _fiq_stack
fiq_stack_buf_size .word _fiq_stack_size
svc_stack_buf .word _svc_stack
svc_stack_buf_size .word _svc_stack_size
timer_hisr_stack_buf  .word _timer_hisr_stack
timer_hisr_stack_buf_size  .word _timer_hisr_stack_size

  .endif
  .if LONG_JUMP = 3
  .align 4
BSS_IntMem_Start: .field  _S_D_Mem,32
  .align 4
BSS_IntMem_End:   .field  _E_D_Mem,32
  .endif

  .if OP_L1_STANDALONE = 0
StackSegment
	.word	stack_segment
;
  .endif

Loaded_Flag
	.word	_INT_Loaded_Flag
;
System_Limit
	.word	_TCT_System_Limit
;
System_Stack
	.word	_TCD_System_Stack
;
HISR_Stack_Ptr
	.word	_TMD_HISR_Stack_Ptr
;
HISR_Stack_Size	
	.word	_TMD_HISR_Stack_Size
;
HISR_Priority
	.word	_TMD_HISR_Priority
;
  .if OP_L1_STANDALONE = 0
Exception_Stack
    .word   exception_stack
;
Xdump_buffer
    .word   _xdump_buffer
  .endif

;
;   The following code is pulled from rts.src, which is part of the
;   TI tools installation.
;
;***************************************************************************
;*  PROCESS INITIALIZATION TABLE.
;*
;*  THE TABLE CONSISTS OF A SEQUENCE OF RECORDS OF THE FOLLOWING FORMAT:
;*                                                                          
;*       .word  <length of data (bytes)>
;*       .word  <address of variable to initialize>                         
;*       .word  <data>
;*                                                                          
;*  THE INITIALIZATION TABLE IS TERMINATED WITH A ZERO LENGTH RECORD.
;*                                                                          
;***************************************************************************
                                        
;****auto_init(register int *table)
;****{
;****   register int length;
;****   register int *addr;
;****
;****   while (length = *table++)
;****   {
;****      addr = (int *)*table++;
;****      while (length)
;****      {
;****         if (length > 3)
;****	      {
;****	         *addr++ = *table++;
;****	         length -= 4;
;****	      }
;****	      else
;****	      {
;****	         *(char *)addr++ = *(char *)table++;
;****	         length--;
;****	      }
;****      }
;****   }
;****}

tbl_addr: .set    R0
var_addr: .set    R1
length:   .set    R3
data:     .set    R4

_auto_init:
	B	rec_chk

record:
        ;*------------------------------------------------------
	;* PROCESS AN INITIALIZATION RECORD
        ;*------------------------------------------------------
	LDR	var_addr, [tbl_addr], #4

copy:
        ;*------------------------------------------------------
	;* COPY THE INITIALIZATION DATA
        ;*------------------------------------------------------
	CMP	length, #3

	LDRHI	data, [tbl_addr], #4  
	STRHI	data, [var_addr], #4	   ; COPY A WORD OF DATA
	SUBHI	length, length, #4	   ; OR ...
	LDRLSB	data, [tbl_addr], #1       ;
	STRLSB	data, [var_addr], #1       ; COPY A BYTE OF DATA
	SUBLS	length, length, #1

	CMP     length, #0		   ; CONTINUE TO COPY IF
	BNE	copy                       ; LENGTH IS NONZERO

	ANDS	length, tbl_addr, #0x3     ; MAKE SURE THE ADDRESS
	RSBNE	length, length, #0x4       ; IS WORD ALIGNED
	ADDNE	tbl_addr, tbl_addr, length ;

rec_chk:LDR	length, [tbl_addr], #4     ; PROCESS NEXT
	CMP	length, #0                 ; RECORD IF LENGTH IS
	BNE	record                     ; NONZERO

	MOV	PC, LR
;



;
; NEW COMPILER MANAGEMENT
; Creation of INT_memset and INT_memcpy, respectively identical to memset and 
; memcpy from the rts library of compiler V2.51.
; They are used to make the initialization of the .bss section and the load 
; of the internal ram code not dependent to the 32-bit alignment.
; The old code used for the initialization and the load used a loop with
; 4-byte increment, assuming the 32-bit alignment of the .bss section.
; This alignment is not true with compiler V2.51.
; This change applies whatever the compiler version.
;  
;******************************************************************************
;* INT_memset - INITIALIZE MEMORY WITH VALUE                                  *
;******************************************************************************
;* MEMSET32.ASM  - 32 BIT STATE -  v2.51                                      *
;* Copyright (c) 1996-2003 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* INT_memset - INITIALIZE MEMORY WITH VALUE.
;*
;*  Same memset defined in rts.src.
;*  Used in INT_Initialize to clear bss area.
;*  Used in f_load_int_mem() function to clear internal memory space used
;*  for data and code.
;*  The memset function defined in rts library is loaded into internal memory,
;*  then, it can not be used in either INT_Initialize, or f_load_int_mem().
;*
;*  C Prototype   : void *INT_memset(void *s, int c, size_t n);
;*  C++ Prototype : void *std::INT_memset(void *s, int c, std::size_t n);
;*
;****************************************************************************
;*
;*   o DESTINATION LOCATION IS IN r0
;*   o INITIALIZATION VALUE IS IN r1
;*   o NUMBER OF BYTES TO INITIALIZE IS IN r2
;*
;*   o ORIGINAL DESTINATION LOCATION RETURNED IN r0
;****************************************************************************

	.state32
  .def _INT_memset
_INT_memset:
	STMFD	SP!, {LR}		      ; save LR for return address
    MOV LR,PC   	     	   	  ;emulate BL
    LDR  PC, ROM_MEMSET
	LDMFD SP!, {LR}
	MOV PC, LR
	

_INT_memset_Old:
	STMFD	SP!, {R0, LR}		      ; save R0 also since original dst
					                    ; address is returned.

	TST	R0, #3			            ; check for word alignment
	BEQ	_word_aligned

	CMP	R2, #0			            ; set bytes until there are no more
					                    ; to set or until address is aligned
_unaligned_loop:
	STRHIB	R1, [R0], #1
	SUBHIS	R2, R2, #1
	TSTHI	R0, #3
	BNE	_unaligned_loop
          
	CMP	R2, #0			            ; return early if no more bytes
	LDMEQFD	SP!, {R0, PC}		    ; to set.

_word_aligned:
	AND	R1, R1, #255		        ; be safe since prototype has value as
					                    ; as an int rather than unsigned char

	ORR	R1, R1, R1, LSL	#8      ; replicate byte in 2nd byte of
					                    ; register

	CMP	R2,#4			              ; are at least 4 bytes being set
	BCC	_INT_memset3

	ORR	R1, R1, R1, LSL	#16	    ; replicate byte in upper 2 bytes
					                    ; of register. note that each of
					                    ; the bottom 2 bytes already contain 
					                    ; the byte value from above.

	CMP	R2,#8	              		; are at least 8 bytes being set
	BCC	_INT_memset7
        
	MOV	LR,R1		              	; copy bits into another register so
					                    ; 8 bytes at a time can be copied.
				                    	; use LR since it is already being
				                    	; saved/restored.

	CMP	R2,#16		            	; are at least 16 bytes being set
	BCC	_INT_memset15

	STMFD	SP!, {R4}		          ; save regs needed by 16 byte copies

	MOV	R4, R1		            	; copy bits into 2 other registers so
	MOV	R12, R1		            	; 16 bytes at a time can be copied

	SUB	R3, R2, #15	          	; set up loop count
	AND	R2, R2, #15		          ; determine number of bytes to set
					                    ; after setting 16 byte blocks

_INT_memset16_loop:			    	; set blocks of 16 bytes
	STMIA	R0!, {R1, R4, R12, LR}
	SUBS	R3, R3, #16
	BHI	_INT_memset16_loop

	LDMFD	SP!, {R4}	          	; resotre regs used by 16 byte copies

_INT_memset15:			         	; may still be as many as 15 bytes to 
					                    ; set. the address in R0 is guaranteed
					                    ; to be word aligned here.

	TST	R2, #8		            	; are at least 8 bytes being set
	STMNEIA	R0!, {R1, LR}


_INT_memset7:		          		; may still be as many as 7 bytes to 
					                    ; set. the address in R0 is guaranteed
					                    ; to be word aligned here.

	TST	R2, #4			            ; are at least 4 bytes being set
	STRNE	R1, [R0], #4

_INT_memset3:	          			; may still be as many as 3 bytes to 
				                    	; set. the address in R0 is guaranteed
				                    	; to be word aligned here.

	TST	R2, #2		            	; are there at least 2 more bytes to 
	STRNEH	R1, [R0], #2	     	; set.  the address in R0 is guaranteed
				                    	; to be half-word aligned here.
	
	TST	R2, #1		            	; is there one remaining byte to set
	STRNEB	R1, [R0]


	LDMFD     SP!, {R0, PC}		  ; restore regs and return

;******************************************************************************
;* INT_memcpy - COPY CHARACTERS FROM SOURCE TO DEST                           *
;******************************************************************************
;* MEMCPY32.ASM  - 32 BIT STATE -  v2.51                                      *
;* Copyright (c) 1996-2003 Texas Instruments Incorporated                     *
;******************************************************************************
 
;****************************************************************************
;* INT_memcpy - COPY CHARACTERS FROM SOURCE TO DEST
;*
;*  Same as C_MEMCPY defined in rts.src.
;*  Used in INT_Initialize to download code into internal memory space.
;*  The memcpy function defined in rts library is loaded into internal memory.
;*  then, it can not be used in f_load_int_mem().
;*
;****************************************************************************
;*
;*   o DESTINATION LOCATION IS IN r0
;*   o SOURCE LOCATION IS IN r1
;*   o NUMBER OF CHARACTERS TO BE COPIED IS IN r2
;****************************************************************************

 	.state32
  .def _INT_memcpy
_INT_memcpy:
  	STMFD	SP!, {LR}		      ; save LR
    MOV LR,PC   	    		   ;emulate BL
    LDR  PC, ROM_MEMCPY
	LDMFD SP!, {LR}
	MOV PC, LR

_INT_memcpy_Old: 
	CMP	r2, #0			                    ; CHECK FOR n == 0
	BXEQ	lr			;

	STMFD	sp!, {r0, lr}		              ; SAVE RETURN VALUE AND ADDRESS

	TST	r1, #0x3		                    ; CHECK ADDRESS ALIGNMENT
	BNE	_unaln			                    ; IF NOT WORD ALIGNED, HANDLE SPECIALLY
	TST	r0, #0x3		                    ;
	BNE	_saln			;

_aln:	CMP	r2, #16			                ; CHECK FOR n >= 16
	BCC	_l16			;

	STMFD	sp!, {r4}		;
	SUB	r2, r2, #16		;
_c16:	LDMIA	r1!, {r3, r4, r12, lr}	  ; COPY 16 BYTES
	STMIA	r0!, {r3, r4, r12, lr}	;
	SUBS	r2, r2, #16		;
	BCS	_c16			;
	LDMFD	sp!, {r4}		;
	ADDS	r2, r2, #16		                ; RETURN IF DONE
	LDMEQFD	sp!, {r0, pc}		;
	
_l16:	ANDS	r3, r2, #0xC		;
	BEQ	_cp1			;
	BICS	r2, r2, #0xC		;
	ADR	r12, _4line - 16	;
	ADD	pc, r12, r3, LSL #2	;

_4line:	LDR	r3, [r1], #4		          ; COPY 4 BYTES
	STR	r3, [r0], #4		;
	LDMEQFD	sp!, {r0, pc} 	          	; CHECK FOR n == 0
	B	_cp1			;

	LDMIA	r1!, {r3, r12}		            ; COPY 8 BYTES
	STMIA	r0!, {r3, r12}		;
	LDMEQFD	sp!, {r0, pc} 		          ; CHECK FOR n == 0
	B	_cp1			;

	LDMIA	r1!, {r3, r12, lr}	          ; COPY 12 BYTES
	STMIA	r0!, {r3, r12, lr}	;
	LDMEQFD	sp!, {r0, pc} 		          ; CHECK FOR n == 0

_cp1:	SUBS	r2, r2, #1		;
	ADRNE	r3, _1line - 4		            ; SETUP TO COPY 1 - 3 BYTES...
	ADDNE	pc, r3, r2, LSL #4	;

_1line:	LDRB	r3, [r1], #1		        ; COPY 1 BYTE
	STRB	r3, [r0], #1		;
	LDMFD	sp!, {r0, pc}		;

	LDRH	r3, [r1], #2		              ; COPY 2 BYTES
	STRH	r3, [r0], #2		;
	LDMFD	sp!, {r0, pc}		;
	NOP				;

	LDRH	r3, [r1], #2		              ; COPY 3 BYTES
	STRH	r3, [r0], #2		;
	LDRB	r3, [r1], #1		;
	STRB	r3, [r0], #1		;
	LDMFD	sp!, {r0, pc}		;

_unaln:	LDRB	r3, [r1], #1		        ; THE ADDRESSES ARE NOT WORD ALIGNED.
	STRB	r3, [r0], #1		              ; COPY BYTES UNTIL THE SOURCE IS
	SUBS	r2, r2, #1		                ; WORD ALIGNED OR THE COPY SIZE
	LDMEQFD	sp!, {r0, pc}		            ; BECOMES ZERO
	TST	r1, #0x3		;
	BNE	_unaln			;

_saln:	TST	r0, #0x1		              ; IF THE ADDRESSES ARE OFF BY 1 BYTE
	BNE	_off1			                      ; JUST BYTE COPY

	TST	r0, #0x2	                    	; IF THE ADDRESSES ARE NOW WORD ALIGNED
	BEQ	_aln			                      ; GO COPY.  ELSE THEY ARE OFF BY 2, SO
					                            ; GO SHORT WORD COPY

_off2:	SUBS	r2, r2, #4		          ; COPY 2 BYTES AT A TIME...
	BCC	_c1h			;
_c2:	LDR	r3, [r1], #4		            ; START BY COPYING CHUNKS OF 4,
	.if	.TMS470_BIG
	STRH	r3, [r0, #2]		;
	MOV	r3, r3, LSR #16		;
	STRH	r3, [r0], #4		;
	.else
	STRH	r3, [r0], #4		;
	MOV	r3, r3, LSR #16		;
	STRH	r3, [r0, #-2]		;
	.endif
	SUBS	r2, r2, #4		;
	BCS	_c2			;
	CMN	r2, #4			;
	LDMEQFD	sp!, {r0, pc}		;

_c1h:	ADDS	r2, r2, #2		            ; THEN COPY THE ODD BYTES.
	LDRCSH	r3, [r1], #2		;
	STRCSH	r3, [r0], #2		;
	SUBCS	r2, r2, #2		;
	ADDS	r2, r2, #1		;
	LDRCSB	r3, [r1], #1		;
	STRCSB	r3, [r0], #1		;
	LDMFD	sp!, {r0, pc}		;

_off1:	SUBS	r2, r2, #4		          ; COPY 1 BYTE AT A TIME...
	BCC	_c1b			;
_c1:	LDR	r3, [r1], #4		            ; START BY COPYING CHUNKS OF 4,
	.if	.TMS470_BIG
	STRB	r3, [r0, #3]		              ;
	MOV	r3, r3, LSR #8		              ;
	STRB	r3, [r0, #2]		              ;
	MOV	r3, r3, LSR #8		              ;
	STRB	r3, [r0, #1]		              ;
	MOV	r3, r3, LSR #8		              ;
	STRB	r3, [r0], #4		              ;
	.else
	STRB	r3, [r0], #4		              ;
	MOV	r3, r3, LSR #8		              ;
	STRB	r3, [r0, #-3]		              ;
	MOV	r3, r3, LSR #8		              ;
	STRB	r3, [r0, #-2]		              ;
	MOV	r3, r3, LSR #8		              ;
	STRB	r3, [r0, #-1]		              ;
	.endif
	SUBS	r2, r2, #4		;
	BCS	_c1			;

_c1b:	ADDS	r2, r2, #4	            	; THEN COPY THE ODD BYTES.
	LDMEQFD	sp!, {r0, pc}		;
_lp1:	LDRB	r3, [r1], #1		;
	STRB	r3, [r0], #1		;
	SUBS	r2, r2, #1		;
	BNE	_lp1			; 
	LDMFD	sp!, {r0, pc}		;

        .end

