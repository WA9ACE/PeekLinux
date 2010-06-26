/**
 * @file
 *
 * @brief Implementation of the timer handling function.
 *
 * This function allows the AS SWE to handle expired timers.
 * It is called upon a timer set by the AS SWE expires.
 */

/*
 * History:
 *
 *	Date       	Author			Modification
 *	-------------------------------------------------------------------
 *	1/25/2005	f-maria@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */


#include "as/as_i.h"
#include "as/as_env.h"
#include "as/as_buzzer.h"


/**
 * This function is called every time the AS SWE is in WAITING state
 * (from the RVM point of view) and one of its timers has expired.
 *
 * @param msg @in Neither used, nor pointing to a valid structure.
 *
 * @return An error code telling if the processing is successful or not.
 *
 * @retval RVM_OK Successful, the only return value for this sample function !
 */
T_RVM_RETURN as_env_handle_timer(T_RV_HDR* msg)
{	
    AS_TRACE_0(DEBUG_LOW, "env_handle_timer() called");

    if (msg != NULL)
    {
        // Check the received timer is a 'message' timer.
        if (msg->msg_id == RVM_TMS_MSG)
        {
#if (AS_OPTION_BUZZER == 1)           
            if (((T_RVF_TMS_MSG*) msg)->tm_id == as_ctrl_blk->buzzer.tm_id)
            {
                // Process buzzer timer expiration.
                as_buzzer_timer_expired();
            }
#endif // AS_OPTION_BUZZER   
        }

        // Free the passed timer message.
        rvf_free_timer_msg(msg);
    }

    return RVM_OK;
}
