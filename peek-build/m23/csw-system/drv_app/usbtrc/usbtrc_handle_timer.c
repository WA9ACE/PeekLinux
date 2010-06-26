/**
 * @file
 *
 * @brief Definition of the handling timer function.
 *
 * This function allows the USBTRC SWE to handle expired timer.
 * It is called upon a timer set by the USBTRC SWE expires.
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	4/14/2004	Charles-Hubert BESSON		Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */


#include "usbtrc/usbtrc_i.h"


/**
 * This function is called every time the USBTRC SWE is in WAITING state
 * (from the RVM point of view) and one of its timers has expired.
 *
 * @param msg_p @in Neither used, nor pointing to a valid structure.
 *
 * @return An error code telling if the processing is successful or not.
 *
 * @retval RVM_OK Successful, the only return value for this sample function !
 */
T_RVM_RETURN usbtrc_handle_timer(T_RV_HDR * msg_p)
{	
	// Got a timer event.

	// Timer processing ...

	return RVM_OK;
}
