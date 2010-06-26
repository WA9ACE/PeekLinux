/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  SM
+-----------------------------------------------------------------------------
|             Copyright 2003 Texas Instruments.
|             All rights reserved. 
| 
|             This file is confidential and a trade secret of Texas 
|             Instruments .
|             The receipt of or possession of this file does not convey 
|             any rights to reproduce or disclose its contents or to 
|             manufacture, use, or sell anything it may describe, in 
|             whole, or in part, without the specific written consent of 
|             Texas Instruments. 
+-----------------------------------------------------------------------------
| Purpose:    3G TFT utility functions implementation in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES ============================================================*/

#include "sm.h"

#include "sm_tft.h"

/*==== CONST ===============================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_count_tfts_in_mask
+------------------------------------------------------------------------------
| Description : Returns the number of filters (set bits) in a TFT mask.
|
| Parameters  : tft_mask    - TFT mask
+------------------------------------------------------------------------------
*/
static U8 sm_tft_count_tfts_in_mask(U8 tft_mask) /*@*/
{
  U8 count_array[16] = {
    (U8)0 /* 0000 */, (U8)1 /* 0001 */, (U8)1 /* 0010 */, (U8)2 /* 0011 */,
    (U8)1 /* 0100 */, (U8)2 /* 0101 */, (U8)2 /* 0110 */, (U8)3 /* 0111 */,
    (U8)1 /* 1000 */, (U8)2 /* 1001 */, (U8)2 /* 1010 */, (U8)3 /* 1011 */,
    (U8)2 /* 1100 */, (U8)3 /* 1101 */, (U8)3 /* 1110 */, (U8)4 /* 1111 */
  };
  return (count_array[(U16)tft_mask >> 4] + count_array[(U16)tft_mask & 15]);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_is_tft_in_mask
+------------------------------------------------------------------------------
| Description : Returns TRUE if the TFT with the specified ID (bit) is
|               set in the specified TFT mask.
|
| Parameters  : id          - TFT identifier
|               tft_mask    - TFT mask
+------------------------------------------------------------------------------
*/
static BOOL sm_tft_is_tft_in_mask(U8 id, U8 tft_mask) /*@*/
{
  return ((tft_mask & (1UL << id)) != 0);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_is_port_range
+------------------------------------------------------------------------------
| Description : Query function returning TRUE if high_limit is not zero, and
|               high_limit is greater than (but not equal) to low_limit.
|
| Parameters  : low_limit   - lower port limit
|               high_limit  - upper port limit
+------------------------------------------------------------------------------
*/
static BOOL sm_tft_is_port_range(U16 low_limit, U16 high_limit)
{
  return (high_limit != 0 && low_limit < high_limit);
}

static /*@exposed@*/T_NAS_tft_pf *
sm_tft_get_active_tft_by_id(/*@returned@*/ /*@partial@*/struct T_SM_CONTEXT_DATA *context,
			    U8 id)
{
  TRACE_ASSERT(id < (U8)NAS_SIZE_TFT_FILTER);
  TRACE_ASSERT(context->active_tft.ptr_tft_pf != NULL);
  return (&context->active_tft.ptr_tft_pf[id]);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_pf_size
+------------------------------------------------------------------------------
| Description : Query function returning the packed size of a packet filter.
|
| Parameters  : tft_pf      - packet filter
+------------------------------------------------------------------------------
*/
static U16 sm_tft_pf_size(T_NAS_tft_pf *tft_pf)
{
  U16 tft_pf_size = (U16)3; /* Filter ID, precedence and length octet mandatory */

  U16 lower3bits[8] = {
  (U16)   0, /* 0x00: None */
  (U16)   9, /* 0x01: IPv4_SRC_ADDR_MASK */
  (U16)  33, /* 0x02: IPv6_SRC_ADDR_MASK */
  (U16)   0, /* 0x03: IPv4_SRC_ADDR_MASK + IPv6_SRC_ADDR_MASK -- INVALID! */
  (U16)   3, /* 0x04: PROTOCOL_OR_NEXT_HDR */
  (U16)  12, /* 0x05: IPv4_SRC_ADDR_MASK + PROTOCOL_OR_NEXT_HDR */
  (U16)  36, /* 0x06: IPv6_SRC_ADDR_MASK + PROTOCOL_OR_NEXT_HDR */
  (U16)   0  /* 0x07: IPv4_SRC_ADDR_MASK + IPv6_SRC_ADDR_MASK + PROTOCOL_OR_NEXT_HDR -- INVALID! */
  };
  U16 upper3bits[8] = {
  (U16)   0, /* 0x00: None */
  (U16)   5, /* 0x20: IPSEC_SPI */
  (U16)   3, /* 0x40: TOS_AND_MASK */
  (U16)   5, /* 0x60: IPSEC_SPI + TOS_AND_MASK */
  (U16)   4, /* 0x80: FLOW_LABEL */
  (U16)   0, /* 0xa0: IPSEC_SPI + FLOW_LABEL - INVALID! */
  (U16)   7, /* 0xc0: TOS_AND_MASK + FLOW_LABEL */
  (U16)   0  /* 0xe0: IPSEC_SPI + TOS_AND_MASK + FLOW_LABEL - INVALID! */
  };

  if ((tft_pf->tft_pf_valid_bits & (U8)0x07) != (U8)0)
  {
    tft_pf_size   += lower3bits[(U16)tft_pf->tft_pf_valid_bits & 0x07];
  }

  if ((tft_pf->tft_pf_valid_bits & (U8)0xe0) != (U8)0)
  {
    tft_pf_size   += upper3bits[(U16)tft_pf->tft_pf_valid_bits >> 5];
  }

  if ((tft_pf->tft_pf_valid_bits & (U8)NAS_TFT_ID_DEST_PORT_RANGE) != (U8)0)
  {
    T_NAS_tft_dest_port_range *range = &tft_pf->tft_pf_entry.tft_pf_ipv4.tft_dest_port_range;
    if (sm_tft_is_port_range(range->low_limit, range->high_limit))
    {
      tft_pf_size += (U16)5;  /* Destination port range */
    } else {
      tft_pf_size += (U16)3;  /* Single destination port */
    }
  }

  if ((tft_pf->tft_pf_valid_bits & (U8)NAS_TFT_ID_SRC_PORT_RANGE) != (U8)0)
  {
    T_NAS_tft_src_port_range *range = &tft_pf->tft_pf_entry.tft_pf_ipv4.tft_src_port_range;
    if (sm_tft_is_port_range(range->low_limit, range->high_limit))
    {
      tft_pf_size += (U16)5;  /* Source port range */
    } else {
      tft_pf_size += (U16)3;  /* Single source port */
    }
  }

  return tft_pf_size;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_are_equal
+------------------------------------------------------------------------------
| Description : Query function returning TRUE if the two input packet filters
|               are equal (same parameters).
|
| Parameters  : tft1        - packet filter 1
|               tft2        - packet filter 2
+------------------------------------------------------------------------------
*/
static BOOL sm_tft_are_equal(T_NAS_tft_pf *tft1, T_NAS_tft_pf *tft2)
{
  U8 valid_bits;

  /* Compare outer-level parameters */
  if (tft1->tft_pf_id         != tft2->tft_pf_id ||
      tft1->tft_pf_precedence != tft2->tft_pf_precedence ||
      tft1->tft_pf_valid_bits != tft2->tft_pf_valid_bits ||
      tft1->ctrl_tft_pf_entry != tft2->ctrl_tft_pf_entry)
  {
    return FALSE;
  }

  valid_bits = tft1->tft_pf_valid_bits;

  if (tft1->ctrl_tft_pf_entry == NAS_is_tft_pf_ipv4) {
    T_NAS_tft_pf_ipv4 *v4_pf_1 = &tft1->tft_pf_entry.tft_pf_ipv4;
    T_NAS_tft_pf_ipv4 *v4_pf_2 = &tft2->tft_pf_entry.tft_pf_ipv4;

    if (((valid_bits & (U8)NAS_TFT_ID_PROTOCOL_OR_NEXT_HDR) != (U8)0
	    && v4_pf_1->tft_protocol != v4_pf_2->tft_protocol)
	|| ((valid_bits & (U8)NAS_TFT_ID_TOS_AND_MASK) != (U8)0
	    && v4_pf_1->tft_tos_and_mask.tos_value != v4_pf_2->tft_tos_and_mask.tos_value
	    && v4_pf_1->tft_tos_and_mask.tos_mask  != v4_pf_2->tft_tos_and_mask.tos_mask)
	|| ((valid_bits & (U8)NAS_TFT_ID_DEST_PORT_RANGE) != (U8)0
	    && memcmp(&v4_pf_1->tft_dest_port_range, &v4_pf_2->tft_dest_port_range, sizeof(T_NAS_tft_dest_port_range)) != 0)
	|| ((valid_bits & (U8)NAS_TFT_ID_SRC_PORT_RANGE) != (U8)0
	    && memcmp(&v4_pf_1->tft_src_port_range, &v4_pf_2->tft_src_port_range, sizeof(T_NAS_tft_src_port_range)) != 0)
	|| ((valid_bits & (U8)NAS_TFT_ID_IPSEC_SPI) != (U8)0
	    && v4_pf_1->tft_ipsec_spi != v4_pf_2->tft_ipsec_spi)
	|| ((valid_bits & (U8)NAS_TFT_ID_IPv4_SRC_ADDR_MASK) != (U8)0
	    && memcmp(&v4_pf_1->tft_ipv4_src_addr_mask, &v4_pf_2->tft_ipv4_src_addr_mask, sizeof(T_NAS_tft_ipv4_src_addr_mask)) != 0))
    {
      return FALSE;
    }
  } else if (tft1->ctrl_tft_pf_entry == NAS_is_tft_pf_ipv6)
  {
    T_NAS_tft_pf_ipv6 *v6_pf_1 = &tft1->tft_pf_entry.tft_pf_ipv6;
    T_NAS_tft_pf_ipv6 *v6_pf_2 = &tft2->tft_pf_entry.tft_pf_ipv6;

    if (((valid_bits & (U8)NAS_TFT_ID_PROTOCOL_OR_NEXT_HDR) != (U8)0
	    && v6_pf_1->tft_next_hdr != v6_pf_2->tft_next_hdr)
	|| ((valid_bits & (U8)NAS_TFT_ID_TOS_AND_MASK) != (U8)0
	    && v6_pf_1->tft_tos_and_mask.tos_value != v6_pf_2->tft_tos_and_mask.tos_value
	    && v6_pf_1->tft_tos_and_mask.tos_mask  != v6_pf_2->tft_tos_and_mask.tos_mask)
	|| ((valid_bits & (U8)NAS_TFT_ID_DEST_PORT_RANGE) != (U8)0
	    && memcmp(&v6_pf_1->tft_dest_port_range, &v6_pf_2->tft_dest_port_range, sizeof(T_NAS_tft_dest_port_range)) != 0)
	|| ((valid_bits & (U8)NAS_TFT_ID_SRC_PORT_RANGE) != (U8)0
	    && memcmp(&v6_pf_1->tft_src_port_range, &v6_pf_2->tft_src_port_range, sizeof(T_NAS_tft_src_port_range)) != 0)
	|| ((valid_bits & (U8)NAS_TFT_ID_IPSEC_SPI) != (U8)0
	    && v6_pf_1->tft_ipsec_spi != v6_pf_2->tft_ipsec_spi)
	|| ((valid_bits & (U8)NAS_TFT_ID_FLOW_LABEL) != (U8)0
	    && v6_pf_1->tft_flow_label != v6_pf_2->tft_flow_label)
	|| ((valid_bits & (U8)NAS_TFT_ID_IPv6_SRC_ADDR_MASK) != (U8)0
	    && memcmp(&v6_pf_1->tft_ipv6_src_addr_mask, &v6_pf_2->tft_ipv6_src_addr_mask, sizeof(T_NAS_tft_ipv6_src_addr_mask)) != 0))
    {
      return FALSE;
    }
  } else {
    (void)TRACE_EVENT_P1("ERROR: Invalid union controller %d in ctrl_tft_pf_entry!",
                         tft1->ctrl_tft_pf_entry);
  }
  return TRUE;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_copy_ipv4_pf
+------------------------------------------------------------------------------
| Description : Copies an IPv4 TFT filter from src_tft to dst_tft AIM struct.
|
| Parameters  : dst_tft     - destination aim TFT structs
|               src_tft     - source TFT structs
|               valid_flag  - valid flags for filter components
+------------------------------------------------------------------------------
*/
static void sm_tft_copy_ipsec_spi(/*@out@*/T_M_SM_tft_ipsec_spi *dst_spi,
				  U32 spi)
{
  M_SM_BUF_ipsec_spi_value *dst = &dst_spi->ipsec_spi_value;
#ifdef TFT_DEBUG
  (void)TRACE_EVENT_P1("tft_copy: Copying IPSEC SPI filter (SPI 0x%08x)", spi);
#endif
  dst->l_ipsec_spi_value = (U16)32;
  dst->o_ipsec_spi_value = (U16)0;
  dst->b_ipsec_spi_value[0] = (U8)((spi >> 24) & 255);
  dst->b_ipsec_spi_value[1] = (U8)((spi >> 16) & 255);
  dst->b_ipsec_spi_value[2] = (U8)((spi >>  8) & 255);
  dst->b_ipsec_spi_value[3] = (U8)( spi        & 255);
}

static void
sm_tft_copy_ipv4_pf(/*@out@*/ T_M_SM_tft_filter_entry *dst,
		    T_NAS_tft_pf_ipv4 *src, U8 valid_flag)
{
  /* Copy IPv4 address, if present */
  if ((valid_flag & NAS_TFT_ID_IPv4_SRC_ADDR_MASK) != (U8)0)
  {
    dst->v_tft_ipv4_addr_mask = (U8)TRUE;
    memcpy(dst->tft_ipv4_addr_mask.src_addr,
	   src->tft_ipv4_src_addr_mask.tft_ipv4_addr,
	   (size_t)NAS_SIZE_IPv4_ADDR);
    memcpy(dst->tft_ipv4_addr_mask.addr_mask,
	   src->tft_ipv4_src_addr_mask.tft_ipv4_mask,
	   (size_t)NAS_SIZE_IPv4_ADDR);
#ifdef TFT_DEBUG
    {
      U8 *addr, *mask;
      addr = dst->tft_ipv4_addr_mask.src_addr;
      mask = dst->tft_ipv4_addr_mask.addr_mask;
      (void)TRACE_EVENT_P8("tft_copy: Copy IPv4 src addr/mask filter: "
			   "%u.%u.%u.%u/%u.%u.%u.%u",
			   addr[0], addr[1], addr[2], addr[3],
			   mask[0], mask[1], mask[2], mask[3]);
    }
#endif
  } else {
    dst->v_tft_ipv4_addr_mask            = (U8)FALSE;
  } /* NAS_TFT_ID_IPv4_SRC_ADDR_MASK */

  /* IPv6 addr/mask never invluded in IPv4 packet filter */
  dst->v_tft_ipv6_addr_mask              = (U8)FALSE;

  /* Copy IPv4 protocol number field */
  if ((valid_flag & NAS_TFT_ID_PROTOCOL_OR_NEXT_HDR) != (U8)0)
  {
    dst->v_tft_protocol = (U8)TRUE;
    dst->tft_protocol.tft_protocol_val   = src->tft_protocol;
#ifdef TFT_DEBUG
    (void)TRACE_EVENT_P1("tft_copy: Copy IPv4 protocol: 0x%02x",
			 src->tft_protocol);
#endif
  } else {
    dst->v_tft_protocol = (U8)FALSE;
  } /* NAS_TFT_ID_PROTOCOL_OR_NEXT_HDR */

  /* Copy destination single port/port range parameter */
  if ((valid_flag & NAS_TFT_ID_DEST_PORT_RANGE) != (U8)0)
  {
    T_NAS_tft_dest_port_range *port_range = &src->tft_dest_port_range;
    if (sm_tft_is_port_range(port_range->low_limit, port_range->high_limit)) {
      dst->v_tft_dest_port_range          = (U8)TRUE;
      dst->v_tft_dest_port                = (U8)FALSE;
      dst->tft_dest_port_range.low_limit  = port_range->low_limit;
      dst->tft_dest_port_range.high_limit = port_range->high_limit;
#ifdef TFT_DEBUG
      (void)TRACE_EVENT_P2("tft_copy: Copying destination port range filter (ports %d-%d)",
			   port_range->low_limit, port_range->high_limit);
#endif
    } else {
      dst->v_tft_dest_port_range          = (U8)FALSE;
      dst->v_tft_dest_port                = (U8)TRUE;
      dst->tft_dest_port.low_limit        = port_range->low_limit;
#ifdef TFT_DEBUG
      (void)TRACE_EVENT_P1("tft_copy: Copying single destination port filter (port %d)",
			 port_range->low_limit);
#endif
    }
  } else {
    dst->v_tft_dest_port_range           = (U8)FALSE;
    dst->v_tft_dest_port                 = (U8)FALSE;
  } /* NAS_TFT_ID_DEST_PORT_RANGE */

  /* Copy source single port/port range parameter */
  if ((valid_flag & NAS_TFT_ID_SRC_PORT_RANGE) != (U8)0)
  {
    T_NAS_tft_src_port_range *port_range = &src->tft_src_port_range;
    if (sm_tft_is_port_range(port_range->low_limit, port_range->high_limit))
    {
      dst->v_tft_src_port_range          = (U8)TRUE;
      dst->v_tft_src_port                = (U8)FALSE;
      dst->tft_src_port_range.low_limit  = port_range->low_limit;
      dst->tft_src_port_range.high_limit = port_range->high_limit;
#ifdef TFT_DEBUG
      (void)TRACE_EVENT_P2("tft_copy: Copying source port range filter (ports %d-%d)",
			   port_range->low_limit, port_range->high_limit);
#endif
    } else {
      dst->v_tft_src_port_range          = (U8)FALSE;
      dst->v_tft_src_port                = (U8)TRUE;
      dst->tft_src_port.low_limit        = port_range->low_limit;
#ifdef TFT_DEBUG
    (void)TRACE_EVENT_P1("tft_copy: Copying single source port filter (port %d)",
			 port_range->low_limit);
#endif
    }
  } else {
    dst->v_tft_src_port_range = (U8)FALSE;
    dst->v_tft_src_port       = (U8)FALSE;
  } /* NAS_TFT_ID_SRC_PORT_RANGE */

  if ((valid_flag & NAS_TFT_ID_IPSEC_SPI) != (U8)0)
  {
    dst->v_tft_ipsec_spi = (U8)TRUE;
    sm_tft_copy_ipsec_spi(&dst->tft_ipsec_spi, src->tft_ipsec_spi);
  } else {
    dst->v_tft_ipsec_spi = (U8)FALSE;
  }/* NAS_TFT_ID_IPSEC_SPI */

  if ((valid_flag & NAS_TFT_ID_TOS_AND_MASK) != (U8)0)
  {
    dst->v_tft_tos_and_mask = (U8)TRUE;
    dst->tft_tos_and_mask.tos_value = src->tft_tos_and_mask.tos_value;
    dst->tft_tos_and_mask.tos_mask  = src->tft_tos_and_mask.tos_mask;
  } else {
    dst->v_tft_tos_and_mask = (U8)FALSE;
  } /* NAS_TFT_ID_TOS_AND_MASK */

  if ((valid_flag & NAS_TFT_ID_FLOW_LABEL) != (U8)0) {
    (void)TRACE_ERROR("tft_copy: Found flow label in IPv4 packet filter - Discarded...");
  } /* NAS_TFT_ID_FLOW_LABEL */
  dst->v_tft_flow_label = (U8)FALSE;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_copy_ipv6_pf
+------------------------------------------------------------------------------
| Description : Copies an IPv6 TFT filter from src_tft to dst_tft AIM struct.
|
| Parameters  : dst_tft     - destination aim TFT structs
|               src_tft     - source TFT structs
|               valid_flag  - valid flags for filter components
+------------------------------------------------------------------------------
*/
static void
sm_tft_copy_ipv6_pf(/*@out@*/ T_M_SM_tft_filter_entry *dst,
		    T_NAS_tft_pf_ipv6 *src, U8 valid_flag)
{
  /* IPv4 addr/mask never invluded in IPv4 packet filter */
  dst->v_tft_ipv4_addr_mask              = (U8)FALSE;

  /* Copy IPv6 address, if present */
  if ((valid_flag & NAS_TFT_ID_IPv6_SRC_ADDR_MASK) != (U8)0)
  {
    dst->v_tft_ipv6_addr_mask = (U8)TRUE;
    memcpy(dst->tft_ipv6_addr_mask.src_addr,
	   src->tft_ipv6_src_addr_mask.tft_ipv6_addr,
	   (size_t)NAS_SIZE_IPv6_ADDR);
    memcpy(dst->tft_ipv6_addr_mask.addr_mask,
	   src->tft_ipv6_src_addr_mask.tft_ipv6_mask,
	   (size_t)NAS_SIZE_IPv6_ADDR);
#ifdef TFT_DEBUG
    /* Dump address/mask */
    {
      char addr[SM_SIZE_FORMATTED_IPv6_ADDR], mask[SM_SIZE_FORMATTED_IPv6_ADDR];
      (void)TRACE_EVENT_P2("tft_copy: Copy IPv6 src addr/mask filter: %s/%s",
            sm_format_ipv6_addr(dst->tft_ipv6_addr_mask.src_addr, addr),
            sm_format_ipv6_addr(dst->tft_ipv6_addr_mask.addr_mask, mask));
    }
#endif
  } else {
    dst->v_tft_ipv6_addr_mask = (U8)FALSE;
  } /* NAS_TFT_ID_IPv6_SRC_ADDR_MASK */

  /* Copy IPv6 next header field */
  if ((valid_flag & NAS_TFT_ID_PROTOCOL_OR_NEXT_HDR) != (U8)0)
  {
    dst->v_tft_protocol = (U8)TRUE;
    dst->tft_protocol.tft_protocol_val = src->tft_next_hdr;
#ifdef TFT_DEBUG
    (void)TRACE_EVENT_P1("tft_copy: Copy IPv6 next header field: 0x%02x",
		   src->tft_next_hdr);
#endif
  } else {
    dst->v_tft_protocol = (U8)FALSE;
  } /* NAS_TFT_ID_PROTOCOL_OR_NEXT_HDR */

  /* Copy destination single port/port range parameter */
  if ((valid_flag & NAS_TFT_ID_DEST_PORT_RANGE) != (U8)0)
  {
    T_NAS_tft_dest_port_range *port_range = &src->tft_dest_port_range;
    if (sm_tft_is_port_range(port_range->low_limit, port_range->high_limit))
    {
      dst->v_tft_dest_port_range = (U8)TRUE;
      dst->v_tft_dest_port       = (U8)FALSE;
      dst->tft_dest_port_range.low_limit  = port_range->low_limit;
      dst->tft_dest_port_range.high_limit = port_range->high_limit;
#ifdef TFT_DEBUG
    (void)TRACE_EVENT_P2("tft_copy: Copying destination port range filter (ports %d-%d)",
			 port_range->low_limit, port_range->high_limit);
#endif
    } else {
      dst->v_tft_dest_port_range = (U8)FALSE;
      dst->v_tft_dest_port       = (U8)TRUE;
      dst->tft_dest_port.low_limit        = port_range->low_limit;
#ifdef TFT_DEBUG
    (void)TRACE_EVENT_P1("tft_copy: Copying single destination port filter (port %d)",
			 port_range->low_limit);
#endif
    }
  } else {
    dst->v_tft_dest_port_range = (U8)FALSE;
    dst->v_tft_dest_port       = (U8)FALSE;
  } /* NAS_TFT_ID_DEST_PORT_RANGE */

  /* Copy source single port/port range parameter */
  if ((valid_flag & NAS_TFT_ID_SRC_PORT_RANGE) != (U8)0)
  {
    T_NAS_tft_src_port_range *port_range = &src->tft_src_port_range;
    if (sm_tft_is_port_range(port_range->low_limit, port_range->high_limit))
    {
      dst->v_tft_src_port_range = (U8)TRUE;
      dst->v_tft_src_port       = (U8)FALSE;
      dst->tft_src_port_range.low_limit  = port_range->low_limit;
      dst->tft_src_port_range.high_limit = port_range->high_limit;
#ifdef TFT_DEBUG
    (void)TRACE_EVENT_P2("tft_copy: Copying source port range filter (ports %d-%d)",
			 port_range->low_limit, port_range->high_limit);
#endif
    } else {
      dst->v_tft_src_port_range = (U8)FALSE;
      dst->v_tft_src_port       = (U8)TRUE;
      dst->tft_src_port.low_limit        = port_range->low_limit;
#ifdef TFT_DEBUG
    (void)TRACE_EVENT_P1("tft_copy: Copying single source port filter (port %d)",
			 port_range->low_limit);
#endif
    }
  } else {
    dst->v_tft_src_port_range = (U8)FALSE;
    dst->v_tft_src_port       = (U8)FALSE;
  } /* NAS_TFT_ID_SRC_PORT_RANGE */

  if ((valid_flag & NAS_TFT_ID_IPSEC_SPI) != (U8)0)
  {
    dst->v_tft_ipsec_spi = (U8)TRUE;
    sm_tft_copy_ipsec_spi(&dst->tft_ipsec_spi, src->tft_ipsec_spi);
  } else {
    dst->v_tft_ipsec_spi = (U8)FALSE;
  } /* NAS_TFT_ID_IPSEC_SPI */

  if ((valid_flag & NAS_TFT_ID_TOS_AND_MASK) != (U8)0)
  {
    dst->v_tft_tos_and_mask = (U8)TRUE;
    dst->tft_tos_and_mask.tos_value = src->tft_tos_and_mask.tos_value;
    dst->tft_tos_and_mask.tos_mask  = src->tft_tos_and_mask.tos_mask;
  } else {
    dst->v_tft_tos_and_mask = (U8)FALSE;
  } /* NAS_TFT_ID_TOS_AND_MASK */

  if ((valid_flag & NAS_TFT_ID_FLOW_LABEL) != (U8)0)
  {
    dst->v_tft_flow_label = (U8)TRUE;
    dst->tft_flow_label.flow_label_value = src->tft_flow_label;
  } else {
    dst->v_tft_flow_label = (U8)FALSE;
  } /* NAS_TFT_ID_FLOW_LABEL */
}

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_copy_filters
+------------------------------------------------------------------------------
| Description : Copies TFT filters from src_tft to dst_tft AIM structs.
|
| Parameters  : dst_tft     - destination aim TFT structs
|               src_tft     - source TFT structs
|               tft_mask    - TFT mask (determines which TFTs to consider)
+------------------------------------------------------------------------------
*/
static U8
sm_tft_copy_filters(/*@special@*/T_M_SM_tft   *dst_tft,
		    /*@in@*/     T_NAS_tft_pf *src_tft,
		    U8 max_count, U8 tft_mask)
{
  int                i, count;
  T_M_SM_tft_filter *dst;
  U16                size, total_size = 0;
  U8                 change_mask = (U8)0;

  dst_tft->v_tft_filter = (U8)TRUE;

  /* Initialize pointer to air interface TFT */
  dst = dst_tft->tft_filter;

  for (i = 0, count = 0; i < (int)max_count; i++)
  {
    if (sm_tft_is_tft_in_mask(src_tft->tft_pf_id, tft_mask))
    {
      size = sm_tft_pf_size(src_tft);
      /* Only include packet filter, if it fits in air interface message */
      if (total_size + size > (U16)255) {
	break;
      }

      dst->tft_filter_id   = src_tft->tft_pf_id;
      dst->tft_filter_prio = src_tft->tft_pf_precedence;

      if (src_tft->ctrl_tft_pf_entry == NAS_is_tft_pf_ipv4) {
	sm_tft_copy_ipv4_pf(&dst->tft_filter_entry,
			    &src_tft->tft_pf_entry.tft_pf_ipv4,
			    src_tft->tft_pf_valid_bits);
      } else if (src_tft->ctrl_tft_pf_entry == NAS_is_tft_pf_ipv6) {
	sm_tft_copy_ipv6_pf(&dst->tft_filter_entry,
			    &src_tft->tft_pf_entry.tft_pf_ipv6,
			    src_tft->tft_pf_valid_bits);
      } else {
	(void)TRACE_EVENT_P1("tft_copy: ERROR! Wrong union controller (%d) "
			     "for tft_pf_entry; discarded...",
			     src_tft->ctrl_tft_pf_entry);
      }
      /* Update change mask and total TFT size */
      change_mask |= (U8)(1UL << dst->tft_filter_id);
      total_size  += size;
      /* Advance to next destination TFT */
      dst++;
      count++;
    } /* if (sm_tft_is_tft_in_mask()) */
    /* Skip to next input TFT */
    src_tft++;
  }

  dst_tft->c_tft_filter = (U8)count;

  return change_mask;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_copy_filter_numbers
+------------------------------------------------------------------------------
| Description : Copies filter IDs to dst_tft AIM struct (derived from tft_mask)
|
| Parameters  : dst_tft     - destination aim TFT structs
|               tft_mask    - TFT mask (determines which TFTs to consider)
+------------------------------------------------------------------------------
*/
static void
sm_tft_copy_filter_numbers(/*@special@*/T_M_SM_tft *dst_tft,
			   U8 tft_mask)
{
  int  index, dst_idx;

  dst_tft->v_tft_filter_id = (U8)TRUE;

  for (index = 0, dst_idx = 0; index < (int)NAS_SIZE_TFT_FILTER; index++)
  {
    if (sm_tft_is_tft_in_mask((U8)index, tft_mask))
    {
      dst_tft->tft_filter_id[dst_idx] = (U8)index;
      dst_idx++;
    }
  } /* for */

  /* Set counter to the number of filters numbers inserted */
  dst_tft->c_tft_filter_id = (U8)dst_idx;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_update_active_tft
+------------------------------------------------------------------------------
| Description : Update context data structure with new active TFT as it would
|               be would the modification succeed.
|
| Parameters  : context     - context data
|               tft_mask    - TFT mask (determines which TFTs to consider)
+------------------------------------------------------------------------------
*/
static void
sm_tft_update_active_tft(/*@special@*/struct T_SM_CONTEXT_DATA *context,
			 U8 change_mask)
{
  T_NAS_tft_pf *req_tft, *act_tft;
  U16           index;
  U8            req_mask, act_mask, add_mask, delete_mask;

  req_mask    = context->requested_tft.tft_precence_mask;
  act_mask    = context->active_tft.tft_precence_mask;
  add_mask    = req_mask & change_mask;
  delete_mask = (U8)(0xffUL ^ req_mask) & change_mask;

  if (req_mask == (U8)0) {
    sm_nw_free_active_tft(context);
    return;
  }

  TRACE_ASSERT(context->requested_tft.ptr_tft_pf != NULL);

  /* Allocate active TFT memory, if required */
  if (act_mask == (U8)0 &&
      /*req_mask != (U8)0 &&*/  /*commented out because of previous return*/
      context->active_tft.ptr_tft_pf == NULL)
  {
    sm_nw_allocate_active_tft(context);
    TRACE_ASSERT(context->active_tft.ptr_tft_pf != NULL);
  }

  /* Loop over TFTs, in order to find any TFTs to delete */
  for (index = 0; index < (U16)NAS_SIZE_TFT_FILTER; index++)
  {
    if (sm_tft_is_tft_in_mask((U8)index, delete_mask))
    {
      /* Was filter removed ?  If so, clear filter from mask and store */
      act_mask  &= (U8)(0xffUL ^ 1UL << index);
      act_tft    = sm_tft_get_active_tft_by_id(context, (U8)index);
      memset(act_tft, 0, sizeof(T_NAS_tft_pf));
    }
  }

  for (index = 0, req_tft = context->requested_tft.ptr_tft_pf;
       index < (U16)context->requested_tft.c_tft_pf;
       index++, req_tft++)
  {
    U8  pf_id                 = req_tft->tft_pf_id;

    if (sm_tft_is_tft_in_mask(pf_id, add_mask))
    {
      /* Copy any remaining requested TFTs into active TFT */
      act_mask  |= (U8)(1UL << pf_id);
      act_tft    = sm_tft_get_active_tft_by_id(context, pf_id);
      memcpy(act_tft, req_tft, sizeof(T_NAS_tft_pf));
    }
  } /* for */

  context->active_tft.tft_precence_mask = act_mask;
  context->active_tft.c_tft_pf          = sm_tft_count_tfts_in_mask(act_mask);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_tft_next_action
+------------------------------------------------------------------------------
| Description : Determines the next action to perform (opcode) on the TFTs
|               for a context, and the filters affected.
|               (8 upper bits is change mask; 8 lower bits is opcode)
|
| Parameters  : context     - context data
+------------------------------------------------------------------------------
*/
static U16 sm_tft_next_action(struct T_SM_CONTEXT_DATA *context)
{
  U8  req_mask, act_mask, missing_mask, surplus_mask, replace_mask;
  U16 index;

  req_mask = context->requested_tft.tft_precence_mask;
  act_mask = context->active_tft.tft_precence_mask;

  /* If requested TFT contains 0 filters, remove TFT altogether */
  if (context->requested_tft.c_tft_pf == (U8)0)
  {
    return (U16)M_SM_TFT_OPCODE_DELETE_TFT;
  }

  TRACE_ASSERT(context->requested_tft.ptr_tft_pf != NULL);

  /* If there are no activate TFT filters, and requested != 0, create new TFT */
  if (act_mask == (U8)0 && req_mask != (U8)0)
  {
    return (U16)M_SM_TFT_OPCODE_CREATE_TFT | (U16)req_mask << 8;
  }

  /* Add filters, if requested filter count > active filter count or
   * requested filters exist, that are not active. */
  missing_mask = req_mask & (0xff ^ act_mask);

  if (context->requested_tft.c_tft_pf > context->active_tft.c_tft_pf ||
      missing_mask != (U8)0)
  {
    return (U16)M_SM_TFT_OPCODE_ADD_FILTERS | (U16)missing_mask << 8;
  }

  /* Delete filters, if requested filter count < active filter count or
   * active filters exist, that are not requested. */
  surplus_mask = act_mask & (0xff ^ req_mask);

  if (context->requested_tft.c_tft_pf < context->active_tft.c_tft_pf ||
      surplus_mask != (U8)0)
  {
    return (U16)M_SM_TFT_OPCODE_DELETE_FILTERS | (U16)surplus_mask << 8;
  }

  /* Now, requested and active TFTs have the same filter *numbers* active;
   * We then need to check, whether the *contents* of the filters that
   * have the same number also match. */
  for (index = 0, replace_mask = (U8)0;
       index < (U16)context->requested_tft.c_tft_pf;
       index++)
  {
    T_NAS_tft_pf *req_tft = &context->requested_tft.ptr_tft_pf[index];
    T_NAS_tft_pf *act_tft = sm_tft_get_active_tft_by_id(context, req_tft->tft_pf_id);

    if (!sm_tft_are_equal(req_tft, act_tft))
    {
      replace_mask |= (U8)(1UL << req_tft->tft_pf_id);
    }
  }

  if (replace_mask != (U8)0) {
    return (U16)M_SM_TFT_OPCODE_REPLACE_FILTERS | (U16)replace_mask << 8;
  }

  /* Error.  Nothing to change, or invalid configuration */
  return (U16)M_SM_TFT_OPCODE_SPARE;
}

/*==== PUBLIC FUNCTIONS =====================================================*/

U8 sm_tft_precence_mask(T_NAS_tft_pf *tft, U8 count)
{
  U16 index;
  U8  precence_mask = (U8)0;

  for (index = 0; index < (U16)count; index++)
  {
    precence_mask |= (1UL << tft[index].tft_pf_id);
  }

  return precence_mask;
}

void sm_tft_convert_to_aim(struct T_SM_CONTEXT_DATA *context,
			   /*@out@*/ T_M_SM_tft     *dst_tft)
{
  U8	opcode, filter_mask, change_mask, max_count;
  U16   opcode_and_mask;

  T_NAS_tft_pf *src_tft     = context->requested_tft.ptr_tft_pf;
  max_count                 = context->requested_tft.c_tft_pf;

  if (src_tft == NULL)
  {
    return;
  }

  opcode_and_mask           = sm_tft_next_action(context);
  opcode                    = (U8)(opcode_and_mask & 0xff);
  filter_mask               = (U8)(opcode_and_mask >> 8);

  dst_tft->tft_opcode       = opcode;

  switch (opcode)
  {
  case M_SM_TFT_OPCODE_CREATE_TFT:
  case M_SM_TFT_OPCODE_ADD_FILTERS:
  case M_SM_TFT_OPCODE_REPLACE_FILTERS:
    TRACE_ASSERT(src_tft != NULL);
    /* Copy filter count to air interface message TFT struct */
    change_mask = sm_tft_copy_filters(dst_tft, src_tft, max_count, filter_mask);
    sm_tft_update_active_tft(context, change_mask);
    dst_tft->tft_filter_count = sm_tft_count_tfts_in_mask(change_mask);
    break;

  case M_SM_TFT_OPCODE_DELETE_TFT:
    /*
     * DELETE TFT can have no fields other than TFT opcode and
     * filter count (== 0), and is thus complete.
     */
    context->requested_tft.tft_precence_mask = (U8)0;
    sm_tft_update_active_tft(context, (U8)0xff);
    dst_tft->tft_filter_count                = (U8)0;
    break;

  case M_SM_TFT_OPCODE_DELETE_FILTERS:
    TRACE_ASSERT(src_tft != NULL);
    /* Copy filter count to air interface message TFT struct */
    sm_tft_copy_filter_numbers(dst_tft, filter_mask);
    sm_tft_update_active_tft(context, filter_mask);
    dst_tft->tft_filter_count = sm_tft_count_tfts_in_mask(filter_mask);
    break;

  default:
    (void)TRACE_EVENT_P1("tft_convert_to_aim: ERROR!"
			 "Invalid opcode (%d) - TFT skipped!", opcode);
    break;
  }
}

BOOL sm_tft_more_to_modify(struct T_SM_CONTEXT_DATA *context)
{
  int          filter;

  (void)TRACE_FUNCTION("sm_tft_more_to_modify");

  if (context->requested_tft.c_tft_pf != context->active_tft.c_tft_pf ||
      context->requested_tft.tft_precence_mask != context->active_tft.tft_precence_mask)
  {
    return TRUE;
  }
  if (context->requested_tft.c_tft_pf > (U8)0
      && context->requested_tft.ptr_tft_pf != NULL
      && context->active_tft.ptr_tft_pf != NULL)
  {
    for (filter = 0; filter < (int)context->requested_tft.c_tft_pf; filter++)
    {
      if (!sm_tft_are_equal(&context->requested_tft.ptr_tft_pf[filter],
			    sm_tft_get_active_tft_by_id(context, context->requested_tft.ptr_tft_pf[filter].tft_pf_id)))
      {
	return TRUE;
      }
    } /* for */
  } /* if (c_tft_pf > 0) */
  return FALSE;
}
