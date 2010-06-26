/*
+-----------------------------------------------------------------------------
|  File     : psi_rxp.c
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
|             the SDL-documentation (RX-statemachine)
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_RXP_C
#define PSI_RXP_C
#endif /* #ifndef PSI_RXP_C */

#define ENTITY_PSI


/*==== INCLUDES =============================================================*/

#ifdef _SIMULATION_
#include "nucleus.h"
#endif /* _SIMULATION_ */

#include "typedefs.h"         /* Condat data types */
#include "vsi.h"              /* a lot of macros */
#include "macdef.h"           /* a lot of macros */
#include "custom.h"
#include "gsm.h"              /* a lot of macros */
#include "cnf_psi.h"          /* cnf-definitions */
#include "mon_psi.h"          /* mon-definitions */
#include "prim.h"             /* the definitions of used SAP and directions */
#include "DIO_inline.h" /* to get the function definition of used SAP DIO */
#include "dti.h"              /* dti lib */
#include "pei.h"              /* PEI interfac */
#include "psi.h"             /* the global entity definitions */
#include "psi_kerf.h"       /* ker_device_no_switch() */
#include "psi_rxf.h"        /* rx functions */
#include "psi_kers.h"       /* ker signals */
#include "psi_kerp.h"       /* ker signals */
#include "psi_dtxs.h"       /* dtx signals */
#include <string.h>
#include <stdio.h>

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : psi_store_pend_read_buf
+------------------------------------------------------------------------------
| Description : storing of rx data in buffer list
|               until dtx is able to send data to upper layer
|
| Parameters  : read buffer data
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_store_pend_read_buf(T_dio_buffer* read_buffer, T_desc2* list)
{
  int i;
  TRACE_FUNCTION ("psi_store_pend_read_buf()");
 
  for (i=0; i < PSI_READ_BUF_MAX; i++)
  {
    if (psi_data->rx.psi_buffer_pending[i]EQ NULL) 
    {
      psi_data->rx.psi_buffer_pending[i]      = read_buffer;
      psi_data->rx.psi_buffer_desc_pending[i] = list;
      psi_data->rx.psi_buffer_pend[i] = TRUE; 
      break;
    }
  }                       
}
/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : psi_tx_sig_read_ind
+------------------------------------------------------------------------------
| Description : reaction to internal signal PSI_SIG_READ_IND
|
| Parameters  : device number
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_tx_sig_read_ind (U32 device)
{
  U16 result = 0;
  T_DIO_CTRL_LINES *ctrl_info = NULL;
  T_dio_buffer* buffer = NULL;
  
  TRACE_FUNCTION ("psi_tx_sig_read_ind()");

  psi_ker_search_basic_data_by_device(device);
  /* allocate struc for line states */
  psi_malloc_ctrl_ser(&ctrl_info);

  switch(GET_STATE(PSI_SERVICE_RX))
  {
    case PSI_RX_NOT_READY:
      /* read first buffer -> get back buffer 1 control */ 
      if(psi_data->rx.psi_buffer_1_used AND (psi_data->rx.next_send_id EQ BUFFER_1))
      {
        switch(psi_data->ker.capabilities->device_type)
        {
          case DIO_TYPE_SER:
            result = dio_read(psi_data->device_no, (T_DIO_CTRL*)ctrl_info, &buffer);
            if(result EQ DRV_OK)
            {/* evaluate  escape sequence detection and DTR drop */
              psi_check_control_info(psi_data->device_no,ctrl_info);
            }
            else
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 1 in PSI_RX_NOT_READY");
            }
            break;
          case DIO_TYPE_PKT:
            result = dio_read(psi_data->device_no, NULL, &buffer);/* no line states support */
            if(result NEQ DRV_OK)
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 1 in PSI_RX_NOT_READY");
            }
            break;
          default:
            TRACE_ERROR( "SIG_READ_IND: wrong device type" );
            break;
        }
        if(result EQ DRV_OK)
        {
          if(buffer NEQ NULL)
          {
            if(buffer EQ psi_data->rx.psi_buffer_1)
            {/* received buffer 1 is pending and is stored in pending buffer list */                         
              psi_store_pend_read_buf(buffer,psi_data->rx.psi_buffer_1_desc);
              psi_data->rx.psi_buffer_1_used = FALSE;/* first buffer already free */
              psi_data->rx.psi_buffer_1_read = TRUE;
            }
            else
            {
              TRACE_ERROR( "SIG_READ_IND: no buffer 1 in PSI_RX_NOT_READY" );
            }
          }
        }
        else
          break;
        /* TRACE_ERROR ("psi_tx_sig_read_ind:NOT RDY get read buf_1 back"); */
      }
      else if(psi_data->rx.psi_buffer_2_used AND (psi_data->rx.next_send_id EQ BUFFER_2))
      {
        switch(psi_data->ker.capabilities->device_type)
        {
          case DIO_TYPE_SER:
            result = dio_read(psi_data->device_no, (T_DIO_CTRL*)ctrl_info, &buffer);
            if(result EQ DRV_OK)
            {/* evaluate  escape sequence detection and DTR drop */
              psi_check_control_info(psi_data->device_no,ctrl_info);
            }
            else
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 2 in PSI_RX_NOT_READY");
            }
            break;
          case DIO_TYPE_PKT:
            result = dio_read(psi_data->device_no, NULL, &buffer);/* no line states support */
            if(result NEQ DRV_OK)
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 2 in PSI_RX_NOT_READY");
            }
            break;
          default:
            TRACE_ERROR( "SIG_READ_IND: wrong device type" );
            break;
        }
        if(result EQ DRV_OK)
        {
          if(buffer NEQ NULL)
          {
            if(buffer EQ psi_data->rx.psi_buffer_2)
            {/* received buffer 2 is pending and is stored in pending buffer list */                         
              psi_store_pend_read_buf(buffer,psi_data->rx.psi_buffer_2_desc);
              psi_data->rx.psi_buffer_2_used = FALSE;/* second buffer already free */
              psi_data->rx.psi_buffer_1_read = FALSE;
            }
            else
            {
              TRACE_ERROR( "SIG_READ_IND: no buffer 2 in PSI_RX_NOT_READY" );
            }
          }
        }
        else
          break;
      }
      else
      {
        TRACE_ERROR("psi_tx_sig_read_ind : SHOULD NOT COME");
      }
      /* no further PSI provides read buffer to DIO until dtx is ready */ 
      SET_STATE(PSI_SERVICE_RX, PSI_RX_BUFFER);      
      break;
    case PSI_RX_BUFFER:
      if(psi_data->rx.psi_buffer_1_used) 
      {
        switch(psi_data->ker.capabilities->device_type)
        {
          case DIO_TYPE_SER:
            result = dio_read(psi_data->device_no, (T_DIO_CTRL*)ctrl_info, &buffer);
            /*  TRACE_ERROR ("psi_tx_sig_read_ind:BUF get read buf_1 back ser"); */
            if(result EQ DRV_OK)
            {/* evaluate  escape sequence detection and DTR drop */
              psi_check_control_info(psi_data->device_no,ctrl_info);
            }
            else
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 1 in PSI_RX_BUFFER");
            }
            break;
          case DIO_TYPE_PKT:
            result = dio_read(psi_data->device_no, NULL, &buffer);/* no line states support */
            /*  TRACE_ERROR ("psi_tx_sig_read_ind:BUF get read buf_1 back pkt"); */
            if(result NEQ DRV_OK)
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 1 in PSI_RX_BUFFER");
            }
            break;
          default:
            TRACE_ERROR( "SIG_READ_IND: wrong device type" );
            break;
        }
        if(result EQ DRV_OK)
        {
          if(buffer NEQ NULL)
          {
            if(buffer EQ psi_data->rx.psi_buffer_1)
            {/* received buffer 1 is pending and is stored in pending buffer list */                         
              psi_store_pend_read_buf(buffer,psi_data->rx.psi_buffer_1_desc);
              psi_data->rx.psi_buffer_1_used = FALSE;/* first buffer already free */
              psi_data->rx.psi_buffer_1_read = TRUE;
            }
            else
            {
              TRACE_ERROR( "SIG_READ_IND: no buffer 1 in PSI_RX_BUFFER" );
            }
          }
        }
        else
          break;
      }
      else if(psi_data->rx.psi_buffer_2_used)
      {
        switch(psi_data->ker.capabilities->device_type)
        {
          case DIO_TYPE_SER:
            result = dio_read(psi_data->device_no, (T_DIO_CTRL*)ctrl_info, &buffer);
           /* TRACE_ERROR ("psi_tx_sig_read_ind:BUF get read buf_2 back ser"); */
            if(result EQ DRV_OK)
            {/* evaluate  escape sequence detection and DTR drop */
              psi_check_control_info(psi_data->device_no,ctrl_info);
            }
            else
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 2 in PSI_RX_BUFFER");
            }
            break;
          case DIO_TYPE_PKT:
            result = dio_read(psi_data->device_no, NULL, &buffer);/* no line states support */
            /* TRACE_ERROR ("psi_tx_sig_read_ind:BUF get read buf_2 back pkt"); */
            if(result NEQ DRV_OK)
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 2 in PSI_RX_BUFFER");
            }
            break;
          default:
            TRACE_ERROR( "SIG_READ_IND: wrong device type" );
            break;
        }
        if(result EQ DRV_OK)
        {
          if(buffer NEQ NULL)
          {
            if(buffer EQ psi_data->rx.psi_buffer_2)
            {/* received buffer 2 is pending and is stored in pending buffer list */                         
              psi_store_pend_read_buf(buffer,psi_data->rx.psi_buffer_2_desc);
              psi_data->rx.psi_buffer_2_used = FALSE;/* second buffer already free */
              psi_data->rx.psi_buffer_1_read = FALSE;
            }
            else
            {
              TRACE_ERROR( "SIG_READ_IND: no buffer 2 in PSI_RX_BUFFER" );
            }
          }
        }
      }      /* no further PSI provides read buffer to DIO until dtx is ready */
      else
      {
        switch(psi_data->ker.capabilities->device_type)
        {
          case DIO_TYPE_SER:
            result = dio_read(psi_data->device_no, (T_DIO_CTRL*)ctrl_info, &buffer);
            if(result EQ DRV_OK)
            {/* evaluate  escape sequence detection and DTR drop */
              TRACE_ERROR("psi_tx_sig_read_ind : PSI_RX_BUFFER : READ CRTL PARAMETERS");
              psi_check_control_info(psi_data->device_no,ctrl_info);
            }
            else
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 1 in PSI_RX_NOT_READY");
            }
            break;
          
          default:
            TRACE_ERROR( "SIG_READ_IND: wrong device type" );
            break;
        }
      }
      break;
    case PSI_RX_READY:
      /* read first buffer -> get back buffer 1 control */ 
      if(psi_data->rx.psi_buffer_1_used AND !psi_data->rx.psi_buffer_1_read)
      {
        switch(psi_data->ker.capabilities->device_type)
        {
          case DIO_TYPE_SER:
            result = dio_read(psi_data->device_no, (T_DIO_CTRL*)ctrl_info, &buffer);
            /* TRACE_ERROR ("psi_tx_sig_read_ind:RDY get read buf_1 back ser");*/
            if(result EQ DRV_OK)
            {/* evaluate  escape sequence detection and DTR drop */
              psi_check_control_info(psi_data->device_no,ctrl_info);
            }
            else
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 1 in PSI_RX_BUFFER");
            }
            break;
          case DIO_TYPE_PKT:
            result = dio_read(psi_data->device_no, NULL, &buffer);/* no line states support */
           /* TRACE_ERROR ("psi_tx_sig_read_ind:RDY get read buf_1 back pkt");*/
            if(result NEQ DRV_OK)
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 1 in PSI_RX_BUFFER");
            }
            break;
          default:
            break;
        }
        if(result EQ DRV_OK)
        {         
          if(buffer NEQ NULL)
          {
            if(buffer EQ psi_data->rx.psi_buffer_1)
            {
              switch(psi_data->ker.capabilities->device_type)
              {
                case DIO_TYPE_SER:
                  psi_rx_send_data_to_dtx(psi_data->rx.psi_buffer_1, (T_DIO_CTRL*)&psi_data->rx.psi_control_info_ser);                 
                  break;
                case DIO_TYPE_PKT:
                  psi_rx_send_data_to_dtx_pkt(psi_data->rx.psi_buffer_1);
                  break;
                default:
                  break;
              }
              psi_data->rx.psi_buffer_1_used = FALSE;/* first buffer already free */     
              psi_data->rx.next_send_id = BUFFER_2;
              psi_data->rx.psi_buffer_1_read = TRUE;
              psi_mfree_buffers(psi_data->rx.psi_buffer_1);
            }
            else
            {
              TRACE_ERROR( "SIG_READ_IND: no buffer 1" );
            }
          } 
        }
        else
        {
          TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 1" );
        }
      }
      /* read second buffer -> get back buffer control */ 
      else if(psi_data->rx.psi_buffer_2_used AND psi_data->rx.psi_buffer_1_read)
      {
        buffer = NULL;
        switch(psi_data->ker.capabilities->device_type)
        {
          case DIO_TYPE_SER:
            result = dio_read(psi_data->device_no, (T_DIO_CTRL*)ctrl_info, &buffer);
            /* TRACE_ERROR ("psi_tx_sig_read_ind:RDY get read buf_2 back ser");*/
            if(result EQ DRV_OK)
            {/* evaluate  escape sequence detection and DTR drop */
              psi_check_control_info(psi_data->device_no,ctrl_info);
            }
            else
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 1 in PSI_RX_BUFFER");
            }
            break;
          case DIO_TYPE_PKT:
            result = dio_read(psi_data->device_no, NULL, &buffer);/* no line states support */
           /* TRACE_ERROR ("psi_tx_sig_read_ind:RDY get read buf_2 back pkt");*/
            if(result NEQ DRV_OK)
            {
             TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 1 in PSI_RX_BUFFER");
            }
            break;
          default:
            TRACE_ERROR( "SIG_READ_IND: wrong device type" );
            break;
        }
        if(result EQ DRV_OK)
        {
          if(buffer NEQ NULL)
          {
            if(buffer EQ psi_data->rx.psi_buffer_2)
            {/* received buffer 2 is pending */              
              psi_data->rx.psi_buffer_1_read = FALSE;
              switch(psi_data->ker.capabilities->device_type)
              {
                case DIO_TYPE_SER:
                  psi_rx_send_data_to_dtx(psi_data->rx.psi_buffer_2, (T_DIO_CTRL*)&psi_data->rx.psi_control_info_ser);                
                  break;
                case DIO_TYPE_PKT:
                  psi_rx_send_data_to_dtx_pkt(psi_data->rx.psi_buffer_2);
                  break;
                default:
                  break;
              }
              psi_data->rx.psi_buffer_2_used = FALSE;/* second buffer already free */
              psi_data->rx.next_send_id = BUFFER_1;
              psi_mfree_buffers(psi_data->rx.psi_buffer_2);
            }
            else
            {
              TRACE_ERROR( "SIG_READ_IND: no buffer 2" );              
            }
          }
        }
        else
        {
          TRACE_ERROR( "SIG_READ_IND: no DRV_OK buffer 2 ");
        }
      }
      else
      {
          TRACE_ERROR( "SIG_READ_IND/RDY: no known buffer/wrong order");
          break;
      }      
      /* state PSI_RX_READY stays until BUFFER_FULL by DTI2 (DTI2 queue size 1) */
      psi_rx_read();/* next read buffer provided to DIO */
      break;
    case PSI_RX_DEAD_NOT_READY:
    default:
     TRACE_ERROR( "SIG_READ_IND unexpected" );
      break;

   } /* switch */
   psi_mfree_ctrl_ser(ctrl_info);
} /* psi_tx_sig_read_ind() */

