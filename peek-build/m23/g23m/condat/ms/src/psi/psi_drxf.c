/*
+-----------------------------------------------------------------------------
|  File     : psi_drxf.c
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
|             SDL-documentation (DRX-statemachine)
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
#include "psi.h"      /* to get the global entity definitions */
#include "psi_txs.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : psi_drx_dti_reason_data_received
+------------------------------------------------------------------------------
| Description : reaction to the callback from dti lib
|
| Parameters  : data primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_drx_dti_reason_data_received(T_DTI2_DATA_IND *dti2_data_ind)
{
  TRACE_FUNCTION ("psi_drx_dti_reason_data_received()");

  switch(GET_STATE(PSI_SERVICE_DRX))
  {
    case PSI_DRX_READY:
      dti_stop(psi_data->hDTI, 
               psi_data->instance,      /* U8 instance */
               0,                       /* U8 interfac */
               0);                      /* U8 channel */
      SET_STATE(PSI_SERVICE_DRX, PSI_DRX_NOT_READY);
      switch(psi_data->ker.capabilities->device_type)
      {
        case DIO_TYPE_SER:
          psi_drx_tx_data(&dti2_data_ind->desc_list2,&dti2_data_ind->parameters );
          break;
        case DIO_TYPE_PKT:
          psi_drx_tx_data_pkt(&dti2_data_ind->desc_list2,dti2_data_ind->parameters.p_id );
          break;
        default:
          break;
      }
      PFREE(dti2_data_ind);/* give only primitive free, desc list later */
      break;
    case PSI_DRX_NOT_READY:
    case PSI_DRX_DEAD_NOT_READY:
    case PSI_DRX_DEAD_READY:
    default:
      TRACE_ERROR( "DTI_REASON_DATA_RECEIVED unexpected" );
      PFREE_DESC2(dti2_data_ind);
      break;
  } /* switch */
} /* psi_drx_dti_reason_data_received()() */


/*
+------------------------------------------------------------------------------
| Function    : psi_drx_init
+------------------------------------------------------------------------------
| Description : The function drx_init() initializes the service DRX
|
| Parameters  : none
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_drx_init (void)
{ 
  TRACE_FUNCTION( "psi_drx_init" );

  INIT_STATE(PSI_SERVICE_DRX, PSI_DRX_DEAD_NOT_READY);

} /* psi_drx_init() */



