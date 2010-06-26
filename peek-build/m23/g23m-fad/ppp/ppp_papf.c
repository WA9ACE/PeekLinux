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
|             SDL-documentation (PAP-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_PAPF_C
#define PPP_PAPF_C
#endif /* !PPP_PAPF_C */

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
#include <string.h>     /* to get memcpy */

/*==== CONST ================================================================*/

#define PAP_AUTH_ACK_LENGTH         5

/*
 * the authentication request packet must be passable to the ISP in protocol
 * configuration options (pco). The maximum length of pco is 
 * 251 octets (253 - 2 (identification overhead) = 251). There must also be
 * space for other protocol packet, like LCP and IPCP.
 * LCP needs 
 * 15 octets (3 (overhead pco) + 4 (overhead packet) + 4 (mru) +  4 
 * (authentication protocol PAP) = 15) for server packet and 
 * 11 octets (3 (overhead pco) + 4 (overhead packet) + 4 (mru) = 11) for 
 * client packet.
 * IPCP needs 
 * 31 octets (3 (overhead pco) + 4 (overhead packet) + 6 (IP address) + 6 
 * (VJ header compression) + 6 (primary DNS) + 6 (secondary DNS) = 31) for
 * client packet and
 * 13 octets (3 (overhead pco) + 4 (overhead packet) + 6 
 * (Gateway address) = 13) for gateway address packet.
 * PAP needs 
 * 3 octets for pco overhead.
 */
#define PAP_AUTH_REQ_LENGTH_MAX     (251 - 15 - 11 - 31 - 13 - 3)

#define PAP_AUTH_NAK_TEXT           "authentication packet to long"
#define PAP_AUTH_NAK_TEXT_LENGTH    sizeof(PAP_AUTH_NAK_TEXT)
#define PAP_AUTH_NAK_LENGTH         (5 + PAP_AUTH_NAK_TEXT_LENGTH)

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : pap_init
+------------------------------------------------------------------------------
| Description : The function pap_init() initializes Password Authentication
|               Protocol
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void pap_init ()
{ 
  TRACE_FUNCTION( "pap_init" );
  
  /*
   * initialize values
   */
  ppp_data->pap.counter   = 0;		
  ppp_data->pap.nari      = 0;
  ppp_data->pap.ar_packet = NULL;

  INIT_STATE( PPP_SERVICE_PAP , PAP_DOWN );
} /* pap_init() */



/*
+------------------------------------------------------------------------------
| Function    : pap_fill_out_packet
+------------------------------------------------------------------------------
| Description : The function pap_fill_out_packet() puts a PAP packet into 
|               the protocol configuration list
|
| Parameters  : pco_buf - pco list buffer
|               ptr_pos - position where to write the PAP packet, this value
|                         must get back to the calling funtion
|
+------------------------------------------------------------------------------
*/
GLOBAL void pap_fill_out_packet (UBYTE pco_buf[], USHORT* ptr_pos)
{
  USHORT  pos;

  TRACE_FUNCTION( "pap_fill_out_packet" );

  pos = *ptr_pos;

  /*
   * copy stored authentication request packet
   */
    /*
     * Protocol ID
     */
  pco_buf[pos] = PROTOCOL_PAP_MSB;
  pos++;
  pco_buf[pos] = PROTOCOL_PAP_LSB;
  pos++;
    /*
     * Length of Protocol contents
     */
  pco_buf[pos] = (UBYTE)ppp_data->pap.ar_packet->len;
  pos++;
      /*
       * copy packet
       */
  memcpy(&pco_buf[pos], 
         ppp_data->pap.ar_packet->buffer, 
         ppp_data->pap.ar_packet->len);
  pos+= ppp_data->pap.ar_packet->len;
  /*
   * return new position
   */
  *ptr_pos = pos;
} /* pap_fill_out_packet() */



/*
+------------------------------------------------------------------------------
| Function    : pap_get_sar
+------------------------------------------------------------------------------
| Description : The function pap_get_sar() returns a PAP Authenticate Request
|               packet.
|
| Parameters  : ptr_packet - returns the Authenticate Request packet
|                            THE MEMORY FOR THE PACKET WILL BE ALLOCATED BY
|                            THIS FUNCTION
|
+------------------------------------------------------------------------------
*/
GLOBAL void pap_get_sar (T_desc2** ptr_packet)
{ 
  T_desc2* ret_desc;
  USHORT   len_pos;
  USHORT   pos;
  UBYTE    i;

  TRACE_FUNCTION( "pap_get_sar" );

  /*
   * Allocate the necessary size for the data descriptor. The size is 
   * calculated as follows:
   * - take the size of a descriptor structure
   * - subtract one because of the array buffer[1] to get the size of
   *   descriptor control information
   * - add number of octets of descriptor data
   */
  MALLOC (ret_desc, (USHORT)(sizeof(T_desc2) - 1 + 
                     5 +               /*
                                        * 1 octet  code field
                                        * 1 octet  identifier field
                                        * 2 octets packet length field
                                        * 1 octet  peer id length field
                                        */
                     ppp_data->login.name_len +                     
                     1 +               /* size of password length field */
                     ppp_data->login.password_len));
  /*
   * fill the packet
   */
  ret_desc->next = (ULONG)NULL;
  pos = 0;
    /*
     * Code field
     */
  ret_desc->buffer[pos] = CODE_AUTH_REQ;
  pos++;
    /*
     * Identifier field
     */
  ret_desc->buffer[pos] = ppp_data->pap.nari;/*lint !e415 (Warning -- access of out-of-bounds pointer)*/
  pos++;
    /*
     * Length field (store the position)
     */
  len_pos = pos;
  pos++;
  pos++;
  /*
   * authentication: peer-id and passwort
   */
  ret_desc->buffer[pos] = ppp_data->login.name_len;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  pos++;
  for (i = 0; i < ppp_data->login.name_len; i++)
  {
    ret_desc->buffer[pos] = ppp_data->login.name[i];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
    pos++;
  }
  ret_desc->buffer[pos] = ppp_data->login.password_len;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
  pos++;
  for (i = 0; i < ppp_data->login.password_len; i++)
  {
    ret_desc->buffer[pos] = ppp_data->login.password[i];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
    pos++;
  }
  /*
   * insert packet length
   */
  ret_desc->len = pos;
  ret_desc->buffer[len_pos] = (UBYTE)(pos >> 8);/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
  len_pos++;
  ret_desc->buffer[len_pos] = (UBYTE)(pos & 0x00ff);/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
  
  /*
   * return the created packet
   */
  *ptr_packet = ret_desc;
} /* pap_get_sar() */



/*
+------------------------------------------------------------------------------
| Function    : pap_get_saa
+------------------------------------------------------------------------------
| Description : The function pap_get_saa() returns a PAP Authenticate Ack 
|               packet.
|
| Parameters  : ptr_packet - returns the Authenticate Ack packet
|                            THE MEMORY FOR THE PACKET WILL BE ALLOCATED BY
|                            THIS FUNCTION
|
+------------------------------------------------------------------------------
*/
GLOBAL void pap_get_saa (T_desc2** ptr_packet)
{ 
  T_desc2* packet;
  USHORT   pos;

  TRACE_FUNCTION( "pap_get_saa" );

  /*
   * Allocate the necessary size for the data descriptor. The size is 
   * calculated as follows:
   * - take the size of a descriptor structure
   * - subtract one because of the array buffer[1] to get the size of
   *   descriptor control information
   * - add number of octets of descriptor data
   */
  MALLOC (packet, (USHORT)(sizeof(T_desc2) - 1 + PAP_AUTH_ACK_LENGTH));
  packet->len  = PAP_AUTH_ACK_LENGTH;
  packet->next = (ULONG)NULL;
  pos = 0;
    /*
     * code field
     */
  packet->buffer[pos] = CODE_AUTH_ACK;
  pos++;
    /*
     * identifier field
     */
  packet->buffer[pos] = ppp_data->pap.nari;/*lint !e415 (Warning -- access of out-of-bounds pointer)*/
  pos++;
    /*
     * length field
     */
  packet->buffer[pos] = 0;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  pos++;
  packet->buffer[pos] = PAP_AUTH_ACK_LENGTH;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  pos++;
    /*
     * set Msg-Length field
     */
  packet->buffer[pos] = 0;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  /*
   * return the created packet
   */
  *ptr_packet = packet;
} /* pap_get_saa() */



/*
+------------------------------------------------------------------------------
| Function    : pap_rar
+------------------------------------------------------------------------------
| Description : The function pap_rar() checks whether the given 
|               Authenticate Request packet is valid and if so it returns
|               FORWARD_RARP and an Authentication Ack packet.
|
| Parameters  : ptr_packet - Authenticate Request packet
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void pap_rar (T_desc2** ptr_packet, UBYTE* forward)
{
  T_desc2* packet;
  USHORT  packet_len;
  USHORT  pos;

  TRACE_FUNCTION( "pap_rar" );

  /*
   * this packet can only reveiced in server mode
   */
  if(ppp_data->mode NEQ PPP_SERVER)
  {
    *forward = FORWARD_DISCARD;
    return;
  }
  /*
   * check correct length
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  if((packet_len < 6) || (packet_len > packet->len))
  {
    *forward = FORWARD_DISCARD;
    return;
  }

  /*
   * check consistence of length of packet and length of log on information
   */
  pos = 4;
  pos+= packet->buffer[pos];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  pos++;
  pos+= packet->buffer[pos];
  pos++;
  if(pos > packet_len)
  {
    *forward = FORWARD_DISCARD;
    return;
  }

  /*
   * store authentication packet
   */
  if(packet_len > PAP_AUTH_REQ_LENGTH_MAX)
  {
    /*
     * create Authentication Nak
     */
      /*
       * change code field
       */
    packet->buffer[0] = CODE_AUTH_NAK;
      /*
       * change length field
       */
    packet->buffer[2] = 0;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
    packet->buffer[3] = PAP_AUTH_NAK_LENGTH;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
    packet->len       = PAP_AUTH_NAK_LENGTH;
      /*
       * set Msg-Length field
       */
    packet->buffer[4] = PAP_AUTH_NAK_TEXT_LENGTH;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      /*
       * fill out Message
       */
    memcpy(&packet->buffer[5], PAP_AUTH_NAK_TEXT, PAP_AUTH_NAK_TEXT_LENGTH);/*lint !e419 !e416 (Warning -- creation of out-of-bounds pointer, Apparent data overrun)*/
    /*
     * set return values
     */
    *forward    = FORWARD_RARN;
    *ptr_packet = packet;
    return;
  }
  arb_discard_packet(ppp_data->pap.ar_packet);
  ppp_data->pap.ar_packet = packet;
  ppp_data->pap.nari      = packet->buffer[1];/*lint !e415 (Warning -- access of out-of-bounds pointer)*/
  /*
   * create Authenticate Ack packet and
   * set return values
   */
  pap_get_saa(ptr_packet);
  *forward = FORWARD_RARP;
} /* pap_rar */

  
  
/*
+------------------------------------------------------------------------------
| Function    : pap_raa
+------------------------------------------------------------------------------
| Description : The function pap_raa() checks whether the given 
|               Authenticate Ack packet is valid and if so it returns
|               FORWARD_RAA.
|
| Parameters  : packet  - Authenticate Ack packet
|               forward - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void pap_raa (T_desc2* packet, UBYTE* forward)
{ 
  USHORT  packet_len;

  TRACE_FUNCTION( "pap_raa" );

  /*
   * this packet can only reveiced in client mode
   */
  if(ppp_data->mode NEQ PPP_CLIENT)
  {
    *forward = FORWARD_DISCARD;
    return;
  }
  /*
   * check correct length
   */
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  if((packet_len < 5) || (packet_len > packet->len))
  {
    *forward = FORWARD_DISCARD;
    return;
  }

  if(packet->buffer[1] NEQ ppp_data->pap.nari)/*lint !e415 (Warning -- access of out-of-bounds pointer)*/
  {
    /*
     * invalid packet
     */
    *forward = FORWARD_DISCARD;
    return;
  }
  /*
   * acceptable packet
   */
  arb_discard_packet(packet);
  ppp_data->pap.nari++;
  *forward = FORWARD_RAA;
} /* pap_raa */



/*
+------------------------------------------------------------------------------
| Function    : pap_ran
+------------------------------------------------------------------------------
| Description : The function pap_ran() checks whether the given 
|               Authenticate Nak packet is valid and if so it returns
|               FORWARD_RAN.
|
| Parameters  : packet  - Authenticate Nak packet
|               forward - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void pap_ran (T_desc2* packet, UBYTE* forward)
{ 
  USHORT  packet_len;

  TRACE_FUNCTION( "pap_ran" );
  
  /*
   * this packet can only reveiced in client mode
   */
  if(ppp_data->mode NEQ PPP_CLIENT)
  {
    *forward = FORWARD_DISCARD;
    return;
  }
  /*
   * check correct length
   */
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  if((packet_len < 5) || (packet_len > packet->len))
  {
    *forward = FORWARD_DISCARD;
    return;
  }

  if(packet->buffer[1] NEQ ppp_data->pap.nari)/*lint !e415 (Warning -- access of out-of-bounds pointer)*/
  {
    /*
     * invalid packet
     */
    *forward = FORWARD_DISCARD;
    return;
  }
  /*
   * acceptable packet
   */
  arb_discard_packet(packet);
  *forward = FORWARD_RAN;
} /* pap_ran */
