/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-PS
|  Modul   :  DRV_PWR
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
|  Purpose :  This Module defines the power management device driver
|             for the G23 protocol stack.
|             
|             This driver is used to control all power related functions 
|             such as charger and battery control. The driver does support
|             multiple devices and therefore no open and close functionality
|             is supported. The driver can be configured to signal different
|             state transitions, for example battery level has reached the
|             "battery low" level. This is done by setting an OS signal or 
|             calling a specified call-back function.
+----------------------------------------------------------------------------- 
$History: Power.c
	Dec 05, 2006 REF:OMAPS00106691 a0393213(R.Prabakar)
	Description:compiler error is arising in simulation build since timer.h is not included in simulation build
	Solution    :flag off timer.h and its dependencies in gdi

      Oct 30,2006 ER:OMAPS00091029 x0039928(sumanth)
      Bootup time measurement
      
 	Apr 17, 2006    REF: ER OMAPS00075178 x0pleela
   	Description: When the charger connect to phone(i-sample), the charger driver works, but the MMI and App don't work
   	Solution: Registering to LCC events and calling pwr_register at pwr_init() 

*/ 

#ifndef DRV_PWR_C
#define DRV_PWR_C
#endif

/*==== INCLUDES ===================================================*/
#if defined (NEW_FRAME)

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gdi.h"
#include "pwr.h"

#else

#include <string.h>
#include "stddefs.h"
#include "gdi.h"
#include "pwr.h"

#endif
#ifdef _SIMULATION_
#undef USE_LCC  0   /* For L23-ACI windows simulation */
#else
#define USE_LCC  1 //x0pleela 10 Apr, 2006 ER: OMAPS00075178
#endif
/*==== EXPORT =====================================================*/
#if defined (_TMS470_NOT_YET)
EXTERN void BAT_Init         (void (*pwr_batlevel)(UBYTE level),
                              void (*pwr_batstatus)(UBYTE status));
#else
LOCAL  void BAT_Init         (void (*pwr_batlevel)(UBYTE level),
                              void (*pwr_batstatus)(UBYTE status));
#endif
#if !defined(USE_LCC)
LOCAL void pwr_batlevel      (UBYTE level);
#endif
LOCAL void pwr_batstatus     (UBYTE status);

/*==== VARIABLES ==================================================*/
drv_SignalCB_Type  pwr_signal_callback = NULL;
#ifdef USE_LCC  //x0pleela 10 Apr, 2006 ER: OMAPS00075178

#include "lcc/lcc_api.h"

T_PWR_MMI_INFO_IND_EVENT lcc_event;
T_RV_RETURN_PATH  lcc_cb={0, NULL};
#else
UBYTE              pwr_act_level;
UBYTE              pwr_act_status;
#endif //x0pleela 10 Apr, 2006 ER: OMAPS00075178
pwr_Status_Type    pwr_Status;
pwr_DCB_Type       pwr_DCB;
/*==== VARIABLES ==================================================*/

/*==== FUNCTIONS ==================================================*/

/*==== CONSTANTS ==================================================*/
#ifdef USE_LCC //x0pleela 10 Apr, 2006 ER: OMAPS00075178

#ifdef FF_EM_MODE //x0pleela 10 Apr, 2006 ER: OMAPS00075178
/*
msgid:

    MMI_CHG_PLUG_IND     = 1,
    MMI_CHG_UNPLUG_IND   = 2,
    MMI_CHG_START_IND    = 3,
    MMI_CHG_STOP_IND     = 4,
    MMI_CHG_UNKNOWN_IND  = 5,
    MMI_BAT_UNKNOWN_IND  = 6,
    MMI_CHG_FAILED_IND   = 7,
    MMI_BAT_SUPERVISION_INFO_IND = 10
*/
T_PWR_MMI_INFO_IND_EVENT curBatInfo={{0,},0,0,0,0};

#endif

// Callback function used for testing MMI reporting
static void lcc_cb_function(void *ptr) 
{
	UBYTE               calculated_level;
	T_PWR_MMI_INFO_IND_EVENT *event;
	drv_SignalID_Type   signal_params;

	TRACE_FUNCTION("lcc_cb_function()");

	event = (T_PWR_MMI_INFO_IND_EVENT *)ptr;
#ifdef FF_EM_MODE
	memcpy(&curBatInfo, event, sizeof(T_PWR_MMI_INFO_IND_EVENT));
#endif

  if (event->Vbat  <  3500)
    calculated_level = 0;
  else if ( (event->Vbat  < 3650) AND (event->Vbat  >=  3500) )
    calculated_level = 1;
  else if ( ( event->Vbat  < 3800) AND (event->Vbat  >= 3650) )
    calculated_level = 2;
  else if ( ( event->Vbat  < 3950) AND (event->Vbat  >= 3800) )
    calculated_level = 3;
  else 
    calculated_level = 4;


//	pwr_Status.ChargeLevel = event->header->;
	if(event->header.msg_id==MMI_CHG_PLUG_IND)
	{
		TRACE_EVENT("MMI_CHG_PLUG_IND");
		pwr_Status.ChargeLevel=calculated_level; //event->Cbat; // 0..100 [%]
		pwr_Status.Status=PWR_CHARGER_PLUG;
		signal_params.SignalType  = PWR_SIGTYPE_CHARGER;
		signal_params.UserData    = (void*)&pwr_Status;
	}
	else if(event->header.msg_id==MMI_CHG_UNPLUG_IND)
	{
		TRACE_EVENT("MMI_CHG_UNPLUG_IND");
		pwr_Status.ChargeLevel=calculated_level; //event->Cbat; // 0..100 [%]
		pwr_Status.Status=PWR_CHARGER_UNPLUG;
		signal_params.SignalType  = PWR_SIGTYPE_CHARGER;
		signal_params.UserData    = (void*)&pwr_Status;
	}
	else if(event->header.msg_id==MMI_CHG_START_IND)
	{
		TRACE_EVENT("MMI_CHG_START_IND");
		pwr_Status.ChargeLevel=calculated_level; //event->Cbat; // 0..100 [%]
		pwr_Status.Status=PWR_CHARGER_BEGIN;
		signal_params.SignalType  = PWR_SIGTYPE_CHARGER;
		signal_params.UserData    = (void*)&pwr_Status;
	}
	else if(event->header.msg_id==MMI_CHG_STOP_IND)
	{
		TRACE_EVENT("MMI_CHG_STOP_IND");
		pwr_Status.ChargeLevel=calculated_level; //event->Cbat; // 0..100 [%]
		pwr_Status.Status=PWR_CHARGER_STOP;
		signal_params.SignalType  = PWR_SIGTYPE_CHARGER;
		signal_params.UserData    = (void*)&pwr_Status;
	}
	else //MMI_BAT_SUPERVISION_INFO_IND
	{
		TRACE_EVENT("MMI_BAT_SUPERVISION_INFO_IND");
		if (calculated_level EQ pwr_Status.BatteryLevel)
		return;

		pwr_Status.BatteryLevel=calculated_level; //event->Cbat; // 0..100 [%]
		pwr_Status.Status=event->Tbat;
		signal_params.SignalType  = PWR_SIGTYPE_BATLEVEL;
		signal_params.UserData    = (void*)&pwr_Status;
	}

	if (pwr_signal_callback NEQ NULL)
		(*pwr_signal_callback)(&signal_params);
}


#endif
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : pwr_Init                   |
+--------------------------------------------------------------------+

  PURPOSE : The function initializes the driver´s internal data. 
            The function returns DRV_OK in case of a successful
            completition. The function returns DRV_INITIALIZED if
            the driver has already been initialized and is ready to
            be used or is already in use. In case of an initialization
            failure, which means the that the driver cannot be used,
            the function returns DRV_INITFAILURE.
            
*/

GLOBAL UBYTE pwr_Init (drv_SignalCB_Type in_SignalCBPtr)
{
#ifdef USE_LCC //x0pleela 10 Apr, 2006 ER: OMAPS00075178

TRACE_FUNCTION ("pwr_init()");

  pwr_signal_callback = in_SignalCBPtr;    /* store call-back function */
  pwr_DCB.RangeMin    = 10;                /* 10 Percent               */
  pwr_DCB.RangeMax    = 100;               /* 100 Percent              */
  pwr_DCB.Steps       = 4;                 /* 4 Steps                  */

  lcc_cb.callback_func=lcc_cb_function;
#if (BOARD == 71)
  pwr_register(&lcc_cb, &lcc_event);
#endif
  
#else
  pwr_signal_callback = in_SignalCBPtr;    /* store call-back function */
  pwr_DCB.RangeMin    = 10;                /* 10 Percent               */
  pwr_DCB.RangeMax    = 100;               /* 100 Percent              */
  pwr_DCB.Steps       = 4;                 /* 4 Steps                  */

  pwr_Status.Status       = 0;
  pwr_Status.BatteryLevel = 0;   
  pwr_Status.ChargeLevel  = 0;

  /*
   * Initialise TI driver with internal callback functions
   *
   * pwr_batlevel is called after change of battery level
   * pwr_batstatus is called after change of external or charger
   * unit
   *
   */
  BAT_Init (pwr_batlevel, pwr_batstatus);
#endif //x0pleela 10 Apr, 2006 ER: OMAPS00075178
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : pwr_Exit                   |
+--------------------------------------------------------------------+

  PURPOSE : The function is used to indicate PWR that the driver 
            and its functionality isn´t needed anymore.

*/

GLOBAL void pwr_Exit (void)
{
  pwr_signal_callback = NULL;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : pwr_SetSignal              |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to define a single signal or multiple
            signals that is/are indicated to the process when the event
            identified in the signal information data type as SignalType
            occurs.
            To remove a signal, call the function pwr_ResetSignal().
            If one of the parameters of the signal information data is
            invalid, the function returns DRV_INVALID_PARAMS.
            If no signal call-back function has been defined at the
            time of initilization the driver returns DRV_SIGFCT_NOTAVAILABLE.

*/

GLOBAL UBYTE pwr_SetSignal (drv_SignalID_Type * in_SignalIDPtr)
{
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : pwr_ResetSignal            |
+--------------------------------------------------------------------+

  PURPOSE : The function is used to remove a single or multiple signals
            that has previously been set. The signals that are removed
            are identified by the Signal Information Data element called
            SignalType. All other elements of the Signal Information Data 
            must be identical to the signal(s) that is/are to be 
            removed. If the SignalID provided can not be found, the 
            function returns DRV_INVALID_PARAMS.
            If no signal call-back function has beed defined at the 
            time of initialization, the driver returns DRV_SIGFCT_NOTAVAILABLE.

*/

GLOBAL UBYTE pwr_ResetSignal (drv_SignalID_Type * in_SignalIDPtr)
{
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : pwr_SetConfig              |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to configure the driver. 
            If any value of this configuration is out of range or
            invalid in combination with any other value of the
            configuration, the function returns DRV_INVALID_PARAMS.
            Call the pwr_GetConfig() function to retrieve the drivers 
            configuration.

*/

GLOBAL UBYTE pwr_SetConfig (pwr_DCB_Type * in_DCBPtr)
{
  memcpy (&pwr_DCB, in_DCBPtr, sizeof (pwr_DCB_Type));
  pwr_Status.BatteryLevel = 0;   
  pwr_Status.ChargeLevel  = 0;

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : pwr_GetConfig              |
+--------------------------------------------------------------------+

  PURPOSE : The function is used to retrieve the configuration of 
            the driver. The configuration is returned in the driver
            control block to which the pointer out_DCBPtr points.
            If the driver is not configured, the function returns
            DRV_NOTCONFIGURED.
            Call the pwr_SetConfig() function to configure the driver.
            
*/

GLOBAL UBYTE pwr_GetConfig (pwr_DCB_Type * out_DCBPtr)
{
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : pwr_GetStatus              |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to retrieve the status of the driver
            respectively the power unit.
            In case of a successful completion the driver returns 
            DRV_OK and the current status of the driver in the buffer
            out_StatusPtr points to.
            In case the driver is not configured yet, it returns
            DRV_NOTCONFIGURED. In this case the contents of the
            buffer out_StatusPtr is invalid.
            In case out_StatusPtr equals NULL the driver returns 
            DRV_INVALID_PARAMS.
            
*/

GLOBAL UBYTE pwr_GetStatus (pwr_Status_Type * out_StatusPtr)
{
  if ( out_StatusPtr EQ NULL )
  {
    return DRV_INVALID_PARAMS;
  }
  else
  {
    out_StatusPtr->Status       = pwr_Status.Status;
    out_StatusPtr->BatteryLevel = pwr_Status.BatteryLevel;
    out_StatusPtr->ChargeLevel  = pwr_Status.ChargeLevel;
  }

  return DRV_OK;
}



/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : pwr_BatLevel               |
+--------------------------------------------------------------------+

  PURPOSE : This function is called by the low level driver after
            change of battery level.
*/
#if !defined(USE_LCC)
LOCAL void pwr_batlevel (UBYTE level)
{
  UBYTE               calculated_level=0;
  drv_SignalID_Type   signal_params;

TRACE_FUNCTION("pwr_batlevel()");
  pwr_Status.ChargeLevel = level;

  TRACE_EVENT_P1("pwr_Status.ChargeLevel:%d", pwr_Status.ChargeLevel);

  if (level <= pwr_DCB.RangeMin)
    calculated_level = 0;
  if (level >= pwr_DCB.RangeMax)
    calculated_level = pwr_DCB.Steps +1;
  if (level > pwr_DCB.RangeMin AND
      level < pwr_DCB.RangeMax)
  {
    level -= pwr_DCB.RangeMin;
    calculated_level = ((level * pwr_DCB.Steps) / 
                        (pwr_DCB.RangeMax - pwr_DCB.RangeMin))+1;
  }

  if (calculated_level EQ pwr_Status.BatteryLevel)
    return;

  signal_params.SignalType  = PWR_SIGTYPE_BATLEVEL;
#if defined (NEW_FRAME)
  signal_params.UserData    = (void*)&pwr_Status;
#else
  signal_params.SignalValue = 0;
  signal_params.UserData    = (ULONG)&pwr_Status;
#endif
  pwr_Status.BatteryLevel   = calculated_level;

  if (pwr_signal_callback NEQ NULL)
    (*pwr_signal_callback)(&signal_params);
}
#endif
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : pwr_PowerOffMobile             |
+--------------------------------------------------------------------+

  PURPOSE : This function is switching off the mobile
*/

GLOBAL UBYTE pwr_PowerOffMobile (void)
{

/* power off HW is not applicable in simulation */


#if !defined (WIN32)

	/* power-off the board / HW */
	AI_Power(0);

//#endif /* _TARGET_ */
#endif /* _TMS470 */

    return 1;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : get_boot_timel               |
+--------------------------------------------------------------------+

  PURPOSE : this function is used to get bootup time measurements
*/
#ifndef _SIMULATION_
void get_boot_time(unsigned int * boot_time)
{
	TRACE_FUNCTION("get_boot_time()");
	Bsp_get_boot_time(boot_time);
}		

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_PWR                    |
| STATE   : code                ROUTINE : boot_time_snapshot               |
+--------------------------------------------------------------------+

  PURPOSE : this function is used to mark end of a bootup event
*/
void boot_time_snapshot(BootTimeEvent event)
{
	TRACE_EVENT_P1("boot_time_snapshot %d", event);
	Bsp_Boot_Time_Take_Snapshot(event);
}
#endif

