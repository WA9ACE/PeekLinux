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
|             MEAS of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_MEASF_C
#define GRLC_MEASF_C
#endif /* #ifndef GRLC_MEASF_C */

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"
#include "ccdapi.h"
#include "prim.h"
#include "message.h"
#include "grlc.h"
#include "grlc_measf.h"

/*==== CONST ================================================================*/

#define I_LEVEL_MIN             I_LEVEL_0
#define I_LEVEL_0               0    /* i.lev. is greater than C                  */
#define I_LEVEL_15              15   /* i.lev. is less than or equal to C - 28 dB */    
#define I_LEVEL_MAX             I_LEVEL_15
#define I_LEVEL_GAMMA_0_SKIPPED 0xF0 /* used for tracing                          */
#define I_LEVEL_GAMMA_1_SKIPPED 0xF1 /* used for tracing                          */
#define I_LEVEL_GAMMA_2_SKIPPED 0xF2 /* used for tracing                          */
#define I_LEVEL_GAMMA_3_SKIPPED 0xF3 /* used for tracing                          */
#define I_LEVEL_STATE_MISMATCH  0xFD /* used for tracing                          */
#define I_LEVEL_IDX_TO_SMALL    0xFE /* used for tracing                          */
#define I_LEVEL_NOT_AVAIL       0xFF /* used for tracing                          */

/*==== LOCAL VARS ===========================================================*/

/*==== LOCAL TYPES===========================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : meas_c_calc_mean 
+------------------------------------------------------------------------------
| Description : This function calculates the mean of the received signal level
|               of the four normal bursts that compose a block. It returns
|               the amount of bursts with valid receibed signal values.
|
| Parameters  : burst_level - receive signal level of the first valid downlink
|                             PDCH radio block
|               radio_freq  - radio frequency of TDMA frame within a
|                             radio block
|               ss_block    - mean of the received signal level
|               pb_rdc      - Pb reduction
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE meas_c_calc_mean ( UBYTE              *burst_level,
                                USHORT             *radio_freq,
                                ULONG              *ss_block,
                                UBYTE              *pb_rdc,
                                T_CGRLC_freq_param *freq_param )
{ 
  UBYTE i;            /* used for counting         */
  UBYTE vld_smpl = 0; /* valid measurement samples */

  TRACE_FUNCTION( "meas_c_calc_mean" );

  *pb_rdc   = 0;
  *ss_block = 0;

  /* add up all measurement data */
  for( i = 0; i < MAC_BURST_PER_BLOCK; i++ )
  {
    if( burst_level[i] NEQ MAC_RXLEV_NONE ) 
    {
      vld_smpl++;      

      *ss_block += burst_level[i];

      if( !( grlc_data->tm.freq_param.pdch_hopping EQ FALSE           AND 
             grlc_data->tm.freq_param.bcch_arfcn   EQ radio_freq[i] )     )
      {
        *pb_rdc += 1;
      }
    }
  }

  /* calculate the mean */
  if( vld_smpl )
  {
    *ss_block = ( *ss_block * MEAS_ACRCY ) / vld_smpl;
  }

  return( vld_smpl );
} /* meas_c_calc_mean() */

/*
+------------------------------------------------------------------------------
| Function    : meas_int_fill_rel_iLevel
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_int_fill_rel_iLevel ( UBYTE  *v_ilev,
                                       UBYTE  *ilev,
                                       UBYTE   idx,
                                       USHORT  c_raw_data_lev )
{
  UBYTE val; /* holds interference level data */

  if( grlc_data->meas.ilev.ilev[idx] NEQ CGRLC_ILEV_NONE )
  {
    if( c_raw_data_lev < grlc_data->meas.ilev.ilev[idx] * MEAS_ACRCY )
    {
      *ilev = I_LEVEL_MIN;
    }
    else
    {
      val = ( c_raw_data_lev - grlc_data->meas.ilev.ilev[idx] * MEAS_ACRCY ) / 
            ( 2 * MEAS_ACRCY ) + 1;
    
      if( val > I_LEVEL_MAX ) *ilev = I_LEVEL_MAX;
      else                    *ilev = val;
    }

    *v_ilev = TRUE;
  }
  else
  {
    *v_ilev = FALSE;
  }
} /* meas_int_fill_rel_iLevel() */
