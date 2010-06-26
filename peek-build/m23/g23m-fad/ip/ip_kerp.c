/*
+----------------------------------------------------------------------------
|  Project :
|  Modul   :
+----------------------------------------------------------------------------
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
+----------------------------------------------------------------------------
|  Purpose :  This Modul defines the functions for processing
|             of incomming primitives for the component
|             Internet Protocol of the mobile station
+----------------------------------------------------------------------------
*/

#define ENTITY_IP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h" /* Get PFREE_DESC2 */
#include "pconst.cdg"
#include "custom.h"
#include "gsm.h"
#include "cnf_ip.h"
#include "mon_ip.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "ccdapi.h"

#include "dti.h" /* To get DTI lib */
#include "ip.h"
#include "ip_udp.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== MACROS ======================================================*/

/*==== Prototypes ==================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : WAP       MODULE  : IP_KERP                              |
| STATE   : code      ROUTINE : ker_ipa_dti_req                      |
+--------------------------------------------------------------------+

  PURPOSE : Process IPA_DTI_REQ primitive
*/
void ker_ipa_dti_req (T_IPA_DTI_REQ * ipa_dti_req)
{
  T_KER * p_ker = & ip_data->ker;
  BOOL confirm = TRUE;
  UBYTE dti_conn = IPA_DISCONNECT_DTI;

  PACCESS (ipa_dti_req);
  TRACE_FUNCTION ("ker_ipa_dti_req()");

  switch (GET_STATE (KER)) {
  case DEACTIVATED:
    switch (ipa_dti_req->dti_conn) {
    case IPA_CONNECT_DTI:
      switch (ipa_dti_req->dti_direction) {
      case IPA_DTI_TO_HIGHER_LAYER:
        if (p_ker->entity_name_hl[0] EQ 0)
        {
          hCommHL = vsi_c_open (
            VSI_CALLER (char *) ipa_dti_req->entity_name
          );
          if (hCommHL > VSI_OK)
          {
            p_ker->link_id_hl = ipa_dti_req->link_id;
            strcpy (
              p_ker->entity_name_hl,
              (char *) ipa_dti_req->entity_name
            );
            SET_STATE (HILA, CONNECTING);
            if (dti_open (
              ip_hDTI,
              IP_DTI_DEF_INSTANCE,
              IP_DTI_HL_INTERFACE,
              IP_DTI_DEF_CHANNEL,
              IP_DTI_UPLINK_QUEUE_SIZE,
              DTI_CHANNEL_TO_HIGHER_LAYER,
              FLOW_CNTRL_ENABLED,
              DTI_VERSION_10,
              (U8 *) ipa_dti_req->entity_name,
              ipa_dti_req->link_id
            ) == TRUE)
            {
              confirm = FALSE;
            }
            else
            { /* dti_open failed, close vsi handle */
              vsi_c_close (VSI_CALLER hCommHL);
              hCommHL = VSI_ERROR;
            }
          }
        }
        else
        {
          TRACE_ERROR ("DTI link to higher layer already requested");
        }
        break;
      case IPA_DTI_TO_LOWER_LAYER:
        if (p_ker->entity_name_ll[0] EQ 0)
        {
          p_ker->link_id_ll = ipa_dti_req->link_id;
          strcpy (
            p_ker->entity_name_ll,
            (char *) ipa_dti_req->entity_name
          );
          SET_STATE (LOLA, CONNECTING);
          if (dti_open (
            ip_hDTI,
            IP_DTI_DEF_INSTANCE,
            IP_DTI_LL_INTERFACE,
            IP_DTI_DEF_CHANNEL,
            IP_DTI_DOWNLINK_QUEUE_SIZE,
            DTI_CHANNEL_TO_LOWER_LAYER,
            FLOW_CNTRL_ENABLED,
            DTI_VERSION_10,
            (U8 *) ipa_dti_req->entity_name,
            ipa_dti_req->link_id
          ) == TRUE)
          {
            confirm = FALSE;
          }
        }
        else
        {
          TRACE_ERROR ("DTI link to lower layer already requested");
        }
        break;
      default:
        TRACE_ERROR ("illegal parameter (ipa_dti_req->dti_direction)");
      } /* End "switch (ipa_dti_req->dti_direction)" */
      dti_conn = IPA_DISCONNECT_DTI;
      break;
    case IPA_DISCONNECT_DTI:
      if (p_ker->link_id_hl == ipa_dti_req->link_id)
      {
        dti_close (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_HL_INTERFACE,
          IP_DTI_DEF_CHANNEL,
          FALSE
        );
        p_ker->link_id_hl = IPA_LINK_ID_DEFAULT;
        p_ker->entity_name_hl[0] = '\0';
        vsi_c_close (VSI_CALLER hCommHL);
        hCommHL = VSI_ERROR;
        SET_STATE (HILA, DOWN);
        dti_conn = IPA_DISCONNECT_DTI;
        /*XXX check for remaining UP/DOWN-LINK DTI connections, XXX*/
        /*XXX terminate_ip() if nothing left open               XXX*/
      }
      else if (p_ker->link_id_ll == ipa_dti_req->link_id)
      {
        dti_close (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_LL_INTERFACE,
          IP_DTI_DEF_CHANNEL,
          FALSE
        );
        p_ker->link_id_ll = IPA_LINK_ID_DEFAULT;
        p_ker->entity_name_ll[0] = '\0';
        SET_STATE (LOLA, DOWN);
        dti_conn = IPA_DISCONNECT_DTI;
        /*XXX check for remaining UP/DOWN-LINK DTI connections, XXX*/
        /*XXX terminate_ip() if nothing left open               XXX*/
      }
      else
      {
        TRACE_ERROR ("illegal parameter (ipa_dti_req->dti_conn)");
        dti_conn = IPA_CONNECT_DTI;
      }
      break;
    } /* End "switch (ipa_dti_req->dti_conn)" */
    break;
  case ACTIVE_NC:
    switch (ipa_dti_req->dti_conn) {
    case IPA_DISCONNECT_DTI:
      if (p_ker->link_id_hl == ipa_dti_req->link_id)
      {
        dti_close (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_HL_INTERFACE,
          IP_DTI_DEF_CHANNEL,
          FALSE
        );
        p_ker->link_id_hl = IPA_LINK_ID_DEFAULT;
        p_ker->entity_name_hl[0] = '\0';
        vsi_c_close (VSI_CALLER hCommHL);
        hCommHL = VSI_ERROR;
        SET_STATE (HILA, DOWN);
        dti_conn = IPA_DISCONNECT_DTI;
        SET_STATE (KER, DEACTIVATED);
      }
      else if (p_ker->link_id_ll == ipa_dti_req->link_id)
      {
        dti_close (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_LL_INTERFACE,
          IP_DTI_DEF_CHANNEL,
          FALSE
        );
        p_ker->link_id_ll = IPA_LINK_ID_DEFAULT;
        p_ker->entity_name_ll[0] = '\0';
        SET_STATE (LOLA, DOWN);
        dti_conn = IPA_DISCONNECT_DTI;
        SET_STATE (KER, DEACTIVATED);
      }
      break;
    default:
      TRACE_ERROR ("unexpected parameter (ipa_dti_req->dti_conn)");
      dti_conn = IPA_DISCONNECT_DTI;
      break;
    } /* End "switch (ipa_dti_req->dti_conn)" */
    break;
  case CONNECTED:
  default:
    TRACE_ERROR ("unexpected IPA_DTI_REQ");
    if (ipa_dti_req->dti_conn == IPA_CONNECT_DTI)
      dti_conn = IPA_DISCONNECT_DTI;
    else
      dti_conn = IPA_CONNECT_DTI;
    break;
  } /* End "switch (GET_STATE (KER))" */

  if (confirm)
  {
    PALLOC (ipa_dti_cnf, IPA_DTI_CNF);
    ipa_dti_cnf->dti_conn = dti_conn;
    ipa_dti_cnf->link_id = ipa_dti_req->link_id;
    PSENDX (MMI, ipa_dti_cnf);
  }
  PFREE (ipa_dti_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP       MODULE  : IP_KERP                              |
| STATE   : code      ROUTINE : sig_dti_ker_connection_opened_hl_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_CONNECTION_OPENED_HL_IND
*/
void sig_dti_ker_connection_opened_hl_ind ()
{
  T_KER * p_ker = & ip_data->ker;

  TRACE_FUNCTION ("sig_dti_ker_connection_opened_hl_ind()");

  if (GET_STATE (HILA) EQ CONNECTING)
  {
    SET_STATE (HILA, IDLE);
    switch (GET_STATE (LOLA)) {
    case IDLE:
    case SEND:
    case WAIT:
      SET_STATE (KER, ACTIVE_NC);
      dti_start (
        ip_hDTI,
        IP_DTI_DEF_INSTANCE,
        IP_DTI_LL_INTERFACE,
        IP_DTI_DEF_CHANNEL
      );
      break;
    default:
      break;
    }
    {
      PALLOC (ipa_dti_cnf, IPA_DTI_CNF);
      ipa_dti_cnf->link_id = p_ker->link_id_hl;
      ipa_dti_cnf->dti_conn = IPA_CONNECT_DTI;
      PSENDX (MMI, ipa_dti_cnf);
    }
  } else {
    TRACE_ERROR ("unexpected dti_connect_ind (from higher layer)");
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP       MODULE  : IP_KERP                              |
| STATE   : code      ROUTINE : sig_dti_ker_connection_opened_ll_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_CONNECTION_OPENED_LL_IND
*/
void sig_dti_ker_connection_opened_ll_ind ()
{
  T_KER * p_ker = & ip_data->ker;

  TRACE_FUNCTION ("sig_dti_ker_connection_opened_ll_ind()");

  if (GET_STATE (LOLA) EQ CONNECTING)
  {
    SET_STATE (LOLA, IDLE);
    switch (GET_STATE (HILA)) {
    case IDLE:
    case SEND:
    case WAIT:
      SET_STATE (KER, ACTIVE_NC);
      dti_start (
        ip_hDTI,
        IP_DTI_DEF_INSTANCE,
        IP_DTI_HL_INTERFACE,
        IP_DTI_DEF_CHANNEL
      );
      break;
    default:
      break;
    }
    {
      PALLOC (ipa_dti_cnf, IPA_DTI_CNF);
      ipa_dti_cnf->link_id = p_ker->link_id_ll;
      ipa_dti_cnf->dti_conn = IPA_CONNECT_DTI;
      PSENDX (MMI, ipa_dti_cnf);
    }
  } else {
    TRACE_ERROR ("unexpected dti_connect_ind (from lower layer)");
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP       MODULE  : IP_KERP                              |
| STATE   : code      ROUTINE : sig_dti_ker_connection_closed_hl_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_CONNECTION_CLOSED_HL_IND
*/
void sig_dti_ker_connection_closed_hl_ind ()
{
  T_KER * p_ker = & ip_data->ker;

  TRACE_FUNCTION ("sig_dti_ker_connection_closed_hl_ind()");

  SET_STATE (HILA, DOWN);

  switch (GET_STATE (KER)) {
  case CONNECTED:
    /*XXX bad case, switch to DEACTIVATED, prune buffers ... XXX*/
    TRACE_ERROR ("unexpected DTI connection close by peer (UPLINK)");
    SET_STATE (KER, DEACTIVATED);
    break;
  case ACTIVE_NC:
    SET_STATE (KER, DEACTIVATED);
    break;
  default:
    break;
  } /* End "switch (GET_STATE (KER))" */

  {
    PALLOC (ipa_dti_ind, IPA_DTI_IND);
    ipa_dti_ind->link_id = p_ker->link_id_hl;
    p_ker->link_id_hl = IPA_LINK_ID_DEFAULT;
    vsi_c_close (VSI_CALLER hCommHL);
    hCommHL = VSI_ERROR;
    p_ker->entity_name_hl[0] = '\0';
    PSENDX (MMI, ipa_dti_ind);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP       MODULE  : IP_KERP                              |
| STATE   : code      ROUTINE : sig_dti_ker_connection_closed_ll_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_CONNECTION_CLOSED_LL_IND
*/
void sig_dti_ker_connection_closed_ll_ind ()
{
  T_KER * p_ker = & ip_data->ker;

  TRACE_FUNCTION ("sig_dti_ker_connection_closed_ll_ind()");

  /* Caution!!! this return is absolut necessary for WAP over GPRS. Otherwise IP will
  close itself after SNDCP went down. Unfortunately not everything is deinitialized and
  IP will never come up on subsequent activation requests (eg. p_ker->entity_name_ll[0]
  is not wiped). Since the Dti-Manager of ACI closes each DTI-Connection itself, we can
  suspend this automatism for now */
  /* return; out-commented at the moment: jk */

  /* If this automatism is re-enabled at least the following lines are necessary
  and were missing in previous versions!!! But at least we should not store an indication
  of an connected peer inside the p_ker->entity_name_ll ... */

  /* p_ker->link_id_ll = IPA_LINK_ID_DEFAULT; out-commented at the moment: jk */
  /* p_ker->entity_name_ll[0] = '\0'; out-commented at the moment: jk */
  /* SET_STATE (LOLA, DOWN); out-commented at the moment: jk */

  switch (GET_STATE (KER)) {
  case CONNECTED:
    /*XXX bad case, switch to DEACTIVATED, prune buffers ... XXX*/
    TRACE_ERROR ("unexpected DTI connection close by peer (DOWNLINK)");
    SET_STATE (KER, DEACTIVATED);
    break;
  case ACTIVE_NC:
    SET_STATE (KER, DEACTIVATED);
    break;
  default:
    break;
  } /* End "switch (GET_STATE (KER))" */

  {
    PALLOC (ipa_dti_ind, IPA_DTI_IND);

    ipa_dti_ind->link_id = p_ker->link_id_ll; /* Initialize the only one parameter of the prim */
    p_ker->link_id_ll = IPA_LINK_ID_DEFAULT; /* Set the state of the entity properly */
    p_ker->entity_name_ll[0] = '\0';
    SET_STATE (LOLA, DOWN);
    SET_STATE (KER, DEACTIVATED);

    PSENDX (MMI, ipa_dti_ind); /* Send indication to the MMI */
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP       MODULE  : IP_KERP                              |
| STATE   : code      ROUTINE : sig_dti_ker_tx_buffer_full_ll_ind    |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_TX_BUFFER_FULL_LL_IND
*/
void sig_dti_ker_tx_buffer_full_ll_ind ()
{
  TRACE_FUNCTION ("sig_dti_ker_tx_buffer_full_ll_ind()");
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP          MODULE  : IP_KERP
| STATE   : code         ROUTINE : sig_dti_ker_tx_buffer_full_hl_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_TX_BUFFER_FULL_HL_IND
*/
void sig_dti_ker_tx_buffer_full_hl_ind ()
{
  TRACE_FUNCTION ("sig_dti_ker_tx_buffer_full_hl_ind()");
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                       MODULE  : IP_KERP              |
| STATE   : code                      ROUTINE : ker_ipa_config_req   |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive IPA_CONFIG_REQ received from ACI
*/
void ker_ipa_config_req (T_IPA_CONFIG_REQ * ipa_config_req)
{
  T_KER * p_ker = & ip_data->ker;
  PALLOC (ipa_config_cnf, IPA_CONFIG_CNF);

  TRACE_FUNCTION ("ker_ipa_config_req()");

  PACCESS (ipa_config_req);

  switch (GET_STATE (KER)) {
  case DEACTIVATED:
    ipa_config_cnf->ack_flag = IPA_CONFIG_NAK;
    ipa_config_cnf->all_down = IPA_ALLDOWN_TRUE;
    break;
  case ACTIVE_NC:
    switch (ipa_config_req->cmd) {
    case IPA_CONFIG_UP:
      p_ker->source_addr= ipa_config_req->ip;
      p_ker->peer_addr = ipa_config_req->peer_ip;
      p_ker->mtu = ipa_config_req->mtu;
      SET_STATE (KER, CONNECTED);
      dti_start (
        ip_hDTI,
        IP_DTI_DEF_INSTANCE,
        IP_DTI_LL_INTERFACE,
        IP_DTI_DEF_CHANNEL
      );
      ipa_config_cnf->ack_flag = IPA_CONFIG_ACK;
      ipa_config_cnf->all_down = IPA_ALLDOWN_FALSE;
      break;
    case IPA_CONFIG_DOWN:
      ipa_config_cnf->ack_flag = IPA_CONFIG_NAK;
      ipa_config_cnf->all_down = IPA_ALLDOWN_TRUE;
      break;
    }
    break;
  case CONNECTED:
    switch (ipa_config_req->cmd) {
    case IPA_CONFIG_UP:
      ipa_config_cnf->ack_flag = IPA_CONFIG_NAK;
      ipa_config_cnf->all_down = IPA_ALLDOWN_FALSE;
      break;
    case IPA_CONFIG_DOWN:
      /*XXX prune buffers + config data XXX*/
      SET_STATE (KER, ACTIVE_NC);
      ipa_config_cnf->ack_flag = IPA_CONFIG_ACK;
      ipa_config_cnf->all_down = IPA_ALLDOWN_TRUE;
      config_down_ll ();
      break;
    }
    break;
  } /* End "switch (GET_STATE (KER))" */

  PSENDX (MMI, ipa_config_cnf);
  PFREE (ipa_config_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                       MODULE  : IP_KERP              |
| STATE   : code                      ROUTINE : ker_ip_addr_req      |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive IP_ADDR_REQ received from higher layer
*/
void ker_ip_addr_req (T_IP_ADDR_REQ * ip_addr_req)
{
  T_KER * p_ker = & ip_data->ker;

  TRACE_FUNCTION ("ker_ip_addr_req()");

  PACCESS (ip_addr_req);

  switch (GET_STATE (KER)) {
  case DEACTIVATED:
    if (GET_STATE (HILA) NEQ DOWN)
    {
      PALLOC (ip_addr_cnf, IP_ADDR_CNF);
      ip_addr_cnf->src_addr = p_ker->source_addr;
      ip_addr_cnf->err = IP_ADDR_NOROUTE;
      ip_addr_cnf->trans_prot = ip_addr_req->trans_prot;
      PSENDX (HL, ip_addr_cnf);
    }
    else
    {
      TRACE_ERROR ("IP_ADDR_REQ received but no higher layer known");
    }
    break;
  case ACTIVE_NC:
    {
      PALLOC (ip_addr_cnf, IP_ADDR_CNF);
      ip_addr_cnf->src_addr = p_ker->source_addr;
      ip_addr_cnf->err = IP_ADDR_NOROUTE;
      ip_addr_cnf->trans_prot = ip_addr_req->trans_prot;
      PSENDX (HL, ip_addr_cnf);
    }
    break;
  case CONNECTED:
    {
      PALLOC (ip_addr_cnf, IP_ADDR_CNF);
      ip_addr_cnf->src_addr = p_ker->source_addr;
      ip_addr_cnf->err = IP_ADDR_NOERROR;
      ip_addr_cnf->trans_prot = ip_addr_req->trans_prot;
      PSENDX (HL, ip_addr_cnf);
    }
    break;
  } /* End "switch (GET_STATE (KER))" */

  PFREE (ip_addr_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP           MODULE  : ip_kerp.c                        |
| STATE   : code          ROUTINE : sig_dti_ker_data_received_hl_ind |
+--------------------------------------------------------------------+

    UDP
     |
     V
    IP

  PURPOSE : Process signal SIG_DTI_KER_DATA_RECEIVED_HL_IND
*/
void sig_dti_ker_data_received_hl_ind (T_DTI2_DATA_REQ * dti_data_req)
{
  T_HILA * p_hl = & ip_data->hila;
  T_KER * p_ker = & ip_data->ker;
  BOOL dest_addresses[MAX_ADDR_TYPES];
  BOOL src_addresses[MAX_ADDR_TYPES];
  BOOL send_ready = FALSE;

  TRACE_FUNCTION ("sig_dti_ker_data_received_hl_ind()");

  if (dti_data_req == NULL)
    return;

  PACCESS (dti_data_req);

  switch (GET_STATE (KER)) {
  case DEACTIVATED:
  case ACTIVE_NC: /* Fall through */
    PFREE_DESC2 (dti_data_req);
    break;
  case CONNECTED:
    if (ip_packet_validator (& dti_data_req->desc_list2))
    {
      T_desc_list2 * desc_list;
      T_desc2 * desc;
      UBYTE * ip_header, to_do;
      USHORT packet_len;

      p_hl->dti_data_req = dti_data_req;
      desc_list = & p_hl->dti_data_req->desc_list2;
      desc = (T_desc2 *) desc_list->first;
      ip_header = desc->buffer;
      to_do = B_NORMAL_PACKET;

      dti_stop (
        ip_hDTI,
        IP_DTI_DEF_INSTANCE,
        IP_DTI_HL_INTERFACE,
        IP_DTI_DEF_CHANNEL
      );

      /* Check destination and source address */
      {
        ULONG dest_addr = GET_IP_DEST_ADDR (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
        check_ip_address (
          dest_addresses,
          src_addresses,
          dest_addr,
          p_ker->source_addr
        );
        if (src_addresses[BAD_UL_SRC_ADDR] OR dest_addresses[BAD_UL_DEST_ADDR])
          p_hl->drop_packet = TRUE;
      }

      /* Check if ICMP message from higher layer and build the ICMP packet */
      if ((GET_IP_PROT (ip_header) EQ ICMP_PROT) AND (! p_hl->drop_packet))/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
        build_ip_packet (TRUE, B_ICMP_PACKET);

      /* Check if fragmenting necessary - build the fragment */
      packet_len = p_hl->dti_data_req->desc_list2.list_len;

      if ((packet_len > p_ker->mtu) AND (! p_hl->drop_packet))
      {
        to_do = B_SEGMENT;
        build_ip_packet (TRUE, to_do);
      }

      /* Build "normal" IP packet */
      if ((to_do EQ B_NORMAL_PACKET) AND (! p_hl->drop_packet))
        build_ip_packet (TRUE, to_do);

      /* Check if ICMP message is to be send */
      if ((p_ker->send_icmp) AND (GET_STATE (HILA) EQ WAIT))
      {
        T_DTI2_DATA_REQ * data_req = p_ker->icmp_dti_data_req;
        data_req->parameters.p_id = DTI_PID_IP;
        data_req->parameters.st_lines.st_flow = DTI_FLOW_ON;
        data_req->parameters.st_lines.st_line_sa = DTI_SA_ON;
        data_req->parameters.st_lines.st_line_sb = DTI_SB_ON;
        data_req->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

        {
          PPASS (data_req, dti_data_ind, DTI2_DATA_IND);
          dti_send_data (
            ip_hDTI,
            IP_DTI_DEF_INSTANCE,
            IP_DTI_HL_INTERFACE,
            IP_DTI_DEF_CHANNEL,
            dti_data_ind
          );
        }
        SET_STATE (HILA, IDLE);
        dti_start (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_HL_INTERFACE,
          IP_DTI_DEF_CHANNEL
        );
      }

      /* Bad packet - drop */
      if (p_hl->drop_packet)
      {
        p_hl->drop_packet = FALSE;
        PFREE_DESC2 (p_hl->dti_data_req);
        send_ready = TRUE;
      }
      else
      {
        if (GET_STATE (HILA) EQ WAIT)
        {
          /* Send dti2_data_req */
          dti_data_req->parameters.p_id = DTI_PID_IP;
          dti_data_req->parameters.st_lines.st_flow = DTI_FLOW_ON;
          dti_data_req->parameters.st_lines.st_line_sa = DTI_SA_ON;
          dti_data_req->parameters.st_lines.st_line_sb = DTI_SB_ON;
          dti_data_req->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

          PACCESS (dti_data_req);
          {
            PPASS (dti_data_req, dti_data_ind, DTI2_DATA_IND);
            dti_send_data (
              ip_hDTI,
              IP_DTI_DEF_INSTANCE,
              IP_DTI_LL_INTERFACE,
              IP_DTI_DEF_CHANNEL,
              dti_data_ind
            );
          }
          p_hl->dti_data_req = NULL;
          if (p_hl->state_segment EQ NO_SEGMENTS)
          {
            SET_STATE (HILA, IDLE);
            send_ready = TRUE;
          }
          else
          {
            /* When the ready_ind is received, we don't know whether
             * the first segment has been sent or not (state is sent
             * in any case), so we don't know whether we have to transmit
             * the first or to build the next one. To avoid the problem
             * we build the next packet here. */
            build_ip_packet (TRUE, B_SEGMENT);
            SET_STATE (HILA, SEND);
            send_ready = FALSE;
          }
        }
        else
        {
          SET_STATE (HILA, SEND);
        }
      }
      /* Send ready indication to higher layer */
      if (send_ready)
      {
        dti_start (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_HL_INTERFACE,
          IP_DTI_DEF_CHANNEL
        );
      }
    }
    else
    {
      PFREE_DESC2 (dti_data_req);
    }
    break;
  } /* End "switch (GET_STATE (KER))" */
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP           MODULE  : ip_kerp.c                        |
| STATE   : code          ROUTINE : sig_dti_ker_data_received_ll_ind |
+--------------------------------------------------------------------+

    IP
     ^
     |
    PPP

  PURPOSE : Process signal SIG_DTI_KER_DATA_RECEIVED_LL_IND
*/
void sig_dti_ker_data_received_ll_ind (T_DTI2_DATA_IND * dti_data_ind)
{
  T_LOLA * p_ll = & ip_data->lola;
  T_KER * p_ker = & ip_data->ker;

  TRACE_FUNCTION ("sig_dti_ker_data_received_ll_ind()");

  if (dti_data_ind == NULL)
    return;

  PACCESS (dti_data_ind);

  switch (GET_STATE (KER)) {
  case DEACTIVATED:
  case ACTIVE_NC: /* Fall through */
    PFREE_DESC2 (dti_data_ind);
    break;
  case CONNECTED:
    if (ip_packet_validator (& dti_data_ind->desc_list2))
    {
      BOOL send_getdata_req;
      T_desc_list2 * desc_list;
      T_desc2 * desc;
      UBYTE * ip_header, to_do, ip_prot;
      USHORT header_len_b;
      USHORT header_chksum, chksum;
      ULONG source_addr, dest_addr;
      BOOL dest_addresses[MAX_ADDR_TYPES];
      BOOL src_addresses[MAX_ADDR_TYPES];

      send_getdata_req = FALSE;
      p_ll->dti_data_ind = dti_data_ind;
      desc_list = & p_ll->dti_data_ind->desc_list2;
      desc = (T_desc2 *) desc_list->first;
      ip_header = desc->buffer;
      to_do = BUILD_NO_PACKET;
      header_len_b = (USHORT) GET_IP_HEADER_LEN_B (ip_header);
      source_addr = p_ker->source_addr;
      ip_prot = GET_IP_PROT (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

      /* Check IP protocol */
      if (GET_IP_VERSION (ip_header) NEQ IP_VERSION)
        p_ll->drop_packet = TRUE;
      else
      {
        UBYTE chk_len_ind;
        USHORT desc_len;

        /* Check the datagram length */
        chk_len_ind = chk_packet_len (ip_header, desc_list);

        if (chk_len_ind NEQ NO_ERROR)
        {
          /* Datagram > length indicated in header - truncate */
          if (chk_len_ind EQ CHANGE_PACKET_LEN)
          {
            desc_len = (USHORT) GET_IP_TOTAL_LEN (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
            truncate_descs (desc_list, desc_len);
          }
          else
            p_ll->drop_packet = TRUE;
        }
      }

      /* Checksum */
      if (! p_ll->drop_packet)
      {
        desc = (T_desc2 *) desc_list->first;
        header_chksum = (USHORT) GET_IP_CHECKSUM (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
        RESET_IP_CHECKSUM (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
        chksum = inet_checksum (ip_header, header_len_b);
        SET_IP_CHECKSUM (ip_header, chksum);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
        if (header_chksum NEQ chksum)
        {
          p_ll->drop_packet = TRUE;
          TRACE_EVENT ("header checksum error indicated") ;
        }
      }

      /* Check destination address and source address */
      if (! p_ll->drop_packet)
      {
        dest_addr = GET_IP_DEST_ADDR (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
        check_ip_address (
          dest_addresses,
          src_addresses,
          dest_addr,
          source_addr
        );
        if (src_addresses[BAD_DL_SRC_ADDR] OR dest_addresses[BAD_DL_DEST_ADDR])
          p_ll->drop_packet = TRUE;

        if (dest_addresses[NO_DEST_ADDR])
        {
          /* Send ICMP message if the IP address is ok and
           * we have not got more than one segment. */
          if (
            (ip_prot EQ ICMP_PROT) OR
            (p_ll->state_reassembly[p_ll->pos_server] EQ READ_SEGMENT) OR
            dest_addresses[LINK_LAYER_BCAST] OR
            dest_addresses[BCAST_ADDR_255] OR
            src_addresses[BCAST_ADDR_255] OR
            src_addresses[MCAST_ADDR] OR
            src_addresses[NETW_ADDR] OR
            src_addresses[LOOP_BACK_ADDR] OR
            src_addresses[CLASS_E_ADDR]
          )
            p_ll->drop_packet = TRUE;
          else
          {
            to_do = B_ICMP_NO_FORWARD;
            p_ker->send_icmp = TRUE;
          }
        }
      }

      /* Check fragmenting */
      if ((! p_ll->drop_packet) AND (to_do NEQ B_ICMP_NO_FORWARD))
      {
        BOOL df_flag, mf_flag;
        USHORT fragm_offset;
        BOOL first_segment, middle_segment, last_segment;

        df_flag = GET_IP_DF_FLAG (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
        mf_flag = GET_IP_MF_FLAG (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

        fragm_offset = (USHORT) GET_IP_FRAG_OFFSET (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
        first_segment =
          (df_flag EQ FLAG_NOT_SET) AND
          (mf_flag EQ FLAG_SET) AND
          (fragm_offset EQ 0);
        middle_segment =
          (df_flag EQ FLAG_NOT_SET) AND
          (mf_flag EQ FLAG_SET) AND
          (fragm_offset > 0);
        last_segment =
          (df_flag EQ FLAG_NOT_SET) AND
          (mf_flag EQ FLAG_NOT_SET) AND
          (fragm_offset > 0);

        if (first_segment OR middle_segment OR last_segment)

          /* Filter out ICMP fragments - not supported */
          if (ip_prot EQ ICMP_PROT)
            p_ll->drop_packet = TRUE;
          else
            /* Start reassemble fragments */
            reassemble_fragments (
              & p_ll->dti_data_ind,
              p_ll,
              ip_header,
              first_segment,
              /* middle_segment, */
              last_segment /*,*/
              /* fragm_offset */
            );
      }

      /* Incoming ICMP message */
      if (! p_ll->drop_packet)
        if (ip_prot EQ ICMP_PROT)
          if (GET_ICMP_TYPE (ip_header, header_len_b) EQ ICMP_TYP_ECHO)
          {
            to_do = B_ICMP_ECHO_REPLY;
            p_ker->send_icmp = TRUE;
          }

      /* Build selected packets */
      if ((! p_ll->drop_packet) AND (to_do NEQ BUILD_NO_PACKET))
        build_ip_packet (FALSE, to_do);

      /* Skip by reading fragments - only send dti_getdata_req */
      if (p_ll->state_reassembly[p_ll->pos_server] EQ READ_SEGMENT)
        send_getdata_req = TRUE;

      /* Drop packet and free resources */
      else if (p_ll->drop_packet)
      {
        p_ll->drop_packet = FALSE;
        PFREE_DESC2 (p_ll->dti_data_ind);
        p_ll->dti_data_ind = NULL;
        send_getdata_req = TRUE;
      }
      else
      {
        /* Send an ICMP message first */
        if (p_ker->send_icmp EQ TRUE)
        {
          if (GET_STATE (HILA) EQ WAIT)
          {
            T_DTI2_DATA_REQ * data_req;
            p_ker->send_icmp = FALSE;

            data_req = p_ker->icmp_dti_data_req;
            data_req->parameters.p_id = DTI_PID_IP;
            data_req->parameters.st_lines.st_flow = DTI_FLOW_ON;
            data_req->parameters.st_lines.st_line_sa = DTI_SA_ON;
            data_req->parameters.st_lines.st_line_sb = DTI_SB_ON;
            data_req->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

            PACCESS (data_req);
            {
              PPASS (data_req, dti_data_indication, DTI2_DATA_IND);
              dti_send_data (
                ip_hDTI,
                IP_DTI_DEF_INSTANCE,
                IP_DTI_LL_INTERFACE,
                IP_DTI_DEF_CHANNEL,
                dti_data_indication
              );
              SET_STATE (HILA, IDLE);
              p_ker->send_icmp = FALSE;
              p_ker->icmp_dti_data_req = NULL;
              send_getdata_req = TRUE; /*XXX not clear to me XXX*/
            }
          }
        }
        else
        {
          /* Check we are not in the middle of a reassembly process */
          if (p_ll->state_reassembly[p_ll->pos_server] EQ NO_SEGMENTS)
          {
            if (GET_STATE (LOLA) EQ WAIT)
            {
              T_DTI2_DATA_IND * data_ind;

              SET_STATE (LOLA, IDLE);

              data_ind = p_ll->dti_data_ind;
              data_ind->parameters.p_id = DTI_PID_IP;
              data_ind->parameters.st_lines.st_flow = DTI_FLOW_ON;
              data_ind->parameters.st_lines.st_line_sa = DTI_SA_ON;
              data_ind->parameters.st_lines.st_line_sb = DTI_SB_ON;
              data_ind->parameters.st_lines.st_break_len = DTI_BREAK_OFF;
              dti_send_data (
                ip_hDTI,
                IP_DTI_DEF_INSTANCE,
                IP_DTI_HL_INTERFACE,
                IP_DTI_DEF_CHANNEL,
                data_ind
              );
              p_ll->dti_data_ind = NULL;

              send_getdata_req = TRUE;
            }
            else
            {
              SET_STATE (LOLA, SEND);
            }
          }
        }
      }

      if (send_getdata_req)
      {
        dti_start (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_LL_INTERFACE,
          IP_DTI_DEF_CHANNEL
        );
      }
    }
    else
    {
      PFREE_DESC2 (dti_data_ind);
    }
    break;
  } /* End "switch (GET_STATE (KER))" */
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP         MODULE  : ip_kerp.c                          |
| STATE   : code        ROUTINE : sig_dti_ker_tx_buffer_ready_ll_ind |
+--------------------------------------------------------------------+

  "DTI2_READY_IND received"
  PURPOSE : Process signal SIG_DTI_KER_TX_BUFFER_READY_LL_IND
*/
void sig_dti_ker_tx_buffer_ready_ll_ind ()
{
  T_HILA * p_hl = & ip_data->hila;
  T_KER * p_ker = & ip_data->ker;
  BOOL send_ready = FALSE;

  TRACE_FUNCTION ("sig_dti_ker_tx_buffer_ready_ll_ind()");

  switch (GET_STATE (KER)) {
  case DEACTIVATED:
  case ACTIVE_NC: /* Fall through */
  SET_STATE (HILA, WAIT);
    break;
  case CONNECTED:
    {
      /* Check if ICMP message */
      if (p_ker->send_icmp)
      {
        T_DTI2_DATA_REQ * dti_data_req;
        p_ker->send_icmp = FALSE;
        dti_data_req = p_ker->icmp_dti_data_req;
        dti_data_req->parameters.p_id = DTI_PID_IP;
        dti_data_req->parameters.st_lines.st_flow = DTI_FLOW_ON;
        dti_data_req->parameters.st_lines.st_line_sa = DTI_SA_ON;
        dti_data_req->parameters.st_lines.st_line_sb = DTI_SB_ON;
        dti_data_req->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

        PACCESS (dti_data_req);
        {
          PPASS (dti_data_req, dti_data_ind, DTI2_DATA_IND);
          dti_send_data (
            ip_hDTI,
            IP_DTI_DEF_INSTANCE,
            IP_DTI_LL_INTERFACE,
            IP_DTI_DEF_CHANNEL,
            dti_data_ind
          );
        }
        dti_start (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_HL_INTERFACE,
          IP_DTI_DEF_CHANNEL
        );
        p_ker->send_icmp = FALSE;
        p_ker->icmp_dti_data_req = NULL;
      }
      else if (GET_STATE (HILA) EQ SEND)
      {
        /* Send waiting data packet */
        p_hl->dti_data_req->parameters.p_id = DTI_PID_IP;
        p_hl->dti_data_req->parameters.st_lines.st_flow = DTI_FLOW_ON;
        p_hl->dti_data_req->parameters.st_lines.st_line_sa = DTI_SA_ON;
        p_hl->dti_data_req->parameters.st_lines.st_line_sb = DTI_SB_ON;
        p_hl->dti_data_req->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

        PACCESS (p_hl->dti_data_req);
        {
          PPASS (p_hl->dti_data_req, dti_data_ind, DTI2_DATA_IND);
          dti_send_data (
            ip_hDTI,
            IP_DTI_DEF_INSTANCE,
            IP_DTI_LL_INTERFACE,
            IP_DTI_DEF_CHANNEL,
            dti_data_ind
          );
        }
        p_hl->dti_data_req = NULL;
        dti_start (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_LL_INTERFACE,
          IP_DTI_DEF_CHANNEL
        );
        p_hl->dti_data_req = NULL;

        /* Check if fragmenting - stay in the same state */
        if (p_hl->state_segment EQ NO_SEGMENTS)
        {
          SET_STATE (HILA, IDLE);
          send_ready = TRUE;
        }
        else
        {
          /* Fragmentation -> build next packet */
          build_ip_packet (TRUE, B_SEGMENT);
          if (p_hl->drop_packet)
          {
            p_hl->drop_packet = FALSE;
            PFREE_DESC2 (p_hl->dti_data_req);
            SET_STATE (HILA, IDLE);
            send_ready = TRUE;
          }
        }
        if (send_ready)
          dti_start (
            ip_hDTI,
            IP_DTI_DEF_INSTANCE,
            IP_DTI_HL_INTERFACE,
            IP_DTI_DEF_CHANNEL
          );
      }
      else
      {
        SET_STATE (HILA, WAIT);
      }
    }
    break;
  } /* End "switch (GET_STATE (KER))" */
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP         MODULE  : ip_kerp.c                          |
| STATE   : code        ROUTINE : sig_dti_ker_tx_buffer_ready_hl_ind |
+--------------------------------------------------------------------+

  "DTI2_GETDATA_REQ received"
  PURPOSE : Process signal SIG_DTI_KER_TX_BUFFER_READY_HL_IND
*/
void sig_dti_ker_tx_buffer_ready_hl_ind ()
{
  T_LOLA * p_ll = & ip_data->lola;

  TRACE_FUNCTION ("sig_dti_ker_tx_buffer_ready_hl_ind()");

  switch (GET_STATE (KER)) {
  case DEACTIVATED:
  case ACTIVE_NC: /* Fall through */
    /*XXX ???? XXX*/
    break;
  case CONNECTED:
    {
      /* state_send - primitive can be sended */
      if (GET_STATE (LOLA) EQ SEND)
      {
        SET_STATE (LOLA, IDLE);
        p_ll->dti_data_ind->parameters.p_id = DTI_PID_IP;
        p_ll->dti_data_ind->parameters.st_lines.st_flow = DTI_FLOW_ON;
        p_ll->dti_data_ind->parameters.st_lines.st_line_sa = DTI_SA_ON;
        p_ll->dti_data_ind->parameters.st_lines.st_line_sb = DTI_SB_ON;
        p_ll->dti_data_ind->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

        dti_send_data (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_HL_INTERFACE,
          IP_DTI_DEF_CHANNEL,
          p_ll->dti_data_ind
        );
        p_ll->dti_data_ind = NULL;

        /* Send DTI2_GETDATA_REQ and indicate ready for new packet */
        dti_start (
          ip_hDTI,
          IP_DTI_DEF_INSTANCE,
          IP_DTI_LL_INTERFACE,
          IP_DTI_DEF_CHANNEL
        );
        SET_STATE (LOLA, IDLE);
      }
      else
      {
        SET_STATE (LOLA, WAIT);
      }
    }
    break;
  } /* End "switch (GET_STATE (KER))" */
}

/*-------------------------------------------------------------------------*/

