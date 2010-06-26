/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_pdp.c
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
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (PD-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

#include "sndcp_sds.h"     /* to get the signals to sd. */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : pd_ll_unitdata_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_UNITDATA_IND
|
| Parameters  : *ll_unitdata_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void pd_ll_unitdata_ind ( T_LL_UNITDATA_IND *ll_unitdata_ind )
{ 
  UBYTE sapi_index = 0;


#ifdef FLOW_TRACE
  sndcp_trace_flow_control(FLOW_TRACE_SNDCP, 
                           FLOW_TRACE_DOWN, 
                           FLOW_TRACE_BOTTOM, 
                           FALSE);
#endif
  TRACE_FUNCTION( "pd_ll_unitdata_ind" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(ll_unitdata_ind->sapi, &sapi_index);
  sndcp_data->pd = & sndcp_data->pd_base[sapi_index];
  
  switch( GET_STATE( PD ) )
  {
    case PD_DEFAULT:
#ifdef SNDCP_TRACE_ALL
      TRACE_EVENT_P1("downlink at LL SAP: %d octets", 
                     ll_unitdata_ind->sdu.l_buf >> 3);

      sndcp_trace_sdu(& ll_unitdata_ind->sdu);

#endif        
      sig_pd_sd_unitdata_ind(ll_unitdata_ind);
      break;
    default:
      TRACE_ERROR( "LL_UNITDATA_IND unexpected" );
      if (ll_unitdata_ind != NULL) {
        PFREE (ll_unitdata_ind);
      }
      break;
  }

} /* pd_ll_unitdata_ind() */


