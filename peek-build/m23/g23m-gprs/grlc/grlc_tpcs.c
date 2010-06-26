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
|  Purpose :  This module implements signal handler functions for service
|             TPC of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_TPCS_C
#define GRLC_TPCS_C
#endif /* #ifndef GRLC_TPCS_C */

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
#include "grlc_tpcg.h"
#include "grlc_tpcs.h"
#include "grlc_meass.h"

/*==== CONST ================================================================*/

/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

LOCAL void tpc_update_pch        ( void );
LOCAL void tpc_reset_fix_pwr_par ( void );

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : sig_pei_config_tpc_set_pwr_par
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_pei_config_tpc_set_pwr_par( UBYTE alpha, UBYTE gamma )
{ 
  TRACE_ISIG( "sig_pei_config_tpc_set_pwr_par" );

  if( alpha NEQ TPC_ALFGAM_NOT_PRESENT AND gamma NEQ TPC_ALFGAM_NOT_PRESENT )
  {
    UBYTE i; /* used for counting */

    grlc_data->tpc.v_fix_pwr_par = TRUE;

    /* process parameter alpha */
    grlc_data->tpc.fix_pwr_par.alpha = alpha;

    /* process parameter gamma */
    for( i = 0; i < MAC_MAX_TIMESLOTS; i++ )
    {
      grlc_data->tpc.fix_pwr_par.gamma_ch[i] = gamma;
    }
  }
  else
  {
    tpc_reset_fix_pwr_par( );
  }
} /* sig_pei_config_tpc_set_pwr_par() */

/*
+------------------------------------------------------------------------------
| Function    : sig_pei_config_tpc_fix_pcl
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_pei_config_tpc_fix_pcl ( UBYTE pcl )
{ 
  TRACE_ISIG( "sig_pei_config_tpc_fix_pcl" );

  grlc_data->tpc.fix_pcl = pcl;

} /* sig_pei_config_tpc_fix_pcl() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gff_tpc_pwr_ctrl_ind
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_GFF_TPC_PWR_CTRL_IND
|
| Parameters  : *mac_pwr_ctrl_ind - Ptr to primitive MAC_PWR_CTRL_IND
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gff_tpc_pwr_ctrl_ind ( T_MAC_PWR_CTRL_IND * mac_pwr_ctrl_ind )
{ 
  TRACE_ISIG( "sig_gff_tpc_pwr_ctrl_ind" );
  
  meas_c_val_update_ptm(  mac_pwr_ctrl_ind,
                         &grlc_data->tpc.glbl_pwr_par,
                         &grlc_data->tm.freq_param );
     
  meas_sv_update(  mac_pwr_ctrl_ind,  
                  &grlc_data->tpc.glbl_pwr_par,
                  &grlc_data->tm.freq_param );

  tpc_update_pch( );

} /* sig_gff_tpc_pwr_ctrl_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tm_tpc_update_pch
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_TM_TPC_UPDATE_PCH
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_tpc_update_pch ( void  )
{ 
  TRACE_ISIG( "sig_tm_tpc_update_pch" );
  
  tpc_update_pch( );

} /* sig_tm_tpc_update_pch() */

/*
+------------------------------------------------------------------------------
| Function    : tpc_grlc_init
+------------------------------------------------------------------------------
| Description : The function tpc_grlc_init() initialize the services TPC
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tpc_grlc_init ( void )
{ 
  TRACE_FUNCTION( "tpc_grlc_init" ); 

  /* init power control parameter */
  tpc_reset_fix_pwr_par( );

  grlc_data->tpc.fix_pcl     = TPC_SWITCHED_ON;

#if !defined (NTRACE)

  grlc_data->tpc.n_tpc_trace = 0;

#endif /* #if !defined (NTRACE) */

} /* tpc_grlc_init() */

/*
+------------------------------------------------------------------------------
| Function    : tpc_update_pch
+------------------------------------------------------------------------------
| Description : This function calculates the transmit output power
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void tpc_update_pch ( void )
{ 
  UBYTE *pch;
  SHORT  nop;        /* nominal output power applied by the MS */
  SHORT  pms_max;    /* maximum output power applied by the MS */

  LONG   summand;
  LONG   gamma_ch;
  LONG   c_value;
  
  T_MS_PWR_CAP mspc; /* MS power capabilities                  */

  T_C_FILTER             *c_data       = &grlc_data->meas.c_filter;
  T_CGRLC_pwr_ctrl_param *pwr_par      = &grlc_data->tpc.pwr_par;

  UBYTE  pcl;        /* power control level                    */
  UBYTE  i;          /* used for counting                      */

  UBYTE  alpha;

#if !defined (NTRACE)

  BOOL   tracing = FALSE;

#endif /* #if !defined (NTRACE) */

  TRACE_FUNCTION( "tpc_update_pch" );

  if( grlc_data->tpc.pch_buffer.pch_idx EQ PCH_IDX_A )
  {
    pch = &grlc_data->tpc.pch_buffer.pch_array[PCH_IDX_B].pch[0];
  }
  else
  {
    pch = &grlc_data->tpc.pch_buffer.pch_array[PCH_IDX_A].pch[0];
  }

  tpc_get_ms_pwr_cap( grlc_data->tm.freq_param.pdch_band, &mspc );

  pms_max = grlc_data->tpc.glbl_pwr_par.pwr_max;

  if( grlc_data->tpc.pwr_par.alpha NEQ CGRLC_ALPHA_INVALID )
  {
    alpha = grlc_data->tpc.pwr_par.alpha;
  }
  else
  {
    alpha = grlc_data->tpc.glbl_pwr_par.alpha;
  }

  if( grlc_data->tpc.fix_pcl NEQ TPC_SWITCHED_ON )
  {

#if !defined (NTRACE)

    for( i = 0; i < MAC_MAX_TIMESLOTS; i++ )
    {
      if( pch[i] NEQ grlc_data->tpc.fix_pcl )
      {
        tracing = TRUE;
      }
    }
    
#endif /* #if !defined (NTRACE) */

    memset( pch, grlc_data->tpc.fix_pcl, MAC_MAX_TIMESLOTS );
  }
  else
  {
    /*
     * select the used power parameter, either the data received
     * from BSS or the data received by dynamic configuration
     */
    if( grlc_data->tpc.v_fix_pwr_par EQ TRUE )
    {
      pwr_par = &grlc_data->tpc.fix_pwr_par;
      alpha   =  grlc_data->tpc.fix_pwr_par.alpha;
    }

    if( c_data->index EQ 0 AND alpha NEQ 0 )
    {
      pcl = tpc_get_pcl( mspc.p_control, pms_max );
    
#if !defined (NTRACE)

      for( i = 0; i < MAC_MAX_TIMESLOTS; i++ )
      {
        if( pch[i] NEQ pcl )
        {
          tracing = TRUE;
        }
      }

#endif /* #if !defined (NTRACE) */

      memset( pch, pcl, MAC_MAX_TIMESLOTS );
    }
    else
    {
      /* process parameter summand */
      c_value  = ( 63 * MEAS_ACRCY ) - c_data->value;
      summand  = ( MEAS_ACRCY * mspc.gamma_0 ) + ( ( alpha * c_value ) / 10 );

      /* set the power control level for each timeslot */
      for( i = 0; i < MAC_MAX_TIMESLOTS; i++ )
      {
        if( pwr_par->gamma_ch[i] NEQ CGRLC_GAMMA_INVALID )
        {
          gamma_ch = 2 * MEAS_ACRCY * pwr_par->gamma_ch[i];

          nop = MINIMUM( summand - gamma_ch, pms_max * MEAS_ACRCY ) / 
                MEAS_ACRCY;

          if( nop < mspc.pwr_min ) 
          {
            nop = mspc.pwr_min;
          }
        }
        else
        {
          nop = pms_max;
        }

        pcl = tpc_get_pcl( mspc.p_control, nop );
      
#if !defined (NTRACE)

        if( pch[i] NEQ pcl )
        {
          tracing = TRUE;
        }

#endif /* #if !defined (NTRACE) */

        pch[i] = pcl;
      }
    }
  }

#if !defined (NTRACE)

  if( ( grlc_data->tpc.n_tpc_trace &  M_TPC_TRACE_STATIC     ) OR 
      ( grlc_data->tpc.n_tpc_trace &  M_TPC_TRACE_CHANGE AND 
        tracing                    EQ TRUE                   )    )
  {
    ULONG trace[8];

    trace[0]  = ( grlc_data->tpc.glbl_pwr_par.t_avg_t      << 24 );
    trace[0] |= ( grlc_data->tpc.glbl_pwr_par.pb           << 16 );
    trace[0] |= ( grlc_data->tpc.glbl_pwr_par.pc_meas_chan <<  8 );
    trace[0] |= ( grlc_data->tpc.glbl_pwr_par.pwr_max      <<  0 );

    trace[1]  = ( grlc_data->tm.freq_param.pdch_hopping    << 24 );
    trace[1] |= ( grlc_data->tm.freq_param.pdch_band       << 16 );
    trace[1] |= ( grlc_data->tpc.glbl_pwr_par.alpha        <<  8 );
    trace[1] |= ( grlc_data->tpc.pwr_par.alpha             <<  0 );

    trace[2]  = ( grlc_data->tpc.fix_pwr_par.alpha         << 24 );
    trace[2] |= ( grlc_data->tpc.fix_pcl                   << 16 );
    trace[2] |= ( grlc_data->tpc.pch_buffer.pch_idx        <<  8 );

    trace[3]  = 0x00000000;
    trace[4]  = 0x00000000;

    for( i = 0; i < MAC_MAX_TIMESLOTS; i++ )
    {
      trace[( i / 4 ) + 3] |= ( pwr_par->gamma_ch[i] << ( 24 - ( 8 * ( i % 4 ) ) ) );
    }

    trace[5]  = ( c_data->index << 16 );
    trace[5] |= ( c_data->value <<  0 );

    trace[6]  = ( pch[0] << 24 );
    trace[6] |= ( pch[1] << 16 );
    trace[6] |= ( pch[2] <<  8 );
    trace[6] |= ( pch[3] <<  0 );

    trace[7]  = ( pch[4] << 24 );
    trace[7] |= ( pch[5] << 16 );
    trace[7] |= ( pch[6] <<  8 );
    trace[7] |= ( pch[7] <<  0 );

    TRACE_EVENT_P8( "TPC: %08X %08X %08X %08X %08X %08X %08X %08X",
                    trace[0], trace[1], trace[2], trace[3], 
                    trace[4], trace[5], trace[6], trace[7] );
  }

#endif /* #if !defined (NTRACE) */

  if( grlc_data->tpc.pch_buffer.pch_idx EQ PCH_IDX_A )
  {
    grlc_data->tpc.pch_buffer.pch_idx = PCH_IDX_B;
  }
  else
  {
    grlc_data->tpc.pch_buffer.pch_idx = PCH_IDX_A;
  }
} /* tpc_update_pch() */

/*
+------------------------------------------------------------------------------
| Function    : tpc_get_pch
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tpc_get_pch ( UBYTE *pch_dest )
{ 
  UBYTE *pch_src;

  TRACE_FUNCTION( "tpc_get_pch" ); 

  if( grlc_data->tpc.pch_buffer.pch_idx EQ PCH_IDX_A )
  {
    pch_src = &grlc_data->tpc.pch_buffer.pch_array[PCH_IDX_A].pch[0];
  }
  else
  {
    pch_src = &grlc_data->tpc.pch_buffer.pch_array[PCH_IDX_B].pch[0];
  }

  memcpy( pch_dest, pch_src, MAC_MAX_TIMESLOTS );

} /* tpc_get_pch() */

/*
+------------------------------------------------------------------------------
| Function    : tpc_set_pwr_ctrl_param
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tpc_set_pwr_ctrl_param ( T_CGRLC_pwr_ctrl_param *pwr_ctrl_param )
{ 
  TRACE_FUNCTION( "tpc_set_pwr_ctrl_param" ); 

  grlc_data->tpc.pwr_par = *pwr_ctrl_param;

} /* tpc_set_pwr_ctrl_param() */

/*
+------------------------------------------------------------------------------
| Function    : tpc_set_glbl_pwr_ctrl_param
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void tpc_set_glbl_pwr_ctrl_param
                           ( T_CGRLC_glbl_pwr_ctrl_param *glbl_pwr_ctrl_param )
{ 
  TRACE_FUNCTION( "tpc_set_glbl_pwr_ctrl_param" );

  grlc_data->tpc.glbl_pwr_par = *glbl_pwr_ctrl_param;

} /* tpc_set_glbl_pwr_ctrl_param() */

/*
+------------------------------------------------------------------------------
| Function    : tpc_reset_fix_pwr_par
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
LOCAL void tpc_reset_fix_pwr_par ( void )
{ 
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "tpc_reset_fix_pwr_par" );

  grlc_data->tpc.v_fix_pwr_par = FALSE;

  /* process parameter alpha */
  grlc_data->tpc.fix_pwr_par.alpha = TPC_ALFGAM_NOT_PRESENT;

  /* process parameter gamma */
  for( i = 0; i < MAC_MAX_TIMESLOTS; i++ )
  {
    grlc_data->tpc.fix_pwr_par.gamma_ch[i] = TPC_ALFGAM_NOT_PRESENT;
  }
} /* tpc_reset_fix_pwr_par() */
