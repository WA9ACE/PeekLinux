/*
 * @file	lcd_pwr.h
 *
 * Power management implementation for LCD.
 *
 * @author santosh v kondajji
 *	
 * @version 0.1
 *
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *		
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef LCD_PWR_H_
#define LCD_PWR_H_

#include "general.h"


//typedef unsigned short UWORD16;
extern int lcd_g_state;
void lcd_sleep_timer1_expiration();
void lcd_sleep_timer2_expiration();
UINT8 lcd_pwr_interface(UINT8);
typedef enum 
{
	LCD_ACTIVE,
	DISPLAY_OFF,
	CLOCK_OFF
}LCD_STATE;


#if (L1_POWER_MGT !=0)
void Lcd_Vote_DeepSleepStatus(void);
#endif

#endif
