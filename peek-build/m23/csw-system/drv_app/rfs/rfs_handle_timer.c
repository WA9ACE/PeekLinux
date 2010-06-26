/**
 * @file	rfs_handle_timer.c
 *
 * RFS handle_timer function, which is called when a timer
 * of the SWE expires.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/23/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */


#include "rfs/rfs_i.h"


/**
 * Called every time the SW entity is in WAITING 
 * state and one of its timers has expired.
 *
 * @param	Not used
 * @return	RVM_OK
 */
T_RVM_RETURN rfs_handle_timer (T_RV_HDR * msg_p)
{	
	/*
	 * Got a timer event.
	 */


	return RVM_OK;
}
