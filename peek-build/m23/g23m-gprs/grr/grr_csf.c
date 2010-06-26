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
|  Purpose :  This module implements local functions for service CS of
|             entity GRR.
+----------------------------------------------------------------------------- 
*/ 
       
#ifndef GRR_CSF_C
#define GRR_CSF_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include <string.h>
#include "typedefs.h"    /* to get Condat data types */
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
#include "grr_ctrls.h"
#include "grr_meass.h"
#include "grr_csf.h"
#include "grr_em.h"     /*for Engineering mode*/


/*==== CONST ================================================================*/
#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
 * Bitmasks
 */
#define BIT_0                     0x01
#define BIT_1                     0x02
#define BIT_2                     0x04
#define BIT_3                     0x08
#define BIT_4                     0x10
#define BIT_5                     0x20
#define BIT_6                     0x40
#define BIT_7                     0x80
#endif

T_NC_DATA* nc_data = NULL; /* pointer to neighbour cell data */

const SHORT p_res_off[32]       = { -52,  /*  0 -> -52 dB */
                                    -48,  /*  1 -> -48 dB */
                                    -44,  /*  2 -> -44 dB */
                                    -40,  /*  3 -> -40 dB */
                                    -36,  /*  4 -> -36 dB */
                                    -32,  /*  5 -> -32 dB */
                                    -28,  /*  6 -> -28 dB */
                                    -24,  /*  7 -> -24 dB */
                                    -20,  /*  8 -> -20 dB */
                                    -16,  /*  9 -> -16 dB */
                                    -12,  /* 10 -> -12 dB */
                                    -10,  /* 11 -> -10 dB */
                                    - 8,  /* 12 -> - 8 dB */
                                    - 6,  /* 13 -> - 6 dB */
                                    - 4,  /* 14 -> - 4 dB */
                                    - 2,  /* 15 -> - 2 dB */
                                      0,  /* 16 ->   0 dB */
                                      2,  /* 17 ->   2 dB */
                                      4,  /* 18 ->   4 dB */
                                      6,  /* 19 ->   6 dB */
                                      8,  /* 20 ->   8 dB */
                                     10,  /* 21 ->  10 dB */
                                     12,  /* 22 ->  12 dB */
                                     16,  /* 23 ->  16 dB */
                                     20,  /* 24 ->  20 dB */
                                     24,  /* 25 ->  24 dB */
                                     28,  /* 26 ->  28 dB */
                                     32,  /* 27 ->  32 dB */
                                     36,  /* 28 ->  36 dB */
                                     40,  /* 29 ->  40 dB */
                                     44,  /* 30 ->  44 dB */
                                     48   /* 31 ->  48 dB */
};

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
PURPOSE : checks whether the network colour code (ncc) is member
          of the ncc permitted field.
*/
LOCAL const UBYTE ncc_bit_mask[8] =
{
  BIT_0, BIT_1, BIT_2, BIT_3,
  BIT_4, BIT_5, BIT_6, BIT_7
};
#endif

#define NC_MODE_LEN        1
#define NC_RXLEV_SCELL_LEN 6
#define NC_IF_SCELL_LEN    6
#define NC_NUM_OF_MEAS_LEN 3
#define NC_FREQ_N_LEN      6
#define NC_BSIC_N_LEN      6
#define NC_RXLEV_N_LEN     6

#if defined (REL99) AND defined (TI_PS_FF_EMR)
#define BA_USED_LEN        1
#define BA_USED_3G_LEN     1
#define PSI3_CM_LEN        2
#define PMO_USED_LEN       1
#endif

#define GET_NC_RPT_PRD(x)  ((0x01<<(x))*480) /* multiple of 480 ms */

#define CS_PERIOD_1_SEC    217               /* 1 second defined by           */
                                             /* multiplies of TDMA frames     */
#define MAX_SYNC_FAILED_CNT 3

#if defined (REL99) AND defined (TI_PS_FF_EMR)
#define INRANGE(min, x, max)  ((unsigned)(x-min) <= (max-min))
#endif

#ifdef _TARGET_

#define MIN_RXLEV_FOR_SIX_STRGST 4 /* minimum RXLEV value of neighbour cells  */
                                   /* which shall be considered for cell      */
                                   /* re-selection decision                   */
#endif /* #ifdef _TARGET_ */

#define RXLEV_ACRCY        1000
#define RXLEV_MAX_NBR      ((T_RXLEV_DATA_NBR)(~((T_RXLEV_DATA_NBR)0)))
#define RXLEV_AVG_INVALID  0xFF

#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
#define CHECK_DUAL_BANDS (std EQ STD_DUAL        OR std EQ STD_DUAL_EGSM    OR \
                          std EQ STD_DUAL_US     OR std EQ STD_850_1800     OR \
                          std EQ STD_900_1900    OR std EQ STD_850_900_1800 OR \
                          std EQ STD_850_900_1900)
#endif

/*==== GLOBAL VARS ===========================================================*/

#if defined (REL99) AND defined (TI_PS_FF_EMR)
EXTERN BOOL use_ba_gprs;
#endif 

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

LOCAL void   cs_trace_nc_mval_lst ( void                           );
LOCAL void   cs_init_nc_mval      ( T_NC_MVAL       *mval          );
LOCAL void   cs_reorg_nc_mval_lst ( void                           );


LOCAL BOOL   cs_build_meas_rpt    ( T_U_MEAS_REPORT *u_meas_report );

LOCAL BOOL cs_process_serving_cell_data 
                                 ( UBYTE *rxlev_scell, UBYTE *v_i_scell, 
                                   UBYTE *i_scell, UBYTE *used_bits,
                                   BOOL *rsc_avail, BOOL *isc_avail );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
LOCAL void cs_build_enh_sorted_strng_arfcn_list
                                 (T_ENH_STRNG_ARFCN_LIST *f_bin   );
/*
LOCAL UBYTE cs_sort_arfcns_into_bins
                                 ( T_ENH_STRNG_ARFCN_LIST *p_s_bin,
                                   T_ENH_STRNG_ARFCN_LIST *p_m_bin,
                                   T_ENH_STRNG_ARFCN_LIST *p_r_bin);*/
LOCAL UBYTE cs_sort_arfcns_into_bins
                                 ( T_ENH_STRNG_ARFCN_LIST *p_s_bin,
                                   T_ENH_STRNG_ARFCN_LIST *p_r_bin);
LOCAL void cs_sort_store_in_desc_order_rxlev
                                 ( T_ENH_STRNG_ARFCN_LIST *p_bin, 
                                   T_ENH_STRNG_ARFCN_LIST *p_f_bin);
LOCAL BOOL cs_build_enh_meas_rpt( T_U_ENHNC_MEAS_REPORT *u_enh_meas_report );

LOCAL BOOL cs_fill_nc_enh_meas_results
                                 (T_nc_meas_rep_enh *nc_meas_rep_enh);
LOCAL void cs_form_enh_cell_list (void                            );
LOCAL void cs_form_ba_bcch_enh_cell_list
                                 (void                            );
LOCAL void cs_nc_sort_and_update_emr_info
                                 (void                            );
LOCAL void cs_nc_sort_cells_into_bins
                                 (T_ENH_BIN *p_sbin,T_ENH_BIN *p_mbin,
                                  T_ENH_BIN *p_rbin               );
LOCAL void cs_nc_sort_and_store_meas_results
                                 (T_ENH_BIN *p_bin, UBYTE nbr_rpt );
LOCAL void cs_nc_update_rest_bin
                                 (T_ENH_BIN *p_rbin, 
                                  T_ENH_BIN *p_bin, UBYTE nbr_rpt );
LOCAL UBYTE cs_get_band_index_for_emr 
                                 (USHORT arfcn);
LOCAL UBYTE cs_nc_scale_rxlev    (void);
#endif

LOCAL void   cs_build_nc_freq_list
                                  ( T_SC_DATABASE   *db            );
LOCAL void   cs_find_strongest_with_status
                                  ( UBYTE           *count, 
                                    UBYTE           *index,
                                    UBYTE            select_status,
                                    UBYTE            maximum,
                                    UBYTE            limitation    );
LOCAL void   cs_consider_multiband
                                  ( UBYTE           *count, 
                                    UBYTE           *index,
                                    UBYTE            select_status,
                                    UBYTE            maximum       );
LOCAL BOOL   cs_update_strongest  ( UBYTE            cnt_strgst,
                                    UBYTE            cnt_candid,
                                    UBYTE           *index,
                                    ULONG            fn_offset     );
LOCAL void   cs_start_t3158       ( T_TIME           time          );
LOCAL void   cs_set_cnt_nc_six    ( UBYTE            strgst,
                                    UBYTE            candid        );
LOCAL T_TIME cs_get_nc_rpt_prd_idle
                                  ( void                           );

LOCAL void cs_copy_rxlev_from_avg( UBYTE            rxlev_avg,
                                   T_CELL          *cell           );
LOCAL void cs_reuse_old_cell_rxlev  (void);
#if defined (REL99) AND defined (TI_PS_FF_EMR)
LOCAL void cs_build_ba_bcch_nc_freq_list ();
#endif

/*
+------------------------------------------------------------------------------
| Function    : cs_set_cnt_nc_six
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_set_cnt_nc_six ( UBYTE strgst, UBYTE candid )
{
  TRACE_FUNCTION( "cs_set_cnt_nc_six" );

#if defined FF_EOTD
  grr_data->db.cnt_nc_six.strgst = MINIMUM( CS_MAX_STRONG_CARRIER_CR, strgst );
#else  /* #if defined FF_EOTD */
  grr_data->db.cnt_nc_six.strgst = strgst;
#endif /* #if defined FF_EOTD */

  grr_data->db.cnt_nc_six.candid = candid;

} /* cs_set_cnt_nc_six() */

/*
+------------------------------------------------------------------------------
| Function    : cs_build_meas_rpt
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : u_meas_report - Pointer to measurement report
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL cs_build_meas_rpt ( T_U_MEAS_REPORT *u_meas_report )
{
  T_NC_ORDER      nc_ord        = cs_get_network_ctrl_order( TRUE );
  BOOL            pbcch_present = grr_is_pbcch_present( );
  UBYTE           used_bits;     
                           /* bits currently used for the measurement report */
  UBYTE           next_meas_bits = 0;
                           /* bits used for the next element of the          */
                           /* measurement result list                        */
  BOOL            is_first_instance;

  T_CS_MEAS_DATA *nmeas      = &grr_data->cs_meas;
  T_nc_meas_rep  *ncmeas_rep = &u_meas_report->nc_meas_rep;
  T_nc_meas_s1   *p_mrpt;
  T_ncell_info   *ncell;
  T_NC_MVAL      *mval;
  UBYTE return_value;
  BOOL rsc_avail = FALSE;
  BOOL isc_avail = FALSE;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  UBYTE psi3_cm = grr_get_psi3_cm();
#endif
  TRACE_FUNCTION( "cs_build_meas_rpt" );

  /* process message type */
  used_bits               = PMR_MSG_TYPE_LEN;
  u_meas_report->msg_type = U_MEAS_REPORT_c;

  /* process TLLI */
  used_bits += PMR_TLLI_LEN;
  grr_set_buf_tlli( &u_meas_report->tlli_value, grr_get_tlli( ) );

  /* process PSI5 change mark */
  used_bits += PMR_FLAG_LEN;

  if( pbcch_present                 EQ FALSE                       OR 
      grr_data->db.nc_ref_lst.param EQ &psc_db->nc_ms.ncmeas.param    )
  { 
    u_meas_report->v_psi5_cm = FALSE;
  }
  else if( grr_data->db.nc_ref_lst.param EQ &psc_db->nc_cw.param )
  {
    used_bits += PMR_PSI5_CHNGE_MRK_LEN;

    u_meas_report->v_psi5_cm = TRUE;
    u_meas_report->psi5_cm   = psc_db->psi5_params.psi5_change_mark;
  }
  else
  {
    TRACE_ERROR( "cs_build_meas_rpt no valid data set" );
    return( FALSE );
  }
  
  /* process flags */
  used_bits                    += PMR_FLAG_LEN;
  u_meas_report->flag           = 0;
  u_meas_report->v_nc_meas_rep  = TRUE;
  u_meas_report->v_xmeas_rep    = FALSE;

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
      TRACE_EVENT_P1("pbcch_present %d", pbcch_present);
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
#endif

  /* process NC mode */
  used_bits += NC_MODE_LEN;

  switch( nc_ord )
  {
    case NC_NC1: ncmeas_rep->nc_mode = NCMODE_NC1; break;
    case NC_NC2: ncmeas_rep->nc_mode = NCMODE_NC2; break;

    default: TRACE_ERROR( "cs_build_meas_rpt no valid NC mode" ); return( FALSE );
  }

  /* process serving cell data - rxlev and ilev */
  
  return_value = 
  cs_process_serving_cell_data(&ncmeas_rep->rxlev_scell, 
                              &ncmeas_rep->v_i_scell,
                              &ncmeas_rep->i_scell, &used_bits,
                              &rsc_avail, &isc_avail);
  if((return_value EQ FALSE) AND (rsc_avail EQ FALSE))
    return(FALSE);

  
  /* process NC measurement results */
  used_bits               += NC_NUM_OF_MEAS_LEN;
  ncmeas_rep->num_nc_meas  = 0;
  ncmeas_rep->c_nc_meas_s1 = 0;
  is_first_instance        = nmeas->pmr_snd_ref EQ 0 ? TRUE : FALSE;

  do
  {
    /* calculate estimated length of next measurement result */
    if( nmeas->pmr_snd_ref >= grr_data->db.cnt_nc_six.candid )
    {
      mval  = NULL;
      ncell = NULL;    
    }
    else
    {
      mval = &grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[nmeas->pmr_snd_ref].idx];

      next_meas_bits = NC_FREQ_N_LEN + NC_RXLEV_N_LEN + PMR_FLAG_LEN;

      if( mval->sync_info.sync.status EQ STAT_SYNC_OK )
      {
        UBYTE ncell_idx = 0;

        ncell = grr_get_ncell_info( mval->arfcn, mval->sync_info.bsic );

        while( ncell_idx  <  grr_data->db.nc_ref_lst.number AND
               ncell      EQ NULL                               )
        {
          ncell = grr_data->db.nc_ref_lst.info[ncell_idx];

          if( ncell->arfcn                   NEQ mval->arfcn       OR 
              NC_GET_TYPE( ncell->info_src ) NEQ INFO_TYPE_BA_BCCH    )
          {
            ncell = NULL;
          }

          ncell_idx++;
        }

        if( ncell NEQ NULL )
        {
          next_meas_bits = NC_FREQ_N_LEN + NC_RXLEV_N_LEN + PMR_FLAG_LEN;

          if( NC_GET_TYPE( ncell->info_src ) EQ INFO_TYPE_BA_BCCH )
          {
            next_meas_bits += NC_BSIC_N_LEN;
          }
        }
      }
      else
      {
        ncell = NULL;
      }
    }

    if( ncell NEQ NULL AND mval NEQ NULL)
    {
      if( BIT_UL_CTRL_BLOCK_CONTENTS - used_bits  >=  next_meas_bits     )
      {
        /* copy next measurement result to measurement report */

        ncmeas_rep->num_nc_meas++;
        ncmeas_rep->c_nc_meas_s1++;
        used_bits += next_meas_bits;
        p_mrpt     = &ncmeas_rep->nc_meas_s1[ncmeas_rep->c_nc_meas_s1 - 1];

        p_mrpt->freq_n   = ncell->index;
        p_mrpt->rxlev_n  = mval->rxlev_avg;

        if( NC_GET_TYPE( ncell->info_src ) EQ INFO_TYPE_BA_BCCH )
        {
          p_mrpt->bsic   = mval->sync_info.bsic;
          p_mrpt->v_bsic = TRUE;
        }
        else
        {
          p_mrpt->v_bsic = FALSE;
        }

        nmeas->pmr_snd_ref++;
      }
      else
      {
        mval = NULL;
      }
    }
    else
    {
      nmeas->pmr_snd_ref++;
    }

  }
  while( mval NEQ NULL );

  return( is_first_instance OR ncmeas_rep->num_nc_meas );


} /* cs_build_meas_rpt() */

/*
+------------------------------------------------------------------------------
| Function    : cs_is_meas_reporting
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE cs_is_meas_reporting ( void )
{
  T_NC_ORDER ctrl_order = cs_get_network_ctrl_order( TRUE );

  TRACE_FUNCTION( "cs_is_meas_reporting" );

  return( ctrl_order EQ NC_NC1 OR ctrl_order EQ NC_NC2 );

} /* cs_is_meas_reporting() */

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : cs_is_enh_meas_reporting
+------------------------------------------------------------------------------
| Description : This function is used to check whether enhanced measurement 
|               reporting is to be done.
|
| Parameters  : returns TRUE if ENH reporting is to be done
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL cs_is_enh_meas_reporting ( void )
{
  BOOL return_value = FALSE;
    
  TRACE_FUNCTION( "cs_is_enh_meas_reporting" );

  if ( cs_is_meas_reporting() AND 
    (grr_data->db.nc_ref_lst.enh_param->rept_type EQ REPORT_TYPE_ENH_REP) )
  {
    return_value = TRUE;
  }

  return( return_value );
} /* cs_is_enh_meas_reporting() */
#endif

/*
+------------------------------------------------------------------------------
| Function    : cs_build_nc_freq_list
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : db - pointer to serving cell database
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_build_nc_freq_list ( T_SC_DATABASE *db )
{ 
  UBYTE i, j, k;        /* used for counting                              */
  UBYTE act_idx;        /* index of actual processed reference list entry */
  BOOL  is_removed;
  BOOL  is_overwritten;
  
  T_ncell_info *info_ref_lst;
  T_ncell_info *info_prev  = ( db->nc_cw.list.number EQ 0 ?
                               NULL :
                               &db->nc_cw.list.info[db->nc_cw.list.number-1] );
  T_ncell_info *info_curr;
  T_INFO_SRC    info_src   = 0;

  TRACE_FUNCTION( "cs_build_nc_freq_list" );

  /*
   * add neighbour cell information of BA(GPRS) to reference list
   * in case they are not removed 
   */
  for( i = 0; i < db->nc_cw.list.number; i++ )
  {
    /*
     * looking for entry with correct index, 
     * they are no longer sorted with increasing number
     */
    k = 0;
    
    while( k < db->nc_cw.list.number AND i NEQ db->nc_cw.list.info[k].index )
    {
      k++;
    }

    if( db->nc_cw.list.info[k].arfcn EQ psc_db->pbcch.bcch.arfcn )
    {
      /* remove the serving cell from the neighbour cell reference list */
      is_removed = TRUE;
    }
    else
    {
      for( j = 0, is_removed = FALSE;
           j < db->nc_ms.rfreq.number AND is_removed EQ FALSE; 
           j++ )
      {
        if( db->nc_cw.list.info[k].index EQ db->nc_ms.rfreq.idx[j] )
        {
          is_removed = TRUE;
        }
      }
    }
  
    if( is_removed EQ FALSE )
    {
      if( !( db->nc_cw.list.info[k].v_cr_par                EQ  FALSE             AND
             NC_GET_TYPE( db->nc_cw.list.info[k].info_src ) NEQ INFO_TYPE_BA_BCCH AND
             cs_get_network_ctrl_order( TRUE )              NEQ NC_NC2                ) 
           AND
          !( db->nc_cw.list.info[k].cr_par.cell_ba                                    ) )
      {
        grr_data->db.nc_ref_lst.info[grr_data->db.nc_ref_lst.number] =
                                                       &db->nc_cw.list.info[k];
        grr_data->db.nc_ref_lst.number++;
      }
      else
      {

#if !defined (NTRACE)

        if( grr_data->cs.v_crp_trace EQ TRUE )
        {
          TRACE_EVENT_P2( "cs_build_nc_freq_list 1: not added to NC_REF_LST %d %d",
                          db->nc_cw.list.info[k].arfcn, db->nc_cw.list.info[k].bsic );
        }

#endif /* #if !defined (NTRACE) */

      }
    }
    else
    {

#if !defined (NTRACE)

      if( grr_data->cs.v_crp_trace EQ TRUE )
      {
        TRACE_EVENT_P2( "cs_build_nc_freq_list 2: not added to NC_REF_LST %d %d",
                        db->nc_cw.list.info[k].arfcn, db->nc_cw.list.info[k].bsic );
      }

#endif /* #if !defined (NTRACE) */


    }
  }

  /*
   * add/overwrite neighbour cell information to/of reference list
   */
  i = 0;
  
  while( ( info_curr = grr_get_next_bigger_ncell_param
                              ( &db->nc_ms.ncmeas.list, info_src ) ) NEQ NULL )
  {
    j              = 0;
    is_overwritten = FALSE;

    while( j < grr_data->db.nc_ref_lst.number AND is_overwritten EQ FALSE )
    {
      info_ref_lst = grr_data->db.nc_ref_lst.info[j];
  
      if( NC_GET_TYPE( info_ref_lst->info_src ) NEQ INFO_TYPE_BA_BCCH AND
          info_ref_lst->v_cr_par                EQ  TRUE              AND
          info_curr->v_cr_par                   EQ  TRUE              AND
          info_ref_lst->bsic                    EQ  info_curr->bsic   AND
          info_ref_lst->arfcn                   EQ  info_curr->arfcn      )
      {
        is_overwritten = TRUE;
      }
      else
      {
        j++;
      }
    }

    /*
     * calculate the index of the reference list entry 
     * which shall be used for further processing 
     */
    if( is_overwritten EQ FALSE )
    {
      if( grr_data->db.nc_ref_lst.number < MAX_NR_OF_NCELL )
      {
        act_idx = grr_data->db.nc_ref_lst.number;
      }
      else
      {
        TRACE_ERROR( "cs_build_nc_freq_list: reference list full" );
        return;
      }
    }
    else
    {
      act_idx = j;
    }


    if(
        !( info_curr->v_cr_par                EQ  FALSE                    AND
           NC_GET_TYPE( info_curr->info_src ) NEQ INFO_TYPE_BA_BCCH        AND
           cs_get_network_ctrl_order( TRUE )  NEQ NC_NC2                       ) 
         AND
        !( info_curr->cr_par.cell_ba                                           ) 
         AND
         ( info_curr->arfcn                   NEQ psc_db->pbcch.bcch.arfcn     )
      )
    {
     /*
      * add/overwrite reference list entry
      */
      grr_data->db.nc_ref_lst.info[act_idx] = info_curr;
      grr_data->db.nc_ref_lst.number++;

      /* process cell selection parameter */
      grr_restore_cs_param
                  ( info_curr, info_prev, (UBYTE)( db->nc_cw.list.number + i ) );


      if( info_curr->v_cr_par EQ TRUE )
      {
        info_prev = info_curr;
      }
    }
    else
    {

#if !defined (NTRACE)

      if( grr_data->cs.v_crp_trace EQ TRUE )
      {
        TRACE_EVENT_P2( "cs_build_nc_freq_list 3: not added to NC_REF_LST %d %d",
                        info_curr->arfcn, info_curr->bsic );
      }

#endif /* #if !defined (NTRACE) */

      info_curr->index = db->nc_cw.list.number + i;
    }

    info_src = info_curr->info_src;
  
    i++;  
  }  

#if !defined (NTRACE)

  if( grr_data->cs.v_crp_trace EQ TRUE )
  {
    UBYTE         read;
    USHORT        crp_s;
    ULONG         crp[4];
    T_ncell_info *info;

    crp[0]  = ( ( ULONG )psc_db->pbcch.bcch.arfcn  << 16 );
    crp[0] |= ( ( ULONG )psc_db->pbcch.bcch.bsic   <<  8 );
    crp[0] |=   ( ULONG )psc_db->scell_par.cell_ba;

    crp[1]  = ( ( ULONG )psc_db->scell_par.exc_acc                                   << 24 );
    crp[1] |= ( ( ULONG )psc_db->scell_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min << 16 );
    crp[1] |= ( ( ULONG )psc_db->scell_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch <<  8 );

    if( psc_db->scell_par.cr_par_1.v_hcs_par )
    {
      crp[1] |=   ( ULONG  )psc_db->scell_par.cr_par_1.hcs_par.gprs_prio_class;
      crp_s   = ( ( USHORT )psc_db->scell_par.cr_par_1.hcs_par.gprs_hcs_thr    << 8 );
    }
    else
    {
      crp[1] |=   ( ULONG  )0xFF;
      crp_s   = ( ( USHORT )0xFF << 8 );
    }

    crp_s |= ( ULONG )psc_db->scell_par.multi_band_rep;

    TRACE_EVENT_P3( "SCELL %08X%08X%04X", crp[0], crp[1], crp_s );

    for( read = 0; read < grr_data->db.nc_ref_lst.number; read++ )
    {
      info = grr_data->db.nc_ref_lst.info[read];
      
      crp[0]  = ( ( ULONG )info->arfcn << 16 );
      crp[0] |= ( ( ULONG )info->bsic  <<  8 );
      
      if( info->v_cr_par )
      {
        crp[0] |=   ( ULONG )info->cr_par.cell_ba;
      
        crp[1]  = ( ( ULONG )info->cr_par.exc_acc                                   << 24 );
        crp[1] |= ( ( ULONG )info->cr_par.same_ra_scell                             << 16 );
        crp[1] |= ( ( ULONG )info->cr_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min <<  8 );
        crp[1] |=   ( ULONG )info->cr_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch;

        crp[2]  = ( ( ULONG )info->cr_par.cr_offset.gprs_temp_offset  << 24 );
        crp[2] |= ( ( ULONG )info->cr_par.cr_offset.gprs_penalty_time << 16 );
        crp[2] |= ( ( ULONG )info->cr_par.gprs_resel_off              <<  8 );

        if( grr_data->db.nc_ref_lst.info[read]->cr_par.cr_par_1.v_hcs_par )
        {
          crp[2] |=   ( ULONG )info->cr_par.cr_par_1.hcs_par.gprs_prio_class;
          crp[3]  = ( ( ULONG )info->cr_par.cr_par_1.hcs_par.gprs_hcs_thr    << 24 );
        }
        else
        {
          crp[2] |=   ( ULONG )0xFF;
          crp[3]  = ( ( ULONG )0xFF << 24 );
        }

        if( info->cr_par.v_si13_pbcch )
        {
          if( info->cr_par.si13_pbcch.v_si13_location )
          {
            crp[3] |= ( ( ULONG )info->cr_par.si13_pbcch.si13_location << 16 );
            crp[3] |= ( ( ULONG )0xFF                                  << 8  );
            crp[3] |=   ( ULONG )0xFF;
          }
          else
          {
            crp[3] |= ( ( ULONG )0xFF                                       << 16 );
            crp[3] |= ( ( ULONG )info->cr_par.si13_pbcch.pbcch_location     << 8  );
            crp[3] |=   ( ULONG )info->cr_par.si13_pbcch.psi1_repeat_period;
          }
        }
        else
        {
          crp[3] |= ( ULONG )0xFFFFFF;
        }
      }
      else
      {
        crp[1] |= ( ULONG )0xFF;
        crp[2]  = ( ULONG )0xFFFFFFFF;
        crp[3]  = ( ULONG )0xFFFFFFFF;
      }

      TRACE_EVENT_P5( "NC_REF_LST[%d] %08X%08X%08X%08X", 
                      read, crp[0], crp[1], crp[2], crp[3] );
    }
  }

#endif /* #if !defined (NTRACE) */

}/* cs_build_nc_freq_list */

/*
+------------------------------------------------------------------------------
| Function    : cs_reorg_nc_mval_lst
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_reorg_nc_mval_lst ( void )
{ 
  BOOL          scell_restored = FALSE;
  UBYTE         mval_idx;
  UBYTE         nc_idx;
  T_NC_MVAL    *nc_mval;
  T_ncell_info *ncell_info;

  TRACE_FUNCTION( "cs_reorg_nc_mval_lst" );

  for( mval_idx = 0; mval_idx < MAX_NR_OF_NC_MVAL; mval_idx++ )
  {
    /* 
     * check whether measured values can be reused
     */
    
    nc_mval = &grr_data->db.nc_mval_list.nc_mval[mval_idx];
    
    if( nc_mval->status NEQ NC_MVAL_STAT_NONE )
    {
      /*
       * only used entries shall be considered 
       */
    
      if( psc_db->pbcch.bcch.arfcn EQ  nc_mval->arfcn      AND 
          psc_db->pbcch.bcch.arfcn NEQ RRGRR_INVALID_ARFCN     )
      {
        /*
         * remove the measured values from the serving cell,
         * this cell will not be used for cell re-selection
         */ 
        cs_init_nc_mval( nc_mval );
      }
      else
      {
        ncell_info = grr_get_ncell_info( nc_mval->arfcn, RRGRR_INVALID_BSIC );

        if( ncell_info EQ NULL )
        {
          /*
           * cell is not on the neighbor cell list,
           * keep the measured values until RR informs GRR,
           * but do not use them
           */
          nc_mval->status     = NC_MVAL_STAT_PENDING;
          nc_mval->rla_p.stat = CS_RLA_P_NOT_AVAIL;
        }
        else
        {
          /*
           * cell is on the neighbor cell list,
           * re-activate the measured values
           */
          nc_mval->status = NC_MVAL_STAT_ASSIGNED;
        }

        if( grr_data->db.old_scell.arfcn EQ  nc_mval->arfcn      AND 
            grr_data->db.old_scell.arfcn NEQ RRGRR_INVALID_ARFCN     )
        {
          /*
           * in case the old serving cell is on the measured values list,
           * keep the results for further use
           */
          scell_restored = TRUE;

          nc_mval->sync_info.bsic                 = grr_data->db.old_scell.bsic;
          nc_mval->sync_info.sync.status          = STAT_SYNC_OK;
          nc_mval->sync_info.sync.sync_failed_cnt = 0;
        }
      }
    }
  }

  for( nc_idx = 0; nc_idx < grr_data->db.nc_ref_lst.number; nc_idx++ )
  {
    nc_mval = grr_get_nc_mval( grr_data->db.nc_ref_lst.info[nc_idx]->arfcn,
                               RRGRR_INVALID_BSIC, &mval_idx );

    if( nc_mval EQ NULL )
    {
      nc_mval =
        grr_get_nc_mval( RRGRR_INVALID_ARFCN, RRGRR_INVALID_BSIC, &mval_idx );

      if( nc_mval NEQ NULL )
      {
        nc_mval->status = NC_MVAL_STAT_ASSIGNED;
        nc_mval->arfcn  = grr_data->db.nc_ref_lst.info[nc_idx]->arfcn;

        if( grr_data->db.old_scell.arfcn EQ  nc_mval->arfcn      AND
            grr_data->db.old_scell.arfcn NEQ RRGRR_INVALID_ARFCN     )
        {
          scell_restored = TRUE;

          nc_mval->sync_info.bsic                 = grr_data->db.old_scell.bsic;
          nc_mval->sync_info.sync.status          = STAT_SYNC_OK;
          nc_mval->sync_info.sync.sync_failed_cnt = 0;

          TRACE_EVENT_P2( "cs_reorg_nc_mval_lst: old SCELL on new NCELL list (1) %d %d",
                          grr_data->db.nc_ref_lst.info[nc_idx]->arfcn,
                          grr_data->db.nc_ref_lst.info[nc_idx]->bsic );
        }
      }
      else
      {
        TRACE_EVENT_P1( "cs_reorg_nc_mval_lst: nc_mval EQ NULL (1) %d",
                        RRGRR_INVALID_ARFCN );
      }
    }
    else
    {
      if( grr_data->db.old_scell.arfcn EQ  nc_mval->arfcn      AND 
          grr_data->db.old_scell.arfcn NEQ RRGRR_INVALID_ARFCN     )
      {
        scell_restored = TRUE;

        nc_mval->status                         = NC_MVAL_STAT_ASSIGNED;
        nc_mval->sync_info.bsic                 = grr_data->db.old_scell.bsic;
        nc_mval->sync_info.sync.status          = STAT_SYNC_OK;
        nc_mval->sync_info.sync.sync_failed_cnt = 0;

        TRACE_EVENT_P2( "cs_reorg_nc_mval_lst: old SCELL on new NCELL list (2) %d %d",
                        grr_data->db.nc_ref_lst.info[nc_idx]->arfcn,
                        grr_data->db.nc_ref_lst.info[nc_idx]->bsic );
      }
    }
  }

  if( scell_restored               EQ  FALSE               AND 
      grr_data->db.old_scell.arfcn NEQ RRGRR_INVALID_ARFCN     )
  {
    nc_mval =
      grr_get_nc_mval( RRGRR_INVALID_ARFCN, RRGRR_INVALID_BSIC, &mval_idx );

    if( nc_mval NEQ NULL )
    {
      nc_mval->status                         = NC_MVAL_STAT_PENDING;
      nc_mval->arfcn                          = grr_data->db.old_scell.arfcn;
      nc_mval->sync_info.bsic                 = grr_data->db.old_scell.bsic;
      nc_mval->sync_info.sync.status          = STAT_SYNC_OK;
      nc_mval->sync_info.sync.sync_failed_cnt = 0;
    }
    else
    {
      TRACE_EVENT_P1( "cs_reorg_nc_mval_lst: nc_mval EQ NULL (2) %d",
                      RRGRR_INVALID_ARFCN );
    }
  }

  grr_data->cs.is_mval_initialized = FALSE;

  cs_trace_nc_mval_lst( );

}/* cs_reorg_nc_mval_lst */

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : cs_grr_init
+------------------------------------------------------------------------------
| Description : The function cs_grr_init() .... name change becauce there is a
|               function in cs_init in TIL.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_grr_init ( void )
{
  TRACE_FUNCTION( "cs_grr_init" );

  grr_data->cs.gmm_state           = STANDBY_STATE;
  grr_data->cs.last_cr             = 0;
  grr_data->cs.reselect_cause      = CS_RESELECT_CAUSE_CS_NORMAL;
  grr_data->cs.is_mval_initialized = FALSE;
  grr_data->cs.nc_sync_rep_pd      = 0;

#if !defined (NTRACE)

  grr_data->cs.v_crp_trace = FALSE;
      
#endif /* #if !defined (NTRACE) */

  grr_init_ncmeas_extd_struct( &psc_db->nc_ms, FALSE );
  cs_init_nc_ref_list( &grr_data->db.nc_ref_lst );
  cs_init_nc_mval_lst( );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  /* Initialize Enhanced Measurement parameters and list */
  grr_init_enh_param(&psc_db->enh_ms, TRUE);
  grr_init_enh_cell_list();
#endif

  grr_data->cs_meas.pmr_snd_ref = 0;
  grr_data->cs_meas.packet_mode = PACKET_MODE_NULL;

  nc_data = &grr_data->nc_data;

  cs_reset_all_cell_results( );

  grr_data->cs.list_id             = 0;
  grr_data->cs.stop_req            = FALSE;

  nc_data->c_cell                  = 0;

  grr_data->cs.cr_meas_mode        = CS_CRMM_BA_GPRS;
  grr_data->cs.cr_meas_update      = FALSE;
  grr_data->cs.last_assignment_id  = 0;


  INIT_STATE( CS_MEAS, CS_MEAS_NULL  );
  INIT_STATE( CS, CS_NULL );
} /* cs_grr_init() */

/*
+------------------------------------------------------------------------------
| Function    : cs_process_cc_order
+------------------------------------------------------------------------------
| Description : The function cs_process_cc_order() ....
|
| Parameters  : d_cell_chan_order - Pointer to packet cell change order message
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_process_cc_order ( T_nc_meas_par  *nc_meas_par,
                                  UBYTE           v_nc_freq_list,
                                  T_nc_freq_list *nc_freq_list
#if defined (REL99) AND defined (TI_PS_FF_EMR)
                                , UBYTE       v_enh_meas_param_pcco,  
                                  T_enh_meas_param_pcco   *enh_meas_param_pcco
#endif
                                )
{
  TRACE_FUNCTION( "cs_process_cc_order" );

  grr_init_ncmeas_extd_struct( &pcr_db->nc_ms, FALSE );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  /* Initialize Enhanced Measurement parameters */
  grr_init_enh_param(&pcr_db->enh_ms, TRUE);
#endif

  /*
   * process the NC measurement parameter
   *
   * only one instance of PCCO, store data in final location
   */
  grr_prcs_nc_param_struct ( &pcr_db->nc_ms.ncmeas.param, 
                             nc_meas_par,
                             0 );

  /* For PCCO the paramater obtained in PCCO should be applied immediately
  Sec 5.6.1 Spec 4.60 */
  grr_data->db.nc_ref_lst.param = &pcr_db->nc_ms.ncmeas.param;

  /* 
   * process NC_FREQUENCY_LIST
   *
   * only one instance of PCCO, store data in final location
   */
  grr_prcs_nc_freq_list ( &pcr_db->nc_ms.ncmeas.list,
                          &pcr_db->nc_ms.rfreq,
                          v_nc_freq_list,
                          nc_freq_list,
                          INFO_TYPE_PCCO,
                          0 );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  /*
   * process the ENH measurement parameter
   *
   * only one instance of PCCO, store data in final location
   */
  if(v_enh_meas_param_pcco)
  {
  grr_prcs_enh_param_pcco ( pcr_db, enh_meas_param_pcco );
  }
#endif

} /* cs_process_cc_order() */

/*
+------------------------------------------------------------------------------
| Function    : cs_calc_cr_criterion
+------------------------------------------------------------------------------
| Description : The function cs_calc_cr_criterion() ....
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_calc_cr_criterion ( T_CR_CRITERIONS *cr_crit,
                                   USHORT           arfcn, 
                                   T_rla_p         *rla_p,
                                   T_CR_PAR_1      *cr_par_1 )
{
  T_MS_PWR_CAP mspc;
  SHORT        A, B;

  TRACE_FUNCTION( "cs_calc_cr_criterion" );

  /* C1     = A - MAX(B,0)                                                 */
  /* A      = RLA_P - GPRS_RXLEV_ACCESS_MIN                                */
  /* B      = GPRS_MS_TXPWR_MAX_CCH - P                                    */
  /* P      = max MS RF output power                                       */

  /* C31    = RLA_P(s) - HCS_THR(s) (serving cell)                         */
  /* C31    = RLA_P(n) - HCS_THR(n) - TO(n)*L(n) (neighbour cell)          */
  /* TO(n)  = GPRS_TEMPORARY_OFFSET(n) * H( GPRS_PENALTY_TIME(N) - T(n) )  */
  /* L(n)   = 0 if PRIORITY_CLASS(n) = PRIORITY_CLASS(s)                   */
  /*          1 if PRIORITY_CLASS(n) = PRIORITY_CLASS(s)                   */

  /* C32(s) = C1(s)                                                        */
  /* C32(n) = C1(n) + GPRS_RESELECT OFFSET(n) - TO(n)*(1-L(n))             */
  /* TO(n)  = GPRS_TEMPORARY_OFFSET(n) * H( GPRS_PENALTY_TIME(N) - T(n) )  */
  /* L(n)   = 0 if PRIORITY_CLASS(n) = PRIORITY_CLASS(s)                   */
  /*          1 if PRIORITY_CLASS(n) = PRIORITY_CLASS(s)                   */
  /* H(n)   = 0 for x <  0                                                 */
  /*          1 for x >= 0                                                 */
  /* T(n)   = ?????                                                        */

  grr_get_ms_pwr_cap( arfcn, grr_data->meas.pwr_offset, &mspc );

  /*
   * calculate cell re-selection criteria acc. GSM05.08, 
   * chapter 10.1.2 for the serving/neighbour cell
   */

  /* intermediate results for serving/neighbour cell */
  A = (SHORT)rla_p->lev - (SHORT)cr_par_1->cr_pow_par.gprs_rxlev_access_min;
  B = mspc.p_control[cr_par_1->cr_pow_par.gprs_ms_txpwr_max_cch] - mspc.p[mspc.ms_power];

  /* path loss criterion for serving/neighbour cell */
  cr_crit->c1 = A - MAXIMUM( B, 0 );

  /* signal threshold criterion for serving/neighbour cell */
  cr_crit->c31 = ( SHORT )rla_p->lev;

  if( cr_par_1->v_hcs_par )
  {
    cr_crit->c31 -= ( 2 * cr_par_1->hcs_par.gprs_hcs_thr );
  }

  /* cell ranking criterion for serving/neighbour cell */
  cr_crit->c32 = cr_crit->c1;

} /* cs_calc_cr_criterion() */

/*
+------------------------------------------------------------------------------
| Function    : cs_calc_params
+------------------------------------------------------------------------------
| Description : The function cs_calc_params() ....
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_calc_params ( void )
{
  BOOL          prio_cr = FALSE;
  UBYTE         h;
  UBYTE         i, prios, prion, strgst = 0xFF;
  T_ncell_info *ncell_info;
  T_NC_MVAL    *nc_mval;

  TRACE_FUNCTION( "cs_calc_params" );

  /* C1     = A - MAX(B,0)                                                 */
  /* A      = RLA_P - GPRS_RXLEV_ACCESS_MIN                                */
  /* B      = GPRS_MS_TXPWR_MAX_CCH - P                                    */
  /* P      = max MS RF output power                                       */

  /* C31    = RLA_P(s) - HCS_THR(s) (serving cell)                         */
  /* C31    = RLA_P(n) - HCS_THR(n) - TO(n)*L(n) (neighbour cell)          */
  /* TO(n)  = GPRS_TEMPORARY_OFFSET(n) * H( GPRS_PENALTY_TIME(N) - T(n) )  */
  /* L(n)   = 0 if PRIORITY_CLASS(n) = PRIORITY_CLASS(s)                   */
  /*          1 if PRIORITY_CLASS(n) = PRIORITY_CLASS(s)                   */

  /* C32(s) = C1(s)                                                        */
  /* C32(n) = C1(n) + GPRS_RESELECT OFFSET(n) - TO(n)*(1-L(n))             */
  /* TO(n)  = GPRS_TEMPORARY_OFFSET(n) * H( GPRS_PENALTY_TIME(N) - T(n) )  */
  /* L(n)   = 0 if PRIORITY_CLASS(n) = PRIORITY_CLASS(s)                   */
  /*          1 if PRIORITY_CLASS(n) = PRIORITY_CLASS(s)                   */
  /* H(n)   = 0 for x <  0                                                 */
  /*          1 for x >= 0                                                 */
  /* T(n)   = ?????                                                        */

  cs_calc_cr_criterion( &grr_data->db.scell_info.cr_crit,
                         psc_db->pbcch.bcch.arfcn, 
                        &grr_data->db.scell_info.rla_p,
                        &psc_db->scell_par.cr_par_1 );

  /* priority class for serving cell */
  prios = ( psc_db->scell_par.cr_par_1.v_hcs_par ) ?
                        psc_db->scell_par.cr_par_1.hcs_par.gprs_prio_class : 0;
    
  /*
   * calculate cell re-selection criteria acc. GSM05.08, 
   * chapter 10.1.2 for the neighbour cells
   */
  for( i = 0; i < grr_data->db.cnt_nc_six.strgst; i++ )
  {
    h          = FALSE;
    nc_mval = &grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[i].idx];

    if( nc_mval->sync_info.sync.status EQ STAT_SYNC_OK )
    {
      ncell_info = 
        grr_get_ncell_info( nc_mval->arfcn, nc_mval->sync_info.bsic );

      if( ncell_info NEQ NULL )
      {
        cs_calc_cr_criterion( &grr_data->db.nc_six_strgst[i].cr_crit,
                               ncell_info->arfcn, 
                              &nc_mval->rla_p,
                              &ncell_info->cr_par.cr_par_1 );

        /* cell ranking criterion for neighbour cell, add GPRS_RESELECT_OFFSET */

        /*
         * if the parameter C32_QUAL is set, positive GPRS_RESELECT_OFFSET values
         * shall only be applied to the neighbour cell with the highest RLA_P value
         * of those cells for which C32 is compared.
         *
         * calculation part 1
         */
        if( psc_db->gen_cell_par.c32_qual                EQ FALSE OR
            p_res_off[ncell_info->cr_par.gprs_resel_off] <  0        )
        {
          grr_data->db.nc_six_strgst[i].cr_crit.c32 +=
                                      p_res_off[ncell_info->cr_par.gprs_resel_off];
        }

        /* priority class of neighbour cell */
        prion = ( ncell_info->cr_par.cr_par_1.v_hcs_par ) ? 
                           ncell_info->cr_par.cr_par_1.hcs_par.gprs_prio_class : 0;

        /* check whether GPRS_TEMPORARY_OFFSET shall be considered */
        if( (ULONG)( ( ncell_info->cr_par.cr_offset.gprs_penalty_time + 1 ) * 
            10 * CS_PERIOD_1_SEC ) > grr_data->db.nc_six_strgst[i].avail_time )
        {
          h = TRUE;
        }

        /*
         * cell ranking and signal threshold criterion for neighbour cell,
         * subtract GPRS_TEMPORARY_OFFSET 
         */
        if( h )
        {
          SHORT *criterion;

          if( prios NEQ prion )
          {
            criterion = &grr_data->db.nc_six_strgst[i].cr_crit.c31;
          }
          else
          {
            criterion = &grr_data->db.nc_six_strgst[i].cr_crit.c32;
          }

          if( ncell_info->cr_par.cr_offset.gprs_temp_offset EQ
              GPRS_TEMPORARY_OFFSET_INFINITY                   )
          {
            *criterion = CS_SMALLEST_SHORT_VALUE;
          }
          else
          {
            *criterion -= 10 * ncell_info->cr_par.cr_offset.gprs_temp_offset;
          }
        }

        /*
         * cell ranking and signal threshold criterion for neighbour cell,
         * subtract RA_RESELECT_HYSTERESIS and/or GPRS_CELL_RESELECT_HYSTERESIS
         */
        if( !ncell_info->cr_par.same_ra_scell )
        {
          grr_data->db.nc_six_strgst[i].cr_crit.c32 -= 
                                               2 * psc_db->gen_cell_par.ra_re_hyst;
        }
        else if( grr_data->cs.gmm_state NEQ STANDBY_STATE )
        {
          grr_data->db.nc_six_strgst[i].cr_crit.c32 -= 
                                              2 * psc_db->gen_cell_par.gprs_c_hyst;
      
          if( psc_db->gen_cell_par.c31_hyst )
          {
            grr_data->db.nc_six_strgst[i].cr_crit.c31 -= 
                                              2 * psc_db->gen_cell_par.gprs_c_hyst;
          }
        }
    
        /*
         * in case a cell re-selection occured within the
         * previous 15 seconds, subtract 5 dB
         */
        if(grr_t_status( T15_SEC_CC ) > 0)
        {
          grr_data->db.nc_six_strgst[i].cr_crit.c32 -= 5;
        }
      }
      else
      {
        grr_data->db.nc_six_strgst[i].cr_crit.c1  = -2;
        grr_data->db.nc_six_strgst[i].cr_crit.c31 = -2;
        grr_data->db.nc_six_strgst[i].cr_crit.c32 = -2;
      }
    }
    else
    {
      grr_data->db.nc_six_strgst[i].cr_crit.c1  = -1;
      grr_data->db.nc_six_strgst[i].cr_crit.c31 = -1;
      grr_data->db.nc_six_strgst[i].cr_crit.c32 = -1;
    }
  }

  /*
   * calculate the correct neighbour cell for adding positive
   * GPRS_RESELECT_OFFSET values to C32 in case C32_QUAL is set
   */
  if( psc_db->gen_cell_par.c32_qual EQ TRUE )
  {
    /*
     * check whether priorized cell re-selection should be performed
     */
    for( i = 0; i < grr_data->db.cnt_nc_six.strgst; i++ )
    {
      if( grr_data->db.nc_six_strgst[i].cr_crit.c31 >= 0 )
      {
        prio_cr = TRUE;
      }
    }

    if( grr_data->db.scell_info.cr_crit.c31 >= 0 )
    {
      prio_cr = TRUE;
    }

    /* find out the strongest neighbour cell */
    for( i = 0; i < grr_data->db.cnt_nc_six.strgst; i++ )
    {
      if( ( prio_cr AND grr_data->db.nc_six_strgst[i].cr_crit.c31 >= 0 ) OR 
          ( !prio_cr )                                                      )
      {
        if( strgst EQ 0xFF                                                                        OR
            grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[i].idx].rla_p.lev      >
            grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[strgst].idx].rla_p.lev      )
        {
          strgst = i;
        }
      }
    }

    if( strgst NEQ 0xFF )
    {
      nc_mval = &grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[strgst].idx];

      if( nc_mval->sync_info.sync.status EQ STAT_SYNC_OK )
      {
        ncell_info = 
          grr_get_ncell_info( nc_mval->arfcn, nc_mval->sync_info.bsic );

        if( ncell_info NEQ NULL )
        {
          /*
           * if the parameter C32_QUAL is set, positive GPRS_RESELECT_OFFSET values
           * shall only be applied to the neighbour cell with the highest RLA_P value
           * of those cells for which C32 is compared.
           *
           * calculation part 2
           */
      
          if( p_res_off[ncell_info->cr_par.gprs_resel_off] > 0 )
          {
            grr_data->db.nc_six_strgst[strgst].cr_crit.c32 +=
                                      p_res_off[ncell_info->cr_par.gprs_resel_off];
          }
        }
      }
    }
  }

  /*
   * perform some tracing
   */

#if !defined (NTRACE)

  if( grr_data->cs.v_crp_trace EQ TRUE )
  {
    /* trace the C1 criterion */
    for( i = grr_data->db.cnt_nc_six.strgst; i < CS_MAX_STRONG_CARRIER; i++ )
    {
      grr_data->db.nc_six_strgst[i].cr_crit.c1 = 0;
    }

     TRACE_EVENT_P7
    (
      "C1   : %6d %6d %6d %6d %6d %6d %6d",
      grr_data->db.scell_info.cr_crit.c1,
      grr_data->db.nc_six_strgst[0].cr_crit.c1,
      grr_data->db.nc_six_strgst[1].cr_crit.c1,
      grr_data->db.nc_six_strgst[2].cr_crit.c1,
      grr_data->db.nc_six_strgst[3].cr_crit.c1,
      grr_data->db.nc_six_strgst[4].cr_crit.c1,
      grr_data->db.nc_six_strgst[5].cr_crit.c1
    );

    /* trace the C31 criterion */
    for( i = grr_data->db.cnt_nc_six.strgst; i < CS_MAX_STRONG_CARRIER; i++ )
    {
      grr_data->db.nc_six_strgst[i].cr_crit.c31 = 0;
    }

    TRACE_EVENT_P7
    (
      "C31  : %6d %6d %6d %6d %6d %6d %6d",

        grr_data->db.scell_info.cr_crit.c31,
        grr_data->db.nc_six_strgst[0].cr_crit.c31, 
        grr_data->db.nc_six_strgst[1].cr_crit.c31,
        grr_data->db.nc_six_strgst[2].cr_crit.c31, 
        grr_data->db.nc_six_strgst[3].cr_crit.c31,
        grr_data->db.nc_six_strgst[4].cr_crit.c31, 
        grr_data->db.nc_six_strgst[5].cr_crit.c31
      );

    /* trace the C32 criterion */
    for( i = grr_data->db.cnt_nc_six.strgst; i < CS_MAX_STRONG_CARRIER; i++ )
    {
      grr_data->db.nc_six_strgst[i].cr_crit.c32 = 0;
    }

    TRACE_EVENT_P7
      ( 
        "C32  : %6d %6d %6d %6d %6d %6d %6d",
        grr_data->db.scell_info.cr_crit.c32,
        grr_data->db.nc_six_strgst[0].cr_crit.c32, 
        grr_data->db.nc_six_strgst[1].cr_crit.c32,
        grr_data->db.nc_six_strgst[2].cr_crit.c32, 
        grr_data->db.nc_six_strgst[3].cr_crit.c32,
        grr_data->db.nc_six_strgst[4].cr_crit.c32, 
        grr_data->db.nc_six_strgst[5].cr_crit.c32
      );
  }

#endif /* #if !defined (NTRACE) */

} /* cs_calc_params() */

/*
+------------------------------------------------------------------------------
| Function    : cs_store_meas_values
+------------------------------------------------------------------------------
| Description : The function cs_store_meas_values() saves the received
|               rla_p values in the grr database
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_store_meas_values ( void )
{
  UBYTE   i, j; /* used for counting */

  USHORT  arfcn;

  T_rla_p rla_p;

  TRACE_FUNCTION( "cs_store_meas_values" );

  /*
   * store the first value as serving cell
   */
  grr_data->db.scell_info.rla_p.stat =
                            cs_get_rla_p( &nc_data->cell[0].rla_p_data, 
                                          &grr_data->db.scell_info.rla_p.lev );

  if( grr_data->db.scell_info.rla_p.stat NEQ CS_RLA_P_NOT_AVAIL )
  {
#ifdef FF_PS_RSSI
    RX_SetValue( grr_data->db.scell_info.rla_p.lev,
                 RX_QUAL_UNAVAILABLE,
                 psc_db->scell_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min);
#else
    RX_SetValue( grr_data->db.scell_info.rla_p.lev );
#endif
  }

  /*
   * store the next values as neighbour cells
   */
  for( j = 1; j < nc_data->c_cell; j++ )
  { 
    i          = 0;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    arfcn      = nc_data->cell[j].arfcn;
#else
    arfcn      = grr_l1_arfcn_to_g23( nc_data->cell[j].arfcn );
#endif
    rla_p.stat = cs_get_rla_p( &nc_data->cell[j].rla_p_data, &rla_p.lev );

    while( i                                          <   MAX_NR_OF_NC_MVAL AND
           grr_data->db.nc_mval_list.nc_mval[i].arfcn NEQ arfcn                 )

    {
      i++;
    }
    
    if( i < MAX_NR_OF_NC_MVAL )
    {
      grr_data->db.nc_mval_list.nc_mval[i].rla_p = rla_p;
    }
  }
} /* cs_store_meas_values() */

/*
+------------------------------------------------------------------------------
| Function    : cs_store_rxlev_values
+------------------------------------------------------------------------------
| Description : The function cs_store_rxlev_values()
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_store_rxlev_values ( void )
{
  UBYTE  i, j; /* used for counting */

  USHORT  arfcn;

  TRACE_FUNCTION( "cs_store_rxlev_values" );

  /*
   * store the first value as serving cell
   */
  grr_data->db.scell_info.rxlev_avg = 
                              cs_get_rxlev_avg( &nc_data->cell[0].rxlev_data );

  /*
   * store the next values as neighbour cells
   */
  for( j = 1; j < nc_data->c_cell; j++ )
  { 
    i     = 0;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    arfcn      = nc_data->cell[j].arfcn;
#else
    arfcn      = grr_l1_arfcn_to_g23( nc_data->cell[j].arfcn );
#endif

    while( i                                          <   MAX_NR_OF_NC_MVAL AND
           grr_data->db.nc_mval_list.nc_mval[i].arfcn NEQ arfcn                 )

    {
      i++;
    }
    
    if( i < MAX_NR_OF_NC_MVAL )
    {
      grr_data->db.nc_mval_list.nc_mval[i].rxlev_avg = 
                              cs_get_rxlev_avg( &nc_data->cell[j].rxlev_data );
    }
  }
} /* cs_store_rxlev_values() */

/*
+------------------------------------------------------------------------------
| Function    : cs_find_strongest
+------------------------------------------------------------------------------
| Description : The function cs_find_strongest() finds out the six strongest
|               neighbour cells and stores their indices in an array.
|
| Parameters  : cnt_strgst - number of strongest carriers
|               cnt_candid - number of strongest plus candidate carriers
|               index      - index of carriers
|               status     - status of carriers
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL cs_find_strongest ( ULONG fn_offset )
{
  UBYTE cnt_strgst;
  UBYTE cnt_candid;
  UBYTE index[CS_MAX_STRONG_CARRIER];

  UBYTE max_nc_excl_sc_band = 0; /* maximum number of reported neighbour */
                                 /* cells that should be not in the band */
                                 /* of the serving cell                  */

  TRACE_FUNCTION( "cs_find_strongest" );
  
#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
  if( CHECK_DUAL_BANDS EQ TRUE)
#else
  if( std EQ STD_DUAL OR std EQ STD_DUAL_EGSM OR std EQ STD_DUAL_US )
#endif
  {
    switch( psc_db->scell_par.multi_band_rep )
    {
      case GRR_MULTIBAND_REPORTING_1: max_nc_excl_sc_band = 1; break;
      case GRR_MULTIBAND_REPORTING_2: max_nc_excl_sc_band = 2; break;
      case GRR_MULTIBAND_REPORTING_3: max_nc_excl_sc_band = 3; break;
      default                       : max_nc_excl_sc_band = 0; break;
    }
  }

  /*
   * look for strongest neighbour cells with a valid RLA_P value,
   * these are used for cell re-selection only
   */
  cnt_strgst = 0;

  cs_consider_multiband
    ( &cnt_strgst, index, CS_RLA_P_VALID, max_nc_excl_sc_band );

  /*
   * look for strongest neighbour cells with a RLA_P which is not valid
   * but available and add them to the remaining storage areas in the list
   * of strongest neighbour cells, these are used for checking the BSIC
   * in advance, these cells may become a valid RLA_P value soon
   */
  cnt_candid = cnt_strgst;

  if( cnt_candid < CS_MAX_STRONG_CARRIER )
  {
    cs_consider_multiband
      ( &cnt_candid, index, CS_RLA_P_NOT_VALID, max_nc_excl_sc_band );
  }

  return(cs_update_strongest( cnt_strgst, cnt_candid, index, fn_offset ));
} /* cs_find_strongest() */

/*
+------------------------------------------------------------------------------
| Function    : cs_consider_multiband
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_consider_multiband ( UBYTE *count, 
                                   UBYTE *index,
                                   UBYTE  select_status,
                                   UBYTE  maximum )
{
  TRACE_FUNCTION( "cs_consider_multiband" );

  if( maximum NEQ 0 )
  {
    /*
     * A maximum of zero means that normal reporting of the six strongest
     * cell irrespective of the band used should be performed.
     *
     * A maximum greater zero means that multi band reporting should be
     * performed, so the MS shall report the strongest cells in each of the
     * frequency bands in the BA list, excluding the frequency band of the 
     * serving cell. The remaining positions shall be used for reporting of 
     * the cells in the band of the serving cell.
     */
    cs_find_strongest_with_status
      ( count, index, select_status, maximum, CS_EXCLUDE_SC_BAND );

    cs_find_strongest_with_status
      ( count, index, select_status, 
        (UBYTE)( CS_MAX_STRONG_CARRIER - maximum ), CS_ONLY_SC_BAND );
  }

  if( *count < CS_MAX_STRONG_CARRIER )
  {
    cs_find_strongest_with_status
      ( count, index, select_status,
        (UBYTE)( CS_MAX_STRONG_CARRIER - *count ), CS_NO_BAND_LIMITATION );
  }
} /* cs_consider_multiband() */

/*
+------------------------------------------------------------------------------
| Function    : cs_find_strongest_with_status
+------------------------------------------------------------------------------
| Description : The function cs_find_strongest_with_status() finds out the 
|               six strongest neighbour cells with a specific status and 
|               stores their indices in an array.
|
| Parameters  : count         - number of strongest carriers
|               index         - index of carriers
|               status        - status of carriers
|               select_status - status which should be used for selection
|
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_find_strongest_with_status ( UBYTE *count, 
                                           UBYTE *index,
                                           UBYTE  select_status,
                                           UBYTE  maximum,
                                           UBYTE  limitation )
{
  UBYTE          strgst_idx;         /* used for counting                       */
  UBYTE          mval_idx;           /* used for counting                       */
  UBYTE          k;                  /* used for counting                       */  
  BOOL           cells_left = TRUE;  /* indicates whether there are more cells  */
                                     /* which may belong to the strongest       */
  SHORT          max_rla_p;          /* next maximum RLA_P value                */
  UBYTE          idx_next_max;       /* index of the cell with the next max.    */
                                     /* RLA_P value                             */
  BOOL           one_of_six_flag;    /* indicates whether a cell already        */
                                     /* belongs to the strongest                */
  BOOL           limit;              /* indicator for limitation precedence     */
  BOOL           scell_is_high_band; /* serving cell is in DCS or PCS band      */
  BOOL           ncell_is_high_band; /* neighbour cell is in DCS or PCS band    */
  UBYTE          max_found_cell = MINIMUM( *count + maximum, CS_MAX_STRONG_CARRIER );
                                     /* maximum number of strongest neighbour   */
                                     /* cells that should be found after        */
                                     /* leaving this function                   */
  USHORT         low_channel;
  USHORT         high_channel;
  
  T_NC_MVAL  *nc_mval;

  TRACE_FUNCTION( "cs_find_strongest_with_status" );

#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
  if( std EQ STD_DUAL_US OR std EQ STD_900_1900 OR 
      std EQ STD_850_900_1900 )
#else
  if( std EQ STD_DUAL_US )
#endif
  {
    low_channel  = LOW_CHANNEL_1900;
    high_channel = HIGH_CHANNEL_1900;
  }
  else
  {
    low_channel  = LOW_CHANNEL_1800;
    high_channel = HIGH_CHANNEL_1800;
  }

  for( strgst_idx = *count;
       strgst_idx < max_found_cell AND cells_left;
       strgst_idx++ )
  {
    max_rla_p    = -1;
    idx_next_max = 0xFF;

    for( mval_idx = 0; mval_idx < MAX_NR_OF_NC_MVAL; mval_idx++  )
    {
      nc_mval = &grr_data->db.nc_mval_list.nc_mval[mval_idx];
      
      if( nc_mval->status EQ NC_MVAL_STAT_ASSIGNED  )
      {
        one_of_six_flag = FALSE;

        for( k = 0; k < *count AND one_of_six_flag EQ FALSE; k++ )
        {
          if( mval_idx EQ index[k] )
          {
            one_of_six_flag = TRUE;
          }
        }

        if( one_of_six_flag     EQ FALSE                    AND
            nc_mval->rla_p.stat EQ select_status            AND

#ifdef _TARGET_
            nc_mval->rla_p.lev  >= MIN_RXLEV_FOR_SIX_STRGST AND
#endif /* #ifdef _TARGET_ */

            nc_mval->rla_p.lev  >  max_rla_p                    )        
        {
          switch( limitation )
          {   
            default:
              TRACE_ASSERT( limitation EQ CS_NO_BAND_LIMITATION OR 
                            limitation EQ CS_EXCLUDE_SC_BAND    OR
                            limitation EQ CS_ONLY_SC_BAND          );
            
              /*lint -fallthrough*/
              
              /*
               * fallthrough statement inserted to pass lint process,
               * error 644: (Warning -- limit may not have been initialized) 
               */
            
            case( CS_NO_BAND_LIMITATION ):
              limit = FALSE;
              break;
        
            case( CS_EXCLUDE_SC_BAND ):
              scell_is_high_band =
                ( psc_db->pbcch.bcch.arfcn >= low_channel  AND
                  psc_db->pbcch.bcch.arfcn <= high_channel     );

              ncell_is_high_band =
                ( nc_mval->arfcn >= low_channel  AND
                  nc_mval->arfcn <= high_channel     );

               limit = ( scell_is_high_band EQ ncell_is_high_band  );
              break;

            case( CS_ONLY_SC_BAND ):
              scell_is_high_band =
                ( psc_db->pbcch.bcch.arfcn >= low_channel  AND
                  psc_db->pbcch.bcch.arfcn <= high_channel     );

              ncell_is_high_band = 
                ( nc_mval->arfcn >= low_channel  AND
                  nc_mval->arfcn <= high_channel     );

              limit = ( scell_is_high_band NEQ ncell_is_high_band );
              break;
          }

          /*
           * limitation may take precedence,
           * do not consider neighbour cell in case 
           */
          if( limit EQ FALSE )
          {
            max_rla_p    = nc_mval->rla_p.lev;
            idx_next_max = mval_idx;
          }
        }
      }
    }
    
    if( idx_next_max EQ 0xFF )
    {
      cells_left = FALSE;
    }
    else
    {
      index[strgst_idx] = idx_next_max;

      (*count)++;
    }
  }
} /* cs_find_strongest_with_status() */

/*
+------------------------------------------------------------------------------
| Function    : cs_update_strongest
+------------------------------------------------------------------------------
| Description : The function cs_update_strongest() updates the list of the 
|               strongest neighbour cells stored in the database.
|
| Parameters  : cnt_strgst - number of strongest carriers
|               cnt_candid - number of strongest plus candidate carriers
|               index      - index of carriers
|               status     - status of carriers
|               fn_offset  - frame number offset
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL cs_update_strongest ( UBYTE  cnt_strgst,
                                 UBYTE  cnt_candid,
                                 UBYTE *index,
                                 ULONG  fn_offset )
{
  UBYTE i, j;          /* used for counting                                  */
  ULONG avail_time[CS_MAX_STRONG_CARRIER];
                       /* used to store the times when a cell becomes first  */
                       /* a cell of the strongest carriers for a possible    */
                       /* rearrangment of the list of the strongest carriers */

  T_NC_SIX_STRGST *six_strgst;
  T_NC_SIX_STRGST *six_candid;

  BOOL has_changed = ( grr_data->db.cnt_nc_six.candid NEQ cnt_candid );


  TRACE_FUNCTION( "cs_update_strongest" );

  memset( avail_time, 0, sizeof( avail_time ) );

  for( i = 0; i < cnt_candid; i++ )
  {
    /*
     * check whether cell is already inserted in list of strongest carrier 
     */
    six_strgst = NULL;
    six_candid = NULL;
    j          = 0;    
    
    while( j          <  grr_data->db.cnt_nc_six.candid AND 
           six_strgst EQ NULL                           AND
           six_candid EQ NULL                               )
    {
      six_candid = &grr_data->db.nc_six_strgst[j];

      if( index[i] EQ six_candid->idx )
      {
        if( j < grr_data->db.cnt_nc_six.strgst )
        {
          six_strgst = six_candid;
        }
      }
      else
      {
        six_candid = NULL;

        j++;
      }
    }


    if( six_strgst NEQ NULL )
    {      
      if( six_strgst->avail_time                               <=
            ( GPRS_PENALTY_TIME_MAX + 1 ) * 10 * CS_PERIOD_1_SEC    )
      {
        /*
         * the time how long the cell is already inserted
         * is stored up to a limited period
         */
        avail_time[i] = six_strgst->avail_time + fn_offset;
      }
      else
      {
        avail_time[i] = six_strgst->avail_time;
      }    
    }
    else
    {
      if( six_candid EQ NULL )
      {
        has_changed = TRUE;
      }

      if( grr_data->db.old_scell.arfcn EQ
                      grr_data->db.nc_mval_list.nc_mval[index[i]].arfcn           AND
          grr_data->db.old_scell.bsic  EQ
                      grr_data->db.nc_mval_list.nc_mval[index[i]].sync_info.bsic           )
      {
        /* for the old serving cell timer T shall always set to status expired */
        avail_time[i] = 
                  ( ( GPRS_PENALTY_TIME_MAX + 1 ) * 10 * CS_PERIOD_1_SEC ) + 1;
      }

    }
  }

  for( i = 0; i < cnt_candid; i++ )
  {
    grr_data->db.nc_six_strgst[i].idx        = index[i];
    grr_data->db.nc_six_strgst[i].avail_time = avail_time[i];

    /*
     * every time a new list of strongest carriers is build-up 
     * the parameter mode should be set to CS_MODE_IDLE.
     */
    grr_data->db.nc_six_strgst[i].mode = CS_MODE_IDLE;
  } 

  cs_set_cnt_nc_six( cnt_strgst, cnt_candid );

  /*
   * perform some tracing
   */

#if !defined (NTRACE)

  if( grr_data->cs.v_crp_trace EQ TRUE )
  {
    /* trace the ARFCN */
    {
      USHORT arfcn[CS_MAX_STRONG_CARRIER];

      memset( arfcn, RRGRR_INVALID_ARFCN, sizeof( arfcn ) );
      
      for( i = 0; i < grr_data->db.cnt_nc_six.candid; i++ )
      {
        arfcn[i] =
          grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[i].idx].arfcn;
      }

      TRACE_EVENT_P7
      (
        "ARFCN: %6d %6d %6d %6d %6d %6d %6d",
        psc_db->pbcch.bcch.arfcn,
        arfcn[0], arfcn[1], arfcn[2], arfcn[3], arfcn[4], arfcn[5]
      );
    }

    /* trace the BSIC */
    {
      USHORT bsic[CS_MAX_STRONG_CARRIER];

      memset( bsic, RRGRR_INVALID_BSIC, sizeof( bsic ) );
      
      for( i = 0; i < grr_data->db.cnt_nc_six.candid; i++ )
      {
        bsic[i] =
          grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[i].idx].sync_info.bsic;
      }

      TRACE_EVENT_P7
      (
        "BSIC : %6d %6d %6d %6d %6d %6d %6d",
        psc_db->pbcch.bcch.bsic,
        bsic[0], bsic[1], bsic[2], bsic[3], bsic[4], bsic[5]
      );
    }

    /* trace the RLA_P */
    {
      UBYTE rla_p[CS_MAX_STRONG_CARRIER];
      UBYTE rxlev_avg[CS_MAX_STRONG_CARRIER];

      memset( rla_p, 0, sizeof( rla_p ) );
      memset( rxlev_avg, 0, sizeof( rxlev_avg ) );

      for( i = 0; i < grr_data->db.cnt_nc_six.candid; i++ )
      {
        rla_p[i] = 
          grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[i].idx].rla_p.lev;


        rxlev_avg[i] = 
          grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[i].idx].rxlev_avg;

      }

      TRACE_EVENT_P7
      ( 
        "RLA_P: %6d %6d %6d %6d %6d %6d %6d",
        grr_data->db.scell_info.rla_p.lev,
        rla_p[0], rla_p[1], rla_p[2], rla_p[3], rla_p[4], rla_p[5]
      );

      TRACE_EVENT_P7
      ( 
        "RXLEV_AVG: %6d %6d %6d %6d %6d %6d %6d",
        grr_data->db.scell_info.rxlev_avg,
        rxlev_avg[0], rxlev_avg[1], rxlev_avg[2], rxlev_avg[3], rxlev_avg[4], rxlev_avg[5]
      );

    }

    /* trace the synchronisation status */
    {
      UBYTE sync_stat[CS_MAX_STRONG_CARRIER];

      memset( sync_stat, 0xFF, sizeof( sync_stat ) );
     
      for( i = 0; i < grr_data->db.cnt_nc_six.candid; i++ )
      {
        sync_stat[i] =
          grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[i].idx].sync_info.sync.status;
      }

      TRACE_EVENT_P6
      (
        "SYNC :        %6d %6d %6d %6d %6d %6d",
        sync_stat[0], sync_stat[1], sync_stat[2],
        sync_stat[3], sync_stat[4], sync_stat[5]
      );
    }
  }

#endif /* #if !defined (NTRACE) */

  return( has_changed );

} /* cs_update_strongest() */

/*
+------------------------------------------------------------------------------
| Function    : cs_compare_bsic
+------------------------------------------------------------------------------
| Description : The function cs_compare_bsic() compares the indicated BSIC
|               with the stored BSICs
|
| Parameters  : rrgrr_check_bsic_ind - pointer to primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_compare_bsic ( T_RRGRR_NCELL_SYNC_IND *rrgrr_check_bsic_ind )
{
  UBYTE      i = 0;
  UBYTE      j;
  UBYTE      nc_mval_idx;
  T_NC_MVAL *nc_mval;

    
  TRACE_FUNCTION( "cs_compare_bsic" );



  while( i                                          <   RRGRR_MAX_RSLT_NCELL_SYNC_IND AND 
         rrgrr_check_bsic_ind->sync_result[i].arfcn NEQ RRGRR_INVALID_ARFCN               )
  {
    if( ( nc_mval = grr_get_nc_mval(  rrgrr_check_bsic_ind->sync_result[i].arfcn,
                                      RRGRR_INVALID_BSIC, &nc_mval_idx ) ) NEQ NULL )
    {
      switch( rrgrr_check_bsic_ind->sync_result[i].sb_flag )
      {
        case( SB_FOUND ):
          nc_mval->sync_info.bsic                 = rrgrr_check_bsic_ind->sync_result[i].bsic;
          nc_mval->sync_info.sync.status          = STAT_SYNC_OK;
          nc_mval->sync_info.sync.sync_failed_cnt = 0;
          break;
  
        case( NO_SB_FOUND ):
          nc_mval->sync_info.bsic = RRGRR_INVALID_BSIC;

          if( nc_mval->sync_info.sync.sync_failed_cnt EQ MAX_SYNC_FAILED_CNT )
          {
            nc_mval->sync_info.sync.status          = STAT_SYNC_NONE;
            nc_mval->sync_info.sync.sync_failed_cnt = 0;

            j=0;     
            while( j < grr_data->nc_data.c_cell                   AND 
                   grr_data->nc_data.cell[j].arfcn    NEQ
                   nc_mval->arfcn         )
            {
              j++;
            }

            if( j < grr_data->nc_data.c_cell )
            {
              cs_reset_meas_result( &grr_data->nc_data.cell[j] );
            }

          }
          else
          {
            nc_mval->sync_info.sync.status          =  STAT_SYNC_FAILED;
            nc_mval->sync_info.sync.sync_failed_cnt += 1;
          }
          break;

        case( SB_UNKNOWN ):
          if( nc_mval->status EQ NC_MVAL_STAT_PENDING )
          {
            cs_init_nc_mval( nc_mval );
          }
          else
          {
            nc_mval->sync_info.bsic                 = RRGRR_INVALID_BSIC;
            nc_mval->sync_info.sync.status          = STAT_SYNC_NONE;
            nc_mval->sync_info.sync.sync_failed_cnt = 0;
          }
          break;
      }
    }

    if( nc_mval EQ NULL )
    {
      if( ( nc_mval = grr_get_nc_mval( RRGRR_INVALID_ARFCN,
                                       RRGRR_INVALID_BSIC,
                                       &nc_mval_idx ) ) NEQ NULL )
      {
        switch( rrgrr_check_bsic_ind->sync_result[i].sb_flag )
        {
          case( SB_FOUND ):
            nc_mval->status                         = NC_MVAL_STAT_PENDING;
            nc_mval->arfcn                          = rrgrr_check_bsic_ind->sync_result[i].arfcn;
            nc_mval->sync_info.bsic                 = rrgrr_check_bsic_ind->sync_result[i].bsic;
            nc_mval->sync_info.sync.status          = STAT_SYNC_OK;
            nc_mval->sync_info.sync.sync_failed_cnt = 0;
          
            grr_data->cs.is_mval_initialized        = FALSE;
            break;

          case( NO_SB_FOUND ):
            nc_mval->status                         = NC_MVAL_STAT_PENDING;
            nc_mval->arfcn                          = rrgrr_check_bsic_ind->sync_result[i].arfcn;
            nc_mval->sync_info.bsic                 = RRGRR_INVALID_BSIC;
            nc_mval->sync_info.sync.status          = STAT_SYNC_FAILED;
            nc_mval->sync_info.sync.sync_failed_cnt = 0;
          
            grr_data->cs.is_mval_initialized        = FALSE;
            break;

          case( SB_UNKNOWN ):
            TRACE_EVENT_P1( "cs_compare_bsic: SB_UNKNOWN for %d which is not on measured values list, e.g. after GRR suspension",
                            rrgrr_check_bsic_ind->sync_result[i].arfcn );
            break;
        }
      }
      else
      {
        TRACE_EVENT_P1( "cs_compare_bsic: no free entry for measured values found %d",
                        rrgrr_check_bsic_ind->sync_result[i].arfcn );
      }
    }

#ifdef _SIMULATION_

    TRACE_EVENT_P3( "BSIC: %d %d %d",
                    rrgrr_check_bsic_ind->sync_result[i].arfcn,
                    rrgrr_check_bsic_ind->sync_result[i].sb_flag,
                    rrgrr_check_bsic_ind->sync_result[i].bsic );

#endif /* #ifdef _SIMULATION_ */

    i++;
  }

  cs_trace_nc_mval_lst( );

} /* cs_compare_bsic() */




/*
+------------------------------------------------------------------------------
| Function    : cs_get_best_cell
+------------------------------------------------------------------------------
| Description : The function cs_get_best_cell() checks the start of
|               cell reselection.
|               Is a criterion fulfilled for cell reselction, then reading of
|               best neighbourcell is started.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_get_best_cell ( T_NC_SIX_STRGST **nc, T_scell_info **sc )
{
  UBYTE            strgst_idx;
  UBYTE            i;
  T_NC_SIX_STRGST *next_cell;
  BOOL             c31_bc;     /* best neighbour cell priorized           */
  BOOL             c31_nc;     /* next neighbour cell priorized           */
  BOOL             c31_sc;     /* serving cell priorized                  */
  T_ncell_info    *ncell_best; /* best neighbour cell                     */
  T_ncell_info    *ncell_next; /* next neighbour cell                     */
  T_NC_MVAL       *mval_best;  /* measured values for best neighbour cell */
  T_NC_MVAL       *mval_next;  /* measured values for next neighbour cell */

  TRACE_FUNCTION( "cs_get_best_cell" );

  /*
   * start cell reselection if:
   *
   * 1. C1 for the serving cell falls below zero: C1 < 0
   *
   * 2. A non-serving cell is evaluated to be better than the serving cell.
   *
   *    a) The best cell is the cell with the highest value of C32 among those
   *       cells that have the highest PRIOROTY_CLASS among those that fulfill
   *       the criterion C31 >= 0.
   *       
   *       Interpretation: If C31 is fulfilled, take cells with highest
   *                       priority and then select the cell with highest
   *                       C32 value.
   *
   *    b) The best cell is the cell with the highest value of C32 among all
   *       cells if no cell fulfill the criterion C31 >= 0.
   *
   *       Interpretation: If C31 is not fulfilled, take the cell with the
   *                       highest C32 value.
   *
   * NOTE: When selecting a neighbour cell to be the new serving cell,
   *       C1 criterion of that cell shall be taken into account.
   */

  /*
   * set default results
   */
  *sc        = NULL;
  *nc        = NULL;
  mval_best  = NULL;
  strgst_idx = 0xFF;

  if( grr_data->cs.is_upd_ncell_stat_needed )
  {
    TRACE_EVENT( "cs_get_best_cell: is_upd_ncell_stat_needed EQ TRUE" );

    return;
  }

  if(
      (
        grr_data->db.scell_info.rla_p.stat  NEQ CS_RLA_P_VALID
      )
      OR
      (
        grr_data->db.scell_info.rla_p.stat  EQ  CS_RLA_P_VALID AND
        grr_data->db.scell_info.cr_crit.c1  >=  0              AND
        grr_data->db.scell_info.cell_barred EQ  FALSE
      )
    )
  {
    *sc = &grr_data->db.scell_info;
  }

  /*
   * search the best neighbour cell,
   * this cell should not be used for a cell re-selection attempt before
   */
  for( i = 0; 
       i < grr_data->db.cnt_nc_six.strgst AND *nc EQ NULL;
       i++ )
  {
    *nc        = &grr_data->db.nc_six_strgst[i];
    mval_best  = &grr_data->db.nc_mval_list.nc_mval[(*nc)->idx];
    strgst_idx = i;

    if( (*nc)->mode                      NEQ CS_MODE_IDLE OR
        mval_best->sync_info.sync.status NEQ STAT_SYNC_OK OR
        (*nc)->cr_crit.c1                <   0               )
    {
      *nc        = NULL;
      mval_best  = NULL;
      strgst_idx = 0xFF;
    }
  }

  /*
   * if no best neighbour cell was found inform the caller
   */
  if( *nc EQ NULL OR mval_best EQ NULL )
  {
    return;
  }

  ncell_best = 
    grr_get_ncell_info( mval_best->arfcn, mval_best->sync_info.bsic );

  if( ncell_best EQ NULL )
  {
    TRACE_EVENT( "cs_get_best_cell: ncell_best EQ NULL" );
    *nc        = NULL;
    strgst_idx = 0xFF;

    return;
  }

  /* 
   * check whether best neighbour cell is priorized
   */
  c31_bc = ( (*nc)->cr_crit.c31                    >= 0    AND
             ncell_best->cr_par.cr_par_1.v_hcs_par EQ TRUE     );

#if !defined (NTRACE)

  if( grr_data->cs.v_crp_trace EQ TRUE )
  {
    TRACE_EVENT_P4( "cs_get_best_cell: ncell_best %d %d %d %d",
                    mval_best->arfcn,
                    mval_best->sync_info.bsic,
                    c31_bc,
                    ncell_best->cr_par.cr_par_1.hcs_par.gprs_prio_class );
  }

#endif /* #if !defined (NTRACE) */

  /* 
   * look for a better neighbour cell 
   */
  for( i = 0; i < grr_data->db.cnt_nc_six.strgst; i++ )
  {
    next_cell = &grr_data->db.nc_six_strgst[i];
    mval_next = &grr_data->db.nc_mval_list.nc_mval[next_cell->idx];
    
    if( next_cell->mode                  EQ   CS_MODE_IDLE AND
        next_cell                        NEQ *nc           AND
        mval_next->sync_info.sync.status EQ   STAT_SYNC_OK AND
        next_cell->cr_crit.c1            >=   0                )
    {
      ncell_next = 
        grr_get_ncell_info( mval_next->arfcn, mval_next->sync_info.bsic );

      if( ncell_next EQ NULL )
      {
        TRACE_EVENT( "cs_get_best_cell: ncell_next EQ NULL" );

        *nc        = NULL;
        strgst_idx = 0xFF;

        return;
      }

      /* 
       * check whether next neighbour cell is priorized
       */
      c31_nc = ( next_cell->cr_crit.c31                >= 0    AND
                 ncell_next->cr_par.cr_par_1.v_hcs_par EQ TRUE     );

#if !defined (NTRACE)

      if( grr_data->cs.v_crp_trace EQ TRUE )
      {
        TRACE_EVENT_P4( "cs_get_best_cell: ncell_next %d %d %d %d",
                        mval_next->arfcn,
                        mval_next->sync_info.bsic,
                        c31_nc,
                        ncell_next->cr_par.cr_par_1.hcs_par.gprs_prio_class );
      }

#endif /* #if !defined (NTRACE) */

      if( c31_bc AND
          c31_nc AND
          (
            ( ncell_next->cr_par.cr_par_1.hcs_par.gprs_prio_class EQ
                ncell_best->cr_par.cr_par_1.hcs_par.gprs_prio_class  AND
              next_cell->cr_crit.c32 > (*nc)->cr_crit.c32                )
            OR
            ( ncell_next->cr_par.cr_par_1.hcs_par.gprs_prio_class >
                ncell_best->cr_par.cr_par_1.hcs_par.gprs_prio_class      )
          )
        )
      {
        /*
         * priorized cell reselection must be done
         */
         *nc        = next_cell;
         ncell_best = ncell_next;
         strgst_idx = i;
      }
      else if ( !c31_bc )
      {
        /*
         * check whether next neighbour cell is priorized 
         */
        if( c31_nc )
        {
          /*
           * first priorized cell found
           */
          c31_bc     = TRUE;
          *nc        = next_cell;
          ncell_best = ncell_next;
          strgst_idx = i;
        }
        else if ( next_cell->cr_crit.c32 > (*nc)->cr_crit.c32 )
        {
          /*
           * until now, no priorized cell found
           */
          *nc        = next_cell;
          ncell_best = ncell_next;
          strgst_idx = i;
        }
      }
    }
  }

  /*
   * check whether serving cell is priorized 
   */
  c31_sc = ( grr_data->db.scell_info.cr_crit.c31  >= 0    AND
             psc_db->scell_par.cr_par_1.v_hcs_par EQ TRUE     );

#if !defined (NTRACE)

  if( grr_data->cs.v_crp_trace EQ TRUE )
  {
    TRACE_EVENT_P3( "cs_get_best_cell: scell %d %d %d",
                    *sc, c31_sc,
                    psc_db->scell_par.cr_par_1.hcs_par.gprs_prio_class );
  }

#endif /* #if !defined (NTRACE) */

  /*
   * check whether cell reselection must be done
   */
  if( 
      (
        *sc EQ NULL
      )
      OR
      (
        c31_bc AND
        c31_sc AND
        (
          ( ncell_best->cr_par.cr_par_1.hcs_par.gprs_prio_class EQ
              psc_db->scell_par.cr_par_1.hcs_par.gprs_prio_class     AND
            (*nc)->cr_crit.c32 > grr_data->db.scell_info.cr_crit.c32     )
          OR
          ( ncell_best->cr_par.cr_par_1.hcs_par.gprs_prio_class >
              psc_db->scell_par.cr_par_1.hcs_par.gprs_prio_class         )
        )
      )
      OR
      (
        !c31_bc                                                  AND
        !c31_sc                                                  AND
        (*nc)->cr_crit.c32 > grr_data->db.scell_info.cr_crit.c32
      )
      OR
      (
         c31_bc AND 
        !c31_sc
      )
    )
  {
    *sc = NULL;
  }
  else
  {
    *nc        = NULL;
    strgst_idx = 0xFF;
  }

#if !defined (NTRACE)

  if( grr_data->cs.v_crp_trace EQ TRUE )
  {
    TRACE_EVENT_P3( "cs_get_best_cell: strong/candid %d %d %d",
                    grr_data->db.cnt_nc_six.strgst,
                    grr_data->db.cnt_nc_six.candid,
                    strgst_idx );
  }

#endif /* #if !defined (NTRACE) */

} /* cs_get_best_cell() */


/*
+------------------------------------------------------------------------------
| Function    : cs_get_network_ctrl_order
+------------------------------------------------------------------------------
| Description : The NETWORK_CONTROL_ORDER values NC1 and NC2 shall only apply
|               in Ready state. In Standby state, the MS shall always use 
|               normal MS control independent of the ordered NC mode.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL T_NC_ORDER cs_get_network_ctrl_order ( BOOL consider_gmm_state )
{
  T_NC_ORDER ctrl_order = NC_NC0;

  TRACE_FUNCTION( "cs_get_network_ctrl_order" );

  if( grr_data->nc2_on )
  { 
    if(
        (
          (
            consider_gmm_state     EQ TRUE        AND
            grr_data->cs.gmm_state EQ READY_STATE 
          ) 
          OR
          (
            consider_gmm_state     EQ FALSE
          )
        )
        AND
        (
          grr_data->db.nc_ref_lst.param NEQ NULL 
        )
      )
    {
      ctrl_order = grr_data->db.nc_ref_lst.param->ctrl_order;
    }
  }

#ifdef _SIMULATION_

  TRACE_EVENT_P2( "NETWORK_CONTROL_ORDER: %d, GMM_STATE: %d",
                  grr_data->db.nc_ref_lst.param EQ NULL ?
                    ctrl_order : grr_data->db.nc_ref_lst.param->ctrl_order,
                  grr_data->cs.gmm_state );

#endif /* #ifdef _SIMULATION_ */

  return( ctrl_order );

} /* cs_get_network_ctrl_order() */

/*
+------------------------------------------------------------------------------
| Function    : cs_reset_nc_change_mark
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_reset_nc_change_mark ( T_SC_DATABASE *db )
{
  TRACE_FUNCTION( "cs_reset_nc_change_mark" );

  db->nc_cw.list.chng_mrk.curr         = 0;
  db->nc_cw.list.chng_mrk.prev         = 0;
  db->nc_cw.param.chng_mrk.curr        = 0;
  db->nc_cw.param.chng_mrk.prev        = 0;
  db->nc_ms.ncmeas.list.chng_mrk.curr  = 0;
  db->nc_ms.ncmeas.list.chng_mrk.prev  = 0;
  db->nc_ms.ncmeas.param.chng_mrk.curr = 0;
  db->nc_ms.ncmeas.param.chng_mrk.prev = 0;

} /* cs_reset_nc_change_mark() */

/*
+------------------------------------------------------------------------------
| Function    : cs_get_cr_meas_mode
+------------------------------------------------------------------------------
| Description : The function cs_get_cr_meas_mode() returns the cell
|               re-selection measurement mode used in context of TBF 
|               establishment.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE cs_get_cr_meas_mode ( void )
{
  TRACE_FUNCTION( "cs_get_cr_meas_mode" );

  return( grr_is_pbcch_present() ? CS_CRMM_BA_GPRS : CS_CRMM_BA_BCCH );
} /* cs_get_cr_meas_mode() */

/*
+------------------------------------------------------------------------------
| Function    : cs_send_cr_meas_req
+------------------------------------------------------------------------------
| Description : The function cs_send_cr_meas_req() sends the cell reselection
|               measurement request to L1 (MPHP_CR_MEAS_REQ)
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_send_cr_meas_req ( UBYTE del_meas_rslt )
{
  BOOL           is_pbcch_present = grr_is_pbcch_present( );
  UBYTE          i, j,number_of_frequencies;
  BOOL           is_existing;
  BOOL           v_cs_par[MAX_NR_OF_NCELL];
  T_NC_ORDER     nc_ord  = cs_get_network_ctrl_order( TRUE );
  T_NC_REF_LIST *nc_list = &grr_data->db.nc_ref_lst;

  TRACE_FUNCTION( "cs_send_cr_meas_req" );

  {   
    /*
     * first value represents the arfcn of the serving cell
     */
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    grr_data->nc_data.cell[0].arfcn =  psc_db->pbcch.bcch.arfcn;
#else
    grr_data->nc_data.cell[0].arfcn =  grr_g23_arfcn_to_l1( psc_db->pbcch.bcch.arfcn );
#endif
    number_of_frequencies           = 1;
    
    memset( v_cs_par, ( is_pbcch_present ? FALSE : TRUE ), sizeof( v_cs_par ) );

    for( i = 0; i < nc_list->number; i++ )
    {
      if( is_pbcch_present )
      {
        v_cs_par[i] |= nc_list->info[i]->v_cr_par;
      }
      
      for( j = i + 1, is_existing = FALSE;
           j < nc_list->number AND is_existing EQ FALSE; 
           j++ )
      {
        if( nc_list->info[j]->arfcn EQ nc_list->info[i]->arfcn )
        {
          is_existing  = TRUE;

          if( is_pbcch_present )
          {
            v_cs_par[j] |= nc_list->info[i]->v_cr_par;
          }
        }
      }

      if( !is_existing                                                  AND
          ( nc_ord EQ NC_NC2 OR ( nc_ord NEQ NC_NC2 AND v_cs_par[i] ) )     )
      {
#if defined (REL99) AND defined (TI_PS_FF_EMR)
        grr_data->nc_data.cell[number_of_frequencies].arfcn = nc_list->info[i]->arfcn;
#else
        grr_data->nc_data.cell[number_of_frequencies].arfcn = 
            grr_g23_arfcn_to_l1( nc_list->info[i]->arfcn );
#endif
        number_of_frequencies++;
      }
    } 
         
    if( del_meas_rslt EQ CS_DELETE_MEAS_RSLT )
    {
      cs_reuse_old_cell_rxlev( );
    }
    grr_data->nc_data.c_cell = number_of_frequencies;
    cs_req_cr_meas( );
  }
} /* cs_send_cr_meas_req() */

/*
+------------------------------------------------------------------------------
| Function    : cs_send_cr_meas_stop_req
+------------------------------------------------------------------------------
| Description : The function cs_send_cr_meas_stop_req() sends the cell
|               reselection measurement stop request to L1
|               (MPHP_CR_MEAS_STOP_REQ)
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_send_cr_meas_stop_req ( void )
{
  TRACE_FUNCTION( "cs_send_cr_meas_stop_req" );


  grr_data->nc_data.c_cell   = 0;
  grr_data->cs.cr_meas_mode  = cs_get_cr_meas_mode( );
  cs_stop_cr_meas( );

} /* cs_send_cr_meas_stop_req() */

/*
+------------------------------------------------------------------------------
| Function    : cs_send_update_ba_req
+------------------------------------------------------------------------------
| Description : The function cs_send_update_ba_req() updates the BA(BCCH) list
|               by passing the lists of added and removed frequencies.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_send_update_ba_req ( T_SC_DATABASE *db )
{
  UBYTE            i, j;     /* used for counting */
  BOOL             is_found;
  T_add_freq_list *afreq;
  T_ncell_info    *ncinfo;
  T_NC_ORDER       ctrl_order = cs_get_network_ctrl_order( FALSE );

  TRACE_FUNCTION( "cs_send_update_ba_req" );

  {
    PALLOC( rrgrr_upd_ba_req, RRGRR_UPDATE_BA_REQ );

    /* process cell type */
    rrgrr_upd_ba_req->cell_type = SYNC_SCELL;

    /* process NC mode */
    switch( ctrl_order )
    {
      case NC_NC2:   rrgrr_upd_ba_req->nc_mode = NC2_USED;     break;
      case NC_RESET: rrgrr_upd_ba_req->nc_mode = NC_RESET;     break;
      default    :   rrgrr_upd_ba_req->nc_mode = NC2_NOT_USED; break;
    }

    /* process list of removed frequencies */
    if( db->nc_ms.rfreq.number > RRGRR_BA_LIST_SIZE )
    {
      TRACE_ERROR ( "Number of removed frequencies > RRGRR_BA_LIST_SIZE" );
    }

    i = 0;
    while( i < db->nc_ms.rfreq.number AND i < RRGRR_BA_LIST_SIZE )
    {
      j        = 0;
      is_found = FALSE;
      while( j < db->nc_cw.list.number AND is_found EQ FALSE )
      {
        if( db->nc_cw.list.info[j].index EQ db->nc_ms.rfreq.idx[i] )
        {
          is_found = TRUE;
        }
        else
        {
          j++;
        }
      }

      if( is_found EQ TRUE )
      {
        rrgrr_upd_ba_req->rm_freq_list[i].arfcn = db->nc_cw.list.info[j].arfcn;
        rrgrr_upd_ba_req->rm_freq_list[i].bsic  = db->nc_cw.list.info[j].bsic;
      }
      else
      {
        TRACE_ERROR( "Removed frequency cannot be found in nc_cw structure" );
      }

      i++;
    }

    if( i < RRGRR_BA_LIST_SIZE )
    {
      rrgrr_upd_ba_req->rm_freq_list[i].arfcn = RRGRR_INVALID_ARFCN;
      rrgrr_upd_ba_req->rm_freq_list[i].bsic  = RRGRR_INVALID_BSIC;
    }

#ifdef _SIMULATION_ 

    i++;
    while( i < RRGRR_BA_LIST_SIZE )
    {
      rrgrr_upd_ba_req->rm_freq_list[i].arfcn = RRGRR_INVALID_ARFCN;
      rrgrr_upd_ba_req->rm_freq_list[i].bsic  = RRGRR_INVALID_BSIC;
      i++;
    }

#endif /* #ifdef _SIMULATION_  */

    /* process list of added frequencies */
    if( db->nc_ms.ncmeas.list.number > RRGRR_BA_LIST_SIZE )
    {
      TRACE_ERROR ( "Number of added frequencies > RRGRR_BA_LIST_SIZE" );
    }

    i = 0;
    while( i < db->nc_ms.ncmeas.list.number AND i < RRGRR_BA_LIST_SIZE )
    {
      afreq        = &rrgrr_upd_ba_req->add_freq_list[i];
      ncinfo       = &db->nc_ms.ncmeas.list.info[i];

      afreq->arfcn = ncinfo->arfcn;
      afreq->bsic  = ncinfo->bsic;

      if( ncinfo->v_cr_par EQ TRUE )
      {
        afreq->v_cr_par = TRUE;

        afreq->cr_par.cell_bar_access_2       = ncinfo->cr_par.cell_ba;
        afreq->cr_par.exc_acc                 = ncinfo->cr_par.exc_acc;
        afreq->cr_par.same_ra_as_serving_cell = ncinfo->cr_par.same_ra_scell;
        afreq->cr_par.gprs_rxlev_access_min   = ncinfo->cr_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min;
        afreq->cr_par.gprs_ms_txpwr_max_cch   = ncinfo->cr_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch;
        afreq->cr_par.gprs_temporary_offset   = ncinfo->cr_par.cr_offset.gprs_temp_offset;
        afreq->cr_par.gprs_penalty_time       = ncinfo->cr_par.cr_offset.gprs_penalty_time;
        afreq->cr_par.gprs_reselect_offset    = ncinfo->cr_par.gprs_resel_off;
        afreq->cr_par.priority_class          = ncinfo->cr_par.cr_par_1.hcs_par.gprs_prio_class;
        afreq->cr_par.hcs_thr                 = ncinfo->cr_par.cr_par_1.hcs_par.gprs_hcs_thr;
        afreq->cr_par.si13_location           = SI13_LOC_INVALID;
        afreq->cr_par.pbcch_location          = PBCCH_LOC_INVALID;
        afreq->cr_par.psi1_repeat_period      = PSI1_RPT_PRD_INVALID;

        if( ncinfo->cr_par.v_si13_pbcch )
        {
          if( ncinfo->cr_par.si13_pbcch.v_si13_location )
          {
            afreq->cr_par.si13_location       = ncinfo->cr_par.si13_pbcch.si13_location;
          }
          else
          {
            afreq->cr_par.pbcch_location      = ncinfo->cr_par.si13_pbcch.pbcch_location;
            afreq->cr_par.psi1_repeat_period  = ncinfo->cr_par.si13_pbcch.psi1_repeat_period - 1;
          }
        }
      }
      else
      {
        afreq->v_cr_par = FALSE;
      }

      i++;
    }

    if( i < RRGRR_BA_LIST_SIZE )
    {
      rrgrr_upd_ba_req->add_freq_list[i].arfcn = RRGRR_INVALID_ARFCN;
      rrgrr_upd_ba_req->add_freq_list[i].bsic  = RRGRR_INVALID_BSIC;
    }

#ifdef _SIMULATION_ 

    i++;
    while( i < RRGRR_BA_LIST_SIZE )
    {
      rrgrr_upd_ba_req->add_freq_list[i].arfcn = RRGRR_INVALID_ARFCN;
      rrgrr_upd_ba_req->add_freq_list[i].bsic  = RRGRR_INVALID_BSIC;
      i++;
    }

#endif /* #ifdef _SIMULATION_  */

    PSEND( hCommRR, rrgrr_upd_ba_req );
  }
} /* cs_send_update_ba_req() */

/*
+------------------------------------------------------------------------------
| Function    : cs_send_meas_rep_req
+------------------------------------------------------------------------------
| Description : The function cs_send_meas_rep_req() updates RR with cause 
|               NC_START_MEAS or NC_REPORT_MEAS or NC_STOP_MEAS.
|
| Parameters  : NC_START_MEAS  - start the measurement
|               NC_REPORT_MEAS - report the measurement
|               NC_STOP_MEAS   - stop the measurement
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_send_meas_rep_req ( UBYTE cause )
{
  TRACE_FUNCTION( "cs_send_meas_rep_req" );

  {
    PALLOC( rrgrr_meas_rep_req, RRGRR_MEAS_REP_REQ );

    rrgrr_meas_rep_req->meas_cause = grr_data->cs_meas.nc_meas_cause = cause;

    PSEND( hCommRR, rrgrr_meas_rep_req );
  }
} /* cs_send_meas_rep_req() */

/*
+------------------------------------------------------------------------------
| Function    : cs_store_meas_rep_cnf
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_store_meas_rep_cnf ( T_RRGRR_MEAS_REP_CNF *rrgrr_meas_rep_cnf )
{ 
  UBYTE      i;        /* used for counting */
  UBYTE      mval_idx;
  T_NC_MVAL *mval;

  TRACE_FUNCTION( "cs_store_meas_rep_cnf" );

  /*
   * store the first value as serving cell
   */
  grr_data->db.scell_info.rxlev_avg = rrgrr_meas_rep_cnf->meas_res[0].rxlev;
  
  /*
   * store the next values as neighbour cells
   */
  cs_init_nc_mval_lst( );

  for( i = 1;
       i                                     <   RRGRR_MEAS_REP_LIST_SIZE AND 
       rrgrr_meas_rep_cnf->meas_res[i].arfcn NEQ RRGRR_INVALID_ARFCN;
       i++ )
  {
    mval = grr_get_nc_mval( RRGRR_INVALID_ARFCN, RRGRR_INVALID_BSIC, &mval_idx );
  
    if( mval NEQ NULL )
    {
      mval->status                = NC_MVAL_STAT_ASSIGNED;
      mval->arfcn                 = rrgrr_meas_rep_cnf->meas_res[i].arfcn;
      mval->rla_p.stat            = CS_RLA_P_VALID;
      mval->rla_p.lev             = 
      mval->rxlev_avg             = rrgrr_meas_rep_cnf->meas_res[i].rxlev;
      mval->sync_info.bsic        = rrgrr_meas_rep_cnf->meas_res[i].bsic;
      mval->sync_info.sync.status = STAT_SYNC_OK;

      grr_data->cs.is_mval_initialized = FALSE;
    }
    else
    {
      TRACE_EVENT_P2( "No free entry for measured value found: %d %d",
                      rrgrr_meas_rep_cnf->meas_res[i].arfcn,
                      rrgrr_meas_rep_cnf->meas_res[i].bsic );
    }     
  }
} /* cs_store_meas_rep_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : cs_build_strongest
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_build_strongest ( void )
{
  UBYTE i;
  UBYTE count = 0;
  UBYTE index[CS_MAX_STRONG_CARRIER];

  TRACE_FUNCTION( "cs_build_strongest" );

  cs_find_strongest_with_status
    ( &count, index, CS_RLA_P_VALID,
      (UBYTE)( CS_MAX_STRONG_CARRIER - count ), CS_NO_BAND_LIMITATION );

  for( i = 0; i < count; i++ )
  {
    grr_data->db.nc_six_strgst[i].idx = index[i];
  } 

  cs_set_cnt_nc_six( count, count );

} /* cs_build_strongest() */

/*
+------------------------------------------------------------------------------
| Function    : cs_send_meas_rpt
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL cs_send_meas_rpt ( BOOL perform_init )
{
  BOOL            rpt_snd;
  T_U_MEAS_REPORT meas_rpt;

  TRACE_FUNCTION( "cs_send_meas_rpt" );

  if( perform_init EQ TRUE )
  {
    grr_data->cs_meas.pmr_snd_ref = 0;
  }

  memset(&meas_rpt, 0, sizeof(T_U_MEAS_REPORT));

  rpt_snd = cs_build_meas_rpt( &meas_rpt );

  if( rpt_snd EQ TRUE )
  {
    sig_cs_ctrl_meas_report( &meas_rpt );
  }

  return( rpt_snd );
} /* cs_send_meas_rpt() */


/*
+------------------------------------------------------------------------------
| Function    : cs_start_t_reselect
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_start_t_reselect ( void )
{
  TRACE_FUNCTION( "cs_start_t_reselect" );

  if( grr_t_status( T_RESELECT ) EQ 0 )
  {
    vsi_t_start( GRR_handle, T_RESELECT, T_RESELECT_VALUE );

    TRACE_EVENT( "Timer T_RESELECT started" );
  }
} /* cs_start_t_reselect() */

/*
+------------------------------------------------------------------------------
| Function    : cs_stop_t_reselect
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_stop_t_reselect ( void )
{
  TRACE_FUNCTION( "cs_stop_t_reselect" );

  if( grr_t_status( T_RESELECT ) NEQ 0 )
  {
    vsi_t_stop( GRR_handle, T_RESELECT );

    TRACE_EVENT( "Timer T_RESELECT stopped" );
  }
} /* cs_stop_t_reselect() */

/*
+------------------------------------------------------------------------------
| Function    : cs_process_t3158
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_process_t3158 ( void )
{
  T_TIME reporting_period;
  T_TIME time_to_go;
  BOOL   tmr_3158_rng_le_rp = FALSE;

  TRACE_FUNCTION( "cs_process_t3158" );

  if( cs_is_meas_reporting( ) )
    /* Implies NC measurement conditions are satisfied. May mean Enhanced 
       measurement conditions are also satisfied */
  {
    switch( grr_data->cs_meas.packet_mode )
    {
      case( PACKET_MODE_PIM ):
      case( PACKET_MODE_PAM ):
        reporting_period = cs_get_nc_rpt_prd_idle( );
        break;

      case( PACKET_MODE_PTM ):
        /*
         * In packet transfer mode, the reporting period is indicated in 
         * NC_REPORTING_PERIOD_T.
         */
        reporting_period = 
                    GET_NC_RPT_PRD( grr_data->db.nc_ref_lst.param->rep_per_t );
        break;

      default:
        if( GET_STATE( CS ) EQ CS_CR_NETWORK )
        {
          /*
           * In case timer T3158 expires during processing of Packet Cell Change
           * Order, the timer should be re-started and not stopped at all.
           */
          reporting_period = cs_get_nc_rpt_prd_idle( );
        }
        else
        {  
          reporting_period = 0;
        }
        break;
    }

    time_to_go = grr_t_status( T3158 );
    
    if( time_to_go > 0 )
    {
      if( time_to_go > reporting_period )
      {
        cs_stop_t3158( );
      
        if(  grr_is_pbcch_present( ) EQ FALSE      AND
             grr_data->cs_meas.nc_meas_cause NEQ NC_STOP_MEAS   )
        {
           cs_send_meas_rep_req ( NC_STOP_MEAS ); 
        }
      }
      else
      {
        tmr_3158_rng_le_rp = TRUE;
      }
    }

    if( reporting_period NEQ 0 )     
    {
      if( tmr_3158_rng_le_rp EQ FALSE )
      {
        cs_reset_all_rxlev_results( );   
        cs_start_t3158( reporting_period );
        /* 
         * Start rx_lev averaging when NC=1 or NC=2 in ALR 
         */
        if(  grr_is_pbcch_present( ) EQ FALSE     AND
             grr_data->cs_meas.nc_meas_cause EQ NC_STOP_MEAS   ) 
        {
          cs_send_meas_rep_req ( NC_START_MEAS ) ;
        }
      }
    }
    else
    {
      cs_reset_all_rxlev_results( );
    }
  }
  else
  {
    cs_reset_all_rxlev_results( );
    cs_stop_t3158( );
    /* 
     * Stop rx_lev averaging when NC=1 or NC=2 in ALR 
     */
    if(  grr_is_pbcch_present( ) EQ FALSE      AND
         grr_data->cs_meas.nc_meas_cause NEQ NC_STOP_MEAS   )
    {
      cs_send_meas_rep_req ( NC_STOP_MEAS ); 
    }
  }
} /* cs_process_t3158() */

/*
+------------------------------------------------------------------------------
| Function    : cs_start_t3158
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_start_t3158 ( T_TIME time )
{
  TRACE_FUNCTION( "cs_start_t3158" );

  vsi_t_start( GRR_handle, T3158, time );

  TRACE_EVENT_P1( "T3158: %d", time );

} /* cs_start_t3158() */

/*
+------------------------------------------------------------------------------
| Function    : cs_stop_t3158
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_stop_t3158 ( void )
{
  TRACE_FUNCTION( "cs_stop_t3158" );

  if( grr_t_status( T3158 ) > 0 )
  {
    vsi_t_stop( GRR_handle, T3158 );

    TRACE_EVENT( "T3158 stopped" );
  }
} /* cs_stop_t3158() */

/*
+------------------------------------------------------------------------------
| Function    : cs_cancel_meas_report
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_cancel_meas_report ( void )
{
  TRACE_FUNCTION( "cs_cancel_meas_report" );

  switch( GET_STATE( CS_MEAS ) )
  {
    case CS_MEAS_REP_REQ:
      SET_STATE( CS_MEAS, CS_NULL );
      break;
    case CS_MEAS_PMR_SENDING:
      sig_cs_ctrl_cancel_meas_report( );
      break;
    default:
      /* do nothing */
      break;
  }
} /* cs_cancel_meas_report() */

/*
+------------------------------------------------------------------------------
| Function    : cs_build_nc_ref_list
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : db - pointer to serving cell database
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_build_nc_ref_list ( T_SC_DATABASE *db, BOOL is_dsf )
{ 
  TRACE_FUNCTION( "cs_build_nc_ref_list" );
  
  cs_init_nc_ref_list( &grr_data->db.nc_ref_lst );

  if( 
      grr_data->cs.gmm_state EQ READY_STATE AND 
      is_dsf                 EQ FALSE       AND 
      (
        db->nc_ms.ncmeas.param.ctrl_order EQ NC_NC0 OR
        db->nc_ms.ncmeas.param.ctrl_order EQ NC_NC1 OR
        db->nc_ms.ncmeas.param.ctrl_order EQ NC_NC2         
      ) 
    )
  {
    grr_data->db.nc_ref_lst.param = &db->nc_ms.ncmeas.param;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
    grr_data->db.nc_ref_lst.enh_param = &db->enh_ms;
#endif
  }
  else if( db->nc_cw.param.ctrl_order EQ NC_NC0 OR
           db->nc_cw.param.ctrl_order EQ NC_NC1 OR
           db->nc_cw.param.ctrl_order EQ NC_NC2    )
  {
    grr_data->db.nc_ref_lst.param = &db->nc_cw.param;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
    grr_data->db.nc_ref_lst.enh_param = &db->enh_cw;
#endif
  }

  cs_build_nc_freq_list( db );
  cs_reorg_nc_mval_lst( );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
   if( !grr_is_pbcch_present() AND 
    grr_data->db.nc_ref_lst.enh_param->rept_type EQ REPORT_TYPE_ENH_REP)
  {
    cs_init_ba_bcch_nc_ref_list();
    cs_build_ba_bcch_nc_freq_list();
  }
#endif

}/* cs_build_nc_ref_list */

/*
+------------------------------------------------------------------------------
| Function    : cs_init_nc_ref_list
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : list  - pointer to NC measurement pointer list
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_init_nc_ref_list ( T_NC_REF_LIST *list )
{ 
  TRACE_FUNCTION( "cs_init_nc_ref_list" );
  
  list->number = 0;
  list->param  = NULL;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  list->enh_param = NULL;
#endif

}/* cs_init_nc_ref_list */


/*
+------------------------------------------------------------------------------
| Function    : cs_init_nc_mval_lst
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_init_nc_mval_lst ( void )
{ 
  UBYTE i;

  TRACE_FUNCTION( "cs_init_nc_mval_lst" );
  
  if( grr_data->cs.is_mval_initialized EQ FALSE )
  {
    for( i = 0; i < MAX_NR_OF_NC_MVAL; i++ )
    {
      cs_init_nc_mval( &grr_data->db.nc_mval_list.nc_mval[i] );
    }

    grr_data->cs.is_mval_initialized = TRUE;
  
#if !defined (NTRACE)

    if( grr_data->cs.v_crp_trace EQ TRUE )
    {
      TRACE_EVENT( "cs_init_nc_mval_lst: now initialized" );
    }

#endif /* #if !defined (NTRACE) */

  }
  else
  {

#if !defined (NTRACE)

    if( grr_data->cs.v_crp_trace EQ TRUE )
    {
      TRACE_EVENT( "cs_init_nc_mval_lst: already initialized" );
    }

#endif /* #if !defined (NTRACE) */

  }
}/* cs_init_nc_mval_lst */

/*
+------------------------------------------------------------------------------
| Function    : cs_init_nc_mval
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : mval - pointer to NC measured value
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_init_nc_mval ( T_NC_MVAL *mval )
{ 
  TRACE_FUNCTION( "cs_init_nc_mval" );
  
  mval->arfcn                          = RRGRR_INVALID_ARFCN;
  mval->sync_info.bsic                 = RRGRR_INVALID_BSIC;
  mval->sync_info.sync.status          = STAT_SYNC_NONE;
  mval->sync_info.sync.sync_failed_cnt = 0;
  mval->rla_p.stat                     = CS_RLA_P_NOT_AVAIL;
  mval->rxlev_avg                      = RXLEV_AVG_INVALID;
  mval->status                         = NC_MVAL_STAT_NONE;
}/* cs_init_nc_mval */


/*
+------------------------------------------------------------------------------
| Function    : cs_update_bsic
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_update_bsic ( void )
{ 
  UBYTE i 
#if defined (REL99) AND defined (TI_PS_FF_EMR)
        ,j, k
#endif
        ;
        /* used for counting */
#if defined (TI_PS_FF_RTD) AND defined (REL99)
  UBYTE i_max,rtd_index,rtd_count;
#else 
  UBYTE i_max;
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */


  TRACE_FUNCTION( "cs_update_bsic" );

  i     = 0;
  i_max = MINIMUM( grr_data->db.cnt_nc_six.candid, CS_MAX_STRONG_CARRIER );
  i_max = MINIMUM( i_max, RRGRR_MAX_ARFCN_NCELL_SYNC_REQ );

  if( i_max NEQ 0 )
  {
    PALLOC( rrgrr_check_bsic_req, RRGRR_NCELL_SYNC_REQ );

    while( i < i_max )
    {
#if defined (TI_PS_FF_RTD) AND defined (REL99)
      rrgrr_check_bsic_req->ncell_sync_list[i].arfcn = grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[i].idx].arfcn;   
#else 
      rrgrr_check_bsic_req->arfcn[i] = grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[i].idx].arfcn;
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */


      i++;
    }
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  /* Extend the ARFCN list only if ENH reporting is requested */
  if(cs_is_enh_meas_reporting())
  {
    T_ENH_STRNG_ARFCN_LIST f_bin;/* Stores the ARFCN-RXLEV pair in descending
                                    order of RXLEV */
    BOOL found; /* Will be set, if ARFCN in f_bin is found in NC_MVAL */

    memset(&f_bin, 0, sizeof(T_ENH_STRNG_ARFCN_LIST));

    /* Form a ARFCN-RXLEV pair list in descending order of RXLEV */
    cs_build_enh_sorted_strng_arfcn_list(&f_bin);
    
    /* Fill the NCELL_SYNC_REQ with ARFCNs from the above list. This list
       will be filled without disturbing the six strongest cells for
       cell reselection or PMR. The request for SYNC is now done for a
       max of 12 cells with 1st few cells upto a max of six for cell
       reselection and the remaining will be filled with ARFCNs from
       the ENH list in the order of serving band, multiband and the rest
       contains the excess from the above bands, each group containing
       cells in descending order of RXLEV */
    j = 0;
    while(i_max < RRGRR_MAX_ARFCN_NCELL_SYNC_REQ AND j < f_bin.num)
    {
      found = FALSE;
      for(k = 0; k < grr_data->db.cnt_nc_six.candid; k++)
      {
        if(f_bin.meas[j].arfcn EQ 
          grr_data->db.nc_mval_list.nc_mval[grr_data->db.nc_six_strgst[k].idx].arfcn)
        {
          found = TRUE;
          break;
        }
      }
      
      if(!found)
      {
#if defined (TI_PS_FF_RTD) AND defined (REL99)
          rrgrr_check_bsic_req->ncell_sync_list[i_max].arfcn = f_bin.meas[j].arfcn;
#else 
          rrgrr_check_bsic_req->arfcn[i_max] = f_bin.meas[j].arfcn;
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */

        i_max++;
      }
      j++;
    }
  }
  
  if( i_max < RRGRR_MAX_ARFCN_NCELL_SYNC_REQ )
  {
#if defined (TI_PS_FF_RTD) AND defined (REL99)
      rrgrr_check_bsic_req->ncell_sync_list[i_max].arfcn = RRGRR_INVALID_ARFCN;
#else 
      rrgrr_check_bsic_req->arfcn[i_max] = RRGRR_INVALID_ARFCN;
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */
    } /*if*/
#if defined (TI_PS_FF_RTD) AND defined (REL99)
    i=0;
    while (i < i_max)
    {
      j = 0;
      rtd_count = 0;
      /*More than 3 RTD values is not acceptable */
      while( j < grr_data->db.nc_ref_lst.number AND rtd_count < MAX_NUM_OF_RTD_VALUES )
      {
        /* Check the NC ARFCN in the BA-list to get the index of that ARFCN in BA-list*/
        if(rrgrr_check_bsic_req->ncell_sync_list[i].arfcn EQ (grr_data->db.nc_ref_lst.info[j])->arfcn)
        {
          /* Pickup the right RTD value for that ARFCN */
          rtd_index = (grr_data->db.nc_ref_lst.info[j])->index;
          /*Maximum number of ncell is 32 if it exceeds then store RTD_NOT_AVAILABLE for that ARFCN*/
          if(rtd_index < MAX_NR_OF_NCELL AND psc_db->rtd[rtd_index] NEQ RTD_NOT_AVAILABLE )
          {
            rrgrr_check_bsic_req->ncell_sync_list[i].v_rtd          = 1;
            rrgrr_check_bsic_req->ncell_sync_list[i].rtd[rtd_count] = psc_db->rtd[rtd_index];
            rtd_count++;
          }/*if*/
        } /*if*/
        j++;
      } /*while*/
      rrgrr_check_bsic_req->ncell_sync_list[i].c_rtd = rtd_count;
      i++;
    } /*while*/
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */


#ifdef _SIMULATION_

  i_max = i_max + 1;
  while( i_max < RRGRR_MAX_ARFCN_NCELL_SYNC_REQ )
  {
#if defined (TI_PS_FF_RTD) AND defined (REL99)
      rrgrr_check_bsic_req->ncell_sync_list[i_max].arfcn = RRGRR_INVALID_ARFCN;
#else 
      rrgrr_check_bsic_req->arfcn[i_max] = RRGRR_INVALID_ARFCN;
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */

    i_max++;
  }
#endif
#else

    if( i < RRGRR_MAX_ARFCN_NCELL_SYNC_REQ )
    {
      rrgrr_check_bsic_req->arfcn[i] = RRGRR_INVALID_ARFCN;
    }

#ifdef _SIMULATION_

    i = i + 1;
    while( i < RRGRR_MAX_ARFCN_NCELL_SYNC_REQ )
    {
      rrgrr_check_bsic_req->arfcn[i] = RRGRR_INVALID_ARFCN;
      i++;
    }

#endif /* #ifdef _SIMULATION_ */

#endif /* REL99 AND TI_PS_FF_EMR */

    rrgrr_check_bsic_req->sync_type = SYNC_INITIAL;
    PSEND( hCommRR, rrgrr_check_bsic_req );
  }
}/* cs_update_bsic */


/*
+------------------------------------------------------------------------------
| Function    : cs_find_candidate
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL cs_find_candidate ( void )
{ 
  BOOL          is_new_candidate = FALSE;
  T_scell_info *sc;

  TRACE_FUNCTION( "cs_find_candidate" );

  if( grr_data->cs.reselect_cause   EQ CS_RESELECT_CAUSE_CTRL_ABNORMAL AND
      psc_db->gen_cell_par.rab_acc_re EQ GRR_RA_RETRY_DISABLED               )
  {
    grr_data->db.cr_cell = NULL;

#if !defined (NTRACE)

      if( grr_data->cs.v_crp_trace EQ TRUE )
      {
        TRACE_EVENT( "no candidate found, RA retry bit for abnormal cell re-selection not set" );
      }

#endif /* #if !defined (NTRACE) */

  }
  else if( grr_data->cs.reselect_cause EQ CS_RESELECT_CAUSE_CTRL_SCELL )
  {
    if( cs_create_cr_cell( psc_db->pbcch.bcch.arfcn, 
                           psc_db->pbcch.bcch.bsic ) NEQ NULL )
    {

#if !defined (NTRACE)

      if( grr_data->cs.v_crp_trace EQ TRUE )
      {
        TRACE_EVENT( "serving cell found as candidate" );
      }

#endif /* #if !defined (NTRACE) */

      /*
       * the cell re-selection process will be started
       */
      is_new_candidate = TRUE;
    }
  }
  else
  {
    cs_get_best_cell( &grr_data->db.cr_cell, &sc );

    if( grr_data->db.cr_cell NEQ NULL )
    {

#if !defined (NTRACE)

      if( grr_data->cs.v_crp_trace EQ TRUE )
      {
        TRACE_EVENT( "candidate found" );
      }

#endif /* #if !defined (NTRACE) */
  
      /*
       * the cell re-selection process will be started
       */
      is_new_candidate = TRUE;
    }
    else if( sc NEQ NULL )
    {

#if !defined (NTRACE)

      if( grr_data->cs.v_crp_trace EQ TRUE )
      {
        TRACE_EVENT( "no candidate found, serving cell good" );
      }

#endif /* #if !defined (NTRACE) */

      /*
       * the serving cell becomes good again or is still good
       */
      cs_stop_t_reselect( );
    }
    else
    {

#if !defined (NTRACE)

      if( grr_data->cs.v_crp_trace EQ TRUE )
      {
        TRACE_EVENT( "no candidate found, serving cell bad" );
      }

#endif /* #if !defined (NTRACE) */

      /*
       * the serving cell becomes bad but 
       * no suitable neighbour cell is available
       */
      cs_start_t_reselect( );
    }
  }

  return( is_new_candidate );
}/* cs_find_candidate */

/*
+------------------------------------------------------------------------------
| Function    : cs_cr_decision
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL cs_cr_decision ( T_CRDM mode )
{ 
  T_NC_ORDER ctrl_order; 
  BOOL       cr_initiated = FALSE;

  TRACE_FUNCTION( "cs_cr_decision" );

  switch( mode )
  {
    case( CRDM_T_RESELECT ):
      sig_cs_ctrl_no_more_candidate( grr_data->cs.reselect_cause );
    
      cr_initiated = TRUE;
      break;

    case( CRDM_CR_CTRL ):
      ctrl_order = cs_get_network_ctrl_order( TRUE );

      if( ctrl_order                  EQ NC_NC0                       OR
          ctrl_order                  EQ NC_NC1                       OR
          grr_data->cs.reselect_cause EQ CS_RESELECT_CAUSE_CTRL_SCELL            OR 
          ( grr_data->cs.reselect_cause EQ CS_RESELECT_CAUSE_CTRL_DL_SIG_FAIL    AND 
            ctrl_order                  EQ NC_NC2 ) )
      {
        /*
         * Only in case the NETWORK_CONTROL_ORDER is either equal to
         * NC0 or NC1 the MS shall perform automomous cell re-selection
         */

        if( cs_find_candidate( ) )
        {
          sig_cs_ctrl_new_candidate( grr_data->cs.reselect_cause );
        }
        else
        {
          sig_cs_ctrl_no_more_candidate( grr_data->cs.reselect_cause );
        }
      }
      else
      {
        sig_cs_ctrl_no_more_candidate( grr_data->cs.reselect_cause );
      }
      
      cr_initiated = TRUE;
      break;

    case( CRDM_CR_INITIAL ):
      ctrl_order = cs_get_network_ctrl_order( TRUE );

      if( ctrl_order EQ NC_NC0 OR ctrl_order EQ NC_NC1 )
      {
        /*
         * Only in case the NETWORK_CONTROL_ORDER is either equal to
         * NC0 or NC1 the MS shall perform automomous cell re-selection
         */
    
        grr_data->db.cr_cell = NULL;

        cs_calc_params( );

        if( cs_find_candidate( ) )
        {
          sig_cs_ctrl_new_candidate( grr_data->cs.reselect_cause );

          cr_initiated = TRUE;
        }
      }
      break;

    case( CRDM_CR_CONT ):
      if( cs_find_candidate( ) )
      {
        sig_cs_ctrl_new_candidate( grr_data->cs.reselect_cause );
      }
      else
      {
        sig_cs_ctrl_no_more_candidate( grr_data->cs.reselect_cause );
      }
      
      cr_initiated = TRUE;
      break;

    default:
      TRACE_ASSERT( mode EQ CRDM_T_RESELECT OR 
                    mode EQ CRDM_CR_CTRL    OR
                    mode EQ CRDM_CR_INITIAL OR
                    mode EQ CRDM_CR_CONT       );
      break;  
  }

  if( cr_initiated EQ TRUE )
  {
    TRACE_EVENT_P2( "cs_cr_decision: mode %d, reselect_cause %d",
                    mode, grr_data->cs.reselect_cause );
  }

  return( cr_initiated );

}/* cs_cr_decision */





/*
+------------------------------------------------------------------------------
| Function    : cs_trace_nc_mval_lst
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_trace_nc_mval_lst ( void )
{

#if !defined (NTRACE)

  UBYTE            read, write;
  USHORT           arfcn[8];
  UBYTE            bsic[8];
  T_NC_MVAL_STATUS status[8];
  T_RXLEV_AVG      rxlev_avg[8];
  UBYTE            rla_p_lev[8];
  UBYTE            rla_p_stat[8];


  TRACE_FUNCTION( "cs_trace_nc_mval_lst" );

  if( grr_data->cs.v_crp_trace EQ TRUE )
  {
    memset( arfcn,  0xFF, sizeof( arfcn  ) );
    memset( bsic,   0xFF, sizeof( bsic   ) );
    memset( status, 0xFF, sizeof( status ) );
    memset( rxlev_avg, 0xFF, sizeof( rxlev_avg ) );
    memset( rla_p_lev, 0xFF, sizeof( rla_p_lev ) );
    memset( rla_p_stat, 0xFF, sizeof( rla_p_stat ) );

    write = 0;

    for( read = 0; read < MAX_NR_OF_NC_MVAL; read++ )
    {
      if( grr_data->db.nc_mval_list.nc_mval[read].status NEQ NC_MVAL_STAT_NONE )
      {
        arfcn[write]  = grr_data->db.nc_mval_list.nc_mval[read].arfcn;
        bsic[write]   = grr_data->db.nc_mval_list.nc_mval[read].sync_info.bsic;
        status[write] = grr_data->db.nc_mval_list.nc_mval[read].status;
        rla_p_lev [write] = grr_data->db.nc_mval_list.nc_mval[read].rla_p.lev;
        rla_p_stat [write] = grr_data->db.nc_mval_list.nc_mval[read].rla_p.stat;
        rxlev_avg [write] = grr_data->db.nc_mval_list.nc_mval[read].rxlev_avg;

        write++;
      }

      if( write EQ 8 )
      {
        TRACE_EVENT_P8
          ( "NC_MVAL_LIST ARFCN:  %5d %5d %5d %5d %5d %5d %5d %5d",
            arfcn[0], arfcn[1], arfcn[2], arfcn[3], 
            arfcn[4], arfcn[5], arfcn[6], arfcn[7] );

        TRACE_EVENT_P8
          ( "NC_MVAL_LIST BSIC:   %5d %5d %5d %5d %5d %5d %5d %5d",
            bsic[0], bsic[1], bsic[2], bsic[3], 
            bsic[4], bsic[5], bsic[6], bsic[7] );

        TRACE_EVENT_P8
          ( "NC_MVAL_LIST STATUS: %5d %5d %5d %5d %5d %5d %5d %5d",
            status[0], status[1], status[2], status[3], 
            status[4], status[5], status[6], status[7] );

        TRACE_EVENT_P8
          ( "NC_MVAL_LIST T_RXLEV_AVG: %5d %5d %5d %5d %5d %5d %5d %5d",
            rxlev_avg[0], rxlev_avg[1], rxlev_avg[2], rxlev_avg[3], 
            rxlev_avg[4], rxlev_avg[5], rxlev_avg[6], rxlev_avg[7] );

        TRACE_EVENT_P8
          ( "NC_MVAL_LIST rla_p.stat: %5d %5d %5d %5d %5d %5d %5d %5d",
            rla_p_stat[0], rla_p_stat[1], rla_p_stat[2], rla_p_stat[3], 
            rla_p_stat[4], rla_p_stat[5], rla_p_stat[6], rla_p_stat[7] );

        TRACE_EVENT_P8
          ( "NC_MVAL_LIST rla_p.lev: %5d %5d %5d %5d %5d %5d %5d %5d",
            rla_p_lev[0], rla_p_lev[1], rla_p_lev[2], rla_p_lev[3], 
            rla_p_lev[4], rla_p_lev[5], rla_p_lev[6], rla_p_lev[7] );

        memset( arfcn,  0xFF, sizeof( arfcn  ) );
        memset( bsic,   0xFF, sizeof( bsic   ) );
        memset( status, 0xFF, sizeof( status ) );
        memset( rxlev_avg, 0xFF, sizeof( rxlev_avg ) );
        memset( rla_p_lev, 0xFF, sizeof( rla_p_lev ) );
        memset( rla_p_stat, 0xFF, sizeof( rla_p_stat ) );

        write = 0;
      }
    }

    if( write NEQ 0 )
    {
      TRACE_EVENT_P8
        ( "NC_MVAL_LIST ARFCN:  %5d %5d %5d %5d %5d %5d %5d %5d",
          arfcn[0], arfcn[1], arfcn[2], arfcn[3], 
          arfcn[4], arfcn[5], arfcn[6], arfcn[7] );

      TRACE_EVENT_P8
        ( "NC_MVAL_LIST BSIC:   %5d %5d %5d %5d %5d %5d %5d %5d",
          bsic[0], bsic[1], bsic[2], bsic[3], 
          bsic[4], bsic[5], bsic[6], bsic[7] );

      TRACE_EVENT_P8
        ( "NC_MVAL_LIST STATUS: %5d %5d %5d %5d %5d %5d %5d %5d",
          status[0], status[1], status[2], status[3], 
          status[4], status[5], status[6], status[7] );

      TRACE_EVENT_P8
          ( "NC_MVAL_LIST T_RXLEV_AVG: %5d %5d %5d %5d %5d %5d %5d %5d",
            rxlev_avg[0], rxlev_avg[1], rxlev_avg[2], rxlev_avg[3], 
            rxlev_avg[4], rxlev_avg[5], rxlev_avg[6], rxlev_avg[7] );

        TRACE_EVENT_P8
          ( "NC_MVAL_LIST rla_p.stat: %5d %5d %5d %5d %5d %5d %5d %5d",
            rla_p_stat[0], rla_p_stat[1], rla_p_stat[2], rla_p_stat[3], 
            rla_p_stat[4], rla_p_stat[5], rla_p_stat[6], rla_p_stat[7] );

        TRACE_EVENT_P8
          ( "NC_MVAL_LIST rla_p.lev: %5d %5d %5d %5d %5d %5d %5d %5d",
            rla_p_lev[0], rla_p_lev[1], rla_p_lev[2], rla_p_lev[3], 
            rla_p_lev[4], rla_p_lev[5], rla_p_lev[6], rla_p_lev[7] );
    }
  }

#endif /* #if !defined (NTRACE) */

} /* cs_trace_nc_mval_lst( ) */


/*
+------------------------------------------------------------------------------
| Function    : cs_stop_cr_meas 
+------------------------------------------------------------------------------
| Description : The function cs_stop_cr_meas () .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_stop_cr_meas( void )
{ 
  TRACE_FUNCTION( "cs_stop_cr_meas" );

  switch( grr_data->cs.cr_meas_mode )
  {
    case( CS_CRMM_BA_GPRS ):
      if( grr_data->cs.stop_req EQ FALSE )
      {
        PALLOC( mphp_cr_meas_stop_req, MPHP_CR_MEAS_STOP_REQ );

#if 0

        /*
         * the timing for the MPHP_CR_MEAS_STOP_CON is not correct on
         * layer 1 side, so we are no longer consider the confirmation
         */

        grr_data->cs.stop_req = TRUE;

#endif /* #if 0 */

        PSEND( hCommL1, mphp_cr_meas_stop_req );
      }
      break;
    case( CS_CRMM_BA_BCCH ):
      grr_data->cs.cr_meas_update = FALSE;
      break;
  }
} /* cs_stop_cr_meas() */


/*
+------------------------------------------------------------------------------
| Function    : cs_req_cr_meas 
+------------------------------------------------------------------------------
| Description : The function cs_req_cr_meas () .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_req_cr_meas ( void )
{ 
  TRACE_FUNCTION( "cs_req_cr_meas" );

  if( nc_data->c_cell NEQ 0 )
  {
    if( grr_data->cs_meas.packet_mode NEQ PACKET_MODE_PTM )
    {
      PALLOC( mphp_cr_meas_req, MPHP_CR_MEAS_REQ );
    
      cs_fill_cr_meas_req( mphp_cr_meas_req, grr_data->cs.list_id );

      grr_data->cs.list_id++;

      PSEND( hCommL1, mphp_cr_meas_req );
    }
    else
    {
      PALLOC( mphp_tcr_meas_req, MPHP_TCR_MEAS_REQ );

      grr_data->cs.last_assignment_id++;
      grr_data->cs.cr_meas_update = TRUE;

      cs_fill_cr_meas_req( (T_MPHP_CR_MEAS_REQ*)mphp_tcr_meas_req,
                             grr_data->cs.last_assignment_id );

      PSEND( hCommL1, mphp_tcr_meas_req );
    }

    grr_data->cs.stop_req = FALSE;
  }
} /* cs_req_cr_meas() */


/*
+------------------------------------------------------------------------------
| Function    : cs_fill_cr_meas_req
+------------------------------------------------------------------------------
| Description : ... 
|
| Parameters  : prim - Pointer to primitive buffer
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_fill_cr_meas_req ( T_MPHP_CR_MEAS_REQ *prim, UBYTE id )
{
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "cs_fill_cr_meas_req" );

  prim->nb_carrier = nc_data->c_cell;
  prim->list_id    = id;
    
  for( i = 0; i < nc_data->c_cell; i++ )
  {
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    prim->carrier_list[i] = grr_g23_arfcn_to_l1(nc_data->cell[i].arfcn);
#else
    prim->carrier_list[i] = nc_data->cell[i].arfcn;
#endif
  }

#ifdef _SIMULATION_

  for( i = nc_data->c_cell; i < MPHP_NUMC_BA_GPRS_SC; i++ )
  {
    prim->carrier_list[i] = 0xFFFF;
  }

#endif /* #ifdef _SIMULATION_ */

} /* cs_fill_cr_meas_req() */


/*
+------------------------------------------------------------------------------
| Function    : cs_reset_all_cell_results
+------------------------------------------------------------------------------
| Description : The function cs_reset_all_cell_results() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_reset_all_cell_results ( void )
{
  UBYTE i; /* used for counting */
    
  TRACE_FUNCTION( "cs_reset_all_cell_results" );

  for( i = 0; i < MPHP_NUMC_BA_GPRS_SC; i++ )
  {
    cs_reset_meas_result( &nc_data->cell[i] );
  }
} /* cs_reset_all_cell_results () */

/*
+------------------------------------------------------------------------------
| Function    : cs_reset_all_rxlev_results
+------------------------------------------------------------------------------
| Description : The function cs_reset_all_rxlev_results() .... 
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_reset_all_rxlev_results ( void )
{
  UBYTE i; /* used for counting */
    
  TRACE_FUNCTION( "cs_reset_all_rxlev_results" );

  for( i = 0; i < MPHP_NUMC_BA_GPRS_SC; i++ )
  {
    nc_data->cell[i].rxlev_data.acc = 0;
    nc_data->cell[i].rxlev_data.nbr = 0;
  }
} /* cs_reset_all_rxlev_results () */

/*
+------------------------------------------------------------------------------
| Function    : cs_reset_meas_result
+------------------------------------------------------------------------------
| Description : The function cs_reset_meas_result() .... 
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_reset_meas_result ( T_CELL *cell )
{
  T_RLA_P_DATA *rla_p_data = &cell->rla_p_data;
  T_RXLEV_DATA *rxlev_data = &cell->rxlev_data;

  UBYTE i; /* used for counting */
    
  TRACE_FUNCTION( "cs_reset_meas_result" );
  TRACE_EVENT_P2( "cs_reset_meas_result arfcn =%ld LOI=%ld",cell->arfcn,rla_p_data->loi );

  rla_p_data->loi    = CS_IDX_NOT_USED;
  rla_p_data->ovrflw = FALSE;

  for( i = 0; i < CS_MAX_MEAS_RSLT; i++ )
  {
    rla_p_data->meas[i].cnt     = 0;
    rla_p_data->meas[i].acc     = 0;
    rla_p_data->meas[i].rpt_prd = 0;
  }

  rxlev_data->acc = 0;
  rxlev_data->nbr = 0;
} /* cs_reset_meas_result () */

/*
+------------------------------------------------------------------------------
| Function    : cs_fill_meas_rslt
+------------------------------------------------------------------------------
| Description : ... 
|
| Parameters  : ...
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_fill_meas_rslt ( T_MPHP_CR_MEAS_IND  *mphp_cr_meas_ind,
                                T_MPHP_TCR_MEAS_IND *mphp_tcr_meas_ind  )
{
  T_RLA_P_DATA *rla_p_data;
  T_RXLEV_DATA *rxlev_data;

  T_RXLEV_DATA_NBR del_nbr;

  UBYTE  i;
  UBYTE  index  = 0;
  BOOL   ovrflw = FALSE;

  UBYTE  cnt;
  SHORT  acc;
  USHORT rpt_prd;

  TRACE_FUNCTION( "cs_fill_meas_rslt" );

  if( mphp_cr_meas_ind NEQ NULL )
  {
    rpt_prd = mphp_cr_meas_ind->reporting_period;
  }
  else
  {
    rpt_prd = CS_RPT_PRD_PTM;
  }
  
  /*
   * Process all data relevant for RLA_P derivation
   */

  for( i = 0; i < nc_data->c_cell; i++ )
  {
    rla_p_data = &nc_data->cell[i].rla_p_data;
    rxlev_data = &nc_data->cell[i].rxlev_data;

    if( mphp_cr_meas_ind NEQ NULL )
    {
      cnt = 1;
	  if((signed char)mphp_cr_meas_ind->p_ncell_meas[i].rxlev <0)
      {
        acc = 0;
      }
      else     
      {
        acc = (signed char)mphp_cr_meas_ind->p_ncell_meas[i].rxlev;
	  }
    }
    else
    {
      cnt = mphp_tcr_meas_ind->acc_nbr[i];
	   if((SHORT)mphp_tcr_meas_ind->acc_level[i] <0)
      {
        acc = 0;
      }
      else   
      {
        acc = mphp_tcr_meas_ind->acc_level[i];
      }
    }

    if( rla_p_data->loi NEQ CS_IDX_NOT_USED )
    {
      if( rla_p_data->loi EQ CS_MAX_MEAS_RSLT - 1 )
      {
        ovrflw = TRUE;
      }
      else
      {
        index = rla_p_data->loi + 1;
      }
    }
    
    if( cnt EQ 0 )
    {
      if( rla_p_data->meas[index].cnt EQ 0 )
      {
        rla_p_data->meas[index].rpt_prd += rpt_prd;
      }
      else
      {
        rla_p_data->meas[index].cnt      = 0;
        rla_p_data->meas[index].acc      = 0;
        rla_p_data->meas[index].rpt_prd = rpt_prd;
      }
    }
    else
    {
      rla_p_data->meas[index].cnt     = cnt;
      rla_p_data->meas[index].acc     = acc;
      rla_p_data->meas[index].rpt_prd = rpt_prd;

      rla_p_data->loi = index;

      /* if an overflow once occured it will be forever */
      if( rla_p_data->ovrflw EQ FALSE )
      {
        rla_p_data->ovrflw = ovrflw;
      }
    }

    /*
     * Process all data relevant for average RXLEV derivation
     */

    if( cnt NEQ 0 )
    {
      if( rxlev_data->nbr > RXLEV_MAX_NBR - cnt )
      {
        del_nbr          = cnt - ( RXLEV_MAX_NBR - rxlev_data->nbr );
        rxlev_data->acc -= del_nbr * ( rxlev_data->acc / rxlev_data->nbr );
        rxlev_data->nbr -= del_nbr;
      }

      rxlev_data->acc += ( acc * RXLEV_ACRCY );
      rxlev_data->nbr += cnt;
    }
  }
} /* cs_fill_meas_rslt() */

/*
+------------------------------------------------------------------------------
| Function    : cs_get_rla_p
+------------------------------------------------------------------------------
| Description : The function cs_get_rla_p() .... 
|
| Parameters  : rla_p_data - pointer to RLA_P raw data
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL cs_get_rla_p( T_RLA_P_DATA *rla_p_data, UBYTE *rla_p )
{
  BOOL  s_avg;             /* status of average RXLEV value                  */
  LONG  avg;               /* average RXLEV value used for cell re-selection */
  LONG  cnt         = 0;   /* number of accumulated measurement samples      */
  LONG  acc         = 0;   /* accumulated receive level values               */
  ULONG run_avg_prd = 0;   /* running average period                         */
  UBYTE c_meas      = 0;   /* number of measurements                         */
  UBYTE index       = rla_p_data->loi;
                           /* index to actual used measurement result  */
  BOOL  v_meas_left = rla_p_data->loi NEQ CS_IDX_NOT_USED;
                           /* indicates whether more measurement results     */
                           /* are not yet considered                         */

  TRACE_FUNCTION( "cs_get_rla_p" );

  while(
         (
           c_meas      <  CS_MIN_RLA_P_SAMPLES OR
           run_avg_prd <  CS_MIN_RUN_AVG_PRD     
         )
         AND
         (
           v_meas_left EQ TRUE
         )
       )
  {
    /*
     * check whether valid measurement results are available 
     */
    if( rla_p_data->meas[index].cnt NEQ 0                AND     
        c_meas                      <   CS_MAX_MEAS_RSLT     )
    {
      c_meas++;
      
      /* 
       * accumulate individual measurement results 
       */
      run_avg_prd += rla_p_data->meas[index].rpt_prd;
      cnt         += rla_p_data->meas[index].cnt;
      acc         += rla_p_data->meas[index].acc;

      /* determine the next index to be used for RLA_P calculation */
      if( index EQ 0 )
      {
        if( rla_p_data->ovrflw EQ TRUE )
        {
          index = CS_MAX_MEAS_RSLT - 1;
        }
        else
        {
          v_meas_left= FALSE;
        }
      }
      else
      {
        index--;
      }


    }
    else
    {
      v_meas_left = FALSE;
    }
  }
           
  /* 
   * calculate status of RLA_P
   */
  if( cnt EQ 0 )
  {
    s_avg = CS_RLA_P_NOT_AVAIL;
  }
  else if(
           (
             c_meas      < CS_MIN_RLA_P_SAMPLES
           )
           OR 
           ( 
             run_avg_prd < CS_MIN_RUN_AVG_PRD 
             AND
             c_meas      < CS_MAX_MEAS_RSLT
           )
         )
  {
    s_avg = CS_RLA_P_NOT_VALID;
  }
  else
  {
    s_avg = CS_RLA_P_VALID;
  }

  if( cnt > 0 )
  { 
    /*
     * calculate RLA_P
     */
    avg = acc / cnt;


    /*
     * clip RLA_P to values between 0 and 63
     */
    if( (signed char)( avg ) < CGRLC_RXLEV_MIN )
    {
      avg = CGRLC_RXLEV_MIN;
    }
    else if ( (signed char)( avg ) > CGRLC_RXLEV_MAX )
    {
      avg = CGRLC_RXLEV_MAX;
    }
  }
  else
  {
    avg = CGRLC_RXLEV_NONE;
  }


  *rla_p = (UBYTE)avg;

  return( s_avg );

} /* cs_get_rla_p () */

/*
+------------------------------------------------------------------------------
| Function    : cs_get_rxlev_avg
+------------------------------------------------------------------------------
| Description : The function cs_get_rxlev_avg() .... 
|
| Parameters  : rxlev_data - pointer to RXLEV raw data
|
+------------------------------------------------------------------------------
*/
GLOBAL T_RXLEV_AVG cs_get_rxlev_avg( T_RXLEV_DATA *rxlev_data )
{
  T_RXLEV_AVG rxlev_avg;
  
  TRACE_FUNCTION( "cs_get_rxlev_avg" );


  if( rxlev_data->nbr EQ 0 )
  {
    rxlev_avg = RXLEV_AVG_INVALID;
  }
  else
  {
    rxlev_avg = 
        ( T_RXLEV_AVG )( rxlev_data->acc / ( rxlev_data->nbr * RXLEV_ACRCY ) );
  }

  TRACE_EVENT_P2("1_cs_get_rxlev_avg: nbr=%ld rx_avg=%ld",rxlev_data->nbr,rxlev_avg);

  return( rxlev_avg );
} /* cs_get_rxlev_avg () */

/*
+------------------------------------------------------------------------------
| Function    : cs_tcr_meas_ind_to_pl 
+------------------------------------------------------------------------------
| Description : The function cs_tcr_meas_ind_to_pl () forwards the cell
|               re-selection measurement results catched during packet transfer
|               mode to ALR.
|
| Parameters  : prim - pointer to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_tcr_meas_ind_to_pl ( T_MPHP_TCR_MEAS_IND * prim )
{ 
  UBYTE i; /* used for counting */

  TRACE_FUNCTION( "cs_tcr_meas_ind_to_pl" );

  {
    PALLOC( tb_meas_ind, TB_MEAS_IND );

    for( i = 0; i < TB_BA_LIST_SIZE; i++ )
    {
      if(( grr_data->cs.cr_meas_update EQ TRUE) AND 
         (nc_data->cell[i].arfcn NEQ 0))
      {
        if(nc_data->cell[i].arfcn NEQ 0)
      {
        tb_meas_ind->tb_meas_result[i].arfcn  = grr_g23_arfcn_to_l1(nc_data->cell[i].arfcn);
      }
      else
      {
        tb_meas_ind->tb_meas_result[i].arfcn  = TB_INVALID_ARFCN;
      }
      }
      else
      {
        tb_meas_ind->tb_meas_result[i].arfcn  = TB_INVALID_ARFCN;
      }

      tb_meas_ind->tb_meas_result[i].rxlev    = prim->acc_level[i];
      tb_meas_ind->tb_meas_result[i].num_meas = prim->acc_nbr[i];
    }

    PSEND( hCommPL, tb_meas_ind );
  }
} /* cs_tcr_meas_ind_to_pl() */

/*
+------------------------------------------------------------------------------
| Function    : cs_create_cr_cell 
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL T_NC_MVAL* cs_create_cr_cell ( USHORT arfcn, UBYTE  bsic )
{ 
  T_NC_MVAL *nc_mval;
  UBYTE      nc_mval_idx;

  TRACE_FUNCTION( "cs_create_cr_cell" );

  /* if the new selected cell is not found in the neighbour cell list */
  nc_mval = grr_get_nc_mval( RRGRR_INVALID_ARFCN, RRGRR_INVALID_BSIC, &nc_mval_idx );
  
  if( nc_mval NEQ NULL )
  {
    nc_mval->arfcn          = arfcn;
    nc_mval->sync_info.bsic = bsic;

    /* 
     * The function cs_create_cr_cell is only called in case of processing a 
     * packet cell change order. If the packet cell change order has been 
     * received on a cell without PBCCH, GRR should forward the RRGRR_CR_REQ
     * with parameter CR_NEW in all cases. This is forced by setting the
     * synchronisation status to STAT_SYNC_OK.
     */
    if( grr_is_pbcch_present( ) )
    {
      TRACE_EVENT( "cs_create_cr_cell: cell not part of neighbour cell list" );

      nc_mval->sync_info.sync.status = STAT_SYNC_NONE;
    }
    else
    {
      nc_mval->sync_info.sync.status = STAT_SYNC_OK;
    }

    grr_data->db.cr_cell             = &grr_data->db.nc_nw_slctd.strgst;
    grr_data->db.cr_cell->idx        = nc_mval_idx;
    grr_data->db.cr_cell->mode       = CS_MODE_IDLE;
    grr_data->db.cr_cell->avail_time = 0;
  }

  return( nc_mval );

} /* cs_create_cr_cell() */

/*
+------------------------------------------------------------------------------
| Function    : cs_get_nc_rpt_prd_idle 
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
LOCAL T_TIME cs_get_nc_rpt_prd_idle ( void )
{ 
  T_TIME reporting_period;
  ULONG  small_int_multi; /* smallest integer multiple of DRX period */
  ULONG  drx_period;      /* DRX period in milliseconds              */
        
  TRACE_FUNCTION( "cs_get_nc_rpt_prd_idle" );

  reporting_period = GET_NC_RPT_PRD( grr_data->db.nc_ref_lst.param->rep_per_i );
  drx_period       = meas_im_get_drx_period_seconds( );
  drx_period       = M_ROUND_UP( drx_period, DRX_NORM_FACTOR );

  /*
   * In packet idle mode, the reporting period is NC_REPORTING_PERIOD_I
   * rounded off to the nearest smaller integer multiple of DRX period
   * if NC_REPORTING_PERIOD_I is greater than DRX period, else, the
   * reporting period is DRX period.
   */
  if( drx_period NEQ NO_PAGING )
  {
    if( reporting_period > drx_period )
    {
      small_int_multi  = reporting_period / drx_period;
      reporting_period = drx_period * small_int_multi;
    }
    else
    {
      reporting_period = drx_period;
    }
  }
  
  return( reporting_period );

} /* cs_get_nc_rpt_prd_idle() */


/*
+------------------------------------------------------------------------------
| Function    : cs_reset_meas_rep_params
+------------------------------------------------------------------------------
| Description : This function will reset measurement reporting parameters
|
| Parameters  : db - pointer to serving cell database
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_reset_meas_rep_params ( T_SC_DATABASE *db )
{
  TRACE_FUNCTION( "cs_reset_meas_rep_params" );

  db->nc_ms.ncmeas.param.ctrl_order = NC_RESET;
  db->nc_ms.ncmeas.param.rep_per_i = 0;
  db->nc_ms.ncmeas.param.rep_per_t = 0;

} /* cs_reset_meas_rep_params() */

/*
+------------------------------------------------------------------------------
| Function    : cs_check_nc_sync_timer
+------------------------------------------------------------------------------
| Description : Handles the nc sync timer of 10 sec in case of PIM_PBCCH and
| PTM_PBCCH
|
| Parameters  : reporting_period
|
+------------------------------------------------------------------------------
*/


GLOBAL void cs_check_nc_sync_timer(USHORT reporting_pd)
{
   if(!grr_data->db.cnt_nc_six.candid) return;
   grr_data->cs.nc_sync_rep_pd += reporting_pd;


   if(grr_data->cs.nc_sync_rep_pd >= CS_NCSYNC_RPT_PRD_PIM)
   {
      PALLOC( rrgrr_check_bsic_req, RRGRR_NCELL_SYNC_REQ );
      rrgrr_check_bsic_req->sync_type = SYNC_RECONFIRM;
      PSEND( hCommRR, rrgrr_check_bsic_req );
      /*subtracting nc_sync_rep_pd from CS_NCSYNC_RPT_PRD_PIM wil provide take care 
      of periodicity of non-uniform reporting_period values*/
      TRACE_EVENT_P1("GRR_NC_SYNC:EXPIRY: nc_sync_rep_pd=%d", grr_data->cs.nc_sync_rep_pd);
      grr_data->cs.nc_sync_rep_pd -= CS_NCSYNC_RPT_PRD_PIM; 
   }
   return;

}

/*
+------------------------------------------------------------------------------
| Function    : cs_process_serving_cell_data
+------------------------------------------------------------------------------
| Description : This function is used to fill rxlev and interference measurements
|               of the serving cell
|
| Parameters  : rxlev, iscell
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL cs_process_serving_cell_data ( UBYTE *rxlev_scell, UBYTE *v_i_scell, 
                                       UBYTE *i_scell, UBYTE *used_bits, 
                                       BOOL *rsc_avail, BOOL *isc_avail )

{
  TRACE_FUNCTION( "cs_process_serving_cell_data" );

  *used_bits += NC_RXLEV_SCELL_LEN;

  if( grr_data->db.scell_info.rxlev_avg EQ RXLEV_AVG_INVALID )
  {
    TRACE_ERROR( "cs_build_xxx_meas_rpt no RXLEV of serving cell" );
    return( FALSE );
  }
  else
  {
    *rsc_avail   = TRUE;
    *rxlev_scell = grr_data->db.scell_info.rxlev_avg;
  }
  
  /* process interference measurement of serving cell */
  *used_bits             += PMR_FLAG_LEN;
  *v_i_scell  = FALSE;

  if( grr_is_pbcch_present( )       EQ  TRUE            AND 
      psc_db->paging_group.kc       NEQ 0               AND
      grr_data->cs_meas.packet_mode EQ  PACKET_MODE_PIM     )
  {
    T_p_frequency_par freq_par;

    if( grr_get_pccch_freq_par
          ( ( UBYTE )( grr_imsi_mod( ) % psc_db->paging_group.kc ), 
            &freq_par.p_chan_sel, &freq_par.p_freq_list ) )
    {
      T_MEAS_IM_CARRIER ma;
      UBYTE             written_bits;
      T_ilev_abs        i_level;

      meas_im_set_carrier( &ma, &freq_par );

      written_bits = meas_im_get_abs_i_level( &ma, &i_level );

      if( written_bits > CGRLC_MAX_TIMESLOTS * PMR_FLAG_LEN )
      {
        *v_i_scell = TRUE;
        *used_bits += NC_IF_SCELL_LEN;

        if     ( i_level.v_ilevabs0 EQ TRUE )
        {
          *i_scell = i_level.ilevabs0;
          *isc_avail = TRUE;
        }
        else if( i_level.v_ilevabs1 EQ TRUE ) 
        {
          *i_scell = i_level.ilevabs1; 
          *isc_avail = TRUE;
        }
        else if( i_level.v_ilevabs2 EQ TRUE ) 
        {
          *i_scell = i_level.ilevabs2; 
          *isc_avail = TRUE;
        }
        else if( i_level.v_ilevabs3 EQ TRUE ) 
        {
          *i_scell = i_level.ilevabs3; 
          *isc_avail = TRUE;
        }
        else if( i_level.v_ilevabs4 EQ TRUE ) 
        {
          *i_scell = i_level.ilevabs4; 
          *isc_avail = TRUE;
        }
        else if( i_level.v_ilevabs5 EQ TRUE ) 
        {
          *i_scell = i_level.ilevabs5; 
          *isc_avail = TRUE;
        }
        else if( i_level.v_ilevabs6 EQ TRUE ) 
        {
          *i_scell = i_level.ilevabs6; 
          *isc_avail = TRUE;
        }
        else if( i_level.v_ilevabs7 EQ TRUE ) 
        {
          *i_scell = i_level.ilevabs7; 
          *isc_avail = TRUE;
        }
        else 
        {
          TRACE_ERROR( "cs_process_serving_cell_data corrupted I_LEVEL data" );
          return( FALSE );
        }
      }
    }
  }
  return(TRUE);
}/* cs_process_serving_cell_data */

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : cs_send_enh_meas_rpt
+------------------------------------------------------------------------------
| Description : This function is used to send enh meas report to the network
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL cs_send_enh_meas_rpt (void)
{
  T_U_ENHNC_MEAS_REPORT enh_meas_rpt;
  BOOL  rpt_send;

  TRACE_FUNCTION( "cs_send_enh_meas_rpt" );

  if(grr_is_pbcch_present())
  {
    cs_form_enh_cell_list();
  }
  else
  {
    cs_form_ba_bcch_enh_cell_list();
  }

  cs_nc_sort_and_update_emr_info();

  memset (&enh_meas_rpt, 0, sizeof(T_U_ENHNC_MEAS_REPORT));

  rpt_send = cs_build_enh_meas_rpt(&enh_meas_rpt);

  if( rpt_send EQ TRUE )
  {
    sig_cs_ctrl_enh_meas_report(&enh_meas_rpt);
  }

  return(rpt_send);
    
} /* cs_send_enh_meas_rpt() */


/*
+------------------------------------------------------------------------------
| Function    : cs_form_enh_cell_list
+------------------------------------------------------------------------------
| Description : This function is used to form enhanced cell list from BA list
|               (nc_ref_lst) and Measured values list(nc_mval_list)
|               
|
| Parameters  : Nil
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_form_enh_cell_list(void)
{
  UBYTE i = 0, n = 0, j;
  UBYTE nc_ref_index_arfcn, nc_ref_index_bsic;
  
  TRACE_FUNCTION( "cs_form_enh_cell_list" );

  memset((grr_data->db.enh_cell_list), 0xFF, sizeof(T_ENH_CELL_LIST)*MAX_NR_OF_NCELL);
  
  grr_data->db.cnt_enh_cell_list = 0; 

    /* Check one entry of Measured values list against all entries of BA list */
  while(i < MAX_NR_OF_NC_MVAL AND n < MAX_NR_OF_NCELL)
  {
    if( (grr_data->db.nc_mval_list.nc_mval[i].status EQ NC_MVAL_STAT_ASSIGNED)
         AND 
         (grr_data->db.nc_mval_list.nc_mval[i].arfcn NEQ RRGRR_INVALID_ARFCN)
         AND
        (grr_data->db.nc_mval_list.nc_mval[i].sync_info.sync.status EQ 
         STAT_SYNC_OK)
         AND
         (grr_data->db.nc_mval_list.nc_mval[i].rla_p.stat EQ CS_RLA_P_VALID) )
    {
      nc_ref_index_arfcn = NOT_SET; /* Will be set the 1st time ARFCN matches 
                                      in the BA list */
      nc_ref_index_bsic = NOT_SET;  /* Will be set the 1st time ARFCN-BSIC pair
                                      matches in the BA list */

      /* Comparison with BA list */
      for (j = 0; j < grr_data->db.nc_ref_lst.number; j++)
      {
        if(grr_data->db.nc_mval_list.nc_mval[i].arfcn EQ
           grr_data->db.nc_ref_lst.info[j]->arfcn)
        {          
          if(nc_ref_index_arfcn EQ NOT_SET)
          {
            /* ARFCN matched for the 1st time in BA list */
            nc_ref_index_arfcn  = j;
          }

          /* In case the same cell (ARFCN+BSIC) or the same ARFCN without BSIC
             occur more than once in the resulting GSM Neighbour Cell list, 
             each occurrence shall be assigned an index but only the cell with
             the highest index shall be used for cell re-selection and referred
             to in measurement reports. */
          if(grr_data->db.nc_ref_lst.info[j]->index >
             grr_data->db.nc_ref_lst.info[nc_ref_index_arfcn]->index)
          {
            nc_ref_index_arfcn = j;
          }
          
          if(grr_data->db.nc_mval_list.nc_mval[i].sync_info.bsic EQ
             grr_data->db.nc_ref_lst.info[j]->bsic)
          {
            nc_ref_index_bsic = nc_ref_index_arfcn;
          }
        }
      }
      
      /* Store ARFCN, BSIC, RLA_P and RXLEV, if ARFCN and/or BSIC matches */
      if(nc_ref_index_arfcn NEQ NOT_SET)
      {
        grr_data->db.enh_cell_list[n].arfcn =
          grr_data->db.nc_mval_list.nc_mval[i].arfcn;

        grr_data->db.enh_cell_list[n].bsic.bsic =
          grr_data->db.nc_mval_list.nc_mval[i].sync_info.bsic;

        grr_data->db.enh_cell_list[n].rla_p =
          grr_data->db.nc_mval_list.nc_mval[i].rla_p.lev;
        
        grr_data->db.enh_cell_list[n].rxlev_avg =
          grr_data->db.nc_mval_list.nc_mval[i].rxlev_avg;
      }
      
      /* Store in the enhanced cell list as Valid and Invalid BSICs */
      if (nc_ref_index_bsic NEQ NOT_SET)
      {
        grr_data->db.enh_cell_list[n].index =
          grr_data->db.nc_ref_lst.info[nc_ref_index_bsic]->index;

        grr_data->db.enh_cell_list[n].rept_prio = 
          grr_data->db.nc_ref_lst.enh_param->gprs_rept_prio_desc.
          rept_prio[grr_data->db.nc_ref_lst.info[nc_ref_index_bsic]->index];

        grr_data->db.enh_cell_list[n++].bsic.status = BSIC_VALID;
      }
      else if(nc_ref_index_arfcn NEQ NOT_SET)
      {
        grr_data->db.enh_cell_list[n].index =
          grr_data->db.nc_ref_lst.info[nc_ref_index_arfcn]->index;
        /* Set the reporting priority to high for invalid BSICs and allowed 
        NCC part. Refer Section 8.4.8.1 of 3GPP TS 05.08 V8.15.0 (2002-06) */
        grr_data->db.enh_cell_list[n].rept_prio = HIGH_PRIO;
        grr_data->db.enh_cell_list[n++].bsic.status = BSIC_INVALID;
      }
    }
    i++;
  }
  grr_data->db.cnt_enh_cell_list = n;
}/* cs_form_enh_cell_list */

/*
+------------------------------------------------------------------------------
| Function    : cs_form_ba_bcch_enh_cell_list
+------------------------------------------------------------------------------
| Description : This function is used to form enhanced cell list from BA list
|               (nc_ref_lst) and Measured values list(nc_mval_list)
|               
|
| Parameters  : Nil
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_form_ba_bcch_enh_cell_list(void)
{
  UBYTE i = 0, n = 0, j;
  UBYTE nc_ref_index_arfcn, nc_ref_index_bsic;

  TRACE_FUNCTION( "cs_form_ba_bcch_enh_cell_list" );

  memset((grr_data->db.enh_cell_list), 0xFF, sizeof(T_ENH_CELL_LIST)*MAX_NR_OF_NCELL);
  
  grr_data->db.cnt_enh_cell_list = 0; 

  /* Check one entry of Measured values list against all entries of BA list */
  while(i < MAX_NR_OF_NC_MVAL AND n < MAX_NR_OF_NCELL)
   {
    if( (grr_data->db.nc_mval_list.nc_mval[i].status EQ NC_MVAL_STAT_ASSIGNED)
         AND 
         (grr_data->db.nc_mval_list.nc_mval[i].arfcn NEQ RRGRR_INVALID_ARFCN)
         AND
        (grr_data->db.nc_mval_list.nc_mval[i].sync_info.sync.status EQ 
         STAT_SYNC_OK)
         AND
         (grr_data->db.nc_mval_list.nc_mval[i].rla_p.stat EQ CS_RLA_P_VALID) )
    {
      nc_ref_index_arfcn = NOT_SET; /* Will be set the 1st time ARFCN matches 
                                      in the BA list */
      nc_ref_index_bsic = NOT_SET; /* Will be set the 1st time ARFCN-BSIC pair
                                      matches in the BA list */

      /* Comparison with BA list */
      for (j = 0; j < grr_data->db.ba_bcch_nc_ref_lst.number; j++)
      {
        if(grr_data->db.nc_mval_list.nc_mval[i].arfcn EQ
           grr_data->db.ba_bcch_nc_ref_lst.info[j]->arfcn)
        {
          if(nc_ref_index_arfcn EQ NOT_SET)
          {
            /* ARFCN matched for the 1st time in BA list */
            nc_ref_index_arfcn  = j;
          }

          /* In case the same cell (ARFCN+BSIC) or the same ARFCN without BSIC
             occur more than once in the resulting GSM Neighbour Cell list, 
             each occurrence shall be assigned an index but only the cell with
             the highest index shall be used for cell re-selection and referred
             to in measurement reports. */
          if(grr_data->db.ba_bcch_nc_ref_lst.info[j]->index >
             grr_data->db.ba_bcch_nc_ref_lst.info[nc_ref_index_arfcn]->index)
          {
            nc_ref_index_arfcn = j;
          }
          
          if(grr_data->db.nc_mval_list.nc_mval[i].sync_info.bsic EQ
             grr_data->db.ba_bcch_nc_ref_lst.info[j]->bsic)
          {
            nc_ref_index_bsic = nc_ref_index_arfcn;
          }
        }
      }
      
      /* Store ARFCN, BSIC, RLA_P AND RXLEV, if ARFCN and/or BSIC matches */
      if(nc_ref_index_arfcn NEQ NOT_SET)
      {
        grr_data->db.enh_cell_list[n].arfcn =
          grr_data->db.nc_mval_list.nc_mval[i].arfcn;

        grr_data->db.enh_cell_list[n].bsic.bsic =
          grr_data->db.nc_mval_list.nc_mval[i].sync_info.bsic;

        grr_data->db.enh_cell_list[n].rla_p =
          grr_data->db.nc_mval_list.nc_mval[i].rla_p.lev;

        grr_data->db.enh_cell_list[n].rxlev_avg =
          grr_data->db.nc_mval_list.nc_mval[i].rxlev_avg;
      }
      
      /* Store in the enhanced cell list as Valid and Invalid BSICs */
      if (nc_ref_index_bsic NEQ NOT_SET)
      {
        grr_data->db.enh_cell_list[n].index =
          grr_data->db.ba_bcch_nc_ref_lst.info[nc_ref_index_bsic]->index;

        grr_data->db.enh_cell_list[n].rept_prio = 
          grr_data->db.nc_ref_lst.enh_param->gprs_rept_prio_desc.
          rept_prio[grr_data->db.ba_bcch_nc_ref_lst.info[nc_ref_index_bsic]->index];

        grr_data->db.enh_cell_list[n++].bsic.status = BSIC_VALID;
      }
      else if(nc_ref_index_arfcn NEQ NOT_SET)
      {
        grr_data->db.enh_cell_list[n].index =
          grr_data->db.ba_bcch_nc_ref_lst.info[nc_ref_index_arfcn]->index;
        /* Set the reporting priority to high for invalid BSICs and allowed 
        NCC part. Refer Section 8.4.8.1 of 3GPP TS 05.08 V8.15.0 (2002-06) */
        grr_data->db.enh_cell_list[n].rept_prio = HIGH_PRIO;
        grr_data->db.enh_cell_list[n++].bsic.status = BSIC_INVALID;
      }
    }
    i++;
  }
  grr_data->db.cnt_enh_cell_list = n;
}/* cs_form_ba_bcch_enh_cell_list */

/*
+------------------------------------------------------------------------------
| Function    : cs_nc_sort_and_update_emr_info
+------------------------------------------------------------------------------
| Description : The purpose of this function is to handle all the specific 
|               requirements for EMR - sorting according to priority as given 
|               in 5.08 V8.16.0, sec.8.4.8.1, and scaling of rxlev values.
|               
|
| Parameters  : Nil
|
+------------------------------------------------------------------------------
*/

LOCAL void cs_nc_sort_and_update_emr_info(void)
{
  T_ENH_BIN     sc_bin;
  T_ENH_BIN     mband_bin[5]; /* 0 - 900, 1 - 1800, 2 - 400, 3 - 1900, 4 - 850*/
  T_ENH_BIN     rest_bin;
  UBYTE         i;
  UBYTE         temp;
  T_GRR_ENH_PARA  *p_enh          = grr_data->db.nc_ref_lst.enh_param;
  T_ENH_CELL_LIST *enh_cell_list  = &(grr_data->db.enh_cell_list[0]);
  BOOL            include_cell    = FALSE;
  grr_data->db.sorted_enh_cell_list.num_valid = 0;

  TRACE_FUNCTION( "cs_nc_sort_and_update_emr_info" );

  /* Flush off the old sorted ENH cell list before forming a new one */
  memset(&grr_data->db.sorted_enh_cell_list,0,sizeof(T_ENH_BIN));

  /* Step 1: Sort cells into different bins - serving cell band bin,
    non-serving cell band bin, and the rest which can contain invalid BSIC
    cells */
  cs_nc_sort_cells_into_bins(&sc_bin, mband_bin, &rest_bin);

  /* Step 2: Fill the cells from each bin, after sorting in descending order
     of RXLEV, in the order of : sc_bin, mband_bin and rest_bin.
     Further number of cells from sc_bin and mband_bin is restricted by SERVING_BAND
     and MULTIBAND reporting parameters */
  if ( (sc_bin.num_valid > 0) AND (p_enh->servingband_rep > 0) )
  {
    cs_nc_sort_and_store_meas_results(&sc_bin, p_enh->servingband_rep);
    if (sc_bin.num_valid > p_enh->servingband_rep)
    {
      cs_nc_update_rest_bin(&rest_bin, &sc_bin, p_enh->servingband_rep);
    }
  }
    
  /* Fill cells from multi band after sorting*/
  for ( i = 0; i < MAX_NUM_BANDS AND 
      grr_data->db.sorted_enh_cell_list.num_valid 
      < grr_data->db.cnt_enh_cell_list; i++)    
  {
    if ( (mband_bin[i].num_valid > 0) AND (p_enh->multiband_rep > 0) )
    {
      cs_nc_sort_and_store_meas_results(&mband_bin[i], p_enh->multiband_rep);
      if (mband_bin[i].num_valid > p_enh->multiband_rep)
      {
        cs_nc_update_rest_bin(&rest_bin, &mband_bin[i], p_enh->multiband_rep );
      }
    }
  }
  
  /* Now sort cells in the rest bin which may contain both valid and 
    invalid BSIC cells. Here we use the reporting offset along with measured RXLEV,
    for sorting. According to 5.08, sec.8.4.8.1 we need to find those cells whose sum of 
    RXLEV and xxx_reporting_offset is maximum. This can be acheived by sorting the cells
    in the order of their rxlevm, where rxlevm = rxlev + offset */
  if ( (rest_bin.num_valid > 0) AND (grr_data->db.sorted_enh_cell_list.num_valid 
      < grr_data->db.cnt_enh_cell_list) )
  {
    UBYTE     j;
    UBYTE     max_rxlevm;
    UBYTE     band;
    UBYTE     rxlevm;
    UBYTE     k;
    T_ncell_info *ncell;
    
    for (i = 0; i < rest_bin.num_valid; i++)
    {
      band = cs_get_band_index_for_emr (enh_cell_list[rest_bin.enh_index[i]].arfcn);
      max_rxlevm = enh_cell_list[rest_bin.enh_index[i]].rla_p + 
        (p_enh->enh_rep_data[band].rep_offset * 6);
      k = i;
      for ( j = i+1; j < rest_bin.num_valid ; j++ )
      {
        band = cs_get_band_index_for_emr (enh_cell_list[rest_bin.enh_index[j]].arfcn);
        rxlevm = enh_cell_list[rest_bin.enh_index[j]].rla_p +
          (p_enh->enh_rep_data[band].rep_offset * 6) ;
        if ( rxlevm > max_rxlevm )
        {
          k = j;
          max_rxlevm = rxlevm;            
        }
      }
      include_cell = TRUE;
      /* When reduced reporting is enabled we have to report low priority cell 
      once in 4 reporting periods. The RXLEV then should be average of previous
      two periods */
      if ( (enh_cell_list[rest_bin.enh_index[k]].rept_prio EQ NORMAL_PRIO) AND 
           (p_enh->rep_rate EQ REPORTING_RATE_REDUCED) )
      {
        /* ncell cannot be NULL since the loop will be entered only for a 
           valid BSIC */
        ncell = grr_get_ncell_info(enh_cell_list[rest_bin.enh_index[k]].arfcn, 
          enh_cell_list[rest_bin.enh_index[k]].bsic.bsic);

        /* For every 4th time, we include this cell in the report */
        if ( (ncell->rep_count & (0x03)) EQ 0 )
        {
          /* For those cells that are not reported in every measurement report,
          the MS shall average the measurements of the current and the previous
          reporting period (i.e. over two reporting periods). */
          enh_cell_list[rest_bin.enh_index[k]].rxlev_avg = 
            (enh_cell_list[rest_bin.enh_index[k]].rxlev_avg + ncell->last_rxlev) >> 1;
                                                          /* division by 2 */
        }
        else
        {
          ncell->last_rxlev = enh_cell_list[rest_bin.enh_index[k]].rxlev_avg;
          include_cell = FALSE;
        }
        /* modulo 4 addition */
        ncell->rep_count = (ncell->rep_count + 1) & 0x03;
      }

      if (include_cell EQ TRUE)
      {
        grr_data->db.sorted_enh_cell_list.
          enh_index[grr_data->db.sorted_enh_cell_list.num_valid] = 
          rest_bin.enh_index[k];
        grr_data->db.sorted_enh_cell_list.num_valid++;
      }

      if(k NEQ i)
      {
        temp = rest_bin.enh_index[k];
        rest_bin.enh_index[k] = rest_bin.enh_index[i];
        rest_bin.enh_index[i] = temp;
      }
    }
  }
  /* This leaves us with final step in neighbour cell measurement reporting - scaling.*/
  grr_data->db.pemr_params.scale_used = cs_nc_scale_rxlev ();  
}/* cs_nc_sort_and_update_emr_info */

/*
+------------------------------------------------------------------------------
| Function    : cs_nc_sort_cells_into_bins
+------------------------------------------------------------------------------
| Description : The purpose of this function is to sort the cells into various
|               bins. This is the first step in handling reporting priority 
|               as given in 5.08 sec.8.4.8.1
|               
|
| Parameters  : Pointers to the three bins
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_nc_sort_cells_into_bins(T_ENH_BIN *p_sbin,T_ENH_BIN *p_mbin,
                                      T_ENH_BIN *p_rbin)
{
  UBYTE     i, j;
  UBYTE     band;
  UBYTE     sc_band;
  UBYTE     rth;  
  T_ENH_CELL_LIST *enh_cell_list  = &(grr_data->db.enh_cell_list[0]);
  T_GRR_ENH_PARA  *p_enh          = grr_data->db.nc_ref_lst.enh_param;

  TRACE_FUNCTION( "cs_nc_sort_cells_into_bins" );
  
  memset(p_sbin, 0xFF, sizeof(T_ENH_BIN));
  p_sbin->num_valid = 0;

  memset(p_rbin, 0xFF, sizeof(T_ENH_BIN));
  p_rbin->num_valid = 0;
  
  for( i = 0; i < MAX_NUM_BANDS; i++)
  {
    memset(&p_mbin[i], 0xFF, sizeof(T_ENH_BIN));
    p_mbin[i].num_valid = 0;
  }

  sc_band = cs_get_band_index_for_emr (psc_db->pbcch.bcch.arfcn);
  
  for ( j = 0; j < grr_data->db.cnt_enh_cell_list; j++ )
  {
    if(enh_cell_list[j].bsic.status EQ BSIC_VALID) /* Valid BSIC */
    {
      /*Find the band of this arfcn*/
      band = cs_get_band_index_for_emr(enh_cell_list[j].arfcn);
      rth = p_enh->enh_rep_data[band].rep_threshold;
      if (band EQ sc_band )
      {
        /* Apply the threshold criteria for this band*/
        if ( rth NEQ REP_THRESHOLD_INF )
        {
          if((rth EQ REP_THRESHOLD_DEF) OR
            (enh_cell_list[j].rla_p >= (rth * 6)))
          {
            p_sbin->enh_index[p_sbin->num_valid++] = j;
          }
          else
          {
            p_rbin->enh_index[p_rbin->num_valid++] = j;
          }
        } /*otherwise report-never*/            
      } /* otherwise cell in non-serving band*/
      else
      {
        /* Apply the threshold criteria for this band*/
        if ( rth NEQ REP_THRESHOLD_INF )                  
        {
          if((rth EQ REP_THRESHOLD_DEF) OR
            (enh_cell_list[j].rla_p >= (rth * 6))) 
          {
            p_mbin[band].enh_index[p_mbin[band].num_valid++] = j;
          }
          else
          {
            p_rbin->enh_index[p_rbin->num_valid++] = j;
          }
        } /*otherwise report never*/
      }
    }
    else /* If there's no cell with the given BSIC, then it falls under Invalid 
            BSIC category */
    {
      /* check whether Invalid BSIC reporting is enabled or not and NCC part is 
        permitted or not */
      if ( (p_enh->inv_bsic_enabled EQ TRUE) AND
           ((BOOL)((UBYTE)(p_enh->ncc_permitted & 
           ncc_bit_mask[(enh_cell_list[j].bsic.bsic & BSIC_NCC_MASK) >> 3])) NEQ 0) )
      {
        /* put it in rest bin */
        p_rbin->enh_index[p_rbin->num_valid++] = j;
      }     
    } /* if enh_cell_list.index */
  } /* end of for */
}/* cs_nc_sort_cells_into_bins */

/*
+------------------------------------------------------------------------------
| Function    : cs_get_band_index_for_emr
+------------------------------------------------------------------------------
| Description : This is implementation specific function. This is used to
|               get index into emr reporting data (reporting threshold and
|               offset) array, which is band specific.
|               
|
| Parameters  : ARFCN
|
+------------------------------------------------------------------------------
*/

LOCAL UBYTE cs_get_band_index_for_emr (USHORT arfcn)
{
 
  TRACE_FUNCTION( "cs_get_band_index_for_emr" );

  if ( INRANGE(LOW_CHANNEL_900, arfcn, HIGH_CHANNEL_900) )
  {
    return 0;
  }
  else if ( INRANGE(LOW_CHANNEL_1800, arfcn, HIGH_CHANNEL_1800))
  {
    return 1;
  }
  else if (INRANGE(LOW_CHANNEL_1900, arfcn, HIGH_CHANNEL_1900))
  {
    return 3;
  }
  else if ( INRANGE(LOW_CHANNEL_850, arfcn, HIGH_CHANNEL_850))
  {
    return 4;
  }
  else
  {
    return 2;
  }
}/* cs_get_band_index_for_emr */

/*
+------------------------------------------------------------------------------
| Function    : cs_nc_sort_and_store_meas_results
+------------------------------------------------------------------------------
| Description : This function sorts the cells collected in each bin, in
|               descending order of RXLEV
|               
|
| Parameters  : Pointer to bin, Number of cells to be reported
|
+------------------------------------------------------------------------------
*/

LOCAL void cs_nc_sort_and_store_meas_results(T_ENH_BIN *p_bin, UBYTE nbr_rpt)
{
  UBYTE     i;
  UBYTE     j;
  UBYTE     max_rxlev;
  UBYTE     k;
  UBYTE     temp;
  T_ENH_CELL_LIST *enh_cell_list  = &(grr_data->db.enh_cell_list[0]);
  
  TRACE_FUNCTION( "cs_nc_sort_and_store_meas_results" );

  for (i = 0; (i < p_bin->num_valid AND nbr_rpt > 0); i++,nbr_rpt--)
  {
    max_rxlev = enh_cell_list[p_bin->enh_index[i]].rla_p;

    k = i;

    for ( j = i+1; j < p_bin->num_valid ; j++ )
    {
      if ( enh_cell_list[p_bin->enh_index[j]].rla_p > max_rxlev )
      {

        k = j;
        
        max_rxlev = enh_cell_list[p_bin->enh_index[j]].rla_p;
      }
    }
    
    grr_data->db.sorted_enh_cell_list.
      enh_index[grr_data->db.sorted_enh_cell_list.num_valid] = 
      p_bin->enh_index[k];
    grr_data->db.sorted_enh_cell_list.num_valid++;
    
    if(k NEQ i)
    {
      temp = p_bin->enh_index[k];
      p_bin->enh_index[k] = p_bin->enh_index[i];
      p_bin->enh_index[i] = temp;
    }

  }
}/* cs_nc_sort_and_store_meas_results */

/*
+------------------------------------------------------------------------------
| Function    : cs_nc_update_rest_bin
+------------------------------------------------------------------------------
| Description : This function updates the rest bin by appending the left out
|               cells in the other bins. These may also have to be reported
|               along with the cells in rest bin.
|               
|
| Parameters  : Pointers to the rest and a bin, Number of cells to be reported
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_nc_update_rest_bin(T_ENH_BIN *p_rbin, T_ENH_BIN *p_bin, 
                                 UBYTE nbr_rpt)
{
  UBYTE i, count;

  TRACE_FUNCTION( "cs_nc_update_rest_bin" );

  count = p_bin->num_valid - nbr_rpt;

  for ( i = 0; i < count; i++)
  {
    p_rbin->enh_index[p_rbin->num_valid++] = p_bin->enh_index[nbr_rpt++];
    p_bin->num_valid--;
  }

}/* cs_nc_update_rest_bin */

/*
+------------------------------------------------------------------------------
| Function    : cs_nc_scale_rxlev
+------------------------------------------------------------------------------
| Description : This function performs the scaling of RXLEV.
|               
|
| Parameters  : Nil
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE cs_nc_scale_rxlev (void)
{
  UBYTE     i;
  UBYTE     no_of_cells = grr_data->db.cnt_enh_cell_list;
  UBYTE     scale;
  T_ENH_CELL_LIST   *enh_cell_list   = &(grr_data->db.enh_cell_list[0]);
  UBYTE     scale_order = grr_data->db.nc_ref_lst.enh_param->scale_order;

  TRACE_FUNCTION( "cs_nc_scale_rxlev" );

    /*If scaling is automatic, then we find the scaling as below:
     If the maximum RXLEV is greater than 63 (MAX_RXLEV), then we add 10dB
     and check whether it falls into the range after scaling*/
  /* Note: The assumption is L1 reports values in the following way -
     if actual value of rxlev (x) is between y and y+1 dBm, then the reported value
     is 110+x rounded off to next integer. */
  if ( scale_order EQ SCALE_AUTO )
  {
    UBYTE     max = 0;
    
    if( grr_data->db.scell_info.rxlev_avg NEQ RXLEV_AVG_INVALID )
    {
      max = grr_data->db.scell_info.rxlev_avg;
    }

    for ( i = 0; i < no_of_cells ; i++ )
    {
      if (enh_cell_list[i].rxlev_avg > max )
      {
        max = enh_cell_list[i].rxlev_avg;
      }
    }
    
    if ( max > 63 )
    {
      /* If the max value is greater than 63, we scale by 10 so that
         rxlev values of maximum cells will come into range(0 - 63) */
      scale = 10;
    }
    else
    {
      scale = 0;
    }
  }
  else
  {
    scale = scale_order * 10;
  }

  /* Apply the scale to the values to be reported for SCELL and NCELLs */

  if( grr_data->db.scell_info.rxlev_avg NEQ RXLEV_AVG_INVALID )
  {
    grr_data->db.scell_info.rxlev_avg -= scale;
  }

  for ( i = 0; i < no_of_cells ; i++ )
  {
    if (enh_cell_list[i].rxlev_avg > scale)
    {
      enh_cell_list[i].rxlev_avg -= scale;
    }
    else
    {
      enh_cell_list[i].rxlev_avg = 0;
    }

    if (enh_cell_list[i].rxlev_avg > 63)
    {
      enh_cell_list[i].rxlev_avg = 63;
    }
  }
  return (scale/10);
}/* cs_nc_scale_rxlev */

/*
+------------------------------------------------------------------------------
| Function    : cs_build_enh_meas_rpt
+------------------------------------------------------------------------------
| Description : This function is used to build PEMR
|
| Parameters  : u_enh_meas_report - Pointer to ENH measurement report
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL cs_build_enh_meas_rpt ( T_U_ENHNC_MEAS_REPORT *u_enh_meas_report )
{
  T_NC_ORDER nc_ord        = cs_get_network_ctrl_order( TRUE );
  /*UBYTE     size_avail     = MSG_SIZE_PEMR - MAND_SIZE_PEMR;*/
  BOOL   pbcch_present     = grr_is_pbcch_present( );
  UBYTE  psi3_cm = grr_get_psi3_cm();
  T_nc_meas_rep_enh *nc_meas_rep_enh = &(u_enh_meas_report->nc_meas_rep_enh);
  UBYTE used_bits = 0; /* for formality sake only, not used here */
  UBYTE rxlev_scell;
  UBYTE v_i_scell;
  UBYTE i_scell;
  BOOL rsc_avail = FALSE;
  BOOL isc_avail = FALSE;

  TRACE_FUNCTION( "cs_build_enh_meas_rpt" );

  /* process message type */
  u_enh_meas_report->msg_type = U_ENHNC_MEAS_REPORT_c;

  /* process TLLI */
  grr_set_buf_tlli( &u_enh_meas_report->tlli_value, grr_get_tlli( ) );

  /* Process NC Measurements */

  /* process NC mode */
  switch( nc_ord )
  {
    case NC_NC1: nc_meas_rep_enh->nc_mode = NCMODE_NC1; break;
    case NC_NC2: nc_meas_rep_enh->nc_mode = NCMODE_NC2; break;

    default:
    TRACE_ERROR( "cs_build_enh_meas_rpt no valid NC mode" ); 
    return( FALSE );
  }
  
  /* process BA used, PSI3 change mark and PMO used */
  if( pbcch_present EQ FALSE )
  {
    u_enh_meas_report->nc_meas_rep_enh.ba_psi3_str.flag = FALSE;
    u_enh_meas_report->nc_meas_rep_enh.ba_psi3_str.v_ba_ind_used = TRUE;
    u_enh_meas_report->nc_meas_rep_enh.ba_psi3_str.v_ba_ind_used_3g = TRUE;
    u_enh_meas_report->nc_meas_rep_enh.ba_psi3_str.ba_ind_used =
      psc_db->ba_ind;
  }
  else
  {
    u_enh_meas_report->nc_meas_rep_enh.ba_psi3_str.flag = TRUE;
    u_enh_meas_report->nc_meas_rep_enh.ba_psi3_str.v_psi3_cm = TRUE;
    u_enh_meas_report->nc_meas_rep_enh.ba_psi3_str.psi3_cm = psi3_cm;
  }
  
  u_enh_meas_report->nc_meas_rep_enh.ba_psi3_str.pmo_ind_used =
    psc_db->nc_ms.pmo_ind;

  /* process serving cell data - rxlev and ilev */
  cs_process_serving_cell_data(&rxlev_scell, &v_i_scell, &i_scell, &used_bits,
                                 &rsc_avail, &isc_avail);
  
  if((rsc_avail EQ TRUE) AND (isc_avail EQ FALSE))
  {
    /* Only RXLEV of SCELL avalable. ILEV of SCELL not available */
    nc_meas_rep_enh->v_serv_cell_data = TRUE;
    nc_meas_rep_enh->serv_cell_data.v_i_scell = FALSE;
    nc_meas_rep_enh->serv_cell_data.rxlev_scell = rxlev_scell;
  }
  else if(rsc_avail EQ FALSE)
  {
    /* RXLEV of SCELL not avalable. Hence SCELL info is not reported */
    nc_meas_rep_enh->v_serv_cell_data = FALSE;
  }
  else if((rsc_avail EQ TRUE) AND (isc_avail EQ TRUE))
  {
    /* Both RXLEV and ILEV of SCELL available and reported */
    nc_meas_rep_enh->v_serv_cell_data = TRUE;
    nc_meas_rep_enh->serv_cell_data.v_i_scell = TRUE;
    nc_meas_rep_enh->serv_cell_data.rxlev_scell = rxlev_scell;
    nc_meas_rep_enh->serv_cell_data.i_scell = i_scell;
  }

  /* process NC measurement results */
  if(cs_fill_nc_enh_meas_results(nc_meas_rep_enh) EQ FALSE)
  {
    nc_meas_rep_enh->v_nc_rep_quan = FALSE;
  }

  /* process BSIC_SEEN and SCALE */
  if(nc_meas_rep_enh->c_rep_invalid_bsic_info NEQ 0)
  {
    nc_meas_rep_enh->bsic_seen = TRUE;
  }
  else
  {
    nc_meas_rep_enh->bsic_seen = FALSE;
  }

  nc_meas_rep_enh->scale = grr_data->db.pemr_params.scale_used;

  return(TRUE);
    
} /* cs_build_enh_meas_rpt() */


/*
+------------------------------------------------------------------------------
| Function    : cs_fill_nc_meas_results
+------------------------------------------------------------------------------
| Description : This function is used to fill ENH measurements of neighbour 
|               cells in the PEMR message
|               
|
| Parameters  : Pointer to ENH meas parameters struct
|
+------------------------------------------------------------------------------
*/

LOCAL BOOL cs_fill_nc_enh_meas_results(T_nc_meas_rep_enh *nc_meas_rep_enh)
{
  UBYTE     size_avail = MSG_SIZE_PEMR - MAND_SIZE_PEMR;
  UBYTE     i;
  UBYTE     highest_index = 0;
  T_ENH_CELL_LIST *enh_cell_list = &(grr_data->db.enh_cell_list[0]);
  T_ENH_BIN *sorted_list = &grr_data->db.sorted_enh_cell_list;
  UBYTE     index_0 = 1;

  nc_meas_rep_enh->nc_rep_quan.c_reporting_quantity_bmp = 0;
  nc_meas_rep_enh->c_rep_invalid_bsic_info = 0;
  
  
  TRACE_FUNCTION( "cs_fill_nc_meas_results" );

  /*lint -e{437} Passing struct '' to ellipsis */
  TRACE_EVENT_P1("dump=%x",grr_data->db.enh_cell_list[0]);

  if(grr_data->db.sorted_enh_cell_list.num_valid EQ 0)
  {
    return(FALSE);
  }

  nc_meas_rep_enh->v_nc_rep_quan = TRUE;
  
  for(i = 0; i < sorted_list->num_valid; i++)
  {
    if(enh_cell_list[sorted_list->enh_index[i]].bsic.status EQ BSIC_VALID)
    {
      /*Fill it in valid BSIC bit map reporting, enh_cell_list[i]->index 
        corresponds to the bit in the report. Find whether size permits */
        TRACE_EVENT_P2("I%x = %x",i,enh_cell_list[sorted_list->enh_index[i]].index);
      nc_meas_rep_enh->nc_rep_quan.
        reporting_quantity_bmp[enh_cell_list[sorted_list->enh_index[i]].index].
        v_reporting_quantity = TRUE;
      nc_meas_rep_enh->nc_rep_quan.
        reporting_quantity_bmp[enh_cell_list[sorted_list->enh_index[i]].index].
        reporting_quantity = enh_cell_list[sorted_list->enh_index[i]].rxlev_avg;
      
      size_avail = size_avail - index_0;
      index_0 = 0;
        
      if ( enh_cell_list[sorted_list->enh_index[i]].index > highest_index ) 
      {
        /* When the place where the RXLEV has to be filled requires
           additional bits in bit map, then we have to account for
           these single bits and additional 6 bits for RXLEV */
        if ( size_avail >= (enh_cell_list[sorted_list->enh_index[i]].index 
                            - highest_index) + NC_RXLEV_N_LEN )
        {
          /* This means we require atleast enh_cell_list[i]->index - 
             highest_index+6 bits in bit map to include this rxlev */
          /* 6 bits for RXLEV itself */
          size_avail = size_avail - 
            (enh_cell_list[sorted_list->enh_index[i]].index - highest_index
             + NC_RXLEV_N_LEN);
          highest_index = enh_cell_list[sorted_list->enh_index[i]].index;
          nc_meas_rep_enh->nc_rep_quan.
            c_reporting_quantity_bmp = 
            enh_cell_list[sorted_list->enh_index[i]].index + 1; /* counter is index+1 */
          continue;
        }
        else if (size_avail >= NC_RXLEV_N_LEN)
        {
          continue;
        }
        else
          break; /* No more filling possible */       
      }
      else if (size_avail >= NC_RXLEV_N_LEN)
      {
        size_avail -= NC_RXLEV_N_LEN; /* size for bit map is already accounted for*/
        nc_meas_rep_enh->nc_rep_quan.
              c_reporting_quantity_bmp = highest_index +1;
        continue;
      }
      else
      {
        break; /*no more inclusion of results is possible*/
      }              
    }
    else
    {
      /* fill in invalid BSIC list since cell is not present in the neighbour cell
         list. Here the index that needs to be filled is index of the ARFCN in BA(list)*/
      if ( size_avail > NC_INVBSIC_PEMR )
      {
        nc_meas_rep_enh->v_rep_invalid_bsic_info = TRUE;
        nc_meas_rep_enh->rep_invalid_bsic_info
        [nc_meas_rep_enh->c_rep_invalid_bsic_info].bcch_freq_ncell
        = enh_cell_list[sorted_list->enh_index[i]].index;
        nc_meas_rep_enh->rep_invalid_bsic_info
          [nc_meas_rep_enh->c_rep_invalid_bsic_info].bsic
          = enh_cell_list[sorted_list->enh_index[i]].bsic.bsic;
        nc_meas_rep_enh->rep_invalid_bsic_info
          [nc_meas_rep_enh->c_rep_invalid_bsic_info].rxlev_ncell
          = enh_cell_list[sorted_list->enh_index[i]].rxlev_avg;
        nc_meas_rep_enh->c_rep_invalid_bsic_info++;
        size_avail -= NC_INVBSIC_PEMR;
      }          
    }
  }

  return(TRUE);

}/* cs_fill_nc_enh_meas_results */

/*
+------------------------------------------------------------------------------
| Function    : cs_build_enh_sorted_strng_arfcn_list
+------------------------------------------------------------------------------
| Description : The purpose of this function is to sort the cells into various
|               bins and in desc order of RXLEV in each of the bin. The ARFCNs
|               in the sorted bin are finally sent to RR for decoding purpose
|               
|
| Parameters  : Pointers to the final sorted bin for storage
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_build_enh_sorted_strng_arfcn_list(T_ENH_STRNG_ARFCN_LIST *f_bin)
{
  T_ENH_STRNG_ARFCN_LIST s_bin, r_bin;

  UBYTE total_arfcns;

  f_bin->num = 0;
  
  TRACE_FUNCTION( "cs_build_enh_sorted_strng_arfcn_list" );

  /* Sort ARFCNs into Serving, Non-serving or the Rest*/
  total_arfcns = cs_sort_arfcns_into_bins(&s_bin, &r_bin);

  /* Sort the ARFCNs in desc order of RXLEV and store them in the final bin */
  if(total_arfcns NEQ 0)
  {
    cs_sort_store_in_desc_order_rxlev(&s_bin, f_bin);
    cs_sort_store_in_desc_order_rxlev(&r_bin, f_bin);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : cs_sort_arfcns_into_bins
+------------------------------------------------------------------------------
| Description : The purpose of this function is to sort the ARFCNs into various
|               bins. 
|               
|
| Parameters  : Pointers to the the bins
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE cs_sort_arfcns_into_bins(T_ENH_STRNG_ARFCN_LIST *p_s_bin,
                                     T_ENH_STRNG_ARFCN_LIST *p_r_bin)
{
  UBYTE     i;
  UBYTE     band, sc_band;
  UBYTE     total_cells = 0;
  
  p_s_bin->num = 0;
  p_r_bin->num = 0;

  TRACE_FUNCTION( "cs_sort_arfcns_into_bins" );

  sc_band = cs_get_band_index_for_emr (psc_db->pbcch.bcch.arfcn);
  
  /* Check the ARFCNs in the NC_MVAL and store them in Serving or
     Rest bins according to their bands */
  i = 0;
  while( (i < MAX_NR_OF_NC_MVAL) AND (total_cells <= MAX_NR_OF_NCELL) )
  {
    if( (grr_data->db.nc_mval_list.nc_mval[i].arfcn NEQ RRGRR_INVALID_ARFCN) 
      AND (grr_data->db.nc_mval_list.nc_mval[i].status EQ NC_MVAL_STAT_ASSIGNED) )
    {
      band = cs_get_band_index_for_emr(grr_data->db.nc_mval_list.nc_mval[i].arfcn);
      if (band EQ sc_band)
      {
        p_s_bin->meas[p_s_bin->num].arfcn =
          grr_data->db.nc_mval_list.nc_mval[i].arfcn;
        p_s_bin->meas[p_s_bin->num].rxlev = 
          grr_data->db.nc_mval_list.nc_mval[i].rla_p.lev;
        p_s_bin->num++;
        
      }
      else /* Process the non-serving bands */
      {
        p_r_bin->meas[p_r_bin->num].arfcn = 
          grr_data->db.nc_mval_list.nc_mval[i].arfcn;
        p_r_bin->meas[p_r_bin->num].rxlev = 
          grr_data->db.nc_mval_list.nc_mval[i].rla_p.lev;
        p_r_bin->num++;
      }
      total_cells = p_s_bin->num + p_r_bin->num;
    }
    i++;
  } /* end of while */
  return(total_cells);
}/* cs_sort_arfcns_into_bins */

/*
+------------------------------------------------------------------------------
| Function    : cs_sort_in_desc_order_rxlev
+------------------------------------------------------------------------------
| Description : This function sorts the cells collected in each bin, in
|               descending order of RXLEV. It will be stored in the final bin
|               as and when sorted.
|               
|
| Parameters  : Pointer to serving or multiband bin, Pointer to final bin
|
+------------------------------------------------------------------------------
*/

LOCAL void cs_sort_store_in_desc_order_rxlev(T_ENH_STRNG_ARFCN_LIST *p_bin, 
                                             T_ENH_STRNG_ARFCN_LIST *p_f_bin)
{
  UBYTE     i;
  UBYTE     j;
  UBYTE     max_rxlev;
  UBYTE     k;
  T_MEAS    temp;
  
  TRACE_FUNCTION( "cs_sort_store_in_desc_order_rxlev" );

  for (i = 0; i < p_bin->num; i++)
  {
    max_rxlev = p_bin->meas[i].rxlev;
    k = i;
    for ( j = i + 1; j < p_bin->num; j++ )
    {
      if ( p_bin->meas[j].rxlev > max_rxlev )
      {
        k = j;
        max_rxlev = p_bin->meas[j].rxlev;
      }
    }
    
    /* Store the ARFCN-RXLEV pair in the final bin in desc order of RXLEV */
    p_f_bin->meas[p_f_bin->num++] = p_bin->meas[k];

    if(k NEQ i)
    {
      temp = p_bin->meas[k];
      p_bin->meas[k] = p_bin->meas[i];
      p_bin->meas[i] = temp;
    }
  }
}/* cs_sort_store_in_desc_order_rxlev */

/*
+------------------------------------------------------------------------------
| Function    : cs_init_ba_bcch_nc_ref_list
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : list  - pointer to NC measurement pointer list
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_init_ba_bcch_nc_ref_list ()
{ 
  TRACE_FUNCTION( "cs_init_ba_bcch_nc_ref_list" );
  
  grr_data->db.ba_bcch_nc_ref_lst.number = 0;

}/* cs_init_ba_bcch_nc_ref_list */

/*
+------------------------------------------------------------------------------
| Function    : cs_build_ba_bcch_nc_freq_list
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : db - pointer to serving cell database
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_build_ba_bcch_nc_freq_list ()
{
  UBYTE i;
  
  TRACE_FUNCTION( "cs_build_ba_bcch_nc_freq_list" );

  /*
   * add neighbour cell information of SI2 and PMO to reference list
   */
  for( i = 0; i < psc_db->nc_ba_bcch_cw.number; i++ )
  {
    grr_data->db.ba_bcch_nc_ref_lst.info[i] = &psc_db->nc_ba_bcch_cw.info[i];
    grr_data->db.ba_bcch_nc_ref_lst.number++;
  }

  if(use_ba_gprs EQ FALSE)  
  {
    UBYTE number = psc_db->nc_ba_bcch_cw.number;
    for(i = 0; i < psc_db->nc_ms.ncmeas.list.number; i++)
    {
      grr_data->db.ba_bcch_nc_ref_lst.info[number] =
        &psc_db->nc_ms.ncmeas.list.info[i];
      grr_data->db.ba_bcch_nc_ref_lst.info[number]->index = number;
      grr_data->db.ba_bcch_nc_ref_lst.number++;
      number++;
    }
  } 
}

#endif
/*
+------------------------------------------------------------------------------
| Function    : cs_copy_rxlev_from_avg
+------------------------------------------------------------------------------
| Description : The function cs_copy_rxlev_from_avg() helps to retain the old
|               rxlev data obtained for the neighbour cells before the cell 
|               change. 
| Parameters  : Index of the nc_data->cell,nc_mval->rxlev_avg 
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_copy_rxlev_from_avg(UBYTE rxlev_avg,T_CELL *cell)
{
#define INITIAL_ACC_MEAS 4
  UBYTE i; /* used for counting */ 
  T_RLA_P_DATA *rla_p_data = &cell->rla_p_data;
  T_RXLEV_DATA *rxlev_data = &cell->rxlev_data;
  
  TRACE_FUNCTION( "cs_copy_rxlev_from_avg" );
  TRACE_EVENT_P3( "cs_copy_rxlev_from_avg arfcn=%ld LOI=%ld rxlev_avg=%ld", cell->arfcn,rla_p_data->loi,rxlev_avg);
  
  rla_p_data->loi    = INITIAL_ACC_MEAS - 1 ;
  rla_p_data->ovrflw = FALSE;
  
  for( i = 0; i < INITIAL_ACC_MEAS; i++ )
  {
    rla_p_data->meas[i].cnt     = 1;
    rla_p_data->meas[i].acc     = (USHORT)rxlev_avg;
    rla_p_data->meas[i].rpt_prd = 208;
  }
  
  rxlev_data->acc = rxlev_avg*INITIAL_ACC_MEAS*RXLEV_ACRCY; 
  rxlev_data->nbr = INITIAL_ACC_MEAS;
}
/*
+------------------------------------------------------------------------------
| Function    : cs_reuse_old_cell_rxlev
+------------------------------------------------------------------------------
| Description : The function cs_reuse_old_cell_rxlev() helps to reuse rxlev data
|               obtained for the neighbour cells in the old cell before the cell 
|               change. 
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_reuse_old_cell_rxlev()
{
  T_NC_MVAL  *nc_mval;
  UBYTE       i,found,mval_idx ;
  UBYTE       mval_index=0;
  
  TRACE_FUNCTION( "cs_reuse_old_cell_rxlev" );
  TRACE_EVENT( "cs_reuse_old_cell_rxlev" );
  
  for( i = 0; i < MPHP_NUMC_BA_GPRS_SC; i++ )
  {
#if 0	  
    mval_index = 0;
    nc_mval = grr_get_nc_mval( nc_data->cell[i].arfcn, RRGRR_INVALID_BSIC, &mval_index );
    if(nc_mval NEQ 0)
    {
      if(nc_mval->rla_p.stat EQ CS_RLA_P_VALID)
      {
        cs_copy_rxlev_from_avg( (UBYTE)nc_mval->rxlev_avg,&nc_data->cell[i] );
      }
      else
      {
        cs_reset_meas_result( &nc_data->cell[i] );		
      }
    }
    else
    {
      cs_reset_meas_result( &nc_data->cell[i] );
    }
#else
    found=0;
    for( mval_idx = 0; mval_idx < MAX_NR_OF_NC_MVAL; mval_idx++ )
    {
      nc_mval = &grr_data->db.nc_mval_list.nc_mval[mval_idx];
      if((UBYTE)nc_mval->rxlev_avg EQ 0xFF AND 
          nc_mval->rla_p.stat EQ CS_RLA_P_VALID)
      {
        TRACE_EVENT_P4("PATCH: arfcn=%ld avg_lev=%ld rla_p_lev=%ld stat=%ld "
                    ,nc_mval->arfcn,nc_mval->rxlev_avg,nc_mval->rla_p.lev,nc_mval->rla_p.stat);
      }
      if(
          (nc_mval->arfcn EQ nc_data->cell[i].arfcn) 
          AND 
          (nc_mval->rla_p.stat EQ CS_RLA_P_VALID)
          AND
          ((UBYTE)nc_mval->rxlev_avg NEQ 0xFF)
        )
      {
        cs_copy_rxlev_from_avg( (UBYTE)nc_mval->rxlev_avg,&nc_data->cell[i] );
        found=1;
      }
    }
    if(!found)
      cs_reset_meas_result( &nc_data->cell[i] );
#endif
  }
}







