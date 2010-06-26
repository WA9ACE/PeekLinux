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
|  Purpose :  This module implements primitive handler functions for service
|             GFP of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_GFPP_C
#define GRR_GFPP_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */

#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grr.h"        /* to get the global entity definitions */
#include "grr_f.h"      /* to get the global entity procedure definitions */
#include "grr_gfpf.h"   
#include "grr_pgs.h"   
#include "grr_psis.h"   
#include "grr_css.h"   
#include "grr_meass.h"   
#include "grr_measf.h"
#include "grr_tcs.h"   
#include "grr_ctrls.h"
#include "cl_rlcmac.h"

/*==== CONST ================================================================*/

/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/
LOCAL void gfp_adapt_fn ( ULONG fn );
/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : gfp_adapt_fn()
+------------------------------------------------------------------------------
| Description : The function gfp_adapt_fn() calculates the received frame 
|               number and the first framenumber of the current radio block
|
| Parameters  : fn - frame number, where the block is passed to layer23
|
+------------------------------------------------------------------------------
*/
LOCAL void gfp_adapt_fn ( ULONG fn )
{ 
  TRACE_FUNCTION( "gfp_adapt_fn" );

  /*
   * The received frame number is always 4 frames before fn.
   */


  grr_data->dl_fn = fn-4;
  /* 
   * adapte uplink framenumber from downlink framenumber.
   * fn could be an idle frame !!!
   */  

  if(!fn)
  {
    TRACE_EVENT_P1("INVALID FN IN MPHP PRIMITIVE: fn = %ld not alowed",fn);
    grr_data->dl_fn = fn;
  }
  grr_data->ul_fn = fn;
  if((grr_data->ul_fn % 13) EQ 12)
    grr_data->ul_fn++;
  grr_data->ul_fn %= FN_MAX;    

} /* gfp_adapt_fn() */


/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_data_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_DATA_IND
|
| Parameters  : *mphp_data_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_data_ind ( T_MPHP_DATA_IND *mphp_data_ind )
{ 
  TRACE_FUNCTION( "gfp_mphp_data_ind" );


  gfp_adapt_fn(mphp_data_ind->fn);

  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_data_ind);

      if(mphp_data_ind->error_flag) 
      {
        if(mphp_data_ind->l2_channel EQ L2_PCHANNEL_PPCH)
        {
          if(grr_decrease_dsc())
          {
            /*
             * Inform GRR
             */
            sig_gfp_ctrl_dsf_ind();
          }
        }
/*
        TRACE_ERROR( "MPHP_DATA_IND with error_flag(invalid block)" );
*/

        TRACE_BINDUMP
          ( hCommGRR, TC_USER5, 
            cl_rlcmac_get_msg_name( D_MSG_TYPE_CRC_ERROR_c, RLC_MAC_ROUTE_DL ),
            mphp_data_ind->l2_frame, MAX_L2_FRAME_SIZE ); /*lint !e569*/
      }
      else
      {
        if(mphp_data_ind->l2_channel EQ L2_PCHANNEL_PPCH)
        {
          sig_gfp_meas_rxlev_pccch_ind( mphp_data_ind->pccch_lev );      
          grr_increase_dsc();
        }
        gfp_prcs_ctrl_msg( &mphp_data_ind->l2_frame[0], 
                            mphp_data_ind->l2_channel,
                            mphp_data_ind->relative_pos,
                            mphp_data_ind->fn - 4 );
      }
      break;
    default:
      TRACE_ERROR( "MPHP_DATA_IND unexpected" );
      /* 
       * SZML-SGLBL/008
       */
      break;
  }
  PFREE(mphp_data_ind);


} /* gfp_mphp_data_ind() */

/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_scell_pbcch_stop_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_SCELL_PBCCH_STOP_CON
|
| Parameters  : *mphp_scell_pbcch_stop_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_scell_pbcch_stop_con ( 
                       T_MPHP_SCELL_PBCCH_STOP_CON *mphp_scell_pbcch_stop_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_scell_pbcch_stop_con" );
  
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_scell_pbcch_stop_con);
      sig_gfp_psi_scell_pbcch_stop_con(mphp_scell_pbcch_stop_con);
      break;
    default:
      TRACE_ERROR( "MPHP_SCELL_PBCCH_STOP_CON unexpected" );
      break;
  }
  PFREE(mphp_scell_pbcch_stop_con);

} /* gfp_mphp_scell_pbcch_stop_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_stop_pccch_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_STOP_PCCCH_CON
|
| Parameters  : *mphp_stop_pccch_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_stop_pccch_con ( T_MPHP_STOP_PCCCH_CON *mphp_stop_pccch_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_stop_pccch_con" );
  
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_stop_pccch_con);
      sig_gfp_pg_stop_pccch_con(mphp_stop_pccch_con);
      break;
    default:      
      TRACE_ERROR( "MPHP_STOP_PCCCH_CON unexpected" );
      break;
  }
  PFREE(mphp_stop_pccch_con);

} /* gfp_mphp_stop_pccch_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_cr_meas_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_CR_MEAS_IND
|
| Parameters  : *mphp_cr_meas_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_cr_meas_ind ( T_MPHP_CR_MEAS_IND *mphp_cr_meas_ind )
{ 
  TRACE_FUNCTION( "gfp_mphp_cr_meas_ind" );
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS( mphp_cr_meas_ind );   
      sig_gfp_cs_cr_meas_ind( mphp_cr_meas_ind );
      break;
    default:
      TRACE_ERROR( "MPHP_CR_MEAS_IND unexpected" );
      break;
  }

  PFREE( mphp_cr_meas_ind );
} /* gfp_mphp_cr_meas_ind() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_cr_meas_stop_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_CR_MEAS_STOP_CON
|
| Parameters  : *mphp_cr_meas_stop_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_cr_meas_stop_con ( T_MPHP_CR_MEAS_STOP_CON *mphp_cr_meas_stop_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_cr_meas_stop_con" );
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS( mphp_cr_meas_stop_con );
      sig_gfp_cs_cr_meas_stop_con( mphp_cr_meas_stop_con );
      break;
    default:
      TRACE_ERROR( "MPHP_CR_MEAS_STOP_CON unexpected" );
      break;
  }

  PFREE( mphp_cr_meas_stop_con );
} /* gfp_mphp_cr_meas_stop_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_int_meas_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_INT_MEAS_IND
|
| Parameters  : *mphp_int_meas_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_int_meas_ind ( T_MPHP_INT_MEAS_IND *mphp_int_meas_ind )
{ 
  TRACE_FUNCTION( "gfp_mphp_int_meas_ind" );

  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_int_meas_ind);
      sig_gfp_meas_int_meas_ind(mphp_int_meas_ind);
      break;
    default:
      TRACE_ERROR( "MPHP_INT_MEAS_IND unexpected" );
      break;
  }

  PFREE(mphp_int_meas_ind);
} /* gfp_mphp_int_meas_ind() */

/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_tint_meas_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_TINT_MEAS_IND
|
| Parameters  : *mphp_tint_meas_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_tint_meas_ind ( T_MPHP_TINT_MEAS_IND *mphp_tint_meas_ind )
{ 
  TRACE_FUNCTION( "gfp_mphp_tint_meas_ind" );

  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_tint_meas_ind);
      sig_gfp_meas_int_meas_ind((T_MPHP_INT_MEAS_IND*)mphp_tint_meas_ind);
      break;
    default:
      TRACE_ERROR( "MPHP_TINT_MEAS_IND unexpected" );
      break;
  }

  PFREE(mphp_tint_meas_ind);
} /* gfp_mphp_tint_meas_ind() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_int_meas_stop_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_INT_MEAS_STOP_CON
|
| Parameters  : *mphp_int_meas_stop_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_int_meas_stop_con ( 
                             T_MPHP_INT_MEAS_STOP_CON *mphp_int_meas_stop_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_int_meas_stop_con" );
  
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_int_meas_stop_con);
      sig_gfp_meas_int_meas_stop_con(mphp_int_meas_stop_con);
      break;
    default:
      TRACE_ERROR( "MPHP_INT_MEAS_STOP_CON unexpected" );
      break;
  }

  PFREE(mphp_int_meas_stop_con);
} /* gfp_mphp_int_meas_stop_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_ncell_pbcch_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_NCELL_PBCCH_IND
|
| Parameters  : *mphp_ncell_pbcch_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_ncell_pbcch_ind ( 
                                  T_MPHP_NCELL_PBCCH_IND *mphp_ncell_pbcch_ind )
{ 
  TRACE_FUNCTION( "gfp_mphp_ncell_pbcch_ind" );

  gfp_adapt_fn(mphp_ncell_pbcch_ind->fn);

  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_ncell_pbcch_ind);
      if(mphp_ncell_pbcch_ind->error_flag) 
      {       
        TRACE_ERROR( "MPHP_NCELL_PBCCH_IND with error_flag(invalid block)" );

        TRACE_BINDUMP
          ( hCommGRR, TC_USER5,
            cl_rlcmac_get_msg_name( D_MSG_TYPE_CRC_ERROR_c, RLC_MAC_ROUTE_DL ),
            mphp_ncell_pbcch_ind->l2_frame, MAX_L2_FRAME_SIZE ); /*lint !e569*/
      }
      else
      {
        gfp_prcs_ctrl_msg( &mphp_ncell_pbcch_ind->l2_frame[0], 
                            mphp_ncell_pbcch_ind->l2_channel,
                            mphp_ncell_pbcch_ind->relative_pos,
                            mphp_ncell_pbcch_ind->fn - 4 );
      }
      break;
    default:
      TRACE_ERROR( "MPHP_NCELL_PBCCH_IND unexpected" );
      break;
  }
  PFREE(mphp_ncell_pbcch_ind);

} /* gfp_mphp_ncell_pbcch_ind() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_ra_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_RA_CON
|
| Parameters  : *mphp_ra_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_ra_con ( T_MPHP_RA_CON *mphp_ra_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_ra_con" );

  gfp_adapt_fn(mphp_ra_con->fn);
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_ra_con);
      sig_gfp_tc_ra_con(mphp_ra_con);
      break;
    default:
      TRACE_ERROR( "MPHP_RA_CON unexpected" );
      break;
  }
  PFREE(mphp_ra_con);

} /* gfp_mphp_ra_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_ra_stop_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_RA_STOP_CON
|
| Parameters  : *mphp_ra_stop_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_ra_stop_con ( T_MPHP_RA_STOP_CON *mphp_ra_stop_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_ra_stop_con" );
  
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_ra_stop_con);
      sig_gfp_tc_ra_stop_con(mphp_ra_stop_con);
      break;
    default:
      TRACE_ERROR( "MPHP_RA_STOP_CON unexpected" );
      break;
  }
  PFREE(mphp_ra_stop_con);

} /* gfp_mphp_ra_stop_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_polling_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_POLLING_IND
|
| Parameters  : *mphp_polling_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_polling_ind ( T_MPHP_POLLING_IND *mphp_polling_ind )
{ 
  TRACE_FUNCTION( "gfp_mphp_polling_ind" );
 

  gfp_adapt_fn(mphp_polling_ind->fn);
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_polling_ind);
      /*
       * This Message is not reported to TC, it is not requested
       */
      break;
    default:
      TRACE_ERROR( "MPHP_POLLING_IND unexpected" );
      break;
  }
  PFREE(mphp_polling_ind);

} /* gfp_mphp_polling_ind() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_assignment_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_ASSIGNMENT_CON
|
| Parameters  : *mphp_assignment_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_assignment_con ( T_MPHP_ASSIGNMENT_CON *mphp_assignment_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_assignment_con" );

  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_assignment_con);
      sig_gfp_tc_assignment_con();
      break;
    default:
      TRACE_ERROR( "MPHP_ASSIGNMENT_CON unexpected" );
      break;
  }
      PFREE(mphp_assignment_con);
} /* gfp_mphp_assignment_con() */

/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_repeat_ul_fixed_alloc_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_REPEAT_UL_FIXED_ALLOC_CON
|
| Parameters  : *mphp_repeat_ul_fixed_alloc_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_repeat_ul_fixed_alloc_con ( 
             T_MPHP_REPEAT_UL_FIXED_ALLOC_CON *mphp_repeat_ul_fixed_alloc_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_repeat_ul_fixed_alloc_con" );
  
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_repeat_ul_fixed_alloc_con);
      /*    
       * no signal to tc requested 
       */
      break;
    default:
      TRACE_ERROR( "MPHP_REPEAT_UL_FIXED_ALLOC_CON unexpected" );
      break;
  }
  PFREE(mphp_repeat_ul_fixed_alloc_con);
} /* gfp_mphp_repeat_ul_fixed_alloc_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_single_block_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_SINGLE_BLOCK_CON
|
| Parameters  : *mphp_single_block_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_single_block_con ( 
                               T_MPHP_SINGLE_BLOCK_CON *mphp_single_block_con )
{ 
  BOOL result = FALSE;
  TRACE_FUNCTION( "gfp_mphp_single_block_con" );
  

  TRACE_EVENT_P8("BLK_C purp=%d dl_err=%d last_id=%d rec_id=%d sb_st=%d l2_f[0]=0x%x l2_f[1]=0x%x l2_f[2]=0x%x"
                                ,mphp_single_block_con->purpose
                                ,mphp_single_block_con->dl_error
                                ,grr_data->tc.last_rec_nb_id
                                ,mphp_single_block_con->assign_id
                                ,mphp_single_block_con->sb_status
                                ,mphp_single_block_con->l2_frame[0]
                                ,mphp_single_block_con->l2_frame[1]
                                ,mphp_single_block_con->l2_frame[2]);


  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_single_block_con);
      if ( (mphp_single_block_con->purpose EQ SINGLE_BLOCK_TRANSFER_DL)         AND
           (grr_data->tc.last_rec_nb_id    EQ mphp_single_block_con->assign_id)     )
      {
        if( (!mphp_single_block_con->dl_error)                  AND 
            (mphp_single_block_con->sb_status EQ SB_STAT_DL_OK)     )
        {
          result = gfp_prcs_ctrl_msg( mphp_single_block_con->l2_frame,
                                      L2_PCHANNEL_PACCH,
                                      NOT_PRESENT_8BIT,
                                      grr_data->tc.last_rec_nb_fn );
        }
        else
        {
          TRACE_BINDUMP
            ( hCommGRR, TC_USER5,
              cl_rlcmac_get_msg_name( D_MSG_TYPE_CRC_ERROR_c, RLC_MAC_ROUTE_DL ),
              mphp_single_block_con->l2_frame, MAX_L2_FRAME_SIZE ); /*lint !e569*/
        }
      } 

      if(!result)
      {
        /*
         * no SINGLE_BLOCK_TRANSFER_DL or
         * sb_status = 1 --> invalid single block or
         * no assignment in downlink single block 
         */
        sig_gfp_tc_single_block_cnf(mphp_single_block_con);
      }
      break;
    default:
      TRACE_ERROR( "MPHP_SINGLE_BLOCK_CON unexpected" );
      break;
  }
  PFREE(mphp_single_block_con);
} /* gfp_mphp_single_block_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_tbf_release_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_TBF_RELEASE_CON
|
| Parameters  : *mphp_tbf_release_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_tbf_release_con ( 
                                 T_MPHP_TBF_RELEASE_CON *mphp_tbf_release_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_tbf_release_con" );
  
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_tbf_release_con);
      sig_gfp_tc_tbf_release_con(mphp_tbf_release_con);
      break;
    default:
      TRACE_ERROR( "MPHP_TBF_RELEASE_CON unexpected" );
      break;
  }
  PFREE(mphp_tbf_release_con);

} /* gfp_mphp_tbf_release_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_pdch_release_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_PDCH_RELEASE_CON
|
| Parameters  : *mphp_pdch_release_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_pdch_release_con ( 
                               T_MPHP_PDCH_RELEASE_CON *mphp_pdch_release_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_pdch_release_con" );
  
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_pdch_release_con);
      /*
       * no signal to tc requested
       */
      break;
    default:
      TRACE_ERROR( "MPHP_PDCH_RELEASE_CON unexpected" );
      break;
  }
  PFREE(mphp_pdch_release_con);

} /* gfp_mphp_pdch_release_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_timing_advance_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_TIMING_ADVANCE_CON
|
| Parameters  : *mphp_timing_advance_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_timing_advance_con ( 
                           T_MPHP_TIMING_ADVANCE_CON *mphp_timing_advance_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_timing_advance_con" );
  
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_timing_advance_con);
      /*not passed to other service, not needed*/
      break;
    default:
      TRACE_ERROR( "MPHP_TIMING_ADVANCE_CON unexpected" );
      break;
  }
  PFREE(mphp_timing_advance_con);

} /* gfp_mphp_timing_advance_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_update_psi_param_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_UPDATE_PSI_PARAM_CON
|
| Parameters  : *mphp_update_psi_param_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_update_psi_param_con ( 
                       T_MPHP_UPDATE_PSI_PARAM_CON *mphp_update_psi_param_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_update_psi_param_con" );
  
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_update_psi_param_con);
      /*not passed to other service, not needed*/
      break;
    default:
      TRACE_ERROR( "MPHP_UPDATE_PSI_PARAM_CON unexpected" );
      break;
  }
  PFREE(mphp_update_psi_param_con);

} /* gfp_mphp_update_psi_param_con() */



/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_tcr_meas_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_TCR_MEAS_IND
|
| Parameters  : *mphp_tcr_meas_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_tcr_meas_ind ( T_MPHP_TCR_MEAS_IND *mphp_tcr_meas_ind )
{ 
  TRACE_FUNCTION( "gfp_mphp_tcr_meas_ind" );
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      PACCESS(mphp_tcr_meas_ind);              
      sig_gfp_cs_tcr_meas_ind( mphp_tcr_meas_ind );      
      break;
    default:
      TRACE_ERROR( "MPHP_TCR_MEAS_IND unexpected" );
      break;
  }

  PFREE( mphp_tcr_meas_ind );
} /* gfp_mphp_tcr_meas_ind() */





/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_stop_single_block_con
+------------------------------------------------------------------------------
| Description : Handles the primitive GFP_MPHP_STOP_SINGLE_BLOCK_CON
|
| Parameters  : *gfp_mphp_stop_single_block_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_stop_single_block_con ( T_MPHP_STOP_SINGLE_BLOCK_CON *mphp_stop_single_block_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_stop_single_block_con" );

  /* SZML-SGLBL/010 */
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      sig_gfp_tc_stop_single_block_con( mphp_stop_single_block_con );
      break;
     default:
      break;
  }
  PFREE(mphp_stop_single_block_con);

} /* gfp_mphp_stop_single_block_con() */





/*
+------------------------------------------------------------------------------
| Function    : gfp_mphp_ncell_pbcch_stop_con
+------------------------------------------------------------------------------
| Description : Handles the primitive MPHP_NCELL_PBCCH_STOP_CON
|
| Parameters  : *mphp_ncell_pbcch_stop_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_mphp_ncell_pbcch_stop_con ( T_MPHP_NCELL_PBCCH_STOP_CON *mphp_ncell_pbcch_stop_con )
{ 
  TRACE_FUNCTION( "gfp_mphp_ncell_pbcch_stop_con" );

  /* SZML-SGLBL/011 */
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      break;
     default:
      break;
  }
  PFREE(mphp_ncell_pbcch_stop_con);

} /* gfp_mphp_ncell_pbcch_stop_con() */


/*
+------------------------------------------------------------------------------
| Function    : gfp_cgrlc_data_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive gfp_cgrlc_data_ind
|
| Parameters  : cgrlc_data_ind - Ptr to primitive payload  
|
+------------------------------------------------------------------------------
*/
GLOBAL void gfp_cgrlc_data_ind ( T_CGRLC_DATA_IND * cgrlc_data_ind )
{ 
  T_D_HEADER  header; 
  T_MSGBUF    * ptr_msg_complete;

  TRACE_FUNCTION( "gfp_cgrlc_data_ind" );

  gfp_adapt_fn ( cgrlc_data_ind->fn+4 );
  
  switch( GET_STATE( GFP ) )
  {
    case GFP_IDLE:
      grr_decode_rlcmac((UBYTE *)(cgrlc_data_ind->data_array), &header);
            
      if(gfp_check_tfi(&header,cgrlc_data_ind->tn))
      {
        if(header.payload NEQ 3)    
        {      
          ptr_msg_complete = grr_handle_rlcmac_header(PACKET_MODE_PTM,&header,cgrlc_data_ind->fn);
          if(ptr_msg_complete != NULL)
            gfp_send_ctrl_block(cgrlc_data_ind->fn, 
                                cgrlc_data_ind->tn, 
                                header.d_ctrl.rrbp, 
                                header.d_ctrl.sp,
                                header.pctrl_ack,
                                ptr_msg_complete);
        }
      }
      break;
    default:
      TRACE_ERROR( "cgrlc_data_ind unexpected" );
      break;
  }

  PFREE(cgrlc_data_ind);

} /* gfp_cgrlc_data_ind() */
