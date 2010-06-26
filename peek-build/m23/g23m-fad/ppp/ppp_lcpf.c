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
|             SDL-documentation (LCP-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_LCPF_C
#define PPP_LCPF_C
#endif /* !PPP_LCPF_C */

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

#include <string.h>     /* to get memcpy */
#include "ppp_arbf.h"   /* to get function interface from arb */
#include "ppp_capf.h"

#ifdef _SIMULATION_
#include <stdio.h>      /* to get sprintf */
#endif /* _SIMULATION_ */

/*==== CONST ================================================================*/

#define TYPE_MRU                  1
#define TYPE_ACCM                 2
#define TYPE_AP                   3
#define TYPE_QP                   4
#define TYPE_MAGIC                5
#define TYPE_PFC                  7
#define TYPE_ACFC                 8
#define TYPE_FCS                  9

#define LENGTH_MRU                4
#define LENGTH_ACCM               6
#define LENGTH_AP_PAP             4
#define LENGTH_AP_CHAP            5
#define LENGTH_AP_MIN             4
#define LENGTH_AP_MAX             LENGTH_AP_CHAP
#define LENGTH_MAGIC              6
#define LENGTH_PFC                2
#define LENGTH_ACFC               2
#define LENGTH_FCS                3

                /*
                 * all other configuration options not yet supported
                 */
#define LCP_CONF_REQ_LENGTH_MAX   (4 +              \
                                   LENGTH_MRU +     \
                                   LENGTH_ACCM +    \
                                   LENGTH_AP_MAX +  \
                                   LENGTH_PFC +     \
                                   LENGTH_ACFC)
#define LCP_TERM_REQ_LENGTH       6

#define ALGORITHM_MD5             5

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : lcp_init
+------------------------------------------------------------------------------
| Description : The function lcp_init() initialize Link Control Protocol (LCP)
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_init ()
{ 
  TRACE_FUNCTION( "lcp_init" );
  /*
   * initialize values
   */
  ppp_data->lcp.req_mru=PPP_MRU_DEFAULT;
  ppp_data->lcp.req_ap=PPP_AP_DEFAULT;
  ppp_data->lcp.req_accm=PPP_ACCM_DEFAULT;

  ppp_data->lcp.r_mru=PPP_MRU_DEFAULT;
  ppp_data->lcp.r_accm=PPP_ACCM_DEFAULT;
  ppp_data->lcp.r_pfc=PPP_PFC_DEFAULT;
  ppp_data->lcp.r_acfc=PPP_ACFC_DEFAULT;

  ppp_data->lcp.s_mru=PPP_MRU_DEFAULT;
  ppp_data->lcp.s_accm=PPP_ACCM_DEFAULT;
  ppp_data->lcp.s_pfc=PPP_PFC_DEFAULT;
  ppp_data->lcp.s_acfc=PPP_ACFC_DEFAULT;

  ppp_data->lcp.n_ap=PPP_AP_DEFAULT;

  ppp_data->lcp.s_rejected=0;

  ppp_data->lcp.nscri=0;
  ppp_data->lcp.nstri=0;
  ppp_data->lcp.nscji=0;

  ppp_data->lcp.scr=FALSE;
  ppp_data->lcp.str=FALSE;
  
  INIT_STATE( PPP_SERVICE_LCP , LCP_STATE );
} /* lcp_init() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_get_values
+------------------------------------------------------------------------------
| Description : The function lcp_get_values() returns negotiated values
|
| Parameters  : ptr_ap   - returns Authentication Protocol
|               ptr_mru  - returns Maximum Receive Unit
|               ptr_accm - returns Async Control Character Map
|               ptr_pfc  - returns Protocol Field Compression
|               ptr_acfc - returns Address and Control Field Compression
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_get_values (UBYTE*  ptr_ap, 
                            USHORT* ptr_mru, 
                            ULONG*  ptr_accm, 
                            UBYTE*  ptr_pfc, 
                            UBYTE*  ptr_acfc)
{
  TRACE_FUNCTION( "lcp_get_values" );

  *ptr_mru = ppp_data->lcp.r_mru;
  *ptr_accm = ppp_data->lcp.r_accm | ppp_data->lcp.s_accm;
  *ptr_ap   = ppp_data->lcp.n_ap;
  *ptr_pfc  = ppp_data->lcp.r_pfc;
  *ptr_acfc = ppp_data->lcp.r_acfc;

} /* lcp_get_values() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_fill_out_packet
+------------------------------------------------------------------------------
| Description : The function lcp_fill_out_packet() puts a LCP packet into 
|               the protocol configuration list
|
| Parameters  : pco_buf - pco list buffer
|               ptr_pos - position where to write the LCP packet, this value
|                         must get back to the calling funtion
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_fill_out_packet (UBYTE pco_buf[], USHORT* ptr_pos)
{
  USHORT  pos;
  USHORT  len_pos1, len_pos2;
  USHORT  mru;

  TRACE_FUNCTION( "lcp_fill_out_packet" );

  if((ppp_data->pco_mask & PPP_PCO_MASK_LCP_MRU) ||
     (ppp_data->pco_mask & PPP_PCO_MASK_LCP_AP)  ||
     (ppp_data->pco_mask & PPP_PCO_MASK_LCP_TWO))
  {
    pos = *ptr_pos;
#ifdef _SIMULATION_
    TRACE_EVENT_P3("parameters: pco_buf[]=%08x, ptr_pos=%08x, pos=%d",
                    pco_buf, 
                    ptr_pos, 
                    (int)pos);
#endif /* _SIMULATION_ */

    /*
     * create Configure-Request packet
     */
      /*
       * Protocol ID
       */
    pco_buf[pos] = PROTOCOL_LCP_MSB;
    pos++;
    pco_buf[pos] = PROTOCOL_LCP_LSB;
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

    if(ppp_data->pco_mask & PPP_PCO_MASK_LCP_MRU)
    {
      /*
       * Maximum Receive Unit
       */
        /*
         * if PPP_PCO_MASK_LCP_TWO is set use always s_mru
         * if PPP_PCO_MASK_LCP_TWO is not set 
         * use the smaller one of s_mru and r_mru
         */
      if((ppp_data->pco_mask & PPP_PCO_MASK_LCP_TWO) ||
         (ppp_data->lcp.s_mru < ppp_data->lcp.r_mru))
        mru = ppp_data->lcp.s_mru;
      else
        mru = ppp_data->lcp.r_mru;
      pco_buf[pos]=TYPE_MRU;
      pos++;
      pco_buf[pos]=LENGTH_MRU;
      pos++;
      pco_buf[pos]=(UBYTE)(mru >> 8);
      pos++;
      pco_buf[pos]=(UBYTE)(mru & 0x00ff);
      pos++;
    }

    if(ppp_data->pco_mask & PPP_PCO_MASK_LCP_AP)
    {
            /*
             * Authentication Protocol
             */
      switch(ppp_data->lcp.n_ap)
      {
        case PPP_AP_PAP:
          pco_buf[pos]=TYPE_AP;
          pos++;
          pco_buf[pos]=LENGTH_AP_PAP;
          pos++;
          pco_buf[pos]=PROTOCOL_PAP_MSB;
          pos++;
          pco_buf[pos]=PROTOCOL_PAP_LSB;
          pos++;
          break;
        case PPP_AP_CHAP:
          pco_buf[pos]=TYPE_AP;
          pos++;
          pco_buf[pos]=LENGTH_AP_CHAP;
          pos++;
          pco_buf[pos]=PROTOCOL_CHAP_MSB;
          pos++;
          pco_buf[pos]=PROTOCOL_CHAP_LSB;
          pos++;
          pco_buf[pos]=ALGORITHM_MD5;
          pos++;
          break;
      }
    }
        /*
         * insert packet length
         */
#ifdef _SIMULATION_
    TRACE_EVENT_P3("len_pos1=%d, len_pos2=%d, pos=%d",
                    (int)len_pos1, 
                    (int)len_pos2, 
                    (int)pos);
#endif /* _SIMULATION_ */
    pco_buf[len_pos2] = 0;
    len_pos2++;
    pco_buf[len_pos2] = (UBYTE)(pos - len_pos2 + 3);
      /*
       * insert Length of Protocol Contents
       */
    pco_buf[len_pos1] = pco_buf[len_pos2];

    if(ppp_data->pco_mask & PPP_PCO_MASK_LCP_TWO)
    {
      /*
       * create client Configure-Request packet
       */
        /*
         * Protocol ID
         */
      pco_buf[pos] = PROTOCOL_LCP_MSB;
      pos++;
      pco_buf[pos] = PROTOCOL_LCP_LSB;
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

      if(ppp_data->pco_mask & PPP_PCO_MASK_LCP_MRU)
      {
          /*
           * Maximum Receive Unit
           */
        mru = ppp_data->lcp.r_mru;
        pco_buf[pos]=TYPE_MRU;
        pos++;
        pco_buf[pos]=LENGTH_MRU;
        pos++;
        pco_buf[pos]=(UBYTE)(mru >> 8);
        pos++;
        pco_buf[pos]=(UBYTE)(mru & 0x00ff);
        pos++;
      }

          /*
           * insert packet length
           */
#ifdef _SIMULATION_
      TRACE_EVENT_P3("len_pos1=%d, len_pos2=%d, pos=%d",
                      (int)len_pos1, 
                      (int)len_pos2, 
                      (int)pos);
#endif /* _SIMULATION_ */
      pco_buf[len_pos2] = 0;
      len_pos2++;
      pco_buf[len_pos2] = (UBYTE)(pos - len_pos2 + 3);
        /*
         * insert Length of Protocol Contents
         */
      pco_buf[len_pos1] = pco_buf[len_pos2];
    }

#ifdef _SIMULATION_
    TRACE_EVENT_P6("pco_buf[%d]=%02x, pco_buf[%d]=%02x, pco_buf[%d]=%02x",
                    (int)(len_pos1),
                    pco_buf[len_pos1],
                    (int)(len_pos2 - 1),
                    pco_buf[len_pos2 - 1],
                    (int)len_pos2,
                    pco_buf[len_pos2]);
#endif /* _SIMULATION_ */
    /*
     * return new position
     */
    *ptr_pos=pos;
  }
} /* lcp_fill_out_packet() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_get_scr
+------------------------------------------------------------------------------
| Description : The function lcp_get_scr() returns a LCP
|               Configure Request packet
|
| Parameters  : ptr_packet - returns the Configure Request packet
|                            THE MEMORY FOR THE PACKET WILL ALLOCATED BY
|                            THIS FUNCTION
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_get_scr (T_desc2** ptr_packet)
{
  T_desc2* ret_desc;
  USHORT  len_pos;
  USHORT  pos;


  TRACE_FUNCTION( "lcp_get_scr" );

  /*
   * Allocate the necessary size for the data descriptor. The size is 
   * calculated as follows:
   * - take the size of a descriptor structure
   * - subtract one because of the array buffer[1] to get the size of
   *   descriptor control information
   * - add number of octets of descriptor data
   */
  MALLOC (ret_desc, (USHORT)(sizeof(T_desc2) - 1 + LCP_CONF_REQ_LENGTH_MAX));
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
  ret_desc->buffer[pos]=ppp_data->lcp.nscri;/*lint !e415 (Warning -- access of out-of-bounds pointer) */
  pos++;
    /*
     * Length field (store the position)
     */
  len_pos=pos;
  pos++;
  pos++;
      /*
       * Maximum Receive Unit
       */
  if(((ppp_data->lcp.s_rejected & (1UL << TYPE_MRU)) EQ 0) && 
     (ppp_data->lcp.s_mru NEQ PPP_MRU_DEFAULT))
  {
    ret_desc->buffer[pos]=TYPE_MRU;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
    pos++;
    ret_desc->buffer[pos]=LENGTH_MRU;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->lcp.s_mru >> 8);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->lcp.s_mru & 0x00ff);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
    pos++;
  }
      /*
       * Async Control Character Map
       */
  if(((ppp_data->lcp.s_rejected & (1UL << TYPE_ACCM)) EQ 0) && 
     (ppp_data->lcp.s_accm NEQ PPP_ACCM_DEFAULT))
  {
    ret_desc->buffer[pos]=TYPE_ACCM;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
    pos++;
    ret_desc->buffer[pos]=LENGTH_ACCM;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->lcp.s_accm >> 24);/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
    pos++;
    ret_desc->buffer[pos]=(UBYTE)((ppp_data->lcp.s_accm >> 16) & 0x000000ff);/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
    pos++;
    ret_desc->buffer[pos]=(UBYTE)((ppp_data->lcp.s_accm >>  8) & 0x000000ff);/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
    pos++;
    ret_desc->buffer[pos]=(UBYTE)(ppp_data->lcp.s_accm & 0x000000ff);/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
    pos++;
  }
      /*
       * Authentication Protocol
       * (only in server mode)
       */
  if(((ppp_data->lcp.s_rejected & (1UL << TYPE_AP)) EQ 0) && 
     (ppp_data->mode EQ PPP_SERVER) &&
     (ppp_data->lcp.n_ap NEQ PPP_AP_DEFAULT))
    if(ppp_data->lcp.n_ap EQ PPP_AP_PAP)
    {
      ret_desc->buffer[pos]=TYPE_AP;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
      pos++;
      ret_desc->buffer[pos]=LENGTH_AP_PAP;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
      pos++;
      ret_desc->buffer[pos]=PROTOCOL_PAP_MSB;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
      pos++;
      ret_desc->buffer[pos]=PROTOCOL_PAP_LSB;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
      pos++;
    }
    else
    {
      ret_desc->buffer[pos]=TYPE_AP;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
      pos++;
      ret_desc->buffer[pos]=LENGTH_AP_CHAP;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
      pos++;
      ret_desc->buffer[pos]=PROTOCOL_CHAP_MSB;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
      pos++;
      ret_desc->buffer[pos]=PROTOCOL_CHAP_LSB;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
      pos++;
      ret_desc->buffer[pos]=ALGORITHM_MD5;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
      pos++;
    }
      /*
       * Protocol Field Compression
       */
  if(((ppp_data->lcp.s_rejected & (1UL << TYPE_PFC)) EQ 0) && 
     (ppp_data->lcp.s_pfc NEQ PPP_PFC_DEFAULT))
  {
    ret_desc->buffer[pos]=TYPE_PFC;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
    pos++;
    ret_desc->buffer[pos]=LENGTH_PFC;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
    pos++;
  }
      /*
       * Address and Control Field Compression
       */
  if(((ppp_data->lcp.s_rejected & (1UL << TYPE_ACFC)) EQ 0) && 
     (ppp_data->lcp.s_acfc NEQ PPP_ACFC_DEFAULT))
  {
    ret_desc->buffer[pos]=TYPE_ACFC;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
    pos++;
    ret_desc->buffer[pos]=LENGTH_ACFC;/*lint !e661 !e662 (Warning -- Likely access/creation of out-of-bounds pointer) */
    pos++;
  }
  

  /*
   * insert packet length
   */
  ret_desc->len=pos;
  ret_desc->buffer[len_pos]=(UBYTE)(pos >> 8);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  len_pos++;
  ret_desc->buffer[len_pos]=(UBYTE)(pos & 0x00ff);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  
  /*
   * return the created packet
   */
  ppp_data->lcp.scr=TRUE;
  *ptr_packet=ret_desc;
  
} /* lcp_get_scr() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_get_str
+------------------------------------------------------------------------------
| Description : The function lcp_get_str() returns a LCP
|               Terminate Request packet
|
| Parameters  : ptr_packet - returns the Terminate Request packet
|                            THE MEMORY FOR THE PACKET WILL ALLOCATED BY
|                            THIS FUNCTION
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_get_str (T_desc2** ptr_packet)
{
  T_desc2* ret_desc;
  USHORT  pos;

  TRACE_FUNCTION( "lcp_get_str" );
  
  /*
   * Allocate the necessary size for the data descriptor. The size is 
   * calculated as follows:
   * - take the size of a descriptor structure
   * - subtract one because of the array buffer[1] to get the size of
   *   descriptor control information
   * - add number of octets of descriptor data
   */
  MALLOC (ret_desc, (USHORT)(sizeof(T_desc2) - 1 + LCP_TERM_REQ_LENGTH));
  /*
   * fill the packet
   */
  ret_desc->next = (ULONG)NULL;
  ret_desc->len  = LCP_TERM_REQ_LENGTH;
  pos            = 0;
    /*
     * Code field
     */
  ret_desc->buffer[pos] = CODE_TERM_REQ;
  pos++;
    /*
     * Identifier field
     */
  ret_desc->buffer[pos] = ppp_data->lcp.nstri;/*lint !e415 (Warning -- access of out-of-bounds pointer) */
  pos++;
    /*
     * Length field
     */
  ret_desc->buffer[pos] = 0;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  pos++;
  ret_desc->buffer[pos] = LCP_TERM_REQ_LENGTH;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  pos++;
    /*
     * Data field contains the error code
     */
  ret_desc->buffer[pos] = (U8)((ppp_data->ppp_cause >> 8) & 0x00ff);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  pos++;
  ret_desc->buffer[pos] = (U8)((ppp_data->ppp_cause)      & 0x00ff);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  pos++;

  /*
   * return the created packet
   */
  ppp_data->lcp.str = TRUE;
  *ptr_packet       = ret_desc;
} /* lcp_get_str() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_rcr
+------------------------------------------------------------------------------
| Description : The function lcp_rcr() analyzes the given 
|               Configure Request packet and returns either FORWARD_RCRP or
|               FORWARD_RCRN depend on the result of the analysis. 
|               The packet pointer points to an appropriate response packet. 
|
| Parameters  : ptr_packet  - pointer to a Configure Request packet
|               forward     - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_rcr (T_desc2** ptr_packet, UBYTE* isnew, UBYTE* forward)
{
  T_desc2* packet;
  USHORT  packet_len;
  UBYTE   type_len;
  USHORT  pos;
  USHORT  copy_pos;
  USHORT  analyzed;
  UBYTE   code_ret;
  USHORT  ap_id;
  UBYTE   error_found;

  TRACE_FUNCTION( "lcp_rcr" );
  
  /*
   * check for correct length field
   */
  packet=*ptr_packet;
  packet_len=packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len=packet_len << 8;
  packet_len+=packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
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
    if(packet->buffer[pos] < 2)
    {
      *forward=FORWARD_DISCARD;
      return;
    }
    pos += packet->buffer[pos];
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
  if((ppp_data->lcp.rcr) && (ppp_data->lcp.lrcri EQ packet->buffer[1]))/*lint !e415 (Warning -- access of out-of-bounds pointer) */
    *isnew=FALSE;
  ppp_data->lcp.lrcri=packet->buffer[1];/*lint !e415 (Warning -- access of out-of-bounds pointer) */
  ppp_data->lcp.rcr=TRUE;
  /*
   * analyze configuration options
   */
  ppp_data->lcp.r_mru  = PPP_MRU_DEFAULT;
  ppp_data->lcp.r_accm = PPP_ACCM_DEFAULT;
  ppp_data->lcp.r_pfc  = PPP_PFC_DEFAULT;
  ppp_data->lcp.r_acfc = PPP_ACFC_DEFAULT;
  if(ppp_data->mode EQ PPP_CLIENT)
    ppp_data->lcp.n_ap=PPP_AP_DEFAULT;
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
    type_len=packet->buffer[pos + 1];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
    switch(packet->buffer[pos])/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
    {
      /*
       * yet supported configuration options
       */
      case TYPE_MRU:
        ppp_data->lcp.r_mru = packet->buffer[pos + 2];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
        ppp_data->lcp.r_mru = (ppp_data->lcp.r_mru << 8);
        ppp_data->lcp.r_mru+= packet->buffer[pos + 3];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */

        switch(code_ret)
        {
          case CODE_CONF_ACK:
            if(((analyzed & (1UL << TYPE_MRU)) EQ 0) &&
               (type_len EQ LENGTH_MRU) && 
               (ppp_data->lcp.r_mru >= PPP_MRU_MIN))
            {
              analyzed|=(1UL << TYPE_MRU);
              pos+=LENGTH_MRU;
              break;
            }
            code_ret=CODE_CONF_NAK;
          case CODE_CONF_NAK:
            if((analyzed & (1UL << TYPE_MRU)) EQ 0)
            {
              analyzed|=(1UL << TYPE_MRU);

              error_found=FALSE;
              if(type_len < LENGTH_MRU)
              {
                T_desc2* temp_desc;

                /*
                 * create a new larger packet and copy the content
                 * of the old packet into the new
                 */
                MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1 
                                                           + packet_len 
                                                           + LENGTH_MRU
                                                           - type_len));
                temp_desc->next=packet->next;
                temp_desc->len=packet_len + LENGTH_MRU - type_len;
                memcpy(temp_desc->buffer, packet->buffer, pos);/*lint !e669 !e670 (Warning -- Possible data overrun, Possible access beyond array ) */
                memcpy(&temp_desc->buffer[pos + LENGTH_MRU - type_len], 
                       &packet->buffer[pos], packet_len - pos);/*lint !e669 !e670 !e662 (Warning -- Possible data overrun, Possible access beyond array ) */
                arb_discard_packet(packet);
                packet_len += (LENGTH_MRU - type_len);
                pos        += (LENGTH_MRU - type_len);
                packet=temp_desc;

                error_found=TRUE;
                ppp_data->lcp.r_mru=PPP_MRU_MIN;
              }
              else if(ppp_data->lcp.r_mru < PPP_MRU_MIN)
              {
                error_found=TRUE;
                ppp_data->lcp.r_mru=PPP_MRU_MIN;
              }
              else if(type_len > LENGTH_MRU)
              {
                error_found=TRUE;
              }
              if(error_found EQ TRUE)
              {
                packet->buffer[copy_pos]=TYPE_MRU;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                copy_pos++;
                packet->buffer[copy_pos]=LENGTH_MRU;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)(ppp_data->lcp.r_mru >> 8);/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                copy_pos++;
                packet->buffer[copy_pos]=(UBYTE)(ppp_data->lcp.r_mru & 0x00ff);/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                copy_pos++;
              }
              pos+= type_len;
              break;
            }
            code_ret = CODE_CONF_REJ;
            copy_pos = 4;
          case CODE_CONF_REJ:
            if((analyzed & (1UL << TYPE_MRU)) EQ 0)
            {
              analyzed|=(1UL << TYPE_MRU);
              pos+= type_len;
              break;
            }
            while(type_len > 0)
            {
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
              copy_pos++;
              pos++;
              type_len--;
            }
            break;
        }
        break;
      case TYPE_ACCM:
        ppp_data->lcp.r_accm=packet->buffer[pos + 2];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
        ppp_data->lcp.r_accm=(ppp_data->lcp.r_accm << 8);
        ppp_data->lcp.r_accm+=packet->buffer[pos + 3];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
        ppp_data->lcp.r_accm=(ppp_data->lcp.r_accm << 8);
        ppp_data->lcp.r_accm+=packet->buffer[pos + 4];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
        ppp_data->lcp.r_accm=(ppp_data->lcp.r_accm << 8);
        ppp_data->lcp.r_accm+=packet->buffer[pos + 5];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */

        switch(code_ret)
        {
          case CODE_CONF_ACK:
            if(((analyzed & (1UL << TYPE_ACCM)) EQ 0) &&
               (type_len EQ LENGTH_ACCM))
            {
              analyzed|=(1UL << TYPE_ACCM);
              pos+=LENGTH_ACCM;
              break;
            }
            code_ret=CODE_CONF_NAK;
          case CODE_CONF_NAK:
            if((analyzed & (1UL << TYPE_ACCM)) EQ 0)
            {
              analyzed|=(1UL << TYPE_ACCM);

              error_found=FALSE;
              if(type_len < LENGTH_ACCM)
              {
                T_desc2* temp_desc;

                /*
                 * create a new larger packet and copy the content
                 * of the old packet into the new
                 */
                MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1 
                                                           + packet_len 
                                                           + LENGTH_ACCM
                                                           - type_len));
                temp_desc->next=packet->next;
                temp_desc->len=packet_len + LENGTH_ACCM - type_len;
                memcpy(temp_desc->buffer, packet->buffer, pos);/*lint !e669 !e670 (Warning -- Possible data overrun, Possible access beyond array ) */
                memcpy(&temp_desc->buffer[pos + LENGTH_ACCM - type_len], 
                       &packet->buffer[pos], packet_len - pos);/*lint !e669 !e670 !e662 (Warning -- Possible data overrun, Possible access beyond array ) */
                arb_discard_packet(packet);
                packet_len += (LENGTH_ACCM - type_len);
                pos        += (LENGTH_ACCM - type_len);
                packet=temp_desc;

                error_found=TRUE;
              }
              else if(type_len > LENGTH_ACCM)
              {
                error_found=TRUE;
              }
              if(error_found EQ TRUE)
              {
                memmove(&packet->buffer[copy_pos], 
                        &packet->buffer[pos],
                        LENGTH_ACCM);/*lint !e669 !e670 !e662 (Warning -- Possible data overrun, Possible access beyond array ) */
                packet->buffer[copy_pos + 1]=LENGTH_ACCM;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                copy_pos+=LENGTH_ACCM;
              }
              pos+=type_len;
              break;
            }
            code_ret = CODE_CONF_REJ;
            copy_pos = 4;
          case CODE_CONF_REJ:
            if((analyzed & (1UL << TYPE_ACCM)) EQ 0)
            {
              analyzed|=(1UL << TYPE_ACCM);
              pos+=type_len;
              break;
            }
            while(type_len > 0)
            {
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
              copy_pos++;
              pos++;
              type_len--;
            }
            break;
        }
        break;
      case TYPE_AP:
        ap_id  = (packet->buffer[pos + 2]) << 8;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
        ap_id |= packet->buffer[pos + 3];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */

        switch(code_ret)
        {
          case CODE_CONF_ACK:
            if(((analyzed & (1UL << TYPE_AP)) EQ 0) &&
               (ppp_data->mode EQ PPP_CLIENT))
            {
              if((ap_id EQ DTI_PID_PAP) &&
                 (type_len EQ LENGTH_AP_PAP) &&
                 (ppp_data->lcp.req_ap EQ PPP_AP_PAP))
              {
                analyzed|=(1UL << TYPE_AP);
                pos+=type_len;
                ppp_data->lcp.n_ap = PPP_AP_PAP;
                break;
              }
              if((ap_id EQ DTI_PID_PAP) &&
                 (type_len EQ LENGTH_AP_PAP) &&
                 (ppp_data->lcp.req_ap EQ PPP_AP_AUTO))
              {
                analyzed|=(1UL << TYPE_AP);
                pos+=type_len;
                ppp_data->lcp.n_ap = PPP_AP_PAP;
                break;
              }
              if((ap_id EQ DTI_PID_CHAP) &&
                 (type_len EQ LENGTH_AP_CHAP) &&
                 (ppp_data->lcp.req_ap EQ PPP_AP_CHAP))
              {
                analyzed|=(1UL << TYPE_AP);
                pos+=type_len;
                ppp_data->lcp.n_ap = PPP_AP_CHAP;
                break;
              }
              if((ap_id EQ DTI_PID_CHAP) &&
                 (type_len EQ LENGTH_AP_CHAP) &&
                 (ppp_data->lcp.req_ap EQ PPP_AP_AUTO))
              {
                analyzed|=(1UL << TYPE_AP);
                pos+=type_len;
                ppp_data->lcp.n_ap = PPP_AP_CHAP;
                break;
              }
            }
            code_ret=CODE_CONF_NAK;
          case CODE_CONF_NAK:
            if(((analyzed & (1UL << TYPE_AP)) EQ 0) &&
               (ppp_data->mode EQ PPP_CLIENT) &&
               (ppp_data->lcp.req_ap NEQ PPP_AP_NO))
            {
              analyzed|=(1UL << TYPE_AP);

              error_found=FALSE;
              if(((ppp_data->lcp.req_ap EQ PPP_AP_PAP) && 
                  (type_len < LENGTH_AP_PAP)) || 
                 ((ppp_data->lcp.req_ap EQ PPP_AP_CHAP) && 
                  (type_len < LENGTH_AP_CHAP)))
              {
                T_desc2* temp_desc;

                /*
                 * create a new larger packet and copy the content
                 * of the old packet into the new
                 */
                if(ppp_data->lcp.req_ap EQ PPP_AP_PAP)
                {
                  MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1 
                                                             + packet_len 
                                                             + LENGTH_AP_PAP
                                                             - type_len));
                  temp_desc->len=packet_len + LENGTH_AP_PAP - type_len;
                  memcpy(&temp_desc->buffer[pos + LENGTH_AP_PAP - type_len],
                         &packet->buffer[pos], packet_len - pos);/*lint !e669 !e670 !e662 (Warning -- Possible data overrun, Possible access beyond array ) */
                  packet_len += (LENGTH_AP_PAP - type_len);
                  pos        += (LENGTH_AP_PAP - type_len);
                }
                else
                {
                  MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1 
                                                             + packet_len 
                                                             + LENGTH_AP_CHAP
                                                             - type_len));
                  temp_desc->len=packet_len + LENGTH_AP_CHAP - type_len;
                  memcpy(&temp_desc->buffer[pos + LENGTH_AP_CHAP - type_len],
                         &packet->buffer[pos], packet_len - pos);/*lint !e669 !e670 !e662 (Warning -- Possible data overrun, Possible access beyond array ) */
                  packet_len += (LENGTH_AP_CHAP - type_len);
                  pos        += (LENGTH_AP_CHAP - type_len);
                }
                temp_desc->next=packet->next;
                memcpy(temp_desc->buffer, packet->buffer, pos);/*lint !e669 !e670 (Warning -- Possible data overrun, Possible access beyond array ) */
                arb_discard_packet(packet);
                packet=temp_desc;

                error_found=TRUE;
              }
              else if(((ppp_data->lcp.req_ap EQ PPP_AP_PAP) && 
                       (ap_id NEQ DTI_PID_PAP)) || 
                      ((ppp_data->lcp.req_ap EQ PPP_AP_CHAP) && 
                       (ap_id NEQ DTI_PID_CHAP)))
              {
                error_found=TRUE;
              }
              else if(((ppp_data->lcp.req_ap EQ PPP_AP_PAP) && 
                       (type_len > LENGTH_AP_PAP)) || 
                      ((ppp_data->lcp.req_ap EQ PPP_AP_CHAP) && 
                       (type_len > LENGTH_AP_CHAP)))
              {
                error_found=TRUE;
              }
              if(error_found EQ TRUE)
              {
                packet->buffer[copy_pos]=TYPE_AP;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                copy_pos++;
                if(ppp_data->lcp.req_ap EQ PPP_AP_PAP)
                {
                  packet->buffer[copy_pos]=LENGTH_AP_PAP;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                  copy_pos++;
                  packet->buffer[copy_pos]=PROTOCOL_PAP_MSB;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                  copy_pos++;
                  packet->buffer[copy_pos]=PROTOCOL_PAP_LSB;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                  copy_pos++;
                }
                else
                {
                  packet->buffer[copy_pos]=LENGTH_AP_CHAP;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                  copy_pos++;
                  packet->buffer[copy_pos]=PROTOCOL_CHAP_MSB;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                  copy_pos++;
                  packet->buffer[copy_pos]=PROTOCOL_CHAP_LSB;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                  copy_pos++;
                  packet->buffer[copy_pos]=ALGORITHM_MD5;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                  copy_pos++;
                }
              }
              pos+=type_len;
              break;
            }
            code_ret=CODE_CONF_REJ;
            copy_pos=4;
          case CODE_CONF_REJ:
            if(((analyzed & (1UL << TYPE_AP)) EQ 0) &&
               (ppp_data->mode EQ PPP_CLIENT) &&
               (ppp_data->lcp.req_ap NEQ PPP_AP_NO))
            {
              analyzed|=(1UL << TYPE_AP);
              pos+=type_len;
              break;
            }
            while(type_len > 0)
            {
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
              copy_pos++;
              pos++;
              type_len--;
            }
            break;
        }
        break;
      case TYPE_PFC:
        ppp_data->lcp.r_pfc=TRUE;

        switch(code_ret)
        {
          case CODE_CONF_ACK:
            if(((analyzed & (1UL << TYPE_PFC)) EQ 0) &&
               (type_len EQ LENGTH_PFC))
            {
              analyzed|=(1UL << TYPE_PFC);
              pos+=LENGTH_PFC;
              break;
            }
            code_ret=CODE_CONF_NAK;
          case CODE_CONF_NAK:
            if((analyzed & (1UL << TYPE_PFC)) EQ 0)
            {
              analyzed|=(1UL << TYPE_PFC);

              if(type_len > LENGTH_PFC)
              {
                memmove(&packet->buffer[copy_pos], 
                        &packet->buffer[pos],
                        LENGTH_PFC);/*lint !e669 !e670 !e662 (Warning -- Possible data overrun, Possible access beyond array ) */
                packet->buffer[copy_pos + 1]=LENGTH_PFC;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                copy_pos+=LENGTH_PFC;
              }
              pos+=type_len;
              break;
            }
            code_ret=CODE_CONF_REJ;
            copy_pos=4;
          case CODE_CONF_REJ:
            if((analyzed & (1UL << TYPE_PFC)) EQ 0)
            {
              analyzed|=(1UL << TYPE_PFC);
              pos+=type_len;
              break;
            }
            while(type_len > 0)
            {
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
              copy_pos++;
              pos++;
              type_len--;
            }
            break;
        }
        break;
      case TYPE_ACFC:
        ppp_data->lcp.r_acfc=TRUE;

        switch(code_ret)
        {
          case CODE_CONF_ACK:
            if(((analyzed & (1UL << TYPE_ACFC)) EQ 0) &&
               (type_len EQ LENGTH_ACFC))
            {
              analyzed|=(1UL << TYPE_ACFC);
              pos+=LENGTH_ACFC;
              break;
            }
            code_ret=CODE_CONF_NAK;
          case CODE_CONF_NAK:
            if((analyzed & (1UL << TYPE_ACFC)) EQ 0)
            {
              analyzed|=(1UL << TYPE_ACFC);

              if(type_len > LENGTH_ACFC)
              {
                memmove(&packet->buffer[copy_pos], 
                        &packet->buffer[pos],
                        LENGTH_ACFC);/*lint !e669 !e670 !e662 (Warning -- Possible data overrun, Possible access beyond array ) */
                packet->buffer[copy_pos + 1]=LENGTH_ACFC;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
                copy_pos+=LENGTH_ACFC;
              }
              pos+=type_len;
              break;
            }
            code_ret=CODE_CONF_REJ;
            copy_pos=4;
          case CODE_CONF_REJ:
            if((analyzed & (1UL << TYPE_ACFC)) EQ 0)
            {
              analyzed|=(1UL << TYPE_ACFC);
              pos+=type_len;
              break;
            }
            while(type_len > 0)
            {
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
              copy_pos++;
              pos++;
              type_len--;
            }
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
              packet->buffer[copy_pos]=packet->buffer[pos];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
              copy_pos++;
              pos++;
              type_len--;
            }
            break;
        }
    }
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
    packet->buffer[2]=(UBYTE)(copy_pos >> 8);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
    packet->buffer[3]=(UBYTE)(copy_pos & 0x00ff);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
    return;
  }

#ifdef _SIMULATION_
  ppp_trace_desc(packet);
#endif
  /*
   * all configuration options are acceptable
   */
  *forward=FORWARD_RCRP;
} /* lcp_rcr() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_rca
+------------------------------------------------------------------------------
| Description : The function lcp_rca() checks whether the given 
|               Configure Ack packet is valid and if so it returns
|               FORWARD_RCA.
|
| Parameters  : packet  - Configure Ack packet
|               forward - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_rca (T_desc2* packet, UBYTE* forward)
{ 
  USHORT  packet_len;

  TRACE_FUNCTION( "lcp_rca" );

  packet_len=packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len=packet_len << 8;
  packet_len+=packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if((packet_len < 4) || (packet_len > packet->len))
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  if((ppp_data->lcp.scr EQ FALSE) || 
     (packet->buffer[1] NEQ ppp_data->lcp.nscri))/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
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
  ppp_data->lcp.nscri++;
  *forward=FORWARD_RCA;
} /* lcp_rca() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_rcn
+------------------------------------------------------------------------------
| Description : The function lcp_rcn() analyze the given 
|               Configure Nak packet, changes some requested values and returns
|               FORWARD_RCN
|               The packet pointer points to an appropriate response packet.
|
| Parameters  : ptr_packet - Configure Nak packet
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_rcn (T_desc2** ptr_packet, UBYTE* forward)
{ 
  T_desc2* packet;
  USHORT   packet_len;
  UBYTE    type_len;
  USHORT   pos;
  USHORT   mru;
  ULONG    accm;

  TRACE_FUNCTION( "lcp_rcn" );
  
  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if((packet_len > packet->len) || (packet_len < 4))
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  if((ppp_data->lcp.scr EQ FALSE) || 
     (packet->buffer[1] NEQ ppp_data->lcp.nscri))/*lint !e415  (Warning -- access of out-of-bounds pointer) */
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
    if(packet->buffer[pos] < 2)
    {
      *forward=FORWARD_DISCARD;
      return;
    }
    pos += packet->buffer[pos];
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
    type_len=packet->buffer[pos + 1];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
    switch(packet->buffer[pos])/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
    {
      /*
       * yet supported configuration options
       */
      case TYPE_MRU:
        mru = packet->buffer[pos + 2];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
        mru = mru << 8;
        mru+= packet->buffer[pos + 3];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */

        if((type_len EQ LENGTH_MRU) && 
           (mru >= PPP_MRU_MIN) &&
           (mru <= ppp_data->lcp.req_mru))
        {
          /*
           * clear reject flag and set new mru value
           */
          ppp_data->lcp.s_rejected &= ~(1UL << TYPE_MRU);
          ppp_data->lcp.s_mru=mru;
        }
        break;
      case TYPE_ACCM:
        if(type_len EQ LENGTH_ACCM)
        {
          accm=packet->buffer[pos + 2];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
          accm=accm << 8;
          accm+=packet->buffer[pos + 3];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
          accm=accm << 8;
          accm+=packet->buffer[pos + 4];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
          accm=accm << 8;
          accm+=packet->buffer[pos + 5];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */

          /*
           * clear reject flag and set new accm value
           */
          ppp_data->lcp.s_rejected &= ~(1UL << TYPE_ACCM);
          ppp_data->lcp.s_accm |= accm;
        }
        break;
      case TYPE_AP:
        if(ppp_data->lcp.n_ap EQ PPP_AP_CHAP)
        {
          TRACE_EVENT("LCP: CHAP rejected by client");
        }
        else if(ppp_data->lcp.n_ap EQ PPP_AP_PAP)
        {
          TRACE_EVENT("LCP: PAP rejected by client");
        }
        if((ppp_data->lcp.req_ap EQ PPP_AP_AUTO) &&
           (ppp_data->mode EQ PPP_SERVER))
        {
          /*
           * clear reject flag ans set new authentication protocol value
           */
          ppp_data->lcp.s_rejected &= ~(1UL << TYPE_AP);

          if((packet->buffer[pos + 2] EQ PROTOCOL_CHAP_MSB) &&
             (packet->buffer[pos + 3] EQ PROTOCOL_CHAP_LSB))/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
          {
            ppp_data->lcp.n_ap=PPP_AP_CHAP;
          }
          else if((packet->buffer[pos + 2] EQ PROTOCOL_PAP_MSB) &&
                  (packet->buffer[pos + 3] EQ PROTOCOL_PAP_LSB))/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
          {
            ppp_data->lcp.n_ap=PPP_AP_PAP;
          }
          else
          {
            ppp_data->lcp.n_ap=PPP_AP_NO;
          }
        }
        break;
      case TYPE_PFC:
        if(type_len EQ LENGTH_PFC)
        {
          /*
           * clear reject flag ans set new pfc value
           */
          ppp_data->lcp.s_rejected &= ~(1UL << TYPE_PFC);
          ppp_data->lcp.s_pfc=TRUE;
        }
        break;
      case TYPE_ACFC:
        if(type_len EQ LENGTH_ACFC)
        {
          /*
           * clear reject flag ans set new acfc value
           */
          ppp_data->lcp.s_rejected &= ~(1UL << TYPE_ACFC);
          ppp_data->lcp.s_acfc=TRUE;
        }
        break;
    }
    pos+=type_len;
  }
  /*
   * free this packet and create a new with changed configuration options
   */
  arb_discard_packet(packet);
  *forward=FORWARD_RCN;
  ppp_data->lcp.nscri++;
  lcp_get_scr(&packet);
  *ptr_packet=packet;
} /* lcp_rcn() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_rcj
+------------------------------------------------------------------------------
| Description : The function lcp_rcj() analyze the given 
|               Configure Reject packet, marks some values as rejected and
|               returns FORWARD_RCJ
|               The packet pointer points to an appropriate response packet.
|
| Parameters  : ptr_packet  - pointer to a Configure Reject packet
|               forward     - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_rcj (T_desc2** ptr_packet, UBYTE* forward)
{
  T_desc2* packet;
  USHORT  packet_len;
  UBYTE   type_len;
  USHORT  pos;

  TRACE_FUNCTION( "lcp_rcj" );
  
  /*
   * check for correct length field
   */
  packet=*ptr_packet;
  packet_len=packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len=packet_len << 8;
  packet_len+=packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if((packet_len > packet->len) || (packet_len < 4))
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  if((ppp_data->lcp.scr EQ FALSE) || 
     (packet->buffer[1] NEQ ppp_data->lcp.nscri))/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
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
    if(packet->buffer[pos] < 2)
    {
      *forward=FORWARD_DISCARD;
      return;
    }
    pos += packet->buffer[pos];
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
    type_len=packet->buffer[pos + 1];/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
    switch(packet->buffer[pos])/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer) */
    {
      /*
       * yet supported configuration options
       */
      case TYPE_MRU:
        ppp_data->lcp.s_rejected |= (1UL << TYPE_MRU);
        break;
      case TYPE_ACCM:
        ppp_data->lcp.s_rejected |= (1UL << TYPE_ACCM);
        break;
      case TYPE_PFC:
        ppp_data->lcp.s_rejected |= (1UL << TYPE_PFC);
        break;
      case TYPE_ACFC:
        ppp_data->lcp.s_rejected |= (1UL << TYPE_ACFC);
        break;
      case TYPE_AP:
        if(ppp_data->lcp.n_ap EQ PPP_AP_CHAP)
        {
          TRACE_EVENT("LCP: CHAP rejected by client");
        }
        else if(ppp_data->lcp.n_ap EQ PPP_AP_PAP)
        {
          TRACE_EVENT("LCP: PAP rejected by client");
        }
        if((ppp_data->lcp.req_ap EQ PPP_AP_AUTO) &&
           (ppp_data->mode EQ PPP_SERVER))
        {
          ppp_data->lcp.s_rejected |= (1UL << TYPE_AP);
        }
        break;
    }
    pos+=type_len;
  }
  /*
   * free this packet and create a new with changed configuration options
   */
  arb_discard_packet(packet);
  *forward=FORWARD_RCN;
  ppp_data->lcp.nscri++;
  lcp_get_scr(&packet);
  *ptr_packet=packet;
} /* lcp_rcj() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_rtr
+------------------------------------------------------------------------------
| Description : The function lcp_rtr() creates a Terminate Ack packet. 
|
| Parameters  : ptr_packet  - pointer to a Terminate Request packet
|               forward     - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_rtr (T_desc2** ptr_packet, UBYTE* forward)
{ 
  T_desc2* packet;
  USHORT  packet_len;

  TRACE_FUNCTION( "lcp_rtr" );
  
  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
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
} /* lcp_rtr() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_rta
+------------------------------------------------------------------------------
| Description : The function lcp_rta() checks whether the given 
|               Terminate Ack packet is valid and if so it returns
|               FORWARD_RTA
|
| Parameters  : packet  - Terminate Ack packet
|               forward - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_rta (T_desc2* packet, UBYTE* forward)
{ 
  USHORT  packet_len;

  TRACE_FUNCTION( "lcp_rta" );
  
  /*
   * check for correct length field
   */
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if((packet_len < 4) || (packet_len > packet->len))
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  if((ppp_data->lcp.str EQ FALSE) || 
     (packet->buffer[1] NEQ ppp_data->lcp.nstri))/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
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
  ppp_data->lcp.nstri++;
  *forward=FORWARD_RTA;
} /* lcp_rta() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_rxj
+------------------------------------------------------------------------------
| Description : The function lcp_rxj() analyzes whether the given Code Reject
|               is acceptable. If not it returns FORWARD_RXJN
|
| Parameters  : ptr_packet - Pointer to a Code Reject packet
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_rxj (T_desc2** ptr_packet, UBYTE* forward)
{
  USHORT  packet_len;
  T_desc2* packet;

  TRACE_FUNCTION( "lcp_rxj" );
  
  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if((packet_len < 5) || (packet_len > packet->len))
  {
    *forward=FORWARD_DISCARD;
    return;
  }
  
  switch(packet->buffer[4])/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  {
    case CODE_CONF_REQ:
    case CODE_CONF_REJ:
    case CODE_CONF_NAK:
    case CODE_CONF_ACK:
    case CODE_TERM_REQ:
    case CODE_TERM_ACK:
    case CODE_CODE_REJ:
    case CODE_PROT_REJ:
      arb_discard_packet(packet);
      ppp_data->lcp.nstri++;
      lcp_get_str(&packet);
      *ptr_packet=packet;
      *forward=FORWARD_RXJN;
      break;
    default:
      *forward=FORWARD_DISCARD;
      break;
  }
} /* lcp_rxj() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_rpj
+------------------------------------------------------------------------------
| Description : The function lcp_rpj() analyzes which protocol is rejected.
|
| Parameters  : packet  - Protocol Reject packet
|               forward - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_rpj (T_desc2* packet, UBYTE* forward)
{
  USHORT  packet_len;
  USHORT  rej_protocol;

  TRACE_FUNCTION( "lcp_rpj" );
  
  /*
   * check for correct length field
   */
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if((packet_len < 6) || (packet_len > packet->len))
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  /*
   * get rejected protocol
   */
  rej_protocol=packet->buffer[4];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  rej_protocol=rej_protocol << 8;
  rej_protocol+=packet->buffer[5];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

  /* 
   * analyze rejected protocol
   */
  switch(rej_protocol)
  {
    case DTI_PID_LCP:
      arb_discard_packet(packet);
      *forward=FORWARD_RPJ_LCP;
      break;
    case DTI_PID_PAP:
      arb_discard_packet(packet);
      *forward=FORWARD_RPJ_PAP;
      break;
    case DTI_PID_CHAP:
      arb_discard_packet(packet);
      *forward=FORWARD_RPJ_CHAP;
      break;
    case DTI_PID_IPCP:
      arb_discard_packet(packet);
      *forward=FORWARD_RPJ_IPCP;
      break;
    case DTI_PID_IP:
      arb_discard_packet(packet);
      *forward=FORWARD_RPJ_IP;
      break;
    case DTI_PID_CTCP:
      arb_discard_packet(packet);
      *forward=FORWARD_RPJ_CTCP;
      break;
    case DTI_PID_UTCP:
      arb_discard_packet(packet);
      *forward=FORWARD_RPJ_UTCP;
      break;
    default:
      *forward=FORWARD_DISCARD;
      break;
  }
} /* lcp_rpj() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_rer
+------------------------------------------------------------------------------
| Description : The function lcp_rer() creates an Echo Reply
|
| Parameters  : ptr_packet - Echo Request packet
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_rer (T_desc2** ptr_packet, UBYTE* forward)
{ 
  T_desc2* packet;
  USHORT  packet_len;

  TRACE_FUNCTION( "lcp_rer" );
  
  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if((packet_len < 8) || (packet_len > packet->len))
  {
    *forward=FORWARD_DISCARD;
    return;
  }

  /* 
   * insert zero for magic number because we not yet support magic number
   * negotiation
   */
  packet->buffer[4]=0;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet->buffer[5]=0;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet->buffer[6]=0;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet->buffer[7]=0;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

  /*
   * change Code field
   */
  packet->buffer[0]=CODE_ECHO_REP;

  /*
   * set return value
   */
  *forward=FORWARD_RER;
} /* lcp_rer() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_rep_rdr
+------------------------------------------------------------------------------
| Description : The function lcp_rep_rdr() just analyzes the given 
|               Magic Number.
|
| Parameters  : packet  - Echo Reply packet
|               forward - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_rep_rdr (T_desc2* packet, UBYTE* forward)
{ 
  TRACE_FUNCTION( "lcp_rep_rdr" );
  
  *forward=FORWARD_DISCARD;
} /* lcp_rep_rdr() */



/*
+------------------------------------------------------------------------------
| Function    : lcp_ruc
+------------------------------------------------------------------------------
| Description : The function lcp_ruc() creates a Code Reject packet and returns 
|               FORWARD_RUC.
|
| Parameters  : ptr_packet - packet with unknown code
|               forward    - returns result of analysis
|
+------------------------------------------------------------------------------
*/
GLOBAL void lcp_ruc (T_desc2** ptr_packet, UBYTE* forward)
{ 
  T_desc2* packet;
  T_desc2* temp_desc;
  USHORT  packet_len;

  TRACE_FUNCTION( "lcp_ruc" );
  
  /*
   * check for correct length field
   */
  packet = *ptr_packet;
  packet_len = packet->buffer[2];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet_len = packet_len << 8;
  packet_len+= packet->buffer[3];/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  if((packet_len < 4)  || (packet_len > packet->len))
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
  memcpy(&temp_desc->buffer[4], packet->buffer, packet_len - 4);/*lint !e669 !e670 !e416 (Warning -- Possible data overrun, Possible access beyond array ) */
  arb_discard_packet(packet);
  packet = temp_desc;

  /* 
   * fill the first bytes to create a Code Reject
   */
  packet->buffer[0] = CODE_CODE_REJ;
  ppp_data->lcp.nscji++;
  packet->buffer[1] = ppp_data->lcp.nscji;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet->buffer[2] = (UBYTE)(packet_len >> 8);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */
  packet->buffer[3] = (UBYTE)(packet_len & 0x00ff);/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer) */

  /*
   * set return values
   */
  *ptr_packet = packet;
  *forward    = FORWARD_RUC;

} /* lcp_ruc() */
