#ifndef SYSAPI_H
#define SYSAPI_H


#include "sysdefs.h"
#include "exeapi.h"
#include "hwdapi.h"

/* Define functions normally accessed from the C library */
#include <string.h>
#define SysMemset   memset
#define SysMemcpy   memcpy
#define SysMemcmp   memcmp

/* Define variables used for System Timer interrupts */
#define ST_CPINT_125   0x0001
#define ST_CPINT_250   0x0002
#define ST_CPINT_375   0x0004
#define ST_CPINT_500   0x0008
#define ST_CPINT_625   0x0010
#define ST_CPINT_750   0x0020
#define ST_CPINT_875   0x0040
#define ST_CPINT_1000  0x0080
#define ST_CPINT_1125  0x0100
#define ST_CPINT_1250  0x0200
#define ST_CPINT_1375  0x0400
#define ST_CPINT_1500  0x0800
#define ST_CPINT_1625  0x1000
#define ST_CPINT_1750  0x2000
#define ST_CPINT_1875  0x4000
#define ST_CPINT_0     0x8000

#define ST_CPINT_ALL   0xffff

#define SYS_MAX_20MS_FRAME_COUNT    (20 * 8)   /* Convert frame to counts    */

#define INVALID_SYSTIME_SECS 0xFFFFFFFF

/* structure used for finer system time resolution (36 bits) */
typedef PACKED struct
{
   uint32 MostSignificant32Bits; 
   uint8  LeastSignificant4Bits;
}SysSystemTimeFineT;

typedef PACKED struct
{
   bool           Immediate;  /* do immdediate or at action time */
   SysSystemTimeT Time;       /* action time in 20 ms */
} SysActionTimeT;

typedef enum 
{
   SYS_FRAME_SIZE_20MS=0,
   SYS_FRAME_SIZE_26MS=1
}SysFrameSizeT;

typedef enum
{
   SYS_TX_SIGNAL_PERIOD_5MS,
   SYS_TX_SIGNAL_PERIOD_10MS,
   SYS_TX_SIGNAL_PERIOD_20MS
}SysTxSignalPeriodT;

typedef enum
{
   SYS_TX_SIGNAL_START_FRAME_SUBFRAME_0, /* Tx signal starts at 0 ms into 20 ms frame */
   SYS_TX_SIGNAL_START_FRAME_SUBFRAME_1, /* Tx signal starts at 5 ms into 20 ms frame */
   SYS_TX_SIGNAL_START_FRAME_SUBFRAME_2, /* Tx signal starts at 10 ms into 20 ms frame */
   SYS_TX_SIGNAL_START_FRAME_SUBFRAME_3  /* Tx signal starts at 15 ms into 20 ms frame */
}SysTxSignalStartFrameT;

typedef enum
{
   SYS_TIME_MODE_CDMA,
   SYS_TIME_MODE_AMPS
}SysTimeModeT;   

typedef enum
{
   SINGLE_PCG_EVENT,
   PERIODIC_PCG_EVENT
} PcgEventTypeT;

typedef struct
{
   uint32 Lower32;
   uint8  Upper6;
   uint8  LpSec;
   int8   LtmOff;
   bool   DayLt;
}SysTimeDataT;


/*****************************************************************************
 
  FUNCTION NAME: SysIntDisable

  DESCRIPTION:

    This routine disables ARM IRQ or IFQ interrupts. These 
    interrupts can be disabled individually or togther. This
    routine must be used in conjunction with SysIntEnable.

  PARAMETERS:

    IntType - Type of interrupt to disable as defined by SysIntT
    
  RETURNED VALUES:

    None

*****************************************************************************/
#define SysIntDisable(IntType) ExeInterruptDisable(IntType)


/*****************************************************************************
 
  FUNCTION NAME: SysIntEnable

  DESCRIPTION:

    This routine enables ARM IRQ or IFQ interrupts. These 
    interrupts can be enabled individually or togther. This
    routine must be used in conjunction with SysIntDisable.

  PARAMETERS:

    IntType - Type of interrupt to enable as defined by SysIntT
    
  RETURNED VALUES:

    None

*****************************************************************************/
#define SysIntEnable(IntType) ExeInterruptEnable()


/*****************************************************************************
 
  FUNCTION NAME: SysChipCountRead

  DESCRIPTION:

    This routine takes a "snapshot" of the System Time counter synchronized
    with the 9MHz clock domain.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    uint32 value containing the 20-bit system time counter value

  NOTE: Once the system time counter is locked, there is a required 12 CP clock
  cycle delay before a valid count value can be read (hence the 12 NOPs).

*****************************************************************************/
#if !((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))
#define SysChipCountDelay() \
                            __asm \
                            {\
                               NOP\
                               NOP\
                               NOP\
                               NOP\
                               NOP\
                               NOP\
                               NOP\
                               NOP\
                               NOP\
                               NOP\
                               NOP\
                               NOP\
                            }\                               

#define SysChipCountRead() \
                        { HwdWrite(HWD_ST_CNT_LOCK, 0x0001);\
                          SysChipCountDelay();\
                          (uint32) (HwdRead(HWD_ST_CNT_LO) | (HwdRead(HWD_ST_CNT_HI) << 16));\
                        }
#endif

#ifdef SYS_DEBUG_NO_ACTION_STATS

/* Define Sys spy routine as empty macro if no debug is set */

#define SysSpyActionTimeStats()

#else

/*****************************************************************************
 
  FUNCTION NAME: SysSpyActionTimeStats

  DESCRIPTION:

     This routine collects the action time stats and puts them in a Spy.

  PARAMETERS:
    
     None

  RETURNED VALUES:

     None

*****************************************************************************/
extern void SysSpyActionTimeStats(void);

#endif


/*****************************************************************************
 
  FUNCTION NAME: SysResetCpProcessor

  DESCRIPTION:

    This routine disables resets the CP processor by jumping
    to the __main routine in the Nucleus RTOS.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void SysResetCpProcessor(void);

/*****************************************************************************
 
  FUNCTION NAME: SysJumpToBootCode

  DESCRIPTION:

    This routine jump to the boot code.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void SysJumpToBootCode(void);

/*****************************************************************************
 
  FUNCTION NAME: SysJumpToCpCode

  DESCRIPTION:

    This routine jump to the CP code.

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
extern void SysJumpToCpCode(void);



/*****************************************************************************
 
  FUNCTION NAME: SysTimeRegister

  DESCRIPTION:

    This routine registers either a signal OR a call back function associated 
    with an action time in the system time queue. The system time queue contains an order 
    list of all action times.

    NOTE: The Routine function pointer MUST be set to NULL to cause a signal to be sent.

  PARAMETERS:
      
    ActionTime - Action time to insert into the time queue
    TaskId     - Task ID of task to send signal.
    Signal     - Signal to send.
    Routine    - Call back routine when action time is reached

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimeRegister(uint32 ActionTime, ExeTaskIdT TaskId, ExeSignalT Signal, void (*Routine)(uint32) );


/****************************************************************************

  FUNCTION NAME: SysDelayUs

  DESCRIPTION:

    This routine executes a software loop for the input number of
    microseconds.  The maximum delay accepted by this routine is 30  
    microseconds.  Note that interrupts are NOT disabled internally by
    this routine so the actual delay time may be extended if this routine
    is interrupted.

  C FUNCTION PROTOTYPE:

    void SysDelayUs(uint32 DelayTimeInUs);

  PARAMETERS:

    DelayTimeInUs: Number of microseconds this routine should wait before exiting.

  RETURNED VALUES:

    None.

*****************************************************************************/
extern void SysDelayUs(uint32 DelayTimeInUs);


/****************************************************************************

  FUNCTION NAME: SysInLisr

  DESCRIPTION:

    This routine checks the currently executing thread and determines if
    it is in the LISR range of Thread Ids.
    is interrupted.

  PARAMETERS:

    None.

  RETURNED VALUES:

    TRUE if executing in LISR, FALSE if not.

*****************************************************************************/
extern bool SysInLisr(void);
       

/*****************************************************************************
 
  FUNCTION NAME: SysTimeRegisterCancel

  DESCRIPTION:

    This routine cancels the signal OR callback that is registered at a specified 
    action time.
    
  PARAMETERS:

    ActionTime - Action time of registered signal to cancel.
    TaskId     - Task ID of task to send signal.
    Signal     - Signal to send.
    Routine    - Call back routine when action time is reached

  RETURNED VALUES:

    Boolean flag indicating operation status.  
    Returns TRUE if action cancelled successfully.
    Returns FALSE if specified action not found in registration queue.

  Note: Input paramters must match those used when SysTimeRegister was called
  originally.
   
*****************************************************************************/
extern bool SysTimeRegisterCancel(uint32 ActionTime, ExeTaskIdT TaskId, ExeSignalT Signal, void (*Routine)(uint32) );


/*****************************************************************************
 
  FUNCTION NAME: SysPcgEventRegister

  DESCRIPTION:

    This routine registers either a signal OR a call back function associated 
    with a PCG trigger with the system timer interrupts. The PCG event queue contains
    the number of events based on which PCG is checked.
 
  PARAMETERS:
      
    PcgMask    - System Timer event mask to identify which PCG(s) to trigger event
    EventType  - Continuous periodic or single PCG event types.
    TaskId     - Task ID of task to send signal.
    Signal     - Signal to send.
    Routine    - Call back routine when action time is reached

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysPcgEventRegister(uint16 PcgMask, PcgEventTypeT EventType, ExeTaskIdT TaskId, ExeSignalT Signal, void (*Routine)(uint32) );


/*****************************************************************************
 
  FUNCTION NAME: SysTimePcgEventRegister

  DESCRIPTION:

    This routine registers either a signal OR a callback function associated
    with a PCG trigger at an defined action time. This is done with a special
    callback at the set action time which then logs the PCG event in the
    appropriate PCG event queue.

  PARAMETERS:
      
    ActionTime - Action time to insert PCG event callback routine into the time queue
    PcgMask    - System Timer event mask to identify which PCG(s) to trigger event
    EventType  - Continuous periodic or single PCG event types.
    TaskId     - Task ID of task to send signal.
    Signal     - Signal to send.
    Routine    - Call back routine when action time is reached

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimePcgEventRegister(uint32 ActionTime, uint16 PcgMask, PcgEventTypeT EventType, ExeTaskIdT TaskId, ExeSignalT Signal, void (*Routine)(uint32) );


/*****************************************************************************
 
  FUNCTION NAME: SysPcgEventRegisterCancel

  DESCRIPTION:

    This routine cancels the signal OR callback that is registered at a specified
    PCG event.

    NOTE: Input parameters must match those used when SysEventRegister was called
    originally. This function would normally be used to clear a PCG event which
    is generated continuously/periodically - single PCG events clear themselves.

  PARAMETERS:
      
    PcgMask    - System Timer event mask to identify which PCG(s) to clear event
    TaskId     - Task ID of task to send signal.
    Signal     - Signal to send.
    Routine    - Call back routine when action time is reached

  RETURNED VALUES:

    Boolean flag indicating operation status.  
    Returns TRUE if action cancelled successfully.
    Returns FALSE if specified action not found in PCG event queue.

*****************************************************************************/
extern bool SysPcgEventRegisterCancel(uint16 PcgMask, ExeTaskIdT TaskId, ExeSignalT Signal, void (*Routine)(uint32) );


/*****************************************************************************
 
  FUNCTION NAME: SysTimeSet

  DESCRIPTION:

    This routine set the system time variable to a new value.

  PARAMETERS:
      
    NewSysTime - A new value for system time.

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimeSet(uint32 NewSysTime);


/*****************************************************************************
 
  FUNCTION NAME: SysTimeGet

  DESCRIPTION:

    This routine gets a copy of the system time variable.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

    uint32 - System time variable is returned.
 
*****************************************************************************/
extern uint32 SysTimeGet(void);


/*****************************************************************************
 
  FUNCTION NAME: SysTimeGetFine

  DESCRIPTION:

    This routine returns the current system time with finer resolution, 
    nominally 1.25 ms resolution.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

   Structure containing 36 bit resolution system time.
    
*****************************************************************************/
extern SysSystemTimeFineT SysTimeGetFine(void);


/*****************************************************************************
 
  FUNCTION NAME: SysTimeInquire

  DESCRIPTION:

    This routine returns the number of entries in the system time
    queue.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

    uint16 - Number of entries in System time queue
 
*****************************************************************************/
extern uint16 SysTimeInquire(void);


/*****************************************************************************
 
  FUNCTION NAME: SysTimeLisr

  DESCRIPTION:

    This routine is the Lisr for system time. It increments
    the system time variable and then determines if an action
    is equal to the new system time. If so the HISR routine
    is activated.

  PARAMETERS:
      
    StIntSrc - System Time Block Interrupt Source register contents at time of
	           interrupt.

  RETURNED VALUES:

    None.
    
*****************************************************************************/
extern void SysTimeLisr(uint32 StIntSrc);


/*****************************************************************************
 
  FUNCTION NAME: SysTimeHisr

  DESCRIPTION:

    This routine is the Hisr for system time. It scans the entire system
    time queue for action time entries that match the current system time. 
    If there is a match the call back routine is envoked and then the entry
    is removed from the queue.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimeHisr(void);



/*****************************************************************************
 
  FUNCTION NAME: SysPcgEventHisr

  DESCRIPTION:

    This routine is the Hisr for the PCG event queue.
    
  PARAMETERS:
      
    None.

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysPcgEventHisr(void);



/*****************************************************************************
 
  FUNCTION NAME: SysInit

  DESCRIPTION:

    This routine initializes system time variables.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysInit(void);



/*****************************************************************************
 
  FUNCTION NAME: SysTimeEnable

  DESCRIPTION:

    This routine enables the system time counter in either CDMA or AMPS mode.
    the system time services are not available until this routine is called
    for the first time.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimeEnable(void);


/*****************************************************************************
 
  FUNCTION NAME: SysEnableTxSignal

  DESCRIPTION:

    This routine controls the sending of the Tx signal to any tasks registered
	using the SysRegisterTxSignal routine.


  PARAMETERS:

    TxSigPeriod - Sets rate at which Tx signal occurs.
	TxSigStartFrame - Sets which sub-20ms frame the first Tx signal occurs in.
	                  Parameter is relative to the 20 ms frame.  This is ignored
					  for a 20 ms frame.  Note this is valid for the first Tx
					  Signal; after the first one interrupts occur at each (sub)frame.

    Note: Tx signal interrupt is configured for a constant number of ms before
	the frame.
  
  
  RETURNED VALUES:

    None.

*****************************************************************************/
extern void SysEnableTxSignal(SysTxSignalPeriodT TxSigPeriod, 
                              SysTxSignalStartFrameT TxSigStartFrame);


/*****************************************************************************
 
  FUNCTION NAME: SysDisableTxSignal

  DESCRIPTION:

    This routine disables the sending of the Tx signal to any tasks registered
	using the SysRegisterTxSignal routine.


  PARAMETERS:

    None.


  RETURNED VALUES:

    None.

*****************************************************************************/
extern void SysDisableTxSignal(void);


/*****************************************************************************
 
  FUNCTION NAME: SysEnableTimerSync

  DESCRIPTION:

    This routine enables/disables the synchronization of the CTS/DTS timers  
	with the System Timer.  When enabled the System Timer block controls the
	rollover of the CTS Timer.  On disable, the CTS timer is set to rollover
    at 20 ms, but not synced to the System Timer.  

  PARAMETERS:

    Enable - TRUE for enable, FALSE for disable.

  RETURNED VALUES:

    None.

*****************************************************************************/
void SysEnableTimerSync( bool Enable );

/*****************************************************************************
 
  FUNCTION NAME: SysSetFrameSize

  DESCRIPTION:

    This routine sets the System Timer block hardware to produce interrupts 
    corresponding to either a 20 ms or 26 ms period.

  PARAMETERS:

    FrameSize - Either 20 or 26 ms frames.

  RETURNED VALUES:

    None.

*****************************************************************************/
extern void SysSetFrameSize( SysFrameSizeT FrameSize );


/*****************************************************************************
 
  FUNCTION NAME:   SysCtsCallbackClear

  DESCRIPTION:     Clears out all instances of the specified function from the
                   CTS callback routine .

  PARAMETERS:      CallbackPtr - routine to clear out of callback queue

  RETURNED VALUES: None

*****************************************************************************/
extern void SysCtsCallbackClear(void (*CallbackPtr) (void));


/*****************************************************************************
 
  FUNCTION NAME:   SysCtsRegisterCallback

  DESCRIPTION:     Provides a callback after the specified delay. The delay
                   is generated by using the hardware frame counter to 
                   determine the current time, and then programming a single
                   shot strobe to occur after the specified delay.
                   
                   Notes:
                      1) Assumes frame time is 20 ms. 
                      2) Contains overrun checking.

  PARAMETERS:      DelayMs     - Delay in ms.
                   CallbackPtr - Function to call after the delay.

  RETURNED VALUES: Flag indicating if a callback was scheduled successfully.
                   TRUE: Callback scheduled.
                   FALSE: NO CTS Timers available, callback NOT scheduled.

*****************************************************************************/
extern bool SysCtsRegisterCallback(uint32 DelayMs, void (*CallbackPtr) (void) );


/*****************************************************************************
 
  FUNCTION NAME:   SysCtsRegisterCallbackAbs

  DESCRIPTION:     Provides a callback when the hardware frame counter reaches
                   the specified value. This is implemented by programming a 
                   single shot strobe to occur at the specified count.
                   
                   Notes:
                      1) Assumes frame time is 20 ms. 
                      2) Contains overrun checking.
                      3) Uses same CTS as SysCtsRegisterCallbackDly
                      

  PARAMETERS:      AbsCount    - Frame counter value to generate callback
                   CallbackPtr - Function to call after the delay.

  RETURNED VALUES: None

*****************************************************************************/
bool SysCtsRegisterCallbackAbs(uint32 AbsCount, void (*CallbackPtr) (void) );


/*****************************************************************************
 
  FUNCTION NAME: SysTimeReset

  DESCRIPTION:

    This routine re-initializes system time variables and clears all callback
    queues.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimeReset(void);

/*****************************************************************************
 
  FUNCTION NAME: SysTimeAdcMeasEnable

  DESCRIPTION:

    This function enables or disables the system time interrupts used for 
    Aux ADC measurements.
    
  PARAMETERS:
      
    Enable - Enable/Disable flag.
    AdcMeasCallbackPtr - Pointer to ADC Measurement function to call on ST interrupt.
                         Only required if enabled.
                  
  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimeAdcMeasEnable(bool Enable, void (*AdcMeasCallbackPtr)(uint32));

/*****************************************************************************
 
  FUNCTION NAME: SysTimeIsEnabled

  DESCRIPTION:

    This function returns a flag indicating if the system timer is enabled and
    generating interrupts.
    
  PARAMETERS:
      
    None.
                      
  RETURNED VALUES:

    TRUE if the System timer is enabled.  FALSE if disabled.
 
*****************************************************************************/
extern bool SysTimeIsEnabled(void);


/*****************************************************************************
 
  FUNCTION NAME: SysTimeModeSet

  DESCRIPTION:

    This routine sets the system time counter mode to either CDMA or AMPS mode.
    The system time services are not available until this routine is called
    for the first time.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimeModeSet(SysAmpsCdmaSelectT Mode);
 

/*****************************************************************************
 
  FUNCTION NAME: Sys32KHzGet

  DESCRIPTION:

    This routine returns the last system time frame aligned read of the
    32KHz counter.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

    uint32 - 32KHz counter at the last system time interrupt is returned.
 
*****************************************************************************/
extern uint32 Sys32KHzGet(void);


/*****************************************************************************
 
  FUNCTION NAME: SysTimeDataGet

  DESCRIPTION:

    This routine retrieves the current system time data including the 38 bit
    system time in 20 ms units and the other information most recently received
    in the sync channel message.

  PARAMETERS:
      
    TimeDataP - Pointer to Time data structure to fill in with current data.

  RETURNED VALUES:

    bool - TRUE if valid data was filled in.  FALSE if valid data does NOT exist.
 
*****************************************************************************/
extern bool SysTimeDataGet(SysTimeDataT *TimeDataP);


/*****************************************************************************
 
  FUNCTION NAME: SysTimeSync

  DESCRIPTION:

    This routine is used to synchronize the system time variables received on
    the sync channel with the data stored by SYS.

  PARAMETERS:
      
    LpSec             : Leap seconds as received in sync channel message.
    LtmOff            : Local time offset as received in sync channel message
                        (units of signed 30 min).
    DayLt             : Daylight savings time boolean flag.
    
  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimeSync( uint8 *FullSystemTimeP, 
                         uint8  LpSec,
                         int8   LtmOff,
                         bool   DayLt );


/*****************************************************************************
 
  FUNCTION NAME: SysTimeUpper6BitsGet

  DESCRIPTION:

    This routine retrieves the upper 6 bits of the current system time kept in
    20 ms units, making the full system time 38 bits.

  PARAMETERS:
    
    None.

  RETURNED VALUES:

    Upper 6 bits of system time (right justified in the 8 bits returned).
 
*****************************************************************************/
extern uint8 SysTimeUpper6BitsGet(void);



/*****************************************************************************
 
  FUNCTION NAME: SysTimeFullGet

  DESCRIPTION:

    This routine retrieves the current system time in 80 ms units.
    It is returned in the same format as that received on the sync channel,
    36 bits contained in a five byte array with the data left justified.
    
    

  PARAMETERS:
    
    FullSystemTimeP - Pointer to 5 byte array for return data.

  RETURNED VALUES:

    Format:
    FullSystemTimeP[0] => Current System Time, bits 35-28
    FullSystemTimeP[1] => Current System Time, bits 27-20
    FullSystemTimeP[2] => Current System Time, bits 19-12
    FullSystemTimeP[3] => Current System Time, bits 11-4
    FullSystemTimeP[4] => Current System Time, bits 3-0
                          (contained in bits 7-4 of this byte)

 
*****************************************************************************/
extern void SysTimeFullGet(uint8 *FullSystemTimeP);


/*****************************************************************************
 
  FUNCTION NAME: SysTimeFullSet

  DESCRIPTION:

    This routine sets the current system time given the input system time
    is in the same format as that received on the sync channel, i.e.,
    36 bits contained in a five byte array with the data left justified.
    
    

  PARAMETERS:
    
    FullSystemTimeP - Pointer to 5 byte input system time array.

    Format:
    FullSystemTimeP[0] => Current System Time, bits 35-28
    FullSystemTimeP[1] => Current System Time, bits 27-20
    FullSystemTimeP[2] => Current System Time, bits 19-12
    FullSystemTimeP[3] => Current System Time, bits 11-4
    FullSystemTimeP[4] => Current System Time, bits 3-0
                          (contained in bits 7-4 of this byte)

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimeFullSet(uint8 *FullSystemTimeP);


/*****************************************************************************
 
  FUNCTION NAME: SysTime32BitTimeCalc

  DESCRIPTION:

    This routine calculates the system time in 20 ms, given the system
    time in sync channel message format (36 bits in 80 ms units, left 
    justified) and returns the lower 32 bits of the calculated value.   
    
    

  PARAMETERS:
    
    FullSystemTimeP - Pointer to 5 byte system time array.
    Format:
    FullSystemTimeP[0] => Current System Time, bits 35-28
    FullSystemTimeP[1] => Current System Time, bits 27-20
    FullSystemTimeP[2] => Current System Time, bits 19-12
    FullSystemTimeP[3] => Current System Time, bits 11-4
    FullSystemTimeP[4] => Current System Time, bits 3-0
                          (contained in bits 7-4 of this byte)

  RETURNED VALUES:

    Lower 32 bits of system time in 20 ms units.
 
*****************************************************************************/
extern uint32 SysTime32BitTimeCalc(uint8 *FullSystemTimeP);

/*****************************************************************************

  FUNCTION NAME: SysTimeAdjust

  DESCRIPTION:

    This routine adjusts the system time variables to new values given a delta.

  PARAMETERS:
      
    AdjustTicks - System timer ticks to adjust (always positive).

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void SysTimeAdjust(uint32 AdjustTicks );

/*****************************************************************************
 
	FUNCTION NAME:	SysTime2Secs
   
	DESCRIPTION:	Converts system time data into number of seconds since 
                  the beginning of System Time.  System time data includes
                  local time offset, leap seconds, and daylight savings time 
                  indicator.
                  
	PARAMETERS:		SysTimeData

	RETURNS:		   uint32 - Number of seconds since start of System Time.
                           Returns 0xFFFFFFFF if not valid.
                           

  ***********************************************************************
  * NOTE: THIS FUNCTION WILL NOT RETURN CORRECT VALUES FOR SYSTEM TIMES *
  *       CORRESPONDING TO A DATE LATER THAN FEB 12, 2067.              *
  *       SEE EMBEDDED COMMENTS IN THIS FUNCTION FOR MORE DETAILS.      *
  ***********************************************************************
         
*****************************************************************************/
uint32 SysTime2Secs( SysTimeDataT* SysTimeData );



#endif

