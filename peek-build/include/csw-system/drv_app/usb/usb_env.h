/**
 * @file	usb_env.h
 *
 * Declarations of the Riviera Generic Functions 
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

#ifndef __USB_ENV_H_
#define __USB_ENV_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "rvf/rvf_pool_size.h"	/* Stack & Memory Bank sizes definitions */


/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a Type 2 SWE
 * (Group Member SWE).
 */
/*@{*/
extern T_RVM_RETURN usb_get_info (T_RVM_INFO_SWE  *info_swe);

extern T_RVM_RETURN usb_set_info ( T_RVF_ADDR_ID	addr_id,
							T_RV_RETURN_PATH return_path[],
							T_RVF_MB_ID		bk_id_table[],
							T_RVM_CB_FUNC	call_back_error_ft);

extern T_RVM_RETURN usb_init (void);

extern T_RVM_RETURN usb_stop (T_RV_HDR * msg);
extern T_RVM_RETURN usb_kill (void);

/* Type 3 specific generic functions */
extern T_RVM_RETURN usb_start (void);
extern T_RVM_RETURN usb_handle_message (T_RV_HDR * msg);

/* End of specific */

/*@}*/


void change_internal_state(T_USB_INTERNAL_STATE state);


#endif /*__USB_ENV_H_*/
