#ifndef _HWDPSAVE_H_
#define _HWDPSAVE_H_

              
/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "hwdapi.h"

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Data
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif
HwdPwrSaveModesT HwdPwrSaveGetMode( void );
void HwdPwrSaveModeSet( HwdPwrSaveModesT PwrSaveMode );
void HwdPwrSavePdmValue( uint32 PdmNumber, uint16 PdmValue );
void HwdPwrSaveAmpsModeSet( HwdPwrSaveModesT PwrSaveMode );
void HwdPwrSaveDspMClkCtrl( bool HwState );
void HwdPwrSaveDspVClkCtrl( bool HwState );
void HwdPwrSaveDspvDnldCtrl( bool HwState );
void HwdPwrSaveGpsUartClkCtrl( bool HwState );
void HwdPwrSaveDspAppsCtrl( bool HwState );
void HwdPwrOffBoard( void );
void HwdPwrSaveRxDcBiasCtrl(bool CdmaRxHwEnabled);
#ifdef __cplusplus
}
#endif




#endif

