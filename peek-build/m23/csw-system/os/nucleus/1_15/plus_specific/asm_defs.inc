;************************************************************************
;*                                                                       
;*               Copyright Mentor Graphics Corporation 2005              
;*                         All Rights Reserved.                          
;*                                                                       
;* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS  
;* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS   
;* SUBJECT TO LICENSE TERMS.                                             
;*                                                                       
;************************************************************************
;************************************************************************
;*                                                                       
;* FILE NAME                                         VERSION                
;*                                                                       
;*      asm_defs.inc               Nucleus PLUS - ARM926 DM320 Code Composer v. 1.15.1 
;*                                                                       
;* COMPONENT                                                             
;*                                                                       
;*      IN - Initialization                                              
;*                                                                       
;* DESCRIPTION                                                           
;*                                                                       
;*      This file contains the target processor dependent initialization 
;*      values used in int.s, tct.s, and tmt.s                      
;*                                                                       
;************************************************************************

;***********************************
;* TARGET BUILD OPTIONS            *
;***********************************

;NU_ROM_SUPPORT          .equ    0           ; Set to 1 for support to run from ROM
NU_FIQ_SUPPORT          .equ    0           ; Set to 1 to enable FIQ nested interrupt
                                            ; handling / functionality
NU_THUMB_SUPPORT        .equ    1           ; Set to 1 for thumb support
NU_PROFILE_PLUS         .equ    0           ; Set to 1 to enable Nucleus Profiler
NU_TEST2_SUPPORT        .equ    0           ; Set to 1 to run timing test 2
NU_TEST3_SUPPORT        .equ    0           ; Set to 1 to run timing test 3


;**********************************
;* NUCLEUS SYSTEM CONSTANTS       *
;**********************************

HISR_STACK_SIZE         .equ    2048        ; Define timer HISR stack size
SYSTEM_STACK_SIZE       .equ    2048        ; Define the system stack size
IRQ_STACK_SIZE          .equ    256         ; Define the IRQ stack size
FIQ_STACK_SIZE          .equ    512         ; Define the FIQ stack size
ABORT_STACK_SIZE        .equ    256         ; Define the ABORT stack size
UNDEF_STACK_SIZE        .equ    256         ; Define the UNDEF stack size
INT_STACK_SIZE          .equ    72          ; Size of interrupt stack frame (in bytes)
SOL_STACK_SIZE          .equ    48          ; Size of solicited stack frame (in bytes)
HISR_PRIORITY           .equ    2           ; Timer HISR has priority of 2 (lowest)

;***********************************
;* TARGET INITIALIZATION CONSTANTS *
;***********************************

; CPSR bit / mask constants

LOCKOUT                 .equ    0x000000C0  ; Interrupt lockout value
LOCK_MSK                .equ    0x000000C0  ; Value to mask lockout bits
MODE_MASK               .equ    0x0000001F  ; Processor Mode Mask
SUP_MODE                .equ    0x00000013  ; Supervisor Mode (SVC)
;USR_MODE                .equ    0x00000010  ; User Mode(USR)
IRQ_MODE                .equ    0x00000012  ; Interrupt Mode (IRQ)
FIQ_MODE                .equ    0x00000011  ; Fast Interrupt Mode (FIQ)
UNDEF_MODE              .equ    0x0000001B  ; Undefined Instruction Mode (UNDEF)
;ABT_MODE                .equ    0x00000017  ; Abort Instruction Mode (ABT)
;IRQ_BIT                 .equ    0x00000080  ; IRQ bit of CPSR and SPSR
;FIQ_BIT                 .equ    0x00000040  ; FIQ bit of CPSR and SPSR
THUMB_BIT               .equ    0x00000020  ; THUMB Mode bit in CPSR and SPSR


;******************************
;* NUCLEUS TIMER CONSTANTS    *
;******************************

; Define timer interrupt mode (FIQ or IRQ)

;TIMER_INTERRUPT_MODE    .equ    IRQ_MODE

; This Nucleus timer is based off of the TIMER 0 interrupt.

;CNTL_TIMER_BASE         .equ    0x00030000  ; Define base for all timer registers
;TIMER_MODE              .equ    0x02        ; Set the timer mode to free running
;TMPS0_OFFSET            .equ    0x0004      ; Timer Prescale Register Offset
;TIMER_PRESCALE          .equ    5           ; Set the prescale value
;TMVAL_OFFSET            .equ    0x0006      ; Timer 0 Value Register Offset
;LOAD_TIM_OFFSET         .equ    0x04        ; Offset of timer load register from base
;TIMER_ENABLE            .equ    0x20        ; Must set this before a timer 
                                            ; is used (pg 6-5)
;TIMER_CLOCK             .equ    27000000    ; Clock used by timer
;TIMER_TICKS_PER_SEC     .equ    100         ; Number of Timer ticks per second


; Calculate timer count value based on timer clock, timer prescale and the number
; of timer ticks per second (100 = 10 ms timer interrupt)
; The equation for this count value is:  count = (CLOCK/PRESCALE) * .010 seconds 

;TIMER_COUNT             .equ    ((TIMER_CLOCK/TIMER_PRESCALE) / TIMER_TICKS_PER_SEC)
;TIMER_LOAD_VAL          .equ    TIMER_COUNT ; 32-bit count
;TIMER_RESET             .equ    0x00000000  ; Timer reset value

; Timer interrupt bits
;TIMER_IRQ               .equ    30          ; Timer IRQ interrupt bit
;TIMER_MASK              .equ    0x40000000  ; Timer mask
;TIMER_ILR               .equ    0x0000      ; Timer interrupt level register

;ENABLE_TIMER_CLK        .equ    0x80        ; Enables clock of MPU timer connected to TIPB

; Clock Controller Bits

;CLOCK_PLLA_VAL          .equ    0x00C1      ; PLLA: 0x00C1 -> 27 * 13 / 2 = 175.5 MHz 
;CLOCK_PLLB_VAL          .equ    0x0071      ; PLLB: 0x0071 -> 27 * 8 / 2 = 108 MHz 
;CLOCK_SEL2_VAL          .equ    0x0101      ; SEL2: 0x0101 -> AXL, DSP = PLLA, SDR, ARM = PLLB 
;CLOCK_BYP_VAL           .equ    0x0000      ; 0x0000 -> No PLL Bypass 
;CLOCK_DIV0_VAL          .equ    0x0003      ; 0x0100 -> ARM clock is PLLB and AHB = ARM / 2 
;CLOCK_DIV1_VAL          .equ    0x0300      ; 0x0000 -> SDRAM clock = PLLB, AXL clock = PLLA 
;CLOCK_DIV2_VAL          .equ    0x0300      ; 0x0100 -> DSP clock = PLLA/2 
;CLOCK_MOD0_VAL          .equ    0x0DF7      ; Enable all except EXHOST and ETM 
;CLOCK_MOD1_VAL          .equ    0x0FFF      ; Enable all clocks 
;CLOCK_MOD2_VAL          .equ    0x7FFF      ; Enable all clocks except TEST 
;CLOCK_CTL_BASE          .equ    0x00030880  ; Clock Controller base address

; Clock Controller Register Offsets

;CLOCK_PLLA              .equ    0x00        ; PLLA Offset of Clock Control Register
;CLOCK_PLLB              .equ    0x02        ; PLLB Offset of Clock Control Register
;CLOCK_CTL               .equ    0x04        ; Clock Control Register Offset
;CLOCK_SEL2              .equ    0x08        ; Clock Select 2 Register Offset
;CLOCK_DIV0              .equ    0x0A        ; Clock 0 Divider Register Offset
;CLOCK_DIV1              .equ    0x0C        ; Clock 1 Divider Register Offset
;CLOCK_DIV2              .equ    0x0E        ; Clock 2 Divider Register Offset
;CLOCK_BYPASS            .equ    0x14        ; Clock Bypass Register Offset
;CLOCK_MOD0              .equ    0x18        ; Clock Mod0 Register Offset
;CLOCK_MOD1              .equ    0x1A        ; Clock Mod1 Register Offset
;CLOCK_MOD2              .equ    0x1C        ; Clock Mod2 Register Offset

;CLOCK_CTL_TIMER0        .equ    0x02

;TIMER0_ARM              .equ    0x20        ; Timer 0 bit

;GIO_REGISTER_BASE       .equ    0x00030580  ; GIO Register Base address


;***********************************
;* NUCLEUS INTERRUPT CONSTANTS     *
;***********************************

; The following are the vector numbers associated with each source.

;INT_IRQ0_TIMER0         .equ    0           ; Timer 0 Interrupt
;INT_IRQ1_TIMER1         .equ    1           ; Timer 1 Interrupt           
;INT_IRQ2_TIMER2         .equ    2           ; Timer 2 Interrupt
;INT_IRQ3_TIMER3         .equ    3           ; Timer 3 Interrupt
;INT_IRQ4_CCDVD0			.equ    4           ; VDINT0 register (CCD Controller)
;INT_IRQ5_CCDVD1			.equ    5           ; VDINT1 register (CCD Controller)
;INT_IRQ6_CCDWEN         .equ    6           ; CCD WEN Interrupt
;INT_IRQ7_VENC           .equ    7           ; VENC Interrupt
;INT_IRQ8_SERIAL0        .equ    8           ; Serial 0 Interrupt
;INT_IRQ9_SERIAL1        .equ    9           ; Serial 1 Interrupt
;INT_IRQ10_EXTERNAL_HOST .equ    10          ; External Host Interrupt
;INT_IRQ11_IMGBUD        .equ    11          ; Image Buffer Interrupt
;INT_IRQ12_UART0         .equ    12          ; UART 0 Interrupt
;INT_IRQ13_UART1         .equ    13          ; UART 1 Interrupt
;INT_IRQ14_USB0          .equ    14          ; USB 0 Interrupt
;INT_IRQ15_USB1          .equ    15          ; USB 1 Interrupt
;INT_IRQ16_VLYNQ         .equ    16          ; VLYNQ Interrupt
;INT_IRQ17_MTC0          .equ    17          ; MTC 0 Interrupt
;INT_IRQ18_MTC1          .equ    18          ; MTC 1 Interrupt
;INT_IRQ19_SDMMCMS       .equ    19          ; SD/MMC or MS Host Interrupt
;INT_IRQ20_SDIOMS		.equ	20			; SDIO or MS DMA Interrupt
;INT_IRQ21_GIO_0         .equ    21          ; GPIO 0 Interrupt
;INT_IRQ22_GIO_1         .equ    22          ; GPIO 1 Interrupt
;INT_IRQ23_GIO_2         .equ    23          ; GPIO 2 Interrupt
;INT_IRQ24_GIO_3         .equ    24          ; GPIO 3 Interrupt
;INT_IRQ25_GIO_4         .equ    25          ; GPIO 4 Interrupt
;INT_IRQ26_GIO_5         .equ    26          ; GPIO 5 Interrupt
;INT_IRQ27_GIO_6         .equ    27          ; GPIO 6 Interrupt
;INT_IRQ28_GIO_7         .equ    28          ; GPIO 7 Interrupt
;INT_IRQ29_GIO_8         .equ    29          ; GPIO 8 Interrupt
;INT_IRQ30_GIO_9         .equ    30          ; GPIO 9 Interrupt
;INT_IRQ31_GIO_10        .equ    31          ; GPIO 10 Interrupt
;INT_IRQ32_GIO_11        .equ    32          ; GPIO 11 Interrupt
;INT_IRQ33_GIO_12        .equ    33          ; GPIO 12 Interrupt           
;INT_IRQ34_GIO_13        .equ    34          ; GPIO 13 Interrupt
;INT_IRQ35_GIO_14        .equ    35          ; GPIO 14 Interrupt
;INT_IRQ36_GIO_15        .equ    36          ; GPIO 15 Interrupt
;INT_IRQ37_PREV0         .equ    37          ; Preview 0 Interrupt
;INT_IRQ38_PREV1         .equ    38          ; Preview 1 Interrupt
;INT_IRQ39_WDT           .equ    39          ; Watchdog Timer Interrupt
;INT_IRQ40_I2C           .equ    40          ; I2C Interrupt
;INT_IRQ41_CLKC          .equ    41          ; CLKC Interrupt
;INT_IRQ42_E2ICE			.equ	42			; Embedded ICE Interrupt
;INT_IRQ43_ARMCOMRX		.equ    43			; ARM COMM Receive
;INT_IRQ44_ARMCOMTX		.equ    44			; ARM COMM Transmit
;INT_IRQ45_RESERVED      .equ    45          ; Reserved


;***********************************
;* TARGET INTERRUPT CONSTANTS      *
;***********************************    

; The following can be found in the MPU Private Peripherals Chapter under
; interrupt handler level 1 and level 2 registers

INT_BASE_ADDRESS        .equ    0x00030500  ; Interrupt base address
;INT_FIQ0                .equ    0x00        ; Base address for FIQ handler 0
;INT_FIQ1                .equ    0x02        ; Base address for FIQ handler 1
;INT_FIQ2                .equ    0x04        ; Base address for FIQ handler 2
;INT_IRQ0                .equ    0x08        ; IRQ 0 pending interrupt register
;INT_IRQ1                .equ    0x0A        ; IRQ 1 pending interrupt register
;INT_IRQ2                .equ    0x0C        ; IRQ 2 pending interrupt register
;INT_FIQ_ENTRY0          .equ    0x10        ; Lowest 16 bits of entry address to highest priority reg
;INT_FIQ_ENTRY1          .equ    0x12        ; Highest 16 bits of entry address to highest priority reg
;INT_IRQ_ENTRY0          .equ    0x18        ; Lowest 16 bits of entry address to highest priority reg
;INT_IRQ_ENTRY1          .equ    0x1A        ; Highest 16 bits of entry address to highest priority reg
;INT_FISEL0              .equ    0x20        ; Base address for interrupt select register
;INT_FISEL1              .equ    0x22        ; Base address for interrupt select register
;INT_FISEL2              .equ    0x24        ; Base address for interrupt select register
;INT_EINT0               .equ    0x28        ; Enable interrupt bank 0 register
;INT_EINT1               .equ    0x2A        ; Enable interrupt bank 1 register
;INT_EINT2               .equ    0x2C        ; Enable interrupt bank 2 register
;INT_RAW                 .equ    0x30        ; Make sure the interrupt we get is IRQ
;INTPRIOR0_OFFSET        .equ    0x40        ; Interrupt Priority Offset

;INT_NUM_PRIOR_REGS      .equ    22          ; Number of Interrupt Priority Registers

;INTERRUPT_MODE          .equ    0x0000      ; Sets the interrupt mode to IRQ
;INTERRUPTS_DISABLE      .equ    0xFFFF      ; Make sure all interrupts are disabled

;INT_EN_TIMER            .equ    0x01        ; Enables Timer 0
;IRQ_ENTRY               .equ    0x02        ; Set interrupts to IRQ

;INT_NO_PENDING          .equ    0x0         ; Value returned by offset register for no pending
                                            ; interrupts
;MASK_REG_SPACE          .equ    8

;MASK1_STACK_OFFSET      .equ    24          ; Level 1 mask register
;MASK2_STACK_OFFSET      .equ    28          ; Level 2 mask register
;MASK3_STACK_OFFSET      .equ    32          ; Level 3 mask register

;MASK_REG1_VECT          .equ    32          ; Max vector number (combining interrupt bank 0 and 1
                                            ; registers)

;INTERRUPTS_ADDR         .equ    0x00000000  ; Begining address for all handlers


;***********************************
;* TARGET INITIALIZATION CONSTANTS *
;***********************************

;EMI_BASE                .equ    0x00030A00  ; External memory interface base address
;CS0_CNTL1               .equ    0x00        ; Chip select 0 control 1 offset
;CS0_CNTL2               .equ    0x02        ; Chip select 0 control 2 offset
;CS1_CNTL1A              .equ    0x04        ; Chip select 1 control 1A offset
;CS1_CNTL1B              .equ    0x06        ; Chip select 1 control 1B offset
;CS1_CNTL2               .equ    0x08        ; Chip select 1 control 2 offset
;CS2_CNTL1               .equ    0x0A        ; Chip select 2 control 1 offset
;CS2_CNTL2               .equ    0x0C        ; Chip select 2 control 2 offset
;CS3_CNTL1               .equ    0x0E        ; Chip select 3 control 1 offset
;CS3_CNTL2               .equ    0x10        ; Chip select 3 control 2 offset
;CS4_CNTL1               .equ    0x12        ; Chip select 4 control 1 offset
;CS4_CNTL2               .equ    0x14        ; Chip select 4 control 2 offset

;CS0_CNTL1_BITS          .equ    0x4455      ; Bits for chip select 0 control 1 register
;CS0_CNTL2_BITS          .equ    0x0012      ; Bits for chip select 0 control 2 register
;CS1_CNTL1A_BITS         .equ    0x1E1F      ; Bits for chip select 1 control 1A register
;CS1_CNTL1B_BITS         .equ    0x1D1D      ; Bits for chip select 1 control 1B register
;CS3_CNTL1_BITS          .equ    0x5557      ; Bits for chip select 3 control 1 register
;CS3_CNTL2_BITS          .equ    0x1222      ; Bits for chip select 3 control 2 register
;CS4_CNTL1_BITS          .equ    0xCFFF      ; Bits for chip select 3 control 1 register
;CS4_CNTL2_BITS          .equ    0x7330      ; Bits for chip select 4 control 2 register

; Defines for the SDRAM Controller

;SDRAM_BASE              .equ    0x00030980  ; SDRAM Controller Base Address
;SDRAM_SDMODE            .equ    0x26        ; SDRAM SDMODE offset
;SDRAM_REFCTL            .equ    0x28        ; SDRAM Refresh Control offset
;SDRAM_PRTYON            .equ    0x4E        ; SDRAM Priority ON/OFF offset
;SDRAM_SDPRY1            .equ    0x3A        ; SDRAM Priority 1 offset
;SDRAM_SDPRY2            .equ    0x3C        ; SDRAM Priority 2 offset
;SDRAM_SDPRY3            .equ    0x3E        ; SDRAM Priority 3 offset
;SDRAM_SDPRY4            .equ    0x40        ; SDRAM Priority 4 offset
;SDRAM_SDPRY5            .equ    0x42        ; SDRAM Priority 5 offset
;SDRAM_SDPRY6            .equ    0x44        ; SDRAM Priority 6 offset
;SDRAM_SDPRY7            .equ    0x46        ; SDRAM Priority 7 offset
;SDRAM_SDPRY8            .equ    0x48        ; SDRAM Priority 8 offset
;SDRAM_SDPRY9            .equ    0x4A        ; SDRAM Priority 9 offset
;SDRAM_SDPRY10           .equ    0x4C        ; SDRAM Priority 10 offset
;SDMODE_VAL              .equ    0xB280      ; SDRAM config - DQM Control = force '1'
;REFCTL_VAL              .equ    0x0140      ; Refresh command sent after every 145*8 SDRAM clock cycles

;SDMODE_NOP              .equ    0x0000      ; SDRAM initialization - NOP 
;SDMODE_MSR              .equ    0x0001      ; SDRAM initialization - Mode Status Register Set 
;SDMODE_PREA             .equ    0x0002      ; SDRAM initialization - Precharge All 
;SDMODE_AUTOREF          .equ    0x0004      ; SDRAM initialization - AutoRefresh 
;SDMODE_AUTOPDN          .equ    0x0040      ; SDRAM initialization - Auto PowerDown 
;SDMODE_EMRS             .equ    0x0011      ; SDRAM initialization - Extended Mode Register Set 
;SDMODE_DSNORM           .equ    0x0011      ; SDRAM initialization - Drive Strength = Normal 
;SDMODE_DSHALF           .equ    0x0111      ; SDRAM initialization - Drive Strength = 1/2 
;SDMODE_DSQUART          .equ    0x0211      ; SDRAM initialization - Drive Strength = 1/4 
  
;SDMODE_VALUE32          .equ    0x0000      ; SDRAM initialization for MSR for 32 bit mode and CAS latency of 2 cycles
;SDMODE_VALUE16          .equ    0x2000      ; SDRAM initialization for MSR for 16 bit mode and CAS latency of 2 cycles
;SDMODE_DQM              .equ    0x0080      ; SDRAM config - DQM Control bit 

;SDPRY1_VALUE            .equ    0x0100      ; Set the CCD as highest priority
;SDPRY2_VALUE            .equ    0x0040      ; Set the priority of the preview engine
;SDPRY3_VALUE            .equ    0x0001      ; Set hardware engine to lowest priority
;SDPRY4_VALUE            .equ    0x0100      ; Set the priority of the OSD
;SDPRY5_VALUE            .equ    0x0002      ; Set the priority of the external host request
;SDPRY6_VALUE            .equ    0x0004      ; Set the priority of the ARM CPU request
;SDPRY7_VALUE            .equ    0x0010      ; Set the priority of the External Memory SDRAM DMA1
;SDPRY8_VALUE            .equ    0x0008      ; Set the priority of the External Memory SDRAM DMA2
;SDPRY9_VALUE            .equ    0x0020      ; Set the priority of the Image buffer DMA request
;SDPRY10_VALUE           .equ    0x0080      ; Set the priority of SDRAM refresh

;PRIORITY_ON             .equ    0x1         ; Turn priority on
 
; SDRAM End address configuration
 
;DM320_AHB_SDRAMEA       .equ    0x60F04  
;DM320_AHB_SDRAMEA_VALUE .equ    0x2900000   ; 32 MB SDRAM - 0x900000-0x2900000 


;***********************************
;*      MACROS                     *
;***********************************

    ; This macro is used to unnest interrupts.  It switches to the correct
    ; interrupt mode (FIQ/IRQ), removes the interrupt controller mask registers
    ; off the interrupt stack, restores the interrupt controller mask values,
    ; and returns back to the mode it was in before invoking the macro.
    
;UNNEST_INTERRUPT .macro INT_MODE
    
    ; Switch to correct mode

;    MRS     r0,CPSR                         ; Pickup current CPSR
;    ORR     r0,r0,#LOCKOUT                  ; Lock-out interrupts
;    BIC     r1,r0,#MODE_MASK                ; Clear the mode bits (preserve
                                            ; current mode in r0)
;    ORR     r1,r1,#INT_MODE                 ; Set the new mode bits
;    MSR     CPSR,r1                         ; Switch to correct mode

    ; ************ START BOARD SPECIFIC SECTION **************
    
    ; Get interrupt masks from stack

;    LDMIA   sp!,{r1-r2}                     ; Get Level 1\2 mask registers
                                            
    ; Update mask registers                 
                                            
;    LDR     r3,INT_BASE_ADDRESS1            ; Get base address of interrupt registers
;    STRH    r1,[r3,#INT_EINT0]              ; Store value of Level 0 mask register
;    MOV     r1,r1,LSR #16
;    STRH    r1,[r3,#INT_EINT1]              ; Store value of Level 1 mask register
;    STRH    r2,[r3,#INT_EINT2]              ; Store value of Level 2 mask register

    ; ************ END BOARD SPECIFIC SECTION **************

    ; Return back to correct original mode with interrupts locked-out

;    MSR     CPSR,r0

    ; UNNEST_INTERRUPT END

;    .endm    


    ; This macro is used to unnest minimal context save interrupts.
    ; It removes the interrupt controller mask registers from the stack frame,
    ; restores the interrupt mask register values, restores the minimally
    ; saved registers (saved in INT_IRQ or INT_FIQ) and adjusts the
    ; stack pointer to its pre-interrupt state.
    
;UNNEST_MIN_INTERRUPT .macro    

    ; ************ START BOARD SPECIFIC SECTION **************
    
    ; Get interrupt masks from stack

;    LDR     r0,[sp,#MASK1_STACK_OFFSET]
;    LDR     r1,[sp,#MASK2_STACK_OFFSET]

    ; Update mask registers

;    LDR     r3,INT_BASE_ADDRESS1            ; Get base address of interrupt registers
;    STRH    r0,[r3,#INT_EINT0]              ; Store value of Level 0 mask register
;    MOV     r0,r0,LSR #16
;    STRH    r0,[r3,#INT_EINT1]              ; Store value of Level 1 mask register
;    STRH    r1,[r3,#INT_EINT2]              ; Store value of Level 2 mask register


    ; ************ END BOARD SPECIFIC SECTION **************

    ; Restore registers r0-r5 from stack

;    LDMIA   sp!,{r0-r5}

    ; Adjust stack to remove space saved
    ; for mask registers

;    ADD     sp,sp, #MASK_REG_SPACE

    ; UNNEST_MIN_INTERRUPT END

;    .endm    

;********************************************
;*  TC_TCB and TC_HCB STRUCT OFFSET DEFINES *
;********************************************
; Task / HISR control block offsets

TC_CREATED              .equ     0x00       ; Node for linking to created task list
TC_ID                   .equ     0x0C       ; Internal TCB ID
TC_NAME                 .equ     0x10       ; Task name
TC_STATUS               .equ     0x18       ; Task status
TC_DELAYED_SUSPEND      .equ     0x19       ; Delayed task suspension
TC_PRIORITY             .equ     0x1A       ; Task priority
TC_PREEMPTION           .equ     0x1B       ; Task preemption enable
TC_SCHEDULED            .equ     0x1C       ; Task scheduled count
TC_CUR_TIME_SLICE       .equ     0x20       ; Current time slice
TC_STACK_START          .equ     0x24       ; Stack starting address
TC_STACK_END            .equ     0x28       ; Stack ending address
TC_STACK_POINTER        .equ     0x2C       ; Task stack pointer
TC_STACK_SIZE           .equ     0x30       ; Task stack's size
TC_STACK_MINIMUM        .equ     0x34       ; Minimum stack size
TC_CURRENT_PROTECT      .equ     0x38       ; Current protection
TC_SAVED_STACK_PTR      .equ     0x3C       ; Previous stack pointer
TC_ACTIVE_NEXT          .equ     0x3C       ; Next activated HISR
TC_TIME_SLICE           .equ     0x40       ; Task time slice value
TC_ACTIVATION_COUNT     .equ     0x40       ; Activation counter
TC_HISR_ENTRY           .equ     0x44       ; HISR entry function
TC_HISR_SU_MODE         .equ     0x58       ; Sup/User mode indicator for HISRs
TC_HISR_MODULE          .equ     0x5C       ; Module identifier for HISR's
TC_SU_MODE              .equ     0xA8       ; Sup/User mode indicator for Tasks
TC_MODULE               .equ     0xAC       ; Module identifier for Tasks

; Priority control block offset

TC_TCB_PTR              .equ     0x00       ; Owner of protection
TC_THREAD_WAIT          .equ     0x04       ; Waiting thread flag

; End of low-level initialization constants.

; ** END ASM_DEFS.INC **


