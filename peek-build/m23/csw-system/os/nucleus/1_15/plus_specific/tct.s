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
;* FILE NAME                                          VERSION                
;*                
;*   tct.s                       Nucleus PLUS\ARM926 DM320\Code Composer 1.15.1
;*
;*  COMPONENT
;*
;*      TC - Thread Control
;*
;*  DESCRIPTION
;*
;*      This file contains the target processor dependent routines for
;*      performing target-dependent scheduling functions.
;*
;*  GLOBAL FUNCTIONS
;*
;*      *****************************
;*      * GENERIC FUNCTIONS         *
;*      *****************************
;*
;*      TCT_Control_Interrupts          Enable / disable interrupts
;*      TCT_Local_Control_Interrupts    Local enable / disable ints
;*      TCT_Restore_Interrupts          Restore global interrupts
;*      TCT_Build_Task_Stack            Build initial task stack
;*      TCT_Build_HISR_Stack            Build initial HISR stack
;*      TCT_Build_Signal_Frame          Build signal handler frame
;*      TCT_Check_Stack                 Check current stack
;*      TCT_Schedule                    Schedule the next thread
;*      TCT_Control_To_Thread           Transfer control to a thread
;*      TCT_Control_To_System           Transfer control from thread
;*      TCT_Signal_Exit                 Exit from signal handler
;*      TCT_Current_Thread              Returns a pointer to current
;*                                      thread
;*      TCT_Set_Execute_Task            Sets TCD_Execute_Task under
;*                                      protection from interrupts
;*      TCT_Protect                     Protect critical section
;*      TCT_Unprotect                   Unprotect critical section
;*      TCT_Unprotect_Specific          Release specific protection
;*      TCT_Set_Current_Protect         Set the thread's current
;*                                      protection field
;*      TCT_Protect_Switch              Switch to protected thread
;*      TCT_Schedule_Protected          Schedule the protected thread
;*      TCT_Interrupt_Context_Save      Save interrupted context
;*      TCT_Interrupt_Context_Restore   Restore interrupted context
;*      TCT_Activate_HISR               Activate a HISR
;*      TCT_HISR_Shell                  HISR execution shell
;*
;*      *****************************
;*      * TARGET SPECIFIC FUNCTIONS *
;*      *****************************
;*
;*      None
;*
;*  LOCAL FUNCTIONS
;*
;*      None
;*
;************************************************************************

;**********************************
;* INCLUDE ASSEMBLY CONSTANTS     *
;**********************************

    .include     asm_defs.inc

;**********************************
;* EXTERNAL VARIABLE DECLARATIONS *
;**********************************

;extern TC_TCB      *TCD_Execute_Task;
;extern TC_HCB      *TCD_Execute_HISR;
;extern VOID        *TCD_Current_Thread;
;extern VOID        *TCD_System_Stack;
;extern INT         TCD_Interrupt_Count;
;extern TC_HCB      *TCD_Active_HISR_Heads[TC_HISR_PRIORITIES];
;extern TC_HCB      *TCD_Active_HISR_Tails[TC_HISR_PRIORITIES];
;extern INT         TCD_Interrupt_Level;
;extern UNSIGNED    TMD_Time_Slice;
;extern INT         TMD_Time_Slice_State;

    .ref          _TCD_Execute_Task
    .ref          _TCD_Execute_HISR
    .ref          _TCD_Current_Thread
    .ref          _TCD_System_Stack
    .ref          _TCD_Interrupt_Count
    .ref          _TCD_Active_HISR_Heads
    .ref          _TCD_Active_HISR_Tails
    .ref          _TCD_Interrupt_Level
    .ref          _TMD_Time_Slice
    .ref          _TMD_Time_Slice_State

;**********************************
;* EXTERNAL FUNCTION DECLARATIONS *
;**********************************

;extern VOID        TCC_Task_Shell(VOID);
;extern VOID        TCC_Signal_Shell(VOID);
;extern VOID        ERC_System_Error(INT);

    .ref          _TCC_Task_Shell
    .ref          _TCC_Signal_Shell
    .ref          _ERC_System_Error
    
    .if NU_TEST2_SUPPORT
    
    .ref          _Set_Suspend_Time
    
    .endif
    
    .if NU_TEST3_SUPPORT
    
    .ref          _Set_Context_Save_Start
    .ref          _Set_Context_Save_End
    .ref          _Set_Context_Restore_Start
    .ref          _Set_Context_Restore_End
    
    .endif

    .if NU_PROFILE_PLUS

    .ref          __NU_Idle_Hook
    .ref          __NU_Schedule_Task_Hook

    .endif

;**********************************
;* GLOBAL VARIABLE DECLARATIONS   *
;**********************************

    ; Define variable used to hold system stack limit.  The stack limit is utilized
    ; by the ARM tools when the /swst switch is invoked.  This will allow
    ; stack over-flows to be checked.

    .def    _TCT_System_Limit
    .bss    _TCT_System_Limit, 4, 4          ; Reserve 4-bytes with 4-byte alignment

    .if  NU_FIQ_SUPPORT

    ; Define variable used as flag to indicate when a "special" nested interrupt
    ; occurs.  This situation occurs when an FIQ nests on top of an IRQ before
    ; the global interrupt count has been incremented.  This flag is used to
    ; help correct context save/restore routines in this situation.

    .bss    TCT_Special_Nested, 4, 4        ; Reserve 4-bytes with 4-byte alignment

    .endif

;**********************************
;* LOCAL VARIABLES                *
;**********************************
    .text
    .align 4

    ; The following are local variables.  ARM Architecture uses
    ; PC relative addressing, so all global data accessed must
    ; reside within close proximity (<4Kb in ARM, <1Kb in THUMB)
    ; to the instructions that access them.
    ; These are essentially pointers to global data.

    .if  NU_FIQ_SUPPORT

TCT_Special_Nested1
    .word     TCT_Special_Nested

    .endif

TCT_System_Limit1
    .word     _TCT_System_Limit

TCT_Int_Level
    .word     _TCD_Interrupt_Level

TCT_Task_Shell
    .word     _TCC_Task_Shell

TCT_Signal_Shell
    .word     _TCC_Signal_Shell

TCT_Current_Thread1
    .word     _TCD_Current_Thread

TCT_Execute_HISR
    .word     _TCD_Execute_HISR

TCT_Execute_Task
    .word     _TCD_Execute_Task

TCT_Time_Slice
    .word     _TMD_Time_Slice

TCT_Slice_State
    .word     _TMD_Time_Slice_State

TCT_System_Stack
    .word     _TCD_System_Stack

TCT_Int_Count
    .word     _TCD_Interrupt_Count

TCT_HISR_Tails
    .word     _TCD_Active_HISR_Tails

TCT_HISR_Heads
    .word     _TCD_Active_HISR_Heads

TCT_HISR_Shell1
    .word     TCT_HISR_Shell


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Control_Interrupts
;*
;*  DESCRIPTION
;*
;*      This function enables and disables interrupts as specified by
;*      the caller.  Interrupts disabled by this call are left disabled
;*      until the another call is made to enable them.
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
;*      r0 -    New Interrupt Level
;*
;*  OUTPUTS
;*
;*      r0 -    Old Interrupt Level
;*
;*  REGISTERS MODIFIED
;*
;*      r0, r1, r2, r3
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;INT    TCT_Control_Interrupts(INT interrupt_level);

    .def    _TCT_Control_Interrupts
_TCT_Control_Interrupts

    ; Lock out all interrupts before any checking or changing

    MRS     r2,CPSR                         ; Pickup current CPSR
    ORR     r2,r2,#LOCKOUT                  ; Build lockout CPSR
    MSR     CPSR,r2                         ; Lockout interrupts temporarily

    ; Obtain the current interrupt lockout posture.

    LDR     r1,TCT_Int_Level                ; Pickup interrupt level
    LDR     r3,[r1]                         ; Pickup current interrupt lockout

    ; Setup new interrupt lockout posture.

    BIC     r2,r2,#LOCK_MSK                 ; Clear lockout mask
    ORR     r2,r2,r0                        ; Set correct lock-out bits in CPSR
    STR     r0,[r1]                         ; Save current lockout

    ; Reenable interrupts for the specified lockout

    MSR     CPSR,r2                         ; Setup new CPSR lockout bits

    ; Get old lock-out value in return register (r0)

    AND     r0,r3,#LOCK_MSK

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Local_Control_Interrupts
;*
;*  DESCRIPTION
;*
;*      This function enables and disables interrupts as specified by
;*      the caller.
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
;*      r0 -    New Interrupt Level
;*
;*  OUTPUTS
;*
;*      r0 -    Old Interrupt Level
;*
;*  REGISTERS MODIFIED
;*
;*      r0, r2, r3
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;INT    TCT_Local_Control_Interrupts(INT interrupt_level);

    .def    _TCT_Local_Control_Interrupts
_TCT_Local_Control_Interrupts

    ; Read in and save the old level

    MRS     r3,CPSR                         ; Pickup current CPSR

    ; Clear out the old level and set the new level

    BIC     r2,r3,#LOCK_MSK                 ; Clear all current interrupts bits
    ORR     r2,r2,r0                        ; Build new CPSR with new lock-out bits

    ; Reenable interrupts for the specified lockout

    MSR     CPSR,r2

    ; Get old lock-out value in return register (r0)

    AND     r0,r3,#LOCK_MSK

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Restore_Interrupts
;*
;*  DESCRIPTION
;*
;*      This function restores interrupts to the specified value in
;*      the global TCD_Interrupt_Level variable.
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
;*      None
;*
;*  REGISTERS MODIFIED
;*
;*      r0, r1, r2
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID   TCT_Restore_Interrupts(VOID);

    .def    _TCT_Restore_Interrupts
_TCT_Restore_Interrupts

    ; Lock-out all interrupts before any checking or changing

    MRS     r2,CPSR                         ; Pickup current CPSR
    ORR     r1,r2,#LOCKOUT                  ; Build lockout CPSR
    MSR     CPSR,r1                         ; Lockout interrupts temporarily

    ; Obtain the current interrupt lockout posture.

    BIC     r2,r2,#LOCK_MSK                 ; Clear current interrupt levels
    LDR     r1,TCT_Int_Level                ; Load address of TCD_Interrupt_Level
    LDR     r0,[r1]                         ; Pickup current interrupt lockout
    ORR     r2,r2,r0                        ; Build new CPSR with appropriate
                                            ; interrupts locked out

    ; Setup new CPSR lockout bits

    MSR     CPSR,r2

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Build_Task_Stack
;*
;*  DESCRIPTION
;*
;*      This function builds an initial stack frame for a task.  The
;*      initial stack contains information concerning initial values of
;*      registers and the task's point of entry.  Furthermore, the
;*      initial stack frame is in the same form as an interrupt stack
;*      frame.
;*
;*  CALLED BY
;*
;*      TCC_Create_Task
;*      TCC_Reset_Task
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    Task Control Block pointer
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
;VOID   TCT_Build_Task_Stack(TC_TCB *task);

    .def    _TCT_Build_Task_Stack
_TCT_Build_Task_Stack

    ; Pickup the stack starting address from task control block

    LDR     r2,[r0,#TC_STACK_START]

    ; Pickup the stack size (in bytes) from task control block

    LDR     r1,[r0,#TC_STACK_SIZE]

    ; Calculate the stack ending address

    ADD     r3,r1,r2                        ; Compute the stack end (start + size)
    BIC     r3,r3,#3                        ; Insure 4-byte alignment
    SUB     r3,r3,#4                        ; Reserve a word

    ; Save the stack ending address in control block

    STR     r3,[r0,#TC_STACK_END]

    ; Build an initial stack frame as follows:
    ;
    ;     (Lower Address) Stack Top ->    1       (Interrupt stack type)
    ;                                     CPSR    Saved CPSR
    ;                                     r0      Saved r0
    ;                                     r1      Saved r1
    ;                                     r2      Saved r2
    ;                                     r3      Saved r3
    ;                                     r4      Saved r4
    ;                                     r5      Saved r5
    ;                                     r6      Saved r6
    ;                                     r7      Saved r7
    ;                                     r8      Saved r8
    ;                                     r9      Saved r9
    ;                                     r10     Saved r10
    ;                                     r11     Saved r11
    ;                                     r12     Saved r12
    ;                                     sp      Saved sp
    ;                                     lr      Saved lr
    ;     (Higher Address) Stack Bottom-> pc      Saved pc

    ; Get address of task shell

    LDR     r2,TCT_Task_Shell

    BIC     r2,r2,#1                        ; Clear low bit

    STR     r2,[r3], #-4                    ; Push pc (task shell address)
    MOV     r2,#0                           ; Clear value for initial registers
    STR     r2,[r3], #-4                    ; Push lr
    ADD     r2,r3,#0x8                      ; Compute initial sp
    STR     r2,[r3], #-4                    ; Push sp
    STR     r2,[r3], #-4                    ; Push r12
    STR     r2,[r3], #-4                    ; Push r11
    LDR     r2,[r0,#TC_STACK_START]         ; Pickup the stack starting address
    STR     r2,[r3], #-4                    ; Push r10 (stack limit)
    MOV     r2,#0                           ; Clear value for initial registers
    STR     r9,[r3], #-4                    ; Push r9
    STR     r2,[r3], #-4                    ; Push r8
    STR     r2,[r3], #-4                    ; Push r7
    STR     r2,[r3], #-4                    ; Push r6
    STR     r2,[r3], #-4                    ; Push r5
    STR     r2,[r3], #-4                    ; Push r4
    STR     r2,[r3], #-4                    ; Push r3
    STR     r2,[r3], #-4                    ; Push r2
    STR     r2,[r3], #-4                    ; Push r1
    STR     r2,[r3], #-4                    ; Push r0
    MRS     r2,CPSR                         ; Pickup the CPSR
    BIC     r2,r2,#LOCK_MSK                 ; Clear initial interrupt lockout

    ; Check to see if Task shell is build in thumb state

    LDR     r1,TCT_Task_Shell               ; Get address of task shell
    TST     r1,#1                           ; Check bit 0 (set if THUMB)
    ORRNE   r2,r2,#THUMB_BIT                ; If bit 0 set, add THUMB bit to CPSR

    STR     r2,[r3], #-4                    ; Push CPSR
    MOV     r2,#1                           ; Build interrupt stack type (1)
    STR     r2,[r3]                         ; Push stack type

    ; Save the new stack pointer into the task's control block.

    STR     r3,[r0, #TC_STACK_POINTER]

    ; Pickup the stack size (in bytes) from task control block

    LDR     r1,[r0,#TC_STACK_SIZE]

    ; Save the minimum amount of remaining stack memory.

    MOV     r2,#INT_STACK_SIZE              ; Size of interrupt stack frame
    SUB     r1,r1,r2                        ; Compute minimum available bytes
    STR     r1,[r0, #TC_STACK_MINIMUM]      ; Save in minimum stack area

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Build_HISR_Stack
;*
;*  DESCRIPTION
;*
;*      This function builds an initial stack frame for a HISR.  The
;*      initial stack contains information concerning initial values of
;*      registers and the HISR's point of entry.  Furthermore, the
;*      initial stack frame is in the same form as a solicited stack
;*      frame.
;*
;*  CALLED BY
;*
;*      TCC_Create_HISR
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    HISR Control Block pointer
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
;VOID   TCT_Build_HISR_Stack(TC_HCB *hisr);

    .def    _TCT_Build_HISR_Stack
_TCT_Build_HISR_Stack

    ; Pickup the stack starting address from task control block

    LDR     r2,[r0,#TC_STACK_START]

    ; Pickup the stack size (in bytes) from task control block

    LDR     r1,[r0,#TC_STACK_SIZE]

    ; Calculate the stack ending address

    ADD     r3,r1,r2                        ; Compute the stack end (start + size)
    BIC     r3,r3,#3                        ; Insure 4-byte alignment
    SUB     r3,r3,#4                        ; Reserve a word

    ; Save the stack ending address in control block

    STR     r3,[r0,#TC_STACK_END]

    ; Build an initial stack.with the following format:
    ;
    ;     (Lower Address) Stack Top ->    0       (Solicited stack type)
    ;                                     CPSR    Saved CPSR
    ;                                     r4      Saved r4
    ;                                     r5      Saved r5
    ;                                     r6      Saved r6
    ;                                     r7      Saved r7
    ;                                     r8      Saved r8
    ;                                     r9      Saved r9
    ;                                     r10     Saved r10
    ;                                     r11     Saved r11
    ;                                     r12     Saved r12
    ;     (Higher Address) Stack Bottom-> pc      Saved pc

    ; Get address of HISR shell

    LDR     r2, TCT_HISR_Shell1             ; Pickup address of shell entry
    STR     r2,[r3], #-4                    ; Store entry address on stack
    ADD     r2,r3,#0x4                      ; Compute initial sp
    STR     r2,[r3], #-4                    ; Push r12
    STR     r2,[r3], #-4                    ; Push r11
    LDR     r2,[r0,#TC_STACK_START]         ; Pickup the stack starting address
    STR     r2,[r3], #-4                    ; Push r10 (stack limit)
    MOV     r2,#0                           ; Clear value for initial registers
    STR     r9,[r3], #-4                    ; Store initial r9
    STR     r2,[r3], #-4                    ; Store initial r8
    STR     r2,[r3], #-4                    ; Store initial r7
    STR     r2,[r3], #-4                    ; Store initial r6
    STR     r2,[r3], #-4                    ; Store initial r5
    STR     r2,[r3], #-4                    ; Store initial r4
    MRS     r1,CPSR                         ; Pickup the CPSR
    BIC     r1,r1,#LOCK_MSK                 ; Clear initial interrupt lockout
    STR     r1,[r3], #-4                    ; Push CPSR

    ; Store solicited stack type on the top of the stack

    STR     r2,[r3]

    ; Save the new stack pointer into the task's control block.

    STR     r3,[r0, #TC_STACK_POINTER]

    ; Get size of solicited stack frame

    MOV     r2,#SOL_STACK_SIZE

    ; Pickup the stack size (in bytes) from task control block

    LDR     r1,[r0,#TC_STACK_SIZE]

    ; Compute minimum available bytes and save in minimum stack area.

    SUB     r1,r1,r2
    STR     r1,[r0, #TC_STACK_MINIMUM]

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Build_Signal_Frame
;*
;*  DESCRIPTION
;*
;*      This function builds a frame on top of the task's stack to
;*      cause the task's signal handler to execute the next time
;*      the task is executed.  This is built as a solicited stack
;*      frame.
;*
;*  CALLED BY
;*
;*      TCC_Send_Signals
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    Task Control Block pointer
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
;VOID   TCT_Build_Signal_Frame(TC_TCB *task);

    .def    _TCT_Build_Signal_Frame
_TCT_Build_Signal_Frame

    ; Pickup the stack pointer.

    LDR     r3,[r0,#TC_STACK_POINTER]       ; Pickup the current stack pointer

    ; Reserve a word on stack top

    SUB     r3,r3,#4

    ; Build a signal stack with the following format:
    ;
    ;     (Lower Address) Stack Top ->    0       (Solicited stack type)
    ;                                     CPSR    Saved CPSR
    ;                                     r4      Saved r4
    ;                                     r5      Saved r5
    ;                                     r6      Saved r6
    ;                                     r7      Saved r7
    ;                                     r8      Saved r8
    ;                                     r9      Saved r9
    ;                                     r10     Saved r10
    ;                                     r11     Saved r11
    ;                                     r12     Saved r12
    ;     (Higher Address) Stack Bottom-> pc      Saved pc

    ; Get address of signal shell

    LDR     r2,TCT_Signal_Shell             ; Pickup address of shell entry

    ; If in THUMB state, clear low bit.

    BIC     r2,r2,#1

    STR     r2,[r3], #-4                    ; Push pc (signal shell)
    ADD     r2,r3,#0x4                      ; Compute initial sp
    STR     r2,[r3], #-4                    ; Push r12
    STR     r2,[r3], #-4                    ; Push r11
    LDR     r2,[r0,#TC_STACK_START]         ; Pickup the stack starting address
    STR     r2,[r3], #-4                    ; Push r10 (stack limit)
    MOV     r2,#0                           ; Clear value for initial registers
    STR     r9,[r3], #-4                    ; Push r9
    STR     r2,[r3], #-4                    ; Push r8
    STR     r2,[r3], #-4                    ; Push r7
    STR     r2,[r3], #-4                    ; Push r6
    STR     r2,[r3], #-4                    ; Push r5
    STR     r2,[r3], #-4                    ; Push r4

    ; Build CPSR value to put on stack

    MRS     r1,CPSR                         ; Pickup the CPSR
    BIC     r1,r1,#LOCK_MSK                 ; Clear initial interrupt lockout

    ; Check to see if Signal shell is build in thumb state

    LDR     r2,TCT_Signal_Shell             ; Get address of signal shell
    TST     r2,#1                           ; Check bit 0 (set if THUMB)
    ORRNE   r1,r1,#THUMB_BIT                ; If bit 0 set, add THUMB bit to CPSR

    ; Push CPSR on stack

    STR     r1,[r3], #-4

    ; Store solicited stack type on the top of the stack (0 = solicited)

    MOV     r2,#0
    STR     r2,[r3]

    ; Save the new stack pointer into the task's control block.

    STR     r3,[r0, #TC_STACK_POINTER]

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Check_Stack
;*
;*  DESCRIPTION
;*
;*      This function checks the current stack for overflow conditions.
;*      Additionally, this function keeps track of the minimum amount
;*      of stack space for the calling thread and returns the current
;*      available stack space.
;*
;*  CALLED BY
;*
;*      TCC_Send_Signals                Send signals to a task
;*
;*  CALLS
;*
;*      ERC_System_Error                System error handler
;*
;*  INPUTS
;*
;*      None
;*
;*  OUTPUTS
;*
;*      r0 -    Available bytes on stack
;*
;*  REGISTERS MODIFIED
;*
;*      r0, r1, r2, r3
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;INT    TCT_Check_Stack(VOID);

    .def    _TCT_Check_Stack
_TCT_Check_Stack

    ; Pickup the current thread pointer.

    LDR     r0,TCT_Current_Thread1
    LDR     r0,[r0]

    ; Determine if there is a current thread.

    CMP     r0,#0                           ; Determine if a thread is active
    MOV     r3,#0                           ; Default remaining value
    BEQ     TCT_Skip_Stack_Check            ; If NU_NULL, skip stack checking

    ; Determine if the stack pointer has overflowed

    LDR     r2,[r0,#TC_STACK_START]         ; Pickup start of stack area
    CMP     sp,r2                           ; Compare with current stack ptr
    BLT     TCT_Stack_Range_Error           ; If less, stack is out of range

    ; Determine if the stack pointer has underflowed

    LDR     r1,[r0,#TC_STACK_END]           ; Pickup end of stack area
    CMP     sp,r1                           ; Compare with current stack ptr
    BLE     TCT_Stack_Range_Okay            ; If less, stack range is okay

TCT_Stack_Range_Error

    MOV     r0,#3                           ; Build NU_STACK_OVERFLOW code
    BL      _ERC_System_Error               ; Call system error handler

TCT_Stack_Range_Okay

    ; Calculate the amount of available space on the stack.

    SUB     r3,sp,r2

    ; Determine if there is enough memory on the stack to save
    ; all of the registers.

    CMP     r3,#INT_STACK_SIZE

    ; If there is enough memory, there is no stack overflow yet

    BGE     TCT_No_Stack_Error

    ; Stack overflow condition is about to happen.

    MOV     r0,#3                           ; Build NU_STACK_OVERFLOW code
    BL      _ERC_System_Error               ; Call system error handler

TCT_No_Stack_Error

    ; Determine if this is a new minimum amount of stack space.
    ; If so, save the new stack minimum.

    LDR     r2,[r0,#TC_STACK_MINIMUM]
    CMP     r3,r2
    STRCC   r3,[r0,#TC_STACK_MINIMUM]

TCT_Skip_Stack_Check

    ; Return the remaining number of bytes on the stack.

    MOV     r0,r3

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Schedule
;*
;*  DESCRIPTION
;*
;*      This function waits for a thread to become ready.  Once a thread
;*      is ready, this function initiates a transfer of control to that
;*      thread.
;*
;*  CALLED BY
;*
;*      INC_Initialize                  Main initialization routine
;*      TCT_Control_To_System           Transfers control to system
;*      TCT_Signal_Exit                 Exit routine for signals
;*      TCT_Unprotect_Specific          Unprotects specific protect
;*      TCT_Interrupt_Context_Restore   Restore context after interrupt
;*      TCT_HISR_Shell                  Entry point for HISRs
;*
;*  CALLS
;*
;*      TCT_Control_To_Thread           Transfer control to a thread
;*
;*  INPUTS
;*
;*      None
;*
;*  OUTPUTS
;*
;*      r0 -    Pointer to scheduled thread
;*
;*  REGISTERS MODIFIED
;*
;*      r0, r1, r2
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TCT_Schedule(VOID)

    .def    _TCT_Schedule
_TCT_Schedule


    .if NU_TEST2_SUPPORT

    BL      _Set_Suspend_Time
    
    .endif
    
    
    ; Restore interrupts according to the value contained in TCD_Interrupt_Level.

    LDR     r1,TCT_Int_Level                ; Get address of interrupt level
    LDR     r1,[r1]                         ; Pickup current interrupt lockout
    MRS     r0,CPSR                         ; Pickup current CPSR
    BIC     r0,r0,#LOCK_MSK                 ; Clear the interrupt lockout bits
    ORR     r0,r0,r1                        ; Build new interrupt lockout CPSR
    MSR     CPSR,r0                         ; Setup new CPSR

    .def	TCT_Schedule_Loop
TCT_Schedule_Loop

    ; Get the addresses of the HISR and Task to execute

    LDR     r2,TCT_Execute_HISR
    LDR     r3,TCT_Execute_Task

    .if NU_PROFILE_PLUS

    ; Check to see if there is a task to execute upon entering TCT_Schedule.
    ; If not, we start IDLE.

    LDR     r0,[r2]                         ; Pickup highest priority HISR ptr
    CMP     r0,#0                           ; Is there a HISR active?
    BNE     TCT_Schedule_Thread             ; Found an HISR
    LDR     r0,[r3]                         ; Pickup highest priority Task ptr
    CMP     r0,#0                           ; Is there a task active?
    BNE     TCT_Schedule_Thread             ; If not, start IDLE.
    STMDB   sp!,{r2-r3}                     ; Save r2-r3
    BL      __NU_Idle_Hook
    LDMIA   sp!,{r2-r3}                     ; Restore r2-r3

    .endif

    ; Wait until a thread (task or HISR) is available to execute.
    ; When a thread is available, branch to TCT_Schedule_Thread.

    LDR     r0,[r2]                         ; Pickup highest priority HISR ptr
    CMP     r0,#0                           ; Is there a HISR active?
    BNE     TCT_Schedule_Thread             ; Found a HISR
    LDR     r0,[r3]                         ; Pickup highest priority Task ptr
        
    CMP     r0,#0                           ; Is there a task active?
    
;;;;; TI CODE : modification starts

    .ref    INT_Small_Sleep
    BEQ     INT_Small_Sleep		    ; No, enter the GSM Small Sleep mode
        
;;;;;BEQ     TCT_Schedule_Loop               ; If not, continue the search

;;;;!; TI CODE : modification ends

TCT_Schedule_Thread

    ; Either a task or HISR is ready to execute.  Lockout interrupts while
    ; the thread is transferred (by falling through to the following routine).

    MRS     r1,CPSR                         ; Pickup CPSR again
    ORR     r1,r1,#LOCKOUT                  ; Build interrupt lockout value
    MSR     CPSR,r1                         ; Lockout interrupts


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Control_To_Thread
;*
;*  DESCRIPTION
;*
;*      This function transfers control to the specified thread.  Each
;*      time control is transferred to a thread, its scheduled counter
;*      is incremented.  Additionally, time-slicing for task threads is
;*      enabled in this routine.  The TCD_Current_Thread pointer is
;*      setup by this function.
;*
;*  CALLED BY
;*
;*      TCT_Schedule                    Indirectly called
;*      TCT_Protect                     Protection task switch
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    Thread pointer to schedule
;*
;*  OUTPUTS
;*
;*      None
;*
;*  REGISTERS MODIFIED
;*
;*      r0, r1, r2, r3, sp, pc, SPSR
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TCT_Control_To_Thread(VOID *thread)

    .def    _TCT_Control_To_Thread
_TCT_Control_To_Thread

    ; Put thread pointer into TCD_Current_Thread

    LDR     r1,TCT_Current_Thread1          ; Pickup current thread ptr address
    STR     r0,[r1]                         ; Setup current thread pointer

; TI CODE : Call the Function that log the Current Thread.

  .if OP_L1_STANDALONE = 0
    .if TI_NUC_MONITOR = 1
  		.global _ti_nuc_monitor_Thread_log
		STMFD	sp!,{r0-r5}
		BL		_ti_nuc_monitor_Thread_log		; Call the Thread Log function.
		LDMFD	sp!,{r0-r5}
    .endif

    ; WCP Profiler
    .if WCP_PROF = 1
       .global _prf_LogTaskSwitch
       STMFD    sp!,{a1-a4}
       BL       _prf_LogTaskSwitch
       LDMFD	sp!,{a1-a4}
    .endif
  .endif

    ; Increment the thread scheduled counter

    LDR     r2,[r0, #TC_SCHEDULED]          ; Pickup scheduled count
    ADD     r2,r2,#1                        ; Increment the scheduled count
    STR     r2,[r0, #TC_SCHEDULED]          ; Store new scheduled count

    ; Check for time slice option

    LDR     r3,[r0, #TC_CUR_TIME_SLICE]     ; Pickup time slice value
    CMP     r3,#0                           ; Is there a time slice?
    BEQ     TCT_No_Start_TS_1               ; If 0, there is no time slice

    ; Set-up the time slice value

    LDR     r2,TCT_Time_Slice               ; Pickup address of TMD_Time_Slice
    STR     r3,[r2]                         ; Setup the time slice

    ; Enable the time-slice timer

    LDR     r1,TCT_Slice_State              ; Pickup address of TMD_Time_Slice_State
    MOV     r2,#0                           ; Build active state flag
    STR     r2,[r1]                         ; Set the active flag

TCT_No_Start_TS_1

    .if NU_PROFILE_PLUS

    STMDB   sp!,{r0}                        ; Push r0
    BL      _NU_Schedule_Task_Hook          ; Branch to RTView
    LDMIA   sp!,{r0}                        ; Pop r0

    .endif

    ; Pickup the thread's stack pointer and resume the thread.

    LDR     sp,[r0, #TC_STACK_POINTER]

    ; Pop off the stack top to determine stack type

    LDR     r1,[sp], #4

    ; Remove CPSR from stack and put in SPSR

    LDR     r0,[sp], #4                     ; Pop off the CPSR
    MSR     SPSR,r0                         ; Place it into the SPSR

    ; Check if interrupt stack or solicited stack

    CMP     r1,#1                           ; Compare stack type with 1
    BEQ     TCT_Interrupt_Resume            ; If equal to 1, interrupt stack frame

    ; Recover all solicited stack frame registers and return

    LDMIA   sp!,{r4-r12,pc}^                ; A solicited return is required.
                                            ; This type of return only
                                            ; Recovers r4-r12 & pc
TCT_Interrupt_Resume

    ; Recover all registers and resume at point of interrupt

    LDMIA   sp,{r0-pc}^


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Control_To_System
;*
;*  DESCRIPTION
;*
;*      This function returns control from a thread to the system.  Note
;*      that this service is called in a solicited manner, i.e. it is
;*      not called from an interrupt thread.  Registers required by the
;*      compiler to be preserved across function boundaries are saved by
;*      this routine.  Note that this is usually a sub-set of the total
;*      number of available registers.
;*
;*  CALLED BY
;*
;*      Other Components
;*
;*  CALLS
;*
;*      TCT_Schedule                        Schedule the next thread
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
;*      r0, r1, r2, r3, r10, sp, CPSR
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TCT_Control_To_System(VOID)

    .def    _TCT_Control_To_System
_TCT_Control_To_System

    ; Lockout interrupts.

    MRS     r0,CPSR                         ; Pickup current CPSR
    ORR     r1,r0,#LOCKOUT                  ; Build interrupt lockout value
    MSR     CPSR,r1                         ; Lockout interrupts

    ; Save a minimal context of the thread on the current stack.

    STMDB   sp!,{r4-r12,lr}

    ; Check to see if caller was in THUMB mode
    ; and update saved CPSR accordingly

    TST     lr,#1                           ; Check bit 0 (set if THUMB mode)
    ORRNE   r0,r0,#THUMB_BIT                ; Set THUMB bit if caller in THUMB mode

    ; Save CPSR on stack (with caller's lock-out bits / mode set correctly)

    STR     r0,[sp, #-4]!

    ; Push solicited stack type on the top of the stack.

    MOV     r2,#0
    STR     r2,[sp, #-4]!

    ; Setup a pointer to the thread control block and set the current
    ; thread pointer to NU_NULL.

    LDR     r1,TCT_Current_Thread1          ; Pickup current thread ptr address
    LDR     r0,[r1]                         ; Pickup current thread pointer
    STR     r2,[r1]                         ; Set current thread pointer to
                                            ; NU_NULL

    ; Check to see if a time slice is active.

    LDR     r3,TCT_Slice_State              ; Pickup time slice state address
    LDR     r1,[r3]                         ; Pickup time slice state flag
    CMP     r1,#0                           ; Compare with active value
    BNE     TCT_No_Stop_TS_1                ; If non-active, don't disable

    ; Insure that the next time the task runs it gets a fresh time slice.

    LDR     r1,[r0, #TC_TIME_SLICE]         ; Pickup original time slice
    STR     r1,[r0, #TC_CUR_TIME_SLICE]     ; Reset current time slice

    ; Clear any active time slice by setting the state to NOT_ACTIVE.

    MOV     r2,#1                           ; Build disable value
    STR     r2,[r3]                         ; Disable time slice

TCT_No_Stop_TS_1

    ; Save off the current stack pointer in the control block.

    STR     sp,[r0, #TC_STACK_POINTER]      ; Save the thread's stack pointer

    ; Clear the task's current protection.

    LDR     r1,[r0, #TC_CURRENT_PROTECT]    ; Pickup current thread pointer
    MOV     r2,#0                           ; Build NU_NULL value
    STR     r2,[r0, #TC_CURRENT_PROTECT]    ; Clear the protect pointer field
    STR     r2,[r1]                         ; Release the actual protection

    ; Switch to the system stack / system stack limit

    LDR     r1,TCT_System_Stack             ; Pickup address of stack pointer
    LDR     r2,TCT_System_Limit1            ; Pickup address of stack limit ptr
    LDR     sp,[r1]                         ; Switch to system stack
    LDR     r10,[r2]                        ; Setup system stack limit

    ; Finished, return to the scheduling loop.

    B       _TCT_Schedule                   ; Return to scheduling loop


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Signal_Exit
;*
;*  DESCRIPTION
;*
;*      This function exits from a signal handler.  The primary purpose
;*      of this function is to clear the scheduler protection and switch
;*      the stack pointer back to the normal task's stack pointer.
;*
;*  CALLED BY
;*
;*      TCC_Signal_Shell                Signal handling shell function
;*
;*  CALLS
;*
;*      TCT_Schedule                    Scheduler
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
;*      r0, r1, r2, r3, r10, sp, CPSR
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TCT_Signal_Exit(VOID)

    .def    _TCT_Signal_Exit
_TCT_Signal_Exit

    ; Lockout interrupts.

    MRS     r3,CPSR                         ; Pickup current CPSR
    ORR     r3,r3,#LOCKOUT                  ; Build lockout value
    MSR     CPSR,r3                         ; Lockout interrupts

    ; Setup a pointer to the thread control block.

    LDR     r1,TCT_Current_Thread1          ; Pickup address of thread pointer
    LDR     r0,[r1]                         ; Pickup current thread pointer

    ; Clear the current thread control block.

    MOV     r2,#0                           ; Build NU_NULL value
    STR     r2,[r1]                         ; Clear current thread pointer

    ; Check to see if a time slice is active.

    LDR     r3,TCT_Slice_State              ; Pickup time slice state address
    LDR     r1,[r3]                         ; Pickup time slice state flag
    CMP     r1,#0                           ; Compare with active value
    BNE     TCT_No_Stop_TS_2                ; If non-active, don't disable

    ; Insure that the next time the task runs it gets a fresh time slice.

    LDR     r1,[r0, #TC_TIME_SLICE]         ; Pickup original time slice
    STR     r1,[r0, #TC_CUR_TIME_SLICE]     ; Reset current time slice

    ; Clear any active time slice by setting the state to NOT_ACTIVE.

    MOV     r2,#1                           ; Build disable value
    STR     r2,[r3]                         ; Disable time slice


TCT_No_Stop_TS_2

    ; Switch back to the saved stack.  The saved stack pointer was saved
    ; before the signal frame was built.

    LDR     r1,[r0, #TC_SAVED_STACK_PTR]    ; Pickup saved stack pointer
    STR     r1,[r0, #TC_STACK_POINTER]      ; Place in current stack pointer

    ; Clear the task's current protection.

    LDR     r1,[r0, #TC_CURRENT_PROTECT]    ; Pickup current thread pointer
    MOV     r2,#0                           ; Build NU_NULL value
    STR     r2,[r0, #TC_CURRENT_PROTECT]    ; Clear the protect pointer field
    STR     r2,[r1]                         ; Release the actual protection

    ; Switch to the system stack / system stack limit

    LDR     r1,TCT_System_Stack             ; Pickup address of stack pointer
    LDR     r2,TCT_System_Limit1            ; Pickup address of stack limit ptr
    LDR     sp,[r1]                         ; Switch to system stack
    LDR     r10,[r2]                        ; Setup system stack limit

    ; Finished, return to the scheduling loop.

    B       _TCT_Schedule                   ; Return to scheduling loop


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Current_Thread
;*
;*  DESCRIPTION
;*
;*      This function returns the current thread pointer.
;*
;*  CALLED BY
;*
;*      Application
;*      System Components
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
;*      r0 -    Pointer to current thread
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
;VOID  *TCT_Current_Thread(VOID)

    .def    _TCT_Current_Thread
_TCT_Current_Thread

    ; Return the current thread pointer.

    LDR     r0,TCT_Current_Thread1          ; Pickup address of thread pointer
    LDR     r0,[r0]                         ; Pickup current thread pointer

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Set_Execute_Task
;*
;*  DESCRIPTION
;*
;*      This function sets the current task to execute variable under
;*      protection against interrupts.
;*
;*  CALLED BY
;*
;*      TCC Scheduling Routines
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    Pointer to task control block
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
;VOID  TCT_Set_Execute_Task(TC_TCB *task)

    .def    _TCT_Set_Execute_Task
_TCT_Set_Execute_Task

    ; Setup the TCD_Execute_Task pointer.

    LDR     r1,TCT_Execute_Task             ; Pickup execute task ptr address
    STR     r0,[r1]                         ; Setup new task to execute

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Protect
;*
;*  DESCRIPTION
;*
;*      This function protects against multiple thread access.
;*
;*  CALLED BY
;*
;*      Application
;*      System Components
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    Pointer to protection block
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
;VOID  TCT_Protect(TC_PROTECT *protect)

    .def    _TCT_Protect
_TCT_Protect

    ; Determine if the caller is in a task or HISR thread.

    LDR     r1,TCT_Current_Thread1          ; Pickup current thread ptr address
    LDR     r1,[r1]                         ; Pickup current thread pointer
    CMP     r1,#0                           ; Check to see if it is non-NULL
    BEQ     TCT_Skip_Protect                ; If NULL, skip protection

    ; Save current CPSR

    MRS     r2,CPSR                         ; Pickup current CPSR
    STMDB   sp!,{r2}                        ; Save CPSR on stack

TCT_Protect_Loop

    ; Lock-out interrupts

    ORR     r2,r2,#LOCKOUT                  ; Place lockout value in r2
    MSR     CPSR,r2                         ; Lockout interrupts

    ; Wait until the protect structure is available.

    LDR     r2,[r0, #TC_TCB_PTR]            ; Pickup protection owner field
    CMP     r2,#0                           ; Does another thread own this protection?
    BEQ     TCT_Protect_Available           ; If NU_NULL, protection not owned

    ; Protection structure is not available.
    ; Indicate that another thread is waiting.

    MOV     r3,#1                           ; Build thread waiting flag
    STR     r3,[r0, #TC_THREAD_WAIT]        ; Set waiting field

    ; Save r0-r1 and lr on stack

    STMDB   sp!,{r0-r1,lr}

    ; Directly schedule the thread holding the protection

    MOV     r0,r2                           ; Place thread owning protect into r0
    BL      _TCT_Schedule_Protected         ; Call routine to schedule the
                                            ; owner of the protection

    ; Recover r0-r1 and lr off stack

    LDMIA   sp!,{r0-r1,lr}

    ; Pickup current CPSR

    MRS     r2,CPSR

    ; Loop until protection available

    B       TCT_Protect_Loop

TCT_Protect_Available

    ; Protection structure is available.
    ; Indicate that current thread owns the protection.

    STR     r1,[r0, #TC_TCB_PTR]

    ; Clear the thread waiting flag.

    MOV     r3,#0
    STR     r3,[r0, #TC_THREAD_WAIT]        ; Clear the thread waiting flag

    ; Save the protection pointer in the thread's control block.

    STR     r0,[r1, #TC_CURRENT_PROTECT]

    ; Restore interrupts to entry level

    LDMIA   sp!,{r2}
    MSR     CPSR,r2

TCT_Skip_Protect

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Unprotect
;*
;*  DESCRIPTION
;*
;*      This function releases protection of the currently active
;*      thread.  If the caller is not an active thread, then this
;*      request is ignored.
;*
;*  CALLED BY
;*
;*      Application
;*      System Components
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
;*      r0, r1, r2, r3
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TCT_Unprotect(void)

    .def    _TCT_Unprotect
_TCT_Unprotect

    ; Determine if the caller is in a task or HISR thread.

    LDR     r1,TCT_Current_Thread1          ; Pickup current thread ptr address
    LDR     r1,[r1]                         ; Pickup current thread pointer
    CMP     r1,#0                           ; Check to see if it is non-NULL
    BEQ     TCT_Skip_Unprotect              ; If NULL, skip unprotection

    ; Lockout interrupts.

    MRS     r2,CPSR                         ; Pickup current CPSR
    STMDB   sp!,{r2}                        ; Save CPSR on stack
    ORR     r2,r2,#LOCKOUT                  ; Place lockout value in
    MSR     CPSR,r2                         ; Lockout interrupts

    ; Determine if there is a currently active protection.

    LDR     r0,[r1, #TC_CURRENT_PROTECT]    ; Pickup current protect field
    CMP     r0,#0                           ; Is there a protection in force?
    BEQ     TCT_Not_Protected               ; If not, nothing to unprotect

    ; Check for a higher priority thread waiting for the protection structure.

    LDR     r2,[r0, #TC_THREAD_WAIT]        ; Pickup thread waiting flag
    CMP     r2,#0                           ; Are there any threads waiting?
    BEQ     TCT_Not_Waiting_Unpr            ; If not, just release protection

    ; Save link register

    STMDB   sp!,{lr}

    ; Transfer control to the system.  Note that this
    ; automatically clears the current protection

    BL      _TCT_Control_To_System           ; Return control to the system

    ; Restore link register

    LDMIA   sp!,{lr}

    B       TCT_Not_Protected

TCT_Not_Waiting_Unpr

    ; Clear the protection.

    MOV     r2,#0                           ; Build NU_NULL value
    STR     r2,[r0, #TC_TCB_PTR]            ; Release the protection
    STR     r2,[r1, #TC_CURRENT_PROTECT]    ; Clear protection pointer in the
                                            ; control block

TCT_Not_Protected

    ; Restore interrupts from stack

    LDMIA   sp!,{r1}
    MSR     CPSR,r1

TCT_Skip_Unprotect

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Unprotect_Specific
;*
;*  DESCRIPTION
;*
;*      This function releases a specific protection structure.
;*
;*  CALLED BY
;*
;*      Application
;*      System Components
;*
;*  CALLS
;*
;*      TCT_Schedule
;*
;*  INPUTS
;*
;*      r0 -    Pointer to protection block
;*
;*  OUTPUTS
;*
;*      None
;*
;*  REGISTERS MODIFIED
;*
;*      r1, r2, r3, sp
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TCT_Unprotect_Specific(TC_PROTECT *protect)

    .def    _TCT_Unprotect_Specific
_TCT_Unprotect_Specific

    ; Determine if the caller is in a task or HISR thread.

    LDR     r1,TCT_Current_Thread1          ; Pickup current thread ptr address
    LDR     r3,[r1]                         ; Pickup current thread pointer
    CMP     r3,#0                           ; Check to see if it is non-NULL
    BEQ     TCT_Skip_Unprot_Spec            ; If NULL, skip unprotect specific

    ; Lockout interrupts.

    MRS     r3,CPSR                         ; Pickup current CPSR
    ORR     r2,r3,#LOCKOUT                  ; Place lockout value in
    MSR     CPSR,r2                         ; Lockout interrupts

    ; Clear the protection pointer.

    MOV     r2,#0                           ; Build NU_NULL value
    STR     r2,[r0, #TC_TCB_PTR]            ; Clear protection ownership

    ; Determine if a thread is waiting.

    LDR     r2,[r0, #TC_THREAD_WAIT]        ; Pickup the waiting field
    CMP     r2,#0                           ; Is there another thread waiting?
    BEQ     TCT_Not_Waiting_Unspec          ; No, restore interrupts and return

    ; Save a minimal context of the thread on the current stack.

    STMDB   sp!,{r4-r12,lr}

    ; Check to see if caller was in THUMB mode
    ; and update saved CPSR accordingly

    TST     lr,#1                           ; Check bit 0 (set if THUMB mode)
    ORRNE   r3,r3,#THUMB_BIT                ; Set THUMB bit if caller in THUMB mode

    ; Save CPSR on stack (with caller's lock-out bits / mode set correctly)

    STR     r3,[sp, #-4]!

    ; Place the solicited stack type value on the top of the stack

    MOV     r2,#0                           ; Build solicited stack type value
    STR     r2,[sp, #-4]!                   ; Place it on the top of the stack

    ; Setup a pointer to the thread control block and clear the current
    ; thread control block pointer.

    LDR     r1,TCT_Current_Thread1          ; Pickup current thread ptr address
    LDR     r0,[r1]                         ; Pickup current thread pointer
    STR     r2,[r1]                         ; Set current thread pointer to NU_NULL

    ; Check to see if a time slice is active.

    LDR     r3,TCT_Slice_State              ; Pickup time slice state address
    LDR     r1,[r3]                         ; Pickup time slice state flag
    CMP     r1,#0                           ; Compare with active value
    BNE     TCT_No_Stop_TS_3                ; If non-active, don't disable

    ; Insure that the next time the task runs it gets a fresh time slice

    LDR     r1,[r0, #TC_TIME_SLICE]         ; Pickup original time slice
    STR     r1,[r0, #TC_CUR_TIME_SLICE]     ; Reset current time slice

    ; Set the time-slice state to NOT_ACTIVE.

    MOV     r2,#1                           ; Build disable value
    STR     r2,[r3]                         ; Disable time slice

TCT_No_Stop_TS_3

    ; Save off the current stack pointer in the control block.

    STR     sp,[r0, #TC_STACK_POINTER]

    ; Switch to the system stack.

    LDR     r1,TCT_System_Stack             ; Pickup address of stack pointer
    LDR     r2,TCT_System_Limit1            ; Pickup address of stack limit ptr
    LDR     sp,[r1]                         ; Switch to system stack
    LDR     r10,[r2]                        ; Setup system stack limit

    ; Finished, return to the scheduling loop

    B       _TCT_Schedule

TCT_Not_Waiting_Unspec

    ; Restore interrupts to original level

    MSR     CPSR,r3

TCT_Skip_Unprot_Spec

    ; Return to caller.

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Set_Current_Protect
;*
;*  DESCRIPTION
;*
;*      This function sets the current protection field of the current
;*      thread's control block to the specified protection pointer.
;*
;*  CALLED BY
;*
;*      TCC_Resume_Task                 Resume task function
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    Pointer to protection block
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
;VOID  TCT_Set_Current_Protect(TC_PROTECT *protect)

    .def    _TCT_Set_Current_Protect
_TCT_Set_Current_Protect

    ; Determine if the caller is in a task or HISR thread.

    LDR     r1,TCT_Current_Thread1          ; Pickup current thread ptr address
    LDR     r1,[r1]                         ; Pickup current thread pointer
    CMP     r1,#0                           ; Check to see if a thread is active

    ; If the caller is in a task or HISR, modify the current protection.

    STRNE   r0,[r1, #TC_CURRENT_PROTECT]

    ; Return to caller.

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Protect_Switch
;*
;*  DESCRIPTION
;*
;*      This function waits until a specific task no longer has any
;*      protection associated with it.  This is necessary since task's
;*      cannot be suspended or terminated unless they have released all
;*      of their protection.
;*
;*  CALLED BY
;*
;*      System Components
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    Pointer to protection block
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
;VOID  TCT_Protect_Switch(TC_TCB *task)

    .def    _TCT_Protect_Switch
_TCT_Protect_Switch

    ; Lockout interrupts.

    MRS     r1,CPSR                         ; Pickup current CPSR
    STMDB   sp!,{r1}                        ; Save CPSR on stack

TCT_Swtich_Loop

    ORR     r1,r1,#LOCKOUT                  ; Place lockout value in
    MSR     CPSR,r1                         ; Lockout interrupts

    ; Wait until the specified task has no protection associated with it.

    LDR     r1,[r0, #TC_CURRENT_PROTECT]    ; Pickup protection of specified thread
    CMP     r1,#0                           ; Does the specified thread have
                                            ; an active protection?
    BEQ     TCT_Switch_Done                 ; If not, protect switch is done

    ; Indicate that a higher priority thread is waiting.

    MOV     r2,#1                           ; Build waiting flag value
    STR     r2,[r1, #TC_THREAD_WAIT]        ; Set waiting flag

    ; Save r0 and lr on stack

    STMDB   sp!,{r0,lr}

    ; Get address of thread holding protection

    LDR     r0,[r1, #TC_TCB_PTR]

    ; Schedule the protected thread

    BL      _TCT_Schedule_Protected

    ; Restore registers

    LDMIA   sp!,{r0,lr}

    ; Pickup current CPSR

    MRS     r1,CPSR

    ; Keep looping until switch complete (protection free)

    B       TCT_Swtich_Loop

TCT_Switch_Done

    ; Restore interrupts.

    LDMIA   sp!,{r1}                        ; Get saved CPSR off stack
    MSR     CPSR,r1                         ; Restore CPSR

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Schedule_Protected
;*
;*  DESCRIPTION
;*
;*      This function saves the minimal context of the thread and then
;*      directly schedules the thread that has protection over the
;*      the thread that called this routine.
;*
;*  CALLED BY
;*
;*      TCT_Protect
;*      TCT_Protect_Switch
;*
;*  CALLS
;*
;*      TCT_Control_To_Thread           Transfer control to protected
;*                                      thread
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
;*      EAX, EDX, ESP
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TCT_Schedule_Protected(TC_TCB *task)

    .def    _TCT_Schedule_Protected
_TCT_Schedule_Protected

    ; Interrupts are already locked out by the caller.
    ; Save minimal context required by the system.

    STMDB   sp!,{r4-r12,lr}                 ; Save minimal context

    ; Pickup current CPSR

    MRS     r1,CPSR

    ; Check to see if caller was in THUMB mode
    ; and update saved CPSR accordingly

    TST     lr,#1                           ; Check bit 0 (set if THUMB mode)
    ORRNE   r1,r1,#THUMB_BIT                ; Set THUMB bit if caller in THUMB mode

    ; Save CPSR on stack (with caller's lock-out bits / mode set correctly)

    STR     r1,[sp, #-4]!

    ; Put solicited stack type on top of the stack

    MOV     r2,#0                           ; Build solicited stack type value
                                            ; and NU_NULL value
    STR     r2,[sp, #-4]!                   ; Place it on the top of the stack

    ; Setup a pointer to the thread control block and clear it.

    LDR     r3,TCT_Current_Thread1          ; Pickup current thread ptr address
    LDR     r1,[r3]                         ; Get current thread pointer address
    STR     r2,[r3]                         ; Set current thread pointer to NU_NULL

    ; Check to see if a time slice is active.

    LDR     r3,TCT_Slice_State              ; Pickup time slice state address
    LDR     r2,[r3]                         ; Pickup time slice state flag
    CMP     r2,#0                           ; Compare with active value
    BNE     TCT_No_Stop_TS_4                ; If non-active, don't disable

    ; Insure that the next time the task runs it gets a fresh time slice

    LDR     r2,[r1, #TC_TIME_SLICE]         ; Pickup original time slice
    STR     r2,[r1, #TC_CUR_TIME_SLICE]     ; Reset current time slice

    ; Set time-slice state to NOT_ACTIVE.

    MOV     r2,#1                           ; Build disable value
    STR     r2,[r3]                         ; Disable time slice

TCT_No_Stop_TS_4

    ; Save off the current stack pointer in the control block.

    STR     sp,[r1, #TC_STACK_POINTER]

    ; Switch to the system stack and system stack limit

    LDR     r1,TCT_System_Stack             ; Pickup address of stack pointer
    LDR     r2,TCT_System_Limit1            ; Pickup address of stack limit ptr
    LDR     sp,[r1]                         ; Switch to system stack
    LDR     r10,[r2]                        ; Setup system stack limit

    ; Transfer control to the specified thread directly.

    B       _TCT_Control_To_Thread


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Interrupt_Context_Save
;*
;*  DESCRIPTION
;*
;*      This function saves the interrupted thread's context.  Nested
;*      interrupts are also supported.  If a task or HISR thread was
;*      interrupted, the stack pointer is switched to the system stack
;*      after the context is saved.
;*
;*  CALLED BY
;*
;*      Application ISRs                Assembly language ISRs
;*      INT_IRQ_Shell                   IRQ Interrupt handler shell
;*      INT_FIQ_Shell                   FIQ Interrupt handler shell
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    Interrupt's vector number
;*
;*  OUTPUTS
;*
;*      None
;*
;*  REGISTERS MODIFIED
;*
;*      r1, r2, r3, r4, r5, sp, CPSR
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID  TCT_Interrupt_Context_Save(vector)

    .def    _TCT_Interrupt_Context_Save
_TCT_Interrupt_Context_Save


    .if NU_TEST3_SUPPORT
    
    STMDB   sp!,{r0,r12,lr}
    BL      _Set_Context_Save_Start
    LDMIA   sp!,{r0,r12,lr} 
    
    .endif
    
    ; Determine if this is a nested interrupt.

    LDR     r1,TCT_Int_Count                ; Pickup address of interrupt count
    LDR     r2,[r1]                         ; Pickup interrupt counter
    ADD     r2,r2,#1                        ; Add 1 to interrupt counter
    STR     r2,[r1]                         ; Store new interrupt counter value
    CMP     r2,#1                           ; Is this first interrupt?
    BNE     TCT_Nested_Save                 ; If not first interrupt, do nested save

    .if  NU_FIQ_SUPPORT

    ; Check for a special nested case.  A special nested case occurs when
    ; a second interrupt goes off before TCT_Int_Count is incremented.

    MRS     r1,SPSR                         ; Pickup value of SPSR
    AND     r1,r1,#MODE_MASK                ; Clear all but mode from SPSR
    TEQ     r1,#IRQ_MODE                    ; Check to see if in IRQ Mode
    BNE     TCT_Normal_Save                 ; If not equal, a normal save is done

    ; Set special nested flag

    MOV     r1,#1
    LDR     r2,TCT_Special_Nested1          ; Get address of special nested flag
    STR     r1,[r2]                         ; Set flag to 1

    ; Check if a thread is executing

    LDR     r1,TCT_Current_Thread1          ; Get current thread pointer
    LDR     r1,[r1]                         ; Get current thread
    CMP     r1,#0                           ; Is a thread running?
    BNE     TCT_Thread_Save                 ; No, just do a normal context save

    .else

    B       TCT_Normal_Save

    .endif   ; NU_FIQ_SUPPORT

TCT_Nested_Save

    ; Save the necessary exception registers into r1-r3

    MOV     r1,sp                           ; Put the exception sp into r1
    MOV     r2,lr                           ; Move the return address for the caller
                                            ; of this function into r2
    MRS     r3,spsr                         ; Put the exception spsr into r3

    ; Adjust the exception stack pointer for future exceptions

    ADD     sp,sp,#24                       ; sp will point to enable reg value when done

    ; Switch CPU modes to save context on system stack

    MRS     r5,CPSR                         ; Pickup the current CPSR
    BIC     r5,r5,#MODE_MASK                ; Clear the mode bits
    ORR     r5,r5,#SUP_MODE                 ; Change to supervisor mode (SVD)
    MSR     CPSR,r5                         ; Switch modes (IRQ->SVC)

    ; Store the SVC sp into r5 so the sp can be saved as is

    MOV     r5,sp

    ; Save the exception return address on the stack (PC).

    STMDB   r5!,{r4}

    ; Save r6-r14 on stack

    STMDB   r5!,{r6-r14}

    ; Switch back to using sp now that the original sp has been saved.

    MOV     sp,r5

    ; Get r0-r5 off exception stack and save on system stack

    LDMIA   r1!,{r5-r10}
    STMDB   sp!,{r5-r10}

    ; Save the SPSR on the system stack (CPSR)

    STMDB   sp!,{r3}

    ; Return to caller

    BX      r2

TCT_Normal_Save

    ; Determine if a thread was interrupted.

    LDR     r1,TCT_Current_Thread1          ; Pickup current thread ptr address
    LDR     r1,[r1]                         ; Pickup the current thread pointer
    CMP     r1,#0                           ; Is it NU_NULL?
    BEQ     TCT_Idle_Context_Save           ; If no, no real save is necessary

TCT_Thread_Save

    ; Yes, a thread was interrupted.  Save complete context on the
    ; thread's stack.

    ; Save the necessary exception registers into r1-r3

    MOV     r1,sp                           ; Put the exception sp into r1
    MOV     r2,lr                           ; Move the return address for the caller
                                            ; of this function into r2
    MRS     r3,spsr                         ; Put the exception spsr into r3

    ; Adjust the exception stack pointer for future exceptions

    ADD     sp,sp,#24                       ; sp will point to interrupt mask register(s)

    ; Switch CPU modes to save context on thread's stack

    MRS     r5,CPSR                         ; Pickup the current CPSR
    BIC     r5,r5,#MODE_MASK                ; Clear the mode bits
    ORR     r5,r5,#SUP_MODE                 ; Change to supervisor mode (SVC)
    MSR     CPSR,r5                         ; Switch modes

    ; Store the thread's sp into r5 so the sp can be saved as is

    MOV     r5,sp

    ; Save the exception return address on the stack (PC)

    STMDB   r5!,{r4}

    ; Save r6-r14 on stack

    STMDB   r5!,{r6-r14}

    ; Switch back to using sp now that the original sp has been saved.

    MOV     sp,r5

    ; Get r0-r5 off exception stack and save on thread's stack

    LDMIA   r1!,{r5-r10}
    STMDB   sp!,{r5-r10}

    ; Save the SPSR on the system stack (CPSR)

    STMDB   sp!,{r3}

    ; Save stack type to the task stack (1=interrupt stack)

    MOV     r1,#1
    STMDB   sp!,{r1}

    ; Save the thread's stack pointer in the control block.

    LDR     r1,TCT_Current_Thread1          ; Pickup current thread ptr address
    LDR     r3,[r1]                         ; Pickup current thread pointer
    STR     sp,[r3, #TC_STACK_POINTER]      ; Save stack pointer

    ; Switch to the system stack / system stack limit

    LDR     r1,TCT_System_Stack             ; Pickup address of stack pointer
    LDR     r3,TCT_System_Limit1            ; Pickup address of stack limit ptr
    LDR     sp,[r1]                         ; Switch to system stack
    LDR     r10,[r3]                        ; Setup system stack limit

    ; Determine if a time slice is active.  If so, the remaining time left on
    ; the time slice must be saved in the task's control block.

    LDR     r4,TCT_Slice_State              ; Pickup time slice state address
    LDR     r1,[r4]                         ; Pickup time slice state
    CMP     r1,#0                           ; Determine if time slice active
    BNE     TCT_Save_Exit                   ; If not, skip time slice reset

    ; Pickup the remaining portion of the time slice and save it
    ; in the task's control block.  */

    LDR     r5,TCT_Time_Slice               ; Pickup address of time slice left
    LDR     r5,[r5]                         ; Pickup remaining time slice
    STR     r5,[r3, #TC_CUR_TIME_SLICE]     ; Store remaining time slice

    ; Disable time-slice timer

    MOV     r1,#1                           ; Build disable time slice value
    STR     r1,[r4]                         ; Disable time slice


TCT_Save_Exit

    .if NU_TEST3_SUPPORT
    
    STMDB   sp!,{r0}
    BL      _Set_Context_Save_End
    LDMIA   sp!,{r0}
    
    .endif

    ; Return to caller ISR.

    BX      r2

TCT_Idle_Context_Save

    MOV     r2,lr                           ; Save lr in r2
    ADD     sp,sp,#24                       ; Adjust exception sp for future interrupts

    ; Switch to supervisor mode (SVC)

    MRS     r1,CPSR                         ; Pickup current CPSR
    BIC     r1,r1,#MODE_MASK                ; Clear the current mode
    ORR     r1,r1,#SUP_MODE                 ; Prepare to switch to supervisor mode (SVC)
    MSR     CPSR,r1                         ; Switch mode

    .if NU_TEST3_SUPPORT
    
    STMDB   sp!,{r0}
    BL      _Set_Context_Save_End
    LDMIA   sp!,{r0}
    
    .endif

    ; Return to caller ISR

    BX      r2


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Interrupt_Context_Restore
;*
;*  DESCRIPTION
;*
;*      This function restores the interrupt context if a nested
;*      interrupt condition is present.  Otherwise, this routine
;*      transfers control to the scheduling function.
;*
;*  CALLED BY
;*
;*      Application ISRs                Assembly language ISRs
;*      INT_IRQ_Shell                   IRQ Interrupt handler shell
;*      INT_FIQ_Shell                   FIQ Interrupt handler shell
;*
;*  CALLS
;*
;*      TCT_Schedule                    Thread scheduling function
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
;*      r0, r1, r2
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID    TCT_Interrupt_Context_Restore(VOID)

    .def    _TCT_Interrupt_Context_Restore
_TCT_Interrupt_Context_Restore


    .if NU_TEST3_SUPPORT
    
    BL      _Set_Context_Restore_Start
    
    .endif
    
    ; Decrement and check for nested interrupt conditions.

    LDR     r1,TCT_Int_Count                ; Pickup address of interrupt count
    LDR     r2,[r1]                         ; Pickup interrupt counter
    SUB     r2,r2,#1                        ; Decrement interrupt counter
    STR     r2,[r1]                         ; Store interrupt counter
    CMP     r2,#0                           ; Is interrupt count 0?
    BNE     TCT_Nested_Restore              ; If not 0, nested interrupt restore

    .if  NU_FIQ_SUPPORT

    ; Check for special nested case

    LDR     r1,TCT_Special_Nested1
    LDR     r2,[r1]
    CMP     r2,#1
    BNE     TCT_Not_Nested_Restore

    ; Clear Special nested flag

    MOV     r2,#0
    STR     r2,[r1]

    ; See if a thread was active during special nested situation

    LDR     r0,TCT_Current_Thread1          ; Pickup current thread ptr address
    LDR     r0,[r0]                         ; Pickup current thread pointer
    CMP     r0,#0                           ; Determine if a thread is active
    BEQ     TCT_Nested_Restore              ; If not, just do a nested restore

    ; A thread was active during this special situation
    ; Switch to this thread's stack

    LDR     sp,[r0,#TC_STACK_POINTER]

    ; Adjust the SP below the stack type

    ADD     sp,sp,#4

    .else

    B       TCT_Not_Nested_Restore
    
    .endif   ; NU_FIQ_SUPPORT

TCT_Nested_Restore

    LDR     r1,[sp], #4                     ; Pickup the saved CPSR

    MSR     SPSR,r1                         ; Place into saved SPSR

    ; Return to the point of interrupt.

    LDMIA   sp,{r0-r15}^

TCT_Not_Nested_Restore

    ; Clear TCD_Current_Thread

    MOV     r2,#0
    LDR     r1,TCT_Current_Thread1
    STR     r2,[r1]

    ; Reset the system stack pointer / system stack limit

    LDR     r1,TCT_System_Stack             ; Pickup address of stack pointer
    LDR     r2,TCT_System_Limit1            ; Pickup address of stack limit ptr
    LDR     sp,[r1]                         ; Switch to system stack
    LDR     r10,[r2]                        ; Setup system stack limit

    .if NU_TEST3_SUPPORT
    
    BL      _Set_Context_Restore_End
    
    .endif

    ; Return to scheduling loop
    
    B       _TCT_Schedule


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_Activate_HISR
;*
;*  DESCRIPTION
;*
;*      This function activates the specified HISR.  If the HISR is
;*      already activated, the HISR's activation count is simply
;*      decremented.  Otherwise, the HISR is placed on the appropriate
;*      HISR priority list in preparation for execution.
;*
;*  CALLED BY
;*
;*      Application LISRs
;*
;*  CALLS
;*
;*      None
;*
;*  INPUTS
;*
;*      r0 -    Pointer to HISR to activate
;*
;*  OUTPUTS
;*
;*      r0 -    Successful completion (0 = NU_SUCCESS)
;*
;*  REGISTERS MODIFIED
;*
;*      r1, r2, r3, r4
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;STATUS  TCT_Activate_HISR(TC_HCB *HISR)

    .def    _TCT_Activate_HISR
_TCT_Activate_HISR

    ; Save working registers

    STR     r4,[sp, #-4]!

    ; Lockout interrupts.

    MRS     r4,CPSR                         ; Pickup current CPSR
    ORR     r1,r4,#LOCKOUT                  ; Build interrupt lockout value
    MSR     CPSR,r1                         ; Lockout interrupts

    ; Determine if the HISR is already active.

    LDR     r1,[r0,#TC_ACTIVATION_COUNT]    ; Pickup current activation count
    CMP     r1,#0                           ; Is it the first activation?
    BEQ     TCT_First_Activate              ; Yes, place it on the correct list

    ; Increment the activation count.  Make sure that it does not go to zero

    ADDS    r1,r1,#1                        ; Increment the activation count
    MVNEQ   r1,#0                           ; If counter rolled-over reset
    STR     r1,[r0,#TC_ACTIVATION_COUNT]    ; Store all ones count

    ; Finished with activation

    B       TCT_Activate_Done

TCT_First_Activate

    ; Set the activation count to 1

    MOV     r1,#1                           ; Initial activation count
    STR     r1,[r0,#TC_ACTIVATION_COUNT]    ; Store initial activation count

    ; Get HISR Tails address

    LDR     r2,TCT_HISR_Tails               ; Pickup tail pointer base

    ; Pickup the HISR's priority and adjust for offset in HISR head / tail lists

    LDRB    r1,[r0,#TC_PRIORITY]            ; Pickup priority of HISR
    MOV     r1,r1,LSL #2                    ; Multiply by 4 to get offset

    ; Determine if there is something in the given priority list.

    LDR     r3,[r2,r1]                      ; Pickup tail pointer for priority
    CMP     r3,#0                           ; Is this first HISR at priority?
    BEQ     TCT_First_HISR                  ; No, append to end of HISR list

    ; Something is already on this list.  Add after the tail.

    STR     r0,[r3,#TC_ACTIVE_NEXT]         ; Setup the active next pointer
    STR     r0,[r2,r1]                      ; Setup the tail pointer
    B       TCT_Activate_Done               ; Finished with activate processing

TCT_First_HISR

    ; Nothing is on this list.

    LDR     r3,TCT_HISR_Heads               ; Pickup address of head pointers
    STR     r0,[r2,r1]                      ; Set tail pointer to this HISR
    STR     r0,[r3,r1]                      ; Set head pointer to this HISR

    ; Determine the highest priority HISR.

    LDR     r1,[r3]                         ; Pickup priority 0 head pointer
    CMP     r1,#0                           ; Is priority 0 active?
    LDREQ   r1,[r3,#4]                      ; If not, pickup priority 1 head
    CMPEQ   r1,#0                           ; Is priority 1 active?
    LDREQ   r1,[r3,#8]                      ; Else, must be priority 2 active

    ; Set execute HISR pointer to highest priority active HISR

    LDR     r0,TCT_Execute_HISR             ; Build address to execute HISR ptr
    STR     r1,[r0]                         ; Set execute HISR to highest priority

TCT_Activate_Done

    ; Restore interrupt lockout

    MSR     CPSR,r4

    ; Build NU_SUCCESS return value

    MOV     r0,#0

    ; Restore working registers

    LDR     r4,[sp], #4

    ; Return to caller

    BX      lr


;************************************************************************
;*
;*  FUNCTION
;*
;*      TCT_HISR_Shell
;*
;*  DESCRIPTION
;*
;*      This function is the execution shell of each and every HISR.  If
;*      the HISR has completed its processing, this shell routine exits
;*      back to the system.  Otherwise, it sequentially calls the HISR
;*      routine until the activation count goes to zero.
;*
;*  CALLED BY
;*
;*      TCT_Control_To_Thread
;*
;*  CALLS
;*
;*      hisr -> tc_entry                Actual entry function of HISR
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
;*      r0, r1, r2, r3, r4, r5, sp, CPSR
;*
;*  HISTORY
;*
;*      NAME            DATE            REMARKS
;*
;*      S. Nguyen       03/28/2005      Released version 1.15.1
;*
;************************************************************************
;VOID   TCT_HISR_Shell(VOID)

    .def    TCT_HISR_Shell
TCT_HISR_Shell

    ; Point at the HISR.

    LDR     r0,TCT_Current_Thread1          ; Build address of thread pointer
    LDR     r5,[r0]                         ; Pickup control block pointer

TCT_HISR_Loop

    ; Get global interrupt level

    LDR     r2,TCT_Int_Level                ; Pickup address of interrupt level
    LDR     r3,[r2]                         ; Pickup interrupt lockout level

    ; Restore interrupts to global level

    MRS     r1,CPSR                         ; Pickup current CPSR
    BIC     r1,r1,#LOCK_MSK                 ; Clear lockout bits
    ORR     r1,r1,r3                        ; Build new interrupt lockout
    MSR     CPSR,r1                         ; Setup CPSR appropriately

    ; Get address of HISR entry function

    LDR     r4,[r5,#TC_HISR_ENTRY]

    ; Check to see if HISR entry function is in THUMB mode

    TST     r4,#0x01

    ; Set return address based on THUMB mode

    ADRNE   lr,TCT_Thumb_Return
    ADREQ   lr,TCT_Arm_Return

    ; Jump to HISR entry function

    BX      r4                              ; Jump to sub-routine

    .state16
TCT_Thumb_Return

    ; Use BX instruction to switch back to ARM mode

    ADR     r4,TCT_Arm_Return
    BX      r4

    .state32
TCT_Arm_Return

    ; Lockout interrupts.

    MRS     r1,CPSR                         ; Pickup current CPSR
    ORR     r1,r1,#LOCKOUT                  ; Build interrupt lockout
    BIC     r1,r1,#THUMB_BIT                ; Ensure in ARM mode
    MSR     CPSR,r1                         ; Lockout interrupts

    ; On return, decrement the activation count and check to see if
    ; it is 0.  Once it reaches 0, the HISR should be made inactive.

    LDR     r0,[r5, #TC_ACTIVATION_COUNT]   ; Pickup current activation count
    SUBS    r0,r0,#1                        ; Subtract and set condition codes
    STR     r0,[r5, #TC_ACTIVATION_COUNT]   ; Store new activation count

    ; Keep looping until activation count is 0

    BNE     TCT_HISR_Loop

    ; Get addresses of HISR_Heads array, Execute HISR, and HISR_Tails

    LDR     r1,TCT_HISR_Heads               ; Pickup head pointers address
    LDR     r2,TCT_Execute_HISR             ; Build address to execute HISR ptr
    LDR     r4,TCT_HISR_Tails               ; Pickup tail pointers address

    ; Get the current HISR's priority and adjust for offset into
    ; HISR_Heads and HISR_Tails arrays

    LDRB    r3,[r5,#TC_PRIORITY]            ; Get HISR priority
    MOV     r3,r3,LSL #2                    ; Multiply by 4 to get offset

    ; Determine if this is the only HISR on the given priority list.

    LDR     r6,[r4,r3]                      ; Get tail pointer for this priority
    CMP     r6,r5                           ; Is this priority tail the same as
                                            ; the current HISR?
    BNE     TCT_More_HISRs                  ; If not, more HISRs at this
                                            ; priority

    ; The only HISR on the list.  Clean up the list and check for the
    ; highest priority HISR.

    MOV     r12,#0                          ; Clear r12
    STR     r12,[r1,r3]                     ; Set head pointer to NU_NULL
    STR     r12,[r4,r3]                     ; Set tail pointer to NU_NULL

    ; Determine the highest priority HISR.

    LDR     r4,[r1]                         ; Pickup priority 0 head pointer
    CMP     r4,#0                           ; Is there a HISR active?
    LDREQ   r4,[r1,#4]                      ; If not, pickup priority 1 pointer
    CMPEQ   r4,#0                           ; Is there a HISR active?
    LDREQ   r4,[r1,#8]                      ; If not, pickup priority 2 pointer

    ; Jump to set the new highest priority active HISR

    B       TCT_Set_Execute_HISR

TCT_More_HISRs

    ; Move to the next HISR on this thread's priority list

    LDR     r4,[r5,#TC_ACTIVE_NEXT]         ; Pickup next HISR to activate

    ; Move head pointer for this priority to next HISR on list

    STR     r4,[r1,r3]

TCT_Set_Execute_HISR

    ; Set-up TCD_Execute_HISR with highest priority HISR

    STR     r4,[r2]                         ; Setup execute HISR pointer

TCT_HISR_Exit

    ; Build fake return to the top of this loop.  The next time the HISR
    ; is activated, it will return to the top of this function.

    LDR     lr,TCT_HISR_Shell1              ; Pickup address of shell entry
    STMDB   sp!,{r4-r12,lr}                 ; Save minimal context of thread on
                                            ; the current stack

    ; Pickup the CPSR

    MRS     r1,CPSR                     

    ; Ensure interrupt bits are clear (enabled)

    BIC     r1,r1,#LOCK_MSK                 ; Clear initial interrupt lockout

    ; Save CPSR on stack

    STR     r1,[sp, #-4]!

    ; Save stack type (0 = solicited stack)

    MOV     r2, #0
    STR     r2,[sp, #-4]!

    ; Clear the current thread control block.

    LDR     r1,TCT_Current_Thread1          ; Pickup current thread ptr address
    STR     r2,[r1]                         ; Set current thread pointer to NU_NULL

    ; Save off the current stack pointer in the control block.

    STR     sp,[r5, #TC_STACK_POINTER]      ; Save the thread's stack pointer

    ; Switch to the system stack / system stack limit

    LDR     r1,TCT_System_Stack             ; Pickup address of stack pointer
    LDR     r2,TCT_System_Limit1            ; Pickup address of stack limit ptr
    LDR     sp,[r1]                         ; Switch to system stack
    LDR     r10,[r2]                        ; Setup system stack limit

    ; Transfer control to the main scheduling loop.

    B       _TCT_Schedule


;******************
;* THUMB SUPPORT  *
;******************

    .if NU_THUMB_SUPPORT

    ; The following section provides veneers utilized when C code calls
    ; the Nucleus PLUS assembly files when built in THUMB mode.  These
    ; labels allow inter-working between THUMB build C code and ARM built
    ; assembly code.  All PLUS assembly files are build in ARM mode.

;********************************
;* TCT_Protect Veneer for THUMB *
;********************************
    .def    $TCT_Protect
$TCT_Protect

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Protect


;*******************************************
;* TCT_Control_Interrupts Veneer for THUMB *
;*******************************************
    .def    $TCT_Control_Interrupts
$TCT_Control_Interrupts

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Control_Interrupts


;**************************************************
;* _TCT_Local_Control_Interrupts Veneer for THUMB *
;**************************************************
    .def    $TCT_Local_Control_Interrupts
$TCT_Local_Control_Interrupts

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Local_Control_Interrupts

;*******************************************
;* TCT_Restore_Interrupts Veneer for THUMB *
;*******************************************
    .def    $TCT_Restore_Interrupts
$TCT_Restore_Interrupts

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Restore_Interrupts

;*******************************************
;* TCT_Build_Task_Stack Veneer for THUMB *
;*******************************************
    .def    $TCT_Build_Task_Stack
$TCT_Build_Task_Stack

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Build_Task_Stack

;*******************************************
;* TCT_Build_HISR_Stack Veneer for THUMB *
;*******************************************
    .def    $TCT_Build_HISR_Stack
$TCT_Build_HISR_Stack

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Build_HISR_Stack

;*******************************************
;* TCT_Build_Signal_Frame Veneer for THUMB *
;*******************************************
    .def    $TCT_Build_Signal_Frame
$TCT_Build_Signal_Frame

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Build_Signal_Frame

;************************************
;* TCT_Check_Stack Veneer for THUMB *
;************************************
    .def    $TCT_Check_Stack
$TCT_Check_Stack

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Check_Stack

;*********************************
;* TCT_Schedule Veneer for THUMB *
;*********************************
    .def    $TCT_Schedule
$TCT_Schedule

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Schedule

;*******************************************
;* TCT_Control_To_Thread Veneer for THUMB *
;*******************************************
    .def    $TCT_Control_To_Thread
$TCT_Control_To_Thread

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Control_To_Thread


;*******************************************
;* TCT_Control_To_System Veneer for THUMB *
;*******************************************
    .def    $TCT_Control_To_System
$TCT_Control_To_System

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Control_To_System


;************************************
;* TCT_Signal_Exit Veneer for THUMB *
;************************************
    .def    $TCT_Signal_Exit
$TCT_Signal_Exit

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Signal_Exit


;***************************************
;* TCT_Current_Thread Veneer for THUMB *
;***************************************
    .def    $TCT_Current_Thread
$TCT_Current_Thread

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Current_Thread


;*****************************************
;* TCT_Set_Execute_Task Veneer for THUMB *
;*****************************************
    .def    $TCT_Set_Execute_Task
$TCT_Set_Execute_Task

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Set_Execute_Task


;**********************************
;* TCT_Unprotect Veneer for THUMB *
;**********************************
    .def    $TCT_Unprotect
$TCT_Unprotect

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Unprotect


;*******************************************
;* TCT_Unprotect_Specific Veneer for THUMB *
;*******************************************
    .def    $TCT_Unprotect_Specific
$TCT_Unprotect_Specific

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Unprotect_Specific


;********************************************
;* TCT_Set_Current_Protect Veneer for THUMB *
;********************************************
    .def    $TCT_Set_Current_Protect
$TCT_Set_Current_Protect

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Set_Current_Protect


;***************************************
;* TCT_Protect_Switch Veneer for THUMB *
;***************************************
    .def    $TCT_Protect_Switch
$TCT_Protect_Switch

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Protect_Switch


;*******************************************
;* TCT_Schedule_Protected Veneer for THUMB *
;*******************************************
    .def    $TCT_Schedule_Protected
$TCT_Schedule_Protected

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Schedule_Protected


;**************************************
;* TCT_Activate_HISR Veneer for THUMB *
;**************************************
    .def    $TCT_Activate_HISR
$TCT_Activate_HISR

    .state16
    BX      pc
    NOP

    .state32
    B       _TCT_Activate_HISR

    .endif  ; NU_THUMB_SUPPORT

    .end
