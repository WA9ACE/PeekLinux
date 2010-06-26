/**
 * @file	dma_handle_timer.c
 *
 * DMA handle_timer function, which is called when a timer
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
 *	7/2/2003	 ()		Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */


#include "dma/dma_i.h"


/**
 * Called every time the SW entity is in WAITING 
 * state and one of its timers has expired.
 *
 * @param	Not used
 * @return	RVM_OK
 */
T_RVM_RETURN dma_handle_timer (T_RV_HDR * msg_p)
{	
	/*
	 * Got a timer event.
	 */


	return RVM_OK;
}
