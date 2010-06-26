/**
 * @file	datlight.h
 *
 * Internal definitions for Data Light .
 *
 * @author	
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	               			
 *
 * 
 */

#ifndef __DATALIGHT_H__
#define __DATALIGHT_H__

#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_ext_use_id_list.h"
#include "swconfig.cfg"
#include "gbi/gbi_pi_datalight_i.h"



enum {
	DL_OK					  = 0,
	DL_NOT_SUPPORTED		  = -2,
	DL_NOT_READY			  = -3,
	DL_MEMORY_WARNING		  = -4,
	DL_MEMORY_ERR			  = -5,
	DL_MEMORY_REMAINING	  = -6,
	DL_INTERNAL_ERR		  = -9,
	DL_INVALID_PARAMETER	  = -10,
  	DL_INVALID_BLOCK_ERR    = -11,
  	DL_PAGE_NOT_EMPTY_ERR   = -12,
  	DL_ECC_ERROR            = -13,
  	DL_ALLOCATION_ERROR     = -1
} ;




#define DATALIGHT_ENABLE_TRACE

/** Macro used for tracing DATALIGHT messages. */
#ifdef DATALIGHT_ENABLE_TRACE
#ifndef _WINDOWS
  #define DL_SEND_TRACE(string, trace_level) 
    
  #define DL_SEND_TRACE_PARAM(string, param, trace_level) 

#else
  #define DL_SEND_TRACE(string, trace_level) \
    rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, DATALIGHT_USE_ID)
  #define DL_SEND_TRACE_PARAM(string, param, trace_level) \
    rvf_send_trace (string, (sizeof(string) - 1), param, trace_level, DATALIGHT_USE_ID)
#endif
#endif




/**
 * The Control Block buffer of DL, which gathers all 'Global variables'
 * used by DL instance.
 *
 * A structure should gathers all the 'global variables' of DL instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_DL_ENV_CTRL_BLK buffer is allocated when creating DL instance
 * and is
 * then always refered by DL instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
  BOOL initialised;
  BOOL disk_initialised[GBI_DATALIGHT_MAX_NR_OF_PARTITIONS];

	/** Mem bank id. */
  UINT32 prim_mb_id;
  T_RVF_ADDR_ID addr_id;

  /* store task ID*/
  T_RVF_ADDR_ID task_id;
  
}T_DL_ENV_CTRL_BLK;


/** External ref "global variables" structure. */
extern T_DL_ENV_CTRL_BLK *dl_env_ctrl_blk_p;


#endif /* __DATALIGHT_H__ */


