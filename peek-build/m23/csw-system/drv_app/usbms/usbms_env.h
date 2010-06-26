/**
 * @file	usbms_env.h
 *
 * Declarations of the Riviera Generic Functions 
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

#ifndef __USBMS_ENV_H_
#define __USBMS_ENV_H_


#include "rvm/rvm_gen.h"			/* Generic RVM types and functions. */
//#include "usbms/usbms_i.h"
#include "usbms/usbms_pool_size.h"	/* Stack & Memory Bank sizes definitions */


/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a Type 3 SWE
 * (Group Member SWE).
 */
/*@{*/
T_RVM_RETURN usbms_get_info (T_RVM_INFO_SWE  *info_swe);

T_RVM_RETURN usbms_set_info ( T_RVF_ADDR_ID	addr_id,
							T_RV_RETURN_PATH return_path[],
							T_RVF_MB_ID		bk_id_table[],
							T_RVM_CB_FUNC	call_back_error_ft);

T_RVM_RETURN usbms_init (void);

T_RVM_RETURN usbms_kill (void);

/* Type 3 specific generic functions */
T_RVM_RETURN usbms_start (void);
T_RVM_RETURN usbms_stop (T_RV_HDR * msg);
T_RVM_RETURN usbms_handle_message (T_RV_HDR * msg);
T_RVM_RETURN usbms_handle_timer (T_RV_HDR * msg);
/* End of specific */
void usbms_change_internal_state(T_USBMS_INTERNAL_STATE state);

/*@}*/

#endif /*__USBMS_ENV_H_*/
