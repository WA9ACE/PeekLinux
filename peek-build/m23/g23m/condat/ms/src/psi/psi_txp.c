/*
+-----------------------------------------------------------------------------
|  File     : psi_txp.c
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
|             functions to handles the incoming primitives as described in
|             the SDL-documentation (TX-statemachine)
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_TXP_C
#define PSI_TXP_C
#endif /* !PSI_TXP_C */

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
#include "cnf_psi.h"    /* to get cnf-definitions */
#include "mon_psi.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "DIO_inline.h" /* to get the function definition of used SAP DIO */
#include "pei.h"        /* to get PEI interface */
#include "dti.h"
#include "psi.h"        /* to get the global entity definitions */
#include "psi_kerf.h"   /* ker_device_no_switch() */
#include "psi_txf.h"    /* to get tx functions */
#include "psi_txs.h"    /* to get tx functions */
#include "psi_kers.h"   /* to get ker signals */
#include "psi_kerp.h"   /* to get ker signals */
#include "psi_drxs.h"   /* to get drx signals */
#include "psi_txp.h"    /* to get tx_writedata */
/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : psi_free_tx_buffer
+------------------------------------------------------------------------------
| Description : free dio buffer and descr list
|
| Parameters  : write buffer
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL psi_free_tx_buffer (T_dio_buffer* write_buffer)
{
  int i;

  TRACE_FUNCTION ("psi_free_tx_buffer()");

  for (i=0; i < PSI_WRITE_BUF_MAX; i++)
  {/* check if the received buffer is one of the sent buffer; PSI trusts the order of WRITE_IND */
    if (psi_data->tx.psi_buffer[i] EQ write_buffer)
    {
      psi_mfree_desc2_chain(psi_data->tx.psi_buf_desc[i]);/* free the according descr list sent by DTI2 */
      if(psi_data->tx.psi_buf_pid_desc[i] NEQ NULL)
      {
        psi_mfree_desc2_chain(psi_data->tx.psi_buf_pid_desc[i]);
      }
      psi_mfree_buffers(psi_data->tx.psi_buffer[i]);/* free the write buffer structure */
      psi_data->tx.psi_buffer[i]      = NULL;
      psi_data->tx.psi_buf_desc[i]    = NULL;
      psi_data->tx.psi_buf_pid_desc[i]= NULL;
      return TRUE;
    }
  }
  return FALSE;
} /* psi_free_tx_buffer() */

/*
+------------------------------------------------------------------------------
| Function    : psi_mfree_desc2_chain
+------------------------------------------------------------------------------
| Description : free descr list with elements received via DTI
|
| Parameters  : desc2 list
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_mfree_desc2_chain (T_desc2* dti_data_list)
{

  TRACE_FUNCTION ("psi_mfree_desc2_chain()");

  if(dti_data_list NEQ NULL)
  {
    MFREE_DESC2(dti_data_list);
    dti_data_list = 0L;
  }
} /* psi_mfree_desc2_chain() */
/*
+------------------------------------------------------------------------------
| Function    : psi_fill_tx_buf_list
+------------------------------------------------------------------------------
| Description : fill in write buffer in tx list
|
| Parameters  : write buffer
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_fill_tx_buf_list (T_dio_buffer* write_buffer, T_desc2* list, T_desc2* pid_buffer)
{
  int i;

  TRACE_FUNCTION ("psi_fill_tx_buf_list()");

  if(psi_data->tx.in_driver < PSI_WRITE_BUF_MAX)
  {
    for (i=0; i < PSI_WRITE_BUF_MAX; i++)
    {
      if (psi_data->tx.psi_buffer[i] EQ NULL)
      {/* after successful sending store the buffer and according desc list in write_list */
        psi_data->tx.psi_buffer[i]      = write_buffer;
        psi_data->tx.psi_buf_desc[i]    = list;
        if(pid_buffer NEQ NULL)
        {
          psi_data->tx.psi_buf_pid_desc[i] = pid_buffer;
        }
        break;
      }
    }
  }
} /* psi_fill_tx_buf_list() */

/*
+------------------------------------------------------------------------------
| Function    : psi_fill_tx_buf_pending_list
+------------------------------------------------------------------------------
| Description : fill in pending write buffer in tx list
|
| Parameters  : write buffer, T_desc2 chain, T_desc2 protocol id element
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_fill_tx_buf_pending_list (T_dio_buffer* write_buffer, T_desc2* list, T_desc2* pid_buffer)
{
  TRACE_FUNCTION ("psi_fill_tx_buf_pending_list()");

  psi_data->tx.psi_buffer_pending = write_buffer;
  psi_data->tx.psi_buffer_desc_pending = list;
  if(pid_buffer NEQ NULL)
  {
    psi_data->tx.psi_buffer_desc_pid_pending = pid_buffer;
  }
} /* psi_fill_tx_buf_pending_list() */

/*
+------------------------------------------------------------------------------
| Function    : psi_fill_tx_buf_flush_pending_list
+------------------------------------------------------------------------------
| Description : fill in pending write buffer by flush requirement in tx list
|
| Parameters  : write buffer, T_desc2 chain
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_fill_tx_buf_flush_pending_list (T_dio_buffer* write_buffer, T_desc2* list)
{
  TRACE_FUNCTION ("psi_fill_tx_buf_flush_pending_list()");

  psi_data->tx.psi_buffer_pending_flush = write_buffer;
  psi_data->tx.psi_buffer_desc_pending_flush = list;
} /* psi_fill_tx_buf_flush_pending_list() */



/*
+------------------------------------------------------------------------------
| Function    : psi_free_pending_tx_buf
+------------------------------------------------------------------------------
| Description : free entries of pending write buffer
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_free_pending_tx_buf (void)
{
  TRACE_FUNCTION ("psi_free_pending_tx_buf()");

  psi_data->tx.psi_buffer_pending = NULL;
  psi_data->tx.psi_buffer_desc_pending = NULL;
  psi_data->tx.psi_buffer_desc_pid_pending = NULL;
} /* psi_free_pending_tx_buf() */


/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : psi_tx_sig_flush_ind
+------------------------------------------------------------------------------
| Description : reaction to internal signal PSI_SIG_FLUSH_IND
|
| Parameters  : device number
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_tx_sig_flush_ind (U32 device)
{

  TRACE_FUNCTION ("psi_tx_sig_flush_ind()");

  if (psi_ker_search_basic_data_by_device(device) NEQ PSI_DEVICE_FOUND)
    return; /* No such device */

  switch(GET_STATE(PSI_SERVICE_TX))
  {
    case PSI_TX_FLUSHING:
      SET_STATE(PSI_SERVICE_TX, PSI_TX_READY);
      psi_ker_tx_flushed();
      psi_tx_drx_ready();  /* start DTI again */
      break;
    case PSI_TX_DEAD:
    case PSI_TX_BUFFER:
    case PSI_TX_READY:
    default:
      TRACE_ERROR( "PSI_SIG_FLUSH_IND unexpected" );
      break;
  } /* switch */
} /* psi_tx_sig_flush_ind() */

/*
+------------------------------------------------------------------------------
| Function    : psi_tx_sig_write_ind
+------------------------------------------------------------------------------
| Description : reaction to internal signal PSI_SIG_WRITE_IND
|
| Parameters  : device number
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_tx_sig_write_ind (U32 device)
{
  U16 result = 0;
  T_dio_buffer* buffer = NULL;
  BOOL found_buffer = FALSE;

  TRACE_FUNCTION ("psi_tx_sig_write_ind()");

  psi_ker_search_basic_data_by_device(device);

  switch(GET_STATE(PSI_SERVICE_TX))
  {
    case PSI_TX_BUFFER:
      dio_get_tx_buffer(psi_data->device_no, &buffer);
      /* TRACE_ERROR ("psi_tx_sig_write_ind:BUF write buffer back");*/
      if(buffer NEQ NULL)
      {
        if(psi_free_tx_buffer(buffer) EQ TRUE)
        {
           found_buffer = TRUE;
        }
        if(found_buffer)
        {
          if (psi_data->tx.in_driver > 0)
          {
            psi_data->tx.in_driver --;
          }
          /* send the buffered write buffer */
          if(!psi_data->tx.psi_dio_flush)
          {
            switch(psi_data->ker.capabilities->device_type)
            {
              case DIO_TYPE_SER:
                result = dio_write(psi_data->device_no,(T_DIO_CTRL*)&psi_data->tx.psi_control_info_ser,psi_data->tx.psi_buffer_pending);
                break;
              case DIO_TYPE_PKT:
                result = dio_write(psi_data->device_no,NULL,psi_data->tx.psi_buffer_pending);
                break;
              default:
                break;
            }

            /* TRACE_ERROR ("psi_tx_sig_write_ind:BUF send buffered write buffer to DIO"); */
            switch(result)
            {
              case DRV_INVALID_PARAMS:
                TRACE_ERROR("psi_tx_sig_write_ind(): BUF Buffer is to big");
                return;
              case DRV_OK:
                psi_fill_tx_buf_list(psi_data->tx.psi_buffer_pending,psi_data->tx.psi_buffer_desc_pending,psi_data->tx.psi_buffer_desc_pid_pending);
                psi_data->tx.in_driver ++;
                psi_free_pending_tx_buf();
                if(!psi_data->tx.psi_dio_flush)
                {
                  psi_tx_drx_ready();  /*start DTI2 again to receive data after write_ind for buffer_1 */
                }
              break;
              default:/* if result is f.e. DRV_BUFFER_FULL the service state PSI_SERVICE_TX is not changed (PSI_TX_BUFFER) and
                         PSI waits for the next WRITE_IND to send the pending buffer  */
                return;
            }
            SET_STATE(PSI_SERVICE_TX, PSI_TX_READY);
          }
          else
          {
            SET_STATE(PSI_SERVICE_TX, PSI_TX_READY);
            if(psi_data->tx.psi_dio_flush AND (psi_data->tx.in_driver EQ 0))
            {/* start flushing procedure */
              psi_data->tx.psi_dio_flush = FALSE;
              psi_ker_tx_flush();
            }
          }
        }
        else
        {
          TRACE_ERROR("psi_tx_sig_write_ind(): BUF buf no buffer found");
        }
      }
      else
      {
         TRACE_ERROR("psi_tx_sig_write_ind(): BUF Buffer is NULL");
      }
      break;
    case PSI_TX_READY:
      dio_get_tx_buffer(psi_data->device_no, &buffer);
      /* TRACE_ERROR ("psi_tx_sig_write_ind:RDY get write buffer back"); */
      if(buffer NEQ NULL)
      {
        if(psi_free_tx_buffer(buffer) EQ TRUE)
        {
           found_buffer = TRUE;
        }
        if(found_buffer)
        {
          if (psi_data->tx.in_driver > 0)
          {
            psi_data->tx.in_driver --;
          }
          if(psi_data->tx.psi_dio_flush AND (psi_data->tx.in_driver EQ 0))
          {
            psi_data->tx.psi_dio_flush = FALSE;
            psi_ker_tx_flush();/* start flushing procedure */
          }
        }
        else
        {
          TRACE_ERROR("psi_tx_sig_write_ind():RDY no buffer found");
        }
      }
      else
      {
        TRACE_ERROR("psi_tx_sig_write_ind():RDY Buffer is NULL");
      }
      break;
    case PSI_TX_DEAD:
    default:
      TRACE_ERROR( "PSI_SIG_WRITE_IND unexpected" );
      break;
  } /* switch */
} /* psi_tx_sig_write_ind() */

