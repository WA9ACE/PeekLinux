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
|  Purpose :  This module implements local functions for service MEAS of
|             entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_MEASF_C
#define GRR_MEASF_C
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
#include "grr_measf.h"  /* to get the own definitions */
#include "grr_meass.h"  /* to get the own definitions */
#include "grr_ctrls.h"

/*==== CONST ================================================================*/

#define IDX_UBYTE_INVALID    0xFF             /* invalid index                 */
#define IDX_USHORT_INVALID   0xFFFF           /* invalid index                 */
#define TIME_INVALID         0x7FFFFFFF       /* invalid time, should be the   */
                                              /* largest possible value for an */
                                              /* variable of type LONG         */

#define USE_ALL_TIMESLOTS    0xFF             /* timeslot mask                 */

#define EM_EXT_RPT_TYPE_LEN   2   /* length of EXT reporting type bit field    */
#define EM_ABS_I_LEVEL_LEN    6   /* length of absolute I_LEVEL bit field      */
#define EM_NUM_OF_MEAS_LEN    5   /* length of number of measurement bit field */
#define EM_FREQ_N_LEN         6   /* length of FREQ_N bit field                */
#define EM_BSIC_N_LEN         6   /* length of BSIC_N bit field                */
#define EM_RXLEV_N_LEN        6   /* length of RXLEV_N bit field               */

#ifdef _SIMULATION_
#define GET_EM_RPT_PRD(x)     ((0x01<<(x))*10000) /* multiple of 10 sec., to get */            
                                                  /* a reduced test duration     */
#else /* #ifdef _SIMULATION_ */
#define GET_EM_RPT_PRD(x)     ((0x01<<(x))*60000) /* multiple of 60 sec.         */
#endif /* #ifdef _SIMULATION_ */

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

#define NORM_NAVGI_FAC (ULONG)10     /* normalised Navg_i factor                     */

#ifdef _TARGET_

#define MIN_GAMMA_FOR_I_LEVEL_RPTING 4 /* minimum I-LEVEL value of neighbour */
                                       /* cells that will be reported to the */
#else                                  /* network, otherwise I-LEVEL values  */
                                       /* will be reported as not available  */
#define MIN_GAMMA_FOR_I_LEVEL_RPTING 0

#endif /* #ifdef _TARGET_ */

#define MEAS_C_INC_INDEX(index) {                                        \
                                  if( index < (T_C_INDEX)(~0) ) index++; \
                                }

/*==== LOCAL VARS ===========================================================*/

LOCAL T_MEAS_IM_DATA* imeas = NULL; 
                          /* pointer to interference measurement parameter   */
LOCAL T_MEAS_EM_DATA* emeas = NULL;
                          /* pointer to extended measurement parameter       */

#if defined (REL99) AND defined (TI_PS_FF_EMR)
LOCAL BOOL  nc_freq_list_valid = FALSE;
LOCAL BOOL  nc_freq_list_pres = FALSE;
GLOBAL BOOL use_ba_gprs = TRUE;
#endif

/*
 * normalised math.power value 
 *
 * NORM_POW_MIN * ( 2 ** ( k / 2 ) )
 */

GLOBAL const ULONG norm_pow[] = { NORM_POW_MIN, 
                                     1414,    2000,    2828,    4000,    5657,
                                     8000,   11314,   16000,   22627,   32000,
                                    45255,   64000,   90510,  128000,  181019,
                                   256000,  362039,  512000,  724077, 1024000,
                                  1448155, 2048000, 2896309, 4096000, 5792619 };

/*==== LOCAL TYPES===========================================================*/

LOCAL void   meas_im_restart_all_filters ( void                                  );

LOCAL void   meas_im_restart_filter      ( T_MEAS_IM_FILTER     *filter          );

LOCAL void   meas_im_restart_gamma       ( T_MEAS_IM_FILTER     *filter,
                                           UBYTE                 index           );

LOCAL T_MEAS_IM_FILTER*
             meas_im_get_unused_filter   ( void                                  );

LOCAL void   meas_im_update_filter       ( T_MEAS_IM_FILTER     *filter, 
                                           UBYTE                *rxlev,
                                           UBYTE                 state           );

LOCAL BOOL   meas_im_freq_cmp_a          ( USHORT                arfcn1,
                                           USHORT                arfcn2          );

LOCAL BOOL   meas_im_freq_cmp_m          ( T_MEAS_IM_CARRIER    *ma1,
                                           T_MEAS_IM_CARRIER    *ma2             );

LOCAL ULONG  meas_im_get_val_trns        ( void                                  );

LOCAL ULONG  meas_im_get_val_idle        ( void                                  );

LOCAL UBYTE  meas_im_get_number_of_measured_channels
                                         ( void                                  );

LOCAL T_MEAS_IM_FILTER*
             meas_im_get_oldest_filter   ( void                                  );

LOCAL BOOL   meas_im_get_int_meas_freq   ( UBYTE                 index, 
                                           T_MEAS_IM_CARRIER    *carrier,
                                           UBYTE                *ts_mask         );

LOCAL BOOL   meas_im_get_trns_freq       ( T_MEAS_IM_CARRIER    *ma              );

LOCAL BOOL   meas_im_get_idle_freq       ( ULONG                 id, 
                                           T_MEAS_IM_CARRIER    *carrier,
                                           UBYTE                *ts_mask         );

LOCAL void   meas_im_timer_reorg         ( T_TIME                time_to_elapse,
                                           LONG                  time_diff       );

LOCAL void   meas_im_delete_em_rslt      ( void                                  );

LOCAL USHORT meas_im_get_unmsk_int_freq  ( UBYTE                 index           );

LOCAL void   meas_im_start_t_im_sync     ( T_TIME                vld_prd         );

LOCAL ULONG  meas_im_get_paging_period   ( void                                  );

LOCAL void   meas_im_set_freq_par        ( T_p_frequency_par    *freq_par, 
                                           T_MEAS_IM_CARRIER    *carrier         );

LOCAL void   meas_em_init_pmo_seq        ( void                                  );

LOCAL UBYTE  meas_em_build_meas_rpt      ( T_U_MEAS_REPORT      *u_meas_report   );

LOCAL UBYTE  meas_em_extrct_strgst       ( void                                  );

LOCAL UBYTE  meas_em_extrct_strgst_with_bsic
                                         ( void                                  );
LOCAL void   meas_em_req                 ( void                                  );

LOCAL BOOL   meas_em_is_meas_reporting   ( void                                  );

LOCAL void   meas_em_start_t3178         ( T_TIME                time            );

LOCAL void   meas_em_stop_t3178          ( void                                  );

/*==== PRIVATE FUNCTIONS ====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : meas_im_init 
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_init ( T_MEAS_IM_MODE im_mode )
{
  TRACE_FUNCTION( "meas_im_init" );

  INIT_STATE( MEAS_IM, MEAS_IM_NULL );

  imeas = &grr_data->meas_im;

#if !defined (NTRACE)

  imeas->n_im_trace       = 0;

#endif /* #if !defined (NTRACE) */

  imeas->mode             = im_mode;
  imeas->carrier_id       = IDX_USHORT_INVALID;
  imeas->trig_signal      = IM_TRIG_SIGNAL_NULL;
  imeas->v_cs_meas_active = FALSE;

  meas_im_restart_all_filters( );
} /* meas_im_init() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_restart_all_filters
+------------------------------------------------------------------------------
| Description : This function restarts all filters used in context of
|               interference measurements.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_im_restart_all_filters ( void )
{ 
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "meas_im_restart_all_filters" );
  
  for( i = 0; i < MAX_IM_CHANNELS; i++ )
  {
    meas_im_restart_filter( &imeas->filter[i] );
  }
} /* meas_im_restart_all_filters() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_restart_filter
+------------------------------------------------------------------------------
| Description : This function restarts all filters for one carrier 
|               used in context of interference measurements.
|
| Parameters  : *filter - Ptr to filter
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_im_restart_filter ( T_MEAS_IM_FILTER* filter )
{ 
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "meas_im_restart_filter" );

  for( i = 0; i < CGRLC_MAX_TIMESLOTS; i++ )
  {
    meas_im_restart_gamma( filter, i );
  }
} /* meas_im_restart_filter() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_restart_gamma
+------------------------------------------------------------------------------
| Description : This function restarts an individual filter for 
|               one channel used in context of interference measurements.
|
| Parameters  : *filter - Ptr to filter
|               index   - index of the interference level values
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_im_restart_gamma ( T_MEAS_IM_FILTER* filter,
                                   UBYTE             index )
{ 
  T_MEAS_IM_GAMMA* gamma = &filter->i_level[index];

  SET_FLAG( filter->used, IM_U_MASK << index, IM_U_UNUSED << index, UBYTE );

  gamma->index       = 0;
  gamma->m_gamma     = IM_I_NONE;
  gamma->gamma       = 0;
  gamma->remain_time = TIME_INVALID;
} /* meas_im_restart_gamma() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_filter
+------------------------------------------------------------------------------
| Description : This function is used to look for the filter that should be
|               updated. It searches for the filter with the same channel
|               description.
|
| Parameters  : *ma     - Ptr to frequency information in MA format
|
+------------------------------------------------------------------------------
*/
GLOBAL T_MEAS_IM_FILTER* meas_im_get_filter ( T_MEAS_IM_CARRIER *ma )
{ 
  UBYTE             i;                    /* used for counting */
  T_MEAS_IM_FILTER *filter       = NULL;  /* found filter      */
  BOOL              filter_found = FALSE; /* indicator         */

  TRACE_FUNCTION( "meas_im_get_filter" );

  i = 0;

  while( i < MAX_IM_CHANNELS AND !filter_found )
  {
    filter = &imeas->filter[i];

    if( ma->hopping )
    {
      if( ma                      NEQ NULL        AND
          filter->used            NEQ IM_U_UNUSED AND
          filter->carrier.hopping EQ  TRUE        AND
          meas_im_freq_cmp_m( ma, &filter->carrier ))
      {
        filter_found = TRUE;
      }
    }  
    else
    {
      if( filter->used            NEQ IM_U_UNUSED AND
          filter->carrier.hopping EQ  FALSE       AND 
          meas_im_freq_cmp_a( ma->alloc.arfcn,  
                              filter->carrier.alloc.arfcn ) )          
      {
        filter_found = TRUE;
      }
    }
    
    i++;
  }

  return( filter_found ? filter : NULL );
} /* meas_im_get_filter() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_unused_filter
+------------------------------------------------------------------------------
| Description : This function is used to look for the first unused filter.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL T_MEAS_IM_FILTER* meas_im_get_unused_filter ( void )
{
  UBYTE             i;             /* used for counting */
  T_MEAS_IM_FILTER *filter = NULL; /* found filter      */

  TRACE_FUNCTION( "meas_im_get_unused_filter" );

  i = 0;

  while( i < MAX_IM_CHANNELS AND filter EQ NULL )
  {
    if( imeas->filter[i].used EQ IM_U_UNUSED )
    {
      filter = &imeas->filter[i];
    }

    i++;
  }

  return( filter );
} /* meas_im_get_unused_filter() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_update_filter
+------------------------------------------------------------------------------
| Description : This function is used to update an interference level filter.
|
| Parameters  : *filter - Ptr to filter
|               *rxlev  - Ptr to measurement values
|               mode    - Packet idle or packet transfer mode
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_im_update_filter ( T_MEAS_IM_FILTER *filter, 
                                   UBYTE            *rxlev,
                                   UBYTE             state )
{ 
  UBYTE  i;                      /* used for counting                          */
  BOOL   valid_meas     = FALSE; /* indicates if at least one valid            */
                                 /* measurement sample is present              */
  T_TIME time_to_elapse =0;      /* this is the remaining time until           */
                                 /* timer T_IM_SYNC will elpase                */
  LONG   remain_time    = 0;     /* this is the remaining time for an          */
                                 /* individual filter                          */
  ULONG  d;                      /* forgetting factor                          */  
  ULONG  g_value;
  T_MEAS_IM_GAMMA *ilevel; 
                                 /* inteference level                          */
  ULONG  validity;               /* validity period of the measurement results */
  BOOL   v_t_im_sync    = FALSE; /* indicates whether the timer T_IM_SYNC is   */
                                 /* running or not                             */



  TRACE_FUNCTION( "meas_im_update_filter" );

  /* calculate validity period of the measurements */
  if( state EQ MEAS_IDLE )
  {
    validity = meas_im_get_val_idle( );
  }
  else
  {
    validity = meas_im_get_val_trns( );
  }

  /* calculate remaining time */
  time_to_elapse = grr_t_status( T_IM_SYNC );

  if(  time_to_elapse > 0 )
  {
    remain_time = validity - time_to_elapse;
    v_t_im_sync = TRUE;
  }

  for( i = 0; i < CGRLC_MAX_TIMESLOTS; i++ )
  {
    ilevel = &filter->i_level[i];

    if( rxlev[i] NEQ CGRLC_RXLEV_NONE )
    {
      SET_FLAG( filter->used, IM_U_MASK << i, IM_U_USED << i, UBYTE );
      valid_meas = TRUE;

      ilevel->remain_time = remain_time;

#ifdef _SIMULATION_
      TRACE_EVENT_P1("REMAIN_TIME = %d", remain_time);
#endif /* #ifdef (_SIMULATION_) */

      /* counting the iteration index */
      if( NORM_POW_MIN * ilevel->index < norm_pow[psc_db->g_pwr_par.n_avg_i] )
      {
        ilevel->index++;
      }

      /* calculate the forgetting factor */
      d = ( NORM_POW_MIN * NORM_POW_MIN * NORM_NAVGI_FAC ) / 
          MINIMUM( NORM_POW_MIN * ilevel->index,
                   norm_pow[psc_db->g_pwr_par.n_avg_i] );

      /* update the filter */
      g_value = ( ( NORM_POW_MIN * NORM_NAVGI_FAC ) - d ) * ilevel->gamma 
                +
                d * MEAS_ACRCY * rxlev[i];
    
      ilevel->gamma = (T_IM_GAMMA)(g_value / (NORM_POW_MIN * NORM_NAVGI_FAC));

#ifdef _SIMULATION_
      TRACE_EVENT_P2("TS = %d, I-LEVEL = %d", i, ilevel->gamma);
#endif /* #ifdef (_SIMULATION_) */

      /* store the mode in which the measurement was performed */
      if( state EQ MEAS_IDLE )
      {
        SET_FLAG( ilevel->m_gamma, IM_I_MASK_IDL,  IM_I_IDL,  UBYTE );
        SET_FLAG( ilevel->m_gamma, IM_I_MASK_TRNS, IM_I_NONE, UBYTE );
      }
      else
      {
        SET_FLAG( ilevel->m_gamma, IM_I_MASK_TRNS, IM_I_TRNS, UBYTE );
        SET_FLAG( ilevel->m_gamma, IM_I_MASK_IDL,  IM_I_NONE, UBYTE );
      }

    }
  } /* i */

  /*
   * do some administrative stuff only in case there was at least
   * one valid sample reported by Layer 1
   */
  if( valid_meas )
  {
    if( v_t_im_sync )
    {
      if( remain_time < 0 )
      {
        meas_im_timer_reorg( time_to_elapse, remain_time );
      }
    }
    else
    {
      meas_im_start_t_im_sync( (ULONG)validity );
    }
  }
} /* meas_im_update_filter() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_freq_cmp_a
+------------------------------------------------------------------------------
| Description : This function returns whether two frequencies are identical
|               or not.
|
| Parameters  : arfcn1 - First frequency used for comparison
|               arfcn2 - Second frequency used for comparison
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL meas_im_freq_cmp_a ( USHORT arfcn1, USHORT arfcn2 )
{
  return( arfcn1 EQ arfcn2 );
} /* meas_im_freq_cmp_a() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_freq_cmp_m
+------------------------------------------------------------------------------
| Description : This function returns whether two frequency information given
|               in mobile allocation format are identical or not. Channels 
|               that only differ in MAIO are considered the same.
|
| Parameters  : *ma1 - First channel description used for comparison
|               *ma2 - Second channel description used for comparison
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL meas_im_freq_cmp_m ( T_MEAS_IM_CARRIER *ma1, T_MEAS_IM_CARRIER *ma2  )
{
  return
  (
    ma1->alloc.hop_param.hsn EQ ma2->alloc.hop_param.hsn 
    
    AND
    
    memcmp( ma1->alloc.hop_param.arfcn_bit_field, 
            ma2->alloc.hop_param.arfcn_bit_field,
            sizeof( ma1->alloc.hop_param.arfcn_bit_field ) ) EQ 0
  );
} /* meas_im_freq_cmp_m() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_val_trns
+------------------------------------------------------------------------------
| Description : This function returns the validity period of a measurement 
|               performed in packet transfer mode (unit is milliseconds).
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL ULONG meas_im_get_val_trns ( void )
{
  ULONG n_avg_i_half = norm_pow[psc_db->g_pwr_par.n_avg_i] / 2;

  return(ULONG)
    ( ( n_avg_i_half * ( USEC_PER_MF / (ULONG)10 ) ) /
                            ( NORM_POW_MIN * (ULONG)MEAS_ACRCY / (ULONG)10 ) );
} /* meas_im_get_val_trns() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_val_idle
+------------------------------------------------------------------------------
| Description : This function returns the validity period of a measurement 
|               performed in packet idle mode (unit is milliseconds).
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL ULONG meas_im_get_val_idle ( void )
{
  ULONG val_idle      = TIME_INVALID - 1;
  ULONG paging_period = meas_im_get_paging_period( );

  if( paging_period NEQ NO_PAGING )
  {
    ULONG acrcy_n, acrcy_p;
    ULONG novimc_t_navgi = meas_im_get_number_of_measured_channels( ) * 
                           norm_pow[psc_db->g_pwr_par.n_avg_i];

    if     ( paging_period  <= 650000  ) acrcy_p = 10;
    else if( paging_period  <= 6500000 ) acrcy_p = 100;
    else                                 acrcy_p = 1000;

    if     ( novimc_t_navgi <= 130000  ) acrcy_n = 1;
    else if( novimc_t_navgi <= 1300000 ) acrcy_n = 10;
    else                                 acrcy_n = 100;

    val_idle = 
      (ULONG)
        (
          ( MAXIMUM(
                     ( novimc_t_navgi / ( (ULONG)4 * acrcy_n ) ) *
                     ( USEC_PER_MF    / acrcy_p ),
                     ( novimc_t_navgi / ( (ULONG)2 * acrcy_n ) ) *
                     ( paging_period  / acrcy_p )
                   )
          ) / ( NORM_POW_MIN * (ULONG)MEAS_ACRCY / ( acrcy_n * acrcy_p ) ) 
        );
  }

  return val_idle;
} /* meas_im_get_val_idle() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_number_of_measured_channels
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE meas_im_get_number_of_measured_channels ( void )
{
  UBYTE number = psc_db->number_of_valid_int_meas_channels;

  if( imeas->idle_chan.ident[CHN_ID_EM_BASE] NEQ CHN_ID_INVALID )
  {
    number++;
  }

  if( imeas->idle_chan.ident[CHN_ID_PCCCH_BASE] NEQ CHN_ID_INVALID )
  {
    number++;
  }
 
  return( number );
} /* meas_im_get_number_of_measured_channels() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_oldest_filter
+------------------------------------------------------------------------------
| Description : This functions returns a filter that can be used for storing
|               measurement results
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL T_MEAS_IM_FILTER* meas_im_get_oldest_filter ( void )
{
  UBYTE i, j;                           /* used for counting                */
  LONG  remain1    = TIME_INVALID;      /* holds remaining time             */
  LONG  remain2;                        /* holds remaining time             */
  BOOL  all_invalid;                    /* indicates if all remaining times */
                                        /* are invalid                      */
  UBYTE filter_idx = IDX_UBYTE_INVALID; /* index of oldest filter           */
  T_MEAS_IM_GAMMA *ilevel;              /* interference level               */

  TRACE_FUNCTION( "meas_im_get_oldest_filter" );

  for( i = 0; i < MAX_IM_CHANNELS; i++ )
  {
    remain2     = TIME_INVALID;
    all_invalid = TRUE;

    for( j = 0; j < CGRLC_MAX_TIMESLOTS; j++ )
    {
      ilevel = &imeas->filter[i].i_level[j];
  
      if( ilevel->remain_time NEQ TIME_INVALID )
      {
        all_invalid = FALSE;

        if( remain2 EQ TIME_INVALID )
        {
          remain2 = ilevel->remain_time;
        }
        else
        {
          remain2 = MINIMUM( ilevel->remain_time, remain2 );
        }
      }
    }

    if( all_invalid )
    {
      filter_idx = i;
    }
    else if( remain1 EQ TIME_INVALID OR remain1 < remain2 )
    {
      remain1    = remain2;
      filter_idx = i;
    }
  }

  return( filter_idx EQ IDX_UBYTE_INVALID ? NULL : &imeas->filter[i] );
} /* meas_im_get_oldest_filter() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_int_meas_freq
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL meas_im_get_int_meas_freq ( UBYTE              index, 
                                       T_MEAS_IM_CARRIER *carrier,
                                       UBYTE             *ts_mask )
{
  T_p_frequency_par    freq_par;
  T_int_meas_chan_lst *lst   = &psc_db->int_meas_chan_list[index];
  BOOL                 valid = TRUE;

  TRACE_FUNCTION( "meas_im_get_int_meas_freq" );

  if( lst->v_arfcn EQ TRUE )
  {
    carrier->hopping     = FALSE;
    carrier->alloc.arfcn = lst->arfcn;
  }
  else if( lst->v_ma_num_maio EQ TRUE )
  {
    if( ( valid = grr_create_freq_list(  lst->ma_num,
                                         lst->maio,
                                        &freq_par.p_chan_sel,
                                        &freq_par.p_freq_list ) ) EQ TRUE )
    {
      meas_im_set_carrier( carrier, &freq_par );
    }
  }
  else
  {
    valid = FALSE;
  }
  
  *ts_mask = lst->ts_alloc;

  return( valid );
} /* meas_im_get_int_meas_freq() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_trns_freq
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL meas_im_get_trns_freq ( T_MEAS_IM_CARRIER *ma )
{
  TRACE_FUNCTION( "meas_im_get_trns_freq" );

  if( grr_data->tc.v_freq_set EQ TRUE )
  {
    T_p_frequency_par freq_par;

    grr_set_freq_par( &freq_par );
    meas_im_set_carrier( ma, &freq_par );
  }

  return( grr_data->tc.v_freq_set );
} /* meas_im_get_trns_freq() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_idle_freq
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL meas_im_get_idle_freq ( ULONG              id, 
                                   T_MEAS_IM_CARRIER *carrier,
                                   UBYTE             *ts_mask )
{
  BOOL                valid_id = TRUE;
  T_MEAS_IM_IDLE_CHN* chn      = &imeas->idle_chan;

  TRACE_FUNCTION( "meas_im_get_idle_freq" );

  if( id             >= MAX_IM_IDLE_CHANNELS OR 
      chn->ident[id] EQ CHN_ID_INVALID          )
  {
    TRACE_ERROR( "ID invalid in meas_im_get_idle_freq" );
    valid_id = FALSE; 
  }
  else if( id EQ CHN_ID_EM_BASE )
  {
    *ts_mask             = USE_ALL_TIMESLOTS; /* SZML-MEAS/005 */
    carrier->hopping     = FALSE;
    carrier->alloc.arfcn = meas_im_get_unmsk_int_freq( chn->ident[id] );
  }
  else if( id EQ CHN_ID_PCCCH_BASE )
  { 
    T_p_frequency_par freq_par;

    *ts_mask = ( 0x80 >> psc_db->paging_group.pccch[chn->ident[id]].tn );

    grr_get_pccch_freq_par(  chn->ident[id],
                            &freq_par.p_chan_sel,
                            &freq_par.p_freq_list );
    
    meas_im_set_carrier( carrier, &freq_par );
  }
  else
  {
    valid_id = meas_im_get_int_meas_freq( chn->ident[id], carrier, ts_mask );
  }

  return( valid_id );
} /* meas_im_get_idle_freq() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_timer_reorg
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_im_timer_reorg ( T_TIME time_to_elapse, LONG time_diff )
{
  UBYTE i, j; /* used for counting */

  TRACE_FUNCTION( "meas_im_timer_reorg" );
   
  vsi_t_stop( GRR_handle, T_IM_SYNC );

  for( i = 0; i < MAX_IM_CHANNELS; i++ )
  {
    for( j = 0; j < CGRLC_MAX_TIMESLOTS; j++ )
    {
      if( imeas->filter[i].i_level[j].remain_time NEQ TIME_INVALID )
      {
        imeas->filter[i].i_level[j].remain_time -= time_diff;
      }
    }
  }
    
  meas_im_start_t_im_sync( ( LONG )time_to_elapse + time_diff  );

} /* meas_im_timer_reorg() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_fill_rel_iLevel
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_fill_rel_iLevel ( UBYTE            *v_ilev,
                                      UBYTE            *ilev,
                                      T_MEAS_IM_FILTER *filter,
                                      UBYTE             idx )
{
  UBYTE val; /* holds interference level data */

  *v_ilev = FALSE;

  if( filter NEQ NULL )
  {

#ifdef _TARGET_

    UBYTE state = GET_STATE( MEAS );

    if(
        ( 
          state EQ MEAS_IDLE                                                   
          AND
          IS_FLAGGED( filter->i_level[idx].m_gamma, IM_I_MASK_IDL,  IM_I_IDL  )
        ) 
        OR
        ( 
          state EQ MEAS_TRANSFER       
          AND
          IS_FLAGGED( filter->i_level[idx].m_gamma, IM_I_MASK_TRNS, IM_I_TRNS )  
        )
      )

#endif /* #ifdef _TARGET_ */

    {
      if( NORM_POW_MIN * filter->i_level[idx].index >= 
                                          norm_pow[psc_db->g_pwr_par.n_avg_i] )
      {
        if( filter->i_level[idx].gamma >= 
                                ( MIN_GAMMA_FOR_I_LEVEL_RPTING * MEAS_ACRCY ) ) /*lint !e685*/
        {
          T_C_FILTER *c_filter = &grr_data->meas.c_filter;
          USHORT      c_value;

          if( c_filter->index > 0 )
          {
            c_value = c_filter->value;
          }
          else
          {
            c_value = C_VALUE_DEFAULT;
          }

          /* calculation of interference level */
          if( c_value < filter->i_level[idx].gamma )
          {
            *ilev = I_LEVEL_MIN;
          }
          else
          {
            val = ( c_value - filter->i_level[idx].gamma ) / ( 2 * MEAS_ACRCY ) + 1;
      
            if( val > I_LEVEL_MAX ) *ilev = I_LEVEL_MAX;
            else                    *ilev = val;
          }

          *v_ilev = TRUE;
        }

#if !defined (NTRACE)

        else
        {
          /* prepare proper tracing information */
          switch( filter->i_level[idx].gamma / MEAS_ACRCY )
          {
            case 0: *ilev = I_LEVEL_GAMMA_0_SKIPPED; break;
            case 1: *ilev = I_LEVEL_GAMMA_1_SKIPPED; break;
            case 2: *ilev = I_LEVEL_GAMMA_2_SKIPPED; break;
            case 3: *ilev = I_LEVEL_GAMMA_3_SKIPPED; break;

            default: 
              TRACE_ERROR( "meas_im_fill_rel_iLevel: impossible case" );
              break;
          }
        }

#endif /* #if !defined (NTRACE) */

      }

#if !defined (NTRACE)

      else
      {
        /* prepare proper tracing information */
        *ilev = I_LEVEL_IDX_TO_SMALL;
      }

#endif /* #if !defined (NTRACE) */

    }

#ifdef _TARGET_

  #if !defined (NTRACE)

    else
    {
      /* prepare proper tracing information */
      *ilev = I_LEVEL_STATE_MISMATCH;
    }

  #endif /* #if !defined (NTRACE) */

#endif /* #ifdef _TARGET_ */

  }
  
#if !defined (NTRACE)

  else
  {
    /* prepare proper tracing information */
    *ilev = I_LEVEL_NOT_AVAIL;
  }

#endif /* #if !defined (NTRACE) */

} /* meas_im_fill_rel_iLevel() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_fill_abs_iLevel
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE meas_im_fill_abs_iLevel ( UBYTE            *v_ilev,
                                       UBYTE            *ilev,
                                       T_MEAS_IM_FILTER *filter,
                                       UBYTE             idx )
{
  UBYTE used_bits = PMR_FLAG_LEN;

  *v_ilev = FALSE;

  if( filter NEQ NULL )
  {

#ifdef _TARGET_

    UBYTE state = GET_STATE( MEAS );

    if(
        ( 
          state EQ MEAS_IDLE                                                   
          AND
          IS_FLAGGED( filter->i_level[idx].m_gamma, IM_I_MASK_IDL,  IM_I_IDL  )
        ) 
        OR
        ( 
          state EQ MEAS_TRANSFER       
          AND
          IS_FLAGGED( filter->i_level[idx].m_gamma, IM_I_MASK_TRNS, IM_I_TRNS )  
        )
      )

#endif /* #ifdef _TARGET_ */

    {
      if( NORM_POW_MIN * filter->i_level[idx].index >= 
                                          norm_pow[psc_db->g_pwr_par.n_avg_i] )
      {
        if( filter->i_level[idx].gamma >= 
                                ( MIN_GAMMA_FOR_I_LEVEL_RPTING * MEAS_ACRCY ) ) /*lint !e685*/
        {
          /* calculation of interference level */
          used_bits += EM_ABS_I_LEVEL_LEN;

          *ilev   = (UBYTE)M_ROUND_UP( filter->i_level[idx].gamma, MEAS_ACRCY );
          *v_ilev = TRUE;
        }

#if !defined (NTRACE)

        else
        {
          /* prepare proper tracing information */
          switch( filter->i_level[idx].gamma / MEAS_ACRCY )
          {
            case 0: *ilev = I_LEVEL_GAMMA_0_SKIPPED; break;
            case 1: *ilev = I_LEVEL_GAMMA_1_SKIPPED; break;
            case 2: *ilev = I_LEVEL_GAMMA_2_SKIPPED; break;
            case 3: *ilev = I_LEVEL_GAMMA_3_SKIPPED; break;

            default: 
              TRACE_ERROR( "meas_im_fill_rel_iLevel: impossible case" );
              break;
          }
        }

#endif /* #if !defined (NTRACE) */

      }

#if !defined (NTRACE)

      else
      {
        /* prepare proper tracing information */
        *ilev = I_LEVEL_IDX_TO_SMALL;
      }

#endif /* #if !defined (NTRACE) */

    }

#ifdef _TARGET_

  #if !defined (NTRACE)

    else
    {
      /* prepare proper tracing information */
      *ilev = I_LEVEL_STATE_MISMATCH;
    }

  #endif /* #if !defined (NTRACE) */

#endif /* #ifdef _TARGET_ */

  }

#if !defined (NTRACE)

  else
  {
    /* prepare proper tracing information */
    *ilev = I_LEVEL_NOT_AVAIL;
  }

#endif /* #if !defined (NTRACE) */

  return( used_bits );
} /* meas_im_fill_abs_iLevel() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_delete_em_rslt
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_im_delete_em_rslt ( void )
{
  T_MEAS_IM_FILTER  *filter;

  TRACE_FUNCTION( "meas_im_delete_em_rslt" );

  if( emeas->xmeas_set.param->em1.param.int_frequency NEQ NOT_SET )
  {
    T_MEAS_IM_CARRIER ma;

    ma.hopping     = FALSE;    
    ma.alloc.arfcn =
      meas_im_get_unmsk_int_freq( emeas->xmeas_set.param->em1.param.int_frequency );
  
    filter = meas_im_get_filter( &ma );

    if( filter NEQ NULL )
    {
      meas_im_restart_filter ( filter );
    }
  }
} /* meas_im_delete_em_rslt() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_unmsk_int_freq
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
LOCAL USHORT meas_im_get_unmsk_int_freq ( UBYTE index )
{
  USHORT arfcn;

  TRACE_FUNCTION( "meas_im_get_unmsk_int_freq" );

  arfcn = emeas->xmeas_set.param->em1.list.freq[index];
  SET_FLAG( arfcn, EM_VLDTY_ARFCN_MASK, EM_VLD_ARFCN, USHORT );

  return( arfcn );
} /* meas_im_get_unmsk_int_freq() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_start_t_im_sync
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_im_start_t_im_sync ( T_TIME vld_prd )
{
  TRACE_FUNCTION( "meas_im_start_t_im_sync" );

  if( vld_prd NEQ 0 )
  {
    vsi_t_start( GRR_handle, T_IM_SYNC, vld_prd );

#ifdef _SIMULATION_
    TRACE_EVENT_P1( "T_IM_SYNC: %d ms", vld_prd );
#endif /* #ifdef _SIMULATION_ */

  }
  else
  {
    TRACE_ERROR( "Try to start T_IM_SYNC with 0 ms" );
  }
} /* meas_im_start_t_im_sync() */


/*
+------------------------------------------------------------------------------
| Function    : meas_im_get_paging_period 
+------------------------------------------------------------------------------
| Description : This functon calculates the paging period in microseconds.
|               
|               Please look at GSM 05.02, chapter 6.5.6, Determination of
|               PCCCH_GROUP and PAGING_GROUP for MS in GPRS attached mode.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL ULONG meas_im_get_paging_period ( void )
{
  ULONG  paging_period = NO_PAGING; /* paging period in MICROSECONDS             */
  USHORT paging_distance;           /* paging period as difference of two paging */
                                    /* groups                                    */
  USHORT paging_group[BL_FOR_PAGE_PERIOD_CALC]; 
                                    /* blocks which are used to page the MS, in  */
                                    /* maximum 2 blocks are needed to calculate  */
                                    /* the paging period                         */
  USHORT M;                         /* number of paging blocks "available" on    */
                                    /* one PCCCH                                 */ 
  USHORT m;                         /* number of paging groups for this MS       */
  USHORT min;                       /* minimum of M and SPLIT_PG_CYCLE           */
  USHORT imsi_mod   = grr_imsi_mod( );
                                    /* IMSI modulo 1000                          */
  USHORT N          = 1;            /* N                                         */
  USHORT res_blocks = psc_db->pccch.bs_pag_blks + psc_db->pccch.bs_pbcch_blks + 1;
                                    /* number of reserved blocks for access      */
                                    /* grant and PBCCH                           */

  TRACE_FUNCTION( "meas_im_get_paging_period" );

  /* see GSM 05.02, Clause 7 Table 7 of 7: Mapping of logical channels onto */
  /* physical channels ( see subclause 6.3, 6.4, 6.5 )                      */
  M = ( res_blocks > ( BLOCK_PER_MF - 1 ) ? 
                            0 : ( BLOCK_PER_MF - res_blocks ) * PAGING_CYCLE );

  /* in case M is equal to zero no paging blocks are reserved */
  if( M NEQ 0 )
  {
    if( grr_data->ms.split_pg_cycle EQ GMMRR_NO_DRX )
    {
      /* in non-DRX mode the MS shall listen to all M blocks per */
      /* multiframe where paging may appear on a PCCCH channel   */
      /* we assume that all paging groups have the same distance */
      /* to its preceding and subsequent paging groups ( equi-   */
      /* distance)                                               */
      paging_period = ( PAGING_CYCLE * USEC_PER_MF ) / M;
    }
    else
    {
      min = MINIMUM( M, grr_data->ms.split_pg_cycle );

      if( min EQ 1 )
      {
        /* in case only one paging group is available the MS is */
        /* paged every PAGING_CYCLE multiframes                 */
        paging_period = PAGING_CYCLE * USEC_PER_MF;
      }  
      else
      {
        /* calculate two paging groups, this is the minimum which */
        /* is needed to calculate a time difference ( paging      */
        /* period ), we assume that all paging groups have the    */
        /* same distance to its preceding and subsequent paging   */
        /* groups ( equi-distance )                               */
        for( m = 0; m < BL_FOR_PAGE_PERIOD_CALC; m++ )
        {
          /* the following statement calculates the paging groups */
          paging_group[m] =
            ( 
              ( ( imsi_mod / ( psc_db->paging_group.kc * N ) ) * N ) +
              ( imsi_mod % N )                                       +
              MAXIMUM( ( m * M ) / grr_data->ms.split_pg_cycle, m )
            ) % M;
        } 

        /* the paging is repeated cyclic every PAGING_CYCLE multiframes, */
        /* this should be taken into account when calculating the time   */
        /* difference between two paging groups                          */
        if( paging_group[1] > paging_group[0] )
        {
          paging_distance = paging_group[1] - paging_group[0];
        }
        else
        {
          paging_distance = M - paging_group[0] + paging_group[1];
        }

        paging_period = ( paging_distance * PAGING_CYCLE * USEC_PER_MF ) / M;
      }
    }
  }

  return( paging_period );
} /* meas_im_get_paging_period() */

/*
+------------------------------------------------------------------------------
| Function    : meas_em_init 
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_em_init ( void )
{
  TRACE_FUNCTION( "meas_em_init" );

  INIT_STATE( MEAS_EM, MEAS_EM_NULL );

  emeas = &grr_data->meas_em;

  emeas->xmeas_set.ident = 0;
  emeas->xmeas_set.param = NULL;
  emeas->pmr_snd_ref     = 0;
  
  grr_init_xmeas_struct( &psc_db->ext_pmo );
  meas_em_init_pmo_seq( );
} /* meas_em_init() */

/*
+------------------------------------------------------------------------------
| Function    : meas_em_init_pmo_seq
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_em_init_pmo_seq ( void )
{
  UBYTE i;

  TRACE_FUNCTION( "meas_em_init_pmo_seq" );

  grr_init_xmeas_struct( &emeas->pmo.extd );
  grr_init_ncmeas_extd_struct( &emeas->pmo.nc, FALSE );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  grr_init_enh_param(&emeas->pmo.enh, TRUE);
#endif

  for( i = 0; i < MAX_NR_OF_INSTANCES_OF_PMO; i++ )
  {
    emeas->pmo.prm.instances[i] = FALSE;
    emeas->pmo.prm.idx[i].start = RRGRR_INVALID_IDX;
    emeas->pmo.prm.idx[i].stop  = RRGRR_INVALID_IDX;
  }

  emeas->pmo.prm.count = NOT_SET;
} /* meas_em_init_pmo_seq() */

/*
+------------------------------------------------------------------------------
| Function    : meas_em_build_meas_rpt
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : u_meas_report - Pointer to measurement report
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE meas_em_build_meas_rpt ( T_U_MEAS_REPORT *u_meas_report )
{
  UBYTE        used_bits;      /* bits currently used for the measurement report    */
  UBYTE        next_meas_bits; /* bits used for the next element of the             */
                               /* measurement result list                           */

  UBYTE        strg_bsic;
  UBYTE        strg_rxlev;
  UBYTE        rslt_rxlev;
  T_ext_mp_s1  *p_mrpt;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  UBYTE        psi3_cm = grr_get_psi3_cm();
  BOOL         pbcch_present = grr_is_pbcch_present( );
#endif


  T_xmeas_rep    *xmeas_rep = &u_meas_report->xmeas_rep;

  TRACE_FUNCTION( "meas_em_build_meas_rpt" );

  /* process message type */
  used_bits               = PMR_MSG_TYPE_LEN;
  u_meas_report->msg_type = U_MEAS_REPORT_c;

  /* process TLLI */
  used_bits += PMR_TLLI_LEN;
  grr_set_buf_tlli( &u_meas_report->tlli_value, grr_get_tlli( ) );

  /* process PSI5 change mark */
  used_bits += PMR_FLAG_LEN;

  if( emeas->xmeas_set.param EQ &psc_db->ext_psi5 )
  {
    used_bits += PMR_PSI5_CHNGE_MRK_LEN;

    u_meas_report->v_psi5_cm = TRUE;
    u_meas_report->psi5_cm   = psc_db->psi5_params.psi5_change_mark;
  }
  else if ( emeas->xmeas_set.param EQ &psc_db->ext_pmo )
  {
    u_meas_report->v_psi5_cm = FALSE;
  }
  else
  {
    TRACE_ERROR( "no valid data set in meas_em_build_meas_rpt" );
    return( 0 );
  }
  
  /* process flags */
  used_bits                    += PMR_FLAG_LEN;
  u_meas_report->flag           = 1;
  u_meas_report->v_nc_meas_rep  = FALSE;
  u_meas_report->v_xmeas_rep    = TRUE;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  /*if(psc_db->network_rel NEQ BSS_NW_REL_97)*/
  {
    used_bits                += PMR_FLAG_LEN;
    u_meas_report->v_release_99_str_pmr = TRUE;
    
    used_bits                += PMR_FLAG_LEN;
    u_meas_report->release_99_str_pmr.v_meas_rep_3g_str = FALSE;
    
    used_bits                += PMR_FLAG_LEN;      
    u_meas_report->release_99_str_pmr.v_ba_psi3_str = TRUE;
    
    used_bits                += PMO_IND_USED_LEN;
    u_meas_report->release_99_str_pmr.ba_psi3_str.pmo_ind_used = psc_db->nc_ms.pmo_ind;
    
    /* 1 bit for u_meas_report->ba_psi3_str.flag */
    used_bits                += PMR_FLAG_LEN;      
    
    /* process BA used, PSI3 change mark and PMO used */
    if( pbcch_present EQ FALSE )
    {
      u_meas_report->release_99_str_pmr.ba_psi3_str.flag = FALSE;
      
      u_meas_report->release_99_str_pmr.ba_psi3_str.v_ba_ind_used = TRUE;
      u_meas_report->release_99_str_pmr.ba_psi3_str.v_ba_ind_used_3g = TRUE;
      
      used_bits                += PMR_BA_IND_USED_LEN;
      u_meas_report->release_99_str_pmr.ba_psi3_str.ba_ind_used = psc_db->ba_ind;
      
      /* Add 1 bit for ba_ind_used_3g */              
      u_meas_report->release_99_str_pmr.ba_psi3_str.ba_ind_used_3g = 0;
      used_bits                += PMR_BA_IND_USED_LEN;
    }
    else
    {
      u_meas_report->release_99_str_pmr.ba_psi3_str.flag = TRUE;
      
      u_meas_report->release_99_str_pmr.ba_psi3_str.v_psi3_cm = TRUE;
      
      used_bits                += PMR_PSI3_CHNGE_MRK_LEN;
      u_meas_report->release_99_str_pmr.ba_psi3_str.psi3_cm = psi3_cm;
    }
  }
#elif defined (REL99)
  used_bits                += PMR_FLAG_LEN;
  u_meas_report->v_release_99_str_pmr = FALSE;
#endif

  /* process report type */
  used_bits            += EM_EXT_RPT_TYPE_LEN;
  xmeas_rep->xrep_type  = emeas->xmeas_set.param->em1.param.reporting_type;
  
  /* process interference measurement results */
  used_bits += PMR_FLAG_LEN;

  if( emeas->xmeas_set.param->em1.param.reporting_type EQ  REP_TYPE_3 AND
      emeas->xmeas_set.param->em1.param.int_frequency  NEQ NOT_SET        )
  {
    T_MEAS_IM_CARRIER ma;

    xmeas_rep->v_ilev_abs = TRUE;
     
    ma.hopping     = FALSE;    
    ma.alloc.arfcn =
      meas_im_get_unmsk_int_freq( emeas->xmeas_set.param->em1.param.int_frequency );

    used_bits += meas_im_get_abs_i_level( &ma, &xmeas_rep->ilev_abs );
  }
  else
  {
    xmeas_rep->v_ilev_abs = FALSE;
  }

  /* process EXT measurement results */
  used_bits              += EM_NUM_OF_MEAS_LEN;
  xmeas_rep->num_meas     = 0;
  xmeas_rep->c_ext_mp_s1  = 0;

  do
  {
    /* calculate length of next measurement result */
    next_meas_bits = EM_FREQ_N_LEN + EM_RXLEV_N_LEN + PMR_FLAG_LEN;
    strg_rxlev     = 0;
    rslt_rxlev     = 0;
    strg_bsic      = EM_INVLD_BSIC;


    switch( emeas->xmeas_set.param->em1.param.reporting_type )
    {
      case( REP_TYPE_1 ):
      case( REP_TYPE_2 ):
        if( emeas->pmr_snd_ref EQ emeas->strg.number )
        {
          next_meas_bits = 0;
        }
        else
        {
          strg_bsic  = emeas->rslt.elem[emeas->strg.refidx[emeas->pmr_snd_ref]].bsic;
          strg_rxlev = emeas->rslt.elem[emeas->strg.refidx[emeas->pmr_snd_ref]].rxlev;

          if( strg_bsic                                        NEQ RRGRR_INVALID_BSIC OR 
              emeas->xmeas_set.param->em1.param.reporting_type EQ  REP_TYPE_2       )
          {
            next_meas_bits += EM_BSIC_N_LEN;
          }
        }
        break;

      case( REP_TYPE_3 ):        
        if( emeas->pmr_snd_ref EQ emeas->rslt.number )
        {
          next_meas_bits = 0;
        }
        else
        {
          rslt_rxlev = emeas->rslt.elem[emeas->pmr_snd_ref].rxlev;
        }
        break;
      default:
        next_meas_bits = 0;        
        TRACE_ERROR( "no valid reporting type in meas_em_build_meas_rpt" );
        break;
    }

    if( next_meas_bits                             NEQ 0              AND
        ( BIT_UL_CTRL_BLOCK_CONTENTS - used_bits ) >=  next_meas_bits     )
    {
      /* copy next measurement result to measurement report */
      xmeas_rep->num_meas++;
      xmeas_rep->c_ext_mp_s1++;
      used_bits += next_meas_bits;
      p_mrpt     = &xmeas_rep->ext_mp_s1[xmeas_rep->c_ext_mp_s1 - 1];

      switch( emeas->xmeas_set.param->em1.param.reporting_type )
      {
        case( REP_TYPE_1 ):
        case( REP_TYPE_2 ):
          p_mrpt->freq_n   = emeas->rslt.elem[emeas->strg.refidx[emeas->pmr_snd_ref]].refidx;
          p_mrpt->rxlev_n  = strg_rxlev;

          if( strg_bsic                                        NEQ RRGRR_INVALID_BSIC OR 
              emeas->xmeas_set.param->em1.param.reporting_type EQ  REP_TYPE_2       )
          {
            p_mrpt->bsic   = strg_bsic;
            p_mrpt->v_bsic = TRUE;
          }
          else
          {
            p_mrpt->v_bsic = FALSE;
          }
          break;

        case( REP_TYPE_3 ):
          p_mrpt->freq_n   = emeas->rslt.elem[emeas->pmr_snd_ref].refidx;
          p_mrpt->rxlev_n  = rslt_rxlev;
          p_mrpt->v_bsic   = FALSE;
          break;
        default:
          TRACE_ERROR( "no valid reporting type in meas_em_build_meas_rpt" );
          break;
      }

      emeas->pmr_snd_ref++;
    }
    else
    {
      next_meas_bits = 0;
    }
  }
  while( next_meas_bits NEQ 0 );

  return( xmeas_rep->num_meas );
} /* meas_em_build_meas_rpt() */

/*
+------------------------------------------------------------------------------
| Function    : meas_em_extrct_strgst
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE meas_em_extrct_strgst ( void )
{
  UBYTE i, j;      /* used for couting */
  UBYTE min_rxlev;
  UBYTE min_index;

  TRACE_FUNCTION( "meas_em_extrct_strgst" );

  emeas->strg.number = 0;

  for( i = 0; i < emeas->rslt.number; i++ )
  {
    if( emeas->rslt.elem[i].rxlev NEQ RRGRR_INVALID_RXLEV  )
    {
      if( emeas->strg.number < EM_MAX_STRONG_CARRIER )
      {
        emeas->strg.refidx[emeas->strg.number] = i;
        emeas->strg.number++;
      }
      else
      {
        min_rxlev = 0xFF;
        min_index = 0xFF;
      
        for( j = 0; j < emeas->strg.number; j++ )
        {
          if( emeas->rslt.elem[emeas->strg.refidx[j]].rxlev < min_rxlev  )
          {
            min_rxlev = emeas->rslt.elem[emeas->strg.refidx[j]].rxlev;
            min_index = j;
          }
        }

        if( emeas->rslt.elem[i].rxlev < min_rxlev  )
        {
          if( min_index < EM_MAX_STRONG_CARRIER )
          {
            emeas->strg.refidx[min_index] = i;
          }
          else
          {
            TRACE_ASSERT( min_index < EM_MAX_STRONG_CARRIER );
          }
        }
      }
    }
  }

  return( emeas->strg.number );
} /* meas_em_extrct_strgst() */

/*
+------------------------------------------------------------------------------
| Function    : meas_em_extrct_strgst_with_bsic
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE meas_em_extrct_strgst_with_bsic ( void )
{
  UBYTE                i, j;      /* used for couting */

  TRACE_FUNCTION( "meas_em_extrct_strgst_with_bsic" );

  meas_em_extrct_strgst( );

  for( i = 0, j = 0; i < emeas->strg.number; i++ )
  {

    if(
        emeas->rslt.elem[emeas->strg.refidx[i]].bsic      NEQ RRGRR_INVALID_BSIC

        AND

        ((0x01 << ((emeas->rslt.elem[emeas->strg.refidx[i]].bsic & BSIC_NCC_MASK) >> 3)) &
        emeas->xmeas_set.param->em1.param.ncc_permitted ) NEQ 0
      )
    {
      emeas->strg.refidx[j] = emeas->strg.refidx[i];
      j++;
    }
  }

  emeas->strg.number = j;

  return( emeas->strg.number );
} /* meas_em_extrct_strgst_with_bsic() */


/*
+------------------------------------------------------------------------------
| Function    : meas_em_store_rslt
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL meas_em_store_rslt ( T_RRGRR_EXT_MEAS_CNF *rrgrr_em_cnf )
{
  UBYTE i;              /* used for counting */
  BOOL  result = FALSE;

  TRACE_FUNCTION( "meas_em_store_rslt" );

  /* check call reference */
  if( rrgrr_em_cnf->call_ref EQ emeas->xmeas_set.ident - 1 )
  {
    if( rrgrr_em_cnf->xmeas_cause EQ EXT_MEAS_OK )
    {
      T_xmeas_res *em_res;

      result             = TRUE;
      emeas->rslt.number = 0;

      for( i = 0; i < rrgrr_em_cnf->c_xmeas_res; i++ )
      {
        em_res = &rrgrr_em_cnf->xmeas_res[i];
      
        if( em_res->arfcn_idx.idx                                        < 
            emeas->xmeas_set.param->em1.list.number                         AND
            em_res->arfcn_idx.arfcn                                      EQ 
            emeas->xmeas_set.param->em1.list.freq[em_res->arfcn_idx.idx]        )
        {
          emeas->rslt.elem[emeas->rslt.number].refidx = em_res->arfcn_idx.idx;
          emeas->rslt.elem[emeas->rslt.number].rxlev  = em_res->rxlev;
          emeas->rslt.elem[emeas->rslt.number].bsic   = em_res->bsic;

          emeas->rslt.number++;
        }
        else
        {
          TRACE_ERROR( "meas_em_store_rslt: mismatch of ARFCNs" );
        }
      }
    }
    else
    {
      TRACE_EVENT_P1( "meas_em_store_rslt: unexpected cause %d",
                      rrgrr_em_cnf->xmeas_cause  );
    }
  }
  else
  {
    TRACE_EVENT_P2( "meas_em_store_rslt: mismatch of call references %d %d",
                    rrgrr_em_cnf->call_ref, emeas->xmeas_set.ident - 1 );
  }

  return( result );
} /* meas_em_store_rslt() */



/*
+------------------------------------------------------------------------------
| Function    : meas_em_req
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_em_req ( void )
{
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "meas_em_req" );

  {
    PALLOC( em_req, RRGRR_EXT_MEAS_REQ );

    em_req->c_arfcn_idx = 0;
    
    for( i = 0; i < emeas->xmeas_set.param->em1.list.number; i++ )
    {
      if( IS_NOT_FLAGGED( emeas->xmeas_set.param->em1.list.freq[i],
                          EM_VLDTY_ARFCN_MASK,
                          EM_NON_VLD_ARFCN ) )
      {
        em_req->arfcn_idx[em_req->c_arfcn_idx].idx   = i;
        em_req->arfcn_idx[em_req->c_arfcn_idx].arfcn =
                                      emeas->xmeas_set.param->em1.list.freq[i];
        em_req->c_arfcn_idx++;
      }
    }
    
#ifdef _SIMULATION_
    for( i = em_req->c_arfcn_idx; i < RRGRR_MAX_ARFCN_EXT_MEAS; i++ )
    {
      em_req->arfcn_idx[i].arfcn = RRGRR_INVALID_ARFCN;
      em_req->arfcn_idx[i].idx   = RRGRR_INVALID_IDX;
    }
#endif /* #ifdef _SIMULATION_ */

    em_req->call_ref      = emeas->xmeas_set.ident;
    em_req->report_type   = emeas->xmeas_set.param->em1.param.reporting_type;
    em_req->ncc_permitted = emeas->xmeas_set.param->em1.param.ncc_permitted;

    PSEND( hCommRR, em_req );
  }

  emeas->xmeas_set.ident++;

} /* meas_em_req() */


/*
+------------------------------------------------------------------------------
| Function    : meas_em_is_meas_reporting
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL meas_em_is_meas_reporting ( void )
{
  TRACE_FUNCTION( "meas_em_is_meas_reporting" );
  
  return( emeas->xmeas_set.param                NEQ NULL            AND
          emeas->xmeas_set.param->em_order_type EQ  EM_EM1          AND
          GET_STATE( MEAS_EM )                  NEQ MEAS_EM_SUSPEND     );
} /* meas_em_is_meas_reporting() */


/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : meas_im_new_value
+------------------------------------------------------------------------------
| Description : This function is used to process new interference measurement 
|               samples send by Layer 1 measured in packet transfer or packet
|               transfer mode.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_new_value ( T_MPHP_INT_MEAS_IND *mphp_int_meas_ind,
                                UBYTE                state )
{ 
  T_MEAS_IM_FILTER *filter;    /* filter to be updated      */
  UBYTE             rx_lev[8];
  UBYTE             dummy;     /* dummy parameter, not used */

  TRACE_FUNCTION( "meas_im_new_value" );

  meas_handle_int_meas_values ( mphp_int_meas_ind, rx_lev );

  if( imeas->carrier_id NEQ mphp_int_meas_ind->carrier_id )
  {
    BOOL valid;

    if( state EQ MEAS_IDLE )
    {
      valid = meas_im_get_idle_freq( mphp_int_meas_ind->carrier_id,
                                     &imeas->carrier,
                                     &dummy );
    }
    else
    {
      valid = meas_im_get_trns_freq( &imeas->carrier );
    }

    if( valid EQ TRUE )
    {
      imeas->carrier_id = ( USHORT )mphp_int_meas_ind->carrier_id;
    }
    else
    {
      TRACE_ERROR( "FATAL ERROR 1 in meas_im_new_value" );
      return;
    }
  }

  filter = meas_im_get_filter( &imeas->carrier );

  if( filter EQ NULL )
  {
    filter = meas_im_get_unused_filter( );

    if( filter EQ NULL )
    {
      filter = meas_im_get_oldest_filter( );

      if( filter EQ NULL )
      {
        TRACE_ERROR( "FATAL ERROR 2 in meas_im_new_value" );
        return;
      }
      else
      {
        meas_im_restart_filter( filter );
      }
    }

    filter->carrier = imeas->carrier;
  }

  meas_im_update_filter( filter, rx_lev, state );

  if( state EQ MEAS_IDLE )
  {
    meas_im_start_idle( );
  }

  meas_im_cgrlc_int_level_req( FALSE );

} /* meas_im_new_value() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_cgrlc_int_level_req 
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_cgrlc_int_level_req ( BOOL init )
{ 
  PALLOC( int_level_req, CGRLC_INT_LEVEL_REQ );

  TRACE_FUNCTION( "meas_im_cgrlc_int_level_req" );

  /*
   *      |            | primitive content | primitive shall
   * init | v_ilev_abs |      changed      |     be sent
   * -----+------------+-------------------+----------------
   *   0  |      0     |         X         |        1       
   *   0  |      0     |         X         |        1       
   *   0  |      1     |         0         |        0        
   *   0  |      1     |         1         |        1        
   *   1  |      0     |         X         |        1        
   *   1  |      0     |         X         |        1        
   *   1  |      1     |         0         |        0        
   *   1  |      1     |         1         |        1         
   */

  memset( int_level_req->ilev, CGRLC_RXLEV_NONE, CGRLC_MAX_TIMESLOTS );

  if( init EQ FALSE AND grr_data->tc.v_freq_set EQ TRUE )
  {
    T_p_frequency_par freq_par;
    T_MEAS_IM_CARRIER ma;
    T_ilev_abs        ilev;

    grr_set_freq_par( &freq_par );
    meas_im_set_carrier( &ma, &freq_par );
    meas_im_get_abs_i_level( &ma, &ilev );

    if( ilev.v_ilevabs0 EQ TRUE ) int_level_req->ilev[0] = ilev.ilevabs0; 
    if( ilev.v_ilevabs1 EQ TRUE ) int_level_req->ilev[1] = ilev.ilevabs1; 
    if( ilev.v_ilevabs2 EQ TRUE ) int_level_req->ilev[2] = ilev.ilevabs2; 
    if( ilev.v_ilevabs3 EQ TRUE ) int_level_req->ilev[3] = ilev.ilevabs3; 
    if( ilev.v_ilevabs4 EQ TRUE ) int_level_req->ilev[4] = ilev.ilevabs4; 
    if( ilev.v_ilevabs5 EQ TRUE ) int_level_req->ilev[5] = ilev.ilevabs5; 
    if( ilev.v_ilevabs6 EQ TRUE ) int_level_req->ilev[6] = ilev.ilevabs6; 
    if( ilev.v_ilevabs7 EQ TRUE ) int_level_req->ilev[7] = ilev.ilevabs7; 
  }

  PSEND( hCommGRLC, int_level_req );

} /* meas_im_cgrlc_int_level_req() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_stop_trns
+------------------------------------------------------------------------------
| Description : This function stops the interference measurements in packet
|               transfer mode.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_stop_trns ( void )
{
  TRACE_FUNCTION( "meas_im_stop_trns" );

  imeas->carrier_id = IDX_USHORT_INVALID;
} /* meas_im_stop_trns() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_stop_idle
+------------------------------------------------------------------------------
| Description : This function stops the interference measurements in packet
|               idle mode.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_stop_idle ( void )
{
  TRACE_FUNCTION( "meas_im_stop_idle" );

  {
    PALLOC( mphp_int_meas_stop_req, MPHP_INT_MEAS_STOP_REQ );
    PSEND( hCommL1, mphp_int_meas_stop_req );
  }

  imeas->carrier_id = IDX_USHORT_INVALID;
} /* meas_im_stop_idle() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_prepare_trns
+------------------------------------------------------------------------------
| Description : This function prepares the interference measurements in packet
|               transfer mode.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_prepare_trns ( void )
{
  TRACE_FUNCTION( "meas_im_prepare_trns" );
} /* meas_im_prepare_trns() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_prepare_idle
+------------------------------------------------------------------------------
| Description : This function prepares the interference measurements in packet
|               idle mode.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_prepare_idle ( void )
{
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "meas_im_prepare_idle" );

  /* process the carrier indicated in the parameter EXT_FREQUENCY_LIST */
  #if ( ! ( CHN_ID_EM_BASE < MAX_IM_IDLE_CHANNELS ) )
  #error CHN_ID_EM_BASE < MAX_IM_IDLE_CHANNELS required
  #endif

  if( emeas->xmeas_set.param                           NEQ NULL       AND
      emeas->xmeas_set.param->em1.param.reporting_type EQ  REP_TYPE_3 AND
      emeas->xmeas_set.param->em1.param.int_frequency  NEQ NOT_SET        )
  {
    imeas->idle_chan.ident[CHN_ID_EM_BASE] = 
                               emeas->xmeas_set.param->em1.param.int_frequency;
  }
  else
  {
    imeas->idle_chan.ident[CHN_ID_EM_BASE] = CHN_ID_INVALID;
  }

  /* process the monitored PCCCH */
  #if ( ! ( CHN_ID_PCCCH_BASE < MAX_IM_IDLE_CHANNELS ) )
  #error CHN_ID_PCCCH_BASE < MAX_IM_IDLE_CHANNELS required
  #endif

  if( imeas->v_cs_meas_active EQ  TRUE AND
      psc_db->paging_group.kc NEQ 0        )
  {
    UBYTE pccch_group;

    pccch_group = ( UBYTE )( grr_imsi_mod( ) % psc_db->paging_group.kc );
  
    if( pccch_group > psc_db->paging_group.kc - 1 )
    {
      TRACE_ERROR( "pccch_group > psc_db->paging_group.kc - 1 in meas_im_prepare_idle" );
      imeas->idle_chan.ident[CHN_ID_PCCCH_BASE] = CHN_ID_INVALID;
    }
    else
    {
      imeas->idle_chan.ident[CHN_ID_PCCCH_BASE] = pccch_group;
    }
  }
  else
  {
    imeas->idle_chan.ident[CHN_ID_PCCCH_BASE] = CHN_ID_INVALID;
  }

  /* process the carriers indicated by the parameter INT_MEAS_CHANNEL_LIST */
  i = CHN_ID_IM_BASE;

  while( i                      < MAX_IM_IDLE_CHANNELS                      AND
         ( i - CHN_ID_IM_BASE ) < psc_db->number_of_valid_int_meas_channels     )
  {
    imeas->idle_chan.ident[i] = i - CHN_ID_IM_BASE;
    i++;
  }

  while( i < MAX_IM_IDLE_CHANNELS )
  {
    imeas->idle_chan.ident[i] = CHN_ID_INVALID;
    i++;
  }

  /* let's start measurements with the first entry in the idle channel list */
  imeas->idle_chan.index = 0;
} /* meas_im_prepare_idle() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_timer_elapsed
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_timer_elapsed ( void )
{
  UBYTE i, j; /* used for counting */
  LONG  new_remain_time = TIME_INVALID;
  LONG* old_remain_time;

  TRACE_FUNCTION( "meas_im_timer_elapsed" );

  for( i = 0; i < MAX_IM_CHANNELS; i++ )
  {
    for( j = 0; j < CGRLC_MAX_TIMESLOTS; j++ )
    {
      old_remain_time = &imeas->filter[i].i_level[j].remain_time;

      if( *old_remain_time < 0 )
      {
        TRACE_ERROR( "unexpected time value in meas_im_timer_elapsed" );
      }
      if( *old_remain_time EQ 0 )
      {
        *old_remain_time = TIME_INVALID;
      }
      else if ( *old_remain_time NEQ TIME_INVALID    AND
                *old_remain_time <   new_remain_time     )
      {
        new_remain_time = *old_remain_time;
      }
    }
  }
    
  if( new_remain_time NEQ TIME_INVALID )
  {
    meas_im_timer_reorg( 0, new_remain_time );
  }
} /* meas_im_timer_elapsed() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_state_changed
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_state_changed ( UBYTE state )
{
  UBYTE i, j; /* used for counting */

  TRACE_FUNCTION( "meas_im_state_changed" );

  /*
   * the three if-statements below are just used to skip 
   * the following for-loops in all impossible cases
   */ 
  if( grr_data->meas_im.mode EQ IM_MODE_NONE )
  {
    return;
  }
  
  if( ( state                  EQ MEAS_IDLE    OR 
        state                  EQ MEAS_ACCESS      )
      AND 
      ( grr_data->meas_im.mode EQ IM_MODE_TRANSFER ) )
  {
    return;
  }

  if( state                  EQ MEAS_TRANSFER AND 
      grr_data->meas_im.mode EQ IM_MODE_IDLE      )
  {
    return;
  }

  for( i = 0; i < MAX_IM_CHANNELS; i++ )
  {
    for( j = 0; j < CGRLC_MAX_TIMESLOTS; j++ )
    {
      if( ( ( state EQ MEAS_IDLE OR state EQ MEAS_ACCESS )   AND
            IS_FLAGGED( imeas->filter[i].i_level[j].m_gamma, 
                        IM_I_MASK_IDL,  IM_I_IDL  )              )
            OR
          ( ( state EQ MEAS_TRANSFER )                       AND
            IS_FLAGGED( imeas->filter[i].i_level[j].m_gamma,
                        IM_I_MASK_TRNS, IM_I_TRNS )              ) )
      {
        if( imeas->filter[i].i_level[j].remain_time EQ TIME_INVALID )
        {
          meas_im_restart_gamma( &imeas->filter[i], j );
        }
      }
    }
  }
} /* meas_im_state_changed() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_start_idle
+------------------------------------------------------------------------------
| Description : This function starts the interference measurements in packet
|               idle mode.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL meas_im_start_idle ( void )
{
  T_MEAS_IM_IDLE_CHN* chn         = &imeas->idle_chan;
  UBYTE               ts_mask;
  UBYTE               max_checks  = MAX_IM_IDLE_CHANNELS;
  BOOL                is_started  = FALSE;

  TRACE_FUNCTION( "meas_im_start_idle" );

  while( chn->ident[chn->index] EQ CHN_ID_INVALID AND
         max_checks             >  0                  )
  {
    chn->index = ((chn->index < MAX_IM_IDLE_CHANNELS - 1) ? chn->index + 1 : 0);
    max_checks--;
  }

  if( max_checks > 0 )
  {
    PALLOC( mphp_int_meas_req, MPHP_INT_MEAS_REQ );

    meas_im_get_idle_freq( chn->index, &imeas->carrier, &ts_mask );
    meas_im_set_freq_par( &mphp_int_meas_req->p_frequency_par, &imeas->carrier );

    imeas->carrier_id             =          chn->index;
    mphp_int_meas_req->carrier_id = ( UBYTE )chn->index;

    mphp_int_meas_req->ts_mask    = ts_mask;
    mphp_int_meas_req->m_class    = grr_get_gprs_ms_class( );
          
    PSEND( hCommL1, mphp_int_meas_req );

    is_started = TRUE;

    if( chn->index < MAX_IM_IDLE_CHANNELS )
    {
      chn->index++;
    }
    else
    {
      chn->index = 0;
    }
  }

  return( is_started );
} /* meas_im_start_idle() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_set_freq_par
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
LOCAL void meas_im_set_freq_par ( T_p_frequency_par *freq_par, 
                                  T_MEAS_IM_CARRIER *carrier )
{
  TRACE_FUNCTION( "meas_im_set_freq_par" );

  if( carrier->hopping )
  {
    UBYTE  i = 0;
    UBYTE  bit_shift;
    USHORT array_idx;
    USHORT arfcn;

    freq_par->p_chan_sel.hopping       = MPHP_INT_MEAS_HOPPING;
    freq_par->p_chan_sel.p_rf_ch.arfcn = 
      ( ( USHORT )carrier->alloc.hop_param.hsn << 8 ) |
        ( USHORT )carrier->alloc.hop_param.maio;

    for ( array_idx = 0; 
          array_idx < MAX_IM_HOP_ARFCN_BIT_FIELD_SIZE;
          array_idx++ )
    {
      for( bit_shift = 0; bit_shift < 8 AND i < MPHP_NUMC_MA; bit_shift++ )
      {
        if( ( carrier->alloc.hop_param.arfcn_bit_field[array_idx] &
                                                ( 0x01 << bit_shift ) ) NEQ 0 )
        {
          arfcn = ( array_idx * 8 ) + bit_shift;

          switch( std )
          {
            case STD_1900: arfcn += LOW_CHANNEL_1900; break;
            case STD_1800: arfcn += LOW_CHANNEL_1800; break;
            default      :                            break;
          }

          freq_par->p_freq_list.p_rf_chan_no.p_radio_freq[i] = arfcn;
          i++;
        }
      }
    }

    freq_par->p_freq_list.p_rf_chan_cnt = i;
  }
  else
  {
    freq_par->p_chan_sel.hopping       = MPHP_INT_MEAS_STATIC;
    freq_par->p_chan_sel.p_rf_ch.arfcn =
      grr_g23_arfcn_to_l1( carrier->alloc.arfcn );
  }
} /* meas_im_set_freq_par() */

/*
+------------------------------------------------------------------------------
| Function    : meas_im_start_trns
+------------------------------------------------------------------------------
| Description : This function starts the interference measurements in packet
|               transfer mode.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_im_start_trns ( void )
{
  TRACE_FUNCTION( "meas_im_start_trns" );
} /* meas_im_start_trns() */

/*
+------------------------------------------------------------------------------
| Function    : meas_em_prcs_meas_order
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : d_meas_order - Pointer to packet measurement order message
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE meas_em_prcs_meas_order ( T_D_MEAS_ORDER *d_meas_order,
                                       T_NC_ORDER     *ctrl_order )
{
  BOOL  is_pmo_ok;
  UBYTE i;
  UBYTE ret_value = PMO_NONE_VLD;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  UBYTE psi3_cm = grr_get_psi3_cm();
#endif

  TRACE_FUNCTION( "meas_em_prcs_meas_order" );

  TRACE_EVENT_P2( "D_MEAS_ORDER received idx = %d, cnt = %d",
                  d_meas_order->pmo_index, d_meas_order->pmo_cnt );


  /* process the PMO_COUNT and PMO_INDEX parameter */
  if( ( emeas->pmo.prm.count NEQ NOT_SET               AND
        emeas->pmo.prm.count NEQ d_meas_order->pmo_cnt     ) OR
        d_meas_order->pmo_cnt < d_meas_order->pmo_index         )
  {
    TRACE_ERROR( "D_MEAS_ORDER: parameter inconsistent" );
    
    meas_em_init_pmo_seq( );

    return( ret_value );
  }

#if defined (REL99) AND defined (TI_PS_FF_EMR)
   /* Set to ENH params not present by default  */
  if( emeas->pmo.prm.count EQ NOT_SET )
  {
    emeas->pmo.v_enh_meas_param_pmo = FALSE;
    nc_freq_list_pres = FALSE;
    nc_freq_list_valid = FALSE;
    use_ba_gprs = TRUE;
  }
#endif

  emeas->pmo.prm.instances[d_meas_order->pmo_index] = TRUE;
  emeas->pmo.prm.count                              = d_meas_order->pmo_cnt;

  /* process the EXT measurement parameter */
  if( d_meas_order->v_xmeas_par EQ TRUE )
  {
    grr_prcs_xmeas_struct 
      ( &emeas->pmo.extd, &d_meas_order->xmeas_par,
        TRUE, d_meas_order->pmo_index, 
        &emeas->pmo.prm.idx[d_meas_order->pmo_index].start,
        &emeas->pmo.prm.idx[d_meas_order->pmo_index].stop );
  }

  if( d_meas_order->v_nc_meas_par_list EQ TRUE )
  {
    /*
     * process the NC measurement parameter
     *
     * maybe more than one instance of PMO, store data in temporary location
     */
    emeas->v_pmo_nc_ncmeas_param = TRUE;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
    /* Do not get back to BA-GPRS list since NC_FREQ_LIST is present */
    if(d_meas_order->nc_meas_par_list.v_nc_freq_list)
    {
      nc_freq_list_pres = TRUE;
    }
#endif

    grr_prcs_nc_param_struct ( &emeas->pmo.nc.ncmeas.param, 
                               &d_meas_order->nc_meas_par_list.nc_meas_par,
                               d_meas_order->pmo_index );

   
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  /* Check for presence of ENH parameters in PMO. If present, then PSI3_CM or 
     BA_IND will be checked depending on the presence or absence of PBCCH for
     consistency with the BA list existing currently. If consistent, then only
     the PMO NC frequency list is considered. */
    if( d_meas_order->release_98_str_d_meas_order.v_release_99_str_d_meas_order AND 
      (d_meas_order->release_98_str_d_meas_order.release_99_str_d_meas_order.v_enh_meas_param_pmo EQ TRUE) )
    {
      if(grr_is_pbcch_present() AND
        d_meas_order->release_98_str_d_meas_order.release_99_str_d_meas_order.enh_meas_param_pmo.ba_psi3_str.v_psi3_cm)
      {
        if(d_meas_order->release_98_str_d_meas_order.release_99_str_d_meas_order.enh_meas_param_pmo.ba_psi3_str.psi3_cm EQ psi3_cm)
        {
          nc_freq_list_valid = TRUE;
        }
      }
      else if(!grr_is_pbcch_present() AND
        d_meas_order->release_98_str_d_meas_order.release_99_str_d_meas_order.enh_meas_param_pmo.ba_psi3_str.v_ba_ind_used)
      {
        if(d_meas_order->release_98_str_d_meas_order.release_99_str_d_meas_order.enh_meas_param_pmo.ba_psi3_str.ba_ind_used EQ 
          psc_db->ba_ind)
        {
          nc_freq_list_valid = TRUE;
        }
      }
    }
    else
    /* PMO is not checked for BA_IND or PSI3_CM consistency since ENH params
       are absent */
    {
      nc_freq_list_valid = TRUE;
    }

    if(nc_freq_list_valid EQ TRUE)
    {
#endif

      /*
       * process NC_FREQUENCY_LIST
       *
       * maybe more than one instance of PMO, store data in temporary location
       */

    emeas->v_pmo_nc_ncmeas_list_rfreq =
         grr_prcs_nc_freq_list ( &emeas->pmo.nc.ncmeas.list,
                                 &emeas->pmo.nc.rfreq,
                                 d_meas_order->nc_meas_par_list.v_nc_freq_list,
                                 &d_meas_order->nc_meas_par_list.nc_freq_list,
                                 INFO_TYPE_PMO,
                                 d_meas_order->pmo_index );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
    }
  }

  if( d_meas_order->release_98_str_d_meas_order.v_release_99_str_d_meas_order AND 
    (d_meas_order->release_98_str_d_meas_order.release_99_str_d_meas_order.v_enh_meas_param_pmo EQ TRUE) )
  {
    emeas->pmo.v_enh_meas_param_pmo = TRUE;
    /*
    * process the ENH measurement parameter
    *
    * maybe more than one instance of PMO, store data in temporary location
    */
    grr_prcs_enh_param_pmo(&emeas->pmo.enh, &d_meas_order->release_98_str_d_meas_order.release_99_str_d_meas_order.enh_meas_param_pmo, 
      d_meas_order->pmo_index, &emeas->pmo.nc.pmo_ind);
#endif

  }

  for( i  = 0, is_pmo_ok = TRUE;
       i <= emeas->pmo.prm.count AND is_pmo_ok EQ TRUE; 
       i++ )
  {
    if( emeas->pmo.prm.instances[i] EQ FALSE )
    {
      is_pmo_ok = FALSE;
    }
  }

  if( is_pmo_ok )
  {
    /* process EXT measurement service */
    grr_sort_ext_lst_freq ( &emeas->pmo.extd.em1.list, 
                            MAX_NR_OF_INSTANCES_OF_PMO,
                            &emeas->pmo.prm.idx[0] );

    memcpy( &psc_db->ext_pmo, &emeas->pmo.extd, sizeof( psc_db->ext_pmo ) );

    if(    emeas->pmo.extd.em_order_type            NEQ EM_EMPTY     AND 
        !( emeas->pmo.extd.em_order_type            EQ  EM_EM1       AND 
           emeas->pmo.extd.em1.param.reporting_type EQ  REP_TYPE_RES     ) )
    {
      ret_value |= PMO_EXT_VLD;
    }

    /* process NC measurement service */
    
    /* 
     * Before copying the NC parameter store the old value
     * for the network control order
     */
    *ctrl_order = psc_db->nc_ms.ncmeas.param.ctrl_order;

    grr_prcs_nc_param_final ( &psc_db->nc_ms.ncmeas.param,
                              &emeas->v_pmo_nc_ncmeas_param,
                              &emeas->pmo.nc.ncmeas.param );
#if defined (REL99) AND defined (TI_PS_FF_EMR)

    if( (nc_freq_list_pres EQ TRUE) AND
      (psc_db->nc_ms.ncmeas.param.ctrl_order NEQ NC_RESET) )
    {
      use_ba_gprs = FALSE;
    }

    if(rr_get_support_for_emr() AND emeas->pmo.v_enh_meas_param_pmo EQ TRUE)
    {
    /* The ENH parameters that are temporarily stored are moved to final 
      destination after consistent set of PMO is received */
      memcpy(&(psc_db->enh_ms), &(emeas->pmo.enh), sizeof(T_GRR_ENH_PARA));
      psc_db->nc_ms.pmo_ind = emeas->pmo.nc.pmo_ind;
    }
    else
    {
      memset(&(psc_db->enh_ms), 0, sizeof(T_GRR_ENH_PARA));
      psc_db->enh_ms.rept_type = REPORT_TYPE_REP;
      emeas->pmo.v_enh_meas_param_pmo = FALSE;
    }

#endif

    /*
     * At cell re-selection the NC measurement parameters valid for the mobile
     * station in the new cell are brought from the old cell if received in a
     * PACKET MEASUREMENT ORDER message. Storing is done in advance even in
     * case no cell re-selection took place at this time.
     */
    memcpy( &pcr_db->nc_ms.ncmeas.param, &psc_db->nc_ms.ncmeas.param, 
            sizeof( T_NC_PARAM ) );

    pcr_db->nc_ms.ncmeas.param.chng_mrk.curr = 0;
    pcr_db->nc_ms.ncmeas.param.chng_mrk.prev = 0;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
    if(nc_freq_list_valid EQ TRUE)
    {
      grr_prcs_nc_freq_final ( &psc_db->nc_ms.ncmeas.list,
                               &psc_db->nc_ms.rfreq,
                               &emeas->v_pmo_nc_ncmeas_list_rfreq,
                               &emeas->pmo.nc.ncmeas.list,
                               &emeas->pmo.nc.rfreq );
    }

#else
    grr_prcs_nc_freq_final ( &psc_db->nc_ms.ncmeas.list,
                             &psc_db->nc_ms.rfreq,
                             &emeas->v_pmo_nc_ncmeas_list_rfreq,
                             &emeas->pmo.nc.ncmeas.list,
                             &emeas->pmo.nc.rfreq );
#endif

    if( emeas->pmo.nc.ncmeas.param.ctrl_order NEQ NC_EMPTY )
    {
      ret_value |= PMO_NC_VLD;
    }

    meas_em_init_pmo_seq( );
  }

  return( ret_value );
} /* meas_em_prcs_meas_order() */

/*
+------------------------------------------------------------------------------
| Function    : meas_em_valid_pmo
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_em_valid_pmo ( void )
{
  TRACE_FUNCTION( "meas_em_valid_pmo" );

  switch( psc_db->ext_pmo.em_order_type )
  {
    case( EM_RESET ):
      meas_em_valid_psi5( );
      break;

    default:
      if( psc_db->ext_pmo.em_order_type EQ EM_EM1 )
      {
        emeas->xmeas_set.param = &psc_db->ext_pmo;
      }
      else
      {
        emeas->xmeas_set.param = NULL;
      }
      break;
  }
} /* meas_em_valid_pmo() */

/*
+------------------------------------------------------------------------------
| Function    : meas_em_valid_psi5
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_em_valid_psi5 ( void )
{
  TRACE_FUNCTION( "meas_em_valid_psi5" );

  if( psc_db->ext_psi5.em_order_type EQ EM_EM1 )
  {
    emeas->xmeas_set.param = &psc_db->ext_psi5;
  }
  else
  {
    emeas->xmeas_set.param = NULL;
  }
} /* meas_em_valid_psi5() */


/*
+------------------------------------------------------------------------------
| Function    : meas_em_is_valid_pmo_present
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL meas_em_is_valid_pmo_present ( void )
{
  TRACE_FUNCTION( "meas_em_is_valid_pmo_present" );

  return( psc_db->ext_pmo.em_order_type NEQ EM_EMPTY AND 
          psc_db->ext_pmo.em_order_type NEQ EM_RESET     );
} /* meas_em_is_valid_pmo_present() */


/*
+------------------------------------------------------------------------------
| Function    : meas_em_process_t3178
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_em_process_t3178 ( void )
{
  T_TIME reporting_period;
  T_TIME time_to_go;
  BOOL   tmr_3178_rng_le_rp = FALSE;

  TRACE_FUNCTION( "meas_em_process_t3178" );

  if( meas_em_is_meas_reporting( ) )
  {
    reporting_period = 
          GET_EM_RPT_PRD( emeas->xmeas_set.param->em1.param.reporting_period );

    time_to_go = grr_t_status( T3178 );
    
    if( time_to_go > 0 )
    {
      if( time_to_go > reporting_period )
      {
        meas_em_stop_t3178( );
      }
      else
      {
        tmr_3178_rng_le_rp = TRUE;
      }
    }

    if( reporting_period   NEQ 0     AND 
        tmr_3178_rng_le_rp EQ  FALSE     )
    {
      meas_em_start_t3178( reporting_period );
    }
  }
  else
  {
    meas_em_stop_t3178( );
  }
} /* meas_em_process_t3178() */

/*
+------------------------------------------------------------------------------
| Function    : meas_em_start_t3178
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_em_start_t3178 ( T_TIME time )
{
  TRACE_FUNCTION( "meas_em_start_t3178" );

  vsi_t_start( GRR_handle, T3178, time );

  TRACE_EVENT_P1( "T3178: %d", time );

} /* meas_em_start_t3178() */


/*
+------------------------------------------------------------------------------
| Function    : meas_em_stop_t3178
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL  void meas_em_stop_t3178 ( void )
{
  TRACE_FUNCTION( "meas_em_stop_t3178" );
  
  if( grr_t_status( T3178 ) > 0 )
  {
    vsi_t_stop( GRR_handle, T3178 );

    TRACE_EVENT( "T3178 stopped" );
  }
} /* meas_em_stop_t3178() */

                                     
/*
+------------------------------------------------------------------------------
| Function    : meas_em_stop_req
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_em_stop_req ( void )
{
  TRACE_FUNCTION( "meas_em_stop_req" );
  
  {
    PALLOC( rrgrr_em_stop_req, RRGRR_EXT_MEAS_STOP_REQ  );
    PSEND( hCommRR, rrgrr_em_stop_req );  
  }
} /* meas_em_stop_req() */

/*
+------------------------------------------------------------------------------
| Function    : meas_em_start
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_em_start ( void )
{

  TRACE_FUNCTION( "meas_em_start" );

  meas_im_delete_em_rslt( );  
  meas_em_req( );  
  meas_em_process_t3178( );


} /* meas_em_start() */


/*
+------------------------------------------------------------------------------
| Function    : meas_em_send_meas_rpt
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL meas_em_send_meas_rpt ( BOOL perform_init )
{
  BOOL            rpt_snd;
  T_U_MEAS_REPORT meas_rpt;

  TRACE_FUNCTION( "meas_em_send_meas_rpt" );

  if( perform_init EQ TRUE )
  {
    emeas->pmr_snd_ref = 0;

    if     ( emeas->xmeas_set.param->em1.param.reporting_type EQ REP_TYPE_1 )
    {
      meas_em_extrct_strgst( );
    }
    else if( emeas->xmeas_set.param->em1.param.reporting_type EQ REP_TYPE_2 )
    {
      meas_em_extrct_strgst_with_bsic( );
    }

  }

  if( meas_em_build_meas_rpt( &meas_rpt ) NEQ 0    OR
      perform_init                        EQ  TRUE    )
  {
    sig_meas_ctrl_meas_report( &meas_rpt );

    rpt_snd = TRUE;
  }
  else
  {
    rpt_snd = FALSE;
  }

  return( rpt_snd );
} /* meas_em_send_meas_rpt() */


/*
+------------------------------------------------------------------------------
| Function    : meas_is_spgc_ccch_supported
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL meas_is_spgc_ccch_supported ( void )
{
  TRACE_FUNCTION( "meas_is_spgc_ccch_supported" );

  return
    ( psc_db->net_ctrl.spgc_ccch_supp.nw_supp EQ GMMRR_SPGC_CCCH_SUPP_YES AND
      psc_db->net_ctrl.spgc_ccch_supp.ms_supp EQ GMMRR_SPGC_CCCH_SUPP_YES     );
} /* meas_is_spgc_ccch_supported() */



/*
+------------------------------------------------------------------------------
| Function    : meas_handle_int_meas_values
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : mphp_int_meas_ind - pointer to primitive
|               rx_lev            - pointer on array of 8 byte,
|                                   where the results are stored
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_handle_int_meas_values
                      ( T_MPHP_INT_MEAS_IND *mphp_int_meas_ind, UBYTE *rx_lev )
{
  UBYTE             i;      /* used for counting */
  T_int_meas_rxlev *sample;

  TRACE_FUNCTION( "meas_handle_int_meas_values" );

  for ( i = 0; i < CGRLC_MAX_TIMESLOTS; i++ )
  {
    sample = &mphp_int_meas_ind->int_meas_rxlev[i];

    if( sample->rx_lev[0] EQ CGRLC_RXLEV_NONE )
    {
      if( sample->rx_lev[1] EQ CGRLC_RXLEV_NONE )
      {
        rx_lev[i] = CGRLC_RXLEV_NONE;
      }
      else
      {
        rx_lev[i] = sample->rx_lev[1];
      }
    }
    else
    {
      if( sample->rx_lev[1] EQ CGRLC_RXLEV_NONE )
      {
        rx_lev[i] = sample->rx_lev[0];
      }
      else
      {
        rx_lev[i] = MINIMUM( (signed char)( sample->rx_lev[0] ), 
                                          (signed char)( sample->rx_lev[1] ) );
      }
    }
  }

  grr_clip_rxlev ( rx_lev, rx_lev, CGRLC_MAX_TIMESLOTS );

#if !defined (NTRACE)

  if( grr_data->meas_im.n_im_trace & M_IM_TRACE_INPUT_VALUE )
  {
    USHORT rx_lev[8];

    rx_lev[0]  = ( USHORT )mphp_int_meas_ind->int_meas_rxlev[0].rx_lev[0] << 0;
    rx_lev[0] |= ( USHORT )mphp_int_meas_ind->int_meas_rxlev[0].rx_lev[1] << 8;
    rx_lev[1]  = ( USHORT )mphp_int_meas_ind->int_meas_rxlev[1].rx_lev[0] << 0;
    rx_lev[1] |= ( USHORT )mphp_int_meas_ind->int_meas_rxlev[1].rx_lev[1] << 8;
    rx_lev[2]  = ( USHORT )mphp_int_meas_ind->int_meas_rxlev[2].rx_lev[0] << 0;
    rx_lev[2] |= ( USHORT )mphp_int_meas_ind->int_meas_rxlev[2].rx_lev[1] << 8;
    rx_lev[3]  = ( USHORT )mphp_int_meas_ind->int_meas_rxlev[3].rx_lev[0] << 0;
    rx_lev[3] |= ( USHORT )mphp_int_meas_ind->int_meas_rxlev[3].rx_lev[1] << 8;
    rx_lev[4]  = ( USHORT )mphp_int_meas_ind->int_meas_rxlev[4].rx_lev[0] << 0;
    rx_lev[4] |= ( USHORT )mphp_int_meas_ind->int_meas_rxlev[4].rx_lev[1] << 8;
    rx_lev[5]  = ( USHORT )mphp_int_meas_ind->int_meas_rxlev[5].rx_lev[0] << 0;
    rx_lev[5] |= ( USHORT )mphp_int_meas_ind->int_meas_rxlev[5].rx_lev[1] << 8;
    rx_lev[6]  = ( USHORT )mphp_int_meas_ind->int_meas_rxlev[6].rx_lev[0] << 0;
    rx_lev[6] |= ( USHORT )mphp_int_meas_ind->int_meas_rxlev[6].rx_lev[1] << 8;
    rx_lev[7]  = ( USHORT )mphp_int_meas_ind->int_meas_rxlev[7].rx_lev[0] << 0;
    rx_lev[7] |= ( USHORT )mphp_int_meas_ind->int_meas_rxlev[7].rx_lev[1] << 8;

    TRACE_EVENT_P8( "meas_handle_int_meas_values: %04X %04X %04X %04X %04X %04X %04X %04X",
                    rx_lev[0], rx_lev[1], rx_lev[2], rx_lev[3], 
                    rx_lev[4], rx_lev[5], rx_lev[6], rx_lev[7] );
  }

#endif /* #if !defined (NTRACE) */

} /* meas_handle_int_meas_values() */

/*
+------------------------------------------------------------------------------
| Function    : meas_c_init
+------------------------------------------------------------------------------
| Description : This function initializes the parameters for deriving the
|               C value.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_c_init ( void )
{
  TRACE_FUNCTION( "meas_c_init" );

  grr_data->meas.c_filter.value = 0;
  grr_data->meas.c_filter.index = 0;
} /* meas_c_init() */

/*
+------------------------------------------------------------------------------
| Function    : meas_c_val_update_pim 
+------------------------------------------------------------------------------
| Description : ... 
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_c_val_update_pim ( UBYTE rx_lev )
{ 
  ULONG ff;       /* forgetting factor */  
  ULONG avgw_drx;
  ULONG c_value;
  UBYTE c_block;

  T_C_FILTER *c_filter = &grr_data->meas.c_filter;

  TRACE_FUNCTION( "meas_c_val_update_pim" );

  grr_clip_rxlev ( &c_block, &rx_lev, 1 );

  if( c_block NEQ CGRLC_RXLEV_NONE )
  {
    MEAS_C_INC_INDEX( c_filter->index );

    /* calculate the forgetting factor */
    avgw_drx = 
      ( (NORM_TAVGW_FAC * NORM_TAVGW_FAC *
         norm_pow[psc_db->g_pwr_par.t_avg_w]) / 6 ) / meas_im_get_drx_period_seconds( );

    ff = DRX_NORM_FACTOR / MINIMUM( NORM_TAVGW_FAC * c_filter->index,
                                    MAXIMUM( 5 * NORM_TAVGW_FAC, avgw_drx ) );

    /* update the filter */
    c_value = ( NORM_POW_MIN - ff ) * c_filter->value +
              MEAS_ACRCY * ff * c_block;

    c_filter->value = (T_C_VALUE)( M_ROUND_UP( c_value, NORM_POW_MIN ) );

#ifdef _SIMULATION_
    TRACE_EVENT_P2( "C-Value = %d, C-Index = %d", 
                    c_filter->value, c_filter->index );
#endif /* #ifdef _SIMULATION_ */

    grr_data->pwr_ctrl_valid_flags.v_c_value = TRUE;
  }
  else
  {
#ifdef _SIMULATION_
    TRACE_EVENT( "no valid RXLEV information in meas_c_val_update_pim" );
#endif /* #ifdef _SIMULATION_ */
  }
} /* meas_c_val_update_pim() */

/*
+------------------------------------------------------------------------------
| Function    : meas_handle_pwr_par
+------------------------------------------------------------------------------
| Description : The function meas_handle_pwr_par() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_handle_pwr_par ( void )
{ 
  MCAST( d_ctrl_pwr_ta, D_CTRL_PWR_TA ); /* T_D_CTRL_PWR_TA */

  TRACE_FUNCTION( "meas_handle_pwr_par" );
  
  /* process global power control parameter */
  if( d_ctrl_pwr_ta->v_g_pwr_par )
  {
    grr_store_g_pwr_par( &d_ctrl_pwr_ta->g_pwr_par );
  }
  
  /* process power control parameter */
  if( d_ctrl_pwr_ta->v_pwr_par )
  {
    grr_store_type_pwr_par( &d_ctrl_pwr_ta->pwr_par, FALSE );
  }
} /* meas_handle_pwr_par() */
