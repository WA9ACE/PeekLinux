/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_rxp.c
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
|  Purpose :  This modul is part of the entity GMM and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (RX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef GMM_RXP_C
#define GMM_RXP_C
#endif

#define ENTITY_GMM

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_gmm.h"    /* to get cnf-definitions */
#include "mon_gmm.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "gmm.h"        /* to get the global entity definitions */

#include "gmm_kerns.h"  /* to get some signals */
#include "gmm_rxf.h"    /* to get some functions */

#include  <string.h>    /* to get memset */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : rx_ll_unitdata_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_UNITDATA_IND
|
| Parameters  : *ll_unitdata_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void rx_ll_unitdata_ind ( T_LL_UNITDATA_IND *ll_unitdata_ind_ )
{ 
  UBYTE pd;
  UBYTE tiskip;
  UBYTE ccdenttype;
#ifndef GMM_TCS4
  U8   *payload;
  U16   length;
#endif
  
  GMM_TRACE_FUNCTION( "rx_ll_unitdata_ind" );
  {
  PPASS (ll_unitdata_ind_, ll_unitdata_ind, LL_UNITDATA_IND );

  
  switch( GET_STATE( RX ) )
  {
    case RX_READY:
      /*
       * get the Protokol discriminator
       */
      GET_PD (ll_unitdata_ind->sdu, pd);
     
      if ( pd == PD_SM )
      {
        /*
         * macro PPASS (ll_unitdata_ind,
         *              gmmsm_unitdata_ind,
         *              GMMSM_UNITDATA_IND);
         * is not possible, because o f different parameters
         */
#ifdef GMM_TCS4
        PALLOC_SDU ( gmmsm_unitdata_ind, MMPM_UNITDATA_IND, (USHORT)(ll_unitdata_ind->sdu.l_buf+ll_unitdata_ind->sdu.o_buf) );
#else
        PALLOC_SDU ( gmmsm_unitdata_ind, GMMSM_UNITDATA_IND, (USHORT)(ll_unitdata_ind->sdu.l_buf+ll_unitdata_ind->sdu.o_buf) );
#endif
        gmmsm_unitdata_ind->sdu.l_buf = ll_unitdata_ind->sdu.l_buf;
        gmmsm_unitdata_ind->sdu.o_buf = ll_unitdata_ind->sdu.o_buf;            
        memcpy (&gmmsm_unitdata_ind->sdu.buf[0],&ll_unitdata_ind->sdu.buf[0],
                (gmmsm_unitdata_ind->sdu.l_buf+gmmsm_unitdata_ind->sdu.o_buf+7)/8);
        
        PSEND ( hCommSM, gmmsm_unitdata_ind );

        #ifdef TRACE_EVE
          #ifndef GMM_TCS4
            switch ( gmmsm_unitdata_ind->sdu.buf[((gmmsm_unitdata_ind->sdu.o_buf)>>3)+1])
            {
              case ACTIVATE_PDP_REQ:
                TRACE_EVENT ( "OPar: ACTIVATE_PDP_REQ");break;
              case ACTIVATE_PDP_ACC:      
                TRACE_EVENT ( "OPar: ACTIVATE_PDP_ACC");break;
              case ACTIVATE_PDP_REJ:      
                TRACE_EVENT ( "OPar: ACTIVATE_PDP_REJ");break;
              case REQ_PDP_ACT:      
                TRACE_EVENT ( "OPar: REQ_PDP_ACT");break;
              case REQ_PDP_ACT_REJ:      
                TRACE_EVENT ( "OPar: REQ_PDP_ACT_REJ");break;
              case MOD_PDP_REQ:      
                TRACE_EVENT ( "OPar: MOD_PDP_REQ");break;
              case MOD_PDP_ACC:      
                TRACE_EVENT ( "OPar: MOD_PDP_ACC");break;
              case DEACT_PDP_REQ:      
                TRACE_EVENT ( "OPar: DEACT_PDP_REQ");break;
              case DEACT_PDP_ACC:      
                TRACE_EVENT ( "OPar: DEACT_PDP_ACC");break; 
              case SM_STATUS:      
                TRACE_EVENT ( "OPar: SM_STATUS");break;
              default:
                TRACE_EVENT ( "OPar: SNDCP unknown");break;
          }
          #else  /* #ifndef GMM_TCS4 */
            switch ( gmmsm_unitdata_ind->sdu.buf[((gmmsm_unitdata_ind->sdu.o_buf)>>3)+1])
            {
              case ACTIVATE_PDP_CONTEXT_REQUEST:
                TRACE_EVENT ( "OPar: ACTIVATE_PDP_CONTEXT_REQUEST");break;
              case ACTIVATE_PDP_CONTEXT_ACCEPT:      
                TRACE_EVENT ( "OPar: ACTIVATE_PDP_CONTEXT_ACCEPT");break;
              case ACTIVATE_PDP_CONTEXT_REJECT:      
                TRACE_EVENT ( "OPar: ACTIVATE_PDP_CONTEXT_REJECT");break;
              case REQUEST_PDP_CONTEXT_ACTIVATION:      
                TRACE_EVENT ( "OPar: REQUEST_PDP_CONTEXT_ACTIVATION");break;
              case REQUEST_PDP_CONTEXT_ACTIVATION_REJECT:      
                TRACE_EVENT ( "OPar: REQUEST_PDP_CONTEXT_ACTIVATION_REJECT");break;
              case U_MODIFY_PDP_CONTEXT_REQUEST:      
                TRACE_EVENT ( "OPar: U_MODIFY_PDP_CONTEXT_REQUEST");break;
              case U_MODIFY_PDP_CONTEXT_ACCEPT:      
                TRACE_EVENT ( "OPar: U_MODIFY_PDP_CONTEXT_ACCEPT");break;
              case DEACTIVATE_PDP_CONTEXT_REQUEST:      
                TRACE_EVENT ( "OPar: DEACTIVATE_PDP_CONTEXT_REQUEST");break;
              case DEACTIVATE_PDP_CONTEXT_ACCEPT:      
                TRACE_EVENT ( "OPar: DEACTIVATE_PDP_CONTEXT_ACCEPT");break; 
              case SM_STATUS:      
                TRACE_EVENT ( "OPar: SM_STATUS");break;
              default:
                TRACE_EVENT ( "OPar: SNDCP unknown");break;
			}
          #endif /* #ifndef GMM_TCS4 */
      #endif /* _SIMULATION_ */
        
        PFREE ( ll_unitdata_ind );
        GMM_RETURN;
      }
      else if ( PD_GMM != pd && PD_TST != pd )
      {
        /* 
         * 04.07 ch. 11.2.3.1.1
         * "If the Mobile Station receives, on a SAP where it expects standard L3 messages, a
         * standard L3 message with a protocol discriminator different from those specified in
         * table 11.2, or for a protocol that it does not support, the Mobile Station shall
         * ignore the message."
         */
        TRACE_ERROR ("message with wrong PD received");
        
        PFREE (ll_unitdata_ind);
        GMM_RETURN;
      }  
      GET_TI (ll_unitdata_ind->sdu, tiskip);

      if ( tiskip != SKIP_VALID )
      {
        TRACE_ERROR ("SKIP is invalid in reseived GMM meassage");
        PFREE ( ll_unitdata_ind );
        GMM_RETURN;
      }
      /*
       * rx_decode_msg
       */
      ll_unitdata_ind->sdu.l_buf -= 8;
      ll_unitdata_ind->sdu.o_buf += 8;
      ccdenttype = (PD_GMM==pd)?CCDENT_GMM:CCDENT_TST;
      
      if (ccdOK != ccd_decodeMsg (ccdenttype,
                       DOWNLINK,
                       (T_MSGBUF /*FAR*/ *) &ll_unitdata_ind->sdu,
                       (UBYTE /*FAR*/ *) _decodedMsg,
                       NOT_PRESENT_8BIT) 
         )
      {
        /*
         * Error Handling
         */
        USHORT parlist[6];
        UBYTE  ccd_err;

        /* 
         * clear parlist
         */
        memset (parlist,0, sizeof (parlist));
        ccd_err = ccd_getFirstError (ccdenttype, parlist);

        /*
         * Error Handling
         */
        do 
        {
          switch (ccd_err)
          {
            case ERR_COMPREH_REQUIRED:    /* Comprehension required     */
            case ERR_MAND_ELEM_MISS:      /* Mandatory elements missing */
              rx_gmm_status (ERRCS_INVALID_M_INFO);
              TRACE_ERROR( "CCD ERROR: Mandatory elements missing");
              PFREE (ll_unitdata_ind);
              GMM_RETURN;
              /* break; */
            case ERR_IE_NOT_EXPECTED:
              TRACE_ERROR("CCD ERROR: IE not expected. properly an R99 optional element like cell notification, eplmn, or t3302");
              break;
            case ERR_IE_SEQUENCE:
              TRACE_ERROR("wrong sequence of information elements");
              break;
            case ERR_MAX_IE_EXCEED:
              TRACE_ERROR("maximum amount of repeatable information elements has exceeded");
              break;
            case ERR_MAX_REPEAT:
              TRACE_ERROR("a repeatable element occurs too often in the message");
              break;
            case ERR_INVALID_MID:     
              rx_gmm_status (ERRCS_TYPE_INVALID);
              TRACE_ERROR("the message ID is not correct");
              PFREE (ll_unitdata_ind);
              GMM_RETURN;
            case ERR_INTERNAL_ERROR:
              TRACE_ERROR("an internal CCD error occured ");
              rx_gmm_status (ERRCS_MESSAGE_INVALID);            
              PFREE (ll_unitdata_ind);
              GMM_RETURN;
              /* break; */
            default:
              TRACE_ERROR( "CCD ERROR: unknown error");
              break;
              /* break; */
					}
          ccd_err = ccd_getNextError (ccdenttype, parlist);
        }while(ccd_err NEQ ERR_NO_MORE_ERROR);
      }

#ifndef GMM_TCS4
      payload  = &(ll_unitdata_ind->sdu.buf[0]);     /* beginning of buffer      */
      payload += (ll_unitdata_ind->sdu.o_buf) >> 3;  /*  plus offset (bytes)     */
      length   = (ll_unitdata_ind->sdu.l_buf) >> 3;  /* length (bytes, bits / 8) */
#endif

      switch (_decodedMsg[0])
      {
        case ATTACH_ACCEPT:
          TRACE_0_PARA ( "ATTACH_ACC");
#ifndef GMM_TCS4
          TRACE_BINDUMP( GMM_handle,
             TC_USER4,
             "ATTACH ACCEPT",
             payload,
             length);
#endif
          sig_rx_kern_att_acc_ind ();
          break;
        case ATTACH_REJECT:
          TRACE_0_PARA ( "ATTACH_REJ");
#ifndef GMM_TCS4
          TRACE_BINDUMP( GMM_handle,
             TC_USER4,
             "ATTACH REJECT",
             payload,
             length);
#endif
          sig_rx_kern_att_rej_ind ();
          break;
        case D_DETACH_REQUEST:
          TRACE_0_PARA ( "D_DETACH_REQ");
          sig_rx_kern_det_req_ind ();
          break;
        case D_DETACH_ACCEPT:
          TRACE_0_PARA ( "D_DETACH_ACC");
          sig_rx_kern_det_acc_ind ();
          break;
        case ROUTING_AREA_UPDATE_ACCEPT:
          TRACE_0_PARA ( "RAU_ACC");
#ifndef GMM_TCS4
          TRACE_BINDUMP( GMM_handle,
             TC_USER4,
             "ROUTING AREA UPDATE ACCEPT",
             payload,
             length);
#endif
          sig_rx_kern_rau_acc_ind ();
          break;
        case ROUTING_AREA_UPDATE_REJECT:
          TRACE_0_PARA ( "RAU_REJ");
#ifndef GMM_TCS4
          TRACE_BINDUMP( GMM_handle,
             TC_USER4,
             "ROUTING AREA UPDATE REJECT",
             payload,
             length);
#endif
          sig_rx_kern_rau_rej_ind ();
          break;
        case P_TMSI_REALLOCATION_COMMAND:
          TRACE_0_PARA ( "PTMSI_REALLOC_CMD");
          sig_rx_kern_realloc_cmd_ind ();
          break;
        case AUTHENTICATION_AND_CIPHERING_REQUEST:
          TRACE_0_PARA ( "A&C_REQ");
          sig_rx_kern_auth_req_ind ();
          break;
        case AUTHENTICATION_AND_CIPHERING_REJECT:
          TRACE_0_PARA ( "A&C_REJ");
          sig_rx_kern_auth_rej_ind ();
          break;
        case IDENTITY_REQUEST:
          TRACE_0_PARA ( "ID_REQ");
          sig_rx_kern_id_req_ind ();
          break;
        case GMM_STATUS:
          TRACE_0_PARA ( "GMM_STATUS");
          sig_rx_kern_status_ind ();
          break;
        case GMM_INFORMATION:
          TRACE_0_PARA ( "GMM_INFO");
          sig_rx_kern_info_ind ();
          break;
        case GPRS_TEST_MODE_CMD:
          TRACE_0_PARA ( "GPRS_TEST_MODE_CMD");
          sig_rx_kern_tst_cmd_ind ();
          break;
#ifdef GMM_TCS4
  #ifdef FF_EGPRS
        case EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD:
          TRACE_0_PARA ( "EGPRS_START_RADIO_BLOCK_LOOPBACK_CMD");
          sig_rx_kern_egprs_loopb_cmd_ind ();
          break;
  #endif /*FF_EGPRS*/
#endif /*GMM_TCS4*/
        default:
          /* 
           * This branch should never be reached
           */
          rx_gmm_status ( ERRCS_TYPE_INVALID );
          TRACE_ERROR( "NO valid GMM message reseived" );
          break;
      }

      PFREE (ll_unitdata_ind);
      break;
    default:
      PFREE (ll_unitdata_ind);
      TRACE_ERROR( "LL_UNITDATA_IND unexpected" );
      break;
  }
  }
  GMM_RETURN;
} /* rx_ll_unitdata_ind() */
