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

#define ENTITY_UDP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pcm.h"
#include "pconst.cdg"
#include "mconst.cdg"
#include "message.h"
#include "ccdapi.h"
#include "vsi.h"
#include "macdef.h" /* To get PFREE_DESC2 */
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "cnf_udp.h"
#include "mon_udp.h"
#include "pei.h"
#include "tok.h"
#include "dti.h" /* To get DTI lib */
#include "udp.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== MACROS ======================================================*/

/*==== Prototypes ==================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                           MODULE  : UDP_KERP         |
| STATE   : code                          ROUTINE : ker_udpa_dti_req |
+--------------------------------------------------------------------+

  PURPOSE : Process UDPA_DTI_REQ primitive
*/

void ker_udpa_dti_req (T_UDPA_DTI_REQ * udpa_dti_req)
{
  BOOL confirm = TRUE;
  UBYTE dti_conn = UDPA_DISCONNECT_DTI;
  PACCESS (udpa_dti_req);

  TRACE_FUNCTION ("ker_udpa_dti_req()");

  switch (GET_STATE (KER))
  {
    case DEACTIVATED:
    {
      switch (udpa_dti_req->dti_conn)
      {
        case UDPA_CONNECT_DTI:
        {
          switch (udpa_dti_req->dti_direction)
          {
            case UDPA_DTI_TO_HIGHER_LAYER:
              if (! * udp_data->hila.entity_name)
              {
                hCommHL = vsi_c_open (
                  VSI_CALLER (char *) udpa_dti_req->entity_name
                );
                if (hCommHL > VSI_OK)
                {
                  udp_data->hila.link_id = udpa_dti_req->link_id;
                  strcpy (
                    udp_data->hila.entity_name,
                    (char *) udpa_dti_req->entity_name
                  );
                  SET_STATE (HILA, CONNECTING);
                  if (dti_open (
                    udp_hDTI,
                    UDP_DTI_DEF_INSTANCE,
                    UDP_DTI_HL_INTERFACE,
                    UDP_DTI_DEF_CHANNEL,
                    UDP_DTI_INIT_QUEUE_SIZE,
                    DTI_CHANNEL_TO_HIGHER_LAYER,
                    FLOW_CNTRL_ENABLED,
                    DTI_VERSION_10,
                    (U8 *) udpa_dti_req->entity_name,
                    udpa_dti_req->link_id
                  ) EQ TRUE)
                  {
                    confirm = FALSE;
                  }
                  else /* dti_open failed, close VSI handle: */
                  {
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

            case UDPA_DTI_TO_LOWER_LAYER:
              if (! * udp_data->lola.entity_name)
              {
                udp_data->lola.link_id = udpa_dti_req->link_id;
                strcpy (
                  udp_data->lola.entity_name,
                  (char *) udpa_dti_req->entity_name
                );
                SET_STATE (LOLA, CONNECTING);
                if (dti_open (
                  udp_hDTI,
                  UDP_DTI_DEF_INSTANCE,
                  UDP_DTI_LL_INTERFACE,
                  UDP_DTI_DEF_CHANNEL,
                  UDP_DTI_INIT_QUEUE_SIZE,
                  DTI_CHANNEL_TO_LOWER_LAYER,
                  FLOW_CNTRL_ENABLED,
                  DTI_VERSION_10,
                  (U8 *) udpa_dti_req->entity_name,
                  udpa_dti_req->link_id
                ) EQ TRUE)
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
              TRACE_ERROR("illegal parameter (udpa_dti_req->dti_direction)");
          } /* End switch (udpa_dti_req->dti_direction) */
          dti_conn = UDPA_DISCONNECT_DTI;
          break;
        }

        case UDPA_DISCONNECT_DTI:
        {
          if (udp_data->hila.link_id EQ udpa_dti_req->link_id)
          {
            dti_close (
              udp_hDTI,
              UDP_DTI_DEF_INSTANCE,
              UDP_DTI_HL_INTERFACE,
              UDP_DTI_DEF_CHANNEL,
              FALSE
            );
            *udp_data->hila.entity_name = 0;
            SET_STATE (HILA, DOWN);
            dti_conn = UDPA_DISCONNECT_DTI;

            vsi_c_close (VSI_CALLER hCommHL);
            hCommHL = VSI_ERROR;
            /*XXX check for remaining UP/DOWN-LINK DTI connections, XXX*/
            /*XXX terminate_ip() if noting left open                XXX*/
          }
          else if (udp_data->lola.link_id EQ udpa_dti_req->link_id)
          {
            dti_close (
              udp_hDTI,
              UDP_DTI_DEF_INSTANCE,
              UDP_DTI_LL_INTERFACE,
              UDP_DTI_DEF_CHANNEL,
              FALSE
            );
            *udp_data->lola.entity_name = 0;
            SET_STATE (LOLA, DOWN);
            dti_conn = UDPA_DISCONNECT_DTI;

            /*XXX check for remaining UP/DOWN-LINK DTI connections, XXX*/
            /*XXX terminate_ip() if noting left open                XXX*/
          }
          else
          {
            TRACE_ERROR ("illegal parameter (udpa_dti_req->dti_conn)");
            dti_conn = UDPA_CONNECT_DTI;
          }
        }
        break;
      } /* End switch (udpa_dti_req->dti_conn) */
      break;
    }

    case ACTIVE_NC:
      switch (udpa_dti_req->dti_conn)
      {
      case UDPA_DISCONNECT_DTI:
        if (udp_data->hila.link_id EQ udpa_dti_req->link_id)
        {
          dti_close (
            udp_hDTI,
            UDP_DTI_DEF_INSTANCE,
            UDP_DTI_HL_INTERFACE,
            UDP_DTI_DEF_CHANNEL,
            FALSE
          );
          *udp_data->hila.entity_name = 0;
          SET_STATE (HILA, DOWN);
          dti_conn = UDPA_DISCONNECT_DTI;
          SET_STATE (KER, DEACTIVATED);

          vsi_c_close (VSI_CALLER hCommHL);
          hCommHL = VSI_ERROR;
        }
        else if (udp_data->lola.link_id EQ udpa_dti_req->link_id)
        {
          dti_close (
            udp_hDTI,
            UDP_DTI_DEF_INSTANCE,
            UDP_DTI_LL_INTERFACE,
            UDP_DTI_DEF_CHANNEL,
            FALSE
          );
          *udp_data->lola.entity_name = 0;
          SET_STATE (LOLA, DOWN);
          dti_conn = UDPA_DISCONNECT_DTI;
          SET_STATE (KER, DEACTIVATED);
        }
        break;
      default:
        TRACE_ERROR ("unexpected parameter (udpa_dti_req->dti_conn)");
        dti_conn = UDPA_DISCONNECT_DTI;
        break;
      } /* End switch (udpa_dti_req->dti_conn) */
      break;

    case CONNECTED:
    default:
      TRACE_ERROR ("unexpected UDPA_DTI_REQ");
      if (udpa_dti_req->dti_conn EQ UDPA_CONNECT_DTI)
        dti_conn = UDPA_DISCONNECT_DTI;
      else
        dti_conn = UDPA_CONNECT_DTI;
      break;
  } /* End switch (GET_STATE (KER)) */

  if (confirm)
  {
    PALLOC (udpa_dti_cnf, UDPA_DTI_CNF);
    udpa_dti_cnf->dti_conn = dti_conn;
    udpa_dti_cnf->link_id = udpa_dti_req->link_id;
    PSENDX (ACI, udpa_dti_cnf);
  }
  PFREE (udpa_dti_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP         MODULE  : UDP_KERP                           |
| STATE   : code        ROUTINE : sig_dti_ker_tx_buffer_ready_ll_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_TX_BUFFER_READY_DN_IND
*/

void sig_dti_ker_tx_buffer_ready_ll_ind ()
{
  TRACE_FUNCTION ("sig_dti_ker_tx_buffer_ready_ll_ind()");

  switch (GET_STATE (KER))
  {
  case ACTIVE_NC: /* DTI_READY_IND not received yet */
    SET_STATE (HILA, WAIT);
    break;

  case CONNECTED:
    {
      T_KER * p_ker = & udp_data->ker;
      T_HILA * p_ul = & udp_data->hila;

      /* Check if an ICMP message shall be sent */

      if (p_ker->send_icmp)
      {
        T_DTI2_DATA_REQ * dti_data_req;

        p_ker->send_icmp = FALSE;

        if(p_ker->dti_data_req_icmp)
        {
          dti_data_req = p_ker->dti_data_req_icmp;
          p_ker->dti_data_req_icmp = NULL;
        }
        else
        {
          TRACE_ERROR("no ICMP message ready!");
          return;
        }

        dti_data_req->parameters.p_id                  = DTI_PID_IP;
        dti_data_req->parameters.st_lines.st_flow      = DTI_FLOW_ON;
        dti_data_req->parameters.st_lines.st_line_sa   = DTI_SA_ON;
        dti_data_req->parameters.st_lines.st_line_sb   = DTI_SB_ON;
        dti_data_req->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

        PACCESS (dti_data_req);
        {
          PPASS (dti_data_req, dti_data_ind, DTI2_DATA_IND);
          dti_send_data (
            udp_hDTI,
            UDP_DTI_DEF_INSTANCE,
            UDP_DTI_LL_INTERFACE,
            UDP_DTI_DEF_CHANNEL,
            dti_data_ind
          );
        }

        /* Call dti_start to indicate ready */

        dti_start (
          udp_hDTI,
          UDP_DTI_DEF_INSTANCE,
          UDP_DTI_HL_INTERFACE,
          UDP_DTI_DEF_CHANNEL
        );
      }
      /* If we have got a ready indication from LL - send dti_data_req */
      else if (GET_STATE (HILA) EQ SEND AND p_ul->dti_data_req)
      {
        p_ul->dti_data_req->parameters.p_id                  = DTI_PID_IP;
        p_ul->dti_data_req->parameters.st_lines.st_flow      = DTI_FLOW_ON;
        p_ul->dti_data_req->parameters.st_lines.st_line_sa   = DTI_SA_ON;
        p_ul->dti_data_req->parameters.st_lines.st_line_sb   = DTI_SB_ON;
        p_ul->dti_data_req->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

        PACCESS (p_ul->dti_data_req);
        {
          PPASS (p_ul->dti_data_req, dti_data_ind, DTI2_DATA_IND);
          dti_send_data (
            udp_hDTI,
            UDP_DTI_DEF_INSTANCE,
            UDP_DTI_LL_INTERFACE,
            UDP_DTI_DEF_CHANNEL,
            dti_data_ind
          );
        }
        p_ul->dti_data_req = NULL;
        dti_start (
          udp_hDTI,
          UDP_DTI_DEF_INSTANCE,
          UDP_DTI_HL_INTERFACE,
          UDP_DTI_DEF_CHANNEL
        );
      }
      else
      {
        SET_STATE (HILA, WAIT);
      }
    }
    break;

  default:
    break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP           MODULE  : UDP_KERP                         |
| STATE   : code          ROUTINE : sig_dti_ker_data_received_hl_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_DATA_RECEIVED_UP_IND
*/

void sig_dti_ker_data_received_hl_ind (T_DTI2_DATA_REQ * dti_data_req)
{
  PACCESS (dti_data_req);

  TRACE_FUNCTION ("sig_dti_ker_data_received_hl_ind()");

  switch (GET_STATE (KER))
  {
  case CONNECTED:
    {
      T_HILA * p_ul = & udp_data->hila;
      T_KER * p_ker = & udp_data->ker;

      dti_stop (
        udp_hDTI,
        UDP_DTI_DEF_INSTANCE,
        UDP_DTI_HL_INTERFACE,
        UDP_DTI_DEF_CHANNEL
      );

#ifdef _SIMULATION_
      /* Get the T_SRC_DES out of the first and only descriptor
       * created by DTILIB and put it in a descriptor of its own */

      if (dti_data_req->desc_list2.first &&
          dti_data_req->desc_list2.list_len >= sizeof(T_SRC_DES))
      {
        // The following is mostly taken from UDPa_sendRequest() from module
        // wap_udpc.c to achieve the same behaviour. (Issue UDP-FIX-21925)
        // The desc from the desc_list2 is splitted into a linked desc list
        // to simulate large data chunks (such large data chunks are spread over
        // in several decs in the list).
        T_desc2    *desc, *first_desc;
        ULONG      *pLast;
        USHORT     len, dataSize, data_length;
        const char *data_p;
        USHORT     BLOCK_SIZE = 50;

        first_desc = (T_desc2 *) dti_data_req->desc_list2.first;
        data_length = dti_data_req->desc_list2.list_len;

        /* Build the T_desc2 for IP addresses and ports */
        desc = M_ALLOC (offsetof (T_desc2, buffer) + sizeof (T_SRC_DES));
        
        // update the desc list
        dti_data_req->desc_list2.first = (ULONG) desc;
        desc->offset = 0;
        desc->len = sizeof (T_SRC_DES);
        desc->size = sizeof (T_SRC_DES);

        // copy IP addresses and ports
        memcpy(desc->buffer, &first_desc->buffer[first_desc->offset],desc->len);
        
        /* Build the desc for the UDP data */
        pLast = &desc->next;
        len = data_length - desc->len;
        data_p = &first_desc->buffer [first_desc->offset + desc->len];;
        while (len >0)
        {
          if (len > BLOCK_SIZE)
          {
            dataSize = BLOCK_SIZE;
          }
          else
          {
            dataSize = len;
          }
          MALLOC (desc, (USHORT)(sizeof(T_desc2) - 1 + dataSize));
          memcpy( desc->buffer, data_p, dataSize);
          desc->offset = 0;
          desc->len = dataSize;
          desc->size = dataSize;
          len        -= dataSize;
          data_p     += dataSize;
          *pLast      = (ULONG) desc;
          pLast       = &desc->next;
        }

        MFREE (first_desc);
      }
#endif /* _SIMULATION_ */

      p_ul->dti_data_req = dti_data_req;

      if (p_ul->drop_packet)
      {
        p_ul->drop_packet = FALSE;
        PFREE_DESC2 (p_ul->dti_data_req);
        p_ul->dti_data_req = NULL;
        dti_start (
          udp_hDTI,
          UDP_DTI_DEF_INSTANCE,
          UDP_DTI_LL_INTERFACE,
          UDP_DTI_DEF_CHANNEL
        );
      }
      else
      {
        /* Get the IP addresses and the ports from HL */

        T_desc2 * desc = (T_desc2 *) p_ul->dti_data_req->desc_list2.first;
        T_SRC_DES * p_SrcDes;
        USHORT old_len;

        if (desc == NULL) {
          TRACE_ERROR (
            "Hila's T_desc2 is NULL in "
            "sig_dti_ker_data_received_hl_ind()."
          );
          return;
        }

        p_SrcDes = (T_SRC_DES *) (desc->buffer + desc->offset);

        memcpy (& p_ker->src_addr, p_SrcDes->src_ip, 4);
        memcpy (& p_ker->dst_addr, p_SrcDes->des_ip, 4);
        memcpy (& p_ker->src_port, p_SrcDes->src_port, 2);
        memcpy (& p_ker->dst_port, p_SrcDes->des_port, 2);

        old_len = p_ul->dti_data_req->desc_list2.list_len;
        p_ul->dti_data_req->desc_list2.first = desc->next;
        p_ul->dti_data_req->desc_list2.list_len =
          (USHORT) (old_len - sizeof (T_SRC_DES));
        p_ul = & udp_data->hila;
        MFREE (desc);
      }

      /* Send IP_ADDR_REQ to LL */
      {
        PALLOC (ip_addr_req, IP_ADDR_REQ);
        ip_addr_req->dst_addr = p_ker->dst_addr;
        ip_addr_req->trans_prot = UDP_PROTOCOL;
        PSENDX (LL, ip_addr_req);
      }
    }
    break;

  default:
    PFREE_DESC2 (dti_data_req);
    break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                            MODULE  : udp_kerp.c      |
| STATE   : code                           ROUTINE : ker_ip_addr_cnf |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T_IP_ADDR_CNF
*/

void ker_ip_addr_cnf (T_IP_ADDR_CNF * ip_addr_cnf)
{
  TRACE_FUNCTION ("ker_ip_addr_cnf()");
  PACCESS (ip_addr_cnf);

  switch (GET_STATE (KER))
  {
  case CONNECTED:
    {
      T_KER * p_ker = & udp_data->ker;
      T_HILA * p_ul = & udp_data->hila;
      BOOL send_ready_ind = FALSE;

      /* No error ? */
      if (ip_addr_cnf->err NEQ IP_ADDR_NOERROR)
      {
        PALLOC (udp_error_ind, UDP_ERROR_IND);
        udp_error_ind->dst_port = p_ker->dst_port;
        udp_error_ind->src_port = p_ker->src_port;
        udp_error_ind->err_msg  = ip_addr_cnf->err;
        udp_error_ind->src_addr = ip_addr_cnf->src_addr;
        udp_error_ind->dst_addr = p_ker->dst_addr;
        PSENDX (HL, udp_error_ind);
        p_ul->drop_packet = TRUE; /* Packet error - drop it */
      }
      /* Is the address correct ? */
      else if (ip_addr_cnf->src_addr NEQ p_ker->src_addr)
      {
        PALLOC (udp_error_ind, UDP_ERROR_IND);
        udp_error_ind->dst_port = p_ker->dst_port;
        udp_error_ind->src_port = p_ker->src_port;
        udp_error_ind->err_msg  = IP_ADDR_NOROUTE;
        udp_error_ind->src_addr = ip_addr_cnf->src_addr;
        udp_error_ind->dst_addr = p_ker->dst_addr;
        PSENDX (HL, udp_error_ind);
        p_ul->drop_packet = TRUE; /* Packet error - drop it */
      }
      else /* Build the "standard" packet for higher layer */
      {
        udp_build_packet (TRUE, B_NORMAL_PACKET);
      }

      /* ICMP sent at first */
      if (p_ker->send_icmp && GET_STATE (HILA) EQ WAIT)
      {
        T_DTI2_DATA_REQ * dti_data_req;
        dti_data_req = p_ker->dti_data_req_icmp;
        dti_data_req->parameters.p_id                  = DTI_PID_IP;
        dti_data_req->parameters.st_lines.st_flow      = DTI_FLOW_ON;
        dti_data_req->parameters.st_lines.st_line_sa   = DTI_SA_ON;
        dti_data_req->parameters.st_lines.st_line_sb   = DTI_SB_ON;
        dti_data_req->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

        PACCESS (dti_data_req);
        {
          PPASS (dti_data_req, dti_data_ind, DTI2_DATA_IND);
          dti_send_data (
            udp_hDTI,
            UDP_DTI_DEF_INSTANCE,
            UDP_DTI_LL_INTERFACE,
            UDP_DTI_DEF_CHANNEL,
            dti_data_ind
          );
        }
        p_ker->dti_data_req_icmp = NULL;
        SET_STATE (HILA, SEND);

        /* Call dti_start to indicate ready for new packet */
        dti_start (
          udp_hDTI,
          UDP_DTI_DEF_INSTANCE,
          UDP_DTI_LL_INTERFACE,
          UDP_DTI_DEF_CHANNEL
        );
      }

      /* Packet not correct - drop it */
      if (p_ul->drop_packet)
      {
        p_ul->drop_packet = FALSE;
        PFREE_DESC2 (p_ul->dti_data_req);
        p_ul->dti_data_req = NULL;
        send_ready_ind = TRUE;
      }
      else
      {
        /* We have got a ready indication from LL -> send */

        if (GET_STATE (HILA) EQ WAIT) /* Send dti_data_req packet */
        {
          p_ul->dti_data_req->parameters.p_id                 = DTI_PID_IP;
          p_ul->dti_data_req->parameters.st_lines.st_flow     = DTI_FLOW_ON;
          p_ul->dti_data_req->parameters.st_lines.st_line_sa  = DTI_SA_ON;
          p_ul->dti_data_req->parameters.st_lines.st_line_sb  = DTI_SB_ON;
          p_ul->dti_data_req->parameters.st_lines.st_break_len=DTI_BREAK_OFF;

          PACCESS (p_ul->dti_data_req);
          {
            PPASS (p_ul->dti_data_req, dti_data_ind, DTI2_DATA_IND);
            dti_send_data (
              udp_hDTI,
              UDP_DTI_DEF_INSTANCE,
              UDP_DTI_LL_INTERFACE,
              UDP_DTI_DEF_CHANNEL,
              dti_data_ind
            );
          }
          SET_STATE (HILA, IDLE);
          send_ready_ind = TRUE;
          p_ul->dti_data_req = NULL;
        }
        else
        {
          SET_STATE (HILA, SEND);
          send_ready_ind = FALSE;
        }
      }

      /* Send dti_ready indication */
      if (send_ready_ind)
      {
        dti_start (
          udp_hDTI,
          UDP_DTI_DEF_INSTANCE,
          UDP_DTI_HL_INTERFACE,
          UDP_DTI_DEF_CHANNEL
        );
      }
      PFREE (ip_addr_cnf);
    }
    break;

    default:
      PFREE (ip_addr_cnf);
      break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP           MODULE  : udp_kerp.c                       |
| STATE   : code          ROUTINE : sig_dti_ker_data_received_ll_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_DATA_RECEIVED_DN_IND
*/

void sig_dti_ker_data_received_ll_ind (T_DTI2_DATA_IND * dti_data_ind)
{
  TRACE_FUNCTION ("sig_dti_ker_data_received_ll_ind()");
  PACCESS (dti_data_ind);

  switch (GET_STATE (KER))
  {
  case CONNECTED:
    {
      USHORT calc_chksum, header_chksum, start_udp, udp_len;
      UBYTE prot;
      BOOL send_getdata_req = FALSE;
      UBYTE * ip_header;
      T_desc_list2 * desc_list;
      T_desc2 * desc;

      T_KER * p_ker = & udp_data->ker;
      T_LOLA * p_dl = & udp_data->lola;

      dti_stop (
        udp_hDTI,
        UDP_DTI_DEF_INSTANCE,
        UDP_DTI_LL_INTERFACE,
        UDP_DTI_DEF_CHANNEL
      );

      /* Build necessary variable */

      p_dl->dti_data_ind = dti_data_ind;
      p_dl->state_err = STATE_DL_NO_ERROR;

      desc_list = & p_dl->dti_data_ind->desc_list2;
      desc = (T_desc2 *) desc_list->first;

      if (
        desc == NULL ||
        desc->len < LEN_IP_HEADER_B ||
        GET_IP_HEADER_LEN_B (desc->buffer + desc->offset) < LEN_IP_HEADER_B ||
        desc->len < GET_IP_HEADER_LEN_B (desc->buffer + desc->offset) +
          LEN_UDP_HEADER_B
      ) {
        TRACE_ERROR (
          "Lola's UDP packet is too short in "
          "sig_dti_ker_data_received_ll_ind()."
        );
        return;
      }

      ip_header = desc->buffer + desc->offset;
      start_udp = (USHORT) GET_IP_HEADER_LEN_B (ip_header);

      /* Fill in port numbers and IP addresses */

      p_dl->src_addr = GET_IP_SOURCE_ADDR (ip_header);
      p_dl->dst_addr = GET_IP_DEST_ADDR (ip_header);
      p_dl->src_port = (USHORT) GET_UDP_SRC_PORT (ip_header, start_udp);
      p_dl->dst_port = (USHORT) GET_UDP_DST_PORT (ip_header, start_udp);

      udp_len = (USHORT) GET_UDP_LEN (ip_header, start_udp);
      prot = GET_IP_PROT (ip_header);

      /* Check if ICMP Message, if yes - send UDP_ERROR_IND */

      if (GET_IP_PROT (ip_header) EQ ICMP_PROT)
      {
        USHORT pos_icmp = (USHORT) GET_IP_HEADER_LEN_B (ip_header);

        p_dl->drop_packet = TRUE;
        p_dl->state_err = STATE_DL_ERROR;
        {
          PALLOC (udp_error_ind, UDP_ERROR_IND);
          udp_error_ind->dst_port = p_dl->dst_port;
          udp_error_ind->src_port = p_dl->src_port;
          udp_error_ind->err_msg  = GET_ICMP_TYPE (ip_header, pos_icmp);
          udp_error_ind->src_addr = GET_IP_SOURCE_ADDR (ip_header);
          udp_error_ind->dst_addr = GET_IP_DEST_ADDR (ip_header);
          PSENDX (HL, udp_error_ind);
        }
      }
      else /* No ICMP_PROT */
      {
        {
          ULONG overlay;
          UBYTE pseudo_header [LEN_PSEUDO_HEADER_B];

          /* Build the pseudoheader for the calculation of the checksum */

          SET_UDP_PSEUDO_H_SRC_ADDR (pseudo_header, p_dl->src_addr);
          SET_UDP_PSEUDO_H_DEST_ADDR (pseudo_header, p_dl->dst_addr);
          SET_UDP_PSEUDO_H_ZERO (pseudo_header, 0);
          SET_UDP_PSEUDO_H_PROT (pseudo_header, prot);
          SET_UDP_PSEUDO_H_LEN (pseudo_header, udp_len);

          /* Calculate a part of the checksum for the pseudoheader */
          overlay = partial_checksum (pseudo_header, LEN_PSEUDO_HEADER_B);

          /* Calculate for the UDP header and the data */
          header_chksum = (USHORT) GET_UDP_CHK_SUM (ip_header, start_udp);
          RESET_UDP_CHKSUM (ip_header, start_udp);
          calc_chksum = desc_checksum (desc_list, start_udp, overlay);
          SET_UDP_CHKSUM (ip_header, start_udp, header_chksum);
        }

        /* Packet checksum error ? */
        if (header_chksum NEQ calc_chksum)
        {
          p_dl->drop_packet = TRUE;
        }
        else
        {

#define POS_UDP_DEST_PORT 1


          /* Is it the correct port ? */
          if (p_ker->src_port EQ p_dl->dst_port)
          {
            udp_build_packet (FALSE, B_NORMAL_PACKET);
          }
          else
          {
            /* Build ICMP packet for no dest port.
             * If LL is ready to collect then send */
            udp_build_packet (FALSE, B_NO_DEST_PORT);
            p_ker->send_icmp = TRUE;
          }
        }
      } /* No ICMP_PROT */

      /* Packet is not correct -> drop it and free the primitive */
      if (p_dl->drop_packet)
      {
        p_dl->drop_packet = FALSE;
        PFREE_DESC2 (p_dl->dti_data_ind);
        p_dl->dti_data_ind = NULL;
        send_getdata_req = TRUE;
      }
      else
      {
        /* An ICMP message - send it */
        if (p_ker->send_icmp)
        {
          UBYTE state = GET_STATE (LOLA);
          if (state EQ IDLE OR state EQ WAIT)
          {
            T_DTI2_DATA_REQ * dti_data_req;

            udp_data->ker.send_icmp = FALSE;

            dti_data_req = p_ker->dti_data_req_icmp;
            dti_data_req->parameters.p_id                  = DTI_PID_IP;
            dti_data_req->parameters.st_lines.st_flow      = DTI_FLOW_ON;
            dti_data_req->parameters.st_lines.st_line_sa   = DTI_SA_ON;
            dti_data_req->parameters.st_lines.st_line_sb   = DTI_SB_ON;
            dti_data_req->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

            PACCESS (dti_data_req);
            {
              PPASS (dti_data_req, dti_data_indication, DTI2_DATA_IND);
              dti_send_data (
                udp_hDTI,
                UDP_DTI_DEF_INSTANCE,
                UDP_DTI_LL_INTERFACE,
                UDP_DTI_DEF_CHANNEL,
                dti_data_indication
              );
            }
            SET_STATE (LOLA, IDLE);
            p_ker->dti_data_req_icmp = NULL;
            send_getdata_req = TRUE;
          }
        }
        else
        {
          /* We have got a DTI2_GETDATA_REQ from LL. Send the packet. */

          if (GET_STATE (LOLA) EQ WAIT)
          {
            SET_STATE (LOLA, IDLE);

            p_dl->dti_data_ind->parameters.p_id = DTI_PID_IP;
            p_dl->dti_data_ind->parameters.st_lines.st_flow = DTI_FLOW_ON;
            p_dl->dti_data_ind->parameters.st_lines.st_line_sa = DTI_SA_ON;
            p_dl->dti_data_ind->parameters.st_lines.st_line_sb = DTI_SB_ON;
            p_dl->dti_data_ind->parameters.st_lines.st_break_len =
              DTI_BREAK_OFF;

            dti_send_data (
              udp_hDTI,
              UDP_DTI_DEF_INSTANCE,
              UDP_DTI_HL_INTERFACE,
              UDP_DTI_DEF_CHANNEL,
              p_dl->dti_data_ind
            );
            send_getdata_req = TRUE;
            p_dl->dti_data_ind = NULL;
          }
          else
          {
            SET_STATE (LOLA, SEND);
          }
        }
      }

      if (send_getdata_req) /* Send DTI2_GETDATA_REQ to LL */
      {
        dti_start (
          udp_hDTI,
          UDP_DTI_DEF_INSTANCE,
          UDP_DTI_LL_INTERFACE,
          UDP_DTI_DEF_CHANNEL
        );
      }
    }
    break;

  default:
    PFREE_DESC2 (dti_data_ind);
    break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP         MODULE  : udp_kerp.c                         |
| STATE   : code        ROUTINE : sig_dti_ker_tx_buffer_ready_hl_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_TX_BUFFER_READY_UP_IND
*/

void sig_dti_ker_tx_buffer_ready_hl_ind ()
{
  TRACE_FUNCTION ("sig_dti_ker_tx_buffer_ready_hl_ind()");

  switch (GET_STATE (KER))
  {
  case CONNECTED:
    {
      T_KER * p_ker = & udp_data->ker;
      T_LOLA * p_dl = & udp_data->lola;

      /* We have bound a port and got a DTI_GETDATA_REQ */

      if (p_ker->port_state EQ PORT_BOUND)
      {
        p_ker->port_state = PORT_ACTIVE;
        dti_start (
          udp_hDTI,
          UDP_DTI_DEF_INSTANCE,
          UDP_DTI_HL_INTERFACE,
          UDP_DTI_DEF_CHANNEL
        );
      }

      /* Send dti_data_ind packet if we have one */

      if (GET_STATE (LOLA) EQ SEND)
      {
        p_dl->dti_data_ind->parameters.p_id                  = DTI_PID_IP;
        p_dl->dti_data_ind->parameters.st_lines.st_flow      = DTI_FLOW_ON;
        p_dl->dti_data_ind->parameters.st_lines.st_line_sa   = DTI_SA_ON;
        p_dl->dti_data_ind->parameters.st_lines.st_line_sb   = DTI_SB_ON;
        p_dl->dti_data_ind->parameters.st_lines.st_break_len = DTI_BREAK_OFF;

        dti_send_data (
          udp_hDTI,
          UDP_DTI_DEF_INSTANCE,
          UDP_DTI_HL_INTERFACE,
          UDP_DTI_DEF_CHANNEL,
          p_dl->dti_data_ind
        );
        p_dl->dti_data_ind = NULL;
        SET_STATE (LOLA, IDLE);

        /* Send DTI2_GETDATA_REQ and indicate ready for new packet */

        dti_start (
          udp_hDTI,
          UDP_DTI_DEF_INSTANCE,
          UDP_DTI_LL_INTERFACE,
          UDP_DTI_DEF_CHANNEL
        );
      }
      else
      {
        /* No data packet to be sent - change state */
        SET_STATE (LOLA, WAIT);
      }
    }
    break;
  default:
    break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                           MODULE  : udp_kerp.c       |
| STATE   : code                          ROUTINE : ker_udp_bind_cnf |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T_UDP_BIND_CNF
*/

static void ker_udp_bind_cnf (U16 port, U8 err)
{
  PALLOC (udp_bind_cnf, UDP_BIND_CNF);

  TRACE_FUNCTION ("ker_udp_bind_cnf()");

  udp_bind_cnf->port = port;
  udp_bind_cnf->err = err;
  PSENDX (HL, udp_bind_cnf);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                           MODULE  : udp_kerp.c       |
| STATE   : code                          ROUTINE : ker_udp_bind_req |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T_UDP_BIND_REQ
*/

void ker_udp_bind_req (T_UDP_BIND_REQ * udp_bind_req)
{
  TRACE_FUNCTION ("ker_udp_bind_req()");
  PACCESS (udp_bind_req);

  switch (GET_STATE (KER))
  {
  case ACTIVE_NC:
    ker_udp_bind_cnf (udp_bind_req->port, UDP_BIND_UDPDOWN);
    break;

  case CONNECTED:
    {
      T_KER * p_ker = & udp_data->ker;

      if (p_ker->port_state EQ PORT_DOWN)
      {
        if (udp_bind_req->port EQ NO_PORT)
        {
          p_ker->src_port = 1025;
        }
        else
        {
          p_ker->src_port = udp_bind_req->port;
        }
        p_ker->port_state = PORT_BOUND;
        ker_udp_bind_cnf (p_ker->src_port, UDP_BIND_NOERROR);
      }
      else /* Port in use */
      {
        ker_udp_bind_cnf (udp_bind_req->port, UDP_BIND_PORTINUSE);
      }
    }
    break;

  case DEACTIVATED:
    TRACE_ERROR ("unexpected UDP_BIND_REQ");
    break;

  default:
    break;
  }
  PFREE (udp_bind_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                      MODULE  : udp_kerp.c            |
| STATE   : code                     ROUTINE : ker_udp_closeport_req |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T_UDP_CLOSEPORT_REQ
*/

void ker_udp_closeport_req (T_UDP_CLOSEPORT_REQ * udp_closeport_req)
{
  T_KER * p_ker;

  TRACE_FUNCTION ("ker_udp_closeport_req()");
  PACCESS (udp_closeport_req);

  p_ker = & udp_data->ker;

  switch (GET_STATE (KER))
  {
  case ACTIVE_NC:
  case CONNECTED:
    /* Close port */
    p_ker->src_port = NO_PORT;
    p_ker->port_state = PORT_DOWN;
    {
      PALLOC (udp_closeport_cnf, UDP_CLOSEPORT_CNF);
      PSENDX (HL, udp_closeport_cnf);
    }
    break;

  case DEACTIVATED:
    TRACE_ERROR ("unexpected UDP_CLOSEPORT_REQ");
    break;

  default:
    break;
  }
  PFREE (udp_closeport_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                          MODULE  : udp_kerp.c        |
| STATE   : code                         ROUTINE : ker_udp_error_res |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T_UDP_ERROR_RES
*/

void ker_udp_error_res (T_UDP_ERROR_RES * udp_error_res)
{
  TRACE_FUNCTION ("ker_udp_error_res()");
  PACCESS (udp_error_res);

  /* Confirm the error message from ACI */

  switch (GET_STATE (KER))
  {
  case CONNECTED:
    {
      T_LOLA * p_dl = & udp_data->lola;

      if (p_dl->state_err EQ STATE_DL_ERROR)
        p_dl->state_err = STATE_DL_NO_ERROR;
      else
        dti_start (
          udp_hDTI,
          UDP_DTI_DEF_INSTANCE,
          UDP_DTI_LL_INTERFACE,
          UDP_DTI_DEF_CHANNEL
        );
    }
    break;
  default:
    break;
  }
  PFREE (udp_error_res);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP       MODULE  : UDP_KERP                             |
| STATE   : code      ROUTINE : sig_dti_ker_connection_opened_hl_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal sig_dti_ker_connection_opened_hl_ind
*/

void sig_dti_ker_connection_opened_hl_ind ()
{
  TRACE_FUNCTION ("sig_dti_ker_connection_opened_hl_ind()");

  if (GET_STATE (HILA) EQ CONNECTING)
  {
    UBYTE state = GET_STATE (LOLA);
    SET_STATE (HILA, IDLE);
    if (state NEQ CONNECTING AND state NEQ DOWN)
    {
      SET_STATE (KER, ACTIVE_NC);
    }
    {
      PALLOC (udpa_dti_cnf, UDPA_DTI_CNF);
      udpa_dti_cnf->link_id = udp_data->hila.link_id;
      udpa_dti_cnf->dti_conn = UDPA_CONNECT_DTI;
      PSENDX (ACI, udpa_dti_cnf);
    }
  }
  else
  {
    TRACE_ERROR ("unexpected dti_connect_ind (from higher layer)");
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP       MODULE  : UDP_KERP                             |
| STATE   : code      ROUTINE : sig_dti_ker_connection_opened_ll_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal sig_dti_ker_connection_opened_ll_ind
*/

void sig_dti_ker_connection_opened_ll_ind ()
{
  TRACE_FUNCTION ("sig_dti_ker_connection_opened_ll_ind()");

  if (GET_STATE (LOLA) EQ CONNECTING)
  {
    UBYTE state = GET_STATE (HILA);
    SET_STATE (LOLA, IDLE);
    if (state NEQ CONNECTING AND state NEQ DOWN)
    {
      SET_STATE (KER, ACTIVE_NC);
    }
    {
      PALLOC (udpa_dti_cnf, UDPA_DTI_CNF);
      udpa_dti_cnf->link_id = udp_data->lola.link_id;
      udpa_dti_cnf->dti_conn = UDPA_CONNECT_DTI;
      PSENDX (ACI, udpa_dti_cnf);
    }
  }
  else
  {
    TRACE_ERROR ("unexpected dti_connect_ind (from lower layer)");
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                       MODULE  : udp_kerp.c           |
| STATE   : code                      ROUTINE : ker_udp_shutdown_ind |
+--------------------------------------------------------------------+

  PURPOSE :
*/

static void ker_udpa_dti_ind (ULONG link_id)
{
  PALLOC (udpa_dti_ind, UDPA_DTI_IND);
  udpa_dti_ind->link_id = link_id;
  PSENDX (ACI, udpa_dti_ind);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                       MODULE  : udp_kerp.c           |
| STATE   : code                      ROUTINE : ker_udp_shutdown_ind |
+--------------------------------------------------------------------+

  PURPOSE :
*/

static void ker_udp_shutdown_ind (void)
{
  PALLOC (udp_shutdown_ind, UDP_SHUTDOWN_IND);
  PSENDX (HL, udp_shutdown_ind);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                    MODULE  : udp_kerp.c              |
| STATE   : code                   ROUTINE : check_port_shutdown_ind |
+--------------------------------------------------------------------+

  PURPOSE :
*/

static void check_port_shutdown_ind (void)
{
  T_KER * p_ker = & udp_data->ker;

  TRACE_FUNCTION ("check_port_shutdown_ind()");

  if (p_ker->port_state EQ PORT_ACTIVE)
  {
    p_ker->src_port = NO_PORT;
    p_ker->port_state = PORT_DOWN;
    ker_udp_shutdown_ind ();
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP       MODULE  : UDP_KERP                             |
| STATE   : code      ROUTINE : sig_dti_ker_connection_closed_hl_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal sig_dti_ker_connection_closed_hl_ind
*/

void sig_dti_ker_connection_closed_hl_ind ()
{
  TRACE_FUNCTION ("sig_dti_ker_connection_closed_hl_ind()");

  switch (GET_STATE (KER))
  {
  case CONNECTED:
    /*XXX bad case, switch to DEACTIVATED, prune buffers... XXX*/
    TRACE_ERROR ("unexpected DTI connection close from higher layer");
    check_port_shutdown_ind ();
    SET_STATE (KER, DEACTIVATED);
    break;
  case ACTIVE_NC:
    SET_STATE (KER, DEACTIVATED);
    break;
  default:
    break;
  }
  vsi_c_close (VSI_CALLER hCommHL);
  hCommHL = VSI_ERROR;

  SET_STATE (HILA, DOWN);
  *udp_data->hila.entity_name = 0;
  ker_udpa_dti_ind (udp_data->hila.link_id);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP       MODULE  : UDP_KERP                             |
| STATE   : code      ROUTINE : sig_dti_ker_connection_closed_ll_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal sig_dti_ker_connection_closed_ll_ind
*/

void sig_dti_ker_connection_closed_ll_ind ()
{
  TRACE_FUNCTION ("sig_dti_ker_connection_closed_ll_ind()");

  switch (GET_STATE (KER))
  {
  case CONNECTED:
    /*XXX bad case, switch to DEACTIVATED, prune buffers... XXX*/
    TRACE_ERROR ("unexpected DTI connection close from lower layer");
    check_port_shutdown_ind ();
    SET_STATE (KER, DEACTIVATED);
    break;
  case ACTIVE_NC:
    SET_STATE (KER, DEACTIVATED);
    break;
  default:
    break;
  }
  SET_STATE (LOLA, DOWN);
  *udp_data->lola.entity_name = 0;
  ker_udpa_dti_ind (udp_data->lola.link_id);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                        MODULE  : udp_kerp.c          |
| STATE   : code                       ROUTINE : ker_udpa_config_req |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T_UDPA_CONFIG_REQ
*/

void ker_udpa_config_req (T_UDPA_CONFIG_REQ * udpa_config_req)
{
  TRACE_FUNCTION ("ker_udpa_config_req()");
  PACCESS (udpa_config_req);

  /* Configuration of lower layer -> change to state CONNECTED */

  switch (GET_STATE (KER))
  {
  case CONNECTED:
    switch (udpa_config_req->cmd)
    {
    case UDPA_CONFIG_DOWN:
      check_port_shutdown_ind ();
      SET_STATE (KER, ACTIVE_NC);
      {
        PALLOC (udpa_config_cnf, UDPA_CONFIG_CNF);
        PSENDX (ACI, udpa_config_cnf);
      }
     break;
    default:
      break;
    }
    break;

  case ACTIVE_NC:
    switch (udpa_config_req->cmd)
    {
    case UDPA_CONFIG_UP:
      dti_start (
        udp_hDTI,
        UDP_DTI_DEF_INSTANCE,
        UDP_DTI_LL_INTERFACE,
        UDP_DTI_DEF_CHANNEL
      );
      SET_STATE (KER, CONNECTED);
      TRACE_EVENT ("KER -> CONNECTED");
      {
        PALLOC (udpa_config_cnf, UDPA_CONFIG_CNF);
        PSENDX (ACI, udpa_config_cnf);
      }
      break;
    case UDPA_CONFIG_DOWN:
      {
        PALLOC (udpa_config_cnf, UDPA_CONFIG_CNF);
        PSENDX (ACI, udpa_config_cnf);
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  PFREE (udpa_config_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                       MODULE  : udp_kerp.c           |
| STATE   : code                      ROUTINE : ker_udp_shutdown_res |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T_UDP_SHUTDOWN_RES
*/

void ker_udp_shutdown_res (T_UDP_SHUTDOWN_RES * udp_shutdown_res)
{
  TRACE_FUNCTION ("DUMMY ker_udp_shutdown_res()");
  PACCESS (udp_shutdown_res);
  PFREE (udp_shutdown_res);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP          MODULE  : udp_kerp.c                        |
| STATE   : code         ROUTINE : sig_dti_ker_tx_buffer_full_hl_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_TX_BUFFER_FULL_UP_IND
*/

void sig_dti_ker_tx_buffer_full_hl_ind ()
{
  TRACE_FUNCTION ("DUMMY sig_dti_ker_tx_buffer_full_hl_ind()");
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP          MODULE  : UDP_KERP                          |
| STATE   : code         ROUTINE : sig_dti_ker_tx_buffer_full_ll_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal SIG_DTI_KER_TX_BUFFER_FULL_DN_IND
*/

void sig_dti_ker_tx_buffer_full_ll_ind ()
{
  TRACE_FUNCTION ("DUMMY sig_dti_ker_tx_buffer_full_ll_ind()");
}

/*-------------------------------------------------------------------------*/

