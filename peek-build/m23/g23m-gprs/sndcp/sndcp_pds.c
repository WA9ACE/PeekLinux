/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_pds.c
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
|  Purpose :  This modul is part of the entity SNDCP and implements all 
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (PD-statemachine)
+----------------------------------------------------------------------------- 
*/ 


#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_sd_pd_getunitdata_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SD_PD_GETUNITDATA_REQ
|
| Parameters  : the affected sapi.
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_sd_pd_getunitdata_req (UBYTE sapi) 
{ 
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_sd_pd_getunitdata_req" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->pd = & sndcp_data->pd_base[sapi_index];
 
  switch( GET_STATE( PD ) )
  {
    case PD_DEFAULT:
      {
        PALLOC(ll_getunitdata_req, LL_GETUNITDATA_REQ);

        ll_getunitdata_req->sapi = sapi;
        /*
         * TLLI is not evaluated by LLC.
         */
#ifdef FLOW_TRACE
        sndcp_trace_flow_control(FLOW_TRACE_SNDCP, 
                                 FLOW_TRACE_DOWN, 
                                 FLOW_TRACE_BOTTOM, 
                                 TRUE);
#endif
        PSEND(hCommLLC, ll_getunitdata_req);
      }
      break;
    default:
      TRACE_ERROR( "SIG_SD_PD_GETUNITDATA_REQ unexpected" );
      break;
  }
} /* sig_sd_pd_getunitdata_req() */


