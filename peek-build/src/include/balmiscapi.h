

#ifndef _BAL_MISC_API_H_
#define _BAL_MISC_API_H_

#include "sysdefs.h"
#include "balapi.h"
  
#define BAL_MISC_MAX_REG_TASK      (5)

/* the type for power up reason. definitions of reason should be same with defintions in bootldr.c */
typedef enum 
{
    BAL_POWER_UP_BY_POWER_KEY       = 0x0001,
    BAL_POWER_UP_BY_USB_CBALE       = 0x0004,
    BAL_POWER_UP_BY_CHARGE_CABLE    = 0x0010
}BalPowerUpFlagT;

#ifdef __cplusplus
extern "C"
{ 
#endif

bool   BalStatusGet( void );
bool   BalCalibrateModeGet(void);
RegIdT BalRegister( const BalEventHandlerT* EventHandlerP,
                    BalEventFunc CallBack );
void   BalUnRegister( const BalEventHandlerT* EventHandlerP,
                      RegIdT RegId );
void   BalProcessEvent( const BalEventHandlerT* EventHandlerP,
                        uint32 Event,
                        void* EventDataP );
void   BalWatchdogKick(void);         
uint32 BalGetSystemTimeInSec( void );
void   BalSysInteruptDisable( void );
void   BalPwrOffBoard( void );  
void*  BalMalloc( uint32 size );
void   BalFree(void* Ptr);
uint32 BalGetAvailableSpace(void);
bool   BalMiscIramRead( uint8 *BufferP, uint8 offset, uint8 *SizeP);
bool   BalMiscIramWrite( uint8 *BufferP, uint8 offset, uint8 *SizeP);
uint8  BalMiscIramBufferSizeGet(void);
uint16  BalGetPowerUpFlag(void );

void   BalMiscLocSessionStart( void );
void   BalMiscPowerDown( BalPowerDownIdT Signal );
RegIdT BalMiscRegister( BalEventFunc CallBack );
void   BalMiscUnregister( RegIdT RegId );
void   BalMiscNamRequest( void );
void   BalMiscGetEriVersion(BalEriVersionNumberMsgT *MsgP);
bool   BalMiscPowerKeyExist(void);

BalRptCpEventT BalPktSvcState( void );

void BalGetFirmwareVersion (void);
void BalTestModeStartRefurbishAging (BalRfPllBandT Band, uint16 Channel);
void BalTestModeStopRefurbishAging (void);
void BalTestModeRefurbishAgingChangeTxPwr (bool Increment);
int  BalTimeZoneGet(void);
bool BalIsDaySavingActive(void);
bool BalMiscNamGet(NamNumber NamSeg, PswIs95NamT *pNamBuffer);
#ifdef __cplusplus
}
#endif

#endif

/*****************************************************************************
 * $Log: valmiscapi.h $
 * Revision 1.1  2007/10/29 10:53:07  binye
 * Initial revision
 * Revision 1.1  2007/10/12 15:31:08  dsu
 * Initial revision
 * Revision 1.4  2007/01/31 13:47:47  dsu
 * add power key judge
 * Revision 1.3  2007/01/23 10:00:48  dsu
 * add timezone api
 * Revision 1.2  2006/12/30 16:35:49  dsu
 * move misc api from valapi.h to valmiscapi.h
 * Revision 1.2  2006/12/30 15:48:58  dsu
 * move misc api from valapi.h to valmiscapi.h
 * Revision 1.1  2006/12/30 15:02:33  dsu
 * Initial revision
******************************************************************************/

