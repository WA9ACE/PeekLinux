/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_nuf.c
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
|  Purpose :  This modul is part of the entity SNDCP and implements all
|             procedures and functions as described in the
|             SDL-documentation (NU-statemachine)
+-----------------------------------------------------------------------------
*/

/*---- HISTORY --------------------------------------------------------------*/
#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

#ifndef NCONFIG
#include "sndcp_nuf.h"
#include "sndcp_suas.h"
#include "sndcp_sus.h"
#endif

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

LOCAL void nu_delete (T_NPDU_BUFFER* npdu_buffer);


/*
+------------------------------------------------------------------------------
| Function    : nu_delete
+------------------------------------------------------------------------------
| Description : The function nu_delete() deletes one buffered N-PDU.
|               It is used to delete one N-PDU in form of an SN_DATA_REQ
|               pointer.
|               It will be useful when one N-PDU is deleted in case of
|               LL_DATA_CNF orwhen every N-PDU for one NSAPI is deleted in case
|               of SIG_MG_DELETE_NPDUS.
|               First every descriptor in the SN_DATA_REQ is deleted, then the
|               primitive is deleted, then the T_NPDU_BUFFER* is deleted. (This
|               type is only specified in the code.)
| Parameter   : the one N-PDU to be deleted.
|
+------------------------------------------------------------------------------
*/
LOCAL void nu_delete (T_NPDU_BUFFER* npdu_buffer)
{

  TRACE_FUNCTION( "nu_delete" );

  if (npdu_buffer == NULL) {
    return;
  }
  /*
   * Free the SN_DATA_REQ.
   */
#ifdef _SNDCP_DTI_2_
  if (npdu_buffer->sn_data_req != NULL) {
    MFREE_PRIM(npdu_buffer->sn_data_req);
    npdu_buffer->sn_data_req = NULL;
  }
#else /*_SNDCP_DTI_2_*/
  if (npdu_buffer->sn_data_req != NULL) {
    PFREE_DESC(npdu_buffer->sn_data_req);
    npdu_buffer->sn_data_req = NULL;
  }
#endif /*_SNDCP_DTI_2_*/
  /*
   * If this was the next to send in recovery state, move to next now.
   */
  if (npdu_buffer == sndcp_data->nu->next_resend) {
    sndcp_data->nu->next_resend = npdu_buffer->next;
  }
  if (npdu_buffer == sndcp_data->nu->first_buffered_npdu) {
    sndcp_data->nu->first_buffered_npdu = npdu_buffer->next;
  }
  /*
   * Free the T_NPDU_BUFFER
   */
  MFREE(npdu_buffer);
  npdu_buffer = NULL;
} /* nu_delete() */


/*==== PUBLIC FUNCTIONS =====================================================*/




/*
+------------------------------------------------------------------------------
| Function    : nu_buffer_npdu
+------------------------------------------------------------------------------
| Description : The function nu_buffer_npdu() buffers the given SN_DATA_REQ
|               together with the given N-PDU number.
|
| Parameters  : N-PDU number, SN_DATA_REQ primitive.
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_buffer_npdu (UBYTE npdu_number, T_SN_DATA_REQ* sn_data_req)
{
  T_NPDU_BUFFER* npdu_buffer = 0;

  TRACE_FUNCTION( "nu_buffer_npdu" );

  /*
   * Allocate the T_NPDU_BUFFER struct for
   */
  MALLOC(npdu_buffer, sizeof(T_NPDU_BUFFER));
  /*
   * Insert the given values.
   */
  npdu_buffer->npdu_number = npdu_number;
  npdu_buffer->sn_data_req = sn_data_req;
  npdu_buffer->next = NULL;
  if (sndcp_data->nu->first_buffered_npdu == NULL) {
    sndcp_data->nu->first_buffered_npdu = npdu_buffer;
    sndcp_data->nu->next_resend = npdu_buffer;
  } else {
    T_NPDU_BUFFER* help = sndcp_data->nu->first_buffered_npdu;

    if (help->next == NULL) {
      help->next = npdu_buffer;
    } else {
      while (help->next != NULL) {
        help = help->next;
      }
      help->next = npdu_buffer;
    }

  }


} /* nu_buffer_npdu() */

#ifndef NCONFIG

/*
+------------------------------------------------------------------------------
| Function    : nu_config_discard
+------------------------------------------------------------------------------
| Description : reaction to config prim
|
| Parameters  : nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_config_discard (UBYTE nsapi)
{
  TRACE_FUNCTION( "nu_config_discard" );

  sndcp_data->nu = &sndcp_data->nu_base[nsapi];
  sndcp_data->nu->discard = TRUE;
}

/*
+------------------------------------------------------------------------------
| Function    : nu_config_resume
+------------------------------------------------------------------------------
| Description : reaction to config prim
|
| Parameters  : nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_config_resume (UBYTE nsapi)
{
  TRACE_FUNCTION( "nu_config_resume" );

  sndcp_data->nu = &sndcp_data->nu_base[nsapi];
  sndcp_data->nu->discard = FALSE;
}

/*
+------------------------------------------------------------------------------
| Function    : nu_config_send
+------------------------------------------------------------------------------
| Description : reaction to config prim
|
| Parameters  : nsapi, number of octets
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_config_send (UBYTE nsapi, U32 octets)
{
  U32 *data_count = NULL;

  TRACE_FUNCTION( "nu_config_send" );

  sndcp_data->nu = &sndcp_data->nu_base[nsapi];
  
  data_count = &sndcp_data->data_count[nsapi];

  if (*data_count == 0) {
    *data_count = octets;
  } else {
    *data_count += octets;
    return;
  }

  switch( GET_STATE( NU ) )
  {
    case NU_ACK_SUA_RECEPTIVE:
    case NU_SEND_ACK_SUA_RECEPTIVE:
      SET_STATE(NU, NU_SEND_ACK_SUA_NOT_RECEPTIVE);
      nu_config_send_next(nsapi);
      sndcp_data->nu->sn_ready_ind_pending = FALSE;
      break;
    case NU_ACK_SUA_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SEND_ACK_SUA_NOT_RECEPTIVE);
      break;
    case NU_SEND_ACK_SUA_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SEND_ACK_SUA_NOT_RECEPTIVE);
      *data_count += octets;
      sndcp_data->nu->sn_ready_ind_pending = FALSE;
      break;
    case NU_UNACK_SU_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SEND_UNACK_SU_NOT_RECEPTIVE);
      break;
    case NU_SEND_UNACK_SU_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SEND_UNACK_SU_NOT_RECEPTIVE);
      *data_count += octets;
      sndcp_data->nu->sn_unitready_ind_pending = FALSE;
      break;
    case NU_UNACK_SU_RECEPTIVE:
    case NU_SEND_UNACK_SU_RECEPTIVE:
      SET_STATE(NU, NU_SEND_UNACK_SU_NOT_RECEPTIVE);
      nu_config_send_next(nsapi);
      sndcp_data->nu->sn_unitready_ind_pending = FALSE;
      break;
    case NU_REC_SUA_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SEND_REC_SUA_NOT_RECEPTIVE);
      break;
    case NU_SEND_REC_SUA_NOT_RECEPTIVE:
      break;
    case NU_SUS_SUA_NOT_RECEPTIVE:
      SET_STATE(NU, NU_SEND_SUS_SUA_NOT_RECEPTIVE);
      break;
    case NU_SEND_SUS_SUA_NOT_RECEPTIVE:
      break;
    case NU_SUS_SUA_RECEPTIVE:
      SET_STATE(NU, NU_SEND_SUS_SUA_RECEPTIVE);
      break;
    case NU_SEND_SUS_SUA_RECEPTIVE:
      break;

    default:
      TRACE_ERROR( "config prim SEND unexpected" );
      break;
  }


} /* nu_config_send() */

/*
+------------------------------------------------------------------------------
| Function    : nu_config_send_next
+------------------------------------------------------------------------------
| Description : sends data with 'data_count' octets. State is not changed.
|
| Parameters  : nsapi
| Pre         : data_count > 0
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_config_send_next (UBYTE nsapi)
{
  BOOL ack = FALSE;
  /*
   * Index and number in pattern.
   */
  USHORT i = 0;
  USHORT r = 0;
  UBYTE sapi = 0;
  USHORT k    = 0;
  UBYTE sapi_index = 0;
  USHORT send_size = 0;
  U16 ip_size = 0;
  U32 length;
  U32* data_count = NULL;
#ifdef _SNDCP_DTI_2_
  T_desc2* desc = NULL,*help;
#else /*_SNDCP_DTI_2_*/
  T_desc* desc = NULL;
#endif /*_SNDCP_DTI_2_*/

  T_SN_DATA_REQ* sn_data_req = NULL;
  T_SN_UNITDATA_REQ* sn_unitdata_req = NULL;

  data_count = &sndcp_data->data_count[nsapi];
  length = *data_count;

  TRACE_FUNCTION( "nu_config_send_next" );

  if (length == 0) {
    sndcp_get_nsapi_ack(nsapi, &ack);
    if (ack) {
      SET_STATE(NU, NU_SEND_ACK_SUA_RECEPTIVE);
    } else {
      SET_STATE(NU, NU_SEND_UNACK_SU_RECEPTIVE);
    }
    return;
  }

  sndcp_get_nsapi_sapi(sndcp_data->nu->nsapi, &sapi);
  sndcp_get_sapi_index(sapi, &sapi_index);

  sndcp_data->nu = &sndcp_data->nu_base[nsapi];
  sndcp_get_nsapi_ack(nsapi, &ack);

  if (ack) {
    send_size =
      sndcp_data->sua_base[sapi_index].n201_i - SN_DATA_PDP_HDR_LEN_BIG;
    TRACE_EVENT_P1("send_size: %d", send_size);
  } else {
    send_size =
      sndcp_data->su_base[sapi_index].n201_u - SN_UNITDATA_PDP_HDR_LEN_BIG;
    TRACE_EVENT_P1("send_size: %d", send_size);
  }

  /*
   * The amount of data to be segmented
   * shall be a multiple of N201_I or N201_U respectively
   */
  if(ack)
  {
    ip_size = 2 * sndcp_data->sua_base[sapi_index].n201_i + 100;
    if (length > ip_size)
    {
      if ( sndcp_data->sua_base[sapi_index].n201_i < ip_size)
      {
        k = (ip_size - (ip_size % 
             sndcp_data->sua_base[sapi_index].n201_i))/
             sndcp_data->sua_base[sapi_index].n201_i;
        length = k * (sndcp_data->sua_base[sapi_index].n201_i)
                 -(k-1)- SN_DATA_PDP_HDR_LEN_BIG;
      }
      else
      {
        length = sndcp_data->sua_base[sapi_index].n201_i - 
                 SN_DATA_PDP_HDR_LEN_BIG;
      }
    }
  }
  else
  {
    ip_size = 2 * sndcp_data->su_base[sapi_index].n201_u + 100;
    if (length > ip_size)
    {
      if ( sndcp_data->su_base[sapi_index].n201_u < ip_size)
      {
        k = (ip_size - (ip_size % 
             sndcp_data->su_base[sapi_index].n201_u))/
             sndcp_data->su_base[sapi_index].n201_u;
        length = k * (sndcp_data->su_base[sapi_index].n201_u) 
                 - (k * (SN_UNITDATA_PDP_HDR_LEN_BIG -1) +1);
      }
      else
      {
        length = sndcp_data->su_base[sapi_index].n201_u - 
                 SN_UNITDATA_PDP_HDR_LEN_BIG;
      }
    }
  }

  TRACE_EVENT_P2("data to segment: %d, data to send: %d", 
                  length, *data_count);

  *data_count -= length;

  /*
   * Allocates small segments
   */
  k = (U16)(length / 100); /* Number of 'full' 100 byte descriptors*/
  r = (U16)(length % 100); /* Number of rest bytes */

  if(r > 0){
#ifdef _SNDCP_DTI_2_
    MALLOC (desc, (USHORT)(sizeof(T_desc2) - 1 + r));
#else /*_SNDCP_DTI_2_*/
    MALLOC (desc, (USHORT)(sizeof(T_desc) - 1 + r));
#endif /*_SNDCP_DTI_2_*/

  desc->next  = (ULONG)NULL;
  desc->len   = r;

#ifdef _SNDCP_DTI_2_
  desc->offset = 0;
  desc->size = r;
#endif
  }

  for ( i=0; i < k; i++){
    /*
     * Allocate memory for descriptor
     */
    MALLOC (help, (USHORT)(sizeof(T_desc2) - 1 + 100));




    /*
     * Fill descriptor control information.
     */
    help->next  = (ULONG)desc;
    help->len   = 100;
    help->offset = 0;
    help->size = help->len;

    desc = help;
  } /* for k */



  if (ack) {
#ifdef _SNDCP_DTI_2_    
    MALLOC(sn_data_req, sizeof(T_SN_DATA_REQ));
    sn_data_req->desc_list2.first = (ULONG) desc;
    sn_data_req->desc_list2.list_len = (U16)length;
#else /*_SNDCP_DTI_2_*/
    PALLOC_DESC (sn_data_req, SN_DATA_REQ);
    sn_data_req->desc_list.first = (ULONG) desc;
    sn_data_req->desc_list.list_len = (U16)length;
#endif /*_SNDCP_DTI_2_*/


    sn_data_req->nsapi = nsapi;
#ifdef SNDCP_UPM_INCLUDED 
    sn_data_req->p_id = DTI_PID_IP;
#else
    sn_data_req->p_id = PPP_PID_IP;
#endif /*SNDCP_UPM_INCLUDED*/
    /*
     * Buffer the given N-PDU
     */
    nu_buffer_npdu(sndcp_data->nu->send_npdu_number_ack, sn_data_req);

    sndcp_data->nu->send_npdu_number_ack =
      (U8)((sndcp_data->nu->send_npdu_number_ack + 1) % MAX_NPDU_NUMBER_ACK);
    sig_nu_sua_data_req(sn_data_req,
                       (U8)((sndcp_data->nu->send_npdu_number_ack - 1
                       + MAX_NPDU_NUMBER_ACK) % MAX_NPDU_NUMBER_ACK),
                       sn_data_req->nsapi,
                       sapi);


  } else {
#ifdef _SNDCP_DTI_2_
    MALLOC(sn_unitdata_req, sizeof(T_SN_UNITDATA_REQ));
    sn_unitdata_req->desc_list2.first = (ULONG) desc;
    sn_unitdata_req->desc_list2.list_len = (U16)length;
#else /*_SNDCP_DTI_2_*/
    PALLOC_DESC (sn_unitdata_req, SN_UNITDATA_REQ);
    sn_unitdata_req->desc_list.first = (ULONG) desc;
    sn_unitdata_req->desc_list.list_len = (U16)length;
#endif /*_SNDCP_DTI_2_*/
    sn_unitdata_req->nsapi = nsapi;
#ifdef SNDCP_UPM_INCLUDED 
    sn_unitdata_req->p_id = DTI_PID_IP;
#else
    sn_unitdata_req->p_id = PPP_PID_IP;
#endif /*SNDCP_UPM_INCLUDED*/

    sndcp_data->nu->send_npdu_number_unack =
      (sndcp_data->nu->send_npdu_number_unack + 1) % MAX_NPDU_NUMBER_UNACK;
    sig_nu_su_unitdata_req(sn_unitdata_req,
                           (U16)((sndcp_data->nu->send_npdu_number_unack - 1
                           + MAX_NPDU_NUMBER_UNACK) % MAX_NPDU_NUMBER_UNACK),
                           sndcp_data->nu->nsapi,
                           sapi);

  }

} /* nu_config_send_next() */

#endif



/*
+------------------------------------------------------------------------------
| Function    : nu_delete_npdu
+------------------------------------------------------------------------------
| Description : The function nu_delete_npdu() searches and deletes one
|               buffered N-PDU.
| Parameter   : nsapi, npdu_number.
| Pre         : correct nu instance must be active, NPDU must exist.
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_delete_npdu (UBYTE nsapi, UBYTE npdu_number)
{
  T_NPDU_BUFFER* npdu_buffer = NULL;
  T_NPDU_BUFFER* pre = NULL;
  BOOL found = FALSE;

  TRACE_FUNCTION( "nu_delete_npdu" );

  /*
   * Find the npdu.
   */
  npdu_buffer = sndcp_data->nu->first_buffered_npdu;
  while (!found && npdu_buffer != NULL) {
    if (sndcp_data->nu->nsapi == nsapi &&
        npdu_buffer->npdu_number == npdu_number)
    {
      found = TRUE;
      if (pre != NULL) {
        pre->next = npdu_buffer->next;
      }
    } else {
      pre = npdu_buffer;
      npdu_buffer = npdu_buffer->next;
    }
  }
  /*
   * Delete it.
   */
  if (npdu_buffer != NULL) {
    nu_delete(npdu_buffer);
  }
} /* nu_delete_npdu() */



/*
+------------------------------------------------------------------------------
| Function    : nu_delete_npdus
+------------------------------------------------------------------------------
| Description : The function nu_delete_npdus() deletes all buffered N-PDUs.
|
| Pre         : The correct nu instance has to be "active".
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_delete_npdus (void)
{
  T_NPDU_BUFFER* help = NULL;

  TRACE_FUNCTION( "nu_delete_npdus" );

  while (sndcp_data->nu->first_buffered_npdu != NULL) {
    help = sndcp_data->nu->first_buffered_npdu->next;
    /*
     * Delete the first one.
     */
    nu_delete(sndcp_data->nu->first_buffered_npdu);
    /*
     * Go to the next.
     */
    sndcp_data->nu->first_buffered_npdu = help;
  }
  sndcp_data->nu->next_resend = NULL;

} /* nu_delete_npdus() */


/*
+------------------------------------------------------------------------------
| Function    : nu_init
+------------------------------------------------------------------------------
| Description : The function nu_init() ....
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_init (void)
{

  UBYTE nsapi = 0;

  TRACE_FUNCTION( "nu_init" );

  sndcp_data->nu = & sndcp_data->nu_base[0];
  INIT_STATE(NU_0, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[1];
  INIT_STATE(NU_1, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[2];
  INIT_STATE(NU_2, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[3];
  INIT_STATE(NU_3, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[4];
  INIT_STATE(NU_4, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[5];
  INIT_STATE(NU_5, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[6];
  INIT_STATE(NU_6, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[7];
  INIT_STATE(NU_7, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[8];
  INIT_STATE(NU_8, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[9];
  INIT_STATE(NU_9, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[10];
  INIT_STATE(NU_10, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[11];
  INIT_STATE(NU_11, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[12];
  INIT_STATE(NU_12, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[13];
  INIT_STATE(NU_13, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[14];
  INIT_STATE(NU_14, NU_UNACK_SU_RECEPTIVE);

  sndcp_data->nu = & sndcp_data->nu_base[15];
  INIT_STATE(NU_15, NU_UNACK_SU_RECEPTIVE);

  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    sndcp_data->nu = & sndcp_data->nu_base[nsapi];

    sndcp_data->nu->nsapi = nsapi;
    sndcp_data->nu->send_npdu_number_unack = 0;
    sndcp_data->nu->send_npdu_number_ack = 0;
    sndcp_data->nu->rec_npdu_number_ack = 0;
    sndcp_data->nu->first_buffered_npdu = NULL;
    sndcp_data->nu->next_resend = NULL;
    sndcp_data->nu->sn_ready_ind_pending = FALSE;
    sndcp_data->nu->sn_unitready_ind_pending = FALSE;
    sndcp_data->nu->connection_is_opened = FALSE;
    sndcp_data->nu->discard = FALSE;
    sndcp_data->nu->discarded_data = 0;
    sndcp_data->nu->sent_data = 0;
  }
} /* nu_init() */


/*
+------------------------------------------------------------------------------
| Function    : nu_ready_ind_if_nec
+------------------------------------------------------------------------------
| Description : Sends an SN_READY_IND if none is pending and DTI connection is
|               opened.
|
| Parameters  : nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_ready_ind_if_nec (UBYTE nsapi)
{
  BOOL used = FALSE;
  USHORT stat = MG_IDLE;
  UBYTE sapi = 0;

  TRACE_FUNCTION( "nu_ready_ind_if_nec" );
#ifdef SNDCP_TRACE_ALL
  if (sndcp_data->nu->sn_ready_ind_pending) {
    TRACE_EVENT("sn_ready_ind_pending TRUE");
  } else {
    TRACE_EVENT("sn_ready_ind_pending FALSE");
  }
  if (sndcp_data->nu->connection_is_opened) {
    TRACE_EVENT("connection_is_opened TRUE");
  } else {
    TRACE_EVENT("connection_is_opened FALSE");
  }
#endif /* SNDCP_TRACE_ALL */

  /*
   * set service instance according to nsapi parameter
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];

  sndcp_get_nsapi_sapi(nsapi, &sapi);
  sndcp_get_sapi_state(sapi, &stat);
  sndcp_is_nsapi_used(nsapi, &used);

  if (! sndcp_data->nu->sn_ready_ind_pending
      &&
      sndcp_data->nu->connection_is_opened
      &&
      used
     ) {

    sndcp_data->nu->sn_ready_ind_pending = TRUE;

#ifdef SNDCP_UPM_INCLUDED    
    dti_start(sndcp_data->hDTI, 0, SNDCP_INTERFACE_UNACK, nsapi);
#else
    dti_start(sndcp_data->hDTI, 0, SNDCP_INTERFACE_ACK, nsapi);
#endif
  }

} /* nu_ready_ind_if_nec() */

/*
+------------------------------------------------------------------------------
| Function    : nu_unitready_ind_if_nec
+------------------------------------------------------------------------------
| Description : Sends an SN_UNITREADY_IND if none is pending and DTI connection
|               is opened.
|
| Parameters  : nsapi
| Pre         : The correct nu instance has to be "active".
|
+------------------------------------------------------------------------------
*/
GLOBAL void nu_unitready_ind_if_nec (UBYTE nsapi)
{
  BOOL used = FALSE;
  USHORT stat = MG_IDLE;
  UBYTE sapi = 0;

  TRACE_FUNCTION( "nu_unitready_ind_if_nec" );
#ifdef SNDCP_TRACE_ALL
  if (sndcp_data->nu->sn_unitready_ind_pending) {
    TRACE_EVENT("sn_unitready_ind_pending TRUE");
  } else {
    TRACE_EVENT("sn_unitready_ind_pending FALSE");
  }
  if (sndcp_data->nu->connection_is_opened) {
    TRACE_EVENT("connection_is_opened TRUE");
  } else {
    TRACE_EVENT("connection_is_opened FALSE");
  }
#endif

  sndcp_get_nsapi_sapi(nsapi, &sapi);
  sndcp_get_sapi_state(sapi, &stat);
  sndcp_is_nsapi_used(nsapi, &used);

  if (! sndcp_data->nu->sn_unitready_ind_pending
      &&
      sndcp_data->nu->connection_is_opened
      &&
      used
     ) {
#ifdef SNDCP_TRACE_ALL
      TRACE_EVENT("trying to palloc/send SN_UNITREADY_IND");
#endif


    sndcp_data->nu->sn_unitready_ind_pending = TRUE;

    dti_start(sndcp_data->hDTI, 0, SNDCP_INTERFACE_UNACK, nsapi);

  }
} /* nu_unitready_ind_if_nec() */
