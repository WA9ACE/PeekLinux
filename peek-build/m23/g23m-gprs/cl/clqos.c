/*-----------------------------------------------------------------------------
|  Project :  3G UMTS PS
|  Module  :  CL
+------------------------------------------------------------------------------
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
+------------------------------------------------------------------------------
| Purpose:    QoS handling functions.
|             For design details, see:
|             8010.149 CL SAP
+----------------------------------------------------------------------------*/

#ifndef CLQOS_C
#define CLQOS_C
#endif

#ifndef ENTITY_CL
#define ENTITY_CL
#endif
#define ENTITY_CLT

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get TI data types */
#include "vsi.h"        /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
#include "message.h"    /* to get the message definitions for sm */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "p_cl.val"  /* to get the type of sgsn_rel*/
#include "p_8010_152_ps_include.h" /*to get T_PS_qos_rXX*/
#include "cl_inline.h"


typedef U8 T_CL_result; /*I really don't like this ! Can we do something in cl.sap ?*/

/*==== CONST ================================================================*/

/*
 * Conversion table for 1/n in T_ratio.
 * Contains 1/n * 1E9 entries used for scaling.
 */
static const U32 cl_qos_ratio_table[10] = {
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

/*-----------------------------------------------------------------------------
| Function    : cl_qos_ratio_to_U32
+------------------------------------------------------------------------------
| Description : Conversion function: T_ratio to integer
|
| Parameters  : T_ratio
|
| Return      : ratio as integer
+----------------------------------------------------------------------------*/
static U32 cl_qos_ratio_to_U32(U8 ratio_mant, U8 ratio_exp)
{
  U32   value;
  int   count;

  /* Error checks */
  if (ratio_exp > (U8)9) 
  {
    /*TRACE_ERROR( "QoS ratio exponent parameter out of bounds!" );*/
    return 0;
  }
  else
  {
    if (ratio_mant > (U8)9)
    {
      /*TRACE_ERROR( "QoS ratio exponent parameter out of bounds!" );*/
      return 0;
    }
    else
    {
      if (ratio_exp == (U8)9 && ratio_mant < (U8)5)
      {
        /*TRACE_ERROR( "QoS ratio parameter exceeds data type range!" );*/
        return 0;
      }
    }
  }

  for (count = 9-(int)ratio_exp, value = 1UL; count > 0; count--)
  {
    value *= 10;
  }
  /* Get real value by rounding 1/x and dividing by 10^(9-ratio_exp) */
  value = (cl_qos_ratio_table[(U16)ratio_mant] + (value >> 1)) / value;

  return value;
}

/*-----------------------------------------------------------------------------
| Function    : cl_qos_r99_to_delay
+------------------------------------------------------------------------------
| Description : Given an input R99 QoS, return the corresponding R97 delay
|               parameter
|
| Parameters  : R99 Qos
|
| Return      : Delay
+----------------------------------------------------------------------------*/
static T_PS_delay cl_qos_r99_to_delay(T_PS_qos_r99 *src_qos_r99)
{
  T_PS_delay delay;

  /* Traffic class / handling priority */
  switch ((T_PS_tc)src_qos_r99->tc)
  {
    case PS_TC_SUB:       /* 0 - Subscribed */
      delay               = PS_DELAY_SUB;
      break;
    case PS_TC_CONV:      /* 1 - Conversational */
      delay               = PS_DELAY_1;
      break;
    case PS_TC_STREAM:    /* 2 - Streaming */
      delay               = PS_DELAY_1;
      break;
    case PS_TC_INTER:     /* 3 - Interactive */
      switch (src_qos_r99->handling_pri) 
      {
        case PS_HANDLING_PRI_1:
          delay             = PS_DELAY_1;
          break;
        case PS_HANDLING_PRI_2:
          delay             = PS_DELAY_2;
          break;
        case PS_HANDLING_PRI_3:
        default:            /* For safety */
          delay             = PS_DELAY_3;
          break;
      }   /* switch */
      break;
    case PS_TC_BG:        /* 4 - Background */
      delay               = PS_DELAY_4;
      break;
    default:              /* 5, 6, 7 or other garbage */
      delay               = PS_DELAY_SUB;
      break;
  }     /* switch */

  return delay;
} /* cl_qos_r99_to_delay */

/*-----------------------------------------------------------------------------
| Function    : cl_qos_r99_to_relclass
+------------------------------------------------------------------------------
| Description : Given an input R99 QoS, return the corresponding R97 relclass
|               parameter
|
| Parameters  : R99 Qos
|
| Return      : Reliability class
+----------------------------------------------------------------------------*/
static T_PS_relclass cl_qos_r99_to_relclass(T_PS_qos_r99 *src_qos_r99)
{
  U32             sdu_err;
  T_PS_relclass  relclass;

  /* First, convert SDU error ratio to integral number */
  sdu_err = cl_qos_ratio_to_U32(src_qos_r99->sdu_err_ratio.ratio_mant,
                                src_qos_r99->sdu_err_ratio.ratio_exp);

  if (sdu_err == 0UL)
  {             /* SDU err ratio == subscribed value */
    relclass       = PS_RELCLASS_SUB;      /* 0 */
  }
  else
  {
    if (sdu_err >= 100000UL)
    { /* SDU err ratio <= 1E-5 */
      relclass         = PS_LLC_RLC_PROT;    /* 2 */
    }
    else
    {
      if (sdu_err >= 2000UL)
      {  /* 1E-5 < SDU err ratio <= 5E-4 */
        relclass         = PS_RLC_PROT;        /* 3 */
      }
      else
      {                        /* SDU err ratio > 5E-4 */
        if (src_qos_r99->ber.ratio_exp > (U8)4 ||
            (src_qos_r99->ber.ratio_exp == (U8)4 && src_qos_r99->ber.ratio_mant >= (U8)2))
        {
                                  /* BER <= 2E-4 */
          relclass       = PS_PROT;              /* 4 */
        }
        else
        {                    /* BER > 2E-4 */
          relclass       = PS_NO_REL;            /* 5 */
        }
      }
    }
  }  /* if (sdu_err ...) */

  return relclass;
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*-----------------------------------------------------------------------------
| Function    : cl_qos_convert_r99_to_r97_req
+------------------------------------------------------------------------------
| Description : Function for converting a R99 QoS parameter set to a R97/R98
|               ditto. Conversion rules according to [3G 23.107, sec 9]
|
| Parameters  : Originating R99 QoS parameter struct
|               Destination R97 QoS parameter struct (overwritten in this function)
|
| Return      : Conversion result and Destination R97 QoS parameter struct.
+----------------------------------------------------------------------------*/
T_CL_result cl_qos_convert_r99_to_r97(T_PS_qos_r99 *src_qos_r99,
                                      T_PS_qos_r97 *dst_qos_r97)
{
  T_CL_result  success = (T_CL_result)TRUE;
  U16          count, max_rate;

  dst_qos_r97->delay    = (U8)cl_qos_r99_to_delay(src_qos_r99);

  /*
   * SDU error ratio / Residual bit error ratio
   */
  dst_qos_r97->relclass = (U8)cl_qos_r99_to_relclass(src_qos_r99);

  /*
   * Maximum bit-rate
   *
   * In case max uplink and downlink data rates differ, [3G 23.107]
   * says to map according to max(R99_uplink, R99_downlink),
   * i.e. the higher of the two.
   */
  max_rate = (src_qos_r99->max_rate_ul > src_qos_r99->max_rate_dl ?
              src_qos_r99->max_rate_ul : src_qos_r99->max_rate_dl);

  if (max_rate == (U16)PS_MAX_BR_UL_SUB)
  { /* Subscribed */
    dst_qos_r97->peak = (U8)PS_PEAK_SUB;
  }
  else
  { /* ! Subscribed */
    /* Perform pseudo log2(bit-rate).  Really, we are finding the highest set
     * bit to use in our comparison rather than making a *huge*
     * if-[else if]*-else clause.
     * Fixed ceiling to 2048 (count < 12) => max_rate <= 2^11 */
    for (count = (U16)4; ((max_rate >> count) != 0) && count < (U16)12; count++);

    /* The quick / code optimal way:
     * dst_qos_r97->peak = count - 3; */

    switch (count)
    {
      case 4:  dst_qos_r97->peak = (U8)PS_PEAK_1K;   break;
      case 5:  dst_qos_r97->peak = (U8)PS_PEAK_2K;   break;
      case 6:  dst_qos_r97->peak = (U8)PS_PEAK_4K;   break;
      case 7:  dst_qos_r97->peak = (U8)PS_PEAK_8K;   break;
      case 8:  dst_qos_r97->peak = (U8)PS_PEAK_16K;  break;
      case 9:  dst_qos_r97->peak = (U8)PS_PEAK_32K;  break;
      case 10: dst_qos_r97->peak = (U8)PS_PEAK_64K;  break;
      case 11: dst_qos_r97->peak = (U8)PS_PEAK_128K; break;
      case 12: dst_qos_r97->peak = (U8)PS_PEAK_256K; break;
    }   /* switch */
  }     /* if - Subscribed */

  /* R97 peak throughput class is fixed to 31 (best effort) */
  dst_qos_r97->mean   = (U8)PS_MEAN_BEST;
  dst_qos_r97->preced = (U8)PS_PRECED_SUB;

  return success;
}

/*-----------------------------------------------------------------------------
| Function    : cl_qos_convert_r97_to_r99_req
+------------------------------------------------------------------------------
| Description : Function for converting a R97/R98 QoS parameter set to a R99
|               ditto. Conversion rules according to [3G 23.107, sec 9]
|
| Parameters  : Originating R97 QoS parameter struct
|               Destination R99 QoS parameter struct (overwritten in this function)
|
| Return      : Conversion result and Destination R99 QoS parameter struct.
+----------------------------------------------------------------------------*/
T_CL_result cl_qos_convert_r97_to_r99(T_PS_qos_r97 *src_qos_r97,
                                      T_PS_qos_r99 *dst_qos_r99)
{
  T_CL_result  success = (T_CL_result)TRUE;

  /* Delay parameter */
  switch ((T_PS_delay)src_qos_r97->delay)
  {
    case PS_DELAY_SUB:               /* 0 */
      dst_qos_r99->tc           = (U8)PS_TC_SUB;
      dst_qos_r99->handling_pri = (U8)PS_HANDLING_PRI_SUB;
      break;
    case PS_DELAY_1:                 /* 1 */
      dst_qos_r99->tc           = (U8)PS_TC_INTER;
      dst_qos_r99->handling_pri = (U8)PS_HANDLING_PRI_1;
      break;
    case PS_DELAY_2:                 /* 2 */
      dst_qos_r99->tc           = (U8)PS_TC_INTER;
      dst_qos_r99->handling_pri = (U8)PS_HANDLING_PRI_2;
      break;
    case PS_DELAY_3:                 /* 3 */
      dst_qos_r99->tc           = (U8)PS_TC_INTER;
      dst_qos_r99->handling_pri = (U8)PS_HANDLING_PRI_3;
      break;
    case PS_DELAY_4:                 /* 4 */
      dst_qos_r99->tc           = (U8)PS_TC_BG;
      dst_qos_r99->handling_pri = (U8)PS_HANDLING_PRI_SUB;
      break;
    default:                      /* 5 - 7 or other garbage */
      /* Error handling? */
      success = (T_CL_result)FALSE;
      break;
  }     /* switch */

  /* Reliability class */
  switch ((T_PS_relclass)src_qos_r97->relclass)
  {
    case PS_RELCLASS_SUB:                      /* 0 - Subscribed */
      dst_qos_r99->sdu_err_ratio.ratio_mant = dst_qos_r99->sdu_err_ratio.ratio_exp = (U8)0;
      dst_qos_r99->ber.ratio_mant = dst_qos_r99->ber.ratio_exp = (U8)0;
      dst_qos_r99->del_err_sdu = (U8)PS_DEL_ERR_SUB;
      break;
    case PS_GTP_LLC_RLC_PROT:                  /* 1 - Acknowledged GTP, LLC, and RLC */
      dst_qos_r99->sdu_err_ratio.ratio_mant = (U8)1;
      dst_qos_r99->sdu_err_ratio.ratio_exp  = (U8)6; /* SDU error ratio = 1E-6 */
      dst_qos_r99->ber.ratio_mant           = (U8)1;
      dst_qos_r99->ber.ratio_exp            = (U8)5; /* Residual BER = 1E-5 */
      dst_qos_r99->del_err_sdu = (U8)PS_DEL_ERR_NO;
      break;
    case PS_LLC_RLC_PROT:                      /* 2 - Unacknowledged GTP; Acknowledged LLC and RLC */
      dst_qos_r99->sdu_err_ratio.ratio_mant = (U8)1;
      dst_qos_r99->sdu_err_ratio.ratio_exp  = (U8)6; /* SDU error ratio = 1E-6 */
      dst_qos_r99->ber.ratio_mant           = (U8)1;
      dst_qos_r99->ber.ratio_exp            = (U8)5; /* Residual BER = 1E-5 */
      dst_qos_r99->del_err_sdu = (U8)PS_DEL_ERR_NO;
      break;
    case PS_RLC_PROT:                          /* 3 - Unacknowledged GTP and LLC; Acknowledged RLC */
      dst_qos_r99->sdu_err_ratio.ratio_mant = (U8)1;
      dst_qos_r99->sdu_err_ratio.ratio_exp  = (U8)4; /* SDU error ratio = 1E-4 */
      dst_qos_r99->ber.ratio_mant           = (U8)1;
      dst_qos_r99->ber.ratio_exp            = (U8)5; /* Residual BER = 1E-5 */
      dst_qos_r99->del_err_sdu = (U8)PS_DEL_ERR_NO;
      break;
    case PS_PROT:                              /* 4 - Unacknowledged GTP, LLC, and RLC */
      dst_qos_r99->sdu_err_ratio.ratio_mant = (U8)1;
      dst_qos_r99->sdu_err_ratio.ratio_exp  = (U8)3; /* SDU error ratio = 1E-3 */
      dst_qos_r99->ber.ratio_mant           = (U8)1;
      dst_qos_r99->ber.ratio_exp            = (U8)5; /* Residual BER = 1E-5 */
      dst_qos_r99->del_err_sdu = (U8)PS_DEL_ERR_NO;
      break;
    case PS_NO_REL:                            /* 5 - Unacknowledged GTP, LLC, and RLC (Unprotected) */
      dst_qos_r99->sdu_err_ratio.ratio_mant = (U8)1;
      dst_qos_r99->sdu_err_ratio.ratio_exp  = (U8)3; /* SDU error ratio = 1E-3 */
      dst_qos_r99->ber.ratio_mant           = (U8)4;
      dst_qos_r99->ber.ratio_exp            = (U8)3; /* Residual BER = 4E-3 */
      dst_qos_r99->del_err_sdu = (U8)PS_DEL_ERR_YES;
      break;
    default:                                      /* 6, 7 or other garbage */
      /* Error handling? */
      success = (T_CL_result)FALSE;
      break;
  }     /* switch */

  /* Peak throughput parameter */
  /* NOTE: Could be optimized heavily into the following:
   * dst_qos_r99->max_br_dl = dst_qos_r99->max_br_ul = (peak == 0 ? 0 : 4 << peak)
   */

  switch ((T_PS_peak)src_qos_r97->peak)
  {
    case PS_PEAK_SUB:             /* 0 */
      dst_qos_r99->max_rate_dl = dst_qos_r99->max_rate_ul = (U16)PS_MAX_BR_UL_SUB;
      break;
    case PS_PEAK_1K:              /* 1 */
      dst_qos_r99->max_rate_dl = dst_qos_r99->max_rate_ul = (U16)8;
      break;
    case PS_PEAK_2K:              /* 2 */
      dst_qos_r99->max_rate_dl = dst_qos_r99->max_rate_ul = (U16)16;
      break;
    case PS_PEAK_4K:              /* 3 */
      dst_qos_r99->max_rate_dl = dst_qos_r99->max_rate_ul = (U16)32;
      break;
    case PS_PEAK_8K:              /* 4 */
      dst_qos_r99->max_rate_dl = dst_qos_r99->max_rate_ul = (U16)64;
      break;
    case PS_PEAK_16K:             /* 5 */
      dst_qos_r99->max_rate_dl = dst_qos_r99->max_rate_ul = (U16)128;
      break;
    case PS_PEAK_32K:             /* 6 */
      dst_qos_r99->max_rate_dl = dst_qos_r99->max_rate_ul = (U16)256;
      break;
    case PS_PEAK_64K:             /* 7 */
      dst_qos_r99->max_rate_dl = dst_qos_r99->max_rate_ul = (U16)512;
      break;
    case PS_PEAK_128K:            /* 8 */
      dst_qos_r99->max_rate_dl = dst_qos_r99->max_rate_ul = (U16)1024;
      break;
    case PS_PEAK_256K:            /* 9 */
      dst_qos_r99->max_rate_dl = dst_qos_r99->max_rate_ul = (U16)2048;
      break;
    default:
      /* Handling? */
      success = (T_CL_result)FALSE;
      break;
  }     /* switch */

  /* R99 max SDU size is fixed to 1500 */
  dst_qos_r99->max_sdu  = (U16)1500;

  /* Remaining R99 fields are not specified. We fill them in with subscribed */
  dst_qos_r99->order      =  (U8)PS_ORDER_SUB;
  dst_qos_r99->xfer_delay = (U16)PS_XFER_DELAY_SUB;
  dst_qos_r99->guar_br_ul = (U16)PS_GUAR_BR_UL_SUB;
  dst_qos_r99->guar_br_dl = (U16)PS_GUAR_BR_DL_SUB;

  return success;
}

/*-----------------------------------------------------------------------------
| Function    : cl_qos_use_ack_mode_llc_req
+------------------------------------------------------------------------------
| Description : Query function: Given input QoS, should NSAPI use acknowledged
|               LLC mode?
|
| Parameters  : Originating R99 QoS parameter struct
|
| Return      : LLC mode
+----------------------------------------------------------------------------*/
T_PS_llc_mode cl_qos_use_ack_mode_llc(T_PS_qos_r99 *src_qos_r99)
{
  T_PS_relclass  relclass;
  T_PS_llc_mode   ret_val = PS_LLC_UNACKNOWLEDGED;

  relclass = cl_qos_r99_to_relclass(src_qos_r99);

  if (relclass == PS_GTP_LLC_RLC_PROT || relclass == PS_LLC_RLC_PROT)
  {
    ret_val = PS_LLC_ACKNOWLEDGED;
  }

  return ret_val;
}
