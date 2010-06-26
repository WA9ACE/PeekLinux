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
|  Purpose :  This modul is part of the entity LLC and implements all labels
|             in the SDL-documentation (TX-statemachine) that are called from
|             more than one diagram/place.
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_TXL_C
#define LLC_TXL_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

#include "llc_uitxs.h"  /* to get signal interface to UITX */
#include "llc_itxs.h"  /* to get signal interface to ITX */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/
#ifdef TRACE_EVE

LOCAL void tx_trace_outprim_para(UBYTE sapi, T_sdu *frame)
{
  int len = BYTELEN(frame->l_buf);
  
  if( (len >= U_CTRL_MIN_OCTETS) && ((frame->buf[(frame->o_buf/8)+1] & U_FRAME_MASK) EQ U_FRAME_ID) )
  {
    switch (frame->buf[(frame->o_buf/8)+1] & 0x0F)
    {
      case U_FRAME_DM:   TRACE_2_OUT_PARA("DM s:%d len:%d", sapi, len);   break;
      case U_FRAME_DISC: TRACE_2_OUT_PARA("DISC s:%d len:%d", sapi, len); break;
      case U_FRAME_UA:   TRACE_2_OUT_PARA("UA s:%d len:%d", sapi, len);   break;
      case U_FRAME_SABM: TRACE_2_OUT_PARA("SABM s:%d len:%d", sapi, len); break;
      case U_FRAME_FRMR: TRACE_2_OUT_PARA("FRMR s:%d len:%d", sapi, len); break;
      case U_FRAME_XID:  TRACE_2_OUT_PARA("XID s:%d len:%d", sapi, len);  break;
      default:           TRACE_2_OUT_PARA("U ??? s:%d len:%d", sapi, len);break;
    }
  }
  else 
  if( (len >= UI_CTRL_MIN_OCTETS) && ((frame->buf[(frame->o_buf/8)+1] & UI_FRAME_MASK) EQ UI_FRAME_ID) )
  {
    T_FRAME_NUM nr;

    USHORT cipher = (USHORT)(frame->buf[(frame->o_buf/8)+2] >> 1) & 0x0001;

    nr  = (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+1] & 0x07) << 6;
    nr |= (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+2] >> 2);

    TRACE_4_OUT_PARA("UI s:%d len:%d nr:%d c:%d", sapi, len, nr, cipher);
  }
  else
  if ((len >= I_CTRL_MIN_OCTETS) && (frame->buf[(frame->o_buf/8)+1] & I_FRAME_MASK) EQ I_FRAME_ID)
  {
    T_FRAME_NUM ns, nr;
    ns = (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+1] & 0x1F) << 4;
    ns |= (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+2] >> 4);

    nr = (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+2] & 0x07) << 6;
    nr |= (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+3] >> 2);

    /*
     * Determine the command of the I frame (S1 & S2 bits)
     */
    switch (frame->buf[(frame->o_buf/8)+3] & 0x03)
    {
      case I_FRAME_RR:  TRACE_4_OUT_PARA("I-RR s:%d len:%d nr:%d ns:%d", sapi, len, nr, ns);  break;
      case I_FRAME_ACK: TRACE_4_OUT_PARA("I-ACK s:%d len:%d nr:%d ns:%d", sapi, len, nr, ns); break;
      case I_FRAME_RNR: TRACE_4_OUT_PARA("I-RNR s:%d len:%d nr:%d ns:%d", sapi, len, nr, ns); break;
      case I_FRAME_SACK:TRACE_4_OUT_PARA("I-SACK s:%d len:%d nr:%d ns:%d", sapi, len, nr, ns);break;
      default:          TRACE_2_OUT_PARA("I ??? s:%d len:%d", sapi, len);                     break;
    }
  }
  else 
  if( (len >= S_CTRL_MIN_OCTETS) && (frame->buf[(frame->o_buf/8)+1] & S_FRAME_MASK) EQ S_FRAME_ID)
  {
    T_FRAME_NUM nr;

    nr = (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+1] & 0x07) << 6;
    nr |= (T_FRAME_NUM)(frame->buf[(frame->o_buf/8)+2] >> 2);

    switch (frame->buf[(frame->o_buf/8)+2] & 0x03)
    {
      case I_FRAME_RR:  TRACE_3_OUT_PARA("S-RR s:%d len:%d nr:%d", sapi, len, nr);  break;
      case I_FRAME_ACK: TRACE_3_OUT_PARA("S-ACK s:%d len:%d nr:%d", sapi, len, nr); break;
      case I_FRAME_RNR: TRACE_3_OUT_PARA("S-RNR s:%d len:%d nr:%d", sapi, len, nr); break;
      case I_FRAME_SACK:TRACE_3_OUT_PARA("S-SACK s:%d len:%d nr:%d", sapi, len, nr);break;
      default:          TRACE_2_OUT_PARA("S ??? s:%d len:%d", sapi, len);           break;
    }
  }
  else
  {
    TRACE_2_OUT_PARA("??? s:%d len:%d", sapi, len);
  }
} /* tx_trace_outprim_para() */

#endif
/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : tx_label_s_data
+------------------------------------------------------------------------------
| Description : Describes label S_DATA
|
| Parameters  : rx_service    - indicates the service to receive a READY_IND
|                               signal, or NO_SERVICE
|               grlc_data_req  - a valid pointer to a GRLC_DATA_REQ primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_label_s_data (T_SERVICE rx_service, 
                             T_GRLC_DATA_REQ *grlc_data_req)
{ 
  UBYTE sapi = grlc_data_req->sapi;

  TRACE_FUNCTION( "tx_label_s_data" );

#ifdef TRACE_EVE
  tx_trace_outprim_para (sapi, &grlc_data_req->sdu);
#endif  

  PSEND (hCommGRLC, grlc_data_req);

  SET_STATE (TX, TX_TLLI_ASSIGNED_NOT_READY);

  /*
   * Recursion possible. Therefore send ready indication after all!
   */
  if (rx_service EQ SERVICE_UITX)
  {
    SWITCH_LLC (sapi);
    sig_tx_uitx_ready_ind();
  }
  else if (rx_service EQ SERVICE_ITX)
  {
    SWITCH_LLC (sapi);
    sig_tx_itx_ready_ind();
  }

  return;
} /* tx_label_s_data() */


/*
+------------------------------------------------------------------------------
| Function    : tx_label_s_unitdata
+------------------------------------------------------------------------------
| Description : Describes label S_UNITDATA
|
| Parameters  : rx_service        - indicates the service to receive a 
|                                   READY_IND signal, or NO_SERVICE
|               grlc_unitdata_req  - a valid pointer to a GRLC_UNITDATA_REQ 
|                                   primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_label_s_unitdata (T_SERVICE rx_service, 
                                 T_GRLC_UNITDATA_REQ *grlc_unitdata_req)
{ 
  UBYTE sapi = grlc_unitdata_req->sapi;

  TRACE_FUNCTION( "tx_label_s_unitdata" );

#ifdef TRACE_EVE
  tx_trace_outprim_para (sapi, &grlc_unitdata_req->sdu);
#endif  

  PSEND (hCommGRLC, grlc_unitdata_req);

  SET_STATE (TX, TX_TLLI_ASSIGNED_NOT_READY);

  /*
   * Handle ready indication at last! Recursion possible!
   */
  if (rx_service EQ SERVICE_UITX)
  {
    SWITCH_LLC (sapi);
    sig_tx_uitx_ready_ind();
  }

  return;
} /* tx_label_s_unitdata() */


/*
+------------------------------------------------------------------------------
| Function    : tx_label_s_removed
+------------------------------------------------------------------------------
| Description : Describes label S_REMOVED
|
| Parameters  : rx_service        - indicates the service to receive a 
|                                   READY_IND signal, or NO_SERVICE
|               sapi              - sapi of removed entry
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_label_s_removed (T_SERVICE rx_service, UBYTE sapi)
{ 
  TRACE_FUNCTION( "tx_label_s_removed" );

  if (rx_service EQ SERVICE_UITX)
  {
    SWITCH_LLC (sapi);
    sig_tx_uitx_ready_ind();
  }
  else if (rx_service EQ SERVICE_ITX)
  {
    SWITCH_LLC (sapi);
    sig_tx_itx_ready_ind();
  }

  return;
} /* tx_label_s_removed() */
