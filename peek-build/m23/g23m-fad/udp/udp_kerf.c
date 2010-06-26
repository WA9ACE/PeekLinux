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

#include <string.h>
#include "typedefs.h"
#include "pcm.h"
#include "pconst.cdg"
#include "mconst.cdg"
#include "message.h"
#include "ccdapi.h"
#include "vsi.h"
#include "macdef.h" /* to get PFREE_DESC2 */
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "cus_udp.h"
#include "cnf_udp.h"
#include "mon_udp.h"
#include "pei.h"
#include "tok.h"
#include "dti.h" /* to get dti lib */
#include "udp.h"
#include "ip_udp.h"

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                                 MODULE  : udp_kerf.c |
| STATE   : code                                ROUTINE : init_udp   |
+--------------------------------------------------------------------+

  PURPOSE : Init the global variables of UDP
*/

void init_udp (void)
{
  T_HILA * p_ul;
  T_LOLA * p_dl;
  T_KER * p_ker = & udp_data->ker;

  /* Set global variables for every entity */

  INIT_STATE (HILA, DOWN);
  INIT_STATE (LOLA, DOWN);

  p_ul = & udp_data->hila;
  *p_ul->entity_name = 0;
  p_ul->dti_state    = DTI_CLOSED;
  p_ul->dti_data_req = NULL;

  p_dl = & udp_data->lola;
  *p_dl->entity_name = 0;
  p_dl->dti_state    = DTI_CLOSED;
  p_dl->dti_data_ind = NULL;
  p_dl->drop_packet  = FALSE;
  p_dl->state_err    = STATE_DL_NO_ERROR;
  p_dl->src_addr     = NO_ADDR;
  p_dl->dst_addr     = NO_ADDR;
  p_dl->dst_port     = NO_PORT;
  p_dl->src_port     = NO_PORT;

  /* Downlink and uplink for one entity */

  p_ker->send_icmp         = FALSE;
  p_ker->port_state        = PORT_DOWN;
  p_ker->dti_data_req_icmp = NULL;

  p_ker->dst_addr = NO_ADDR;
  p_ker->src_addr = NO_ADDR;
  p_ker->src_port = NO_PORT;
  p_ker->dst_port = NO_PORT;
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                           MODULE  : udp_kerf.c       |
| STATE   : code                          ROUTINE : udp_build_packet |
+--------------------------------------------------------------------+

  PURPOSE : Build UDP and ICMP packets to send
*/

void udp_build_packet (BOOL hila, UBYTE to_do)
{
  T_KER * p_ker = & udp_data->ker;

  if (hila) /* Build the packets for higher layer */
  {
    switch (to_do)
    {
    case B_NORMAL_PACKET:
      {
        T_desc_list2 * desc_list = & udp_data->hila.dti_data_req->desc_list2;
        T_desc2 * desc = (T_desc2 *) desc_list->first;
        USHORT udp_payload_len = desc_list->list_len;

        if (desc == NULL)
        {
          TRACE_ERROR ("Hila's desc == NULL in udp_build_packet().");
          return;
        }
        else
        {
          /* Make a new desc for the IP and UDP packet header */

          USHORT headroom = LEN_IP_HEADER_B + LEN_UDP_HEADER_B;
          UBYTE * ip_header;
          T_desc2 * desc_new = M_ALLOC (offsetof (T_desc2, buffer) + headroom);

          if (desc_new == NULL)
          {
            TRACE_ERROR ("Not enough memory in udp_build_packet().");
            return;
          }
          desc_new->next = (ULONG) desc;
          desc_new->offset = 0;
          desc_new->len = headroom;
          desc_new->size = headroom;
          ip_header = desc_new->buffer;
          {
            register unsigned i;
            for (i=0; i < headroom; i++)
              ip_header[i] = '\0';/*lint !e661 !e662 (Warning -- access/creation of out-of-bounds pointer) */
          }

          /* Put the desc at the first place of the descs and
           * build the UDP header */

          desc_list->first = (ULONG) desc_new;
          desc_list->list_len = (USHORT) (desc_new->len + desc_list->list_len);

          /* Fill in IP header information */

          SET_IP_VERSION (ip_header, IP_VERSION);
          SET_IP_HEADER_LEN (ip_header, MIN_HEADER_LEN);
          /*lint -e{661, 662} (Warning -- access/creation of out-of-bounds pointer) */
         {
          SET_IP_TOTAL_LEN (ip_header, desc_list->list_len);
          SET_IP_PROT (ip_header, UDP_PROT);
          SET_IP_SOURCE_ADDR (ip_header, p_ker->src_addr);
          SET_IP_DEST_ADDR (ip_header, p_ker->dst_addr);

          SET_UDP_SRC_PORT (ip_header, p_ker->src_port);
          SET_UDP_DST_PORT (ip_header, p_ker->dst_port);
          SET_UDP_LEN (ip_header, udp_payload_len + LEN_UDP_HEADER_B);
          }
          {
            UBYTE pseudo_header [LEN_PSEUDO_HEADER_B];
            ULONG overlay;
            USHORT chksum;

            /* Build the pseudoheader for to calculate the chksum */

            SET_UDP_PSEUDO_H_SRC_ADDR (pseudo_header, p_ker->src_addr);
            SET_UDP_PSEUDO_H_DEST_ADDR (pseudo_header, p_ker->dst_addr);
            SET_UDP_PSEUDO_H_ZERO (pseudo_header, 0);
            SET_UDP_PSEUDO_H_PROT (pseudo_header, UDP_PROT);
            SET_UDP_PSEUDO_H_LEN (pseudo_header, udp_payload_len + LEN_UDP_HEADER_B);

            /* Calculate the checksum for the pseudoheader */
            overlay = partial_checksum (pseudo_header, LEN_PSEUDO_HEADER_B);

            /* Calculate for the UDP header and the data */
            RESET_UDP_CHKSUM (ip_header, LEN_IP_HEADER_B);/*lint !e661 !e662 (Warning -- access/creation of out-of-bounds pointer) */
            chksum = desc_checksum (desc_list, LEN_IP_HEADER_B, overlay);
            SET_UDP_CHKSUM (ip_header, LEN_IP_HEADER_B, chksum);/*lint !e661 !e662 (Warning -- access/creation of out-of-bounds pointer) */
          }
        }
      }
      break;

    default:
      break;
    }
  }
  else /* Build the packets for lower layer */
  {
    T_LOLA * p_dl = & udp_data->lola;
    T_desc_list2 * desc_list = & p_dl->dti_data_ind->desc_list2;
    T_desc2 * desc = (T_desc2 *) desc_list->first;

    if (desc == NULL)
    {
      TRACE_ERROR ("Lola's desc == NULL in udp_build_packet().");
      return;
    }
    if (desc->len < LEN_IP_HEADER_B)
    {
      TRACE_ERROR (
        "Lola's desc->len < LEN_IP_HEADER_B "
        "in udp_build_packet()."
      );
      return;
    }
    if (GET_IP_HEADER_LEN_B (desc->buffer + desc->offset) < LEN_IP_HEADER_B)
    {
      TRACE_ERROR (
        "Lola's GET_IP_HEADER_LEN_B() < LEN_IP_HEADER_B "
        "in udp_build_packet()."
      );
      return;
    }

    switch (to_do)
    {
    case B_NORMAL_PACKET:
      {
        /* Build a new desc for IP addresses and port numbers */

        T_desc2 * addr = M_ALLOC (
          offsetof (T_desc2, buffer) + sizeof (T_SRC_DES)
        );
        if (addr == NULL) {
          TRACE_ERROR ("Not enough memory in udp_build_packet().");
          return;
        }

        addr->next = (ULONG) desc;
        addr->offset = 0;
        addr->len = sizeof (T_SRC_DES);
        addr->size = sizeof (T_SRC_DES);

        memcpy (((T_SRC_DES *) addr->buffer)->src_ip, & p_dl->src_addr, 4);
        memcpy (((T_SRC_DES *) addr->buffer)->des_ip, & p_dl->dst_addr, 4);
        memcpy (((T_SRC_DES *) addr->buffer)->src_port, & p_dl->src_port, 2);
        memcpy (((T_SRC_DES *) addr->buffer)->des_port, & p_dl->dst_port, 2);

        desc_list->first = (ULONG) addr;
        desc_list->list_len = (USHORT) (addr->len + desc_list->list_len);

        /* Filter out the given IP and UDP header */
        {
          register unsigned headroom =
            GET_IP_HEADER_LEN_B (desc->buffer + desc->offset) + LEN_UDP_HEADER_B;
          if (desc->len < headroom)
          {
            TRACE_ERROR ("Lola's IP packet is short in udp_build_packet().");
            return;
          }
          memmove (
            desc->buffer + desc->offset,
            desc->buffer + desc->offset + headroom,
            desc->len - headroom
          );
          desc->len = (USHORT) (desc->len - headroom);
          desc_list->list_len = (USHORT) (desc_list->list_len - headroom);
        }
      }
      break;

    case B_NO_DEST_PORT:
      {
        /* We build the whole ICMP packet */

        PPASS (p_dl->dti_data_ind, data_req, DTI2_DATA_REQ);

        desc_list = & data_req->desc_list2;
        desc = (T_desc2 *) desc_list->first;

        build_icmp_with_payload (
          data_req,
          IDENTITY_0,
          STANDARD_TTL,
          GET_IP_DEST_ADDR (desc->buffer + desc->offset),
          ICMP_TYP_DEST_URECHBL,
          ICMP_CODE_NO_PORT
        );

        p_ker->dti_data_req_icmp = data_req;
      }
      break;

    default:
      break;
    }
  }
}

/*-------------------------------------------------------------------------*/

