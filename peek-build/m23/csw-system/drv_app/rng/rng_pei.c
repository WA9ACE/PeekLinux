/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Modul   :  RNG
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  This module implements the process body interface
|             for the entity RNG
|
|             Exported functions:
|
|          pei_create    - Create the Protocol Stack Entity
|
|          pei_init      - Initialize Protocol Stack Entity
|          pei_exit      - Close resources and terminate
|          pei_primitive - Process Primitive
|          pei_timeout   - Process Timeout
|          pei_signal    - Process Signals             
|          pei_run       - Process Messages
|          pei_config    - Dynamic Configuration
|          pei_monitor   - Monitoring of physical Parameters
|+----------------------------------------------------------------------------- 
*/ 

/*==== INCLUDES =============================================================*/

#ifndef _WINDOWS
	#include "memif/mem.h"
#endif /* _WINDOWS */
#include "rng/rng_i.h"

#include <string.h>
#include "rng/rng_env.h"

/*==== DEFINITIONS ==========================================================*/

#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

T_RNG_ENV_CTRL_BLK *rng_env_ctrl_blk_p;
BOOL rng_idle = FALSE;

/*
+------------------------------------------------------------------------------
| Function    : pei_init
+------------------------------------------------------------------------------
| Description : This function is called by the frame. It is used to initialise
|               the entitiy.
|
| Parameters  : handle            - task handle
|
| Return      : PEI_OK            - entity initialised
|               PEI_ERROR         - entity not (yet) initialised
|
+------------------------------------------------------------------------------
*/
 T_RVM_RETURN rng_init()
{
     /* Create instance gathering all the variable used by RNG instance */
     
	if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_RNG_ENV_CTRL_BLK), (T_RVF_BUFFER**)&rng_env_ctrl_blk_p) != RVF_OK)
	{
            /* The environemnt will cancel the EXPL instance creation. */
            RNG_SEND_TRACE ("RNG: Error to get memory ", RV_TRACE_LEVEL_ERROR);
            return RVM_MEMORY_ERR;	
	}

	/*
	 * Here the instance (rng_env_ctrl_blk_p) could be initialised.
	 */
	#ifdef _WINDOWS
	  rng_env_ctrl_blk_p->rng_initialised = TRUE; /* Initialisation is done */
	#else
	  rng_env_ctrl_blk_p->rng_initialised = FALSE; /* RNG not initialised */
	//  *(volatile UINT32 *) RNG_MASK |= RNG_AUTO_IDLE_BIT; /* Activate power saving mode */
	#endif /* _WINDOWS */
	*(volatile UINT32 *) RNG_MASK |= RNG_AUTO_IDLE_BIT; /* Activate power saving mode */
    	  rng_start();  
    return (RVF_OK);

} /* End pei_init(..) */


/**
 * Called by the RV manager to start the RNG SWE,
 * Note: Only for type 1, 2 and 3 SWE.
 *
 * @return	RVM_OK.
 */
T_RVM_RETURN rng_start(void)
{
  rng_idle = TRUE;
  return RVM_OK;
}


/**
 * Called by the RV manager to stop the RNG SWE.
 *
 * @return	RVM_OK
 */
T_RVM_RETURN rng_stop (void)
{
	/*
	 * Here we should stop the activities of the SWE
	 * It is still possible to send messages to other SWE, to unregister for example.
	 */
	RNG_SEND_TRACE("RNG: stop called", RV_TRACE_LEVEL_DEBUG_LOW);
  rng_idle = FALSE;

	return RVM_OK;
}


/**
 * Called by the RV manager to kill the rng SWE,
 * after the rng_stop function has been called.
 *
 * @return	RVM_OK
 */
T_RVM_RETURN rng_kill (void)
{
	/*
	 * Here we cannot send messages anymore. We only free the last
	 * used resources, like the control block buffer.
	 */
	RNG_SEND_TRACE("RNG: kill called", RV_TRACE_LEVEL_DEBUG_LOW);

  rng_idle = FALSE;
	rvf_free_buf(rng_env_ctrl_blk_p);

	return RVM_OK;
}

