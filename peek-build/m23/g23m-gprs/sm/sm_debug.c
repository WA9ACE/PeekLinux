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
| Purpose:    Debug functions implementation in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include "sm.h"

#include "sm_timer_handler.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

#ifdef DEBUG
/*
 * Debug function for dumping the contents of an QoS structure.
 */
static void sm_qos_dump_r97_qos(T_PS_qos_r97 *qos_r97)
{
  /*@observer@*/const char *indent = "          - ";

  /*@observer@*/const char *peak_text[16] = {
    /* NAS_PEAK_SUB  */ "SUBSCRIBED",
    /* NAS_PEAK_1K   */ "Up to 1000 octet/s",
    /* NAS_PEAK_2K   */ "Up to 2000 octet/s",
    /* NAS_PEAK_4K   */ "Up to 4000 octet/s",
    /* NAS_PEAK_8K   */ "Up to 8000 octet/s",
    /* NAS_PEAK_16K  */ "Up to 16000 octet/s",
    /* NAS_PEAK_32K  */ "Up to 32000 octet/s",
    /* NAS_PEAK_64K  */ "Up to 64000 octet/s",
    /* NAS_PEAK_128K */ "Up to 128000 octet/s",
    /* NAS_PEAK_256K */ "Up to 256000 octet/s",
    /* 10            */ "RESERVED - 1000 octets/s",
    /* 11            */ "RESERVED - 1000 octets/s",
    /* 12            */ "RESERVED - 1000 octets/s",
    /* 13            */ "RESERVED - 1000 octets/s",
    /* 14            */ "RESERVED - 1000 octets/s",
    /* 15            */ "RESERVED",
  };

  /*@observer@*/const char *mean_text[32] = {
    /* NAS_MEAN_SUB  */ "SUBSCRIBED",
    /* NAS_MEAN_100  */ "100 octets/h",
    /* NAS_MEAN_200  */ "200 octets/h",
    /* NAS_MEAN_500  */ "500 octets/h",
    /* NAS_MEAN_1K   */ "1000 octets/h",
    /* NAS_MEAN_2K   */ "2000 octets/h",
    /* NAS_MEAN_5K   */ "5000 octets/h",
    /* NAS_MEAN_10K  */ "10000 octets/h",
    /* NAS_MEAN_20K  */ "20000 octets/h",
    /* NAS_MEAN_50K  */ "50000 octets/h",
    /* NAS_MEAN_100K */ "100000 octets/h", 
    /* NAS_MEAN_200K */ "200000 octets/h",
    /* NAS_MEAN_500K */ "500000 octets/h",
    /* NAS_MEAN_1M   */ "1000000 octets/h",
    /* NAS_MEAN_2M   */ "2000000 octets/h",
    /* NAS_MEAN_5M   */ "5000000 octets/h",
    /* NAS_MEAN_10M  */ "10000000 octets/h",
    /* NAS_MEAN_20M  */ "20000000 octets/h",
    /* NAS_MEAN_50M  */ "50000000 octets/h",
    /* 19            */ "RESERVED",
    /* 20            */ "RESERVED",
    /* 21            */ "RESERVED",
    /* 22            */ "RESERVED",
    /* 23            */ "RESERVED",
    /* 24            */ "RESERVED",
    /* 25            */ "RESERVED",
    /* 26            */ "RESERVED",
    /* 27            */ "RESERVED",
    /* 28            */ "RESERVED",
    /* 29            */ "RESERVED",
    /* 30            */ "RESERVED",
    /* NAS_MEAN_BEST */"BEST EFFORT"
  };
  /*@observer@*/const char *rel_text[8] = {
    /* NAS_RELCLASS_SUB     */ "Subscribed reliability class",
    /* NAS_GTP_LLC_RLC_PROT */ "Ack'ed GTP, LLC, and RLC; Protected data",
    /* NAS_LLC_RLC_PROT     */ "Unack'ed GTP; Ack'ed LLC and RLC, Protected data",
    /* NAS_RLC_PROT         */ "Unack'ed GTP and LLC; Ack'ed RLC, Protected data",
    /* NAS_PROT             */ "Unack'ed GTP, LLC, and RLC, Protected data",
    /* NAS_NO_PROT          */ "Unack'ed GTP, LLC, and RLC, Unprotected data",
    /* 6                    */ "UNKNOWN VALUE",
    /* 7                    */ "RESERVED"
  };

  (void)TRACE_EVENT_P3("%sDelay class:             0x%02x (Class %d)",
        indent, qos_r97->delay, qos_r97->delay);
  (void)TRACE_EVENT_P3("%sReliability class:       0x%02x (%s)",
        indent, qos_r97->relclass, rel_text[(U16)qos_r97->relclass]);
  (void)TRACE_EVENT_P3("%sPeak bitrate             0x%02x (%s)",
        indent, qos_r97->peak, peak_text[(U16)qos_r97->peak]);
  (void)TRACE_EVENT_P2("%sPrecedence class:        0x%02x",
        indent, qos_r97->preced);
  (void)TRACE_EVENT_P3("%sMean bitrate             0x%02x (%s)",
        indent, qos_r97->mean, mean_text[(U16)qos_r97->mean]);
}

static void sm_qos_dump_r99_qos(T_PS_qos_r99 *qos_r99)
{
  /*@observer@*/const char *indent = "          - ";

  (void)TRACE_EVENT_P3("%sTraffic class:           0x%02x   (%s)",
        indent, qos_r99->tc,
        (qos_r99->tc == (U8)PS_TC_CONV      ? "CONVERSATIONAL" :
        (qos_r99->tc == (U8)PS_TC_STREAM    ? "STREAMING" :
        (qos_r99->tc == (U8)PS_TC_INTER     ? "INTERACTIVE" :
        (qos_r99->tc == (U8)PS_TC_BG        ? "BACKGROUND" :
        (qos_r99->tc == (U8)PS_TC_SUB       ? "SUBSCRIBED" :
        "UNKNOWN"))))));
  (void)TRACE_EVENT_P3("%sDelivery order:          0x%02x   (%s)",
        indent, qos_r99->order,
        (qos_r99->order == (U8)PS_ORDER_YES ? "YES" :
        (qos_r99->order == (U8)PS_ORDER_NO  ? "NO" :
        (qos_r99->order == (U8)PS_ORDER_SUB ? "SUBSCRIBED" :
        "UNKNOWN"))));
  (void)TRACE_EVENT_P3("%sDeliver erroneous SDUs:  0x%02x   (%s)",
        indent, qos_r99->del_err_sdu,
        (qos_r99->del_err_sdu == (U8)PS_DEL_ERR_YES      ? "YES" :
        (qos_r99->del_err_sdu == (U8)PS_DEL_ERR_NO       ? "NO" :
        (qos_r99->del_err_sdu == (U8)PS_DEL_ERR_NODETECT ? "NODETECT" :
        (qos_r99->del_err_sdu == (U8)PS_DEL_ERR_SUB      ? "SUBSCRIBED" :
        "UNKNOWN")))));
  (void)TRACE_EVENT_P3("%sMax SDU size:            0x%04x (%d octets)",
        indent, qos_r99->max_sdu, qos_r99->max_sdu);
  (void)TRACE_EVENT_P3("%sMax bit-rate uplink:     0x%04x (%dkbps)",
        indent, qos_r99->max_rate_ul, qos_r99->max_rate_ul);
  (void)TRACE_EVENT_P3("%sMax bit-rate downlink:   0x%04x (%dkbps)",
        indent, qos_r99->max_rate_dl, qos_r99->max_rate_dl);
  (void)TRACE_EVENT_P3("%sMax residual BER:        %dE-%d",
        indent, qos_r99->ber.ratio_mant, qos_r99->ber.ratio_exp);
  (void)TRACE_EVENT_P3("%sMax SDU error ratio:     %dE-%d",
        indent, qos_r99->sdu_err_ratio.ratio_mant, 
        qos_r99->sdu_err_ratio.ratio_exp);
  (void)TRACE_EVENT_P3("%sTransfer delay:          0x%04x (%dms)",
        indent, qos_r99->xfer_delay, qos_r99->xfer_delay);
  (void)TRACE_EVENT_P3("%sTraffic handling prio:   0x%02x   (%d)",
        indent, qos_r99->handling_pri, qos_r99->handling_pri);
  (void)TRACE_EVENT_P3("%sGuar. bit-rate uplink:   0x%04x (%dkbps)",
        indent, qos_r99->guar_br_ul, qos_r99->guar_br_ul);
  (void)TRACE_EVENT_P3("%sGuar. bit-rate downlink: 0x%04x (%dkbps)",
        indent, qos_r99->guar_br_dl, qos_r99->guar_br_dl);
}

static void sm_qos_dump_qos(T_SM_qos *qos, const char *type)
{
  if (qos->ctrl_qos == PS_is_R97)
  {
    (void)TRACE_EVENT_P1(   "          R97 %s QoS:", type);
    sm_qos_dump_r97_qos(&qos->qos.qos_r97);
  } else if (qos->ctrl_qos == PS_is_R99) {
    (void)TRACE_EVENT_P1(   "          R99 %s QoS:", type);
    sm_qos_dump_r99_qos(&qos->qos.qos_r99);
  } else {
    (void)TRACE_EVENT_P2("ERROR! Invalid union controller == %d in %s QoS!",
                         qos->ctrl_qos, type);
  }
}

static BOOL sm_debug_is_port_range(U16 low_limit, U16 high_limit)
{
  return (high_limit != 0 && low_limit < high_limit);
}

static /*@observer@*/char *
sm_debug_dump_port_range(U16 low_limit, U16 high_limit)
{
  static char range[sizeof("65535-65535")];

  /*@-bufferoverflowhigh@*/
  if (sm_debug_is_port_range(low_limit, high_limit))
  {
    sprintf(range, "%5hu-%5hu", low_limit, high_limit);
  } else {
    sprintf(range, "%5hu", low_limit);
  }
  /*@=bufferoverflowhigh@*/
  return range;
}

static U32 sm_debug_octet_as_bits(U8 octet) /*@*/
{
  const U32 bit_masks[16] = {
    0000UL, 0001UL, 0010UL, 0011UL, 0100UL, 0101UL, 0110UL, 0111UL,
    1000UL, 1001UL, 1010UL, 1011UL, 1100UL, 1101UL, 1110UL, 1111UL
  };
  return (bit_masks[(U16)octet >> 4] * 10000UL + bit_masks[(U16)octet & 15]);
}

static void sm_debug_dump_ipv4_tft(T_NAS_tft_pf_ipv4 *pf, U8 valid_bits)
{
  if ( (valid_bits & NAS_TFT_ID_PROTOCOL_OR_NEXT_HDR) != (U8)0)
  {
    (void)TRACE_EVENT_P1("          + IPv4 protocol number  = %hu",
                         (U16)pf->tft_protocol);
  }
  if ( (valid_bits & NAS_TFT_ID_TOS_AND_MASK) != (U8)0)
  {
    (void)TRACE_EVENT_P2("          + IPv4 ToS and mask     = 0x%02x/0x%02x",
                         pf->tft_tos_and_mask.tos_value,
                         pf->tft_tos_and_mask.tos_mask);
  }
  if ( (valid_bits & NAS_TFT_ID_DEST_PORT_RANGE) != (U8)0)
  {
    (void)TRACE_EVENT_P1("          + Dest port (range)     = %s",
                         sm_debug_dump_port_range(pf->tft_dest_port_range.low_limit, pf->tft_dest_port_range.high_limit));
  }
  if ( (valid_bits & NAS_TFT_ID_SRC_PORT_RANGE) != (U8)0)
  {
    (void)TRACE_EVENT_P1("          + Source port (range)   = %s",
                         sm_debug_dump_port_range(pf->tft_src_port_range.low_limit, pf->tft_src_port_range.high_limit));
  }
  if ( (valid_bits & NAS_TFT_ID_IPSEC_SPI) != (U8)0)
  {
    (void)TRACE_EVENT_P1("          + IPv4 IPSEC SPI        = %08x",
                         pf->tft_ipsec_spi);
  }
  if ( (valid_bits & NAS_TFT_ID_IPv4_SRC_ADDR_MASK) != (U8)0)
  {
    U8 *a4 = pf->tft_ipv4_src_addr_mask.tft_ipv4_addr;
    (void)TRACE_EVENT_P8("          + IPv4 src address/mask = %hu.%hu.%hu.%hu"
                         "/%hu.%hu.%hu.%hu",
                         (U16)a4[0], (U16)a4[1], (U16)a4[2], (U16)a4[3],
                         (U16)a4[0], (U16)a4[1], (U16)a4[2], (U16)a4[3]);
  }
}

static void sm_debug_dump_ipv6_tft(T_NAS_tft_pf_ipv6 *pf, U8 valid_bits)
{
  if ( (valid_bits & NAS_TFT_ID_PROTOCOL_OR_NEXT_HDR) != (U8)0)
  {
    (void)TRACE_EVENT_P1("          + IPv6 next header      = %hu",
                         (U16)pf->tft_next_hdr);
  }
  if ( (valid_bits & NAS_TFT_ID_TOS_AND_MASK) != (U8)0)
  {
    (void)TRACE_EVENT_P2("          + IPv6 traffic class/mask= 0x%02x/0x%02x",
                         pf->tft_tos_and_mask.tos_value,
                         pf->tft_tos_and_mask.tos_mask);
  }
  if ( (valid_bits & NAS_TFT_ID_DEST_PORT_RANGE) != (U8)0)
  {
    (void)TRACE_EVENT_P1("          + Dest port (range)     = %s",
                         sm_debug_dump_port_range(pf->tft_dest_port_range.low_limit, pf->tft_dest_port_range.high_limit));
  }
  if ( (valid_bits & NAS_TFT_ID_SRC_PORT_RANGE) != (U8)0)
  {
    (void)TRACE_EVENT_P1("          + Source port (range)   = %s",
                         sm_debug_dump_port_range(pf->tft_src_port_range.low_limit, pf->tft_src_port_range.high_limit));
  }
  if ( (valid_bits & NAS_TFT_ID_IPSEC_SPI) != (U8)0)
  {
    (void)TRACE_EVENT_P1("          + IPv6 IPSEC SPI        = %08x",
                         pf->tft_ipsec_spi);
  }
  if ( (valid_bits & NAS_TFT_ID_FLOW_LABEL) != (U8)0)
  {
    (void)TRACE_EVENT_P1("          + IPv6 flow label       = %06x",
                         pf->tft_flow_label);
  }
  if ( (valid_bits & NAS_TFT_ID_IPv6_SRC_ADDR_MASK) != (U8)0)
  {
    char src_addr [SM_SIZE_FORMATTED_IPv6_ADDR],
         addr_mask[SM_SIZE_FORMATTED_IPv6_ADDR];
    (void)sm_format_ipv6_addr(pf->tft_ipv6_src_addr_mask.tft_ipv6_addr, src_addr);
    (void)sm_format_ipv6_addr(pf->tft_ipv6_src_addr_mask.tft_ipv6_mask, addr_mask);
    (void)TRACE_EVENT_P2("          + IPv6 src address/mask = %s/%s",
                         src_addr, addr_mask);
  }
}

static void sm_debug_dump_tft_pf(T_NAS_tft_pf *tft_pf, U16 index)
{
  (void)TRACE_EVENT_P3("          #%-2u: ID=%u, precedence=%3u, valid_mask=%08ul",
                       index, tft_pf->tft_pf_precedence,
                       sm_debug_octet_as_bits(tft_pf->tft_pf_valid_bits));
      if (tft_pf->ctrl_tft_pf_entry == NAS_is_tft_pf_ipv4) {
        sm_debug_dump_ipv4_tft(&tft_pf->tft_pf_entry.tft_pf_ipv4,
                               tft_pf->tft_pf_valid_bits);
      } else if (tft_pf->ctrl_tft_pf_entry == NAS_is_tft_pf_ipv6) {
        sm_debug_dump_ipv6_tft(&tft_pf->tft_pf_entry.tft_pf_ipv6,
                               tft_pf->tft_pf_valid_bits);
      } else {
	(void)TRACE_EVENT_P1("  ERROR!   Wrong union controller (%d) "
			     "for tft_pf_entry; discarded...",
			     tft_pf->ctrl_tft_pf_entry);
      }
}

static void sm_debug_dump_tft(T_SM_tft *tft)
{
  if (tft->ptr_tft_pf != NULL && tft->c_tft_pf > (U8)0)
  {
    U16 index;

    (void)TRACE_EVENT_P3("          TFT [%08x] with %d filters (mask 0b%08ul)",
                         tft->ptr_tft_pf, tft->c_tft_pf,
                         sm_debug_octet_as_bits(tft->tft_precence_mask));
    for (index = 0; index < (U16)NAS_SIZE_TFT_FILTER; index++) {
      if ( (tft->tft_precence_mask & (1UL << index)) != 0)
      {
        sm_debug_dump_tft_pf(&tft->ptr_tft_pf[index], index);
      }
    }
  } else {
    (void)TRACE_EVENT   ("          TFT [  NULL  ]");
  }
}

/*@observer@*/char *
sm_format_ipv6_addr(U8 *addr, /*@out@*/ /*@returned@*/ char *dest)
{
  /*@-bufferoverflowhigh@*/
  (void)sprintf(dest, "%04hx:%04hx:%04hx:%04hx:%04hx:%04hx:%04hx:%04hx",
        ((U16)addr[ 0] << 8) | (U16)addr[ 1], ((U16)addr[ 2] << 8) | (U16)addr[ 3],
        ((U16)addr[ 4] << 8) | (U16)addr[ 5], ((U16)addr[ 6] << 8) | (U16)addr[ 7],
        ((U16)addr[ 8] << 8) | (U16)addr[ 9], ((U16)addr[10] << 8) | (U16)addr[11],
        ((U16)addr[12] << 8) | (U16)addr[13], ((U16)addr[14] << 8) | (U16)addr[15]);
  /*@=bufferoverflowhigh@*/
  return dest;
}

static void sm_format_ip_address(T_NAS_ip *ip_addr, /*@out@*/char *dest)
{
  if (ip_addr->ctrl_ip_address == NAS_is_ip_not_present)
  {
    strcpy(dest, "NOT_PRESENT");
  } else if (ip_addr->ctrl_ip_address == NAS_is_ipv4) {
    U8  *ptr_addr = ip_addr->ip_address.ipv4_addr.a4;
  /*@-bufferoverflowhigh@*/
    (void)sprintf(dest, "%hu.%hu.%hu.%hu",
		   (U16)ptr_addr[0], (U16)ptr_addr[1],
		   (U16)ptr_addr[2], (U16)ptr_addr[3]);
  /*@=bufferoverflowhigh@*/
  } else if (ip_addr->ctrl_ip_address == NAS_is_ipv6) {
    (void)sm_format_ipv6_addr(ip_addr->ip_address.ipv6_addr.a6, dest);
  } else {
    strcpy(dest, "INVALID_CTRL");
  }
}

struct T_SM_FLAG_STRING {
  U16             flag;
  /*@null@*/ /*@observer@*/const char *name;
};

static const struct T_SM_FLAG_STRING sm_context_flags[7] = {
  {(U16)SM_CONTEXT_FLAG_COMP_PARAMS,            "COMP_PARAMS"},
  {(U16)SM_CONTEXT_FLAG_STARTED_DURING_SUSPEND, "STARTED_DURING_SUSPEND"},
  {(U16)SM_CONTEXT_FLAG_SECONDARY_CONTEXT,      "SECONDARY_CONTEXT"},
  {(U16)SM_CONTEXT_FLAG_PENDING_DEALLOCATION,   "PENDING_DEALLOCATION"},
  {(U16)SM_CONTEXT_FLAG_PENDING_REACTIVATION,   "PENDING_REACTIVATION"},
  {(U16)SM_CONTEXT_FLAG_PFI_PRESENT,            "PFI_PRESENT"},
  {(U16)0, NULL}
};

static void sm_flags_to_string(const struct T_SM_FLAG_STRING *flag_string,
			       /*@out@*/ char *dest,
			       U16 flags)
{
  U16                      index, flag_count;

  flag_count = 0;

  for (index = 0; index < (U16)16 && flag_string->flag != 0; index++)
  {
    if ((flags & flag_string->flag) != 0 && flag_string->name != NULL)
    {
      if (flag_count != 0)
      {
	*dest++ = ','; 	*dest++ = ' ';
      } else {
	flag_count++;
      }
      strcpy(dest, flag_string->name);
      dest = &dest[strlen(flag_string->name)];
    } /* if */
    flag_string++;
  } /* for */
  *dest = '\0';
}

/*@observer@*/static const char *sm_pdp_type_name(U8 pdp_type)
{
  switch ((T_SMREG_pdp_type) pdp_type) {
  case SMREG_PDP_PPP:             return "PPP";
  case SMREG_PDP_IPV4:            return "IPv4";
  case SMREG_PDP_IPV6:            return "IPv6";
  case SMREG_PDP_EMPTY:           return "DYNAMIC";
  default:                        return "INVALID!";
  }
}

/*@observer@*/const char *sm_timer_name(U8 timer)
{
  switch ((T_SM_TIMER_TYPE) timer) {
  case SM_TIMER_NONE:            return "NONE";
  case SM_TIMER_T3380:           return "T3380";
  case SM_TIMER_T3381:           return "T3381";
  case SM_TIMER_T3390:           return "T3390";
  default:                       return "UNKNOWN";
  }
}

/*@observer@*/static const char *sm_pfi_name(U8 pfi)
{
  switch ((T_PS_pkt_flow_id) pfi) {
  case PS_PFI_BEST_EFFORT:      return "Best Effort";
  case PS_PFI_SIGNALING:        return "Signalling";
  case PS_PFI_SMS:              return "SMS";
  case PS_PKT_FLOW_ID_NOT_PRES: return "NONE";
  default:                       return "UNKNOWN";
  }
}

static void sm_format_apn(T_SMREG_apn *apn, /*@out@*/char *dest)
{
  U16  index;

  assert(apn != NULL && apn->c_apn_buf > (U8)0);

  /* First, copy (all) APN text skipping first length byte. */
  if (apn == NULL) { 
    return; /*Fix for Lint warning*/
  }

  memcpy(dest, &apn->apn_buf[1], (size_t)apn->c_apn_buf - 1);

  index   = (U16)apn->apn_buf[0];
  while (index < (U16)apn->c_apn_buf) {
    dest[index] = '.';
    index += (U16)apn->apn_buf[index];
  }
  dest[(U16)apn->c_apn_buf - 1] = '\0';
}

/*@observer@*/char *sm_context_bitfield(/*@out@*/ /*@returned@*/char *status,
					U16 status_bits)
{
  U16 index;

  for (index = 0; index < (U16)SM_MAX_NSAPI_OFFSET; index++)
  {
    U16 nsapi = sm_index_to_nsapi(index);
    status[index] = (sm_is_nsapi_in_nsapi_set(nsapi, status_bits) ? '1' : '0');
  }
  status[SM_MAX_NSAPI_OFFSET] = '\0';

  return status;
}

void sm_dump_state(void)
{
  int  nsapi;
  char req_addr[SM_SIZE_FORMATTED_IPv6_ADDR],
       neg_addr[SM_SIZE_FORMATTED_IPv6_ADDR];
  char context_status[SM_MAX_NSAPI_OFFSET + 1];

  (void)TRACE_FUNCTION("sm_dump_state");

  (void)TRACE_EVENT_P3("SM is active in a(n) %s network in %s RAT; SM is%s suspended",
		       (sm_get_current_nw_release() == PS_SGSN_98_OLDER ? "pre-R99" :
			(sm_get_current_nw_release() == PS_SGSN_99_ONWARDS ? "R99" : "UNKNOWN")),
		       (sm_get_current_rat() == PS_RAT_GSM       ? "GSM" :
			(sm_get_current_rat() == PS_RAT_UMTS_FDD ? "UMTS" : "NONE")),
		       (sm_is_suspended() ? "" : " not"));
  (void)TRACE_EVENT_P1("Context activation status: %s",
		       sm_context_bitfield(context_status, sm_data.sm_context_activation_status));

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    struct T_SM_CONTEXT_DATA *context;

    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL)
    {
      char flags[256];

      (void)TRACE_EVENT_P7("NSAPI%3d: [%08x] nsapi=%d, ti=%d, linked_ti=%d, "
			   "active_timer=%s, timeouts=%d",
			   nsapi, context, context->nsapi, context->ti,
			   context->linked_ti,
			   sm_timer_name(context->active_timer),
			   context->timeouts);
      (void)TRACE_EVENT_P4("          sapi=%d, radio_prio=%d, pfi=%d (%s)",
			   context->sapi, context->radio_prio, context->pfi,
                           sm_pfi_name(context->pfi));

      sm_flags_to_string(sm_context_flags, flags, (U16)context->flags);
      (void)TRACE_EVENT_P2("          flags=0x%02x (%s)",
                           context->flags, flags);
      (void)TRACE_EVENT_P1("          Network Control state:            %s",
			   sm_network_control_state(context));
      (void)TRACE_EVENT_P1("          Context Control state:            %s",
			   sm_context_control_state(context));
      (void)TRACE_EVENT_P1("          Context Deactivate Control state: %s",
			   sm_context_deactivate_control_state(context));
      (void)TRACE_EVENT_P1("          User Plane Control state:         %s",
			   sm_user_plane_control_state(context));
      if (!sm_is_secondary(context))
      {
      sm_format_ip_address(&context->requested_address,  req_addr);
      sm_format_ip_address(&context->negotiated_address, neg_addr);
      (void)TRACE_EVENT_P4("          PDP type=0x%02x (%s), "
			   "requested_address=%s, negotiated_address=%s",
			   context->pdp_type, 
			   sm_pdp_type_name(context->pdp_type),
			   req_addr, neg_addr);
      }
      sm_qos_dump_qos(&context->minimum_qos, "minimum");
      sm_qos_dump_qos(&context->requested_qos, "requested");
      sm_qos_dump_qos(&context->accepted_qos, "negotiated");
      if (!sm_is_secondary(context))
      {
        if (context->apn == NULL)
        {
          (void)TRACE_EVENT   ("          APN [  NULL  ]");
        } else {
          char apn[103];
          sm_format_apn(context->apn, apn);
          (void)TRACE_EVENT_P2("          APN [%08x]: %s", context->apn, apn);
        }
      }
      sm_debug_dump_tft(&context->active_tft);
    } else {
      (void)TRACE_EVENT_P1("NSAPI%3d: [  NULL  ]", nsapi);
    }
  }
}
#endif /* DEBUG */

/*==== END OF FILE ==========================================================*/
