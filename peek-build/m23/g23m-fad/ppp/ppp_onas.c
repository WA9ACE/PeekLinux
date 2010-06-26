/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  This modul is part of the entity PPP and implements all 
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (ONA-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_ONAS_C
#define PPP_ONAS_C
#endif /* !PPP_ONAS_C */

#define ENTITY_PPP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"     /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_ppp.h"    /* to get cnf-definitions */
#include "mon_ppp.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"        /* to get the DTILIB definitions */
#include "ppp.h"        /* to get the global entity definitions */

#include "ppp_arbf.h"   /* to get the arb function definitions */
#include "ppp_ftxs.h"   /* to get the ftx signal interface */
#include "ppp_lcps.h"   /* to get the lcp signal interface */
#include "ppp_ncps.h"   /* to get the ncp signal interface */
#include "ppp_onaf.h"   /* to get the ona function definitions */
#include "ppp_rts.h"    /* to get the rt signal interface */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_ruc_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_RUC_IND
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_ruc_ind (USHORT ptype, T_desc2* packet) 
{ 
  UBYTE index;
  TRACE_ISIG( "sig_any_ona_ruc_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSING:
    case ONA_REQ_SENT:
    case ONA_ACK_RCVD:
    case ONA_ACK_SENT:
    case ONA_OPENED:
      /*
       * send code reject
       */
      sig_any_ftx_packet_req(ptype, packet); /* scj */
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_RUC_IND unexpected" );
      arb_discard_packet(packet);
      break;
  }
} /* sig_any_ona_ruc_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_open_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_OPEN_IND
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_open_ind (USHORT ptype, T_desc2* packet) 
{ 
  UBYTE index;
  TRACE_ISIG( "sig_any_ona_open_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_OPENED:
    case ONA_CLOSED:
    case ONA_CLOSING:
    case ONA_REQ_SENT:
    case ONA_ACK_RCVD:
    case ONA_ACK_SENT:
      SET_STATE( PPP_SERVICE_ONA, ONA_REQ_SENT );
      /*
       * reset counters
       */
      ppp_data->ona->restarts = ppp_data->mc;
      ppp_data->ona->failures = ppp_data->mf;
      ppp_data->ona->loops    = ppp_data->mc + 1;
      /*
       * send configure request
       */
      sig_any_ftx_packet_req(ptype, packet); /* scr */
      /*
       * start timer
       */
      if(ppp_data->ona->restarts > 0)
        ppp_data->ona->restarts--;
      sig_any_rt_rrt_req();
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_OPEN_IND unexpected" );
      arb_discard_packet(packet);
      break;
  }
} /* sig_any_ona_open_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_down_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_DOWN_IND
|
| Parameters  : ptype  - type of packet
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_down_ind (USHORT ptype) 
{ 
  UBYTE index;
  TRACE_ISIG( "sig_any_ona_down_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSED:
    case ONA_OPENED:
      SET_STATE( PPP_SERVICE_ONA, ONA_CLOSED );
      break;
    case ONA_CLOSING:
    case ONA_REQ_SENT:
    case ONA_ACK_RCVD:
    case ONA_ACK_SENT:
      SET_STATE( PPP_SERVICE_ONA, ONA_CLOSED );
      /*
       * stop timer
       */
      sig_any_rt_srt_req();
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_DOWN_IND unexpected" );
      break;
  }
} /* sig_any_ona_down_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_close_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_CLOSE_IND
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_close_ind (USHORT ptype, T_desc2* packet) 
{ 
  UBYTE index;
  TRACE_ISIG( "sig_any_ona_close_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSED:
      arb_discard_packet(packet);
      /*
       * send This-Layer-Finished signal
       */
      if (ptype EQ DTI_PID_LCP) 
      {
        sig_ona_lcp_tlf_req();
      } 
      else if (ptype EQ DTI_PID_IPCP) 
      {
        sig_ona_ncp_tlf_req();
      } 
      break;
    case ONA_CLOSING:
      arb_discard_packet(packet);
      break;
    case ONA_OPENED:
    case ONA_REQ_SENT:
    case ONA_ACK_RCVD:
    case ONA_ACK_SENT:
      SET_STATE( PPP_SERVICE_ONA, ONA_CLOSING );
      /*
       * reset counter
       */
      ppp_data->ona->restarts = ppp_data->mt;
      /*
       * send terminate request
       */
      sig_any_ftx_packet_req(ptype, packet); /* str */
      /*
       * start timer
       */
      if(ppp_data->ona->restarts > 0)
        ppp_data->ona->restarts--;
      sig_any_rt_rrt_req();
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_CLOSE_IND unexpected" );
      arb_discard_packet(packet);
      break;
  }
} /* sig_any_ona_close_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_rtr_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_RTR_IND
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_rtr_ind (USHORT ptype, T_desc2* packet) 
{
  UBYTE index;

  TRACE_ISIG( "sig_any_ona_rtr_ind" );

  TRACE_EVENT( "Peer requested Termination by PPP frame" );

  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSING:
      /*
       * send terminate acknowledge
       */
      sig_any_ftx_packet_req(ptype, packet); /* sta */
      break;
    case ONA_OPENED:
      SET_STATE( PPP_SERVICE_ONA, ONA_CLOSING );
      /*
       * send This-Layer-Down signal
       */
      if (ptype EQ DTI_PID_LCP) 
      {
        sig_ona_lcp_tld_req();
      } 
      else if (ptype EQ DTI_PID_IPCP) 
      {
        sig_ona_ncp_tld_req();
      } 
      ppp_data->ona = & ppp_data->ona_base[index];
    case ONA_REQ_SENT:
    case ONA_ACK_RCVD:
    case ONA_ACK_SENT:
      SET_STATE( PPP_SERVICE_ONA, ONA_CLOSING );
      /*
       * set counter to zero
       */
      ppp_data->ona->restarts = 0;
      /*
       * send terminate acknowledge
       */
      sig_any_ftx_packet_req(ptype, packet); /* sta */
      /*
       * start timer
       */
      sig_any_rt_rrt_req();
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_RTR_IND unexpected" );
      arb_discard_packet(packet);
      break;
  }

  /*
   * set error code
   */
  if(ppp_data->ppp_cause EQ 0)
    ppp_data->ppp_cause = PPP_TERM_OK_PEER;

} /* sig_any_ona_rtr_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_to_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_TO_IND
|
| Parameters  : ptype  - type of packet
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_to_ind (USHORT ptype) 
{ 
  UBYTE index;
  T_desc2* packet = NULL;
  TRACE_ISIG( "sig_any_ona_to_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSING:
      if (ppp_data->ona->restarts EQ 0)
      {
        /*
         * negative timeout
         */
        SET_STATE( PPP_SERVICE_ONA, ONA_CLOSED );
        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
          ppp_data->ppp_cause = PPP_TERM_NO_RESPONSE;
        /*
         * send This-Layer-Finished signal
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_tlf_req();
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_tlf_req();
        }
      }
      else
      {
        /*
         * positive timeout
         */
        /*
         * request a new terminate request packet
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_str_req(&packet);
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_str_req(&packet);
        }
        /*
         * set counter and send terminate request packet
         */
        ppp_data->ona->restarts--;
        sig_any_ftx_packet_req(ptype, packet); /* str */
        /*
         * restart timer
         */
        sig_any_rt_rrt_req();
      }
      break;
    case ONA_REQ_SENT:
    case ONA_ACK_SENT:
      if (ppp_data->ona->restarts EQ 0)
      {
        /*
         * negative timeout
         */
        SET_STATE( PPP_SERVICE_ONA, ONA_CLOSED );
        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
          ppp_data->ppp_cause = PPP_TERM_NO_RESPONSE;
        /*
         * send This-Layer-Finished signal
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_tlf_req();
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_tlf_req();
        }
      }
      else
      {
        /*
         * positive timeout
         */
        /*
         * request a new configure request packet
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_scr_req(&packet);
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_scr_req(&packet);
        }
        /*
         * set counter and send configure request packet
         */
        ppp_data->ona->restarts--;
        sig_any_ftx_packet_req(ptype, packet); /* scr */
        /*
         * restart timer
         */
        sig_any_rt_rrt_req();
      }      
      break;
    case ONA_ACK_RCVD:
      if (ppp_data->ona->restarts EQ 0)
      {
        /*
         * negative timeout
         */
        SET_STATE( PPP_SERVICE_ONA, ONA_CLOSED );
        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
          ppp_data->ppp_cause = PPP_TERM_NO_RESPONSE;
        /*
         * send This-Layer-Finished signal
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_tlf_req();
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_tlf_req();
        }
      }
      else
      {
        /*
         * positive timeout
         */
        SET_STATE( PPP_SERVICE_ONA, ONA_REQ_SENT );
        /*
         * request a new configure request packet
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_scr_req(&packet);
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_scr_req(&packet);
        }
        /*
         * set counter and send configure request packet
         */
        ppp_data->ona->restarts--;
        sig_any_ftx_packet_req(ptype, packet); /* scr */
        /*
         * restart timer
         */
        sig_any_rt_rrt_req();
      }      
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_TO_IND unexpected" );
      break;
  }
} /* sig_any_ona_to_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_rta_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_RTA_IND
|
| Parameters  : ptype  - type of packet
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_rta_ind (USHORT ptype) 
{ 
  UBYTE index;
  T_desc2* packet = NULL;
  TRACE_ISIG( "sig_any_ona_rta_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSING:
      SET_STATE( PPP_SERVICE_ONA, ONA_CLOSED );
      /*
       * stop timer
       */
      sig_any_rt_srt_req();
      /*
       * send This-Layer-Finished signal
       */
      if (ptype EQ DTI_PID_LCP) 
      {
        sig_ona_lcp_tlf_req();
      } 
      else if (ptype EQ DTI_PID_IPCP) 
      {
        sig_ona_ncp_tlf_req();
      }
      break;
    case ONA_REQ_SENT:
    case ONA_ACK_RCVD:
    case ONA_ACK_SENT:
      break;
    case ONA_OPENED:
      SET_STATE( PPP_SERVICE_ONA, ONA_REQ_SENT );
      /*
       * send This-Layer-Down signal and
       * request a new configure request packet
       */
      if (ptype EQ DTI_PID_LCP) 
      {
        sig_ona_lcp_tld_req();
        sig_ona_lcp_scr_req(&packet);
      } 
      else if (ptype EQ DTI_PID_IPCP) 
      {
        sig_ona_ncp_tld_req();
        sig_ona_ncp_scr_req(&packet);
      }
      ppp_data->ona = & ppp_data->ona_base[index];
      /*
       * send configure request packet
       */
      sig_any_ftx_packet_req(ptype, packet); /* scr */
      /*
       * start timer
       */
      if(ppp_data->ona->restarts > 0)
        ppp_data->ona->restarts--;
      sig_any_rt_rrt_req();
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_RTA_IND unexpected" );
      break;
  }
} /* sig_any_ona_rta_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_rxjn_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_RXJN_IND
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_rxjn_ind (USHORT ptype, T_desc2* packet) 
{ 
  UBYTE index;
  TRACE_ISIG( "sig_any_ona_rxjn_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  /*
   * set error code
   */
  if(ppp_data->ppp_cause EQ 0)
  {
    switch(ptype)
    {
      case DTI_PID_LCP:
        ppp_data->ppp_cause = PPP_TERM_LCP_NOT_CONVERGE;
        break;
      case DTI_PID_IPCP:
        ppp_data->ppp_cause = PPP_TERM_IPCP_NOT_CONVERGE;
        break;
    }
  }

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSING:
      SET_STATE( PPP_SERVICE_ONA, ONA_CLOSED );
      arb_discard_packet(packet);
      /*
       * stop timer
       */
      sig_any_rt_srt_req();
      /*
       * send This-Layer-Finished signal
       */
      if (ptype EQ DTI_PID_LCP) 
      {
        sig_ona_lcp_tlf_req();
      } 
      else if (ptype EQ DTI_PID_IPCP) 
      {
        sig_ona_ncp_tlf_req();
      }
      break;
    case ONA_OPENED:
      SET_STATE( PPP_SERVICE_ONA, ONA_CLOSING );
      /*
       * send This-Layer-Down signal
       */
      if (ptype EQ DTI_PID_LCP) 
      {
        sig_ona_lcp_tld_req();
      } 
      else if (ptype EQ DTI_PID_IPCP) 
      {
        sig_ona_ncp_tld_req();
      }
      ppp_data->ona = & ppp_data->ona_base[index];
    case ONA_REQ_SENT:
    case ONA_ACK_RCVD:
    case ONA_ACK_SENT:
      SET_STATE( PPP_SERVICE_ONA, ONA_CLOSING );
      /*
       * set counter
       */
      ppp_data->ona->restarts = ppp_data->mt;
      /*
       * send terminate request packet
       */
      sig_any_ftx_packet_req(ptype, packet); /* str */
      /*
       * start timer
       */
      if(ppp_data->ona->restarts > 0)
        ppp_data->ona->restarts--;
      sig_any_rt_rrt_req();
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_RXJN_IND unexpected" );
      arb_discard_packet(packet);
      break;
  }
} /* sig_any_ona_rxjn_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_rcrn_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_RCRN_IND
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|               newFlag - flag
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_rcrn_ind (USHORT ptype, T_desc2* packet, UBYTE newFlag) 
{ 
  UBYTE    index;
  T_desc2* req_packet = NULL;

  TRACE_ISIG( "sig_any_ona_rcrn_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSING:
      arb_discard_packet(packet);
      break;
    case ONA_REQ_SENT:
    case ONA_ACK_SENT:
      if (ppp_data->ona->failures EQ 0)
      {
        /*
         * protocol not converge
         */
        SET_STATE( PPP_SERVICE_ONA, ONA_CLOSING );
        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
        {
          switch(ptype)
          {
            case DTI_PID_LCP:
              ppp_data->ppp_cause = PPP_TERM_LCP_NOT_CONVERGE;
              break;
            case DTI_PID_IPCP:
              ppp_data->ppp_cause = PPP_TERM_IPCP_NOT_CONVERGE;
              break;
          }
        }
        arb_discard_packet(packet);
        /*
         * set counter
         */
        ppp_data->ona->restarts = ppp_data->mt;
        /*
         * request terminate request packet
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_str_req(&packet);
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_str_req(&packet);
        }
        /*
         * send terminate request
         */
        sig_any_ftx_packet_req(ptype, packet); /* str */
        /*
         * start timer
         */
        if(ppp_data->ona->restarts > 0)
          ppp_data->ona->restarts--;
        sig_any_rt_rrt_req();
      }
      else
      {
        SET_STATE( PPP_SERVICE_ONA, ONA_REQ_SENT );
        /*
         * send configure nak packet
         */
        sig_any_ftx_packet_req(ptype, packet); /* scn */
        if (newFlag EQ TRUE)
        {
          ppp_data->ona->failures--;
        }
      }
      break;
    case ONA_ACK_RCVD:
      if (ppp_data->ona->failures EQ 0)
      {
        /*
         * protocol not converge
         */
        SET_STATE( PPP_SERVICE_ONA, ONA_CLOSING );
        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
        {
          switch(ptype)
          {
            case DTI_PID_LCP:
              ppp_data->ppp_cause = PPP_TERM_LCP_NOT_CONVERGE;
              break;
            case DTI_PID_IPCP:
              ppp_data->ppp_cause = PPP_TERM_IPCP_NOT_CONVERGE;
              break;
          }
        }
        arb_discard_packet(packet);
        /*
         * set counter
         */
        ppp_data->ona->restarts = ppp_data->mt;
        /*
         * request terminate request packet
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_str_req(&packet);
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_str_req(&packet);
        }
        /*
         * send terminate request packet
         */
        sig_any_ftx_packet_req(ptype, packet); /* str */
        /*
         * start timer
         */
        if(ppp_data->ona->restarts > 0)
          ppp_data->ona->restarts--;
        sig_any_rt_rrt_req();
      }
      else
      {
        /*
         * send configure nak packet
         */
        sig_any_ftx_packet_req(ptype, packet); /* scn */
        if (newFlag EQ TRUE)
        {
          ppp_data->ona->failures--;
          sig_any_rt_rrt_req();
        }
      }
      break;
    case ONA_OPENED:
      SET_STATE( PPP_SERVICE_ONA, ONA_REQ_SENT );
      /*
       * send This_Layer_Down signal and
       * request configure request packet
       */
      if (ptype EQ DTI_PID_LCP) 
      {
        sig_ona_lcp_tld_req();
        sig_ona_lcp_scr_req(&req_packet);
      } 
      else if (ptype EQ DTI_PID_IPCP) 
      {
        sig_ona_ncp_tld_req();
        sig_ona_ncp_scr_req(&req_packet);
      }
      ppp_data->ona = & ppp_data->ona_base[index];
      /*
       * send configure request packet
       */
      sig_any_ftx_packet_req(ptype, req_packet); /* scr */
      if(ppp_data->ona->restarts > 0)
        ppp_data->ona->restarts--;
      if ((newFlag EQ TRUE) && (ppp_data->ona->failures > 0))
      {
        ppp_data->ona->failures--;
      }
      /*
       * send configure nak packet
       */
      sig_any_ftx_packet_req(ptype, packet); /* scn */
      /*
       * start timer
       */
      sig_any_rt_rrt_req();
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_RCRN_IND unexpected" );
      break;
  }
} /* sig_any_ona_rcrn_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_rcrp_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_RCRP_IND
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_rcrp_ind (USHORT ptype, T_desc2* packet) 
{ 
  UBYTE index;
  T_desc2* req_packet = NULL;
  TRACE_ISIG( "sig_any_ona_rcrp_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSING:
      arb_discard_packet(packet);
      break;
    case ONA_REQ_SENT:
    case ONA_ACK_SENT:
      SET_STATE( PPP_SERVICE_ONA, ONA_ACK_SENT );
      /*
       * set counter
       */
      ppp_data->ona->failures = ppp_data->mf;
      /*
       * send configure acknowledge packet
       */
      sig_any_ftx_packet_req(ptype, packet); /* sca */
      break;
    case ONA_ACK_RCVD:
      SET_STATE( PPP_SERVICE_ONA, ONA_OPENED );
      /*
       * stop timer
       */
      sig_any_rt_srt_req();
      /*
       * reset counter
       */
      ppp_data->ona->failures = ppp_data->mf;
      /*
       * send configure acknowledge packet
       */
      sig_any_ftx_packet_req(ptype, packet); /* sca */
      /*
       * send This-Layer_Up signal
       */
      if (ptype EQ DTI_PID_LCP) 
      {
        sig_ona_lcp_tlu_req();
      } 
      else if (ptype EQ DTI_PID_IPCP) 
      {
        sig_ona_ncp_tlu_req();
      }
      break;
    case ONA_OPENED:
      SET_STATE( PPP_SERVICE_ONA, ONA_ACK_SENT );
      /*
       * send This-Layer-Down signal and
       * request configure request packet
       */
      if (ptype EQ DTI_PID_LCP) 
      {
        sig_ona_lcp_tld_req();
        sig_ona_lcp_scr_req(&req_packet);
      } 
      else if (ptype EQ DTI_PID_IPCP) 
      {
        sig_ona_ncp_tld_req();
        sig_ona_ncp_scr_req(&req_packet);
      }
      ppp_data->ona = & ppp_data->ona_base[index];
      /*
       * send configure request packet
       */
      sig_any_ftx_packet_req(ptype, req_packet); /* scr */
      if(ppp_data->ona->restarts > 0)
        ppp_data->ona->restarts--;
      /*
       * send configure acknowledge packet
       */
      sig_any_ftx_packet_req(ptype, packet); /* sca */
      /* 
       * restart timer
       */
      sig_any_rt_rrt_req();
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_RCRP_IND unexpected" );
      arb_discard_packet(packet);
      break;
  }
} /* sig_any_ona_rcrp_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_rcn_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_RCN_IND
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_rcn_ind (USHORT ptype, T_desc2* packet) 
{ 
  UBYTE index;
  TRACE_ISIG( "sig_any_ona_rcn_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSING:
      arb_discard_packet(packet);
      break;
    case ONA_REQ_SENT:
    case ONA_ACK_SENT:
      /*
       * reset counter
       */
      ppp_data->ona->restarts = ppp_data->mc;
      /*
       * send configure request packet
       */
      sig_any_ftx_packet_req(ptype, packet); /* scr */
      /*
       * start timer
       */
      if(ppp_data->ona->restarts > 0)
        ppp_data->ona->restarts--;
      sig_any_rt_rrt_req();
      break;
    case ONA_ACK_RCVD:
      SET_STATE( PPP_SERVICE_ONA, ONA_REQ_SENT );
      /*
       * send configure request packet
       */
      sig_any_ftx_packet_req(ptype, packet); /* scr */
      /*
       * start timer
       */
      sig_any_rt_rrt_req();
      break;
    case ONA_OPENED:
      SET_STATE( PPP_SERVICE_ONA, ONA_REQ_SENT );
      /*
       * send This-Layer-Down signal
       */
      if (ptype EQ DTI_PID_LCP) 
      {
        sig_ona_lcp_tld_req();
      } 
      else if (ptype EQ DTI_PID_IPCP) 
      {
        sig_ona_ncp_tld_req();
      }
      ppp_data->ona = & ppp_data->ona_base[index];
      /*
       * send configure request packet
       */
      sig_any_ftx_packet_req(ptype, packet); /* scr */
      /*
       * start timer
       */
      if(ppp_data->ona->restarts > 0)
        ppp_data->ona->restarts--;
      sig_any_rt_rrt_req();
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_RCN_IND unexpected" );
      arb_discard_packet(packet);
      break;
  }
} /* sig_any_ona_rcn_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_rca_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_RCA_IND
|
| Parameters  : ptype  - type of packet
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_rca_ind (USHORT ptype) 
{ 
  UBYTE index;
  T_desc2* temp_desc = NULL;

  TRACE_ISIG( "sig_any_ona_rca_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSING:
    case ONA_ACK_RCVD:
    case ONA_OPENED:
      break;
    case ONA_REQ_SENT:
      if(ppp_data->ona->loops EQ 0)
      {
        /*
         * protocol not converge
         */
        SET_STATE( PPP_SERVICE_ONA, ONA_CLOSING );
        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
        {
          switch(ptype)
          {
            case DTI_PID_LCP:
              ppp_data->ppp_cause = PPP_TERM_LCP_NOT_CONVERGE;
              break;
            case DTI_PID_IPCP:
              ppp_data->ppp_cause = PPP_TERM_IPCP_NOT_CONVERGE;
              break;
          }
        }
        /*
         * set counter
         */
        ppp_data->ona->restarts = ppp_data->mt;
        /*
         * request terminate request packet
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_str_req(&temp_desc);
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_str_req(&temp_desc);
        }
        /*
         * send terminate request
         */
        sig_any_ftx_packet_req(ptype, temp_desc); /* str */
        /*
         * start timer
         */
        if(ppp_data->ona->restarts > 0)
          ppp_data->ona->restarts--;
        sig_any_rt_rrt_req();
      }
      else
      {
        SET_STATE( PPP_SERVICE_ONA, ONA_ACK_RCVD );
        /*
         * reset counters
         */
        ppp_data->ona->restarts = ppp_data->mc;
        ppp_data->ona->loops--;
        /*
         * start timer
         */
        sig_any_rt_rrt_req();
      }
      break;

    case ONA_ACK_SENT:
      if(ppp_data->ona->loops EQ 0)
      {
        /*
         * protocol not converge
         */
        SET_STATE( PPP_SERVICE_ONA, ONA_CLOSING );
        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
        {
          switch(ptype)
          {
            case DTI_PID_LCP:
              ppp_data->ppp_cause = PPP_TERM_LCP_NOT_CONVERGE;
              break;
            case DTI_PID_IPCP:
              ppp_data->ppp_cause = PPP_TERM_IPCP_NOT_CONVERGE;
              break;
          }
        }
        /*
         * set counter
         */
        ppp_data->ona->restarts = ppp_data->mt;
        /*
         * request terminate request packet
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_str_req(&temp_desc);
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_str_req(&temp_desc);
        }
        /*
         * send terminate request
         */
        sig_any_ftx_packet_req(ptype, temp_desc); /* str */
        /*
         * start timer
         */
        if(ppp_data->ona->restarts > 0)
          ppp_data->ona->restarts--;
        sig_any_rt_rrt_req();
      }
      else
      {
        SET_STATE( PPP_SERVICE_ONA, ONA_OPENED );
        /*
         * stop timer
         */
        sig_any_rt_srt_req();
        /*
         * reset counter
         */
        ppp_data->ona->restarts = ppp_data->mc;
        ppp_data->ona->loops--;
        /* 
         * send This_Layer_Up signal
         */
        if (ptype EQ DTI_PID_LCP) 
        {
          sig_ona_lcp_tlu_req();
        } 
        else if (ptype EQ DTI_PID_IPCP) 
        {
          sig_ona_ncp_tlu_req();
        }
      }
      break;

    default:
      TRACE_ERROR( "SIG_ANY_ONA_RCA_IND unexpected" );
      break;
  }
} /* sig_any_ona_rca_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_ona_rxr_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ONA_RXR_IND
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ona_rxr_ind (USHORT ptype, T_desc2* packet) 
{ 
  UBYTE index;
  TRACE_ISIG( "sig_any_ona_rxr_ind" );
  
  /*
   * determine ONA-instance
   */
  ona_get_inst_index(ptype, &index);
  ppp_data->ona = & ppp_data->ona_base[index];

  switch( GET_STATE( PPP_SERVICE_ONA ) )
  {
    case ONA_CLOSING:
    case ONA_REQ_SENT:
    case ONA_ACK_RCVD:
    case ONA_ACK_SENT:
      arb_discard_packet(packet);
      break;
    case ONA_OPENED:
      /*
       * send echo reply packet
       */
      sig_any_ftx_packet_req(ptype, packet); /* ser */
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ONA_RXR_IND unexpected" );
      arb_discard_packet(packet);
      break;
  }
} /* sig_any_ona_rxr_ind() */
