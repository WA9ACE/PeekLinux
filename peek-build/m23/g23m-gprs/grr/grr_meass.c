/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRR
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
|             MEAS of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_MEASS_C
#define GRR_MEASS_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include <string.h>
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grr.h"    /* to get cnf-definitions */
#include "mon_grr.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grr.h"        /* to get the global entity definitions */
#include "grr_f.h"
#include "grr_measf.h"  /* to get the definitions for interference measurements */
#include "grr_meass.h"  /* to get the own definitions */
#include "grr_measp.h"  /* to get the own definitions */
#include "cl_rlcmac.h"
#include "grr_tcs.h"

/*==== CONST ================================================================*/

#define BS_PA_MFRMS_OFFSET       2 /* BS_PA_MRMS is coded within 3 bits and  */
                                   /* ranges from 2 to 9                     */
#define DRX_DENOMINATOR_FOR_CCCH 4

/*==== LOCAL VARS ===========================================================*/

/*==== LOCAL TYPES===========================================================*/

#if !defined (NTRACE)

LOCAL void meas_im_trace_i_level( T_MEAS_IM_FILTER *filter, 
                                  T_ilev           *i_level );
  
#endif /* #if !defined (NTRACE) */

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_permit
+------------------------------------------------------------------------------
| Description : This function returns whether interference measurements in
|               packet transfer mode shall be performed by Layer 1 
|               autonomously.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE meas_im_get_permit ( void )
{ 
  UBYTE permit = MPHP_IM_ENABLED;

  TRACE_FUNCTION( "meas_im_get_permit" );
  
  if( grr_data->meas_im.mode NEQ IM_MODE_IDLE_TRANSFER AND
      grr_data->meas_im.mode NEQ IM_MODE_TRANSFER          )
  {
    permit = MPHP_IM_DISABLED;
  }

  return( permit );
} /* meas_im_get_permit() */

/*
+------------------------------------------------------------------------------
| Function    : meas_init
+------------------------------------------------------------------------------
| Description : This function initializes the measurement module.
|
| Parameters  : im_mode - operation mode for interference measurements
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_init ( T_MEAS_IM_MODE im_mode )
{ 
  TRACE_FUNCTION( "meas_init" );
  
  /* general initialization */
  INIT_STATE( MEAS, MEAS_NULL );

  /* initialization of interference measurements */
  meas_im_init( im_mode );

  /* initialization of extended measurements */
  meas_em_init( );

  /* initialization of C value derivation */
  meas_c_init( );

} /* meas_init() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_rel_i_level
+------------------------------------------------------------------------------
| Description : This function returns the interference level values mapped to
|               values relative to the C value.
|
| Parameters  : *ma      - Ptr to frequency information in MA format
|               *i_level - Ptr to relative interference level values
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_get_rel_i_level ( T_MEAS_IM_CARRIER *ma,
                                      T_ilev            *i_level )
{
  T_MEAS_IM_FILTER *filter; /* filter */

  TRACE_FUNCTION( "meas_im_get_rel_i_level" );

  filter = meas_im_get_filter( ma );

  meas_im_fill_rel_iLevel( &i_level->v_ilev0, &i_level->ilev0, filter, 0 );
  meas_im_fill_rel_iLevel( &i_level->v_ilev1, &i_level->ilev1, filter, 1 );
  meas_im_fill_rel_iLevel( &i_level->v_ilev2, &i_level->ilev2, filter, 2 );
  meas_im_fill_rel_iLevel( &i_level->v_ilev3, &i_level->ilev3, filter, 3 );
  meas_im_fill_rel_iLevel( &i_level->v_ilev4, &i_level->ilev4, filter, 4 );
  meas_im_fill_rel_iLevel( &i_level->v_ilev5, &i_level->ilev5, filter, 5 );
  meas_im_fill_rel_iLevel( &i_level->v_ilev6, &i_level->ilev6, filter, 6 );
  meas_im_fill_rel_iLevel( &i_level->v_ilev7, &i_level->ilev7, filter, 7 );

#if !defined (NTRACE)

  meas_im_trace_i_level( filter, i_level );
  
#endif /* #if !defined (NTRACE) */

} /* meas_im_get_rel_i_level() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_drx_period_seconds 
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG meas_im_get_drx_period_seconds ( void )
{
  ULONG t_drx; /* DRX period for the MS */

  TRACE_FUNCTION( "meas_im_get_drx_period_seconds" );

  if( grr_is_pbcch_present( ) EQ TRUE )
  {
    if( grr_is_non_drx_mode( ) )
    {
      t_drx = DRX_NORM_FACTOR / BLOCK_PER_MF;
    }
    else
    {
      t_drx = ( DRX_NORM_FACTOR * PAGING_CYCLE ) / grr_data->ms.split_pg_cycle;
    }
  
    t_drx *= ( USEC_PER_MF / MEAS_ACRCY );
  }
  else
  {
    if( meas_is_spgc_ccch_supported( ) )
    {
      if( grr_is_non_drx_mode( ) )
      {
        t_drx = DRX_NORM_FACTOR / DRX_DENOMINATOR_FOR_CCCH;
      }
      else
      {
        t_drx = ( DRX_NORM_FACTOR * PAGING_CYCLE ) / grr_data->ms.split_pg_cycle;
      }
    }
    else
    {
      t_drx = DRX_NORM_FACTOR *
                         ( psc_db->net_ctrl.bs_pa_mfrms + BS_PA_MFRMS_OFFSET );
    }

    t_drx *= ( USEC_PER_CS_MF / MEAS_ACRCY );
  }

  return( t_drx );
} /* meas_im_get_drx_period_seconds() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_drx_period_frames
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL USHORT meas_im_get_drx_period_frames ( void )
{
  ULONG frames;

  TRACE_FUNCTION( "meas_im_get_drx_period_frames" );

  frames = meas_im_get_drx_period_seconds( ) * N_MSEC_PER_FRAME;
  frames = M_ROUND_UP( frames, Z_MSEC_PER_FRAME * DRX_NORM_FACTOR ); 
  
  return( ( USHORT )frames );
} /* meas_im_get_drx_period_frames() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_abs_i_level
+------------------------------------------------------------------------------
| Description : This function returns the interference level values mapped to
|               values as defined for RXLEV. It returns the number of bits 
|               occupied within the measurement report.
|
| Parameters  : *ma      - Ptr to frequency information in MA format
|               *i_level - Ptr to absolute interference level values
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE meas_im_get_abs_i_level ( T_MEAS_IM_CARRIER *ma,
                                       T_ilev_abs        *i_level )
{
  UBYTE used_bits;

  T_MEAS_IM_FILTER *filter; /* filter */

  TRACE_FUNCTION( "meas_im_get_abs_i_level" );

  filter = meas_im_get_filter(  ma );

  used_bits  = 
      meas_im_fill_abs_iLevel( &i_level->v_ilevabs0, &i_level->ilevabs0, filter, 0 );
  used_bits += 
      meas_im_fill_abs_iLevel( &i_level->v_ilevabs1, &i_level->ilevabs1, filter, 1 );
  used_bits +=
      meas_im_fill_abs_iLevel( &i_level->v_ilevabs2, &i_level->ilevabs2, filter, 2 );
  used_bits +=
      meas_im_fill_abs_iLevel( &i_level->v_ilevabs3, &i_level->ilevabs3, filter, 3 );
  used_bits +=
      meas_im_fill_abs_iLevel( &i_level->v_ilevabs4, &i_level->ilevabs4, filter, 4 );
  used_bits +=
      meas_im_fill_abs_iLevel( &i_level->v_ilevabs5, &i_level->ilevabs5, filter, 5 );
  used_bits +=
      meas_im_fill_abs_iLevel( &i_level->v_ilevabs6, &i_level->ilevabs6, filter, 6 );
  used_bits +=
      meas_im_fill_abs_iLevel( &i_level->v_ilevabs7, &i_level->ilevabs7, filter, 7 );

#if !defined (NTRACE)

  meas_im_trace_i_level( filter, ( T_ilev* )i_level );
  
#endif /* #if !defined (NTRACE) */

  return( used_bits );
} /* meas_im_get_abs_i_level() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_set_carrier
+------------------------------------------------------------------------------
| Description : 
|
|            Frequency Band    config.       ETSI GSM spec.  coding used in L1
|            GSM 900           STD_900       1-124           1-124
|            E-GSM             STD_EGSM      1-124,          1-124,
|                                            975-1023, 0     125-174
|            PCS 1900          STD_1900      512-810         512-810
|            DCS 1800          STD_1800      512-885         512-885
|            GSM 900/DCS 1800  STD_DUAL      1-124,          1-124,
|                                            512-885         125-498
|            E-GSM/DCS 1800    STD_DUAL_EGSM 1-124,          1-124,
|                                            975-1023,0      125-174,
|                                            512-885         175-548
|            GSM 850           STD_850       128-251         128-251
|            GSM 850/PCS 1900  STD_DUAL_US   128-251,        1-124,
|                                            512-810         125-424
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_set_carrier ( T_MEAS_IM_CARRIER *carrier,
                                  T_p_frequency_par *freq_par )
{
  TRACE_FUNCTION( "meas_im_set_carrier" );

  if( freq_par->p_chan_sel.hopping )
  {
    UBYTE  i;
    UBYTE  bit_shift;
    USHORT array_idx;
    USHORT arfcn;
  
    carrier->hopping              = TRUE;
    carrier->alloc.hop_param.hsn  = ( freq_par->p_chan_sel.p_rf_ch.arfcn >> 8 );
    carrier->alloc.hop_param.maio = ( freq_par->p_chan_sel.p_rf_ch.arfcn & 0x00FF );

    memset( carrier->alloc.hop_param.arfcn_bit_field,
            0, MAX_IM_HOP_ARFCN_BIT_FIELD_SIZE );

    for( i = 0; i < freq_par->p_freq_list.p_rf_chan_cnt; i++ )
    {
      arfcn = freq_par->p_freq_list.p_rf_chan_no.p_radio_freq[i];
      
      switch( std )
      {
        case STD_1900: arfcn -= LOW_CHANNEL_1900; break;
        case STD_1800: arfcn -= LOW_CHANNEL_1800; break;
        default      :                            break;
      }

      array_idx =            arfcn / 8;
      bit_shift = ( UBYTE )( arfcn % 8 );

      if( array_idx < MAX_IM_HOP_ARFCN_BIT_FIELD_SIZE )
      {
        carrier->alloc.hop_param.arfcn_bit_field[array_idx] |= 
                                                         ( 0x01 << bit_shift );
      }
      else
      {
        TRACE_ERROR( "meas_im_set_carrier: array_idx >= MAX_IM_HOP_ARFCN_BIT_FIELD_SIZE" );
      }
    }
  }
  else
  {
    carrier->hopping     = FALSE;
    carrier->alloc.arfcn = freq_par->p_chan_sel.p_rf_ch.arfcn;
  }

#if !defined (NTRACE)
  
  if( grr_data->meas_im.n_im_trace & M_IM_TRACE_FREQUENCY )
  {
    if( carrier->hopping )
    {
      ULONG arfcn[4];
      ULONG freq[4];

      arfcn[0]  = ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[0]  <<  0;
      arfcn[0] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[1]  <<  8;
      arfcn[0] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[2]  << 16;
      arfcn[0] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[3]  << 24;
      arfcn[1]  = ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[4]  <<  0;
      arfcn[1] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[5]  <<  8;
      arfcn[1] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[6]  << 16;
      arfcn[1] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[7]  << 24;
      arfcn[2]  = ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[8]  <<  0;
      arfcn[2] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[9]  <<  8;
      arfcn[2] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[10] << 16;
      arfcn[2] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[11] << 24;
      arfcn[3]  = ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[12] <<  0;
      arfcn[3] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[13] <<  8;
      arfcn[3] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[14] << 16;
      arfcn[3] |= ( ULONG )carrier->alloc.hop_param.arfcn_bit_field[15] << 24;
    
      freq[0]   = ( ULONG )freq_par->p_freq_list.p_rf_chan_no.p_radio_freq[0] << 16;
      freq[0]  |= ( ULONG )freq_par->p_freq_list.p_rf_chan_no.p_radio_freq[1] <<  0;
      freq[1]   = ( ULONG )freq_par->p_freq_list.p_rf_chan_no.p_radio_freq[2] << 16;
      freq[1]  |= ( ULONG )freq_par->p_freq_list.p_rf_chan_no.p_radio_freq[3] <<  0;
      freq[2]   = ( ULONG )freq_par->p_freq_list.p_rf_chan_no.p_radio_freq[4] << 16;
      freq[2]  |= ( ULONG )freq_par->p_freq_list.p_rf_chan_no.p_radio_freq[5] <<  0;
      freq[3]   = ( ULONG )freq_par->p_freq_list.p_rf_chan_no.p_radio_freq[6] << 16;
      freq[3]  |= ( ULONG )freq_par->p_freq_list.p_rf_chan_no.p_radio_freq[7] <<  0;


      TRACE_EVENT_P9( "meas_im_get_trns_freq: %08X%08X%08X%08X Num: %d Freq: %08X %08X %08X %08X",
                      arfcn[3], arfcn[2], arfcn[1], arfcn[0],
                      freq_par->p_freq_list.p_rf_chan_cnt,
                      freq[0],  freq[1],  freq[2],  freq[3] );
    }
    else
    {
      TRACE_EVENT_P1( "meas_im_get_trns_freq: %d", carrier->alloc.arfcn );
    }
  }

#endif /* #if !defined (NTRACE) */

} /* meas_im_set_carrier() */

/*
+------------------------------------------------------------------------------
| Function    : meas_prepare_and_start_idle
+------------------------------------------------------------------------------
| Description : ...
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_prepare_and_start_idle ( void ) 
{ 
  TRACE_FUNCTION( "meas_prepare_and_start_idle" );

  meas_im_cgrlc_int_level_req( TRUE );

  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
      if ( grr_data->meas_im.mode EQ IM_MODE_IDLE_TRANSFER OR
           grr_data->meas_im.mode EQ IM_MODE_IDLE             )
      {
        meas_im_prepare_idle( );

        switch( GET_STATE( MEAS_IM ) )
        {
          case MEAS_IM_NULL:
            {
              if( meas_im_start_idle( ) )
              {
                SET_STATE( MEAS_IM, MEAS_IM_START );
              }
            }
            break;

           default:
            /* do nothing */
            break;
        }
      }
      break;

    default:
      /* do nothing */
      break;
  }
} /* meas_prepare_and_start_idle() */

/*
+------------------------------------------------------------------------------
| Function    : meas_prepare_and_stop_idle
+------------------------------------------------------------------------------
| Description : ...
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_prepare_and_stop_idle ( void ) 
{ 
  TRACE_FUNCTION( "meas_prepare_and_stop_idle" );

  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
      {
        meas_im_prepare_idle( );
      }
      break;

    default:
      /* do nothing */
      break;
  }
} /* meas_prepare_and_stop_idle() */

/*==== SIGNAL FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_start
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_START
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_start ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_meas_start" );
  
  /* process the extended measurement service */
  if( psc_db->is_ext_psi5_valid EQ TRUE )
  {
    sig_ctrl_meas_ext_valid_psi5( );
  }

  /* process the interference measurement service */
  sig_ctrl_meas_int_list_valid( );

} /* sig_ctrl_meas_start() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_stop
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_STOP
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_stop ( void ) 
{ 
  UBYTE state = GET_STATE( MEAS );

  TRACE_ISIG( "sig_ctrl_meas_stop" );
  
  /* process the interference measurement service */
  switch( state )
  {
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      {
        meas_im_stop_trns( );
        SET_STATE( MEAS_IM, MEAS_IM_NULL );
      }
      break;

    case MEAS_IDLE:
      {
        switch( GET_STATE( MEAS_IM ) )
        {
          case MEAS_IM_START:
            {
              meas_im_stop_idle( );

              SET_STATE( MEAS_IM, MEAS_IM_STOP_REQ ); 
            }
            break;

          case MEAS_IM_NULL:
            SET_STATE( MEAS_IM, MEAS_IM_NULL );
            break;

          default:
            TRACE_ERROR( "SIG_CTRL_MEAS_STOP unexpected INT MEAS 1" );
            break;
        }
      }
      break;

    default:
      /* do nothing */;
      break;
  }

  /* process the extended measurement service */
  switch( state )
  {
    case MEAS_NULL:
    case MEAS_IDLE:
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      switch( GET_STATE( MEAS_EM ) )
      {
        case MEAS_EM_REP_REQ:
          meas_em_stop_req( );
          break;

        case MEAS_EM_PMR_SENDING:
        case MEAS_EM_PENDING:
          SET_STATE( MEAS_EM, MEAS_EM_NULL );
          break;

        default:
          TRACE_ERROR( "SIG_CTRL_MEAS_STOP unexpected EXT MEAS 1" );
          break;
      }
      
      if( grr_t_status( T3178) NEQ 0 )
      {
         vsi_t_stop( GRR_handle, T3178);
         TRACE_EVENT( "Timer T3178 stopped" );
      }
      break;

    default:
     TRACE_ERROR( "SIG_CTRL_MEAS_STOP unexpected EXT MEAS 2" );
      break;
  }

  /* process the signal quality service */
  if( state EQ MEAS_TRANSFER )
  {
    RX_SetRxQual( RX_QUAL_UNAVAILABLE );
  }  

  SET_STATE( MEAS, MEAS_NULL );

} /* sig_ctrl_meas_stop() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_suspend
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_SUSPEND
|               This signal suspends the measurement service
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_suspend ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_meas_suspend" );
  
  /* process the interference measurement service */
  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      {
        switch( GET_STATE( MEAS_IM ) )
        {
          case MEAS_IM_START:
            {
              meas_im_stop_idle( );

              grr_data->meas_im.trig_signal = IM_TRIG_SIGNAL_PAUSE;

              SET_STATE( MEAS_IM, MEAS_IM_STOP_REQ ); 
            }
            break;

          default:
            /* do nothing */
            break;
        }
      }
      break;

    default:
      /* do nothing */
      break;
  }

  /* process the extended measurement service */
  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      SET_STATE( MEAS_EM, MEAS_EM_SUSPEND );
    
      meas_em_process_t3178( );
      break;

    default:
      /* do nothing */
      break;
  }
} /* sig_ctrl_meas_suspend() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_resume
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_RESUME
|               This signal resumes the measurement service
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_resume ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_meas_resume" );
  
  /* process the interference measurement service */
  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      if ( grr_data->meas_im.mode EQ IM_MODE_IDLE_TRANSFER OR
           grr_data->meas_im.mode EQ IM_MODE_IDLE             )
      {
        if( meas_im_start_idle( ) )
        {
          SET_STATE( MEAS_IM, MEAS_IM_START );
        }
        else
        {
          SET_STATE( MEAS_IM, MEAS_IM_NULL );
        }
        break;
      }
      break;

    default:
      /* do nothing */
      break;
  }

  /* process the extended measurement service */
  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      SET_STATE( MEAS_EM, MEAS_EM_NULL );
    
      meas_em_process_t3178( );
      break;

    default:
      /* do nothing */
      break;
  }

} /* sig_ctrl_meas_resume() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_pim
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_PIM
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_pim ( void ) 
{ 
  UBYTE state = GET_STATE( MEAS );

  TRACE_ISIG( "sig_ctrl_meas_pim" );
  
  /* process the interference measurement service */
  switch( state )
  {
    case MEAS_TRANSFER:
      meas_im_state_changed( MEAS_IDLE );

      /*lint -fallthrough*/

    case MEAS_ACCESS:
      meas_im_stop_trns( );
      SET_STATE( MEAS_IM, MEAS_IM_NULL );
      if ( grr_data->meas_im.mode EQ IM_MODE_IDLE_TRANSFER OR
           grr_data->meas_im.mode EQ IM_MODE_IDLE             )
      {
        meas_im_prepare_idle( );
        
        if( meas_im_start_idle( ) )
        {
          SET_STATE( MEAS_IM, MEAS_IM_START );
        }
      }
      break;

    default:
      /* do nothing */
      break;
  }

  /* process the extended measurement service */
  switch( state )
  {
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      {
        if( GET_STATE( MEAS_EM ) EQ MEAS_EM_PENDING )
        {
          meas_em_start( );
          SET_STATE( MEAS_EM, MEAS_EM_REP_REQ );
        }
      }
      break;

    default:
      /* do nothing */
      break;
  }

  /* process the signal quality service */
  if( state EQ MEAS_TRANSFER )
  {
    RX_SetRxQual( RX_QUAL_UNAVAILABLE );
  }  

  SET_STATE( MEAS, MEAS_IDLE );

} /* sig_ctrl_meas_pim() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_pam
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_PAM
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_pam ( void ) 
{ 
  UBYTE state = GET_STATE( MEAS );

  TRACE_ISIG( "sig_ctrl_meas_pam" );
  
  /* process the interference measurement service */
  switch( state )
  {
    case MEAS_IDLE:
      {
        switch( GET_STATE( MEAS_IM ) )
        {
          case MEAS_IM_START:
            {
              meas_im_stop_idle( );

              grr_data->meas_im.trig_signal = IM_TRIG_SIGNAL_PAM;

              SET_STATE( MEAS_IM, MEAS_IM_STOP_REQ ); 
            }
            break;

          case MEAS_IM_NULL:
            {
              if ( grr_data->meas_im.mode EQ IM_MODE_IDLE_TRANSFER OR
                   grr_data->meas_im.mode EQ IM_MODE_TRANSFER         )
              {
                meas_im_prepare_trns( );
                meas_im_start_trns( );

                SET_STATE( MEAS_IM, MEAS_IM_START );
              }
            }
            break;

          default:
            TRACE_ERROR( "SIG_CTRL_MEAS_PAM unexpected" );
            break;
        }
      }
      break;

    case MEAS_TRANSFER:
      meas_im_state_changed( MEAS_ACCESS );
      break;

    default:
      /* do nothing */
      break;
  }


  /* process the extended measurement service */
  switch( state )
  {
    case MEAS_IDLE:
      {
        switch( GET_STATE( MEAS_EM ) )
        {
          case MEAS_EM_REP_REQ:
            meas_em_stop_req( );
            SET_STATE( MEAS_EM, MEAS_EM_PENDING );
            break;

          default:
            /* do nothing */
            break;
        }
      }
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_MEAS_PAM unexpected, EXT MEAS 2" );
      break;
  }

  /* process the signal quality service */
  if( state EQ MEAS_TRANSFER )
  {
    RX_SetRxQual( RX_QUAL_UNAVAILABLE );
  }  

  SET_STATE( MEAS, MEAS_ACCESS );

} /* sig_ctrl_meas_pam() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_ptm
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_PTM
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_ptm ( void )
{ 
  TRACE_ISIG( "sig_ctrl_meas_ptm" );

  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
    case MEAS_ACCESS:
      meas_im_state_changed( MEAS_TRANSFER );
      break;

    default:
      /* do nothing */
      break;
  }
  
  SET_STATE( MEAS, MEAS_TRANSFER );

} /* sig_ctrl_meas_ptm() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_int_list_invalid
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_INT_LIST_INVALID
|               If this signal is received then the list for interfrence
|               measurements are invalid
|               and interference measurements shall be stopped, if running
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_int_list_invalid ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_meas_int_list_invalid" );
  
  meas_prepare_and_stop_idle( );
} /* sig_ctrl_meas_int_list_invalid() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_int_list_valid
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_INT_LIST_VALID
|               If this signal is received then the list for interfrence
|               measurements are valid
|               and interference measurements could be started, refreshed etc.
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_int_list_valid ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_meas_int_list_valid" );
  
  meas_prepare_and_start_idle( );

} /* sig_ctrl_meas_int_list_valid() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_ext_invalid_psi5
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_EXT_INVALID_PSI5
|               If this signal is received then the list for extended
|               measurements are invalid
|               and interference measurements could be stopped, if already running
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_ext_invalid_psi5 ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_meas_ext_invalid_psi5" );
  
  switch( GET_STATE( MEAS ) )
  {
    case MEAS_NULL:
    case MEAS_IDLE:
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      if( !meas_em_is_valid_pmo_present( ) )
      {
        meas_em_valid_psi5( );

        switch( GET_STATE( MEAS_EM ) )
        {
          case MEAS_EM_REP_REQ:
            meas_em_stop_req( );
            break;

          default:
            SET_STATE( MEAS_EM, MEAS_NULL );
            break;
        }
      }
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_MEAS_EXT_INVALID_PSI5 unexpected" );
      break;
  }
} /* sig_ctrl_meas_ext_invalid_psi5() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_ext_valid_psi5
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_EXT_VALID_PSI5
|               If this signal is received then the list for extended
|               measurements are valid
|               and interference measurements could performed
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_ext_valid_psi5 ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_meas_ext_valid_psi5" );
  

  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
      if( !meas_em_is_valid_pmo_present( ) )
      {
        switch( GET_STATE( MEAS_EM ) )
        {
          case MEAS_EM_REP_REQ:
            meas_em_stop_req( );
            
            /*lint -fallthrough*/
          
          case MEAS_EM_NULL:
            meas_em_valid_psi5( );
            meas_em_process_t3178( );
            break;    
          default:
            TRACE_ERROR( "SIG_CTRL_MEAS_EXT_VALID_PSI5 unexpected 1" );
            break;
        }
      }
      break;

    case MEAS_NULL:
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      if( !meas_em_is_valid_pmo_present( ) )
      {
        /* measurement report parameters of PSI5 shall only be taken into */
        /* account in case there is no individually parameter set send to */
        /* the MS using a Packet Measurement Order message                */
        meas_em_valid_psi5( );
        meas_em_process_t3178( );
      }
      break;
    
    default:
      TRACE_ERROR( "SIG_CTRL_MEAS_EXT_VALID_PSI5 unexpected 2" );
      break;
  }

} /* sig_ctrl_meas_ext_valid_psi5() */

/*
+------------------------------------------------------------------------------
| Function    : sig_cs_meas_start
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CS_MEAS_START
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cs_meas_start ( void ) 
{ 
  TRACE_ISIG( "sig_cs_meas_start" );
  
  grr_data->meas_im.v_cs_meas_active = TRUE;
  
  meas_prepare_and_start_idle( );

} /* sig_cs_meas_start() */

/*
+------------------------------------------------------------------------------
| Function    : sig_cs_meas_stop
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CS_MEAS_STOP
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cs_meas_stop ( void ) 
{ 
  TRACE_ISIG( "sig_cs_meas_stop" );
  
  grr_data->meas_im.v_cs_meas_active = FALSE;

  meas_prepare_and_stop_idle( );

} /* sig_cs_meas_stop() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_pmr_accept
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_PMR_ACCEPT
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_pmr_accept ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_meas_pmr_accept" );

  switch( GET_STATE( MEAS_EM ) )
  {
    case( MEAS_EM_PMR_SENDING ):
      if( meas_em_send_meas_rpt( FALSE ) EQ FALSE )
      {
        SET_STATE( MEAS_EM, MEAS_EM_NULL );
  
        meas_em_process_t3178( );
      }
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_MEAS_PMR_ACCEPT unexpected" );
      break;
  }

} /* sig_ctrl_meas_pmr_accept() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_pmr_reject
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_MEAS_PMR_REJECT
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_pmr_reject ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_meas_pmr_reject" );

  switch( GET_STATE( MEAS_EM ) )
  {
    case( MEAS_EM_PMR_SENDING ):
      SET_STATE( MEAS_EM, MEAS_EM_NULL );

      meas_em_process_t3178( );
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_MEAS_PMR_REJECT unexpected" );
      break;
  }

} /* sig_ctrl_meas_pmr_reject() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_ext_meas_cnf
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_ext_meas_cnf 
                                   ( T_RRGRR_EXT_MEAS_CNF *rrgrr_ext_meas_cnf )
{ 
  TRACE_ISIG( "sig_ctrl_meas_ext_meas_cnf" );

  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      switch( GET_STATE( MEAS_EM ) )
      {
        case( MEAS_EM_REP_REQ ):
          if( meas_em_store_rslt( rrgrr_ext_meas_cnf ) EQ TRUE )
          {
            SET_STATE( MEAS_EM, MEAS_EM_PMR_SENDING );

            if( meas_em_send_meas_rpt( TRUE ) EQ FALSE )
            {
              /*
               * The additional state check is necessary because signals
               * might be send during the processing of the function 
               * meas_em_send_meas_rpt.
               */
              if( GET_STATE( MEAS_EM ) EQ MEAS_EM_PMR_SENDING )
              {
                SET_STATE( MEAS_EM, MEAS_NULL );
  
                meas_em_process_t3178( );
              }
            }
          }
          else
          {
            SET_STATE( MEAS_EM, MEAS_NULL );
  
            meas_em_process_t3178( );
          }
          break;

        default:
          TRACE_EVENT( "SIG_CTRL_MEAS_EXT_MEAS_CNF unexpected 1" );
          break;
      }
      break;

    default:
      TRACE_EVENT( "SIG_CTRL_MEAS_EXT_MEAS_CNF unexpected 2" );
      break;
  }
} /* sig_ctrl_meas_ext_meas_cnf() */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_meas_order
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_GFP_MEAS_ORDER
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_meas_order ( void )
{ 
  MCAST( d_meas_order, D_MEAS_ORDER );
  TRACE_ISIG( "sig_gfp_meas_order" );
  
  if (grr_data->nc2_on)
  {
    meas_p_meas_order_ind(d_meas_order);
  }
  else
  {
    TRACE_EVENT( "PACKET MEASUREMENT ORDER RECEIVED,but NC2 is deactivated" );
  }


} /* sig_gfp_meas_order() */

 

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_meas_int_meas_stop_con
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_GFP_MEAS_INT_MEAS_STOP_CON
|
| Parameters  : *dummy - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_meas_int_meas_stop_con ( T_MPHP_INT_MEAS_STOP_CON * dummy )
{ 
  TRACE_ISIG( "sig_gfp_meas_int_meas_stop_con" );
  
  switch( GET_STATE( MEAS ) )
  {
    case MEAS_NULL:
    case MEAS_IDLE:
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      {
        switch( GET_STATE( MEAS_IM ) )
        {
          case MEAS_IM_STOP_REQ:
            {
              if( grr_data->meas_im.trig_signal EQ IM_TRIG_SIGNAL_PAM )
              {
                meas_im_prepare_trns( );
              }

              SET_STATE( MEAS_IM, MEAS_IM_NULL );

              grr_data->meas_im.trig_signal = IM_TRIG_SIGNAL_NULL;
            }
            break;

          default:
            TRACE_ERROR( "SIG_GFP_MEAS_INT_MEAS_STOP_CON unexpected 1" );
            break;
        }
      }
      break;
      
    default:
      TRACE_ERROR( "SIG_GFP_MEAS_INT_MEAS_STOP_CON unexpected 2" );
      break;
  }

} /* sig_gfp_meas_int_meas_stop_con() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_meas_int_meas_ind
+------------------------------------------------------------------------------
| Description : Handles the signal sig_gfp_meas_int_meas_ind
|
| Parameters  : *mphp_int_meas_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_meas_int_meas_ind ( T_MPHP_INT_MEAS_IND *mphp_int_meas_ind )
{ 
  UBYTE state = GET_STATE( MEAS );

  TRACE_ISIG( "sig_gfp_meas_int_meas_ind" );

  switch( state )
  {
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      meas_im_new_value( mphp_int_meas_ind, state );
      break;

    case MEAS_IDLE:
      {
        switch( GET_STATE( MEAS_IM ) )
        {
          case MEAS_IM_START:
            meas_im_new_value( mphp_int_meas_ind, state );
            break;

          case MEAS_IM_STOP_REQ:
            /* do nothing */
            break;

          default:
            TRACE_ERROR( "SIG_GFP_MEAS_INT_MEAS_IND unexpected 1" );
            break;
        }
      }
      break;

    default:
      TRACE_ERROR( "SIG_GFP_MEAS_INT_MEAS_IND unexpected 2" );
      break;
  }
} /* sig_gfp_meas_int_meas_ind() */

#if !defined (NTRACE)

/*
+------------------------------------------------------------------------------
| Function    : meas_im_trace_i_level
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_im_trace_i_level( T_MEAS_IM_FILTER *filter, 
                                  T_ilev           *i_level )
{ 
  TRACE_ISIG( "meas_im_trace_i_level" );

  if( grr_data->meas_im.n_im_trace & M_IM_TRACE_OUTPUT_VALUE )
  {
    if( filter NEQ NULL )
    {
      TRACE_EVENT_P8( "meas_im_trace_i_level: %02X %02X %02X %02X %02X %02X %02X %02X",
                      i_level->ilev0, i_level->ilev1, i_level->ilev2,
                      i_level->ilev3, i_level->ilev4, i_level->ilev5,
                      i_level->ilev6, i_level->ilev7 );
    }
    else
    {
      TRACE_EVENT( "meas_im_trace_i_level: filter EQ NULL" );
    }
  }
} /* meas_im_trace_i_level() */
  
#endif /* #if !defined (NTRACE) */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_meas_ba_changed
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_CTRL_MEAS_BA_CHANGED
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_meas_ba_changed ( void )
{ 
  TRACE_ISIG( "sig_ctrl_meas_ba_changed" );

  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      psc_db->nc_ms.ncmeas.list.number = 0;
      psc_db->nc_ms.rfreq.number       = 0;

      psc_db->nc_ms.ncmeas.list.chng_mrk.curr++;
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_MEAS_BA_CHANGED unexpected" );
      break;
  }
} /* sig_ctrl_meas_ba_changed() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_meas_rxlev_pccch_ind
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_GFP_MEAS_RXLEV_PCCCH_IND
|
| Parameters  : pccch_lev - signal level received on pccch
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_meas_rxlev_pccch_ind( UBYTE pccch_lev )
{ 
  TRACE_ISIG( "sig_gfp_meas_rxlev_pccch_ind" );
  
  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
    case MEAS_ACCESS:
      meas_c_val_update_pim( pccch_lev );
      grr_cgrlc_pwr_ctrl_req( TRUE );
      break;

    default:
      TRACE_ERROR( "SIG_GFP_MEAS_RXLEV_PCCCH_IND unexpected" );
      break;
  }
} /* sig_gfp_meas_rxlev_pccch_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_meas_ctrl_pwr_ta_ptm
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_GFP_MEAS_CTRL_PWR_TA_PTM
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_meas_ctrl_pwr_ta_ptm ( void )
{ 
  MCAST( d_ctrl_pwr_ta, D_CTRL_PWR_TA );

  TRACE_ISIG( "sig_gfp_meas_ctrl_pwr_ta_ptm" );
  
  switch( GET_STATE( MEAS ) )
  {
    case MEAS_TRANSFER:
      if( d_ctrl_pwr_ta->v_gpta )
      {
        sig_meas_tc_update_ta_req( );
      }

      {
        UBYTE pb;
        
        pb = psc_db->g_pwr_par.pb;
        
        meas_handle_pwr_par( );
        
        if( pb NEQ psc_db->g_pwr_par.pb )
        {
          grr_update_pacch();
        }
      }
      break;

    default:
      TRACE_ERROR( "SIG_GFP_MEAS_CTRL_PWR_TA_PTM unexpected" );
      break;
  }
} /* sig_gfp_meas_ctrl_pwr_ta_ptm() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_meas_update_pch
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_meas_update_pch ( void )
{ 
  TRACE_ISIG( "sig_tc_meas_update_pch" );

  grr_cgrlc_pwr_ctrl_req( FALSE );

} /* sig_tc_meas_update_pch() */

/*
+------------------------------------------------------------------------------
| Function    : meas_c_get_value
+------------------------------------------------------------------------------
| Description : This function returns the current C value.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE meas_c_get_value ( void )
{
  UBYTE c_value;

  TRACE_FUNCTION( "meas_c_get_value" );

  if( grr_data->meas.c_filter.index > 0 )
  {
    c_value = ( M_ROUND_UP( grr_data->meas.c_filter.value, MEAS_ACRCY ) );
  }
  else
  {

#ifdef _SIMULATION_

    TRACE_ERROR( "meas_c_get_value: grr_data->meas.c_filter.index EQ 0" );

#endif /* #ifdef _SIMULATION_ */

    c_value = C_VALUE_DEFAULT;
  }

  return( c_value );

} /* meas_c_get_value() */

/*
+------------------------------------------------------------------------------
| Function    : meas_c_get_c_value
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_c_get_c_value ( T_CGRLC_c_value *c_value )
{
  TRACE_FUNCTION( "meas_c_get_c_value" );

  c_value->c_acrcy = MEAS_ACRCY;
  c_value->c_lev   = grr_data->meas.c_filter.value;
  c_value->c_idx   = grr_data->meas.c_filter.index;

} /* meas_c_get_c_value() */

/*
+------------------------------------------------------------------------------
| Function    : meas_c_set_c_value
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_c_set_c_value ( T_CGRLC_c_value *c_value )
{
  TRACE_FUNCTION( "meas_c_set_c_value" );

  grr_data->meas.c_filter.value = ( c_value->c_lev * MEAS_ACRCY ) / 
                                    c_value->c_acrcy;

  grr_data->meas.c_filter.index = c_value->c_idx;

} /* meas_c_set_c_value() */
