/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Module   : DIE
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
*/


#include "die/die_i.h"

#include "die_env.h"

T_RVM_RETURN die_start(void)
{
  DIE_SEND_TRACE("DIE: start called", RV_TRACE_LEVEL_DEBUG_LOW);

	return RVM_OK;
}


/**
 * Called by the RV manager to stop the DIE SWE.
 *
 * @return	RVM_OK
 */
T_RVM_RETURN die_stop (void)
{
	/*
	 * Here we should stop the activities of the SWE
	 * It is still possible to send messages to other SWE, to unregister for example.
	 */
	DIE_SEND_TRACE("DIE: stop called", RV_TRACE_LEVEL_DEBUG_LOW);


	return RVM_OK;
}

T_RVM_RETURN die_get_info(T_RVM_INFO_SWE  * swe_info)
{
	swe_info->type_info.type1.version = BUILD_VERSION_NUMBER(DIE_MAJOR,DIE_MINOR,DIE_BUILD);

	return RVM_OK;
}

