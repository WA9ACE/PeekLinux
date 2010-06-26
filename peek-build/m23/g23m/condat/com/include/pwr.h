/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM (6301)
|  Modul   :  
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
|  Purpose :  Types definitions for the power driver
|             .
+----------------------------------------------------------------------------- 
$History: Pwr.h
	Dec 05, 2006 REF:OMAPS00106691 a0393213(R.Prabakar)
	Description:compiler error is arising in simulation build since timer.h is not included in simulation build
	Solution    :flag off timer.h and its dependencies in gdi

      Oct 30,2006 ER:OMAPS00091029 x0039928(sumanth)
      Bootup time measurement
      
	Apr 17, 2006    REF: ER OMAPS00075178 x0pleela
   	Description: When the charger connect to phone(i-sample), the charger driver works, but the MMI and App don't work
   	Solution: Defined macros for charging events

*/ 

#ifndef PWR_H
#define PWR_H

#ifndef _SIMULATION_
#include "timer.h" /*OMAPS00091029 x0039928(sumanth)*/
#endif

/*
 * Power signals
 */
#define PWR_SIGTYPE_EXTPOWER 1
#define PWR_SIGTYPE_CHARGER  2
#define PWR_SIGTYPE_BATLEVEL 3

/*
 * Power Status
 */
#define PWR_BATTERY_ON       		0  // use battery power //x0pleela 10 Apr, 2006 ER: OMAPS00075178
#define PWR_EXTPOWER_ON      1
#define PWR_CHARGER_ON         6 //x0pleela 11 Apr, 2006 changed the value from 2 to 6

//x0pleela 10 Apr, 2006 ER: OMAPS00075178
#define PWR_CHARGER_BEGIN       	2 // charger begin 
#define PWR_CHARGER_STOP     	3 // charger end
#define PWR_CHARGER_PLUG   	4 // charger plug 
#define PWR_CHARGER_UNPLUG  	5 // charger unplug

/*
 * Status Type
 */
typedef struct pwr_Status_Type
{
  UBYTE Status;
  UBYTE BatteryLevel;
  UBYTE ChargeLevel;
} pwr_Status_Type;

/*
 * Driver Control Block
 */
typedef struct pwr_DCB_Type
{
  UBYTE    RangeMin;
  UBYTE    RangeMax;
  UBYTE    Steps;
} pwr_DCB_Type;

/*OMAPS00091029 x0039928(sumanth)*/
#ifndef _SIMULATION_
typedef enum BootTimeEvent
{
  EPreBoot=0,
  EAppInit,
  EModemBoot,
  ENetworkSync
} BootTimeEvent;
#endif

#if defined (NEW_FRAME)
/*
 * to achieve backward compatibility with older definitions
 */
#define drv_SignalCB_Type           T_DRV_CB_FUNC
#define drv_SignalID_Type           T_DRV_SIGNAL
#define T_VSI_THANDLE               USHORT
#endif
/*
 * Prototypes
 */
EXTERN UBYTE pwr_Init        (drv_SignalCB_Type   in_SignalCBPtr);
EXTERN void  pwr_Exit        (void);
EXTERN UBYTE pwr_SetSignal   (drv_SignalID_Type * in_SignalIDPtr);
EXTERN UBYTE pwr_ResetSignal (drv_SignalID_Type * in_SignalIDPtr);
EXTERN UBYTE pwr_SetConfig   (pwr_DCB_Type      * in_DCBPtr);
EXTERN UBYTE pwr_GetConfig   (pwr_DCB_Type      * out_DCBPtr);
EXTERN UBYTE pwr_GetStatus   (pwr_Status_Type   * out_StatusPtr);

EXTERN UBYTE pwr_PowerOffMobile   (void);
#ifndef _SIMULATION_
EXTERN void Bsp_get_boot_time(unsigned int * boot_time); /*OMAPS00091029 x0039928(sumanth)*/
void boot_time_snapshot(BootTimeEvent event);	
#endif
#endif
