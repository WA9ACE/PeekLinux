/**
 * @file	dma_env.h
 *
 * Declarations of the Riviera Generic Functions 
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

#ifndef __DMA_ENV_H_
#define __DMA_ENV_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "rvf/rvf_pool_size.h"	/* Stack & Memory Bank sizes definitions */

/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a Type 3 SWE
 * (Group Member SWE).
 */
/*@{*/
T_RVM_RETURN dma_get_info (T_RVM_INFO_SWE  *info_swe);

T_RVM_RETURN dma_set_info ( T_RVF_ADDR_ID	addr_id,
							T_RV_RETURN_PATH return_path[],
							T_RVF_MB_ID		bk_id_table[],
							T_RVM_CB_FUNC	call_back_error_ft);

T_RVM_RETURN dma_init (void);

T_RVM_RETURN dma_stop (T_RV_HDR* hdr);
T_RVM_RETURN dma_kill (void);

/* Type 3 specific generic functions */
T_RVM_RETURN dma_start (void);
T_RVM_RETURN dma_handle_message (T_RV_HDR * msg);
T_RVM_RETURN dma_handle_timer (T_RV_HDR * msg);
/* End of specific */

/*@}*/

#endif /*__DMA_ENV_H_*/
