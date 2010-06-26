/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_txp.c
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
|  Purpose :  This modul is part of the entity GMM and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (TX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef GMM_TXP_C
#define GMM_TXP_C
#endif

#define ENTITY_GMM

/*==== INCLUDES =============================================================*/
#include <string.h>     /* to get memcpy */
#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_gmm.h"    /* to get cnf-definitions */
#include "mon_gmm.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "gmm.h"        /* to get the global entity definitions */
#include "gmm_txf.h"    /* to get the local funktions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : tx_gmmsm_unitdata_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMSM_UNITDATA_REQ
|
| Parameters  : *gmmsm_unitdata_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifdef GMM_TCS4 
GLOBAL void tx_gmmsm_unitdata_req ( T_MMPM_UNITDATA_REQ* gmmsm_unitdata_req )
#else
GLOBAL void tx_gmmsm_unitdata_req ( T_GMMSM_UNITDATA_REQ* gmmsm_unitdata_req )
#endif 
{ 
  GMM_TRACE_FUNCTION( "tx_gmmsm_unitdata_req" );

  
  switch( GET_STATE( TX ) )
  {
    case TX_READY:
    {
      /*
       * The messages from SM are passed to LLC
       */
      /*
       * PPASS (gmmsm_unitdata_req, ll_unitdata_req, LL_UNITDATA_REQ );
       * is not possible, because of different parameters
       */
      PALLOC_SDU ( ll_unitdata_req, LL_UNITDATA_REQ, gmmsm_unitdata_req->sdu.l_buf);
      
      
      tx_fill_unitdata_req ( ll_unitdata_req, LOCAL_TLLI, gmm_data->cipher );
      memcpy (&ll_unitdata_req->sdu.buf[(ll_unitdata_req->sdu.o_buf)>>3],
        &gmmsm_unitdata_req->sdu.buf[(gmmsm_unitdata_req->sdu.o_buf)>>3],
        (gmmsm_unitdata_req->sdu.l_buf)>>3);



      #ifdef TRACE_EVE
      #ifndef GMM_TCS4 
          switch ( ll_unitdata_req->sdu.buf[((ll_unitdata_req->sdu.o_buf)>>3)+1])
          {
            case ACTIVATE_PDP_REQ:
              TRACE_EVENT ( "OPar: ACTIVATE_PDP_REQ");break;
            case ACTIVATE_PDP_ACC:      
              TRACE_EVENT ( "OPar: ACTIVATE_PDP_ACC");break;
            case ACTIVATE_PDP_REJ:      
              TRACE_EVENT ( "OPar: ACTIVATE_PDP_REJ");break;
            case REQ_PDP_ACT:      
              TRACE_EVENT ( "OPar: REQ_PDP_ACT");break;
            case REQ_PDP_ACT_REJ:      
              TRACE_EVENT ( "OPar: REQ_PDP_ACT_REJ");break;
            case MOD_PDP_REQ:      
              TRACE_EVENT ( "OPar: MOD_PDP_REQ");break;
            case MOD_PDP_ACC:      
              TRACE_EVENT ( "OPar: MOD_PDP_ACC");break;
            case DEACT_PDP_REQ:      
              TRACE_EVENT ( "OPar: DEACT_PDP_REQ");break;
            case DEACT_PDP_ACC:      
              TRACE_EVENT ( "OPar: DEACT_PDP_ACC");break; 
            case SM_STATUS:      
              TRACE_EVENT ( "OPar: SM_STATUS");break;
            default:
              TRACE_EVENT ( "OPar: SNDCP unknown");break;
          }
    #else    /*#ifndef GMM_TCS4*/ 
          switch ( ll_unitdata_req->sdu.buf[((ll_unitdata_req->sdu.o_buf)>>3)+1])
          {
            case ACTIVATE_PDP_CONTEXT_REQUEST:
              TRACE_EVENT ( "OPar: ACTIVATE_PDP_CONTEXT_REQUEST");break;
            case ACTIVATE_PDP_CONTEXT_ACCEPT:      
              TRACE_EVENT ( "OPar: ACTIVATE_PDP_CONTEXT_ACCEPT");break;
            case ACTIVATE_PDP_CONTEXT_REJECT:      
              TRACE_EVENT ( "OPar: ACTIVATE_PDP_CONTEXT_REJECT");break;
            case REQUEST_PDP_CONTEXT_ACTIVATION:      
              TRACE_EVENT ( "OPar: REQUEST_PDP_CONTEXT_ACTIVATION");break;
            case REQUEST_PDP_CONTEXT_ACTIVATION_REJECT:      
              TRACE_EVENT ( "OPar: REQUEST_PDP_CONTEXT_ACTIVATION_REJECT");break;
            case U_MODIFY_PDP_CONTEXT_REQUEST:      
              TRACE_EVENT ( "OPar: U_MODIFY_PDP_CONTEXT_REQUEST");break;
            case U_MODIFY_PDP_CONTEXT_ACCEPT:      
              TRACE_EVENT ( "OPar: U_MODIFY_PDP_CONTEXT_ACCEPT");break;
            case DEACTIVATE_PDP_CONTEXT_REQUEST:      
              TRACE_EVENT ( "OPar: DEACTIVATE_PDP_CONTEXT_REQ");break;
            case DEACTIVATE_PDP_CONTEXT_ACCEPT:      
              TRACE_EVENT ( "OPar: DEACTIVATE_PDP_CONTEXT_ACCEPT");break; 
            case SM_STATUS:      
              TRACE_EVENT ( "OPar: SM_STATUS");break;
            default:
              TRACE_EVENT ( "OPar: SNDCP unknown");break;
          }
      #endif   /*#ifndef GMM_TCS4*/
    #endif /* _SIMULATION_ */

      PSEND ( hCommLLC, ll_unitdata_req );
      PFREE ( gmmsm_unitdata_req );
      break;
    }
    default:
      PFREE (gmmsm_unitdata_req);
      TRACE_ERROR( "GMMSM_UNITDATA_REQ unexpected" );
      break;
  }

} /* tx_gmmsm_unitdata_req() */
