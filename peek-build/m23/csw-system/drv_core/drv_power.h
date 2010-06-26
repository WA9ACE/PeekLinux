
/*
 * @file	drv_power.h
 *
 * Common Header file for Power management.
 *
 * @author	santosh v kondajji
 *
 * @version 0.1
 *
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	
 *	
 *(C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef	DRV_POWER_H_
#define	DRV_POWER_H_

#include "general.h"
#include "sys_types.h"
#include "l1sw.cfg"

#if((L1_POWER_MGT == 1) && (PSP_STANDALONE == 0))
#include "l1_pwmgr.h"
#else
#define SUCCESS 1
#define FAILURE 0

#define UART_ID 0
#define USB_ID 1
#define USIM_ID 2
#define LCD_ID 4
#define CAMERA_ID 5


#define ASLEEP 0
#define AWAKE 1

typedef enum
{
 NO_CLOCK,
 DCXO_CLOCK,
 DPLL_CLOCK,
 APLL_CLOCK,
 ARM_CLOCK,
 LOW_CLOCK
}clock;

#if (L1_POWER_MGT != 0x00)
 void Update_Sleep_Status(UINT8 ID,UINT8 State);
#endif    

#endif    //ends inclusion of l1_pwr_mgr

#endif	  //ends inclusion of the Macro DRV_POWER_H_
