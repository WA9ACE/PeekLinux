/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_txs.c
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
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (TX-statemachine)
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_TXS_C
#define GMM_TXS_C
#endif

#define ENTITY_GMM

/*==== INCLUDES =============================================================*/

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
| Function    : sig_kern_tx_data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_KERN_TX_DATA_REQ
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_kern_tx_data_req ( T_TLLI_TYPE tlli_type, UBYTE msg_type ) 
{ 
  TRACE_ISIG( "sig_kern_tx_data_req" );
  tx_unitdata_req ( tlli_type, msg_type );

} /* sig_kern_tx_data_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_rx_tx_data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_KERN_TX_DATA_REQ
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_tx_data_req ( T_TLLI_TYPE tlli_type, UBYTE msg_type ) 
{ 
  TRACE_ISIG( "sig_rx_tx_data_req" );
  
  #ifdef _SIMULATION_
    switch ( msg_type ) {
      case ATTACH_REQUEST:
        TRACE_EVENT ( "                       send: ATTACH_REQUEST");break;
      case ATTACH_ACCEPT:      
        TRACE_EVENT ( "                       send: ATTACH_ACCEPT");break;
      case ATTACH_COMPLETE:
        TRACE_EVENT ( "                       send: ATTACH_COMPLETE");break;
      case ATTACH_REJECT:
        TRACE_EVENT ( "                       send: ATTACH_REJECT");break;
      case D_DETACH_REQUEST:
        TRACE_EVENT ( "                       send: DETACH_REQUEST");break;
      case D_DETACH_ACCEPT:
        TRACE_EVENT ( "                       send: DETACH_ACCEPT");break;
      case ROUTING_AREA_UPDATE_REQUEST:
        TRACE_EVENT ( "                       send: RAU_REQUEST");break;
      case ROUTING_AREA_UPDATE_ACCEPT:
        TRACE_EVENT ( "                       send: RAU_ACCEPT");break;
      case ROUTING_AREA_UPDATE_COMPLETE:
        TRACE_EVENT ( "                       send: RAU_COMPLETE");break;
      case ROUTING_AREA_UPDATE_REJECT:
        TRACE_EVENT ( "                       send: RAU_REJ");break;
      case P_TMSI_REALLOCATION_COMMAND:
        TRACE_EVENT ( "                       send: P_TMSI_REALLOCATION_COMMAND");break;
      case P_TMSI_REALLOCATION_COMPLETE:
        TRACE_EVENT ( "                       send: P_TMSI_REALLOCATION_COMPLETE");break;
      case AUTHENTICATION_AND_CIPHERING_REQUEST:
        TRACE_EVENT ( "                       send: AUTHENTICATION_AND_CIPHERING_REQUEST");break;
      case AUTHENTICATION_AND_CIPHERING_RESPONSE:
        TRACE_EVENT ( "                       send: ATTACH_REQUEST");break;
      case AUTHENTICATION_AND_CIPHERING_REJECT:
        TRACE_EVENT ( "                       send: AUTHENTICATION_AND_CIPHERING_REJECT");break;
      case IDENTITY_REQUEST:
        TRACE_EVENT ( "                       send: IDENTITY_REQUEST");break;
      case IDENTITY_RESPONSE:
        TRACE_EVENT ( "                       send: IDENTITY_RESPONSE");break;
      case GMM_STATUS:
        TRACE_EVENT ( "                       send: GMM_STATUS");break;
      case GMM_INFORMATION:
        TRACE_EVENT ( "                       send: GMM_INFORMATION");break;
      default:
        break;
    }
  #endif /* _SIMULATION_ */

  tx_unitdata_req ( tlli_type, msg_type );

} /* sig_rx_tx_data_req() */


