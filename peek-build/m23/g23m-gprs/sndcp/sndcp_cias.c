/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_cias.c
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
|
|  These files implement Van Jacobson Compression algorithm which is taken from
|  University of California Berkley's Implementation.
|
|
|  "Copyright (c)1989 Regents of the University of California.
|  All rights reserved.Redistribution and use in source and binary forms are
|  permitted, provided that the above  copyright notice and this paragraph are
|  duplicated in all such forms and that any documentation advertising materials,
|  and  other materials related to such distributionand use acknowledge that the
|  software was developed by the University of California, Berkeley.
|  The name of the University may not be  used to endorse or promote products
|  derived from this software without specific  prior written permission. 
|  THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
|  INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND 
|  FITNESS FOR A PARTICULAR PURPOSE"
+-----------------------------------------------------------------------------
|  Purpose :  This modul is part of the entity SNDCP and implements all
|             functions to handles the incoming process internal signals as
|             described in the SDL-documentation (CIA-statemachine)
+-----------------------------------------------------------------------------
*/


#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"         /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"         /* to get a lot of macros */
#include "prim.h"        /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"       /* to get the global entity definitions */
#include "sndcp_f.h"     /* to get the functions to access the global arrays*/

#include <string.h>      /* to get memcpy() */
#include "sndcp_cias.h"  /* to get the signals to service cia */
#include "sndcp_ciap.h"  /* to get primitives to service cia */
#include "sndcp_ciaf.h"  /* to get primitives to service cia */
#include "sndcp_sdf.h"   /* to get sd functions */
#include "sndcp_sdaf.h"  /* to get sda functions */
#include "sndcp_sds.h"   /* to get sd signals */

#ifdef TI_PS_FF_V42BIS
#include "v42b_type.h"
#include "v42b_dico.h"
#include "v42b_enc.h"
#include "v42b_dec.h"
#include "v42b_debug.h"
#endif /* TI_PS_FF_V42BIS */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
 * Update connection state cs & send uncompressed packet ('uncompressed'
 * means a regular ip/tcp packet but with the 'conversation id' we hope
 * to use on future compressed packets in the protocol field).
 */
#define UNCOMPRESSED {\
  memcpy(cs->cs_hdr, (UBYTE*)cbuf->c_hdr, tip_hlen);\
  ip->ip_p = cs->cs_id;\
  sndcp_data->cia.comp.last_xmit = cs->cs_id;\
}


/*
+------------------------------------------------------------------------------
| Function    : cia_vj_comp
+------------------------------------------------------------------------------
| Description : compresses the TCP/IP header of the given packet
|               (Van Jacobson algorithm). A part of this routine has been taken
|               from implementation of University of California, Berkeley.
|
| Parameters  : com_buf - received packet, packet length, packet type
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

LOCAL UBYTE cia_vj_comp(struct comp_buf *cbuf)
{
  struct cstate            *cs  = sndcp_data->cia.comp.last_cs->cs_next;
  T_SNDCP_TCP_HEADER       *oth, *oth_tmp;     /* last TCP header */
  T_SNDCP_TCP_HEADER       *th;                /* current TCP header */
  T_SNDCP_IP_HEADER        *ip;                /* current IP header */
  USHORT                   ip_hlen, tip_hlen;  /* IP hdr len., TCP/IP hdr len*/
  USHORT                   tmp1, tmp2;         /* general purpose tempories */
  ULONG                    changes = 0;        /* change mask */
  UBYTE                    new_seq[16];       /* changes from last to current*/
  UBYTE                    *cp = new_seq;
  UBYTE                    th_off, oth_off;
  BOOL                     found = FALSE;
  USHORT                   th_sum;
  ULONG                    seq, o_seq;

  ip      = (T_SNDCP_IP_HEADER *)cbuf->c_hdr;
  ip_hlen = ip->ip_vhl & HL_MASK;            /* IP header length in integers */
  th      = (T_SNDCP_TCP_HEADER *)&((ULONG *)cbuf->c_hdr)[ip_hlen];
  th_off  = th->th_off >> 4;

  TRACE_FUNCTION( "cia_vj_comp" );

  /*
   * Bail if this is an IP fragment or if the TCP packet isn't
   * `compressible' (i.e., ACK isn't set or some other control bit is
   * set).  (We assume that the caller has already made sure the
   * packet is IP proto TCP).
   */
  if (ip->ip_off & 0xff3f)
  {
    TRACE_EVENT("INFO COMP: return TYPE_IP, packet is an IP fragment");
    return (TYPE_IP);
  }

  if ((th->th_flags & (TH_SYN|TH_FIN|TH_RST|TH_ACK)) != TH_ACK)
  {
    TRACE_EVENT("INFO COMP: return TYPE_IP, TH_ACK is not set");
    return (TYPE_IP);
  }
  /*
   * Packet is compressible -- we're going to send either a
   * COMPRESSED_TCP or UNCOMPRESSED_TCP packet.  Either way we need
   * to locate (or create) the connection state.  Special case the
   * most recently used connection since it's most likely to be used
   * again & we don't have to do any reordering if it's used.
   */
  oth_tmp  = (T_SNDCP_TCP_HEADER *)
             &((ULONG *)cs->cs_hdr)[cs->cs_ip->ip_vhl & HL_MASK];
  if ((ip->ip_src   != cs->cs_ip->ip_src) ||
      (ip->ip_dst   != cs->cs_ip->ip_dst) ||
      (*(ULONG *)th != *(ULONG *)oth_tmp)){
    /*
     * Wasn't the first -- search for it.
     *
     * States are kept in a circularly linked list with
     * last_cs pointing to the end of the list.  The
     * list is kept in lru order by moving a state to the
     * head of the list whenever it is referenced.  Since
     * the list is short and, empirically, the connection
     * we want is almost always near the front, we locate
     * states via linear search.  If we don't find a state
     * for the datagram, the oldest state is (re-)used.
     */
    struct cstate *lcs;
    struct cstate *lastcs = sndcp_data->cia.comp.last_cs;

    do {
      lcs = cs;
      cs = cs->cs_next;
      oth_tmp  = (T_SNDCP_TCP_HEADER *)
                 &((ULONG *)cs->cs_hdr)[cs->cs_ip->ip_vhl & HL_MASK];
      if ((ip->ip_src   == cs->cs_ip->ip_src) &&
          (ip->ip_dst   == cs->cs_ip->ip_dst) &&
          (*(ULONG *)th == *(ULONG *)oth_tmp)){
        found = TRUE;
        break;
      }

    } while (cs != lastcs);

    if(found)
    {
      /*
       * Found it -- move to the front on the connection list.
       */
      if (cs == lastcs)
        sndcp_data->cia.comp.last_cs = lcs;
      else {
        lcs->cs_next = cs->cs_next;
        cs->cs_next = lastcs->cs_next;
        lastcs->cs_next = cs;
      }
    } else {
      /*
       * Didn't find it -- re-use oldest cstate.  Send an
       * uncompressed packet that tells the other side what
       * connection number we're using for this conversation.
       * Note that since the state list is circular, the oldest
       * state points to the newest and we only need to set
       * last_cs to update the lru linkage.
       */
      sndcp_data->cia.comp.last_cs = lcs;
      tip_hlen = (ip_hlen+th_off)<<2;
      UNCOMPRESSED;
      return (TYPE_UNCOMPRESSED_TCP);
    }/*if din't find*/
  }

  /*
   * Make sure that only what we expect to change changed. The first
   * line of the `if' checks the IP protocol version, header length &
   * type of service.  The 2nd line checks the "Don't fragment" bit.
   * The 3rd line checks the time-to-live and protocol (the protocol
   * check is unnecessary but costless).  The 4th line checks the TCP
   * header length.  The 5th line checks IP options, if any.  The 6th
   * line checks TCP options, if any.  If any of these things are
   * different between the previous & current datagram, we send the
   * current datagram `uncompressed'.
   */
  oth = (T_SNDCP_TCP_HEADER *)&((ULONG *)cs->cs_ip)[ip_hlen];
  oth_off  = oth->th_off >> 4;
  tip_hlen = (ip_hlen+th_off)<<2;

  if (((USHORT *)ip)[0] != ((USHORT *)cs->cs_ip)[0] ||
      ((USHORT *)ip)[3] != ((USHORT *)cs->cs_ip)[3] ||
      ((USHORT *)ip)[4] != ((USHORT *)cs->cs_ip)[4] ||
      (th_off           != oth_off)                 ||
      ((ip_hlen > 5) && (memcmp(ip + 1, cs->cs_ip + 1, (ip_hlen - 5) << 2))) ||
      ((th_off  > 5) && (memcmp(th + 1, oth + 1, (th_off - 5) << 2)))) {

    UNCOMPRESSED;
    return (TYPE_UNCOMPRESSED_TCP);
  }

  /*
   * Figure out which of the changing fields changed.  The
   * receiver expects changes in the order: urgent, window,
   * ack, seq (the order minimizes the number of temporaries
   * needed in this section of code).
   */
  if (th->th_flags & TH_URG) {
    tmp1 = sndcp_swap2(th->th_urp);
    if (tmp1 >= 256 || tmp1 == 0) {
      *cp++ = 0;
      cp[1] = (UBYTE)tmp1;
      cp[0] = (tmp1 & 0xff00) >> 8;
      cp += 2;
    } else {
      *cp++ = (UBYTE)tmp1;
    }
    changes |= NEW_U;
  } else if (th->th_urp != oth->th_urp){
    /* argh! URG not set but urp changed -- a sensible
     * implementation should never do this but RFC793
     * doesn't prohibit the change so we have to deal
     * with it. */
    UNCOMPRESSED;
    return (TYPE_UNCOMPRESSED_TCP);
  }
  if ((tmp1 = sndcp_swap2(th->th_win) - sndcp_swap2(oth->th_win)) != 0) {
    if (tmp1 >= 256) {
      *cp++ = 0;
      cp[1] = (UBYTE)tmp1;
      cp[0] = (tmp1 & 0xff00) >> 8;
      cp += 2;
    } else {
      *cp++ = (UBYTE)tmp1;
    }
    changes |= NEW_W;
  }

  if ((tmp2 = (USHORT)(sndcp_swap4(th->th_ack)-sndcp_swap4(oth->th_ack)))!=0){
    if (tmp2 > MAX_CHANGE){
      UNCOMPRESSED;
      return (TYPE_UNCOMPRESSED_TCP);
    }
    if (tmp2 >= 256) {
      *cp++ = 0;
      cp[1] = (UBYTE)tmp2;
      cp[0] = (tmp2 & 0xff00) >> 8;
      cp += 2;
    } else {
      *cp++ = (UBYTE)tmp2;
    }
    changes |= NEW_A;
  }

  seq   = sndcp_swap4(th->th_seq);
  o_seq = sndcp_swap4(oth->th_seq);
  if(seq < o_seq) {
    TRACE_EVENT("WARNING COMP: seq < o_seq, return UNCOMPRESSED_TCP ");
    UNCOMPRESSED;
    return (TYPE_UNCOMPRESSED_TCP);
  } else if((tmp1 = (USHORT)(seq - o_seq)) != 0) {
    if (tmp1 > MAX_CHANGE){
      UNCOMPRESSED;
      return (TYPE_UNCOMPRESSED_TCP);
    }
    if (tmp1 >= 256) {
      *cp++ = 0;
      cp[1] = (UBYTE)tmp1;
      cp[0] = (tmp1 & 0xff00) >> 8;
      cp += 2;
    } else {
      *cp++ = (UBYTE)tmp1;
    }
    changes |= NEW_S;
  }

  /* look for special case encodings. */
  switch (changes) {

  case 0:
    /*
     * Nothing changed. If this packet contains data and the
     * last one didn't, this is probably a data packet following
     * an ack (normal on an interactive connection) and we send
     * it compressed.  Otherwise it's probably a retransmit,
     * retransmitted ack or window probe.  Send it uncompressed
     * in case the other side missed the compressed version.
     */
    if ((ip->ip_len != cs->cs_ip->ip_len) &&
        (sndcp_swap2(cs->cs_ip->ip_len) == tip_hlen))
      break;

    /* (fall through) */

  case SPECIAL_I:
  case SPECIAL_D:
    /*
     * actual changes match one of our special case encodings --
     * send packet uncompressed.
     */
    UNCOMPRESSED;
    return (TYPE_UNCOMPRESSED_TCP);

  case NEW_S|NEW_A:
    if ((tmp1 == tmp2) &&
        (tmp1 == sndcp_swap2(cs->cs_ip->ip_len) - tip_hlen)) {
      /* special case for echoed terminal traffic */
      changes = SPECIAL_I;
      cp = new_seq;
    }
    break;

  case NEW_S:
    if (tmp1 == sndcp_swap2(cs->cs_ip->ip_len) - tip_hlen) {
      /* special case for data xfer */
      changes = SPECIAL_D;
      cp = new_seq;
    }
    break;
  }

  tmp1 = sndcp_swap2(ip->ip_id) - sndcp_swap2(cs->cs_ip->ip_id);
  if (tmp1 != 1) {
    if ((tmp1 >= 256) || (tmp1 == 0)) {
      *cp++ = 0;
      cp[1] = (UBYTE)tmp1;
      cp[0] = (tmp1 & 0xff00) >> 8;
      cp += 2;
    } else {
      *cp++ = (UBYTE)tmp1;
    }
    changes |= NEW_I;
  }

  if (th->th_flags & TH_PUSH)
    changes |= TCP_PUSH_BIT;
  /*
   * Grab the cksum before we overwrite it below.  Then update our
   * state with this packet's header.
   */
  th_sum = sndcp_swap2(th->th_sum);
  memcpy(cs->cs_ip, ip, tip_hlen);

  /*
   * We want to use the original packet as our compressed packet.
   * (cp - new_seq) is the number of bytes we need for compressed
   * sequence numbers.  In addition we need one byte for the change
   * mask, one for the connection id and two for the tcp checksum.
   * So, (cp - new_seq) + 4 bytes of header are needed.  hlen is how
   * many bytes of the original packet to toss so subtract the two to
   * get the new packet size.
   */
  tmp1 = cp - new_seq;
  cp = (UBYTE *)ip;

  /*
   * we always send a "new" connection id so the receiver state
   * stays synchronized.
   */
  sndcp_data->cia.comp.last_xmit = cs->cs_id;
  tip_hlen -= tmp1 + 4;
  cp       += tip_hlen;
  *cp++ = (UBYTE)(changes | NEW_C);
  *cp++ = cs->cs_id;

  cbuf->pack_len -= tip_hlen;
  cbuf->hdr_len  -= tip_hlen;
  cbuf->c_hdr    += tip_hlen;
  *cp++ = th_sum >> 8;
  *cp++ = (UBYTE)(th_sum & 0x00ff);
  memcpy(cp, new_seq, tmp1);
  return (TYPE_COMPRESSED_TCP);
}

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : cia_header_comp
+------------------------------------------------------------------------------
| Description : compresses the TCP/IP header of the given packet
|               (Van Jacobson algorithm)
|
| Parameters  : packet as desc_list, packet_type.
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

#ifdef _SNDCP_DTI_2_
GLOBAL void cia_header_comp (T_desc_list2* dest_desc_list,
                                T_desc_list2* src_desc_list,
                                UBYTE* packet_type)
#else /*_SNDCP_DTI_2_*/
GLOBAL void cia_header_comp (T_desc_list* dest_desc_list,
                                T_desc_list* src_desc_list,
                                UBYTE* packet_type)
#endif /*_SNDCP_DTI_2_*/
{

#ifdef _SNDCP_DTI_2_
  T_desc2* src_desc = (T_desc2*)src_desc_list->first;
  T_desc2* new_desc;
  U8 *src_desc_buff = &src_desc->buffer[src_desc->offset];
#else /*_SNDCP_DTI_2_*/
  T_desc*  src_desc = (T_desc*)src_desc_list->first;
  T_desc*  new_desc;
  U8 *src_desc_buff = &src_desc->buffer[0];
#endif /*_SNDCP_DTI_2_*/


/*
 * A struct mbuf is used in the call cia_vj_comp because that routine
 * needs to modify both the start address and length if the incoming
 * packet is compressed
 */

  struct comp_buf           cbuf;
  T_SNDCP_IP_HEADER         *ip;
  T_SNDCP_TCP_HEADER        *tcp;
  USHORT                    hdr_len;
  USHORT                    offset;

  TRACE_FUNCTION( "cia_header_comp" );

  if ((src_desc == NULL))
  {
    TRACE_EVENT("ERROR: Descriptor Pointer is a NULLPTR");
    *packet_type = TYPE_ERROR;
    return;
  }

  /*
   * Verify protocol type and header length
   */
  ip = (T_SNDCP_IP_HEADER *)&src_desc_buff[0];
  if((ip->ip_p == PROT_TCPIP))
  {
    hdr_len  = (ip->ip_vhl & HL_MASK) << 2;
    tcp      = (T_SNDCP_TCP_HEADER *)&src_desc_buff[hdr_len];
    hdr_len += (tcp->th_off >> 4) << 2;
    if( (src_desc->len >= hdr_len) && (hdr_len >=40) ){
      /*
       * initialize comp_buf
       */
      cbuf.hdr_len  = hdr_len;       /* header len. passed to compr. routine*/
      cbuf.pack_len = src_desc->len; /* descriptor length */
      cbuf.p_type   = TYPE_IP;       /* set default packet type */
      /* use temporary buffer to store TCP/IP header */
      cbuf.c_hdr    = (ULONG)&sndcp_data->cia.comp.tcpip_hdr[0];
      memcpy((UBYTE*)cbuf.c_hdr, &src_desc_buff[0], cbuf.hdr_len);
      /*
       * Call VJ header compression routine
       */
      *packet_type = cia_vj_comp(&cbuf);
    }
    else
      *packet_type = TYPE_IP;

  }/* if TCP_IP_PROTOKOL */
  else{
    *packet_type = TYPE_IP;
    if(src_desc->len <40)
      TRACE_EVENT("INFO COMP: Descriptor length < 40");
  }

  switch(*packet_type)
  {
    /*
     * IP packet
     */
    case TYPE_IP:
      /*
       * if acknowledged mode
       */
      if(src_desc_list != dest_desc_list)
      {
        dest_desc_list->first = src_desc_list->first;
        dest_desc_list->list_len = src_desc_list->list_len;
      }
      TRACE_EVENT("INFO COMP: Header Type TYPE_IP");
      break;

    /*
     * Uncompressed TCP/IP packet
     */
    case TYPE_UNCOMPRESSED_TCP:
      /*
       * if acknowledged mode
       */
      if(src_desc_list != dest_desc_list)
      {
#ifdef _SNDCP_DTI_2_
      MALLOC(new_desc, (USHORT)(sizeof(T_desc2)-1+src_desc->len));
#else
      MALLOC(new_desc, (USHORT)(sizeof(T_desc)-1+src_desc->len));
#endif
        /*
         * Copy data packet to destination descriptor
         */
        memcpy(new_desc->buffer, src_desc_buff, src_desc->len);
        /*
         * store connection id
         */
        new_desc->buffer[PR_TYPE_POS] = ((UBYTE*)cbuf.c_hdr)[PR_TYPE_POS]
        ;/*lint !e644 !e415 !e416 creation and access of out-of-bounds pointer*/
        /*
         * Build destination descriptor list
         */
        new_desc->next   = src_desc->next;
        new_desc->len    = src_desc->len;
#ifdef _SNDCP_DTI_2_
        new_desc->offset = src_desc->offset;
        new_desc->size   = src_desc->size;
#endif
        dest_desc_list->first    = (ULONG)new_desc;
        dest_desc_list->list_len = src_desc_list->list_len;
      }
      else{
       /*
        * store connection id
        */
        src_desc_buff[PR_TYPE_POS] = ((UBYTE*)cbuf.c_hdr)[PR_TYPE_POS]
        ;/*lint !e415 !e416 creation and access of out-of-bounds pointer */
      }
      TRACE_EVENT("INFO COMP: Header Type TYPE_UNCOMPRESSED_TCP");
      break;


    /*
     * Compressed TCP/IP packet
     */
    case TYPE_COMPRESSED_TCP:
      /* compute compressed header length */
      offset = (USHORT)(src_desc->len - cbuf.pack_len);
#ifdef _SNDCP_DTI_2_
      MALLOC(new_desc, (USHORT)(sizeof(T_desc2) - 1 + cbuf.pack_len));
#else
      MALLOC(new_desc, (USHORT)(sizeof(T_desc) - 1 + cbuf.pack_len));
#endif
      /*
       * Copy compressed header to new descriptor
       */
      memcpy(&new_desc->buffer[0], (UBYTE*)cbuf.c_hdr, cbuf.hdr_len);
      /*
       * Copy payload to new descriptor
       */
      memcpy(&new_desc->buffer[cbuf.hdr_len],
             &src_desc_buff[hdr_len], cbuf.pack_len-cbuf.hdr_len)
       ;/*lint !e644 !e662 Possible creation of out-of-bounds pointer */
      /*
       * Build destination descriptor list
       */
      new_desc->next   = src_desc->next;
      new_desc->len    = (USHORT)cbuf.pack_len;
#ifdef _SNDCP_DTI_2_
      new_desc->offset = 0;
      new_desc->size   = (USHORT)cbuf.pack_len;
#endif
      dest_desc_list->first    = (ULONG)new_desc;
      dest_desc_list->list_len = src_desc_list->list_len - offset;
      /*
       * if unacknowledgement mode
       */
      if(src_desc_list == dest_desc_list)
      {
        /*
         * Free source decsriptor
         */
        MFREE(src_desc);
      }
      TRACE_EVENT("INFO COMP: Header TYPE_COMPRESSED_TCP");
      break;

    default:
      TRACE_EVENT("Header Type: TYPE_ERROR");
      break;
    }

#ifdef SNDCP_TRACE_BUFFER
    sndcp_trace_desc_list(dest_desc_list);
#endif /* SNDCP_TRACE_BUFFER */

} /* cia_header_comp*/

#endif /* CF_FAST_EXEC */


#ifndef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : cia_su_cia_comp_req
+------------------------------------------------------------------------------
| Description : Simulation for cia reaction to SIG_SU_CIA_DATA_REQ.
|               Instead of sending the pdu to cia and then receiving
|               1 or more CIA_COMP_IND.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   !defined(REL99)   || defined(SNDCP_2to1) */

LOCAL void cia_su_cia_comp_req (T_SN_UNITDATA_REQ* sn_unitdata_req,
                                USHORT npdu_number,
                                UBYTE nsapi,
                                UBYTE sapi,
                                UBYTE packet_type
                                )
{
  BOOL ready = FALSE;
  USHORT header_size = SN_UNITDATA_PDP_HDR_LEN_BIG;
  /*
   * Bit offset in destination sdu.
   */
  USHORT bit_offset = ENCODE_OFFSET + (USHORT)(header_size << 3);
  /*
   * Offset in current desc.
   */
  USHORT desc_offset = 0;
  USHORT sdu_len = 0;
  UBYTE first = SEG_POS_FIRST;
  UBYTE segment_number = 0;
  T_desc_list* desc_list = &sn_unitdata_req->desc_list;

  TRACE_FUNCTION( "cia_su_cia_comp_req" );

  while (!ready && desc_list->first != NULL) {
   /*
    * How long will sdu be?
    */
    if (desc_list->list_len + header_size >= sndcp_data->su->n201_u) {
      sdu_len = (USHORT)(sndcp_data->su->n201_u << 3);
    } else {
      sdu_len = (USHORT)((desc_list->list_len + header_size) << 3);
    }
    {
      PALLOC_SDU(cia_comp_ind, CCI_COMP_IND, sdu_len);
      /*
       * Set parameters.
       */

      cia_comp_ind->sapi = sapi;
      /*
       * cia_qos is not yet used, set to 0.
       */
      cia_comp_ind->cia_qos.delay = 0;
      cia_comp_ind->cia_qos.relclass = 0;
      cia_comp_ind->cia_qos.peak = 0;
      cia_comp_ind->cia_qos.preced = 0;
      cia_comp_ind->cia_qos.mean = 0;
      /*
       * Will be changed as soon as more that 1 instance of V42.bis is used.
       */
      cia_comp_ind->algo_type = CIA_ALGO_V42;
      cia_comp_ind->comp_inst = CIA_COMP_INST_V42_0;

      cia_comp_ind->pdu_ref.ref_nsapi = nsapi;
      cia_comp_ind->pdu_ref.ref_npdu_num = npdu_number;
      cia_comp_ind->pdu_ref.ref_seg_num = segment_number;
      segment_number ++;
      if (desc_list->list_len + header_size <= sndcp_data->su->n201_u) {
        cia_comp_ind->seg_pos = first + SEG_POS_LAST;
      } else {
        cia_comp_ind->seg_pos = first;
      }
      cia_comp_ind->packet_type = packet_type;
      /*
       * Copy descriptors to cia_comp_ind->sdu.
       */
      while (desc_list->first != NULL) {
        T_desc* help = (T_desc*)desc_list->first;
        USHORT cur_len = (((T_desc*)desc_list->first)->len - desc_offset);
        if ((cur_len << 3) <=
             cia_comp_ind->sdu.l_buf +
             cia_comp_ind->sdu.o_buf -
             bit_offset) {

          /*
           * Copy current desc to sdu.
           */
          if (cur_len>0)
          {
            memcpy(&cia_comp_ind->sdu.buf[bit_offset >> 3],
                   &((T_desc*)desc_list->first)->buffer[desc_offset],
                   cur_len);
          }
          bit_offset += (USHORT)(cur_len << 3);
          desc_list->list_len -= cur_len;

          /*
           * Free read desc and go to next in list.
           */
          desc_list->first = help->next;
          desc_offset = 0;
          MFREE(help);
          help = NULL;
        } else {
          /*
           * Current desc does not fit completely in sdu.
           */
          USHORT part_len = (USHORT)(cia_comp_ind->sdu.l_buf +
                                    cia_comp_ind->sdu.o_buf -
                                    bit_offset)
                            >> 3;
          if (part_len > 0)
          {
            memcpy(&cia_comp_ind->sdu.buf[bit_offset >> 3],
                   &((T_desc*)desc_list->first)->buffer[desc_offset],
                   part_len);
            desc_offset += part_len;
          }
          desc_list->list_len -= part_len;

          header_size = SN_UNITDATA_PDP_HDR_LEN_SMALL;
          bit_offset = ENCODE_OFFSET + (USHORT)(header_size << 3);

          break;
        }
      } /* while (desc_list->first != NULL) { */
      /*
       * Instead of PSEND(SNDCP_handle, cia_comp_ind);
       */
      cia_cia_comp_ind(cia_comp_ind);
      /*
       * One segment sent, set 'first' to 'none'.
       */
      first = SEG_POS_NONE;
    }
    /*
     * If the desc_list is empty now, leave.
     */

    if (sn_unitdata_req->desc_list.list_len == 0) {
      PFREE(sn_unitdata_req);
      sn_unitdata_req = NULL;
      ready = TRUE;
    }
  }

} /* cia_su_cia_comp_req() */
/*#endif */ /* CF_FAST_EXEC || _SIMULATION_ || !REL99 || SNDCP_2to1 */

#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : cia_su_cia_comp_req
+------------------------------------------------------------------------------
| Description : This function is used, if no data compression is used in
|               uplink. It does only the segmentation of the incoming data.
| 
|               Simulation for cia reaction to SIG_SU_CIA_DATA_REQ.
|               Instead of sending the pdu to cia and then receiving
|               1 or more CIA_COMP_IND.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   !defined(REL99)   || defined(SNDCP_2to1) */

LOCAL void cia_su_cia_comp_req (T_SN_UNITDATA_REQ* sn_unitdata_req,
                                USHORT npdu_number,
                                UBYTE nsapi,
                                UBYTE sapi,
                                UBYTE packet_type
                                )
{
  BOOL ready = FALSE;
  U16 header_size = SN_UNITDATA_PDP_HDR_LEN_BIG;
  U16 desc2_data_struct_offset = 0;
  U8 first = SEG_POS_FIRST;
  U8 segment_number = 0;
  U32* segment_header;
  T_desc_list2* desc_list2 = &sn_unitdata_req->desc_list2;
  T_desc_list3 desc_list3;
  T_desc3* desc3;
  T_desc3* current_desc3;
  /*
   * Offset in current desc. Set the desc_offset to the offset of 
   * the first desc in the list that is to be read.
   */
  U16 desc_offset = ((T_desc2*)desc_list2->first)->offset;

  TRACE_FUNCTION( "cia_su_cia_comp_req" );

  while (!ready && desc_list2->first != NULL)
  {
   /*
    * How long will the segment be?
    */
    desc_list3.first = NULL;
    desc_list3.list_len = 0;
    if ((desc_list2->list_len + header_size) >= sndcp_data->su->n201_u)
    {
      desc_list3.list_len = (USHORT)sndcp_data->su->n201_u;
    }
    else
    {
      desc_list3.list_len = (USHORT)(desc_list2->list_len + header_size);
    }

    {
      T_CIA_COMP_IND *cia_comp_ind;
      MALLOC(cia_comp_ind, sizeof(T_CIA_COMP_IND));
      /*
       * Set parameters.
       */

      cia_comp_ind->sapi = sapi;
      /*
       * cia_qos is not yet used, set to 0.
       */
      cia_comp_ind->cia_qos.delay = 0;
      cia_comp_ind->cia_qos.relclass = 0;
      cia_comp_ind->cia_qos.peak = 0;
      cia_comp_ind->cia_qos.preced = 0;
      cia_comp_ind->cia_qos.mean = 0;
      /*
       * Will be changed as soon as more that 1 instance of V42.bis is used.
       */
      cia_comp_ind->algo_type = CIA_ALGO_V42;
      cia_comp_ind->comp_inst = CIA_COMP_INST_V42_0;

      cia_comp_ind->pdu_ref.ref_nsapi = nsapi;
      cia_comp_ind->pdu_ref.ref_npdu_num = npdu_number;
      cia_comp_ind->pdu_ref.ref_seg_num = segment_number;
      segment_number ++;

      /* Allocate memory for SNDCP header and LLC encode offset */
      MALLOC (segment_header, (USHORT)(ENCODE_OFFSET_BYTE + header_size));

      MALLOC (desc3, (U16)(sizeof(T_desc3)));
      /*
       * Fill desc3 descriptor control information.
       */
      desc3->next  = (U32)NULL;
      desc3->len   = header_size;
      desc3->offset = ENCODE_OFFSET_BYTE;
      desc3->buffer = (U32)segment_header;
      desc_list3.first = (U32)desc3;
      current_desc3 = desc3;

      if (desc_list2->list_len + header_size <= sndcp_data->su->n201_u)
      {
        cia_comp_ind->seg_pos = first + SEG_POS_LAST;
      } else {
        cia_comp_ind->seg_pos = first;
      }
      cia_comp_ind->packet_type = packet_type;
      cia_comp_ind->desc_list3.first = desc_list3.first;
      cia_comp_ind->desc_list3.list_len = desc_list3.list_len;

      desc_list3.list_len = 0;
      desc_list3.list_len += desc3->len;
      /*
       * Copy descriptors to cia_comp_ind->desc_list3, list may have to be built.
       */
      while (desc_list2->first != NULL)
      {/* This while moves data from desc2 to segment defined by desc3 descriptors*/
        T_desc2* help = (T_desc2*)desc_list2->first;
        U16 cur_len = (((T_desc2*)desc_list2->first)->len - desc_offset);
        desc2_data_struct_offset = offsetof(T_desc2, buffer);
        if (cur_len <= (sndcp_data->su->n201_u - desc_list3.list_len))
        {/* Is there room for all the desc2 data in the current segment */

          /*
           * describe current desc2 by desc3 descriptors.
           */

          MALLOC (desc3, (USHORT)(sizeof(T_desc3)));
          /*
           * Fill desc3 descriptor control information.
           */
          desc3->next  = (U32)NULL;
          desc3->len   = cur_len;
          desc3->offset = desc_offset + desc2_data_struct_offset;
          desc3->buffer = (U32)desc_list2->first;
          current_desc3->next = (U32)desc3;
          current_desc3 = desc3;
          desc_list3.list_len += desc3->len;

          /* Attach desc3 to desc2 allocation, this is always the last
             attach on a desc2 descriptor */
          sndcp_cl_desc2_attach(help);

          desc_list2->list_len -= cur_len;

          /*
           * Free read desc and go to next in list.
           */
          desc_list2->first = help->next;
          /* 
           * If another desc is present in the list to be read, then 
           * set the desc_offset to the offset of the next desc in the 
           * list that is to be read. Else set desc_offset to zero
           */
          if (desc_list2->first != NULL)
          {
            desc_offset = ((T_desc2*)desc_list2->first)->offset;
          }
          else
          {
            desc_offset = 0;
          }
          MFREE(help);
          help = NULL;
        } else {
          /*
           * Current desc does not fit completely in sdu.
           */

          USHORT part_len = (USHORT)(sndcp_data->su->n201_u - desc_list3.list_len);

          if (part_len > 0)
          {
            MALLOC (desc3, (USHORT)(sizeof(T_desc3)));
            /*
             * Fill desc3 descriptor control information.
             */
            desc3->next  = (ULONG)NULL;
            desc3->len   = part_len;
            desc3->offset = desc_offset + desc2_data_struct_offset;
            desc3->buffer = (ULONG)desc_list2->first;
            current_desc3->next = (ULONG)desc3;
            current_desc3 = desc3;
            desc_list3.list_len += desc3->len;

            /* Attach desc3 to desc2 allocation, this is an intermediate
               attach on a desc2 descriptor */
            sndcp_cl_desc2_attach(help);

            desc_offset += part_len;
          }
          desc_list2->list_len -= part_len;

          header_size = SN_UNITDATA_PDP_HDR_LEN_SMALL;
          break;
        }
      } /* while (desc_list->first != NULL) { */

      /*
       * Instead of PSEND(SNDCP_handle, cia_comp_ind);
       */
      cia_cia_comp_ind(cia_comp_ind);
      /*
       * One segment sent, set 'first' to 'none'.
       */
      first = SEG_POS_NONE;
    }
    /*
     * If the desc_list is empty now, leave.
     */
    if (sn_unitdata_req->desc_list2.list_len == 0) {

      MFREE(sn_unitdata_req);
      sn_unitdata_req = NULL;
      ready = TRUE;
    }
  }
} /* cia_su_cia_comp_req() */
/*#endif */ /* CF_FAST_EXEC || _SIMULATION_ || !REL99 || SNDCP_2to1 */

#ifdef TI_PS_FF_V42BIS
/*
+------------------------------------------------------------------------------
| Function    : cia_su_cci_comp_req
+------------------------------------------------------------------------------
| Description : This function is used, if data compression is used in uplink
|               direction. At first it calls the data compression function with
|               parameters, that allow a segmentation without memcopy. Than it
|               does the segmentation.
|
|               Simulation for CCI reaction to SIG_SU_CIA_DATA_REQ.
|               Instead of sending the pdu to CCI and then receiving
|               1 or more CCI_COMP_IND.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   !defined(REL99)   || defined(SNDCP_2to1) */
   
LOCAL void cia_su_cci_comp_req (T_SN_UNITDATA_REQ* sn_unitdata_req,
                                          USHORT npdu_number,
                                          UBYTE nsapi,
                                          UBYTE sapi,
                                          UBYTE packet_type
                                        )
{
  BOOL ready = FALSE;
  U16 header_size = SN_UNITDATA_PDP_HDR_LEN_BIG;
  U16 desc2_data_struct_offset = offsetof(T_desc2, buffer);
  U8 segment_position = SEG_POS_FIRST;
  U8 segment_number = 0;
  U32* segment_header;
  T_desc_list2* desc_list2 = &sn_unitdata_req->desc_list2;
  T_desc2* desc2;
  T_desc_list3 desc_list3;
  T_desc3* desc3;
  T_desc3* current_desc3;
  
  U8 descs_per_seg = 5;
  U8 i;
  U16 desc_size = (sndcp_data->su->n201_u - header_size -
                  (desc2_data_struct_offset + 1) * descs_per_seg) /
                  descs_per_seg +
                  desc2_data_struct_offset + 1;
  
  
  
  
  
  
  TRACE_FUNCTION( "cia_su_cci_comp_req" );
  TRACE_EVENT_P1("sn_unitdata_req in: %08x", sn_unitdata_req);
  //TRACE_EVENT_P1("PacketType: %d",packet_type);

  /*
   * Reset V.42 context and call the encoder routine
   */
  //TRACE_EVENT_P1("V42 ENC: Input Uncompressed Packet, length %d", desc_list2->list_len);
#ifdef SNDCP_TRACE_BUFFER
  //sndcp_trace_desc_list(desc_list2);
#endif
  v42b_init(sndcp_data->cia.enc, 0, 0, 0, 0);
  TRACE_FUNCTION ("as reinit in uplink");
  /*
   * the function can be called with 0s as parameters, because it was initialized
   * befor what the function sees on the valid magic number
   *
   * the call replaces an independent reinit function
   */
  v42b_encoder(sndcp_data->cia.enc, desc_list2, NULL, desc_size);
  //TRACE_EVENT_P1("V42 ENC: Output Compresset Packet, length %d", desc_list2->list_len);
#ifdef SNDCP_TRACE_BUFFER
  //sndcp_trace_desc_list(desc_list2);
#endif


  desc2 = (T_desc2*)desc_list2->first;

  while (desc2)
  {
    
    
    
    desc_list3.first = (U32) NULL;
    desc_list3.list_len = 0;
    

    MALLOC(segment_header, (ENCODE_OFFSET_BYTE + header_size));
    MALLOC(desc3,sizeof(T_desc3));

    desc3->next = (U32) NULL;
    desc3->len = header_size;
    desc3->offset = ENCODE_OFFSET_BYTE;
    desc3->buffer = (U32)segment_header;

    desc_list3.first = (U32)desc3;
    desc_list3.list_len = desc3->len;

    current_desc3 = desc3;

    for (i = 0; (i < descs_per_seg) && desc2; i++)
    {
      MALLOC(desc3,sizeof(T_desc3));

      desc3->next = (U32) NULL;
      desc3->len = desc2->len;
      desc3->offset = desc2->offset + desc2_data_struct_offset;
      desc3->buffer = (U32)desc2;

      current_desc3->next = (U32) desc3;
      current_desc3 = desc3;
      desc_list3.list_len += desc3->len;

      desc2 = (T_desc2*) desc2->next;
    }
    header_size = SN_UNITDATA_PDP_HDR_LEN_SMALL;

    {
      T_CIA_COMP_IND *cia_comp_ind;
      MALLOC(cia_comp_ind, sizeof(T_CIA_COMP_IND));
      /*
       * Set parameters.
       */

      cia_comp_ind->sapi = sapi;
      /*
       * cci_qos is not yet used, set to 0.
       */
      cia_comp_ind->cia_qos.delay = 0;
      cia_comp_ind->cia_qos.relclass = 0;
      cia_comp_ind->cia_qos.peak = 0;
      cia_comp_ind->cia_qos.preced = 0;
      cia_comp_ind->cia_qos.mean = 0;
      /*
       * Will be changed as soon as more that 1 instance of V42.bis is used.
       */
      cia_comp_ind->algo_type = CCI_ALGO_V42;
      cia_comp_ind->comp_inst = CCI_COMP_INST_V42_0;

      cia_comp_ind->pdu_ref.ref_nsapi = nsapi;
      cia_comp_ind->pdu_ref.ref_npdu_num = npdu_number;
      cia_comp_ind->pdu_ref.ref_seg_num = segment_number;
      segment_number ++;

      if (desc2 != NULL)
      {
        cia_comp_ind->seg_pos = segment_position;
      }
      else
      {
        cia_comp_ind->seg_pos = segment_position | SEG_POS_LAST;
      }

      cia_comp_ind->packet_type = packet_type;
      cia_comp_ind->desc_list3.first = desc_list3.first;
      cia_comp_ind->desc_list3.list_len = desc_list3.list_len;

      /*
       * Instead of PSEND(SNDCP_handle, cci_comp_ind);
       */
      cia_cia_comp_ind(cia_comp_ind);
      /*
       * One segment sent, set 'first' to 'none'.
       */
      segment_position = SEG_POS_NONE;
    }
  }
  /*
   * If the desc_list2 is empty now, leave.
   */
  TRACE_EVENT_P1("sn_unitdata_req out: %08x", sn_unitdata_req);
  MFREE(sn_unitdata_req);
  sn_unitdata_req = NULL;
}
/*#endif */ /* FAST_EXEC */
#endif /* TI_PS_FF_V42BIS */
#endif /* _SNDCP_DTI_2_ */

#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : cia_sua_cia_comp_req
+------------------------------------------------------------------------------
| Description : Simulation for cia reaction to SIG_SUA_CIA_DATA_REQ.
|               Instead of sending the pdu to cia and then receiving
|               1 or more CIA_COMP_IND.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

LOCAL void cia_sua_cia_comp_req (T_desc_list2 dest_desc_list,
                                 U8 npdu_number,
                                 U8 nsapi,
                                 U8 sapi,
                                 U8 packet_type
                                 )
{
  BOOL ready = FALSE;
  U16 header_size = SN_DATA_PDP_HDR_LEN_BIG;
  U16 desc2_data_struct_offset = 0;
  U8 first = SEG_POS_FIRST;
  U8 segment_number = 0;
  U32* segment_header;
  T_desc_list2* desc_list2 = &dest_desc_list;
  T_desc_list3 desc_list3;
  T_desc3* desc3;
  T_desc3* current_desc3;
  USHORT list_len = desc_list2->list_len;
  T_desc2* help = (T_desc2*)desc_list2->first;
  T_desc2* delhelp = NULL;

  /* 
   * Offset in current descriptor  Set the desc_offset to the offset of 
   * the first desc in the list that is to be read.
   */
  U16 desc_offset = help->offset;

  TRACE_FUNCTION( "cia_sua_cia_comp_req" );

  while (!ready && help != NULL) {
   /*
    * How long will the segment be?
    */
    desc_list3.first = NULL;
    desc_list3.list_len = 0;
    if ((list_len + header_size) >= sndcp_data->sua->n201_i)
    {
      desc_list3.list_len = (U16)sndcp_data->sua->n201_i;
    }
    else
    {
      desc_list3.list_len = (U16)(list_len + header_size);
    }

    {
      /*
       * First desc in list must be header!!!
       */
      BOOL first_part = TRUE;

      T_CIA_COMP_IND *cia_comp_ind;
      MALLOC(cia_comp_ind, sizeof(T_CIA_COMP_IND));
      /*
       * Set parameters.
       */
      cia_comp_ind->sapi = sapi;
      /*
       * cia_qos is not yet used, set to 0.
       */
      cia_comp_ind->cia_qos.delay = 0;
      cia_comp_ind->cia_qos.relclass = 0;
      cia_comp_ind->cia_qos.peak = 0;
      cia_comp_ind->cia_qos.preced = 0;
      cia_comp_ind->cia_qos.mean = 0;
      /*
       * Will be changed as soon as more that 1 instance of V42.bis is used.
       */
      cia_comp_ind->algo_type = CIA_ALGO_V42;
      cia_comp_ind->comp_inst = CIA_COMP_INST_V42_0;

      cia_comp_ind->pdu_ref.ref_nsapi = nsapi;
      cia_comp_ind->pdu_ref.ref_npdu_num = npdu_number;
      cia_comp_ind->pdu_ref.ref_seg_num = segment_number;
      segment_number ++;

      /* Allocate memory for SNDCP header and LLC encode offset */
      MALLOC (segment_header, (U16)(ENCODE_OFFSET_BYTE + header_size));

      MALLOC (desc3, (U16)(sizeof(T_desc3)));
      /*
       * Fill desc3 descriptor control information.
       */
      desc3->next  = (U32)NULL;
      desc3->len   = header_size;
      desc3->offset = ENCODE_OFFSET_BYTE;
      desc3->buffer = (U32)segment_header;
      desc_list3.first = (U32)desc3;
      current_desc3 = desc3;

      if (list_len + header_size <= sndcp_data->sua->n201_i)
      {
        cia_comp_ind->seg_pos = first + SEG_POS_LAST;
      } else {
        cia_comp_ind->seg_pos = first;
      }
      cia_comp_ind->packet_type = packet_type;
      cia_comp_ind->desc_list3.first = desc_list3.first;
      cia_comp_ind->desc_list3.list_len = desc_list3.list_len;

      desc_list3.list_len = 0;
      desc_list3.list_len += desc3->len;
      /*
       * Copy descriptors to cia_comp_ind->desc_list3, list may have to be built.
       */
      while (help != NULL)
      {/*
        * This while moves data from desc2 to segment defined
        * by desc3 descriptors
        */
        U16 cur_len = (help->len - desc_offset);
        desc2_data_struct_offset = offsetof(T_desc2, buffer);
        if (cur_len <= (sndcp_data->sua->n201_i - desc_list3.list_len))
        {/* Is there room for all the desc2 data in the current segment */
          /*
           * describe current desc2 by desc3 descriptors.
           */
          if (cur_len>0)
          {
            MALLOC (desc3, (U16)(sizeof(T_desc3)));
            /*
             * Fill desc3 descriptor control information.
             */
            desc3->next  = (U32)NULL;
            desc3->len   = cur_len;
            desc3->offset = desc_offset + desc2_data_struct_offset;
            desc3->buffer = (U32)help;
            current_desc3->next = (ULONG)desc3;
            current_desc3 = desc3;
            desc_list3.list_len += desc3->len;

            /* Attach desc3 to desc2 allocation, this is always the last
               attach on a desc2 descriptor */
            sndcp_cl_desc2_attach(help);

          }
          list_len -= cur_len;

          /*
           * Free read desc and go to next in list.
           */
          if (help != NULL)
          {
            delhelp = help;
            help = (T_desc2*)help->next;
          }
          /* 
           * If another desc is present in the list to be read, then 
           * set the desc_offset to the offset of the next desc in the 
           * list that is to be read. Else set desc_offset to zero
           */
          if (help != NULL)
          {
            desc_offset = help->offset;
          }
          else
          {
            desc_offset = 0;
          }
          if (delhelp != NULL &&
              first_part &&
              (packet_type == TYPE_COMPRESSED_TCP ||
               packet_type == TYPE_UNCOMPRESSED_TCP)) {

            MFREE(delhelp);
            delhelp = NULL;
            first_part = FALSE;
          }
        }
        else
        {
          /*
           * Current desc does not fit completely in sdu.
           */

          U16 part_len = (USHORT)(sndcp_data->sua->n201_i-desc_list3.list_len);

          if (part_len>0)
          {
            MALLOC (desc3, (USHORT)(sizeof(T_desc3)));
            /*
             * Fill desc3 descriptor control information.
             */
            desc3->next  = (ULONG)NULL;
            desc3->len   = part_len;
            desc3->offset = desc_offset + desc2_data_struct_offset;
            desc3->buffer = (ULONG)help;
            current_desc3->next = (ULONG)desc3;
            current_desc3 = desc3;
            desc_list3.list_len += desc3->len;

            /* Attach desc3 to desc2 allocation, this is an intermediate
               attach on a desc2 descriptor */
            sndcp_cl_desc2_attach(help);

            desc_offset += part_len;
          }
          list_len -= part_len;

          header_size = SN_DATA_PDP_HDR_LEN_SMALL;
          break;
        }
      }
      /*
       * Instead of PSEND(SNDCP_handle, cia_comp_ind);
       */
      cia_cia_comp_ind(cia_comp_ind);
      /*
       * One segment sent, set 'first' to 'none'.
       */
      first = SEG_POS_NONE;
    }
    /*
     * If the desc_list is empty now, leave.
     */
    if (list_len == 0) {
      ready = TRUE;
    }
  }
} /* cia_sua_cia_comp_req() */

#endif /* CF_FAST_EXEC */

#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : cia_sua_cia_comp_req
+------------------------------------------------------------------------------
| Description : Simulation for cia reaction to SIG_SUA_CIA_DATA_REQ.
|               Instead of sending the pdu to cia and then receiving
|               1 or more CIA_COMP_IND.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

LOCAL void cia_sua_cia_comp_req (T_desc_list dest_desc_list,
                                 UBYTE npdu_number,
                                 UBYTE nsapi,
                                 UBYTE sapi,
                                 UBYTE packet_type
                                 )
{
  BOOL ready = FALSE;
  USHORT header_size = SN_DATA_PDP_HDR_LEN_BIG;
  USHORT bit_offset = ENCODE_OFFSET + (USHORT)(header_size << 3);
  USHORT desc_offset = 0;
  USHORT sdu_len = 0;
  UBYTE first = SEG_POS_FIRST;
  UBYTE segment_number = 0;
  T_desc_list* desc_list = &dest_desc_list;
  USHORT list_len = desc_list->list_len;
  T_desc* help = (T_desc*)desc_list->first;
  T_desc* delhelp = NULL;

  USHORT help_buffer_offset = 0;

  TRACE_FUNCTION( "cia_sua_cia_comp_req" );

  while (!ready && help != NULL) {
   /*
    * How long will sdu be?
    */
    if (list_len + header_size >= sndcp_data->sua->n201_i) {
      sdu_len = (USHORT)(sndcp_data->sua->n201_i << 3);
    } else {
      sdu_len = (USHORT)((list_len + header_size) << 3);
    }
    {
      /*
       * First desc in list must be header!!!
       */
      BOOL first_part = TRUE;

      PALLOC_SDU(cia_comp_ind, CCI_COMP_IND, sdu_len);   
      /*
       * Set parameters.
       */
      cia_comp_ind->sapi = sapi;
      /*
       * cia_qos is not yet used, set to 0.
       */
      cia_comp_ind->cia_qos.delay = 0;
      cia_comp_ind->cia_qos.relclass = 0;
      cia_comp_ind->cia_qos.peak = 0;
      cia_comp_ind->cia_qos.preced = 0;
      cia_comp_ind->cia_qos.mean = 0;
      /*
       * Will be changed as soon as more that 1 instance of V42.bis is used.
       */
      cia_comp_ind->algo_type = CIA_ALGO_V42;
      cia_comp_ind->comp_inst = CIA_COMP_INST_V42_0;

      cia_comp_ind->pdu_ref.ref_nsapi = nsapi;
      cia_comp_ind->pdu_ref.ref_npdu_num = npdu_number;
      cia_comp_ind->pdu_ref.ref_seg_num = segment_number;
      segment_number ++;
      if (list_len + header_size <= sndcp_data->sua->n201_i) {
        cia_comp_ind->seg_pos = first + SEG_POS_LAST;
      } else {
        cia_comp_ind->seg_pos = first;
      }
      cia_comp_ind->packet_type = packet_type;
      /*
       * Copy descriptors to cia_comp_ind->sdu.
       */
      while (help != NULL) {
        USHORT cur_len = (help->len - desc_offset);

        if ((cur_len << 3) <=
             cia_comp_ind->sdu.l_buf +
             cia_comp_ind->sdu.o_buf -
             bit_offset) {

          /*
           * Copy current desc to sdu.
           */
          if (cur_len>0)
          {
            memcpy(&cia_comp_ind->sdu.buf[bit_offset >> 3],
                   &help->buffer[desc_offset],
                   cur_len);
          }
          bit_offset += (USHORT)(cur_len << 3);
          list_len -= cur_len;

          /*
           * Free read desc and go to next in list.
           */
          if (help != NULL) {
            delhelp = help;
            help = (T_desc*)help->next;
          }
          help_buffer_offset = 0;
          desc_offset = 0;
          if (delhelp != NULL &&
              first_part &&
              (packet_type == TYPE_COMPRESSED_TCP ||
               packet_type == TYPE_UNCOMPRESSED_TCP)) {

            MFREE(delhelp);
            delhelp = NULL;
            first_part = FALSE;
          }
        } else {
          /*
           * Current desc does not fit completely in sdu.
           */
          desc_offset = (USHORT)(cia_comp_ind->sdu.l_buf +
                                 cia_comp_ind->sdu.o_buf -
                                 bit_offset)
                         >> 3;
          if (desc_offset>0)
          {
            memcpy(&cia_comp_ind->sdu.buf[bit_offset >> 3],
                   &help->buffer[help_buffer_offset],
                   desc_offset);
            help_buffer_offset += (desc_offset);
          }
          list_len -= desc_offset;

          header_size = SN_DATA_PDP_HDR_LEN_SMALL;
          bit_offset = ENCODE_OFFSET + (USHORT)(header_size << 3);
          break;
        }
      }
      /*
       * Instead of PSEND(SNDCP_handle, cia_comp_ind);
       */
      cia_cia_comp_ind(cia_comp_ind);
      /*
       * One segment sent, set 'first' to 'none'.
       */
      first = SEG_POS_NONE;
    }
    /*
     * If the desc_list is empty now, leave.
     */
    if (list_len == 0) {
      ready = TRUE;
    }
  }

} /* cia_sua_cia_comp_req() */

#endif /* CF_FAST_EXEC */

#endif /*_SNDCP_DTI_2_*/


/*
+------------------------------------------------------------------------------
| Function    : cia_sd_cia_decomp_req
+------------------------------------------------------------------------------
| Description : This Function does the defragmentation for noncompressed data.
| 
| Simulation for cia reaction to SIG_SD_CIA_TRANSFER_REQ.
| If (cur_seg_pos & SEG_POS_FIRST > 0) then a new CIA_DECOMP_IND is allocated.
| T_desc is allocated with the length of the sdu included in the
| ll_unitdata_ind. The sdu data is copied to the desc and the desc is added to
| the currently assembled desc_list in the current CIA_DECOMP_IND.
| If (cur_seg_pos & SEG_POS_LAST > 0) then a the CIA_DECOMP_IND is now complete
| and is "sent to this service" by calling the cia function cia_cia_decomp_ind.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   !defined(REL99)   || defined(SNDCP_2to1) */

GLOBAL void cia_sd_cia_decomp_req(T_LL_UNITDATA_IND* ll_unitdata_ind)
{
  USHORT length = 0;
  USHORT header_len = SN_UNITDATA_PDP_HDR_LEN_SMALL;
  USHORT sdu_index = 0;
  T_CIA_DECOMP_IND *cur_cia_decomp_ind;
  UBYTE nsapi;
#ifdef _SNDCP_DTI_2_
  T_desc2* help = NULL;
  T_desc2* descriptor = NULL;
#else /*_SNDCP_DTI_2_*/
  T_desc* help = NULL;
  T_desc* descriptor = NULL;
#endif /*_SNDCP_DTI_2_*/

  TRACE_FUNCTION( "cia_sd_cia_decomp_req" );

  nsapi = (ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf / 8)]) & 0xf;
  cur_cia_decomp_ind = sndcp_data->cia.cur_cia_decomp_ind[nsapi];
  
  /*
   * In case of first segment allocate new N-PDU.
   */
  if ((sndcp_data->cur_seg_pos[nsapi] & SEG_POS_FIRST) > 0) {
    {

      T_CIA_DECOMP_IND *cia_decomp_ind;
      MALLOC(cia_decomp_ind, sizeof(T_CIA_DECOMP_IND));

#ifdef SNDCP_TRACE_ALL

      sndcp_data->cia.cia_decomp_ind_number[nsapi] ++;
      TRACE_EVENT_P1("number of cia_decomp_ind: % d",
                     sndcp_data->cia.cia_decomp_ind_number[nsapi]);
#endif /* SNDCP_TRACE_ALL */


      /*
       * if there is an unfinished cur_cia_decomp_ind deallocate it together
       * with the allocated descriptors
       */
      if (cur_cia_decomp_ind NEQ NULL)
      {
        MFREE_PRIM(cur_cia_decomp_ind);
        TRACE_EVENT("Deallocate unfinished cur_cia_decomp_ind");
      }
      cur_cia_decomp_ind = cia_decomp_ind;
      /*
       * Will be changed as soon as more that 1 instance of V42.bis is used.
       */
      cur_cia_decomp_ind->algo_type = CIA_ALGO_V42;
      cur_cia_decomp_ind->comp_inst = CIA_COMP_INST_V42_0;

      cur_cia_decomp_ind->pdu_ref = sndcp_data->cur_pdu_ref[nsapi];
      cur_cia_decomp_ind->desc_list2.first = (ULONG)NULL;
      cur_cia_decomp_ind->desc_list2.list_len = 0;

      header_len = SN_UNITDATA_PDP_HDR_LEN_BIG;
      cia_decomp_ind->pcomp =
        ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf >> 3) + 1] & 0xf;
    }
  }
  /*
   * If big header has been received in state != RECEIVE_FIRST_SEGMENT
   */
  if (sndcp_data->big_head[nsapi]) {
    header_len = SN_UNITDATA_PDP_HDR_LEN_BIG;
  }
  sdu_index = (ll_unitdata_ind->sdu.o_buf >> 3) + header_len;
  length = (ll_unitdata_ind->sdu.l_buf >> 3) - header_len;
  /*
   * Allocate new descriptor and copy sdu data.
   */
#ifdef _SNDCP_DTI_2_
  MALLOC (descriptor, (USHORT)(sizeof(T_desc2) - 1 + length));
#else /*_SNDCP_DTI_2_*/
  MALLOC (descriptor, (USHORT)(sizeof(T_desc) - 1 + length));
#endif /*_SNDCP_DTI_2_*/

  /*
   * Fill descriptor control information.
   */
  descriptor->next  = (ULONG)NULL;
  descriptor->len   = length;
#ifdef _SNDCP_DTI_2_
  descriptor->offset = 0;
  descriptor->size = descriptor->len;
#endif

  /*
   * Add length of descriptor data to list length.
   */
  cur_cia_decomp_ind->desc_list2.list_len += length;

  /*
   * Copy user data from SDU to descriptor.
   */
  if (length>0)
  {
    memcpy (descriptor->buffer,
            &ll_unitdata_ind->sdu.buf[sdu_index],
            length);
  }

  /*
   * Add desc to desc_list.
   */
#ifdef _SNDCP_DTI_2_
  help = (T_desc2*)cur_cia_decomp_ind->desc_list2.first;
#else /*_SNDCP_DTI_2_*/
  help = (T_desc*)cur_cia_decomp_ind->desc_list2.first;
#endif /*_SNDCP_DTI_2_*/
  if (help == NULL) {
    cur_cia_decomp_ind->desc_list2.first = (ULONG)descriptor;
  } else {
    if (help->next == NULL) {
      help->next = (ULONG) descriptor;
    } else {
      while (help->next != NULL) {
#ifdef _SNDCP_DTI_2_
        help = (T_desc2*)help->next;
#else /*_SNDCP_DTI_2_*/
        help = (T_desc*)help->next;
#endif /*_SNDCP_DTI_2_*/
      }
      help->next = (ULONG)descriptor;
    }
  }

  /*
   * If this is the last segment, send it to this same service with a simulated
   * primitive.
   */
  if ((sndcp_data->cur_seg_pos[nsapi] & SEG_POS_LAST) > 0) {
    /*
     * By now algo_type, cia_qos and comp_inst are not evaluated.
     */
    cur_cia_decomp_ind->pdu_ref = sndcp_data->cur_pdu_ref[nsapi];
    cia_cia_decomp_ind(cur_cia_decomp_ind);
    cur_cia_decomp_ind = NULL;
  } else {
    /*
     * Request next segment.
     */
    sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
  }
  /*
   * Update global current CIA_DECOMP_IND
   */
  sndcp_data->cia.cur_cia_decomp_ind[nsapi] = cur_cia_decomp_ind;
  /*
   * Free incoming prim.
   */
  if (ll_unitdata_ind != NULL) {
    PFREE (ll_unitdata_ind);
    ll_unitdata_ind = NULL;
  }
} /* cia_sd_cia_decomp_req() */

/*#endif */ /* CF_FAST_EXEC || _SIMULATION_ || !REL99 || SNDCP_2to1 */

#ifdef TI_PS_FF_V42BIS
/*
+------------------------------------------------------------------------------
| Function    : cia_sd_cci_decomp_req
+------------------------------------------------------------------------------
| Description : This function does the desegmentation and decomressoin for 
|               compressed data.
|
| Simulation for CCI reaction to SIG_SD_CIA_TRANSFER_REQ.
| If (cur_seg_pos & SEG_POS_FIRST > 0) then a new CCI_DECOMP_IND is allocated.
| T_desc is allocated with the length of the sdu included in the
| ll_unitdata_ind. The sdu data is copied to the desc and the desc is added to
| the currently assembled desc_list in the current CCI_DECOMP_IND.
| If (cur_seg_pos & SEG_POS_LAST > 0) then a the CCI_DECOMP_IND is now complete
| and is "sent to this service" by calling the cia function cia_cci_decomp_ind.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   !defined(REL99)   || defined(SNDCP_2to1) */

LOCAL void cia_sd_cci_decomp_req (/*T_pdu_ref pdu_ref,
                                  USHORT cur_seg_pos,
                                  */T_LL_UNITDATA_IND* ll_unitdata_ind
                                  )
{

  USHORT length = 0;
  USHORT header_len = SN_UNITDATA_PDP_HDR_LEN_SMALL;
  USHORT sdu_index = 0;
  T_CIA_DECOMP_IND *cur_cia_decomp_ind;
  UBYTE nsapi;
#ifdef _SNDCP_DTI_2_
  T_desc2* help = NULL;
  T_desc2* descriptor = NULL;
#else /*_SNDCP_DTI_2_*/
  T_desc* help = NULL;
  T_desc* descriptor = NULL;
#endif /*_SNDCP_DTI_2_*/

  TRACE_FUNCTION( "cia_sd_cci_decomp_req" );
  
  nsapi = (ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf / 8)]) & 0xf;
  cur_cia_decomp_ind = sndcp_data->cia.cur_cia_decomp_ind[nsapi];
  
  /*
   * In case of first segment allocate new N-PDU.
   */
  if ((sndcp_data->cur_seg_pos[nsapi] & SEG_POS_FIRST) > 0) 
  {
    {
      T_CIA_DECOMP_IND *cia_decomp_ind;
      MALLOC(cia_decomp_ind, sizeof(T_CIA_DECOMP_IND));

#ifdef SNDCP_TRACE_ALL

      sndcp_data->cia.cia_decomp_ind_number[nsapi] ++;
      TRACE_EVENT_P1("number of cia_decomp_ind: % d",
                     sndcp_data->cia.cia_decomp_ind_number[nsapi]);
#endif /* SNDCP_TRACE_ALL */


      /*
       * if there is an unfinished cur_cia_decomp_ind deallocate it together
       * with the allocated descriptors
       */
      if (cur_cia_decomp_ind NEQ NULL)
      {
        MFREE_PRIM(cur_cia_decomp_ind);
        TRACE_EVENT("Deallocate unfinished cur_cia_decomp_ind");
      }
      cur_cia_decomp_ind = cia_decomp_ind;
      /*
       * Will be changed as soon as more that 1 instance of V42.bis is used.
       */
      cur_cia_decomp_ind->algo_type = CIA_ALGO_V42;
      cur_cia_decomp_ind->comp_inst = CIA_COMP_INST_V42_0;

      cur_cia_decomp_ind->pdu_ref = sndcp_data->cur_pdu_ref[nsapi];
      cur_cia_decomp_ind->desc_list2.first = (ULONG)NULL;
      cur_cia_decomp_ind->desc_list2.list_len = 0;

      header_len = SN_UNITDATA_PDP_HDR_LEN_BIG;
      cia_decomp_ind->pcomp =
        ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf >> 3) + 1] & 0xf;
    }
  }
  /*
   * If big header has been received in state != RECEIVE_FIRST_SEGMENT
   */
  if (sndcp_data->big_head[nsapi]) {
    header_len = SN_UNITDATA_PDP_HDR_LEN_BIG;
  }
  sdu_index = (ll_unitdata_ind->sdu.o_buf >> 3) + header_len;
  length = (ll_unitdata_ind->sdu.l_buf >> 3) - header_len;
  /*
   * Allocate new descriptor and copy sdu data.
   */
#ifdef _SNDCP_DTI_2_
  MALLOC (descriptor, (USHORT)(sizeof(T_desc2) - 1 + length));
#else /*_SNDCP_DTI_2_*/
  MALLOC (descriptor, (USHORT)(sizeof(T_desc) - 1 + length));
#endif /*_SNDCP_DTI_2_*/

  /*
   * Fill descriptor control information.
   */
  descriptor->next  = (ULONG)NULL;
  descriptor->len   = length;
#ifdef _SNDCP_DTI_2_
  descriptor->offset = 0;
  descriptor->size = descriptor->len;
#endif

  /*
   * Add length of descriptor data to list length.
   */
  cur_cia_decomp_ind->desc_list2.list_len += length;

  /*
   * Copy user data from SDU to descriptor.
   */
  if (length>0)
  {
    memcpy (descriptor->buffer,
            &ll_unitdata_ind->sdu.buf[sdu_index],
            length);
  }

  /*
   * Add desc to desc_list.
   */
#ifdef _SNDCP_DTI_2_
  help = (T_desc2*)cur_cia_decomp_ind->desc_list2.first;
#else /*_SNDCP_DTI_2_*/
  help = (T_desc*)cur_cia_decomp_ind->desc_list2.first;
#endif /*_SNDCP_DTI_2_*/
  if (help == NULL) {
    cur_cia_decomp_ind->desc_list2.first = (ULONG)descriptor;
  } else {
    if (help->next == NULL) {
      help->next = (ULONG) descriptor;
    } else {
      while (help->next != NULL) {
#ifdef _SNDCP_DTI_2_
        help = (T_desc2*)help->next;
#else /*_SNDCP_DTI_2_*/
        help = (T_desc*)help->next;
#endif /*_SNDCP_DTI_2_*/
      }
      help->next = (ULONG)descriptor;
    }
  }

  /*
   * If this is the last segment, send it to this same service with a simulated
   * primitive.
   */
  if ((sndcp_data->cur_seg_pos[nsapi] & SEG_POS_LAST) > 0) 
  {
    T_desc_list2 desc_list2;
    /*
     * By now algo_type, cci_qos and comp_inst are not evaluated.
     */
    cur_cia_decomp_ind->pdu_ref = sndcp_data->cur_pdu_ref[nsapi];

    desc_list2.first = cur_cia_decomp_ind->desc_list2.first;
    desc_list2.list_len = cur_cia_decomp_ind->desc_list2.list_len;

    /*
     * Reset V.42 context and call the decoder routine.
     */
    TRACE_EVENT_P1("V42 DEC: Input Compresset Packet, length %d", desc_list2.list_len);
#ifdef SNDCP_TRACE_BUFFER
    sndcp_trace_desc_list(&desc_list2);

    sndcp_data->cia.trabu[0] = 0;///////////////////////////////////////////////////////////////

#endif
    v42b_init(sndcp_data->cia.dec, 0, 0, 0, 0);
    TRACE_EVENT ("as reinit in downlink");
    /*
     * the function can be called with 0s as parameters, because it was initialized
     * befor what the function sees on the valid magic number
     *
     * the call replaces an independent reinit function
     */
    v42b_decoder(sndcp_data->cia.dec, &desc_list2, NULL, 100);
    if (!IS_ERROR(sndcp_data->cia.dec))
    {
      TRACE_EVENT_P1("V42 DEC: Output Decompressed Packet, length %d", desc_list2.list_len);
#ifdef SNDCP_TRACE_BUFFER
      sndcp_trace_desc_list(&desc_list2);
#endif

      cur_cia_decomp_ind->desc_list2.first = desc_list2.first;
      cur_cia_decomp_ind->desc_list2.list_len = desc_list2.list_len;
      
      cia_cia_decomp_ind(cur_cia_decomp_ind);
      cur_cia_decomp_ind = NULL;
    } 
    else 
    {
      MFREE_PRIM (cur_cia_decomp_ind);
      cur_cia_decomp_ind = NULL;
      TRACE_EVENT("Deallocate corrupted V.42 bis packet");

      /*
       * Request next segment.
       */
      sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
    }
  } 
  else 
  {
    /*
     * Request next segment.
     */
    sd_get_unitdata_if_nec(ll_unitdata_ind->sapi);
  }
  /*
   * Update global current CIA_DECOMP_IND
   */
   sndcp_data->cia.cur_cia_decomp_ind[nsapi] = cur_cia_decomp_ind;
  /*
   * Free incoming prim.
   */
  if (ll_unitdata_ind != NULL) {
    PFREE (ll_unitdata_ind);
    ll_unitdata_ind = NULL;
  }
} /* cia_sd_cci_decomp_req_sim() */

/* #endif *//* CF_FAST_EXEC || _SIMULATION_ || !REL99 || SNDCP_2to1 */
#endif /* TI_PS_FF_V42BIS */

/*
+------------------------------------------------------------------------------
| Function    : sig_sda_cia_cia_decomp_req
+------------------------------------------------------------------------------
| Description : Simulation for cia reaction to SIG_SDA_CIA_TRANSFER_REQ.
| If (cur_seg_pos & SEG_POS_FIRST > 0) then a new CIA_DECOMP_IND is allocated.
| T_desc is allocated with the length of the sdu included in the
| ll_data_ind. The sdu data is copied to the desc and the desc is added to
| the currently assembled desc_list in the current CCI_DECOMP_IND.
| If (cur_seg_pos & SEG_POS_LAST > 0) then a the CIA_DECOMP_IND is now complete
| and is "sent to this service" by calling the cia function cia_cia_decomp_ind.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_sda_cia_cia_decomp_req ( T_LL_DATA_IND* ll_data_ind )
{
  USHORT length = 0;
  USHORT header_len = SN_DATA_PDP_HDR_LEN_SMALL;
  USHORT sdu_index = 0;
  T_CIA_DECOMP_IND *cur_cia_decomp_ind;
  UBYTE nsapi = 0;
#ifdef _SNDCP_DTI_2_
  T_desc2* help = NULL;
  T_desc2* local_desc = NULL;
#else /*_SNDCP_DTI_2_*/
  T_desc* help = NULL;
  T_desc* local_desc = NULL;
#endif /*_SNDCP_DTI_2_*/

  TRACE_FUNCTION( "cia_sig_sda_cia_cia_decomp_req_sim" );

  nsapi = (ll_data_ind->sdu.buf[(ll_data_ind->sdu.o_buf / 8)]) & 0xf;
  cur_cia_decomp_ind = sndcp_data->cia.cur_cia_decomp_ind[nsapi];

  /*
   * In case of first segment allocate new N-PDU.
   */
  if ((sndcp_data->cur_seg_pos[nsapi] & SEG_POS_FIRST) > 0) {

    T_CIA_DECOMP_IND *cia_decomp_ind;
    MALLOC(cia_decomp_ind, sizeof(T_CIA_DECOMP_IND));

#ifdef SNDCP_TRACE_ALL
    sndcp_data->cia.cia_decomp_ind_number[nsapi] ++;
    TRACE_EVENT_P1("number of cia_decomp_ind: % d",
                   sndcp_data->cia.cia_decomp_ind_number[nsapi]);
#endif

    if (cur_cia_decomp_ind NEQ NULL)
    {
      MFREE_PRIM(cur_cia_decomp_ind);
      TRACE_EVENT("Deallocate unfinished cur_cia_decomp_ind");
    }

    cur_cia_decomp_ind = cia_decomp_ind;

    /*
     * Will be changed as soon as more that 1 instance of V42.bis is used.
     */
    cur_cia_decomp_ind->algo_type = CIA_ALGO_V42;
    cur_cia_decomp_ind->comp_inst = CIA_COMP_INST_V42_0;

    cur_cia_decomp_ind->pdu_ref = sndcp_data->cur_pdu_ref[nsapi];
    cur_cia_decomp_ind->desc_list2.first = (ULONG)NULL;
    cur_cia_decomp_ind->desc_list2.list_len = 0;

    header_len = SN_DATA_PDP_HDR_LEN_BIG;
    cia_decomp_ind->pcomp =
      ll_data_ind->sdu.buf[(ll_data_ind->sdu.o_buf >> 3) + 1] & 0xf;

  }
  /*
   * If big header has been received in state != RECEIVE_FIRST_SEGMENT
   */
  if (sndcp_data->big_head[nsapi]) {
    header_len = SN_DATA_PDP_HDR_LEN_BIG;
  }
  sdu_index = (ll_data_ind->sdu.o_buf >> 3) + header_len;
  
  if ((ll_data_ind->sdu.l_buf >> 3) < header_len){
     /*This condition is added as part of fix for GCF 46.1.2.2.3.2 failure because of wrong length*/
     TRACE_ERROR("SNDCP: Unexpected Length of N-PDU");
     PFREE (ll_data_ind);
     ll_data_ind = NULL;
     return;
  }else{

     length = (ll_data_ind->sdu.l_buf >> 3) - header_len;
  }

  /*
   * Allocate new descriptor and copy sdu data.
   */
#ifdef _SNDCP_DTI_2_
  MALLOC (local_desc, (USHORT)(sizeof(T_desc2) - 1 + length));
#else /*_SNDCP_DTI_2_*/
  MALLOC (local_desc, (USHORT)(sizeof(T_desc) - 1 + length));
#endif /*_SNDCP_DTI_2_*/

  /*
   * Fill descriptor control information.
   */
  local_desc->next  = (ULONG)NULL;
  local_desc->len   = length;
#ifdef _SNDCP_DTI_2_
  local_desc->offset = 0;
  local_desc->size = local_desc->len;
#endif
  /*
   * Add length of descriptor data to list length.
   */
  cur_cia_decomp_ind->desc_list2.list_len += length;

  /*
   * Copy user data from SDU to descriptor.
   */
  if (length>0)
  {
    memcpy (local_desc->buffer, &ll_data_ind->sdu.buf[sdu_index], length);
  }

  /*
   * Add desc to desc_list.
   */
#ifdef _SNDCP_DTI_2_
  help = (T_desc2*)cur_cia_decomp_ind->desc_list2.first;
#else /*_SNDCP_DTI_2_*/
  help = (T_desc*)cur_cia_decomp_ind->desc_list2.first;
#endif /*_SNDCP_DTI_2_*/
  if (help == NULL) {
    cur_cia_decomp_ind->desc_list2.first = (ULONG)local_desc;
  } else {
    if (help->next == NULL) {
      help->next = (ULONG) local_desc;
    } else {
      while (help->next != NULL) {
#ifdef _SNDCP_DTI_2_
        help = (T_desc2*)help->next;
#else /*_SNDCP_DTI_2_*/
        help = (T_desc*)help->next;
#endif /*_SNDCP_DTI_2_*/
      }
      help->next = (ULONG)local_desc;
    }
  }
  /*
   * If this is the last segment, send it to this same service with a simulated
   * primitive.
   */
  if ((sndcp_data->cur_seg_pos[nsapi] & SEG_POS_LAST) > 0) {
    /*
     * By now algo_type and comp_inst are not evaluated.
     */
    cur_cia_decomp_ind->pdu_ref = sndcp_data->cur_pdu_ref[nsapi];
    cia_cia_decomp_ind(cur_cia_decomp_ind);
    cur_cia_decomp_ind = NULL;
    /*
     * Reset the Current Segment Counter to zero, since we have received the 
     * last segment 
     */
    sndcp_data->cur_segment_number[nsapi] = 0;
  } else {
    /*
     * Check the Current Segment Number whether we have received more than 
     * SNDCP_MAX_SEGMENT_NUMBER segments or not. If we have received more than 
     * SNDCP_MAX_SEGMENT_NUMBER segments in a single NPDU, then we will discard
     * the stored segments and also the remaining segments of this NPDU, till 
     * we receive that last segment of this NPDU.
     * This has been done in order to encounter the PARTITION problem which
     * we will face if we keep on storing the segments in SNDCP.
     */
    sndcp_data->cur_segment_number[nsapi] ++;
    if (sndcp_data-> cur_segment_number[nsapi] > SNDCP_MAX_SEGMENT_NUMBER)
    {
      TRACE_EVENT("Segment Number in Single NPDU exceeds max segment number");

      /* Deleting the stored segments */
      sig_mg_cia_delete_npdus(nsapi);

      /* Change the state to SDA_ACK_DISCARD */
      sndcp_set_nsapi_rec_state(nsapi, SDA_ACK_DISCARD);
    }

    /*
     * Request next segment.
     */
    sda_get_data_if_nec(ll_data_ind->sapi);
  }
  /*
   * Update global current CIA_DECOMP_IND
   */
  sndcp_data->cia.cur_cia_decomp_ind[nsapi] = cur_cia_decomp_ind;
  /*
   * Free incoming prim.
   */
  if (ll_data_ind != NULL) {
    PFREE (ll_data_ind);
    ll_data_ind = NULL;
  }
} /* cia_sig_sda_cia_cia_decomp_req_sim() */

#endif /* CF_FAST_EXEC */


/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_cia_reset_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_CIA_RESET_IND
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_cia_reset_ind (void)
{
  TRACE_ISIG( "sig_mg_cia_reset_ind" );

  switch( GET_STATE(CIA) )
  {
    case CIA_DEFAULT:
      sndcp_reset_xid_block(&sndcp_data->cia.cur_xid_block);
      break;
    default:
      TRACE_ERROR( "SIG_MG_CIA_RESET_IND unexpected" );
      break;
  }
} /* sig_mg_cia_reset_ind() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sig_su_cia_cia_comp_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SU_CIA_CIA_COMP_REQ
|               This function makes a decision whether we are using data
|               compression in uplink direction and calls the according
|               function.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   defined(SNDCP_2to1) */

GLOBAL void sig_su_cia_cia_comp_req (T_SN_UNITDATA_REQ* sn_unitdata_req,
                                 USHORT npdu_number,
                                 UBYTE nsapi,
                                 UBYTE sapi
                                 )
{
  U8 direction = 0;
  BOOL compressed = FALSE;
  UBYTE packet_type = TYPE_IP;
  TRACE_ISIG( "sig_su_cia_cia_comp_req" );

  switch( GET_STATE(CIA) )
  {
    case CIA_DEFAULT:
      sndcp_is_nsapi_header_compressed(nsapi, &compressed);
      if (compressed) {
#ifdef _SNDCP_DTI_2_
        cia_header_comp(&sn_unitdata_req->desc_list2,
                        &sn_unitdata_req->desc_list2,
                        &packet_type);
#else /*_SNDCP_DTI_2_*/
        cia_header_comp(&sn_unitdata_req->desc_list,
                        &sn_unitdata_req->desc_list,
                        &packet_type);
#endif /*_SNDCP_DTI_2_*/
      }

      sndcp_is_nsapi_data_compressed(nsapi, &compressed);
      if (sndcp_data->cia.cur_xid_block.v42.is_set)
      {
        if (sndcp_data->cia.cur_xid_block.v42.p0_set)
        {
          direction = sndcp_data->cia.cur_xid_block.v42.p0;
          TRACE_EVENT_P1("dir: %d",direction);
        }
      }
      if (compressed && (direction & 0x01)) { /* datacompr. in uplink ? */
#ifdef TI_PS_FF_V42BIS

        cia_su_cci_comp_req(sn_unitdata_req,
                            npdu_number,
                            nsapi,
                            sapi,
                            packet_type);

#else  /* !TI_PS_FF_V42BIS */
        TRACE_EVENT("INFO CIA: Data compression is not implemented yet!");
        MFREE_PRIM(sn_unitdata_req);
        sn_unitdata_req = NULL;

#endif /* TI_PS_FF_V42BIS */
      } else { /* if (compressed)  */
        cia_su_cia_comp_req(sn_unitdata_req,
                                    npdu_number,
                                    nsapi,
                                    sapi,
                                    packet_type);
      }
      break;
    default:
      TRACE_ERROR( "SIG_SU_CIA_CIA_COMP_REQ unexpected" );
#ifdef _SNDCP_DTI_2_
      MFREE_PRIM(sn_unitdata_req);
#else /*_SNDCP_DTI_2_*/
      PFREE_DESC(sn_unitdata_req);
#endif /*_SNDCP_DTI_2_*/
      break;
  }
} /* sig_su_cia_cia_comp_req() */

/*#endif */ /* CF_FAST_EXEC || _SIMULATION_ || !REL99 || SNDCP_2to1 */

/*
+------------------------------------------------------------------------------
| Function    : sig_sua_cia_cia_comp_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SUA_CIA_CIA_COMP_REQ
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_sua_cia_cia_comp_req (T_SN_DATA_REQ* sn_data_req,
                                 UBYTE npdu_number,
                                 UBYTE nsapi,
                                 UBYTE sapi
                                 )
{
  BOOL compressed = FALSE;
  UBYTE packet_type = TYPE_IP;
  /*
   * define desc_list for comp header. Is initialized later.
   */
#ifdef _SNDCP_DTI_2_
  T_desc_list2 dest_desc_list;
#else /*_SNDCP_DTI_2_*/
  T_desc_list dest_desc_list;
#endif /*_SNDCP_DTI_2_*/

  TRACE_ISIG( "sig_sua_cia_cia_comp_req" );

  switch( GET_STATE(CIA) )
  {
    case CIA_DEFAULT:
      sndcp_is_nsapi_header_compressed(nsapi, &compressed);
      if (compressed) {
#ifdef _SNDCP_DTI_2_
        cia_header_comp(&dest_desc_list,
                        &sn_data_req->desc_list2,
                        &packet_type);
      } else {
        dest_desc_list = sn_data_req->desc_list2;
#else /*_SNDCP_DTI_2_*/
        cia_header_comp(&dest_desc_list,
                        &sn_data_req->desc_list,
                        &packet_type);
      } else {
        dest_desc_list = sn_data_req->desc_list;
#endif /*_SNDCP_DTI_2_*/
      }

      sndcp_is_nsapi_data_compressed(nsapi, &compressed);
      if (compressed) {

       /*
        * The data compression routine shall be invoked here.
        */
        TRACE_EVENT("INFO CIA: Data compression is not implemented yet!");
        MFREE_PRIM(sn_data_req);
        sn_data_req = NULL;

      } else { /* if (compressed) */
        cia_sua_cia_comp_req(dest_desc_list,
                                     npdu_number,
                                     nsapi,
                                     sapi,
                                     packet_type);
      }
      /*
       * free comp header
       */
      if (compressed && dest_desc_list.first != 0) {
        MFREE(dest_desc_list.first);
        dest_desc_list.first = 0;
      }
      break;
    default:
      TRACE_ERROR( "SIG_SUA_CIA_CIA_COMP_REQ unexpected" );
#ifdef _SNDCP_DTI_2_
      MFREE_PRIM(sn_data_req);
#else /*_SNDCP_DTI_2_*/
      PFREE_DESC(sn_data_req);
#endif /*_SNDCP_DTI_2_*/      
      break;
  }
} /* sig_sua_cia_cia_comp_req() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_cia_delete_npdus
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_CIA_DELETE_NPDUS
|
| Parameters  : nsapi
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_cia_delete_npdus (UBYTE nsapi)
{
  TRACE_ISIG( "sig_mg_cia_delete_npdus" );

  switch( GET_STATE(CIA) )
  {
    case CIA_DEFAULT:
      if (sndcp_data->cia.cur_cia_decomp_ind[nsapi] != NULL) {
#ifdef SNDCP_TRACE_ALL
        sndcp_data->cia.cia_decomp_ind_number[nsapi] --;
        TRACE_EVENT_P1("number of cia_decomp_ind: % d",
                       sndcp_data->cia.cia_decomp_ind_number[nsapi]);
#endif /* SNDCP_TRACE_ALL */
          /*
           * Currently assembled pdu is for given nsapi.
           */
        MFREE_PRIM(sndcp_data->cia.cur_cia_decomp_ind[nsapi]);
        sndcp_data->cia.cur_cia_decomp_ind[nsapi] = NULL;
      }
      break;
    default:
      TRACE_ERROR( "SIG_MG_CIA_DELETE_XID unexpected" );
      break;
  }
} /* SIG_MG_CIA_DELETE_NPDUS() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_cia_new_xid
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_CIA_NEW_XID
|
| Parameters  : new T_XID_BLOCK
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_mg_cia_new_xid (T_XID_BLOCK* new_xid)
{
#ifdef TI_PS_FF_V42BIS
  T_XID_BLOCK* old_xid;
  U8  p0 = SNDCP_V42_DEFAULT_DIRECTION;
  U16 p1 = SNDCP_V42_DEFAULT_P1;
  U8  p2 = SNDCP_V42_DEFAULT_P2;

  old_xid = &(sndcp_data->cia.cur_xid_block);
#endif

  TRACE_ISIG( "sig_mg_cia_new_xid" );

  switch( GET_STATE(CIA) )
  {
    case CIA_DEFAULT:
      if (new_xid->vj.is_set) {
        if (new_xid->vj.s0_m_1_set) {
          cia_compress_init((UBYTE)(new_xid->vj.s0_m_1));
        }
      }
#ifdef TI_PS_FF_V42BIS
      if (new_xid->v42.is_set)
      {
        /* block is valid */
        if (new_xid->v42.p0_set)
        {
          p0 = new_xid->v42.p0;
          /* what have we to do, if p0 is not set? */
        }
        if (new_xid->v42.p1_set)
        {
          p1 = new_xid->v42.p1;
          /* what have we to do, if p1 is not set? */
        }
        if (new_xid->v42.p2_set)
        {
          p2 = new_xid->v42.p2;
          /* what have we to do, if p2 is not set? */
        }            
        
        if (new_xid->v42.nsapis_set && new_xid->v42.nsapis == 0)
        {
          /* no applicable NSAPI
           * turn off data compression in either direction
           */
          if (old_xid->v42.p0_set && old_xid->v42.p0 & 0x1) /* uplink */
          {
            TRACE_EVENT_P1("1 enc dico:%08x",sndcp_data->cia.enc);
            v42b_deinit(sndcp_data->cia.enc);
            MFREE(sndcp_data->cia.enc);
            TRACE_EVENT("uplink deinit");
          }
          if (old_xid->v42.p0_set && old_xid->v42.p0 & 0x2) /* downlink */
          {
            TRACE_EVENT_P1("1 dec dico:%08x",sndcp_data->cia.dec);
            v42b_deinit(sndcp_data->cia.dec);
            MFREE(sndcp_data->cia.dec);
            TRACE_EVENT("downlink deinit");
          }
        }
        else if (old_xid->v42.is_set)
        {
          /* currently we have a valid xid-block for data compression
           * we have to check the parameters 
           */
          
          /* we have to check the parameters */
          if (old_xid->v42.p0 != p0 ||
              old_xid->v42.p1 != p1 ||
              old_xid->v42.p2 != p2)
          {
            /* parameters have changed */
            if (old_xid->v42.p0_set && old_xid->v42.p0 & 0x1) /* uplink */
            {
              TRACE_EVENT_P1("2 enc dico:%08x",sndcp_data->cia.enc);
              v42b_deinit(sndcp_data->cia.enc);
              MFREE(sndcp_data->cia.enc);
              TRACE_EVENT("uplink deinit");
            }
            if (old_xid->v42.p0_set && old_xid->v42.p0 & 0x2) /* downlink */
            {
              TRACE_EVENT_P1("2 dec dico:%08x",sndcp_data->cia.dec);
              v42b_deinit(sndcp_data->cia.dec);
              MFREE(sndcp_data->cia.dec);
              TRACE_EVENT("uplink deinit");
            }
            /* we have to init with new parameters */
            if (p0 & 0x1)/* uplink */
            {
              TRACE_EVENT_P1("DICO_SIZE:%d",sizeof(T_V42B_DICO));
              MALLOC(sndcp_data->cia.enc, sizeof(T_V42B_DICO));
              TRACE_EVENT_P1("enc dico:%08x",sndcp_data->cia.enc);
              v42b_init(sndcp_data->cia.enc, p1, p2, 50, BANK_SIZE_512);
              TRACE_EVENT("uplink init");
            }
            if (p0 & 0x2)/* downlink */
            {
              TRACE_EVENT_P1("DICO_SIZE:%d",sizeof(T_V42B_DICO));
              MALLOC(sndcp_data->cia.dec, sizeof(T_V42B_DICO));
              v42b_init(sndcp_data->cia.dec, p1, p2, 0, BANK_SIZE_512);
              TRACE_EVENT_P1("dec dico:%08x",sndcp_data->cia.dec);
              TRACE_EVENT("downlink init");
            }     
          }
        }
        else
        {
          /* currently we have no valid xid-block for data comprssion */
          if (p0 & 0x1)/* uplink */
          {
            TRACE_EVENT_P1("DICO_SIZE:%d",sizeof(T_V42B_DICO));
            MALLOC(sndcp_data->cia.enc, sizeof(T_V42B_DICO));
            TRACE_EVENT_P1("enc dico:%08x",sndcp_data->cia.enc);
            v42b_init(sndcp_data->cia.enc, p1, p2, 50, BANK_SIZE_512);
            TRACE_EVENT("uplink init");
          }
          if (p0 & 0x2)/* downlink */
          {
            TRACE_EVENT_P1("DICO_SIZE:%d",sizeof(T_V42B_DICO));
            MALLOC(sndcp_data->cia.dec, sizeof(T_V42B_DICO));
            TRACE_EVENT_P1("dec dico:%08x",sndcp_data->cia.dec);
            v42b_init(sndcp_data->cia.dec, p1, p2, 0, BANK_SIZE_512);
            TRACE_EVENT("downlink init");
          }          
        }
      }
      else
      {
        if (old_xid->v42.is_set && old_xid->v42.nsapis_set && old_xid->v42.nsapis != 0)
        {
          /* we have no XID with V42 set */
          if (old_xid->v42.p0_set && old_xid->v42.p0 & 0x1) /* uplink */
          {
            TRACE_EVENT_P1("3 enc dico:%08x",sndcp_data->cia.enc);
            v42b_deinit(sndcp_data->cia.enc);
            MFREE(sndcp_data->cia.enc);
            TRACE_EVENT("uplink deinit");
          }
          if (old_xid->v42.p0_set && old_xid->v42.p0 & 0x2) /* downlink */
          {
            TRACE_EVENT_P1("3 dec dico:%08x",sndcp_data->cia.dec);
            v42b_deinit(sndcp_data->cia.dec);
            MFREE(sndcp_data->cia.dec);
            TRACE_EVENT("downlink deinit");
          }
        }
      }
#endif /* TI_PS_FF_V42BIS */
      sndcp_data->cia.cur_xid_block = *new_xid;
      break;
    default:
      TRACE_ERROR( "SIG_MG_CIA_NEW_XID unexpected" );
      break;
  }
} /* SIG_MG_CIA_NEW_XID() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sig_sd_cia_cia_decomp_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SD_CIA_TRANSFER_REQ
|               This function mekes a decision whether we are using data 
|               compression in downlink direction and calls the according
|               function.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   defined(SNDCP_2to1) */

GLOBAL void sig_sd_cia_cia_decomp_req (T_LL_UNITDATA_IND* ll_unitdata_ind)/*UBYTE dcomp,
                                     T_pdu_ref pdu_ref,
                                     USHORT cur_seg_pos,
                                     T_LL_UNITDATA_IND* ll_unitdata_ind
                                    )*/
{
#ifdef TI_PS_FF_V42BIS
  U8 direction = 0;
  U8 nsapi;
#endif /* TI_PS_FF_V42BIS */
  TRACE_ISIG( "sig_sd_cia_cia_decomp_req" );
#ifdef TI_PS_FF_V42BIS  
  nsapi = (ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf / 8)]) & 0xf;
#endif /* TI_PS_FF_V42BIS */   
  switch( GET_STATE(CIA) )
  {
    case CIA_DEFAULT:
#ifdef TI_PS_FF_V42BIS
      if (sndcp_data->cia.cur_xid_block.v42.is_set)
      {
        if (sndcp_data->cia.cur_xid_block.v42.p0_set)
        {
          direction = sndcp_data->cia.cur_xid_block.v42.p0;
          TRACE_EVENT_P1("dir: %d",direction);
        }
      }
      /*
       * Is dcomp in sdu equal to dcomp in cur_xid_block and downlink?
       */
      TRACE_EVENT_P1("our dcomp value: %d", sndcp_data->cia.cur_xid_block.v42.dcomp);
      TRACE_EVENT_P1("receipt dcomp value: %d", sndcp_data->cur_dcomp[nsapi]);
      if (sndcp_data->cur_dcomp[nsapi] == sndcp_data->cia.cur_xid_block.v42.dcomp && (direction & 0x2))
      {
        /*
         * Compression used
         */
        cia_sd_cci_decomp_req(/*pdu_ref, cur_seg_pos, */ll_unitdata_ind);

      } else {
        /*
         * No compression used, request will not be sent to CCI, but handled
         * in cia service.
         */
        cia_sd_cia_decomp_req (/*pdu_ref, cur_seg_pos, */ll_unitdata_ind);
      }
#else /* ! TI_PS_FF_V42BIS */
      cia_sd_cia_decomp_req (/*pdu_ref, cur_seg_pos, */ll_unitdata_ind);
#endif /* TI_PS_FF_V42BIS */

      break;
    default:
      TRACE_ERROR( "SIG_SD_CIA_TRANSFER_REQ unexpected" );
      break;
  }
} /* sig_sd_cia_cia_decomp_req() */
/*#endif */
