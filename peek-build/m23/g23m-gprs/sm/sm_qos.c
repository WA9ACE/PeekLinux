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
| Purpose:    3G QoS utility functions implementation in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "sm.h"

#include "sm_qos.h"

/*==== CONST ================================================================*/

struct qos_from_to {
  U32        integer;
  struct {
    U16      ratio_mant, ratio_exp;
  }          ratio;
  U8         packed;
};

/*@checked@*/
static const struct qos_from_to sm_qos_ber_table[] = {
  {      20UL, {(U16)5,(U16)2}, (U8)M_SM_QOS_BER_5E_2},
  {     100UL, {(U16)5,(U16)2}, (U8)M_SM_QOS_BER_1E_2},
  {     200UL, {(U16)5,(U16)3}, (U8)M_SM_QOS_BER_5E_3},
  {     250UL, {(U16)4,(U16)3}, (U8)M_SM_QOS_BER_4E_3},
  {    1000UL, {(U16)1,(U16)3}, (U8)M_SM_QOS_BER_1E_3},
  {   10000UL, {(U16)1,(U16)4}, (U8)M_SM_QOS_BER_1E_4},
  {  100000UL, {(U16)1,(U16)5}, (U8)M_SM_QOS_BER_1E_5},
  { 1000000UL, {(U16)1,(U16)6}, (U8)M_SM_QOS_BER_1E_6},
  {16666667UL, {(U16)6,(U16)8}, (U8)M_SM_QOS_BER_6E_8},
  {       0UL, {(U16)0,(U16)0}, (U8)M_SM_QOS_BER_SUB}
};

/*@checked@*/
static const struct qos_from_to sm_qos_sdu_err_ratio_table[] = {
  {      10UL, {(U16)1,(U16)1}, (U8)M_SM_QOS_SDU_ERR_1E_1}, /* NOTE: M_SM_QOS_SDU_ERR_1E_1 == 7 */
  {     100UL, {(U16)1,(U16)2}, (U8)M_SM_QOS_SDU_ERR_1E_2},
  {     143UL, {(U16)7,(U16)3}, (U8)M_SM_QOS_SDU_ERR_7E_3},
  {    1000UL, {(U16)1,(U16)3}, (U8)M_SM_QOS_SDU_ERR_1E_3},
  {   10000UL, {(U16)1,(U16)4}, (U8)M_SM_QOS_SDU_ERR_1E_4},
  {  100000UL, {(U16)1,(U16)5}, (U8)M_SM_QOS_SDU_ERR_1E_5},
  { 1000000UL, {(U16)1,(U16)6}, (U8)M_SM_QOS_SDU_ERR_1E_6},
  {       0UL, {(U16)0,(U16)0}, (U8)M_SM_QOS_SDU_ERR_SUB}
};

/*
 * Conversion table for 1/n in T_ratio.
 * Contains 1/n * 1E9 entries used for scaling.
 */
/*@checked@*/
static const U32 sm_qos_ratio_table[10] = {
           0UL,
  1000000000UL,  /* 1E9 / 1 */
   500000000UL,  /* 1E9 / 2 */
   333333333UL,  /* 1E9 / 3 */
   250000000UL,  /* 1E9 / 4 */
   200000000UL,  /* 1E9 / 5 */
   166666667UL,  /* 1E9 / 6 */
   142857143UL,  /* 1E9 / 7 */
   125000000UL,  /* 1E9 / 8 */
   111111111UL   /* 1E9 / 9 */
};

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/* Conversion function: ratio to integer */
static U32 sm_qos_ratio_to_U32(U8 ratio_mant, U8 ratio_exp)
     /*@globals sm_qos_ratio_table@*/
{
  U32   value;
  int   count;

  /* Error checks */
  if (ratio_exp > (U8)9)
  {
    (void)TRACE_ERROR( "QoS ratio exponent parameter out of bounds!" );
    return 0;
  } else if (ratio_mant > (U8)9) {
    (void)TRACE_ERROR( "QoS ratio mantissa parameter out of bounds!" );
    return 0;
  } else if (ratio_exp == (U8)9 && ratio_mant < (U8)5) {
    (void)TRACE_ERROR( "QoS ratio parameter exceeds data type range!" );
    return 0;
  } else {
    for (count = 9-(int)ratio_exp, value = 1UL; count > 0; count--) {
      value *= 10;
    }
    /* Get real value by rounding 1/x and dividing by 10^(9-ratio_exp) */
    value = (sm_qos_ratio_table[(U16)ratio_mant] + (value >> 1)) / value;

    return value;
  }
}

static U8 sm_qos_ratio_to_r99aim(const struct qos_from_to *table,
                                 U8 ratio_mant, U8 ratio_exp)
     /*@globals sm_qos_ratio_table@*/
{
  int   index;
  U32   value;

  /* Check for zero / subscribed parameter */
  if (ratio_mant == (U8)0)
  {
    return (U8)M_SM_QOS_BER_SUB;
  }

  value = sm_qos_ratio_to_U32(ratio_mant, ratio_exp);

  /* Find the smallest matching table entry that is >= value */
  for (index = 0;
       table[index].integer != 0 && table[index].integer < value;
       index++)
    {};

  return (table[index].packed);
}

static void sm_qos_r99aim_to_ratio(const struct qos_from_to *table,
                                   U8 packed_ratio,
                                   /*@out@*/U8 *ratio_mant,
                                   /*@out@*/U8 *ratio_exp)
     /*@modifies *ratio_mant, *ratio_exp@*/
{
  int   index;

  for (index = 0;
       table[index].integer != 0 && packed_ratio != table[index].packed;
       index++)
    {};

  *ratio_mant   = (U8)table[index].ratio.ratio_mant;
  *ratio_exp    = (U8)table[index].ratio.ratio_exp;
}

static U32 sm_qos_r99aim_ratio_to_U32(const struct qos_from_to *table,
                                      U8 packed_ratio)
{
  int   index;

  for (index = 0;
       table[index].integer != 0 && packed_ratio != table[index].packed;
       index++)
    {};

  return (table[index].integer);
}

/* Helper function for converting "human readable" bit-rate values into
 * packed 3G air interface equvalents.
 *
 * Parameters:  Integer representation of the bit-rate
 * Returns:     Packed bit-rate value
 *
 * Conversion rules according to [3G 24.008, sec 11.5.6.5]
 */

static U8 sm_qos_bitrate_to_r99aim(U16 bitrate) /*@*/
{
  U8    result = (U8)0;

  if (bitrate < (U16)64) {
    /* 1 - 63 kbps map to values 1 - 63 @ 1kbps increments*/
    result = (U8)bitrate;
  } else if (bitrate < (U16)576) {
    /* 64 - 568 kbps map to values 64 - 127 @ 8kbps increments.
     * Values not on boundaries are rounded up.
     * Formula: result = ((bitrate + 7) / 8) + (64 - (64 / 8)) */
    result = (U8)((U16)(bitrate + ((64 * 8) - 64 + 7)) >> 3);
  } else if (bitrate < (U16)8640) {
    /* 576 - 8640 kbps map to values 128 - 254 @ 64kbps increments.
     * Values not on boundaries are rounded up.
     * Formula: result = ((bitrate + 63) / 64) + (128 - (576 / 64)) */
    result = (U8)((U16)(bitrate + ((128 * 64) - 576 + 63)) >> 6);
  } else {
    /* Error */
    (void)TRACE_EVENT( "Warning: Bitrate parameter out of bounds! Truncated..." );
    result = (U8)254;
  }
  return result;
}

/* Helper function for converting packed 3G air interface bit-rate values
 * into "human readable" equvalents.
 *
 * Parameters:  Packed bit-rate value
 * Returns:     Integer representation of the bit-rate
 *
 * Conversion rules according to [3G 24.008, sec 11.5.6.5]
 */

static U16 sm_qos_r99aim_to_bitrate(U8 packed_bitrate) /*@*/
{
  U16   bitrate = 0;

  if (packed_bitrate < (U8)64) {
    bitrate = (U16)packed_bitrate;
  } else if (packed_bitrate < (U8)128) {
    bitrate = (U16)((packed_bitrate - (U8)56) << 3);
  } else if (packed_bitrate < (U8)255) {
    bitrate = (U16)((packed_bitrate - (U8)119) << 6);
  } else {
    bitrate = 0;
  }
  return bitrate;
}

/* Helper function for converting "human readable" max SDU size values into
 * packed 3G air interface equvalents.
 *
 * Parameters:  Integer representation of the SDU size
 * Returns:     Packed SDU size value
 *
 * Conversion rules according to [3G 24.008, sec 11.5.6.5]
 */

static U8 sm_qos_max_sdu_to_r99aim(U16 sdu_size) /*@*/
{
  U8    result = (U8)0;

  if (sdu_size == (U16)PS_MAX_SDU_SUB) {
    result = (U8)PS_MAX_SDU_SUB;
  } else if (sdu_size <= (U16)1500) {/* Round up to nearest multiple of 10 and divide by 10. */
    result = (U8)((sdu_size + 9) / 10);
  } else if (sdu_size <= (U16)1502) {/* Round 1501-1502 to 1502. */
    result = (U8)PS_MAX_SDU_1502;
  } else if (sdu_size <= (U16)1510) {/* Round 1503-1510 to 1510. */
    result = (U8)PS_MAX_SDU_1510;
  } else if (sdu_size <= (U16)1520) {/* Round 1511-1520 to 1520. */
    result = (U8)PS_MAX_SDU_1520;
  } else {                      /* > 1520: We are forgiving and just truncate to 1520 */
    result = (U8)PS_MAX_SDU_1520;
    (void)TRACE_EVENT( "Warning: Max SDU size specified > 1520! Truncated..." );
  }
  return result;
}

/*
 * Helper function for converting packed 3G air interface max SDU size values
 * into "human readable" equvalents.
 *
 * Parameters:  Packed max SDU value
 * Returns:     Integer representation of the SDU size
 *
 * Conversion rules according to [3G 24.008, sec 11.5.6.5]
 */

static U16 sm_qos_r99aim_to_max_sdu(U8 packed_sdu_size) /*@*/
{
  U16   result = 0;

  if (packed_sdu_size < (U8)PS_MAX_SDU_1502) {
    result = (U16)packed_sdu_size * (U16)10;
  } else if (packed_sdu_size == (U8)PS_MAX_SDU_1502) {
    result = (U16)1502;
  } else if (packed_sdu_size == (U8)PS_MAX_SDU_1510) {
    result = (U16)1510;
  } else if (packed_sdu_size == (U8)PS_MAX_SDU_1520) {
    result = (U16)1520;
  } else {                      /* Error handling ? */
    result = (U16)1520;
    (void)TRACE_EVENT( "Warning: Packed max SDU size value > 153 (1520 octets)!  Truncated...");
  }
  return result;
}

/*
 * Helper function for converting "human readable" transfer delay values
 * into packed 3G air interface equvalents.
 *
 * Parameters:  Integer representation of the transfer delay
 * Returns:     Packed transfer delay value
 *
 * Conversion rules according to [3G 24.008, sec 11.5.6.5]
 */

static U8 sm_qos_xfer_delay_to_r99aim(U16 xfer_delay) /*@*/
{
  U8    result = (U8)0;

  if (xfer_delay == 0) {
    result = (U8)0;
  } else if (xfer_delay < (U16)10) { /* Make sure low values do not map to SUBSCRIBED */
    result = (U8)1;
  } else if (xfer_delay <= (U16)150) {
    /* 10 - 150ms map to values 1 - 15 @ 10ms increments. We round down. */
    result = (U8)(xfer_delay / 10);
  } else if (xfer_delay <= (U16)1000) {
    /* 200 - 950ms map to values 16 - 31 @ 50ms increments.
     * Values not on boundaries are rounded down.
     * Formula: result = (xfer_delay / 50) + (16 - (200 / 50)) */
    result = (U8)((xfer_delay + ((16 * 50) - 200)) / 50);
  } else if (xfer_delay <= (U16)4000) {
    /* 1000 - 4000ms map to values 32-62 @ 100ms increments.
     * Values not on boundaries are rounded down.
     * Formula: result = (xfer_delay / 100) + (32 - (1000 / 100)) */
    result = (U8)((xfer_delay + ((32 * 100) - 1000)) / 100);
  } else {
    /* Error */
    (void)TRACE_EVENT( "Warning: Transfer delay out of bounds! Truncated..." );
    result = (U8)62;
  }
  return result;
}

/*
 * Helper function for converting packed 3G air interface transfer delay values
 * into "human readable" equvalents.
 *
 * Parameters:  Packed transfer delay value
 * Returns:     Integer representation of the transfer delay
 *
 * Conversion rules according to [3G 24.008, sec 11.5.6.5]
 */

static U16 sm_qos_r99aim_to_xfer_delay(U8 packed_xfer_delay) /*@*/
{
  U16   result = 0;

  if (packed_xfer_delay == (U8)0) {
    result = (U16)PS_XFER_DELAY_SUB;
  } else if (packed_xfer_delay < (U8)16) {
    result = (U16)packed_xfer_delay * (U16)10;
  } else if (packed_xfer_delay < (U8)32) {
    result = ((U16)packed_xfer_delay - (U16)12) * (U16)50;
  } else if (packed_xfer_delay < (U8)63) {
    result = ((U16)packed_xfer_delay - (U16)22) * (U16)100;
  } else {
    result = (U16)4000;
    (void)TRACE_EVENT( "Warning: R99 transfer delay parameter out of bounds (== 63)! Truncated..." );
  }

  return result;
}

/*
 * Helper function for converting packed 3G air interface traffic class values
 * into AT command equivalents as per [3G 27.007].
 *
 * Parameters:  Packed traffic class value
 * Returns:     AT command representation of the same traffic class value
 */
static U8 sm_qos_r99aim_to_tc(U8 packed_tc) /*@*/
{
  switch (packed_tc) {
  case M_SM_QOS_TC_CONV:          return (U8)PS_TC_CONV;
  case M_SM_QOS_TC_STREAM:        return (U8)PS_TC_STREAM;
  case M_SM_QOS_TC_INTER:         return (U8)PS_TC_INTER;
  case M_SM_QOS_TC_BG:            return (U8)PS_TC_BG;
  default:
    (void)TRACE_EVENT_P1("Warning: AIM traffic class '%d' out of bounds! Defaulting to SUBSCRIBED...", packed_tc);
    return (U8)PS_TC_SUB;
  }
}

/*
 * Helper function for converting AT command traffic class values
 * into packed 3G air interface equivalents.
 *
 * Parameters:  AT command traffic class value
 * Returns:     3G AIM representation of the same traffic class value
 */
static U8 sm_qos_tc_to_r99aim(U8 tc) /*@*/
{
  switch (tc) {
  case PS_TC_CONV:               return (U8)M_SM_QOS_TC_CONV;
  case PS_TC_STREAM:             return (U8)M_SM_QOS_TC_STREAM;
  case PS_TC_INTER:              return (U8)M_SM_QOS_TC_INTER;
  case PS_TC_BG:                 return (U8)M_SM_QOS_TC_BG;
  case PS_TC_SUB:                return (U8)M_SM_QOS_TC_SUB;
  default:
    (void)TRACE_EVENT_P1("ERROR: QoS traffic class '%d' out of bounds! Defaulting to SUBSCRIBED, but expect an imminent crash!", tc);
    return (U8)M_SM_QOS_TC_SUB;
  }
}

/*
 * Helper function for converting packed 3G air interface delivery order
 * parameter values into AT command equivalents as per [3G 27.007].
 *
 * Parameters:  Packed delivery order parameter
 * Returns:     AT command representation of the same delivery order value
 */
static U8 sm_qos_r99aim_to_order(U8 packed_order) /*@*/
{
  switch (packed_order) {
  case M_SM_QOS_ORDER_NO:         return (U8)PS_ORDER_NO;
  case M_SM_QOS_ORDER_YES:        return (U8)PS_ORDER_YES;
  default:
    (void)TRACE_EVENT_P1("Warning: AIM delivery order parameter '%d' out of bounds! Defaulting to SUBSCRIBED...", packed_order);
    return (U8)PS_ORDER_SUB;
  }
}

/*
 * Helper function for converting AT command delivery order parameter values
 * into packed 3G air interface equivalents.
 *
 * Parameters:  AT command delivery order parameter
 * Returns:     Packed 3G AIM representation of the same delivery order value
 */
static U8 sm_qos_order_to_r99aim(U8 order) /*@*/
{
  switch (order) {
  case PS_ORDER_NO:              return (U8)M_SM_QOS_ORDER_NO;
  case PS_ORDER_YES:             return (U8)M_SM_QOS_ORDER_YES;
  case PS_ORDER_SUB:             return (U8)M_SM_QOS_ORDER_SUB;
  default:
    (void)TRACE_EVENT_P1("Warning: Delivery order parameter '%d' out of bounds! Defaulting to SUBSCRIBED...", order);
    return (U8)M_SM_QOS_ORDER_SUB;
  }
}

/*
 * Helper function for converting packed 3G air interface "delivery of
 * erroneous SDUs" parameter values into AT command equivalents as per
 * [3G 27.007].
 *
 * Parameters:  Packed delivery parameter
 * Returns:     AT command representation of the same delivery parameter
 */
static U8 sm_qos_r99aim_to_del_err_sdu(U8 packed_del_err_sdu) /*@*/
{
  switch (packed_del_err_sdu) {
  case M_SM_QOS_DEL_ERR_NO:       return (U8)PS_DEL_ERR_NO;
  case M_SM_QOS_DEL_ERR_YES:      return (U8)PS_DEL_ERR_YES;
  case M_SM_QOS_DEL_ERR_NODETECT: return (U8)PS_DEL_ERR_NODETECT;
  default:
    (void)TRACE_EVENT_P1("Warning: AIM delivery of err SDU parameter '%d' out of bounds! Defaulting to SUBSCRIBED", packed_del_err_sdu);
    return (U8)PS_DEL_ERR_SUB;
  }
}

/*
 * Helper function for converting AT command "delivery of erroneous SDUs"
 * parameter values into packed 3G air interface equivalents.
 *
 * Parameters:  AT command delivery parameter
 * Returns:     Packed 3G AIM representation of the same delivery value
 */
static U8 sm_qos_del_err_sdu_to_r99aim(U8 del_err_sdu) /*@*/
{
  switch (del_err_sdu) {
  case PS_DEL_ERR_NO:            return (U8)M_SM_QOS_DEL_ERR_NO;
  case PS_DEL_ERR_YES:           return (U8)M_SM_QOS_DEL_ERR_YES;
  case PS_DEL_ERR_NODETECT:      return (U8)M_SM_QOS_DEL_ERR_NODETECT;
  case PS_DEL_ERR_SUB:           return (U8)M_SM_QOS_DEL_ERR_SUB;
  default:
    (void)TRACE_EVENT_P1("Warning: Delivery of err SDU parameter '%d' out of bounds! Defaulting to SUBSCRIBED...", del_err_sdu);
    return (U8)M_SM_QOS_DEL_ERR_SUB;
  }
}

/*
 * Function for converting a R97 QoS parameter set in SM internal representation
 * into a 3G air message QoS parameter set.
 *
 * Conversion rules according to [3G 24.008, sec 10.5.6.5]
 *
 * Parameters:  Originating R97 SM QoS parameter struct
 *              Destination R97/R99 3G QoS parameter struct (overwritten in this function)
 */

static void sm_qos_convert_r97_to_aim(/*@in@*/ T_PS_qos_r97  *src_qos_r97,
                                      /*@out@*/T_M_SM_qos     *dst_qos)
     /*@modifies dst_qos->qos_r97@*/
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION( "sm_qos_convert_r97_to_aim" );
#endif

  dst_qos->qos_r97.delay       = src_qos_r97->delay;
  dst_qos->qos_r97.reliability = src_qos_r97->relclass;
  dst_qos->qos_r97.peak        = src_qos_r97->peak;
  dst_qos->qos_r97.precedence  = src_qos_r97->preced;
  dst_qos->qos_r97.mean        = src_qos_r97->mean;
}

static void sm_qos_convert_r97aim_to_r97(/*@in@*/ T_M_SM_qos_r97 *src_qos,
                                         /*@out@*/T_PS_qos_r97  *dst_qos_r97)
     /*@modifies dst_qos_r97@*/
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION( "sm_qos_convert_r97aim_to_r97" );
#endif

  dst_qos_r97->delay           = src_qos->delay;
  dst_qos_r97->relclass        = src_qos->reliability;
  dst_qos_r97->peak            = src_qos->peak;
  dst_qos_r97->preced          = src_qos->precedence;
  dst_qos_r97->mean            = src_qos->mean;
}

/*
 * Function for converting a R99 QoS parameter set in SM internal representation
 * into a 3G air message QoS parameter set.
 *
 * Conversion rules according to [3G 24.008, sec 10.5.6.5]
 *
 * Parameters:  Originating R99 SM QoS parameter struct
 *              Destination R99 3G QoS parameter struct (overwritten in this function)
 */

static void sm_qos_convert_r99_to_aim(/*@in@*/ T_PS_qos_r99 *src_qos_r99,
                                      /*@out@*/T_M_SM_qos    *dst_qos)
 /*@globals sm_qos_ber_table, sm_qos_sdu_err_ratio_table, sm_qos_ratio_table@*/
 /*@modifies dst_qos->qos_r99@*/
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION( "sm_qos_convert_r99_to_aim" );
#endif

  /* Traffic class */
  dst_qos->qos_r99.tc          = sm_qos_tc_to_r99aim(src_qos_r99->tc);

  /* Delivery order */
  dst_qos->qos_r99.order       = sm_qos_order_to_r99aim(src_qos_r99->order);

  /* Delivery of erroneous SDUs */
  dst_qos->qos_r99.del_err_sdu = sm_qos_del_err_sdu_to_r99aim(src_qos_r99->del_err_sdu);

  /* Max SDU size */
  dst_qos->qos_r99.max_sdu     = sm_qos_max_sdu_to_r99aim(src_qos_r99->max_sdu);

  /* Max uplink bit-rate */
  dst_qos->qos_r99.max_br_ul   = sm_qos_bitrate_to_r99aim(src_qos_r99->max_rate_ul);

  /* Max downlink bit-rate */
  dst_qos->qos_r99.max_br_dl   = sm_qos_bitrate_to_r99aim(src_qos_r99->max_rate_dl);

  /* Residual BER */
  dst_qos->qos_r99.ber         = sm_qos_ratio_to_r99aim(sm_qos_ber_table,
                                   src_qos_r99->ber.ratio_mant,
                                   src_qos_r99->ber.ratio_exp);

  /* SDU error ratio */
  dst_qos->qos_r99.sdu_err_ratio = sm_qos_ratio_to_r99aim(sm_qos_sdu_err_ratio_table,
                                   src_qos_r99->sdu_err_ratio.ratio_mant,
                                   src_qos_r99->sdu_err_ratio.ratio_exp);

  /* Transfer delay */
  dst_qos->qos_r99.xfer_delay  = sm_qos_xfer_delay_to_r99aim(src_qos_r99->xfer_delay);

  /* Traffic handling priority */
  dst_qos->qos_r99.handling_pri= src_qos_r99->handling_pri;

  /* Guaranteed uplink bit-rate */
  dst_qos->qos_r99.guar_br_ul  = sm_qos_bitrate_to_r99aim(src_qos_r99->guar_br_ul);

  /* Guaranteed downlink bit-rate */
  dst_qos->qos_r99.guar_br_dl  = sm_qos_bitrate_to_r99aim(src_qos_r99->guar_br_dl);
}

/*
 * Function for converting a R99 3G air message QoS parameter set into a
 * R99 QoS parameter set in SM internal representation
 *
 * Conversion rules according to [3G 24.008, sec 10.5.6.5]
 *
 * Parameters:  Originating R99 3G QoS parameter struct
 *              Destination R99 SM QoS parameter struct (overwritten in this function)
 */

static void sm_qos_convert_r99aim_to_r99(/*@in@*/ T_M_SM_qos_r99 *src_qos_r99,
                                         /*@out@*/T_PS_qos_r99  *dst_qos_r99)
 /*@globals sm_qos_ber_table, sm_qos_sdu_err_ratio_table@*/
 /*@modifies dst_qos_r99@*/
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION( "sm_qos_convert_r99aim_to_r99" );
#endif

  /* Traffic class */
  dst_qos_r99->tc          = sm_qos_r99aim_to_tc(src_qos_r99->tc);

  /* Delivery order */
  dst_qos_r99->order       = sm_qos_r99aim_to_order(src_qos_r99->order);

  /* Delivery of erroneous SDUs */
  dst_qos_r99->del_err_sdu = sm_qos_r99aim_to_del_err_sdu(src_qos_r99->del_err_sdu);

  /* Max SDU size */
  dst_qos_r99->max_sdu     = sm_qos_r99aim_to_max_sdu(src_qos_r99->max_sdu);

  /* Max uplink bit-rate */
  dst_qos_r99->max_rate_ul = sm_qos_r99aim_to_bitrate(src_qos_r99->max_br_ul);

  /* Max downlink bit-rate */
  dst_qos_r99->max_rate_dl = sm_qos_r99aim_to_bitrate(src_qos_r99->max_br_dl);

  /* Residual BER */
  sm_qos_r99aim_to_ratio(sm_qos_ber_table, src_qos_r99->ber,
                      &dst_qos_r99->ber.ratio_mant,
                      &dst_qos_r99->ber.ratio_exp);

  /* SDU error ratio */
  sm_qos_r99aim_to_ratio(sm_qos_sdu_err_ratio_table, src_qos_r99->sdu_err_ratio,
                      &dst_qos_r99->sdu_err_ratio.ratio_mant,
                      &dst_qos_r99->sdu_err_ratio.ratio_exp);

  /* Transfer delay */
  dst_qos_r99->xfer_delay  = sm_qos_r99aim_to_xfer_delay(src_qos_r99->xfer_delay);

  /* Traffic handling priority */
  dst_qos_r99->handling_pri = src_qos_r99->handling_pri;

  /* Guaranteed uplink bit-rate */
  dst_qos_r99->guar_br_ul  = sm_qos_r99aim_to_bitrate(src_qos_r99->guar_br_ul);

  /* Guaranteed downlink bit-rate */
  dst_qos_r99->guar_br_dl  = sm_qos_r99aim_to_bitrate(src_qos_r99->guar_br_dl);
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
 * Function for extracting the traffic class element stored for
 * the given context.
 */
U8 sm_qos_get_traffic_class(struct T_SM_CONTEXT_DATA *context)
{
   /*lint -e613 (Possible use of null pointer 'context' in left argument to operator '->') */
  TRACE_ASSERT(context != NULL);
 
  if (context->requested_qos.ctrl_qos == PS_is_R97) {
    T_PS_qos_r99 temp_qos;

    (void)cl_qos_convert_r97_to_r99(&context->requested_qos.qos.qos_r97,
                                        &temp_qos);

    return temp_qos.tc;
  }
  else if (context->requested_qos.ctrl_qos == PS_is_R99) {
    return context->requested_qos.qos.qos_r99.tc;
  }
  else {
    /* Default to SUBSCRIBED */
    return (U8)PS_TC_SUB;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_convert_to_aim
+------------------------------------------------------------------------------
| Description : Function for converting a QoS parameter set in internal SM
|               representation into an air interface QoS parameter set.
|               Handles both R97 and R99 types, skipping R99 elements if
|               network is R97.
|
| Parameters  : src_qos          - Source QoS (internal representation)
|               dst_qos          - Destination air interface QoS structure
|               release          - Core network release (R97/R99)
+------------------------------------------------------------------------------
*/
void sm_qos_convert_to_aim(/*@in@*/ T_SM_qos   *src_qos,
                           /*@out@*/T_M_SM_qos *dst_qos,
                           T_PS_sgsn_rel        release)
 /*@globals sm_qos_ber_table, sm_qos_sdu_err_ratio_table, sm_qos_ratio_table@*/
{
  
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_qos_convert_to_aim");
#endif

  /* Is SGSN release = pre-R99 (or R98 and older) ? */
  if (release == PS_SGSN_98_OLDER) {
    if (src_qos->ctrl_qos == PS_is_R97) {
      sm_qos_convert_r97_to_aim(&src_qos->qos.qos_r97, dst_qos);
    } else if (src_qos->ctrl_qos == PS_is_R99) {
      /* Convert R99 to R97 values */
      T_PS_qos_r97  tmp_qos_r97;
      cl_qos_convert_r99_to_r97(&src_qos->qos.qos_r99, &tmp_qos_r97);
      sm_qos_convert_r97_to_aim(&tmp_qos_r97, dst_qos);
    } else {
      (void)TRACE_ERROR( "Invalid ctrl_qos value!" );
    }
    /* Do not include R99 QoS elements in air interface message */
    dst_qos->v_qos_r99 = (U8)FALSE;
    dst_qos->tlv_len   = (U8)M_SM_SIZE_R97_QOS;
  } else if (release == PS_SGSN_99_ONWARDS) {
    if (src_qos->ctrl_qos == PS_is_R97) {
      T_PS_qos_r99  tmp_qos;
      /* Convert R97 to R99 struct */
      (void)cl_qos_convert_r97_to_r99(&src_qos->qos.qos_r97, &tmp_qos);
      /* Include both R97 and R99 QoS info in AIM */
      sm_qos_convert_r97_to_aim(&src_qos->qos.qos_r97, dst_qos);
      sm_qos_convert_r99_to_aim(&tmp_qos, dst_qos);
    } else if (src_qos->ctrl_qos == PS_is_R99) {
      T_PS_qos_r97  tmp_qos_r97;
      /* Fill in R99 AIM fields */
      sm_qos_convert_r99_to_aim(&src_qos->qos.qos_r99, dst_qos);
      /* [3G 24.008] says to always include R97/R98 parameters in QoS IE */
      cl_qos_convert_r99_to_r97(&src_qos->qos.qos_r99, &tmp_qos_r97);
      sm_qos_convert_r97_to_aim(&tmp_qos_r97, dst_qos);
    } else {
      (void)TRACE_ERROR( "Invalid ctrl_qos value!" );
    }
    /* Include R99 QoS elements in air interface message */
    dst_qos->v_qos_r99 = (U8)TRUE;
    dst_qos->tlv_len   = (U8)M_SM_SIZE_R99_QOS;
  } else {
    (void)TRACE_EVENT_P1("ERROR: Invalid network release union controller %d!",
                         release);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_assign_from_aim
+------------------------------------------------------------------------------
| Description : Function for converting an air interface QoS parameter set
|               to internal SM representation.  Handles both R97 and R99 types,
|               and sets the destination QoS union controller to the R97/R99
|               type of the air interface message.
|
| Parameters  : dst_qos          - Destination QoS (internal representation)
|               src_qos          - Source air interface QoS structure
+------------------------------------------------------------------------------
*/
void sm_qos_assign_from_aim(/*@out@*/T_SM_qos   *dst_qos,
                            /*@in@*/ T_M_SM_qos *src_qos)
 /*@globals sm_qos_ber_table, sm_qos_sdu_err_ratio_table@*/
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION( "sm_qos_assign_from_aim");
#endif

  /* Does QoS include R99 elements? */
  if (src_qos->v_qos_r99 == (U8)FALSE) {
    /* FALSE == No R99 elements present; Set type to R97 */
    dst_qos->ctrl_qos = PS_is_R97;
    sm_qos_convert_r97aim_to_r97(&src_qos->qos_r97, &dst_qos->qos.qos_r97);
  } else {
    /* TRUE == R99 elements present; Set type to R99 */
    dst_qos->ctrl_qos = PS_is_R99;
    sm_qos_convert_r99aim_to_r99(&src_qos->qos_r99, &dst_qos->qos.qos_r99);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_compare_r97_lt_r97
+------------------------------------------------------------------------------
| Description : Compares an R97 QoS structure with another (minimum) R97 QoS
|               structure in internal SM representation.  Returns TRUE if
|               the supplied QoS is *less than* minimum QoS.
| Parameters  : cmp_qos_r97      - comparison QoS structure
|               min_qos_r97      - minimum QoS structure
+------------------------------------------------------------------------------
*/
/* NOTE:  Removed.  sm-to-sm comparisons only occur during MO modify
 * procedures, which are not possible in R97.*/
#if 0
static BOOL sm_qos_compare_r97_lt_r97(/*@in@*/T_PS_qos_r97 *cmp_qos_r97,
                                      /*@in@*/T_PS_qos_r97 *min_qos_r97)
{
  (void)TRACE_FUNCTION( "sm_qos_compare_r97aim_lt_r97");

  if (min_qos_r97->delay       != (U8)NAS_DELAY_SUB &&
      cmp_qos_r97->delay       > min_qos_r97->delay)
  {
    (void)TRACE_EVENT_P2("R97 DELAY parameter insufficient: %d > %d (min_qos)",
                         cmp_qos_r97->delay, min_qos_r97->delay);
    return TRUE;
  }
  if (min_qos_r97->relclass    != (U8)NAS_RELCLASS_SUB &&
      cmp_qos_r97->relclass    > min_qos_r97->relclass)
  {
    (void)TRACE_EVENT_P2("R97 RELIABILITY CLASS parameter insufficient: "
                         "%d > %d (min_qos)",
                         cmp_qos_r97->relclass, min_qos_r97->relclass);
    return TRUE;
  }
  if (min_qos_r97->peak        != (U8)NAS_PEAK_SUB &&
      cmp_qos_r97->peak        < min_qos_r97->peak)
  {
    (void)TRACE_EVENT_P2("R97 PEAK BITRATE parameter insufficient: "
                         "%d < %d (min_qos)",
                         cmp_qos_r97->peak, min_qos_r97->peak);
    return TRUE;
  }
  if (min_qos_r97->preced      != (U8)NAS_PRECED_SUB &&
      cmp_qos_r97->preced      > min_qos_r97->preced)
  {
    (void)TRACE_EVENT_P2("R97 PRECEDENCE CLASS parameter insufficient: "
                         "%d > %d (min_qos)",
                         cmp_qos_r97->preced, min_qos_r97->preced);
    return TRUE;
  }
  if (min_qos_r97->mean        != (U8)NAS_MEAN_SUB &&
      cmp_qos_r97->mean        < min_qos_r97->mean)
  {
    (void)TRACE_EVENT_P2("R97 MEAN BITRATE parameter insufficient: "
                         "%d < %d (min_qos)",
                         cmp_qos_r97->mean, min_qos_r97->mean);
    return TRUE;
  }
  return FALSE;
}
#endif

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_compare_r99_lt_r99
+------------------------------------------------------------------------------
| Description : Compares an R99 QoS structure with a (minimum) R99 QoS
|               structure in internal SM representation.  Returns TRUE if
|               comparison QoS is *less than* minimum QoS.
| Parameters  : cmp_qos_r99      - air interface QoS structure
|               min_qos_r99      - minimum QoS structure
+------------------------------------------------------------------------------
*/
static BOOL sm_qos_compare_r99_lt_r99(/*@in@*/T_PS_qos_r99 *cmp_qos_r99,
                                      /*@in@*/T_PS_qos_r99 *min_qos_r99)
{
  T_PS_tc  traffic_class = (T_PS_tc)cmp_qos_r99->tc;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION( "sm_qos_compare_r99_lt_r99");
#endif

  /* Traffic class */
  if ((min_qos_r99->tc != (U8)PS_TC_SUB) &&
      (cmp_qos_r99->tc > min_qos_r99->tc))
  {
    (void)TRACE_EVENT_P2("R99 TRAFFIC CLASS parameter insufficient: "
                         "%d > %d (min_qos)",
                         cmp_qos_r99->tc, min_qos_r99->tc);
    return TRUE;
  }

  /* Delivery order */
  if ((min_qos_r99->order != (U8)PS_ORDER_SUB) &&
      (cmp_qos_r99->order != min_qos_r99->order))
  {
    (void)TRACE_EVENT_P2("R99 DELIVERY ORDER parameter insufficient: "
                         "%d != %d (min_qos)",
                         cmp_qos_r99->order, min_qos_r99->order);
    return TRUE;
  }

  /* Delivery of erroneous SDUs */
  if ((min_qos_r99->del_err_sdu != (U8)PS_DEL_ERR_SUB) &&
      (cmp_qos_r99->del_err_sdu != min_qos_r99->del_err_sdu))
  {
    (void)TRACE_EVENT_P2("R99 DELIVERY of ERRONEUOS SDUs parameter "
                         "insufficient: %d != %d (min_qos)",
                         cmp_qos_r99->del_err_sdu, min_qos_r99->del_err_sdu);
    return TRUE;
  }

  /* Max SDU size */
  if ((min_qos_r99->max_sdu != (U16)PS_MAX_SDU_SUB) &&
      (cmp_qos_r99->max_sdu < min_qos_r99->max_sdu))
  {
    (void)TRACE_EVENT_P2("R99 MAX SDU SIZE parameter insufficient: "
                         "%d < %d (min_qos)",
                         cmp_qos_r99->max_sdu, min_qos_r99->max_sdu);
    return TRUE;
  }

  /* Max uplink bit-rate */
  if ((min_qos_r99->max_rate_ul != (U16)PS_MAX_BR_UL_SUB) &&
      (cmp_qos_r99->max_rate_ul < min_qos_r99->max_rate_ul))
  {
    (void)TRACE_EVENT_P2("R99 MAX UPLINK BITRATE parameter insufficient: "
                         "%d < %d (min_qos)",
                         cmp_qos_r99->max_rate_ul, min_qos_r99->max_rate_ul);
    return TRUE;
  }

  /* Max downlink bit-rate */
  if ((min_qos_r99->max_rate_dl != (U16)PS_MAX_BR_DL_SUB) &&
      (cmp_qos_r99->max_rate_dl < min_qos_r99->max_rate_dl))
  {
    (void)TRACE_EVENT_P2("R99 MAX DOWNLINK BITRATE parameter insufficient: "
                         "%d < %d (min_qos)",
                         cmp_qos_r99->max_rate_dl, min_qos_r99->max_rate_dl);
    return TRUE;
  }

  /* Residual BER */
  if ((min_qos_r99->ber.ratio_exp != (U8)0
       && min_qos_r99->ber.ratio_mant != (U8)0) &&
      (sm_qos_ratio_to_U32(cmp_qos_r99->ber.ratio_mant,
                           cmp_qos_r99->ber.ratio_exp) <
       sm_qos_ratio_to_U32(min_qos_r99->ber.ratio_mant,
                           min_qos_r99->ber.ratio_exp)))
  {
    (void)TRACE_EVENT_P4("R99 RESIDUAL BIT ERROR RATE parameter insufficient: "
                         "%dE-%d < %dE-%d (min_qos)",
                         cmp_qos_r99->ber.ratio_mant,
                         cmp_qos_r99->ber.ratio_exp,
                         min_qos_r99->ber.ratio_mant,
                         min_qos_r99->ber.ratio_exp);
    return TRUE;
  }

  /* SDU error ratio */
  if ((min_qos_r99->sdu_err_ratio.ratio_exp != (U8)0 &&
       min_qos_r99->sdu_err_ratio.ratio_mant != (U8)0) &&
      (sm_qos_ratio_to_U32(cmp_qos_r99->sdu_err_ratio.ratio_mant,
                           cmp_qos_r99->sdu_err_ratio.ratio_exp) <
       sm_qos_ratio_to_U32(min_qos_r99->sdu_err_ratio.ratio_mant,
                           min_qos_r99->sdu_err_ratio.ratio_exp)))
  {
    (void)TRACE_EVENT_P4("R99 SDU ERROR RATIO parameter insufficient: "
                         "%dE-%d < %dE-%d (min_qos)",
                         cmp_qos_r99->sdu_err_ratio.ratio_mant,
                         cmp_qos_r99->sdu_err_ratio.ratio_exp,
                         min_qos_r99->sdu_err_ratio.ratio_mant,
                         min_qos_r99->sdu_err_ratio.ratio_exp);
    return TRUE;
  }

  /* Transfer delay - Note! Only for real-time traffic class traffic! */
  if ((traffic_class == PS_TC_CONV || traffic_class == PS_TC_STREAM) &&
      (min_qos_r99->xfer_delay != (U16)PS_XFER_DELAY_SUB) &&
      (cmp_qos_r99->xfer_delay > min_qos_r99->xfer_delay))
  {
    (void)TRACE_EVENT_P2("R99 TRANSFER DELAY parameter insufficient: "
                         "%d > %d (min_qos)",
                         cmp_qos_r99->xfer_delay, min_qos_r99->xfer_delay);
    return TRUE;
  }

  /* Traffic handling priority - Note: Only interactive traffic class! */
  if ((traffic_class == PS_TC_INTER) &&
      (min_qos_r99->handling_pri != (U8)PS_HANDLING_PRI_SUB) &&
      (cmp_qos_r99->handling_pri > min_qos_r99->handling_pri))
  {
    (void)TRACE_EVENT_P2("R99 TRANSFER HANDLING PRIORITY parameter "
                         "insufficient: %d > %d (min_qos)",
                         cmp_qos_r99->handling_pri, min_qos_r99->handling_pri);
    return TRUE;
  }

  /* Guaranteed uplink bit-rate - Note: Only for real-time traffic class traffic! */
  if ((traffic_class == PS_TC_CONV || traffic_class == PS_TC_STREAM) &&
      (min_qos_r99->guar_br_ul != (U16)PS_GUAR_BR_UL_SUB) &&
      (cmp_qos_r99->guar_br_ul < min_qos_r99->guar_br_ul))
  {
    (void)TRACE_EVENT_P2("R99 GUARANTEED UPLINK BITRATE parameter "
                         "insufficient: %d < %d (min_qos)",
                         cmp_qos_r99->guar_br_ul, min_qos_r99->guar_br_ul);
    return TRUE;
  }

  /* Guaranteed downlink bit-rate  - Note: Only for real-time traffic class traffic! */
  if ((traffic_class == PS_TC_CONV || traffic_class == PS_TC_STREAM) &&
      (min_qos_r99->guar_br_dl != (U16)PS_GUAR_BR_DL_SUB) &&
      (cmp_qos_r99->guar_br_dl < min_qos_r99->guar_br_dl))
  {
    (void)TRACE_EVENT_P2("R99 GUARANTEED DOWNLINK BITRATE parameter "
                         "insufficient: %d < %d (min_qos)",
                         cmp_qos_r99->guar_br_dl, min_qos_r99->guar_br_dl);
    return TRUE;
  }

  return FALSE;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_compare_r97aim_lt_r97
+------------------------------------------------------------------------------
| Description : Compares an R97 air interface QoS structure with an R97 QoS
|               structure in internal SM representation.  Returns TRUE if
|               AIM QoS is *less than* min QoS or *greater than* req QoS.
| Parameters  : aim_qos_r97      - air interface QoS structure
|               min_qos_r97      - minimum QoS structure
|               req_qos_r97      - requested QoS structure
+------------------------------------------------------------------------------
*/
static BOOL sm_qos_compare_r97aim_lt_r97(/*@in@*/T_M_SM_qos_r97 *aim_qos_r97,
                                         /*@in@*/T_PS_qos_r97  *min_qos_r97,
                                         /*@in@*/T_PS_qos_r97  *req_qos_r97 )
{
  (void)TRACE_FUNCTION( "sm_qos_compare_r97aim_lt_r97");

  /* Compare R97 AIM QoS with R97 parameter struct */
  if ( (min_qos_r97->delay       != (U8)PS_DELAY_SUB && 
        aim_qos_r97->delay       > min_qos_r97->delay ) ||
       (req_qos_r97->delay       != (U8)PS_DELAY_SUB && 
        aim_qos_r97->delay       < req_qos_r97->delay) )
  {
    (void)TRACE_EVENT_P4("R97 DELAY parameter insufficient: "
                          "%d > %d (min_qos) || %d < %d (req_qos)", 
                          aim_qos_r97->delay, min_qos_r97->delay, \
                          aim_qos_r97->delay, req_qos_r97->delay);
    return TRUE;
  }
  if ( (min_qos_r97->relclass    != (U8)PS_RELCLASS_SUB &&
        aim_qos_r97->reliability > min_qos_r97->relclass) || 
       (req_qos_r97->relclass    != (U8)PS_RELCLASS_SUB && 
        aim_qos_r97->reliability < req_qos_r97->relclass) && 
        /*the following line is for the TC 46.1.2.2.1.2*/
      !((aim_qos_r97->reliability ==2)  && (req_qos_r97->relclass == 5)) ) 
  {
    (void)TRACE_EVENT_P4("R97 RELIABILITY CLASS parameter insufficient: "
                         "%d > %d (min_qos) %d < %d (req_qos)",
                         aim_qos_r97->reliability, min_qos_r97->relclass, \
                         aim_qos_r97->reliability, req_qos_r97->relclass);
    return TRUE;
  }
  if ( (min_qos_r97->peak != (U8)PS_PEAK_SUB && 
        aim_qos_r97->peak <  min_qos_r97->peak) ||
       (req_qos_r97->peak != (U8)PS_PEAK_SUB && 
        aim_qos_r97->peak >  req_qos_r97->peak) )
  {
    (void)TRACE_EVENT_P4("R97 PEAK BITRATE parameter insufficient: "
                         "%d < %d (min_qos) || %d > %d (req_qos)",
                         aim_qos_r97->peak, min_qos_r97->peak, \
                         aim_qos_r97->peak, req_qos_r97->peak);
    return TRUE;
  }
  if ( (min_qos_r97->preced      != (U8)PS_PRECED_SUB &&
        aim_qos_r97->precedence  > min_qos_r97->preced) ||
       (req_qos_r97->preced      != (U8)PS_PRECED_SUB &&
        aim_qos_r97->precedence  < req_qos_r97->preced))
  {
    (void)TRACE_EVENT_P4("R97 PRECEDENCE CLASS parameter insufficient: "
                         "%d > %d (min_qos) || %d < %d (req_qos)",
                         aim_qos_r97->precedence, min_qos_r97->preced, \
                         aim_qos_r97->precedence, req_qos_r97->preced);
    return TRUE;
  }
  if ( (min_qos_r97->mean        != (U8)PS_MEAN_SUB &&
        aim_qos_r97->mean        < min_qos_r97->mean) || 
       (req_qos_r97->mean        != (U8)PS_MEAN_SUB &&
        aim_qos_r97->mean        > req_qos_r97->mean) )
  {
    (void)TRACE_EVENT_P4("R97 MEAN BITRATE parameter insufficient: "
                         "%d < %d (min_qos) || %d > %d (req_qos)",
                         aim_qos_r97->mean, min_qos_r97->mean, \
                         aim_qos_r97->mean, req_qos_r97->mean);
    return TRUE;
  }

  return FALSE;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_compare_r99aim_lt_r99
+------------------------------------------------------------------------------
| Description : Compares an R99 air interface QoS structure with an R99 QoS
|               structure in internal SM representation.  Returns TRUE if
|               AIM QoS is *less than* minimum QoS.
| Parameters  : aim_qos_r99      - air interface QoS structure
|               min_qos_r99      - minimum QoS structure
+------------------------------------------------------------------------------
*/
static BOOL sm_qos_compare_r99aim_lt_r99(/*@in@*/T_M_SM_qos_r99 *aim_qos_r99,
                                         /*@in@*/T_PS_qos_r99  *min_qos_r99)
{
  (void)TRACE_FUNCTION( "sm_qos_compare_r99aim_lt_r99" );

  /* Traffic class */
  if ((min_qos_r99->tc != (U8)PS_TC_SUB) &&
      (sm_qos_r99aim_to_tc(aim_qos_r99->tc) > min_qos_r99->tc))
  {
    (void)TRACE_EVENT_P2("R99 TRAFFIC CLASS parameter insufficient: "
                         "%d > %d (min_qos)",
                         sm_qos_r99aim_to_tc(aim_qos_r99->tc),
                         min_qos_r99->tc);
    return TRUE;
  }

  /* Delivery order */
  if ((min_qos_r99->order != (U8)PS_ORDER_SUB) &&
      (sm_qos_r99aim_to_order(aim_qos_r99->order) != min_qos_r99->order))
  {
    (void)TRACE_EVENT_P2("R99 DELIVERY ORDER parameter insufficient: "
                         "%d != %d (min_qos)",
                         sm_qos_r99aim_to_order(aim_qos_r99->order),
                         min_qos_r99->order);
    return TRUE;
  }

  /* Delivery of erroneous SDUs */
  if ((min_qos_r99->del_err_sdu != (U8)PS_DEL_ERR_SUB) &&
      (sm_qos_r99aim_to_del_err_sdu(aim_qos_r99->del_err_sdu) != min_qos_r99->del_err_sdu))
  {
    (void)TRACE_EVENT_P2("R99 DELIVERY of ERRONEUOS SDUs parameter "
                         "insufficient: %d != %d (min_qos)",
                         sm_qos_r99aim_to_del_err_sdu(aim_qos_r99->del_err_sdu),
                         min_qos_r99->del_err_sdu);
    return TRUE;
  }

  /* Max SDU size */
  if ((min_qos_r99->max_sdu != (U16)PS_MAX_SDU_SUB) &&
      (sm_qos_r99aim_to_max_sdu(aim_qos_r99->max_sdu) < min_qos_r99->max_sdu))
  {
    (void)TRACE_EVENT_P2("R99 MAX SDU SIZE parameter insufficient: "
                         "%d < %d (min_qos)",
                         sm_qos_r99aim_to_max_sdu(aim_qos_r99->max_sdu),
                         min_qos_r99->max_sdu);
    return TRUE;
  }

  /* Max uplink bit-rate */
  if ((min_qos_r99->max_rate_ul != (U16)PS_MAX_BR_UL_SUB) &&
      (sm_qos_r99aim_to_bitrate(aim_qos_r99->max_br_ul) < min_qos_r99->max_rate_ul))
  {
    (void)TRACE_EVENT_P2("R99 MAX UPLINK BITRATE parameter insufficient: "
                         "%d < %d (min_qos)",
                         sm_qos_r99aim_to_bitrate(aim_qos_r99->max_br_ul),
                         min_qos_r99->max_rate_ul);
    return TRUE;
  }

  /* Max downlink bit-rate */
  if ((min_qos_r99->max_rate_dl != (U16)PS_MAX_BR_DL_SUB) &&
      (sm_qos_r99aim_to_bitrate(aim_qos_r99->max_br_dl) < min_qos_r99->max_rate_dl))
  {
    (void)TRACE_EVENT_P2("R99 MAX DOWNLINK BITRATE parameter insufficient: "
                         "%d < %d (min_qos)",
                         sm_qos_r99aim_to_bitrate(aim_qos_r99->max_br_ul),
                         min_qos_r99->max_rate_dl);
    return TRUE;
  }

  /* Residual BER */
  if ((min_qos_r99->ber.ratio_exp != (U8)0
       && min_qos_r99->ber.ratio_mant != (U8)0) &&
      (sm_qos_r99aim_ratio_to_U32(sm_qos_ber_table, aim_qos_r99->ber) <
       sm_qos_ratio_to_U32(min_qos_r99->ber.ratio_mant,
                           min_qos_r99->ber.ratio_exp)))
  {
#ifdef DEBUG
    U8 ratio_exp, ratio_mant;
    sm_qos_r99aim_to_ratio(sm_qos_ber_table, aim_qos_r99->ber,
                           &ratio_mant, &ratio_exp);
    (void)TRACE_EVENT_P4("R99 RESIDUAL BIT ERROR RATE parameter insufficient: "
                         "%dE-%d < %dE-%d (min_qos)",
                         ratio_mant, ratio_exp,
                         min_qos_r99->ber.ratio_mant,
                         min_qos_r99->ber.ratio_exp);
#endif
    return TRUE;
  }

  /* SDU error ratio */
  if ((min_qos_r99->sdu_err_ratio.ratio_exp != (U8)0 &&
       min_qos_r99->sdu_err_ratio.ratio_mant != (U8)0) &&
      (sm_qos_r99aim_ratio_to_U32(sm_qos_sdu_err_ratio_table, aim_qos_r99->sdu_err_ratio) <
       sm_qos_ratio_to_U32(min_qos_r99->sdu_err_ratio.ratio_mant,
                           min_qos_r99->sdu_err_ratio.ratio_exp)))
  {
#ifdef DEBUG
    U8 ratio_exp, ratio_mant;
    sm_qos_r99aim_to_ratio(sm_qos_sdu_err_ratio_table,
                           aim_qos_r99->sdu_err_ratio,
                           &ratio_mant, &ratio_exp);
    (void)TRACE_EVENT_P4("R99 SDU ERROR RATIO parameter insufficient: "
                         "%dE-%d < %dE-%d (min_qos)",
                         ratio_mant, ratio_exp,
                         min_qos_r99->sdu_err_ratio.ratio_mant,
                         min_qos_r99->sdu_err_ratio.ratio_exp);
#endif
    return TRUE;
  }

  /* Transfer delay - Note! Only for real-time traffic class traffic! */
  if ((aim_qos_r99->tc == (U8)M_SM_QOS_TC_CONV
       || aim_qos_r99->tc == (U8)M_SM_QOS_TC_STREAM) &&
      (min_qos_r99->xfer_delay != (U16)PS_XFER_DELAY_SUB) &&
      (sm_qos_r99aim_to_xfer_delay(aim_qos_r99->xfer_delay) > min_qos_r99->xfer_delay))
  {
    (void)TRACE_EVENT_P2("R99 TRANSFER DELAY parameter insufficient: "
                         "%d > %d (min_qos)",
                         sm_qos_r99aim_to_xfer_delay(aim_qos_r99->xfer_delay),
                         min_qos_r99->xfer_delay);
    return TRUE;
  }

  /* Traffic handling priority - Note: Only interactive traffic class! */
  if ((aim_qos_r99->tc == (U8)M_SM_QOS_TC_INTER) &&
      (min_qos_r99->handling_pri != (U8)PS_HANDLING_PRI_SUB) &&
      (aim_qos_r99->handling_pri > min_qos_r99->handling_pri))
  {
    (void)TRACE_EVENT_P2("R99 TRANSFER HANDLING PRIORITY parameter "
                         "insufficient: %d > %d (min_qos)",
                         aim_qos_r99->handling_pri, min_qos_r99->handling_pri);
    return TRUE;
  }

  /* Guaranteed uplink bit-rate - Note: Only for real-time traffic class traffic! */
  if ((aim_qos_r99->tc == (U8)M_SM_QOS_TC_CONV
       || aim_qos_r99->tc == (U8)M_SM_QOS_TC_STREAM) &&
      (min_qos_r99->guar_br_ul != (U16)PS_GUAR_BR_UL_SUB) &&
      (sm_qos_r99aim_to_bitrate(aim_qos_r99->guar_br_ul) < min_qos_r99->guar_br_ul))
  {
    (void)TRACE_EVENT_P2("R99 GUARANTEED UPLINK BITRATE parameter "
                         "insufficient: %d < %d (min_qos)",
                         sm_qos_r99aim_to_bitrate(aim_qos_r99->guar_br_ul),
                         min_qos_r99->guar_br_ul);
    return TRUE;
  }

  /* Guaranteed downlink bit-rate  - Note: Only for real-time traffic class traffic! */
  if ((aim_qos_r99->tc == (U8)M_SM_QOS_TC_CONV
       || aim_qos_r99->tc == (U8)M_SM_QOS_TC_STREAM) &&
      (min_qos_r99->guar_br_dl != (U16)PS_GUAR_BR_DL_SUB) &&
      (sm_qos_r99aim_to_bitrate(aim_qos_r99->guar_br_dl) < min_qos_r99->guar_br_dl))
  {
    (void)TRACE_EVENT_P2("R99 GUARANTEED DOWNLINK BITRATE parameter "
                         "insufficient: %d < %d (min_qos)",
                         sm_qos_r99aim_to_bitrate(aim_qos_r99->guar_br_dl),
                         min_qos_r99->guar_br_dl);
    return TRUE;
  }

  return FALSE;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_is_minimum_satisfied_by_aim
+------------------------------------------------------------------------------
| Description : Main comparison function. Compares a 3G air interface message
|               QoS parameter set (R97 or R99) with the minimum QoS parameter
|               set in context data.
|               Returns TRUE if the 3G air message parameters are greater than
|               or equal to the minimum parameters in SM representation
|               ("greater than" is different for each value).
|
| Parameters  : context          - Context data
|               aim_qos          - air interface QoS structure
+------------------------------------------------------------------------------
*/
BOOL sm_qos_is_minimum_satisfied_by_aim(struct T_SM_CONTEXT_DATA *context,
                                        T_M_SM_qos               *aim_qos)
{
  BOOL  less_than = TRUE;

#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION( "sm_qos_is_minimum_satisfied_by_aim");
#endif
   /*lint -e613 (Possible use of null pointer 'context' in left argument to operator '->') */
  TRACE_ASSERT(context != NULL);

  if (context->minimum_qos.ctrl_qos == PS_is_qos_not_present) {
      /* Accept QoS without any checking */
    return TRUE; 
  } 
  if (context->minimum_qos.ctrl_qos == PS_is_R97) {
    T_PS_qos_r97 *min_qos = &context->minimum_qos.qos.qos_r97;
    T_PS_qos_r97 *req_qos = &context->requested_qos.qos.qos_r97;
    if (aim_qos->v_qos_r99 == (U8)FALSE) {
      less_than = sm_qos_compare_r97aim_lt_r97(&aim_qos->qos_r97, min_qos, 
                                               req_qos);
    } else { 

     /* This #if.. #else part is required because we currently do not set
      * RAT when MMPM_ATTACH_IND is received. When the RAT is set properly 
      * retain ONLY the #else part. 
      */
/*#ifdef SM_EDGE */ /*ONLY FOR GPRS WORLD*/
      /*In GPRS world try comparing only R97 part of QoS*/
      TRACE_EVENT("GSM/GPRS RAT.. Comparing only R97 part of QoS.");
      less_than = sm_qos_compare_r97aim_lt_r97(&aim_qos->qos_r97, min_qos, 
                                               req_qos);
#if 0
      /*#else*/ /*FOR UMTS and DUAL-MODE*/
      if (sm_get_current_rat() == PS_RAT_GSM){
        /*In GPRS world compare only R97 part of QoS*/
        TRACE_EVENT("GSM/GPRS RAT.. Comparing only R97 part of QoS.");
        less_than = sm_qos_compare_r97aim_lt_r97(&aim_qos->qos_r97, min_qos,
                                                 req_qos);
      } else { /*We should be in UMTS world now..*/
        T_PS_qos_r99      tmp_qos;
        TRACE_EVENT("Calling cl_qos_convert_r97_to_r99() for min qos");
        cl_qos_convert_r97_to_r99(&context->minimum_qos.qos.qos_r97, &tmp_qos);
        TRACE_EVENT("UMTS RAT.. Comparing only R99 part of QoS.");
        less_than = sm_qos_compare_r99aim_lt_r99(&aim_qos->qos_r99, &tmp_qos);
      }
      /*#endif*/
#endif

    }
  }
  else if (context->minimum_qos.ctrl_qos == PS_is_R99) {
    T_PS_qos_r99        tmp_qos;
    if (aim_qos->v_qos_r99 == (U8)FALSE) {
      cl_qos_convert_r97_to_r99(&context->minimum_qos.qos.qos_r97, &tmp_qos);
    }
  else {
      memcpy(&tmp_qos, &context->minimum_qos.qos.qos_r99, sizeof(T_PS_qos_r99));
    }

    less_than = sm_qos_compare_r99aim_lt_r99(&aim_qos->qos_r99, &tmp_qos);
  } else { /* if (context->ctrl_minimum_qos) */
    (void)TRACE_EVENT_P1("ERROR! Invalid union controller == %d in minimum QoS!",
                         context->minimum_qos.ctrl_qos);
  }

  return !less_than;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_is_minimum_satisfied_by_aim
+------------------------------------------------------------------------------
| Description : Comparison function. Compares a the minimum QoS parameter set
|               configured in context data with another QoS structure in
|               internal SM representation.  Returns TRUE if the comparison QoS
|               parameters are greater than or equal to the minimum parameters.
|               ("greater than" is different for each value).
|
| Parameters  : context          - Context data
|               sm_qos           - comparison QoS structure
+------------------------------------------------------------------------------
*/
BOOL sm_qos_is_minimum_satisfied_by_sm(struct T_SM_CONTEXT_DATA *context,
                                       T_SM_qos                 *sm_qos)
{
  BOOL  less_than = TRUE;

#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION( "sm_qos_is_minimum_satisfied_by_sm");
#endif
   /*lint -e613 (Possible use of null pointer 'context' in left argument to operator '->') */
  TRACE_ASSERT(context != NULL);

  if (context->minimum_qos.ctrl_qos == PS_is_qos_not_present) {
    /* Accept QoS without any checking */
    return TRUE; 
  } 
  if (context->minimum_qos.ctrl_qos == PS_is_R97) {
    T_PS_qos_r97 *min_qos = &context->minimum_qos.qos.qos_r97;
    if (sm_qos->ctrl_qos == PS_is_R97) {
      /*
        less_than = sm_qos_compare_r97_lt_r97(&sm_qos->qos.qos_r97, min_qos);
        * Construct not used, as this function is called only during
        * MO modify procedures, which is a R99 feature.
      */
      less_than = FALSE;
    } else {
      T_PS_qos_r99      tmp_qos;
      cl_qos_convert_r97_to_r99(min_qos, &tmp_qos);

      less_than = sm_qos_compare_r99_lt_r99(&sm_qos->qos.qos_r99, &tmp_qos);
    }
  } else if (context->minimum_qos.ctrl_qos == PS_is_R99) {
    T_PS_qos_r99 *min_qos = &context->minimum_qos.qos.qos_r99;
    if (sm_qos->ctrl_qos == PS_is_R97) {
      T_PS_qos_r99      tmp_qos;
      cl_qos_convert_r97_to_r99(&sm_qos->qos.qos_r97, &tmp_qos);

      less_than = sm_qos_compare_r99_lt_r99(&tmp_qos, min_qos);
    } else {
      less_than = sm_qos_compare_r99_lt_r99(&sm_qos->qos.qos_r99, min_qos);
    }
  } else {
    (void)TRACE_EVENT_P1("ERROR! Invalid union controller == %d in minimum QoS!",
                       context->minimum_qos.ctrl_qos);
    }


  return !less_than;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_copy_from_sm
+------------------------------------------------------------------------------
| Description : Copy QoS structure and union controller from context data to
|               another QoS structure (in a primitive etc).
|
| Parameters  : dst              - Destination QoS structure
|               src              - Source QoS structure
|               dctrl            - Destination union controller
+------------------------------------------------------------------------------
*/
void sm_qos_copy_from_sm(T_PS_qos *dst, T_SM_qos *src, T_PS_ctrl_qos *dctrl)
{
  *dctrl     = src->ctrl_qos;
  memcpy(dst, &src->qos, sizeof(T_PS_qos));
}

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_copy_to_sm
+------------------------------------------------------------------------------
| Description : Copy QoS structure and union controller from e.g. a primitive
|               to context data.
|
| Parameters  : dst              - Destination QoS structure (in context data)
|               src              - Source QoS structure
|               ctrl             - Source union controller
+------------------------------------------------------------------------------
*/
void sm_qos_copy_to_sm(T_SM_qos *dst, T_PS_qos *src, T_PS_ctrl_qos ctrl)
{
  dst->ctrl_qos  = ctrl;
  memcpy(&dst->qos, src, sizeof(T_PS_qos));
}

/*
+------------------------------------------------------------------------------
| Function    : sm_qos_is_requested_qos_present
+------------------------------------------------------------------------------
| Description : Returns TRUE if requested QoS element is present.
|
| Parameters  : context          - Context data
+------------------------------------------------------------------------------
*/
BOOL sm_qos_is_requested_qos_present(struct T_SM_CONTEXT_DATA *context)
{
   /*lint -e613 (Possible use of null pointer 'context' in left argument to operator '->') */
  TRACE_ASSERT(context != NULL);

  return (context->requested_qos.ctrl_qos != PS_is_qos_not_present);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_rank_del_contexts_based_on_tc
+------------------------------------------------------------------------------
| Description : Ranks all contexts linked to a pdp address based on traffic 
|               class and traffic handling priority. Locally deactivates all 
|               but the best ranking context. Informs ACI about the deactivation.
|               Also sends a pdp context status request to GMM.
|               Reference 23.107 Annex C
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
#ifdef TI_PS_OP_SM_RETAIN_BEST_CONTEXT

U16 sm_rank_del_contexts_based_on_tc()
{
  U8  nsapi        = 0;
  U8  temp_ti      = 0; 
  U8  count        = 0; 
  U8  temp_nsapi   = 0;
  U16 context_ti   = 0;
  U16 linked_contexts  = 0;
  U8  best_qos_rank = SM_HIGH_VALUE; /*Initialize with a high value*/
  U16 nsapis_to_deactivate = 0;

  for (nsapi=(int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
     struct T_SM_CONTEXT_DATA *context;
     struct T_SM_CONTEXT_DATA *temp_context;
  
     context = sm_get_context_data_from_nsapi(nsapi);
     /* The context_control_state is checked to see if the context is already 
      * getting deactivated
      */
     if ( context != NULL && 
        (context->context_control_state != SM_CONTEXT_DEACTIVATED) )
     {
       if (sm_is_secondary(context))
       {
         context_ti = context->linked_ti;
       } else {
         context_ti = context->ti;
       }
       linked_contexts = sm_add_nsapi_to_nsapi_set(nsapi, linked_contexts);
       count = 1; /*One context is already active*/ 
       /*Rank this context. May be needed if linked contexts are active*/
       context->qos_rank = sm_qos_rank_context(context->ti);
       if (context->qos_rank < best_qos_rank) {
           best_qos_rank = context->qos_rank;
       }
     } else { /*Go to the next nsapi*/
       continue; 
     }
     /* One context active. Check all the remaining active contexts */
     for (temp_nsapi=nsapi; temp_nsapi < NAS_SIZE_NSAPI; temp_nsapi++)
     {
       temp_context = sm_get_context_data_from_nsapi(temp_nsapi); 
       if(temp_context != NULL && temp_context->context_control_state
                                  !=  SM_CONTEXT_DEACTIVATED)
       {
          if (sm_is_secondary(temp_context))
          {
            temp_ti = temp_context->linked_ti;
          } else {
            temp_ti = temp_context->ti;
          }
          if ( (temp_nsapi != nsapi) && (temp_ti == context_ti) )
          {
            linked_contexts = sm_add_nsapi_to_nsapi_set(temp_nsapi, 
                                                   linked_contexts);
            count++; 
            /*A second context active. Rank it. Pass the ti of the context*/
            temp_context->qos_rank = sm_qos_rank_context(temp_context->ti);
            if (temp_context->qos_rank < best_qos_rank) {
              best_qos_rank = temp_context->qos_rank;
            }
          }
        }
     }

     if (count >=2 )
     {
       for (temp_nsapi=(int)NAS_NSAPI_5; temp_nsapi<NAS_SIZE_NSAPI; temp_nsapi++)
       {
         struct T_SM_CONTEXT_DATA *context;
         if( (linked_contexts & (0x0001 << temp_nsapi)) != 0 )
         {
           context = sm_get_context_data_from_nsapi(temp_nsapi); 
           if ( (context->qos_rank > best_qos_rank) && (context != NULL)
                && (context->context_control_state != SM_CONTEXT_DEACTIVATED) )
           { 
              (void)TRACE_EVENT_P1("Deactivating nsapi >>TC based << %d; ", temp_nsapi);
              /*deactivate the context*/
              nsapis_to_deactivate = sm_add_nsapi_to_nsapi_set(temp_nsapi,
                                                      nsapis_to_deactivate);
              /* Make sure to clear the pending reactivation flag */
              sm_set_context_pending_reactivation(context, FALSE);
              /* Order context deactivation */
              sm_context_control(context, SM_I_CONTEXT_LOCAL_DEACTIVATE, 
                                          (void *)TRUE);
            } 
          }
       }
     }
  }

return nsapis_to_deactivate;
}
#endif /*#ifdef TI_PS_OP_SM_RETAIN_BEST_CONTEXT*/


/*
+------------------------------------------------------------------------------
| Function    : sm_qos_rank_context
+------------------------------------------------------------------------------
| Description : Provides a rank for the context based on the tc and handling pri
|               Reference 23.107 Annex C
|
| Parameters  : context ti
+------------------------------------------------------------------------------
*/
#ifdef TI_PS_OP_SM_RETAIN_BEST_CONTEXT

U8 sm_qos_rank_context(U16 ti)
{
  struct T_SM_CONTEXT_DATA *context;
  T_PS_qos_r99 temp_qos;
  U8           qos_rank = 0;

  context = sm_get_context_data_from_ti(ti);

  if(context->accepted_qos.ctrl_qos == PS_is_R97)
  { /*R97 QoS. Convert to R99 QoS before ranking*/
     (void)cl_qos_convert_r97_to_r99(&context->accepted_qos.qos.qos_r97,
                                     &temp_qos);
  } else
  {
     temp_qos = context->accepted_qos.qos.qos_r99;
  }

  switch(temp_qos.tc)
  {
    case PS_TC_INTER:
      switch(temp_qos.handling_pri)
      {
        case PS_HANDLING_PRI_1: 
          qos_rank = 1; 
          break;
        case PS_HANDLING_PRI_2: 
          qos_rank = 4; 
          break;
        case PS_HANDLING_PRI_3: 
          qos_rank = 5; 
          break;
       }
      break;
    case PS_TC_CONV:
      qos_rank = 2; 
      break;
    case PS_TC_STREAM:
      qos_rank = 3; 
      break;
    case PS_TC_BG:
      qos_rank = 6; 
      break;
    default:
      qos_rank = 6;
      break;
  }
  return qos_rank;
}
#endif /*#ifdef TI_PS_OP_SM_RETAIN_BEST_CONTEXT*/

/*
+------------------------------------------------------------------------------
| Function    : sm_retain_cntxt_wth_best_bitrate
+------------------------------------------------------------------------------
| Description : Deactivates all the linked pdp contexts except the one with 
|               highest max bit rate uplink or downlink. Informs ACI about the 
|               deactivation.
|               Also sends a pdp context status request to GMM.
|               Reference 23.107 Annex C
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
#ifdef TI_PS_OP_SM_RETAIN_BEST_CONTEXT

U16 sm_retain_cntxt_wth_best_bitrate()
{
  U8  nsapi      = 0;
  U8  temp_nsapi = 0;
  U8  count      = 0;
  U16 linked_contexts      = 0;        
  U16 best_max_bitrate     = 0;
  U16 nsapis_to_deactivate = 0;
  U16 context_ti           = 0;
  U16 temp_ti              = 0;
  T_PS_qos_r99 temp_qos;


  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    struct T_SM_CONTEXT_DATA *context;
    struct T_SM_CONTEXT_DATA *temp_context;

    context = sm_get_context_data_from_nsapi(nsapi);
    if ( (context != NULL) && (context->context_control_state 
                               != SM_CONTEXT_DEACTIVATED) )
    {
      if (sm_is_secondary(context))
      {
        context_ti = context->linked_ti;
      } else {
        context_ti = context->ti;
      }
      linked_contexts = sm_add_nsapi_to_nsapi_set(nsapi, linked_contexts);
      count = 1; /*One context is already active*/ 
    } else { /* Process the next nsapi */
      continue; 
    }

    for(temp_nsapi=nsapi; temp_nsapi < NAS_SIZE_NSAPI; temp_nsapi++)
    {
      temp_context = sm_get_context_data_from_nsapi(temp_nsapi); 
      if(temp_context != NULL && temp_context->context_control_state
                               !=  SM_CONTEXT_DEACTIVATED)
      {
         if (sm_is_secondary(temp_context))
         {
           temp_ti = temp_context->linked_ti;
         } else {
           temp_ti = temp_context->ti;
         }
         if ( (temp_nsapi != nsapi) && (temp_ti == context_ti) )
         {
           linked_contexts = sm_add_nsapi_to_nsapi_set(temp_nsapi, 
                                                       linked_contexts);
           count++; 
         }
      }
   }

   if (count >=2)
   { /*Multiple contexts active for this ti. Find the best_max_bit rate.*/
     (void)TRACE_EVENT( "Multiple contexts active: sm_retain_cntxt_wth_best_bitrate" );
     for (temp_nsapi=(int)NAS_NSAPI_5; temp_nsapi < NAS_SIZE_NSAPI; temp_nsapi++)
     {
       if( (linked_contexts & (0x0001 << temp_nsapi)) != 0 )
       {
          context = sm_get_context_data_from_nsapi(temp_nsapi); 
          if(context->accepted_qos.ctrl_qos == PS_is_R97)
          { /*R97 QoS. Convert to R99 QoS before ranking*/
            (void)cl_qos_convert_r97_to_r99(&context->accepted_qos.qos.qos_r97,
                                            &temp_qos);
          } else  {
            temp_qos = context->accepted_qos.qos.qos_r99;
          }
          if (temp_qos.max_rate_dl >= best_max_bitrate)
          {
             best_max_bitrate = temp_qos.max_rate_dl; 
          } else if (temp_qos.max_rate_ul >= best_max_bitrate)  {
              best_max_bitrate = temp_qos.max_rate_ul; 
          } 
        }
     }

   }

   if (count >=2) 
   {
     for (temp_nsapi=(int)NAS_NSAPI_5; temp_nsapi < NAS_SIZE_NSAPI; temp_nsapi++)
     {
       if( (linked_contexts & (0x0001 << temp_nsapi)) != 0 )
       {
          context = sm_get_context_data_from_nsapi(temp_nsapi); 
          if(context->accepted_qos.ctrl_qos == PS_is_R97)
          { /*R97 QoS. Convert to R99 QoS before ranking*/
            (void)cl_qos_convert_r97_to_r99(&context->accepted_qos.qos.qos_r97,
                                            &temp_qos);
          } else  {
            temp_qos = context->accepted_qos.qos.qos_r99;
          }
          if ( (temp_qos.max_rate_dl == best_max_bitrate) || 
               (temp_qos.max_rate_ul == best_max_bitrate) )
          { 
             /* Don't do anything */
          } 
          else /* Deactivate the context */
          { 
             (void)TRACE_EVENT_P1("Deactivating nsapi << bit rate based >> %d; ", temp_nsapi);
             /*Deactivate the context*/
             nsapis_to_deactivate = sm_add_nsapi_to_nsapi_set(temp_nsapi, 
                                    nsapis_to_deactivate);
             /* Make sure to clear the pending reactivation flag */
             sm_set_context_pending_reactivation(context, FALSE);
             /* Order context deactivation */
             sm_context_control(context, SM_I_CONTEXT_LOCAL_DEACTIVATE, 
                                         (void *)TRUE);
          }
       }
     }
   }

  }
 return nsapis_to_deactivate;
}

#endif /*#ifdef TI_PS_OP_SM_RETAIN_BEST_CONTEXT*/

/*
+------------------------------------------------------------------------------
| Function    : sm_retain_cntxt_with_least_nsapi
+------------------------------------------------------------------------------
| Description : Deactivates all the linked pdp contexts except the one with 
|               least nsapi value. Informs ACI about the deactivation.
|               Also sends a pdp context status request to GMM.
|               Reference 23.107 Annex C
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
#ifdef TI_PS_OP_SM_RETAIN_BEST_CONTEXT

U16 sm_retain_cntxt_with_least_nsapi()
{
  struct T_SM_CONTEXT_DATA *context;
  struct T_SM_CONTEXT_DATA *temp_context;
  U8  nsapi                = 0;
  U8  temp_nsapi           = 0;
  U16 linked_contexts      = 0;
  U16 nsapis_to_deactivate = 0;
  U16 best_max_bitrate     = 0;
  U8  min_nsapi = SM_HIGH_VALUE; /*Initialize to a high value. Here 255*/
  U8  count                = 0;
  U16 context_ti,temp_ti   = 0;


  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    context = sm_get_context_data_from_nsapi(nsapi);
    if ( (context != NULL) && (context->context_control_state != 
                           SM_CONTEXT_DEACTIVATED) )
    {
      if (sm_is_secondary(context))
       {
         context_ti = context->linked_ti;
       } else {
         context_ti = context->ti;
       }
       linked_contexts = sm_add_nsapi_to_nsapi_set(nsapi, linked_contexts);
       count = 1; /*One context is already active*/ 
     } else { /*Go to the next nsapi*/
       continue;
     }
    
    /* Check all the remaining active contexts */
     for (temp_nsapi=nsapi; temp_nsapi < NAS_SIZE_NSAPI; temp_nsapi++)
     {
       temp_context = sm_get_context_data_from_nsapi(temp_nsapi);
       if(temp_context != NULL && temp_context->context_control_state
                                  !=  SM_CONTEXT_DEACTIVATED)
       {
          if (sm_is_secondary(temp_context))
          {
            temp_ti = temp_context->linked_ti;
          } else {
            temp_ti = temp_context->ti;
          }
          if ( (temp_nsapi != nsapi) && (temp_ti == context_ti) )
          {
            linked_contexts = sm_add_nsapi_to_nsapi_set(temp_nsapi, 
                                                        linked_contexts);
            count++; 
          }
        }
     }


    if (count >= 2)
    { /*Multiple contexts active for this ti. Try deactivating.*/
      (void)TRACE_EVENT( "Multiple contexts active: sm_retain_cntxt_with_least_nsapi" );
      for (temp_nsapi=(int)NAS_NSAPI_5; temp_nsapi<NAS_SIZE_NSAPI; temp_nsapi++)
      {
        if( (linked_contexts & (0x0001 << temp_nsapi)) != 0 )
        {
          temp_context = sm_get_context_data_from_nsapi(temp_nsapi);
          if ((min_nsapi == SM_HIGH_VALUE) && (temp_context != NULL) && 
              (temp_context->context_control_state != SM_CONTEXT_DEACTIVATED) )
          {
            min_nsapi = temp_nsapi;
          } else
            {
               (void)TRACE_EVENT_P1("Deactivating nsapi >>nsapi based << %d; ", temp_nsapi);
               /*Deactivate the context*/
               nsapis_to_deactivate = sm_add_nsapi_to_nsapi_set(temp_nsapi, 
                                                      nsapis_to_deactivate);
               /* Make sure to clear the pending reactivation flag */
               sm_set_context_pending_reactivation(temp_context, FALSE);
               /* Order context deactivation */
               sm_context_control(temp_context, SM_I_CONTEXT_LOCAL_DEACTIVATE,
                                                                (void *)TRUE);
            }
         }
      }
    }
 }
  return nsapis_to_deactivate;
}

#endif /*#ifdef TI_PS_OP_SM_RETAIN_BEST_CONTEXT*/

