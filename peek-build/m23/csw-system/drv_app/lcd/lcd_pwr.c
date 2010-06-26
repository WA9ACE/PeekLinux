/*
 * @file	lcd_pwr.c
 *
 * Power management implementation for LCD.
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
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "lcd_transport.h"
#include "lcd_pwr.h"
#include "r2d/lcds/i_sample/lcd.h"
#include "drv_power.h"

int lcd_g_state = LCD_ACTIVE;

UINT8 lcd_pwr_interface(UINT8 command)
{
	SYS_UWORD16 command2;

	switch(command)
	{
	 case 0 : /* Clock Mask */
	 	if ( lcd_g_state == LCD_ACTIVE )
			return DCXO_CLOCK;
		//else if ( lcd_g_state == DISPLAY_OFF )
		//	return DCXO_CLOCK;
		else
			return NO_CLOCK;
	 case 1 : /* Sleep Command */
		if ( (lcd_g_state == CLOCK_OFF) || (lcd_g_state == DISPLAY_OFF ) )
		{
			// DISPLAY OFF
			//command2 = 0xAE; 
			//f_lcd_if_poll_write(C_LCD_IF_CS0, &(command2), 1, C_LCD_IF_INSTRUCTION);
			// Disable the clock
			LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFFFD) | (LCD_CLOCK13_DIS << LCD_INTERFACE_CNTL_REG_CLOCK13_EN_POS));
			return SUCCESS;
		}
		else
			return FAILURE;
	 case 2 : /* Wakeup Command */
		// Enable the clock
		LCD_INTERFACE_CNTL_REG = ((LCD_INTERFACE_CNTL_REG & 0xFFFD) | (LCD_CLOCK13_EN << LCD_INTERFACE_CNTL_REG_CLOCK13_EN_POS));
		// LCD DISPALY ON
		//command2= 0xAF; 
 		//f_lcd_if_poll_write(C_LCD_IF_CS0, &(command2), 1, C_LCD_IF_INSTRUCTION);
		//lcd_g_state = LCD_ACTIVE;
		return SUCCESS;

	 default :
		 return FAILURE;
	} 
}
/*
void lcd_sleep_timer2_expiration()
{
	lcd_g_state = CLOCK_OFF;
  			
}

void lcd_sleep_timer1_expiration()
{
	
	SYS_UWORD16 command1;
	command1 = 0xAE; 

	f_lcd_if_poll_write(C_LCD_IF_CS0, &(command1), 1, C_LCD_IF_INSTRUCTION);
	lcd_g_state = DISPLAY_OFF;
}
*/
#if (L1_POWER_MGT !=0)
void Lcd_Vote_DeepSleepStatus(void)
{
	if(lcd_g_state == CLOCK_OFF)
		{
		Update_Sleep_Status(LCD_ID,ASLEEP);
		}
	else
		{
		Update_Sleep_Status(LCD_ID,AWAKE);
		}
}
#endif

