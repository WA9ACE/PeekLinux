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
|             SDL-documentation (CHAP-statemachine)
+-----------------------------------------------------------------------------
*/

#ifndef PPP_CAPF_C
#define PPP_CAPF_C
#endif /* !PPP_CAPF_C */

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
#include <stdlib.h>     /* to get rand */
#include "cl_md5.h"     /* to get MD5 routine from common library */

#ifdef _SIMULATION_
#include <stdio.h>      /* to get sprintf */
#endif /* _SIMULATION_ */

/*==== CONST ================================================================*/

#define CHAP_SUCCESS_LENGTH              (4)
#define CHAP_CHALLENGE_HEADER_SIZE       (5)

/*
 * maximum value length is 64 octets and additional packet header is 5 octets
 */
#define CHAP_CHALLENGE_LEN_MAX_SERVER_MODE    (64 + CHAP_CHALLENGE_HEADER_SIZE)
/*
 * maximum value length is 5 octets for packet header + 16 octets for hash value
 * calculated by MD5 algorithm + login name length
 */

#define CHAP_RESPONSE_LEN_MAX_CLIENT_MODE     (CHAP_CHALLENGE_HEADER_SIZE + \
                                               CHAP_MSG_DIGEST_LEN + \
                                               PPP_LOGIN_NAME_LENGTH)

/*
 * the response packet must be passable to the ISP in protocol configuration
 * options (pco). The maximum length of pco is
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
 * CHAP needs
 * 3 octets for pco overhead and
 * CHAP_CHALLENGE_LENGTH_MAX for challenge packet and
 * 3 octets for pco overhead for response packet.
 * The max. user login name legth is
 * - for client mode 25 octets
 * - for server mode
 */
#define CHAP_RESPONSE_LEN_MAX_SERVER_MODE   (251 - 15 - 11 - 31 - 13 - 3 - \
                                             CHAP_CHALLENGE_LEN_MAX_SERVER_MODE\
                                             - 3)
#define CHAP_CHALLENGE_LEN_MAX_CLIENT_MODE  (251 - 15 - 11 - 31 - 13 - 3 - \
                                             CHAP_RESPONSE_LEN_MAX_CLIENT_MODE\
                                             - 3)


#define CHAP_FAILURE_TEXT           "response packet to long"
#define CHAP_FAILURE_TEXT_LENGTH    sizeof(CHAP_FAILURE_TEXT)
#define CHAP_FAILURE_LENGTH         (4 + CHAP_FAILURE_TEXT_LENGTH)


/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : chap_init
+------------------------------------------------------------------------------
| Description : The function chap_init() initializes Challenge Handshake
|               Authentication Protocol
|
| Parameters  : no parameter
|
+------------------------------------------------------------------------------
*/
GLOBAL void chap_init ()
{
  T_TIME  time_val;

  TRACE_FUNCTION( "chap_init" );

  /*
   * initialize random generator
   */
  vsi_t_time (VSI_CALLER &time_val);
  srand((USHORT) time_val);
  /*
   * initialize values
   */
  ppp_data->chap.counter  = 0;      /* counter for timer restart */
  ppp_data->chap.nci      = 0;      /* new challenge identifier */
  ppp_data->chap.sc       = FALSE;  /* indic. for sending of Challenge packet*/
  ppp_data->chap.rc       = FALSE;  /* indic. for receiving of Challenge pack*/
  ppp_data->chap.sr       = FALSE;  /* indic. for sending of Respons packet */
  ppp_data->chap.rs       = FALSE;  /* indic. for receiving of Success packet */
  ppp_data->chap.c_packet = NULL;   /* CHAP Challenge packet (server mode) */
  ppp_data->chap.r_packet = NULL;   /* CHAP Response packet (server mode) */
  /* CHAP and PAP authentication values  */
  memset((UBYTE*)&ppp_data->login, 0, sizeof(T_login));

  /*
   * Set CHAP service state to CHAP_DOWN
   */
  INIT_STATE( PPP_SERVICE_CHAP , CHAP_DOWN );

} /* chap_init() */



/*
+------------------------------------------------------------------------------
| Function    : chap_fill_out_packet
+------------------------------------------------------------------------------
| Description : The function chap_fill_out_packet() puts a CHAP Challenge and
|               a CHAP Response packet into the protocol configuration list
|
| Parameters  : pco_buf - pco list buffer
|               ptr_pos - position where to write the CHAP packets, this value
|                         must get back to the calling funtion
|
+------------------------------------------------------------------------------
*/
GLOBAL void chap_fill_out_packet (UBYTE pco_buf[], USHORT* ptr_pos)
{
  USHORT  pos;

  TRACE_FUNCTION( "chap_fill_out_packet" );

#ifdef _SIMULATION_
  TRACE_EVENT_P3("parameters: pco_buf[]: %08x, ptr_pos: %08x, pos: %d",
                  pco_buf, ptr_pos, *ptr_pos);
#endif /* _SIMULATION_ */
  /*
   * copy stored challenge packet
   */
    /*
     * Protocol ID
     */
  pos = *ptr_pos;
  pco_buf[pos] = PROTOCOL_CHAP_MSB;
  pos++;
  pco_buf[pos] = PROTOCOL_CHAP_LSB;
  pos++;
    /*
     * Length of Protocol contents
     */
  pco_buf[pos] = (UBYTE)ppp_data->chap.c_packet->len;
  pos++;
      /*
       * copy packet
       */
  memcpy(&pco_buf[pos],
         ppp_data->chap.c_packet->buffer,
         ppp_data->chap.c_packet->len);
  pos += ppp_data->chap.c_packet->len;
  /*
   * copy stored response packet
   */
    /*
     * Protocol ID
     */
  pco_buf[pos] = PROTOCOL_CHAP_MSB;
  pos++;
  pco_buf[pos] = PROTOCOL_CHAP_LSB;
  pos++;
    /*
     * Length of Protocol contents
     */
  pco_buf[pos] = (UBYTE)ppp_data->chap.r_packet->len;
  pos++;
      /*
       * copy packet
       */
  memcpy(&pco_buf[pos],
         ppp_data->chap.r_packet->buffer,
         ppp_data->chap.r_packet->len);
  pos += ppp_data->chap.r_packet->len;
  /*
   * Free memory and clear flags
   */
  arb_discard_packet(ppp_data->chap.r_packet);
  ppp_data->chap.r_packet = NULL;
  ppp_data->chap.sr = FALSE;
  arb_discard_packet(ppp_data->chap.c_packet);
  ppp_data->chap.c_packet = NULL;
  ppp_data->chap.rc = FALSE;

  /*
   * return new position
   */
  *ptr_pos=pos;
} /* chap_fill_out_packet() */



/*
+------------------------------------------------------------------------------
| Function    : chap_get_sc
+------------------------------------------------------------------------------
| Description : The function chap_get_sc() returns a CHAP Challenge packet and
|               stores a copy of this packet.
|
| Parameters  : ptr_packet - returns the Challenge packet
|                            THE MEMORY FOR THE PACKET WILL BE ALLOCATED BY
|                            THIS FUNCTION
|
+------------------------------------------------------------------------------
*/
GLOBAL void chap_get_sc (T_desc2** ptr_packet)
{
  T_desc2* ret_desc;
  USHORT  len_pos;
  USHORT  pos;
  UBYTE   i;
  UBYTE   value_size;

  TRACE_FUNCTION( "chap_get_sc" );
  /*
   * set new identifier
   */
  ppp_data->chap.nci++;
    /*
     * random value length 16 to 64 octets
     */
#ifdef _SIMULATION_
  value_size = 16;
#else /* _SIMULATION_ */
  value_size = (UBYTE)(rand() % 49) + 16;
#endif /* _SIMULATION_ */
  /*
   * Allocate the necessary size for the data descriptor. The size is
   * calculated as follows:
   * - take the size of a descriptor structure
   * - subtract one because of the array buffer[1] to get the size of
   *   descriptor control information
   * - add number of octets of descriptor data (value+header)
   */
  MALLOC(ret_desc, (USHORT)(sizeof(T_desc2) - 1
                    + value_size + CHAP_CHALLENGE_HEADER_SIZE));
  /*
   * fill the packet
   */
  ret_desc->next = (ULONG)NULL;
  pos = 0;
    /*
     * Code field
     */
  ret_desc->buffer[pos] = CODE_CHALLENGE;
  pos++;
    /*
     * Identifier field
     */
  ret_desc->buffer[pos] = ppp_data->chap.nci;/*lint !e415 (Warning -- access of out-of-bounds pointer) */
  pos++;
    /*
     * Length field (store the position)
     */
  len_pos = pos;
  pos++;
  pos++;
    /*
     * value length
     */
  ret_desc->buffer[pos] = value_size;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  pos++;
    /*
     * values
     */
  for(i=0; i < value_size; i++)
  {
#ifdef _SIMULATION_
    ret_desc->buffer[pos] = (UBYTE)(i);
#else /* _SIMULATION_ */
    ret_desc->buffer[pos] = (UBYTE)(rand() & 0xff);/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
#endif /* _SIMULATION_ */
    pos++;
  }
  /*
   * insert packet length
   */
  ret_desc->len = pos;
  ret_desc->buffer[len_pos] = (UBYTE)(pos >> 8);/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
  len_pos++;
  ret_desc->buffer[len_pos] = (UBYTE)(pos & 0x00ff);/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
  /*
   * create second packet to store the values
   */
  arb_discard_packet(ppp_data->chap.c_packet);
  MALLOC (ppp_data->chap.c_packet, (USHORT)(sizeof(T_desc2) - 1
                      + value_size + CHAP_CHALLENGE_HEADER_SIZE));
    /*
     * make a copy of the first packet
     */
  ppp_data->chap.c_packet->next = ret_desc->next;
  ppp_data->chap.c_packet->len  = ret_desc->len;
  memcpy(ppp_data->chap.c_packet->buffer, ret_desc->buffer, ret_desc->len);
  /*
   * return the first packet
   */
  ppp_data->chap.sc = TRUE;
  *ptr_packet = ret_desc;

} /* chap_get_sc() */

/*
+------------------------------------------------------------------------------
| Function    : chap_get_sr
+------------------------------------------------------------------------------
| Description : The function chap_get_sr() returns a CHAP Response packet and
|               stores a copy of this packet.
|
| Parameters  : ptr_packet - returns the Response packet
|                            THE MEMORY FOR THE PACKET WILL BE ALLOCATED BY
|                            THIS FUNCTION
|
+------------------------------------------------------------------------------
*/
GLOBAL void chap_get_sr (T_desc2** ptr_packet)
{
  T_desc2 *ret_packet, *md5_pack;
  USHORT  len_pos;
  USHORT  md5_len;
  USHORT  pos;
  UBYTE   value_size;
  T_desc2* packet = *ptr_packet;

  TRACE_FUNCTION( "chap_get_sr" );

  /*
   * Allocate the necessary size for the data descriptor. The size is
   * calculated as follows:
   * - take the size of a descriptor structure
   * - subtract one because of the array buffer[1] to get the size of
   *   descriptor control information
   * - add number of octets of descriptor data
   */
  MALLOC(ret_packet, (USHORT)(sizeof(T_desc2) - 1 + CHAP_MSG_DIGEST_LEN
                                             + ppp_data->login.name_len));
  /*
   * fill the packet
   */
  ret_packet->next = (ULONG)NULL;
  pos = 0;
    /*
     * Code field
     */
  ret_packet->buffer[pos] = CODE_RESPONSE;
  pos++;
    /*
     * Identifier field
     */
  ret_packet->buffer[pos] = ppp_data->chap.nci;/*lint !e415 (Warning -- access of out-of-bounds pointer) */
  pos++;
    /*
     * Length field (store the position)
     */
  len_pos = pos;
  pos++;
  pos++;
  /*
   * Value length: output of MD5 routine is always
   * CHAP_MSG_DIGEST_LEN := 16 Bytes
   */
  value_size = CHAP_MSG_DIGEST_LEN;
  ret_packet->buffer[pos] = value_size;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  pos++;

  /*
   * Values
   * Message digest
   */
  md5_len = 1 + ppp_data->login.password_len + packet->buffer[4];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  MALLOC(md5_pack, (USHORT)(sizeof(T_desc2) - 1 + md5_len));

  /*
   * Build string to pass to MD5 routine:
   * identifier + user password + challenge message
   */
  md5_pack->buffer[0] = ppp_data->chap.nci;
  memcpy(&md5_pack->buffer[1], ppp_data->login.password,
                               ppp_data->login.password_len);
  memcpy(&md5_pack->buffer[ppp_data->login.password_len+1],
                          &packet->buffer[5], packet->buffer[4]);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  /*
   * Call MD5 routine
   */
  cl_md5(md5_pack->buffer, md5_len, &ret_packet->buffer[pos]);/*lint  !e416 (Warning -- creation of out-of-bounds pointer) */
  pos += CHAP_MSG_DIGEST_LEN;
  MFREE(md5_pack);
  /*
   * insert user name
   */
  memcpy(&ret_packet->buffer[pos], ppp_data->login.name,
                              ppp_data->login.name_len);/*lint  !e416 (Warning -- creation of out-of-bounds pointer) */
  pos += ppp_data->login.name_len;
  /*
   * insert packet length
   */
  ret_packet->len = pos;
  ret_packet->buffer[len_pos] = (UBYTE)(pos >> 8);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  len_pos++;
  ret_packet->buffer[len_pos] = (UBYTE)(pos & 0x00ff);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

  /*
   * return the first packet
   */
  ppp_data->chap.sr = TRUE;
  *ptr_packet = ret_packet;

} /* chap_get_sr() */


/*
+------------------------------------------------------------------------------
| Function    : chap_get_ss
+------------------------------------------------------------------------------
| Description : The function chap_get_ss() returns a CHAP Success packet.
|
| Parameters  : ptr_packet - returns the CHAP Success packet
|                            THE MEMORY FOR THE PACKET WILL BE ALLOCATED BY
|                            THIS FUNCTION
|
+------------------------------------------------------------------------------
*/
GLOBAL void chap_get_ss (T_desc2** ptr_packet)
{
  T_desc2* packet;
  USHORT  pos;

  TRACE_FUNCTION( "chap_get_ss" );

  /*
   * Allocate the necessary size for the data descriptor. The size is
   * calculated as follows:
   * - take the size of a descriptor structure
   * - subtract one because of the array buffer[1] to get the size of
   *   descriptor control information
   * - add number of octets of descriptor data
   */
  MALLOC (packet, (USHORT)(sizeof(T_desc2) - 1 + CHAP_SUCCESS_LENGTH));
  packet->len = CHAP_SUCCESS_LENGTH;
  packet->next=(ULONG)NULL;
  pos=0;
    /*
     * code field
     */
  packet->buffer[pos] = CODE_SUCCESS;
  pos++;
    /*
     * identifier field
     */
  packet->buffer[pos] = ppp_data->chap.nci;/*lint !e415 (Warning -- creation of out-of-bounds pointer) */
  pos++;
    /*
     * length field
     */
  packet->buffer[pos] = 0;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  pos++;
  packet->buffer[pos] = CHAP_SUCCESS_LENGTH;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  /*
   * return created packet
   */
  *ptr_packet = packet;
  /*
   * Announce successful CHAP authentification
   */
  TRACE_EVENT("PPP CHAP: authentification successful");
} /* chap_get_ss() */



/*
+------------------------------------------------------------------------------
| Function    : chap_rc
+------------------------------------------------------------------------------
| Description : The function chap_rc() checks whether the given Challenge
|               packet is valid and if so it returns FORWARD_RC and a
|               Response packet.
|
| Parameters  : ptr_packet - Challenge packet
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void chap_rc (T_desc2** ptr_packet, UBYTE* forward)
{
  T_desc2* packet;
  USHORT   packet_len;
  USHORT   pos;

  TRACE_FUNCTION( "chap_rc" );

  /*
   * this packet can only be reveiced in client mode
   */
  if(ppp_data->mode NEQ PPP_CLIENT)
  {
    TRACE_EVENT("PPP CHAP: unexpected CHAP challenge packet in client mode");
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * check correct length
   */
  packet = *ptr_packet;
  packet_len  = packet->buffer[2] << 8;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len |= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if((packet_len < 5) || (packet_len > packet->len))
  {
    TRACE_EVENT("PPP CHAP: invalid CHAP challenge packet length");
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * check consistence of length of packet and length of log on information,
   */
  pos=4;
  pos+= packet->buffer[pos];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  pos++;
  if(pos > packet_len || packet_len > CHAP_CHALLENGE_LEN_MAX_CLIENT_MODE)
  {
    TRACE_EVENT_P1("PPP CHAP: invalid value size %d in CHAP challenge packet",
                                                                  packet_len);
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * Check if challenge was received already, the length are equal,
   * the identifier wasn't change or if the challenge value is the same
   * to the packet before.
   */
  if((ppp_data->chap.rc) &&
     (packet->buffer[4] EQ ppp_data->chap.c_packet->buffer[4]))/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  {
    if((ppp_data->chap.nci EQ packet->buffer[1]) ||
       (!memcmp(&packet->buffer[5],
        &ppp_data->chap.c_packet->buffer[5],
         packet->buffer[4])))/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
    {
      TRACE_EVENT("PPP CHAP: repeated CHAP challenge packet");
      *forward=FORWARD_DISCARD;
      return;
    }
  }

  /*
   * store identifier
   */
  ppp_data->chap.nci = packet->buffer[1];/*lint !e415 (Warning -- creation of out-of-bounds pointer) */
  /*
   * store challenge packet
   */
  arb_discard_packet(ppp_data->chap.c_packet);
  ppp_data->chap.c_packet = packet;
  /*
   * set indicators
   */
  ppp_data->chap.rc = TRUE;
  ppp_data->chap.sr = FALSE;
  ppp_data->chap.rs = FALSE;
  /*
   * create Response packet and
   * set return values
   */
  chap_get_sr(ptr_packet);
  *forward=FORWARD_RC;

} /* chap_rc() */



/*
+------------------------------------------------------------------------------
| Function    : chap_rr
+------------------------------------------------------------------------------
| Description : The function chap_rr() checks whether the given Response
|               packet is valid and if so it returns FORWARD_RRP and a
|               Success packet.
|
| Parameters  : ptr_packet - Response packet
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void chap_rr (T_desc2** ptr_packet, UBYTE* forward)
{
  T_desc2* packet;
  USHORT  packet_len;
  USHORT  pos;

  TRACE_FUNCTION( "chap_rr" );
  /*
   * this packet can only reveiced in server mode
   */
  if(ppp_data->mode NEQ PPP_SERVER)
  {
    TRACE_EVENT("PPP CHAP: unexpected CHAP Response Packet in client mode");
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * check correct length
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if((packet_len < 5) || (packet_len > packet->len))
  {
    *forward=FORWARD_DISCARD;
    TRACE_EVENT_P1("PPP CHAP: invalid Response Packet length %d",packet_len);
    return;
  }

  /*
   * check consistence of length of packet and length of log on information,
   * check for identifier field and whether a Challenge packet was sent
   *
   */
  pos=4;
  pos+= packet->buffer[pos];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  pos++;
#ifdef TRACE_USER_NAME
  {
    UBYTE *user_name;
    USHORT i;
    USHORT name_len = packet_len - 5 - packet->buffer[4];
    MALLOC(user_name, name_len);
    memcpy(user_name, &packet->buffer[packet_len - name_len - 1], name_len);
    TRACE_EVENT("PPP CHAP: user name:");
    for(i = 0; i < name_len; i++)
    {
      TRACE_EVENT_P1(" *%c* ", user_name[i]);
    }
  }
#endif /*TRACE_USER_NAME */
  if((pos > packet_len)                         ||
     (packet->buffer[1] NEQ ppp_data->chap.nci) ||
     (ppp_data->chap.sc EQ FALSE))/*lint !e415 (Warning -- creation of out-of-bounds pointer) */
  {
    TRACE_EVENT("PPP CHAP: invalid value size or unexpected Response Packet");
    *forward=FORWARD_DISCARD;
    return;
  }

#ifdef TEST_MD5
  {
    /*
     * For testing set dual-up connection  password to "test password"
     */
    UBYTE test_pwd[14] = "test password";
    T_desc2 *md5_pack, *c_pack;
    USHORT md5_len;
    UBYTE digest[16];
    /*
     * Values
     * Message digest
     */
    c_pack = ppp_data->chap.c_packet;
    md5_len = 1 + 13 + c_pack->buffer[4];
    MALLOC(md5_pack, (USHORT)(sizeof(T_desc2) - 1 + md5_len));
    /*
     * Build string to pass to MD5 routine:
     * identifier + user password + challenge message
     */
    md5_pack->buffer[0] = ppp_data->chap.nci;
    memcpy(&md5_pack->buffer[1], test_pwd, 14);
    memcpy(&md5_pack->buffer[14+1], &c_pack->buffer[5], c_pack->buffer[4]);
    /*
     * Call MD5 routine
     */
    cl_md5(md5_pack->buffer, md5_len, &digest[0]);
    if(memcmp(&packet->buffer[5], digest, 16))
    {
      TRACE_EVENT("CHAP ERROR: invalid msg digest in received response");
    } else {
      TRACE_EVENT("CHAP INFO: MD5 test OK");
    }
    MFREE(md5_pack);
  }
#endif /* TEST_MD5 */


  /*
   * store authentication packet
   */
  if(packet_len > CHAP_RESPONSE_LEN_MAX_SERVER_MODE)
  {
    TRACE_EVENT_P1("PPP CHAP: invalid Response Packet length %d, send failure",
                                                                   packet_len);
    /*
     * create Failure packet
     */
      /*
       * change code field
       */
    packet->buffer[0] = CODE_FAILURE;
      /*
       * change length field
       */
    packet->buffer[2] = 0;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
    packet->buffer[3] = CHAP_FAILURE_LENGTH;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
    packet->len = CHAP_FAILURE_LENGTH;
      /*
       * fill out Message
       */
    /*lint -e419 (Warning -- data overrun) */
    memcpy(&packet->buffer[4], CHAP_FAILURE_TEXT, CHAP_FAILURE_TEXT_LENGTH);/*lint !e416 (Warning -- creation of out-of-bounds pointer) */
    /*lint +e419 (Warning -- data overrun) */
    /*
     * set return values
     */
    *forward=FORWARD_RRN;
    *ptr_packet = packet;
    return;
  }
  arb_discard_packet(ppp_data->chap.r_packet);
  ppp_data->chap.r_packet = packet;
  /*
   * create Success packet and
   * set return values
   */
  chap_get_ss(ptr_packet);
  *forward=FORWARD_RRP;
} /* chap_rr() */



/*
+------------------------------------------------------------------------------
| Function    : chap_rs
+------------------------------------------------------------------------------
| Description : The function chap_rs() checks whether the given Success
|               packet is valid and if so it returns FORWARD_RS.
|
| Parameters  : packet  - Success packet
|               forward - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void chap_rs (T_desc2* packet, UBYTE* forward)
{
  USHORT  packet_len;

  TRACE_FUNCTION( "chap_rs" );

  /*
   * this packet can only reveiced in client mode
   */
  if(ppp_data->mode NEQ PPP_CLIENT)
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  /*
   * check correct length and identifier
   */
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if(packet_len > packet->len ||
     ppp_data->chap.nci != packet->buffer[1])/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  {
    TRACE_EVENT("PPP CHAP: invalid CHAP Success");
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * check if Response was sent.
   */
  if(!ppp_data->chap.sr)
  {
    TRACE_EVENT("PPP CHAP: unexpected CHAP Success");
    *forward=FORWARD_DISCARD;
    return;
  }

  /*
   * free authentication packet
   */
  arb_discard_packet(packet);
  /*
   * set return value
   */
  ppp_data->chap.rs = TRUE;
  *forward=FORWARD_RS;
  /*
   * Announce successful CHAP authentification
   */
  TRACE_EVENT("PPP CHAP: authentification successful");

} /* chap_rs() */



/*
+------------------------------------------------------------------------------
| Function    : chap_rf
+------------------------------------------------------------------------------
| Description : The function chap_rf() checks whether the given Failure
|               packet is valid and if so it returns FORWARD_RF.
|
| Parameters  : packet  - Failure packet
|               forward - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void chap_rf (T_desc2* packet, UBYTE* forward)
{
  USHORT  packet_len;

  TRACE_FUNCTION( "chap_rf" );

  /*
   * this packet can only reveiced in client mode
   */
  if(ppp_data->mode NEQ PPP_CLIENT)
  {
    TRACE_EVENT("PPP CHAP: unexpected Failure packet in server mode");
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * check correct length
   */
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if(packet_len > packet->len)
  {
    TRACE_EVENT("PPP CHAP: invalid Failure packet length");
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * Check identifier
   */
  if(ppp_data->chap.nci != packet->buffer[1])/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  {
    TRACE_EVENT("PPP CHAP: invalid identifier in Failure packet");
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * Check if Success was already received
   * or if Response was not sent yet
   */
  if(ppp_data->chap.rs || !ppp_data->chap.sr)
  {
    TRACE_EVENT("PPP CHAP: unexpected Failure");
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * free Failure packet and reset Response receiving flag
   */
  arb_discard_packet(packet);
  ppp_data->chap.sr = FALSE;
  /*
   * set return value
   */
  *forward=FORWARD_RF;
  /*
   * Announce failed CHAP authentification
   */
  TRACE_EVENT("PPP CHAP: authentification failed");

} /* chap_rf() */

#ifdef _SIMULATION_
/*
+------------------------------------------------------------------------------
| Function    : ppp_trace_desc
+------------------------------------------------------------------------------
| Description : The function traces desc
|
| Parameters  : packet  - tracing packet
|
+------------------------------------------------------------------------------
*/
GLOBAL void ppp_trace_desc (T_desc2* packet)
{
  UINT i=0;
  UBYTE *buf=&packet->buffer[0];


  while(i < packet->len)
  {
    if((i+4) < packet->len){
      TRACE_EVENT_P4("0x%02x, 0x%02x, 0x%02x, 0x%02x,",
                      buf[i], buf[i+1],buf[i+2],buf[i+3]);
      i+=4;
    }
    else if((i+3) < packet->len){
      TRACE_EVENT_P3("0x%02x, 0x%02x, 0x%02x,",
                      buf[i], buf[i+1],buf[i+2]);
      i+=3;
    }
    else if((i+2) < packet->len){
      TRACE_EVENT_P2("0x%02x, 0x%02x,",buf[i],buf[i+1]);
      i+=2;
    }
    else if((i+1) <= packet->len){
      TRACE_EVENT_P1("0x%02x, ",buf[i]);
      i+=1;
    }
  }
}
#endif /* _SIMULATION_ */





