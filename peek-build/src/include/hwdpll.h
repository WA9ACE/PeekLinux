#ifndef _HWD_PLL_H_
#define _HWD_PLL_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysapi.h"
#include "exeapi.h"
#include "hwdapi.h"
#include "hwdrfapi.h"

/*----------------------------------------------------------------------------
 Global Defines  Macros
----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/

/* PLL function pointer table structure */
typedef struct 
{
   void (*RxIfPllSetup)(uint32 Band);
   void (*RxRfPllSetup)(uint32 Band);
   void (*TxIfPllSetup)(uint32 Band);
   void (*TxIfPllIdle)(uint32 Band);
   void (*RfPllPowerDown)(uint32 Band);
   void (*RfPllPowerUp)(uint32 Band);
   void (*RfPllChannelSet)(HwdRfPllBandT PllBand, uint32 Channel);
   void (*RfTstPllFlush)(uint32 PllSel, uint32* pData);   
}PllFuncPtrTblT;

/*----------------------------------------------------------------------------
 Global Data
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/
extern bool HwdPllIsLockDetectEnabled (void);
extern void HwdPllLockDetectRfTuned(bool Enable);
extern void HwdPllLockPulseSet (uint32 PulseWidth);
extern void HwdPllLockInit (void);
extern void HwdPllLockLost (void);
extern void HwdPllLockTxEnable(bool Enable);
extern void HwdPllPwrCtrl(bool	Status);
extern void HwdPllInit( void );
extern void HwdRfTstPllFlush(uint32 PllSel, uint32* pData);

extern void  HwdTxIfPllSetup(void);
extern void  HwdTxIfPllIdle(void);
extern void  HwdRfPllPowerDown(void);
extern void  HwdRfPllPowerUp(void);
extern void  HwdRfPllBandClear(void);




#endif
