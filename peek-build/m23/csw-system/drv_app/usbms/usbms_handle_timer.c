/**
 * @file	usbms_handle_timer.c
 *
 * USBMS handle_timer function, which is called when a timer
 * of the SWE expires.
 *
 * @author	Your name here (your_email_here)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	3/19/2004	Virgile COULANGE		Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */


#include "usbms/usbms_i.h"


/**
 * Called every time the SW entity is in WAITING 
 * state and one of its timers has expired.
 *
 * @param	Not used
 * @return	RVM_OK
 */
T_RVM_RETURN usbms_handle_timer (T_RV_HDR * msg_p)
{	
	/*
	 * Got a timer event.
	 */


	return RVM_OK;
}
