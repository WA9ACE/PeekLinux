/* 
+------------------------------------------------------------------------------
|  File:       tcpip_dti.c
+------------------------------------------------------------------------------
|  Copyright 2003 Texas Instruments Berlin, AG 
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
|  Purpose :  GPF-based TCP/IP's glue layer towards DTI.
+----------------------------------------------------------------------------- 
*/ 


#define TCPIP_DTI_C

#define ENTITY_TCPIP

/*==== INCLUDES =============================================================*/

#include <string.h>             /* String functions, e. g. strncpy(). */

#include "typedefs.h"           /* Condat data types */
#include "vsi.h"                /* a lot of macros */
#include "custom.h"
#include "gsm.h"                /* a lot of macros */
#include "prim.h"               /* definitions of used SAP and directions */
#include "pei.h"                /* PEI interface */
#include "tools.h"              /* common tools */
#include "dti.h"                /* DTI library definitions. */
#include "tcpip.h"              /* global entity definitions */
#include "macdef.h"             /* PFREE_DESC2() */

/* RNET includes
 */
#include "rv_general.h"
#include "rnet_api.h"
#include "rnet_rt_env.h"
#include "rnet_message.h"
#include "rnet_rt_i.h"


/*==== Local data ===========================================================*/

  


/*==== Local utility functions ==============================================*/



/*==== Specific event handler functions =====================================*/


/** Handle the respective DTI primitive. These functions simply forward the
 * DTI primitive to the appropriate DTI library function, but with the
 * entity's dti_handle as additional argument.
 * 
 * @param dti_*    Pointer to the primitive.
 */

void tcpip_dti_ready_ind(T_DTI2_READY_IND *dti_ready_ind)
{ 
  dti_dti_ready_ind(tcpip_data->dti_handle, dti_ready_ind) ; }

void tcpip_dti_data_req(T_DTI2_DATA_REQ *dti_data_req)
{ 
   dti_dti_data_req(tcpip_data->dti_handle, dti_data_req) ; }

void tcpip_dti_data_ind(T_DTI2_DATA_IND *dti_data_ind)
{ 
  dti_dti_data_ind(tcpip_data->dti_handle, dti_data_ind) ; }

void tcpip_dti_getdata_req(T_DTI2_GETDATA_REQ *dti_getdata_req)
{ 
   dti_dti_getdata_req(tcpip_data->dti_handle, dti_getdata_req) ; }

#if defined(_SIMULATION_)
void tcpip_dti_data_test_ind(T_DTI2_DATA_TEST_IND *dti_data_test_ind)
{ dti_dti_data_test_ind(tcpip_data->dti_handle, dti_data_test_ind) ; }

void tcpip_dti_data_test_req(T_DTI2_DATA_TEST_REQ *dti_data_test_req)
{ dti_dti_data_test_req(tcpip_data->dti_handle, dti_data_test_req) ; }
#endif /* _SIMULATION_ */

void tcpip_dti_connect_req(T_DTI2_CONNECT_REQ *dti_connect_req )
{ 
  dti_dti_connect_req(tcpip_data->dti_handle, dti_connect_req) ; }

void tcpip_dti_disconnect_ind(T_DTI2_DISCONNECT_IND *dti_disconnect_ind)
{ 
dti_dti_disconnect_ind(tcpip_data->dti_handle, dti_disconnect_ind) ; }

void tcpip_dti_connect_cnf(T_DTI2_CONNECT_CNF *dti_connect_cnf)
{
dti_dti_connect_cnf(tcpip_data->dti_handle, dti_connect_cnf) ; }

void tcpip_dti_connect_ind(T_DTI2_CONNECT_IND *dti_connect_ind)
{ 
dti_dti_connect_ind(tcpip_data->dti_handle, dti_connect_ind) ; }

void tcpip_dti_connect_res(T_DTI2_CONNECT_RES *dti_connect_res)
{
dti_dti_connect_res(tcpip_data->dti_handle, dti_connect_res) ; }

void tcpip_dti_disconnect_req(T_DTI2_DISCONNECT_REQ *dti_disconnect_req)
{ 
dti_dti_disconnect_req(tcpip_data->dti_handle, dti_disconnect_req) ; }




/*==== Callback functions ===================================================*/


/** Handle a "DTI connection opened" indication; send a DTI confirmation to
 * the ACI.
 * 
 * @param interfac      To lower layer or to higher layer.
 * @param channel       Number of connection to other layer.
 */
static void tcpip_dti_connection_opened_ind(U8 interfac, U8 channel)
{
  TRACE_FUNCTION("tcpip_dti_connection_opened_ind()") ;
 

  switch (interfac)
  {
    case TCPIP_DTI_TO_LOWER_LAYER:
      dti_start(tcpip_data->dti_handle, 0, interfac, channel) ;
      tcpip_dti_cnf(TCPIP_CONNECT_DTI, tcpip_data->ll[channel].link_id) ;
      break ;
    case TCPIP_DTI_TO_HIGHER_LAYER:
      TRACE_ERROR("dti_connection_opened_ind: DTI to UL not yet implemented") ;
      break ;
    default:
      TRACE_ERROR("DTI connection opened: unknown interface") ;
      break ;
  }
}


/** Handle a "DTI connection closed" indication; send a DTI confirmation to
 * the ACI and mark the connection as dead.
 * 
 * @param interfac      To lower layer or to higher layer.
 * @param channel       Number of connection to other layer.
 */
static void tcpip_dti_connection_closed_ind(U8 interfac, U8 channel)
{
  TRACE_FUNCTION("tcpip_dti_connection_closed_ind()") ;


  switch (interfac)
  {
    case TCPIP_DTI_TO_LOWER_LAYER:
      tcpip_dti_cnf(TCPIP_DISCONNECT_DTI, tcpip_data->ll[channel].link_id) ;
      tcpip_data->ll[channel].link_id = 0 ;
      break ;
    case TCPIP_DTI_TO_HIGHER_LAYER:
      TRACE_ERROR("dti_connection_closed_ind: DTI to UL not yet implemented") ;
      break ;
    default:
      TRACE_ERROR("DTI connection opened: unknown interface") ;
      break ;
  }
}


/** Handle an incoming DTI data indication primitive from the lower layer;
 * relay data to the IP input function.
 * 
 * @param dti_data_ind    Pointer to the primitive.
 */
static void tcpip_ll_dti_data_ind(T_DTI2_DATA_IND *dti_data_ind)
{
  NGbuf *bufp ;
  T_desc2 *ddesc ;               /* DTI2 data descriptor. */
  int n_copied ;                /* Number of bytes already copied. */

  TRACE_FUNCTION("tcpip_ll_dti_data_ind()") ;
  if (dti_data_ind->desc_list2.list_len <= TCPIP_DEFAULT_MTU_SIZE)
  {
    ngBufAlloc(bufp) ;          /* This is macro that assigns to bufp. */
    if (bufp)                   /* If we got a buffer... */
    {
      bufp->buf_datalen = dti_data_ind->desc_list2.list_len ;
      bufp->buf_next = NULL ;
      bufp->buf_iov = NULL ;
      bufp->buf_iovcnt = 0 ;
      bufp->buf_flags = NG_PROTO_IP ;
      bufp->buf_datap = ((NGubyte *) bufp) + ngBufDataOffset;

      /* Copy data to buffer. */
      for (ddesc = (T_desc2 *) dti_data_ind->desc_list2.first, n_copied = 0;
           ddesc NEQ NULL;
           ddesc = (T_desc2 *) ddesc->next)
      {
        memcpy(bufp->buf_datap + n_copied,
               ddesc->buffer + ddesc->offset,
               ddesc->len) ;
        n_copied += ddesc->len ;
      }
      /* Finally, send to IP. */
      ngIfGenInput(&rnet_rt_env_ctrl_blk_p->ifnet_dti.dti_ifnet, bufp, 1) ;
    }
    else
    {
      TRACE_ERROR("ngBufAlloc() failed for incoming from DTI") ;
    }
  }
  else
  {
    /* Packet too big -> drop it. */
    TRACE_ERROR("incoming IP packet > MTU size") ;
  }

  /* TRACE_EVENT("PFREE_DESC2(dti_data_ind) ...") ; */
  PFREE_DESC2(dti_data_ind) ;
  /* TRACE_EVENT("...Done") ; */
}


/** Handle an incoming DTI data indication primitive from the higher layer;
 * relay data to the socket send function.
 * 
 * @param dti_data_ind    Pointer to the primitive.
 */
static void tcpip_hl_dti_data_ind(T_DTI2_DATA_IND *dti_data_ind)
{
  TRACE_FUNCTION("tcpip_hl_dti_data_ind()") ;

  
  /* This will be implemented later for SAT class E over TCP/IP. */
  TRACE_ERROR("DTI_DATA_IND: DTI to higher layer not yet implemented") ;
  PFREE(dti_data_ind) ;
}


/** Act on DTI's indication that the lower-layer send buffer is full.
 * 
 * @param channel    Channel identifier.
 */
static void tcpip_ll_dti_buffer_full_ind(U8 channel)
{
  TRACE_FUNCTION("tcpip_ll_dti_buffer_full_ind()") ;
  
  /* Make a note of this; don't send any data from TCP/IP if this is set. */
  TRACE_EVENT("switch flow control towards lower layer to xoff") ;
  rnet_rt_env_ctrl_blk_p->ifnet_dti.dti_ifnet.if_flags |= NG_IFF_OACTIVE ;
  tcpip_data->ll[channel].flowstat_ul = TCPIP_FLOWCTL_XOFF ;
}


/** Act on DTI's indication that the higher-layer send buffer is full.
 * 
 * @param channel    Channel identifier.
 */
static void tcpip_hl_dti_buffer_full_ind(U8 channel)
{
  TRACE_FUNCTION("tcpip_hl_dti_buffer_full_ind()") ;

  
  /* This will be implemented later for SAT class E over TCP/IP. */
  TRACE_ERROR("DTI buffer full: DTI to higher layer not yet implemented") ;
}


/** Act on DTI's indication that the lower-layer send buffer is ready for
 * sending again.
 * 
 * @param channel    Channel identifier.
 */
static void tcpip_ll_dti_buffer_ready_ind(U8 channel)
{
  TRACE_FUNCTION("tcpip_ll_dti_buffer_ready_ind()") ;

  
  /* Note this; enable fetching data from TCP/IP again. */
  TRACE_EVENT("switch flow control towards lower layer to xon") ;
  rnet_rt_env_ctrl_blk_p->ifnet_dti.dti_ifnet.if_flags &= ~NG_IFF_OACTIVE;
  tcpip_data->ll[channel].flowstat_ul = TCPIP_FLOWCTL_XON ;
}


/** Act on DTI's indication that the higher-layer send buffer is ready for
 * sending again.
 * 
 * @param channel    Channel identifier.
 */
static void tcpip_hl_dti_buffer_ready_ind(U8 channel)
{
  TRACE_FUNCTION("tcpip_hl_dti_buffer_ready_ind()") ;

  
  /* This will be implemented later for SAT class E over TCP/IP. */
  TRACE_ERROR("DTI buffer ready: DTI to higher layer not yet implemented") ;
}


/** Central DTI callback dispatcher function according to dti.h.
 *
 * @param instance        Instance number (unused in TCPIP).
 * @param interfac        Interface number (higher or lower layer).
 * @param channel         Channel number (to be used for lower layer in the
 *                        future).
 * @param reason          Code for the event to be handled.
 * @param dti_data_ind    Data primitive if present.
 */
void tcpip_dti_callback(U8 instance, U8 interfac, U8 channel, U8 reason,
                        T_DTI2_DATA_IND *dti_data_ind)
{
  TRACE_FUNCTION("tcpip_dti_callback()") ;


  /* We don't use the instance parameter at all, so it is not passed to the
   * final handler functions. */
  
  switch (reason)
  {
    case DTI_REASON_CONNECTION_OPENED: /* DTI connect */
      tcpip_dti_connection_opened_ind(interfac, channel) ;
      break;
    case DTI_REASON_CONNECTION_CLOSED: /* DTI disconnect */
      tcpip_dti_connection_closed_ind(interfac, channel) ;
      break;
    case DTI_REASON_DATA_RECEIVED:
      ((interfac EQ TCPIP_DTI_TO_LOWER_LAYER)
       ? tcpip_ll_dti_data_ind : tcpip_hl_dti_data_ind)(dti_data_ind);
      break;
    case DTI_REASON_TX_BUFFER_FULL:
      ((interfac EQ TCPIP_DTI_TO_LOWER_LAYER)
       ? tcpip_ll_dti_buffer_full_ind
       : tcpip_hl_dti_buffer_full_ind)(channel) ;
      break;
    case DTI_REASON_TX_BUFFER_READY:
      ((interfac EQ TCPIP_DTI_TO_LOWER_LAYER)
       ? tcpip_ll_dti_buffer_ready_ind
       : tcpip_hl_dti_buffer_ready_ind)(channel) ;
      break;
    default:
      TRACE_ERROR("bogus DTI reason code");
      break;
  } /* end switch */
} /* tcpip_dti_callback() */



/*==== Service functions ====================================================*/


/** Send the data buffer with the specified length to the lower layer. The
 * data buffer will be freed by the caller.
 * 
 * @param data        Pointer to the data.
 * @param length      Length of the data.
 */
void tcpip_dti_send_data_ll(U8 *data, U16 length)
{
  T_desc2 *ddesc ;              /* Pointer to DTI descriptor. */
  
  TRACE_FUNCTION("tcpip_dti_send_data_ll()") ;

  {
    PALLOC(dind, DTI2_DATA_IND) ;
    MALLOC(ddesc, sizeof(T_desc2) - 1 + length) ;

    dind->desc_list2.list_len = length ;
    dind->desc_list2.first = (U32) ddesc ;
    ddesc->next = NULL ;
    ddesc->offset = 0 ;
    ddesc->len = length ;
    ddesc->size = length ;
    memcpy((U8 *) ddesc->buffer, data, length) ;

    dind->parameters.st_lines.st_line_sa   = DTI_SA_ON;
    dind->parameters.st_lines.st_line_sb   = DTI_SB_ON;
    dind->parameters.st_lines.st_flow      = DTI_FLOW_ON;
    dind->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

    dind->link_id         = 0;
    dind->parameters.p_id = DTI_PID_IP;


#if 0 /* Keep these traces in mind, but not in the code right now. */
    /* tlu: content send to SNDCP */
    TRACE_EVENT_P1("!!!!content (tlu) data send to SNDCP (%d bytes)", length);
    for (i=0; i<length; i++)
    {
      TRACE_EVENT_P1("cont: %02X", data[i]);
    }
#endif /* 0 */

    dti_send_data(tcpip_data->dti_handle, 0,
                  TCPIP_DTI_TO_LOWER_LAYER, 0, dind) ;
  }
}



/*==== Primitive sender functions ===========================================*/



/*==== Primitive handler functions ==========================================*/


/* EOF */
