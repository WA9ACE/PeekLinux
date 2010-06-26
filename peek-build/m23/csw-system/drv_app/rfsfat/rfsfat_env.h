/**
 * @file	rfsfat_env.h
 *
 * Declarations of the Riviera Generic Functions 
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

#ifndef __RFSFAT_ENV_H_
#define __RFSFAT_ENV_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "rvf/rvf_pool_size.h"	/* Stack & Memory Bank sizes definitions */
#include "rfsfat/rfsfat_api.h"	/* return values */


/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a Type 3 SWE
 * (Group Member SWE).
 */
/*@{*/
T_RVM_RETURN rfsfat_get_info (T_RVM_INFO_SWE * info_swe);

T_RVM_RETURN rfsfat_set_info (T_RVF_ADDR_ID addr_id,
							  T_RV_RETURN_PATH return_path[],
							  T_RVF_MB_ID bk_id_table[],
							  T_RVM_CB_FUNC call_back_error_ft);

T_RVM_RETURN rfsfat_init (void);
T_RVM_RETURN rfsfat_stop (T_RV_HDR * hdr);
T_RVM_RETURN rfsfat_kill (void);

/* Type 3 specific generic functions */
T_RVM_RETURN rfsfat_start (void);
T_RVM_RETURN rfsfat_handle_message (T_RV_HDR * msg);
T_RVM_RETURN rfsfat_handle_timer (T_RV_HDR * msg);
void return_queue_init (T_RVF_G_ADDR_ID rq_addr_id, UINT16 rq_event,
						T_RV_RETURN_PATH * path_to_return_queue_p);
void *return_queue_get_msg (void);
T_RFSFAT_RETURN rfsfat_configure (const T_FSCORE_PARTITION_TABLE * core_mnt_tbl);

T_FSCORE_RET rfsfat_int_set_partition_table (const T_FSCORE_PARTITION_TABLE *
											 partition_table);


/* End of specific */

/*@}*/

#endif /*__RFSFAT_ENV_H_*/
