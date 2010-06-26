/*
+-----------------------------------------------------------------------------
|  File     : psi_rxs.c
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
|             described in the SDL-documentation (RX-statemachine)
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_RXS_C
#define PSI_RXS_C
#endif /* !PSI_RXS_C */

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
#include "cnf_psi.h"  /* to get cnf-definitions */
#include "mon_psi.h"  /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "pei.h"        /* to get PEI interface */
#include "dti.h"
#include "psi.h"      /* to get the global entity definitions */
#include "psi_rxf.h"  /* to get rx functions */
#include "psi_kers.h" /* to get ker signals */
#include "psi_kerp.h" /* to get ker signals */
#include "psi_dtxs.h" /* to get dtx signals */
#include "psi_rxp.h"  /* to get rx_readdata */
#include "psi_txp.h"   /* to get TX signal definitions */
/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : psi_dtx_rx_close
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_DTX_RX_CLOSE
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_dtx_rx_close (void)
{
  
  TRACE_FUNCTION ("psi_dtx_rx_close()");

  switch(GET_STATE(PSI_SERVICE_RX)) 
  {
    case PSI_RX_READY:
      SET_STATE(PSI_SERVICE_RX, PSI_RX_NOT_READY);
      break;
    case PSI_RX_BUFFER:
    case PSI_RX_NOT_READY:
    case PSI_RX_DEAD_NOT_READY:
   default:
      TRACE_ERROR( "SIG_DTX_RX_CLOSE unexpected" );
      break;

  } /* switch */

} /* psi_dtx_rx_close() */

/*
+------------------------------------------------------------------------------
| Function    : psi_dtx_rx_ready
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_DTX_RX_READY
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_dtx_rx_ready (void)
{   
  BOOL abort_flag = FALSE;
  int i;
  
  TRACE_FUNCTION ("psi_dtx_rx_ready()");

  switch(GET_STATE(PSI_SERVICE_RX))
  {
    case PSI_RX_NOT_READY:
      SET_STATE(PSI_SERVICE_RX, PSI_RX_READY);
      break;
    case PSI_RX_BUFFER:
      /* check whether rx buffers are filled */
      for(i=0; i<PSI_READ_BUF_MAX;i++)
      {
        if(psi_data->rx.psi_buffer_pending[0] NEQ NULL)/* check if buffered data exist */
        {/* send buffered data to DTI2 */
          switch(psi_data->ker.capabilities->device_type)
          {
            case DIO_TYPE_SER:
              psi_rx_send_data_to_dtx(psi_data->rx.psi_buffer_pending[0],(T_DIO_CTRL*)&psi_data->rx.psi_control_info_ser);               
              break;
            case DIO_TYPE_PKT:
              psi_rx_send_data_to_dtx_pkt(psi_data->rx.psi_buffer_pending[0]);
              break;
            default:
              break;
          }
          psi_data->rx.psi_buffer_pend[0] = FALSE;
          /* structure of sent data is freed, descr data must be freed in ACI */
          psi_mfree_buffers(psi_data->rx.psi_buffer_pending[0]);
          /* controling of buffer which has to be sent after next dtx ready */
          if(psi_data->rx.next_send_id EQ BUFFER_2)
          {
            psi_data->rx.next_send_id = BUFFER_1;
          }
          else
          {  
             psi_data->rx.next_send_id = BUFFER_2; 
          }
          /* if second buffer is buffered it is to shift in pending list */
          if(psi_data->rx.psi_buffer_pending[1] NEQ NULL)
          {
            psi_data->rx.psi_buffer_pending[0] = psi_data->rx.psi_buffer_pending[1];
            psi_data->rx.psi_buffer_desc_pending[0]= psi_data->rx.psi_buffer_desc_pending[1];
            psi_data->rx.psi_buffer_pend[0] = psi_data->rx.psi_buffer_pend[1];
            psi_data->rx.psi_buffer_pending[1]=NULL;
            psi_data->rx.psi_buffer_desc_pending[1] = NULL;
            psi_data->rx.psi_buffer_pend[1] = FALSE;
          }
          else
          {
            psi_data->rx.psi_buffer_pending[0]=NULL;
            psi_data->rx.psi_buffer_desc_pending[0] = NULL;
          }
          /* it is possible that DTI2 sends DTI_REASON_TX_BUFFER_FULL immediately after first data sending (above) - because of this fact
          PSI has to check if the dtx state machine is changed to "not ready" - than rx machine state is set to RX_BUFFER and PSI waits
          for next dtx ready for sending of remain buffered data to DTI2. If already both read buffer contents are buffered in PSI
          the entity provides both read buffer to DIO only if nothing more is buffered in PSI */
          switch(GET_STATE(PSI_SERVICE_RX))
          {
            case PSI_RX_NOT_READY:
              if(psi_data->rx.psi_buffer_pending[0] NEQ NULL)
              {
                SET_STATE(PSI_SERVICE_RX, PSI_RX_BUFFER);
                abort_flag = TRUE;/* no more in the sending loop */
              }
              break;
            default:
              SET_STATE(PSI_SERVICE_RX, PSI_RX_READY);             
              break;
          }
          if(abort_flag)
          {/* no more in the loop */
            break;
          }        
        }
      }
      if(!abort_flag)
      {
        if(!psi_data->rx.psi_buffer_1_used AND !psi_data->rx.psi_buffer_2_used)
        {
          psi_data->rx.next_send_id = BUFFER_1;/* if both read buffer are sent immediately */
          psi_data->rx.psi_buffer_1_read = FALSE;
        }
        /* provide one or both read buffer to DIO */
        SET_STATE(PSI_SERVICE_RX, PSI_RX_READY);		
        psi_rx_read(); 
      }
      break;
    case PSI_RX_DEAD_NOT_READY:
      break;
    case PSI_RX_READY:
    default:
      TRACE_ERROR( "SIG_DTX_RX_READY unexpected" );
      break;
  } /* switch */

} /* psi_dtx_rx_ready() */


/*
+------------------------------------------------------------------------------
| Function    : psi_ker_rx_close
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_KER_RX_CLOSE
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_rx_close (void)
{
  int i;
  
  TRACE_FUNCTION ("psi_ker_rx_close()");

  switch(GET_STATE(PSI_SERVICE_RX)) 
  {
    case PSI_RX_BUFFER:

      for(i=0;i<PSI_READ_BUF_MAX;i++)
      {/* if pending buffers in PSI exist they must be freed */
        if(psi_data->rx.psi_buffer_pend[i])
        {
          psi_mfree_desc2_chain(psi_data->rx.psi_buffer_desc_pending[i]);
          psi_mfree_buffers(psi_data->rx.psi_buffer_pending[i]);
          psi_data->rx.psi_buffer_pending[i] = NULL;
          psi_data->rx.psi_buffer_desc_pending[i] = NULL;
          psi_data->rx.psi_buffer_pend[i] = FALSE;
        }
      }     
      psi_data->rx.psi_buffer_1_read = FALSE;
      psi_data->rx.next_send_id = BUFFER_1;
      SET_STATE(PSI_SERVICE_RX, PSI_RX_DEAD_NOT_READY);
      break;
    case PSI_RX_NOT_READY:
    case PSI_RX_READY:
      SET_STATE(PSI_SERVICE_RX, PSI_RX_DEAD_NOT_READY);
      break;
    case PSI_RX_DEAD_NOT_READY:
    default:
      TRACE_ERROR( "SIG_KER_RX_CLOSE unexpected" );
      break;
  } /* switch */

} /* psi_ker_rx_close() */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_rx_open
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_KER_RX_OPEN
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_rx_open(void)
{  
  TRACE_FUNCTION ("psi_ker_rx_open()");

  switch(GET_STATE(PSI_SERVICE_RX)) 
  {
    case PSI_RX_DEAD_NOT_READY:
      switch(GET_STATE(PSI_SERVICE_DTX))
      {
        case PSI_DTX_READY:
          SET_STATE(PSI_SERVICE_RX, PSI_RX_READY);
          break;
        default:
          SET_STATE(PSI_SERVICE_RX, PSI_RX_NOT_READY); 
          break;
      } /* switch */
      psi_rx_read();
      break;
    case PSI_RX_NOT_READY:
    case PSI_RX_READY:
    case PSI_RX_BUFFER:
    default:
      TRACE_ERROR( "SIG_KER_RX_OPEN unexpected" );
      break;
  } /* switch */
} /* psi_ker_rx_open() */

