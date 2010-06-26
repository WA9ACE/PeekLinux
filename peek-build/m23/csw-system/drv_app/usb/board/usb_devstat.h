/**
 * @file	usb_devstat.h
 *
 * Internal USB function dealing with device state changed interrupts.
 *
 * @author	Anton van Breemen
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	26/09/2003	Anton van Breemen (ICT)		
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __USB_DEVSTAT_H_
#define __USB_DEVSTAT_H_

#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "usb/usb_i.h"


/****************************************************************************************
 |	EXTERNAL FUNCTION PROTOTYPES														|
 V**************************************************************************************V*/

void usb_state_chngd_hndlr(void);
T_RV_RET usb_att_unatt_hndlr(void);
T_RV_RET usb_reset_hndlr(void);
void change_usb_hw_state(T_USB_HW_STATE hw_state);
void usb_cnfg_changed(void);
T_RV_RET usb_suspend_resume(BOOL lh_or_usbh);
void clr_ep_flgs(void);

#endif /*__USB_DEVSTAT_H_*/
