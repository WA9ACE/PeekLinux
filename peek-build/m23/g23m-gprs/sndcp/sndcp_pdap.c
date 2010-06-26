/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_pdap.c
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
|             the SDL-documentation (PDA-statemachine)
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

#include "sndcp_sdas.h"     /* to get the signals to sda. */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : pda_ll_data_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_DATA_IND
|
| Parameters  : *ll_data_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void pda_ll_data_ind ( T_LL_DATA_IND *ll_data_ind )
{ 
  UBYTE sapi_index = 0;

  TRACE_FUNCTION( "pda_ll_data_ind" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(ll_data_ind->sapi, &sapi_index);
  sndcp_data->pda = & sndcp_data->pda_base[sapi_index];
  
  switch( GET_STATE( PDA ) )
  {
    case PDA_DEFAULT:
      sig_pda_sda_data_ind(ll_data_ind);
      break;
    default:
      if (ll_data_ind != NULL) {
        PFREE(ll_data_ind);
      }
      TRACE_ERROR( "LL_DATA_IND unexpected" );
      break;
  }

} /* pda_ll_data_ind() */


