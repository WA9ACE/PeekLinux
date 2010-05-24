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
;* FILE NAME                                           VERSION                
;*             
;*  tmt.s                         Nucleus PLUS\ARM926 DM320\Code Composer 1.15.1
;*
;*  COMPONENT
;*
;*      TM - Timer Management
;*
;*  DESCRIPTION
;*
;*      This file contains the target dependent routines of the timer
;*      management component.
;*
;*  GLOBAL FUNCTIONS
;*
;*
;*      *****************************
;*      * GENERIC FUNCTIONS         *
;*      *****************************
;*
;*      TMT_Set_Clock                   Set system clock
;*      TMT_Retrieve_Clock              Retrieve system clock
;*      TMT_Read_Timer                  Read count-down timer
;*      TMT_Enable_Timer                Enable count-down timer
;*      TMT_Adjust_Timer                Adjust count-down timer
;*      TMT_Disable_Timer               Disable count-down timer
;*      TMT_Retrieve_TS_Task            Retrieve time-sliced task ptr
;*      TMT_Timer_Interrupt             Process timer interrupt
;*
;*      *****************************
;*      * TARGET SPECIFIC FUNCTIONS *
;*      *****************************
;*
;*  LOCAL FUNCTIONS
;*
;*      None
;*
;************************************************************************

;**********************************
;* INCLUDE ASSEMBLY CONSTANTS     *
;**********************************

    .include asm_defs.inc

;**********************************
;* EXTERNAL VARIABLE DECLARATIONS *
;**********************************

;extern VOID        *TCD_Current_Thread;
;extern UNSIGNED    TMD_System_Clock;
;extern UNSIGNED    TMD_Timer;
;extern INT         TMD_Timer_State;
;extern UNSIGNED    TMD_Time_Slice;
;extern TC_TCB      *TMD_Time_Slice_Task;
;extern INT         TMD_Time_Slice_State;
;extern TC_HCB      TMD_HISR;

    .ref          _TCD_Current_Thread
    .ref          _TMD_System_Clock
    .ref          _TMD_Timer
    .ref          _TMD_Timer_State
    .ref          _TMD_Time_Slice
    .ref          _TMD_Time_Slice_Task
    .ref          _TMD_Time_Slice_State
    .ref          _TMD_HISR

;**********************************
;* EXTERNAL FUNCTION DECLARATIONS *
;**********************************

;VOID   TCT_Activate_HISR(TC_HISR *hisr);
;VOID   TCT_Interrupt_Context_Save(VOID);
;VOID   TCT_Interrupt_Context_Restore(VOID);

    .ref          _TCT_Activate_HISR
    .ref          _TCT_Interrupt_Context_Save
    .ref          _TCT_Interrupt_Context_Restore

;**********************************
;* GLOBAL VARIABLE DECLARATIONS   *
;**********************************

    ; No global variable declarations

;**********************************
;* LOCAL VARIABLES                *
;**********************************
    .text
    .align  4

    ; The following are local variables.  ARM Architecture uses
    ; PC relative addressing, so all global data accessed must
    ; reside within close proximity (<4Kb in ARM, <1Kb in THUMB)
    ; to the instructions that access them.
    ; These are essentially pointers to global data.

TMT_System_Clock
    .word     _TMD_System_Clock

TMT_Timer
    .word     _TMD_Timer

TMT_Timer_State
    .word     _TMD_Timer_State

TMT_Slice_State
    .word     _TMD_Time_Slice_State

TMT_Time_Slice
    .word     _TMD_Time_Slice

TMT_Current_Thread
    .word     _TCD_Current_Thread

TMT_Slice_Task
    .word     _TMD_Time_Slice_Task

TMT_HISR
    .word     _TMD_HISR

INT_BASE_ADDRESS1
    .word     INT_BASE_ADDRESS

;************************************************************************
;*
;*  FUNCTION
;*
;*      TMT_Set_Clock
;*
;*  DESCRIPTION
;*
;*      This function sets the system clock to the specified value.
;*
;*  CALLED BY
;*
;*      Application
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    new clock value
;*
;*  OUTPUTS
;*
;*      None
;*
;*  REGISTERS MODIFIED
;*
;*      r1
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TMT_Set_Clock(UNSIGNED new_value)

    .def    _TMT_Set_Clock
_TMT_Set_Clock

    ; Set the system clock to the specified value.

    LDR     r1,TMT_System_Clock             ; Build address of system clock
    STR     r0,[r1]                         ; Store new system clock value

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TMT_Retrieve_Clock
;*
;*  DESCRIPTION
;*
;*      This function returns the current value of the system clock.
;*
;*  CALLED BY
;*
;*      Application
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      None
;*
;*  OUTPUTS
;*
;*      r0 -    TMD_System_Clock
;*
;*  REGISTERS MODIFIED
;*
;*      None
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;UNSIGNED  TMT_Retrieve_Clock(void)

    .def    _TMT_Retrieve_Clock
_TMT_Retrieve_Clock

    ; Return the current value of the system clock.

    LDR     r0,TMT_System_Clock             ; Build address to system clock
    LDR     r0,[r0]                         ; Pickup system clock contents

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TMT_Read_Timer
;*
;*  DESCRIPTION
;*
;*      This function returns the current value of the count-down timer.
;*
;*  CALLED BY
;*
;*      TMC_Start_Timer                 Start timer function
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      None
;*
;*  OUTPUTS
;*
;*      r0 -    value of count-down timer
;*
;*  REGISTERS MODIFIED
;*
;*      r0
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;UNSIGNED  TMT_Read_Timer(void)

    .def    _TMT_Read_Timer
_TMT_Read_Timer

    ; Return the current value of the count-down timer.

    LDR     r0,TMT_Timer                    ; Build address to timer
    LDR     r0,[r0]                         ; Pickup timer contents

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TMT_Enable_Timer
;*
;*  DESCRIPTION
;*
;*      This function enables the count-down timer with the specified
;*      value.
;*
;*  CALLED BY
;*
;*      TMC_Start_Timer                 Start timer function
;*      TMC_Timer_Task                  Timer expiration task
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    New count-down time
;*
;*  OUTPUTS
;*
;*      None
;*
;*  REGISTERS MODIFIED
;*
;*      r0, r1
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TMT_Enable_Timer(UNSIGNED time)

    .def    _TMT_Enable_Timer
_TMT_Enable_Timer

    ; Place the new time value into the count-down timer.

    LDR     r1,TMT_Timer                    ; Build address of timer
    STR     r0,[r1]                         ; Store new timer value

    ; Indicate that the timer is active.

    MOV     r0,#0                           ; Build TM_ACTIVE value
    LDR     r1,TMT_Timer_State              ; Build address of timer state var
    STR     r0,[r1]                         ; Change the state to active

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TMT_Adjust_Timer
;*
;*  DESCRIPTION
;*
;*      This function adjusts the count-down timer with the specified
;*      value - if the new value is less than the current.
;*
;*  CALLED BY
;*
;*      TMC_Start_Timer                 Start timer function
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    New count-down time
;*
;*  OUTPUTS
;*
;*      None
;*
;*  REGISTERS MODIFIED
;*
;*      r1, r2, r3
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TMT_Adjust_Timer(UNSIGNED time)

    .def    _TMT_Adjust_Timer
_TMT_Adjust_Timer

    ; Lockout all interrupts

    MRS     r3,CPSR                         ; Pickup current CPSR
    ORR     r2,r3,#LOCKOUT                  ; Build lockout CPSR (r3 = orig CPSR value)
    MSR     CPSR,r2                         ; Setup new CPSR interrupt bits

    ; Check for the new value is less than the current time value

    LDR     r1,TMT_Timer                    ; Build address to timer var
    LDR     r2,[r1]                         ; read value of the timer
    CMP     r2,r0                           ; Compare timer and passed-in time

    ; Adjust timer with passed in value

    STRHI    r0,[r1]

TMT_No_Adjust

    ; Restore interrupts to entry level

    MSR     CPSR,r3

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TMT_Disable_Timer
;*
;*  DESCRIPTION
;*
;*      This function disables the count-down timer.
;*
;*  CALLED BY
;*
;*      TMC_Start_Timer                 Start timer function
;*      TMC_Timer_Task                  Timer expiration task
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      None
;*
;*  OUTPUTS
;*
;*      None
;*
;*  REGISTERS MODIFIED
;*
;*      r0, r1
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TMT_Disable_Timer(void)

    .def    _TMT_Disable_Timer
_TMT_Disable_Timer

    ; Disable the count-down timer.

    MOV     r1,#1                           ; Build TM_NOT_ACTIVE value
    LDR     r0,TMT_Timer_State              ; Build address to timer state var
    STR     r1,[r0]                         ; Change timer state to not active

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TMT_Retrieve_TS_Task
;*
;*  DESCRIPTION
;*
;*      This function returns the time-sliced task pointer.
;*
;*  CALLED BY
;*
;*      TMC_Timer_HISR                  Timer HISR
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      None
;*
;*  OUTPUTS
;*
;*      r0 -    Time sliced task pointer
;*
;*  REGISTERS MODIFIED
;*
;*      r0, r1
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;NU_TASK  *_TMT_Retrieve_TS_Task(VOID)

    .def    _TMT_Retrieve_TS_Task
_TMT_Retrieve_TS_Task

    ; Read the current TMD_Time_Slice_Task in r0 (return register)

    LDR     r1,TMT_Slice_Task               ; Build address to timer slice var
    LDR     r0,[r1]                         ; Get task pointer to be returned

    ; Return to caller

    BX      lr                              ; Return to caller


;************************************************************************
;*
;*  FUNCTION
;*
;*      TMT_Timer_Interrupt
;*
;*  DESCRIPTION
;*
;*      This function processes the actual hardware interrupt.
;*      Processing includes updating the system clock, the count-
;*      down timer and the time-slice timer.  If one or both of the
;*      timers expire, the timer HISR is activated.
;*
;*  CALLED BY
;*
;*      INT_Timer_Interrupt
;*
;*  CALLS
;*
;*      TCT_Interrupt_Context_Save      Save interrupted context
;*      TCT_Activate_HISR               Activate timer HISR
;*      TCT_Interrupt_Context_Restore   Restore interrupted context
;*
;*  INPUTS
;*
;*      None
;*
;*  OUTPUTS
;*
;*      None
;*
;*  REGISTERS MODIFIED
;*
;*      r0, r1, r2, r3, r4, Interrupt stack SP (IRQ/FIQ)
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TMT_Timer_Interrupt(void)

    .def    _TMT_Timer_Interrupt
_TMT_Timer_Interrupt

    ; Increment the system clock.

    LDR     r0,TMT_System_Clock             ; Pickup system clock address
    LDR     r1,[r0]                         ; Pickup system clock value
    ADD     r1,r1,#1                        ; Increment system clock
    STR     r1,[r0]                         ; Store new system clock value

    ; Determine if the count-down timer is active.

    LDR     r1,TMT_Timer_State              ; Build address to timer state flag
    LDR     r0,[r1]                         ; Pickup timer state
    CMP     r0,#0                           ; Is there a timer active?
    BNE     TMT_No_Timer_Active             ; No, skip timer processing

    ; Pickup the value of the timer.

    LDR     r0,TMT_Timer                    ; Build timer address
    LDR     r2,[r0]                         ; Pickup the current timer value

    ; Test if the Timer is at 0 and if so skip the decrement

    CMP     r2,#0
    BEQ     TMT_Expired

    ; Decrement the count-down timer.

    SUBS    r2,r2,#1                        ; Decrement the timer value
    STR     r2,[r0]                         ; Store the new timer value

    BNE     TMT_No_Timer_Active             ; Skip over the Set Timer State

TMT_Expired

    ; If the timer has expired, modify the state to indicate that
    ; it has expired.

    MOV     r3,#2                           ; Build expired value
    STR     r3,[r1]                         ; Change the timer state to expired

TMT_No_Timer_Active

    ; Determine if the time-slice timer is active.

    LDR     r0,TMT_Slice_State              ; Build time slice state address
    LDR     r2,[r0]                         ; Pickup time slice state
    CMP     r2,#0                           ; Is there a time slice active?
    BNE     TMT_No_Time_Slice_Active        ; No, skip time slice processing

    ; Get the time-slice timer value

    LDR     r2,TMT_Time_Slice               ; Build time slice address
    LDR     r3,[r2]                         ; Pickup the time slice value

    ; Ensure the timer is not 0 before decrementing

    CMP     r3,#0
    BEQ     TMT_Time_Slice_Expire

    ; Decrement the time-slice timer and save it

    SUBS    r3,r3,#1                        ; Decrement the time slice
    STR     r3,[r2]                         ; Store the new time slice value

    ; Has time slice expired?

    BNE     TMT_No_Time_Slice_Active

TMT_Time_Slice_Expire

    ; If the time-slice timer has expired modify the
    ; time-slice state to indicate that it has.

    MOV     r3,#2                           ; Build TM_EXPIRED value
    STR     r3,[r0]                         ; Indicate time slice is expired

    ; Copy the current thread into the time-slice task pointer.

    LDR     r2,TMT_Current_Thread           ; Pickup current thread pointer adr
    LDR     r2,[r2]                         ; Pickup current thread pointer
    LDR     r3,TMT_Slice_Task               ; Pickup time slice task pointer ad
    STR     r2,[r3]                         ; Store current thread pointer

    ; Place a minimal time slice into the task's control block.

    MOV     r3,#1                           ; For safety, place a minimal time-
    STR     r3,[r2,#TC_CUR_TIME_SLICE]      ; slice into the task's control
                                            ; block

TMT_No_Time_Slice_Active

    ; Determine if either of the basic timers have expired.  If so,
    ; activate the timer HISR.

    LDR     r1,[r1]                         ; Pickup timer state
    CMP     r1,#2                           ; Does it indicate expiration?
    LDRNE   r0,[r0]                         ; Pickup time slice state
    CMPNE   r0,#2                           ; Does it indicate expiration?
 ;;   BNE     TMT_Interrupt_Exit              ; If not expired, return to point of interrupt
    BXNE     lr                              ; If not expired, return to caller

    ; Put interrupt return address in r4

 ;;   MOV     r4,lr

    ; Do a complete context save.

 ;;   BL      _TCT_Interrupt_Context_Save

    ; Activate the HISR timer function.
    STR     lr,[sp, #-4]!                      ; Save lr on the stack
    LDR     r0,TMT_HISR                     ; Build address of timer HISR
    BL      _TCT_Activate_HISR              ; Activate timer HISR
    LDR     lr,[sp], #4                 ; Recover return address
    BX      lr                          ; Return to caller



    ; Execute macro to unmask interrupts masked during nesting process
;VENKAT: UNNESTING is not needed already taken care.
;;    UNNEST_INTERRUPT    TIMER_INTERRUPT_MODE

    ; Restore context and return to scheduler (control doesn't return here)

 ;;   B       _TCT_Interrupt_Context_Restore

;TMT_Interrupt_Exit

    ; Unnest / restore the minimal context
;VENKAT: UNNESTING is not needed already taken care.
;;    UNNEST_MIN_INTERRUPT

    ; Put return address on stack

;    STMDB   sp!,{lr}

    ; Return to point of interrupt

;    LDMIA   sp!,{pc}^



;**************************************
; Veneer for thumb mode
;**************************************
    .if NU_THUMB_SUPPORT

;**********************************
;* TMT_Set_Clock Veneer for THUMB *
;**********************************
    .def    $TMT_Set_Clock
$TMT_Set_Clock

    .state16
    BX      pc
    NOP

    .state32
    B       _TMT_Set_Clock


;********************************
;* TMT_Retrieve_Clock for THUMB *
;********************************
    .def    $TMT_Retrieve_Clock
$TMT_Retrieve_Clock

    .state16
    BX      pc
    NOP

    .state32
    B       _TMT_Retrieve_Clock


;***********************************
;* TMT_Read_Timer Veneer for THUMB *
;***********************************
    .def    $TMT_Read_Timer
$TMT_Read_Timer

    .state16
    BX      pc
    NOP

    .state32
    B       _TMT_Read_Timer


;*************************************
;* TMT_Enable_Timer Veneer for THUMB *
;*************************************
    .def    $TMT_Enable_Timer
$TMT_Enable_Timer

    .state16
    BX      pc
    NOP

    .state32
    B       _TMT_Enable_Timer

;*************************************
;* TMT_Adjust_Timer Veneer for THUMB *
;*************************************
    .def    $TMT_Adjust_Timer
$TMT_Adjust_Timer

    .state16
    BX      pc
    NOP

    .state32
    B       _TMT_Adjust_Timer

;**************************************
;* TMT_Disable_Timer Veneer for THUMB *
;**************************************
    .def    $TMT_Disable_Timer
$TMT_Disable_Timer

    .state16
    BX      pc
    NOP

    .state32
    B       _TMT_Disable_Timer

;******************************************
;* TMT_Retrieve_TS_Task Veneer for THUMB *
;******************************************
    .def    $TMT_Retrieve_TS_Task
$TMT_Retrieve_TS_Task

    .state16
    BX      pc
    NOP

    .state32
    B       _TMT_Retrieve_TS_Task

    .endif



    .end


