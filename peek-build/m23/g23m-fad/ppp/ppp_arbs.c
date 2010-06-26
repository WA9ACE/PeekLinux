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
|             described in the SDL-documentation (ARB-statemachine)
+-----------------------------------------------------------------------------
*/

#ifndef PPP_ARBS_C
#define PPP_ARBS_C
#endif /* !PPP_ARBS_C */

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

#include "ppp_arbf.h"   /* to get function interface from arb */
#include "ppp_lcpf.h"   /* to get function interface from lcp */
#include "ppp_ncpf.h"   /* to get function interface from ncp */
#include "ppp_rts.h"    /* to get signal interface from rt */
#include "ppp_ptxs.h"   /* to get signal interface from ptx */
#include "ppp_ftxs.h"   /* to get signal interface from ftx */
#include "ppp_lcps.h"   /* to get signal interface from lcp */
#include "ppp_ncps.h"   /* to get signal interface from ncp */
#include "ppp_paps.h"   /* to get signal interface from pap */
#include "ppp_caps.h"   /* to get signal interface from chap */
#include "ppp_ftxs.h"   /* to get signal interface from ftx */
#include "ppp_prxs.h"   /* to get signal interface from prx */
#include "ppp_frxs.h"   /* to get signal interface from frx */

#ifdef _SIMULATION_
#include <string.h>     /* to get strcpy */
#endif /* _SIMULATION_ */
/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_ptx_arb_packet_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PTX_ARB_PACKET_IND
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ptx_arb_packet_ind (USHORT ptype, T_desc2* packet)
{
  TRACE_ISIG( "sig_ptx_arb_packet_ind" );

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_LCP_ESTABLISH:
    case ARB_LCP_PDP:
    case ARB_LCP_NCP:
    case ARB_LCP_RECONF:
    case ARB_LCP_MOD:
      /*
       * any non-LCP packets received during this phase must be
       * silently discarded
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          sig_arb_lcp_packet_req(packet);
          break;
        default:
          arb_discard_packet(packet);
          break;
      }
      break;
    case ARB_PAP_ESTABLISH:
      /*
       * only LCP and Authentication packets
       * are allowed during this phase
       * all other packets are silently discarded
       * if we are in client mode and an IPCP Configure Req packet is received,
       * we stop authentication and and start ncp
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          sig_arb_lcp_packet_req(packet);
          break;
        case DTI_PID_PAP:
          sig_arb_pap_packet_req(packet);
          break;
        case DTI_PID_IPCP:
          if (ppp_data->mode EQ PPP_CLIENT)
          {
            UBYTE result;
            UBYTE hc;
            UBYTE msid;

            ncp_analyze_first_ipcp(packet, &result, &hc, &msid);
            if(result EQ TRUE)
            {
              /*
               * start IPCP
               */
              SET_STATE( PPP_SERVICE_ARB, ARB_NCP_ESTABLISH );
              sig_arb_pap_stop_req();
              sig_arb_ncp_open_req();
              sig_arb_ncp_packet_req(packet);
              break;
            }
          }
          /* fall through */
        default:
          arb_discard_packet(packet);
          break;
      }
      break;
    case ARB_PAP_RECONF:
      /*
       * only LCP and Authentication packets
       * are allowed during this phase
       * all other packets are silently discarded
       * if we are in client mode and an IPCP Configure Req packet is received,
       * we stop authentication and and start ncp
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          sig_arb_lcp_packet_req(packet);
          break;
        case DTI_PID_PAP:
          sig_arb_pap_packet_req(packet);
          break;
        case DTI_PID_IPCP:
          if (ppp_data->mode EQ PPP_CLIENT)
          {
            UBYTE result;
            UBYTE hc;
            UBYTE msid;

            ncp_analyze_first_ipcp(packet, &result, &hc, &msid);
            if(result EQ TRUE)
            {
              /*
               * start IPCP
               */
              SET_STATE( PPP_SERVICE_ARB, ARB_NCP_RECONF );
              sig_arb_pap_stop_req();
              sig_arb_ncp_open_req();
              sig_arb_ncp_packet_req(packet);
              break;
            }
          }
          /* fall through */
        default:
          arb_discard_packet(packet);
          break;
      }
      break;
    case ARB_CHAP_ESTABLISH:
      /*
       * only LCP and Authentication packets
       * are allowed during this phase
       * all other packets are silently discarded
       * if we are in client mode and an IPCP Configure Req packet is received,
       * we stop authentication and and start ncp
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          sig_arb_lcp_packet_req(packet);
          break;
        case DTI_PID_CHAP:
          sig_arb_chap_packet_req(packet);
          break;
        case DTI_PID_IPCP:
          if (ppp_data->mode EQ PPP_CLIENT)
          {
            UBYTE result;
            UBYTE hc;
            UBYTE msid;

            ncp_analyze_first_ipcp(packet, &result, &hc, &msid);
            if(result EQ TRUE)
            {
              /*
               * start IPCP
               */
              SET_STATE( PPP_SERVICE_ARB, ARB_NCP_ESTABLISH );
              sig_arb_chap_stop_req();
              sig_arb_ncp_open_req();
              sig_arb_ncp_packet_req(packet);
              break;
            }
          }
          /* fall through */
        default:
          arb_discard_packet(packet);
          break;
      }
      break;
    case ARB_CHAP_RECONF:
      /*
       * only LCP and Authentication packets
       * are allowed during this phase
       * all other packets are silently discarded
       * if we are in client mode and an IPCP Configure Req packet is received,
       * we stop authentication and and start ncp
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          sig_arb_lcp_packet_req(packet);
          break;
        case DTI_PID_CHAP:
          sig_arb_chap_packet_req(packet);
          break;
        case DTI_PID_IPCP:
          if (ppp_data->mode EQ PPP_CLIENT)
          {
            UBYTE result;
            UBYTE hc;
            UBYTE msid;

            ncp_analyze_first_ipcp(packet, &result, &hc, &msid);
            if(result EQ TRUE)
            {
              /*
               * start IPCP
               */
              SET_STATE( PPP_SERVICE_ARB, ARB_NCP_RECONF );
              sig_arb_chap_stop_req();
              sig_arb_ncp_open_req();
              sig_arb_ncp_packet_req(packet);
              break;
            }
          }
          /* fall through */
        default:
          arb_discard_packet(packet);
          break;
      }
      break;
    case ARB_PDP_WAIT:
      /*
       * any protocol which is unsupported by the implementation must be
       * returned in a Protocol-Reject packet
       * LCP and IPCP are allowed
       * other supported protocols are silently discarded
       * arbitration waits for an IPCP Configure-Request packet from
       * the PPP peer to create the list of Protocol Configuration Options
       * and fill in the PPP_PDP_ACTIVATE_IND primitive
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          sig_arb_lcp_packet_req(packet);
          break;
        case DTI_PID_IPCP:
          {
            UBYTE result;
            UBYTE hc;
            UBYTE msid;

            ncp_analyze_first_ipcp(packet, &result, &hc, &msid);
            if(result EQ TRUE)
            {
              PALLOC_SDU(ppp_pdp_activate_ind, PPP_PDP_ACTIVATE_IND, (251UL << 3));
              SET_STATE( PPP_SERVICE_ARB, ARB_PDP_SENT );
              sig_any_rt_srt_req();
              TRACE_EVENT("activating PDP context");
              if(hc EQ PPP_HC_OFF)
              {
                TRACE_EVENT("client does not support header compression");
              }
              else if(hc EQ PPP_HC_VJ)
              {
                TRACE_EVENT("client supports VJ header compression");
              }
              ppp_pdp_activate_ind->ppp_hc=hc;
              ppp_pdp_activate_ind->msid=msid;
              arb_get_pco_list (&ppp_pdp_activate_ind->sdu);
              PSEND (hCommMMI, ppp_pdp_activate_ind);
            }
            /*
             * store the packet
             */
            if(ppp_data->arb.last_ipcp)
            {
              MFREE_DESC2(ppp_data->arb.last_ipcp);
            }
            ppp_data->arb.last_ipcp = packet;
          }
          break;
        case DTI_PID_IP:
        case DTI_PID_CTCP:
        case DTI_PID_UTCP:
        case DTI_PID_PAP:
        case DTI_PID_CHAP:
          arb_discard_packet(packet);
          break;
        default:
          {
            T_desc2* temp_desc;

            arb_get_protocol_reject (ptype, packet, &temp_desc);
            sig_any_ftx_packet_req(DTI_PID_LCP, temp_desc);
          }
          break;
      }
      break;
    case ARB_PDP_SENT:
      /*
       * any protocol which is unsupported by the implementation must be
       * returned in a Protocol-Reject packet
       * only LCP and IPCP are allowed
       * IPCP packets are stored until the context is activated
       * other supported protocols are silently discarded
       * arbitration waits for a response to the context activation
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          sig_arb_lcp_packet_req(packet);
          break;

        case DTI_PID_IPCP:
          if(ppp_data->arb.last_ipcp)
          {
            MFREE_DESC2(ppp_data->arb.last_ipcp);
          }
          ppp_data->arb.last_ipcp = packet;
          break;

        case DTI_PID_IP:
        case DTI_PID_CTCP:
        case DTI_PID_UTCP:
        case DTI_PID_PAP:
        case DTI_PID_CHAP:
          arb_discard_packet(packet);
          break;
        default:
          {
            T_desc2* temp_desc;

            arb_get_protocol_reject (ptype, packet, &temp_desc);
            sig_any_ftx_packet_req(DTI_PID_LCP, temp_desc);
          }
          break;
      }
      break;
    case ARB_NCP_ESTABLISH:
    case ARB_NCP_RECONF:
    case ARB_NCP_MOD:
      /*
       * any protocol which is unsupported by the implementation must be
       * returned in a Protocol-Reject packet
       * LCP and IPCP are allowed during this phases
       * other supported protocols are silently discarded
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          sig_arb_lcp_packet_req(packet);
          break;
        case DTI_PID_IPCP:
          sig_arb_ncp_packet_req(packet);
          break;
        case DTI_PID_IP:
        case DTI_PID_CTCP:
        case DTI_PID_UTCP:
        case DTI_PID_PAP:
        case DTI_PID_CHAP:
          arb_discard_packet(packet);
          break;
        default:
          {
            T_desc2* temp_desc;
            arb_get_protocol_reject (ptype, packet, &temp_desc);
            sig_any_ftx_packet_req(DTI_PID_LCP, temp_desc);
          }
          break;
      }
      break;
    case ARB_IP:
      /*
       * any protocol which is unsupported by the implementation must be
       * returned in a Protocol-Reject packet. If Van Jacobson is not
       * negotiated then such packets must also be returned in a
       * Protocol-Reject packet.
       * LCP and IPCP are allowed during this phases
       * other supported protocols are silently discarded
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          sig_arb_lcp_packet_req(packet);
          break;
        case DTI_PID_IPCP:
          sig_arb_ncp_packet_req(packet);
          break;
        case DTI_PID_CHAP:
          if(ppp_data->mode EQ PPP_CLIENT)
          {
            sig_arb_chap_packet_req(packet);
            break;
          }
          /* else fall trough */
        case DTI_PID_IP:
        case DTI_PID_PAP:
          arb_discard_packet(packet);
          break;
        case DTI_PID_CTCP:
        case DTI_PID_UTCP:
          if(ppp_data->n_hc EQ PPP_HC_VJ)
          {
            arb_discard_packet(packet);
            break;
          }
          /* fall through */
        default:
          {
            T_desc2* temp_desc;

            arb_get_protocol_reject (ptype, packet, &temp_desc);
            sig_any_ftx_packet_req(DTI_PID_LCP, temp_desc);
          }
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_PTX_ARB_PACKET_IND unexpected" );
      arb_discard_packet(packet);
      break;
  }
} /* sig_ptx_arb_packet_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_lcp_arb_rpj_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LCP_ARB_RPJ_IND
|
| Parameters  : ptype  - type of rejected packet
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_lcp_arb_rpj_ind (USHORT ptype)
{
  TRACE_ISIG( "sig_lcp_arb_rpj_ind" );

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_LCP_ESTABLISH:
    case ARB_LCP_PDP:
    case ARB_LCP_NCP:
    case ARB_LCP_RECONF:
    case ARB_LCP_MOD:
      /*
       * Protocol-Reject packets received in any other than the LCP Opened
       * state should be silently discarded
       */
      break;
    case ARB_PAP_ESTABLISH:
    case ARB_PAP_RECONF:
      /*
       * Close the connection if LCP or PAP is rejected. Otherwise the packet
       * should be silently discarded.
       */
      switch(ptype)
      {
        case DTI_PID_PAP:
          /*
           * set error code
           */
          if(ppp_data->ppp_cause EQ 0)
            ppp_data->ppp_cause = PPP_TERM_USE_AUTHED_FAILED;
          /* fall through */
        case DTI_PID_LCP:
          /*
           * set error code
           */
          if(ppp_data->ppp_cause EQ 0)
            ppp_data->ppp_cause = PPP_TERM_PROT_REJ_UNEXPECTED;
          SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
          sig_arb_pap_down_req();
          sig_arb_lcp_close_req();
          break;
      }
      break;
    case ARB_CHAP_ESTABLISH:
    case ARB_CHAP_RECONF:
      /*
       * Close the connection if LCP or CHAP is rejected. Otherwise the packet
       * should be silently discarded.
       */
      switch(ptype)
      {
        case DTI_PID_CHAP:
          /*
           * set error code
           */
          if(ppp_data->ppp_cause EQ 0)
            ppp_data->ppp_cause = PPP_TERM_USE_AUTHED_FAILED;
          /* fall through */
        case DTI_PID_LCP:
          /*
           * set error code
           */
          if(ppp_data->ppp_cause EQ 0)
            ppp_data->ppp_cause = PPP_TERM_PROT_REJ_UNEXPECTED;
          SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
          sig_arb_chap_down_req();
          sig_arb_lcp_close_req();
          break;
      }
      break;
    case ARB_PDP_WAIT:
      /*
       * Close the connection if LCP is rejected. Otherwise the packet
       * should be silently discarded.
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          /*
           * set error code
           */
          if(ppp_data->ppp_cause EQ 0)
            ppp_data->ppp_cause = PPP_TERM_PROT_REJ_UNEXPECTED;
          SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
          sig_any_rt_srt_req();
          sig_arb_lcp_close_req();
          break;
      }
      break;
    case ARB_PDP_SENT:
      /*
       * Close the connection if LCP is rejected. Otherwise the packet
       * should be silently discarded.
       */
      switch(ptype)
      {
        case DTI_PID_LCP:
          /*
           * set error code
           */
          if(ppp_data->ppp_cause EQ 0)
            ppp_data->ppp_cause = PPP_TERM_PROT_REJ_UNEXPECTED;
          SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
          if(ppp_data->arb.last_ipcp)
          {
            MFREE_DESC2(ppp_data->arb.last_ipcp);
            ppp_data->arb.last_ipcp = NULL;
          }
          sig_arb_lcp_close_req();
          break;
      }
      break;
    case ARB_NCP_ESTABLISH:
    case ARB_NCP_RECONF:
    case ARB_NCP_MOD:
      /*
       * Close the connection if LCP or IPCP is rejected. Otherwise the packet
       * should be silently discarded.
       */
      switch(ptype)
      {
        case DTI_PID_IPCP:
        case DTI_PID_LCP:
          /*
           * set error code
           */
          if(ppp_data->ppp_cause EQ 0)
            ppp_data->ppp_cause = PPP_TERM_PROT_REJ_UNEXPECTED;
          SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
          sig_arb_ncp_down_req();
          sig_arb_lcp_close_req();
          break;
      }
      break;
    case ARB_IP:
      /*
       * Close the connection if LCP, IPCP or IP is rejected. Also close the
       * connection if (un)compressed TCP is rejected and header compression
       * is switched on. Otherwise the packet should be silently discarded.
       */
      switch(ptype)
      {
        case DTI_PID_CTCP:
        case DTI_PID_UTCP:
          {
            UBYTE hc, msid;
            ULONG ip, pdns, sdns;
            /*
             * determine whether header compression is switched on
             */
            ncp_get_values (&hc, &msid, &ip, &pdns, &sdns);
            if(hc NEQ PPP_HC_VJ)
              break;
          }
          /* fall through */
        case DTI_PID_LCP:
        case DTI_PID_IPCP:
        case DTI_PID_IP:
          /*
           * set error code
           */
          if(ppp_data->ppp_cause EQ 0)
            ppp_data->ppp_cause = PPP_TERM_PROT_REJ_UNEXPECTED;
          SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
          sig_arb_ftx_blocked_mode_req();
          sig_arb_ptx_blocked_mode_req();
          sig_arb_ncp_down_req();
          sig_arb_lcp_close_req();
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_LCP_ARB_RPJ_IND unexpected" );
      break;
  }
} /* sig_lcp_arb_rpj_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_rt_arb_to_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RT_ARB_TO_IND
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rt_arb_to_ind ()
{
  TRACE_ISIG( "sig_rt_arb_to_ind" );

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_LCP_ESTABLISH:
    case ARB_LCP_PDP:
    case ARB_LCP_NCP:
    case ARB_LCP_RECONF:
    case ARB_LCP_MOD:
      /*
       * lcp time out
       */
      sig_arb_lcp_to_req();
      break;
    case ARB_PAP_ESTABLISH:
    case ARB_PAP_RECONF:
      sig_arb_pap_to_req();
      break;
    case ARB_CHAP_ESTABLISH:
    case ARB_CHAP_RECONF:
      sig_arb_chap_to_req();
      break;
    case ARB_PDP_WAIT:
      {
        UBYTE   ap;
        USHORT  mru;
        ULONG   accm;
        UBYTE   pfc;
        UBYTE   acfc;

        TRACE_EVENT("waiting for IPCP Configure Request packet");
        /*
         * determine authentication protocol
         */
        lcp_get_values(&ap, &mru, &accm, &pfc, &acfc);
        switch(ap)
        {
          case PPP_AP_PAP:
            sig_arb_pap_to_req();
            break;
          case PPP_AP_CHAP:
            sig_arb_chap_to_req();
            break;
          default:
            if(ppp_data->arb.to_counter EQ 0)
            {
              SET_STATE( PPP_SERVICE_ARB, ARB_LCP_ESTABLISH );
              /*
               * set error code
               */
              if(ppp_data->ppp_cause EQ 0)
                ppp_data->ppp_cause = PPP_TERM_IPCP_NOT_STARTED;
              sig_arb_lcp_close_req();
            }
            else
            {
              ppp_data->arb.to_counter--;
              sig_any_rt_rrt_req();
            }
            break;
        }

      }
      break;
    case ARB_NCP_ESTABLISH:
    case ARB_NCP_RECONF:
    case ARB_NCP_MOD:
      /*
       * ncp time out
       */
      sig_arb_ncp_to_req();
      break;
    case ARB_IP:
      if(ppp_data->mode EQ PPP_CLIENT)
      {
        sig_arb_chap_to_req();
        break;
      }
      /* else fall trough */
    default:
      TRACE_ERROR( "SIG_RT_ARB_TO_IND unexpected" );
      break;
  }
} /* sig_rt_arb_to_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_arb_tlf_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ARB_TLF_IND
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_arb_tlf_ind ()
{
  TRACE_ISIG( "sig_any_arb_tlf_ind" );

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_LCP_ESTABLISH:
    case ARB_LCP_PDP:
    case ARB_LCP_NCP:
    case ARB_LCP_RECONF:
    case ARB_LCP_MOD:
      if(ppp_data->arb.last_ipcp)
      {
        MFREE_DESC2(ppp_data->arb.last_ipcp);
        ppp_data->arb.last_ipcp = NULL;
      }
      /*
       * send a PPP_TERMINATE_IND
       */
      {
        PALLOC (ppp_terminate_ind, PPP_TERMINATE_IND);
        SET_STATE( PPP_SERVICE_ARB, ARB_DEAD );
        sig_arb_ncp_down_req();
        sig_arb_pap_down_req();
        sig_arb_chap_down_req();
        sig_arb_lcp_down_req();
        sig_arb_ptx_dead_mode_req();
        sig_arb_ftx_dead_mode_req();
        arb_dti_close_prot_ind();
        arb_dti_close_peer_ind();

        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
          ppp_data->ppp_cause = PPP_TERM_PROT_ERROR_UNSPEC;
        ppp_terminate_ind->ppp_cause = ppp_data->ppp_cause;
        PSEND (hCommMMI, ppp_terminate_ind);
      }
      break;
    case ARB_PDP_WAIT:
      /*
       * set error code
       */
      if(ppp_data->ppp_cause EQ 0)
        ppp_data->ppp_cause = PPP_TERM_IPCP_NOT_STARTED;
      /* fall through */
    case ARB_PAP_ESTABLISH:
    case ARB_PAP_RECONF:
    case ARB_CHAP_ESTABLISH:
    case ARB_CHAP_RECONF:
      /*
       * set error code
       */
      if(ppp_data->ppp_cause EQ 0)
        ppp_data->ppp_cause = PPP_TERM_USE_AUTHED_FAILED;
      /* fall through */
    case ARB_NCP_ESTABLISH:
    case ARB_NCP_RECONF:
    case ARB_NCP_MOD:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      sig_arb_lcp_close_req();
      break;
    default:
      TRACE_ERROR( "SIG_ANY_ARB_TLF_IND unexpected" );
      break;
  }
} /* sig_any_arb_tlf_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_arb_tlu_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_ARB_TLU_IND
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_arb_tlu_ind ()
{

  TRACE_ISIG( "sig_any_arb_tlu_ind" );

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_LCP_ESTABLISH:
      {
        UBYTE   ap;
        USHORT  mru;
        ULONG   accm;
        UBYTE   pfc;
        UBYTE   acfc;

        /*
         * get the LCP values and configure the frame transmission
         */
        lcp_get_values(&ap, &mru, &accm, &pfc, &acfc);
        sig_arb_ftx_parameters_req(accm, pfc, acfc);

        switch(ap)
        {
          case PPP_AP_PAP:
            /*
             * start PAP
             */
            SET_STATE( PPP_SERVICE_ARB, ARB_PAP_ESTABLISH );
            sig_arb_pap_open_req();
            break;
          case PPP_AP_CHAP:
            /*
             * start CHAP
             */
            SET_STATE( PPP_SERVICE_ARB, ARB_CHAP_ESTABLISH );
            sig_arb_chap_open_req();
            break;
          default:
            if (ppp_data->mode EQ PPP_CLIENT)
            {
              /*
               * start IPCP modification
               */
              SET_STATE( PPP_SERVICE_ARB, ARB_NCP_ESTABLISH );
              sig_arb_ncp_open_req();
            }
            else
            {

              /*
               * wait for an IPCP Configure-Request
               */
              SET_STATE( PPP_SERVICE_ARB, ARB_PDP_WAIT );
              /*
               * ensure that authentication will not negotiated next time
               */
              sig_arb_lcp_modify_ap_req(PPP_AP_NO);
              ppp_data->arb.to_counter=ppp_data->mc;
              if(ppp_data->arb.to_counter > 0)
                ppp_data->arb.to_counter--;
              sig_any_rt_rrt_req();
            }
            break;
        }
      }
      break;
    case ARB_LCP_PDP:
      /*
       * this will happen only in server mode
       */
      {
        UBYTE   ap;
        USHORT  mru;
        ULONG   accm;
        UBYTE   pfc;
        UBYTE   acfc;

        /*
         * wait for the answer of context activation
         */
        SET_STATE( PPP_SERVICE_ARB, ARB_PDP_SENT );

        /*
         * get the LCP values and configure the frame transmission
         */
        lcp_get_values(&ap, &mru, &accm, &pfc, &acfc);
        sig_arb_ftx_parameters_req(accm, pfc, acfc);
      }
      break;
    case ARB_LCP_NCP:
      /*
       * this will happen only in server mode
       */
      {
        UBYTE    ap;
        USHORT   mru;
        ULONG    accm;
        UBYTE    pfc;
        UBYTE    acfc;
        T_desc2* temp_desc;

        /*
         * start IPCP establishment
         */
        SET_STATE( PPP_SERVICE_ARB, ARB_NCP_ESTABLISH );

        /*
         * get the LCP values and configure the frame transmission
         */
        lcp_get_values(&ap, &mru, &accm, &pfc, &acfc);
        sig_arb_ftx_parameters_req(accm, pfc, acfc);

        sig_arb_ncp_open_req();
        if(ppp_data->arb.last_ipcp)
        {
          temp_desc               = ppp_data->arb.last_ipcp;
          ppp_data->arb.last_ipcp = NULL;
          sig_arb_ncp_packet_req(temp_desc);
        }
      }
      break;
    case ARB_LCP_RECONF:
      {
        UBYTE   ap;
        USHORT  mru;
        ULONG   accm;
        UBYTE   pfc;
        UBYTE   acfc;

        /*
         * get the LCP values and configure the frame transmission
         */
        lcp_get_values(&ap, &mru, &accm, &pfc, &acfc);
        sig_arb_ftx_parameters_req(accm, pfc, acfc);

        if(ppp_data->mode EQ PPP_CLIENT)
          switch(ap)
          {
            case PPP_AP_PAP:
              /*
               * start PAP
               */
              SET_STATE( PPP_SERVICE_ARB, ARB_PAP_RECONF );
              sig_arb_pap_open_req();
              break;
            case PPP_AP_CHAP:
              /*
               * start CHAP
               */
              SET_STATE( PPP_SERVICE_ARB, ARB_CHAP_RECONF );
              sig_arb_chap_open_req();
              break;
            default:
              /*
               * start IPCP modification
               */
              SET_STATE( PPP_SERVICE_ARB, ARB_NCP_RECONF );
              sig_arb_ncp_open_req();
              break;
          }
        else
        {
          SET_STATE( PPP_SERVICE_ARB, ARB_NCP_RECONF );
          sig_arb_ncp_open_req();
        }
      }
      break;
    case ARB_LCP_MOD:
      /*
       * this will happen only in server mode
       */
      {
        UBYTE   ap;
        USHORT  mru;
        ULONG   accm;
        UBYTE   pfc;
        UBYTE   acfc;

        /*
         * start IPCP modification
         */
        SET_STATE( PPP_SERVICE_ARB, ARB_NCP_MOD );

        /*
         * get the LCP values and configure the frame transmission
         */
        lcp_get_values(&ap, &mru, &accm, &pfc, &acfc);
        sig_arb_ftx_parameters_req(accm, pfc, acfc);

        sig_arb_ncp_open_req();
      }
      break;
    case ARB_PAP_ESTABLISH:
    case ARB_CHAP_ESTABLISH:
      if (ppp_data->mode EQ PPP_CLIENT)
      {
        SET_STATE( PPP_SERVICE_ARB, ARB_NCP_ESTABLISH );
        /*
         * reset error code
         */
        ppp_data->ppp_cause = 0;
        /*
         * start IPCP
         */
        sig_arb_ncp_open_req();
      }
      else
      {
        /*
         * PPP_SERVER
         * wait for an IPCP Configure-Request
         */
        SET_STATE( PPP_SERVICE_ARB, ARB_PDP_WAIT );
        /*
         * ensure that authentication will not negotiated next time
         */
        sig_arb_lcp_modify_ap_req(PPP_AP_NO);
      }
      break;
    case ARB_PAP_RECONF:
    case ARB_CHAP_RECONF:
      /*
       * this will happen only in client mode
       */
      SET_STATE( PPP_SERVICE_ARB, ARB_NCP_RECONF );
      sig_arb_ncp_open_req();
      break;
    case ARB_NCP_ESTABLISH:
      {
        UBYTE   ap;
        USHORT  mru;
        ULONG   accm;
        UBYTE   pfc;
        UBYTE   acfc;
        UBYTE   ppp_hc;
        UBYTE   msid;
        ULONG   ip;
        ULONG   pdns;
        ULONG   sdns;

        PALLOC (ppp_establish_cnf, PPP_ESTABLISH_CNF);
        SET_STATE( PPP_SERVICE_ARB, ARB_IP );
        /*
         * get the LCP and IPCP values and store it
         * into the PPP_ESTABLISH_CNF primitive
         */
        lcp_get_values(&ap, &mru, &accm, &pfc, &acfc);
        ncp_get_values(&ppp_hc, &msid, &ip, &pdns, &sdns);

        ppp_data->n_hc = ppp_hc;

        sig_arb_ftx_ready_mode_req();
        sig_arb_ptx_ready_mode_req();

        ppp_establish_cnf->mru=mru;
        ppp_establish_cnf->ppp_hc=ppp_hc;
        ppp_establish_cnf->msid=msid;
        ppp_establish_cnf->ip=ip;
        ppp_establish_cnf->dns1=pdns;
        ppp_establish_cnf->dns2=sdns;

        TRACE_EVENT( "PPP Established" );

        PSEND (hCommMMI, ppp_establish_cnf);
      }
      break;
    case ARB_NCP_RECONF:
      {
        UBYTE ppp_hc;
        UBYTE msid;
        ULONG ip;
        ULONG pdns;
        ULONG sdns;

        SET_STATE( PPP_SERVICE_ARB, ARB_IP );
        /*
         * get IPCP values to set PTX value
         */
        ncp_get_values(&ppp_hc, &msid, &ip, &pdns, &sdns);

        ppp_data->n_hc = ppp_hc;

        sig_arb_ftx_ready_mode_req();
        sig_arb_ptx_ready_mode_req();
      }
      break;
    case ARB_NCP_MOD:
      /*
       * this will happen only in server mode
       */
      {
        UBYTE ppp_hc;
        UBYTE msid;
        ULONG ip;
        ULONG pdns;
        ULONG sdns;

        PALLOC (ppp_modification_cnf, PPP_MODIFICATION_CNF);
        SET_STATE( PPP_SERVICE_ARB, ARB_IP );
        /*
         * get IPCP values and store it into
         * the PPP_MODIFICATION_CNF primitive
         */
        ncp_get_values(&ppp_hc, &msid, &ip, &pdns, &sdns);

        ppp_data->n_hc = ppp_hc;

        sig_arb_ftx_ready_mode_req();
        sig_arb_ptx_ready_mode_req();

        ppp_modification_cnf->ppp_hc=ppp_hc;
        ppp_modification_cnf->msid=msid;
        PSEND (hCommMMI, ppp_modification_cnf);
      }
      break;

    case ARB_IP:
      if (ppp_data->mode EQ PPP_CLIENT)
        /* 
         * do nothing, funct. was called after CHAP re-authentication 
         */
        break;
      /* else fall through */

    default:
      TRACE_ERROR( "SIG_ANY_ARB_TLU_IND unexpected" );
      break;
  }
} /* sig_any_arb_tlu_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_lcp_arb_tld_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LCP_ARB_TLD_IND
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_lcp_arb_tld_ind ()
{
  TRACE_ISIG( "sig_lcp_arb_tld_ind" );

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_PAP_ESTABLISH:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_ESTABLISH );
      if((ppp_data->ppp_cause EQ 0) && (ppp_data->mode EQ PPP_CLIENT))
        ppp_data->ppp_cause = PPP_TERM_USE_AUTHED_FAILED;
      sig_arb_pap_down_req();
      break;
    case ARB_CHAP_ESTABLISH:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_ESTABLISH );
      if((ppp_data->ppp_cause EQ 0) && (ppp_data->mode EQ PPP_CLIENT))
        ppp_data->ppp_cause = PPP_TERM_USE_AUTHED_FAILED;
      sig_arb_chap_down_req();
      break;
    case ARB_PAP_RECONF:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      sig_arb_pap_down_req();
      break;
    case ARB_CHAP_RECONF:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      sig_arb_chap_down_req();
      break;
    case ARB_PDP_WAIT:
      /*
       * this will happen only in server mode
       */
      {
        UBYTE   ap;
        USHORT  mru;
        ULONG   accm;
        UBYTE   pfc;
        UBYTE   acfc;

        SET_STATE( PPP_SERVICE_ARB, ARB_LCP_ESTABLISH );
        /*
         * determine authentication protocol
         */
        lcp_get_values(&ap, &mru, &accm, &pfc, &acfc);
        switch(ap)
        {
          case PPP_AP_PAP:
            sig_arb_pap_down_req();
            break;
          case PPP_AP_CHAP:
            sig_arb_chap_down_req();
            break;
          default:
            sig_any_rt_srt_req();
            break;
        }
      }
      break;
    case ARB_PDP_SENT:
      /*
       * this will happen only in server mode
       */
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_PDP );
      break;
    case ARB_NCP_ESTABLISH:
      if(ppp_data->mode EQ PPP_CLIENT)
      {
        SET_STATE( PPP_SERVICE_ARB, ARB_LCP_ESTABLISH );
      }
      else
      {
        SET_STATE( PPP_SERVICE_ARB, ARB_LCP_NCP );
      }
      /*
       * get ncp down
       */
      sig_arb_ncp_down_req();
      break;
    case ARB_NCP_RECONF:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      /*
       * get ncp down
       */
      sig_arb_ncp_down_req();
      break;
    case ARB_NCP_MOD:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_MOD );
      /*
       * get ncp down
       */
      sig_arb_ncp_down_req();
      break;
    case ARB_IP:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      sig_arb_ftx_blocked_mode_req();
      sig_arb_ptx_blocked_mode_req();
      /*
       * get ncp down
       */
      sig_arb_ncp_down_req();
      break;
    default:
      TRACE_ERROR( "SIG_LCP_ARB_TLD_IND unexpected" );
      break;
  }
} /* sig_lcp_arb_tld_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ncp_arb_tld_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_NCP_ARB_TLD_IND
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ncp_arb_tld_ind ()
{
  TRACE_ISIG( "sig_ncp_arb_tld_ind" );

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_IP:
      SET_STATE( PPP_SERVICE_ARB, ARB_NCP_RECONF );
      sig_arb_ftx_blocked_mode_req();
      sig_arb_ptx_blocked_mode_req();
      break;
    default:
      TRACE_ERROR( "SIG_NCP_ARB_TLD_IND unexpected" );
      break;
  }
} /* sig_ncp_arb_tld_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_frx_arb_escape_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_FRX_ARB_ESCAPE_IND
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_frx_arb_escape_ind ()
{
  /*
   * free all PPP resources and finish
   */
  PALLOC (ppp_terminate_ind, PPP_TERMINATE_IND);

  TRACE_ISIG( "sig_frx_arb_escape_ind" );

  TRACE_EVENT( "Peer requested Termination by DTR drop or +++" );

  SET_STATE( PPP_SERVICE_ARB, ARB_DEAD );
  if(ppp_data->arb.last_ipcp)
  {
    MFREE_DESC2(ppp_data->arb.last_ipcp);
    ppp_data->arb.last_ipcp = NULL;
  }
  sig_arb_ncp_down_req();
  sig_arb_pap_down_req();
  sig_arb_chap_down_req();
  sig_arb_lcp_down_req();
  sig_arb_ptx_dead_mode_req();
  sig_arb_ftx_dead_mode_req();
  arb_dti_close_prot_ind();
  arb_dti_close_peer_ind();

  /*
   * set error code
   */

  if(ppp_data->ppp_cause EQ 0)
    ppp_data->ppp_cause = PPP_TERM_OK_PEER;
  ppp_terminate_ind->ppp_cause = ppp_data->ppp_cause;
  PSEND (hCommMMI, ppp_terminate_ind);
} /* sig_frx_arb_escape_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_dti_arb_connection_opened_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_DTI_ARB_CONNECTION_OPENED_IND
|
| Parameters  : *hDTI       -   Handle for DTI Database
|               instance    -   DTI Instance
|               inter_face  -   DTI Interface
|               channel     -   DTI Channel
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_dti_arb_connection_opened_ind(DTI_HANDLE hDTI,
                                              U8 instance,
                                              U8 inter_face,
                                              U8 channel)
{
  TRACE_ISIG("sig_dti_arb_connection_opened_ind");

  /*
   * Ok - connected to a DTI Channel
   */
  switch (inter_face)
  {
    case PROT_LAYER:
      {
        PALLOC(dti_connected_ind, PPP_DTI_CONNECTED_IND);
        dti_connected_ind->connected_direction = PPP_DTI_CONN_PROT;
        ppp_data->arb.dti_connect_state_prot   = DTI_IDLE;
        PSEND (hCommMMI, dti_connected_ind);
      }
      sig_arb_prx_dti_connected_req();
    break;

    case PEER_LAYER:
      {
        PALLOC(dti_connected_ind, PPP_DTI_CONNECTED_IND);
        dti_connected_ind->connected_direction  = PPP_DTI_CONN_PEER;
        ppp_data->arb.dti_connect_state_peer    = DTI_IDLE;
        PSEND (hCommMMI, dti_connected_ind);
      }
      sig_arb_frx_dti_connected_req();

      switch (GET_STATE( PPP_SERVICE_ARB ) )
      {
        case ARB_DEAD:
          switch (ppp_data->mode)
          {
            case PPP_TRANSPARENT:
              /*
               * transparent is only ready after both dti connections
               * have been acknowledged open
               */
              break;

            case PPP_SERVER:
              SET_STATE( PPP_SERVICE_ARB, ARB_LCP_ESTABLISH );
              TRACE_EVENT("DTI CONNECTED START ARB_LCP_ESTABLISH SERVER");
              sig_arb_lcp_open_req();
              break;

            case PPP_CLIENT:
              SET_STATE( PPP_SERVICE_ARB, ARB_LCP_ESTABLISH );
              TRACE_EVENT("DTI CONNECTED START ARB_LCP_ESTABLISH CLIENT");
              sig_arb_lcp_open_req();
              break;

            default:
              TRACE_ERROR( "PPP_ESTABLISH_REQ with invalid parameter: mode" );
              break;
          }/* switch (ppp_data->mode) */
          break;

        default:
          TRACE_ERROR( "DTI signal unexpected" );
          break;
      }/* switch( GET_STATE( PPP_SERVICE_ARB ) ) */
    break;

    default:
      TRACE_ERROR( "DTI direction unknown" );
      break;
  }/* switch (inter_face) */

 /*
  * Start transparent mode if both DTI channels are connected
  */
 if(
   (ppp_data->mode EQ PPP_TRANSPARENT) AND
   (ppp_data->arb.dti_connect_state_prot EQ DTI_IDLE) AND
   (ppp_data->arb.dti_connect_state_peer EQ DTI_IDLE) AND
   (GET_STATE( PPP_SERVICE_ARB ) EQ ARB_DEAD)
   )
 {
    SET_STATE( PPP_SERVICE_ARB, ARB_TRANSPARENT );
    TRACE_EVENT("DTI CONNECTED TRANSPARENT");
    {
      PALLOC (ppp_establish_cnf, PPP_ESTABLISH_CNF);
#ifdef _SIMULATION_
      ppp_establish_cnf->mru=PPP_MRU_DEFAULT;
      ppp_establish_cnf->ppp_hc=PPP_HC_OFF;
      ppp_establish_cnf->msid=0;
      ppp_establish_cnf->ip=PPP_IP_DYNAMIC;
      ppp_establish_cnf->dns1=PPP_DNS1_DYNAMIC;
      ppp_establish_cnf->dns2=PPP_DNS2_DYNAMIC;
#endif /* _SIMULATION_ */
      PSEND (hCommMMI, ppp_establish_cnf);
    }
    sig_arb_ptx_transparent_mode_req();
    sig_arb_ftx_transparent_mode_req();
 }
}

/*
+------------------------------------------------------------------------------
| Function    : sig_dti_arb_connection_closed_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal
|               SIG_DTI_FTX_ARB_CONNECTION_CLOSED_IND.
|
| Parameters  : *hDTI       -   Handle for DTI Database
|               instance    -   DTI Instance
|               inter_face  -   DTI Interface
|               channel     -   DTI Channel
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_arb_connection_closed_ind (DTI_HANDLE hDTI,
                                               U8 instance,
                                               U8 inter_face,
                                               U8 channel)
{
  TRACE_ISIG("sig_dti_arb_connection_closed_ind");

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_PAP_ESTABLISH:
    case ARB_PAP_RECONF:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      sig_arb_pap_down_req();
      break;
    case ARB_CHAP_ESTABLISH:
    case ARB_CHAP_RECONF:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      sig_arb_chap_down_req();
      break;
    case ARB_PDP_WAIT:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      sig_any_rt_srt_req();
      break;
  }

  /*
   * free all PPP resources and finish
   */
  {
    PALLOC (ppp_terminate_ind, PPP_TERMINATE_IND);

    SET_STATE( PPP_SERVICE_ARB, ARB_DEAD );
    sig_arb_ncp_down_req();
    sig_arb_pap_down_req();
    sig_arb_chap_down_req();
    sig_arb_lcp_down_req();
    sig_arb_ptx_dead_mode_req();
    sig_arb_ftx_dead_mode_req();

    if(ppp_data->ppp_cause EQ 0)
      ppp_data->ppp_cause = PPP_TERM_NO_CHANNEL;
    ppp_terminate_ind->ppp_cause = ppp_data->ppp_cause;
    PSEND (hCommMMI, ppp_terminate_ind);
  }

  /*
   * Be shure all DTI channels are closed
   */
  arb_dti_close_prot_ind();
  arb_dti_close_peer_ind();
}
