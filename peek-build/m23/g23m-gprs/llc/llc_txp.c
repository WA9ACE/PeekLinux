/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  This modul is part of the entity LLC and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (TX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_TXP_C
#define LLC_TXP_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

#include "llc_txf.h"    /* to get local TX functions */
#include "llc_txl.h"    /* to get local TX labels */
#include "llc_uitxs.h"  /* to get signal interface to UITX */
#include "llc_itxs.h"   /* to get ITX signal definitions */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : tx_grlc_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive GRLC_READY_IND
|
| Parameters  : *grlc_ready_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_grlc_ready_ind ( T_GRLC_READY_IND *grlc_ready_ind )
{
  T_GRLC_DATA_REQ              *grlc_data_req;
  T_GRLC_UNITDATA_REQ          *grlc_unitdata_req;
  T_PRIM_TYPE                 prim_type;
  T_SERVICE                   rx_service;
  UBYTE                       sapi;


  TRACE_FUNCTION( "tx_grlc_ready_ind" );

  if (llc_data->grlc_suspended == TRUE)
  {
    /*
     * Ignore it, may be it's due to a collision isse
     */
    PFREE (grlc_ready_ind);
    TRACE_0_INFO("GRLC_READY_IND ignored");
  }
  else switch( GET_STATE( TX ) )
  {
    case TX_TLLI_UNASSIGNED_NOT_READY:
      PFREE (grlc_ready_ind);
      SET_STATE (TX, TX_TLLI_UNASSIGNED_READY);
      break;

    case TX_TLLI_ASSIGNED_NOT_READY:
      PFREE (grlc_ready_ind);

      do {
        tx_get_next_frame (&grlc_data_req, &grlc_unitdata_req, &prim_type, 
          &rx_service, &sapi);
        
        switch (prim_type)
        {
          case PRIM_DATA:
            /*
             * Label S_DATA
             */
            tx_label_s_data (rx_service, grlc_data_req);
            break;
          case PRIM_UNITDATA:
            /*
             * Label S_UNITDATA
             */
            tx_label_s_unitdata (rx_service, grlc_unitdata_req);
            break;
          case PRIM_REMOVED:
            /*
             * Label S_REMOVED
             */
            tx_label_s_removed (rx_service, sapi);
            break;
          default: /* NO_PRIM */
            SET_STATE (TX, TX_TLLI_ASSIGNED_READY);
            break;
        }
      } while (prim_type == PRIM_REMOVED);
      break;

    default:
      PFREE (grlc_ready_ind);
      TRACE_ERROR( "GRLC_READY_IND unexpected" );
      break;
  }

} /* tx_grlc_ready_ind() */

/*
+------------------------------------------------------------------------------
| Function    : tx_grlc_suspend_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive GRLC_SUSPEND_READY_IND
|
| Parameters  : *grlc_suspend_ready_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_grlc_suspend_ready_ind ( T_GRLC_SUSPEND_READY_IND *grlc_suspend_ready_ind )
{
  T_GRLC_DATA_REQ              *grlc_data_req;
  T_GRLC_UNITDATA_REQ          *grlc_unitdata_req;
  T_PRIM_TYPE                 prim_type;
  T_SERVICE                   rx_service;
  UBYTE                       sapi;


  TRACE_FUNCTION( "tx_grlc_suspend_ready_ind" );

  if (llc_data->grlc_suspended == FALSE)
  {
    /*
     * Ignore it, may be it's due to a collision isse
     */
    PFREE (grlc_suspend_ready_ind);
    TRACE_0_INFO("GRLC_SUSPEND_READY_IND ignored");
  }
  else switch( GET_STATE( TX ) )
  {
    case TX_TLLI_UNASSIGNED_NOT_READY:
      PFREE (grlc_suspend_ready_ind);
      SET_STATE (TX, TX_TLLI_UNASSIGNED_READY);
      break;

    case TX_TLLI_ASSIGNED_NOT_READY:
      PFREE (grlc_suspend_ready_ind);

      do {
        tx_get_next_frame (&grlc_data_req, &grlc_unitdata_req, &prim_type, 
          &rx_service, &sapi);
        
        switch (prim_type)
        {
          case PRIM_DATA:
            /*
             * Label S_DATA
             */
            tx_label_s_data (rx_service, grlc_data_req);
            break;
          case PRIM_UNITDATA:
            /*
             * Label S_UNITDATA
             */
            tx_label_s_unitdata (rx_service, grlc_unitdata_req);
            break;
          case PRIM_REMOVED:
            /*
             * Label S_REMOVED
             */
            tx_label_s_removed (rx_service, sapi);
            break;
          default: /* NO_PRIM */
            SET_STATE (TX, TX_TLLI_ASSIGNED_READY);
            break;
        }
      } while (prim_type == PRIM_REMOVED);
      break;

    default:
      PFREE (grlc_suspend_ready_ind);
      TRACE_ERROR( "GRLC_SUSPEND_READY_IND unexpected" );
      break;
  }
} /* tx_grlc_suspend_ready_ind() */


/*
+------------------------------------------------------------------------------
| Function    : tx_cci_cipher_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive CCI_CIPHER_CNF.
|               Note: GRLC_DATA_REQ type is used instead to avoid PPASS.
|
| Parameters  : *grlc_data_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_cci_cipher_cnf (T_GRLC_DATA_REQ *grlc_data_req)
{
  T_GRLC_UNITDATA_REQ          *grlc_unitdata_req;
  T_PRIM_TYPE                 prim_type;
  T_SERVICE                   rx_service;
  UBYTE                       sapi;


  TRACE_FUNCTION( "tx_cci_cipher_cnf" );
  
  switch( GET_STATE( TX ) )
  {
    case TX_TLLI_ASSIGNED_NOT_READY:
      tx_store_buffer (grlc_data_req);
      break;
    case TX_TLLI_ASSIGNED_READY:
      tx_store_buffer (grlc_data_req);

      do {
        tx_get_next_frame (&grlc_data_req, &grlc_unitdata_req, &prim_type, 
          &rx_service, &sapi);

        switch (prim_type)
        {
          case PRIM_DATA:
            /*
             * Label S_DATA
             */
            tx_label_s_data (rx_service, grlc_data_req);
            break;
          case PRIM_UNITDATA:
            /*
             * Label S_UNITDATA
             */
            tx_label_s_unitdata (rx_service, grlc_unitdata_req);
            break;
          case PRIM_REMOVED:
            /*
             * Label S_REMOVED
             */
            tx_label_s_removed (rx_service, sapi);
            break;
          default: /* NO_PRIM */
            break;
        }
      } while (prim_type == PRIM_REMOVED);
      break;
    default:
      PFREE (grlc_data_req);
      TRACE_ERROR( "CCI_CIPHER_CNF unexpected" );
      break;
  }
} /* tx_cci_cipher_cnf() */
