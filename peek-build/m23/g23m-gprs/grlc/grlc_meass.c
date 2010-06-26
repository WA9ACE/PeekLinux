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
|             MEAS of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_MEASS_C
#define GRLC_MEASS_C
#endif /* #ifndef GRLC_MEASS_C */

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
#include "grlc_meass.h"   
#include "grlc_measf.h"
#include "cl_rlcmac.h"

/*==== CONST ================================================================*/

#define MEAS_MAX_NUM_MEAN     1000   /* maximum numbers of element considered for    */
                                     /* calculating the mean                         */
/* maybe for later use */
/* 
#define M_ROUND_UP(x,meas_acrcy) (((x)>=0)?                                        \
                                  ((((x)%(meas_acrcy))< ((meas_acrcy)/ 2))?        \
                                   ( (x)/(meas_acrcy)   ):(((x)/(meas_acrcy))+1)): \
                                  ((((x)%(meas_acrcy))<=((meas_acrcy)/-2))?        \
                                   (((x)/(meas_acrcy))-1):( (x)/(meas_acrcy)   )))
*/

#define M_ROUND_UP(x,meas_acrcy)  ((((x)%(meas_acrcy))< ((meas_acrcy)/ 2))?        \
                                   ( (x)/(meas_acrcy)   ):(((x)/(meas_acrcy))+1))

#define MEAS_C_INC_INDEX(index) {                                        \
                                  if( index < (T_C_INDEX)(~0) ) index++; \
                                }

#define NORM_FFB_MAX   (ULONG)100000 /* normalised forgetting factor b maximum value */
#define NORM_FFC_MAX   (ULONG)500000 /* normalised forgetting factor c maximum value */
#define NORM_FFB_DIV   (ULONG)10     /* normalised forgetting factor b divisor value */
#define NORM_FFC_DIV   (ULONG)20     /* normalised forgetting factor c divisor value */

#define SV_ACRCY_DIV   (ULONG)10     /* signal variance accuracy divisor             */

#define C_VALUE_IDX_DEFAULT  0
#define C_VALUE_DEFAULT      0
#define SV_VALUE_DEFAULT     0
#define RXQUAL_VALUE_DEFAULT 0

#define SV_LEVEL_MAX            63
#define SV_LEVEL_STEP           ((T_SIGN_VAR_VALUE)(MEAS_ACRCY/4))
                                            /* dB2 step size of the signal   */
                                            /* variance                      */

/*==== LOCAL VARS ===========================================================*/

/*
 * normalised forgetting factor b used in context of C value calculation
 *
 * NORM_FFB_MAX * ( 1 / ( 6 * t_avg_t ) )
 */
LOCAL const ULONG norm_ffb[] = { NORM_FFB_MAX,
                                 70711, 50000, 35355, 25000, 17678, 12500,
                                  8839,  6250,  4419,  3125,  2210,  1563,
                                  1105,   781,   552,   391,   276,   195,
                                   138,    98,    69,    49,    35,    24,
                                    17 };

/*
 * normalised forgetting factor c used in context of C value calculation
 *
 * NORM_FFC_MAX * ( 1 / ( 12 * t_avg_t ) )
 */
LOCAL const ULONG norm_ffc[] = { NORM_FFC_MAX,
                                 353553, 250000, 176777, 125000,  88388,  62500,
                                  44194,  31250,  22097,  15625,  11049,   7813,
                                   5524,   3906,   2762,   1953,   1381,    977,
                                    691,    488,    345,    244,    173,    122,
                                     86 };

/*==== LOCAL TYPES===========================================================*/

#if !defined (NTRACE)

LOCAL void meas_int_trace_i_level( T_ilev *i_level );
  
#endif /* #if !defined (NTRACE) */

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : meas_grlc_init
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_grlc_init ( void )
{ 
  TRACE_FUNCTION( "meas_grlc_init" );

  /* initialization of C value management */
  meas_c_restart( );

  /* initialization of signal variance management */
  meas_sv_restart( );

  /* initialization of signal quality management */
  meas_sq_restart( );

} /* grlc_meas_init() */

/*
+------------------------------------------------------------------------------
| Function    : sig_rd_meas_qual_rpt_sent
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RD_MEAS_QUAL_RPT_SENT
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rd_meas_qual_rpt_sent ( void )
{ 
  TRACE_ISIG( "sig_rd_meas_qual_rpt_sent" );

  meas_sv_restart( );
  
  grlc_data->meas.sq_restart = TRUE;
  
} /* sig_rd_meas_qual_rpt_sent() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tm_meas_ptm
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TM_MEAS_PTM
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tm_meas_ptm ( void )
{ 
  TRACE_ISIG( "sig_tm_meas_ptm" );

  meas_sv_restart( );
  meas_sq_restart( );

} /* sig_tm_meas_ptm() */

/*
+------------------------------------------------------------------------------
| Function    : meas_grlc_c_get_value
+------------------------------------------------------------------------------
| Description : This function returns the current C value.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE meas_grlc_c_get_value ( void )
{
  UBYTE c_value;

  TRACE_FUNCTION( "meas_grlc_c_get_value" );

  if( grlc_data->meas.c_filter.index > 0 )
  {
    c_value = ( M_ROUND_UP( grlc_data->meas.c_filter.value, MEAS_ACRCY ) );
  }
  else
  {

#ifdef _SIMULATION_
    TRACE_ERROR( "meas_grlc_c_get_value: grlc_data->meas.c_filter.index EQ 0" );
#endif /* #ifdef _SIMULATION_ */

    c_value = C_VALUE_DEFAULT;
  }

  return( c_value );

} /* meas_grlc_c_get_value() */

/*
+------------------------------------------------------------------------------
| Function    : meas_grlc_c_get_c_value
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_grlc_c_get_c_value ( T_CGRLC_c_value *c_value )
{
  TRACE_FUNCTION( "meas_grlc_c_get_c_value" );

  c_value->c_acrcy = MEAS_ACRCY;
  c_value->c_lev   = grlc_data->meas.c_filter.value;
  c_value->c_idx   = grlc_data->meas.c_filter.index;

} /* meas_grlc_c_get_c_value() */

/*
+------------------------------------------------------------------------------
| Function    : meas_grlc_c_set_c_value
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_grlc_c_set_c_value ( T_CGRLC_c_value *c_value )
{
  TRACE_FUNCTION( "meas_grlc_c_set_c_value" );

  grlc_data->meas.c_filter.value = (c_value->c_lev / c_value->c_acrcy) * 
									MEAS_ACRCY;

  grlc_data->meas.c_filter.index = c_value->c_idx;

} /* meas_grlc_c_set_c_value() */

/*
+------------------------------------------------------------------------------
| Function    : meas_c_val_update_ptm 
+------------------------------------------------------------------------------
| Description : ... 
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_c_val_update_ptm ( T_MAC_PWR_CTRL_IND          *mac_pwr_ctrl_ind,
                                    T_CGRLC_glbl_pwr_ctrl_param *pwr_ctrl_param,
                                    T_CGRLC_freq_param          *freq_param )
{ 
  UBYTE       pb_rdc;       /* Pb reduction              */
  UBYTE       vld_smpl = 0; /* valid measurement samples */
 
  ULONG       ff_value;
  ULONG       c_value;
  
  T_C_FILTER *c_filter = &grlc_data->meas.c_filter;

  TRACE_FUNCTION( "meas_c_val_update_ptm" );

  if( pwr_ctrl_param->pc_meas_chan EQ  CGRLC_MEAS_CHAN_BCCH AND
      mac_pwr_ctrl_ind->bcch_level NEQ MAC_RXLEV_NONE           )
  {
    MEAS_C_INC_INDEX( c_filter->index );

    if( c_filter->index EQ 1 )
    {
      c_filter->value = MEAS_ACRCY * mac_pwr_ctrl_ind->bcch_level;
    }
    else
    {
      /* calculate the forgetting factor */
      ff_value = norm_ffb[pwr_ctrl_param->t_avg_t];

      /* update the filter */
      c_value = ( NORM_FFB_MAX - ff_value )       * 
                ( c_filter->value / NORM_FFB_DIV ) 
                +
                ff_value                          * 
                ( MEAS_ACRCY / NORM_FFB_DIV )     *
                mac_pwr_ctrl_ind->bcch_level;
    
      c_filter->value = (T_C_VALUE)(c_value / (NORM_FFB_MAX / NORM_FFB_DIV));
    }

#ifdef _SIMULATION_
    TRACE_EVENT_P2( "C-Value = %d, C-Index = %d", 
                    c_filter->value, c_filter->index );
#endif /* #ifdef _SIMULATION_ */
  }
  else if( pwr_ctrl_param->pc_meas_chan EQ CGRLC_MEAS_CHAN_PDCH )
  {
    vld_smpl = meas_c_calc_mean ( mac_pwr_ctrl_ind->burst_level,
                                  mac_pwr_ctrl_ind->radio_freq,
                                  &c_value, &pb_rdc, freq_param );

    if( vld_smpl )
    {
      MEAS_C_INC_INDEX( c_filter->index );

      /* consider the BTS output power reduction */
      c_value -= ( ( 2 * pwr_ctrl_param->pb ) *
                   ( MEAS_ACRCY * pb_rdc ) / MAC_BURST_PER_BLOCK );


      if( c_filter->index EQ 1 )
      {
        c_filter->value = (T_C_VALUE)c_value;
      }
      else
      {
        /* calculate the forgetting factor */
        ff_value = norm_ffc[pwr_ctrl_param->t_avg_t];

        /* update the filter */
        c_value = ( NORM_FFC_MAX - ff_value )      *
                  ( c_filter->value / NORM_FFC_DIV ) 
                  +
                  ff_value                         *
                  ( c_value / NORM_FFC_DIV );
    
        c_filter->value = (T_C_VALUE)(c_value / (NORM_FFC_MAX / NORM_FFC_DIV));
      }
    }
    else
    {
#ifdef _SIMULATION_
      TRACE_EVENT( "no valid RXLEV information in meas_c_val_update_ptm 1" );
#endif /* #ifdef _SIMULATION_ */
    }

#ifdef _SIMULATION_
      TRACE_EVENT_P2( "C-Value = %d, C-Index = %d", 
                      c_filter->value, c_filter->index );
#endif /* #ifdef _SIMULATION_ */
  }
  else
  {
#ifdef _SIMULATION_
    TRACE_EVENT( "no valid RXLEV information in meas_c_val_update_ptm 2" );
#endif /* #ifdef _SIMULATION_ */
  }
} /* meas_c_val_update_ptm() */

/*
+------------------------------------------------------------------------------
| Function    : meas_c_restart 
+------------------------------------------------------------------------------
| Description : This function initializes the parameters for deriving the
|               C value.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_c_restart ( void )
{
  TRACE_FUNCTION( "meas_c_restart" );

  grlc_data->meas.c_filter.value = 0;
  grlc_data->meas.c_filter.index = 0;
} /* meas_c_restart() */

/*
+------------------------------------------------------------------------------
| Function    : meas_sv_get_value
+------------------------------------------------------------------------------
| Description : This function returns the current signal variance.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE meas_sv_get_value ( void )
{
  T_SIGN_VAR_VALUE sv_val; 

  TRACE_FUNCTION( "meas_sv_get_value" );

  sv_val = grlc_data->meas.sign_var.value / SV_LEVEL_STEP;

  if( grlc_data->meas.sign_var.value % SV_LEVEL_STEP EQ  0 AND 
      grlc_data->meas.sign_var.value                 NEQ 0     )
  {
    sv_val -= 1;
  }

  if( sv_val > SV_LEVEL_MAX )
  {
    sv_val = SV_LEVEL_MAX;
  }

  return( (UBYTE)sv_val );
} /* meas_sv_get_value() */

/*
+------------------------------------------------------------------------------
| Function    : meas_sv_update
+------------------------------------------------------------------------------
| Description : ... 
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_sv_update ( T_MAC_PWR_CTRL_IND          *mac_pwr_ctrl_ind,
                             T_CGRLC_glbl_pwr_ctrl_param *pwr_ctrl_param,
                             T_CGRLC_freq_param          *freq_param )
{ 
  T_SIGN_VAR *sign_var = &grlc_data->meas.sign_var;

  UBYTE   i;               /* used for counting                         */
  UBYTE   ss_on_bcch[MAC_BURST_PER_BLOCK];
                           /* receive signal level measured on BCCH     */
  UBYTE   ss_off_bcch[MAC_BURST_PER_BLOCK];
                           /* receive signal level not measured on BCCH */
  UBYTE   j    = 0;        /* indicates the number of bursts received   */
                           /* on BCCH frequency                         */
  UBYTE  *ss_k = NULL;     /* points to the receive signal levels       */

  TRACE_FUNCTION( "meas_sv_update" );

  /* initialize arrays */
  for( i = 0; i < MAC_BURST_PER_BLOCK; i++ )
  {
    ss_on_bcch [i] = MAC_RXLEV_NONE;
    ss_off_bcch[i] = MAC_RXLEV_NONE;
  }

  if( mac_pwr_ctrl_ind->crc_error  EQ GRLC_CRC_PASS        AND
      pwr_ctrl_param->pc_meas_chan EQ CGRLC_MEAS_CHAN_PDCH     )
  {
    if( !freq_param->pdch_hopping )
    {
      /* sort burst level acc. its radio frequency */
      for( i = 0; i < MAC_BURST_PER_BLOCK; i++ )
      {
        if( mac_pwr_ctrl_ind->radio_freq[i] EQ freq_param->bcch_arfcn )
        {
          j++;

          ss_on_bcch[i] = mac_pwr_ctrl_ind->burst_level[i];
        }
        else
        {
          ss_off_bcch[i] = mac_pwr_ctrl_ind->burst_level[i];
        }
      }

      /* calculate the burst level that should be taken into account */
      switch( j )
      {
        case( 0 ): ss_k = &ss_off_bcch[0]; j = 4; break;
        case( 1 ): ss_k = &ss_off_bcch[0]; j = 3; break;
        case( 2 ): ss_k = NULL;                   break;
        case( 3 ): ss_k = &ss_on_bcch[0];  j = 3; break;
        case( 4 ): ss_k = &ss_on_bcch[0];  j = 4; break;
        default  : TRACE_ASSERT( j < 4 );         break;

      }
    }
    else
    {
      /* calculate the burst level that should be taken into account */
      ss_k = &mac_pwr_ctrl_ind->burst_level[0];
      j    = 4;
    }
  }

  if( ss_k NEQ NULL )
  {
    T_SIGN_VAR_INDEX  old_num;
    ULONG             old_elem; 
    UBYTE             pb_rdc;        /* Pb reduction                      */
    LONG              base;
    ULONG             sum      = 0;  /* sum of all squares                */
    ULONG             bl_var;        /* block variance                    */
    ULONG             ss_block;

    if( meas_c_calc_mean ( ss_k, mac_pwr_ctrl_ind->radio_freq,
                           &ss_block, &pb_rdc, freq_param ) > 0 )
    {
      /* calculate the sum of the squares of the difference between */
      /* each individual burst level value and the mean             */
      for( i = 0; i < j; i++ )
      {
        if( ss_k[i] NEQ MAC_RXLEV_NONE )
        {
          base = ( ( ss_k[i] * MEAS_ACRCY ) / SV_ACRCY_DIV ) -
                 ( ss_block / SV_ACRCY_DIV );
          sum += ( ULONG )( base * base );
        }
      }

      /*
       * calculate block variance
       *
       * in case ss_k NEQ NULL, j will be either set to the value 3 or 4 
       *
       * the lint comment below only works when linting current file alone, 
       * when linting current file together with all other files of the project,
       * an error message will be generated anyway
       *
       * I don't know why and gave-up to fix it 
       */
      bl_var = sum / ( UBYTE )( j - 1 ); /*lint !e414*/

      /* calculate the signal variance */
      if( sign_var->index EQ MEAS_MAX_NUM_MEAN )
      {
        old_num = MEAS_MAX_NUM_MEAN - 1;
      } 
      else
      {
        old_num = sign_var->index;
        sign_var->index++;
      }

      old_elem = sign_var->value * old_num;

      sign_var->value = 
        (T_SIGN_VAR_VALUE)((old_elem + (bl_var / SV_ACRCY_DIV)) / sign_var->index);
    }
    else
    {
#ifdef _SIMULATION_
      TRACE_EVENT( "no valid RXLEV information in meas_sv_update 1" );
#endif /* #ifdef _SIMULATION_ */
    }

#ifdef _SIMULATION_
    TRACE_EVENT_P2( "SIGN-VAR-Value = %d, SIGN-VAR-Index = %d", 
                    sign_var->value, sign_var->index );
#endif /* #ifdef _SIMULATION_ */
  }
  else
  {
#ifdef _SIMULATION_
    TRACE_EVENT( "no valid RXLEV information in meas_sv_update 2" );
#endif /* #ifdef _SIMULATION_ */
  }
} /* meas_sv_update() */

/*
+------------------------------------------------------------------------------
| Function    : meas_sv_restart 
+------------------------------------------------------------------------------
| Description : This function initializes the parameters for deriving the
|               signal variance.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_sv_restart ( void )
{
  TRACE_FUNCTION( "meas_sv_restart" );

  grlc_data->meas.sign_var.value = 0;
  grlc_data->meas.sign_var.index = 0;
} /* meas_sv_restart() */

/*
+------------------------------------------------------------------------------
| Function    : meas_sq_get_rxqual_value
+------------------------------------------------------------------------------
| Description : This function returns the current RXQUAL value
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE meas_sq_get_rxqual_value ( void)
{
  UBYTE      rxqual;
  T_BER_AVG *ber_avg = &grlc_data->meas.ber_avg; 

  TRACE_FUNCTION( "meas_sq_get_rxqual_value" );

  if      ( ber_avg->value < 200   )
  {
    rxqual = 0;         /* RX_QUAL_0 */
  }
  else if ( ber_avg->value < 400   )
  {
    rxqual = 1;         /* RX_QUAL_1 */
  }
  else if ( ber_avg->value < 800   )
  {
    rxqual = 2;         /* RX_QUAL_2 */
  }
  else if ( ber_avg->value < 1600  )
  {
    rxqual = 3;         /* RX_QUAL_3 */
  }
  else if ( ber_avg->value < 3200  )
  {
    rxqual = 4;         /* RX_QUAL_4 */
  }
  else if ( ber_avg->value < 6400  )
  {
    rxqual = 5;         /* RX_QUAL_5 */
  }
  else if ( ber_avg->value < 12800 )
  {
    rxqual = 6;         /* RX_QUAL_6 */
  }
  else 
  {
    rxqual = 7;         /* RX_QUAL_7 */
  }

  return ( rxqual );
} /* meas_sq_get_rxqual_value() */

/*
+------------------------------------------------------------------------------
| Function    : meas_sq_update
+------------------------------------------------------------------------------
| Description : This function updates the RXQUAL value.
|
| Parameters  : per_ind - Ptr to struct T_BER_IND
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_sq_update ( T_BER_IND *ber_ind )
{
  UBYTE        i;  
  ULONG        norm_fault;
  ULONG        norm_ber;
  ULONG        old_elem; 
  T_BER_INDEX  old_num;

  T_BER_AVG   *ber_avg = &grlc_data->meas.ber_avg;

  TRACE_FUNCTION( "meas_sq_update" );
  
  if(grlc_data->meas.sq_restart)
  {
    grlc_data->meas.ber_avg.value = 0;
    grlc_data->meas.ber_avg.index = 0;

    grlc_data->meas.sq_restart = FALSE;
  }

  for( i = 0; i < ber_ind->num_dl_blck; i++ )
  {
    if( ber_ind->be_info[i].total NEQ 0 )
    {
      if( ber_avg->index EQ MEAS_MAX_NUM_MEAN )
      {
        old_num = MEAS_MAX_NUM_MEAN - 1;
      } 
      else
      {
        old_num = ber_avg->index;
        ber_avg->index++;
      }

      old_elem       = ber_avg->value * old_num;
      norm_fault     = ber_ind->be_info[i].fault * MEAS_ACRCY * 100;
      norm_ber       = norm_fault / ber_ind->be_info[i].total;
      ber_avg->value = (T_BER_VALUE)((old_elem + norm_ber) / ber_avg->index);

      RX_SetRxQual( meas_sq_get_rxqual_value( ) );
    }
  }

#ifdef _SIMULATION_
  TRACE_EVENT_P2( "BER-Value = %d, BER-Index = %d", 
                  ber_avg->value, ber_avg->index );
#endif /* #ifdef _SIMULATION_ */
} /* meas_sq_update() */

/*
+------------------------------------------------------------------------------
| Function    : meas_sq_restart 
+------------------------------------------------------------------------------
| Description : This function initializes the parameters for deriving the
|               RXQUAL value.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_sq_restart ( void )
{
  TRACE_FUNCTION( "meas_sq_restart" );

  grlc_data->meas.ber_avg.value = 0;
  grlc_data->meas.ber_avg.index = 0;
} /* meas_sq_restart() */

/*
+------------------------------------------------------------------------------
| Function    : meas_int_get_rel_i_level
+------------------------------------------------------------------------------
| Description : This function returns the interference level values mapped to
|               values relative to the C value.
|
| Parameters  : *i_level - Ptr to relative interference level values
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_int_get_rel_i_level ( T_ilev *i_level )
{
  USHORT c_raw_data_lev = grlc_data->meas.c_filter.value;

  TRACE_FUNCTION( "meas_int_get_rel_i_level" );

  meas_int_fill_rel_iLevel( &i_level->v_ilev0, &i_level->ilev0, 0, c_raw_data_lev );
  meas_int_fill_rel_iLevel( &i_level->v_ilev1, &i_level->ilev1, 1, c_raw_data_lev );
  meas_int_fill_rel_iLevel( &i_level->v_ilev2, &i_level->ilev2, 2, c_raw_data_lev );
  meas_int_fill_rel_iLevel( &i_level->v_ilev3, &i_level->ilev3, 3, c_raw_data_lev );
  meas_int_fill_rel_iLevel( &i_level->v_ilev4, &i_level->ilev4, 4, c_raw_data_lev );
  meas_int_fill_rel_iLevel( &i_level->v_ilev5, &i_level->ilev5, 5, c_raw_data_lev );
  meas_int_fill_rel_iLevel( &i_level->v_ilev6, &i_level->ilev6, 6, c_raw_data_lev );
  meas_int_fill_rel_iLevel( &i_level->v_ilev7, &i_level->ilev7, 7, c_raw_data_lev );

#if !defined (NTRACE)

  meas_int_trace_i_level( i_level );
  
#endif /* #if !defined (NTRACE) */

} /* meas_int_get_rel_i_level() */

#if !defined (NTRACE)

/*
+------------------------------------------------------------------------------
| Function    : meas_int_trace_i_level
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
LOCAL void meas_int_trace_i_level( T_ilev *i_level )
{ 
  if( grlc_data->meas.v_im_trace NEQ 0 )
  {
    TRACE_EVENT_P8( "meas_int_trace_i_level: %02X %02X %02X %02X %02X %02X %02X %02X",
                    i_level->ilev0, i_level->ilev1, i_level->ilev2,
                    i_level->ilev3, i_level->ilev4, i_level->ilev5,
                    i_level->ilev6, i_level->ilev7 );
  }
} /* meas_int_trace_i_level() */
  
#endif /* #if !defined (NTRACE) */
