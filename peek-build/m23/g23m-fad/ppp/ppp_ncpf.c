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
|             SDL-documentation (NCP-statemachine)
+-----------------------------------------------------------------------------
*/

#define ENTITY_PPP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"     /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"        /* to get the DTILIB definitions */
#include "ppp.h"        /* to get the global entity definitions */

#include <string.h>     /* to get memcpy */
#include "ppp_arbf.h"   /* to get function interface from arb */

/*==== CONST ================================================================*/

#define TYPE_HC                   2
#define TYPE_IP                   3
#define TYPE_PDNS                 129
#define TYPE_SDNS                 131
#define TYPE_ADJUST               125

#define MASK_TYPE_HC              TYPE_HC
#define MASK_TYPE_IP              TYPE_IP
#define MASK_TYPE_PDNS            (TYPE_PDNS - TYPE_ADJUST)
#define MASK_TYPE_SDNS            (TYPE_SDNS - TYPE_ADJUST)

#define LENGTH_HC_VJ              6
#define LENGTH_HC_MAX             LENGTH_HC_VJ
#define LENGTH_IP                 6
#define LENGTH_PDNS               6
#define LENGTH_SDNS               6

#define NCP_CONF_REQ_LENGTH_MAX   (4 +              \
                                   LENGTH_HC_MAX +  \
                                   LENGTH_IP +      \
                                   LENGTH_PDNS +    \
                                   LENGTH_SDNS)
#define NCP_TERM_REQ_LENGTH       6

#define PPP_CSID_YES              1

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : ncp_init
+------------------------------------------------------------------------------
| Description : The function ncp_init() initialize Network Control Protocol
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_init ()
{
  TRACE_FUNCTION( "ncp_init" );
  /*
   * initialize values
   */
  ppp_data->ncp.req_hc      = PPP_HC_DEFAULT;
  ppp_data->ncp.req_msid    = PPP_MSID_DEFAULT;
  ppp_data->ncp.req_ip      = PPP_IP_DEFAULT;
  ppp_data->ncp.req_pdns    = PPP_PDNS_DEFAULT;
  ppp_data->ncp.req_sdns    = PPP_SDNS_DEFAULT;
  ppp_data->ncp.req_gateway = PPP_GATEWAY_DEFAULT;

  ppp_data->ncp.s_hc      = PPP_HC_DEFAULT;
  ppp_data->ncp.s_msid    = PPP_MSID_DEFAULT;
  ppp_data->ncp.r_hc      = PPP_HC_DEFAULT;
  ppp_data->ncp.r_msid    = PPP_MSID_DEFAULT;
  ppp_data->ncp.n_ip      = PPP_IP_DEFAULT;
  ppp_data->ncp.n_pdns    = PPP_PDNS_DEFAULT;
  ppp_data->ncp.n_sdns    = PPP_SDNS_DEFAULT;
  ppp_data->ncp.n_gateway = PPP_GATEWAY_DEFAULT;

  ppp_data->ncp.s_rejected = 0;

  ppp_data->ncp.nscri = 0;
  ppp_data->ncp.nstri = 0;
  ppp_data->ncp.nscji = 0;

  ppp_data->ncp.scr=FALSE;
  ppp_data->ncp.str=FALSE;
  ppp_data->ncp.rcr=FALSE;

  INIT_STATE( PPP_SERVICE_NCP , NCP_STATE );
} /* ncp_init() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_get_values
+------------------------------------------------------------------------------
| Description : The function ncp_get_values() returns negotiated values
|
| Parameters  : ptr_hc   - returns IP Header Compression
|               ptr_msid - returns max slot identifier
|               ptr_ip   - returns IP address
|               ptr_pdns - returns primary DNS server address
|               ptr_pdns - returns secondary DNS server address
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_get_values (UBYTE* ptr_hc, UBYTE* ptr_msid, ULONG* ptr_ip,
                            ULONG* ptr_pdns, ULONG* ptr_sdns)
{
  TRACE_FUNCTION( "ncp_get_values" );

  *ptr_hc   = ppp_data->ncp.r_hc;
  if(ppp_data->ncp.r_msid < ppp_data->ncp.s_msid)
    *ptr_msid = ppp_data->ncp.r_msid;
  else
    *ptr_msid = ppp_data->ncp.s_msid;
  *ptr_ip   = ppp_data->ncp.n_ip;
  *ptr_pdns = ppp_data->ncp.n_pdns;
  *ptr_sdns = ppp_data->ncp.n_sdns;

} /* ncp_get_values() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_get_scr
+------------------------------------------------------------------------------
| Description : The function ncp_get_scr() creates a NCP Configure Request
|               packet.
|
| Parameters  : ptr_packet - returns the Configure Request packet
|                            THE MEMORY FOR THE PACKET WILL ALLOCATED BY
|                            THIS FUNCTION
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_get_scr (T_desc2** ptr_packet)
{
  T_desc2* ret_desc;
  USHORT   len_pos;
  USHORT   pos;

  TRACE_FUNCTION( "ncp_get_scr" );

  /*
   * Allocate the necessary size for the data descriptor. The size is
   * calculated as follows:
   * - take the size of a descriptor structure
   * - subtract one because of the array buffer[1] to get the size of
   *   descriptor control information
   * - add number of octets of descriptor data
   */
  MALLOC (ret_desc, (USHORT)(sizeof(T_desc2) - 1 + NCP_CONF_REQ_LENGTH_MAX));
  /*
   * fill the packet
   */
  ret_desc->next=(ULONG)NULL;
  pos=0;
    /*
     * Code field
     */
  ret_desc->buffer[pos]=CODE_CONF_REQ;
  pos++;
    /*
     * Identifier field
     */
  ret_desc->buffer[pos]=ppp_data->ncp.nscri;/*lint !e415 access of out-of-bounds pointer */
  pos++;
    /*
     * Length field (store the position)
     */
  len_pos=pos;
  pos++;
  pos++;
      /*
       * Header Compression Protocol (only server mode)
       */
  if(((ppp_data->ncp.s_rejected & (1UL << MASK_TYPE_HC)) EQ 0) &&
     (ppp_data->ncp.s_hc NEQ PPP_HC_DEFAULT))
  {
    if(ppp_data->ncp.s_hc EQ PPP_HC_VJ)
    {
      ret_desc->buffer[pos] = TYPE_HC;/*lint !e415 !e416 access of out-of-bounds pointer */
      pos++;
      ret_desc->buffer[pos] = LENGTH_HC_VJ;/*lint !e415 !e416 access of out-of-bounds pointer */
      pos++;
      ret_desc->buffer[pos] = PROTOCOL_VJ_MSB;/*lint !e415 !e416 access of out-of-bounds pointer */
      pos++;
      ret_desc->buffer[pos] = PROTOCOL_VJ_LSB;/*lint !e415 !e416 access of out-of-bounds pointer */
      pos++;
      ret_desc->buffer[pos] = ppp_data->ncp.s_msid;/*lint !e415 !e416 access of out-of-bounds pointer */
      pos++;
      ret_desc->buffer[pos] = PPP_CSID_YES;/*lint !e415 !e416 access of out-of-bounds pointer */
      pos++;
    }
    /*
     * include additional header compression protocols
     */
  }
      /*
       * IP Address (only client mode)
       */
  if(((ppp_data->ncp.s_rejected & (1UL << MASK_TYPE_IP)) EQ 0) &&
     (ppp_data->mode EQ PPP_CLIENT))
  {
    ret_desc->buffer[pos]=TYPE_IP;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=LENGTH_IP;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->ncp.n_ip >> 24);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)((ppp_data->ncp.n_ip >> 16) & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)((ppp_data->ncp.n_ip >>  8) & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->ncp.n_ip & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
  }
      /*
       * Gateway Address (only server mode)
       */
  if(((ppp_data->ncp.s_rejected & (1UL << MASK_TYPE_IP)) EQ 0) &&
     (ppp_data->mode EQ PPP_SERVER)                            &&
     (ppp_data->ncp.n_gateway NEQ PPP_GATEWAY_DEFAULT))
  {
    ret_desc->buffer[pos]=TYPE_IP;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=LENGTH_IP;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->ncp.n_gateway >> 24);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)((ppp_data->ncp.n_gateway >> 16) & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)((ppp_data->ncp.n_gateway >>  8) & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->ncp.n_gateway & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
  }
      /*
       * Primary DNS address (only client mode)
       */
  if(((ppp_data->ncp.s_rejected & (1UL << MASK_TYPE_PDNS)) EQ 0) &&
     (ppp_data->mode EQ PPP_CLIENT))
  {
    ret_desc->buffer[pos]=TYPE_PDNS;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=LENGTH_PDNS;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->ncp.n_pdns >> 24);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)((ppp_data->ncp.n_pdns >> 16) & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)((ppp_data->ncp.n_pdns >>  8) & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->ncp.n_pdns & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
  }
      /*
       * Secondary DNS address (only client mode)
       */
  if(((ppp_data->ncp.s_rejected & (1UL << MASK_TYPE_SDNS)) EQ 0) &&
     (ppp_data->mode EQ PPP_CLIENT))
  {
    ret_desc->buffer[pos]=TYPE_SDNS;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=LENGTH_SDNS;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->ncp.n_sdns >> 24);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)((ppp_data->ncp.n_sdns >> 16) & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)((ppp_data->ncp.n_sdns >>  8) & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->ncp.n_sdns & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    pos++;
  }
  /*
   * insert packet length
   */
  ret_desc->len=pos;
  ret_desc->buffer[len_pos]=(UBYTE)(pos >> 8);/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  len_pos++;
  ret_desc->buffer[len_pos]=(UBYTE)(pos & 0x00ff);/*lint !e415 !e416 creation and access of out-of-bounds pointer */

  /*
   * return the created packet
   */
  ppp_data->ncp.scr=TRUE;
  *ptr_packet=ret_desc;

} /* ncp_get_scr() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_get_str
+------------------------------------------------------------------------------
| Description : The function ncp_get_str() creates a NCP Terminate Request
|               packet.
|
| Parameters  : ptr_packet - returns the Terminate Request packet
|                            THE MEMORY FOR THE PACKET WILL ALLOCATED BY
|                            THIS FUNCTION
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_get_str (T_desc2** ptr_packet)
{
  T_desc2* ret_desc;
  USHORT   pos;

  TRACE_FUNCTION( "ncp_get_str" );

  /*
   * Allocate the necessary size for the data descriptor. The size is
   * calculated as follows:
   * - take the size of a descriptor structure
   * - subtract one because of the array buffer[1] to get the size of
   *   descriptor control information
   * - add number of octets of descriptor data
   */
  MALLOC (ret_desc, (USHORT)(sizeof(T_desc2) - 1 + NCP_TERM_REQ_LENGTH));
  /*
   * fill the packet
   */
  ret_desc->next = (ULONG)NULL;
  ret_desc->len  = NCP_TERM_REQ_LENGTH;
  pos            = 0;
    /*
     * Code field
     */
  ret_desc->buffer[pos] = CODE_TERM_REQ;
  pos++;
    /*
     * Identifier field
     */
  ret_desc->buffer[pos] = ppp_data->ncp.nstri;/*lint !e415 access of out-of-bounds pointer */
  pos++;
    /*
     * Length field
     */
  ret_desc->buffer[pos] = 0;/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  pos++;
  ret_desc->buffer[pos] = NCP_TERM_REQ_LENGTH;/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  pos++;
    /*
     * Data field contains the error code
     */
  ret_desc->buffer[pos] = (U8)((ppp_data->ppp_cause >> 8) & 0x00ff);/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  pos++;
  ret_desc->buffer[pos] = (U8)((ppp_data->ppp_cause)      & 0x00ff);/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  pos++;

  /*
   * return the created packet
   */
  ppp_data->ncp.str = TRUE;
  *ptr_packet       = ret_desc;
} /* ncp_get_str() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_analyze_first_ipcp
+------------------------------------------------------------------------------
| Description : The function ncp_analyze_first_ipcp() determine whether the
|               given packet is a Configure Request packet. If so the function
|               analyzes the packet, sets some values in the data structure and
|               returns the values for header comression.
|
| Parameters  : packet     - received packet
|               ptr_result - returns the result of the analysis
|               ptr_hc     - returns requested header compression
|               ptr_msid   - returns requested max slot identifier
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_analyze_first_ipcp(T_desc2* packet,
                                   UBYTE*   ptr_result,
                                   UBYTE*   ptr_hc,
                                   UBYTE*   ptr_msid)
{
  USHORT  packet_len;
  UBYTE   type_len;
  USHORT  pos;
  USHORT  analyzed;
  USHORT  protocol_hc;
  ULONG   ip;
  ULONG   pdns;
  ULONG   sdns;

  TRACE_FUNCTION( "ncp_analyze_first_ipcp" );

  /*
   * check whether it is a Configure Request packet and
   * for correct length field
   */
  packet_len = packet->buffer[2];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  packet_len <<= 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  if((packet->buffer[0] NEQ CODE_CONF_REQ) ||
     (packet_len > packet->len) ||
     (packet_len < 4))
  {
    *ptr_result = FALSE;
    return;
  }
  /*
   * check consistence of length of packet and length of configuration options
   */
  pos=5;
  while(pos < packet_len)
  {
    if(packet->buffer[pos] < 2)/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    {
      *ptr_result = FALSE;
      return;
    }
    pos+= packet->buffer[pos];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
  }
  if((pos - 1) NEQ packet_len)
  {
    *ptr_result = FALSE;
    return;
  }
  /*
   * analyze configuration options
   */
  ppp_data->ncp.r_hc   = PPP_HC_DEFAULT;
  ppp_data->ncp.r_msid = PPP_MSID_DEFAULT;
  ppp_data->ncp.n_ip   = PPP_IP_DEFAULT;
  ppp_data->ncp.n_pdns = PPP_PDNS_DEFAULT;
  ppp_data->ncp.n_sdns = PPP_SDNS_DEFAULT;
  pos=4;
    /*
     * analyzed is a bit field and marks all already analyzed
     * configuration options in order to reject all configuration options
     * which are listed more than once
     */
  analyzed=0;
  while(pos < packet_len)
  {
    type_len=packet->buffer[pos + 1];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    switch(packet->buffer[pos])/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    {
      /*
       * yet supported configuration options
       */
      case TYPE_HC: /* Header Compression */
        protocol_hc = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        protocol_hc <<= 8;
        protocol_hc+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        if((analyzed & (1UL << MASK_TYPE_HC)) EQ 0)
        {
          analyzed|= (1UL << MASK_TYPE_HC);
          /*
           * max slot identifier should be between 3 and 254
           * comp slot identifier may be compressed
           */
          if((protocol_hc EQ DTI_PID_CTCP) &&
             (packet->buffer[pos + 4] >= 3) &&
             (packet->buffer[pos + 5] EQ PPP_CSID_YES))/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
          {
            ppp_data->ncp.r_hc   = PPP_HC_VJ;
            ppp_data->ncp.r_msid = packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
            if(ppp_data->ncp.r_msid > 254)
              ppp_data->ncp.r_msid = 254;
          }
        }
        break;
      case TYPE_IP:
        ip = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        ip = (ip << 8);
        ip+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        ip = (ip << 8);
        ip+= packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        ip = (ip << 8);
        ip+= packet->buffer[pos + 5];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        if((analyzed & (1UL << MASK_TYPE_IP)) EQ 0)
        {
          analyzed|=(1UL << MASK_TYPE_IP);
          ppp_data->ncp.n_ip = ip;
        }
        break;
      case TYPE_PDNS:
        pdns = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        pdns = (pdns << 8);
        pdns+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        pdns = (pdns << 8);
        pdns+= packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        pdns = (pdns << 8);
        pdns+= packet->buffer[pos + 5];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        if((analyzed & (1UL << MASK_TYPE_PDNS)) EQ 0)
        {
          analyzed|=(1UL << MASK_TYPE_PDNS);
          ppp_data->ncp.n_pdns = pdns;
        }
        break;
      case TYPE_SDNS:
        sdns = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        sdns = (sdns << 8);
        sdns+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        sdns = (sdns << 8);
        sdns+= packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        sdns = (sdns << 8);
        sdns+= packet->buffer[pos + 5];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        if((analyzed & (1UL << MASK_TYPE_SDNS)) EQ 0)
        {
          analyzed|=(1UL << MASK_TYPE_SDNS);
          ppp_data->ncp.n_sdns = sdns;
        }
        break;
      default:
        /*
         * not supported configuration options are not analysed
         */
        break;
    }
    pos+= type_len;
  }
  /*
   * all configuration options analyzed
   */
  *ptr_hc     = ppp_data->ncp.r_hc;
  *ptr_msid   = ppp_data->ncp.r_msid;
  *ptr_result = TRUE;
} /* ncp_analyze_first_ipcp() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_fill_out_packet
+------------------------------------------------------------------------------
| Description : The function ncp_fill_out_packet() puts a IPCP packet into
|               the protocol configuration list
|
| Parameters  : pco_buf - pco list buffer
|               ptr_pos - position where to write the IPCP packet, this value
|                         must get back to the calling funtion
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_fill_out_packet (UBYTE pco_buf[], USHORT* ptr_pos)
{
  USHORT  pos;
  USHORT  len_pos1, len_pos2;

  TRACE_FUNCTION( "ncp_fill_out_packet" );

  if((ppp_data->pco_mask & PPP_PCO_MASK_IPCP_HC)   ||
     (ppp_data->pco_mask & PPP_PCO_MASK_IPCP_IP)   ||
     (ppp_data->pco_mask & PPP_PCO_MASK_IPCP_PDNS) ||
     (ppp_data->pco_mask & PPP_PCO_MASK_IPCP_SDNS))
  {
    pos=*ptr_pos;
    /*
     * create the Configure-Request packet
     */
      /*
       * Protocol ID
       */
    pco_buf[pos] = PROTOCOL_IPCP_MSB;
    pos++;
    pco_buf[pos] = PROTOCOL_IPCP_LSB;
    pos++;
      /*
       * Length of Protocol contents (store the position)
       */
    len_pos1 = pos;
    pos++;
        /*
         * Code field
         */
    pco_buf[pos] = CODE_CONF_REQ;
    pos++;
        /*
         * Identifier field (some value)
         */
    pco_buf[pos] = 1;
    pos++;
        /*
         * Length field (store the position)
         */
    len_pos2 = pos;
    pos++;
    pos++;

    if(ppp_data->pco_mask & PPP_PCO_MASK_IPCP_HC)
    {
            /*
             * Header Compression
             */
      if(ppp_data->ncp.r_hc EQ PPP_HC_VJ)
      {
        pco_buf[pos] = TYPE_HC;
        pos++;
        pco_buf[pos] = LENGTH_HC_VJ;
        pos++;
        pco_buf[pos] = PROTOCOL_VJ_MSB;
        pos++;
        pco_buf[pos] = PROTOCOL_VJ_LSB;
        pos++;
        pco_buf[pos] = ppp_data->ncp.r_msid;
        pos++;
        pco_buf[pos] = PPP_CSID_YES;
        pos++;
      }
    }

    if(ppp_data->pco_mask & PPP_PCO_MASK_IPCP_IP)
    {
            /*
             * IP Address
             */
      pco_buf[pos] = TYPE_IP;
      pos++;
      pco_buf[pos] = LENGTH_IP;
      pos++;
      pco_buf[pos] = (UBYTE)(ppp_data->ncp.n_ip >> 24);
      pos++;
      pco_buf[pos] = (UBYTE)((ppp_data->ncp.n_ip >> 16) & 0x000000ff);
      pos++;
      pco_buf[pos] = (UBYTE)((ppp_data->ncp.n_ip >> 8) & 0x000000ff);
      pos++;
      pco_buf[pos] = (UBYTE)(ppp_data->ncp.n_ip & 0x000000ff);
      pos++;
    }

    if(ppp_data->pco_mask & PPP_PCO_MASK_IPCP_PDNS)
    {
            /*
             * primary DNS Address
             */
      pco_buf[pos] = TYPE_PDNS;
      pos++;
      pco_buf[pos] = LENGTH_PDNS;
      pos++;
      pco_buf[pos] = (UBYTE)(ppp_data->ncp.n_pdns >> 24);
      pos++;
      pco_buf[pos] = (UBYTE)((ppp_data->ncp.n_pdns >> 16) & 0x000000ff);
      pos++;
      pco_buf[pos] = (UBYTE)((ppp_data->ncp.n_pdns >> 8) & 0x000000ff);
      pos++;
      pco_buf[pos] = (UBYTE)(ppp_data->ncp.n_pdns & 0x000000ff);
      pos++;
    }

    if(ppp_data->pco_mask & PPP_PCO_MASK_IPCP_SDNS)
    {
            /*
             * secondary DNS Address
             */
      pco_buf[pos] = TYPE_SDNS;
      pos++;
      pco_buf[pos] = LENGTH_SDNS;
      pos++;
      pco_buf[pos] = (UBYTE)(ppp_data->ncp.n_sdns >> 24);
      pos++;
      pco_buf[pos] = (UBYTE)((ppp_data->ncp.n_sdns >> 16) & 0x000000ff);
      pos++;
      pco_buf[pos] = (UBYTE)((ppp_data->ncp.n_sdns >> 8) & 0x000000ff);
      pos++;
      pco_buf[pos] = (UBYTE)(ppp_data->ncp.n_sdns & 0x000000ff);
      pos++;
    }

        /*
         * insert packet length
         */
    pco_buf[len_pos2] = 0;
    len_pos2++;
    pco_buf[len_pos2] = (UBYTE)(pos - len_pos2 + 3);
      /*
       * insert Length of Protocol Contents
       */
    pco_buf[len_pos1] = pco_buf[len_pos2];

    /*
     * return new position
     */
    *ptr_pos=pos;
  }

  if(ppp_data->pco_mask & PPP_PCO_MASK_IPCP_GATEWAY)
  {
    pos=*ptr_pos;
    /*
     * create the Configure-NAK packet
     */
      /*
       * Protocol ID
       */
    pco_buf[pos] = PROTOCOL_IPCP_MSB;
    pos++;
    pco_buf[pos] = PROTOCOL_IPCP_LSB;
    pos++;
      /*
       * Length of Protocol contents (store the position)
       */
    len_pos1 = pos;
    pos++;
        /*
         * Code field
         */
    pco_buf[pos] = CODE_CONF_NAK;
    pos++;
        /*
         * Identifier field (some value)
         */
    pco_buf[pos] = 1;
    pos++;
        /*
         * Length field (store the position)
         */
    len_pos2 = pos;
    pos++;
    pos++;

          /*
           * dynamic Gateway Address
           */
    pco_buf[pos] = TYPE_IP;
    pos++;
    pco_buf[pos] = LENGTH_IP;
    pos++;
    pco_buf[pos] = 0;
    pos++;
    pco_buf[pos] = 0;
    pos++;
    pco_buf[pos] = 0;
    pos++;
    pco_buf[pos] = 0;
    pos++;

        /*
         * insert packet length
         */
    pco_buf[len_pos2] = 0;
    len_pos2++;
    pco_buf[len_pos2] = (UBYTE)(pos - len_pos2 + 3);
      /*
       * insert Length of Protocol Contents
       */
    pco_buf[len_pos1] = pco_buf[len_pos2];

    /*
     * return new position
     */
    *ptr_pos=pos;
  }
} /* ncp_fill_out_packet() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_analyze_pco
+------------------------------------------------------------------------------
| Description : The function ncp_analyze_pco() analyzes the return packet from
|               PDP activation and determines the primary and secondary DNS
|               address and the Gateway address.
|
| Parameters  : pco_buf     - pointer to the protocol configuration options
|               pos         - position where the IPCP packet starts
|               ptr_dns1    - returns primary DNS address
|               ptr_dns2    - returns secondary DNS address
|               ptr_gateway - returns Gateway address
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_analyze_pco (UBYTE pco_buf[],
                             USHORT pos,
                             ULONG* ptr_dns1,
                             ULONG* ptr_dns2,
                             ULONG* ptr_gateway)
{
  USHORT  packet_len;
  USHORT  start_pos;
  UBYTE   type_len;
  USHORT  analyzed;
  ULONG   pdns;
  ULONG   sdns;
  ULONG   gateway;

  TRACE_FUNCTION( "ncp_analyze_pco" );

  /*
   * check for correct length field
   */
  pos+= 2;
  packet_len = pco_buf[pos + 3];
  packet_len <<= 8;
  packet_len+= pco_buf[pos + 4];
  if(packet_len NEQ pco_buf[pos])
    return;
  pos++;
  /*
   * check of code field
   */
  if((pco_buf[pos] NEQ CODE_CONF_REQ) &&
     (pco_buf[pos] NEQ CODE_CONF_NAK) &&
     (pco_buf[pos] NEQ CODE_CONF_ACK))
    return;
  /*
   * analyze configuration options
   */
  start_pos = pos;
  pos+= 4;
    /*
     * analyzed is a bit field and marks all already analyzed
     * configuration options in order to determine configuration options
     * which are listed more than once
     */
  analyzed=0;
  while((pos - start_pos) < packet_len)
  {
    type_len=pco_buf[pos + 1];
    switch(pco_buf[pos])
    {
      /*
       * search for IP configuration option
       */
      case TYPE_IP:
        gateway = pco_buf[pos + 2];
        gateway = (gateway << 8);
        gateway+= pco_buf[pos + 3];
        gateway = (gateway << 8);
        gateway+= pco_buf[pos + 4];
        gateway = (gateway << 8);
        gateway+= pco_buf[pos + 5];
        if(((analyzed & (1UL << MASK_TYPE_IP)) EQ 0) &&
           (*ptr_gateway EQ PPP_GATEWAY_DEFAULT)     &&
           (pco_buf[start_pos] EQ CODE_CONF_REQ))
        {
          analyzed|=(1UL << MASK_TYPE_IP);
          *ptr_gateway = gateway;
        }
        break;

      /*
       * search for DNS configuration options
       */
      case TYPE_PDNS:
        pdns = pco_buf[pos + 2];
        pdns = (pdns << 8);
        pdns+= pco_buf[pos + 3];
        pdns = (pdns << 8);
        pdns+= pco_buf[pos + 4];
        pdns = (pdns << 8);
        pdns+= pco_buf[pos + 5];
        if(((analyzed & (1UL << MASK_TYPE_PDNS)) EQ 0) &&
           (*ptr_dns1 EQ PPP_PDNS_DEFAULT)             &&
           ((pco_buf[start_pos] EQ CODE_CONF_NAK) ||
            (pco_buf[start_pos] EQ CODE_CONF_ACK)))
        {
          analyzed|=(1UL << MASK_TYPE_PDNS);
          *ptr_dns1 = pdns;
        }
        break;

      case TYPE_SDNS:
        sdns = pco_buf[pos + 2];
        sdns = (sdns << 8);
        sdns+= pco_buf[pos + 3];
        sdns = (sdns << 8);
        sdns+= pco_buf[pos + 4];
        sdns = (sdns << 8);
        sdns+= pco_buf[pos + 5];
        if(((analyzed & (1UL << MASK_TYPE_SDNS)) EQ 0) &&
           (*ptr_dns2 EQ PPP_SDNS_DEFAULT)             &&
           ((pco_buf[start_pos] EQ CODE_CONF_NAK) ||
            (pco_buf[start_pos] EQ CODE_CONF_ACK)))
        {
          analyzed|=(1UL << MASK_TYPE_SDNS);
          *ptr_dns2 = sdns;
        }
        break;
      default:
        /*
         * other configuration options are not analysed
         */
        break;
    }
    pos+= type_len;
  }
} /* ncp_analyze_pco() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_rcr
+------------------------------------------------------------------------------
| Description : The function ncp_rcr() analyzes the given
|               Configure Request packet and returns either FORWARD_RCRP or
|               FORWARD_RCRN depend on the result of the analysis.
|               The packet pointer points to an appropriate response packet.
|
| Parameters  : ptr_packet  - pointer to a Configure Request packet
|               forward     - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_rcr (T_desc2** ptr_packet, UBYTE* isnew, UBYTE* forward)
{
  T_desc2* packet;
  USHORT   packet_len;
  UBYTE    type_len;
  USHORT   pos;
  USHORT   copy_pos;
  USHORT   analyzed;
  UBYTE    code_ret;
  UBYTE    error_found;
  USHORT   protocol_hc;
  UBYTE    csid;
  ULONG    ip;
  ULONG    pdns;
  ULONG    sdns;

  TRACE_FUNCTION( "ncp_rcr" );

  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  packet_len <<= 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  if((packet_len > packet->len) || (packet_len < 4))
  {
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * check consistence of length of packet and length of configuration options
   */
  pos=5;
  while(pos < packet_len)
  {
    if(packet->buffer[pos] < 2)/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    {
      *forward=FORWARD_DISCARD;
      return;
    }
    pos += packet->buffer[pos];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
  }
  if((pos - 1) NEQ packet_len)
  {
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * check whether it is a new identifier
   */
  *isnew=TRUE;
  if((ppp_data->ncp.rcr) && (ppp_data->ncp.lrcri EQ packet->buffer[1]))/*lint !e415 access of out-of-bounds pointer */
    *isnew=FALSE;
  ppp_data->ncp.lrcri=packet->buffer[1];/*lint !e415 access of out-of-bounds pointer */
  ppp_data->ncp.rcr=TRUE;
  /*
   * analyze configuration options
   */
  ppp_data->ncp.r_hc   = PPP_HC_DEFAULT;
  ppp_data->ncp.r_msid = PPP_MSID_DEFAULT;
  csid                 = 0;
  if(ppp_data->mode EQ PPP_SERVER)
  {
    ppp_data->ncp.n_ip   = PPP_IP_DEFAULT;
    ppp_data->ncp.n_pdns = PPP_PDNS_DEFAULT;
    ppp_data->ncp.n_sdns = PPP_SDNS_DEFAULT;
  }
  pos=4;
    /*
     * position where NAKed or Rejected configuration options are copied to
     */
  copy_pos=4;
    /*
     * code_ret contains actually the status of analysis
     * states are CODE_CONF_ACK, CODE_CONF_NAK and CODE_CONF_REJ
     * this state are also values for the Code-field in the return packet
     */
  code_ret=CODE_CONF_ACK;
    /*
     * analyzed is a bit field and marks all already analyzed
     * configuration options in order to reject all configuration options
     * which are listed more than once
     */
  analyzed=0;
  while(pos < packet_len)
  {
    type_len=packet->buffer[pos + 1];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    switch(packet->buffer[pos])/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    {
      /*
       * yet supported configuration options
       */
      case TYPE_HC: /* Header Compression */
        protocol_hc = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        protocol_hc <<= 8;
        protocol_hc+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        if(protocol_hc EQ DTI_PID_CTCP)
        {
          ppp_data->ncp.r_hc   = PPP_HC_VJ;
          ppp_data->ncp.r_msid = packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
          csid                 = packet->buffer[pos + 5];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        }

        switch(code_ret)
        {
          case CODE_CONF_ACK:
            if(((analyzed & (1UL << MASK_TYPE_HC)) EQ 0) &&
               (ppp_data->ncp.r_hc EQ ppp_data->ncp.req_hc))
            {
              if((ppp_data->ncp.r_hc EQ PPP_HC_VJ) &&
                 (type_len EQ LENGTH_HC_VJ))
              {
                if((ppp_data->mode EQ PPP_SERVER) &&
                   (ppp_data->ncp.r_msid >= ppp_data->ncp.s_msid) &&
                   (csid EQ PPP_CSID_YES) &&
                   (ppp_data->ncp.s_hc EQ PPP_HC_VJ))
                {
                  analyzed|=(1UL << MASK_TYPE_HC);
                  pos+= LENGTH_HC_VJ;
                  break;
                }
                /*
                 * include here negotiation in client mode
                 */
              }
              /*
               * include here additional header compression protocols
               */
            }
            code_ret=CODE_CONF_NAK;
            /* fall through */
          case CODE_CONF_NAK:
            /*
             * this configuration option will NAKed only if the compression
             * protocol is not Van Jacobson Header Compression
             */
            if(((analyzed & (1UL << MASK_TYPE_HC)) EQ 0) &&
               (ppp_data->ncp.req_hc NEQ PPP_HC_DEFAULT) &&
               (ppp_data->ncp.s_hc EQ PPP_HC_VJ) &&
               ((protocol_hc NEQ DTI_PID_CTCP) ||
                (type_len NEQ LENGTH_HC_VJ) ||
                ((ppp_data->ncp.r_msid >= ppp_data->ncp.s_msid) &&
                 (csid EQ PPP_CSID_YES))))
            {
              analyzed|= (1UL << MASK_TYPE_HC);

              error_found = FALSE;
              /*
               * this implementation is only for server mode
               */
              if(type_len < LENGTH_HC_VJ)
              {
                T_desc2* temp_desc;

                /*
                 * create a new larger packet and copy the content
                 * of the old packet into the new
                 */
                MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1
                                                           + packet_len
                                                           + LENGTH_HC_VJ
                                                           - type_len));
                temp_desc->next = packet->next;
                temp_desc->len  = packet_len + LENGTH_HC_VJ - type_len;
                memcpy(temp_desc->buffer, packet->buffer, pos);/*lint !e669 !e670 possible data overrun*/
                memcpy(&temp_desc->buffer[pos + LENGTH_HC_VJ - type_len],
                       &packet->buffer[pos],
                       packet_len - pos);/*lint !e662 !e669 !e670 possible creation of out-of-bounds pointer*/
                arb_discard_packet(packet);
                packet_len+= (LENGTH_HC_VJ - type_len);
                pos       += (LENGTH_HC_VJ - type_len);
                packet     = temp_desc;

                error_found = TRUE;
              }
              else if(ppp_data->ncp.r_hc NEQ PPP_HC_VJ)
              {
                error_found = TRUE;
              }
              else if(type_len > LENGTH_HC_VJ)
              {
                error_found = TRUE;
              }
              if(error_found EQ TRUE)
              {
                packet->buffer[copy_pos] = TYPE_HC;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos] = LENGTH_HC_VJ;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos] = PROTOCOL_VJ_MSB;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos] = PROTOCOL_VJ_LSB;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos] = ppp_data->ncp.req_msid;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos] = PPP_CSID_YES;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
              }
              pos+= type_len;
              break;
            }
            code_ret = CODE_CONF_REJ;
            copy_pos = 4;
            /* fall through */
          case CODE_CONF_REJ:
            if(((analyzed & (1UL << MASK_TYPE_HC)) EQ 0) &&
               (ppp_data->ncp.req_hc NEQ PPP_HC_DEFAULT) &&
               (ppp_data->ncp.s_hc EQ PPP_HC_VJ) &&
               ((ppp_data->ncp.s_rejected & (1UL << MASK_TYPE_HC)) EQ 0) &&
               ((protocol_hc NEQ DTI_PID_CTCP) ||
                (type_len NEQ LENGTH_HC_VJ) ||
                ((ppp_data->ncp.r_msid >= ppp_data->ncp.s_msid) &&
                 (csid EQ PPP_CSID_YES))))
            {
              analyzed|= (1UL << MASK_TYPE_HC);
              pos+= type_len;
              break;
            }
            /*
             * set s_hc to default because we reject this configuration option
             */
            ppp_data->ncp.s_hc = PPP_HC_DEFAULT;
            while(type_len > 0)
            {
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
              copy_pos++;
              pos++;
              type_len--;
            }
            break;
          default:
            TRACE_ERROR("Packet return code invalid");
            break;
        }
        break;
      case TYPE_IP:
        ip = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        ip = (ip << 8);
        ip+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        ip = (ip << 8);
        ip+= packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        ip = (ip << 8);
        ip+= packet->buffer[pos + 5];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/

        switch(code_ret)
        {
          case CODE_CONF_ACK:
            if(((analyzed & (1UL << MASK_TYPE_IP)) EQ 0) &&
               (type_len EQ LENGTH_IP))
            {
              if((ppp_data->mode EQ PPP_SERVER) &&
                 (((ip EQ ppp_data->ncp.req_ip) &&
                   (ppp_data->ncp.req_ip NEQ PPP_IP_DEFAULT)) ||
                  ((ip NEQ PPP_IP_DEFAULT) &&
                   (ppp_data->ncp.req_ip EQ PPP_IP_DEFAULT))))
              {
                analyzed|=(1UL << MASK_TYPE_IP);
                ppp_data->ncp.n_ip = ip;
                pos+=LENGTH_IP;
                break;
              }
              else if((ppp_data->mode EQ PPP_CLIENT) &&
                      (ip NEQ PPP_IP_DEFAULT))
              {
                /*
                 * accept Gateway address
                 */
                analyzed|=(1UL << MASK_TYPE_IP);
                ppp_data->ncp.n_gateway = ip;
                pos+=LENGTH_IP;
                break;
              }
            }
            code_ret=CODE_CONF_NAK;
            /* fall through */
          case CODE_CONF_NAK:
            /*
             * this option will be rejected in following cases:
             * - occurence more than once
             * - in client mode if received IP is dynamic
             * - in server mode if received and requested IP are dynamic
             */
            if(((analyzed & (1UL << MASK_TYPE_IP)) EQ 0) &&
               (((ppp_data->mode EQ PPP_SERVER) &&
                 ((ip NEQ ppp_data->ncp.req_ip) ||
                  (ppp_data->ncp.req_ip NEQ PPP_IP_DEFAULT))) ||
                ((ppp_data->mode EQ PPP_CLIENT) &&
                 (ip NEQ PPP_IP_DEFAULT))))
            {
              analyzed|=(1UL << MASK_TYPE_IP);

              error_found=FALSE;
              if(type_len < LENGTH_IP)
              {
                T_desc2* temp_desc;

                /*
                 * create a new larger packet and copy the content
                 * of the old packet into the new
                 */
                MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1
                                                           + packet_len
                                                           + LENGTH_IP
                                                           - type_len));
                temp_desc->next = packet->next;
                temp_desc->len  = packet_len + LENGTH_IP - type_len;
                memcpy(temp_desc->buffer, packet->buffer, pos);/*lint !e669 !e670 Possible data overrun*/
                memcpy(&temp_desc->buffer[pos + LENGTH_IP - type_len],
                       &packet->buffer[pos],
                       packet_len - pos);/*lint !e662 !e669 !e670 possible creation of out-of-bounds pointer*/
                arb_discard_packet(packet);
                packet_len += (LENGTH_IP - type_len);
                pos        += (LENGTH_IP - type_len);
                packet      = temp_desc;
                error_found = TRUE;
              }
              else if((ip NEQ ppp_data->ncp.req_ip) &&
                      (ppp_data->ncp.req_ip NEQ PPP_IP_DEFAULT) &&
                      (ppp_data->mode EQ PPP_SERVER))
              {
                error_found=TRUE;
              }
              else if(type_len > LENGTH_IP)
              {
                error_found=TRUE;
              }
              if(error_found EQ TRUE)
              {
                packet->buffer[copy_pos]=TYPE_IP;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=LENGTH_IP;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)(ppp_data->ncp.req_ip >> 24);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)((ppp_data->ncp.req_ip >> 16)
                                                 & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)((ppp_data->ncp.req_ip >>  8)
                                                 & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)(ppp_data->ncp.req_ip
                                                 & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
              }
              pos+= type_len;
              break;
            }
            code_ret = CODE_CONF_REJ;
            copy_pos = 4;
            /* fall through */
          case CODE_CONF_REJ:
            if(((analyzed & (1UL << MASK_TYPE_IP)) EQ 0) &&
               (((ppp_data->mode EQ PPP_SERVER) &&
                 ((ip NEQ ppp_data->ncp.req_ip) ||
                  (ppp_data->ncp.req_ip NEQ PPP_IP_DEFAULT))) ||
                ((ppp_data->mode EQ PPP_CLIENT) &&
                 (ip NEQ PPP_IP_DEFAULT))))
            {
              analyzed|=(1UL << MASK_TYPE_IP);
              pos+=type_len;
              break;
            }
            while(type_len > 0)
            {
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
              copy_pos++;
              pos++;
              type_len--;
            }
            break;
          default:
            TRACE_ERROR("Packet return code invalid");
            break;
        }
        break;
      case TYPE_PDNS:  /* only in server mode allowed */
        pdns = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        pdns = (pdns << 8);
        pdns+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        pdns = (pdns << 8);
        pdns+= packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        pdns = (pdns << 8);
        pdns+= packet->buffer[pos + 5];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/

        switch(code_ret)
        {
          case CODE_CONF_ACK:
            if(((analyzed & (1UL << MASK_TYPE_PDNS)) EQ 0) &&
               (ppp_data->mode EQ PPP_SERVER) &&
               (type_len EQ LENGTH_PDNS) &&
               (((pdns EQ ppp_data->ncp.req_pdns) &&
                 (ppp_data->ncp.req_pdns NEQ PPP_PDNS_DEFAULT)) ||
                ((pdns NEQ PPP_PDNS_DEFAULT) &&
                 (ppp_data->ncp.req_pdns EQ PPP_PDNS_DEFAULT))))
            {
              analyzed|=(1UL << MASK_TYPE_PDNS);
              ppp_data->ncp.n_pdns = pdns;
              pos+= LENGTH_PDNS;
              break;
            }
            code_ret=CODE_CONF_NAK;
            /* fall through */
          case CODE_CONF_NAK:
            /*
             * this option will be rejected in following cases:
             * - occurence more than once
             * - client mode
             * - received and requested PDNS are dynamic
             */
            if(((analyzed & (1UL << MASK_TYPE_PDNS)) EQ 0) &&
               (ppp_data->mode EQ PPP_SERVER) &&
               ((pdns NEQ ppp_data->ncp.req_pdns) ||
                (ppp_data->ncp.req_pdns NEQ PPP_PDNS_DEFAULT)))
            {
              analyzed|=(1UL << MASK_TYPE_PDNS);

              error_found=FALSE;
              if(type_len < LENGTH_PDNS)
              {
                T_desc2* temp_desc;

                /*
                 * create a new larger packet and copy the content
                 * of the old packet into the new
                 */
                MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1
                                                           + packet_len
                                                           + LENGTH_PDNS
                                                           - type_len));
                temp_desc->next = packet->next;
                temp_desc->len  = packet_len + LENGTH_PDNS - type_len;
                memcpy(temp_desc->buffer, packet->buffer, pos);/*lint !e669 !e670 Possible data overrun*/
                memcpy(&temp_desc->buffer[pos + LENGTH_PDNS - type_len],
                       &packet->buffer[pos],
                       packet_len - pos);/*lint !e662 !e669 !e670 possible creation of out-of-bounds pointer*/
                arb_discard_packet(packet);
                packet_len += (LENGTH_PDNS - type_len);
                pos        += (LENGTH_PDNS - type_len);
                packet      = temp_desc;
                error_found = TRUE;
              }
              else if((pdns NEQ ppp_data->ncp.req_pdns) &&
                      (ppp_data->ncp.req_pdns NEQ PPP_PDNS_DEFAULT))
              {
                error_found=TRUE;
              }
              else if(type_len > LENGTH_PDNS)
              {
                error_found=TRUE;
              }
              if(error_found EQ TRUE)
              {
                packet->buffer[copy_pos]=TYPE_PDNS;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=LENGTH_PDNS;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)(ppp_data->ncp.req_pdns >> 24);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)((ppp_data->ncp.req_pdns >> 16)
                                                 & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)((ppp_data->ncp.req_pdns >>  8)
                                                 & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)(ppp_data->ncp.req_pdns
                                                 & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
              }
              pos+= type_len;
              break;
            }
            code_ret = CODE_CONF_REJ;
            copy_pos = 4;
            /* fall through */
          case CODE_CONF_REJ:
            if(((analyzed & (1UL << MASK_TYPE_PDNS)) EQ 0) &&
               (ppp_data->mode EQ PPP_SERVER) &&
               ((pdns NEQ ppp_data->ncp.req_pdns) ||
                (ppp_data->ncp.req_pdns NEQ PPP_PDNS_DEFAULT)))
            {
              analyzed|=(1UL << MASK_TYPE_PDNS);
              pos+=type_len;
              break;
            }
            while(type_len > 0)
            {
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
              copy_pos++;
              pos++;
              type_len--;
            }
            break;
          default:
            TRACE_ERROR("Packet return code invalid");
            break;
        }
        break;
      case TYPE_SDNS:  /* only in server mode allowed */
        sdns = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        sdns = (sdns << 8);
        sdns+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        sdns = (sdns << 8);
        sdns+= packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        sdns = (sdns << 8);
        sdns+= packet->buffer[pos + 5];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/

        switch(code_ret)
        {
          case CODE_CONF_ACK:
            if(((analyzed & (1UL << MASK_TYPE_SDNS)) EQ 0) &&
               (ppp_data->mode EQ PPP_SERVER) &&
               (type_len EQ LENGTH_SDNS) &&
               (((sdns EQ ppp_data->ncp.req_sdns) &&
                 (ppp_data->ncp.req_sdns NEQ PPP_SDNS_DEFAULT)) ||
                ((sdns NEQ PPP_SDNS_DEFAULT) &&
                 (ppp_data->ncp.req_sdns EQ PPP_SDNS_DEFAULT))))
            {
              analyzed|=(1UL << MASK_TYPE_SDNS);
              ppp_data->ncp.n_sdns = sdns;
              pos+= LENGTH_SDNS;
              break;
            }
            code_ret=CODE_CONF_NAK;
            /* fall through */
          case CODE_CONF_NAK:
            /*
             * this option will be rejected in following cases:
             * - occurence more than once
             * - client mode
             * - received and requested SDNS are dynamic
             */
            if(((analyzed & (1UL << MASK_TYPE_SDNS)) EQ 0) &&
               (ppp_data->mode EQ PPP_SERVER) &&
               ((sdns NEQ ppp_data->ncp.req_sdns) ||
                (ppp_data->ncp.req_sdns NEQ PPP_SDNS_DEFAULT)))
            {
              analyzed|=(1UL << MASK_TYPE_SDNS);

              error_found=FALSE;
              if(type_len < LENGTH_SDNS)
              {
                T_desc2* temp_desc;

                /*
                 * create a new larger packet and copy the content
                 * of the old packet into the new
                 */
                MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1
                                                           + packet_len
                                                           + LENGTH_SDNS
                                                           - type_len));
                temp_desc->next = packet->next;
                temp_desc->len  = packet_len + LENGTH_SDNS - type_len;
                memcpy(temp_desc->buffer, packet->buffer, pos);/*lint !e669 !e670 Possible data overrun*/
                memcpy(&temp_desc->buffer[pos + LENGTH_SDNS - type_len],
                       &packet->buffer[pos],
                       packet_len - pos);/*lint !e662 !e669 !e670 possible creation  of out-of-bounds pointer*/
                arb_discard_packet(packet);
                packet_len += (LENGTH_SDNS - type_len);
                pos        += (LENGTH_SDNS - type_len);
                packet      = temp_desc;
                error_found = TRUE;
              }
              else if((sdns NEQ ppp_data->ncp.req_sdns) &&
                      (ppp_data->ncp.req_sdns NEQ PPP_SDNS_DEFAULT))
              {
                error_found=TRUE;
              }
              else if(type_len > LENGTH_SDNS)
              {
                error_found=TRUE;
              }
              if(error_found EQ TRUE)
              {
                packet->buffer[copy_pos]=TYPE_SDNS;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=LENGTH_SDNS;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)(ppp_data->ncp.req_sdns >> 24);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)((ppp_data->ncp.req_sdns >> 16)
                                                 & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)((ppp_data->ncp.req_sdns >>  8)
                                                 & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)(ppp_data->ncp.req_sdns
                                                 & 0x000000ff);/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
                copy_pos++;
              }
              pos+= type_len;
              break;
            }
            code_ret = CODE_CONF_REJ;
            copy_pos = 4;
            /* fall through */
          case CODE_CONF_REJ:
            if(((analyzed & (1UL << MASK_TYPE_SDNS)) EQ 0) &&
               (ppp_data->mode EQ PPP_SERVER) &&
               ((sdns NEQ ppp_data->ncp.req_sdns) ||
                (ppp_data->ncp.req_sdns NEQ PPP_SDNS_DEFAULT)))
            {
              analyzed|=(1UL << MASK_TYPE_SDNS);
              pos+=type_len;
              break;
            }
            while(type_len > 0)
            {
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
              copy_pos++;
              pos++;
              type_len--;
            }
            break;
          default:
            TRACE_ERROR("Packet return code invalid");
            break;
        }
        break;
      default:
        switch(code_ret)
        {
          case CODE_CONF_ACK:
          case CODE_CONF_NAK:
            code_ret=CODE_CONF_REJ;
            copy_pos=4;
            /* fall through */
          default:
            while(type_len > 0)
            {
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
              copy_pos++;
              pos++;
              type_len--;
            }
            break;
        }
    }
  }
  if(((analyzed & (1UL << MASK_TYPE_HC)) EQ 0) &&
     (ppp_data->ncp.s_hc EQ PPP_HC_VJ) &&
     (ppp_data->ncp.req_hc EQ PPP_HC_VJ) &&
     (code_ret NEQ CODE_CONF_REJ))
  {
    T_desc2* temp_desc;
    /*
     * add header compression to the configure Nak packet
     */
    code_ret = CODE_CONF_NAK;
    /*
     * create a new larger packet and copy the content
     * of the old packet into the new
     */
    MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1
                                               + packet_len
                                               + LENGTH_HC_VJ));
    temp_desc->next = packet->next;
    temp_desc->len  = packet_len + LENGTH_HC_VJ;
    memcpy(temp_desc->buffer, packet->buffer, copy_pos);/*lint !e669 !e670 Possible data overrun*/
    arb_discard_packet(packet);
    packet_len+= LENGTH_HC_VJ;
    packet     = temp_desc;
    /*
     * insert header compression option
     */
    packet->buffer[copy_pos] = TYPE_HC;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    copy_pos++;
    packet->buffer[copy_pos] = LENGTH_HC_VJ;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    copy_pos++;
    packet->buffer[copy_pos] = PROTOCOL_VJ_MSB;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    copy_pos++;
    packet->buffer[copy_pos] = PROTOCOL_VJ_LSB;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    copy_pos++;
    packet->buffer[copy_pos] = ppp_data->ncp.req_msid;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    copy_pos++;
    packet->buffer[copy_pos] = PPP_CSID_YES;/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    copy_pos++;
  }
  /*
   * set new Code field in return packet
   */
  packet->buffer[0]=code_ret;
  *ptr_packet=packet;

  if(copy_pos > 4)
  {
    /*
     * some configuration options are not acceptable
     */
    *forward=FORWARD_RCRN;
      /*
       * set new Length field
       */
    packet->len=copy_pos;
    packet->buffer[2]=(UBYTE)(copy_pos >> 8);/*lint !e415 !e416 creation and access of out-of-bounds pointer */
    packet->buffer[3]=(UBYTE)(copy_pos & 0x00ff);/*lint !e415 !e416 creation and access of out-of-bounds pointer */
    return;
  }
  /*
   * all configuration options are acceptable
   */
  *forward=FORWARD_RCRP;
} /* ncp_rcr() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_rca
+------------------------------------------------------------------------------
| Description : The function ncp_rca() checks whether the given
|               Configure Ack packet is valid and if so it returns
|               FORWARD_RCA. Otherwise it returns FORWARD_DISCARD.
|
| Parameters  : packet  - Configure Ack packet
|               forward - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_rca (T_desc2* packet, UBYTE* forward)
{
  USHORT  packet_len;

  TRACE_FUNCTION( "ncp_rca" );

  packet_len = packet->buffer[2];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  packet_len <<= 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  if((packet_len < 4) ||
     (packet_len > packet->len) ||
     (ppp_data->ncp.scr EQ FALSE) ||
     (packet->buffer[1] NEQ ppp_data->ncp.nscri))/*lint !e415 access of out-of-bounds pointer */
  {
    /*
     * invalid packet
     */
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * acceptable packet
   */
  arb_discard_packet(packet);
  ppp_data->ncp.nscri++;
  *forward=FORWARD_RCA;
} /* ncp_rca() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_rcn
+------------------------------------------------------------------------------
| Description : The function ncp_rcn() analyze the given
|               Configure Nak packet, changes some requested values and returns
|               FORWARD_RCN
|               The packet pointer points to an appropriate response packet.
|
| Parameters  : ptr_packet - Configure Nak packet
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_rcn (T_desc2** ptr_packet, UBYTE* forward)
{
  T_desc2* packet;
  USHORT   packet_len;
  UBYTE    type_len;
  USHORT   pos;
  USHORT   protocol_hc;
  ULONG    ip;
  ULONG    pdns;
  ULONG    sdns;

  TRACE_FUNCTION( "ncp_rcn" );

  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  packet_len <<= 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  if((packet_len > packet->len) ||
     (packet_len < 4) ||
     (ppp_data->ncp.scr EQ FALSE) ||
     (packet->buffer[1] NEQ ppp_data->ncp.nscri))/*lint !e415 access of out-of-bounds pointer */
  {
    /*
     * invalid packet
     */
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * check consistence of length of packet and length of configuration options
   */
  pos=5;
  while(pos < packet_len)
  {
    if(packet->buffer[pos] < 2)/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    {
      *forward=FORWARD_DISCARD;
      return;
    }
    pos+= packet->buffer[pos];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
  }
  if((pos - 1) NEQ packet_len)
  {
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * analyze configuration options
   */
  pos=4;
  while(pos < packet_len)
  {
    type_len=packet->buffer[pos + 1];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    switch(packet->buffer[pos])/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    {
      /*
       * yet supported configuration options
       */
      case TYPE_HC:
        protocol_hc = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        protocol_hc <<= 8;
        protocol_hc+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        if((protocol_hc EQ DTI_PID_CTCP) &&
           (ppp_data->ncp.req_hc EQ PPP_HC_VJ) &&
           (type_len EQ LENGTH_HC_VJ))
        {
          ppp_data->ncp.s_rejected &= ~(1UL << MASK_TYPE_HC);
          ppp_data->ncp.s_hc        = PPP_HC_VJ;
          ppp_data->ncp.s_msid      = ppp_data->ncp.req_msid;
        }
        break;
      case TYPE_IP:
        ip = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        ip = ip << 8;
        ip+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        ip = ip << 8;
        ip+= packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        ip = ip << 8;
        ip+= packet->buffer[pos + 5];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/

        if(type_len EQ LENGTH_IP)
        {
          ppp_data->ncp.s_rejected &= ~(1UL << MASK_TYPE_IP);
          if(ppp_data->mode EQ PPP_CLIENT)
            ppp_data->ncp.n_ip = ip;
          else if(ppp_data->ncp.req_gateway EQ PPP_GATEWAY_DEFAULT)
            ppp_data->ncp.n_gateway = ip;
        }
        break;
      case TYPE_PDNS:
        pdns = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        pdns = pdns << 8;
        pdns+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        pdns = pdns << 8;
        pdns+= packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        pdns = pdns << 8;
        pdns+= packet->buffer[pos + 5];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/

        if((type_len EQ LENGTH_PDNS) &&
           (ppp_data->mode EQ PPP_CLIENT))
        {
          ppp_data->ncp.s_rejected &= ~(1UL << MASK_TYPE_PDNS);
          ppp_data->ncp.n_pdns = pdns;
        }
        break;
      case TYPE_SDNS:
        sdns = packet->buffer[pos + 2];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        sdns = sdns << 8;
        sdns+= packet->buffer[pos + 3];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        sdns = sdns << 8;
        sdns+= packet->buffer[pos + 4];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
        sdns = sdns << 8;
        sdns+= packet->buffer[pos + 5];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/

        if((type_len EQ LENGTH_SDNS) &&
           (ppp_data->mode EQ PPP_CLIENT))
        {
          ppp_data->ncp.s_rejected &= ~(1UL << MASK_TYPE_SDNS);
          ppp_data->ncp.n_sdns = sdns;
        }
        break;
      default:
        /*
         * ignore unknown configuration options
         */
        break;
    }
    pos+= type_len;
  }
  /*
   * free this packet and create a new with changed configuration options
   */
  arb_discard_packet(packet);
  *forward=FORWARD_RCN;
  ppp_data->ncp.nscri++;
  ncp_get_scr(&packet);
  *ptr_packet=packet;
} /* ncp_rcn() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_rcj
+------------------------------------------------------------------------------
| Description : The function ncp_rcj() analyze the given
|               Configure Reject packet, marks some values as rejected and
|               returns FORWARD_RCJ
|               The packet pointer points to an appropriate response packet.
|
| Parameters  : ptr_packet  - pointer to a Configure Reject packet
|               forward     - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_rcj (T_desc2** ptr_packet, UBYTE* forward)
{
  T_desc2* packet;
  USHORT   packet_len;
  UBYTE    type_len;
  USHORT   pos;

  TRACE_FUNCTION( "ncp_rcj" );

  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  packet_len <<= 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  if((packet_len > packet->len) ||
     (packet_len < 4) ||
     (ppp_data->ncp.scr EQ FALSE) ||
     (packet->buffer[1] NEQ ppp_data->ncp.nscri))/*lint !e415 access of out-of-bounds pointer */
  {
    /*
     * invalid packet
     */
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * check consistence of length of packet and length of configuration options
   */
  pos=5;
  while(pos < packet_len)
  {
    if(packet->buffer[pos] < 2)/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    {
      *forward=FORWARD_DISCARD;
      return;
    }
    pos+= packet->buffer[pos];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
  }
  if((pos - 1) NEQ packet_len)
  {
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * analyze configuration options
   */
  pos=4;
  while(pos < packet_len)
  {
    type_len=packet->buffer[pos + 1];/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    switch(packet->buffer[pos])/*lint !e662 !e661 possible creation and access of out-of-bounds pointer*/
    {
      /*
       * yet supported configuration options
       */
      case TYPE_HC:
        ppp_data->ncp.s_rejected|= (1UL << MASK_TYPE_HC);
        ppp_data->ncp.s_hc = PPP_HC_DEFAULT;
        break;
      case TYPE_IP:
        if (ppp_data->mode EQ PPP_CLIENT)
        { /* error: server has to support valid IP address */
          *forward=FORWARD_DISCARD;
          return;
        }
        ppp_data->ncp.s_rejected |= (1UL << MASK_TYPE_IP);
        break;
      case TYPE_PDNS:
        ppp_data->ncp.s_rejected |= (1UL << MASK_TYPE_PDNS);
        break;
      case TYPE_SDNS:
        ppp_data->ncp.s_rejected |= (1UL << MASK_TYPE_SDNS);
        break;
      default:
        /*
         * ignore unknown configuration options
         */
        break;
    }
    pos+= type_len;
  }
  /*
   * free this packet and create a new with changed configuration options
   */
  arb_discard_packet(packet);
  *forward=FORWARD_RCN;
  ppp_data->ncp.nscri++;
  ncp_get_scr(&packet);
  *ptr_packet=packet;
} /* ncp_rcj() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_rtr
+------------------------------------------------------------------------------
| Description : The function ncp_rtr() checks whether the given
|               Terminate Request packet is valid and if so it returns
|               FORWARD_RCA. Otherwise it returns FORWARD_DISCARD.
|
| Parameters  : ptr_packet - pointer to a Terminate Request packet
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_rtr (T_desc2** ptr_packet, UBYTE* forward)
{
  T_desc2* packet;
  USHORT   packet_len;

  TRACE_FUNCTION( "ncp_rtr" );

  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  packet_len <<= 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  if((packet_len < 4) || (packet_len > packet->len))
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  /*
   * change code field
   */
  packet->buffer[0]=CODE_TERM_ACK;

  *forward=FORWARD_RTR;
} /* ncp_rtr() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_rta
+------------------------------------------------------------------------------
| Description : The function ncp_rta() checks whether the given
|               Terminate Ack packet is valid and if so it returns
|               FORWARD_RTA. Otherwise it returns FORWARD_DISCARD.
|
| Parameters  : packet  - Terminate Ack packet
|               forward - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_rta (T_desc2* packet, UBYTE* forward)
{
  USHORT  packet_len;

  TRACE_FUNCTION( "ncp_rta" );

  /*
   * check for correct length field
   */
  packet_len = packet->buffer[2];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  packet_len <<= 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  if((packet_len < 4) || (packet_len > packet->len))
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  if((ppp_data->ncp.str EQ TRUE) &&
     (packet->buffer[1] NEQ ppp_data->ncp.nstri))/*lint !e415 access of out-of-bounds pointer */
  {
    /*
     * invalid packet
     */
    *forward=FORWARD_DISCARD;
    return;
  }
  /*
   * acceptable packet
   */
  arb_discard_packet(packet);
  ppp_data->ncp.nstri++;
  *forward=FORWARD_RTA;
} /* ncp_rta() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_rxj
+------------------------------------------------------------------------------
| Description : The function ncp_rxj() analyzes whether the given Code Reject
|               is acceptable. If not it returns FORWARD_RXJN.
|
| Parameters  : ptr_packet - Pointer to a Code Reject packet
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_rxj (T_desc2** ptr_packet, UBYTE* forward)
{
  USHORT  packet_len;
  T_desc2* packet;

  TRACE_FUNCTION( "ncp_rxj" );

  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  packet_len <<= 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  if((packet_len < 5) || (packet_len > packet->len))
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  switch(packet->buffer[4])/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  {
    case CODE_CONF_REQ:
    case CODE_CONF_REJ:
    case CODE_CONF_NAK:
    case CODE_CONF_ACK:
    case CODE_TERM_REQ:
    case CODE_TERM_ACK:
    case CODE_CODE_REJ:
      arb_discard_packet(packet);
      ncp_get_str(&packet);
      *ptr_packet = packet;
      *forward = FORWARD_RXJN;
      break;
    default:
      *forward=FORWARD_DISCARD;
      break;
  }
} /* ncp_rxj() */



/*
+------------------------------------------------------------------------------
| Function    : ncp_ruc
+------------------------------------------------------------------------------
| Description : The function ncp_ruc() creates a Code Reject packet and returns
|               FORWARD_RUC.
|
| Parameters  : ptr_packet - packet with unknown code
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void ncp_ruc (T_desc2** ptr_packet, UBYTE* forward)
{
  T_desc2* packet;
  T_desc2* temp_desc;
  USHORT  packet_len;

  TRACE_FUNCTION( "ncp_ruc" );

  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  packet_len <<= 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  if((packet->len < packet_len) || (packet_len < 4))
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  /*
   * create a new larger packet and copy the content
   * of the old packet into the new
   */
  packet_len+=4;
  if(packet_len > PPP_MRU_MIN)
    packet_len = PPP_MRU_MIN;
  MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1 + packet_len));

  temp_desc->next = packet->next;
  temp_desc->len  = packet_len;
  memcpy(&temp_desc->buffer[4], packet->buffer, packet_len - 4);/*lint !e416 !e669 !e670 creation of out-of-bounds pointer */
  arb_discard_packet(packet);
  packet = temp_desc;

  /*
   * fill the first bytes to create a Code Reject
   */
  packet->buffer[0] = CODE_CODE_REJ;
  ppp_data->ncp.nscji++;
  packet->buffer[1] = ppp_data->ncp.nscji;/*lint !e415 access of out-of-bounds pointer */
  packet->buffer[2] = (UBYTE)(packet_len >> 8);/*lint !e415 !e416 creation and access of out-of-bounds pointer */
  packet->buffer[3] = (UBYTE)(packet_len & 0x00ff);/*lint !e415 !e416 creation and access of out-of-bounds pointer */

  /*
   * set return values
   */
  *ptr_packet=packet;
  *forward=FORWARD_RUC;

} /* ncp_ruc() */
