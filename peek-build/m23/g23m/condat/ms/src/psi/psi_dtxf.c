/*
+-----------------------------------------------------------------------------
|  File     : psi_dtxf.c
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
|  Purpose  : This modul is part of the entity PSI and implements all 
|             procedures and functions as described in the 
|             SDL-documentation (DTX-statemachine)
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#define ENTITY_PSI

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"
#include "psi.h"       /* to get the global entity definitions */
#include "psi_rxs.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : psi_dtx_dti_reason_tx_buffer_full
+------------------------------------------------------------------------------
| Description : reaction to dti callback DTI_REASON_TX_BUFFER_FULL
|
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_dtx_dti_reason_tx_buffer_full (void)
{

  TRACE_FUNCTION("psi_dtx_dti_reason_tx_buffer_full()");

  switch(GET_STATE(PSI_SERVICE_DTX))
  {
    case PSI_DTX_READY:
      SET_STATE(PSI_SERVICE_DTX, PSI_DTX_NOT_READY);
      SET_STATE(PSI_SERVICE_RX, PSI_RX_NOT_READY);     
      break;
    case PSI_DTX_NOT_READY:
    case PSI_DTX_DEAD:
    default:
      TRACE_ERROR("DTI_REASON_TX_BUFFER_READY unexpected");
      break;
  } /* switch */

} /* psi_dtx_dti_reason_tx_buffer_full() */


/*
+------------------------------------------------------------------------------
| Function    : psi_dtx_dti_reason_tx_buffer_ready
+------------------------------------------------------------------------------
| Description : reaction to dti callback DTI_REASON_TX_BUFFER_READY
|
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_dtx_dti_reason_tx_buffer_ready (void)
{

  TRACE_FUNCTION("psi_dtx_dti_reason_tx_buffer_ready()");

  switch(GET_STATE(PSI_SERVICE_DTX)) 
  {
    case PSI_DTX_NOT_READY:
      SET_STATE(PSI_SERVICE_DTX, PSI_DTX_READY);
      psi_dtx_rx_ready();
      break;
    case PSI_DTX_READY:
    case PSI_DTX_DEAD:
    default:
      TRACE_ERROR("DTI_REASON_TX_BUFFER_READY unexpected");
      break;
  } /* switch */

} /* psi_dtx_dti_reason_tx_buffer_ready() */

/*
+------------------------------------------------------------------------------
| Function    : psi_dtx_init
+------------------------------------------------------------------------------
| Description : The function psi_dtx_init() initializes the service DTX 
|
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_dtx_init (void)
{

  TRACE_FUNCTION( "psi_dtx_init" );
  
  INIT_STATE(PSI_SERVICE_DTX, PSI_DTX_DEAD);

} /* psi_dtx_init() */

