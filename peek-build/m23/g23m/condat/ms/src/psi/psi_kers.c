/*
+-----------------------------------------------------------------------------
|  File     : psi_kers.c
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
|             described in the SDL-documentation (KER-statemachine)
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_KERS_C
#define PSI_KERS_C
#endif /* !PSI_KERS_C */

#define ENTITY_PSI

/*==== INCLUDES =============================================================*/

#ifdef _SIMULATION_
#include "nucleus.h"
#endif /* _SIMULATION_ */
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_psi.h"   /* to get cnf-definitions */
#include "mon_psi.h"   /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "DIO_inline.h" /* to get the function definition of used SAP DIO */
#include "pei.h"        /* to get PEI interface */
#include "dti.h"
#include "psi.h"       /* to get the global entity definitions */

#include "psi_txs.h"   /* to get signal definitions of service TX */
#include "psi_rxs.h"   /* to get signal definitions of service RX */
#include "psi_dtxs.h"  /* to get signal definitions of service DTX */
#include "psi_drxs.h"  /* to get signal definitions of service DRX */
#include "psi_kerf.h"  /* to get function definitions of service KER */
#include "psi_txp.h"   /* to get signal definitions of service TX */

#ifndef _TARGET_
#include <stdio.h>      /* to get sprintf */
#endif /* !_TARGET_ */
#include <string.h>      /* to get memcpy */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_tx_flushed
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_TX_KER_FLUSHED
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_tx_flushed (void)
{
  U16 result = DRV_OK;
  TRACE_FUNCTION ("psi_ker_tx_flushed()");
  
  switch(GET_STATE(PSI_SERVICE_KER)) 
  {
    case PSI_KER_MODIFY:
      SET_STATE(PSI_SERVICE_KER, PSI_KER_READY);
      result = dio_set_config(psi_data->device_no, (T_DIO_DCB*)psi_data->dcb);      
      /* send confirmation to ACI */
      {
        PALLOC(psi_setconf_cnf, PSI_SETCONF_CNF);
        psi_ker_assign_cause(&psi_setconf_cnf->cause, result);
        psi_setconf_cnf->devId = psi_data->device_no;
        PSEND(hCommMMI, psi_setconf_cnf);
      }
      break;
    case PSI_KER_READY:
        /* provide changed line states to DIO after flush*/
        result = dio_write(psi_data->device_no,(T_DIO_CTRL*)&psi_data->tx.psi_control_info_ser,NULL); 
        /*TRACE_FUNCTION ("psi_ker_tx_flushed: send line states to DIO");  */
        switch(result)
        {
          case DRV_INVALID_PARAMS:
            TRACE_ERROR("psi_ker_tx_flushed(): DRV_INVALID_PARAMS");      
            break;
          case DRV_OK:
            /*TRACE_FUNCTION ("psi_ker_tx_flushed: DRV_OK"); */
            /* send buffered data added to new line states to driver */
            if(psi_data->tx.psi_buffer_pending_flush NEQ NULL)
            {
              /* TRACE_FUNCTION("psi_ker_tx_flushed(): send flush Buffer");*/
              result = dio_write(psi_data->device_no,NULL,psi_data->tx.psi_buffer_pending_flush);
              switch(result)
              {
                case DRV_INVALID_PARAMS:
                  TRACE_ERROR("psi_ker_tx_flushed(): flush Buffer is to big");      
                  return;
                case DRV_OK:
                  psi_fill_tx_buf_list(psi_data->tx.psi_buffer_pending_flush, psi_data->tx.psi_buffer_desc_pending_flush, NULL);
                  psi_data->tx.in_driver ++;
                  psi_data->tx.psi_buffer_pending_flush = NULL;
                  psi_data->tx.psi_buffer_desc_pending_flush = NULL;         
                break;
                case DRV_BUFFER_FULL:
                  TRACE_ERROR("psi_ker_tx_flushed(): drv Buffer full");      
                  psi_data->tx.psi_buffer_pending = psi_data->tx.psi_buffer_pending_flush;
                  psi_data->tx.psi_buffer_desc_pending = psi_data->tx.psi_buffer_desc_pending_flush;
                  psi_data->tx.psi_buffer_pending_flush = NULL;
                  psi_data->tx.psi_buffer_desc_pending_flush = NULL;         
                  SET_STATE(PSI_SERVICE_TX, PSI_TX_BUFFER);
                  break;
                default:
                  TRACE_ERROR("psi_ker_tx_flushed: error dio_write");
                  return;
              }             
            }
            if(psi_data->tx.flag_line_state_req)
            {
              PALLOC(psi_line_state_cnf, PSI_LINE_STATE_CNF);
              psi_line_state_cnf->devId = psi_data->device_no;
              PSEND(hCommMMI, psi_line_state_cnf);
              psi_data->tx.flag_line_state_req = FALSE;
            }
            break;
          case DRV_BUFFER_FULL:
            TRACE_ERROR("psi_ker_tx_flushed: error control info");
            break;
          default:
            TRACE_ERROR("psi_ker_tx_flushed: error control info");
            break;
        }       
      break;
    case PSI_KER_CONNECTING:
    case PSI_KER_DEAD:
    default:
      break;
  } /* switch */
} /* psi_ker_tx_flushed() */


