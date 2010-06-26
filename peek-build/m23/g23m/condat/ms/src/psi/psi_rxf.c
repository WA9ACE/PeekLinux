/*
+-----------------------------------------------------------------------------
|  File     : psi_rxf.c
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
|             SDL-documentation (RX-statemachine)
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_RXF_C
#define PSI_RXF_C
#endif /* !PSI_RXF_C */

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
#include "psi_rxf.h"
#include "psi_dtxs.h"
#include "psi_kerp.h"
#include "psi_txp.h"   /* to get TX signal definitions */
#include <string.h>    /* delete warnings: to get memcpy */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : psi_rx_init
+------------------------------------------------------------------------------
| Description : The function psi_rx_init() initializes the RX service
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_rx_init (void)
{
  int i;
  
  TRACE_FUNCTION("psi_rx_init()");

  INIT_STATE(PSI_SERVICE_RX, PSI_RX_DEAD_NOT_READY);
  psi_data->rx.psi_buffer_1 = NULL;
  psi_data->rx.psi_buffer_2 = NULL;

  for(i=0; i<PSI_READ_BUF_MAX;i++)
  {
    psi_data->rx.psi_buffer_pending[i] = NULL;/* pending read buffer list */
    psi_data->rx.psi_buffer_desc_pending[i] = NULL;/* pending desc list */
    psi_data->rx.psi_buffer_pend[i] = FALSE; 
  }
  psi_data->rx.psi_buffer_1_desc = NULL;
  psi_data->rx.psi_buffer_2_desc = NULL;
  psi_data->rx.psi_buffer_1_used = FALSE;
  psi_data->rx.psi_buffer_2_used = FALSE;
  psi_data->rx.psi_buffer_1_read = FALSE;
  psi_data->rx.next_send_id = BUFFER_1;
  psi_data->rx.number_segm = PSI_SEG_SER_NUM;
  /* pre-initializing for serial driver */
   for(i=0;i<psi_data->rx.number_segm; i++)
  {
#ifndef GPRS
    psi_data->rx.psi_seg_size[i] = 200;
#else
    psi_data->rx.psi_seg_size[i] = 1024;
#endif
  } 
  memset(&psi_data->rx.psi_control_info_ser,0, sizeof(T_DIO_CTRL_LINES));
 } /* psi_rx_init() */

/*
+------------------------------------------------------------------------------
| Function    :psi_rx_reconf_pkt

+------------------------------------------------------------------------------
| Description : The function psi_rx_reconf_pkt() reconfigures the segm sizes
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_rx_reconf_pkt (void)
{ 

  TRACE_FUNCTION("psi_rx_reconf_pkt()");

  psi_data->rx.number_segm = PSI_SEG_PKT_NUM;
  psi_data->rx.psi_seg_size[1] = PSI_NUM_PID_BYTE; /* buffer for p_id */
  psi_data->rx.psi_seg_size[0] = psi_data->ker.capabilities->mtu_data;
 
} /* psi_rx_reconf_pkt() */


/*
+------------------------------------------------------------------------------
| Function    : psi_check_control_info
+------------------------------------------------------------------------------
| Description : check the control info for escape sequence detection
|               if DIO indicats this -> send PSI_LINE_STAT_IND to ACI
|               check for DTR drop -> DIO_SA Bit
|               if DIO indicats this -> send PSI_LINE_STAT_IND to ACI
|               store information 
| Parameters  : device, control_info currently filled by DIO, 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_check_control_info(U32 device,T_DIO_CTRL_LINES* control_info_new)
{
  BOOL send_flag = FALSE;
  
  TRACE_FUNCTION("psi_check_control_info()");

  {  
    PALLOC(psi_line_state_ind, PSI_LINE_STATE_IND);
    psi_line_state_ind->line_state = 0;
    /*TRACE_EVENT_P2("psi_check_control_info:line state old %u, send %u",psi_data->rx.psi_control_info_ser.state,control_info_new->state);*/

    /* DTR drop only if state PSI_DIO_SA_ON to PSI_DIO_SA_OFF */
    if((control_info_new->state & DIO_SA) AND !(psi_data->rx.psi_control_info_ser.state & DIO_SA))
    {
      TRACE_FUNCTION("psi_check_control_info:DTR drop");
      psi_line_state_ind->line_state = psi_line_state_ind->line_state | LINE_STD_DTR_LDR;
      send_flag = TRUE; 
    }
    
    /* check escape sequence detection bit - if the previous control info does not contain this bit PSI informs ACI */
    if((control_info_new->state & DIO_ESC) AND !(psi_data->rx.psi_control_info_ser.state & DIO_ESC))
    {
      TRACE_FUNCTION("psi_check_control_info:ESC SEQ");
      psi_line_state_ind->line_state = psi_line_state_ind->line_state | LINE_STD_ESCD_IND;
      send_flag = TRUE; 
    }
    
    if(send_flag)
    {
      psi_line_state_ind->devId = device;
      {
        PSEND(hCommMMI, psi_line_state_ind);
      }
      send_flag = FALSE;
    }
    else
    {
      PFREE(psi_line_state_ind);
    }
    /* store newest state of control info in PSI */
    psi_data->rx.psi_control_info_ser.state = control_info_new->state;
    psi_data->rx.psi_control_info_ser.control_type= control_info_new->control_type;
    psi_data->rx.psi_control_info_ser.length= control_info_new->length;
    /*TRACE_EVENT_P1("psi_check_control_info:line state new %u",control_info_new->state);*/
  }
} /* psi_check_control_info() */       
        
/*
+------------------------------------------------------------------------------
| Function    : psi_rx_read
+------------------------------------------------------------------------------
| Description : Gives two read buffer to DIO driver with dio_set_rx_buffer().
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_rx_read (void)
{
  U16 result = DRV_OK;
  T_dio_buffer *buffer_1 = NULL;
  T_dio_buffer *buffer_2 = NULL;
  T_desc2* segm_to_dtx = NULL;
  T_desc2* segm_to_dtx_help = NULL;
  int i;
  
  TRACE_FUNCTION("psi_rx_read()");
  
  /* allocate data buffer segments for the global segment array entries */
  if (!psi_data->rx.psi_buffer_1_used)
  { 
    /* allocate desc list */
    for(i=0;i<psi_data->rx.number_segm;i++)
    {
      segm_to_dtx = NULL;
      MALLOC(segm_to_dtx,(USHORT)((sizeof(T_desc2) - 1) + psi_data->rx.psi_seg_size[i]));
      segm_to_dtx->next = (ULONG)segm_to_dtx_help;
      segm_to_dtx->offset = 0;
      segm_to_dtx->len =  0;
      segm_to_dtx->size =  psi_data->rx.psi_seg_size[i];
      segm_to_dtx_help = segm_to_dtx;
    }
    psi_data->rx.psi_buffer_1_desc = segm_to_dtx;
    /* allocate buffer structure */
    MALLOC(buffer_1, (USHORT)sizeof(T_dio_buffer));
    /* allocate first segment array without segment buffer part */
    MALLOC(buffer_1->ptr_dio_segment,(USHORT)sizeof(T_dio_segment)*psi_data->rx.number_segm);
    segm_to_dtx = psi_data->rx.psi_buffer_1_desc;
    buffer_1->c_dio_segment = psi_data->rx.number_segm;
    buffer_1->length = 0;
    for (i = 0; i < psi_data->rx.number_segm; i++)
    {/* allocate segment buffer parts */
      if(segm_to_dtx NEQ NULL)
      {
        buffer_1->ptr_dio_segment[i].c_data= segm_to_dtx->size;
        buffer_1->ptr_dio_segment[i].ptr_data = &segm_to_dtx->buffer[segm_to_dtx->offset];
        buffer_1->length += segm_to_dtx->size;
        segm_to_dtx = (T_desc2*)segm_to_dtx->next;
      }
    } 
    psi_data->rx.psi_buffer_1 = buffer_1;
    /* provide buffer 1 to DIO: receive data   */
    result = dio_set_rx_buffer(psi_data->device_no,psi_data->rx.psi_buffer_1);
    /* TRACE_ERROR("psi_rx_read:psi_buffer_1 to DIO"); */
    switch(result)
    {
      case DRV_OK:
        psi_data->rx.psi_buffer_1_used = TRUE;
        break;
      default:
        TRACE_ERROR("psi_rx_read:1.!DRV_OK");
        psi_mfree_desc2_chain(psi_data->rx.psi_buffer_1_desc);
        psi_mfree_buffers(psi_data->rx.psi_buffer_1);
        psi_data->rx.psi_buffer_1 = NULL;
        psi_data->rx.psi_buffer_1_desc = NULL;
        return;
    }
  }
  if (!psi_data->rx.psi_buffer_2_used)
  {
    /* allocate desc list */
    segm_to_dtx_help = NULL;
    for(i=0;i<psi_data->rx.number_segm;i++)
    {
      segm_to_dtx = NULL;
      MALLOC(segm_to_dtx,(USHORT)((sizeof(T_desc2) - 1) + psi_data->rx.psi_seg_size[i]));
      segm_to_dtx->next = (ULONG)segm_to_dtx_help;
      segm_to_dtx->offset = 0;
      segm_to_dtx->len =  0;
      segm_to_dtx->size =  psi_data->rx.psi_seg_size[i];
      segm_to_dtx_help = segm_to_dtx;
    }
    psi_data->rx.psi_buffer_2_desc = segm_to_dtx;
    /* allocate buffer structure */
    MALLOC(buffer_2, (USHORT)sizeof(T_dio_buffer));
    /* allocate first segment array without segment buffer part */
    MALLOC(buffer_2->ptr_dio_segment,(USHORT)sizeof(T_dio_segment)*psi_data->rx.number_segm);
    segm_to_dtx = psi_data->rx.psi_buffer_2_desc;
    buffer_2->c_dio_segment = psi_data->rx.number_segm;
    buffer_2->length = 0;
    for (i = 0; i < psi_data->rx.number_segm; i++)
    {/* allocate segment buffer parts */
      if(segm_to_dtx NEQ NULL)
      {
        buffer_2->ptr_dio_segment[i].c_data= segm_to_dtx->size;
        buffer_2->ptr_dio_segment[i].ptr_data = &segm_to_dtx->buffer[segm_to_dtx->offset];
        buffer_2->length += segm_to_dtx->size;
        segm_to_dtx = (T_desc2*)segm_to_dtx->next;
      }
    } 
    psi_data->rx.psi_buffer_2 = buffer_2; 
    /* provide buffer 2 to DIO: receive data   */
    result = dio_set_rx_buffer(psi_data->device_no,psi_data->rx.psi_buffer_2);
    /* TRACE_ERROR("psi_rx_read:psi_buffer_2 to DIO"); */
    switch(result)
    {
      case DRV_OK:
        psi_data->rx.psi_buffer_2_used = TRUE;
        break;
      default:
        TRACE_ERROR("psi_rx_read:2.!DRV_OK");
        psi_mfree_desc2_chain(psi_data->rx.psi_buffer_2_desc);
        psi_mfree_buffers(psi_data->rx.psi_buffer_2);
        psi_data->rx.psi_buffer_2 = NULL;
        psi_data->rx.psi_buffer_2_desc = NULL;
        return;
    }   
  } 
}/* psi_rx_read */

/*
+------------------------------------------------------------------------------
| Function    : psi_rx_send_data_to_dtx
+------------------------------------------------------------------------------
| Description : buffers filled by DIO are sent to DTI2. They are converted in 
|               descriptors - per segment one descriptor
| Parameters  : T_dio_buffer* buffer
|               T_DIO_CTRL* control_info, line states sent by ser drv
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_rx_send_data_to_dtx (T_dio_buffer* buffer,T_DIO_CTRL* control_info)
{
  T_desc2* segm_to_dtx = NULL;
  T_desc2* segm_to_dtx_first = NULL;
  U16 i = 0;
  U16 entire_data_len = 0;
   
  TRACE_FUNCTION ("psi_rx_send_data_to_dtx()");

  switch(GET_STATE(PSI_SERVICE_RX)) 
  {
    case PSI_RX_READY:    
      if(buffer->c_dio_segment NEQ 0)  /* check if all of segments are sent to DTI */
      {
        if(buffer->length NEQ 0)
        {
          entire_data_len = buffer->length;
          if(buffer EQ psi_data->rx.psi_buffer_1)
          {
            segm_to_dtx = psi_data->rx.psi_buffer_1_desc;
          }
          else
          {
            segm_to_dtx = psi_data->rx.psi_buffer_2_desc;
          }
          segm_to_dtx_first = segm_to_dtx;
          for(i=0;i<psi_data->rx.number_segm;i++)
          {
            if(buffer->length <= buffer->ptr_dio_segment[i].c_data)
            {
              segm_to_dtx->len =  buffer->length;
               psi_rx_dtx_data(segm_to_dtx_first, entire_data_len,control_info);
              break;
            }
            else 
            {
              segm_to_dtx->len =  buffer->ptr_dio_segment[i].c_data;
              segm_to_dtx = (T_desc2*)segm_to_dtx->next;
              buffer->length =  buffer->length - buffer->ptr_dio_segment[i].c_data;
            }
          }
        }
        else
        {
          TRACE_ERROR( "psi_rx_send_data_to_dtx: buffer len 0" );
          if(buffer EQ psi_data->rx.psi_buffer_1)
          {
            segm_to_dtx = psi_data->rx.psi_buffer_1_desc;
          }
          else
          {
            segm_to_dtx = psi_data->rx.psi_buffer_2_desc;
          }
          psi_mfree_desc2_chain(segm_to_dtx);/* free descriptor list if no data are delivered */
        }        
      }
      else
      {
        TRACE_ERROR( "psi_rx_send_data_to_dtx: number of seg 0" );
      }
      break; 
    case PSI_RX_BUFFER:
      if(buffer->c_dio_segment NEQ 0)  /* check if all of segments are sent to DTI */
      {
        if(buffer->length NEQ 0)
        {
          entire_data_len = buffer->length;
          segm_to_dtx = psi_data->rx.psi_buffer_desc_pending[0];/* next sent buffer is always in [0] */
          segm_to_dtx_first = segm_to_dtx;
          for(i=0;i<psi_data->rx.number_segm;i++)
          {
            if(buffer->length <= buffer->ptr_dio_segment[i].c_data)
            {
              segm_to_dtx->len =  buffer->length;
               psi_rx_dtx_data(segm_to_dtx_first, entire_data_len,control_info);
              break;
            }
            else 
            {
              segm_to_dtx->len =  buffer->ptr_dio_segment[i].c_data;
              segm_to_dtx = (T_desc2*)segm_to_dtx->next;
              buffer->length =  buffer->length - buffer->ptr_dio_segment[i].c_data;
            }
          }
        }
        else
        {
          TRACE_ERROR( "psi_rx_send_data_to_dtx: buffer len 0" );
          segm_to_dtx = psi_data->rx.psi_buffer_desc_pending[0];/* next sent buffer is always in [0] */
          psi_mfree_desc2_chain(segm_to_dtx);/* free descriptor list if no data are delivered */
        }
      }
      else
      {
        TRACE_ERROR( "psi_rx_send_data_to_dtx: number of seg 0" );
      }
      break;   
    case PSI_RX_NOT_READY:
    case PSI_RX_DEAD_NOT_READY:
    default:
      TRACE_ERROR( "psi_rx_send_data_to_dtx: state unexpected" );
      break;
  }  
} /* psi_rx_send_data_to_dtx() */

/*
+------------------------------------------------------------------------------
| Function    : psi_rx_send_data_to_dtx_pkt
+------------------------------------------------------------------------------
| Description : buffers filled by DIO are sent to DTI2. They are converted in 
|               descriptors - per segment one descriptor
| Parameters  : T_dio_buffer* buffer
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_rx_send_data_to_dtx_pkt (T_dio_buffer* buffer)
{
  T_desc2* segm_to_dtx = NULL;
  T_desc2*  segm_p_id = NULL;
  T_desc2* segm_to_dtx_first = NULL;
  U16 i = 0;
  U16 entire_data_len = 0;
  U8 protocol_id = 0;
   
  TRACE_FUNCTION ("psi_rx_send_data_to_dtx_pkt()");
  /*lint -e415 Likely access of out-of-bounds pointer*/
  switch(GET_STATE(PSI_SERVICE_RX)) 
  {
    case PSI_RX_READY:    
      if(buffer->c_dio_segment NEQ 0)  /* check if all of segments are sent to DTI */
      {
        if(buffer->length NEQ 0)
        {/* segment 0 contains protocol id and is not sent via DTI in data part */
          entire_data_len = buffer->length - PSI_NUM_PID_BYTE;
          if(buffer EQ psi_data->rx.psi_buffer_1)
          {
            segm_to_dtx = psi_data->rx.psi_buffer_1_desc;
          }
          else
          {
            segm_to_dtx = psi_data->rx.psi_buffer_2_desc;
          }
          segm_p_id = segm_to_dtx;
          protocol_id = segm_p_id->buffer[1];
          segm_to_dtx_first = (T_desc2 *)segm_to_dtx->next; /* points to first data segm */
          segm_to_dtx->next = 0L; /* cut protocol id segm from chain */
          segm_to_dtx = segm_to_dtx_first;
          buffer->length = entire_data_len; /* remain len after cut the protocol id segm from chain */
          for(i=1;i<psi_data->rx.number_segm;i++)
          {
            if(buffer->length <= buffer->ptr_dio_segment[i].c_data)
            {
              segm_to_dtx->len =  buffer->length;
              psi_rx_dtx_data_pkt(segm_to_dtx_first,entire_data_len,protocol_id);
              psi_mfree_desc2_chain(segm_p_id); /* free memory of protocol id segm */
              break;
            }
            else 
            {
              segm_to_dtx->len =  buffer->ptr_dio_segment[i].c_data;
              segm_to_dtx = (T_desc2*)segm_to_dtx->next;
              buffer->length =  buffer->length - buffer->ptr_dio_segment[i].c_data;
            }
          }
        }
        else
        {
          TRACE_ERROR( "psi_rx_send_data_to_dtx_pkt: buffer len 0" );
          if(buffer EQ psi_data->rx.psi_buffer_1)
          {
            segm_to_dtx = psi_data->rx.psi_buffer_1_desc;
          }
          else
          {
            segm_to_dtx = psi_data->rx.psi_buffer_2_desc;
          }
          psi_mfree_desc2_chain(segm_to_dtx);/* free descriptor list if no data are delivered */
        }        
      }
      else
      {
        TRACE_ERROR( "psi_rx_send_data_to_dtx_pkt: number of seg 0" );
      }
      break; 
    case PSI_RX_BUFFER:
      if(buffer->c_dio_segment NEQ 0)  /* check if all of segments are sent to DTI */
      {
        if(buffer->length NEQ 0)
        {/* segment 0 contains protocol id and is not sent via DTI in data part */
          entire_data_len = buffer->length - PSI_NUM_PID_BYTE;
          segm_to_dtx = psi_data->rx.psi_buffer_desc_pending[0];/* next sent buffer is always in [0] */
          segm_p_id = segm_to_dtx;
          protocol_id = segm_p_id->buffer[1];
          segm_to_dtx_first = (T_desc2 *)segm_to_dtx->next; /* points to first data segm */
          segm_to_dtx->next = 0L; /* cut protocol id segm from chain */
          segm_to_dtx = segm_to_dtx_first;
          buffer->length = entire_data_len; /* remain len after cut the protocol id segm from chain */
          for(i=1;i<psi_data->rx.number_segm;i++)
          {
            if(buffer->length <= buffer->ptr_dio_segment[i].c_data)
            {
              segm_to_dtx->len =  buffer->length;
              psi_rx_dtx_data_pkt(segm_to_dtx_first,entire_data_len,protocol_id);
              psi_mfree_desc2_chain(segm_p_id);/* free memory of protocol id segm */
              break;
            }
            else 
            {
              segm_to_dtx->len =  buffer->ptr_dio_segment[i].c_data;
              segm_to_dtx = (T_desc2*)segm_to_dtx->next;
              buffer->length =  buffer->length - buffer->ptr_dio_segment[i].c_data;
            }
          }
        }
        else
        {
          TRACE_ERROR( "psi_rx_send_data_to_dtx_pkt: buffer len 0" );
          segm_to_dtx = psi_data->rx.psi_buffer_desc_pending[0];/* next sent buffer is always in [0] */
          psi_mfree_desc2_chain(segm_to_dtx);/* free descriptor list if no data are delivered */
        }
      }
      else
      {
        TRACE_ERROR( "psi_rx_send_data_to_dtx_pkt: number of seg 0" );
      }
      break;   
    case PSI_RX_NOT_READY:
    case PSI_RX_DEAD_NOT_READY:
    default:
      TRACE_ERROR( "psi_rx_send_data_to_dtx_pkt: state unexpected" );
      break;
  }  
  /*lint +e415 Likely access of out-of-bounds pointer*/
} /* psi_rx_send_data_to_dtx_pkt() */


