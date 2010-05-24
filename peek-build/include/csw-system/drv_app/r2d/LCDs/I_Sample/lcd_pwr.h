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

#include "nucleus.h"
#include "general.h"

//typedef unsigned short int UWORD16;

static NU_TIMER lcd_sleep_timer1,lcd_sleep_timer2;

void lcd_sleep_timer1_expiration(UNSIGNED);
void lcd_sleep_timer2_expiration(UNSIGNED);
UINT8 lcd_pwr_interface(UINT8);

#if (L1_POWER_MGT !=0)
void Lcd_Vote_DeepSleepStatus(void);
#endif

#endif
