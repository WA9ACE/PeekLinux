/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Module  :  CL
|  File    :  cl_nwrl.c
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
|  Purpose :  This file implements common library functions related to the
|             network release(in this case, SGSN release) for all entities of 
|             the GPRS protocol stack
+----------------------------------------------------------------------------- 
*/ 

#ifndef CL_NWRL_C
#define CL_NWRL_C
#endif

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"
#include "p_cl.val"  /* to get the type of sgsn_rel*/
#include "p_8010_152_ps_include.h" /*to get T_PS_qos_rXX*/
#include "cl_inline.h"

LOCAL U8 sgsn_release;

/*
+------------------------------------------------------------------------------
| Function    : cl_nwrl_set_sgsn_release() 
+------------------------------------------------------------------------------
| Description : The function cl_nwrl_set_sgsn_release() sets the status of SGSN 
|               release as received in SI13, PSI1 and PSI13 messages. This is 
|               called by GRR during initialization after Power On, when 
|               processing SI13, PSI1 and PSI13
|
| Parameters  : U8 sgsn_rel
|
+------------------------------------------------------------------------------
*/

GLOBAL void cl_nwrl_set_sgsn_release(U8 sgsn_rel)
{
  TRACE_FUNCTION( "cl_nwrl_set_sgsn_release()" );
  sgsn_release = sgsn_rel;
}


/*
+------------------------------------------------------------------------------
| Function    : cl_nwrl_get_sgsn_release()
+------------------------------------------------------------------------------
| Description : The function cl_nwrl_get_sgsn_release() gets the status of SGSN
|               release. This is called up by entities such as GMM, SM that need
|               the SGSN release. It is advised to call this function only after
|               a cell re-selection as the SGSN release value is invalid during
|               cell re-selection.
|
| Parameters  : Nil
|
+------------------------------------------------------------------------------
*/

GLOBAL U8 cl_nwrl_get_sgsn_release()
{
  TRACE_FUNCTION( "cl_nwrl_get_sgsn_release()" );
  return sgsn_release;
}

