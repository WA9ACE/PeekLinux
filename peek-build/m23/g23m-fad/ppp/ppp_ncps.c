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
|             described in the SDL-documentation (NCP-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_NCPS_C
#define PPP_NCPS_C
#endif /* PPP_NCPS_C */

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

#include "ppp_ncpf.h"   /* to get function interface from ncp */
#include "ppp_arbf.h"   /* to get function interface from arb */
#include "ppp_onas.h"   /* to get signal interface from ona */
#include "ppp_arbs.h"   /* to get signal interface from arb */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ncp_parameters_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_NCP_PARAMETERS_REQ
|
| Parameters  : hc      - IP header compression
|               msid    - max slot identifier
|               ip      - IP address
|               pdns    - primary DNS server address
|               sdns    - secondary DNS server address
|               gateway - Gateway address
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ncp_parameters_req (UBYTE hc,
                                        UBYTE msid,
                                        ULONG ip,
                                        ULONG pdns, 
                                        ULONG sdns,
                                        ULONG gateway)
{
  TRACE_ISIG( "sig_arb_ncp_parameters_req" );

  ppp_data->ncp.req_hc      = hc;
  ppp_data->ncp.req_msid    = msid;
  ppp_data->ncp.req_ip      = ip;
  ppp_data->ncp.req_pdns    = pdns;
  ppp_data->ncp.req_sdns    = sdns;
  ppp_data->ncp.req_gateway = gateway;

} /* sig_arb_ncp_parameters_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ncp_modify_hc_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_NCP_MODIFY_HC_REQ
|
| Parameters  : hc   - IP header compression
|               msid - max slot identifier
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ncp_modify_hc_req (UBYTE hc, UBYTE msid)
{
  TRACE_ISIG( "sig_arb_ncp_modify_hc_req" );

  ppp_data->ncp.req_hc   = hc;
  ppp_data->ncp.req_msid = msid;

} /* sig_arb_ncp_modify_hc_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ncp_packet_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_NCP_PACKET_REQ
|
| Parameters  : packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ncp_packet_req (T_desc2* packet) 
{ 
  UBYTE forward;
  UBYTE isnew = 0;

  TRACE_ISIG( "sig_arb_ncp_packet_req" );
  
  /*
   * depend on the code-field call appropriate function
   */
  switch(packet->buffer[0])
  {
    case CODE_CONF_REQ:
      ncp_rcr(&packet, &isnew, &forward);
      break;
    case CODE_CONF_ACK:
      ncp_rca(packet, &forward);
      break;
    case CODE_CONF_NAK:
      ncp_rcn(&packet, &forward);
      break;
    case CODE_CONF_REJ:
      ncp_rcj(&packet, &forward);
      break;
    case CODE_TERM_REQ:
      ncp_rtr(&packet, &forward);
      break;
    case CODE_TERM_ACK:
      ncp_rta(packet, &forward);
      break;
    case CODE_CODE_REJ:
      ncp_rxj(&packet, &forward);
      break;
    default:
      ncp_ruc(&packet, &forward);
      break;
  }
  switch(forward)
  {
    case FORWARD_RCRP:
      sig_any_ona_rcrp_ind(DTI_PID_IPCP, packet);
      break;
    case FORWARD_RCRN:
      sig_any_ona_rcrn_ind(DTI_PID_IPCP, packet, isnew);
      break;
    case FORWARD_RCA:
      sig_any_ona_rca_ind(DTI_PID_IPCP);
      break;
    case FORWARD_RCN:
      sig_any_ona_rcn_ind(DTI_PID_IPCP, packet);
      break;
    case FORWARD_RTR:
      sig_any_ona_rtr_ind(DTI_PID_IPCP, packet);
      break;
    case FORWARD_RTA:
      sig_any_ona_rta_ind(DTI_PID_IPCP);
      break;
    case FORWARD_RXJN:
      sig_any_ona_rxjn_ind(DTI_PID_IPCP, packet);
      break;
    case FORWARD_RUC:
      sig_any_ona_ruc_ind(DTI_PID_IPCP, packet);
      break;
    default:
      arb_discard_packet(packet);
      break;
  }
} /* sig_arb_ncp_packet_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ncp_open_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_NCP_OPEN_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ncp_open_req () 
{ 
  T_desc2* packet;
  TRACE_ISIG( "sig_arb_ncp_open_req" );

  /*
   * reset all rejected configuration options
   */
  ppp_data->ncp.s_rejected = 0;
  /*
   * set configuration start values
   */
  ppp_data->ncp.s_hc      = ppp_data->ncp.req_hc;
  ppp_data->ncp.s_msid    = ppp_data->ncp.req_msid;
  ppp_data->ncp.n_ip      = ppp_data->ncp.req_ip;
  ppp_data->ncp.n_pdns    = ppp_data->ncp.req_pdns;
  ppp_data->ncp.n_sdns    = ppp_data->ncp.req_sdns;
  ppp_data->ncp.n_gateway = ppp_data->ncp.req_gateway;
  
  ppp_data->ncp.scr = FALSE;
  ppp_data->ncp.str = FALSE;
  ppp_data->ncp.rcr = FALSE;
  /*
   * set new identifier
   */
  ppp_data->ncp.nscri++;
  /*
   * create configure request packet
   */
  ncp_get_scr(&packet);
  /*
   * inform ONA about the open signal
   */
  sig_any_ona_open_ind(DTI_PID_IPCP, packet);

} /* sig_arb_ncp_open_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ncp_close_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_NCP_CLOSE_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ncp_close_req () 
{ 
  T_desc2* packet;

  TRACE_ISIG( "sig_arb_ncp_close_req" );
  
  /*
   * set new identifier
   */
  ppp_data->ncp.nstri++;
  /*
   * create terminate request packet
   */
  ncp_get_str(&packet);
  /*
   * inform ONA about the close signal
   */
  sig_any_ona_close_ind(DTI_PID_IPCP, packet);

} /* sig_arb_ncp_close_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ncp_down_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_NCP_DOWN_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ncp_down_req () 
{ 
  TRACE_ISIG( "sig_arb_ncp_down_req" );
  
  /*
   * inform ONA about the down signal
   */
  sig_any_ona_down_ind(DTI_PID_IPCP);
} /* sig_arb_ncp_down_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ncp_to_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_NCP_TO_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ncp_to_req () 
{ 
  TRACE_ISIG( "sig_arb_ncp_to_req" );
  
  /*
   * inform ONA about the time out signal
   */
  sig_any_ona_to_ind(DTI_PID_IPCP);
} /* sig_arb_ncp_to_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ona_ncp_tld_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ONA_NCP_TLD_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ona_ncp_tld_req () 
{
  TRACE_ISIG( "sig_ona_ncp_tld_req" );

  /*
   * inform ARB about This-Layer-Down signal
   */
  sig_ncp_arb_tld_ind();
} /* sig_ona_ncp_tld_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ona_ncp_tlf_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ONA_NCP_TLF_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ona_ncp_tlf_req () 
{ 
  TRACE_ISIG( "sig_ona_ncp_tlf_req" );
  
  /*
   * inform ARB about This-Layer-Finish signal
   */
  sig_any_arb_tlf_ind();
} /* sig_ona_ncp_tlf_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ona_ncp_str_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ONA_NCP_STR_REQ
|
| Parameters  : pointer on packet
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ona_ncp_str_req (T_desc2** ptr_packet) 
{ 
  TRACE_ISIG( "sig_ona_ncp_str_req" );
  
  /*
   * create terminate request packet
   */
  ncp_get_str(ptr_packet);
} /* sig_ona_ncp_str_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ona_ncp_scr_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ONA_NCP_SCR_REQ
|
| Parameters  : pointer on packet
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ona_ncp_scr_req (T_desc2** ptr_packet) 
{ 
  TRACE_ISIG( "sig_ona_ncp_scr_req" );
  
  /*
   * create terminate request packet
   */
  ncp_get_scr(ptr_packet);
} /* sig_ona_ncp_scr_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ona_ncp_tlu_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ONA_NCP_TLU_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ona_ncp_tlu_req () 
{ 
  TRACE_ISIG( "sig_ona_ncp_tlu_req" );

  /*
   * inform ARB about This-Layer-Up signal
   */
  sig_any_arb_tlu_ind();
} /* sig_ona_ncp_tlu_req() */


