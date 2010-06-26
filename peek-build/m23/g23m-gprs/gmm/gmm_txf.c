/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_txf.c
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
|             procedures and functions as described in the 
|             SDL-documentation (TX-statemachine)
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_TXF_C
#define GMM_TXF_C
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
#include "gmm_f.h"      /* to get the gmm_get_tlli procedure */
#include <string.h>     /* to get memset */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : txgmm_init
+------------------------------------------------------------------------------
| Description : The function txgmm_init() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void txgmm_init ( void )
{ 
  TRACE_FUNCTION( "txgmm_init" );

  /*
   * Initialise service TX with state TX_READY.
   */
  INIT_STATE ( TX, TX_READY );
  
} /* txgmm_init() */
/*
+------------------------------------------------------------------------------
| Function    : tx_fill_unitdata_req
+------------------------------------------------------------------------------
| Description : This procedure tx_fill_unitdata_req() fills the primitive
|               ll_unitdata_req with all necessary dafault parameters.except
|               the SDU 
|               
|
| Parameters  : ll_unitdata_req - the primitive, which has to be filled
|               
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_fill_unitdata_req ( T_LL_UNITDATA_REQ *ll_unitdata_req_p, T_TLLI_TYPE tlli_type, BOOL cipher )
{
  GMM_TRACE_FUNCTION ("tx_fill_unitdata_req");
  ll_unitdata_req_p-> sapi            = LL_SAPI_1;
  ll_unitdata_req_p-> tlli            = gmm_get_tlli ( tlli_type );
  ll_unitdata_req_p-> ll_qos.delay    = LL_DELAY_SUB; 
  ll_unitdata_req_p-> ll_qos.peak     = LL_PEAK_SUB;
  ll_unitdata_req_p-> ll_qos.preced   = LL_PRECED_SUB;
  ll_unitdata_req_p-> ll_qos.mean     = LL_MEAN_SUB;
  ll_unitdata_req_p-> cipher          = 
                                    (gmm_data->config.cipher_on?cipher:FALSE);
  /*
   * all GMM (and SM) messages are transmitted in RLC/MAC acknowledged mode
   */
  ll_unitdata_req_p-> ll_qos.relclass = LL_RLC_PROT;
  /*
   * all GMM messages have highest priority (!!!!! what about SM?)
   */
  ll_unitdata_req_p-> radio_prio      = LL_RADIO_PRIO_1;
  ll_unitdata_req_p->ll_qos.reserved_1  = 0xff;
  ll_unitdata_req_p->ll_qos.reserved_2  = 0xff;
  ll_unitdata_req_p->ll_qos.reserved_3  = 0xff;
  ll_unitdata_req_p->reserved_unitdata_req1.ref_npdu_num = 0xffff;
  ll_unitdata_req_p->reserved_unitdata_req1.ref_nsapi   = 0xff;
  ll_unitdata_req_p->reserved_unitdata_req1.ref_seg_num  = 0xff;
  ll_unitdata_req_p->reserved_unitdata_req4 = 0xffffffff;

#ifdef REL99
  /*
   * This change is related to R99.
   */
  ll_unitdata_req_p->pkt_flow_id        = 0xffff; /* TCS 4.0 */
#else
  ll_unitdata_req_p->reserved_unitdata_req5   = 0xffff;
#endif

  ll_unitdata_req_p->seg_pos            = 0xff;
  ll_unitdata_req_p->attached_counter= 0x00;
  
  GMM_RETURN;
}

/*
+------------------------------------------------------------------------------
| Function    : tx_unitdata_req
+------------------------------------------------------------------------------
| Description : This procedure tx_unitdata_req() fills the primitive
|               ll_unitdata_req with all necessary parameters.and sends
|               the message to LLC
|               
|
| Parameters  : tlli_type       - local, foreign or random
|               
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_unitdata_req ( T_TLLI_TYPE tlli_type, UBYTE msg_type )
{ 
  U8 *payload;
#ifndef GMM_TCS4
  U16 length;
#endif
  
  GMM_TRACE_FUNCTION( "tx_unitdata_req" );
  _decodedMsg[0] = msg_type;

  /*
   * send the message to LLC
   */
  {
    USHORT bit_size = bit_size_message_table[msg_type] + LLC_SDU_OFFSET_BITS;
    PALLOC_SDU ( ll_unitdata_req_p, LL_UNITDATA_REQ, bit_size );    
    
    ll_unitdata_req_p->sdu.o_buf = LLC_SDU_OFFSET_BITS;
    /*
     * l_buf has to be set to inform CCD how many bit must be reset
     */
    ll_unitdata_req_p->sdu.l_buf = bit_size_message_table[msg_type];
    
    /*
     * fills the primitive with all necessary data
     */

    if ( gmm_data->cipher ) 
    {
      switch ( _decodedMsg [0] )
      {
        case ATTACH_REQUEST:
        case ATTACH_REJECT:
        case AUTHENTICATION_AND_CIPHERING_REQUEST:
        case AUTHENTICATION_AND_CIPHERING_RESPONSE:
        case AUTHENTICATION_AND_CIPHERING_REJECT:
        case IDENTITY_RESPONSE:
        case ROUTING_AREA_UPDATE_REQUEST:
         /*
          * <R.GMM.CIPHMSG.M.001>
          */
          tx_fill_unitdata_req ( ll_unitdata_req_p,
                           tlli_type,
                           LL_CIPHER_OFF );

          break;
        default:
          tx_fill_unitdata_req ( ll_unitdata_req_p,
                           tlli_type,
                           LL_CIPHER_ON );
          break;
      }
    }
    else
    {
          tx_fill_unitdata_req ( ll_unitdata_req_p,
                           tlli_type,
                           LL_CIPHER_OFF );

    }

    if ( ccd_codeMsg ( CCDENT_GMM,
                        UPLINK,
                        (T_MSGBUF  *) &ll_unitdata_req_p->sdu,
                        _decodedMsg,
                        NOT_PRESENT_8BIT) != ccdOK
      )
    {

     /*
      * Error Handling
      */
      TRACE_ERROR( "CCD_ERROR: Error Handling for ccd-output not yet supported" ) ;
      PFREE ( ll_unitdata_req_p );
    } 
    else
    {
      /*
       * Set the Protocol discriminator to PD_GMM, defined in GSM 4.0.8
       * Set the Skip indicater to 0 
       */     
      ccd_codeByte (ll_unitdata_req_p->sdu.buf,
                   (USHORT)(ll_unitdata_req_p->sdu.o_buf - 8),
                    8,
                    PD_GMM);
      ll_unitdata_req_p->sdu.l_buf += 8;
      ll_unitdata_req_p->sdu.o_buf -= 8;
    
      #ifndef NTRACE

      payload = &(ll_unitdata_req_p->sdu.buf[0]);     /* beginning of buffer      */
      payload += (ll_unitdata_req_p->sdu.o_buf) >> 3; /*  plus offset (bytes)     */
#ifndef GMM_TCS4
      length  = (ll_unitdata_req_p->sdu.l_buf) >> 3;  /* length (bytes, bits / 8) */
#endif

      switch ( _decodedMsg [0] )
        {
        case ATTACH_REQUEST:
          TRACE_0_OUT_PARA ( "ATTACH_REQ" );
#ifndef GMM_TCS4
          TRACE_BINDUMP( GMM_handle,
             TC_USER4,
             "ATTACH REQUEST",
             payload,
             length);
#endif
          break;
        case ATTACH_COMPLETE:
          TRACE_0_OUT_PARA ( "ATTACH_COMPLETE" ); 
          break;
        case U_DETACH_REQUEST:
          TRACE_0_OUT_PARA ( "DETACH_REQ" );
          break;
        case U_DETACH_ACCEPT:
          TRACE_0_OUT_PARA ( "DETACH_ACC" );
          break;
        case ROUTING_AREA_UPDATE_REQUEST:
          TRACE_0_OUT_PARA ( "RAU_REQ" );
#ifndef GMM_TCS4
          TRACE_BINDUMP( GMM_handle,
             TC_USER4,
             "ROUTING AREA UPDATE REQUEST",
             payload,
             length);
#endif
          break;
        case ROUTING_AREA_UPDATE_COMPLETE:
          TRACE_0_OUT_PARA ( "RAU_COMPLETE" );
          break;
        case P_TMSI_REALLOCATION_COMPLETE:
          TRACE_0_OUT_PARA ( "PTMSI_REALLOC_COMPLETE" );
          break;
        case AUTHENTICATION_AND_CIPHERING_RESPONSE:
          TRACE_0_OUT_PARA ( "A&C_RES" );
          break;
        case IDENTITY_RESPONSE:
          TRACE_0_OUT_PARA ( "ID_RES");
          break;
        case GMM_STATUS:
          TRACE_0_OUT_PARA ( "GMM_STATUS");
          break;
        /* wrong direction: */
        case ATTACH_ACCEPT:      
          TRACE_ERROR ( "ATTACH_ACC not uplink direction");
          break;
        case ATTACH_REJECT:
          TRACE_ERROR ( "ATTACH_REJECT not uplink direction");
          break;
        case ROUTING_AREA_UPDATE_ACCEPT:
          TRACE_ERROR ( "ROUTING_AREA_UPDATE_ACCEPT not uplink direction");
          break;
        case ROUTING_AREA_UPDATE_REJECT:
          TRACE_ERROR ( "ROUTING_AREA_UPDATE_REJECT not uplink direction");
          break;
        case P_TMSI_REALLOCATION_COMMAND:
          TRACE_ERROR ( "PTMSI_REALLOC_CMD not uplink direction");
          break;
        case AUTHENTICATION_AND_CIPHERING_REQUEST:
          TRACE_ERROR ( "A&C_REQ not uplink direction");
          break;
        case AUTHENTICATION_AND_CIPHERING_REJECT:
          TRACE_ERROR ( "A&C_REJ not uplink direction");
          break;
        case IDENTITY_REQUEST:
          TRACE_ERROR ( "ID_REQ not uplink direction");
          break;
        case GMM_INFORMATION:
          TRACE_ERROR ( "GMM_INFO not uplink direction");
          break;
        default:
          TRACE_ERROR ("Unknown GMM PDU");
          break;
      }
    #endif /* _SIMULATION_ */

    PSEND ( hCommLLC, ll_unitdata_req_p);

    }
  }
  GMM_RETURN;
} /* tx_fill_unitdata_req() */


