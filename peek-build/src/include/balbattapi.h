/*=====================================================================================
FILE NAME   : valbattapi.h
MODULE NAME : ASL

  
GENERAL DESCRIPTION
	
Copyright (c) 2008-2009 by BTC. All Rights Reserved.
=======================================================================================
Revision History
	  
Modification                   Tracking
Date              Author       Number       Description of changes
----------   --------------   ---------   --------------------------------------
2008-04-02     Broad Zou                       create the file
		
=====================================================================================*/


#ifndef __BALBATTAPI_H__
#define __BALBATTAPI_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "sysdefs.h"
#include "bmbattapi.h"
#include "balmiscapi.h"

typedef uint8 VibratorStrengthT;

#define VibratorStrengthLevel1 1
#define VibratorStrengthLevel2 2
#define VibratorStrengthLevel3 3
#define VibratorStrengthLevel4 4

typedef enum
{
    BALBATT_CHARGE_STOP = 0,
    BALBATT_CHARGE_PAUSE,
    BALBATT_CHARGE_ONGOING,
    BALBATT_CHARGE_FAILED,
    BALBATT_CHARGE_COMPLETE,
    BALBATT_CHARGE_PREPARE 
} BalBattChargeStateT;

typedef enum  
{
    BAL_BATT_LEVEL_NO_BATTERY = BM_BATT_LEVEL_NO_BATTERY,
    BAL_BATT_LEVEL_SHUTDOWN = BM_BATT_LEVEL_SHUTDOWN,
    BAL_BATT_LEVEL_TOO_LOWEST = BM_BATT_LEVEL_TOO_LOWEST,//Add by #zouyonghang
    BAL_BATT_LEVEL_TOO_LOW = BM_BATT_LEVEL_TOO_LOW,
    BAL_BATT_LEVEL_LOW = BM_BATT_LEVEL_LOW,
    BAL_BATT_LEVEL_MID = BM_BATT_LEVEL_MID,
    BAL_BATT_LEVEL_MIDHIGH = BM_BATT_LEVEL_MIDHIGH,
    BAL_BATT_LEVEL_HIGH = BM_BATT_LEVEL_HIGH,
    BAL_BATT_LEVEL_FULL = BM_BATT_LEVEL_FULL,
    BAL_BATT_LEVEL_NOT_INIT = BM_BATT_LEVEL_NOT_INIT
} BalBattLevelT;

typedef enum
{
    BAL_BATT_CHARGE_STATE = 0,
    BAL_BATT_CAPACITY,
    BAL_BATT_VOLT
} BalBattMsgT;

/*=================================================================================
FUNCTION:
BalPowerOff
CREATE DATE:
2008-4-3
AUTHOR: 
Broad Zou
DESCRIPTION:
power off and shut down.
ARGUMENTS PASSED: void
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void BalPowerOff();

/*=================================================================================
FUNCTION:
HwdBattNotify
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
it is a callback to hwd for hwd notify val batt states changed.
ARGUMENTS PASSED: BalBattMsgT
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void HwdBattNotify(void *pvData);

/*=================================================================================
FUNCTION:
BalBattRegister
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
for app to register a notify callback to val level.
ARGUMENTS PASSED: BalEventFunc
RETURN VALUE: RegIdT
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
RegIdT BalBattRegister(BalEventFunc EventFunc );

/*=================================================================================
FUNCTION:
BalBattUnRegister
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
for app to unregister a notify callback to val level.
ARGUMENTS PASSED: RegIdT
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalBattUnRegister( RegIdT RegId );

/*=================================================================================
FUNCTION:
BalBattLevelFromHwdBattLevel
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
it is a map from hwd batt level to val batt level.
ARGUMENTS PASSED: uint8
RETURN VALUE: BalBattLevelT
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
static BalBattLevelT BalBattLevelFromHwdBattLevel(uint8 hwdBattLevel);

/*=================================================================================
FUNCTION:
BalGetBattLevel
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
get the current batt level.
ARGUMENTS PASSED: void
RETURN VALUE: BalBattLevelT
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
BalBattLevelT BalGetBattLevel(void);

/*=================================================================================
FUNCTION:
BalGetChargeStatus
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
get the current charger status.
ARGUMENTS PASSED: void
RETURN VALUE: BalBattChargeStateT
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
BalBattChargeStateT BalGetChargeStatus(void);

/*=================================================================================
FUNCTION:
BalBattGetVolt
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
get the current batt volt.
ARGUMENTS PASSED: void
RETURN VALUE: uint16
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
uint16 BalBattGetVolt(void);

/*=================================================================================
FUNCTION:
BalBattSetGaugeInterval
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
set the charger status notify interval.
ARGUMENTS PASSED: uint16
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalBattSetGaugeInterval(uint16 Seconds);

/*=================================================================================
FUNCTION:
BalChargerExist
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
to judge the charger exist or not.
ARGUMENTS PASSED: uint16
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalChargerExist(void);
 

/*=================================================================================
FUNCTION:
BalVibratorControl
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
to control vibratro working mode.
ARGUMENTS PASSED: bool, uint32.
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void BalVibratorControl(bool on, uint32 interval);

/*=================================================================================
FUNCTION:
BalVibratorOn
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
swtich on the vibrator.
ARGUMENTS PASSED: void
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void BalVibratorOn();

/*=================================================================================
FUNCTION:
BalVibratorOff
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
swtich off the vibrator.
ARGUMENTS PASSED: void
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void BalVibratorOff();

/*=================================================================================
FUNCTION:
VibrateTimerHandler
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
the callback when a timer coming
ARGUMENTS PASSED: bool, uint32.
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void VibrateTimerHandler(uint32 TimerId);

 /*=================================================================================
FUNCTION:
BalVibratorStrengthSet
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
to set the vibrator strength.
ARGUMENTS PASSED: VibratorStrengthT.
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void BalVibratorStrengthSet(VibratorStrengthT vibratorLevel);

 /*=================================================================================
FUNCTION:
BalVibratorTimeOut
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
swtich off the vibrator and just for once according interval.
ARGUMENTS PASSED: uint32
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void BalVibratorTimeOut(uint32 interval);
void BalVibratorTimeOutClose();

/*=================================================================================
FUNCTION:
VibrateTimerOutHandler
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
the callback when a timer coming for timeout.
ARGUMENTS PASSED: TimerId.
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void VibrateTimerOutHandler(uint32 TimerId);
void BalVibratorCreatTimer(uint32 interval); //added by jason for CIT
void BalVibratorClearTimer();//added by jason for CIT

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BALBATTAPI_H__ */



