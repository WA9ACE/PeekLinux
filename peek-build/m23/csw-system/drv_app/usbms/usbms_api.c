/**
 * @file	usbms_api.c
 *
 * API for USBMS SWE.
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
#include "usbms/usbms_api.h"



/**
 * Sample bridge functions.
 *
 * Detailled description.
 *
 * @return	USBMS_MEMORY_ERR in case of a memory error,
 *			the return value of rvf_send_msg otherwise.
 */									
T_USBMS_RETURN usbms_send_sample ()
{
	T_USBMS_SAMPLE_MESSAGE * sample_msg_p;

	/* Memory bank status (red, yellow, green). */
	T_RVF_MB_STATUS mb_status;


	USBMS_SEND_TRACE("USBMS API: usbms_send_sample bridge function", RV_TRACE_LEVEL_DEBUG_LOW);

	/* Get a message buffer. */
	mb_status = rvf_get_msg_buf(usbms_env_ctrl_blk_p->prim_mb_id,
								sizeof(T_USBMS_SAMPLE_MESSAGE),
								USBMS_SAMPLE_MESSAGE,
								(T_RVF_MSG**) &sample_msg_p);
	if (mb_status == RVF_RED)
	{
		/*
		 * The flag returned by rvf_get_buf is red, there is not enough
		 * memory to allocate the buffer.
		 */
		USBMS_SEND_TRACE("USBMS API: Error to get memory ",RV_TRACE_LEVEL_ERROR);
				
		return USBMS_MEMORY_ERR;
	}
	else if (mb_status == RVF_YELLOW)
	{
		/*
		 * The flag is yellow, there will soon be not enough memory anymore.
		 */
		USBMS_SEND_TRACE("USBMS API: Getting short on memory ", RV_TRACE_LEVEL_WARNING);
	}
	

	/* Init message parameters. */
	/* ... */

	/* Send the message using mailbox. */
	return rvf_send_msg(usbms_env_ctrl_blk_p->addr_id,
						(void*)sample_msg_p);
}
