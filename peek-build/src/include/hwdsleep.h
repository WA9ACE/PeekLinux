

#include "sysdefs.h"

/*------------------------------------------------------------------------
 * exported data
 *------------------------------------------------------------------------*/
#define MAX_32K_TICKS ((uint32)1 << 24)

#define MICROSEC_PER_TICK 31

#define MicroSecTo32KTicks(MicroSec) \
   ( ( ((MicroSec) + (MICROSEC_PER_TICK/2)) / MICROSEC_PER_TICK) + 1 )
   

/*------------------------------------------------------------------------
 * exported functions
 *------------------------------------------------------------------------*/

extern void		HwdSleepInit( void );
extern uint32	HwdSleepRead32KCnt( void );
extern void		HwdSleepSetRtosTimerMark( void );
extern uint32	HwdSleepGetRtosTimerAdj( void );
extern void		HwdSleepOverLisr( void );
extern void		HwdSleepVetoTimerFunction( uint32 TimerId );
extern void     HwdSleepWakeupEventRegister(uint16 WakeupEventsMask);
extern void     HwdSleepWakeupEventCancel(uint16 WakeupEventsMask);
extern void     HwdSleepWakeupEnable(bool Enable);

/* implemented in hwdsleep_iram.c */
extern void    HwdDeepSleepActivate( bool ServiceISR );
extern void    Hwd32kDelay(uint32 delay_cnt);
extern void    HwdMsDelay(uint16 ms);
extern uint32  Hwd32kGetTimeWithOld(uint32 oldCnt);


