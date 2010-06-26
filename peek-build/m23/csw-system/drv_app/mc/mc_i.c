/*/ * @file mc_i.c
 *
 * MC Private Functions
 *
 * @author   (pklok)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  3/01/2004  ()   Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "mc/mc_i.h"

/**
 * Function to filter Trace information
 *
 *
 * @param string
 * @param lenght
 * @param val
 * @param tracelevel
 * @param sw_use_id
 *
 * @return  void 
 */
/*@{*/
void mc_send_trace_priv(char * string, UINT8 msg_length, UINT32 val, UINT8 trace_level, UINT32 swe_use_id)
{
  if(trace_level <= MC_USED_TRACE_LEVEL)
  {
    /** trace level is used so send trace*/
    rvf_send_trace (string, msg_length, val, trace_level, swe_use_id);
  }
  else
  {
    /** trace level is to low so discarded*/

  }
}
/*@}*/
