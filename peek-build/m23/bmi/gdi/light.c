/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-PS
|  Modul   :  DRV_LT
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  This Module defines the G23 light emitting driver. 
+----------------------------------------------------------------------------- 
 

	Apr 26, 2005   REF : CRR 30627 xpradipg
	Bug :	Replace the ABB APIs with Audio Service APIs
	Fix :	Remove the LT_ function calls 	
*******************************************************************************/ 
/*
	June 03, 2005 REF: GSM-ENH-31636 xpradipg
	Description:	Change the board ID for Isample to 71 and add new defination
					for the CALLISTO with baord ID 70
	Solution:		Add the definition of ALLISTO with board ID 70 and change the
					board Id to 71 for Isample

	CRR 28825:  xpradipg - 11 Feb 2005
	Description:Extension of GDI-for display of ISample and code cleanup
	Solution: The code replication for D_Sample and E_sample are removed and a new
	flag is added for the ISample currently the BOARD ID is set to 60 for ISample.

*/
#ifndef DRV_LT_C
#define DRV_LT_C
#endif

/*==== INCLUDES ===================================================*/
#if defined (NEW_FRAME)

#include <string.h>
#include "typedefs.h"
#include "gdi.h"
#include "light.h"

#else

#include <string.h>
#include "stddefs.h"
#include "gdi.h"
#include "light.h"

#endif
/*==== EXPORT =====================================================*/

/*==== VARIABLES ==================================================*/
UBYTE  backlight_status = LIGHT_STATUS_OFF;
/*==== FUNCTIONS ==================================================*/
#if defined (_TMS470)
#ifndef FF_MMI_SERVICES_MIGRATION
EXTERN void LT_Disable       (void);
EXTERN void LT_Enable        (void);
EXTERN void LT_Level         (UBYTE level);
#endif
#else

LOCAL  void LT_Disable       (void);
LOCAL  void LT_Enable        (void);
LOCAL  void LT_Level         (UBYTE level);

#endif

/*==== CONSTANTS ==================================================*/
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_LT                     |
| STATE   : code                ROUTINE : light_Init                 |
+--------------------------------------------------------------------+

  PURPOSE : The function initializes the internal data of the driver.
            The function returns DRV_INITIALIZED if the driver has
            already been initialized and is ready to be used or is 
            already in use. In case of an initialization failure, 
            i.e. the driver cannot be used, the function returns
            DRV_INITFAILURE.

*/

GLOBAL UBYTE light_Init (void)
{
  backlight_status = LIGHT_STATUS_OFF;

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_LT                     |
| STATE   : code                ROUTINE : light_Exit                 |
+--------------------------------------------------------------------+

  PURPOSE : The function is called when the driver functionality is
            not longer needed. The function de-allocates the
            resources.

*/

GLOBAL void light_Exit (void)
{
  backlight_status = LIGHT_STATUS_OFF;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_LT                     |
| STATE   : code                ROUTINE : light_SetStatus            |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to change the status of a specific
            light emitting device supported by this driver. The device is
            identified by the parameter in_DeviceID. Depending on
            the capabilities of the device, the parameter in_NewStatus
            has different meanings.
            
*/

GLOBAL UBYTE light_SetStatus (UBYTE in_DeviceID,
                              UBYTE in_NewStatus)
{
  if (in_DeviceID EQ LIGHT_DEVICE_BACKLIGHT)
  {
    backlight_status = in_NewStatus;

    switch (in_NewStatus)
    {
      case LIGHT_STATUS_OFF:
		light_setBacklightOff();
//	Apr 26, 2005   REF : CRR 30627 xpradipg
#ifndef FF_MMI_SERVICES_MIGRATION		
        LT_Disable ();
#endif        
        break;
      default:
		light_setBacklightOn();
//	Apr 26, 2005   REF : CRR 30627 xpradipg
#ifndef FF_MMI_SERVICES_MIGRATION
        LT_Enable ();
        LT_Level (255);
#endif
        break;
    }
    return DRV_OK;
  }
  else
    return DRV_INVALID_PARAMS;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_LT                     |
| STATE   : code                ROUTINE : light_GetStatus            |
+--------------------------------------------------------------------+

  PURPOSE : This function retrieves the status of a specific light
            emitting device supported by the driver. If the status
            of a specified device could be retrieved, the function
            returns DRV_OK. If the specified device is unknown, the 
            function returns DRV_INVALID_PARAMS.
            
*/

GLOBAL UBYTE light_GetStatus (UBYTE   in_DeviceID,
                              UBYTE * in_StatusPtr)
                              
{
  if (in_DeviceID EQ LIGHT_DEVICE_BACKLIGHT)
  {
    *in_StatusPtr = backlight_status;
    return DRV_OK;
  }
  else
    return DRV_INVALID_PARAMS;
}


#if defined (WIN32)

LOCAL  void LT_Disable       (void)
{
}

LOCAL  void LT_Enable        (void)
{
}

LOCAL  void LT_Level         (UBYTE level)
{
}


#endif

/*******************************************************************************

Code below added for D-sample backlight

*******************************************************************************/
//June 03, 2005 REF: GSM-ENH-31636 xpradipg
//CRR 28825:  xpradipg - 11 Feb 2005
#if ((BOARD == 40) || (BOARD == 41) || (BOARD == 43) || (BOARD == 70) || (BOARD == 71))
#define DSAMPLE_COLOUR
#include "lls/lls_api.h"
#else
/* Procedure stiubs added instead of disabling calls to the procedures. */
/* So we will get duplicate procedures if the actual LLS procedures are present */
#define LLS_BACKLIGHT 0x02
int lls_switch_off(unsigned char equipment)
{
}

int lls_switch_on(unsigned char equipment)
{
}

#endif

enum {
	BACKLIGHT_OFF,
	BACKLIGHT_ON,
	BACKLIGHT_IDLE1
};
UBYTE backLightState= BACKLIGHT_OFF;
/*******************************************************************************

 $Function:  	setBacklightOn

 $Description:	switches backlight on. Called on powerup or on a key press.
 				Should also be called on incoming call, SMS etc ?
 
 $Returns:    	none.

 $Arguments:	none.


*******************************************************************************/
void light_setBacklightOn( void )
{
	if (backLightState == BACKLIGHT_OFF)
	{
//#ifdef  DSAMPLE_COLOUR
	  	lls_switch_on(LLS_BACKLIGHT); //Switch backlight on	
//#endif
	}
	backLightState = BACKLIGHT_ON;
}
/*******************************************************************************

 $Function:  	setBacklightOff

 $Description:	switches backlight off. 
 
 $Returns:    	none.

 $Arguments:	none.

*******************************************************************************/
void light_setBacklightOff( void )
{
	if ((backLightState == BACKLIGHT_ON) ||
	   (backLightState == BACKLIGHT_IDLE1))
	{
		backLightState = BACKLIGHT_OFF;
//#ifdef  DSAMPLE_COLOUR
	  	lls_switch_off(LLS_BACKLIGHT); //Switch backlight on	
//#endif
	}
}
/*******************************************************************************

 $Function:  	setBacklightIdle

 $Description:	switches backlight off (on second call). Called when the 1 minute timer expires.
 				If no key press since the last call, the backlight is switched off.
 
 $Returns:    	none.

 $Arguments:	none.

*******************************************************************************/
void light_setBacklightIdle( void )
{
	if (backLightState == BACKLIGHT_ON)
	{
		backLightState = BACKLIGHT_IDLE1;		
	}
	else
	{
		light_setBacklightOff();
	}
}

