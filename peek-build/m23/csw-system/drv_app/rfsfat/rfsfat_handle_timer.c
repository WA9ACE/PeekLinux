/**
 * @file	rfsfat_handle_timer.c
 *
 * RFSFAT handle_timer function, which is called when a timer
 * of the SWE expires.
 *
 * @author	Anton van Breemen (abreemen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/29/2004	Anton van Breemen (abreemen@ict.nl)		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */


#include "rfsfat/rfsfat_i.h"


/**
 * Called every time the SW entity is in WAITING 
 * state and one of its timers has expired.
 *
 * @param	Not used
 * @return	RVM_OK
 */
T_RVM_RETURN rfsfat_handle_timer (T_RV_HDR * msg_p)
{	
	/*
	 * Got a timer event.
	 */


	return RVM_OK;
}
