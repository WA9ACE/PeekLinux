/*
+-----------------------------------------------------------------------------
|  File     : psi_txs.c
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
|             described in the SDL-documentation (TX-statemachine)
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_TXS_C
#define PSI_TXS_C
#endif /* !PSI_TXS_C */

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
#include "DIO_inline.h" /* to get the function definition of used SAP DIO */
#include "pei.h"        /* to get PEI interface */
#include "dti.h"
#include "psi.h"      /* to get the global entity definitions */
#include "psi_txf.h"  /* to get tx functions */
#include "psi_kers.h"  /* to get ker signals */
#include "psi_kerp.h"  /* to get ker signals */
#include "psi_drxs.h"  /* to get drx signals */
#include "psi_txp.h"  /* to get tx_writedata */
#include "psi_txs.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/
/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : psi_drx_tx_data
+------------------------------------------------------------------------------
| Description : data received from DTI2
|
| Parameters  : data, line states
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_drx_tx_data (T_desc_list2* list, T_parameters* dtx_control_info)
{
  U16 result = DRV_OK;
  T_dio_buffer* buffer = NULL;
  BOOL only_line_states = FALSE;
  T_desc2* p_id_buf = NULL;

  TRACE_FUNCTION ("psi_drx_tx_data()");

  switch(GET_STATE(PSI_SERVICE_TX))
  {
    case PSI_TX_READY:
      if((list->list_len NEQ 0) AND ((T_desc2*)list->first NEQ NULL))
      {/* translate data from desc list to T_dio_buffer struc */
        psi_create_send_buffer(&buffer,list,&p_id_buf, DTI_PID_UOS);
        psi_converts_control_info_data(dtx_control_info);
        if(psi_data->tx.psi_dio_flush)
        {/* flushing is necessary because DIO_SB/SA have been changed; send buffered data only after flushing */
            psi_fill_tx_buf_flush_pending_list(buffer, (T_desc2*)list->first);
            /* TRACE_FUNCTION("psi_drx_tx_data(): received DTI data buffered because flush needed");*/
            if(psi_data->tx.in_driver NEQ 0)
            {
              SET_STATE(PSI_SERVICE_TX, PSI_TX_BUFFER);
            }
            else
            {/* start flush procedure immediately */
              psi_data->tx.psi_dio_flush = FALSE;
              psi_ker_tx_flush();              
            }                      
            break;
        }
        else
        {
          if (psi_data->tx.in_driver EQ PSI_WRITE_BUF_MAX) 
          {
            psi_fill_tx_buf_pending_list(buffer, (T_desc2*)list->first, p_id_buf);
            /*TRACE_FUNCTION("psi_drx_tx_data(): received DTI data buffered"); */                   
            SET_STATE(PSI_SERVICE_TX, PSI_TX_BUFFER);
            break;
          }    
          /* provide buffer to DIO: send data   */
          result = dio_write(psi_data->device_no,(T_DIO_CTRL*)&psi_data->tx.psi_control_info_ser,buffer);
          /* TRACE_FUNCTION ("psi_drx_tx_data: send write buffer to DIO");  */
        }
      }
      else
      {
        only_line_states = TRUE;
        /* only send changed line states */
        psi_converts_control_info_data(dtx_control_info);
        psi_mfree_desc2_chain((T_desc2*)list->first);
        if(psi_data->tx.psi_dio_flush)
        {/* flushing is necessary because DIO_SB/SA have been changed */
          if(psi_data->tx.in_driver NEQ 0)
          {
             SET_STATE(PSI_SERVICE_TX, PSI_TX_BUFFER);
          }
          else
          {/* start flush procedure immediately */
              psi_data->tx.psi_dio_flush = FALSE;
              psi_ker_tx_flush();
          }
          break; /* waiting for WRITE_IND until in_driver EQ 0 */
        }
        /* Need not send line state to DIO if its not changed */
      }
      switch(result)
      {
        case DRV_INVALID_PARAMS:
          TRACE_ERROR("psi_drx_tx_data(): Buffer is to big");      
          return;
        case DRV_OK:
          if(!only_line_states)
          {
             psi_fill_tx_buf_list(buffer,(T_desc2*)list->first, p_id_buf);         
             psi_data->tx.in_driver ++;
          }
          if(!psi_data->tx.psi_dio_flush)
          {
            /* TRACE_FUNCTION ("psi_drx_tx_data: send write buffer 1 to DIO");*/
            psi_tx_drx_ready(); /* start DTI2 again to receive data */
          }
          break;
        case DRV_BUFFER_FULL:
          if(!only_line_states)
          {          
            psi_fill_tx_buf_pending_list(buffer, (T_desc2*)list->first, p_id_buf);
            SET_STATE(PSI_SERVICE_TX, PSI_TX_BUFFER);
          }
          else
          {
            TRACE_ERROR("psi_drx_tx_data: error change line states");
          }
          break;
        default:
          TRACE_ERROR("psi_drx_tx_data: error dio_write");
          psi_mfree_desc2_chain((T_desc2*)list->first);
          if(buffer NEQ NULL)
          {
            psi_mfree_buffers(buffer);
          }
          return;
      }
      break;
    case PSI_TX_FLUSHING:
      break;
    case PSI_TX_BUFFER:
    case PSI_TX_DEAD:
    default:
      TRACE_ERROR( "SIG_DRX_TX_DATA unexpected" );
      break;
  } /* switch */

} /* psi_drx_tx_data() */

/*
+------------------------------------------------------------------------------
| Function    : psi_drx_tx_data_pkt
+------------------------------------------------------------------------------
| Description : data received from DTI2
|
| Parameters  : data, protocol identifier
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_drx_tx_data_pkt (T_desc_list2* list, U8 protocol_id)
{
  U16 result = DRV_OK;
  T_dio_buffer* buffer = NULL;
  T_desc2* p_id_buf = NULL;

  TRACE_FUNCTION ("psi_drx_tx_data_pkt()");

  switch(GET_STATE(PSI_SERVICE_TX))
  {
    case PSI_TX_READY:
      if((list->list_len NEQ 0) AND ((T_desc2*)list->first NEQ NULL))
      {/* translate data from desc list to T_dio_buffer struc */
        psi_create_send_buffer(&buffer,list,&p_id_buf,protocol_id);
        if (psi_data->tx.in_driver EQ PSI_WRITE_BUF_MAX) 
        {
          psi_fill_tx_buf_pending_list(buffer, (T_desc2*)list->first, p_id_buf);
          /*TRACE_FUNCTION("psi_drx_tx_data_pkt(): received DTI data buffered"); */                   
          SET_STATE(PSI_SERVICE_TX, PSI_TX_BUFFER);
          break;
        }    
        /* provide buffer to DIO: send data without any line states  */
        result = dio_write(psi_data->device_no,NULL,buffer);
        /* TRACE_FUNCTION ("psi_drx_tx_data_pkt: send write buffer to DIO");  */
      }
      else
      {/* delete only desc2, T_dio_buffer and/or p_id element do not exist yet */
        psi_mfree_desc2_chain((T_desc2*)list->first);
      }
      switch(result)
      {
        case DRV_INVALID_PARAMS:
          TRACE_ERROR("psi_drx_tx_data_pkt(): Buffer is to big");      
          return;
        case DRV_OK: 
          psi_fill_tx_buf_list(buffer,(T_desc2*)list->first, p_id_buf);         
          psi_data->tx.in_driver ++;
          /* TRACE_FUNCTION ("psi_drx_tx_data_pkt: send write buffer 1 to DIO");*/
          psi_tx_drx_ready(); /* start DTI2 again to receive data */
          break;
        case DRV_BUFFER_FULL:         
          psi_fill_tx_buf_pending_list(buffer, (T_desc2*)list->first, p_id_buf);
          SET_STATE(PSI_SERVICE_TX, PSI_TX_BUFFER);
          break;
        default:
          TRACE_ERROR("psi_drx_tx_data_pkt: error dio_write");
          psi_mfree_desc2_chain((T_desc2*)list->first);
          psi_mfree_desc2_chain(p_id_buf);
          
          if(buffer NEQ NULL)
          {/* free the write buffer structure */
            psi_mfree_buffers(buffer);
          }
          return;
      }
      break;
    case PSI_TX_FLUSHING:
      break;
    case PSI_TX_BUFFER:
    case PSI_TX_DEAD:
    default:
      TRACE_ERROR( "SIG_DRX_TX_DATA unexpected" );
      break;
  } /* switch */

} /* psi_drx_tx_data_pkt() */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_tx_close
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_KER_TX_CLOSE
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_tx_close (void)
{
  
  TRACE_FUNCTION ("psi_ker_tx_close()");

  switch(GET_STATE(PSI_SERVICE_TX)) 
  {
    case PSI_TX_READY:
    case PSI_TX_FLUSHING:
    case PSI_TX_BUFFER:
      SET_STATE(PSI_SERVICE_TX, PSI_TX_DEAD);
      psi_tx_drx_close();
      break;
    case PSI_TX_DEAD:
    default:
      TRACE_ERROR( "SIG_KER_TX_CLOSE unexpected" );
      break;

  } /* switch */

} /* psi_ker_tx_close() */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_tx_flush
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_KER_TX_FLUSH
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_tx_flush (void)
{
  U16 result = 0;
  
  TRACE_FUNCTION("psi_ker_tx_flush()");

  switch(GET_STATE(PSI_SERVICE_TX)) 
  {
    case PSI_TX_BUFFER:
      /* waiting for sig_write_ind */
      psi_data->tx.psi_dio_flush = TRUE;
      TRACE_FUNCTION("psi_ker_tx_flush()PSI_TX_BUFFER");
      break;
    case PSI_TX_READY:
      result = dio_flush(psi_data->device_no);
      if (result EQ DRV_OK) 
      {
        psi_ker_tx_flushed();/* send new driver configuration parameter */
        if(psi_data->ker.dti_state NEQ PSI_KER_DTI_DEAD)
        {
          psi_tx_drx_ready();  /* start DTI again */
        }
      } 
      else if (result EQ DRV_INPROCESS) 
      {
        TRACE_FUNCTION("psi_ker_tx_flush()DRV_INPROCESS");
        SET_STATE(PSI_SERVICE_TX, PSI_TX_FLUSHING);
      } 
      else 
      {
        TRACE_EVENT_P1("dio_flush() returned %d.", result);
      }
      break;
    case PSI_TX_FLUSHING:
    case PSI_TX_DEAD:
    default:
      TRACE_ERROR("SIG_KER_TX_FLUSH unexpected");
      break;

  } /* switch */

} /* psi_ker_tx_flush() */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_tx_open
+------------------------------------------------------------------------------
| Description : reaction to internal signal SIG_KER_TX_OPEN
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_tx_open(void)
{
  
  TRACE_FUNCTION ("psi_ker_tx_open()");

  switch(GET_STATE(PSI_SERVICE_TX))
  {
    case PSI_TX_DEAD:
      psi_tx_drx_ready();
      SET_STATE(PSI_SERVICE_TX, PSI_TX_READY);
      break;
    case PSI_TX_READY:
    case PSI_TX_BUFFER:
    case PSI_TX_FLUSHING:
    default:
      TRACE_ERROR( "SIG_KER_TX_OPEN unexpected" );
      break;
  } /* switch */

} /* psi_ker_tx_open() */

