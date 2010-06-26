/*
+-----------------------------------------------------------------------------
|  File     : psi_drxs.c
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
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (DRX-statemachine)
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

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : psi_tx_drx_close
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_TX_DRX_CLOSE
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_tx_drx_close (void)
{
  
  TRACE_FUNCTION ("psi_tx_drx_close()");

  switch(GET_STATE(PSI_SERVICE_DRX)) 
  {
    case PSI_DRX_READY:
      SET_STATE(PSI_SERVICE_DRX, PSI_DRX_NOT_READY);
      break;  
    case PSI_DRX_DEAD_READY:
      SET_STATE(PSI_SERVICE_DRX, PSI_DRX_DEAD_NOT_READY);
      break;  
    case PSI_DRX_NOT_READY:
    case PSI_DRX_DEAD_NOT_READY:
    default:
      /* TRACE_ERROR( "SIG_TX_DRX_CLOSE unexpected" );*/
      break;

  } /* switch */

} /* psi_tx_drx_close() */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_drx_close
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_KER_DRX_CLOSE
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_drx_close (void)
{
  
  TRACE_FUNCTION ("psi_ker_drx_close()");

  switch(GET_STATE(PSI_SERVICE_DRX)) 
  {
    case PSI_DRX_NOT_READY:
      SET_STATE(PSI_SERVICE_DRX, PSI_DRX_DEAD_NOT_READY);
      break;  
    case PSI_DRX_READY:
      SET_STATE(PSI_SERVICE_DRX, PSI_DRX_DEAD_READY);
      break;  
    case PSI_DRX_DEAD_READY:
    case PSI_DRX_DEAD_NOT_READY:
    default:
      TRACE_ERROR( "SIG_KER_DRX_CLOSE unexpected" );
      break;

  } /* switch */

} /* psi_ker_drx_close() */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_drx_open
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_KER_DRX_OPEN
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_drx_open (void)
{
  
  TRACE_FUNCTION ("psi_ker_drx_open()");

  switch(GET_STATE(PSI_SERVICE_DRX))
  {
    case PSI_DRX_DEAD_NOT_READY:
      SET_STATE(PSI_SERVICE_DRX, PSI_DRX_NOT_READY);  
      break;
    case PSI_DRX_DEAD_READY:
      dti_start(psi_data->hDTI, 
              psi_data->instance,      /* U8 instance */
              0,                       /* U8 interfac */
              0);                      /* U8 channel */
      SET_STATE(PSI_SERVICE_DRX, PSI_DRX_READY);
      break;  
    case PSI_DRX_READY:
    case PSI_DRX_NOT_READY:
    default:
      TRACE_ERROR( "SIG_KER_DRX_OPEN unexpected" );
      break;
  } /* switch */

} /* psi_ker_drx_open() */

/*
+------------------------------------------------------------------------------
| Function    : psi_tx_drx_ready
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_TX_DRX_READY
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_tx_drx_ready (void)
{  
  TRACE_FUNCTION ("psi_tx_drx_ready ()");

  switch(GET_STATE(PSI_SERVICE_DRX))
  {
    case PSI_DRX_DEAD_NOT_READY:
      SET_STATE(PSI_SERVICE_DRX, PSI_DRX_DEAD_READY);
      break;
    case PSI_DRX_NOT_READY:
      dti_start(psi_data->hDTI, 
                psi_data->instance,      /* U8 instance */
                0,                       /* U8 interfac */
                0);                      /* U8 channel  */
      SET_STATE(PSI_SERVICE_DRX, PSI_DRX_READY);
      break;
    case PSI_DRX_READY:                 /* after flushing */
      break;      
    case PSI_DRX_DEAD_READY:
    default:
      TRACE_ERROR( "SIG_TX_DRX_READY unexpected" );
      break;
  } /* switch */

} /* psi_tx_drx_ready () */

