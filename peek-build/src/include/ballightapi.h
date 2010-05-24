/*=====================================================================================
FILE NAME   : vallightapi.h
MODULE NAME : Val

  
GENERAL DESCRIPTION
	
Copyright (c) 2008-2009 by BTC. All Rights Reserved.
=======================================================================================
Revision History
	  
Modification                   Tracking
Date              Author       Number       Description of changes
----------   --------------   ---------   --------------------------------------
2008-04-02     Broad Zou                       create the file
		
=====================================================================================*/



#ifndef __BALLIGHTAPI_H__
#define __BALLIGHTAPI_H__

#include "light_drv_if.h"
#include "exeapi.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BAL_LIGHT_FORCE_LIGHTOFF_ID          0xFFFF  /* force to turn off light or stop flash */
#define BAL_LIGHT_OPERATE_FAILED_ID          0xFFFE  /* invalid operate id  */
#define BAL_LIGHT_FORCE_OPERATEID          0xFFFF  /* force to turn off light or stop flash */


typedef enum
{
  BAL_LIGHT_KEYPAD,
  BAL_LIGHT_LCD, 
  BAL_LIGHT_LCD2,
  BAL_LIGHT_RED,
  BAL_LIGHT_GREEN,
  BAL_LIGHT_BLUE,

  /*user may modify the device name*/
  BAL_LIGHT_DEV_A,
  BAL_LIGHT_DEV_B,
  
  BAL_LIGHT_MAX
} BalLightIdT;

typedef  enum 
{
  BAL_LIGHT_LEVEL_CURRENT = -1,      /*do not modify*/
  BAL_LIGHT_LEVEL_0,
  BAL_LIGHT_LEVEL_1,
  BAL_LIGHT_LEVEL_2,
  BAL_LIGHT_LEVEL_3,
  BAL_LIGHT_LEVEL_4,
  BAL_LIGHT_LEVEL_5,
  BAL_LIGHT_LEVEL_6,
  BAL_LIGHT_LEVEL_7,
  BAL_LIGHT_LEVEL_8,  
  BAL_LIGHT_LEVEL_9,
 
  BAL_LIGHT_LEVEL_MAX
} BalLightLevelT;

typedef enum
{
  BAL_LIGHT_FLASH_TEST = 0,          /*do NOT modify*//* Factory test */
  BAL_LIGHT_FLASH_CALL,              /* Incoming call */
  BAL_LIGHT_FLASH_MSG,               /* New message */
  BAL_LIGHT_FLASH_RED,
  BAL_LIGHT_FLASH_GREEN,
  BAL_LIGHT_FLASH_BLUE,

  BAL_LIGHT_FLASH_USER0,
  BAL_LIGHT_FLASH_USER1,
  BAL_LIGHT_FLASH_USER2,
  BAL_LIGHT_FLASH_USER3,  
  
  BAL_LIGHT_FLASH_MAX
} BalLightFlashTypeT;

/*=================================================================================
FUNCTION:
BalLightInit
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
init the light.
ARGUMENTS PASSED: void
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalLightInit(void);

/*=================================================================================
FUNCTION:
BalLightOn
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
switch the Light on according lightId.
ARGUMENTS PASSED: BalLightIdT
RETURN VALUE: uint16
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
uint16 BalLightOn(BalLightIdT LightId, uint16 OnTime, bool IsDimming , bool IsForce );

/*=================================================================================
FUNCTION:
BalLightOff
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
switch the Light off according lightId.
ARGUMENTS PASSED: BalLightIdT
RETURN VALUE: uint16
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
uint16 BalLightOff(BalLightIdT LightId, uint16 OperateID, bool Dimming);

/*=================================================================================
FUNCTION:
FlashTimerHandler
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
the flash timer call back function.
ARGUMENTS PASSED: uint32
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void FlashTimerHandler(uint32 TimerId);
void FlashTimerHandlerNew(uint32 TimerId);

/*=================================================================================
FUNCTION:
BalLightFlashOn
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
switch the light flash on and begin a flash timer.
ARGUMENTS PASSED: BalLightFlashTypeT,uint16
RETURN VALUE: uint16
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
uint16 BalLightFlashOn(BalLightFlashTypeT FlashType, uint16 FlashTime);
uint16 BalLightFlashOnStatus(BalLightFlashTypeT FlashType, uint16 FlashTime, uint16 Times);

/*=================================================================================
FUNCTION:
BalLightFlashOff
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
switch the light flash off and clear a timer.
ARGUMENTS PASSED: BalLightFlashTypeT
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void BalLightFlashOff(uint16 OperateID);


/*=================================================================================
FUNCTION:
BalLightBrightness
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
to control light brightness level.
ARGUMENTS PASSED: BalLightIdT, BalLightLevelT
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalLightBrightness(BalLightIdT LightId, BalLightLevelT Level);


/*=================================================================================
FUNCTION:
BalGetLightStatus
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
to get the light status according lightId.
ARGUMENTS PASSED: BalLightIdT, BalLightLevelT,bool
RETURN VALUE: uint16
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void BalGetLightStatus(BalLightIdT LightId, BalLightLevelT *peLevel, bool *pfLedStatus);

/*=================================================================================
FUNCTION:
BalLightFlashInProgress
CREATE DATE:
2008-4-2
AUTHOR: 
Broad Zou
DESCRIPTION:
get the led flash status.
ARGUMENTS PASSED: BalLightFlashTypeT
RETURN VALUE: uint16
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalLightFlashInProgress(BalLightFlashTypeT FlashType);

/*=================================================================================
FUNCTION:
BalLcdScreenOn
CREATE DATE:
2008-5-9
AUTHOR: 
Broad Zou
DESCRIPTION:
switch the LCD screen on
ARGUMENTS PASSED: void
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalLcdScreenOn();

/*=================================================================================
FUNCTION:
BalLcdScreenOff
CREATE DATE:
2008-5-9
AUTHOR: 
Broad Zou
DESCRIPTION:
switch the LCD screen off
ARGUMENTS PASSED: void
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalLcdScreenOff();

#ifdef __cplusplus
}
#endif

#endif  /* __BALLIGHTAPI_H__ */




