/*
+-----------------------------------------------------------------------------
|  File     : psi_txf.c
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
|             SDL-documentation (TX-statemachine)
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_TXF_C
#define PSI_TXF_C
#endif /* !PSI_TXF_C */

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
#include <string.h>     /* to get memcpy */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : create_send_buffer
+------------------------------------------------------------------------------
| Description : The function creates segment buffer structure, for tx
|
| Parameters  : user data descr list, protocol_id
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_create_send_buffer(T_dio_buffer** buffer, T_desc_list2 *list, T_desc2** pid_buffer, U8 p_id)
{
  U16 entire_buffer_length = 0;
  T_dio_segment *pArray_of_segments = NULL;
  T_dio_buffer *send_buffer = NULL;
  U8 array_index = 0, number_desc = 0;
  T_desc2 *help_ptr_desc = NULL;
  T_desc2* segm_p_id = NULL;

  TRACE_FUNCTION( "psi_create_send_buffer" );

  if((T_desc2 *)list->first NEQ NULL)
  {
    /* count number of T_desc2 elements */
    help_ptr_desc = (T_desc2 *)list->first;
    number_desc++;
    while((T_desc2 *)help_ptr_desc->next NEQ NULL)
    {
      help_ptr_desc = (T_desc2 *)help_ptr_desc->next;
      number_desc++;
    }
  }
  help_ptr_desc = (T_desc2 *)list->first;
  /* TRACE_EVENT_P1("desc num: %d", number_desc);*/
  /* allocate one data buffer with one segment for each descriptor */
  if ((number_desc NEQ 0) AND (help_ptr_desc NEQ NULL))
  {
    if(psi_data->ker.capabilities->device_type EQ DIO_TYPE_PKT)
    {
      /* add one segm for p_id */
      number_desc++;
      /*create segm and store p_id in first segment */
      MALLOC(segm_p_id,(USHORT)((sizeof(T_desc2) - 1) + PSI_NUM_PID_BYTE));
      segm_p_id->next = 0L;
      segm_p_id->offset = 0;
      segm_p_id->len =  PSI_NUM_PID_BYTE;
      segm_p_id->size =  PSI_NUM_PID_BYTE;
      segm_p_id->buffer[0] = 0;
      segm_p_id->buffer[1] = p_id;/*lint !e415 (Warning --  creation of out-of-bounds pointer)*/
      /* p_id element returned for storing */
      *pid_buffer = segm_p_id;
    }
    /* create send buffer structure */
    MALLOC(send_buffer,sizeof(T_dio_buffer));
    /* create segment array structure */
    MALLOC(pArray_of_segments,sizeof(T_dio_segment)*number_desc);
    send_buffer->ptr_dio_segment = pArray_of_segments;
    send_buffer->c_dio_segment= number_desc;

    if(psi_data->ker.capabilities->device_type EQ DIO_TYPE_PKT)
    {
      /* point from dio buffer to p_id segm */
      if(segm_p_id NEQ NULL)
      {
        pArray_of_segments[array_index].ptr_data = &segm_p_id->buffer[segm_p_id->offset];
        pArray_of_segments[array_index].c_data= segm_p_id->len;
        entire_buffer_length+= segm_p_id->len;
        array_index++;
      }
    }

    /* point to user data */
    while(help_ptr_desc NEQ NULL)
    {
      if(help_ptr_desc->len NEQ 0)
      {
        pArray_of_segments[array_index].ptr_data = &help_ptr_desc->buffer[help_ptr_desc->offset];
        pArray_of_segments[array_index].c_data= help_ptr_desc->len;
        entire_buffer_length+= help_ptr_desc->len;
        array_index++;
      }
      help_ptr_desc = (T_desc2 *)help_ptr_desc->next;
    }
    send_buffer->length = entire_buffer_length;
    *buffer = send_buffer;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : psi_converts_control_info_data
+------------------------------------------------------------------------------
| Description : The function converts the DTI2 control_info for DIO
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_converts_control_info_data(T_parameters *dtx_ctrl_info)
{
  T_DIO_CTRL_LINES* ctrl_lin_ptr = NULL;

  TRACE_FUNCTION( "psi_converts_control_info_data" );

  if((psi_data->device_no & DIO_TYPE_DAT_MASK) EQ DIO_DATA_SER)
  {
    ctrl_lin_ptr = &psi_data->tx.psi_control_info_ser;
    /*TRACE_EVENT_P1("psi_converts_... dio_ctrl old %u",ctrl_lin_ptr->state);*/
    ctrl_lin_ptr->control_type = DIO4_CTRL_LINES;
    ctrl_lin_ptr->length = sizeof(T_DIO_CTRL_LINES);

    if((dtx_ctrl_info->st_lines.st_line_sa EQ DTI_SA_OFF) AND !(ctrl_lin_ptr->state & DIO_SA))
    {
      psi_data->tx.psi_dio_flush = TRUE;
      ctrl_lin_ptr->state = ctrl_lin_ptr->state | DIO_SA;
      TRACE_FUNCTION("psi_converts_.DSR set DIO_SA");
    }
    if((dtx_ctrl_info->st_lines.st_line_sa EQ DTI_SA_ON) AND (ctrl_lin_ptr->state & DIO_SA))
    {
      psi_data->tx.psi_dio_flush = TRUE;
      ctrl_lin_ptr->state = ctrl_lin_ptr->state & ~(DIO_SA);
      TRACE_FUNCTION("psi_converts_..DSR del DIO_SA");
    }
    if((dtx_ctrl_info->st_lines.st_line_sb EQ DTI_SB_OFF) AND !(ctrl_lin_ptr->state & DIO_SB))
    {
      psi_data->tx.psi_dio_flush = TRUE;
      ctrl_lin_ptr->state = ctrl_lin_ptr->state |(DIO_SB);
      TRACE_FUNCTION("psi_converts_... DCD off set DIO_SB");
    }
    if((dtx_ctrl_info->st_lines.st_line_sb EQ DTI_SB_ON) AND (ctrl_lin_ptr->state & DIO_SB))
    {
      psi_data->tx.psi_dio_flush = TRUE;
      ctrl_lin_ptr->state = ctrl_lin_ptr->state & ~ DIO_SB;
      TRACE_FUNCTION("psi_converts_... DCD on del DIO_SB");
    }

    if((dtx_ctrl_info->st_lines.st_flow EQ DTI_FLOW_OFF) AND !(ctrl_lin_ptr->state & DIO_X))
    {
      ctrl_lin_ptr->state = ctrl_lin_ptr->state | DIO_X;
      psi_data->tx.psi_dio_flush = TRUE;
      TRACE_FUNCTION("psi_converts_... outb Flow Control DIO_X on");
    }

    if((dtx_ctrl_info->st_lines.st_flow EQ DTI_FLOW_ON) AND (ctrl_lin_ptr->state & DIO_X))
    {
      ctrl_lin_ptr->state = ctrl_lin_ptr->state & ~(DIO_X);
      psi_data->tx.psi_dio_flush = TRUE;
      TRACE_FUNCTION("psi_converts_... outb Flow Control DIO_X off");
    }

    if(dtx_ctrl_info->st_lines.st_break_len NEQ DTI_BREAK_OFF)
    {/* driver has to send a break signal */
      ctrl_lin_ptr->state = ctrl_lin_ptr->state | DIO_BRK;
      ctrl_lin_ptr->state = ctrl_lin_ptr->state | dtx_ctrl_info->st_lines.st_break_len;
      psi_data->tx.psi_dio_flush = TRUE;
      TRACE_FUNCTION("psi_converts_... break on");
    }
    /*TRACE_EVENT_P1("psi_converts_con... dio_ctrl new %u",ctrl_lin_ptr->state);*/
  }
  else
  {
    TRACE_ERROR( "psi_converts_control_info_data: data type not supported" );
  }
}

/*
+------------------------------------------------------------------------------
| Function    : psi_tx_init
+------------------------------------------------------------------------------
| Description : The function psi_tx_init() initializes the TX service.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_tx_init (void)
{
  int i;

  TRACE_FUNCTION( "psi_tx_init" );

  INIT_STATE(PSI_SERVICE_TX, PSI_TX_DEAD);
  psi_data->tx.in_driver = 0;
  psi_data->tx.psi_buffer_pending = NULL;
  psi_data->tx.psi_buffer_desc_pending = NULL;
  psi_data->tx.psi_buffer_desc_pid_pending = NULL;
  psi_data->tx.psi_buffer_pending_flush = NULL;
  psi_data->tx.psi_buffer_desc_pending_flush = NULL;
  for (i=0; i < PSI_WRITE_BUF_MAX; i++)
  {
    psi_data->tx.psi_buffer[i]      = NULL;
    psi_data->tx.psi_buf_desc[i] = NULL;
    psi_data->tx.psi_buf_pid_desc[i] = NULL;
  }
  psi_data->tx.psi_dio_flush= FALSE;
  psi_data->tx.flag_line_state_req = FALSE;
  memset(&psi_data->tx.psi_control_info_ser,0,sizeof(T_DIO_CTRL_LINES));
} /* psi_tx_init() */

