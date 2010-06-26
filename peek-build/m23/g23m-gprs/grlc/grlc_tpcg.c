/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
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
|  Purpose :  This module implements local functions for service
|             TPC of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

/*
 * !!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!
 * 
 * This file was previously named grlc_tpcf.c.
 *
 * A renaming was necessary due to an ClearCase evil twin problem across the
 * TCS2/TCS3/TCS4 and the TCS5 programs.
 *
 * The same applied for the file grlc_tpcf.h.
 * 
 * !!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!
 */

#ifndef GRLC_TPCG_C
#define GRLC_TPCG_C
#endif /* #ifndef GRLC_TPCG_C */

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"
#include "ccdapi.h"
#include "prim.h"
#include "message.h"
#include "grlc.h"

/*==== CONST ================================================================*/

#define MIN_PCL_GSM900  31 /* minimum power control level GSM 900  */
#define MAX_PCL_GSM900  0  /* maximum power control level GSM 900  */
#define MIN_PCL_DCS1800 28 /* minimum power control level DCS 1800 */
#define MAX_PCL_DCS1800 29 /* maximum power control level DCS 1800 */
#define MIN_PCL_PCS1900 21 /* minimum power control level PCS 1900 */
#define MAX_PCL_PCS1900 22 /* maximum power control level PCS 1900 */

#define GAMMA_0_GSM900  39
#define GAMMA_0_DCS1800 36
#define GAMMA_0_PCS1900 36

#define MAX_PCL 32

/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/
/*
 * The following tables converts the air-interface coding of
 * the system information parameter MAX_TXPWR_CCCH to a value
 * in dBm depending on the frequency standard (GSM, DCS or PCS).
 */
const UBYTE pcl_to_dbm_gsm [MAX_PCL] =
{                                      39,   /*  0 -> 39 dBm */
                                       39,   /*  1 -> 39 dBm */
                                       39,   /*  2 -> 39 dBm */
                                       37,   /*  3 -> 37 dBm */
                                       35,   /*  4 -> 35 dBm */
                                       33,   /*  5 -> 33 dBm */
                                       31,   /*  6 -> 31 dBm */
                                       29,   /*  7 -> 29 dBm */
                                       27,   /*  8 -> 27 dBm */
                                       25,   /*  9 -> 25 dBm */
                                       23,   /* 10 -> 23 dBm */
                                       21,   /* 11 -> 21 dBm */
                                       19,   /* 12 -> 19 dBm */
                                       17,   /* 13 -> 17 dBm */
                                       15,   /* 14 -> 15 dBm */
                                       13,   /* 15 -> 13 dBm */
                                       11,   /* 16 -> 11 dBm */
                                        9,   /* 17 ->  9 dBm */
                                        7,   /* 18 ->  7 dBm */
                                        5,   /* 19 ->  5 dBm */
                                        5,   /* 20 ->  5 dBm */
                                        5,   /* 21 ->  5 dBm */
                                        5,   /* 22 ->  5 dBm */
                                        5,   /* 23 ->  5 dBm */
                                        5,   /* 24 ->  5 dBm */
                                        5,   /* 25 ->  5 dBm */
                                        5,   /* 26 ->  5 dBm */
                                        5,   /* 27 ->  5 dBm */
                                        5,   /* 28 ->  5 dBm */
                                        5,   /* 29 ->  5 dBm */
                                        5,   /* 30 ->  5 dBm */
                                        5    /* 31 ->  5 dBm */
};

const UBYTE pcl_to_dbm_dcs [MAX_PCL] =
{                                      30,   /*  0 -> 30 dBm */
                                       28,   /*  1 -> 28 dBm */
                                       26,   /*  2 -> 26 dBm */
                                       24,   /*  3 -> 24 dBm */
                                       22,   /*  4 -> 22 dBm */
                                       20,   /*  5 -> 20 dBm */
                                       18,   /*  6 -> 18 dBm */
                                       16,   /*  7 -> 16 dBm */
                                       14,   /*  8 -> 14 dBm */
                                       12,   /*  9 -> 12 dBm */
                                       10,   /* 10 -> 10 dBm */
                                        8,   /* 11 ->  8 dBm */
                                        6,   /* 12 ->  6 dBm */
                                        4,   /* 13 ->  4 dBm */
                                        2,   /* 14 ->  2 dBm */
                                        0,   /* 15 ->  0 dBm */
                                        0,   /* 16 ->  0 dBm */
                                        0,   /* 17 ->  0 dBm */
                                        0,   /* 18 ->  0 dBm */
                                        0,   /* 19 ->  0 dBm */
                                        0,   /* 20 ->  0 dBm */
                                        0,   /* 21 ->  0 dBm */
                                        0,   /* 22 ->  0 dBm */
                                        0,   /* 23 ->  0 dBm */
                                        0,   /* 24 ->  0 dBm */
                                        0,   /* 25 ->  0 dBm */
                                        0,   /* 26 ->  0 dBm */
                                        0,   /* 27 ->  0 dBm */
                                        0,   /* 28 ->  0 dBm */
                                       36,   /* 29 -> 36 dBm */
                                       34,   /* 30 -> 34 dBm */
                                       32    /* 31 -> 32 dBm */
};

const UBYTE pcl_to_dbm_pcs [MAX_PCL] =
{                                      30,   /*  0 -> 30 dBm */
                                       28,   /*  1 -> 28 dBm */
                                       26,   /*  2 -> 26 dBm */
                                       24,   /*  3 -> 24 dBm */
                                       22,   /*  4 -> 22 dBm */
                                       20,   /*  5 -> 20 dBm */
                                       18,   /*  6 -> 18 dBm */
                                       16,   /*  7 -> 16 dBm */
                                       14,   /*  8 -> 14 dBm */
                                       12,   /*  9 -> 12 dBm */
                                       10,   /* 10 -> 10 dBm */
                                        8,   /* 11 ->  8 dBm */
                                        6,   /* 12 ->  6 dBm */
                                        4,   /* 13 ->  4 dBm */
                                        2,   /* 14 ->  2 dBm */
                                        0,   /* 15 ->  0 dBm */
                                        0,   /* 16 ->  0 dBm */
                                        0,   /* 17 ->  0 dBm */
                                        0,   /* 18 ->  0 dBm */
                                        0,   /* 19 ->  0 dBm */
                                        0,   /* 20 ->  0 dBm */
                                        0,   /* 21 ->  0 dBm */
                                       33,   /* 22 -> 33 dBm */
                                       33,   /* 23 -> 33 dBm */
                                       33,   /* 24 -> 33 dBm */
                                       33,   /* 25 -> 33 dBm */
                                       33,   /* 26 -> 33 dBm */
                                       33,   /* 27 -> 33 dBm */
                                       33,   /* 28 -> 33 dBm */
                                       33,   /* 29 -> 33 dBm */
                                       33,   /* 30 -> 33 dBm */
                                       32    /* 31 -> 32 dBm */
};

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : tpc_get_ms_pwr_cap
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tpc_get_ms_pwr_cap ( UBYTE         band_indicator,
                                 T_MS_PWR_CAP *ms_pwr_cap )
{ 
  TRACE_FUNCTION( "tpc_get_ms_pwr_cap" ); 

  switch( band_indicator )
  {
    default:
    case CGRLC_GSM_400:
    case CGRLC_GSM_850:
    case CGRLC_GSM_900:
      ms_pwr_cap->p_control = pcl_to_dbm_gsm;
      ms_pwr_cap->pwr_min   = pcl_to_dbm_gsm[MIN_PCL_GSM900];
      ms_pwr_cap->gamma_0   = GAMMA_0_GSM900;
      break;

    case CGRLC_DCS_1800:
      ms_pwr_cap->p_control = pcl_to_dbm_dcs;
      ms_pwr_cap->pwr_min   = pcl_to_dbm_dcs[MIN_PCL_DCS1800];
      ms_pwr_cap->gamma_0   = GAMMA_0_DCS1800;
      break;

    case CGRLC_PCS_1900:
      ms_pwr_cap->p_control = pcl_to_dbm_pcs;
      ms_pwr_cap->pwr_min   = pcl_to_dbm_pcs[MIN_PCL_PCS1900];
      ms_pwr_cap->gamma_0   = GAMMA_0_PCS1900;
      break;
  }

} /* tpc_get_ms_pwr_cap() */

/*
+------------------------------------------------------------------------------
| Function    : tpc_get_pcl
+------------------------------------------------------------------------------
| Description : This function is used to convert the nominal output power to 
|               the power control level
|
| Parameters  : p_ctrl - pointer to table of power control levels
|               nop    - nominal output power
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE tpc_get_pcl ( UBYTE const *p_ctrl, SHORT nop )
{ 
  UBYTE pcl = 0; /* power control level */

  TRACE_FUNCTION( "tpc_get_pcl" ); 

  /* get power control level */
  while( nop < (SHORT)p_ctrl[pcl] - 1 OR nop >= (SHORT)p_ctrl[pcl] + 1 ) pcl++;

  return( pcl );

} /* tpc_get_pcl() */

