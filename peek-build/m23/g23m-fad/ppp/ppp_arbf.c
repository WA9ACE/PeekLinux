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
|             procedures and functions as described in the 
|             SDL-documentation (ARB-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_ARBF_C
#define PPP_ARBF_C
#endif /* !PPP_ARBF_C */

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
#include "ppp_papf.h"   /* to get function interface from pap */
#include "ppp_capf.h"   /* to get function interface from chap */
#include "ppp_ncpf.h"   /* to get function interface from ncp */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : arb_init
+------------------------------------------------------------------------------
| Description : The function arb_init() inizializes the arbitration (ARB)
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_init ()
{
  TRACE_FUNCTION( "arb_init" );

  /*
   * initialize protocol-reject identifier
   */
  ppp_data->arb.pri = 1;
  /*
   * initialize time out counter
   */
  ppp_data->arb.to_counter = 1;
  /*
   * initialize pointer to last sent IPCP packet
   */
  ppp_data->arb.last_ipcp = NULL;


  INIT_STATE( PPP_SERVICE_ARB , ARB_DEAD );

  /*
   *  DTI channels
   */ 

  ppp_data->arb.dti_connect_state_prot = DTI_CLOSED;
  ppp_data->arb.dti_connect_state_peer = DTI_CLOSED;
} /* arb_init() */



/*
+------------------------------------------------------------------------------
| Function    : arb_discard_packet
+------------------------------------------------------------------------------
| Description : The function arb_discard_packet() frees the memory of 
|               the given packet
|
| Parameters  : packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_discard_packet (T_desc2* packet)
{ 
  T_desc2* temp_desc;
  TRACE_FUNCTION( "arb_discard_packet" );
  
  while(packet NEQ NULL)
  {
    temp_desc=(T_desc2*)packet->next;
    MFREE(packet);
    packet=temp_desc;
  }
} /* arb_discard_packet() */



/*
+------------------------------------------------------------------------------
| Function    : arb_get_protocol_reject
+------------------------------------------------------------------------------
| Description : The function arb_get_protocol_reject() creates 
|               a LCP protocol reject
|
| Parameters  : ptype      - packet type
|               packet     - packet which have to be rejected
|               ptr_packet - returns a LCP protocol reject packet
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_get_protocol_reject (USHORT ptype, 
                                     T_desc2* packet, 
                                     T_desc2** ptr_packet)
{
  USHORT    temp_len;
  T_desc2*   temp_desc;
  T_desc2*   ret_desc;

  TRACE_FUNCTION( "arb_get_protocol_reject" );

  /*
   * 1 octet code field
   * 1 octet identifier field
   * 2 octets length field
   * 2 octets rejected-protocol field
   *----------
   * 6 octets
   */
  MALLOC (ret_desc, (USHORT)(sizeof(T_desc2) - 1 + 6));
  ret_desc->next = (ULONG)packet;
  ret_desc->len  = 6;
  
  /*
   * Code field for protocol reject
   */
  ret_desc->buffer[0] = CODE_PROT_REJ;

  /*
   * Identifier field
   */
  ret_desc->buffer[1] = ppp_data->arb.pri;/*lint !e415 (Warning -- access of out-of-bounds pointer) */

  ppp_data->arb.pri++;

  /*
   * Length field
   */
  temp_len = 0;
  packet   = ret_desc;
  while(packet NEQ NULL)
  {
    temp_len+= packet->len;
    packet   = (T_desc2*)packet->next;
  }
  if(temp_len > PPP_MRU_MIN)
    temp_len=ppp_data->mru;
  ret_desc->buffer[2] = (UBYTE)(temp_len >> 8);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  ret_desc->buffer[3] = (UBYTE)(temp_len & 0x00ff);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

  /*
   * Rejected-Protocol field
   */
  ret_desc->buffer[4] = (UBYTE)(ptype >> 8);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  ret_desc->buffer[5] = (UBYTE)(ptype & 0x00ff);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

  /*
   * cut the rejected packet if it is to long
   */
  packet = ret_desc;
  while(temp_len > packet->len)
  {
    temp_len-= packet->len;
    packet   = (T_desc2*)packet->next;
  }
  packet->len  = temp_len;
  temp_desc    = (T_desc2*)packet->next;
  packet->next = (ULONG)NULL;
  while(temp_desc NEQ NULL)
  {
    packet = (T_desc2*)temp_desc->next;
    MFREE(temp_desc);
    temp_desc = packet;
  }
  /*
   * return created protocol reject packet
   */
  *ptr_packet = ret_desc;

  /*lint -e415 -e416 (Warning -- access/creation of out-of-bounds pointer) */
} /* arb_get_protocol_reject() */



/*
+------------------------------------------------------------------------------
| Function    : arb_get_pco_list
+------------------------------------------------------------------------------
| Description : The function arb_get_pco_list() creates the list of 
|               protocol configuratrion options as shown in 10.5.6.3/GSM 04.08
|
| Parameters  : ptr_pco_list - returns list of protocol configuration options
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_get_pco_list (T_sdu* ptr_pco_list)
{ 
  UBYTE   ap;
  USHORT  mru;
  ULONG   accm;
  UBYTE   pfc;
  UBYTE   acfc;

  TRACE_FUNCTION( "arb_get_pco_list" );

  /*
   * initialize sdu
   */
  ptr_pco_list->o_buf=0;
  ptr_pco_list->l_buf=0;
  
  /*
   * declare Configuration Protocol PPP
   */
  ptr_pco_list->buf[ptr_pco_list->l_buf]=0x80;
  ptr_pco_list->l_buf++;

  /*
   * first packets are LCP packets
   */
  lcp_fill_out_packet(ptr_pco_list->buf, &ptr_pco_list->l_buf);
  /*
   * next packets may be authentication packets
   */
  if(ppp_data->pco_mask & PPP_PCO_MASK_AUTH_PROT)
  {
    lcp_get_values(&ap, &mru, &accm, &pfc, &acfc);
    switch(ap)
    {
      case PPP_AP_PAP:
        /*
         * insert PAP authentication packet
         */
        pap_fill_out_packet(ptr_pco_list->buf, &ptr_pco_list->l_buf);
        break;
      case PPP_AP_CHAP:
        /*
         * insert CHAP authentication packets
         */
        chap_fill_out_packet(ptr_pco_list->buf, &ptr_pco_list->l_buf);
        break;
    }
  }
  /*
   * next packet is the IPCP packet received from PPP peer
   */
  ncp_fill_out_packet(ptr_pco_list->buf, &ptr_pco_list->l_buf);
  /*
   * convert length in octet to length in bit
   */
  ptr_pco_list->l_buf=ptr_pco_list->l_buf << 3;

} /* arb_get_pco_list() */



/*
+------------------------------------------------------------------------------
| Function    : arb_analyze_pco_list
+------------------------------------------------------------------------------
| Description : The function arb_analyze_pco_list() analyzes the given list of
|               protocol configuration options and returns values of interest
|
| Parameters  : pco_list    - list of protocol configuration options
|               ptr_dns1    - returns Primary DNS Server Address
|               ptr_dns2    - returns Secondary DNS Server Address
|               ptr_gateway - returns Gateway Address
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_analyze_pco_list (T_sdu*  pco_list, 
                                  ULONG*  ptr_dns1,
                                  ULONG*  ptr_dns2,
                                  ULONG*  ptr_gateway)
{ 
  USHORT  pos;

  TRACE_FUNCTION( "arb_analyze_pco_list" );
  
  /*
   * initialize return values
   */
  *ptr_dns1    = PPP_PDNS_DEFAULT;
  *ptr_dns2    = PPP_SDNS_DEFAULT;
  *ptr_gateway = PPP_GATEWAY_DEFAULT;
  
  /*
   * analyze pco list
   */
  pco_list->l_buf = pco_list->l_buf >> 3;
  pco_list->o_buf = pco_list->o_buf >> 3;
  pos             = pco_list->o_buf;
  if((pco_list->l_buf > 0) && (pco_list->buf[pos] EQ 0x80))
  {
    pos++;
    while(pos < (pco_list->l_buf + pco_list->o_buf))
    {
      if((pco_list->buf[pos]     EQ PROTOCOL_IPCP_MSB) &&
         (pco_list->buf[pos + 1] EQ PROTOCOL_IPCP_LSB))
      {
        /*
         * determine DNS addresses and Gateway address
         */
        ncp_analyze_pco (pco_list->buf, pos, ptr_dns1, ptr_dns2, ptr_gateway);
      }
      pos+= pco_list->buf[pos + 2] + 3;
    }
  }
} /* arb_analyze_pco_list() */

/*
+------------------------------------------------------------------------------
| Function    : arb_dti_close_prot_ind
+------------------------------------------------------------------------------
| Description : Close the DTI Protocol Channel
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_dti_close_prot_ind ()
{  
  TRACE_FUNCTION("arb_dti_close_prot_ind");
  
  switch (ppp_data->arb.dti_connect_state_prot)
  {    
    case DTI_SETUP:
    case DTI_IDLE:
      dti_close(ppp_data->ppphDTI, PPP_INSTANCE, 
                PROT_LAYER, PROT_CHANNEL, FALSE);
      ppp_data->arb.dti_connect_state_prot = DTI_CLOSED;  
      break;    
    default:
      break;
  }
} /* arb_dti_close_prot_ind */

/*
+------------------------------------------------------------------------------
| Function    : arb_dti_close_peer_ind
+------------------------------------------------------------------------------
| Description : Close the DTI Peer Channel
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_dti_close_peer_ind ()
{  
  TRACE_FUNCTION("arb_dti_close_peer_ind");
  
  switch (ppp_data->arb.dti_connect_state_peer)
  {    
    case DTI_SETUP:
    case DTI_IDLE:
      dti_close(ppp_data->ppphDTI, PPP_INSTANCE,
                PEER_LAYER, PEER_CHANNEL, FALSE);
      ppp_data->arb.dti_connect_state_peer = DTI_CLOSED;
      break;
    default:
      break;
  }
} /* arb_dti_close_peer_ind */
