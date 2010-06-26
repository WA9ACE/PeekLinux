/**
 * @file	usb_pwr.c
 *
 * Power management implementation for USB.
 *
 * @author	 Saumar J Dutta
 * @version 0.1
 */
/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *		
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#include "usb_i.h"


#include "usb_pwr.h"
#include "usb_w2fc.h"
#include "usb_state_i.h"
#include "drv_power.h"
UINT8 usb_pwr_interface(UINT8 command)
{
	switch(command)
	{
	 case 0 : /* Clock Mask */
		if( (usb_env_ctrl_blk_p->host_data.hw_state == USB_HW_UNATTACHED))  //checking if USB h/w is in
		{																   // suspended  or unattached state.	
			return NO_CLOCK;
		}
		else
		{
			return APLL_CLOCK;
		}
	 
	 case 1 : /* Sleep Command */
		if( (usb_env_ctrl_blk_p->host_data.hw_state == USB_HW_UNATTACHED))	//Checking if the Usb 
		{															  // device is in suspended or unattached state.
//OMAPS00176084 can not enter deepsleep once the USB has been plugged ¨C unplugged 
//cancel the comment
		W2FC_SYSCON1 |= W2FC_SYSCON1_SOFF_DIS;

		  return SUCCESS;
		}
		else
		{
			return FAILURE;				           //Usb not in suspended state,hence cannot go to sleep.
		}
	 
	 case 2 : /* Wakeup Command */
//OMAPS00176084 can not enter deepsleep once the USB has been plugged ¨C unplugged
//cancel the comment
		W2FC_SYSCON1 &=(~W2FC_SYSCON1_SOFF_DIS); 
		return SUCCESS;								
	 	 
	 default :
		 return SUCCESS;
	} 
}


#if ( L1_POWER_MGT != 0x00)
void Usb_Vote_DeepSleepStatus()
{
	if(usb_env_ctrl_blk_p->host_data.hw_state == USB_HW_UNATTACHED)
	{
		Update_Sleep_Status(USB_ID,ASLEEP);
	}
	else
	{
		Update_Sleep_Status(USB_ID,AWAKE);
	}
}
#endif



