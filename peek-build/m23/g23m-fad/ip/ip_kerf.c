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
#include "dti.h"
#include "ip.h"
#include "ip_udp.h"

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                        MODULE  : ip_kerf.c           |
| STATE   : code                       ROUTINE : ip_packet_validator |
+--------------------------------------------------------------------+

  PURPOSE : Checks for IP packet validity
*/
BOOL ip_packet_validator (T_desc_list2 * desc_list)
{
  TRACE_FUNCTION ("ip_packet_validator()");

  if (desc_list == NULL)
  {
    TRACE_ERROR ("desc_list == NULL in IP validator.");
    return FALSE;
  }

  {
    /* Loop once through all the list */
    register T_desc2 * desc = (T_desc2 *) desc_list->first;
    register unsigned length = 0;
    while (desc != NULL)
    {
      /* Check consistency of descs */
      if (desc->size < desc->offset + desc->len)
      {
        TRACE_ERROR (
          "Packet dropped: "
          "desc->size < desc->offset + desc->len in IP validator."
        );
        return FALSE;
        /* This is a mistake. Must be DTI1. */
        /* desc->offset = 0; */
        /* desc->size = desc->len; */
      }
      /* Make sure that all offsets are 0 */
      if (desc->offset != 0)
      {
        memmove (desc->buffer, desc->buffer + desc->offset, desc->len);
        desc->offset = 0;
      }
      /* Calculate the sum of lengths */
      length += desc->len;
      desc = (T_desc2 *) desc->next;
    }
    /* Check desc_list->list_len */
    if (desc_list->list_len != length)
    {
      TRACE_ERROR (
        "Packet dropped: "
        "desc_list->list_len != length in IP validator."
      );
      return FALSE;
      /* This is a mistake. But we could live with it. */
      /* desc_list->list_len = (USHORT) length; */
    }

    /* Need at least LEN_IP_HEADER_B bytes for one header */
    if (length < LEN_IP_HEADER_B)
    {
      TRACE_ERROR (
        "Packet dropped: "
        "length < LEN_IP_HEADER_B in IP validator."
      );
      return FALSE;
    }

    /* Have enough bytes, but they may be distributed */
    desc = (T_desc2 *) desc_list->first;
    if (desc->len < LEN_IP_HEADER_B)
    {
      /* Collect all bytes in one desc */
      T_desc2 * bigdesc = M_ALLOC (offsetof (T_desc2, buffer) + length);
      if (bigdesc == NULL)
      {
        TRACE_ERROR ("Not enough memory in IP validator.");
        return FALSE;
      }

      bigdesc->next = 0;
      bigdesc->offset = 0;
      bigdesc->len = (USHORT) length;
      bigdesc->size = (USHORT) length;

      /* Loop again through all the list */
      length = 0;
      while (desc != NULL)
      {
        T_desc2 * removable = desc;
        memcpy (bigdesc->buffer + length, desc->buffer, desc->len);
        length += desc->len;
        desc = (T_desc2 *) desc->next;
        MFREE (removable);
      }
      desc_list->first = (ULONG) bigdesc;
      desc = bigdesc;
    }

    /* Need at least GET_IP_HEADER_LEN_B() bytes for the IP header */
    if (length < (unsigned) GET_IP_HEADER_LEN_B (desc->buffer))
    {
      TRACE_ERROR (
        "Packet dropped: "
        "length < GET_IP_HEADER_LEN_B() in IP validator."
      );
      return FALSE;
    }

    /* Survived */
    return TRUE;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)        MODULE  : IP_KERF                  |
| STATE   : code                  ROUTINE : free_primitive_data_ind  |
+--------------------------------------------------------------------+
 *
 * Set a DTI_DATA_IND primitive free and the pointer to NULL
 */
void free_primitive_data_ind (T_DTI2_DATA_IND ** dti_data_ind)
{
  TRACE_FUNCTION ("free_primitive_data_ind()");

  if (dti_data_ind != NULL && *dti_data_ind != NULL) {
    PFREE_DESC2 (* dti_data_ind);
    *dti_data_ind = NULL;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)       MODULE  : IP_KERF                   |
| STATE   : code                 ROUTINE : free_primitive_data_req   |
+--------------------------------------------------------------------+
 *
 * Set a DTI_DATA_REQ primitive free and the pointer to NULL
 */
void free_primitive_data_req (T_DTI2_DATA_REQ ** dti_data_req)
{
  TRACE_FUNCTION ("free_primitive_data_req()");

  if (dti_data_req != NULL && *dti_data_req != NULL) {
    PFREE_DESC2 (* dti_data_req);
    *dti_data_req = NULL;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                       MODULE  : IP                   |
| STATE   : code                      ROUTINE : set_desc_len         |
+--------------------------------------------------------------------+
 *
 * Set the new desc length and fix desc_list length
 */
void set_desc_len (T_desc_list2 * desc_list, T_desc2 * desc, USHORT len_desc)
{
  TRACE_FUNCTION ("set_desc_len()");

  desc_list->list_len = (USHORT) (desc_list->list_len + len_desc - desc->len);
  desc->len = len_desc;
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                       MODULE  : IP                   |
| STATE   : code                      ROUTINE : del_rest_descs       |
+--------------------------------------------------------------------+
 *
 * Free the next desc(s) from desc. Fix the list_len in desc_list
 */
void del_rest_descs (T_desc_list2 * desc_list, T_desc2 * desc)
{
  BOOL go = TRUE;
  T_desc2 * p_desc_last, * p_desc_start;

  TRACE_FUNCTION ("del_rest_descs()");

  if (desc->next > 0)
  {
    p_desc_start = desc;
    desc = (T_desc2 *) desc->next;

    do
    {
      p_desc_last = desc;

      if (desc->next > 0)
        desc = (T_desc2 *) desc->next;
      else
        go = FALSE;

      desc_list->list_len = (USHORT)
        (desc_list->list_len - p_desc_last->len);
      MFREE (p_desc_last);
    }
    while (go);

    desc = p_desc_start;
    desc->next = 0;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)            MODULE  : IP_KERF              |
| STATE   : code                      ROUTINE : del_descs            |
+--------------------------------------------------------------------+
 *
 * Free the desc(s)
 */
void del_descs (T_desc2 * desc)
{
  BOOL go = TRUE;
  T_desc2 * p_desc_last;

  TRACE_FUNCTION ("del_descs()");

  do
  {
    p_desc_last = desc;

    if (desc->next > 0)
      desc = (T_desc2 *) desc->next;
    else
      go = FALSE;

    MFREE (p_desc_last);
  }
  while (go);
}

/** filter_out_in_desc
 *
 * cuts length bytes out of a T_desc2::buffer, starting at 'start'.
 *
 * @param desc     Pointer to a T_desc2
 * @param start    Start of the block to be cut out
 * @param length   Length of the block to be cut out
 *
 * @return TRUE if parameters are valid, FALSE if parameters are invalid
 */
static BOOL filter_out_in_desc (
  T_desc2 * desc,
  USHORT start,
  USHORT length
) {
  TRACE_FUNCTION ("filter_out_in_desc()");

  if (desc != NULL && start + length <= desc->len) {
    memmove (
      desc->buffer + start,
      desc->buffer + start + length,
      desc->len - start - length
    );
    desc->len = (USHORT) (desc->len - length);
    return TRUE;
  } else {
    TRACE_ERROR ("Parameters are invalid in filter_out_in_desc().");
    return FALSE;
  }
}

/** truncate_descs
 *
 * truncates a T_desc_list2 to no more than new_length bytes.
 *
 * @param desc_list    Pointer to a T_desc_list2
 * @param new_length   Maximal new length of that T_desc_list2
 */
void truncate_descs (T_desc_list2 * desc_list, USHORT new_length)
{
  TRACE_FUNCTION ("truncate_descs()");

  if (desc_list != NULL)
  {
    T_desc2 ** pp_desc = (T_desc2 **) & desc_list->first;
    desc_list->list_len = 0; /* Will be recalculated, anyway */

    /* First a loop over all T_descs which will remain. */
    while (*pp_desc != NULL && new_length != 0)
    {
      if (new_length < (*pp_desc)->len)
        (*pp_desc)->len = new_length;
      new_length = (USHORT) (new_length - (*pp_desc)->len);
      desc_list->list_len = /* Recalculation */
        (USHORT) (desc_list->list_len + (*pp_desc)->len);
      pp_desc = (T_desc2 **) & (*pp_desc)->next;
    }

    /* Second a loop over all T_descs which have to be deleted, if any. */
    while (*pp_desc != NULL)
    {
      T_desc2 * p_desc_removable = *pp_desc;
      *pp_desc = (T_desc2 *) p_desc_removable->next;
      MFREE (p_desc_removable);
    }

    /* This way, new_length==0 at function call results in
     * desc_list->first==NULL at function return. */

    /* Third a loop over all T_descs which have to be created, if any.
     * (If it were Prokrustes, not truncation,
     * we would have to allocate new T_descs here.)
     * while (new_length != 0) { ... } */
  }
  else
  {
    TRACE_ERROR ("truncate_descs() called without T_desc_list2.");
    TRACE_ASSERT (0);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)            MODULE  : IP_KERF              |
| STATE   : code                      ROUTINE : insert_sort_descs    |
+--------------------------------------------------------------------+
 *
 * Insert a new descriptor sorted by fragment offset.
 * The less value at first. Filter out descs with same offset.
 */
static void insert_sort_desc (T_desc_list2 * desc_list, T_desc2 * p_new_desc)
{
  if (desc_list != NULL && p_new_desc != NULL)
  {
    T_desc2 * desc;
    T_desc2 * p_desc_last;
    UBYTE * ip_header;
    USHORT offset, new_offset;

    offset = 0xffff;
    ip_header = p_new_desc->buffer;
    new_offset = (USHORT) GET_IP_FRAG_OFFSET (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
    desc = (T_desc2 *) desc_list->first;
    p_desc_last = NULL;

    while (desc NEQ NULL)
    {
      ip_header = desc->buffer;
      offset = (USHORT) GET_IP_FRAG_OFFSET (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
      if (new_offset <= offset)
        break;
      p_desc_last = desc;
      desc = (T_desc2 *) desc->next;
    }
    
    if (new_offset EQ offset)
    {
      /* Two descriptors with the same offset
       * Throw away the new one */
      MFREE (p_new_desc);
    }
    else
    {
      /* Insert descriptor after p_desc_last */
      if (p_desc_last EQ NULL)
      {
        p_new_desc->next = desc_list->first;
        desc_list->first = (ULONG) p_new_desc;
      }
      else
      {
        p_new_desc->next = p_desc_last->next;
        p_desc_last->next = (ULONG) p_new_desc;
      }

      /* Add the length of the new descriptor */
      desc_list->list_len = (USHORT) (desc_list->list_len + p_new_desc->len);
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)            MODULE  : IP_KERF              |
| STATE   : code                      ROUTINE : reassemble_fragments |
+--------------------------------------------------------------------+
 *
 * Reassemble fragments coming in downlink
 */
void reassemble_fragments (
  T_DTI2_DATA_IND ** dti_data_ind,
  T_LOLA * p_dl,
  UBYTE * ip_header,
  BOOL first_segment,
  /* BOOL middle_segment, */
  BOOL last_segment /*,*/
  /* USHORT fragm_offset */
) {
  USHORT fragm_id, header_len_b, offset;
  UBYTE fragm_prot;
  ULONG server_source_addr;
  BOOL found_source_addr = FALSE;

  TRACE_FUNCTION ("reassemble_fragments()");

  fragm_id = (USHORT) GET_IP_IDENT (ip_header);
  fragm_prot = GET_IP_PROT (ip_header);
  server_source_addr = GET_IP_SOURCE_ADDR (ip_header);

  /* Check source address from server */
  p_dl->pos_server = 0;
  do
  {
    if (p_dl->ip_source_addr_segment[p_dl->pos_server] NEQ server_source_addr)
      p_dl->pos_server++;
    else
      found_source_addr = TRUE;

  } while (! found_source_addr && p_dl->pos_server < MAX_SEGM_SERVER);

  /* Select the server - by only one server p_dl->pos_server = 0 */
  if (p_dl->pos_server EQ MAX_SEGM_SERVER)
    p_dl->pos_server = 0;

  p_dl->ip_source_addr_segment[p_dl->pos_server] = server_source_addr;

  /* Check if it is the first or the last segment */
  if (first_segment)
    p_dl->got_first_segment[p_dl->pos_server] = TRUE;

  if (last_segment)
    p_dl->got_last_segment[p_dl->pos_server] = TRUE;

  /* Is it the first of any segment type? */
  if (p_dl->state_reassembly[p_dl->pos_server] EQ NO_SEGMENTS)
  {
    p_dl->data_ind_reassembly[p_dl->pos_server] = *dti_data_ind;
    p_dl->state_reassembly[p_dl->pos_server] = READ_SEGMENT;
    p_dl->id_reassemble[p_dl->pos_server] = fragm_id;
    p_dl->prot_reassemble[p_dl->pos_server] = fragm_prot;

    /* Start reassembly timer */

    /* Implementation problem: the timer index must be
     * layer * MAX_SEGM_SERVER + p_dl->pos_server. The
     * layer variable is not forwarded to the function.
     * It works, because layer is ever set to 0 in the moment. */

    vsi_t_start (VSI_CALLER p_dl->pos_server, TIME_REASSEMBLY);
    p_dl->timer_reass_running[p_dl->pos_server] = TRUE;
  }
  /* Check the fragment ID and protocol. If not the same - free resources */
  else if (
    (p_dl->id_reassemble[p_dl->pos_server] NEQ fragm_id) OR
    (p_dl->prot_reassemble[p_dl->pos_server] NEQ fragm_prot)
  ) {
    if (p_dl->timer_reass_running[p_dl->pos_server])
    {
      /* Implementation problem: the timer index must be
       * layer * MAX_SEGM_SERVER + p_dl->pos_server. The
       * layer variable is not forwarded to the function.
       * It works, because layer is ever set to 0 in the moment. */

      csf_stop_timer (p_dl->pos_server);
      p_dl->timer_reass_running[p_dl->pos_server] = FALSE;
    }

    p_dl->drop_packet = TRUE;
    p_dl->state_reassembly[p_dl->pos_server] = NO_SEGMENTS;

    free_primitive_data_ind (p_dl->data_ind_reassembly + p_dl->pos_server);

    /* Note: We could generate an ICMP packet */
    return;
  }
  else /* Got correct segments before */
  {
    /* Note: The whole datagram must be in the same descriptor */
    /* Note: The timeout value should be changed to MAX (TTL, TIME_REASSEMBLY) */

    BOOL all_fragments_received = FALSE;

    T_desc_list2 * desc_list = & p_dl->data_ind_reassembly[p_dl->pos_server]->desc_list2;
    T_desc2 * desc = (T_desc2 *) p_dl->dti_data_ind->desc_list2.first;

    if (desc == NULL || (T_desc2 *) desc_list->first == NULL)
    {
      TRACE_ERROR ("Pointer is NULL.");
      return;
    }

    /* Insert the descriptor in the right position, according to the offset */
    insert_sort_desc (desc_list, desc);

    /* Check if got all fragments */

    if (
      p_dl->got_first_segment[p_dl->pos_server] AND
      p_dl->got_last_segment[p_dl->pos_server]
    ) {
      BOOL go = TRUE;
      USHORT data_len, next_offset;
      UBYTE * ip_packet;
      desc = (T_desc2 *) desc_list->first;
      ip_packet = desc->buffer;
      header_len_b = (USHORT) GET_IP_HEADER_LEN_B (ip_packet);
      data_len = (USHORT) (desc->len - header_len_b);
      next_offset = 0;
      do
      {
        if ((T_desc2 *) desc->next != NULL)
        {
          desc = (T_desc2 *) desc->next;
          ip_packet = desc->buffer;
          next_offset = (USHORT) GET_IP_FRAG_OFFSET_B (ip_packet);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
          header_len_b = (USHORT) GET_IP_HEADER_LEN_B (ip_packet);

          if ((T_desc2 *) desc->next != NULL)
            data_len = (USHORT) (data_len + desc->len - header_len_b);
        }
        else
          go = FALSE;
      }
      while (go);

      if (data_len >= next_offset)
        all_fragments_received = TRUE;
    }

    /* Reassemble the fragments */

    if (all_fragments_received)
    {
      BOOL go = TRUE;
      USHORT len_fragments, header_len_b_first;
      T_desc2 * p_desc_first, * p_desc_last;
      UBYTE * ip_packet;

      p_dl->state_reassembly[p_dl->pos_server] = NO_SEGMENTS;

      if (
        desc_list == NULL ||
        (T_desc2 *) desc_list->first == NULL ||
        (T_desc2 *) ((T_desc2*)desc_list->first)->next == NULL
      ) {
        TRACE_ERROR ("Pointer is NULL.");
        return;
      }

      /* Fix the first desc */
      p_desc_first = (T_desc2 *) desc_list->first;
      ip_packet = p_desc_first->buffer;
      header_len_b_first = (USHORT) GET_IP_HEADER_LEN_B (ip_packet);

      /* Start from next desc */
      desc = (T_desc2 *) p_desc_first->next;
      ip_packet = desc->buffer;
      offset = (USHORT) GET_IP_FRAG_OFFSET_B (ip_packet);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
      p_desc_first->len = (USHORT) (offset + header_len_b_first);
      len_fragments = offset;

      do
      {
        if (desc->next NEQ 0)
        {
          p_desc_last = desc;
          desc = (T_desc2 *) desc->next;

          /* Get the fragment offset */
          ip_packet = desc->buffer;
          header_len_b = (USHORT) GET_IP_HEADER_LEN_B (ip_packet);
          offset = (USHORT) GET_IP_FRAG_OFFSET_B (ip_packet);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

          /* Filter out IP header */
          ip_packet = p_desc_last->buffer;
          header_len_b = (USHORT) GET_IP_HEADER_LEN_B (ip_packet);
          filter_out_in_desc (p_desc_last, 0, header_len_b);

          len_fragments = (USHORT) (len_fragments + p_desc_last->len);
        }
        else
        {
          go = FALSE;

          /* Filter out last IP header */
          ip_packet = desc->buffer;
          header_len_b = (USHORT) GET_IP_HEADER_LEN_B (ip_packet);
          filter_out_in_desc (desc, 0, header_len_b);

          len_fragments = (USHORT) (len_fragments + desc->len);
        }
      }
      while (go);

      /* Build the IP datagram */

      /* Implementation problem: the timer index must be
       * layer * MAX_SEGM_SERVER + p_dl->pos_server. The
       * layer variable is not forwarded to the function.
       * It works, because layer is ever set to 0 in the moment. */

      csf_stop_timer (p_dl->pos_server);
      p_dl->timer_reass_running[p_dl->pos_server] = FALSE;
      /* PFREE (dti_data_ind); */
      /* Is being freed elsewhere. Would anyway have false level of indirection. */
      p_dl->data_ind_reassembly[p_dl->pos_server]->desc_list2.list_len =
        (USHORT) (len_fragments + header_len_b_first);

      *dti_data_ind = p_dl->data_ind_reassembly[p_dl->pos_server];
      p_desc_first = (T_desc2 *) (*dti_data_ind)->desc_list2.first;
      ip_packet = p_desc_first->buffer;

      {
        ULONG dest_addr;
        UBYTE ttl;
        dest_addr = GET_IP_DEST_ADDR (ip_packet);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
        ttl = GET_IP_TTL (ip_packet);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

        build_ip_header (
          ip_packet,
          fragm_id,
          (UBYTE) (header_len_b_first >> 2),
          NORMAL_SERVICE,
          ttl,
          p_dl->ip_source_addr_segment[p_dl->pos_server],
          dest_addr,
          (UBYTE) (len_fragments + header_len_b_first),
          NO_OFFSET_FRAG,
          FLAG_NOT_SET,
          FLAG_NOT_SET,
          fragm_prot
        );
      }
      p_dl->data_ind_reassembly[p_dl->pos_server] = NULL;
    }
    /* else */
      /* PFREE (dti_data_ind); */
      /* Is being freed elsewhere. Would anyway have false level of indirection. */
  }

}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                       MODULE  : IP                   |
| STATE   : code                      ROUTINE : put_desc_first_pos   |
+--------------------------------------------------------------------+
 *
 * Put desc on the first position of a desc_list.
 * Set the new list_len in desc_list.
 */
void put_desc_first_pos (T_desc_list2 * desc_list, T_desc2 * p_desc_new)
{
  ULONG help;
  T_desc2 * p_desc_help;

  TRACE_FUNCTION ("put_desc_first_pos()");

  p_desc_help = (T_desc2 *) desc_list->first;

  if (p_desc_help NEQ p_desc_new)
  {
    help = desc_list->first;
    desc_list->first = (ULONG) p_desc_new;
    p_desc_new->next = help;
    desc_list->list_len = (USHORT) (desc_list->list_len + p_desc_new->len);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)      MODULE  : IP_KER                     |
| STATE   : code                ROUTINE : copy_from_descs_to_desc    |
+--------------------------------------------------------------------+
 *
 * Copy data from one or more descs into desc_new. Possible to use offset.
 */
void copy_from_descs_to_desc (
  T_desc2 ** desc,
  T_desc2 * desc_new,
  USHORT copy_len,
  USHORT offset_desc,
  USHORT offset_desc_new,
  USHORT * pos_copy,
  USHORT * total_len_copy
) {
  BOOL go = TRUE;
  *pos_copy = offset_desc;
  *total_len_copy = 0;

  do
  {
    while (
      (*pos_copy < (*desc)->len) AND
      (offset_desc_new < desc_new->len) AND
      (*total_len_copy < copy_len)
    ) {
      desc_new->buffer[offset_desc_new] = (*desc)->buffer[*pos_copy];
      offset_desc_new ++;
      (*pos_copy)++;
      (*total_len_copy)++;
    }

    if (
      (*total_len_copy >= copy_len) OR
      (offset_desc_new >= desc_new->len)
    ) {
      go = FALSE;
    }
    else
    {
      if (*pos_copy >= (*desc)->len)
      {
        if ((*desc)->next > 0)
        {
          *desc = (T_desc2 *) (*desc)->next;
          *pos_copy = 0;
        }
        else
        {
          go = FALSE;
        }
      }
    }
  }
  while (go);
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                       MODULE  : IP                   |
| STATE   : code                      ROUTINE : make_new_desc        |
+--------------------------------------------------------------------+
 *
 * Malloc a new desc. Init buffer 0 if buff_init_0 = TRUE
 */
BOOL make_new_desc (
  T_desc2 ** p_desc_new,
  USHORT malloc_len,
  BOOL buff_init_0
) {
  TRACE_FUNCTION ("make_new_desc()");

  MALLOC (*p_desc_new, offsetof (T_desc2, buffer) + malloc_len);
  /* The target compiler issues a warning
   * "pointer type conversion may violate alignment constraints"
   * here and everywhere where MALLOC is being used. */

  if (*p_desc_new EQ 0)
    return FALSE;

  if (buff_init_0)
  {
    USHORT i;
    for (i = 0; i < malloc_len; i++)
      (*p_desc_new)->buffer[i] = 0;
  }
  (*p_desc_new)->next = 0;
  (*p_desc_new)->offset = 0;
  (*p_desc_new)->len = malloc_len;
  (*p_desc_new)->size = malloc_len;
  return TRUE;
}

#if 0
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)            MODULE  : IP_KERF              |
| STATE   : code                      ROUTINE : sort_descs_id_up     |
+--------------------------------------------------------------------+
 *
 * Sort the descs with fragment offset. The less value at
 * first. Filter out descs with same offset.
 */
USHORT sort_descs_id_up (
  T_desc_list * desc_list,
  ULONG sort_array[2][MAX_SEGMENTS]
) {
  T_desc * desc;
  USHORT i, n_descs, code;
  BOOL go = TRUE;
  ULONG addr, wert;
  UBYTE * ip_header;

  TRACE_FUNCTION ("sort_descs_id_up()");

  desc = (T_desc *) desc_list->first;
  n_descs = 0;
  code = GO_ON_SEGMENTING;

  if (desc->next > 0)
  {
    /* Write desc address and search-value into sort_array */

    desc = (T_desc *) desc_list->first;

    i = 0;
    ip_header = desc->buffer;
    sort_array[1][i] = GET_IP_FRAG_OFFSET (ip_header);
    sort_array[0][i++] = desc_list->first;

    do
    {
      if (desc->next > 0 AND i < MAX_SEGMENTS)
      {
        desc = (T_desc *) desc->next;
        sort_array[0][i] = (ULONG) desc;
        ip_header = desc->buffer;
        sort_array[1][i++] = GET_IP_FRAG_OFFSET (ip_header);
      }
      else
        go = FALSE;
    }
    while (go);

    /* Sort the array if not overflow */

    if (i < MAX_SEGMENTS)
    {
      n_descs = i;

      for (i = 0; i < n_descs - 1; i++)
      {
        USHORT min, j;
        min = i;

        for (j = i + 1; j < n_descs; j++)
          if (sort_array[1][j] < sort_array[1][min])
            min = j;

        addr = sort_array[0][i];
        wert = sort_array[1][i];
        sort_array[0][i] = sort_array[0][min];
        sort_array[1][i] = sort_array[1][min];
        sort_array[0][min] = addr;
        sort_array[1][min] = wert;
      }

      /* Filter out descs with same fragment offset */
      {
        ULONG v1;
        USHORT j, k, len;
        len = n_descs - 1;
        i = 0;
        while (i < len)
        {
          v1 = sort_array[1][i];
          j = i + 1;
          if (v1 EQ sort_array[1][j])
          {
            k = j;
            n_descs--;
            while (k <= len)
            {
              sort_array[0][k] = sort_array[0][k + 1];
              sort_array[1][k] = sort_array[1][k + 1];
              k++;
            }
            len--;
          }
          if (sort_array[1][i] NEQ sort_array[1][i + 1])
            i++;
        }
      }

      /* Put the descs together and correct the desc_list->list_len */

      desc_list->first = sort_array[0][0];
      desc = (T_desc *) sort_array[0][0];
      desc_list->list_len = 0;
      desc_list->list_len = desc->len;

      for (i = 1; i < n_descs; i++)
      {
        desc->next = sort_array[0][i];
        desc = (T_desc *) desc->next;
        desc_list->list_len = desc_list->list_len + desc->len;
      }

      desc->next = 0;
    }
    else
      code = NO_SPACE_SEGMENTING;

    desc = (T_desc *) desc_list->first;
  }
  return code;
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)    MODULE  : IP_KERF                      |
| STATE   : code              ROUTINE : build_ip_header              |
+--------------------------------------------------------------------+
 *
 * Build IP header
 */
void build_ip_header (
  UBYTE * ip_header_new,
  USHORT identity,
  UBYTE header_len,
  UBYTE type_of_service,
  UBYTE ttl,
  ULONG src_addr,
  ULONG dest_addr,
  USHORT total_len,
  USHORT fragm_offset,
  UBYTE df_flag,
  UBYTE mf_flag,
  UBYTE prot
) {
  USHORT chk_sum;

  TRACE_FUNCTION ("build_ip_header()");

  SET_IP_VERSION (ip_header_new, IP_VERSION);
  /*lint -e{415, 416} (Warning -- access/creation of out-of-bounds pointer) */
  {
  SET_IP_HEADER_LEN (ip_header_new, header_len);
  SET_IP_TYPE_OF_SERVICE (ip_header_new, type_of_service);
  SET_IP_TOTAL_LEN (ip_header_new, total_len);
  SET_IP_IDENT (ip_header_new, identity);
  SET_IP_OFF_FLAG (ip_header_new, FLAG_NOT_SET);
  SET_IP_DF_FLAG (ip_header_new, df_flag);
  SET_IP_MF_FLAG (ip_header_new, mf_flag);
  SET_IP_FRAG_OFFSET (ip_header_new, fragm_offset);
  SET_IP_PROT (ip_header_new, prot);
  SET_IP_TTL (ip_header_new, ttl);
  SET_IP_SOURCE_ADDR (ip_header_new, src_addr);
  SET_IP_DEST_ADDR (ip_header_new, dest_addr);
  RESET_IP_CHECKSUM (ip_header_new);
  chk_sum = inet_checksum (ip_header_new, (USHORT) (header_len * 4));
  SET_IP_CHECKSUM (ip_header_new, chk_sum);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)    MODULE  : IP_KERF                      |
| STATE   : code              ROUTINE : build_icmp_packet            |
+--------------------------------------------------------------------+
 *
 * Build IP icmp packet without payload
 */
void build_icmp_packet (
  USHORT header_len_b,
  UBYTE typ,
  UBYTE code,
  UBYTE ttl,
  UBYTE * ip_header,
  USHORT identity,
  ULONG dest_addr,
  ULONG src_addr,
  T_desc_list2 * desc_list
) {
  USHORT chk_sum;
  UBYTE header_len_b_bak;

  TRACE_FUNCTION ("build_icmp_packet()");

  header_len_b_bak = (UBYTE) (header_len_b >> 2);

  /* Build ICMP header */

  SET_ICMP_TYPE (ip_header, typ, header_len_b);
  SET_ICMP_CODE (ip_header, code, header_len_b);
  RESET_ICMP_CHK_SUM (ip_header, header_len_b);
  chk_sum = desc_checksum (desc_list, header_len_b, 0);
  SET_ICMP_CHK_SUM (ip_header, chk_sum, header_len_b);

  /* Build IP header */

  build_ip_header (
    ip_header,
    identity,
    header_len_b_bak,
    NORMAL_SERVICE,
    ttl,
    src_addr,
    dest_addr,
    desc_list->list_len,
    NO_OFFSET_FRAG,
    FLAG_NOT_SET,
    FLAG_NOT_SET,
    ICMP_PROT
  );
}

/*
+-------------------------------------------------------------------+
| PROJECT : WAP                  MODULE  : IP                       |
| STATE   : code                 ROUTINE : build_icmp_with_payload  |
+-------------------------------------------------------------------+
 *
 * Build a ICMP packet with payload of 64 bits
 */
void build_icmp_with_payload (
  T_DTI2_DATA_REQ * data_req,
  USHORT identity,
  UBYTE ttl,
  ULONG src_addr,
  UBYTE icmp_type,
  UBYTE icmp_code
) {
  T_desc2 * desc_new, * desc;
  T_desc_list2 * desc_list;
  USHORT malloc_len, header_len_b, chk_sum;
  UBYTE * ip_header;
  ULONG dest_addr;

#define LEN_ICMP_HEADER_PAYLOAD 8
#define LEN_PAYLOAD 8

  TRACE_FUNCTION ("build_icmp_with_payload()");

  desc_list = & data_req->desc_list2;
  desc = (T_desc2 *) desc_list->first;
  ip_header = desc->buffer;
  dest_addr = GET_IP_SOURCE_ADDR (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  header_len_b = (USHORT) GET_IP_HEADER_LEN_B (ip_header);

  /* Up to 8 bytes payload of the old datagram */

  {
    USHORT help = (USHORT) (LEN_PAYLOAD + header_len_b);
    if (help < desc->len)
      set_desc_len (desc_list, desc, help);
  }

  /* Throw away the rest descs if there are any */

  del_rest_descs (desc_list, desc);

  /* Make a new desc for the ICMP packet header */

  malloc_len = LEN_ICMP_HEADER_PAYLOAD + LEN_IP_HEADER_B;
  make_new_desc (& desc_new, malloc_len, TRUE);

  /* Put the desc at the first place of the descs */

  put_desc_first_pos (desc_list, desc_new);

  /* Build the ICMP packet and the IP header */

  ip_header = desc_new->buffer;
  header_len_b = LEN_IP_HEADER_B;

  SET_ICMP_TYPE (ip_header, icmp_type, header_len_b);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  SET_ICMP_CODE (ip_header, icmp_code, header_len_b);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  RESET_ICMP_CHK_SUM (ip_header, header_len_b);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  chk_sum = desc_checksum (desc_list, header_len_b, 0);
  SET_ICMP_CHK_SUM (ip_header, chk_sum, header_len_b);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

  build_ip_header (
    ip_header,
    identity,
    MIN_HEADER_LEN,
    NORMAL_SERVICE,
    ttl,
    src_addr,
    dest_addr,
    desc_list->list_len,
    NO_OFFSET_FRAG,
    FLAG_NOT_SET,
    FLAG_NOT_SET,
    ICMP_PROT
  );
}

/*
+--------------------------------------------------------------------+
| PROJECT : WAP                       MODULE  : IP_KERF              |
| STATE   : code                      ROUTINE : init_ip              |
+--------------------------------------------------------------------+
 *
 * Init the global parameter
 */
void init_ip (void)
{
  T_HILA * p_ul = & ip_data->hila;
  T_LOLA * p_dl = & ip_data->lola;
  T_KER * p_ker = & ip_data->ker;
  USHORT j;

  INIT_STATE (HILA, DOWN);
  INIT_STATE (LOLA, DOWN);
  INIT_STATE (KER, DEACTIVATED);

  p_ul->dti_data_req = NULL;
  p_ul->drop_packet = FALSE;
  p_ul->ttl = STANDARD_TTL;
  p_ul->header_len = MIN_HEADER_LEN;
  p_ul->state_segment = NO_SEGMENTS;
  p_ul->segment_offset = 0;
  p_ul->segment_prot = NO_PROT_ID;
  p_ul->first_desc_segment = NULL;
  p_ul->identity = IDENTITY_0;

  p_dl->drop_packet = FALSE;
  p_dl->dti_data_ind= NULL;
  p_dl->pos_server = 0;

  for (j = 0; j < MAX_SEGM_SERVER; j++)
  {
    p_dl->state_reassembly[j] = NO_SEGMENTS;
    p_dl->got_first_segment[j] = FALSE;
    p_dl->got_last_segment[j] = FALSE;
    p_dl->ip_source_addr_segment[j] = NO_ADDR;
    p_dl->data_ind_reassembly[j] = NULL;
    p_dl->id_reassemble[j] = NO_ID_REASSBL;
    p_dl->prot_reassemble[j] = NO_PROT_REASSBL;
  }

  p_ker->peer_addr = NO_ADDR;
  p_ker->netmask = INIT_NETMASK;
  p_ker->source_addr = NO_ADDR;
  p_ker->dst_addr = NO_ADDR;

#ifdef _SIMULATION_
  p_ker->source_addr = TEST_SRC_ADDR;
  p_ker->dst_addr = TEST_DEST_ADDR;
#endif

  p_ker->entity_name_hl[0] = 0;
  p_ker->entity_name_ll[0] = 0;

  p_ker->link_id_ll = IPA_LINK_ID_DEFAULT;
  p_ker->link_id_hl = IPA_LINK_ID_DEFAULT;

  p_ker->mtu = NO_MTU;
  p_ker->icmp_dti_data_req = NULL;
  p_ker->send_icmp = FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)            MODULE  : IP_KERF              |
| STATE   : code                      ROUTINE : config_down_ll       |
+--------------------------------------------------------------------+
 *
 * Config down a lower layer entity
 */
void config_down_ll (void)
{
  T_HILA * p_ul = & ip_data->hila;
  T_LOLA * p_dl = & ip_data->lola;
  T_KER * p_ker = & ip_data->ker;

  USHORT j;

  TRACE_FUNCTION ("config_down_ll()");

  free_primitive_data_ind (& p_dl->dti_data_ind);
  free_primitive_data_req (& p_ker->icmp_dti_data_req);

  for (j = 0; j < MAX_SEGM_SERVER; j++)
  {
    /* free_primitive_data_ind (p_dl->data_ind_reassembly + j); */
    /* Has already been freed five lines above, */
    /* because p_dl->dti_data_ind == p_dl->data_ind_reassembly. */
    /* Please think anew when MAX_SEGM_SERVER becomes != 1. */

    csf_stop_timer (j);
    p_dl->timer_reass_running[j] = FALSE;

    p_dl->state_reassembly[j] = NO_SEGMENTS;
    p_dl->got_first_segment[j] = FALSE;
    p_dl->got_last_segment[j] = FALSE;
    p_dl->ip_source_addr_segment[j] = NO_ADDR;
    p_dl->data_ind_reassembly[j] = NULL;
    p_dl->id_reassemble[j] = NO_ID_REASSBL;
    p_dl->prot_reassemble[j] = NO_PROT_REASSBL;
  }

  /* Keep STATE_WAIT are stored even if config down
   * send READY_IND and set HILA STATE to IDLE if */

  switch (GET_STATE (HILA)) {
  case WAIT: /* Keep state */
    break;
  case SEND: /* Send ready indication */
    dti_start (
      ip_hDTI,
      IP_DTI_DEF_INSTANCE,
      IP_DTI_HL_INTERFACE,
      IP_DTI_DEF_CHANNEL
    );
  default:
    SET_STATE (HILA, IDLE);
    break;
  }
  SET_STATE (HILA, IDLE)

  p_ul->drop_packet = FALSE;
  p_ul->state_segment = NO_SEGMENTS;
  p_ul->header_len = MIN_HEADER_LEN;
  p_ul->ttl = STANDARD_TTL;
  p_ul->segment_prot = NO_PROT_ID;
  p_ul->segment_offset = 0;
  p_ul->first_desc_segment = NULL;

  /* Be sure that STATE_WAIT are stored even if config down */

  if (GET_STATE (LOLA) NEQ WAIT)
    SET_STATE (LOLA, IDLE)
  p_dl->drop_packet = FALSE;

  p_dl->pos_server = 0;

  p_ker->peer_addr = NO_ADDR;
  p_ker->netmask = INIT_NETMASK;
  p_ker->source_addr = NO_ADDR;
  p_ker->dst_addr = NO_ADDR;
  p_ker->entity_name_ll[0] = 0;
  p_ker->mtu = NO_MTU;
  p_ker->send_icmp = FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)            MODULE  : IP_KERF              |
| Sp_TATE   : code                      ROUTINE : terminate_ip       |
+--------------------------------------------------------------------+
 *
 * Handle global parameter by terminate
 */
void terminate_ip (void)
{
  TRACE_FUNCTION ("terminate_ip()");

  /* Shutdown lower entity interface */
  config_down_ll ();

  /* Parameter for higher layer entity */
  ip_data->ker.entity_name_hl[0] = 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)            MODULE  : IP_KERF              |
| STATE   : code                      ROUTINE : ip_addr_int_to_byte  |
+--------------------------------------------------------------------+
 *
 * Build the IP address in ULONG to 4 bytes
 */
void ip_addr_int_to_byte (UBYTE * b_values, ULONG ul_value)
{
  TRACE_FUNCTION ("int_to_byte()");

  b_values[3] = (UBYTE) (ul_value);
  b_values[2] = (UBYTE) (ul_value>>8);
  b_values[1] = (UBYTE) (ul_value>>16);
  b_values[0] = (UBYTE) (ul_value>>24);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)            MODULE  : IP_KERF              |
| STATE   : code                      ROUTINE : chk_packet_len       |
+--------------------------------------------------------------------+
 *
 * Check the IP packet length
 */
UBYTE chk_packet_len (UBYTE * ip_header, T_desc_list2 * desc_list)
{
  USHORT total_len, chk_len, mtu_len;

  TRACE_FUNCTION ("chk_packet_len()");

  /* This is the calculated length */
  total_len = desc_list->list_len;

  /* This is the length indicated in the IP header */
  chk_len = (USHORT) GET_IP_TOTAL_LEN (ip_header);

  /* This is the max defined packet length */
  mtu_len = ip_data->ker.mtu;

  if (
    (chk_len < MIN_HEADER_LEN) OR (chk_len > total_len) OR
    (total_len < MIN_HEADER_LEN) OR (chk_len > mtu_len)
  )
    return ERR_PACKET_LEN;
  else if (chk_len < total_len)
    return CHANGE_PACKET_LEN;
  else
    return NO_ERROR;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)            MODULE  : IP_KERF              |
| STATE   : code                      ROUTINE : build_ip_packet      |
+--------------------------------------------------------------------+
 *
 * Build the datagram or packets before sending
 */
void build_ip_packet (BOOL uplink, UBYTE select)
{
  UBYTE * ip_header;
  USHORT total_len;
  ULONG dest_addr, src_addr;
  T_desc_list2 * desc_list;
  T_desc2 * desc;
  T_HILA * p_ul = & ip_data->hila;
  T_LOLA * p_dl = & ip_data->lola;
  T_KER * p_ker = & ip_data->ker;

  T_DTI2_DATA_REQ * data_req;
  T_DTI2_DATA_IND * data_ind;

  TRACE_FUNCTION ("build_ip_packet()");

  src_addr = p_ker->source_addr;

  /* Build uplink packets */

  if (uplink)
  {
    data_req = p_ul->dti_data_req;

    switch (select) {
    /* Build "standard" IP packet */
    case B_NORMAL_PACKET:

      desc_list = & p_ul->dti_data_req->desc_list2;
      total_len = desc_list->list_len;
      desc = (T_desc2 *) desc_list->first;
      ip_header = desc->buffer;
      dest_addr = GET_IP_DEST_ADDR (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

      build_ip_header (
        ip_header,
        p_ul->identity,
        (UBYTE) p_ul->header_len,
        NORMAL_SERVICE,
        p_ul->ttl,
        src_addr,
        dest_addr,
        total_len,
        NO_OFFSET_FRAG,
        FLAG_NOT_SET,
        FLAG_NOT_SET,
        UDP_PROT
      );
#ifndef _SIMULATION_
      p_ul->identity++;
#endif
      break;

    /* Build IP header for ICMP messages. Note only ICMP from higher layer. */
    case B_ICMP_PACKET:

      desc_list = & p_ul->dti_data_req->desc_list2;
      total_len = desc_list->list_len;
      desc = (T_desc2 *) desc_list->first;
      ip_header = desc->buffer;
      dest_addr = GET_IP_DEST_ADDR (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

      build_ip_header (
        ip_header,
        p_ul->identity,
        (UBYTE) p_ul->header_len,
        NORMAL_SERVICE,
        p_ul->ttl,
        src_addr,
        dest_addr,
        total_len,
        NO_OFFSET_FRAG,
        FLAG_NOT_SET,
        FLAG_NOT_SET,
        ICMP_PROT
      );
#ifndef _SIMULATION_
      p_ul->identity++;
#endif
      break;

    /* Build fragments. The first fragment use the original header from HL. */
    case B_SEGMENT:
      {
        /* Check if it is the first fragment */

        if (p_ul->state_segment EQ NO_SEGMENTS)
        {
          T_desc2 * desc_new;
          USHORT pos_copy, total_len_copy, malloc_len, header_len_b;

          desc_list = & p_ul->dti_data_req->desc_list2;
          total_len = desc_list->list_len;
          desc = (T_desc2 *) desc_list->first;
          ip_header = desc->buffer;
          dest_addr = GET_IP_DEST_ADDR (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
          src_addr = p_ker->source_addr;
          p_ul->header_len = (USHORT) GET_IP_HEADER_LEN (ip_header);
          header_len_b = (USHORT) GET_IP_HEADER_LEN_B (ip_header);

          p_ul->state_segment = SEND_SEGMENT;
          p_ul->sended_segment_len = p_ker->mtu;
          p_ul->list_len_segment = total_len;
          p_ul->segment_prot = GET_IP_PROT (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
          p_ul->first_desc_segment = desc;
          dest_addr = GET_IP_DEST_ADDR (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

          /* Make a new desc for the fragment */

          malloc_len = p_ker->mtu;
          make_new_desc (& desc_new, malloc_len, FALSE);

          data_req->desc_list2.first = (ULONG) desc_new;
          data_req->desc_list2.list_len = malloc_len;
          desc_new->next = 0;

          /* Build the first fragment */
          copy_from_descs_to_desc (
            & desc, desc_new, malloc_len, 0, 0, & pos_copy, & total_len_copy
          );
          if (total_len_copy NEQ malloc_len)
          {
            /* Corrupted packet -> drop it */
            p_ul->drop_packet = TRUE;

            /* Clean up and free the descs */
            del_descs (p_ul->first_desc_segment);
            p_ul->segment_offset = 0;
            p_ul->next_segment_desc = 0;
            p_ul->last_segment_pos = 0;
            p_ul->sended_segment_len = 0;
            p_ul->state_segment = NO_SEGMENTS;
          }
          else
          {
            p_ul->segment_id = p_ul->identity;
            p_ul->segment_offset = 0;

            build_ip_header (
              desc_new->buffer,
              p_ul->segment_id,
              (UBYTE) p_ul->header_len,
              NORMAL_SERVICE,
              p_ul->ttl,
              src_addr,
              dest_addr,
              malloc_len,
              (USHORT) (p_ul->segment_offset >> 3),
              FLAG_NOT_SET,
              FLAG_SET,
              p_ul->segment_prot
            );
#ifndef _SIMULATION_
            p_ul->identity++;
#endif

            /* For the next fragment */
            p_ul->segment_offset = (USHORT) (malloc_len - header_len_b);
            p_ul->next_segment_desc = (ULONG) desc;
            p_ul->last_segment_pos = pos_copy;
          }
        }

        /* Middle fragment */

        else if (p_ul->sended_segment_len + p_ker->mtu <
                 p_ul->list_len_segment + LEN_IP_HEADER_B)
        {
          T_desc2 * desc_new;

          USHORT pos_copy, total_len_copy, malloc_len;

          /* Make a new primitive for the fragment */

          PALLOC (dti_data_req, DTI2_DATA_REQ);
          p_ul->dti_data_req = dti_data_req;

          /* Malloc the fragment desc */

          malloc_len = p_ker->mtu;
          make_new_desc (& desc_new, malloc_len, FALSE);

          dti_data_req->desc_list2.first = (ULONG) desc_new;
          dti_data_req->desc_list2.list_len = malloc_len;
          desc_new->next = 0;

          /* Copy the data into the fragment desc */

          desc = (T_desc2 *) p_ul->next_segment_desc;
          copy_from_descs_to_desc (
            & desc,
            desc_new,
            (USHORT) (malloc_len - LEN_IP_HEADER_B),
            p_ul->last_segment_pos,
            LEN_IP_HEADER_B,
            & pos_copy,
            & total_len_copy
          );

          if (total_len_copy NEQ malloc_len - LEN_IP_HEADER_B)
          {
            /* Corrupted packet -> drop it */
            p_ul->drop_packet = TRUE;

            /* Clean up and free the descs */
            del_descs (p_ul->first_desc_segment);
            p_ul->segment_offset = 0;
            p_ul->next_segment_desc = 0;
            p_ul->last_segment_pos = 0;
            p_ul->sended_segment_len = 0;
            p_ul->state_segment = NO_SEGMENTS;
          }
          else
          {
            /* Build the IP fragment */

            UBYTE * ip_header_first = p_ul->first_desc_segment->buffer;
            dest_addr = GET_IP_DEST_ADDR (ip_header_first);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

            build_ip_header (
              desc_new->buffer,
              p_ul->segment_id,
              MIN_HEADER_LEN,
              NORMAL_SERVICE,
              p_ul->ttl,
              src_addr,
              dest_addr,
              malloc_len,
              (USHORT) (p_ul->segment_offset >> 3),
              FLAG_NOT_SET,
              FLAG_SET,
              p_ul->segment_prot
            );

            /* For the next fragment */
            p_ul->segment_offset = (USHORT)
              (p_ul->segment_offset + malloc_len - LEN_IP_HEADER_B);
            p_ul->next_segment_desc = (ULONG) desc;
            p_ul->last_segment_pos = pos_copy;
            p_ul->sended_segment_len = (USHORT)
              (p_ul->sended_segment_len + malloc_len - LEN_IP_HEADER_B);
          }
        }
        else /* Last fragment */
        {
          T_desc2 * desc_new;

          USHORT pos_copy, total_len_copy, malloc_len;

          /* Make a new primitive for the fragment */

          PALLOC (dti_data_req, DTI2_DATA_REQ);
          p_ul->dti_data_req = dti_data_req;

          /* Calculate the correct len for fragment desc and malloc */

          malloc_len = (USHORT) (LEN_IP_HEADER_B +
            p_ul->list_len_segment - p_ul->sended_segment_len);
          make_new_desc (& desc_new, malloc_len, FALSE);

          dti_data_req->desc_list2.first = (ULONG) desc_new;
          dti_data_req->desc_list2.list_len = malloc_len;
          desc_new->next = 0;

          /* Copy the data into the fragment desc */

          desc = (T_desc2 *) p_ul->next_segment_desc;

          copy_from_descs_to_desc (
            & desc,
            desc_new,
            (USHORT) (malloc_len - LEN_IP_HEADER_B),
            p_ul->last_segment_pos,
            LEN_IP_HEADER_B,
            & pos_copy,
            & total_len_copy
          );

          if (total_len_copy NEQ malloc_len - LEN_IP_HEADER_B)
          {
            /* Corrupted packet -> drop it */
            p_ul->drop_packet = TRUE;

            /* Clean up and free the descs */
            del_descs (p_ul->first_desc_segment);
            p_ul->segment_offset = 0;
            p_ul->next_segment_desc = 0;
            p_ul->last_segment_pos = 0;
            p_ul->sended_segment_len = 0;
            p_ul->state_segment = NO_SEGMENTS;
          }
          else
          {
            /* Build the fragment header */

            UBYTE * ip_header_first = p_ul->first_desc_segment->buffer;
            dest_addr = GET_IP_DEST_ADDR (ip_header_first);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

            build_ip_header (
              desc_new->buffer,
              p_ul->segment_id,
              MIN_HEADER_LEN,
              NORMAL_SERVICE,
              p_ul->ttl,
              src_addr,
              dest_addr,
              malloc_len,
              (USHORT) (p_ul->segment_offset >> 3),
              FLAG_NOT_SET,
              FLAG_NOT_SET,
              p_ul->segment_prot
            );

            /* Clean up and free the descs */

            del_descs (p_ul->first_desc_segment);

            p_ul->segment_offset = 0;
            p_ul->next_segment_desc = 0;
            p_ul->last_segment_pos = 0;
            p_ul->sended_segment_len = 0;
            p_ul->state_segment = NO_SEGMENTS;
          }
        }
      }
      break;

    default:
      break;
    }
  }
  else
  {
    data_ind = p_dl->dti_data_ind;

    /* Compute DL packets */

    switch (select) {
    /* Ping request */
    case B_ICMP_ECHO_REPLY:
      {
        USHORT header_len_b;

        PPASS (data_ind, data_request, DTI2_DATA_REQ);
        desc_list = & data_request->desc_list2;
        desc = (T_desc2 *) desc_list->first;
        ip_header = desc->buffer;
        header_len_b = (USHORT) GET_IP_HEADER_LEN_B (ip_header);

        dest_addr = GET_IP_SOURCE_ADDR (ip_header);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

        build_icmp_packet (
          (UBYTE) header_len_b,
          ICMP_TYP_ECHO_REPLY,
          ICMP_CODE_ECHO_REPLY,
          p_ul->ttl,
          ip_header,
          p_ul->identity,
          dest_addr,
          src_addr,
          desc_list
        );
#ifndef _SIMULATION_
        p_ul->identity++;
#endif
        p_ker->icmp_dti_data_req = data_request;
      }
      break;

    case B_ICMP_REASSEMBLE_TIMEOUT:
      {
        /* Datagram for ICMP - reassembly - message */

        data_ind = p_dl->data_ind_reassembly[p_dl->pos_server];

        {
          PPASS (data_ind, data_request, DTI2_DATA_REQ);

          build_icmp_with_payload (
            data_request,
            p_ul->identity,
            p_ul->ttl,
            p_ker->source_addr,
            ICMP_TYP_TIME_EXCEDED,
            ICMP_CODE_FRAGM_TIME_EXC
          );
#ifndef _SIMULATION_
          p_ul->identity++;
#endif
          p_ker->icmp_dti_data_req = data_request;
        }
      }
      break;

    case B_ICMP_NO_FORWARD:
      {
        /* No destination address - build ICMP frame */

        PPASS (data_ind, data_request, DTI2_DATA_REQ);

        build_icmp_with_payload (
          data_request,
          p_ul->identity,
          p_ul->ttl,
          p_ker->source_addr,
          ICMP_TYP_DEST_URECHBL,
          ICMP_CODE_NO_HOST
        );
#ifndef _SIMULATION_
        p_ul->identity++;
#endif
        p_ker->icmp_dti_data_req = data_request;
      }
      break;

    default:
      break;
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8444)            MODULE  : IP_KERF              |
| STATE   : code                      ROUTINE : check_ip_address     |
+--------------------------------------------------------------------+
 *
 * Check the IP address
 */
void check_ip_address (
  BOOL * addr_type_dest,
  BOOL * addr_type_src,
  ULONG dest_addr,
  ULONG src_addr
) {
  UBYTE i;
  UBYTE b_dest_addr[4], b_src_addr[4], first_nibbl_dest, first_nibbl_src;

#define BCAST     0xFF
#define LOOP_BACK 0x7F
#define DEFAULT_ROUTE 0
#define CLASS_A   0x0
#define CLASS_B   0x2
#define CLASS_C   0x6
#define MULTICAST 0xE
#define CLASS_E   0xF
#define M_CAST_FF 0xFFFFFFFF

  TRACE_FUNCTION ("check_ip_address()");

  ip_addr_int_to_byte (b_dest_addr, dest_addr);
  ip_addr_int_to_byte (b_src_addr, src_addr);

  first_nibbl_dest = (UBYTE) (b_dest_addr[0] >> 4);
  first_nibbl_src = (UBYTE) (b_src_addr[0] >> 4);

  for (i=0; i < MAX_ADDR_TYPES; i++)
  {
    addr_type_dest[i] = FALSE;
    addr_type_src[i] = FALSE;
  }

  /* Check if broadcast address */

  if (dest_addr EQ M_CAST_FF)
    addr_type_dest[BCAST_ADDR_255] = TRUE;

  if (src_addr EQ M_CAST_FF)
    addr_type_src[BCAST_ADDR_255] = TRUE;

  /* Correct destination address? */

  if (dest_addr NEQ src_addr)
  {
    addr_type_dest[NO_DEST_ADDR] = TRUE;
    addr_type_src[NO_DEST_ADDR] = TRUE;
  }

  /* Loop-back address? */

  if (b_dest_addr[0] EQ LOOP_BACK)
    addr_type_dest[LOOP_BACK_ADDR] = TRUE;

  if (b_src_addr[0] EQ LOOP_BACK)
    addr_type_src[LOOP_BACK_ADDR] = TRUE;

  /* Check kind of class */

  if ((first_nibbl_src >> 3) EQ CLASS_A)
    addr_type_src[CLASS_A_ADDR] = TRUE;

  if ((first_nibbl_dest >> 3) EQ CLASS_A)
    addr_type_dest[CLASS_A_ADDR] = TRUE;

  if ((first_nibbl_src >> 2) EQ CLASS_B)
    addr_type_src[CLASS_B_ADDR] = TRUE;

  if ((first_nibbl_dest >> 2) EQ CLASS_B)
    addr_type_dest[CLASS_B_ADDR] = TRUE;

  if ((first_nibbl_src >> 1) EQ CLASS_C)
    addr_type_src[CLASS_C_ADDR] = TRUE;

  if ((first_nibbl_dest >> 1) EQ CLASS_C)
    addr_type_dest[CLASS_C_ADDR] = TRUE;

  if (first_nibbl_src EQ CLASS_E AND dest_addr NEQ M_CAST_FF)
    addr_type_src[CLASS_E_ADDR] = TRUE;

  if (first_nibbl_dest EQ CLASS_E AND src_addr NEQ M_CAST_FF)
    addr_type_dest[CLASS_E_ADDR] = TRUE;

  /* Multicast or class D */

  if (first_nibbl_dest EQ MULTICAST)
    addr_type_dest[MCAST_ADDR] = TRUE;

  if (first_nibbl_src EQ MULTICAST)
    addr_type_src[MCAST_ADDR] = TRUE;

  /* Default route */

  if (dest_addr EQ DEFAULT_ROUTE)
    addr_type_dest[DEFAULT_R_ADDR] = TRUE;

  if (src_addr EQ DEFAULT_ROUTE)
    addr_type_src[DEFAULT_R_ADDR] = TRUE;

  /* Bad address? */

  if (
    addr_type_src[BCAST_ADDR_255] OR
    addr_type_src[LOOP_BACK_ADDR] OR
    addr_type_src[DEFAULT_R_ADDR]
  ) {
    addr_type_src[BAD_UL_SRC_ADDR] = TRUE;
    addr_type_src[BAD_DL_SRC_ADDR] = TRUE;
  }

  if (
    addr_type_dest[BCAST_ADDR_255] OR
    addr_type_dest[LOOP_BACK_ADDR] OR
    addr_type_dest[DEFAULT_R_ADDR]
  ) {
    addr_type_dest[BAD_UL_DEST_ADDR] = TRUE;
    addr_type_dest[BAD_DL_DEST_ADDR] = TRUE;
  }
}

/* Internet checksum calculations as needed in IP and UDP.
 * See RFC 1071 for details.
 *
 * USHORT inet_checksum (UBYTE * block, USHORT len)
 * Computes the Internet checksum over a simple data block.
 *
 * USHORT desc_checksum (
 *   T_desc_list2 * dlist,
 *   USHORT start_offset,
 *   ULONG start_value
 * )
 * Computes the Internet checksum over a DTI descriptor list,
 * beginning at start_offset and with start_value. */

#define LITTLE_ENDIAN /* Change this for big-endian mode. */

/** Computes the Internet checksum [RFC 1071] over a simple data block.
 *
 * @param block              pointer to data block
 * @param len                length of the block in octets
 * @return the checksum
 */
USHORT inet_checksum (UBYTE * block, USHORT len)
{
  BOOL have_leftover = len % 2; /* If non-zero, there is a leftover
                                 * octet at the end of the (odd-sized)
                                 * data block. */
  ULONG value;                  /* Value to add. (Since we know
                                 * nothing about the alignment of the
                                 * block, we can't read the USHORTs
                                 * directly from the block.) */
  ULONG checksum = 0;           /* Checksum accumulator. */

  len >>= 1;                    /* Count words now, not octets. */

  while (len--)
  {
    /* Because we must read the data bytewise, we cannot profit from
     * the independency of endianness of the original algorithm.
     * That means that we have to make two different cases for big
     * endian and little endian. */

#if defined LITTLE_ENDIAN
    /*lint -e{661} (Warning -- access of out-of-bounds pointer) */
    value = block[1]<<8 | block[0];
#elif defined BIG_ENDIAN
    /*lint -e{662} (Warning -- creation of out-of-bounds pointer) */
    value = block[0]<<8 | block[1];
#else
    #error "LITTLE_ENDIAN or BIG_ENDIAN must be defined."
#endif

    checksum += value;
    /*lint -e{662} (Warning -- creation of out-of-bounds pointer) */
    block +=2;
  }

  if (have_leftover)
    /*lint -e{661} (Warning -- access of out-of-bounds pointer) */  	
    checksum += *block;

  /* Fold into 16 bits. */
  while (checksum >> 16)
    checksum = (checksum & 0xffff) + (checksum >> 16);

  return (USHORT) ~checksum;
}

/** Computes a part of an Internet checksum over a data block. A
 * leftover octet from a previous partial calculation is taken into
 * account. If an octet is left over, it is returned as well as the
 * fact that there is a leftover octet. This function is intended to
 * be called only by desc_checksum() and partial_checksum().
 *
 * @param block              pointer to data block
 * @param len                length of the block in octets
 * @param sum                checksum value to begin with
 * @param have_leftover_ptr  if non-zero, a leftover octet is in *leftover_ptr
 * @param leftover_ptr       pointer to leftover octet; valid on input and
 *                           output iff *have_leftover_ptr
 * @return the part of the sum calculated
 */
static ULONG checksum_block_part (
  UBYTE * block,
  USHORT len,
  ULONG sum,
  BOOL * have_leftover_ptr,
  UBYTE * leftover_ptr
) {
  /* This function is as complicated as it is for two reasons:
   *
   * (a) Each block may have an even or odd number of octets. Because
   * this checksum is 16-bit based, an octet may be left over from the
   * previous calculation and must be taken into account. Also in this
   * calculation an octet may be left over. This fact and the value of
   * the octet must be made known to the caller.
   *
   * (b) We must not make any assumptions about the alignment of the
   * block. Therefore, in order not to cause alignment problems, all
   * 16-bit values must be read bytewise. */

  ULONG value; /* 16-bit value to be summed up */

  TRACE_FUNCTION ("checksum_block_part()");

  /* Previous calculation may have left over an octet. */
  if (*have_leftover_ptr)
  {
    if (len == 0)
      return sum;

    /* See comment in inet_checksum() above for an explanation. */

#if defined LITTLE_ENDIAN
    value = (*block++ << 8) | *leftover_ptr;
#elif defined BIG_ENDIAN
    value = (*leftover_ptr << 8) | *block++;
#else
    #error "LITTLE_ENDIAN or BIG_ENDIAN must be defined."
#endif

    len--;
    sum += value;
  }

  /* Main loop over word values. */
  *have_leftover_ptr = len % 2;
  len >>= 1;
  while (len--)
  {
    /* See comment in inet_checksum() above for an explanation. */

#if defined LITTLE_ENDIAN
    value = block[1]<<8 | block[0];
#elif defined BIG_ENDIAN
    value = block[0]<<8 | block[1];
#else
    #error "LITTLE_ENDIAN or BIG_ENDIAN must be defined."
#endif

    sum += value;
    block +=2;
  }

  /* Check for leftover octet. */
  if (*have_leftover_ptr)
    *leftover_ptr = *block;

  return sum;
}

/** Compute a partial Internet checksum to be used as a astart_value
 * for desc_checksum. The block must have even length.
 *
 * @param block    data block to compute the checksum over
 * @param len      length of the block
 * @return the partial sum calculated
 */
ULONG partial_checksum (UBYTE * block, USHORT len)
{
  BOOL dummy1 = 0; /* Needed to call checksum_block_part(). */
  UBYTE dummy2 = 0;

  return checksum_block_part (block, len, 0, & dummy1, & dummy2);
}

/** Computes the Internet checksum over a DTI descriptor list. There
 * may be a value from a previous partial calculation that is added to
 * the sum as a start value. The function relies on the data length
 * being at least start_offset.
 *
 * @param dlist          descriptor list containing the data
 * @param start_offset   beginning position of interesting data
 * @param start_value    value from previous partial checksum calculation
 * @return the checksum value
 */
USHORT desc_checksum (
  T_desc_list2 * dlist,
  USHORT start_offset,
  ULONG start_value
) {
  ULONG checksum = start_value; /* The checksum to be calculated. */
  T_desc2 * desc_p;             /* Pointer to current descriptor. */
  BOOL have_leftover = 0;       /* True iff we have a leftover octet
                                 * from the previous partial sum. */
  UBYTE leftover_octet = 0;     /* An octet left over from the
                                 * previous partial sum. */

  TRACE_FUNCTION ("desc_checksum()");

  /* Calculating the sum of the first buffer, we have to take the
   * start offset into account. This includes finding the descriptor
   * the offset lies in. */

  desc_p = (T_desc2 *) dlist->first;
  while (start_offset > desc_p->len)
  {
    start_offset = (USHORT) (start_offset - desc_p->len);
    desc_p = (T_desc2 *) desc_p->next;
  }
  checksum = checksum_block_part (
    desc_p->buffer + start_offset,
    (USHORT) (desc_p->len - start_offset),
    checksum,
    & have_leftover,
    & leftover_octet
  );

  /* Now loop over the other descriptors. */
  for (
    desc_p = (T_desc2 *) desc_p->next;
    desc_p != 0;
    desc_p = (T_desc2 *) desc_p->next
  ) {
    checksum = checksum_block_part (
      desc_p->buffer,
      desc_p->len,
      checksum,
      & have_leftover,
      & leftover_octet
    );
  }

  if (have_leftover)
    checksum += leftover_octet;

  /* Fold into 16 bits. */
  while (checksum >> 16)
    checksum = (checksum & 0xffff) + (checksum >> 16);

  return (USHORT) ~checksum;
}

/*-------------------------------------------------------------------------*/

