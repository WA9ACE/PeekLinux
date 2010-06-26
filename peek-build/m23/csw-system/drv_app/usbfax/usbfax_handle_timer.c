/**
 * @file	usbfax_handle_timer.c
 *
 * USBFAX handle_timer function, which is called when a timer
 * of the SWE expires.
 *
 * @author
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	3/17/2004
 */


#include "usbfax/usbfax_i.h"
#include "rvf/rvf_api.h"
 /**
 * Pointer on the structure gathering all the global variables
 * used by USBFAX instance.
 */
extern T_USBFAX_ENV_CTRL_BLK	*usbfax_env_ctrl_blk_p;
T_USBFAX_USB_DEVICE *remu_current_device_p;

/**
 * Called every time the SW entity is in WAITING 
 * state and one of its timers has expired.
 *
 * @param	Not used
 * @return	RVM_OK
 */

#if (REMU==0)
T_RVM_RETURN usbfax_handle_timer (T_RV_HDR * msg_p)
{	
	/* Reference to the current device */
	T_USBFAX_USB_DEVICE		*current_device_p = NULL;
	T_RVF_TMS_MSG*			a_msg_p = NULL;
	
	USBFAX_SEND_TRACE("USBFAX: usbfax_handle_timer", RV_TRACE_LEVEL_DEBUG_LOW);

	if(msg_p->msg_id == RVM_TMS_MSG)
	{
		USBFAX_SEND_TRACE("USBFAX: Call to usbfax_timer_escape_sequence", RV_TRACE_LEVEL_DEBUG_LOW);

		a_msg_p = (T_RVF_TMS_MSG*)msg_p;
		current_device_p = a_msg_p->action;

		usbfax_timer_escape_sequence(current_device_p);
	}

	/*
	 *	Free message driven Timer Notification message
	 */
	if(msg_p) rvf_free_timer_msg(msg_p);

	return RVM_OK;
}

#else
T_RVM_RETURN usbfax_handle_timer (T_RV_HDR * msg_p)
{	
	usbfax_timer_escape_sequence(remu_current_device_p);
	return RVM_OK;
}
#endif
/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_start_timer
 *
 * This function is used to start a timer for detecting escape sequences
 *
 * @param current_device_p:	reference to the current device
 *
 * @return				RVM_OK
 *						RV_INTERNAL_ERR	: Incompatible message
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOLEAN			usbfax_start_timer(T_USBFAX_USB_DEVICE* current_device_p)
{
	USBFAX_SEND_TRACE("USBFAX: usbfax_start_timer", RV_TRACE_LEVEL_DEBUG_LOW);
	
	/* Start the timer */
	remu_current_device_p=current_device_p;	
	rvf_reset_timer(current_device_p->timer,
					RVF_MS_TO_TICKS(current_device_p->device_config.guard_period),
					FALSE);	/* Not periodic */
	return TRUE;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_stop_timer
 *
 * This function is used to stop a timer of detecting escape sequences
 *
 * @param current_device_p:	reference to the current device
 *
 * @return				RVM_OK
 *						RV_INTERNAL_ERR	: Incompatible message
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOLEAN			usbfax_stop_timer(T_USBFAX_USB_DEVICE* current_device_p)
{
	/* Stop the timer */
	remu_current_device_p=current_device_p;	
	rvf_reset_timer(current_device_p->timer, 0, FALSE);

	return TRUE;
}
/*@}*/
