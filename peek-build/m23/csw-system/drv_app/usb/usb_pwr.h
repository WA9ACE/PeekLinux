/**
 * @file	usb_pwr.h
 *
 * Power management implementation for USB.
 *
 * @author	  Saumar J Dutta
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


#ifndef	USB_PWR_H_
#define	USB_PWR_H_

#include "general.h"
#include "sys_types.h"

#if ( L1_POWER_MGT != 0x00)
void Usb_Vote_DeepSleepStatus(void);
#endif

UINT8 usb_pwr_interface(UINT8);

#endif

