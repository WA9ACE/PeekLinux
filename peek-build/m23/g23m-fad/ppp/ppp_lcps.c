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
|             described in the SDL-documentation (LCP-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_LCPS_C
#define PPP_LCPS_C
#endif /* PPP_LCPS_C */

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

#include "ppp_lcpf.h"   /* to get function interface from lcp */
#include "ppp_arbf.h"   /* to get function interface from arb */
#include "ppp_onas.h"   /* to get signal interface from ona */
#include "ppp_arbs.h"   /* to get signal interface from arb */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_lcp_parameters_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_LCP_PARAMETERS_REQ
|
| Parameters  : mru  - Maximum Receive Unit
|               ap   - Authentication Protocol
|               accm - Async Control Character Map
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_lcp_parameters_req (USHORT mru, UBYTE ap, ULONG accm)
{ 
  TRACE_ISIG( "sig_arb_lcp_parameters_req" );

  ppp_data->lcp.req_mru=mru;
  ppp_data->lcp.req_accm=accm;
  ppp_data->lcp.req_ap=ap;
  
} /* sig_arb_lcp_parameters_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_lcp_modify_ap_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_LCP_MODIFY_AP_REQ
|
| Parameters  : ap - Authentication Protocol
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_lcp_modify_ap_req (UBYTE ap) 
{ 
  TRACE_ISIG( "sig_arb_lcp_modify_ap_req" );

  ppp_data->lcp.req_ap=ap;

} /* sig_arb_lcp_modify_ap_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_lcp_packet_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_LCP_PACKET_REQ
|
| Parameters  : packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_lcp_packet_req (T_desc2* packet) 
{
  UBYTE forward;
  UBYTE isnew = 0;

  TRACE_ISIG( "sig_arb_lcp_packet_req" );

  /*
   * depend on the code-field call appropriate function
   */
  switch(packet->buffer[0])
  {
    case CODE_CONF_REQ:
      lcp_rcr(&packet, &isnew, &forward);
      break;
    case CODE_CONF_ACK:
      lcp_rca(packet, &forward);
      break;
    case CODE_CONF_NAK:
      lcp_rcn(&packet, &forward);
      break;
    case CODE_CONF_REJ:
      lcp_rcj(&packet, &forward);
      break;
    case CODE_TERM_REQ:
      lcp_rtr(&packet, &forward);
      break;
    case CODE_TERM_ACK:
      lcp_rta(packet, &forward);
      break;
    case CODE_CODE_REJ:
      lcp_rxj(&packet, &forward);
      break;
    case CODE_PROT_REJ:
      lcp_rpj(packet, &forward);
      break;
    case CODE_ECHO_REQ:
      lcp_rer(&packet, &forward);
      break;
    case CODE_ECHO_REP:
    case CODE_DISC_REQ:
      lcp_rep_rdr(packet, &forward);
      break;
    default:
      lcp_ruc(&packet, &forward);
      break;
  }
  switch(forward)
  {
    case FORWARD_RCRP:
      sig_any_ona_rcrp_ind(DTI_PID_LCP, packet);
      break;
    case FORWARD_RCRN:
      sig_any_ona_rcrn_ind(DTI_PID_LCP, packet, isnew);
      break;
    case FORWARD_RCA:
      sig_any_ona_rca_ind(DTI_PID_LCP);
      break;
    case FORWARD_RCN:
      sig_any_ona_rcn_ind(DTI_PID_LCP, packet);
      break;
    case FORWARD_RTR:
      sig_any_ona_rtr_ind(DTI_PID_LCP, packet);
      break;
    case FORWARD_RTA:
      sig_any_ona_rta_ind(DTI_PID_LCP);
      break;
    case FORWARD_RXJN:
      sig_any_ona_rxjn_ind(DTI_PID_LCP, packet);
      break;
    case FORWARD_RUC:
      sig_any_ona_ruc_ind(DTI_PID_LCP, packet);
      break;
    case FORWARD_RER:
      sig_any_ona_rxr_ind(DTI_PID_LCP, packet);
      break;
    case FORWARD_RPJ_LCP:
      sig_lcp_arb_rpj_ind(DTI_PID_LCP);
      break;
    case FORWARD_RPJ_PAP:
      sig_lcp_arb_rpj_ind(DTI_PID_PAP);
      break;
    case FORWARD_RPJ_CHAP:
      sig_lcp_arb_rpj_ind(DTI_PID_CHAP);
      break;
    case FORWARD_RPJ_IPCP:
      sig_lcp_arb_rpj_ind(DTI_PID_IPCP);
      break;
    case FORWARD_RPJ_IP:
      sig_lcp_arb_rpj_ind(DTI_PID_IP);
      break;
    case FORWARD_RPJ_CTCP:
      sig_lcp_arb_rpj_ind(DTI_PID_CTCP);
      break;
    case FORWARD_RPJ_UTCP:
      sig_lcp_arb_rpj_ind(DTI_PID_UTCP);
      break;
    default:
      arb_discard_packet(packet);
      break;
  }
} /* sig_arb_lcp_packet_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_lcp_open_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_LCP_OPEN_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_lcp_open_req () 
{
  T_desc2* packet;
  TRACE_ISIG( "sig_arb_lcp_open_req" );

  /*
   * reset all rejected configuration options
   */
  ppp_data->lcp.s_rejected=0;
  /*
   * set configuration start values
   */
  ppp_data->lcp.s_mru=ppp_data->lcp.req_mru;
  ppp_data->lcp.s_accm=ppp_data->lcp.req_accm;
  switch(ppp_data->lcp.req_ap)
  {
    case PPP_AP_NO:
    case PPP_AP_PAP:
    case PPP_AP_CHAP:
      ppp_data->lcp.n_ap=ppp_data->lcp.req_ap;
      break;
    default:
      ppp_data->lcp.n_ap=PPP_AP_CHAP;
      break;
  }
  ppp_data->lcp.s_pfc=TRUE;
  ppp_data->lcp.s_acfc=TRUE;
  ppp_data->lcp.scr=FALSE;
  ppp_data->lcp.str=FALSE;
  ppp_data->lcp.rcr=FALSE;
  /*
   * set new identifier
   */
  ppp_data->lcp.nscri++;
  /*
   * create configure request packet
   */
  lcp_get_scr(&packet);
  /*
   * inform ONA about the open signal
   */
  sig_any_ona_open_ind(DTI_PID_LCP, packet);

} /* sig_arb_lcp_open_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_lcp_close_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_LCP_CLOSE_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_lcp_close_req () 
{ 
  T_desc2* packet;

  TRACE_ISIG( "sig_arb_lcp_close_req" );
  
  /*
   * set new identifier
   */
  ppp_data->lcp.nstri++;
  /*
   * create terminate request packet
   */
  lcp_get_str(&packet);
  /*
   * inform ONA about the close signal
   */
  sig_any_ona_close_ind(DTI_PID_LCP, packet);

} /* sig_arb_lcp_close_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_lcp_down_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_LCP_DOWN_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_lcp_down_req () 
{ 
  TRACE_ISIG( "sig_arb_lcp_down_req" );
  
  /*
   * inform ONA about the down signal
   */
  sig_any_ona_down_ind(DTI_PID_LCP);
} /* sig_arb_lcp_down_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_lcp_to_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_LCP_TO_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_lcp_to_req () 
{ 
  TRACE_ISIG( "sig_arb_lcp_to_req" );
  
  /*
   * inform ONA about the time out signal
   */
  sig_any_ona_to_ind(DTI_PID_LCP);
} /* sig_arb_lcp_to_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ona_lcp_tld_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ONA_LCP_TLD_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ona_lcp_tld_req()
{
  TRACE_ISIG( "sig_ona_lcp_tld_req" );

  /*
   * inform ARB about This-Layer-Down signal
   */
  sig_lcp_arb_tld_ind();
} /* sig_ona_lcp_tld_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ona_lcp_tlf_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ONA_LCP_TLF_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ona_lcp_tlf_req()
{
  TRACE_ISIG( "sig_ona_lcp_tlf_req" );

  /*
   * inform ARB about This-Layer-Finish signal
   */
  sig_any_arb_tlf_ind();
} /* sig_ona_lcp_tlf_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ona_lcp_str_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ONA_LCP_STR_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ona_lcp_str_req(T_desc2** ptr_packet)
{
  TRACE_ISIG( "sig_ona_lcp_str_req" );

  /*
   * create terminate request packet
   */
  lcp_get_str(ptr_packet);
} /* sig_ona_lcp_str_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ona_lcp_scr_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ONA_LCP_SCR_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ona_lcp_scr_req(T_desc2** ptr_packet)
{
  TRACE_ISIG( "sig_ona_lcp_scr_req" );

  /*
   * create terminate request packet
   */
  lcp_get_scr(ptr_packet);
} /* sig_ona_lcp_scr_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ona_lcp_tlu_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ONA_LCP_TLU_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ona_lcp_tlu_req()
{
  TRACE_ISIG( "sig_ona_lcp_tlu_req" );

  TRACE_EVENT( "LCP Established" );
  if(ppp_data->lcp.n_ap EQ PPP_AP_CHAP)
  {
    TRACE_EVENT("Authentication Protocol: CHAP");
  }
  else if(ppp_data->lcp.n_ap EQ PPP_AP_PAP)
  {
    TRACE_EVENT("Authentication Protocol: PAP");
  }
  else if(ppp_data->lcp.n_ap EQ PPP_AP_NO)
  {
    TRACE_EVENT("Authentication Protocol: no authentication");
  }
  /*
   * inform ARB about This-Layer-Up signal
   */
  sig_any_arb_tlu_ind();
} /* sig_ona_lcp_tlu_req() */


