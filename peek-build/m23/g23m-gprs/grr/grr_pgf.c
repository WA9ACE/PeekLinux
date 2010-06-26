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
|  Purpose :  This module implements local functions for service PG of
|             entity GRR.
+-----------------------------------------------------------------------------
*/

#ifndef GRR_PGF_C
#define GRR_PGF_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

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

#include "grr_f.h"      /* */
#include "grr_pgf.h"    /* */

#include "grr_ctrls.h"    /* */
#include "grr_meass.h"
#include "grr_css.h"
#include "grr_em.h"     /*for Engineering mode*/

#include <string.h>     /* for memcpy*/
#include <stdio.h>     /* for memcpy*/

/*==== CONST ================================================================*/

const USHORT NC2_NON_DRX_TABLE[8] = {0, 240, 480, 720, 960, 1200, 1440, 1920};

/*==== LOCAL VARS ===========================================================*/
static UBYTE NON_DRX_TABLE[] = {0, 1, 2, 4, 8, 16, 32, 64};

/*==== GLOBAL VARS ===========================================================*/



/*==== PRIVATE FUNCTIONS ====================================================*/
LOCAL void pg_stop_t_nc2_ndrx( void );
LOCAL void pg_stop_non_drx_timer( void );
LOCAL void pg_start_non_drx_timer(USHORT milliseconds);
LOCAL BOOL pg_decode_imsi(T_ms_id ms_id, UBYTE *digits, UBYTE *nr_digits);
/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : pg_init
+------------------------------------------------------------------------------
| Description : The function pg_init() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_init ( void )
{
  TRACE_FUNCTION( "pg_init" );

  pg_init_params();

  grr_data->pg.v_emlpp_prio = FALSE; /* valid flag for emlpp_priority*/
  grr_data->pg.packet_mode  = PACKET_MODE_NULL;

  INIT_STATE(PG,PG_NULL);
} /* pg_init() */

/*
+------------------------------------------------------------------------------
| Function    : pg_init_params
+------------------------------------------------------------------------------
| Description : The function pg_init_params() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_init_params ( void )
{
  TRACE_FUNCTION( "pg_init_params" );

  psc_db->last_pg_mode = psc_db->network_pg_mode = REORG_PAGING;

} /* pg_init_params() */

/*
+------------------------------------------------------------------------------
| Function    : pg_process_pp_req
+------------------------------------------------------------------------------
| Description : The function pg_process_pp_req() ....
|
| Parameters  : T_D_PAGING_REQ *paging_req: pointer to the decoded message,
                UBYTE state: state of pg state machine: IDLE, TRANSFER, TIMER_3172
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_process_pp_req ( T_D_PAGING_REQ *paging_req, UBYTE state )
{
  char i;
  ULONG ul;

  TRACE_FUNCTION( "pg_process_pp_req" );

  if(paging_req->v_pers_lev)
  { /*copy persistence level parameters  into database*/
    grr_save_persistence_level (&(paging_req->pers_lev));
  }
  /*if(paging_req->v_nln)
  { message contains at least one page request for RR connection establishment
    04.08: NLN(PCH) Notification List Number
    "The presence of the NLN(PCH) field indicates that if an NCH is present, reduced NCH monitoring can be used, and
    gives the NLN(PCH) value, to be used as specified in 3.3.3."
    This is used by VGCS MSs...

  }*/
  if(paging_req->rep_page_info_trnc_grp.v_rep_page_info)
  {
    for (i=0; i < paging_req->rep_page_info_trnc_grp.c_rep_page_info; i++)
    {
      if(paging_req->rep_page_info_trnc_grp.rep_page_info[i].v_rep_page_s1
          AND
         grr_t_status( T3172_1 ) EQ 0 /* T3172 is not running*/
        )
      {
        /*
         * page request for TBF establishment
         */
        switch(state)
        {
          case PG_IDLE:
            if(paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s1.v_ptmsi)
            {
              /*
               *Paging with PTMSI
               */
              if(grr_data->db.ms_id.new_ptmsi NEQ GMMRR_TMSI_INVALID)
              {
                if(grr_check_ptmsi(&(paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s1.ptmsi)))
                {
                  TRACE_EVENT("Paging with PTIMSI for TBF");
                  sig_pg_ctrl_downlink_transfer ( GMMRR_PTMSI );
                  i = paging_req->rep_page_info_trnc_grp.c_rep_page_info; /* break condition for for loop*/
                }
                else
                {
                  TRACE_EVENT("Paging with wrong PTIMSI for TBF: IGNORE");
                }
              }
              else
              {
                TRACE_EVENT("Paging with PTMSI, but no valid PTMSI in MS");
              }
            }
            else
            {
              /*
               *Paging with Mobile Identity (IMSI)
               */
              UBYTE dummy_digits[MAX_IMSI_DIGITS];
              UBYTE dummy_nr_digits = 0;

              if(pg_decode_imsi(paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s1.ms_id,
                dummy_digits, &dummy_nr_digits))
              {

                 if( dummy_nr_digits
                      EQ
                      grr_data->db.ms_id.imsi.number_of_digits)/* check the number of digits*/
                  {
                    if( !memcmp
                        ( grr_data->db.ms_id.imsi.digit, /* IMSI stored in the data_base*/
                          dummy_digits, /* received IMSI*/
                          grr_data->db.ms_id.imsi.number_of_digits
                        )
                      )
                    {
                        /* The MS was paged with the IMSI*/
                      TRACE_EVENT("Paging with Mobile Identity (IMSI) for TBF");
                      sig_pg_ctrl_downlink_transfer ( GMMRR_IMSI );
                      i = paging_req->rep_page_info_trnc_grp.c_rep_page_info; /* break condition for for loop*/
                    }
                    else
                    {
                      TRACE_EVENT("Paging with wrong IMSI for TBF: IGNORE");
                    }

                  }
              }
            }
            break;
          default:
            /*
             * ignore paging request for TBF establishment, do
             * not check the content
             */
            break;
        }
      }

      if(paging_req->rep_page_info_trnc_grp.rep_page_info[i].v_rep_page_s2)
      {
        /*
         * page request for RR connection establishment
         */
        /*
         * In packet access mode, class A or B shall respond to a pp_req for
         * RR connection est.
         * During the time T3172 is running, the mobile station shall ignore all
         * received PACKET PAGING REQUEST messages except paging request to trigger
         * RR connection establishment.
         */
        switch(state)
        {
          case PG_IDLE:
          case PG_TRANSFER:
          case PG_ACCESS:
            if(paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s2.v_tmsi_field)
            {
              /*
               *Paging with TMSI
               */
              if(grr_data->db.ms_id.tmsi NEQ GMMRR_TMSI_INVALID)
              {
                ul = grr_buffer2ulong((BUF_ptmsi *)&(paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s2.tmsi_field));
                if(ul EQ grr_data->db.ms_id.tmsi)
                {
                  TRACE_EVENT("Paging with Mobile Identity (TMSI) for RR est");

                  /* Set some parameters needed  RR connection establishment*/
                  grr_data->pg.ch_needed = paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s2.chan_need;
                  grr_data->pg.v_emlpp_prio = paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s2.v_emlpp_prio;
                  grr_data->pg.emlpp_prio = paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s2.emlpp_prio;

                   /* MS was paged with TMSI for RR connection establishment*/
                  grr_data->pg.type = ID_TMSI;
                  sig_pg_ctrl_rr_est_req ( PG_ON_PCCCH);
                  i = paging_req->rep_page_info_trnc_grp.c_rep_page_info; /* break condition for for loop*/
                }
                else
                {
                  TRACE_EVENT("Paging with wrong TMSI for RR: IGNORE");
                }
              }
              else
              {
                TRACE_EVENT("Paging with TMSI, but no valid TMSI in MS");
              }
            }
            else
            {
              /*
               *Paging with Mobile Identity (IMSI)
               */
              UBYTE dummy_digits[MAX_IMSI_DIGITS];
              UBYTE dummy_nr_digits = 0;

              if(pg_decode_imsi(paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s2.ms_id,
                dummy_digits, &dummy_nr_digits))
              {

                 if( dummy_nr_digits
                      EQ
                      grr_data->db.ms_id.imsi.number_of_digits)/* check the number of digits*/
                  {
                    if( !memcmp
                        ( grr_data->db.ms_id.imsi.digit, /* IMSI stored in the data_base*/
                          dummy_digits, /* received IMSI*/
                          grr_data->db.ms_id.imsi.number_of_digits
                        )
                      )
                    {
                        /* The MS was paged with the IMSI*/
                      TRACE_EVENT("Paging with Mobile Identity (IMSI) for RR est");

                      /* Set some parameters needed  RR connection establishment*/
                      grr_data->pg.ch_needed = paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s2.chan_need;
                      grr_data->pg.v_emlpp_prio = paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s2.v_emlpp_prio;
                      grr_data->pg.emlpp_prio = paging_req->rep_page_info_trnc_grp.rep_page_info[i].rep_page_s2.emlpp_prio;

                      /* MS was paged with IMSI for RR connection establishment*/
                      grr_data->pg.type = ID_IMSI;
                      sig_pg_ctrl_rr_est_req ( PG_ON_PCCCH );
                      i = paging_req->rep_page_info_trnc_grp.c_rep_page_info; /* break condition for for loop*/
                    }
                    else
                    {
                      TRACE_EVENT("Paging with wrong IMSI for RR: IGNORE");
                    }
                }
              }
            }
            break;
        default:
            /*
             * ignore paging request for RR connection establishment, do
             * not check the content
             */
            break;
        }
      }
    }
  }
} /* pg_process_pp_req() */



/*
+------------------------------------------------------------------------------
| Function    : pg_send_stop
+------------------------------------------------------------------------------
| Description : The function pg_send_stop() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_send_stop ( void )
{
  TRACE_FUNCTION( "pg_send_stop" );

  if (grr_data->is_pg_started EQ TRUE)
  {
    PALLOC (mphp_stop_pccch_req, MPHP_STOP_PCCCH_REQ);
    PSEND (hCommL1, mphp_stop_pccch_req);
  }
  grr_data->is_pg_started = FALSE;

  pg_stop_non_drx_timer();
  pg_stop_t_nc2_ndrx();

} /* pg_send_stop() */



/*
+------------------------------------------------------------------------------
| Function    : pg_send_start
+------------------------------------------------------------------------------
| Description : The function pg_send_start() ....
|
| Parameters  : UBYTE page_mode
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_send_start ( UBYTE page_mode )
{
  UBYTE  pccch_group; /* 0 .. KC-1*/
  USHORT dummy;

  TRACE_FUNCTION( "pg_send_start" );

  if(grr_is_non_drx_period())
    page_mode = REORG_PAGING;

  switch(page_mode)
  {
    case  NORMAL_PAGING:
    case  EXT_PAGING:
    case  REORG_PAGING:
    case  SAME_PAGING:
      break;/*O.K.*/
    default:
      /* same as before: nothing to do return*/
      TRACE_EVENT("page_mode is corrupt: assume same as before: return");
      return;
  }

  if(psc_db->paging_group.kc EQ 0)
  {
    /*
     * No PCCCH description: return
     */
    TRACE_ERROR("psc_db->paging_group.kc EQ 0");
    return;
  }

  {
    PALLOC( start_pccch, MPHP_START_PCCCH_REQ );

    start_pccch->imsi_mod = grr_imsi_mod( );
    pccch_group           = ( UBYTE )( start_pccch->imsi_mod % psc_db->paging_group.kc );

    if( !grr_get_pccch_freq_par(  pccch_group,
                                 &start_pccch->p_ch_des.p_chan_sel,
                                 &start_pccch->p_freq_list ) )
    {
      PFREE( start_pccch );

      return;
    }

    start_pccch->kcn             = psc_db->paging_group.kc;     /*<  KC multiplied by N             */
    start_pccch->split_pg_cycle  = grr_data->ms.split_pg_cycle; /*<  Split paging cycle             */
    start_pccch->bs_pag_blks_res = psc_db->pccch.bs_pag_blks;   /*<  No paging no PBCCH             */
    start_pccch->bs_pbcch_blks   = psc_db->pccch.bs_pbcch_blks; /*<  Number of PBCCH per multiframe */
    start_pccch->pb              = psc_db->g_pwr_par.pb;        /*<  Power reduction value          */
    start_pccch->p_ch_des.tsc    = psc_db->paging_group.pccch[pccch_group].tsc; /*<  Training sequence code   */
    start_pccch->p_ch_des.tn     = psc_db->paging_group.pccch[pccch_group].tn;

    if( start_pccch->p_ch_des.p_chan_sel.hopping )
    {
      TRACE_EVENT_P8( "Hopping PCCCH - ma_num:%d hsn/maio:%ld tsc:%d tn:%d f1:%d f2:%d f3:%d f4:%d",
                      psc_db->paging_group.pccch[pccch_group].ma_num,
                      start_pccch->p_ch_des.p_chan_sel.p_rf_ch.arfcn,
                      start_pccch->p_ch_des.tsc,
                      start_pccch->p_ch_des.tn,
                      start_pccch->p_freq_list.p_rf_chan_no.p_radio_freq[0],
                      start_pccch->p_freq_list.p_rf_chan_no.p_radio_freq[1],
                      start_pccch->p_freq_list.p_rf_chan_no.p_radio_freq[2],
                      start_pccch->p_freq_list.p_rf_chan_no.p_radio_freq[3] );
    }
    else
    {
      TRACE_EVENT_P3( "Static PCCCH - arfcn:%ld tsc:%d tn:%d",
                      start_pccch->p_ch_des.p_chan_sel.p_rf_ch.arfcn,
                      start_pccch->p_ch_des.tsc,
                      start_pccch->p_ch_des.tn );

      start_pccch->p_ch_des.p_chan_sel.p_rf_ch.arfcn =
        grr_g23_arfcn_to_l1( start_pccch->p_ch_des.p_chan_sel.p_rf_ch.arfcn );
    }

    /*
     * Calculate downlink signalling counter
     */
       /* DRX period is used, there is a valid SPILT_PG_CYCLE value */
    dummy = grr_data->ms.split_pg_cycle;
    

    /* Downlink signalling counter*/
    dummy = ( ( 90 * dummy ) / 64 < 10 ) ? 10 : ( 90 * dummy ) / 64;

    if(!grr_data->is_pg_started)
    {
      grr_data->pg.initial_dsc = dummy;
      grr_data->pg.dsc         = dummy;

      GRR_EM_SET_DSC_VAL(dummy);

    }
    if( grr_data->pg.is_l1_ref_tn_changed NEQ psc_db->paging_group.pccch[pccch_group].tn )
    {
      sig_pg_cs_pause( );
    }

    /*Read PCCCH */
    {
      start_pccch->page_mode =  page_mode;
      PSEND(hCommL1, start_pccch);
    }
    if( grr_data->pg.is_l1_ref_tn_changed NEQ psc_db->paging_group.pccch[pccch_group].tn  )
    {
      sig_pg_cs_resume( );

      grr_data->pg.is_l1_ref_tn_changed = psc_db->paging_group.pccch[pccch_group].tn;
    }

    psc_db->last_pg_mode = page_mode;

    switch(page_mode)
    {
      case  NORMAL_PAGING:
        TRACE_EVENT("page mode NORMAL");
        break;/*O.K.*/
      case  EXT_PAGING:
        TRACE_EVENT("page mode EXT");
        break;/*O.K.*/
      case  REORG_PAGING:
        TRACE_EVENT("page mode REORG");
        break;/*O.K.*/
      case  SAME_PAGING:
        TRACE_EVENT("page mode SAME");
        break;/*O.K.*/
    }

  }
  grr_data->is_pg_started = TRUE;

} /* pg_send_start() */

/*
+------------------------------------------------------------------------------
| Function    : pg_start_non_drx_timer
+------------------------------------------------------------------------------
| Description : The function pg_start_non_drx_timer() starts the timer for
|               non DRX period
|
| Parameters  : USHORT milliseconds
|
+------------------------------------------------------------------------------
*/
LOCAL void pg_start_non_drx_timer(USHORT milliseconds)
{
  TRACE_FUNCTION( "pg_start_non_drx_timer");

  psc_db->non_drx_timer_running = TRUE;
  vsi_t_start(GRR_handle,T_TRANS_NON_DRX, milliseconds);
}/* pg_start_non_drx_timer*/

/*
+------------------------------------------------------------------------------
| Function    : pg_stop_non_drx_timer
+------------------------------------------------------------------------------
| Description : The function pg_stop_non_drx_timer() stops the timer for
|               non DRX period
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_stop_non_drx_timer(void)
{
  TRACE_FUNCTION( "pg_stop_non_drx_timer");

  if(psc_db->non_drx_timer_running)
  {
    psc_db->non_drx_timer_running = FALSE;
    vsi_t_stop(GRR_handle,T_TRANS_NON_DRX);
  }
}/* pg_stop_non_drx_timer*/

/*
+------------------------------------------------------------------------------
| Function    : pg_start_t_nc2_ndrx
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_start_t_nc2_ndrx( UBYTE nc_non_drx_period )
{ 
  TRACE_FUNCTION( "pg_start_t_nc2_ndrx");
  
  if( nc_non_drx_period NEQ 0 )
  {
    USHORT nc2_drx_period = NC2_NON_DRX_TABLE[nc_non_drx_period];
    
    psc_db->nc2_non_drx_period_running = TRUE;
    
    vsi_t_start( GRR_handle, T_NC2_NON_DRX, nc2_drx_period );

    TRACE_EVENT_P1( "Timer T_NC2_NON_DRX started: %d",  nc2_drx_period );
  }
}/* pg_start_t_nc2_ndrx */

/*
+------------------------------------------------------------------------------
| Function    : pg_stop_t_nc2_ndrx
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_stop_t_nc2_ndrx( void )
{
  TRACE_FUNCTION( "pg_stop_t_nc2_ndrx" );

  if( psc_db->nc2_non_drx_period_running EQ TRUE )
  {
    psc_db->nc2_non_drx_period_running = FALSE;
  
    vsi_t_stop( GRR_handle, T_NC2_NON_DRX );

    TRACE_EVENT( "Timer T_NC2_NON_DRX stopped" );
  }
} /* pg_stop_t_nc2_ndrx */

/*
+------------------------------------------------------------------------------
| Function    : pg_handle_non_drx_timers
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_handle_non_drx_timers(void)
{
  USHORT non_drx_timer = 0;
  TRACE_FUNCTION("pg_handle_non_drx_timers");

  non_drx_timer = ( psc_db->non_drx_timer < psc_db->gprs_cell_opt.drx_t_max ) ?
                    psc_db->non_drx_timer : psc_db->gprs_cell_opt.drx_t_max;

  non_drx_timer = (USHORT)(NON_DRX_TABLE[non_drx_timer]*1000);/* in milliseconds*/

  pg_start_non_drx_timer(non_drx_timer);

}/* pg_handle_non_drx_timers */

/*
+------------------------------------------------------------------------------
| Function    : pg_non_drx
+------------------------------------------------------------------------------
| Description : Handles the timer of non DRX mode
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_non_drx ( void )
{
  TRACE_FUNCTION( "pg_non_drx" );

  switch( GET_STATE( PG ) )
  {
    case PG_IDLE:
      /*
       * To ensure that the paging mode is not set to page normal in other states
       * than packet idle mode, the function grr_is_packet_idle_mode is called in
       * advance.
       *
       * State PG_IDLE not necessarily means that GRR is in packet idle mode. In
       * case GRR is currently executing the RRGRR_STOP_TASK_REQ/CNF procedure,
       * just the services CPAP and TC changes their states. All other services
       * remain in state IDLE, which is not quite correct. A similar scenario
       * occurs during execution of the MPHP_ASSIGNMENT_REQ/CON procedure.
       */
      if( grr_is_packet_idle_mode( ) EQ TRUE )
      {
        pg_handle_idle_mode( PTM_IDLE );
      }
      else
      {
        TRACE_EVENT( "pg_non_drx: Timeout T_TRANS_NON_DRX handled by next packet idle mode" );
      }
      break;
    default:
      TRACE_EVENT( "pg_non_drx: Timeout T_TRANS_NON_DRX handled by next PG_IDLE" );
      break;
  }
}/* pg_non_drx*/

/*
+------------------------------------------------------------------------------
| Function    : pg_decode_imsi
+------------------------------------------------------------------------------
| Description : Handles the timer of non DRX mode
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL pg_decode_imsi(T_ms_id ms_id, UBYTE *digits, UBYTE *nr_digits)
{
  UBYTE odd_even_type, i;
  BOOL is_odd;
  TRACE_FUNCTION("pg_decode_imsi");

  *nr_digits = 0;
  if(ms_id.ms_id_len > 0)
  {
    odd_even_type = (ms_id.ident_digit[0] & 0x0f);

    if(odd_even_type & 1) /* Check IMSI or not*/
    {
      is_odd = odd_even_type & 0x08; /* odd: ung.*/
      digits[0] = (ms_id.ident_digit[0] >> 4);
      *nr_digits = 1;

      for(i=1; i< ms_id.ms_id_len; i++)
      {
        if(!is_odd AND (i EQ (ms_id.ms_id_len-1)))
        {
          digits[*nr_digits] = (ms_id.ident_digit[i] & 0x0f);
          *nr_digits = (*nr_digits) + 1;
        }
        else
        {
          digits[*nr_digits] = (ms_id.ident_digit[i] & 0x0f);
          *nr_digits = (*nr_digits) + 1;
          digits[*nr_digits] = (ms_id.ident_digit[i] >> 4);
          *nr_digits = (*nr_digits) + 1;
        }
      }
    }
    else
    {
      /*No IMSI, ignore*/
      return FALSE;
    }
  }
  else
  {
    /* No mobile identity: strange*/
    return FALSE;
  }
  return TRUE;
}/* pg_decode_imsi*/

/*
+------------------------------------------------------------------------------
| Function    : pg_handle_classB_NMO_III
+------------------------------------------------------------------------------
| Description : Handles NMO III for Class B mobile
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_handle_classB_NMO_III(T_PG_TRANS transition)
{
  TRACE_FUNCTION("pg_handle_classB_NMO_III");

  switch(transition)
  {
    case T3172_PTM:
    case ACCESS_PTM:
    case IDLE_PTM:
      if(grr_data->ms.reverts_NMO_III)
      {
        /*
         * The MS reverts to CC or CG in NMO III
         * In this state IDLE->TRANSFER it can be only a CG mobile
         * it shall stop PCCCH reading, if available
         * it shall stop also CCCH reading, if running
         */
        TRACE_EVENT("IDLE/ACCESS->PTM: NMO III Class B reverts. Stop CCCH (and PCCCH)");
        if(grr_is_pbcch_present())
        {
          /*
           * Stop monitoring PCCCH
           */
          pg_send_stop();
        }
        /*
         * Stop monitoring CCCH
         */
        sig_pg_ctrl_stop_mon_ccch();
      }
      else
      {
        /*
         * This is non reverting Class B
         * If PBCCH is present it should stop PCCCH and start CCCH
         * else it should only monitor CCCH
         */
        TRACE_EVENT("IDLE/ACCESS->PTM: NMO III Class B. Start CCCH (and stop PCCCH)");
        if(grr_is_pbcch_present())
        {
          /*
           * Stop monitoring PCCCH
           */
          pg_send_stop();
        }
        /*
         * Start monitoring CCCH (Normal Paging)
         */
        if( grr_data->ms.tbf_mon_ccch
#ifdef REL99
           AND
           !psc_db->gprs_cell_opt.gprs_ext_bits.gprs_ext_info.bss_paging_coord
#endif
          )
        {
          sig_pg_ctrl_start_mon_ccch(PAG_MODE_PTM_NP);
        }
      }
      break;
    case NULL_IDLE:
    case PTM_IDLE:
      if(grr_data->ms.reverts_NMO_III)
      {
        /*
         * The MS reverts to CC or CG in NMO III
         * In this state TRANSFER/NULL->IDLE it can be only a CG mobile
         * it shall start PCCCH reading, if available
         * it shall start also CCCH reading, if NO PBCCH
         */
        TRACE_EVENT("PTM/NULL->IDLE: NMO III Class B reverts. Start (P) or CCCH");
        if(grr_is_pbcch_present())
        {
          /*
           * Start monitoring PCCCH
           */
          pg_send_start( psc_db->last_pg_mode );

        }
        else
        {
          /*
           * Start monitoring CCCH (Normal Paging)
           */
          sig_pg_ctrl_start_mon_ccch(PAG_MODE_DEFAULT);
        }
      }
      else
      {
        /*
         * This is non reverting Class B
         * If PBCCH is present it should start PCCCH and start CCCH
         * else it should only monitor CCCH
         */
        TRACE_EVENT("PTM/NULL->IDLE: NMO III Class B. Start (P) and CCCH");
        if(grr_is_pbcch_present())
        {
          /*
           * Start monitoring PCCCH
           */
          pg_send_start( psc_db->last_pg_mode );
          /*
           * Start monitoring CCCH (Normal Paging), before doing this use PAG_MODE_PIM_NP
           * to enter later to idle mode
           * Exceptional situation: if NON DRX is used, REORG is only for PCCCH
           */
          sig_pg_ctrl_start_mon_ccch(PAG_MODE_PIM_NP);
        }
        else
        {
          /*
           * Start monitoring CCCH (Normal Paging)
           * Normal Paging
           */
          sig_pg_ctrl_start_mon_ccch(PAG_MODE_DEFAULT);
        }
      }
      break;
    case ACCESS_IDLE:
      if(grr_data->ms.reverts_NMO_III)
      {
        /*
         * The MS reverts to CC or CG in NMO III
         * In this state ACCESS->IDLE it can be only a CG mobile
         * it shall start PCCCH reading, if available
         * it shall start CCCH reading, if NO PBCCH
         */
        TRACE_EVENT("ACCESS->IDLE: NMO III Class B reverts. Start (P) or CCCH");
        if(!grr_is_pbcch_present())
        {
          /*
           * Start monitoring CCCH (Normal Paging)
           */
          sig_pg_ctrl_start_mon_ccch(PAG_MODE_DEFAULT);
        }
      }
      else
      {
        /*
         * This is non reverting Class B
         * If PBCCH is present it should start PCCCH and start CCCH
         * else it should only monitor CCCH
         */
        TRACE_EVENT("ACCESS->IDLE: NMO III Class B. Start (P) and CCCH");
        if(grr_is_pbcch_present())
        {
          /*
           * Start monitoring PCCCH
           */
          pg_send_start( psc_db->last_pg_mode );
          /*
           * Start monitoring CCCH (Normal Paging)
           * Exceptional situation: if NON DRX is used, we RORG is only for PCCCH
           */
          if(grr_data->ms.tbf_mon_ccch
#ifdef REL99
              AND
             !psc_db->gprs_cell_opt.gprs_ext_bits.gprs_ext_info.bss_paging_coord
#endif
            )
          {
            sig_pg_ctrl_start_mon_ccch(PAG_MODE_PTM_NP);
          }
        }
        else
        {
          /*
           * Start monitoring CCCH (Normal Paging)
           * Normal Paging
           */
          sig_pg_ctrl_start_mon_ccch(PAG_MODE_DEFAULT);
        }
      }
      break;
    case PTM_ACCESS:
    case IDLE_ACCESS:
      /*
       * Not necessary
       */
      break;
  }/*switch*/
}

/*
+------------------------------------------------------------------------------
| Function    : pg_handle_access_mode
+------------------------------------------------------------------------------
| Description : Handles the paging when entering the packet access mode.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_handle_access_mode ( void )
{
  TRACE_FUNCTION( "pg_handle_access_mode" );


  if( grr_is_pbcch_present( ) )
  {
    if( !grr_data->is_pg_started OR psc_db->last_pg_mode NEQ REORG_PAGING )
    {
      /*
       * enable the L1 reading USF values
       */
      pg_send_start( PG_REORG );
    }
  }
  else
  {
    /*
     * start monitoring CCCH
     */
    sig_pg_ctrl_start_mon_ccch( PAG_MODE_REORG );
  }
} /* pg_handle_access_mode() */

/*
+------------------------------------------------------------------------------
| Function    : pg_set_l1_ref_tn
+------------------------------------------------------------------------------
| Description : The function pg_set_l1_ref_tn () ....
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_set_l1_ref_tn ( UBYTE* out_tn, UBYTE in_tn )
{
  TRACE_FUNCTION( "pg_set_l1_ref_tn" );

  if( in_tn NEQ *out_tn )
  {
    grr_data->pg.is_l1_ref_tn_changed = TRUE;
  }

  *out_tn = in_tn;

} /* pg_set_l1_ref_tn */

/*
+------------------------------------------------------------------------------
| Function    : pg_handle_idle_mode
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_handle_idle_mode ( T_PG_TRANS transition )
{
  TRACE_FUNCTION( "pg_handle_idle_mode" );

  switch( grr_data->pg.nmo )
  {
    case GMMRR_NET_MODE_I:
      /*
       * NMOI:
       * ====
       * Paging on ( CCCH or PCCCH ) and PACCH, GPRS attached MS should
       * monitor one paging channel, so we have to stop monitoring CCCH
       * channel if PCCCH is present.
       */
      if( grr_is_pbcch_present( ) )
      {
        pg_send_start( psc_db->last_pg_mode );
      }
      else
      {
        sig_pg_ctrl_start_mon_ccch( PAG_MODE_DEFAULT );
      }
      break;

    case GMMRR_NET_MODE_II:
      /*
       * NMOII:
       * =====
       * Paging on CCCH, paging for both circuit switched and packet data
       * will be sent on CCCH, PCCCH has been not allocated in the cell.
       */
      switch( grr_data->ms.class_of_mode )
      {
        case GMMRR_CLASS_A:
/* BEGIN GRR_NMO_III */
        case GMMRR_CLASS_B:
/* END   GRR_NMO_III */
        case GMMRR_CLASS_BG:
        case GMMRR_CLASS_BC:
        case GMMRR_CLASS_CG:
        case GMMRR_CLASS_CC:
          sig_pg_ctrl_start_mon_ccch( PAG_MODE_DEFAULT );
        break;
      }
      break;

    case GMMRR_NET_MODE_III:
      /*
       * NMOIII:
       * =======
       * If PBCCH not present paging for both circuit switched and packet
       * data will be sent on CCCH,
       * if PBCCH is present paging for circuit switched data is sent on
       * CCCH and for packet data on PCCCH.
       */
      if( grr_is_pbcch_present( ) )
      {
        TRACE_EVENT( "GMMRR_NET_MODE_III PBCCH" );

        switch(grr_data->ms.class_of_mode)
        {
          case GMMRR_CLASS_A:
            pg_send_start( psc_db->last_pg_mode );
            sig_pg_ctrl_start_mon_ccch( PAG_MODE_PIM_NP );
            break;

          /*
           * We need to check whether the MS class B  mobile
           * reverts to CG or CC in NMO III. If not it should monitor PCCCH and CCCH
           * else it needs to monitor either CCCH or PCCCH
           */
/* BEGIN GRR_NMO_III */
          case GMMRR_CLASS_B:
/* END   GRR_NMO_III */
          case GMMRR_CLASS_BG:
          case GMMRR_CLASS_BC:
            pg_handle_classB_NMO_III( transition );
            break;

          case GMMRR_CLASS_CG:
            sig_pg_ctrl_stop_mon_ccch( );
            pg_send_start( psc_db->last_pg_mode );
            break;

          case GMMRR_CLASS_CC:
            sig_pg_ctrl_start_mon_ccch( PAG_MODE_DEFAULT );
            break;
         }
       }
       else
       {
        TRACE_EVENT ( "GMMRR_NET_MODE_III BCCH" );

        sig_pg_ctrl_start_mon_ccch( PAG_MODE_DEFAULT );
      }
      break;
  }
} /* pg_handle_idle_mode() */

/*
+------------------------------------------------------------------------------
| Function    : pg_handle_transfer_mode
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void pg_handle_transfer_mode ( T_PG_TRANS transition )
{
  TRACE_FUNCTION( "pg_handle_transfer_mode" );

  switch( grr_data->pg.nmo )
  {
    case GMMRR_NET_MODE_I:
      /*
       * NMOI:
       * ====
       * Paging on ( CCCH or PCCCH ) and PACCH, GPRS attached MS should
       * monitor one paging channel, so we have to stop monitoring CCCH
       * channel if PCCCH is present.
       */
      if( grr_is_pbcch_present( ) )
      {
        pg_send_stop( );
      }
      else
      {
        sig_pg_ctrl_stop_mon_ccch( );
      }
      break;

    case GMMRR_NET_MODE_II:
      /*
       * NMOII:
       * =====
       * Paging on CCCH, paging for both circuit switched and packet data
       * will be sent on CCCH, PCCCH has been not allocated in the cell.
       *
       * The MS should monitor CCCH except for the CLASS_CG.
       */
      switch( grr_data->ms.class_of_mode )
      {
        case GMMRR_CLASS_A:
/* BEGIN GRR_NMO_III*/
        case GMMRR_CLASS_B:
/* END   GRR_NMO_III*/
        case GMMRR_CLASS_BG:
        case GMMRR_CLASS_BC:
          if( grr_data->ms.tbf_mon_ccch 
#ifdef REL99
             AND
             !psc_db->gprs_cell_opt.gprs_ext_bits.gprs_ext_info.bss_paging_coord
#endif			
            )
          {
            sig_pg_ctrl_start_mon_ccch( PAG_MODE_PTM_NP );
          }
          else
          {
            sig_pg_ctrl_stop_mon_ccch( );
          }
          break;

        case GMMRR_CLASS_CG:
          sig_pg_ctrl_stop_mon_ccch( );
          break;
      }
      break;

    case GMMRR_NET_MODE_III:
      /*
       * NMOIII:
       * =======
       * If PBCCH not present paging for both circuit switched and packet
       * data will be sent on CCCH,
       * if PBCCH is present paging for circuit switched data is sent on
       * CCCH and for packet data on PCCCH.
       */
      switch( grr_data->ms.class_of_mode )
      {
        case GMMRR_CLASS_A:
          if( grr_is_pbcch_present( ) )
          {
            pg_send_stop( );
          }

          sig_pg_ctrl_start_mon_ccch( PAG_MODE_PTM_NP );
          break;

/* BEGIN GRR_NMO_III*/
        case GMMRR_CLASS_B:
/* END   GRR_NMO_III*/
        case GMMRR_CLASS_BG:
        case GMMRR_CLASS_BC:
          pg_handle_classB_NMO_III( transition );
          break;

        case GMMRR_CLASS_CG:
          if( grr_is_pbcch_present( ) )
          {
            pg_send_stop( );
          }
          else
          {
            sig_pg_ctrl_stop_mon_ccch( );
          }
          break;
      }
      break;
  }
} /* pg_handle_transfer_mode() */
