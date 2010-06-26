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
|  Purpose :  This module implements local functions for service PSI of
|             entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_PSIF_C
#define GRR_PSIF_C
#endif

#define ENTITY_GRR

#ifdef _SIMULATION_

/*
 * Report warning 4005 as an error.
 * 
 * There are identical macro definitons in the GRR message and the RRGRR SAP 
 * document which should be aligned at all the time
 * (e.g. GPRS_RXLEV_ACCESS_MIN_INVALID, GPRS_MS_TXPWR_MAX_CCH, etc.)
 */
#pragma warning( error : 4005 )

#endif /* #ifdef _SIMULATION_ */

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
#include "grr_f.h"
#include "grr_psif.h"   /* */
#include "grr_ctrls.h"  /* */
#include "grr_css.h"   /* signals exchanged between PSI and CS*/
#include "grr_meass.h"   /* signals exchanged between PSI and MEAS*/

#include <string.h>    /* for memcpy */
#include <stdio.h>     /* for memcpy */
#include "grr_em.h"     /*for Engineering mode*/

/*==== DIAGNOSTICS ==========================================================*/

/*==== CONST ================================================================*/
#define PBCCH_NOT_PRESENT    0x0           /* PBCCH not present in the cell  */
#define PBCCH_PRESENT        0x1           /* PBCCH present in the cell      */
/*==== LOCAL VARS ===========================================================*/
LOCAL void psi_reset_psi_pos  ( void );
#ifndef _TARGET_
LOCAL void psi_fill_rel_pos(UBYTE *src, UBYTE max_src, UBYTE *psi_nr, UBYTE *pos_array);
#endif /*_TARGET_*/
/*==== PRIVATE FUNCTIONS ====================================================*/

LOCAL void psi_reset_complete_acq ( void );
LOCAL void psi_reset_psi1 (void );
LOCAL void psi_reset_psi2 (void );
LOCAL void psi_reset_psi3 (void );
LOCAL void psi_reset_psi3bis (void );
#if defined (REL99) AND defined (TI_PS_FF_EMR)
LOCAL void psi_reset_psi3ter (void );
#endif
LOCAL void psi_reset_psi4 (void );
LOCAL void psi_reset_psi5 (void );
#ifdef REL99
LOCAL void psi_reset_psi8 (void);
#endif
LOCAL void psi_reset_psi13 (void );
LOCAL void psi_init_states_of_psi ( void );
LOCAL void psi_init_rfl_psi2(void);
LOCAL void psi_init_cell_alloc(void);
LOCAL void psi_init_gprs_ms_alloc(BOOL psi2_only);

LOCAL void psi_copy_si13_params(T_D_SYS_INFO_13* si13);
LOCAL void psi_copy_psi13_params(T_PSI_13* psi13);
LOCAL BOOL psi_check_change_field (UBYTE psi_change_field);
LOCAL UBYTE psi_compare_change_mark(UBYTE received_change_mark, UBYTE stored_change_mark);

LOCAL BOOL psi_is_pbcch_des_different(T_PSI_13* psi13);

/*
 * Private functions for handling of PSI3 and PSI3BIS messages. 
 * Reading of those messages in not ordered sequence.
 */
/*
 * Storing
 */
LOCAL UBYTE psi_store_ncell_param  ( T_ncell_par      *p_ncell_par, 
                                     UBYTE             c_ncell_par,
                                     UBYTE             v_ncell_par,
                                     T_INFO_TYPE       type,
                                     UBYTE             instance       );

LOCAL void psi_store_ncell_param2  ( T_PSI_3_BIS      *psi3bis,
                                     UBYTE             number         );
LOCAL BOOL psi_store_cs_param2     ( T_NC_LIST        *nc_list,
                                     T_ncell_par2_set *cs_par2,
                                     UBYTE             instance,
                                     UBYTE            *number,
                                     USHORT           *freq,
                                     USHORT            freq_diff,
                                     UBYTE             same_ra_scell,
                                     UBYTE             cell_ba,
                                     UBYTE             bcc            );

/*
 * Restoring 
 */
LOCAL void psi_restore_ncell_param ( void                             );

#ifdef REL99
LOCAL void psi_update_bss_sgsn_rel ( T_D_SYS_INFO_13  *si13, 
                                     BOOL              pbcch_status   );
#endif

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : psi_compare_change_mark()
+------------------------------------------------------------------------------
| Description : The function psi_compare_change_mark() .... compares the change_mark values
|               and indicates the incrementatiton value
|
| Parameters  : UBYTE received_change_mark, UBYTE stored_change_mark
|
+------------------------------------------------------------------------------
*/
LOCAL UBYTE psi_compare_change_mark(UBYTE received_change_mark, UBYTE stored_change_mark)
{
  UBYTE incremented_value = 0;

  if(received_change_mark NEQ stored_change_mark)
  {
    UBYTE dummy = (stored_change_mark + 1)%8;
    if(dummy EQ received_change_mark)
      incremented_value = 1;
    else
      incremented_value = 2;
  }
  return incremented_value;
}/* psi_compare_change_mark*/
/*
+------------------------------------------------------------------------------
| Function    : psi_stop_psi_reading()
+------------------------------------------------------------------------------
| Description : The function psi_stop_psi_reading() .... stops to read PSI messages
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_stop_psi_reading( ACQ_TYPE acq_type_in_idle)
{
  TRACE_FUNCTION( "psi_stop_psi_reading" );
  psc_db->acq_type = acq_type_in_idle;
  if(grr_is_pbcch_present())
  {
    PALLOC(mphp_scell_pbcch_req, MPHP_SCELL_PBCCH_STOP_REQ);
    PSEND(hCommL1,mphp_scell_pbcch_req);
  }
}/* psi_stop_psi_reading*/

/*
+------------------------------------------------------------------------------
| Function    : psi_check_acq_state
+------------------------------------------------------------------------------
| Description : The function psi_check_acq_state() .... checks the state of the acquisition
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_ACQ_STATE_RET psi_check_acq_state ( void )
{
  UBYTE n;
  T_ACQ_STATE_RET return_val = ACQ_RUNNING;

  TRACE_FUNCTION( "psi_check_acq_state" );

  switch(psc_db->acq_type)
  {
    case PARTIAL:
      return_val = ACQ_PART_OK;
      for(n = 0; n <MAX_PSI; n++)
      {
        if(psc_db->state_of_PSI[n].state EQ NEEDED)
        {
          /* 
           * partial acquisition not completed 
           */
          return_val = ACQ_RUNNING;
        }
      }
      if(return_val EQ ACQ_PART_OK)
      {
        psi_stop_10sec(); /* partial acquisition completed */
        psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread PSI1*/
        psc_db->psi1_params.first_psi1 = FALSE;
#ifdef REL99
        /* Partial acquisition is complete. Send CBCH info
         * to RR if needed. 
         */
        if(psc_db->send_cbch_info_ind)
        {
           sig_psi_ctrl_cbch_info_ind();
           psc_db->send_cbch_info_ind = FALSE;
        }
#endif
      }
      break;
    case COMPLETE:
      return_val = ACQ_COMP_OK;
      /*
       * Check whether reading process completed or not
       */
      for(n = 0; n <MAX_PSI; n++)
      {
        if(psc_db->state_of_PSI[n].state EQ NEEDED)
        {
          /* 
           * acquisition of some PSI not completed 
           */
          {
            if(n EQ 3)
            {
              TRACE_EVENT("acq. incomplete PSI3bis missing!");
            }
#if defined (REL99) AND defined (TI_PS_FF_EMR)
            else if(n EQ 4)
            {
              TRACE_EVENT("acq. incomplete PSI3ter  missing!");                
            }
            else if(n EQ 7)
            {
              TRACE_EVENT("acq. incomplete PSI8  missing!");                
            }
            else
            {
              TRACE_EVENT_P1("acq. incomplete PSI%d missing!", ((n>3)?n-1:n+1));               
            }
#else
            else
            {
              TRACE_EVENT_P1("acq. incomplete PSI%d missing!", ((n>3)?n:n+1));               
            }
#endif
          }
          return_val = ACQ_RUNNING;
          break; /* break for loop*/
        }
      }
      if(return_val EQ ACQ_COMP_OK) 
      {
        /*
         * Start timer for 10 sec. if running
         */
        TRACE_EVENT("Acq. complete");
        vsi_t_stop(GRR_handle, T_COMP_PSI);
        psi_stop_10sec(); /* acquisition of all PSI  completed */
        psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread PSI1*/
        psc_db->psi1_params.first_psi1 = FALSE;
      }
      break;
    case PERIODICAL_PSI1_READING:
      psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread PSI1*/
      return_val = ACQ_PERIOD_OK;
      break;
    case FULL_PSI_IN_NEW_CELL:
      return_val = ACQ_NPSI_OK;
      psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread PSI1*/
      /*
       * E.g. read all PSI in the reselected cell
       * Check whether reading process completed or not
       */
      for(n = 0; n <MAX_PSI; n++)
      {
        if(psc_db->state_of_PSI[n].state EQ NEEDED)
        {
          /* 
           * acquisition of some PSI not completed 
           */
          return_val = ACQ_RUNNING;
          psc_db->acq_type = FULL_PSI_IN_NEW_CELL;
          break; /* break for loop*/
        }
      }
      if(n>MAX_PSI)
        psc_db->psi1_params.first_psi1 = FALSE;
      break;
    case NONE:
      /* nothing to do: we may received a PSI message without sending a request
       *                e.g. on PCCCH (PPCH: paging channel or paging group)
       */
      break;
    default:
      break;
  }
  return return_val;
} /* psi_check_acq_state() */


/*
+------------------------------------------------------------------------------
| Function    : psi_reset_complete_acq
+------------------------------------------------------------------------------
| Description : The function psi_reset_complete_acq() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_reset_complete_acq ( void )
{
  TRACE_FUNCTION( "psi_reset_complete_acq" );
  psc_db->acq_type = COMPLETE;
  
  psc_db->complete_acq.needed = TRUE;
  psc_db->complete_acq.psi1_ok = FALSE;
  psc_db->complete_acq.psi2_ok = FALSE;
  psc_db->complete_acq.made_at_least_one_attempt = FALSE;
}/* psi_reset_complete_acq*/

/*
+------------------------------------------------------------------------------
| Function    : psi_reset_psi1
+------------------------------------------------------------------------------
| Description : The function psi_reset_psi1() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL  void psi_reset_psi1 ( void )
{
  TRACE_FUNCTION( "psi_reset_psi1" );

  psc_db->psi1_params.pbcch_change_mark = NOT_SET;
  psc_db->psi1_params.psi_change_field = NOT_SET;
  psc_db->psi1_params.psi_cnt_lr = 0;
  psc_db->psi1_params.psi_cnt_hr = 0;
  psc_db->psi1_params.psi1_repeat_period = NOT_SET;
  psc_db->psi1_params.first_psi1 = TRUE;
}/* psi_reset_psi1*/

/*
+------------------------------------------------------------------------------
| Function    : psi_reset_psi2
+------------------------------------------------------------------------------
| Description : The function psi_reset_psi2() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_reset_psi2 ( void )
{
  UBYTE n;
  TRACE_FUNCTION( "psi_reset_psi2" );
  /* reset psi2 parameters*/
  psc_db->psi2_params.psi2_change_mark = NOT_SET; /*not set*/
  psc_db->psi2_params.psi2_count = NOT_SET; /*not set*/

  for(n = 0; n <MAX_NR_OF_INSTANCES_OF_PSI2 + 1; n++)
    psc_db->psi2_params.instances[n] = FALSE;

#ifdef REL99
  psc_db->v_add_psi = FALSE;
#endif
  psi_init_rfl_psi2();
  psi_init_cell_alloc();
  psi_init_gprs_ms_alloc(TRUE);

} /* psi_reset_psi2 */

/*
+------------------------------------------------------------------------------
| Function    : psi_reset_psi3
+------------------------------------------------------------------------------
| Description : The function psi_reset_psi3() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL  void psi_reset_psi3 ( void )
{
  TRACE_FUNCTION( "psi_reset_psi3" );
  /* reset psi3 parameters*/
  psc_db->psi3_params.psi3_change_mark = NOT_SET;
  psc_db->psi3_params.psi3_bis_count = NOT_SET;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  grr_init_nc_list( &psc_db->nc_cw.list );
#endif
} /* psi_reset_psi3 */

/*
+------------------------------------------------------------------------------
| Function    : psi_reset_psi3bis
+------------------------------------------------------------------------------
| Description : The function psi_reset_psi3bis() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL  void psi_reset_psi3bis ( void )
{
  UBYTE n;
  TRACE_FUNCTION( "psi_reset_psi3bis" );

  psc_db->psi3bis_params.psi3bis_change_mark = NOT_SET;
  psc_db->psi3bis_params.psi3bis_index = 0;

  for(n = 0; n <MAX_NR_OF_INSTANCES_OF_PSI3BIS + 1; n++)
  psc_db->psi3bis_params.instances[n] = FALSE;

  grr_init_nc_list( &psc_db->nc_cw.list );
} /* psi_reset_psi3bis */

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : psi_reset_psi3ter
+------------------------------------------------------------------------------
| Description : The function psi_reset_psi3ter() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL  void psi_reset_psi3ter ( void )
{
  UBYTE n;
  TRACE_FUNCTION( "psi_reset_psi3ter" );

  psc_db->psi3ter_params.psi3ter_change_mark = NOT_SET;
  psc_db->psi3ter_params.prev_psi3ter_index = 0;
  psc_db->psi3ter_params.psi3ter_index = 0;

  for(n = 0; n <MAX_NR_OF_INSTANCES_OF_PSI3TER + 1; n++)
  psc_db->psi3ter_params.instances[n] = FALSE;
  
  psc_db->enh_cw.gprs_rept_prio_desc.num_cells = 0;
#ifdef TI_PS_FF_RTD
  /* Rtd values has to be reset to not available */
  for( n = 0; n < MAX_NR_OF_NCELL; n++ )
    psc_db->rtd[n] = RTD_NOT_AVAILABLE;
#endif /* #ifdef TI_PS_FF_RTD */


} /* psi_reset_psi3ter */
#endif


/*
+------------------------------------------------------------------------------
| Function    : psi_reset_psi4
+------------------------------------------------------------------------------
| Description : The function psi_reset_psi4() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL  void psi_reset_psi4 ( void )
{
  UBYTE n;
  TRACE_FUNCTION( "psi_reset_psi4" );
  /* reset psi_reset_psi4 parameters*/
  psc_db->psi4_params.psi4_index = 0;
  psc_db->psi4_params.psi4_change_mark = NOT_SET;

  for(n = 0; n <MAX_NR_OF_INSTANCES_OF_PSI4 + 1; n++)
  psc_db->psi4_params.instances[n] = FALSE;


} /* psi_reset_psi4 */


/*
+------------------------------------------------------------------------------
| Function    : psi_reset_psi5
+------------------------------------------------------------------------------
| Description : The function psi_reset_psi5() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL  void psi_reset_psi5 ( void )
{
  UBYTE i;

  TRACE_FUNCTION( "psi_reset_psi5" );
  /* reset psi_reset_psi5 parameters*/
  psc_db->psi5_params.psi5_index = 0;
  psc_db->psi5_params.psi5_change_mark = NOT_SET;
  
  for( i = 0; i < MAX_NR_OF_INSTANCES_OF_PSI5; i++ )
  {
    psc_db->psi5_params.idx[i].start = RRGRR_INVALID_IDX;
    psc_db->psi5_params.idx[i].stop  = RRGRR_INVALID_IDX;
    psc_db->psi5_params.instances[i] = FALSE;
  }
  /*
   * used for checking the consistency of PSI5. instances[0]: number of instances
   */
  psc_db->psi5_params.instances[MAX_NR_OF_INSTANCES_OF_PSI5] = FALSE;

  grr_init_nc_param( &psc_db->nc_cw.param, TRUE );
  grr_init_xmeas_struct( &psc_db->ext_psi5 );
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  grr_init_enh_param(&psc_db->enh_cw, FALSE);
  grr_init_enh_param(&grr_data->psi.enh_param, FALSE);
#endif
} /* psi_reset_psi5 */

#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : psi_reset_psi8
+------------------------------------------------------------------------------
| Description : The function psi_reset_psi8() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL  void psi_reset_psi8 ( void )
{
  UBYTE n;
  TRACE_FUNCTION( "psi_reset_psi8" );
  /* reset psi_reset_psi8 parameters*/
  psc_db->psi8_params.psi8_index = 0;
  psc_db->psi8_params.psi8_change_mark = NOT_SET;

  for(n = 0; n <MAX_NR_OF_INSTANCES_OF_PSI8 + 1; n++)
   psc_db->psi8_params.instances[n] = FALSE;
  
  psc_db->send_cbch_info_ind = FALSE;
  psc_db->v_cbch_chan_desc = FALSE;
} /* psi_reset_psi8 */
#endif

/*
+------------------------------------------------------------------------------
| Function    : psi_reset_psi13
+------------------------------------------------------------------------------
| Description : The function psi_reset_psi13() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL  void psi_reset_psi13 ( void )
{
  TRACE_FUNCTION( "psi_reset_psi13" );

  grr_init_nc_param( &psc_db->nc_cw.param, TRUE );

} /* psi_reset_psi13 */

/*
+------------------------------------------------------------------------------
| Function    : psi_reset_all
+------------------------------------------------------------------------------
| Description : The function psi_reset_all() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_reset_all ( void )
{
  TRACE_FUNCTION( "psi_reset_all" );


  /* reset complete acq.*/
  psi_reset_complete_acq();

  /* reset PSI1 parameters */
  psi_reset_psi1();

  /* reset psi2 parameters*/
  psi_reset_psi2();

  /* reset psi3 parameters*/
  psi_reset_psi3();

  /* reset psi3bis parameters*/
  psi_reset_psi3bis();

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  /* reset psi3ter parameters*/
  psi_reset_psi3ter();
#endif

  /* reset psi4 parameters*/
  psi_reset_psi4();

  /* reset psi5 parameters*/
  psi_reset_psi5();

#ifdef REL99
  /* reset psi8 parameters*/
  psi_reset_psi8();
#endif

  /* reset psi13 parameters*/
  psi_reset_psi13();

  /* */
  psc_db->send_psi_status = FALSE;


  /* init the states of the PSI parameters*/
  psi_init_states_of_psi();

  psi_reset_si_entries();

} /* psi_reset_all() */



/*
+------------------------------------------------------------------------------
| Function    : psi_stop_10sec
+------------------------------------------------------------------------------
| Description : The function psi_stop_10sec() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_stop_10sec ( void )
{
  TRACE_FUNCTION( "psi_stop_10sec" );

  vsi_t_stop(GRR_handle,T_10_SEC);
} /* psi_stop_10sec() */



/*
+------------------------------------------------------------------------------
| Function    : psi_start_10sec
+------------------------------------------------------------------------------
| Description : The function psi_start_10sec() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_start_10sec ( void )
{
  TRACE_FUNCTION( "psi_start_10sec" );
  
  if(grr_data->psi.is_start_of_10_sec_allowed)
    vsi_t_start(GRR_handle,T_10_SEC, T_10_SEC_VALUE);
} /* psi_start_10sec() */



/*
+------------------------------------------------------------------------------
| Function    : psi_partial_acq
+------------------------------------------------------------------------------
| Description : The function psi_partial_acq() performs partial acquisition
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_partial_acq ( void )
{
  UBYTE psi_reading_type;
  UBYTE dummy=0;

  TRACE_FUNCTION( "psi_partial_acq" );
  
  psi_reading_type = PSI_IN_HR_AND_LR;

  psc_db->acq_type = PARTIAL;

  if(  psc_db->state_of_PSI[PSI2].state EQ NEEDED)
  {
    psi_reading_type = READ_PSI2;
    psi_reset_psi2();
    dummy++;
    TRACE_EVENT("PSI 2 needed");
  }
  
  if(  psc_db->state_of_PSI[PSI3].state EQ NEEDED)
  {
    psi_reading_type = READ_PSI3_3BIS;
    psi_reset_psi3();
    psi_reset_psi3bis();

#if defined (REL99) AND defined (TI_PS_FF_EMR)
    psi_reset_psi3ter();
#endif

    dummy++;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    TRACE_EVENT("PSI3/3bis/3ter  needed");
#else
    TRACE_EVENT("PSI3/3bis needed");
#endif
  }

  if(  psc_db->state_of_PSI[PSI4].state EQ NEEDED)
  {
    psi_reading_type = READ_PSI4;
    psi_reset_psi4();
    dummy++;
    TRACE_EVENT("PSI 4 needed");
  }

  if(  psc_db->state_of_PSI[PSI5].state EQ NEEDED)
  {
    psi_reading_type = READ_PSI5;
    psi_reset_psi5();
    dummy++;
    TRACE_EVENT("PSI 5 needed");
  }

#ifdef REL99
  if(  psc_db->state_of_PSI[PSI8].state EQ NEEDED)
  {
    psi_reading_type = READ_PSI8;
    psi_reset_psi8();
    dummy++;
    TRACE_EVENT("PSI 8 needed");
  }
#endif

  if(dummy > 1)
    psi_reading_type = PSI_IN_HR_AND_LR;

  psi_receive_psi(psi_reading_type);
} /* psi_partial_acq() */


/*
+------------------------------------------------------------------------------
| Function    : psi_init_states_of_psi
+------------------------------------------------------------------------------
| Description : The function psi_init_states_of_psi() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL  void psi_init_states_of_psi ( void )
{
  psc_db->state_of_PSI[PSI1].state     = NEEDED;
  psc_db->state_of_PSI[PSI2].state     = NEEDED;
  psc_db->state_of_PSI[PSI3].state     = NEEDED;
  psc_db->state_of_PSI[PSI3bis].state  = NEEDED;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  psc_db->state_of_PSI[PSI3ter].state  = NEEDED;
#endif
  psc_db->state_of_PSI[PSI4].state     = NEEDED;
  psc_db->state_of_PSI[PSI5].state     = NEEDED;
#ifdef REL99
  psc_db->state_of_PSI[PSI8].state     = NEEDED;
#endif
  psc_db->state_of_PSI[PSI13].state    = NEEDED;

}/* psi_init_states_of_psi() */

/*
+------------------------------------------------------------------------------
| Function    : psi_init_rfl_psi2
+------------------------------------------------------------------------------
| Description : The function psi_init_rfl_psi2() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_init_rfl_psi2(void)
{
  UBYTE n;

  /*
   * please note that only the RFL_NUMBERs defined 
   * in the PSI2 will be marked as invalid
   */
  for (n = 0; n < MAX_RFL; n++)
  {
    psc_db->rfl[n].num = NOT_SET;
  }
}/* psi_init_rfl_psi2() */
/*
+------------------------------------------------------------------------------
| Function    : psi_init_cell_alloc
+------------------------------------------------------------------------------
| Description : The function psi_init_cell_alloc() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_init_cell_alloc(void)
{
  UBYTE n;
  /*TRACE_EVENT("psi_init_cell_alloc");*/
  psc_db->v_cell_alloc = FALSE;
  for (n = 0; n < MAX_CELL_ALLOC; n++)
    psc_db->cell_alloc[n].rfl_num = NOT_SET;
}/* psi_init_cell_alloc() */

/*
+------------------------------------------------------------------------------
| Function    : psi_init_gprs_ms_alloc
+------------------------------------------------------------------------------
| Description : The function psi_init_gprs_ms_alloc() ....
|
| Parameters  : BOOL psi2_only
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_init_gprs_ms_alloc(BOOL psi2_only)
{
  UBYTE n;
  if(psi2_only)
  {
    TRACE_EVENT("remove only PSI2 entries in GPRS_MA");
  }
  else
  {
    TRACE_EVENT("remove all entries in GPRS_MA");
  }

  for (n = 0; n < MAX_GPRS_MS_ALLOC; n++)
  {
    if(psi2_only)
    {
      if(
          (psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num NEQ MA_NUMBER_4_PSI13_OR_CELL_ALLOC ) 
            AND  /* ignore MA_NUMBER with 14 or 15: 0..13 in PSI2*/
          (psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num NEQ MA_NUMBER_4_ASSIGNMENT )
        )
      {
        psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num = NOT_SET;
      }
    }
    else
    {
      psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num = NOT_SET;
    }
  }

  /*
   * When deleting the MA defined in the assignment message,
   * the corresponding RFL list should be deleted, too.
   */
  psc_db->gprs_ms_alloc_in_assignment.ma_num = NOT_SET;
  psc_db->rfl[MAX_RFL].num                   = NOT_SET;
}/* psi_init_gprs_ms_alloc() */

 /*
+------------------------------------------------------------------------------
| Function    : psi_init
+------------------------------------------------------------------------------
| Description : The function psi_init() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_init ( void )
{
  TRACE_FUNCTION( "psi_init" );

  psi_init_params();
  psi_reset_psi_pos( );

  INIT_STATE(PSI, PSI_NULL);

} /* psi_init() */



/*
+------------------------------------------------------------------------------
| Function    : psi_complete_acq
+------------------------------------------------------------------------------
| Description : The function psi_complete_acq() ....
|
| Parameters  : UBYTE acq_type: COMPLETE (in serving cell) or FULL_PSI_IN_NEW_CELL
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_complete_acq ( ACQ_TYPE acq_type )
{
  TRACE_FUNCTION( "psi_complete_acq" );
  /*
   * Start timer for 10 sec.
   */
  vsi_t_start(GRR_handle,T_COMP_PSI, T_10_SEC_VALUE);
  
  psi_receive_psi(READ_COMPLETE);

  psi_reset_all();

  psc_db->acq_type = acq_type;
  
  psc_db->state_of_PSI[PSI13].state = RECEIPT_OK; /* we do not need to read PSI13 again*/

} /* psi_complete_acq() */



/*
+------------------------------------------------------------------------------
| Function    : psi_send_psi_status
+------------------------------------------------------------------------------
| Description : The function psi_send_psi_status() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_send_psi_status (void )
{
  TRACE_FUNCTION( "psi_send_psi_status" );

  /* SZML-PSI/003 */

} /* psi_send_psi_status() */



/*
+------------------------------------------------------------------------------
| Function    : psi_stop_30sec
+------------------------------------------------------------------------------
| Description : The function psi_stop_30sec() ....
|
| Parameters  : BOOL start_again: whether the timer should be started again or not
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_stop_30sec (BOOL start_again )
{
  TRACE_FUNCTION( "psi_stop_30sec" );

  vsi_t_stop(GRR_handle,T_30_SEC);
  if(start_again)
  {
    /*
    TRACE_EVENT("30 sec. running");
    */
    vsi_t_start(GRR_handle,T_30_SEC, T_30_SEC_VALUE);
  }
  else
  {
    TRACE_EVENT("30 sec. stopped");
  }
} /* psi_stop_30sec() */



/*
+------------------------------------------------------------------------------
| Function    : psi_start_30sec
+------------------------------------------------------------------------------
| Description : The function psi_start_30sec() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_start_30sec (void )
{
  TRACE_FUNCTION( "psi_start_30sec" );
  /*
  TRACE_EVENT("30 sec. running");
  */
  vsi_t_start(GRR_handle,T_30_SEC, T_30_SEC_VALUE);
} /* psi_start_30sec() */



/*
+------------------------------------------------------------------------------
| Function    : psi_start_60sec
+------------------------------------------------------------------------------
| Description : The function psi_start_60sec() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_start_60sec ( void )
{
  TRACE_FUNCTION( "psi_start_60sec" );
  /*
  TRACE_EVENT("60 sec. running");
  */
  vsi_t_start(GRR_handle,T_60_SEC, T_60_SEC_VALUE);
} /* psi_start_60sec() */


/*
+------------------------------------------------------------------------------
| Function    : psi_stop_60sec
+------------------------------------------------------------------------------
| Description : The function psi_stop_60sec () ....
|
| Parameters  : BOOL start_again
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_stop_60sec ( BOOL start_again )
{
  TRACE_FUNCTION( "psi_stop_60sec " );

  vsi_t_stop(GRR_handle,T_60_SEC);
  if(start_again)
  {
    /*
    TRACE_EVENT("60 sec. running");
    */
    vsi_t_start(GRR_handle,T_60_SEC, T_60_SEC_VALUE);
  }
  else
  {
    TRACE_EVENT("60 sec. stopped");
  }

} /* psi_stop_60sec () */

/*
+------------------------------------------------------------------------------
| Function    : psi_copy_ma_from_psi13
+------------------------------------------------------------------------------
| Description : The function psi_copy_ma_from_psi13() ....
|
| Parameters  : T_gprs_ms_alloc* ms_alloc: pointer to T_gprs_ms_alloc
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_copy_ma_from_psi13(T_gprs_ms_alloc_ie* ms_alloc)
{
  UBYTE n;
  MCAST(si13,D_SYS_INFO_13); /* T_D_SYS_INFO_13  */
  
  TRACE_FUNCTION( "psi_copy_ma_from_psi13 " );

  if(D_SYS_INFO_13 NEQ si13->msg_type)
  {  /* PSI13 was received */
    for(n = 0; n < MAX_GPRS_MS_ALLOC; n++)
    {
      if( (psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num EQ MA_NUMBER_4_PSI13_OR_CELL_ALLOC) ||
          (psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num EQ NOT_SET) )
      {
        psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num = MA_NUMBER_4_PSI13_OR_CELL_ALLOC;
        memcpy(&(psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie),
                ms_alloc, sizeof(T_gprs_ms_alloc_ie));
        n = MAX_GPRS_MS_ALLOC; /* break for loop*/
      }
    }
  }
  else
  {  /* SI13 was received */
    UBYTE i;
    T_gprs_ma *gprs_ma = (T_gprs_ma*)ms_alloc;
    for(n = 0; n < MAX_GPRS_MS_ALLOC; n++)
    {
      if( (psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num EQ MA_NUMBER_4_PSI13_OR_CELL_ALLOC) ||
          (psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num EQ NOT_SET) )
      {
        memset(&(psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie), 0, 
                                                  sizeof(T_gprs_ms_alloc_ie));
        psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num = 
                                              MA_NUMBER_4_PSI13_OR_CELL_ALLOC;
        psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.hsn = 
                                                                 gprs_ma->hsn;
        if(gprs_ma->v_rfln)
        {
          psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.v_rfl_num_list
                                                                       = TRUE;
          psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.c_rfl_num_list
                                                            = gprs_ma->c_rfln;
          for(i = 0;i < gprs_ma->c_rfln;i++)
          {
            psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.rfl_num_list[i].rfl_num
                                                           = gprs_ma->rfln[i];
          }
        }
        psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.flag = 
                                                                 gprs_ma->hop;
        if(gprs_ma->hop)
        {
          if(gprs_ma->v_arfcn_idx)
          {
            psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.v_arfcn_index_list
                                                                       = TRUE;
            psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.c_arfcn_index_list
                                                       = gprs_ma->c_arfcn_idx;
            for(i = 0;i < gprs_ma->c_arfcn_idx;i++)
            {
              psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.arfcn_index_list[i].arfcn_index
                                                      = gprs_ma->arfcn_idx[i];
            }
          }
        }
        else
        {
          psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.v_ma_struct
                                                                       = TRUE;
          psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.ma_struct.ma_len
                                                         = gprs_ma->allo_len6;
          psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.ma_struct.c_ma_map
                                                       = gprs_ma->c_allo_bmp6;
          for(i = 0;i < gprs_ma->c_allo_bmp6;i++)
          {
            psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie.ma_struct.ma_map[i]
                                                      = gprs_ma->allo_bmp6[i];
          }
        }
        break;
      }
    }
  }
}/*psi_copy_ma_from_psi13*/

/*
+------------------------------------------------------------------------------
| Function    : psi_process_si13()
+------------------------------------------------------------------------------
| Description : The function psi_process_si13()....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_SI13_RET psi_process_si13( T_D_SYS_INFO_13 *si13)
{
  BOOL pbcch_was_present;
  T_SI13_RET ret_value = SI13_OK;
#ifdef REL99 
  UBYTE current_nw_rel = psc_db->network_rel;
#endif

  TRACE_FUNCTION( "psi_process_si13" );

  if(!si13->si13_rest_oct.v_si13_info)
  {
    TRACE_ERROR( "No restoctects present in SI13: reread si13" );
    ret_value = SI13_REREAD;
  }
  else
  {

#ifdef REL99
    /* Update the BSS release when the 1st SI13 message is received in case of 
     * PBCCH's PRESENCE. 
     * The correct BSS release will be updated after receiving PSIs
     */
    if(si13->si13_rest_oct.si13_info.flag1)
    {
      /* PBCCH is present in the cell */
      /* This is the 1st SI13 message */
      if(psc_db->psi13_params.bcch_change_mark EQ NOT_SET)
      {
        /* Update the BSS and the SGSN releases */
        psi_update_bss_sgsn_rel(si13, PBCCH_PRESENT);
      }
    }
    else
    {
      /* Update the BSS and SGSN releases every time the SI13 message is received 
       * in case of PBCCH's ABSENCE.
       */
      psi_update_bss_sgsn_rel(si13, PBCCH_NOT_PRESENT);

      if(psc_db->network_rel NEQ current_nw_rel)
      {
        TRACE_EVENT_P2("((SI13)BSS Network release changed from %d to %d <0 - REL_97, 1 - REL_99, 2 - REL_04>", 
          current_nw_rel, psc_db->network_rel);
      }
    }
#endif

    pbcch_was_present = grr_is_pbcch_present();
    /*
     * Copy PBCCH or non PBCCH desc. and change field
     */
    if( !pbcch_was_present AND si13->si13_rest_oct.si13_info.v_pbcch_des )
    {
      /* No PBCCH description was present in the GPRS_DATA_BASE or PBCCH was released:
       * SI13 message contains PBCCH description, so we have to start a complete acq of
       * PSI messages.
       */
      if(psc_db->psi13_params.bcch_change_mark EQ NOT_SET)
      {
        psi_copy_si13_params(si13);
      }

      psc_db->psi13_params.bcch_change_mark = si13->si13_rest_oct.si13_info.bcch_cm;
      ret_value = SI13_COMPLETE_PSI;
    }
    else
    {
      psi_copy_si13_params(si13);

      /* 
       * There is no PBCCH description present in the SI13 message and there can be
       * no PBCCH description in the GPRS_DATA_BASE
       */  
      if(psc_db->psi13_params.bcch_change_mark NEQ NOT_SET)
      {
        /*
         * This is not the first SI13 message.
         * Check BCCH_CHANGE_MARK
         */
        {
          UBYTE incremented_value =

            psi_compare_change_mark
            (
            si13->si13_rest_oct.si13_info.bcch_cm,
            psc_db->psi13_params.bcch_change_mark
            );

          if(incremented_value EQ 1)      /*partial acq*/
          {
            if(psi_is_update_needed(si13->si13_rest_oct.si13_info.si_cf))
            {
              ret_value = SI13_PARTIAL_SI;
            }
          }
          else if(incremented_value > 1) /* incremented_value > 1: complete acq*/
          {
            ret_value = SI13_COMPLETE_SI;
          }
        }
      }
      psc_db->psi13_params.bcch_change_mark = si13->si13_rest_oct.si13_info.bcch_cm;
    }
  }
  psc_db->state_of_PSI[PSI13].state = RECEIPT_OK;
  grr_set_pg_nmo();
  TRACE_EVENT_P1("SI13 processed: %d", ret_value);
  return ret_value;
} /* psi_process_si13() */

/*
+------------------------------------------------------------------------------
| Function    : psi_process_psi13
+------------------------------------------------------------------------------
| Description : The function psi_process_psi13() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL T_PSI13_RET psi_process_psi13 ( T_PSI_13 * psi13 )
{
  T_PSI13_RET ret_val = PSI13_OK;

  TRACE_FUNCTION( "psi_process_psi13" );
  
  if(grr_is_pbcch_present())
  {
    /*
     * PBCCH description already exists: we have to compare PBCCH parameters
     */
    if(psi_is_pbcch_des_different(psi13))
    {
      /* 
       * PBCCH no longer available
       * or PBCCH description is differnt from the saved one in the MS
       */
      if(psi13->flag)
      {
        ret_val =  PSI13_COMPLETE_PSI;
      }
      else
      {
        ret_val =  PSI13_PBCCH_RELEASED;
      }
    }
    else
    {
      /* 
       * PBCCH description is the same
       * Copy at least SI13_CHANGE_MARK and GPRS Mobile Allocation
       */
      psi_copy_psi13_params(psi13);
#ifdef REL99
      /* PSI13 message has been received when PBCCH is present.
       * CBCH information may have to be updated if hopping is 
       * allowed.
       */
      if(psc_db->v_cbch_chan_desc AND !psc_db->cbch_chan_desc.freq_par.v_arfcn)
      {
        sig_psi_ctrl_cbch_info_ind();       
      }
#endif
      /*
       * Check BCCH_CHANGE_MARK
       */
      {
        UBYTE incremented_value = psi_compare_change_mark(psi13->bcch_change_ma, psc_db->psi13_params.bcch_change_mark);
        psc_db->psi13_params.bcch_change_mark = psi13->bcch_change_ma;

        if(incremented_value EQ 1)      /*partial acq*/
        {
          if(psi_is_update_needed(psi13->si_change_ma))
          {
            ret_val = PSI13_PARTIAL_SI;
          }
        }
        else if(incremented_value > 1) /* incremented_value > 1: complete acq*/
        {
          ret_val = PSI13_COMPLETE_SI;
        }
      }
    }
  }
  else
  {
    /*
     * No PBCCH description present in the GPRS_DATA_BASE:
     * and we received a PSI13 in BCCH_TRANSFER state
     * So we have to check whether this message contains a
     * PBCCH description or not. If YES, complete acq. of PSI
     * messages should be started. If NO, BCCH_CHANGE_MARK should be compared
     */
    if(psi13->flag)
    {
      ret_val = PSI13_PBCCH_ESTABLISHED;    
    }
    else
    {
      /*
       * Copy PBCCH or non PBCCH desc. and change field
       */
      psi_copy_psi13_params(psi13);

      {
        UBYTE incremented_value = psi_compare_change_mark(psi13->bcch_change_ma, psc_db->psi13_params.bcch_change_mark);
        psc_db->psi13_params.bcch_change_mark = psi13->bcch_change_ma;

        if(incremented_value EQ 1)      /*partial acq*/
        {
          if(psi_is_update_needed(psi13->si_change_ma))
          {
            ret_val = PSI13_PARTIAL_SI;
          }
        }
        else if(incremented_value > 1) /* incremented_value > 1: complete acq*/
        {
          ret_val = PSI13_COMPLETE_SI;
        }
      }
    }
  }

#ifdef REL99
    /* Update the SGSN release */
  if (psi13->v_release_99_str_psi_13)
  {
    psc_db->sgsn_rel = psi13->release_99_str_psi_13.sgsnr ? PS_SGSN_99_ONWARDS : PS_SGSN_98_OLDER;

   /* Update the SGSN release in the Common library context */
    cl_nwrl_set_sgsn_release(psc_db->sgsn_rel);
  }
#endif

  psc_db->state_of_PSI[PSI13].state = RECEIPT_OK;

  grr_set_pg_nmo();
  return ret_val;
} /* psi_process_psi13() */

/*
+------------------------------------------------------------------------------
| Function    : psi_process_psi5
+------------------------------------------------------------------------------
| Description : The function psi_process_psi5() ....
|
| Parameters  : T_PSI_5 *psi5
|
+------------------------------------------------------------------------------
*/
GLOBAL T_PSI5_RET psi_process_psi5 (T_PSI_5 *psi5 )
{
  BOOL  is_psi5_ok;
  UBYTE n;
  BOOL  cpy_prm_set;

  TRACE_FUNCTION( "psi_process_psi5" );

  /*
   * Check consistency
   */
  if(psc_db->psi5_params.psi5_change_mark EQ NOT_SET)
  {
    /*
     * Copy change mark etc.
     */
    psc_db->psi5_params.psi5_change_mark = psi5->psi5_cm;
    psc_db->psi5_params.psi5_count   = psi5->psi5_cnt;
    psc_db->psi5_params.instances[0] = psi5->psi5_cnt;/* number of instances */
  }
  else
  {
    /*
     * There is a valid change mark present in the database
     * Check consistency of COUNT and INDEX
     */
    if(
       (psc_db->psi5_params.psi5_count NEQ psi5->psi5_cnt)
       OR
       (psc_db->psi5_params.psi5_count < psi5->psi5_ind)
       OR
       (psc_db->psi5_params.psi5_change_mark NEQ psi5->psi5_cm)
       )
    {
      /*
       * Error situation read a new PSI5
       * The PSI5 change mark field is changed each time information has been updated
       * A new value indicates that the mobile
       * station shall re-read the information from the PSI5
       */

      TRACE_ERROR( "PSI5: inconsistent parameter" );
      psc_db->is_ext_psi5_valid = FALSE;
      return PSI5_REREAD;
    }

    /* psi5_change_markhas not changed, i.e.
     * PSI5 message has not changed, we already have a consistent set, so we
     * do not have to read the PSI5 message
     */
    if((psc_db->psi5_params.psi5_change_mark EQ psi5->psi5_cm) &&
          (psc_db->state_of_PSI[PSI5].state EQ RECEIPT_OK))
    {
      /* Message has not changed, we already have a consistent set, so we
       * do not have to read it
       */
      TRACE_EVENT( "PSI5: message has not changed" );
      return PSI5_OK;
    }
  }

  psc_db->psi5_params.psi5_index = psi5->psi5_ind;


  if( psc_db->psi5_params.instances[psi5->psi5_ind + 1] EQ FALSE )
  {
    if( grr_data->nc2_on )
    {
      if( 
          psi5->v_nc_meas_par AND 
          (
            psc_db->nc_cw.param.idx EQ NOT_SET        OR
            psc_db->nc_cw.param.idx <  psi5->psi5_ind          
          )
        )
      {
        /*
         * This is the first time that we want to save network controlled 
         * measurement parameters or the last index for NC parameters were
         * not bigger than the currently received one.
         *
         * Maybe more than one instance of PSI5, store data in temporary location.
         */
        grr_data->psi.v_nc_param = TRUE;

        grr_prcs_nc_param_struct 
               ( &grr_data->psi.nc_param, &psi5->nc_meas_par, psi5->psi5_ind );
      }

      if( psi5->v_xmeas_par )
      {
        cpy_prm_set = ( psc_db->ext_psi5.idx EQ NOT_SET        OR
                        psc_db->ext_psi5.idx <  psi5->psi5_ind    );

        grr_prcs_xmeas_struct ( &psc_db->ext_psi5,
                                &psi5->xmeas_par,
                                cpy_prm_set, 
                                psi5->psi5_ind,
                                &psc_db->psi5_params.idx[psi5->psi5_ind].start,
                                &psc_db->psi5_params.idx[psi5->psi5_ind].stop );
      }

#if defined (REL99) AND defined (TI_PS_FF_EMR)
      if(psi5->v_release_99_str_psi_5 AND psi5->release_99_str_psi_5.v_enh_rep_param_struct)
      {
        psc_db->psi5_params.v_enh_rep_param_struct = TRUE;
        grr_prcs_enh_param_cw_temp 
        ( &grr_data->psi.enh_param, &psi5->release_99_str_psi_5.enh_rep_param_struct, 
        psi5->psi5_ind );
      } 
#endif
    }
    /*
     * check whether PSI5 has been received completely or not
     */
    psc_db->psi5_params.instances[psi5->psi5_ind + 1] = TRUE;
    is_psi5_ok = TRUE;
    for(n = 0; n <= psc_db->psi5_params.instances[0]; n++ )
    {
      if(!(psc_db->psi5_params.instances[n + 1]))
      {
        is_psi5_ok = FALSE;/* consistent set of PSI5 not complete */
        break;
      }
    }

    TRACE_EVENT_P2( "PSI5: received with count = %d, index = %d ", 
                                    psi5->psi5_cnt, psi5->psi5_ind);

    if(is_psi5_ok)
    {
      psc_db->state_of_PSI[PSI5].state = RECEIPT_OK;

      /*
       * the NC parameter are stored temporarily and
       * now transferred to final location
       */
      if( grr_data->nc2_on )
      {
        grr_prcs_nc_param_final ( &psc_db->nc_cw.param,
                                  &grr_data->psi.v_nc_param,
                                  &grr_data->psi.nc_param );

        grr_sort_ext_lst_freq ( &psc_db->ext_psi5.em1.list,
                                MAX_NR_OF_INSTANCES_OF_PSI5,
                                &psc_db->psi5_params.idx[0] );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
        if(rr_get_support_for_emr() AND psc_db->psi5_params.v_enh_rep_param_struct)
        {
          /*
           * the ENH parameter are stored temporarily and
           * now transferred to final location
           */
          memcpy(&(psc_db->enh_cw), &(grr_data->psi.enh_param),
            sizeof(T_GRR_ENH_PARA));
        }
        else
        {
          memset(&(psc_db->enh_cw), 0, sizeof(T_GRR_ENH_PARA));
          psc_db->enh_cw.rept_type = REPORT_TYPE_REP;
          psc_db->psi5_params.v_enh_rep_param_struct = FALSE;
        }
#endif
        /*
         * Inform MEAS that the Interference parameters are valid
         */
        psc_db->is_ext_psi5_valid = TRUE;

        return PSI5_MEAS_PARAM_VALID;
      }
    }
  }
  else
  {
    TRACE_EVENT_P2( "PSI5: already received with count = %d, index = %d",
                    psi5->psi5_cnt, psi5->psi5_ind );
  }

  return PSI5_OK;
} /* psi_process_psi5() */



/*
+------------------------------------------------------------------------------
| Function    : psi_process_psi4
+------------------------------------------------------------------------------
| Description : The function psi_process_psi4() ....
|
| Parameters  : T_PSI_4 *psi4
|
+------------------------------------------------------------------------------
*/
GLOBAL T_PSI4_RET psi_process_psi4 (T_PSI_4 *psi4 )
{
  UBYTE i,m,n;
  BOOL is_psi4_ok;
  TRACE_FUNCTION( "psi_process_psi4" );

  /*
   * Check consistency
   */
  if(psc_db->psi4_params.psi4_change_mark EQ NOT_SET)
  {
    /*
     * Copy change mark etc.
     */
    psc_db->psi4_params.psi4_change_mark = psi4->psi4_cm;
    psc_db->psi4_params.psi4_count   = psi4->psi4_cnt;
    psc_db->psi4_params.instances[0] = psi4->psi4_cnt;/* number of instances */
  }
  else
  {
    /*
     * There is a valid change mark present in the database
     * Check consistency of COUNT and INDEX
     */
    if(
       (psc_db->psi4_params.psi4_count NEQ psi4->psi4_cnt)
       OR
       (psc_db->psi4_params.psi4_count < psi4->psi4_ind)
       OR
       (psc_db->psi4_params.psi4_change_mark NEQ psi4->psi4_cm)
       )
    {
      /*
       * Error situation read a new PSI4
       * The PSI4 change mark field is changed each time information has been updated
       * A new value indicates that the mobile
       * station shall re-read the information from the PSI4
       */
      TRACE_ERROR( "PSI4: inconsistent parameters" );
      psc_db->number_of_valid_int_meas_channels = 0;
      return PSI4_REREAD;
    }

    /* psi4_change_markhas not changed, i.e.
     * PSI4 message has not changed, we already have a consistent set, so we
     * do not have to read the PSI4 message
     */
    if((psc_db->psi4_params.psi4_change_mark EQ psi4->psi4_cm) &&
          (psc_db->state_of_PSI[PSI4].state EQ RECEIPT_OK))
    {
      /* Message has not changed, we already have a consistent set, so we
       * do not have to read it
       */
      TRACE_EVENT( "PSI4: message has not changed" );
      return PSI4_OK;
    }
  }

  psc_db->psi4_params.psi4_index = psi4->psi4_ind;

  if( psc_db->psi4_params.instances[psi4->psi4_ind + 1] EQ FALSE )
  {
    /*
     * We assume that we receive PSI4 instances in the right order,
     * i.e. instance1, instance2, instance3, ..., instance8
     * This is ETSI-Requirement 05.02
     */  
    m = psc_db->number_of_valid_int_meas_channels;  
    if(m < MAX_CHAN_IMEAS)
    {
      /*
       * copy first channel list in the current instance
       */
      psc_db->int_meas_chan_list[m].v_arfcn = FALSE;
      psc_db->int_meas_chan_list[m].v_ma_num_maio = FALSE;

      if(psi4->chan_list_imeas.chan_group.v_arfcn)
      {
        psc_db->int_meas_chan_list[m].v_arfcn = TRUE;        /* valid-flag   for arfcn*/
        psc_db->int_meas_chan_list[m].arfcn = psi4->chan_list_imeas.chan_group.arfcn; /* ARFCN*/
      }
      else
      {
        psc_db->int_meas_chan_list[m].v_ma_num_maio = TRUE;
        psc_db->int_meas_chan_list[m].ma_num = psi4->chan_list_imeas.chan_group.ma_num_maio.ma_num;
        psc_db->int_meas_chan_list[m].maio = psi4->chan_list_imeas.chan_group.ma_num_maio.maio;
      }

      psc_db->int_meas_chan_list[m].ts_alloc = psi4->chan_list_imeas.chan_group.ts_alloc;

      m++;
      if(psi4->chan_list_imeas.v_chan_list2)
      {
        for(i=0; i < (psi4->chan_list_imeas.c_chan_list2) AND (m < MAX_CHAN_IMEAS); i++ )
        {
          /*
           * copy remaining channel list in the current instance
           */
          psc_db->int_meas_chan_list[m].v_arfcn = FALSE;
          psc_db->int_meas_chan_list[m].v_ma_num_maio = FALSE;

          if(psi4->chan_list_imeas.chan_list2[i].chan_group.v_arfcn)
          {
            psc_db->int_meas_chan_list[m].v_arfcn = TRUE;        /* valid-flag   for arfcn*/
            psc_db->int_meas_chan_list[m].arfcn = psi4->chan_list_imeas.chan_list2[i].chan_group.arfcn; /* ARFCN*/
          }
          else
          {
            psc_db->int_meas_chan_list[m].v_ma_num_maio = TRUE;
            psc_db->int_meas_chan_list[m].ma_num = psi4->chan_list_imeas.chan_list2[i].chan_group.ma_num_maio.ma_num;
            psc_db->int_meas_chan_list[m].maio = psi4->chan_list_imeas.chan_list2[i].chan_group.ma_num_maio.maio;
          }
          psc_db->int_meas_chan_list[m].ts_alloc = psi4->chan_list_imeas.chan_list2[i].chan_group.ts_alloc;
          m++;
        }
      }
    }
    else
    {
      TRACE_ERROR( "PSI4: list of INT meas. channels full" );
    }
    psc_db->number_of_valid_int_meas_channels = m;

    /*
     * check whether PSI4 has been received completely or not
     */
    psc_db->psi4_params.instances[psi4->psi4_ind + 1] = TRUE;
    is_psi4_ok = TRUE;
    for(n = 0; n <= psc_db->psi4_params.instances[0]; n++ )
    {
      if(!(psc_db->psi4_params.instances[n + 1]))
      {
        is_psi4_ok = FALSE;/* consistent set of PSI4 not complete */
        break;
      }
    }

    TRACE_EVENT_P2( "PSI4: received with count = %d, index = %d",
                    psi4->psi4_cnt, psi4->psi4_ind );

    if(is_psi4_ok)
    {
      psc_db->state_of_PSI[PSI4].state = RECEIPT_OK;
      return PSI4_INT_LIST_VALID;
    }
  }
  else
  {
    TRACE_EVENT_P2( "PSI4: already received with count = %d, index = %d",
                    psi4->psi4_cnt, psi4->psi4_ind );
  }


 
  return PSI4_OK;

} /* psi_process_psi4() */

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : psi_process_psi3ter
+------------------------------------------------------------------------------
| Description : The function psi_process_psi3ter() processes the IEs received 
|               in PSI3 ter message
|
| Parameters  : T_PSI_3_TER *psi3ter
|
+------------------------------------------------------------------------------
*/
GLOBAL T_PSI3TER_RET psi_process_psi3ter (T_PSI_3_TER *psi3ter)
{
  BOOL is_psi3ter_ok;
#ifdef TI_PS_FF_RTD
  UBYTE n,rtd_index;
#else 
  UBYTE n;
#endif /* #ifdef TI_PS_FF_RTD */


  TRACE_FUNCTION( "psi_process_psi3ter" );

  if(psc_db->psi3ter_params.psi3ter_change_mark EQ NOT_SET)
  {
    if(  ( (psc_db->psi3_params.psi3_change_mark NEQ NOT_SET) AND
      (psi3ter->psi3_cm NEQ psc_db->psi3_params.psi3_change_mark) ) OR 
         ( (psc_db->psi3bis_params.psi3bis_change_mark NEQ NOT_SET) AND
      (psi3ter->psi3_cm NEQ psc_db->psi3bis_params.psi3bis_change_mark) )  )
    {
     /*
      * Error situation read new PSI3ter
      * The PSI3 change mark field is changed each time information has been updated
      * in any of the PSI3 to PSI3 ter messages. A new value indicates that the mobile
      * station shall re-read the information from the PSI3 to PSI3 ter messages.
      */
      TRACE_ERROR( "PSI3ter: inconsistent change mark, read PSI3 to PSI3ter");
      return PSI3TER_REREAD_PSI3_3BIS_3TER;
    }

    /*
     * Copy change mark etc.
     */
    psc_db->psi3ter_params.psi3ter_change_mark = psi3ter->psi3_cm;
    psc_db->psi3ter_params.psi3ter_count       = psi3ter->psi3ter_cnt;
    psc_db->psi3ter_params.instances[0]        = psi3ter->psi3ter_cnt; /* number of instances */
  }
  else
    /*
     * There is a valid change mark present in the database
     * Check consistency of COUNT and INDEX
     */
  {
    if(  (psc_db->psi3ter_params.psi3ter_change_mark NEQ psi3ter->psi3_cm) OR
      ( (psc_db->psi3_params.psi3_change_mark NEQ NOT_SET) AND
      (psc_db->psi3ter_params.psi3ter_change_mark NEQ
      psc_db->psi3_params.psi3_change_mark) ) OR
      ( (psc_db->psi3bis_params.psi3bis_change_mark NEQ NOT_SET) AND
      (psc_db->psi3ter_params.psi3ter_change_mark NEQ
      psc_db->psi3bis_params.psi3bis_change_mark) )  )
    {
      /*
       * Error situation read new PSI3ter
       * The PSI3 change mark field is changed each time information has been updated
       * in any of the PSI3 to PSI3 ter messages. A new value indicates that the mobile
       * station shall re-read the information from the PSI3 to PSI3 ter messages.
       */
      TRACE_ERROR( "PSI3ter: inconsistent change mark, read PSI3 to PSI3ter");
      return PSI3TER_REREAD_PSI3_3BIS_3TER;
    }
    if( (psc_db->psi3ter_params.psi3ter_count NEQ psi3ter->psi3ter_cnt) OR
        (psc_db->psi3ter_params.psi3ter_count < psi3ter->psi3ter_ind) )
    {
     TRACE_ERROR( "PSI3ter: inconsistent parameters, read PSI3ter");
     return PSI3TER_REREAD_PSI3TER;
    }
    
    if((psc_db->psi3ter_params.psi3ter_change_mark EQ psi3ter->psi3_cm) AND
      (psc_db->state_of_PSI[PSI3ter].state EQ RECEIPT_OK))
    {
      /* Message has not changed, we already have a consistent set, so we
       * do not have to read it
       */
      TRACE_EVENT("PSI3ter: message has not changed");
      return PSI3TER_OK;
    }
  } /* psi3ter_change_mark valid */

  psc_db->psi3ter_params.psi3ter_index = psi3ter->psi3ter_ind;
    
  if( psc_db->psi3ter_params.instances[psi3ter->psi3ter_ind + 1] EQ FALSE )
  {
    /*
     * Copy GPRS report priority parameters
     */
    /* GPRS Report priority can be received in only one instance since start 
    index is not given for mapping to BA as in RTD and BSIC mapping in SI */
    if( (psi3ter->rtd_rep_prio_trnc_grp.v_gprs_rep_prio_cell_desc EQ TRUE) AND
      (psi3ter->rtd_rep_prio_trnc_grp.gprs_rep_prio_cell_desc.number_cells NEQ 0) )
    {
      if(psi3ter->rtd_rep_prio_trnc_grp.gprs_rep_prio_cell_desc.number_cells > MAX_NR_OF_GSM_NC)
      {
        grr_data->psi.enh_param.gprs_rept_prio_desc.num_cells =
          MAX_NR_OF_GSM_NC;
      }
      else
      {
        grr_data->psi.enh_param.gprs_rept_prio_desc.num_cells = 
          psi3ter->rtd_rep_prio_trnc_grp.gprs_rep_prio_cell_desc.number_cells;
        
        /* Set default report prio for the remaining cells upto max of 96 cells */
        for(n = psi3ter->rtd_rep_prio_trnc_grp.gprs_rep_prio_cell_desc.number_cells;
            n < MAX_NR_OF_GSM_NC; n++)
        {
          grr_data->psi.enh_param.gprs_rept_prio_desc.rept_prio[n] = 
            NORMAL_PRIO;
        }
      }
      
      for (n = 0; n < grr_data->psi.enh_param.gprs_rept_prio_desc.num_cells; n++)
      {
        grr_data->psi.enh_param.gprs_rept_prio_desc.rept_prio[n] = 
          psi3ter->rtd_rep_prio_trnc_grp.gprs_rep_prio_cell_desc.rep_prio[n];
      }      
    }

#ifdef TI_PS_FF_RTD
    /* Store the received RTD values into the temporary location */
    if(psi3ter->rtd_rep_prio_trnc_grp.v_real_time_diff EQ TRUE)
    {
      if(psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.v_rtd_struct_6bit EQ TRUE)
      {
        if(psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.rtd_struct_6bit.v_cell_index_start_rtd EQ TRUE)
          rtd_index = psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.rtd_struct_6bit.cell_index_start_rtd;
        else
          rtd_index = RTD_DEFAULT_INDEX;
        if( rtd_index < MAX_NR_OF_NCELL )
        {
          grr_data->psi.rtd[rtd_index] = psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.rtd_struct_6bit.rtd6_struct.rtd_6bit;
          for(n=0;n<psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.rtd_struct_6bit.c_rtd6_struct_opt_array AND rtd_index < MAX_NR_OF_NCELL - 1;n++)
          {
            rtd_index++;
            grr_data->psi.rtd[rtd_index] = psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.rtd_struct_6bit.rtd6_struct_opt_array[n].rtd_6bit;
          } /*for*/
        } /*if*/
      } /*if*/
      if(psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.v_rtd_struct_12bit EQ TRUE)
      {
        if(psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.rtd_struct_12bit.v_cell_index_start_rtd EQ TRUE)
          rtd_index = psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.rtd_struct_12bit.cell_index_start_rtd;
        else
          rtd_index = RTD_DEFAULT_INDEX;
        if( rtd_index < MAX_NR_OF_NCELL )
        {
          grr_data->psi.rtd[rtd_index] = RTD_12BIT;
          grr_data->psi.rtd[rtd_index] |= psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.rtd_struct_12bit.rtd12_struct.rtd_12bit;
          for(n=0;n<psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.rtd_struct_12bit.c_rtd12_struct_opt_array AND rtd_index < MAX_NR_OF_NCELL - 1;n++)
          { 
            rtd_index++;
            grr_data->psi.rtd[rtd_index] = RTD_12BIT;
            grr_data->psi.rtd[rtd_index] |= psi3ter->rtd_rep_prio_trnc_grp.real_time_diff.rtd_struct_12bit.rtd12_struct_opt_array[n].rtd_12bit;
          } /*for*/
        }/*if*/
      } /*if*/
    } /*if*/
#endif /* #ifdef TI_PS_FF_RTD */

    TRACE_EVENT_P2( "PSI3ter: received with count = %d, index = %d",
                    psi3ter->psi3ter_cnt, psi3ter->psi3ter_ind );

    /*
     * check whether PSI3TER has been received completely or not
     */
    psc_db->psi3ter_params.instances[psi3ter->psi3ter_ind + 1] = TRUE;
    psc_db->psi3ter_params.prev_psi3ter_index = 
    psc_db->psi3ter_params.psi3ter_index;
    
    is_psi3ter_ok = TRUE;
    for(n = 0; n <= psc_db->psi3ter_params.instances[0]; n++ )
    {
      if(!(psc_db->psi3ter_params.instances[n + 1]))
      {
        is_psi3ter_ok = FALSE;/* consistent set of PSI3TER not complete */
        break;
      }
    }

    if(is_psi3ter_ok)
    {
      psc_db->state_of_PSI[PSI3ter].state = RECEIPT_OK;
      
      /* Copy the GPRS priority information to permanent storage */
      if(grr_data->psi.enh_param.gprs_rept_prio_desc.num_cells NEQ 0)
      {
        for(n = 0; n <= grr_data->psi.enh_param.gprs_rept_prio_desc.num_cells; n++)
        {
          psc_db->enh_cw.gprs_rept_prio_desc.rept_prio[n] = 
            grr_data->psi.enh_param.gprs_rept_prio_desc.rept_prio[n];
        }
        /* Initialize to 0 before receiving the next consistent set of PSI3 ter */
        grr_data->psi.enh_param.gprs_rept_prio_desc.num_cells = 0;
      }
      else
      {
        /* Set report priority to NORMAL if not received */
        for(n = 0; n < MAX_NR_OF_GSM_NC; n++)
        {
          psc_db->enh_cw.gprs_rept_prio_desc.rept_prio[n] = NORMAL_PRIO;
        }
      }
#ifdef TI_PS_FF_RTD
      /* store the RTD values received in all instances of PSI3ter in permanent location */
      memcpy(&psc_db->rtd,&grr_data->psi.rtd,MAX_NR_OF_NCELL*sizeof(T_RTD_VALUE));
        /* reset the temporary storage to RTD value not available */
      for( n = 0; n < MAX_NR_OF_NCELL; n++ )
        grr_data->psi.rtd[n] = RTD_NOT_AVAILABLE;
#endif /* #ifdef TI_PS_FF_RTD */
        return PSI3TER_OK;

    }
  }
  else
  {
    TRACE_EVENT_P2( "PSI3ter: already received with count = %d, index = %d",
                    psi3ter->psi3ter_cnt, psi3ter->psi3ter_ind );
  }
  
  return PSI3TER_OK; 
} /* psi_process_psi3ter() */
#endif


/*
+------------------------------------------------------------------------------
| Function    : psi_process_psi3bis
+------------------------------------------------------------------------------
| Description : The function psi_process_psi3bis() ....
|
| Parameters  : T_PSI_3_BIS *psi3bis
|
+------------------------------------------------------------------------------
*/
GLOBAL T_PSI3BIS_RET psi_process_psi3bis (T_PSI_3_BIS *psi3bis)
{
  BOOL is_psi3bis_ok;
  UBYTE n;

  TRACE_FUNCTION( "psi_process_psi3bis" );

  /*
   * Each neighbour cell listed in PSI3 and in one or more instances
   * of PSI3bis is assigned an ascending index used for measurement reports.
   * The first neighbour cell in PSI3 has the lowest index  (= 0),
   * and the last neighbour cell in the highest indexed PSI3bis message has
   * the highest index. The total number of neighbour cells in the BA-GPRS
   * shall not exceed 32.
   * If a mobile station receives more than 32 cells in the BA-GPRS,
   * only the 32 cells with the lowest indexes shall be considered.
   */

  if(psc_db->psi3bis_params.psi3bis_change_mark EQ NOT_SET)
  {
    /*
     * Copy change mark etc.
     */
    psc_db->psi3bis_params.psi3bis_change_mark = psi3bis->psi3_cm;
    psc_db->psi3bis_params.psi3bis_count       = psi3bis->psi3bis_cnt;
    psc_db->psi3bis_params.instances[0]        = psi3bis->psi3bis_cnt; /* number of instances */
  }
  else
  {
    /*
     * There is a valid change mark present in the database
     * Check consistency of COUNT and INDEX
     */
    if(
       (psc_db->psi3bis_params.psi3bis_count NEQ psi3bis->psi3bis_cnt)
       OR
       (psc_db->psi3bis_params.psi3bis_count < psi3bis->psi3bis_ind)
       OR
       (psc_db->psi3bis_params.psi3bis_change_mark NEQ psi3bis->psi3_cm)
      )
    {
      /*
       * Error situation read a new PSI3bis
       * The PSI3 change mark field is changed each time information has been updated
       * in any of the PSI3 or PSI3 bis messages. A new value indicates that the mobile
       * station shall re-read the information from the PSI3 and all PSI3 bis messages.
       */
#if defined (REL99) AND defined (TI_PS_FF_EMR)
      TRACE_ERROR( "PSI3bis: inconsistent parameters, read PSI3 PSI3bis and PSI3ter");

      return PSI3BIS_REREAD_PSI3_3BIS_3TER;
#else
      TRACE_ERROR( "PSI3bis: inconsistent parameters, read PSI3 and PSI3bis");

      return PSI3BIS_REREAD_PSI3_3BIS;
#endif
    }

    if((psc_db->psi3bis_params.psi3bis_change_mark EQ psi3bis->psi3_cm) &&
          (psc_db->state_of_PSI[PSI3bis].state EQ RECEIPT_OK))
    {
      /* Message has not changed, we already have a consistent set, so we
       * do not have to read it
       */
      TRACE_EVENT("PSI3bis: message has not changed");
      return PSI3BIS_OK;
    }
    else if(psc_db->psi3bis_params.psi3bis_change_mark NEQ psi3bis->psi3_cm)
    {
      TRACE_ERROR("PSI3bis: message has changed");
      return PSI3BIS_REREAD_PSI3BIS;
    }

  } /* psi3bis_change_mark valid */

  psc_db->psi3bis_params.psi3bis_index = psi3bis->psi3bis_ind;
  
  if( psc_db->psi3bis_params.instances[psi3bis->psi3bis_ind + 1] EQ FALSE )
  {
    UBYTE number;

    /*
     * Copy NCELL parameters
     */
    number = 
      psi_store_ncell_param( &psi3bis->ncell_par_trnc_grp.ncell_par[0],
                             psi3bis->ncell_par_trnc_grp.c_ncell_par,
                             psi3bis->ncell_par_trnc_grp.v_ncell_par,
                             INFO_TYPE_PSI3BIS, 
                             psi3bis->psi3bis_ind );

    /*
     * Copy NCELL parameters 2
     */
    psi_store_ncell_param2( psi3bis, number );

    TRACE_EVENT_P2( "PSI3bis: received with count = %d, index = %d",
                    psi3bis->psi3bis_cnt, psi3bis->psi3bis_ind );

    /*
     * check whether PSI3BIS has been received completely or not
     */
    psc_db->psi3bis_params.instances[psi3bis->psi3bis_ind + 1] = TRUE;
    is_psi3bis_ok = TRUE;
    for(n = 0; n <= psc_db->psi3bis_params.instances[0]; n++ )
    {
      if(!(psc_db->psi3bis_params.instances[n + 1]))
      {
        is_psi3bis_ok = FALSE;/* consistent set of PSI3BIS not complete */
        break;
      }
    }

    if(is_psi3bis_ok)
    {
      psc_db->state_of_PSI[PSI3bis].state = RECEIPT_OK;
    
      if( psc_db->state_of_PSI[PSI3].state EQ RECEIPT_OK )
      {
        psi_restore_ncell_param( );

        return PSI3BIS_NCELL_VALID;
      }
      else
      {
        return PSI3BIS_OK;
      }
    }
  }
  else
  {
    TRACE_EVENT_P2( "PSI3bis: already received with count = %d, index = %d",
                    psi3bis->psi3bis_cnt, psi3bis->psi3bis_ind );
  }


  return PSI3BIS_OK;
} /* psi_process_psi3bis() */

/*
+------------------------------------------------------------------------------
| Function    : psi_process_psi3
+------------------------------------------------------------------------------
| Description : The function psi_process_psi3() ....
|
| Parameters  : T_PSI_3 *psi3
|
+------------------------------------------------------------------------------
*/
GLOBAL T_PSI3_RET psi_process_psi3 (T_PSI_3 *psi3 )
{
  T_PSI3_RET return_val = PSI3_OK;

  TRACE_FUNCTION( "psi_process_psi3" );

  /*
   * Check whether the cell has been barred or not. If YES, inform CTRL
   */
  if(psi3->scell_par.cell_ba)
  {
    /*Barred*/
    TRACE_EVENT( "PSI3: cell barred" );

    GRR_EM_SET_CELL_BARRED;

    return PSI3_CELL_BARRED;
  }
  
  psc_db->state_of_PSI[PSI3].state = RECEIPT_OK;

  if(psc_db->psi3_params.psi3_change_mark  NEQ NOT_SET)
  {
    /* compare change mark*/
    if(psc_db->psi3_params.psi3_change_mark EQ psi3->psi3_cm)
    {
      /*
       * Information of the PSI3 PSI3ter messages have  NOT been updated: return!!
       */
      TRACE_EVENT( "PSI3: messages PSI3 and PSI3bis have not changed" );
      return PSI3_OK;
    }
    /*
     * Information of the PSI3 message has been updated, so we have to re-read the parameters
     *
     * A new value indicates that the mobile station shall re-read the information from
     * the PSI3 - PSI3ter messages.
     */
    /*
     * Set the change mark parameter of psi3bis to NOT_SET. This causes that the PSI3BIS and
     * PSI3 ter instances to be read again
     */
    psc_db->psi3bis_params.psi3bis_change_mark = NOT_SET;
    psc_db->state_of_PSI[PSI3bis].state = NEEDED;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    psc_db->psi3ter_params.psi3ter_change_mark = NOT_SET;

    if(psc_db->v_add_psi AND psc_db->add_psi.psi3ter_broadcast)
    {
      psc_db->state_of_PSI[PSI3ter].state = NEEDED;   
    }
    else
    {
      psc_db->state_of_PSI[PSI3ter].state = NOT_SEND; 
    }
#endif

    /*
     * Set the consistent set parameters to FALSE
     */
    psi_reset_psi3bis();
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    psi_reset_psi3ter();
#endif

    /* This field is coded as the binary representation of the PSI3 bis index
     * (in the PSI3 bis message) for the last (highest indexed) individual PSI3
     * bis message.Range: 0-15.
     */
    psc_db->psi3_params.psi3_bis_count = psi3->psi3bis_cnt;
    /*
     * Ncell parameter are invalid. We have to inform MEAS-Service
     */
    return_val = PSI3_NCELL_INVALID;
  }
  else
  {
    /*  This is the first time that we have received a PSI3 message*/
    psc_db->psi3_params.psi3_change_mark = psi3->psi3_cm;
    psc_db->psi3_params.psi3_bis_count   = psi3->psi3bis_cnt;
  }

  /* copy Serving Cell parameters        */
  psc_db->scell_par.cell_ba                          = psi3->scell_par.cell_ba;
  psc_db->scell_par.exc_acc                          = psi3->scell_par.exc_acc;
  psc_db->scell_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min 
                                                     = psi3->scell_par.gprs_rxlev_access_min;
  psc_db->scell_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch
                                                     = psi3->scell_par.txpwr_max_cch;
  grr_data->meas.pwr_offset                          = 0;

  psc_db->scell_par.cr_par_1.v_hcs_par               = psi3->scell_par.v_hcs_par;
  psc_db->scell_par.cr_par_1.hcs_par                 = psi3->scell_par.hcs_par;
  psc_db->scell_par.multi_band_rep                   = psi3->scell_par.multi_band_rep;

  /*copy General Cell Selection parameter */
  memcpy(&(psc_db->gen_cell_par), &(psi3->gen_cell_par),sizeof(T_gen_cell_par));

  /* process default values for General Cell Selection parameter */
  if( psc_db->gen_cell_par.v_t_resel EQ FALSE )
  {
    psc_db->gen_cell_par.v_t_resel = TRUE;
    psc_db->gen_cell_par.t_resel   = GRR_T_RESEL_DEFAULT;
  }

  if( psc_db->gen_cell_par.v_ra_re_hyst EQ FALSE )
  {
    psc_db->gen_cell_par.v_ra_re_hyst = TRUE;
    psc_db->gen_cell_par.ra_re_hyst   = psc_db->gen_cell_par.gprs_c_hyst;
  }

  /* copy Neighbor cell parameters*/
  psi_store_ncell_param( &psi3->ncell_par[0], psi3->c_ncell_par,
                         psi3->v_ncell_par, INFO_TYPE_PSI3, 0 );

  if( psc_db->state_of_PSI[PSI3].state    EQ RECEIPT_OK AND 
      psc_db->state_of_PSI[PSI3bis].state EQ RECEIPT_OK     )
  {
    psi_restore_ncell_param( );
    
    return_val = PSI3_NCELL_VALID;
  }
  
  TRACE_EVENT_P1( "PSI3: received with return value %d", return_val );
  
  grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param = TRUE;

  return return_val;
} /* psi_process_psi3() */

/*
+------------------------------------------------------------------------------
| Function    : psi_copy_rfl
+------------------------------------------------------------------------------
| Description : The function psi_copy_rfl() .... copy Reference Frequency List (RFL)
|
| Parameters  : T_PSI_2 *psi2: pointer to PSI2 message
|
+------------------------------------------------------------------------------
*/

LOCAL void  psi_copy_rfl(T_PSI_2 *psi2)
{

  UBYTE m, n;
  TRACE_FUNCTION( "psi_copy_rfl" );

  for(m = 0; m < psi2->psi_2_trnc_grp.c_rfl; m++)
  {
    for(n = 0; n < MAX_RFL; n++)
    {
      if( (psi2->psi_2_trnc_grp.rfl[m].rfl_num EQ psc_db->rfl[n].num) OR (psc_db->rfl[n].num EQ NOT_SET) )
      {
        psc_db->rfl[n].num       = psi2->psi_2_trnc_grp.rfl[m].rfl_num;
        /*
         * This function copies the frequencies into list. This function takes the arranging
         * of ARFCN into account cf. 12.10a GPRS Mobile Allocation in 04.60
         */
        grr_create_channel_list (&psi2->psi_2_trnc_grp.rfl[m], psc_db->rfl[n].list);
        /*
         * Exit from the for loop
         */
        n = MAX_RFL;
      }
    }
  }

}/* psi_copy_rfl*/

/*
+------------------------------------------------------------------------------
| Function    : psi_copy_ma_from_psi2
+------------------------------------------------------------------------------
| Description : The function psi_copy_ma_from_psi2() .... copy GPRS Mobile Allocation (MA)
|
| Parameters  : T_PSI_2 *psi2: pointer to PSI2 message
|
+------------------------------------------------------------------------------
*/
LOCAL void  psi_copy_ma_from_psi2(T_PSI_2 *psi2)
{
  UBYTE m, n, copiedElements = 0;
  TRACE_FUNCTION( "psi_copy_ma_from_psi2" );

  for(m = 0; m < psi2->psi_2_trnc_grp.c_gprs_ms_alloc; m++)
  {
    if((psi2->psi_2_trnc_grp.gprs_ms_alloc[m].ma_num != MA_NUMBER_4_PSI13_OR_CELL_ALLOC ) && /* ignore MA_NUMBER with 14 or 15: 0..13 in PSI2*/
       (psi2->psi_2_trnc_grp.gprs_ms_alloc[m].ma_num != MA_NUMBER_4_ASSIGNMENT ))
    {
      for(n = 0; n < MAX_GPRS_MS_ALLOC; n++)
      {
        if( (psi2->psi_2_trnc_grp.gprs_ms_alloc[m].ma_num EQ psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num) ||
            (psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num EQ NOT_SET) )
        {
          psc_db->gprs_ms_alloc_in_psi2_psi13[n].ma_num = psi2->psi_2_trnc_grp.gprs_ms_alloc[m].ma_num;
          memcpy(&(psc_db->gprs_ms_alloc_in_psi2_psi13[n].gprs_ms_alloc_ie),
                      &(psi2->psi_2_trnc_grp.gprs_ms_alloc[m].gprs_ms_alloc_ie),
                      sizeof(T_gprs_ms_alloc_ie));
          n = MAX_GPRS_MS_ALLOC; /* to break the for loop*/
          copiedElements++; /* for debug*/
        }
      }
    }
  }

  if(copiedElements < psi2->psi_2_trnc_grp.c_gprs_ms_alloc)
  {
    TRACE_ERROR("copiedElements < psi2->c_gprs_ms_alloc in" );
  }
}/* psi_copy_ma_from_psi2*/

/*
+------------------------------------------------------------------------------
| Function    : psi_copy_ca
+------------------------------------------------------------------------------
| Description : The function psi_copy_ca() .... copy Cell Allocation (CA)
|
| Parameters  : T_PSI_2 *psi2: pointer to PSI2 message
|
+------------------------------------------------------------------------------
*/
LOCAL void  psi_copy_ca(T_PSI_2 *psi2)
{
  UBYTE m,n;
  TRACE_FUNCTION( "psi_copy_ca" );
  /*TRACE_EVENT("psi_copy_ca");
  TRACE_EVENT_P1("c_ca: %d", psi2->c_cell_alloc);
  */
  for(m = 0; m < psi2->psi_2_trnc_grp.c_cell_alloc; m++)
  {
    for(n = 0; n < MAX_CELL_ALLOC; n++)
    {
      if( (psi2->psi_2_trnc_grp.cell_alloc[m].rfl_num EQ psc_db->cell_alloc[n].rfl_num)
          || (psc_db->cell_alloc[n].rfl_num EQ NOT_SET) )
      {
        psc_db->cell_alloc[n].rfl_num = psi2->psi_2_trnc_grp.cell_alloc[m].rfl_num;
        n = MAX_CELL_ALLOC;
      }
    }
    psc_db->v_cell_alloc = TRUE;
  }
}/* psi_copy_ca*/

/*
+------------------------------------------------------------------------------
| Function    : psi_copy_pccch
+------------------------------------------------------------------------------
| Description : The function psi_copy_pccch() .... copy PCCCH Description
|
| Parameters  : T_PSI_2 *psi2: pointer to PSI2 message
|
+------------------------------------------------------------------------------
*/
LOCAL void  psi_copy_pccch(T_PSI_2 *psi2)
{
  UBYTE m, n, i, ii, j;

  TRACE_FUNCTION( "psi_copy_pccch" );

  m = 0;
  for(n = 0; n < psi2->psi_2_trnc_grp.c_pccch_des; n++)
  {
      if(psi2->psi_2_trnc_grp.pccch_des[n].v_nh_pccch_c AND (m < MAX_PCCCH_DES))
      {
        /* 
         * Non hopping parameters
         */
        for(j = 0; j < psi2->psi_2_trnc_grp.pccch_des[n].c_nh_pccch_c; j++)
        {
          ii = 128;
          for(i = 0; i < 8; i++)
          {
            if( ((psi2->psi_2_trnc_grp.pccch_des[n].nh_pccch_c[j].ts_alloc & ii ) EQ ii) ) /* to get the timeslot number */
            {
              psc_db->paging_group.pccch[m].is_static = TRUE;
              psc_db->paging_group.pccch[m].tsc    = psi2->psi_2_trnc_grp.pccch_des[n].tsc;
              psc_db->paging_group.pccch[m].arfcn = psi2->psi_2_trnc_grp.pccch_des[n].nh_pccch_c[j].arfcn;
              psc_db->paging_group.pccch[m].tn = i;/* timeslot number*/
              m++;
              if( m >= MAX_PCCCH_DES )
                break;
            }
            ii = ii/2;
          }
        }
      }

      if(psi2->psi_2_trnc_grp.pccch_des[n].v_ma_h_s1 AND (m < MAX_PCCCH_DES))
      {
        /* 
         * Hopping parameters
         */        
        for(j = 0; j < psi2->psi_2_trnc_grp.pccch_des[n].ma_h_s1.c_h_pccch_c; j++)
        {
          
          ii = 128;
          for(i = 0; i < 8; i++)
          {
            if( ((psi2->psi_2_trnc_grp.pccch_des[n].ma_h_s1.h_pccch_c[j].ts_alloc & ii ) EQ ii) ) /* to get the timeslot number */
            {
              psc_db->paging_group.pccch[m].is_static = FALSE;              
              psc_db->paging_group.pccch[m].ma_num    = psi2->psi_2_trnc_grp.pccch_des[n].ma_h_s1.ma_num;
              psc_db->paging_group.pccch[m].tsc       = psi2->psi_2_trnc_grp.pccch_des[n].tsc;
              psc_db->paging_group.pccch[m].maio      = psi2->psi_2_trnc_grp.pccch_des[n].ma_h_s1.h_pccch_c[j].maio;
              psc_db->paging_group.pccch[m].tn        = i;/* timeslot number*/
              m++;
              if( m >= MAX_PCCCH_DES )
                break;
            }
            ii = ii/2;
          }
        }
      }
      psc_db->paging_group.kc = m; /*number of timeslots carrying PCCCH*/
  }
}/* psi_copy_pccch*/

/*
+------------------------------------------------------------------------------
| Function    : psi_copy_cell_id
+------------------------------------------------------------------------------
| Description : The function psi_copy_cell_id() ....
|
| Parameters  : T_PSI_2 *psi2: pointer to PSI2 message strcuture
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_copy_cell_id(T_PSI_2 *psi2)
{
  TRACE_FUNCTION( "psi_copy_cell_id" );

  psc_db->cell_info_for_gmm.cell_info.cell_env.rai.plmn.v_plmn = TRUE;

  if( psi2->psi_2_trnc_grp.cell_id.loc_area_ident.c_mnc EQ 2 )
  {
    /* Internally G23 uses always 3-digit-MNC */
    psi2->psi_2_trnc_grp.cell_id.loc_area_ident.c_mnc  = SIZE_MNC;
    psi2->psi_2_trnc_grp.cell_id.loc_area_ident.mnc[2] = 0xf;
  }

  memcpy( psc_db->cell_info_for_gmm.cell_info.cell_env.rai.plmn.mcc,
          psi2->psi_2_trnc_grp.cell_id.loc_area_ident.mcc, SIZE_MCC );

  memcpy( psc_db->cell_info_for_gmm.cell_info.cell_env.rai.plmn.mnc,
          psi2->psi_2_trnc_grp.cell_id.loc_area_ident.mnc, SIZE_MNC );

  psc_db->cell_info_for_gmm.cell_info.cell_env.rai.lac = psi2->psi_2_trnc_grp.cell_id.loc_area_ident.lac;
  psc_db->cell_info_for_gmm.cell_info.cell_env.rai.rac = psi2->psi_2_trnc_grp.cell_id.rac;
  psc_db->cell_info_for_gmm.cell_info.cell_env.cid     = psi2->psi_2_trnc_grp.cell_id.cell_id_ie;

}/* psi_copy_cell_id*/

/*
+------------------------------------------------------------------------------
| Function    : psi_copy_non_gprs_opt
+------------------------------------------------------------------------------
| Description : The function psi_copy_non_gprs_opt() ....
|
| Parameters  : T_PSI_2 *psi2: pointer to PSI2 message strcuture
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_copy_non_gprs_opt(T_PSI_2 *psi2)
{
  TRACE_FUNCTION( "psi_copy_non_gprs_opt" );

  psc_db->v_non_gprs_opt = TRUE;
  /* copy parameters*/
  memcpy(&(psc_db->non_gprs_opt), &(psi2->psi_2_trnc_grp.non_gprs_opt), sizeof(T_non_gprs_opt));


}/* psi_copy_non_gprs_opt*/
/*
+------------------------------------------------------------------------------
| Function    : psi_process_psi2
+------------------------------------------------------------------------------
| Description : The function psi_process_psi2() ....
|
| Parameters  : T_PSI_2 *psi2
|
+------------------------------------------------------------------------------
*/
GLOBAL T_PSI2_RET psi_process_psi2 (T_PSI_2 *psi2 )
{
  UBYTE n=0;

  TRACE_FUNCTION( "psi_process_psi2" );

  if(psc_db->psi2_params.psi2_change_mark EQ NOT_SET)
  {
    /* This is the first PSI2 message*/
    psc_db->psi2_params.psi2_change_mark = psi2->psi2_cm;
    psc_db->psi2_params.psi2_count   = psi2->psi2_cnt;
    psc_db->psi2_params.instances[0] = psi2->psi2_cnt;/* number of instances */
  }
  else
  {
    /* Check consistency of COUNT and INDEX*/
    if(
       (psc_db->psi2_params.psi2_count NEQ psi2->psi2_cnt)
       OR
       (psc_db->psi2_params.psi2_count < psi2->psi2_ind)
       OR
       (psc_db->psi2_params.psi2_change_mark NEQ psi2->psi2_cm)
      )
    {
      /* Error situation read a new PSI2*/
      TRACE_EVENT( "PSI2: inconsistent parameters" );
      return PSI2_REREAD;
    }

    /* There was a valid psi2_change_mark: compare change_mark values*/
    if((psc_db->psi2_params.psi2_change_mark EQ psi2->psi2_cm) AND
      (psc_db->state_of_PSI[PSI2].state EQ RECEIPT_OK))
    {
      /* PSI2 message has not changed, we already have a consistent set, so we
       * do not have to read the PSI2 message
       */
      TRACE_EVENT( "PSI2: message has not changed" );
      return PSI2_OK;
    }
  } /* psi2_change_mark set */


  if( psc_db->psi2_params.instances[psi2->psi2_ind + 1] EQ FALSE )
  {
    psc_db->psi2_params.instances[psi2->psi2_ind + 1] = TRUE;

    /*check consistency */
    psc_db->complete_acq.psi2_ok = TRUE;
    for(n = 0; n <= psc_db->psi2_params.instances[0]; n++ )
    {
      if(!(psc_db->psi2_params.instances[n + 1]))
      {
        psc_db->complete_acq.psi2_ok = FALSE;/* consistent set of PSI2 not complete */
        break;
      }
    }

    psc_db->state_of_PSI[PSI2].state = (psc_db->complete_acq.psi2_ok) ? RECEIPT_OK : NEEDED;

    if(psi2->psi_2_trnc_grp.v_cell_id)
    {
      /* copy cell identification*/
      psi_copy_cell_id(psi2);
    }

    if(psi2->psi_2_trnc_grp.v_non_gprs_opt)
    {
      /* copy non GPRS cell options*/
      psi_copy_non_gprs_opt(psi2);
    }

    /*
     * copy Reference Frequency List (RFL)
     */
    psi_copy_rfl(psi2);

    /*
     * copy Cell Allocation (CA)
     */
    psi_copy_ca(psi2);

    /*
     * copy GPRS Mobile Allocation (MA)
     */
    psi_copy_ma_from_psi2(psi2);

    /*
     * copy PCCCH Description
     */
    psi_copy_pccch(psi2);
#ifdef REL99
    if(psi2->psi_2_trnc_grp.v_release_99_str_psi_2 AND psi2->psi_2_trnc_grp.release_99_str_psi_2.v_add_psi)
    {
      psc_db->add_psi = psi2->psi_2_trnc_grp.release_99_str_psi_2.add_psi;
      psc_db->v_add_psi = TRUE;
      
      if(!psi2->psi_2_trnc_grp.release_99_str_psi_2.add_psi.Psi8_broadcast)
      {
        psc_db->state_of_PSI[PSI8].state = NOT_SEND;  
        psc_db->v_cbch_chan_desc = FALSE;
        
        /* as this is r-99 network, and PBCCH is present
        * CBCH info should be sent to RR. As psi8 is
        * not present, it will deactivated.
        */
        psc_db->send_cbch_info_ind = TRUE;
      }
      
      /* Process psi3ter broadcast indicator */
      if(!psi2->psi_2_trnc_grp.release_99_str_psi_2.add_psi.psi3ter_broadcast)
      {
        psc_db->state_of_PSI[PSI3ter].state = NOT_SEND;  
      }
    }

    if(psc_db->state_of_PSI[PSI2].state EQ RECEIPT_OK)
    {
      /* A consistent set of PSI2 has been received. */
      /* Additional PSI Messages information element is within truncation
      notation. If it wasn't received at all, then we should assume 0, 
      hence mark PSI3 ter and PSI 8 as "NOT_SEND"                   */
      if(!psc_db->v_add_psi)
      {
        psc_db->state_of_PSI[PSI8].state = NOT_SEND;          
        psc_db->v_cbch_chan_desc = FALSE;
        if(psc_db->network_rel)
        {
          
        /* as this is r-99 network, and PBCCH is present
        * CBCH info should be sent to RR. As psi8 is
        * not present, it will deactivated.
          */
          psc_db->send_cbch_info_ind = TRUE;
        }
        
        psc_db->state_of_PSI[PSI3ter].state = NOT_SEND;          
      }
      /* PSI2 has got updated. CBCH channel description
      * could depend of PSI2 in case hopping is allowed
      */
      if(psc_db->v_cbch_chan_desc AND !psc_db->cbch_chan_desc.freq_par.v_arfcn)
      {
        psc_db->send_cbch_info_ind = TRUE;                 
      }
    }
#endif
    TRACE_EVENT_P2( "PSI2: received with count = %d, index = %d",
                    psi2->psi2_cnt, psi2->psi2_ind );
  }
  else
  {
    TRACE_EVENT_P2( "PSI2: already received with count = %d, index = %d",
                    psi2->psi2_cnt, psi2->psi2_ind );
  }



  return PSI2_OK;
} /* psi_process_psi2() */

#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : psi_process_psi8
+------------------------------------------------------------------------------
| Description : This function stores the contents of PSI 8 for further 
|               processing. 
|               In R-99, PSI8 contains CBCH configuration. If PSI8 is not
|               broadcast or if none of the instances of PSI8 contain 
|               CBCH information, then SMSCB is not active in the cell. 
|
|               CBCH is handled in ALR entity and is configured through RR entity.
|               This message contains CBCH channel description. If cbch 
|               channel description indicates hopping, then to decode 
|               frequency parameters to extract the frequencies for mobile allocation,
|               prior receipt of PSI 2(all instances) and
|               optionally SI13 messages is essential.
|               Hence PSI 8 frequency parameters is not decoded here. It is
|               stored in psc data blase and is decoded before giving
|               RRGRR_CBCH_INFO_IND to RR entity. RRGRR sap messages are 
|               sent from service CTRL.
| Parameters  : T_PSI_4 *psi4
|
+------------------------------------------------------------------------------
*/
GLOBAL T_PSI8_RET psi_process_psi8 (T_PSI_8 *psi8 )
{
  UBYTE n;
  BOOL is_psi8_ok;
  TRACE_FUNCTION( "psi_process_psi8" );

  /*
   * Check consistency
   */
  if(psc_db->psi8_params.psi8_change_mark EQ NOT_SET)
  {
    /*
     * Copy change mark etc.
     */
    psc_db->psi8_params.psi8_change_mark = psi8->psi8_cm;
    psc_db->psi8_params.psi8_count   = psi8->psi8_cnt;
    psc_db->psi8_params.instances[0] = psi8->psi8_cnt;/* number of instances */
    psc_db->v_cbch_chan_desc = FALSE;
  }
  else
  {
    /*
     * There is a valid change mark present in the database
     * Check consistency of COUNT and INDEX
     */
    if(
       (psc_db->psi8_params.psi8_count NEQ psi8->psi8_cnt)
       OR
       (psc_db->psi8_params.psi8_count < psi8->psi8_ind)
       OR
       (psc_db->psi8_params.psi8_change_mark NEQ psi8->psi8_cm)
       )
    {
      /*
       * Error situation; read a new PSI4
       * The PSI4 change mark field is changed each time information has been updated
       * A new value indicates that the mobile
       * station shall re-read the information from the PSI4
       */
      TRACE_ERROR( "PSI8: inconsistent parameters" );
      psc_db->v_cbch_chan_desc = FALSE;
      return PSI8_REREAD;
    }

    /* psi8_change_markh as not changed, i.e.
     * PSI8 message has not changed, we already have a consistent set, so we
     * do not have to read the PSI4 message
     */
    if((psc_db->psi8_params.psi8_change_mark EQ psi8->psi8_cm) &&
          (psc_db->state_of_PSI[PSI8].state EQ RECEIPT_OK))
    {
      /* Message has not changed, we already have a consistent set, so we
       * do not have to read it
       */
      TRACE_EVENT( "PSI8: message has not changed" );
      return PSI8_OK;
    }
  }

  psc_db->psi8_params.psi8_index = psi8->psi8_ind;

  if( psc_db->psi8_params.instances[psi8->psi8_ind + 1] EQ FALSE )
  {
     /* Store CBCH channel description. To decode Frequency Parameters,
      * it is necessary to receive all instances of PSI2 message.
      * Hence defer decoding of hopping frequency list till 
      * end of acquisition.
      */
     if(psi8->v_cbch_chan_desc)
       {
          memcpy(&(psc_db->cbch_chan_desc),&(psi8->cbch_chan_desc),sizeof(T_cbch_chan_desc));
          psc_db->v_cbch_chan_desc = TRUE;
       }
    /*
     * check whether PSI8 has been received completely or not
     */
    psc_db->psi8_params.instances[psi8->psi8_ind + 1] = TRUE;
    is_psi8_ok = TRUE;
    for(n = 0; n <= psc_db->psi8_params.instances[0]; n++ )
    {
      if(!(psc_db->psi8_params.instances[n + 1]))
      {
        is_psi8_ok = FALSE;/* consistent set of PSI8 not complete */
        break;
      }
    }

    TRACE_EVENT_P2( "PSI8: received with count = %d, index = %d",
                    psi8->psi8_cnt, psi8->psi8_ind );

    if(is_psi8_ok)
    {
      if(psc_db->state_of_PSI[PSI8].state NEQ RECEIPT_OK)
        {
          /* A new copy of PSI8 has been received. 
           * RRGRR_CBCH_INFO_IND has to be sent after 
           * all instances of PSI2 have been received when 
           * acquisition is PARTIAL, and in case of COMPLETE
           *  acquisition after access to the cell is enabled.
           */
          psc_db->state_of_PSI[PSI8].state = RECEIPT_OK;
          psc_db->send_cbch_info_ind = TRUE;
          return PSI8_OK;
        }
    }
  }
  else
  {
    TRACE_EVENT_P2( "PSI8: already received with count = %d, index = %d",
                    psi8->psi8_cnt, psi8->psi8_ind );
  }

  return PSI8_OK;

} /* psi_process_psi8() */
#endif

/*
+------------------------------------------------------------------------------
| Function    : psi_check_change_field
+------------------------------------------------------------------------------
| Description : The function psi_check_change_field() ....
|
| Parameters  : UBYTE psi_change_field; return TRUE: acq needed, FALSE: not needed
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL psi_check_change_field (UBYTE psi_change_field)
{
  BOOL return_value = FALSE;
  TRACE_FUNCTION( "psi_check_change_field" );

  switch(psi_change_field)
  {
    case UPDATE_UNS_PSI: /*Update of unspecified PSI message(s);*/
      psc_db->state_of_PSI[PSI2].state = NEEDED;
      psc_db->state_of_PSI[PSI3].state = NEEDED;
      psc_db->state_of_PSI[PSI3bis].state = NEEDED;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
      psc_db->state_of_PSI[PSI3ter].state = NEEDED;
#endif
      psc_db->state_of_PSI[PSI4].state = NEEDED;
      psc_db->state_of_PSI[PSI5].state = NEEDED; /*??*/
#ifdef REL99
      /* It is necessary to set PSI 8 to needed here. In case 
       * PSI 8 is not broadcast in the cell, this value \
       * would be sent to NOT_SEND when PSI 2 is received
       */
      psc_db->state_of_PSI[PSI8].state = NEEDED;
#endif
      /*psc_db->state_of_PSI[13] = NEEDED;*/
      return_value = TRUE;
      break;
    case UPDATE_UNKNOWN: /* Unknown*/
      return_value = FALSE;
      break;
    case UPDATE_PSI2: /* PSI2 updated*/
      psc_db->state_of_PSI[PSI2].state = NEEDED;
      return_value = TRUE;
      break;
    case UPDATE_PSI3: /* PSI3/PSI3bis/PSI3ter updated*/
      psc_db->state_of_PSI[PSI3].state = NEEDED;
      psc_db->state_of_PSI[PSI3bis].state = NEEDED;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
      if(psc_db->v_add_psi AND psc_db->add_psi.psi3ter_broadcast)
      {
        psc_db->state_of_PSI[PSI3ter].state = NEEDED;   
      }
      else
      {
         psc_db->state_of_PSI[PSI3ter].state = NOT_SEND; 
      }
#endif
      return_value = TRUE;
      break;
    case UPDATE_PSI4: /* PSI4 updated*/
      psc_db->state_of_PSI[PSI4].state = NEEDED;
      return_value = TRUE;
      break;
    case UPDATE_PSI5: /* PSI5 updated*/
      psc_db->state_of_PSI[PSI5].state = NEEDED;
      return_value = TRUE;
      break;
#ifdef REL99
    case UPDATE_PSI8:
      psc_db->state_of_PSI[PSI8].state = NEEDED;
      return_value = TRUE;
      break;
#endif
    default: /* All other values shall be interpreted as 'Update of unknown SI message type'.*/
      return_value = FALSE;
      break;
  }
  return return_value;
} /* psi_check_change_field() */

/*
+------------------------------------------------------------------------------
| Function    : psi_process_psi1
+------------------------------------------------------------------------------
| Description : The function psi_process_psi1() ....
|
| Parameters  : T_PSI_1 *psi1
|
+------------------------------------------------------------------------------
*/
GLOBAL T_PSI1_RET psi_process_psi1 (T_PSI_1 *psi1 )
{
  T_PSI1_RET return_val = PSI1_OK;
#ifdef REL99
  UBYTE current_nw_rel  = psc_db->network_rel;
#endif
  
  TRACE_FUNCTION( "psi_process_psi1" );

  if(psi1->pccch_org_par.bs_pcc_rel)
  {
    /*
     * last PDCH carrying PCCCH and PBCCH will be released shortly, receive si13
     */
    return_val = PSI1_PBCCH_RELEASED;
  }
  else if( grr_is_pbcch_present( ) EQ FALSE )
  {
    return_val = PSI1_PBCCH_ESTABLISHED;
  }
  else
  {
    psc_db->state_of_PSI[PSI1].state = RECEIPT_OK;
    psc_db->complete_acq.psi1_ok = TRUE; /*PSI1 has been received*/

    /* copy repeat period*/
    psc_db->psi1_params.psi1_repeat_period = psi1->psi1_rep_per + 1;

    /* copy repetition rates */
    psc_db->psi1_params.psi_cnt_lr = psi1->psi_cnt_lr;
    if(psi1->v_psi_cnt_hr)
    {
      psc_db->psi1_params.psi_cnt_hr = psi1->psi_cnt_hr + 1;
    }
    else
    {
      psc_db->psi1_params.psi_cnt_hr = 0;/*default*/
    }


    psc_db->psi1_params.psi_status_supported = (psi1->psi_status_ind EQ 1) ? TRUE : FALSE;
    psc_db->measurement_order = psi1->meas_order; /*PSI5 is present in the cell or not*/

    if(!psc_db->measurement_order)
      psc_db->state_of_PSI[PSI5].state = RECEIPT_OK;

    /* Process PRACH parameters*/
    psi_process_prach(&(psi1->prach_ctrl_par));

    /*copy pccch org params*/
    psc_db->pccch.bs_pcc_rel    = psi1->pccch_org_par.bs_pcc_rel;
    psc_db->pccch.bs_pbcch_blks = psi1->pccch_org_par.bs_pbcch_blks;
    psc_db->pccch.bs_pag_blks   = CLIP_BS_PAG_BLKS_RES( psi1->pccch_org_par.bs_pag_blks );
    psc_db->pccch.bs_prach_blks = CLIP_BS_PRACH_BLKS( psi1->pccch_org_par.bs_prach_blks );

    /*copy GPRS cell options*/
    psc_db->v_gprs_cell_opt                      = TRUE;
    psc_db->cell_info_for_gmm.cell_info.net_mode = psi1->gprs_cell_opt.nmo;
    psc_db->gprs_cell_opt.nmo                    = psi1->gprs_cell_opt.nmo;
    psc_db->gprs_cell_opt.t3168                  = psi1->gprs_cell_opt.t3168;
    psc_db->gprs_cell_opt.t3192                  = psi1->gprs_cell_opt.t3192;
    psc_db->gprs_cell_opt.drx_t_max              = psi1->gprs_cell_opt.drx_t_max;
    psc_db->gprs_cell_opt.ab_type                = psi1->gprs_cell_opt.ab_type;
    psc_db->gprs_cell_opt.ctrl_ack_type          = psi1->gprs_cell_opt.ctrl_ack_type;
    psc_db->gprs_cell_opt.bs_cv_max              = psi1->gprs_cell_opt.bs_cv_max;
                                                
    memcpy(&(psc_db->gprs_cell_opt), &(psi1->gprs_cell_opt), sizeof(T_gprs_cell_opt));

    /*if(psi1->gprs_cell_opt.v_pan_struct)
    {
      psc_db->gprs_cell_opt.pan_struct.inc  = psi1->gprs_cell_opt.pan_struct.inc;
      psc_db->gprs_cell_opt.pan_struct.dec  = psi1->gprs_cell_opt.pan_struct.dec;
      psc_db->gprs_cell_opt.pan_struct.pmax = ( psi1->gprs_cell_opt.pan_struct.pmax + 1 ) * 4;
      psc_db->gprs_cell_opt.v_pan_struct    = 1;
    }*/
    if(psi1->gprs_cell_opt.v_gprs_ext_bits)
    {
      memcpy(&(psc_db->gprs_cell_opt.gprs_ext_bits), &(psi1->gprs_cell_opt.gprs_ext_bits), sizeof(T_gprs_ext_bits));
      psc_db->gprs_cell_opt.v_gprs_ext_bits = 1;
    }

    /*Global power control parameters*/
    grr_store_g_pwr_par( &psi1->g_pwr_par );

    /* check whether the PSI4 is broadcast or not*/
    if(!psc_db->g_pwr_par.imeas_chan_list)
      psc_db->state_of_PSI[PSI4].state = RECEIPT_OK;


    TRACE_EVENT_P2( "PSI1: database change mark: %d, message change mark: %d",
                    psc_db->psi1_params.pbcch_change_mark, 
                    psi1->pbcch_change_ma );

    if(psc_db->psi1_params.first_psi1)
    {
      psc_db->psi1_params.pbcch_change_mark = psi1->pbcch_change_ma; /* set the new change mark value*/
    }
    else
    {
      UBYTE incremented_value = psi_compare_change_mark(psi1->pbcch_change_ma, psc_db->psi1_params.pbcch_change_mark);
      psc_db->psi1_params.pbcch_change_mark = psi1->pbcch_change_ma; /* set the new change mark value*/

      if(incremented_value EQ 1)/*partial acq*/
      {
        if(psi_check_change_field(psi1->psi_change_field))
        {
          return_val = PSI1_PARTIAL_ACQ;
        }
      }
      else if(incremented_value > 1) /* incremented_value more than 1: complete acq*/
      {
        return_val = PSI1_COMPLETE_ACQ;
      }
    }

#ifdef REL99
    /* Update the BSS release from PSI1 instead of SI13, when PBCCH is present */ 
    /* Update the SGSN release */
    if (psi1->v_release_99_str_psi_1)
    {
      psc_db->network_rel = BSS_NW_REL_99;
      psc_db->sgsn_rel = psi1->release_99_str_psi_1.sgsnr ? PS_SGSN_99_ONWARDS : PS_SGSN_98_OLDER;
#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
      psc_db->band_indicator = psi1->release_99_str_psi_1.band_indicator;
#endif
    }
    else
    {
      psc_db->sgsn_rel = PS_SGSN_98_OLDER;
#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
      psc_db->band_indicator = NOT_PRESENT_8BIT;
#endif
    }

#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
    /*
     * Update std depending on the band indicator
     */
    att_update_std_band_indicator ( psc_db->band_indicator ); 
#endif

    /* Please note that there is no check for (psi1->release_99 EQ FALSE) and 
     * then setting the psc_db->network_rel to BSS_NW_REL_97 as sometimes the
     * the R99 or R4 network may not broadcast the optional R99 IEs. At that 
     * time, the network release is assigned wrongly if the above check exists.
     */
    if(psc_db->network_rel NEQ current_nw_rel)
    {
      TRACE_EVENT_P2("(PSI1)BSS Network release changed from %d to %d <0 - REL_97, 1 - REL_99, 2 - REL_04>", 
        current_nw_rel, psc_db->network_rel);
    }

    /* Update the SGSN release in the Common library context */
    cl_nwrl_set_sgsn_release(psc_db->sgsn_rel);
#endif

  }
  TRACE_EVENT_P1( "PSI1: received with return value %d", return_val );
  grr_set_pg_nmo();
  return return_val;
} /* psi_process_psi1() */


/*
+------------------------------------------------------------------------------
| Function    : psi_receive_psi
+------------------------------------------------------------------------------
| Description : The function psi_receive_psi() ....
|
| Parameters  : BOOL read_all
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_receive_psi ( UBYTE  psi_reading_type)
{
  TRACE_FUNCTION( "psi_receive_psi" );
  
  if(grr_data->psi.is_pbcch_req_allowed)  
  {
    /*
     * Prepare MPHP_SCELL_PBCCH_REQ to receive psi
     */
    PALLOC(mphp_scell_pbcch_req, MPHP_SCELL_PBCCH_REQ);
    psi_prepare_scell_pbcch_req(mphp_scell_pbcch_req, psi_reading_type);    
    PSEND(hCommL1,mphp_scell_pbcch_req);
    grr_data->psi.is_pbcch_req_needed =FALSE;
  }
  else
  {
    TRACE_EVENT("PBCCH request needed after release of TBF, stop of RR tasks, etc.");
    grr_data->psi.is_pbcch_req_needed =TRUE;
    /*
     * store the reading_type in case of needed in/after release procedure
     */
    grr_data->psi.reading_type=psi_reading_type;  
  }
} /* psi_receive_psi() */


/*
+------------------------------------------------------------------------------
| Function    : psi_process_prach
+------------------------------------------------------------------------------
| Description : The function psi_process_prach() ....
|
| Parameters  : T_prach_ctrl_par *prach: pointer to PRACH control parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_process_prach ( T_prach_ctrl_par *prach )
{
  TRACE_FUNCTION( "psi_process_prach" );

  psc_db->v_prach = TRUE;

  memcpy(&(psc_db->prach), prach, sizeof(T_prach_ctrl_par));

  TRACE_EVENT_P5("ac_class: %d max_ret %d %d %d %d", 
    prach->ac_class,         
    prach->max_retrans[0],
    prach->max_retrans[1], 
    prach->max_retrans[2], 
    prach->max_retrans[3]); 
  TRACE_EVENT_P2("s_prach: %d tx_int:%d", prach->s_prach, prach->tx_int);
  if(prach->v_pers_lev)
  {
    TRACE_EVENT_P4("pers: %d %d %d %d",
      prach->pers_lev.plev[0],
      prach->pers_lev.plev[1],
      prach->pers_lev.plev[2],
      prach->pers_lev.plev[3]);
  }


} /* psi_process_prach() */


/*
+------------------------------------------------------------------------------
| Function    : psi_reset_si_entries
+------------------------------------------------------------------------------
| Description : The function psi_reset_si_entries() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_reset_si_entries ( void )
{
  TRACE_FUNCTION( "psi_reset_si_entries" );

  psc_db->psi13_params.bcch_change_mark = NOT_SET;
  psc_db->psi13_params.si13_change_mark = NOT_SET;

} /* psi_reset_si_entries() */

/*
+------------------------------------------------------------------------------
| Function    : psi_acq_state_of_si
+------------------------------------------------------------------------------
| Description : The function psi_acq_state_of_si() ....
|               This function is ONLY called after processing SI13 message
| Parameters  : T_si_states  si_states: contains the state of the SI messages
|               needed for checking for packet access allowance
|
+------------------------------------------------------------------------------
*/
GLOBAL T_ACQ_STATE_RET psi_acq_state_of_si ( T_si_states  si_states)
{
  T_ACQ_STATE_RET ret_value = ACQ_RUNNING;

  TRACE_FUNCTION( "psi_acq_state_of_si" );

  switch(psc_db->acq_type)
  {
    case PARTIAL:
        ret_value = ACQ_PART_OK;
      break;
    case COMPLETE:
      if((si_states.si3_state EQ SI3_RECEIVED) AND (si_states.si13_state EQ SI13_RECEIVED))
      {
        if((si_states.si1_state EQ SI1_NOT_SEND) OR (si_states.si1_state EQ SI1_RECEIVED))
        {
          ret_value = ACQ_COMP_OK;
        }
      }
      break;
    case PERIODICAL_SI13_READING:
      ret_value = ACQ_PERIOD_OK;
      break;
    default:
      break;
  }
  return ret_value;
} /* psi_acq_state_of_si() */


/*
+------------------------------------------------------------------------------
| Function    : psi_is_update_needed
+------------------------------------------------------------------------------
| Description : The function psi_is_update_needed() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL psi_is_update_needed ( UBYTE si_change_field )
{
  TRACE_FUNCTION( "psi_is_update_needed" );

  switch(si_change_field)
  {
    case UNSPECIFIED_SI:
    case UPDATE_SI1:
    case UPDATE_SI2_SI2BIS_OR_SI2TER:
    case UPDATE_SI3_SI4_SI7_OR_SI8:
    case UPDATE_SI9:
      return TRUE;
    default:
      return FALSE;
  }
} /* psi_is_update_needed() */

/*
+------------------------------------------------------------------------------
| Function    : psi_update_data_to_request
+------------------------------------------------------------------------------
| Description : The function psi_update_data_to_request updates the static
|                   and  hopping frequencies and other parameters to request PSI
|                   data. This function is called after receiving SI13 or PSI13 or PSI1
|
| Parameters  : UBYTE init_needed: checks whether initialization is needed or not
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_update_data_to_request( UBYTE init_needed)
{
  T_MPHP_SCELL_PBCCH_REQ *ptr_prim;
  
  TRACE_FUNCTION( "psi_update_data_to_request" );

  ptr_prim = &psc_db->scell_pbcch;

  if(init_needed)
  {
    TRACE_EVENT("Init Needed");
    memset(&psc_db->pccch, 0, sizeof(T_pccch_org_par)); /*initial*/
    psc_db->psi1_params.psi_cnt_hr = 0; /* initial*/
    psc_db->psi1_params.psi_cnt_lr = 0; /* initial*/
  }

  /*
   * set values for mphp_scell_pbcch_req
   */
  ptr_prim->pb              = psc_db->pbcch.pbcch_des.pb;             /*<   Power reduction value          */
  ptr_prim->bs_pbcch_blks   = psc_db->pccch.bs_pbcch_blks;            /*<   Number of PBCCH per multiframe */
  ptr_prim->psi1_rep_period = psc_db->psi1_params.psi1_repeat_period; /*<   PSI1 repeat period             */
  ptr_prim->p_ch_des.tsc    = psc_db->pbcch.pbcch_des.tsc;            /*<  Training sequence code         */
  ptr_prim->p_ch_des.tn     = psc_db->pbcch.pbcch_des.tn;             /*<  timeslot number */

  if  (psc_db->pbcch.pbcch_des.v_arfcn 
        OR 
        (
          (psc_db->pbcch.pbcch_des.flag EQ 0)
          AND /* use bcch.arfcn*/
          (psc_db->pbcch.pbcch_des.flag2 EQ 0)
        )
       )
  {
    ptr_prim->p_ch_des.p_chan_sel.hopping       = 0;
    ptr_prim->p_ch_des.p_chan_sel.p_rf_ch.arfcn = (psc_db->pbcch.pbcch_des.v_arfcn )
                                                  ?grr_g23_arfcn_to_l1(psc_db->pbcch.pbcch_des.arfcn)
                                                  :grr_g23_arfcn_to_l1(psc_db->pbcch.bcch.arfcn);                      

    TRACE_EVENT_P8("Up_s: pbcch:%d per:%d hr:%d lr:%d pb:%d tsc:%d tn:%d arfcn:%d",
      psc_db->pccch.bs_pbcch_blks,
      psc_db->psi1_params.psi1_repeat_period,
      psc_db->psi1_params.psi_cnt_hr,
      psc_db->psi1_params.psi_cnt_lr,
      psc_db->pbcch.pbcch_des.pb,
      psc_db->pbcch.pbcch_des.tsc,
      psc_db->pbcch.pbcch_des.tn,
      ptr_prim->p_ch_des.p_chan_sel.p_rf_ch.arfcn);        
  }
  else
  {
    grr_create_freq_list(  MA_NUMBER_4_PSI13_OR_CELL_ALLOC,
                           psc_db->pbcch.pbcch_des.maio,
                          &ptr_prim->p_ch_des.p_chan_sel,
                          &ptr_prim->p_freq_list );

    TRACE_EVENT_P8("Up_h1: pbcch:%d per:%d hr:%d lr:%d pb:%d tsc:%d tn:%d maio:%d",
      psc_db->pccch.bs_pbcch_blks,
      psc_db->psi1_params.psi1_repeat_period,
      psc_db->psi1_params.psi_cnt_hr,
      psc_db->psi1_params.psi_cnt_lr,
      psc_db->pbcch.pbcch_des.pb,
      psc_db->pbcch.pbcch_des.tsc,
      psc_db->pbcch.pbcch_des.tn,
      psc_db->pbcch.pbcch_des.maio);



  }
} /* psi_update_data_to_request() */

/*
+------------------------------------------------------------------------------
| Function    : psi_is_access_class_changed
+------------------------------------------------------------------------------
| Description : This function checks the access class control parameter and
|               return TRUE/FALSE value
|
| Parameters  : void; return BOOL: TRUE: access class changed; FALSE: nothing has changed
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL psi_is_access_class_changed(void)
{
  USHORT dummy1, dummy2, access_changed;

  TRACE_FUNCTION("psi_is_access_class_changed");

  access_changed = FALSE;
  /* check the access control class and inform the service: CTRL
   * check access class control for authorized classes 0 to 9 and
   * check access class control for authorized classes 11 to 15 and
   */
  dummy1 = grr_data->ms.access_ctrl_class & 0x3FF;
  dummy2 = grr_data->ms.access_ctrl_class & 0xF800;

  if(
      ((dummy1 & ~psc_db->prach.ac_class) > 0) /* 0 to 9*/
      OR
      ((dummy2 & ~psc_db->prach.ac_class) > 0) /* 11 to 15*/
    )
  {
    /*
     * The MS belongs to one of the authorized access classes
     */
    if(psc_db->cell_info_for_gmm.access_status NEQ GPRS_ACCESS_ALLOWED)
    {
      /*
       * Access was NOT allowed, so we have to inform GMM
       */
      access_changed = TRUE;
      psc_db->cell_info_for_gmm.access_status = GPRS_ACCESS_ALLOWED;
    }
  }
  else
  {
    /*
     * Access barred
     */
    if(psc_db->cell_info_for_gmm.access_status NEQ GPRS_ACCESS_BARRED)
    {
      /*
       * Access was ALLOWED, so we have to inform GMM
       */
      access_changed = TRUE;
      psc_db->cell_info_for_gmm.access_status = GPRS_ACCESS_BARRED;
    }
  }

  if( access_changed )
  {
    TRACE_EVENT( "Access class changed" );

    grr_set_cell_info_service( );
  }

  return( access_changed );
}/* psi_is_access_class_changed*/

/*
+------------------------------------------------------------------------------
| Function    : psi_init_params
+------------------------------------------------------------------------------
| Description : The function psi_init_params() ....
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_init_params ( void )
{
#if defined (TI_PS_FF_RTD) AND defined (REL99)
  UBYTE n;
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */

  TRACE_FUNCTION( "psi_init_params" );

  grr_data->psi.is_pbcch_req_needed =FALSE; /*PBCCH req. waiting for perform*/
  grr_data->psi.is_pbcch_req_allowed =TRUE; /*PBCCH req. allowed due to transition rules or not*/
#if defined (TI_PS_FF_RTD) AND defined (REL99)
  for( n = 0; n < MAX_NR_OF_NCELL; n++ )
        grr_data->psi.rtd[n] = RTD_NOT_AVAILABLE;
#endif /* #if defined (TI_PS_FF_RTD) AND defined (REL99) */

  psi_reset_all();

  psi_init_rfl_psi2(); /* set the Reference Frequency List parameters to initial values */
  psi_init_cell_alloc(); /* init cell allocation structure */
  psi_init_gprs_ms_alloc(FALSE); /* init GPRS Mobile Allocations struct */

} /* psi_init_params() */


/*
+------------------------------------------------------------------------------
| Function    : psi_stop_timer
+------------------------------------------------------------------------------
| Description : The function stops psi timers if they are running
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_stop_timer ( void )
{
  TRACE_FUNCTION( "psi_stop_timer" );
  
  vsi_t_stop(GRR_handle,T_10_SEC);

  vsi_t_stop(GRR_handle,T_30_SEC);

  vsi_t_stop(GRR_handle,T_60_SEC);
  
  vsi_t_stop(GRR_handle,T_COMP_PSI);
  
} /* psi_stop_timer() */

/*
+------------------------------------------------------------------------------
| Function    : psi_is_access_allowed
+------------------------------------------------------------------------------
| Description : The function checks whether the access to the network is allowed 
|               or not
|
| Parameters  : BOOL: TRUE if access allowed, otherwise FALSE
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL psi_is_access_allowed(void)
{
  TRACE_FUNCTION("psi_is_access_allowed");
  /* 
   * Check whether access is enabled or not: Access is allowed
   * if PSI acquisition is completed or PSI1&PSI2 was read with 
   * success AND psi status is suppoerted by the network
   */
#ifdef _SIMULATION_
  if(
     psc_db->psi1_params.psi_status_supported
      AND
     psc_db->complete_acq.psi1_ok 
      AND 
     psc_db->complete_acq.psi2_ok
    )
    return TRUE;
#endif /* #ifdef _SIMULATION_ */

  return FALSE;
}/*psi_is_access_allowed */

/*
+------------------------------------------------------------------------------
| Function    : psi_reread_psi2
+------------------------------------------------------------------------------
| Description : The function starts the reading process of PSI2
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_reread_psi2(void)
{
  TRACE_FUNCTION("psi_reread_psi2");

  psc_db->state_of_PSI[PSI2].state = NEEDED;
  psi_partial_acq();
  psi_reset_psi2();
  psi_start_10sec();
}/* psi_reread_psi2*/


/*
+------------------------------------------------------------------------------
| Function    : psi_reread_psi3_and_3bis
+------------------------------------------------------------------------------
| Description : The function starts the reading process of PSI3 nad PSI3bis
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_reread_psi3_and_3bis(void)
{
  TRACE_FUNCTION("psi_reread_psi3_and_3bis");

  psc_db->state_of_PSI[PSI3bis].state = NEEDED;
  psc_db->state_of_PSI[PSI3].state = NEEDED;
  psi_partial_acq();

  /*
   * Reset PSI3 and PSI3BIS paramter
   */
  psi_reset_psi3bis();
  psi_reset_psi3();
  psi_start_10sec();
}/* psi_reread_psi3_and_3bis*/


/*
+------------------------------------------------------------------------------
| Function    : psi_reread_psi3bis
+------------------------------------------------------------------------------
| Description : The function starts the reading process of PSI3bis
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_reread_psi3bis(void)
{
  TRACE_FUNCTION("psi_reread_psi3bis");

  /*
   * Message has changed, reset all values and read new consistent message set
   */
  psc_db->state_of_PSI[PSI3bis].state = NEEDED;
  psi_partial_acq();

  psi_reset_psi3bis();
  psi_start_10sec();
}/* psi_reread_psi3bis*/

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : psi_reread_psi3ter
+------------------------------------------------------------------------------
| Description : The function starts the reading process of PSI3ter
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_reread_psi3ter(void)
{
  TRACE_FUNCTION("psi_reread_psi3ter");

  /*
   * Message has changed, reset all values and read new consistent message set
   */
  psc_db->state_of_PSI[PSI3ter].state = NEEDED;
  psi_partial_acq();

  psi_reset_psi3ter();
  psi_start_10sec();
}/* psi_reread_psi3ter*/

/*
+------------------------------------------------------------------------------
| Function    : psi_reread_psi3_3bis_3ter
+------------------------------------------------------------------------------
| Description : The function starts the reading process of PSI3 to PSI3ter
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_reread_psi3_3bis_3ter(void)
{
  TRACE_FUNCTION("psi_reread_psi3_3bis_3ter");

  if(psc_db->v_add_psi AND psc_db->add_psi.psi3ter_broadcast)
  {
    psc_db->state_of_PSI[PSI3ter].state = NEEDED;   
  }
  else
  {
    psc_db->state_of_PSI[PSI3ter].state = NOT_SEND; 
  }

  psc_db->state_of_PSI[PSI3bis].state = NEEDED;
  psc_db->state_of_PSI[PSI3].state = NEEDED;
  psi_partial_acq();

  /*
   * Reset PSI3 to PSI3ter parameters
   */
  /* psi_reset_psi3quater(); */
  psi_reset_psi3ter();
  psi_reset_psi3bis();
  psi_reset_psi3();
  psi_start_10sec();
}/* psi_reread_psi3_3bis_3ter */

#endif


/*
+------------------------------------------------------------------------------
| Function    : psi_reread_psi4
+------------------------------------------------------------------------------
| Description : The function starts the reading process of PSI4
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_reread_psi4(void)
{
  TRACE_FUNCTION("psi_reread_psi4");

  psc_db->state_of_PSI[PSI4].state = NEEDED;
  psi_partial_acq();

  /*
   * Reset PSI4
   */
  psi_reset_psi4();
  psi_start_10sec();
}/* psi_reread_psi4*/

/*
+------------------------------------------------------------------------------
| Function    : psi_reread_psi5
+------------------------------------------------------------------------------
| Description : The function starts the reading process of PSI5
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_reread_psi5(void)
{
  TRACE_FUNCTION("psi_reread_psi5");

  psc_db->state_of_PSI[PSI5].state = NEEDED;
  psi_partial_acq();

  /*
   * Reset PSI5
   */
  psi_reset_psi5();
  psi_start_10sec();
}/* psi_reread_psi5*/

#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : psi_reread_psi8
+------------------------------------------------------------------------------
| Description : The function starts the reading process of PSI8
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_reread_psi8(void)
{
  TRACE_FUNCTION("psi_reread_psi8");

  psc_db->state_of_PSI[PSI8].state = NEEDED;
  psi_partial_acq();

  /*
   * Reset PSI8
   */
  psi_reset_psi8();
  psi_start_10sec();
}/* psi_reread_psi8*/
#endif

/*
+------------------------------------------------------------------------------
| Function    : psi_handle_psi1
+------------------------------------------------------------------------------
| Description : The function handles PSI1 message on both PBCCH and PACCH channels
|
| Parameters  : T_PSI_1*
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_handle_psi1(T_PSI_1 * psi1)
{
  TRACE_FUNCTION("psi_handle_psi1");
  switch(psi_process_psi1(psi1))
  {    
    case PSI1_PBCCH_RELEASED:
      SET_STATE( PSI, PSI_NULL );
      psi_initiate_pbcch_switching( PSI_DC_PBCCH_RELEASED );
      return; /* sorry: dirty solution */
    case PSI1_PBCCH_ESTABLISHED:
      SET_STATE( PSI, PSI_NULL );
      psi_initiate_pbcch_switching( PSI_DC_PBCCH_ESTABLISHED );
      return; /* sorry: another dirty solution */
    case PSI1_OK:
      if( psc_db->measurement_order EQ FALSE )
      {
        sig_psi_ctrl_meas_param_invalid_psi5( );
      }

      /* update readin parameters of psi */
      psi_update_data_to_request(INIT_NOT_NEEDED);
      /*
       * check the state of the acquisition
       */
      switch(psi_check_acq_state())
      {
        case ACQ_RUNNING:
          /*
           * Check whether access to the network is allowed or not,
           * in case of having complete acq
           */
          if(psi_is_access_allowed() AND (psc_db->acq_type EQ COMPLETE))
          {
            psc_db->psi1_params.first_psi1 = FALSE;
            psi_stop_psi_reading(NONE);
            psi_stop_10sec(); /* Stop this timer, if running */
#ifdef _SIMULATION_
            vsi_t_stop(GRR_handle,T_COMP_PSI);
#endif
            /*
             * restart 30 sec:e.g. wait 30 sec for reread PSI1
             */
            psi_stop_30sec(TRUE);

            /* 
             * Access to the network is allowed
             */
            psi_send_access_enable_if_needed();            
          }
          else if(psi_is_access_allowed() AND (psc_db->acq_type EQ FULL_PSI_IN_NEW_CELL))
          {
            /*
             * Access to the new re-selected cell is allowed
             */
            psc_db->psi1_params.first_psi1 = FALSE;
            psi_stop_psi_reading(NONE);
            /*
             * restart 30 sec:e.g. wait 30 sec for reread PSI1
             */
            psi_stop_30sec(TRUE);
            sig_psi_ctrl_ncell_psi_read( TRUE);      
          }
          else
          {
            /*
             * Acq. is running, check whether we received the PSI1 message
             * for the first time or not.
             * if the PSI1 message is received for the first time, restart to read PBCCH blocks,
             * because of reading new parameters in PSI1 message,e.g. PCCCH parameters
             */
            if(
               psc_db->psi1_params.first_psi1 
                AND 
               ((psc_db->acq_type EQ COMPLETE) OR (psc_db->acq_type EQ FULL_PSI_IN_NEW_CELL))
              )
            {
              psi_stop_30sec(TRUE);/*restart 30 sec*/
              psi_receive_psi(READ_COMPLETE);
            }
          }
          break;
        case ACQ_PART_OK:
          psi_stop_psi_reading(NONE);
#ifdef _TARGET_
          TRACE_EVENT("PSI1->NEW_PCCCH");
          sig_psi_ctrl_new_pccch();
#if defined (REL99) AND defined (TI_PS_FF_EMR)
          psc_db->network_pg_mode = psi1->page_mode;
#endif
#endif
          break;
        case ACQ_COMP_OK:
          psi_stop_60sec(TRUE);/*restart 60 sec*/
          psi_stop_30sec(TRUE);/*restart 30 sec*/
          psi_stop_psi_reading(NONE);
          /* 
           * Access to the network is allowed
           */
          psi_send_access_enable_if_needed();
          break;
        case ACQ_PERIOD_OK:
          /*
           * Periodical pSI1 reading is O.K.
           */
          psi_stop_60sec(TRUE);/*restart 60 sec*/
          psi_stop_30sec(TRUE);/*restart 30 sec*/
          psi_stop_psi_reading(NONE);
          break;
        default:
          TRACE_ERROR("Unexpected acq_ret in PSI1");
          break;
      }/* switch acq_state*/
      break;
    case PSI1_COMPLETE_ACQ:
      psi_initiate_read_complete_psi( INIT_NOT_NEEDED );
      break;
    case PSI1_PARTIAL_ACQ:
      /* update psi data*/
      psi_stop_60sec(TRUE);/*restart 60 sec*/
      psi_stop_30sec(TRUE);/*restart 30 sec*/
      psi_update_data_to_request( INIT_NOT_NEEDED);
      psi_partial_acq();
      /*TRACE_EVENT("Complete instead partial");
      psi_receive_psi(READ_COMPLETE);*/
      psc_db->send_psi_status = TRUE; /* this parameters has meaning if the network supports PSI STATUS */
      psi_start_10sec();
      break;
  }/* switch process_psi1*/
  if(psi_is_access_class_changed() AND !psc_db->psi1_params.first_psi1)
  {
    /*
     * Implies the CTRL to send CELL_IND
     */
    sig_psi_ctrl_access_changed();
  }
  if(psc_db->acq_type NEQ COMPLETE)
  {
    psc_db->psi1_params.first_psi1 = FALSE;
  }

}/* psi_handle_psi1*/

/*
+------------------------------------------------------------------------------
| Function    : psi_handle_psi2
+------------------------------------------------------------------------------
| Description : The function handles PSI2 message on both PBCCH and PACCH channels
|
| Parameters  : T_PSI_2*
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_handle_psi2(T_PSI_2 * psi2)
{
  TRACE_FUNCTION("psi_handle_psi2");
  switch(psi_process_psi2(psi2))
  {
    case PSI2_OK:
      switch(psi_check_acq_state())
      {
        case ACQ_RUNNING:
          /*
           * Check whether access to the network is allowed or not,
           * in case of having complete acq
           */
          if(psi_is_access_allowed() AND (psc_db->acq_type EQ COMPLETE))
          {
            psi_stop_psi_reading(NONE);
            psi_stop_10sec(); /* Stop this timer, if running */
#ifdef _SIMULATION_
            vsi_t_stop(GRR_handle,T_COMP_PSI);
#endif
            /* 
             * Access to the network is allowed
             */
            psi_send_access_enable_if_needed();
          }
          else if(psi_is_access_allowed() AND (psc_db->acq_type EQ FULL_PSI_IN_NEW_CELL))
          {
            /*
             * Access to the new re-selected cell is allowed
             */
            psi_stop_psi_reading(NONE);
            sig_psi_ctrl_ncell_psi_read( TRUE );      
          }
          break;
        case ACQ_PART_OK:
          psi_stop_psi_reading(NONE);
          /* 
           * TimeSlot number may have changed, so we have to restart PCCCH reading
           */
          TRACE_EVENT("PSI2->NEW_PCCCH");
          sig_psi_ctrl_new_pccch();
#if defined (REL99) AND defined (TI_PS_FF_EMR)
          psc_db->network_pg_mode = psi2->page_mode;
#endif
          break;
        case ACQ_COMP_OK:
          psi_stop_psi_reading(NONE);
          /* 
           * Access to the network is allowed
           */
          psi_send_access_enable_if_needed();
          break;
        default:
          TRACE_ERROR("Unexpected acq_ret in PSI2");
          break;
      }/*switch check_acq*/
      break;
    case PSI2_REREAD:
      /* 
       * Error situation read a new PSI2
       */
      psi_reread_psi2();
      break;
  }
}/* psi_handle_psi2*/
/*
+------------------------------------------------------------------------------
| Function    : psi_handle_psi3
+------------------------------------------------------------------------------
| Description : The function handles PSI3 message on both PBCCH and PACCH channels
|
| Parameters  : T_PSI_3*
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_handle_psi3(T_PSI_3 * psi3)
{
  TRACE_FUNCTION("psi_handle_psi3");
  switch(psi_process_psi3(psi3))
  {
    case PSI3_NCELL_VALID:
      /*
       * Inform MEAS that the NCELL parameters are valid
       */
      sig_psi_ctrl_ncell_param_valid();
      
      /*lint -fallthrough*/
    
    case PSI3_OK:
      /*
       * check the acquisition state: it is only for having a complete set of information
       */
      switch(psi_check_acq_state())
      {
        case ACQ_RUNNING:
          /*
           * Nothing to do
           */
          break;
        case ACQ_PART_OK:
          psi_stop_psi_reading(NONE);
#ifdef _TARGET_
          TRACE_EVENT("PSI3->NEW_PCCCH");
          sig_psi_ctrl_new_pccch();
#if defined (REL99) AND defined (TI_PS_FF_EMR)
          psc_db->network_pg_mode = psi3->page_mode;
#endif
#endif
          break;
        case ACQ_COMP_OK:
          psi_stop_psi_reading(NONE);
          /* 
           * Access to the network is allowed
           */
          psi_send_access_enable_if_needed();
          break;
        default:
          TRACE_ERROR("Unexpected acq_ret in PSI3");
          break;
      }/*switch check_acq*/
      break;
    case PSI3_NCELL_INVALID:
      grr_init_nc_list( &psc_db->nc_cw.list );
      sig_psi_ctrl_ncell_param_invalid();
      break;
    case PSI3_CELL_BARRED:
      sig_psi_ctrl_access_barred();
      break;
  }/* switch psi_process_psi3*/
}/*psi_handle_psi3*/
/*
+------------------------------------------------------------------------------
| Function    : psi_handle_psi3bis
+------------------------------------------------------------------------------
| Description : The function handles PSI3_BIS message on both PBCCH and PACCH channels
|
| Parameters  : T_PSI_3_BIS*
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_handle_psi3bis(T_PSI_3_BIS * psi3bis)
{
  TRACE_FUNCTION("psi_handle_psi3bis");
  switch(psi_process_psi3bis(psi3bis))
  {
    case PSI3BIS_REREAD_PSI3BIS:
      psi_reread_psi3bis();
      /*
       * Ncell parameter are invalid. We have to inform MEAS-Service
       */
      grr_init_nc_list( &psc_db->nc_cw.list );
      sig_psi_ctrl_ncell_param_invalid();
      break;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    case PSI3BIS_REREAD_PSI3_3BIS_3TER:
      psi_reread_psi3_3bis_3ter();
#else
    case PSI3BIS_REREAD_PSI3_3BIS:
      psi_reread_psi3_and_3bis();
#endif
      /*
       * Ncell parameter are invalid. We have to inform MEAS-Service
       */
      grr_init_nc_list( &psc_db->nc_cw.list );
      sig_psi_ctrl_ncell_param_invalid();
      break;

    case PSI3BIS_NCELL_VALID:
      /*
       * Inform MEAS that the NCELL parameters are valid
       */
      sig_psi_ctrl_ncell_param_valid();
      
      /*lint -fallthrough*/
    
    case PSI3BIS_OK:
      /*
       * check the acquisition state: it is only for having a complete set of information
       */
      switch(psi_check_acq_state())
      {
        case ACQ_RUNNING:
          /*
           * Nothing to do
           */
          break;
        case ACQ_PART_OK:
          psi_stop_psi_reading(NONE);
#ifdef _TARGET_
          TRACE_EVENT("PSI3bis->NEW_PCCCH");
          sig_psi_ctrl_new_pccch();
#if defined (REL99) AND defined (TI_PS_FF_EMR)
          psc_db->network_pg_mode = psi3bis->page_mode;
#endif
#endif
          break;
        case ACQ_COMP_OK:
          psi_stop_psi_reading(NONE);
          /* 
           * Access to the network is allowed
           */
          psi_send_access_enable_if_needed();
          break;
        default:
          TRACE_ERROR("Unexpected acq_ret in PSI3bis");
          break;
      }/*switch check_acq*/
    break;
  }/* switch process_psi3bis*/

}/*psi_handle_psi3bis*/

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : psi_handle_psi3ter
+------------------------------------------------------------------------------
| Description : The function handles PSI3_TER message on both PBCCH and PACCH channels
|
| Parameters  : T_PSI_3_TER*
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_handle_psi3ter(T_PSI_3_TER * psi3ter)
{
  TRACE_FUNCTION("psi_handle_psi3ter");
  switch(psi_process_psi3ter(psi3ter))
  {
    case PSI3TER_REREAD_PSI3TER:
      psi_reread_psi3ter();
      break;
    case PSI3TER_REREAD_PSI3_3BIS_3TER:
      psi_reread_psi3_3bis_3ter();
      break;
    case PSI3TER_OK:
      /*
       * check the acquisition state: it is only for having a complete set of information
       */
      switch(psi_check_acq_state())
      {
        case ACQ_RUNNING:
          /*
           * Nothing to do
           */
          break;
        case ACQ_PART_OK:
          psi_stop_psi_reading(NONE);
#ifdef _TARGET_
          TRACE_EVENT("PSI3ter->NEW_PCCCH");
          sig_psi_ctrl_new_pccch();
          psc_db->network_pg_mode = psi3ter->page_mode;
#endif
          break;
        case ACQ_COMP_OK:
          psi_stop_psi_reading(NONE);
          /* 
           * Access to the network is allowed
           */
          psi_send_access_enable_if_needed();
          break;
        default:
          TRACE_ERROR("Unexpected acq_ret in PSI3bis");
          break;
      }/*switch check_acq*/
    break;
  }/* switch process_psi3ter*/

}/*psi_handle_psi3ter*/

#endif



/*
+------------------------------------------------------------------------------
| Function    : psi_handle_psi4
+------------------------------------------------------------------------------
| Description : The function handles PSI4 message on both PBCCH and PACCH channels
|
| Parameters  : T_PSI_4*
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_handle_psi4(T_PSI_4 * psi4)
{
  TRACE_FUNCTION("psi_handle_psi4");
  switch(psi_process_psi4(psi4))
  {
    case PSI4_REREAD:
      psi_reread_psi4();
      /*
       * Interference channel list is invalid. We have to inform MEAS-Service
       */
      sig_psi_ctrl_int_list_invalid();
      break;
    case PSI4_INT_LIST_VALID:
      /*
       * Inform MEAS that the Interference parameters are valid
       */
      sig_psi_ctrl_int_list_valid();
      
      /*lint -fallthrough*/

    case PSI4_OK:
      /*
       * check the acquisition state: it is only for having a complete set of information
       */
      switch(psi_check_acq_state())
      {
        case ACQ_RUNNING:
          /*
           * Nothing to do
           */
          break;
        case ACQ_PART_OK:
          psi_stop_psi_reading(NONE);
#ifdef _TARGET_
          TRACE_EVENT("PSI4->NEW_PCCCH");
          sig_psi_ctrl_new_pccch();
#if defined (REL99) AND defined (TI_PS_FF_EMR)
          psc_db->network_pg_mode = psi4->page_mode;
#endif
#endif
          break;
        case ACQ_COMP_OK:
          psi_stop_psi_reading(NONE);
          /* 
           * Access to the network is allowed
           */
          psi_send_access_enable_if_needed();
          break;
        default:
          TRACE_ERROR("Unexpected acq_ret in PSI4");
          break;
      }/*switch check_acq*/
      break;
  }
}/* psi_handle_psi4*/

#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : psi_handle_psi8
+------------------------------------------------------------------------------
| Description : The function handles PSI8 message on both PBCCH and PACCH channels
|
| Parameters  : T_PSI_8*
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_handle_psi8(T_PSI_8 * psi8)
{
  TRACE_FUNCTION("psi_handle_psi8");
  switch(psi_process_psi8(psi8))
  {
    case PSI8_REREAD:
      psi_reread_psi8();
      break;
    case PSI8_OK:
      /*
       * check the acquisition state: it is only for having a complete set of information
       */
      switch(psi_check_acq_state())
      {
        case ACQ_RUNNING:
          /*
           * Nothing to do
           */
          break;
        case ACQ_PART_OK:
          psi_stop_psi_reading(NONE);
#ifdef _TARGET_
          TRACE_EVENT("PSI8->NEW_PCCCH");
          sig_psi_ctrl_new_pccch();
          psc_db->network_pg_mode = psi8->page_mode;
#endif
          break;
        case ACQ_COMP_OK:
          psi_stop_psi_reading(NONE);
          /* 
           * Access to the network is allowed
           */
          psi_send_access_enable_if_needed();
          break;
        default:
          TRACE_ERROR("Unexpected acq_ret in PSI8");
          break;
      }/*switch check_acq*/
      break;
  }
}/* psi_handle_psi8*/
#endif

/*
+------------------------------------------------------------------------------
| Function    : psi_copy_si13_params
+------------------------------------------------------------------------------
| Description : The function copies si13 parameters into sc_db
|
| Parameters  : T_D_SYS_INFO_13*
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_copy_si13_params(T_D_SYS_INFO_13* si13)
{ 
  TRACE_FUNCTION("psi_copy_si13_params");

  /* 
   * Copy SI13_CHANGE_MARK and GPRS Mobile Allocation
   */
  if(si13->si13_rest_oct.si13_info.flag)
  {
    psc_db->psi13_params.si13_change_mark = si13->si13_rest_oct.si13_info.si13_cm;
    psi_copy_ma_from_psi13((T_gprs_ms_alloc_ie*)&(si13->si13_rest_oct.si13_info.gprs_ma));
  }
  
  grr_set_pbcch( si13->si13_rest_oct.si13_info.v_pbcch_des );

  if(si13->si13_rest_oct.si13_info.v_pbcch_des)
  {
    /*
     * PBCCH description present in the SI13 message
     */

    psc_db->psi1_params.psi1_repeat_period = si13->si13_rest_oct.si13_info.psi1_period + 1;
    /* pbcch description in database and in SI13 is different!!!*/
    psc_db->pbcch.pbcch_des.pb = si13->si13_rest_oct.si13_info.pbcch_des.pb;
    psc_db->pbcch.pbcch_des.tsc = si13->si13_rest_oct.si13_info.pbcch_des.tsc;
    psc_db->pbcch.pbcch_des.tn = si13->si13_rest_oct.si13_info.pbcch_des.tn;

    if((si13->si13_rest_oct.si13_info.pbcch_des.flag EQ FALSE) AND (si13->si13_rest_oct.si13_info.pbcch_des.flag2 EQ FALSE))
    {
      psc_db->pbcch.pbcch_des.flag = FALSE;
    }
    else
    {
      psc_db->pbcch.pbcch_des.flag = TRUE;
    }

    psc_db->pbcch.pbcch_des.v_arfcn = si13->si13_rest_oct.si13_info.pbcch_des.v_arfcn;
    psc_db->pbcch.pbcch_des.arfcn = si13->si13_rest_oct.si13_info.pbcch_des.arfcn;
    psc_db->pbcch.pbcch_des.v_maio = si13->si13_rest_oct.si13_info.pbcch_des.v_maio;
    psc_db->pbcch.pbcch_des.maio = si13->si13_rest_oct.si13_info.pbcch_des.maio;
  }
  else
  {
    /*
     * No PBCCH description present in the SI13 message
     */

    /*
     * Copy non PBCCH parameters: RAC, SPGC_CCCH_SUP, PRIORITY_ACCESS_THR,
     * NETWORK_CONTROL_ORDER
     */
    /* Set RAC value */
    psc_db->cell_info_for_gmm.cell_info.cell_env.rai.rac = si13->si13_rest_oct.si13_info.rac;

    psc_db->net_ctrl.spgc_ccch_supp.nw_supp = si13->si13_rest_oct.si13_info.spgc;
    psc_db->net_ctrl.priority_access_thr = si13->si13_rest_oct.si13_info.prio_acc_thr;

    /*
     * Copy NC measurement parameter
     */ 
    {
      T_nc_meas_par nc_meas_par;

      nc_meas_par.ctrl_order = si13->si13_rest_oct.si13_info.nco;
      nc_meas_par.v_nc_meas_per       = FALSE;
      /*
       * SI13 doesnt carry the nc meas parameters. So we should not reset/change the 
       * parameters after receiving every SI13 periodic reading.
       */
      if( (nc_meas_par.ctrl_order EQ NCMEAS_NC2) OR (nc_meas_par.ctrl_order EQ NCMEAS_NC1) )
      {
        if ( (psc_db->nc_cw.param.rep_per_i NEQ NC_REP_PER_I_DEFAULT )  AND 
             (psc_db->nc_cw.param.rep_per_t NEQ NC_REP_PER_T_DEFAULT) AND 
               (psc_db->nc_cw.param.non_drx_per NEQ NC_NON_DRX_PER_DEFAULT) AND
               (psc_db->nc_cw.param.ctrl_order NEQ NC_NC0) AND 
               (psc_db->nc_cw.param.idx NEQ NOT_SET) )
        {
          nc_meas_par.nc_meas_per.non_drx_per = psc_db->nc_cw.param.non_drx_per;
          nc_meas_par.nc_meas_per.rep_per_i   = psc_db->nc_cw.param.rep_per_i ;
          nc_meas_par.nc_meas_per.rep_per_t   = psc_db->nc_cw.param.rep_per_t; 
          nc_meas_par.v_nc_meas_per = TRUE;
        }
      }

      /*
       * Only one instance of SI13, store data in final location.
       */
      grr_prcs_nc_param_struct ( &psc_db->nc_cw.param, &nc_meas_par, 0 );
    }

    /*
     * Copy GPRS cell options
     */
    psc_db->v_gprs_cell_opt = TRUE;
    /*
     * SZML-PSI/001
     */
    memcpy(&(psc_db->gprs_cell_opt), &(si13->si13_rest_oct.si13_info.gprs_cell_opt), sizeof(T_gprs_cell_opt));

    if(si13->si13_rest_oct.si13_info.gprs_cell_opt.nmo EQ GMMRR_NET_MODE_III)
    {
      TRACE_EVENT("NMO III in network ----> GMMRR_NET_MODE_II");
      psc_db->cell_info_for_gmm.cell_info.net_mode =  GMMRR_NET_MODE_II;
    }
    else
    {
      psc_db->cell_info_for_gmm.cell_info.net_mode = si13->si13_rest_oct.si13_info.gprs_cell_opt.nmo;
    }
    

    /*
     * Copy power control parameters
     */
    {
      T_si13_info *si13_info = &si13->si13_rest_oct.si13_info;

      psc_db->v_g_pwr_par = TRUE;

      psc_db->g_pwr_par.alpha           = CLIP_ALPHA( si13_info->alpha );
      psc_db->g_pwr_par.t_avg_w         = CLIP_T_AVG( si13_info->t_avg_w );
      psc_db->g_pwr_par.t_avg_t         = CLIP_T_AVG( si13_info->t_avg_t );
      psc_db->g_pwr_par.pb              = 0;
      psc_db->g_pwr_par.pc_meas_chan    = si13_info->pc_meas_chan;
      psc_db->g_pwr_par.imeas_chan_list = FALSE;
      psc_db->g_pwr_par.n_avg_i         = si13_info->n_avg_i;

      grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param = TRUE;
    }
  }
}/* psi_copy_si13_params */



/*
+------------------------------------------------------------------------------
| Function    : psi_is_pbcch_des_different
+------------------------------------------------------------------------------
| Description : The function checks whether PBCCH description of the received 
|               PSI13 message is different from the stored one
|
| Parameters  : T_PSI_13*; TRUE: If desc. are different FALSE: same
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL psi_is_pbcch_des_different(T_PSI_13* psi13)
{
  TRACE_FUNCTION("psi_is_pbcch_des_different");
  if(!psi13->flag) /* No PBCCH description present: PBCCH no longer available*/
    return TRUE;

  /* compare PBCCH descriptions*/
  if((psc_db->pbcch.pbcch_des.pb EQ psi13->psi1_pbcch_info.pbcch_des.pb) AND
     (psc_db->pbcch.pbcch_des.tsc EQ psi13->psi1_pbcch_info.pbcch_des.tsc) AND
     (psc_db->pbcch.pbcch_des.tn EQ psi13->psi1_pbcch_info.pbcch_des.tn))
  {
    /* compare BCCH carrier-flag, if exists*/
    if(psc_db->pbcch.pbcch_des.flag NEQ psi13->psi1_pbcch_info.pbcch_des.flag)
      return TRUE;

    /* compare non-hopping carrier, if exists: ARFCN*/
    if(
      (psc_db->pbcch.pbcch_des.v_arfcn NEQ psi13->psi1_pbcch_info.pbcch_des.v_arfcn) 
       OR
      (psc_db->pbcch.pbcch_des.arfcn NEQ psi13->psi1_pbcch_info.pbcch_des.arfcn)
      )
        return TRUE;

    /* Compare hopping carrier, if exists: MAIO*/
    if(
      (psc_db->pbcch.pbcch_des.v_maio NEQ psi13->psi1_pbcch_info.pbcch_des.v_maio) 
       OR
      (psc_db->pbcch.pbcch_des.maio NEQ psi13->psi1_pbcch_info.pbcch_des.maio)
      )
        return TRUE;
  }
  else
  {
    return TRUE;
  }
  return FALSE;
}/* psi_is_pbcch_des_different*/

/*
+------------------------------------------------------------------------------
| Function    : psi_copy_psi13_params
+------------------------------------------------------------------------------
| Description : The function copies psi13 parameters into sc_db
|
| Parameters  : T_PSI_13*
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_copy_psi13_params(T_PSI_13* psi13)
{
  TRACE_FUNCTION("psi_copy_psi13_params");

  /* 
   * Handle SI13_CHANGE_MARK and GPRS Mobile Allocation
   */
  if(psi13->v_si13_cm_gprs_alloc)
  {
    psc_db->psi13_params.si13_change_mark = psi13->si13_cm_gprs_alloc.si13_cm;
    psi_copy_ma_from_psi13(&(psi13->si13_cm_gprs_alloc.gprs_ms_alloc_ie));
  }

  grr_set_pbcch( psi13->flag );

  if(psi13->flag)
  { 
    /*
     * PBCCH description present in the PSI13 message
     */

    psc_db->psi1_params.psi1_repeat_period = psi13->psi1_pbcch_info.psi1_rep_per + 1;
    memcpy(&(psc_db->pbcch.pbcch_des), &(psi13->psi1_pbcch_info.pbcch_des), sizeof(T_pbcch_des));
  }
  else
  { 
    /* 
     * No PBCCH description present in the PSI13 message
     */

    /*
     * Copy non PBCCH parameters: RAC, SPGC_CCCH_SUP, PRIORITY_ACCESS_THR
     */
    /* Set RAC value */
    psc_db->cell_info_for_gmm.cell_info.cell_env.rai.rac = psi13->pbcch_n_pres.rac;

    psc_db->net_ctrl.spgc_ccch_supp.nw_supp = psi13->pbcch_n_pres.spgc_ccch_sup;
    psc_db->net_ctrl.priority_access_thr = psi13->pbcch_n_pres.prio_acc_thr;

    /*
     * Copy NC measurement parameter
     */ 
    {
      T_nc_meas_par nc_meas_par;

      nc_meas_par.ctrl_order = psi13->pbcch_n_pres.ctrl_order;
      nc_meas_par.v_nc_meas_per       = FALSE;

      /*
       * Only one instance of PSI13, store data in final location.
       */
      grr_prcs_nc_param_struct ( &psc_db->nc_cw.param, &nc_meas_par, 0 );
    }

    /*
     * Copy GPRS cell options
     */
    psc_db->v_gprs_cell_opt = TRUE;
    /*
     * SZML-PSI/002
     */
    memcpy(&(psc_db->gprs_cell_opt), &(psi13->pbcch_n_pres.gprs_cell_opt), sizeof(T_gprs_cell_opt));
    psc_db->cell_info_for_gmm.cell_info.net_mode = psi13->pbcch_n_pres.gprs_cell_opt.nmo;
  
    /* 
     * Copy power control parameters
     */
    {
      T_psi13_pwr_par *pwr_par = &psi13->pbcch_n_pres.psi13_pwr_par;

      psc_db->v_g_pwr_par = TRUE;
    
      psc_db->g_pwr_par.alpha           = CLIP_ALPHA( pwr_par->alpha);
      psc_db->g_pwr_par.t_avg_w         = CLIP_T_AVG( pwr_par->t_avg_w );
      psc_db->g_pwr_par.t_avg_t         = CLIP_T_AVG( pwr_par->t_avg_t );
      psc_db->g_pwr_par.pb              = 0;
      psc_db->g_pwr_par.pc_meas_chan    = pwr_par->pc_meas_chan;
      psc_db->g_pwr_par.imeas_chan_list = FALSE;
      psc_db->g_pwr_par.n_avg_i         = pwr_par->n_avg_i;

      grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param = TRUE;
    }
  }
} /* psi_copy_psi13_params */

/*
+------------------------------------------------------------------------------
| Function    : psi_send_access_enable_if_needed
+------------------------------------------------------------------------------
| Description : The function sends signal to CTRL to indicate that the access 
|               is enabled
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_send_access_enable_if_needed()
{
  TRACE_FUNCTION("psi_send_access_enable_if_needed");

  if(!psc_db->is_access_enabled)
  {
    psc_db->is_access_enabled = TRUE;

    sig_psi_ctrl_access_enabled( );
  }
#ifdef _TARGET_
  else if(grr_is_pbcch_present())
  {
    /*
     * Inform CTRL about new pccch, if there is a new one exists. 
     * It may be the PBCCH and/or PCCCH description has been
     * changed, so that PCCCH reading should be started again.
     * If not this is not a big problem. This is done to avoid 
     * saving PCCCH description in database to compare whether
     * it has been changed or not.
     */
    TRACE_EVENT("Access enabled, PCCCH may have changed");
    sig_psi_ctrl_new_pccch();
  }
#endif /* _TARGET_ */

#ifdef REL99
  TRACE_EVENT_P2 ("pbbch %d, cbch_info %d", grr_is_pbcch_present(), psc_db->send_cbch_info_ind);
  if(grr_is_pbcch_present() AND psc_db->send_cbch_info_ind)
  {
    sig_psi_ctrl_cbch_info_ind();
    psc_db->send_cbch_info_ind = FALSE;
  }
#endif

}/*psi_send_access_enable_if_needed */

/*
+------------------------------------------------------------------------------
| Function    : psi_initiate_read_complete_si
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_initiate_read_complete_si ( void )
{
  TRACE_FUNCTION( "psi_initiate_read_complete_si" );

  psi_stop_60sec( TRUE );
  psi_stop_30sec( TRUE );
  sig_psi_ctrl_access_disabled( PSI_DC_OTHER );

} /* psi_initiate_read_complete_si */

/*
+------------------------------------------------------------------------------
| Function    : psi_initiate_read_complete_psi
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_initiate_read_complete_psi ( UBYTE init_needed )
{
  TRACE_FUNCTION( "psi_initiate_read_complete_psi" );

  TRACE_EVENT( "PBCCH present" );

  psi_stop_60sec( TRUE );
  psi_stop_30sec( TRUE );
  psi_update_data_to_request( init_needed );
  sig_psi_ctrl_access_disabled( PSI_DC_READ_PSI );

} /* psi_initiate_read_complete_psi */

/*
+------------------------------------------------------------------------------
| Function    : psi_initiate_pbcch_switching
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_initiate_pbcch_switching ( T_PSI_DISABLE_CAUSE dc )
{
  TRACE_FUNCTION( "psi_initiate_pbcch_switching" );

  if( psc_db->acq_type NEQ NONE )
  {
    psi_stop_psi_reading(NONE);
  }

  sig_psi_ctrl_access_disabled( dc );
  psi_stop_timer();
  psi_init_params();      
  grr_set_pbcch( FALSE );

} /* psi_initiate_pbcch_switching */

/*
+------------------------------------------------------------------------------
| Function    : psi_store_ncell_param
+------------------------------------------------------------------------------
| Description : 
| Parameters  : 
+------------------------------------------------------------------------------
*/
LOCAL UBYTE psi_store_ncell_param ( T_ncell_par *p_ncell_par, 
                                    UBYTE        c_ncell_par,
                                    UBYTE        v_ncell_par,
                                    T_INFO_TYPE  type,
                                    UBYTE        instance )
{
  UBYTE  i, j;
  UBYTE  number    = 0;
  USHORT last_freq;

  TRACE_FUNCTION( "psi_store_ncell_param" );

  if( !v_ncell_par )
  {
    switch ( type )
    {
      case( INFO_TYPE_PSI3 ):
        TRACE_EVENT( "No ncell parameters in PSI3" );                  
        break;
      
      case( INFO_TYPE_PSI3BIS ): 
        TRACE_EVENT( "No ncell parameters in PSI3BIS" );               
        break;
      
      default:
        TRACE_EVENT( "Unexpected neighbour cell information source" ); 
        break;
    }

    return( number ); /* no ncell parameters */
  }

  switch ( type )
  {
    case( INFO_TYPE_PSI3 ):
    case( INFO_TYPE_PSI3BIS ): 
      break;
      
    default:
      TRACE_EVENT( "Unexpected neighbour cell information source" ); 
      return( number );
  }

  for( i = 0; i < c_ncell_par; i++ )
  {
    last_freq = 0;

    if( grr_store_cs_param
          ( &psc_db->nc_cw.list,
            TRUE,
            &p_ncell_par[i].cs_par,
            type,
            instance,
            &number,
            &last_freq,
            p_ncell_par[i].start_freq,
            p_ncell_par[i].bsic ) EQ FALSE )
    {
      TRACE_EVENT( "psi_store_ncell_param: NC_CW list full" );

      return( number );
    }

    for( j = 0; j < p_ncell_par[i].n_rest; j++ )
    {
      if( grr_store_cs_param
            ( &psc_db->nc_cw.list,
              TRUE,
              &p_ncell_par[i].ncell_par_rest[j].cs_par,
              type,
              instance,
              &number, 
              &last_freq,
              p_ncell_par[i].ncell_par_rest[j].freq_diff_struct.freq_diff,
              p_ncell_par[i].ncell_par_rest[j].bsic ) EQ FALSE )
      {
        TRACE_EVENT( "psi_store_ncell_param: NC_CW list full" );

        return( number );
      }
    }
  }

  return( number );
}/* psi_store_ncell_param */

/*
+------------------------------------------------------------------------------
| Function    : psi_store_ncell_param2
+------------------------------------------------------------------------------
| Description : 
| Parameters  : 
+------------------------------------------------------------------------------
*/
LOCAL void psi_store_ncell_param2 ( T_PSI_3_BIS *psi3bis, UBYTE number )
{
  T_ncell_par2_s1 *p_sub_1;
  T_ncell_par2_s2 *p_sub_2;
 
  USHORT last_freq;
  UBYTE  i, j, cell_params_ptr, x, y, n;
  
  TRACE_FUNCTION( "psi_store_ncell_param2" );

  if( !psi3bis->ncell_par_trnc_grp.v_ncell_par2 )
  {
    return;
  }

  for( i = 0;
       i < psi3bis->ncell_par_trnc_grp.c_ncell_par2;
       i++ )
  {
    for( n = 0;
         n < psi3bis->ncell_par_trnc_grp.ncell_par2[i].c_ncell_par2_des;
         n++ )
    {
      cell_params_ptr = psi3bis->ncell_par_trnc_grp.ncell_par2[i].ncell_par2_des[n].para_ptr;

      if( cell_params_ptr > psi3bis->ncell_par_trnc_grp.ncell_par2[i].c_ncell_par2_set )
      {
        TRACE_ERROR( "cell_params_ptr > psi3bis->ncell_par2[i].c_ncell_par2_set" );

        return;
      }

      for( j = 0; 
           j < psi3bis->ncell_par_trnc_grp.ncell_par2[i].ncell_par2_des[n].c_ncell_par2_s1;
           j++ )
      {
        last_freq = 0;
        p_sub_1   = &psi3bis->ncell_par_trnc_grp.ncell_par2[i].ncell_par2_des[n].ncell_par2_s1[j];

        if( psi_store_cs_param2
              ( &psc_db->nc_cw.list, 
                &psi3bis->ncell_par_trnc_grp.ncell_par2[i].ncell_par2_set[cell_params_ptr],
                psi3bis->psi3bis_ind, 
                &number,
                &last_freq,
                p_sub_1->start_freq,
                p_sub_1->same_ra_scell,
                p_sub_1->cell_ba,
                p_sub_1->bcc ) EQ FALSE )
        {
          TRACE_EVENT( "psi_store_ncell_param2: NC_CW list full" );
  
          return;
        }

        if( p_sub_1->n_r_cells NEQ 0 )
        {
          y = p_sub_1->c_ncell_par2_s2;

          for( x = 0; x < y; x++ )
          {
            p_sub_2 = &p_sub_1->ncell_par2_s2[x];

            if( psi_store_cs_param2
                  ( &psc_db->nc_cw.list, 
                    &psi3bis->ncell_par_trnc_grp.ncell_par2[i].ncell_par2_set[cell_params_ptr],
                    psi3bis->psi3bis_ind,
                    &number,
                    &last_freq,
                    p_sub_2->freq_diff_struct.freq_diff,
                    p_sub_2->same_ra_scell,
                    p_sub_2->cell_ba,
                    p_sub_2->bcc ) EQ FALSE )
            {
              TRACE_EVENT( "psi_store_ncell_param2: NC_CW list full" );
  
              return;
            }
          }
        }
      }
    }
  }
} /* psi_store_ncell_param2 */

/*
+------------------------------------------------------------------------------
| Function    : psi_store_cs_param2
+------------------------------------------------------------------------------
| Description : 
| Parameters  : 
+------------------------------------------------------------------------------
*/
LOCAL BOOL psi_store_cs_param2 ( T_NC_LIST        *nc_list,
                                 T_ncell_par2_set *cs_par2,
                                 UBYTE             instance,
                                 UBYTE            *number,
                                 USHORT           *freq,
                                 USHORT            freq_diff,
                                 UBYTE             same_ra_scell,
                                 UBYTE             cell_ba,
                                 UBYTE             bcc )
{
  T_INFO_SRC    info_src = 0;
  T_ncell_info *ncell_info;

  TRACE_FUNCTION( "psi_store_cs_param2" );

  NC_SET_TYPE    ( info_src, INFO_TYPE_PSI3BIS );
  NC_SET_INSTANCE( info_src, instance          );
  NC_SET_NUMBER  ( info_src, *number           );

  ncell_info = grr_get_next_ncell_param( MAX_NR_OF_NCELL,
                                         nc_list,
                                         info_src );

  if( ncell_info EQ NULL )
  {
    return( FALSE );
  }

  number++;
  nc_list->number++;

  *freq = ( *freq + freq_diff ) % 1024;

  ncell_info->info_src = info_src;
  ncell_info->arfcn    = *freq;

  if( cs_par2->v_ncc )
  {
    ncell_info->bsic = bcc | ( cs_par2->ncc << 3 );
  }
  else
  {
    ncell_info->bsic = bcc | ( psc_db->pbcch.bcch.bsic & BSIC_NCC_MASK );
  }

  ncell_info->v_cr_par             = TRUE;
  ncell_info->cr_par.same_ra_scell = same_ra_scell;
  ncell_info->cr_par.exc_acc       = cs_par2->exc_acc;
  ncell_info->cr_par.cell_ba       = cell_ba;

#if !defined (NTRACE)
  if( grr_data->cs.v_crp_trace EQ TRUE )
  {
    TRACE_EVENT_P2( "psi_store_cs_param2: cell barred status %d %d",
                    ncell_info->arfcn, ncell_info->cr_par.cell_ba );
  }
#endif /* #if !defined (NTRACE) */



  SET_GPRS_RXLEV_ACCESS_MIN
    ( ncell_info->cr_par.cr_par_1.cr_pow_par.gprs_rxlev_access_min,
      cs_par2->v_gprs_rxlev_access_min, cs_par2->gprs_rxlev_access_min );

  SET_GPRS_MS_TXPWR_MAX_CCH
    ( ncell_info->cr_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch,
      cs_par2->v_txpwr_max_cch, cs_par2->txpwr_max_cch );
      
  SET_GPRS_TEMP_OFFSET
    ( ncell_info->cr_par.cr_offset.gprs_temp_offset,
      TRUE, cs_par2->gprs_temp_offset );

  SET_GPRS_PENALTY_TIME
    ( ncell_info->cr_par.cr_offset.gprs_penalty_time,
      TRUE, cs_par2->gprs_penalty_time );

  SET_GPRS_RESEL_OFF
    ( ncell_info->cr_par.gprs_resel_off,
      TRUE, cs_par2->gprs_resel_off );

  ncell_info->cr_par.cr_par_1.v_hcs_par = 
    ( cs_par2->v_gprs_hcs_thr AND cs_par2->v_gprs_prio_class );

  SET_GPRS_HCS_THR
    ( ncell_info->cr_par.cr_par_1.hcs_par.gprs_hcs_thr,
      cs_par2->v_gprs_hcs_thr, cs_par2->gprs_hcs_thr );

  SET_GPRS_PRIO_CLASS
    ( ncell_info->cr_par.cr_par_1.hcs_par.gprs_prio_class,
      cs_par2->v_gprs_prio_class, cs_par2->gprs_prio_class );

  grr_store_si13_pbcch_location( &ncell_info->cr_par,
                                 cs_par2->v_si13_pbcch,
                                 &cs_par2->si13_pbcch );

  return( TRUE );

}/* psi_store_cs_param2 */

/*
+------------------------------------------------------------------------------
| Function    : psi_restore_ncell_param
+------------------------------------------------------------------------------
| Description : 
| Parameters  : 
+------------------------------------------------------------------------------
*/
LOCAL void psi_restore_ncell_param ( void )
{
  UBYTE         idx_curr   = 0;
  T_ncell_info *info_prev  = NULL;
  T_ncell_info *info_curr;
  T_INFO_SRC    info_src   = 0;
  
  TRACE_FUNCTION( "psi_restore_ncell_param" );

  /* update change mark value */
  psc_db->nc_cw.list.chng_mrk.curr++;

  while( ( info_curr = grr_get_next_bigger_ncell_param
                                 ( &psc_db->nc_cw.list, info_src ) ) NEQ NULL )
  {
    grr_restore_cs_param( info_curr, info_prev, idx_curr );

    info_src  = info_curr->info_src;
    info_prev = info_curr;
    
    idx_curr++;
  }
}/* psi_restore_ncell_param */


/*
+------------------------------------------------------------------------------
| Function    : psi_prepare_scell_pbcch_req
+------------------------------------------------------------------------------
| Description : The function psi_prepare_scell_pbcch_req() prepares the mphp_
|               scell_pbcch_req
|
| Parameters  : T_MPHP_SCELL_PBCCH_REQ *mphp_scell_pbcch_req
|                                        UBYTE  psi_reading_type
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_prepare_scell_pbcch_req(T_MPHP_SCELL_PBCCH_REQ *mphp_scell_pbcch_req, 
                                        UBYTE  psi_reading_type)
{ 
#ifndef _TARGET_

  UBYTE i, j;

#endif /* #ifndef _TARGET_ */

  UBYTE hr, lr, *psi_nr;
  UBYTE *pos_array;
  
  TRACE_FUNCTION( "psi_prepare_scell_pbcch_req" );


  memcpy(mphp_scell_pbcch_req,&psc_db->scell_pbcch,sizeof(T_MPHP_SCELL_PBCCH_REQ));

  psi_nr = &mphp_scell_pbcch_req->psi_nr;
  pos_array = mphp_scell_pbcch_req->relative_pos_array;

  *psi_nr = 0;
  memset(pos_array, 0, MAX_RELATIVE_POS);

  hr = psc_db->psi1_params.psi_cnt_hr;
  lr = psc_db->psi1_params.psi_cnt_lr;


  TRACE_EVENT_P1("psi_reading_type:%d", psi_reading_type);

  switch(psi_reading_type)
  {
    case READ_COMPLETE: 
      /* attempt to receive messages shall be made each time 
       * the message is scheduled on the broadcast channel.
       */
      mphp_scell_pbcch_req->psi_nr = 0;/*read all PSI in all PBCCH*/
      /*
       * mphp_scell_pbcch_req->relative_pos_array has no meaning, 
       * if we want to receive all PSI
       */
      psi_reset_psi_pos();
      break;
    case READ_PSI1_IN_PSI1_REPEAT_PERIODS:
      /* attempt to receive PSI1 each time 
       * the message is scheduled on the broadcast channel 
       * with psi1_repeat_periods (only once in a multiframe, e.g. in PBCCH->B0)
       */

      /* mphp_scell_pbcch_req->relative_pos_array will contain only one element,
       * because of mphp_scell_pbcch_req.psi_nr = 1
       */        
      mphp_scell_pbcch_req->psi_nr = 1;
      
      /*Read PSI1 only in BO in psi1 repeat period
       */
      mphp_scell_pbcch_req->relative_pos_array[0] = 0; 
      break;
    case PSI_IN_HR: /* PSI in high repetition rate*/
      if(!hr) /* no HR */
      {
        TRACE_ERROR("no HR, read all");
        mphp_scell_pbcch_req->psi_nr = 0;/*read all PSI in all PBCCH*/
      }
      else
      {
#ifdef _TARGET_
        mphp_scell_pbcch_req->psi_nr = 0;
#else
        mphp_scell_pbcch_req->psi_nr = 0;
        i = (mphp_scell_pbcch_req->bs_pbcch_blks > 0) ? 1 : 0;

        while(hr > 0)
        {
          mphp_scell_pbcch_req->relative_pos_array[mphp_scell_pbcch_req->psi_nr] = i + hr;
          if(mphp_scell_pbcch_req->psi_nr EQ 19)
            break;/* break condition*/
          mphp_scell_pbcch_req->psi_nr++;
          hr--;
        }
#endif
      }
      break;
    case PSI_IN_LR:  /* PSI in low repetition rate */
      if(!lr) /* no LR */
      {
        TRACE_ERROR("no LR, read all");
        mphp_scell_pbcch_req->psi_nr = 0;/*read all PSI in all PBCCH*/
      }
      else
      {
#ifdef _TARGET_
        mphp_scell_pbcch_req->psi_nr = 0;
#else
        mphp_scell_pbcch_req->psi_nr = 0;
        i = (mphp_scell_pbcch_req->bs_pbcch_blks > 0) ? 1 : 0;

        while(lr > 0)
        {
          mphp_scell_pbcch_req->relative_pos_array[mphp_scell_pbcch_req->psi_nr] = i + hr + lr;
          if(mphp_scell_pbcch_req->psi_nr EQ 19)
            break;/* break condition*/
          mphp_scell_pbcch_req->psi_nr++;
          lr--;
        }
#endif
      }
      break;
    case PSI_IN_HR_AND_LR:   /* PSI in high and low repetition rate*/
      if(!(lr + hr)) /* no LR+HR */
      {
        TRACE_ERROR("no HR+LR, read all");
        mphp_scell_pbcch_req->psi_nr = 0;/*read all PSI in all PBCCH*/
      }
      else
      {
#ifdef _TARGET_
        mphp_scell_pbcch_req->psi_nr = 0;
#else
        mphp_scell_pbcch_req->psi_nr = 0;
        i = (mphp_scell_pbcch_req->bs_pbcch_blks > 0) ? 2 : 1;
        for(j=0; j<(lr + hr);j++)
        {
          mphp_scell_pbcch_req->relative_pos_array[mphp_scell_pbcch_req->psi_nr] = i + j;
          if(mphp_scell_pbcch_req->psi_nr EQ 19)
            break;/* break condition*/
          mphp_scell_pbcch_req->psi_nr++;        
        }
#endif
      }
      break;
    case   READ_PSI1_AND_IN_HR:
      /* PSI in high repetition rate and PSI1*/
#ifdef _TARGET_
      mphp_scell_pbcch_req->psi_nr = 0;
#else
      mphp_scell_pbcch_req->psi_nr = 0;
      mphp_scell_pbcch_req->relative_pos_array[mphp_scell_pbcch_req->psi_nr++] = 0;
      i = (mphp_scell_pbcch_req->bs_pbcch_blks > 0) ? 1 : 0;
      if(i EQ 1)
      {
        mphp_scell_pbcch_req->relative_pos_array[mphp_scell_pbcch_req->psi_nr++] = 1;        
      }

      while(hr > 0)
      {
        mphp_scell_pbcch_req->relative_pos_array[mphp_scell_pbcch_req->psi_nr] = i + hr;
        if(mphp_scell_pbcch_req->psi_nr EQ 19)
          break;/* break condition*/
        mphp_scell_pbcch_req->psi_nr++;
        hr--;
      }
#endif
      break;
    case   READ_PSI1_AND_IN_LR:/* PSI in low repetition rate and PSI1*/
#ifdef _TARGET_
        mphp_scell_pbcch_req->psi_nr = 0;
#else
      mphp_scell_pbcch_req->psi_nr = 0;
      mphp_scell_pbcch_req->relative_pos_array[mphp_scell_pbcch_req->psi_nr++] = 0;
      i = (mphp_scell_pbcch_req->bs_pbcch_blks > 0) ? 1 : 0;
      if(i EQ 1)
      {
        mphp_scell_pbcch_req->relative_pos_array[mphp_scell_pbcch_req->psi_nr++] = 1;        
      }
      
      while(lr > 0)
      {
        mphp_scell_pbcch_req->relative_pos_array[mphp_scell_pbcch_req->psi_nr] = i + hr + lr;
        if(mphp_scell_pbcch_req->psi_nr EQ 19)
          break;/* break condition*/
        mphp_scell_pbcch_req->psi_nr++;
        lr--;
      }
#endif
      break;
    case  READ_PSI2:
#ifdef _TARGET_
      mphp_scell_pbcch_req->psi_nr = 0;
#else
      psi_fill_rel_pos(grr_data->psi.psi2_pos, MAX_NR_OF_INSTANCES_OF_PSI2, psi_nr, pos_array);
#endif
      break;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    case  READ_PSI3TER:
    case  READ_PSI3_3BIS_3TER:
#endif
    case  READ_PSI3_3BIS:
#if defined (REL99) AND defined (TI_PS_FF_EMR)
    case  READ_PSI3BIS:
#endif
#ifdef _TARGET_
      mphp_scell_pbcch_req->psi_nr = 0;
#else
#if defined (REL99) AND defined (TI_PS_FF_EMR)
      psi_fill_rel_pos(grr_data->psi.psi3_set_pos, 
      MAX_NR_OF_INSTANCES_OF_PSI3TER + MAX_NR_OF_INSTANCES_OF_PSI3 + 
      MAX_NR_OF_INSTANCES_OF_PSI3BIS, psi_nr, pos_array);
#else
      psi_fill_rel_pos(grr_data->psi.psi3bis_pos, 
      MAX_NR_OF_INSTANCES_OF_PSI3 + MAX_NR_OF_INSTANCES_OF_PSI3BIS, 
      psi_nr, pos_array);
#endif /* #ifdef REL99 AND TI_PS_FF_EMR */
#endif /* ifdef _TARGET_ */
      break;
    case  READ_PSI4:
#ifdef _TARGET_
      mphp_scell_pbcch_req->psi_nr = 0;
#else
      psi_fill_rel_pos(grr_data->psi.psi4_pos, MAX_NR_OF_INSTANCES_OF_PSI4, psi_nr, pos_array);
#endif
      break;
    case  READ_PSI5:
#ifdef _TARGET_
      mphp_scell_pbcch_req->psi_nr = 0;
#else
      psi_fill_rel_pos(grr_data->psi.psi5_pos, MAX_NR_OF_INSTANCES_OF_PSI5, psi_nr, pos_array);
#endif
      break;
#ifdef REL99
    case  READ_PSI8:
#ifdef _TARGET_
      mphp_scell_pbcch_req->psi_nr = 0;
#else
      psi_fill_rel_pos(grr_data->psi.psi8_pos, MAX_NR_OF_INSTANCES_OF_PSI8, psi_nr, pos_array);
#endif
      break;
#endif
    default:
      TRACE_ERROR ("default in psi_prepare_scell_pbcch_req");
      break;
  }
  TRACE_EVENT_P7("psi_nr:%d blks:%d pb:%d per:%d pos[0]:%d pos[1]:%d pos[2]:%d",
          mphp_scell_pbcch_req->psi_nr,
          mphp_scell_pbcch_req->bs_pbcch_blks,
          mphp_scell_pbcch_req->pb,    
          mphp_scell_pbcch_req->psi1_rep_period,
          mphp_scell_pbcch_req->relative_pos_array[0],
          mphp_scell_pbcch_req->relative_pos_array[1],
          mphp_scell_pbcch_req->relative_pos_array[2]);  
  TRACE_EVENT_P4("tn:%d tsc:%d hop:%d arfcn:%d",
          mphp_scell_pbcch_req->p_ch_des.tn,
          mphp_scell_pbcch_req->p_ch_des.tsc,
          mphp_scell_pbcch_req->p_ch_des.p_chan_sel.hopping,
          mphp_scell_pbcch_req->p_ch_des.p_chan_sel.p_rf_ch.arfcn);
  TRACE_EVENT_P5("f_cnt:%d f1:%d f2:%d f3:%d f4:%d",
          mphp_scell_pbcch_req->p_freq_list.p_rf_chan_cnt,
          mphp_scell_pbcch_req->p_freq_list.p_rf_chan_no.p_radio_freq[0],
          mphp_scell_pbcch_req->p_freq_list.p_rf_chan_no.p_radio_freq[1],
          mphp_scell_pbcch_req->p_freq_list.p_rf_chan_no.p_radio_freq[2],
          mphp_scell_pbcch_req->p_freq_list.p_rf_chan_no.p_radio_freq[3]);    




} /* psi_prepare_scell_pbcch_req */


/*
+------------------------------------------------------------------------------
| Function    : psi_store_rel_pos 
+------------------------------------------------------------------------------
| Description : The function psi_store_rel_pos () .stores the values relative
|               position for each PSI message
| Parameters  : dest: destination array; rel_pos: relative position; max_dest:
|               MAX size OF dest array
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_store_rel_pos(UBYTE *dest, UBYTE rel_pos, UBYTE max_dest)
{
  UBYTE i;
  TRACE_FUNCTION("psi_store_rel_pos");
  
  for ( i = 0; i < max_dest; i++)
  {
    if(dest[i] EQ 0xFF)
    {
      dest[i] = rel_pos;
      break;
    }
  }

}/*psi_store_rel_pos*/

#ifndef _TARGET_
/*
+------------------------------------------------------------------------------
| Function    : psi_fill_rel_pos 
+------------------------------------------------------------------------------
| Description : The function psi_fill_rel_pos () .fills the relative position
|               array and sets the psi_number
| Parameters  : src: array containing rel positions; max_src: MAX size of src
|               psi_nr: ptr to psi_number; pos_array: ptr to position_array
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_fill_rel_pos(UBYTE *src, UBYTE max_src, UBYTE *psi_nr, UBYTE *pos_array) 
{
  UBYTE i;
  TRACE_FUNCTION("psi_fill_rel_pos");

  for(i = 0; i<max_src; i++)
  {
    if(src[i] NEQ 0xff)
    {
      pos_array[(*psi_nr)] = src[i];
      (*psi_nr)++;
    }
  }
}/*psi_fill_rel_pos*/
#endif /*_TARGET_*/

/*
+------------------------------------------------------------------------------
| Function    : psi_reset_psi_pos 
+------------------------------------------------------------------------------
| Description : The function spsi_reset_psi_pos () .... 
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_reset_psi_pos  ( void )
{ 
  TRACE_FUNCTION( "psi_reset_psi_pos " );
  
  memset(grr_data->psi.psi2_pos, 0xFF, MAX_NR_OF_INSTANCES_OF_PSI2);

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  memset(grr_data->psi.psi3_set_pos, 0xFF, MAX_NR_OF_INSTANCES_OF_PSI3TER
    +MAX_NR_OF_INSTANCES_OF_PSI3BIS+MAX_NR_OF_INSTANCES_OF_PSI3);
#else
  memset(grr_data->psi.psi3bis_pos, 0xFF, MAX_NR_OF_INSTANCES_OF_PSI3BIS+MAX_NR_OF_INSTANCES_OF_PSI3);
#endif

  memset(grr_data->psi.psi4_pos, 0xFF, MAX_NR_OF_INSTANCES_OF_PSI4);
  memset(grr_data->psi.psi5_pos, 0xFF, MAX_NR_OF_INSTANCES_OF_PSI5);
#ifdef REL99
  memset(grr_data->psi.psi8_pos, 0xFF, MAX_NR_OF_INSTANCES_OF_PSI8);
#endif

} /* psi_reset_psi_pos () */


#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : psi_update_bss_sgsn_rel 
+------------------------------------------------------------------------------
| Description : The function psi_update_bss_sgsn_rel () updates BSS and SGSN
|               releases during the processing of SI13 message.
|
| Parameters  : SI13 message, status of PBCCH presence in the cell
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_update_bss_sgsn_rel  ( T_D_SYS_INFO_13  *si13, 
                                      BOOL              pbcch_status )
{ 
  TRACE_FUNCTION( "psi_update_bss_sgsn_rel " );

  /* Update the BSS release */  
  if (si13->si13_rest_oct.si13_info.v_sgsnr)
  {
    psc_db->network_rel = BSS_NW_REL_99;
    psc_db->sgsn_rel = 
      si13->si13_rest_oct.si13_info.sgsnr ? PS_SGSN_99_ONWARDS : PS_SGSN_98_OLDER;
  }
  else
  {
    psc_db->network_rel = BSS_NW_REL_97;
    
    if(pbcch_status EQ PBCCH_NOT_PRESENT)
    {
      psc_db->sgsn_rel = PS_SGSN_98_OLDER;
    }
    else
    {
      psc_db->sgsn_rel = PS_SGSN_UNKNOWN;
    }

  }

  /* Update the SGSN release in the Common library context */
  cl_nwrl_set_sgsn_release(psc_db->sgsn_rel);

  
} /* psi_update_bss_sgsn_rel () */

#endif

