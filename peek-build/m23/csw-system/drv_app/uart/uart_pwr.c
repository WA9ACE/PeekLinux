/*
 * @file	uart_pwr.c
 *
 * Power management implementation for UART.
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

#include "uart_pwr.h"
#include "drv_power.h"

UINT8 uart_pwr_interface(UINT8 command)
{
	SYS_BOOL n;
	switch(command)
	{
	 case 0 : /* Clock Mask */
	 	n = SER_UartSleepStatus();
		if(n==0)
			return APLL_CLOCK; /* deep sleep is not possible*/
		else
			return NO_CLOCK;
	 case 1 : /* Sleep Command */
		n = SER_UartSleepStatus();
		if ( n==0)
			return FAILURE;/* deep sleep is not possible */
		else
			return SUCCESS;/* deep sleep is possible */
	 case 2 : /* Wakeup Command */
		SER_WakeUpUarts();
		return SUCCESS;
	 default :
		 return FAILURE;
	} 
}

#if(L1_POWER_MGT != 0x00)
void Uart_Vote_DeepSleepStatus(void)
{
    	SYS_BOOL stat;
	stat = SER_UartSleepStatus();
	if(stat == 0)
		{
		Update_Sleep_Status(UART_ID,AWAKE);
		}
	else
		{
		Update_Sleep_Status(UART_ID,ASLEEP);
		}
}
#endif

