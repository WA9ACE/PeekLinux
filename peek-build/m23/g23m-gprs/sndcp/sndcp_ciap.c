/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_ciap.c
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
|             functions to handles the incoming primitives as described in
|             the SDL-documentation (CIA-statemachine)
+-----------------------------------------------------------------------------
*/


#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"  /* to get Condat data types */
#include "vsi.h"       /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"       /* to get a lot of macros */
#include "prim.h"      /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"     /* to get the global entity definitions */
#include "sndcp_f.h"   /* to get the functions to access the global arrays*/

#include <string.h>    /* to get memcpy() */
#include "sndcp_mgf.h" /* to get the local functions of service mg */
#include "sndcp_sus.h" /* to get signals to service su */
#include "sndcp_suas.h"/* to get signals to service sua */
#include "sndcp_sds.h" /* to get signals to service sd */
#include "sndcp_sdas.h"/* to get signals to service sda */
#include "sndcp_ciap.h"


/*==== CONST ================================================================*/


/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : cia_vj_decomp
+------------------------------------------------------------------------------
| Description : decompresses the TCP/IP header of the given packet
|               (Van Jacobson algorithm). A part of this routine has been taken
|               from implementation of University of California, Berkeley.
|
| Parameters  : com_buf - received packet, packet length, packet type
|
| Return      : new packet length
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

LOCAL USHORT cia_vj_decomp(struct comp_buf *cbuf)
{
  struct slcompress         *comp = &sndcp_data->cia.comp;
  UBYTE                     *cp;     /* pointer to the compressed TCP/IP packet */
  UBYTE                     hlen = 0;
  ULONG                     change_mask = 0;
  T_SNDCP_TCP_HEADER        *th;
  struct cstate             *cs;
  ULONG                     tmp;
  USHORT                    th_off;

  TRACE_FUNCTION( "cia_vj_decomp" );

  switch (cbuf->p_type) {

  case TYPE_UNCOMPRESSED_TCP:
  {
    UBYTE *hdr_ptr = (UBYTE*)cbuf->c_hdr;
    UBYTE slot_nr   = hdr_ptr[9];
    if (slot_nr >= sndcp_data->cia.comp.slots_to_use){
      TRACE_EVENT_P2("ERROR DECOMP: slot_nr(%d) > slots_to_use(%d)",
                     slot_nr, sndcp_data->cia.comp.slots_to_use);
      comp->flags |= SLF_TOSS;
      return (0);
    }
    /* set last received state */
    comp->last_recv = slot_nr;
    /* get related compressed state */
    cs = &comp->rstate[comp->last_recv];
    /* clear toss flag */
    comp->flags &=~ SLF_TOSS;
    /* set protocol type to TCP/IP */
    hdr_ptr[9] = PROT_TCPIP;
    /* get IP header length */
    hlen = hdr_ptr[0] & HL_MASK;
    /* get TCP header */
    th_off = hdr_ptr[hlen*4 + 12];
    th_off = (th_off & 0xf0) >> 4;
    /* calculate IP+TCP header length  */
    hlen += th_off;
    hlen <<= 2;
    /* copy TCP+IP header */
    memcpy(cs->cs_ip, hdr_ptr, hlen);
    cs->cs_ip->ip_sum = 0;
    cs->cs_hlen = hlen;
    return ((USHORT)cbuf->pack_len);
  }

  case TYPE_COMPRESSED_TCP:
    break;

  default:
    comp->flags |= SLF_TOSS;
    TRACE_EVENT_P1("ERROR DECOMP unknown packet type (%d)", cbuf->p_type);
    return (0);
  }

  /* We've got a compressed packet. */
  cp = (UBYTE *)cbuf->c_hdr;
  change_mask = *cp++;
  if (change_mask & NEW_C) {
    /* Make sure the state index is in range, then grab the state.
     * If we have a good state index, clear the 'discard' flag. */
    if (*cp >= sndcp_data->cia.comp.slots_to_use){
      comp->flags |= SLF_TOSS;
      return (0);
    }

    comp->flags &=~ SLF_TOSS;
    /* store connection number */
    comp->last_recv = *cp++;
  } else {
    /* this packet has an implicit state index.  If we've
     * had a line error since the last time we got an
     * explicit state index, we have to toss the packet. */
    if (comp->flags & SLF_TOSS) {
      return (0);
    }
  }
  /* get related connection state */
  cs = &comp->rstate[comp->last_recv];
  /* get IP header length */
  hlen = (cs->cs_ip->ip_vhl & HL_MASK) << 2;
  /* get TCP header */
  th = (T_SNDCP_TCP_HEADER *)&((UBYTE *)cs->cs_ip)[hlen];
  /* store new TCP check sum */
  th->th_sum = sndcp_swap2((USHORT)((*cp << 8) | cp[1]));
  cp += 2;
  /* check if push bit is set */
  if (change_mask & TCP_PUSH_BIT)
    th->th_flags |= TH_PUSH;
  else
    th->th_flags &=~ TH_PUSH;

  switch (change_mask & SPECIALS_MASK) {

    case SPECIAL_I:
    {
      ULONG deltaL = sndcp_swap2(cs->cs_ip->ip_len) - cs->cs_hlen;
      th->th_ack   = sndcp_swap4(sndcp_swap4(th->th_ack) + deltaL);
      th->th_seq   = sndcp_swap4(sndcp_swap4(th->th_seq) + deltaL);
    }
      break;

    case SPECIAL_D:
      th->th_seq = sndcp_swap4(sndcp_swap4(th->th_seq)
                   + sndcp_swap2(cs->cs_ip->ip_len)
                   - cs->cs_hlen);
      break;

    /* no special case */
    default:
      /* urgend data field */
      if (change_mask & NEW_U) {
        th->th_flags |= TH_URG;
        if (*cp == 0) {
          th->th_urp = sndcp_swap2((USHORT)((cp[1] << 8) | cp[2]));
          cp += 3;
        } else {
          th->th_urp = sndcp_swap2((USHORT)((ULONG)*cp++));
        }
      } else {
        th->th_flags &=~ TH_URG;
      }
      /*
       * window size
       */
      if (change_mask & NEW_W){
        if (*cp == 0) {
          th->th_win = sndcp_swap2((USHORT)(sndcp_swap2(th->th_win)
                                 + ((cp[1] << 8) | cp[2])));
          cp += 3;
        } else {
          th->th_win = sndcp_swap2((USHORT)(sndcp_swap2(th->th_win) + (ULONG)*cp++));
        }
      }
      /*
       * acknowledgement number
       */
      if (change_mask & NEW_A){
        if (*cp == 0) {
          th->th_ack = sndcp_swap4(sndcp_swap4(th->th_ack) + ((cp[1] << 8) | cp[2]));
          cp += 3;
        } else {
          th->th_ack = sndcp_swap4(sndcp_swap4(th->th_ack) + (ULONG)*cp++);
        }
      }
      /*
       * sequence number
       */
      if (change_mask & NEW_S){
        if (*cp == 0) {
          (th->th_seq) = sndcp_swap4(sndcp_swap4(th->th_seq) + ((cp[1] << 8) | cp[2]));
          cp += 3;
        } else {
          (th->th_seq) = sndcp_swap4(sndcp_swap4(th->th_seq) + (ULONG)*cp++);
        }
      }
      break;
  }
  /*
   * packet ID
   */
  if (change_mask & NEW_I) {
    if (*cp == 0) {
      cs->cs_ip->ip_id = sndcp_swap2((USHORT)(sndcp_swap2(cs->cs_ip->ip_id)
                                         + ((cp[1] << 8) | cp[2])));
      cp += 3;
    } else {
      cs->cs_ip->ip_id = sndcp_swap2((USHORT)(sndcp_swap2(cs->cs_ip->ip_id)
                                                   + (ULONG)*cp++));
    }
  } else
    cs->cs_ip->ip_id = sndcp_swap2((USHORT)(sndcp_swap2(cs->cs_ip->ip_id) + 1));

  /*
   * At this point, cp points to the first byte of data in the
   * packet.  If we're not aligned on a 4-byte boundary, copy the
   * data down so the ip & tcp headers will be aligned.  Then back up
   * cp by the tcp/ip header length to make room for the reconstructed
   * header (we assume the packet we were handed has enough space to
   * prepend 120 bytes of header).  Adjust the length to account for
   * the new header & fill in the IP total length.
   */
  tmp = cp - (UBYTE*)cbuf->c_hdr;
  cbuf->pack_len -= cp - (UBYTE*)cbuf->c_hdr;
  if ((UBYTE*)cbuf->c_hdr > cp){
    /* we must have dropped some characters (crc should detect
     * this but the old slip framing won't) */
    comp->flags |= SLF_TOSS;
    return (0);
  }

  cbuf->c_hdr    += tmp;
  cbuf->hdr_len -= (USHORT)tmp;
  tmp = (ULONG)cp & 3;
  if (tmp) {
    if (cbuf->pack_len > 0)
      memcpy(cp - tmp, cp, cbuf->hdr_len);
    cbuf->c_hdr -= tmp;
    cp -= tmp;
  }
  cbuf->c_hdr -= cs->cs_hlen;
  cp -= cs->cs_hlen;
  cbuf->hdr_len  += cs->cs_hlen;
  cbuf->pack_len += cs->cs_hlen;
  cs->cs_ip->ip_len = sndcp_swap2(cbuf->pack_len);
  memcpy(cp, cs->cs_ip, cs->cs_hlen);

  /* recompute the ip header checksum */
  {
    USHORT *bp = (USHORT *)cp;

    for (change_mask = 0; hlen > 0; hlen -= 2)
      change_mask += *bp++;
    change_mask = (change_mask & 0xffff) + (change_mask >> 16);
    change_mask = (change_mask & 0xffff) + (change_mask >> 16);
    ((T_SNDCP_IP_HEADER *)cp)->ip_sum = ~ (USHORT)change_mask;
  }
  return (cbuf->pack_len);
}

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : cia_header_decomp
+------------------------------------------------------------------------------
| Description : de-compresses the TCP/IP header of the given packet
|               (Van Jacobson algorithm)
|
| Parameters  : packet as desc_list, packet_type
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

#ifdef _SNDCP_DTI_2_
GLOBAL void cia_header_decomp(T_desc_list2* desc_list, UBYTE* packet_type)
{
#else /*_SNDCP_DTI_2_*/
GLOBAL void cia_header_decomp(T_desc_list* desc_list, UBYTE* packet_type)
{
#endif /*_SNDCP_DTI_2_*/

  struct comp_buf     cbuf;
  /* the length of decompressed header + payload */
  USHORT              decomp_len;

#ifdef _SNDCP_DTI_2_
  T_desc2* desc = (T_desc2*) desc_list->first;
  T_desc2* decomp_desc;
  U8 *desc_buff = &desc->buffer[desc->offset];
#else
  T_desc*  desc = (T_desc*) desc_list->first;
  T_desc*  decomp_desc;
  U8 *desc_buff = &desc->buffer[0];
#endif

  TRACE_FUNCTION( "cia_header_decomp" );

#ifdef SNDCP_TRACE_BUFFER
  TRACE_EVENT("INFO DECOMP: Input Packet");
  sndcp_trace_desc_list(desc_list);
#endif /* SNDCP_TRACE_BUFFER */

  switch(*packet_type)
  {
  /*
   * packet type IP: do nothing
   */
  case TYPE_IP:
    TRACE_EVENT("INFO DECOMP: TYPE_IP");
    *packet_type = TYPE_IP;
    break;

  /*
   * packet type uncompressed TCP
   */
  case TYPE_UNCOMPRESSED_TCP:
    TRACE_EVENT("INFO DECOMP: TYPE_UNCOMPRESSED_TCP");
    cbuf.c_hdr    = (ULONG)&desc_buff[0];
    cbuf.hdr_len  = desc->len;
    cbuf.pack_len = desc_list->list_len;
    cbuf.p_type   = *packet_type;
    decomp_len = cia_vj_decomp(&cbuf);
    if(decomp_len == 0)
    {
      TRACE_EVENT("ERROR DECOMP: decomp_len = 0, TYPE_ERROR");
      *packet_type = TYPE_ERROR;
    }
    else
    {
      *packet_type = TYPE_IP;
     }
    break;

  /*
   * packet type compressed TCP
   */
  case TYPE_COMPRESSED_TCP:
    TRACE_EVENT("INFO DECOMP: TYPE_COMPRESSED_TCP");
    /*
     * Because we don't know the length of compressed TCP/IP header,
     * we have to copy max. 40 bytes, wich sure contain compressed
     * header + maybe some payload bytes.
     */
    cbuf.hdr_len = (desc->len < 40) ? desc->len : 40;
    cbuf.c_hdr = (ULONG)&sndcp_data->cia.comp.
                               tcpip_hdr[TMP_HDR_LEN-1-cbuf.hdr_len];
    cbuf.pack_len = desc_list->list_len;
    cbuf.p_type   = *packet_type;
    memcpy((UBYTE*)cbuf.c_hdr, &desc_buff[0], cbuf.hdr_len);
    decomp_len = cia_vj_decomp(&cbuf);
    if(decomp_len != 0)
    {
      /*
       * Build destination descriptor list
       */
      USHORT offset = cbuf.pack_len - desc_list->list_len;
#ifdef _SNDCP_DTI_2_
      MALLOC(decomp_desc, (USHORT)(sizeof(T_desc2) - 1 + desc->len + offset));
#else
      MALLOC(decomp_desc, (USHORT)(sizeof(T_desc) - 1 + desc->len + offset));
#endif
      /* copy compressed header + piece of data */
      memcpy(&decomp_desc->buffer[0], (UBYTE*)cbuf.c_hdr, cbuf.hdr_len);
      /* copy the rest of data */
      memcpy(&decomp_desc->buffer[cbuf.hdr_len],
             &desc->buffer[cbuf.hdr_len-offset],
              desc->len-cbuf.hdr_len+offset);
      decomp_desc->next   = desc->next;
      decomp_desc->len  = desc->len + offset;
#ifdef _SNDCP_DTI_2_
      decomp_desc->size = desc->size + offset;
      decomp_desc->offset = desc->offset;
#endif
      desc_list->first  = (ULONG)decomp_desc;
      desc_list->list_len = desc_list->list_len - desc->len + decomp_desc->len;
      MFREE(desc);
      *packet_type = TYPE_IP;
    }
    else
    {
      TRACE_EVENT("ERROR DECOMP: decomp_len = 0, TYPE_ERROR");
      *packet_type = TYPE_ERROR;
    }
    break;

  default:
      TRACE_EVENT_P1("ERROR DECOMP: unexpected packet type: %d", packet_type);
      *packet_type = TYPE_ERROR;
    break;

  }

} /* cia_header_decomp() */

#endif /* CF_FAST_EXEC */


/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : cia_cia_decomp_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive CIA_DECOMP_IND
|
| Parameters  : *cia_decomp_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void cia_cia_decomp_ind ( T_CIA_DECOMP_IND *cia_decomp_ind )
{
  UBYTE nsapi = cia_decomp_ind->pdu_ref.ref_nsapi;
  BOOL ack = FALSE;
  BOOL compressed = FALSE;
  UBYTE pcomp = cia_decomp_ind->pcomp;
  UBYTE pntt = 0;
  UBYTE sapi = 0;
  BOOL pcomp_ok = FALSE;

#ifndef _SNDCP_DTI_2_
  T_desc_list temp_desc_list;
#endif /*_SNDCP_DTI_2_*/

  UBYTE p_id = DTI_PID_IP;

  TRACE_FUNCTION( "cia_cia_decomp_ind" );

#ifdef SNDCP_TRACE_ALL
  TRACE_EVENT_P1("cia_decomp_ind->pcomp: %02x", cia_decomp_ind->pcomp);
#endif /* SNDCP_TRACE_ALL */

  /* 
   * Is nsapi in ack mode?
   */
  sndcp_get_nsapi_ack(nsapi, &ack);

  switch( GET_STATE(CIA) )
  {
    case CIA_DEFAULT:
      sndcp_is_nsapi_header_compressed(nsapi, &compressed);
      /*
       * Only compress if pcomp in sn pdu is assigned to context.
       */
      sndcp_get_nsapi_sapi(nsapi, &sapi);
      mg_get_sapi_pcomp_pntt(sapi,
                             pcomp,
                             &pntt);

      mg_get_sapi_pntt_nsapi(sapi, pntt, nsapi, &pcomp_ok);

      if (compressed && pcomp_ok && pcomp != 0) {
        UBYTE packet_type = TYPE_COMPRESSED_TCP;

        if (pcomp == sndcp_data->cia.cur_xid_block.vj.pcomp1) {
          packet_type = TYPE_UNCOMPRESSED_TCP;
        }
#ifndef _SNDCP_DTI_2_
        temp_desc_list.list_len = cia_decomp_ind->desc_list2.list_len;
        temp_desc_list.first = cia_decomp_ind->desc_list2.first;
        cia_header_decomp(&temp_desc_list1, &packet_type);
        cia_decomp_ind->desc_list2.list_len = temp_desc_list.list_len;
        cia_decomp_ind->desc_list2.first = temp_desc_list.first;
#else  /*_SNDCP_DTI_2_*/
        cia_header_decomp(&cia_decomp_ind->desc_list2, &packet_type);
#endif /*_SNDCP_DTI_2_*/
        if (packet_type != TYPE_IP) {
#ifdef SNDCP_TRACE_ALL
          sndcp_data->cia.cia_decomp_ind_number[nsapi] --;
          TRACE_EVENT_P1("number of cia_decomp_ind: % d",
                         sndcp_data->cia.cia_decomp_ind_number[nsapi]);
#endif /* SNDCP_TRACE_ALL */

          MFREE_PRIM(cia_decomp_ind);
          cia_decomp_ind = NULL;
          /*
           * Corupted segment, request next one.
           */
          TRACE_EVENT("WARNING DECOMP: Corupted segment, request next one!");
          if (ack) {
            sig_cia_sda_getdata(sapi, nsapi);
          } else {
            sig_cia_sd_getunitdata(sapi, nsapi);
          }
          return;
        }
      }
      /*
       * Is the cnf for sd or sda?
       */
      if (ack) {
        sig_cia_sda_cia_decomp_ind(cia_decomp_ind, p_id);
      } else {
        sig_cia_sd_cia_decomp_ind(cia_decomp_ind, p_id);
      }
      break;
    default:
      TRACE_ERROR( "CIA_DECOMP_IND unexpected" );
      MFREE_PRIM(cia_decomp_ind); 
      break;
  }
} /* cia_cia_decomp_ind() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : cia_cia_comp_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive CIA_COMP_IND
|
| Parameters  : *cia_comp_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void cia_cia_comp_ind ( T_CIA_COMP_IND *cia_comp_ind )
{
  UBYTE nsapi = cia_comp_ind->pdu_ref.ref_nsapi;
  BOOL ack = FALSE;

  TRACE_FUNCTION( "cia_cia_comp_ind" );


#ifdef SNDCP_TRACE_ALL
  switch (cia_comp_ind->packet_type) {
  case TYPE_IP:
    TRACE_EVENT_P2("cia_comp_ind->packet_type: %02x (%s)",
                   cia_comp_ind->packet_type,
                   "TYPE_IP");
    break;
  case TYPE_UNCOMPRESSED_TCP:
    TRACE_EVENT_P2("cia_comp_ind->packet_type: %02x (%s)",
                   cia_comp_ind->packet_type,
                   "TYPE_UNCOMPRESSED_TCP");
    break;
  case TYPE_COMPRESSED_TCP:
    TRACE_EVENT_P2("cia_comp_ind->packet_type: %02x (%s)",
                   cia_comp_ind->packet_type,
                   "TYPE_COMPRESSED_TCP");
    break;
  case TYPE_ERROR:
    TRACE_EVENT_P2("cia_comp_ind->packet_type: %02x (%s)",
                   cia_comp_ind->packet_type,
                   "TYPE_ERROR");
    break;
  default:
    TRACE_EVENT_P2("cia_comp_ind->packet_type: %02x (%s)",
                   cia_comp_ind->packet_type,
                   "unknown");


  }
#endif /* SNDCP_TRACE_ALL */

  switch( GET_STATE(CIA) )
  {
    case CIA_DEFAULT:
      /*
       * Is the cnf for su or sua?
       */
      sndcp_get_nsapi_ack(nsapi, &ack);
      if (ack) {
        sig_cia_sua_cia_comp_ind(cia_comp_ind);
      } else {
        sig_cia_su_cia_comp_ind(cia_comp_ind);
      }
      break;
    default:
      TRACE_ERROR( "CIA_CIA_COMP_IND unexpected" );
      sndcp_cl_desc3_free((T_desc3*)cia_comp_ind->desc_list3.first);
      MFREE(cia_comp_ind); 
      break;
  }
} /* cia_cia_comp_ind() */

#endif /* CF_FAST_EXEC */




