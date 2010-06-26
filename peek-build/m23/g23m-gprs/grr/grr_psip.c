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
|             PSI of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_PSIP_C
#define GRR_PSIP_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include <string.h>

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

#include "grr_psip.h"   /* header file of primitives*/

#include "grr_psif.h"   /* header file of functions*/

#include "grr_meass.h"   /* signals exchanged between PSI and MEAS*/
#include "grr_ctrls.h"   /* header file of functions*/

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : psi_t_60_sec
+------------------------------------------------------------------------------
| Description : Handles the primitive T_60_SEC
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_t_60_sec ( void )
{
  TRACE_FUNCTION( "t_60_sec" );

#ifndef _TARGET_

  /*
   * No PSI1 or PSI13 or SI13 received within 60 sec
   * This will result in a cell reselection
   */
  TRACE_EVENT( "No PSI1 or PSI13 or SI13 received within 60 sec" );
  
  sig_psi_ctrl_psi1or_psi13_receiption_failure();

#else /* #ifndef _TARGET_ */
  
  TRACE_EVENT( "Processing timeout of T_60_SEC disabled on target" );

#endif /* #ifndef _TARGET_ */

} /* psi_t_60_sec() */



/*
+------------------------------------------------------------------------------
| Function    : psi_t_30_sec
+------------------------------------------------------------------------------
| Description : Handles the primitive T_30_SEC
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_t_30_sec ( void)
{
  TRACE_FUNCTION( "t_30_sec" );

/*#ifndef _TARGET_*/
  /*
   * No PSI1 received within 30 sec
   */

  switch( GET_STATE( PSI ) )
  {
    case PSI_TRANSFER:
      if(grr_data->test_mode NEQ CGRLC_NO_TEST_MODE) 
      {
        psi_start_30sec();
        TRACE_EVENT( "T_30_SEC received during test mode" );
        break;
      }
      /*lint -fallthrough*/
    case PSI_IDLE:
      /*
        If the mobile station has not received the PSI1 message within the last 30 seconds,
        it shall attempt to receive the PSI1 message each time it is scheduled on PBCCH.
        Such attempts shall be made during both packet idle and packet transfer modes.
      */
      switch(psc_db->acq_type)
      {
        case NONE:
          psc_db->acq_type = PERIODICAL_PSI1_READING;
          psi_receive_psi(READ_PSI1_IN_PSI1_REPEAT_PERIODS);
          break;
        default:
          /*
           * Access to the network not allowed or complete PSI
           * has not been read. Nothing to do. Acquisition is running
           */
          TRACE_EVENT("PSI not complete/ access not allowed");
          break;
      }
      break;
 case PSI_BCCH_TRANSFER:
      if(grr_data->test_mode NEQ CGRLC_NO_TEST_MODE) 
      {
        psi_start_30sec();
        TRACE_EVENT( "T_30_SEC received during test mode" );
        break;
      }
      /*lint -fallthrough*/
    case PSI_BCCH_IDLE:
      /*
        If the mobile station has received neither the SI13 nor the PSI13 message within
        the last 30 seconds, it shall attempt to receive the SI13 message each time it is
        scheduled on BCCH. Such attempts shall be made during both packet idle and packet
        transfer modes.
      */
      switch(psc_db->acq_type)
      {
        case NONE:
          psc_db->acq_type = PERIODICAL_SI13_READING;
          sig_psi_ctrl_read_si( UPDATE_SI13 );
          break;
        default:
          /*
           * Access to the network not allowed or complete SI
           * has not been read. Nothing to do. Acquisition is running
           */
          TRACE_EVENT("SI not complete/ access not allowed");
          break;
      }
      break;
    default:
      TRACE_ERROR( "T_30_SEC unexpected" );
      break;
  }
/*#endif*/
} /* psi_t_30_sec() */



/*
+------------------------------------------------------------------------------
| Function    : psi_t_10_sec
+------------------------------------------------------------------------------
| Description : Handles the primitive T_10_SEC
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_t_10_sec ( void )
{
  TRACE_FUNCTION( "t_10_sec" );
  /*
   * partial or complete acq has not completed within 10 seconds
   */
/*#ifndef _TARGET_*/

  if( grr_t_status( T3174 ) > 0 )
  {
	  TRACE_EVENT( "psi_t_10_sec expired when t3174 is running" );
    return;
  }

  switch( GET_STATE( PSI ) )  
  {            
    case PSI_IDLE:
    case PSI_TRANSFER:
      psi_stop_10sec();
      /*  If the mobile station has not received the required messages within 10 seconds
       *  after the start of the acquisition procedure, an attempt to receive a missing message
       *  shall be made each time the message is scheduled on the broadcast channel.
       *  This case should not happen -except the network goes down. Reason:
       *  in case of partial or complete acquisition we read all required (P)SI messages
       *  each time the message is scheduled on the broadcast channel. Our reading process
       *  continues until the 60 second   expires or we receive the required messages.
       *  If the 60 second timer expires we should perform a cell selection
       */
      psi_update_data_to_request(INIT_NOT_NEEDED );
      switch(psc_db->acq_type)
        {
          case COMPLETE:
            psi_complete_acq(COMPLETE);
            break;
          case PARTIAL:
            psi_partial_acq();
            psc_db->send_psi_status = TRUE; /* this parameters has meaning if the network supports PSI STATUS */
            break;
          default:
            TRACE_ERROR("Unknown acq_type in T_10_sec (PBCCH)");
            break;
        }
        break;
    case PSI_BCCH_IDLE:
    case PSI_BCCH_TRANSFER:
      psi_stop_10sec();
      /*  Please see comment above */
      switch(psc_db->acq_type)
        {
          case COMPLETE:
            sig_psi_ctrl_read_si(COMPLETE_SI);
            break;
          case PARTIAL:
            sig_psi_ctrl_read_si(psc_db->si_to_acquire);
            break;
          default:
            TRACE_ERROR("Unknown acq_type in T_10_sec (BCCH)");
            break;
        }
        break;
      default:
        TRACE_ERROR( "T_10_SEC unexpected" );
        break;
	
  }
/*#endif*/
} /* psi_t_10_sec() */
























/*
+------------------------------------------------------------------------------
| Function    : psi_complete_psi_read_failed
+------------------------------------------------------------------------------
| Description : Handles the primitive PSI_COMPLETE_PSI_READ_FAILED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_complete_psi_read_failed ( void )
{
  TRACE_FUNCTION( "psi_complete_psi_read_failed" );
  /*
   * reading of complete PSI within 10 sec failed
   * Stop PBCCH reading if running,
   * Inform CTRL, cell reselection should be performed
   */
  TRACE_EVENT("reading of complete PSI within 10 sec failed");

  if( grr_t_status( T3174 ) > 0 )
  {
	  TRACE_EVENT( "psi_complete_psi expired when t3174 is running" );
    return;
  }

  SET_STATE( PSI, PSI_NULL );
  if(psc_db->acq_type NEQ NONE) /* acquisition has been running, so we have to stop this acq.*/
  {
    TRACE_EVENT("psi_stop_psi_reading");
    psi_stop_psi_reading(NONE);
  }
  
  psi_stop_timer();
  psi_reset_all();
  /*
   * Perform CR
   */
  sig_psi_ctrl_psi1or_psi13_receiption_failure();
  
} /* psi_complete_psi_read_failed() */
