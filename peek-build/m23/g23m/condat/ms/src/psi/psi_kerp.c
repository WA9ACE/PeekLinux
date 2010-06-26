/*
+-----------------------------------------------------------------------------
|  File    :  psi_kerp.c
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
|  Purpose :  This modul is part of the entity PSI and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (KER-statemachine)
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#define ENTITY_PSI

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "DIO_inline.h" /* to get the function definition of used SAP DIO */
#include "dti.h"
#include "psi.h"       /* to get the global entity definitions */
#include "psi_drxs.h"   /* to get signal definitions */
#include "psi_dtxs.h"   /* to get signal definitions */
#include "psi_kerf.h"  /* to get ker functions */
#include "psi_kerp.h"  /* to get ker functions */
#include "psi_rxs.h"   /* to get  rx signals */
#include "psi_rxf.h"   /* to get  rx signals */
#include "psi_txs.h"   /* to get  tx signals */
#include "psi_txp.h" 

#include <string.h>

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/
/*
+------------------------------------------------------------------------------
| Function    : psi_mfree_rbuf1
+------------------------------------------------------------------------------
| Description : frees read buffer 1
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_mfree_rbuf1 (void)
{
  psi_mfree_desc2_chain(psi_data->rx.psi_buffer_1_desc);
  psi_mfree_buffers(psi_data->rx.psi_buffer_1);
  psi_data->rx.psi_buffer_1 = NULL;
  psi_data->rx.psi_buffer_1_desc = NULL;
  psi_data->rx.psi_buffer_1_used = FALSE;
}
/*
+------------------------------------------------------------------------------
| Function    : psi_mfree_rbuf2
+------------------------------------------------------------------------------
| Description : frees read buffer 2
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_mfree_rbuf2 (void)
{
  psi_mfree_desc2_chain(psi_data->rx.psi_buffer_2_desc);
  psi_mfree_buffers(psi_data->rx.psi_buffer_2);
  psi_data->rx.psi_buffer_2 = NULL;
  psi_data->rx.psi_buffer_2_desc = NULL;
  psi_data->rx.psi_buffer_2_used = FALSE;
}
/*
+------------------------------------------------------------------------------
| Function    : psi_mfree_pend_tbuf
+------------------------------------------------------------------------------
| Description : frees pending write buffer
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_mfree_pend_tbuf (void)
{
    psi_mfree_desc2_chain(psi_data->tx.psi_buffer_desc_pending);
    if(psi_data->tx.psi_buffer_desc_pid_pending NEQ NULL)
    {
      psi_mfree_desc2_chain(psi_data->tx.psi_buffer_desc_pid_pending);
    }
    MFREE(psi_data->tx.psi_buffer_pending->ptr_dio_segment);
    MFREE(psi_data->tx.psi_buffer_pending);
    psi_data->tx.psi_buffer_pending = NULL;
    psi_data->tx.psi_buffer_desc_pending = NULL;
    psi_data->tx.psi_buffer_desc_pid_pending = NULL;
}

/*
+------------------------------------------------------------------------------
| Function    : psi_mfree_dcb_ser
+------------------------------------------------------------------------------
| Description : frees the stored dio control block of serial device.
|
| Parameters  : pointer to dcb
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_mfree_dcb_ser (T_DIO_DCB_SER* dcb_ser)
{  
  TRACE_FUNCTION ("psi_mfree_dcb_ser()");

  MFREE(dcb_ser);

} /* psi_mfree_dcb_ser */

/*
+------------------------------------------------------------------------------
| Function    : psi_mfree_dcb_pkt
+------------------------------------------------------------------------------
| Description : frees the stored dio control block of packet device.
|
| Parameters  : pointer to dcb
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_mfree_dcb_pkt (T_DIO_DCB_PKT* dcb_pkt)
{  
  TRACE_FUNCTION ("psi_mfree_dcb_pkt()");

  MFREE(dcb_pkt);

} /* psi_mfree_dcb_pkt */

/*==== PRIVATE FUNCTIONS ====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : psi_ker_free_all_buffers
+------------------------------------------------------------------------------
| Description : frees all of the segmented read and write buffers.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_free_all_buffers (void)
{
  T_dio_buffer* buffer = NULL;
  int i;
  
  TRACE_FUNCTION ("psi_ker_free_all_buffers()");
  
  for(i=0;i<PSI_READ_BUF_MAX;i++)
  {
    if(psi_data->rx.psi_buffer_1_used OR psi_data->rx.psi_buffer_2_used)
    {
      dio_read(psi_data->device_no, NULL, &buffer);
      /* TRACE_ERROR ("psi_ker_free_all_buffers: get read buf back");*/
      if(psi_data->rx.psi_buffer_1 EQ buffer)
      {
        if(buffer NEQ NULL)
        {
          psi_mfree_rbuf1();
        }
        else
        {
          TRACE_ERROR("psi_ker_free_all_buffers: NULL buffer1");
        }
      }
      else if(psi_data->rx.psi_buffer_2 EQ buffer)
      {
        if(buffer NEQ NULL)
        {
          psi_mfree_rbuf2();
        }
        else
        {
          TRACE_ERROR("psi_ker_free_all_buffers: NULL buffer2");
        }
      }
      else
      {
        TRACE_ERROR("psi_ker_free_all_buffers: no valid buffer");
      }
    }
  }

//wupeng change

  if(  psi_data->rx.psi_buffer_1 != NULL)
  {
     psi_mfree_desc2_chain(psi_data->rx.psi_buffer_1_desc);
     psi_mfree_buffers(psi_data->rx.psi_buffer_1);
     psi_data->rx.psi_buffer_1 = NULL;
     psi_data->rx.psi_buffer_1_desc = NULL;
     psi_data->rx.psi_buffer_1_used = FALSE;
  }

  if(  psi_data->rx.psi_buffer_2 != NULL)
  {
    psi_mfree_desc2_chain(psi_data->rx.psi_buffer_2_desc);
    psi_mfree_buffers(psi_data->rx.psi_buffer_2);
    psi_data->rx.psi_buffer_2 = NULL;
    psi_data->rx.psi_buffer_2_desc = NULL;
    psi_data->rx.psi_buffer_2_used = FALSE;
  }

//wupeng change end 


  if(psi_data->tx.in_driver NEQ 0)
  {
    for(i=0; i<psi_data->tx.in_driver;i++)
    {
      dio_get_tx_buffer(psi_data->device_no, &buffer);
      /* TRACE_ERROR ("psi_ker_free_all_buffers: get  write buffer back");*/
      if(buffer NEQ NULL)
      {
        psi_free_tx_buffer(buffer);
      }
    }
    psi_data->tx.in_driver = 0;
  }
   
  if (psi_data->tx.psi_buffer_pending NEQ NULL) 
  {
    psi_mfree_pend_tbuf();
  }
  if(psi_data->dcb NEQ NULL)
  {
    psi_mfree_dcb_ser(psi_data->dcb);
  }
  if(psi_data->dcb_pkt NEQ NULL)
  {
    psi_mfree_dcb_pkt(psi_data->dcb_pkt);
  }
} /* psi_ker_free_all_buffers */
/*
+------------------------------------------------------------------------------
| Function    : psi_mfree_buffers
+------------------------------------------------------------------------------
| Description : frees the segmented receive buffers after sending to DTI2.
|
| Parameters  : pointer to NULL terminated array of buffers
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_mfree_buffers (T_dio_buffer *buffers)
{  
  TRACE_FUNCTION ("psi_mfree_buffers()");

  MFREE(buffers->ptr_dio_segment);
  MFREE(buffers);/* free buffer */

} /* psi_mfree_buffers */

/*
+------------------------------------------------------------------------------
| Function    : psi_mfree_ctrl_ser
+------------------------------------------------------------------------------
| Description : frees the stored dio control line information of serial device.
|
| Parameters  : pointer to dcb
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_mfree_ctrl_ser (T_DIO_CTRL_LINES* ctrl_ser)
{  
  TRACE_FUNCTION ("psi_mfree_ctrl_ser()");

  MFREE(ctrl_ser);

} /* psi_mfree_ctrl_ser */

/*
+------------------------------------------------------------------------------
| Function    : psi_malloc_dcb_ser
+------------------------------------------------------------------------------
| Description : allocate dio control block of serial device for permanent storing  
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_malloc_dcb_ser(void)
{  
  TRACE_FUNCTION ("psi_malloc_dcb_ser()");

  MALLOC(psi_data->dcb, sizeof(T_DIO_DCB_SER));
  memset(psi_data->dcb,0,sizeof(T_DIO_DCB_SER));

} /* psi_malloc_dcb_ser */

/*
+------------------------------------------------------------------------------
| Function    : psi_malloc_dcb_pkt
+------------------------------------------------------------------------------
| Description : allocate dio control block of packet device for permanent storing  
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_malloc_dcb_pkt(void)
{  
  TRACE_FUNCTION ("psi_malloc_dcb_pkt()");

  MALLOC(psi_data->dcb_pkt, sizeof(T_DIO_DCB_PKT));
  memset(psi_data->dcb_pkt,0,sizeof(T_DIO_DCB_PKT));
} /* psi_malloc_dcb_pkt */

/*
+------------------------------------------------------------------------------
| Function    : psi_malloc_ctrl_ser
+------------------------------------------------------------------------------
| Description : allocate dio control line information block 
|               of serial device for permanent storing  
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_malloc_ctrl_ser(T_DIO_CTRL_LINES **ctrl)
{  
  T_DIO_CTRL_LINES *control_line_buffer = NULL;
  TRACE_FUNCTION ("psi_malloc_ctrl_ser()");

  MALLOC(control_line_buffer, sizeof(T_DIO_CTRL_LINES));
  memset(control_line_buffer,0,sizeof(T_DIO_CTRL_LINES));
  control_line_buffer->control_type = DIO4_CTRL_LINES;
  control_line_buffer->length = sizeof(T_DIO_CTRL_LINES);
  *ctrl = control_line_buffer;
} /* psi_malloc_ctrl_ser */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_conn_rej
+------------------------------------------------------------------------------
| Description : reaction to primitive PSI_CONN_REJ
|
| Parameters  : primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_conn_rej (T_PSI_CONN_REJ *psi_conn_rej)
{
  U16 result = 0;

  TRACE_FUNCTION ("psi_ker_conn_rej()");

  if ( PSI_DEVICE_FOUND EQ psi_ker_search_basic_data_by_device(psi_conn_rej->devId))
  {
    switch(GET_STATE(PSI_SERVICE_KER))
    {
      case PSI_KER_CONNECTING:
        result = dio_close_device(psi_data->device_no);
        if(result NEQ DRV_OK)
        {
          TRACE_ERROR( "psi_ker_conn_rej: close device not successful" );
        }
        SET_STATE(PSI_SERVICE_KER, PSI_KER_DEAD);    
        break;
      case PSI_KER_DEAD:
      case PSI_KER_READY:
      case PSI_KER_MODIFY:
      default:
        TRACE_ERROR( "PSI_CONN_REJ unexpected" );
        break;
    } /* switch */
  }
  else
  {
    TRACE_ERROR("psi_ker_conn_rej(): Device not found!");
  }

  PFREE(psi_conn_rej);

} /* psi_ker_conn_rej */

#ifdef _SIMULATION_
/*
+------------------------------------------------------------------------------
| Function    : psi_ker_setconf_req_test
+------------------------------------------------------------------------------
| Description : reaction to primitive PSI_SETCONF_REQ_TEST
|
| Parameters  : primitive of simulation
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_setconf_req_test (T_PSI_SETCONF_REQ_TEST *psi_setconf_req_test)
{
  U32 dev_kind = 0;
    
  TRACE_FUNCTION ("psi_ker_setconf_req_test()");
  
  if ( PSI_DEVICE_FOUND EQ psi_ker_search_basic_data_by_device(psi_setconf_req_test->devId))
  {
    dev_kind = psi_data->device_no & DIO_TYPE_DAT_MASK;
 
    switch(GET_STATE(PSI_SERVICE_KER))
    {
      case PSI_KER_READY:
        if(!psi_ker_assign_dcb_sim((T_DIO_DCB*)psi_data->dcb, 
                                    dev_kind,
                                    &psi_setconf_req_test->DIO_DCB_UN))
        {/* send to ACI to inform about wrong configuration parameter */
          PALLOC(psi_setconf_cnf, PSI_SETCONF_CNF);
          psi_setconf_cnf->cause = PSICS_INVALID_PARAMS;
          psi_setconf_cnf->devId = psi_data->device_no;
          PSEND(hCommMMI, psi_setconf_cnf);
        }
        else
        {
          SET_STATE(PSI_SERVICE_KER, PSI_KER_MODIFY);
          /* stop receiving data from peer entity */
          if(psi_data->ker.dti_state EQ PSI_KER_DTI_READY)
          {
            dti_stop(psi_data->hDTI, 
                     psi_data->instance,      /* U8 instance */
                     0,                       /* U8 interfac */
                     0);                      /* U8 channel */

            SET_STATE(PSI_SERVICE_DRX, PSI_DRX_NOT_READY);
          }

          if(psi_data->tx.in_driver EQ 0)
          {/* start flush procedure */
            psi_ker_tx_flush();
          }
          else
          {/* wait for sig_write_ind for last buffer */ 
            psi_data->tx.psi_dio_flush = TRUE;            
          }
        }
        break;
      case PSI_KER_MODIFY:
      case PSI_KER_DEAD:
      case PSI_KER_CONNECTING:
      default:
        TRACE_ERROR( "PSI_SETCONF_REQ unexpected" );
        break;

    } /* switch */
  }
  else
  {
    TRACE_ERROR("psi_ker_setconf_req_test(): Device not found!");
  }

  PFREE(psi_setconf_req_test);

} /* PSI_SETCONF_REQ_TEST */


#else
/*
+------------------------------------------------------------------------------
| Function    : psi_ker_setconf_req
+------------------------------------------------------------------------------
| Description : reaction to primitive PSI_SETCONF_REQ
|
| Parameters  : primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_setconf_req (T_PSI_SETCONF_REQ *psi_setconf_req)
{
  U32 dev_kind = 0;
    
  TRACE_FUNCTION ("psi_ker_setconf_req()");
  
  if ( PSI_DEVICE_FOUND EQ psi_ker_search_basic_data_by_device(psi_setconf_req->devId))
  {
    dev_kind = psi_data->device_no & DIO_TYPE_DAT_MASK;
 
    switch(GET_STATE(PSI_SERVICE_KER))
    {
      case PSI_KER_READY:
        if(!psi_ker_assign_dcb((T_DIO_DCB*)psi_data->dcb, 
                           dev_kind,
                           psi_setconf_req->ptr_DIO_DCB))
        {/* send to ACI to inform about wrong configuration parameter */
          PALLOC(psi_setconf_cnf, PSI_SETCONF_CNF);
          psi_setconf_cnf->cause = PSICS_INVALID_PARAMS;
          psi_setconf_cnf->devId = psi_data->device_no;
          PSEND(hCommMMI, psi_setconf_cnf);
        }
        else
        {
          SET_STATE(PSI_SERVICE_KER, PSI_KER_MODIFY);
          /* stop receiving data from peer entity */
          if(psi_data->ker.dti_state EQ PSI_KER_DTI_READY)
          {
            dti_stop(psi_data->hDTI, 
                     psi_data->instance,      /* U8 instance */
                     0,                       /* U8 interfac */
                     0);                      /* U8 channel */

            SET_STATE(PSI_SERVICE_DRX, PSI_DRX_NOT_READY);
          }

          if(psi_data->tx.in_driver EQ 0)
          {/* start flush procedure */
            psi_ker_tx_flush();
          }
          else
          {/* wait for sig_write_ind for last buffer */ 
            psi_data->tx.psi_dio_flush = TRUE;            
          }
        }
        break;
      case PSI_KER_MODIFY:
      case PSI_KER_DEAD:
      case PSI_KER_CONNECTING:
      default:
        TRACE_ERROR( "PSI_SETCONF_REQ unexpected" );
        break;

    } /* switch */
  }
  else
  {
    TRACE_ERROR("psi_ker_setconf_req(): Device not found!");
  }

  PFREE(psi_setconf_req);

} /* PSI_SETCONF_REQ */
#endif /* _SIMULATION_ */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_line_state_req
+------------------------------------------------------------------------------
| Description : reaction to primitive PSI_LINE_STATE_REQ: start flushing if 
|               changed line states like RING ON/OFF or DCD ON/OFF sent by ACI
| Parameters  : primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_line_state_req (T_PSI_LINE_STATE_REQ *psi_line_state_req)
{
  
  TRACE_FUNCTION ("psi_ker_line_state_req()");

  if ( PSI_DEVICE_FOUND EQ psi_ker_search_basic_data_by_device(psi_line_state_req->devId))
  { 
    switch(GET_STATE(PSI_SERVICE_KER))
    {
      case PSI_KER_READY:
        /*TRACE_EVENT_P1("psi_ker_line_state_req: line states %d",psi_line_state_req->line_state);*/
        /* check sent line states */
        psi_ker_assign_ctrl(&psi_data->tx.psi_control_info_ser, psi_line_state_req->line_state);
        if(!psi_data->tx.psi_dio_flush)
        {
          PALLOC(psi_line_state_cnf, PSI_LINE_STATE_CNF);
          psi_line_state_cnf->devId = psi_data->device_no;
          PSEND(hCommMMI, psi_line_state_cnf);
        }
        else
        {/* flushing is necessary */
          /* flag to distinguish line states sent in data and line states sent in prim */ 
          psi_data->tx.flag_line_state_req = TRUE;        
          if(psi_data->tx.in_driver EQ 0)
          {/* start flush procedure immediately, no write buffer under drv control */
            psi_data->tx.psi_dio_flush = FALSE;
            psi_ker_tx_flush();
          }
          /* else: waiting for WRITE_IND until in_driver EQ 0 */
        }
        break;
      case PSI_KER_MODIFY:
      case PSI_KER_DEAD:
      case PSI_KER_CONNECTING:
      default:
        TRACE_ERROR( "PSI_LINE_STATE_REQ unexpected" );
        break;

    } /* switch */
  }
  else
  {
    TRACE_ERROR("psi_ker_line_state_req(): Device not found!");
  }
  PFREE(psi_line_state_req);

} /* PSI_LINE_STATE_REQ */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_conn_res
+------------------------------------------------------------------------------
| Description : reaction to primitive PSI_CONN_RES
|
| Parameters  : primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_conn_res (T_PSI_CONN_RES *psi_conn_res)
{

  TRACE_FUNCTION ("psi_ker_conn_res()");

  if ( PSI_DEVICE_FOUND EQ psi_ker_search_basic_data_by_device(psi_conn_res->devId))
  {
    switch(GET_STATE(PSI_SERVICE_KER))
    {
      case PSI_KER_CONNECTING:
        SET_STATE(PSI_SERVICE_KER, PSI_KER_READY);  
        psi_ker_tx_open();
        if(psi_data->ker.capabilities->device_type EQ DIO_TYPE_PKT)
        {
          psi_rx_reconf_pkt();
        }
        psi_ker_rx_open();
        break;
      case PSI_KER_DEAD:
      case PSI_KER_READY:
      case PSI_KER_MODIFY:
      default:
        TRACE_ERROR( "PSI_CONN_RES unexpected" );
        break;
    } /* switch */
  }
  else
  {
    TRACE_ERROR("psi_ker_conn_res(): Device not found!");
  }

  PFREE(psi_conn_res);

} /* psi_ker_conn_res */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_close_req
+------------------------------------------------------------------------------
| Description : reaction to primitive PSI_CLOSE_REQ
|
| Parameters  : primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_close_req (T_PSI_CLOSE_REQ *psi_close_req)
{
  U16 result = 0;

  TRACE_FUNCTION ("psi_ker_close_req()");
  
  if ( PSI_DEVICE_FOUND EQ psi_ker_search_basic_data_by_device(psi_close_req->devId))
  {
    switch (psi_data->ker.dti_state) 
    {
      case PSI_KER_DTI_DEAD:
         break;     
      default:
        TRACE_ERROR("PSI_CLOSE_REQ unexpected");
    } /* switch */

    switch( GET_STATE(PSI_SERVICE_KER) ) 
    {
      case PSI_KER_CONNECTING:
        psi_ker_free_all_buffers();
        result = dio_close_device(psi_data->device_no);
        if(result EQ DRV_INVALID_PARAMS)
        {
          TRACE_ERROR( "psi_ker_close_req: dio_close_dev DRV_INV ");
        }
        {
          PALLOC(psi_close_cnf, PSI_CLOSE_CNF);
          psi_close_cnf->devId = psi_data->device_no;
          PSEND(hCommMMI, psi_close_cnf);
        }
        SET_STATE(PSI_SERVICE_KER, PSI_KER_DEAD);
        break;
      case PSI_KER_READY:
        psi_ker_free_all_buffers();       
        result = dio_close_device(psi_data->device_no);
        if(result EQ DRV_INVALID_PARAMS)
        {
          TRACE_ERROR( "psi_ker_close_req: dio_close_dev DRV_INV ");
        }
        SET_STATE(PSI_SERVICE_KER, PSI_KER_DEAD);
      
        psi_ker_rx_close();
        psi_ker_tx_close();
        {
          PALLOC(psi_close_cnf, PSI_CLOSE_CNF);
          psi_close_cnf->devId = psi_data->device_no;
          PSEND(hCommMMI, psi_close_cnf);
        }
        break;
      case PSI_KER_MODIFY:
        {
          PALLOC(psi_setconf_cnf, PSI_SETCONF_CNF);
          psi_setconf_cnf->devId = psi_data->device_no;
          psi_setconf_cnf->cause = PSICS_DISCONNECT;
          PSEND(hCommMMI, psi_setconf_cnf);
        }
        psi_ker_free_all_buffers();     
        result = dio_close_device(psi_data->device_no);
        if(result EQ DRV_INVALID_PARAMS)
        {
          TRACE_ERROR( "psi_ker_close_req: dio_close_dev DRV_INV ");
        }
        SET_STATE(PSI_SERVICE_KER, PSI_KER_DEAD);
        psi_ker_rx_close();
        psi_ker_tx_close();
        psi_ker_drx_close();
        psi_ker_dtx_close();
        {
          PALLOC(psi_close_cnf, PSI_CLOSE_CNF);
          psi_close_cnf->devId = psi_data->device_no;
          PSEND(hCommMMI, psi_close_cnf);
        }
        break;
      case PSI_KER_DEAD:
      default:
        TRACE_ERROR( "PSI_CLOSE_REQ unexpected" );
        break;
    } /* switch */
  }
  else
  {
    TRACE_ERROR("psi_ker_close_req(): Device not found!");
  }
  
  PFREE(psi_close_req);

} /* psi_ker_close_req */
/*
+------------------------------------------------------------------------------
| Function    : psi_ker_dti_close_req
+------------------------------------------------------------------------------
| Description : reaction to primitive PSI_DTI_CLOSE_REQ
|
| Parameters  : primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_dti_close_req (T_PSI_DTI_CLOSE_REQ *psi_dti_close_req)
{

  TRACE_FUNCTION ("psi_ker_dti_close_req()");
  
  if ( PSI_DEVICE_FOUND EQ psi_ker_search_basic_data_by_device(psi_dti_close_req->devId))
  {  
    switch(psi_data->ker.dti_state) 
    {
      case PSI_KER_DTI_OPENING:
        dti_close(psi_data->hDTI, /* DTI_HANDLE *hDTI, */
                  psi_data->instance,    /* U8 instance */
                  0,                      /* U8 interface, */
                  0,                      /* U8 channel); */
                  FALSE
                 );
        psi_data->ker.dti_state = PSI_KER_DTI_DEAD;
        {
          PALLOC(psi_dti_close_cnf, PSI_DTI_CLOSE_CNF);
          psi_dti_close_cnf->devId = psi_data->device_no;
          psi_dti_close_cnf->link_id = psi_dti_close_req->link_id;
          PSEND(hCommMMI, psi_dti_close_cnf);
        }
        break;
      case PSI_KER_DTI_READY:
        dti_close(psi_data->hDTI,        /* DTI_HANDLE *hDTI, */
                  psi_data->instance,    /* U8 instance */
                  0,                      /* U8 interface, */
                  0,                      /* U8 channel); */
                  FALSE
                 );
        psi_data->ker.dti_state = PSI_KER_DTI_DEAD;
        psi_ker_drx_close();
        psi_ker_dtx_close();
        {
          PALLOC(psi_dti_close_cnf, PSI_DTI_CLOSE_CNF);
          psi_dti_close_cnf->devId = psi_data->device_no;
          psi_dti_close_cnf->link_id = psi_dti_close_req->link_id;
          PSEND(hCommMMI, psi_dti_close_cnf);
        }
        break;
      case PSI_KER_DTI_DEAD:
      default:
        TRACE_ERROR( "PSI_DTI_CLOSE_REQ unexpected" );
        break;

    } /* switch */
  }
  else
  {
    TRACE_ERROR("psi_ker_dti_close_req(): Device not found!");
  }

  PFREE(psi_dti_close_req);

} /* psi_ker_dti_close_req */


/*
+------------------------------------------------------------------------------
| Function    : psi_ker_dti_open_req
+------------------------------------------------------------------------------
| Description : reaction to primitive PSI_DTI_OPEN_REQ
|
| Parameters  : primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_dti_open_req (T_PSI_DTI_OPEN_REQ *psi_dti_open_req)
{
  
  TRACE_FUNCTION ("psi_ker_dti_open_req()");

  if ( PSI_DEVICE_FOUND EQ psi_ker_search_basic_data_by_device(psi_dti_open_req->devId))
  {

    switch(psi_data->ker.dti_state) 
    {
      case PSI_KER_DTI_DEAD:
        psi_data->link_id = psi_dti_open_req->link_id;  /* store  link_id */
        psi_data->ker.dti_state = PSI_KER_DTI_OPENING;
        dti_open(psi_data->hDTI, 
               psi_data->instance,              /* U8 instance */
               0,                               /* U8 interfac */
               0,                               /* U8 channel */
               1,                               /* U8 queue_size */
               psi_dti_open_req->dti_direction, /* U8 direction */
               DTI_QUEUE_WATERMARK,             /* U8 link options */
               DTI_VERSION_10,                  /* U32 version */
               psi_dti_open_req->peer.name,     /* U8 *neighbor_entity */
               psi_dti_open_req->link_id        /* U32 link_id */
               );
        break;
      case PSI_KER_DTI_OPENING:
      case PSI_KER_DTI_READY:
      default:
        TRACE_ERROR( "PSI_DTI_OPEN_REQ unexpected" );
        break;
    } /* switch */
  }
  else
  {
    TRACE_ERROR("psi_ker_dti_open_req(): Device not found!");
  }

  PFREE(psi_dti_open_req);

} /* psi_ker_dti_open_req */


/*
+------------------------------------------------------------------------------
| Function    : psi_ker_sig_connect_ind
+------------------------------------------------------------------------------
| Description : reaction to internal signal PSI_SIG_CONNECT_IND
|
| Parameters  : device number
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_sig_connect_ind (U32 device)
{
  U16 result = 0;
  T_DIO_CAP* capabilities = NULL;

  /*TRACE_ERROR ("psi_ker_sig_connect_ind()");*/
  /*
   * A new instance of psi must be set up.
   */
  result = psi_ker_new_instance(device);
  if (result EQ PSI_NEW_FULL) 
  {
    TRACE_ERROR("No more instances of PSI available.");
    return;
  } 
  if (result EQ PSI_NEW_USED) 
  {
    TRACE_ERROR("Device number already in use.");
    return;
  }

  switch( GET_STATE(PSI_SERVICE_KER)) 
  {
    case PSI_KER_DEAD:
      /* On connect ind it is expected to get the capabilities */
      /* TRACE_ERROR("psi_ker_sig_connect_ind: dio_get_capabilities");*/
      result = dio_get_capabilities(psi_data->device_no, &capabilities);
      switch(result)
      {
        case DRV_OK:
          break;
        default:
          TRACE_ERROR( "psi_ker_sig_connect_ind: error capability" );
          return;
      }
      /* set device_type delivered in capabilities in device_no */
#ifdef _SIMULATION_
      {
        PALLOC(psi_conn_ind_test, PSI_CONN_IND_TEST);        
        psi_conn_ind_test->devId = psi_data->device_no;
        switch(capabilities->device_type)
        {
          case DIO_TYPE_SER:
            psi_conn_ind_test->ctrl_DIO_CAP_UN = ser_cap;
            memcpy(&psi_conn_ind_test->DIO_CAP_UN.DIO_CAP_SER,(T_DIO_CAP_SER*)capabilities, sizeof(T_DIO_CAP_SER));
            psi_conn_ind_test->psi_data_mode = DRIVER_MODE_AT_DATA;
            break;
          case DIO_TYPE_PKT:
            psi_conn_ind_test->ctrl_DIO_CAP_UN = pkt_cap;
            memcpy(&psi_conn_ind_test->DIO_CAP_UN.DIO_CAP_PKT,(T_DIO_CAP_PKT*)capabilities, sizeof(T_DIO_CAP_PKT));
            if((psi_data->device_no & DIO_DRV_MASK) EQ DIO_DRV_BAT)
            {
              psi_conn_ind_test->psi_data_mode = DRIVER_MODE_AT; /* hardcoded for BAT */
            }
            break;
          case DIO_TYPE_SER_MUX:
            psi_conn_ind_test->ctrl_DIO_CAP_UN = mux_cap;
            memcpy(&psi_conn_ind_test->DIO_CAP_UN.DIO_CAP_SER_MUX,(T_DIO_CAP_SER_MUX*)capabilities, sizeof(T_DIO_CAP_SER_MUX));
            break;
          default:
            TRACE_ERROR( "psi_ker_sig_connect_ind: wrong device type" );
            return;
        }
        psi_data->ker.capabilities = capabilities;
        PSEND(hCommMMI, psi_conn_ind_test);
      }
#else
      {
        PALLOC(psi_conn_ind, PSI_CONN_IND);        
        psi_conn_ind->devId = psi_data->device_no;
        psi_conn_ind->ptr_DIO_CAP = capabilities;
        psi_data->ker.capabilities = capabilities;
        if((psi_data->device_no & DIO_DRV_MASK) NEQ DIO_DRV_BAT)
        {
          psi_conn_ind->psi_data_mode = DRIVER_MODE_AT_DATA; /* hardcoded for all kind of driver except BAT  */
        }
        else
          {
          psi_conn_ind->psi_data_mode = DRIVER_MODE_AT; /* hardcoded for BAT*/
          }
        PSEND(hCommMMI, psi_conn_ind);
      }
#endif /* _SIMULATION_ */
      /* set initializing configuration parameter and store dcb values in psi_data->dcb/dcb_ptr */
      psi_ker_set_init_conf();
      /* first driver configuration */
      switch(psi_data->ker.capabilities->device_type)
      {
        case DIO_TYPE_SER:
          result = dio_set_config(psi_data->device_no,(T_DIO_DCB *)psi_data->dcb);
          break;
        case DIO_TYPE_PKT:
          result = dio_set_config(psi_data->device_no,(T_DIO_DCB *)psi_data->dcb_pkt);
          break;
        default:
          TRACE_ERROR( "psi_ker_sig_connect_ind: wrong device type conf" );
          break;
      }
      switch(result)
      {
        case DRV_OK:
          SET_STATE(PSI_SERVICE_KER, PSI_KER_CONNECTING);           
          break;
        default:
          TRACE_ERROR( "psi_ker_sig_connect_ind: wrong configuration" );
          return;
      }
      break;
  case PSI_KER_CONNECTING:
  case PSI_KER_READY:
  case PSI_KER_MODIFY:
  default:
    TRACE_ERROR( "PSI_SIG_CONNECT_IND unexpected" );
    break;
  } /* switch */
} /* psi_ker_sig_connect_ind */

/*
+------------------------------------------------------------------------------
| Function    : psi_ker_sig_disconnect_ind
+------------------------------------------------------------------------------
| Description : reaction to internal signal PSI_SIG_DISCONNECT_IND
|
| Parameters  : device number
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_ker_sig_disconnect_ind (U32 device)
{
  U16 result = 0;

  TRACE_FUNCTION ("psi_ker_sig_disconnect_ind()");
  
  if ( PSI_DEVICE_FOUND EQ psi_ker_search_basic_data_by_device(device))
  {
    switch (psi_data->ker.dti_state) 
    {
      case PSI_KER_DTI_OPENING:
      case PSI_KER_DTI_READY:
        dti_close(psi_data->hDTI, 
                  psi_data->instance,    /* U8 instance */
                  0,
                  0, 
                  FALSE); 
        {
          PALLOC(psi_dti_close_ind, PSI_DTI_CLOSE_IND);
          psi_dti_close_ind->devId = psi_data->device_no;
          psi_dti_close_ind->link_id = psi_data->link_id;
          PSEND(hCommMMI, psi_dti_close_ind);
        }
        psi_data->ker.dti_state = PSI_KER_DTI_DEAD;
        psi_data->rx.psi_buffer_1_read = FALSE;
        break;
      case PSI_KER_DTI_DEAD:
      default:
        TRACE_ERROR("PSI_SIG_DISCONNECT_IND unexpected");
    } /* switch */

    switch( GET_STATE(PSI_SERVICE_KER) ) 
    {
      case PSI_KER_CONNECTING:        
        psi_ker_free_all_buffers();
        result = dio_close_device(psi_data->device_no);
        if(result EQ DRV_INVALID_PARAMS)
        {
          TRACE_ERROR("psi_ker_sig_disconnect_ind(): dio_close_device DRV_INV");
        }
        {
          PALLOC(psi_disconn_ind, PSI_DISCONN_IND);
          psi_disconn_ind->devId = psi_data->device_no;
          psi_disconn_ind->cause = PSICS_DISCONNECT;
          PSEND(hCommMMI, psi_disconn_ind);
        }
        SET_STATE(PSI_SERVICE_KER, PSI_KER_DEAD);
        psi_data->device_no = 0; /* delete device number */ 
        psi_data->used = FALSE; /* instance free */
	psi_tx_init() ;
        break;
      case PSI_KER_READY:
        psi_ker_free_all_buffers();       
        result = dio_close_device(psi_data->device_no);
        if(result EQ DRV_INVALID_PARAMS)
        {
          TRACE_ERROR("psi_ker_sig_disconnect_ind(): dio_close_device DRV_INV");
        }
        SET_STATE(PSI_SERVICE_KER, PSI_KER_DEAD);
        psi_ker_rx_close();
        psi_ker_tx_close();
        psi_ker_drx_close();
        psi_ker_dtx_close();
        {
          PALLOC(psi_disconn_ind, PSI_DISCONN_IND);
          psi_disconn_ind->devId = psi_data->device_no;
          psi_disconn_ind->cause = PSICS_DISCONNECT;
          PSEND(hCommMMI, psi_disconn_ind);
        }
        psi_data->device_no = 0; /* delete device number */
        psi_data->used = FALSE; /* instance free */
	psi_tx_init() ;
        break;
      case PSI_KER_MODIFY:
        {
          PALLOC(psi_setconf_cnf, PSI_SETCONF_CNF);
          psi_setconf_cnf->devId = psi_data->device_no;
          psi_setconf_cnf->cause = PSICS_DISCONNECT;
          PSEND(hCommMMI, psi_setconf_cnf);
        }
        psi_ker_free_all_buffers();      
        result = dio_close_device(psi_data->device_no);
        if(result EQ DRV_INVALID_PARAMS)
        {
          TRACE_ERROR("psi_ker_sig_disconnect_ind(): dio_close_device DRV_INV");
        }
        SET_STATE(PSI_SERVICE_KER, PSI_KER_DEAD);
        psi_ker_rx_close();
        psi_ker_tx_close();
        psi_ker_drx_close();
        psi_ker_dtx_close();
        {
          PALLOC(psi_disconn_ind, PSI_DISCONN_IND);
          psi_disconn_ind->devId = psi_data->device_no;
          psi_disconn_ind->cause = PSICS_DISCONNECT;
          PSEND(hCommMMI, psi_disconn_ind);
        }
        psi_data->device_no = 0; /* delete device number */
        psi_data->used = FALSE; /* instance free */
	psi_tx_init() ;
        break;
      case PSI_KER_DEAD:
      default:
        TRACE_ERROR( "PSI_SIG_DISCONNECT_IND unexpected" );
        break;
    } /* switch */
  }
  else
  {
    TRACE_ERROR("psi_ker_sig_disconnect_ind(): Device not found!");
  }

} /* psi_ker_sig_disconnect_ind */



