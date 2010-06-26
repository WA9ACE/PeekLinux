/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_pdas.c
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
|             described in the SDL-documentation (PDA-statemachine)
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
| Function    : sig_sda_pda_getdata_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SDA_PDA_GETDATA_REQ
|
| Parameters  : the affected sapi.
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_sda_pda_getdata_req (UBYTE sapi) 
{ 
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_sda_pda_getdata_req" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->pda = & sndcp_data->pda_base[sapi_index];
 
  switch( GET_STATE( PDA ) )
  {
    case PDA_DEFAULT:
#ifdef SNDCP_TRACE_ALL
      TRACE_EVENT("trying to palloc/send LL_GETDATA_REQ");
#endif
      {
        PALLOC(ll_getdata_req, LL_GETDATA_REQ);

        ll_getdata_req->sapi = sapi;
        /*
         * TLLI is not evaluated by LLC.
         */
        PSEND(hCommLLC, ll_getdata_req);
      }
      break;
    default:
      TRACE_ERROR( "SIG_SDA_PDA_GETDATA_REQ unexpected" );
      break;
  }
} /* sig_sda_pda_getdata_req() */


