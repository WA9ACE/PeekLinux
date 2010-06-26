/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  RLP_KERF
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
|  Purpose :  This Modul defines the procedures and functions for
|             the component Radio Link Protocol of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef RLP_KERF_C
#define RLP_KERF_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h"
#include "pconst.cdg"
#include "custom.h"
#include "gsm.h"
#include "cnf_rlp.h"
#include "mon_rlp.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "ccdapi.h"
#include "rlp.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_initTRCVS           |
+--------------------------------------------------------------------+

  PURPOSE : Initialise the TCRVS table

*/

LOCAL void ker_initTRCVS(T_RCVS_ENTRY *rcvsTab)
{
  T_FRAME_NUM lookUp;

  TRACE_FUNCTION ("ker_initTRCVS()");

  for (lookUp = 0; lookUp < MAX_SREJ_COUNT; lookUp++)
  {
    rcvsTab[lookUp].isFree       = TRUE;
    rcvsTab[lookUp].slot         = INVALID_IDX;
    rcvsTab[lookUp].trcvs_running = FALSE;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : RLP_KERF            |
| STATE   : code                       ROUTINE : ker_init            |
+--------------------------------------------------------------------+

  PURPOSE : initialise the rlp data for the kernel process

*/

GLOBAL void ker_init(void)
{
  TRACE_FUNCTION ("ker_init()");

  rlp_data->tul_rcv_running = FALSE;

  ker_initTRCVS(rlp_data->ker.rcvsTab);

  rlp_data->ker.DM_State   = IS_IDLE;
  rlp_data->ker.DM_FBit    = 0;

  rbm_reset(&rlp_data->rbm);

  INIT_STATE(KER, RLP_ADM_AND_DETACHED);
  INIT_STATE(KERXID_C, ISW_IDLE);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : RLP_KERF            |
| STATE   : code                       ROUTINE : ker_get_xid_data    |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void ker_get_xid_data
            (
              T_RLP_FRAMEDATA *raw_data,
              UBYTE           index,
              T_RLP_XID_IND   *xid_data,
              BOOL             ms_is_initiator,
              ULONG           *used_flags
            )
{
  UBYTE p_type;
  UBYTE length;
  UBYTE comp_request;

#ifdef _SIMULATION_
  USHORT rcvd;
  char trc_buf[80];
  UBYTE i, idx;

  strcpy (trc_buf, "get XID ");
  idx = strlen(trc_buf);
#endif

  TRACE_FUNCTION ("ker_get_xid_data()");

  rlp_data->ker.Rlp_Vers  = DEF_RLP_VERS;
  rlp_data->ker.K_ms_iwf  = DEF_K_MS_IWF;
  rlp_data->ker.K_iwf_ms  = DEF_K_IWF_MS;
  rlp_data->ker.T1        = rlp_data->ker.Def_T1;
  rlp_data->ker.T2        = DEF_T2;
  rlp_data->ker.N2        = DEF_N2;
  rlp_data->ker.Pt        = DEF_PT;
  rlp_data->ker.P0        = DEF_P0;
  rlp_data->ker.P1        = DEF_P1;
  rlp_data->ker.P2        = DEF_P2;

  *used_flags = 0L;

  while (index < rlp_data->ker.FrameSize - HEADER_LEN - TRAILER_LEN)
  {
    length = raw_data[index] % 16;
    p_type = (raw_data[index] - length) / 16;

    if (p_type EQ XID_T_NULL)
      break;

#ifdef _SIMULATION_
    for (i = 0; i < length+1; i++)
    {
      /*lint -e661 : Possible access of out-of-bounds pointer (1 beyond end of data) by operator 'unary * */
      HEX_BYTE (raw_data[index+i], &trc_buf[idx]);
      /*lint +e661 */
      idx+=2;
    }
#endif

    switch (p_type)
    {

      case XID_T_T1:
        rlp_data->ker.T1 = TIME_EXT2INT(raw_data[index+1]);
#ifdef _SIMULATION_
        rcvd = rlp_data->ker.T1;
#endif
        if (rlp_data->ker.T1 < rlp_data->ker.Orig_T1)
          rlp_data->ker.T1 = rlp_data->ker.Orig_T1;
        *used_flags |= UF_SET_T1;

#ifdef _SIMULATION_
        TRACE_EVENT_P2("RCVD XID T1   : %d -> %d", rcvd, rlp_data->ker.T1);
#endif
        break;

      case XID_T_RLP_VERS:
        rlp_data->ker.Rlp_Vers = raw_data[index+1];
#ifdef _SIMULATION_
        rcvd = rlp_data->ker.Rlp_Vers;
#endif
        if (rlp_data->ker.Rlp_Vers > ORIG_RLP_VERS)
          rlp_data->ker.Rlp_Vers = ORIG_RLP_VERS;

        *used_flags |= UF_SET_RLP_VERS;
#ifdef _SIMULATION_
        TRACE_EVENT_P2("RCVD XID RLPV : %d -> %d", rcvd, rlp_data->ker.Rlp_Vers);
#endif
        break;

      case XID_T_T2:
        rlp_data->ker.T2 = TIME_EXT2INT(raw_data[index+1]);
#ifdef _SIMULATION_
        rcvd = rlp_data->ker.T2;
#endif
        if (rlp_data->ker.T2 < rlp_data->ker.Orig_T2)
          rlp_data->ker.T2 = rlp_data->ker.Orig_T2;

        *used_flags |= UF_SET_T2;
#ifdef _SIMULATION_
        TRACE_EVENT_P2("RCVD XID T2   : %d -> %d", rcvd, rlp_data->ker.T2);
#endif
        break;

      case XID_T_K_MS_IWF:
        rlp_data->ker.K_ms_iwf = raw_data[index+1];
#ifdef _SIMULATION_
       rcvd = rlp_data->ker.K_ms_iwf;
#endif
        if (rlp_data->ker.K_ms_iwf > rlp_data->ker.Orig_K_ms_iwf)
          rlp_data->ker.K_ms_iwf = rlp_data->ker.Orig_K_ms_iwf;

        if (rlp_data->ker.Connected)
          sbm_set_wind_size (rlp_data->ker.K_ms_iwf);

        *used_flags |= UF_SET_K_MS_IWF;
#ifdef _SIMULATION_
        TRACE_EVENT_P2("RCVD XID KMI  : %d -> %d", rcvd, rlp_data->ker.K_ms_iwf);
#endif
        break;

      case XID_T_N2:
        rlp_data->ker.N2 = raw_data[index+1];
#ifdef _SIMULATION_
        rcvd = rlp_data->ker.N2;
#endif
        if (rlp_data->ker.N2 < rlp_data->ker.Orig_N2)
          rlp_data->ker.N2 = rlp_data->ker.Orig_N2;

        sbm_set_retrans (rlp_data->ker.N2);
        *used_flags |= UF_SET_N2;
#ifdef _SIMULATION_
        TRACE_EVENT_P2("RCVD XID N2   : %d -> %d", rcvd, rlp_data->ker.N2);
#endif
        break;

      case XID_T_K_IWF_MS:
        rlp_data->ker.K_iwf_ms = raw_data[index+1];
#ifdef _SIMULATION_
        rcvd = rlp_data->ker.K_iwf_ms;
#endif
        if (rlp_data->ker.K_iwf_ms > rlp_data->ker.Orig_K_iwf_ms)
          rlp_data->ker.K_iwf_ms = rlp_data->ker.Orig_K_iwf_ms;

        if (rlp_data->ker.Connected)
          if (rbm_set_wind_size (rlp_data->ker.K_iwf_ms))
          {
            ker_reset_all_t_rcvs ();
            srm_reset();
          }

        *used_flags |= UF_SET_K_IWF_MS;
#ifdef _SIMULATION_
        TRACE_EVENT_P2("RCVD XID KIM  : %d -> %d", rcvd, rlp_data->ker.K_iwf_ms);
#endif
        break;

      case XID_T_COMPR:
        comp_request = raw_data[index+1] % 16;
        rlp_data->ker.Pt = (raw_data[index+1] - comp_request) / 16;
#ifdef _SIMULATION_
        TRACE_EVENT_P1("RCVD XID PT   : %d", rlp_data->ker.Pt);
#endif

        switch (comp_request)
        {
        case 0:
          rlp_data->ker.P0 = RLP_COMP_DIR_NONE;
          break;

        case 1:
          if (ms_is_initiator)
          {
            if (rlp_data->ker.Orig_P0 EQ RLP_COMP_DIR_RECEIVE
            OR  rlp_data->ker.Orig_P0 EQ RLP_COMP_DIR_NONE)
              rlp_data->ker.P0 = RLP_COMP_DIR_NONE;
            else
              rlp_data->ker.P0 = RLP_COMP_DIR_TRANSMIT;
          }
          else
          {
            if (rlp_data->ker.Orig_P0 EQ RLP_COMP_DIR_TRANSMIT
            OR  rlp_data->ker.Orig_P0 EQ RLP_COMP_DIR_NONE)
              rlp_data->ker.P0 = RLP_COMP_DIR_NONE;
            else
              rlp_data->ker.P0 = RLP_COMP_DIR_RECEIVE;
          }
          break;

        case 2:
          if (ms_is_initiator)
          {
            if (rlp_data->ker.Orig_P0 EQ RLP_COMP_DIR_TRANSMIT
            OR  rlp_data->ker.Orig_P0 EQ RLP_COMP_DIR_NONE)
              rlp_data->ker.P0 = RLP_COMP_DIR_NONE;
            else
              rlp_data->ker.P0 = RLP_COMP_DIR_RECEIVE;
          }
          else
          {
            if (rlp_data->ker.Orig_P0 EQ RLP_COMP_DIR_RECEIVE
            OR  rlp_data->ker.Orig_P0 EQ RLP_COMP_DIR_NONE)
              rlp_data->ker.P0 = RLP_COMP_DIR_NONE;
            else
              rlp_data->ker.P0 = RLP_COMP_DIR_TRANSMIT;
          }
          break;

        case 3:
          rlp_data->ker.P0 = rlp_data->ker.Orig_P0;
          break;

        }

#ifdef _SIMULATION_
        TRACE_EVENT_P2("RCVD XID P0: %d -> %d", comp_request, rlp_data->ker.P0);
#endif
        rlp_data->ker.P1 = (raw_data[index+3] * 256) +  raw_data[index+2];
#ifdef _SIMULATION_
        rcvd = rlp_data->ker.P1;
#endif
        if (rlp_data->ker.P1 > rlp_data->ker.Orig_P1)
          rlp_data->ker.P1 = rlp_data->ker.Orig_P1;

#ifdef _SIMULATION_
        TRACE_EVENT_P2("RCVD XID P1: %d -> %d", rcvd, rlp_data->ker.P1);
#endif

        rlp_data->ker.P2 = raw_data[index+4];
#ifdef _SIMULATION_
        rcvd = rlp_data->ker.P2;
#endif
        if (rlp_data->ker.P2 > rlp_data->ker.Orig_P2)
          rlp_data->ker.P2 = rlp_data->ker.Orig_P2;

        *used_flags |= UF_SET_COMPR;
#ifdef _SIMULATION_
        TRACE_EVENT_P2("RCVD XID P2   : %d -> %d", rcvd, rlp_data->ker.P2);
#endif
        break;
    }
    index += (length+1);
  }

  xid_data->rlp_vers = rlp_data->ker.Rlp_Vers;
  xid_data->k_ms_iwf = rlp_data->ker.K_ms_iwf;
  xid_data->k_iwf_ms = rlp_data->ker.K_iwf_ms;
  
  xid_data->t1 = TIME_INT2EXT (rlp_data->ker.T1);
  xid_data->t2 = TIME_INT2EXT (rlp_data->ker.T2);

  xid_data->n2 = rlp_data->ker.N2;
  xid_data->pt = rlp_data->ker.Pt;
  xid_data->p0 = rlp_data->ker.P0;
  xid_data->p1 = rlp_data->ker.P1;
  xid_data->p2 = rlp_data->ker.P2;

#ifdef _SIMULATION_
  trc_buf[idx] = '\0';
  TRACE_EVENT (trc_buf);
#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : RLP_KERF            |
| STATE   : code                       ROUTINE : ker_put_xid_data    |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void ker_put_xid_data
            (
              T_RLP_FRAMEPTR   l_Data,          /* buffer for data  */
              UBYTE            index,           /* start index in buffer */
              ULONG            l_uf,            /* valid parameters */
              BOOL             ms_is_initiator, /* MS is sending XID*/
              UBYTE            l_rlp_version,   /* negotiated vers. */
              T_FRAME_NUM      l_k_iwf_ms,      /* winSize ms->iwf  */
              T_FRAME_NUM      l_k_ms_iwf,      /* winSize iwf->ms  */
              USHORT           l_t1,            /* ack timer value  */
              UBYTE            l_n2,            /* num restransmiss */
              USHORT           l_t2,            /* reply delay      */
              UBYTE            l_pt,            /* type data compr. */
              UBYTE            l_p0,            /* v42bis comp. req */
              USHORT           l_p1,            /* num possible code*/
              UBYTE            l_p2             /* max encod. strlen*/
            )
{
  register ULONG i;

  TRACE_FUNCTION ("ker_put_xid_data()");

  i = index;

  if (((l_uf & UF_SET_RLP_VERS) EQ 0) OR (l_rlp_version < 1))
  {
    l_uf &= ~UF_SET_COMPR;
  }

  if (l_uf & UF_SET_RLP_VERS)
  {
    l_Data[i]   = (XID_T_RLP_VERS * 16) + XID_L_RLP_VERS;
    l_Data[i+1] = l_rlp_version;
#ifdef _SIMULATION_
    TRACE_EVENT_P1("SEND XID RLPV : %d", l_rlp_version);
#endif
    i += (XID_L_RLP_VERS + 1);
  }

  if (l_uf & UF_SET_K_IWF_MS)
  {
    l_Data[i]   = (XID_T_K_IWF_MS * 16) + XID_L_K_IWF_MS;
    l_Data[i+1] = (UBYTE) l_k_iwf_ms;
#ifdef _SIMULATION_
    TRACE_EVENT_P1("SEND XID KIM  : %d", l_k_iwf_ms);
#endif
    i += (XID_L_K_IWF_MS + 1);
  }

  if (l_uf & UF_SET_K_MS_IWF)
  {
    l_Data[i]   = (XID_T_K_MS_IWF * 16) + XID_L_K_MS_IWF;
    l_Data[i+1] = (UBYTE) l_k_ms_iwf;
#ifdef _SIMULATION_
    TRACE_EVENT_P1("SEND XID KMI  : %d", l_k_ms_iwf);
#endif
    i += (XID_L_K_MS_IWF + 1);
  }

  if (l_uf & UF_SET_T1)
  {
    l_Data[i]   = (XID_T_T1 * 16) + XID_L_T1;
    l_Data[i+1] = TIME_INT2EXT (l_t1);
#ifdef _SIMULATION_
    TRACE_EVENT_P1("SEND XID T1   : %d", l_t1);
#endif
    i += (XID_L_T1 + 1);
  }

  if (l_uf & UF_SET_N2)
  {
    l_Data[i]   = (XID_T_N2 * 16) + XID_L_N2;
    l_Data[i+1] = l_n2;
#ifdef _SIMULATION_
    TRACE_EVENT_P1("SEND XID N2   : %d", l_n2);
#endif
    i += (XID_L_N2 + 1);
  }

  if (l_uf & UF_SET_T2)
  {
    l_Data[i]   = (XID_T_T2 * 16) + XID_L_T2;
    l_Data[i+1] = TIME_INT2EXT (l_t2);
#ifdef _SIMULATION_
    TRACE_EVENT_P1("SEND XID T2   : %d", l_t2);
#endif
    i += (XID_L_T2 + 1);
  }

  if ( (l_uf & UF_SET_COMPR) AND !rlp_data->ker.Connected)
  {
    l_Data[i]   = (XID_T_COMPR * 16) + XID_L_COMPR;

    switch (l_p0)
    {
      case RLP_COMP_DIR_NONE:
        l_Data[i+1] = l_pt * 16;
        break;

      case RLP_COMP_DIR_TRANSMIT:
        if (ms_is_initiator)
          l_Data[i+1] = (l_pt*16) + 1;
        else
          l_Data[i+1] = (l_pt*16) + 2;
        break;

      case RLP_COMP_DIR_RECEIVE:
        if (ms_is_initiator)
          l_Data[i+1] = (l_pt*16) + 2;
        else
          l_Data[i+1] = (l_pt*16) + 1;
        break;

      case RLP_COMP_DIR_BOTH:
        l_Data[i+1] = (l_pt*16) + 3;
        break;
    }
    l_Data[i+2] = (l_p1 % 256);
    l_Data[i+3] = (l_p1 / 256);
    l_Data[i+4] = l_p2;
#ifdef _SIMULATION_
    TRACE_EVENT_P1("SEND XID PT: %d", l_pt);
    TRACE_EVENT_P2("SEND XID P0: %d -> %d", l_p0, l_Data[i+1] % 4);
    TRACE_EVENT_P1("SEND XID P1: %d", l_p1);
    TRACE_EVENT_P1("SEND XID P2: %d", l_p2);
#endif
    i += (XID_L_COMPR + 1);
  }

#ifdef _SIMULATION_
  {
  char trc_buf[80];
  U8 j, k;

  strcpy (trc_buf, "put XID ");
  k = strlen(trc_buf);
  for (j = index; j < i; j++)
  {
    /*lint -e661 : Possible access of out-of-bounds pointer (1 beyond end of data) by operator 'unary * */
    HEX_BYTE (l_Data[j], &trc_buf[k]);
    /*lint +e661 */
    k += 2;
  }
  trc_buf[k] = '\0';
  TRACE_EVENT (trc_buf);
  }
#endif

}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_check_low_water     |
+--------------------------------------------------------------------+

  PURPOSE :

*/

LOCAL void ker_check_low_water(void)
{
  TRACE_FUNCTION ("ker_check_low_water()");

  if (sbm_space_in_buf () >= rlp_data->ker.FramesPerPrim)
  {
    if (rlp_data->ker.UL_Snd_State EQ IW_IDLE)
    {
      PALLOC (rlp_ready_ind, RLP_READY_IND);

      PSENDX (L2R, rlp_ready_ind);
      rlp_data->ker.UL_Snd_State = IW_WAIT;
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_get_send_frame      |
+--------------------------------------------------------------------+

  PURPOSE :

*/

LOCAL void ker_get_send_frame
            (
              T_FRAME_DESC *frameDesc,
              T_FRAME_NUM  *frameNo,
              BOOL         *frameFound
            )
{
  BOOL framesCouldBeSent;

  TRACE_FUNCTION ("ker_get_send_frame()");

  if (rlp_data->ker.RRReady)
  {
    sbm_get_frame
    (
      frameDesc,
      frameNo,
      &framesCouldBeSent,
      frameFound
    );

    if (framesCouldBeSent)
      ker_check_low_water ();
  }
  else
  {
    /*
     * remote receiver can not accept data
     */
    *frameFound = FALSE;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_startTRCVS          |
+--------------------------------------------------------------------+

  PURPOSE : Start the timer TRCVS for a slot

*/

LOCAL void ker_startTRCVS(T_FRAME_NUM slot)
{
  T_FRAME_NUM lookUp;

  TRACE_FUNCTION ("ker_startTRCVS()");

  for (lookUp = 0; lookUp < MAX_SREJ_COUNT; lookUp++)
  {
    if (rlp_data->ker.rcvsTab[lookUp].isFree)
    {
      rlp_data->ker.rcvsTab[lookUp].slot = slot;
      TIMERSTART(lookUp, rlp_data->ker.T1);
      rlp_data->ker.rcvsTab[lookUp].trcvs_running = TRUE;
      rlp_data->ker.rcvsTab[lookUp].isFree = FALSE;
      return;
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_send_srej_sreji_cmd |
+--------------------------------------------------------------------+

  PURPOSE :

*/

LOCAL void ker_send_srej_sreji_cmd(T_FRAME_NUM slot)
{
  T_FRAME_DESC frameDesc;
  T_FRAME_NUM  frame;
  BOOL         frameFound;


  rlp_data->ker.DTX_SF       = DTX_N;

  TRACE_FUNCTION ("ker_send_srej_sreji_cmd()");

  if (rlp_data->ker.Poll_xchg NEQ IW_WAIT
  AND rlp_data->ker.Poll_State EQ ISW_SEND)
  {
    sig_ker_snd_srej_req (1, 1, slot);
    rbm_set_rslot_wait (slot);

    ker_startTRCVS (slot);

    srm_count (slot);

    rlp_data->ker.SREJ_Count++;
    rlp_data->ker.Poll_Count++;
    rlp_data->ker.Poll_State = ISW_WAIT;
    rlp_data->ker.Poll_xchg  = IW_WAIT;

    TIMERSTART (TT_HANDLE, rlp_data->ker.T1);
  }
  else
  {
    ker_get_send_frame (&frameDesc, &frame, &frameFound);

    if (frameFound)
    {
      sig_ker_snd_srej_i_req (1, 0, slot, frame, &frameDesc);

      rbm_set_rslot_wait (slot);

      ker_startTRCVS (slot);

      srm_count (slot);

      rlp_data->ker.SREJ_Count++;

      TIMERSTART (TT_HANDLE, rlp_data->ker.T1);
    }
    else
    {
      sig_ker_snd_srej_req (1, 0, slot);

      rbm_set_rslot_wait (slot);

      ker_startTRCVS (slot);

      srm_count (slot);

      rlp_data->ker.SREJ_Count++;
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_send_rej_reji_cmd   |
+--------------------------------------------------------------------+

  PURPOSE :

*/

LOCAL void ker_send_rej_reji_cmd
            (
              T_FRAME_NUM      vr
            )
{
  T_FRAME_DESC frameDesc;
  T_FRAME_NUM  frame;
  BOOL         frameFound;

  TRACE_FUNCTION ("ker_send_rej_reji_cmd()");

  rlp_data->ker.DTX_SF       = DTX_N;

  if (rlp_data->ker.Poll_xchg NEQ IW_WAIT AND rlp_data->ker.Poll_State EQ ISW_SEND)
  {
    sig_ker_snd_rej_req (1, 1, vr);

    rlp_data->ker.Ackn_State = IS_IDLE;
    rlp_data->ker.REJ_State  = ISW_WAIT;

    TIMERSTART
    (
      TRCVR_HANDLE,
      rlp_data->ker.T1
    );
    rlp_data->ker.Poll_Count++;
    rlp_data->ker.Poll_State = ISW_WAIT;
    rlp_data->ker.Poll_xchg  = IW_WAIT;

    TIMERSTART (TT_HANDLE, rlp_data->ker.T1);
  }
  else
  {
    ker_get_send_frame (&frameDesc, &frame, &frameFound);

    if (frameFound)
    {
      sig_ker_snd_rej_i_req (1, 0, vr, frame, &frameDesc);

      TIMERSTART (TT_HANDLE, rlp_data->ker.T1);
    }
    else
      sig_ker_snd_rej_req (1, 0, vr);

    TIMERSTART(TRCVR_HANDLE, rlp_data->ker.T1);
    rlp_data->ker.Ackn_State = IS_IDLE;
    rlp_data->ker.REJ_State  = ISW_WAIT;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_send_rnr_rnri_cmd   |
+--------------------------------------------------------------------+

  PURPOSE :

*/

LOCAL void ker_send_rnr_rnri_cmd
            (
              T_FRAME_NUM      vr
            )
{
  T_FRAME_DESC frameDesc;
  T_FRAME_NUM  frame;
  BOOL         frameFound;

  TRACE_FUNCTION ("ker_send_rnr_rnri_cmd()");

  if (rlp_data->ker.Poll_xchg NEQ IW_WAIT
  AND rlp_data->ker.Poll_State EQ ISW_SEND)
  {
    sig_ker_snd_rnr_req (1, 1, vr, 0);

    rlp_data->ker.Ackn_State = IS_IDLE;
    rlp_data->ker.Poll_Count++;
    rlp_data->ker.Poll_State = ISW_WAIT;
    rlp_data->ker.Poll_xchg  = IW_WAIT;

    rlp_data->ker.DTX_SF       = DTX_N;

    TIMERSTART (TT_HANDLE, rlp_data->ker.T1);
  }
  else
  {
    ker_get_send_frame (&frameDesc, &frame, &frameFound);

    if (frameFound)
    {
      sig_ker_snd_rnr_i_req (1, 0, vr, frame, &frameDesc);

      rlp_data->ker.Ackn_State = IS_IDLE;

      rlp_data->ker.DTX_SF       = DTX_N;

      TIMERSTART (TT_HANDLE, rlp_data->ker.T1);
    }
    else
    {
      if (rlp_data->ker.Ackn_State EQ IS_IDLE
      AND rlp_data->ker.DTX_SF EQ DTX_RNR
      AND rlp_data->ker.DTX_VR EQ vr)
      {
        sig_ker_snd_rnr_req (1, 0, vr, 1);
      }
      else
      {
        sig_ker_snd_rnr_req (1, 0, vr, 0);

        rlp_data->ker.Ackn_State = IS_IDLE;
        rlp_data->ker.DTX_SF     = DTX_RNR;
        rlp_data->ker.DTX_VR     = vr;
      }
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_send_rr_rri_cmd     |
+--------------------------------------------------------------------+

  PURPOSE :

*/

LOCAL void ker_send_rr_rri_cmd
            (
              T_FRAME_NUM      vr
            )
{
  T_FRAME_DESC frameDesc;
  T_FRAME_NUM  frame;
  BOOL         frameFound;

  TRACE_FUNCTION ("ker_send_rr_rri_cmd()");

  if (rlp_data->ker.Poll_xchg NEQ IW_WAIT AND rlp_data->ker.Poll_State EQ ISW_SEND)
  {

#ifdef ENABLE_DTX
    sig_ker_snd_rr_req (1, 1, vr, 0);
#else
    sig_ker_snd_rr_req (1, 1, vr);
#endif

    rlp_data->ker.Ackn_State = IS_IDLE;
    rlp_data->ker.Poll_Count++;
    rlp_data->ker.Poll_State = ISW_WAIT;
    rlp_data->ker.Poll_xchg  = IW_WAIT;

    rlp_data->ker.DTX_SF       = DTX_N;

    TIMERSTART (TT_HANDLE, rlp_data->ker.T1);
  }
  else
  {
    ker_get_send_frame (&frameDesc, &frame, &frameFound);

    if (frameFound)
    {
      sig_ker_snd_rr_i_req (1, 0, vr, frame, &frameDesc);

      rlp_data->ker.Ackn_State = IS_IDLE;

      TIMERSTART (TT_HANDLE, rlp_data->ker.T1);
      rlp_data->ker.DTX_SF       = DTX_N;

    }
    else
    {
      if (rlp_data->ker.Ackn_State EQ IS_IDLE
      AND rlp_data->ker.DTX_SF EQ DTX_RR
      AND rlp_data->ker.DTX_VR EQ vr)
      {
#ifdef ENABLE_DTX
        sig_ker_snd_rr_req (1, 0, vr, 1);
#else
        sig_ker_snd_rr_req (1, 0, vr);
#endif
      }
      else
      {
#ifdef ENABLE_DTX
        sig_ker_snd_rr_req (1, 0, vr, 0);
#else
        sig_ker_snd_rr_req (1, 0, vr);
#endif
        rlp_data->ker.Ackn_State = IS_IDLE;
        rlp_data->ker.DTX_SF     = DTX_RR;
        rlp_data->ker.DTX_VR     = vr;
      }
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_send_rnr_rnri_resp  |
+--------------------------------------------------------------------+

  PURPOSE :

*/

LOCAL void ker_send_rnr_rnri_resp
            (
              T_FRAME_NUM      vr
            )
{

  TRACE_FUNCTION ("ker_send_rnr_rnri_resp()");

  sig_ker_snd_rnr_req (0, 1, vr, 0);

  rlp_data->ker.Ackn_State = IS_IDLE;
  rlp_data->ker.Ackn_FBit  = 0;
  rlp_data->ker.DTX_SF     = DTX_N;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_send_rr_rri_resp    |
+--------------------------------------------------------------------+

  PURPOSE :

*/

LOCAL void ker_send_rr_rri_resp(T_FRAME_NUM vr)
{

  TRACE_FUNCTION ("ker_send_rr_rri_resp()");

#ifdef ENABLE_DTX
  sig_ker_snd_rr_req (0, 1, vr, 0);
#else
  sig_ker_snd_rr_req (0, 1, vr);
#endif

  rlp_data->ker.Ackn_State = IS_IDLE;
  rlp_data->ker.Ackn_FBit  = 0;
  rlp_data->ker.DTX_SF     = DTX_N;

}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_copy_frame_to_sdu   |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void ker_copy_frame_to_sdu
            (
              T_RLP_FRAMEPTR  frame,
              T_sdu          *sdu
            )
{
  TRACE_FUNCTION ("ker_copy_frame_to_sdu()");

  memcpy (sdu->buf+(sdu->o_buf>>3), frame, rlp_data->ker.FrameSize);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_copy_sdu_to_frame   |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void ker_copy_sdu_to_frame(T_sdu *sdu, T_RLP_FRAMEPTR frame, USHORT n)
{
  TRACE_FUNCTION ("ker_copy_sdu_to_frame()");

  memcpy(frame, sdu->buf + (sdu->o_buf>>3) + (n * rlp_data->ker.FrameSize),
                rlp_data->ker.FrameSize);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_get_frame_desc      |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void ker_get_frame_desc
            (
              T_RLP_FRAMEPTR  frame,
              T_FRAME_DESC   *frameDesc
            )
{
  TRACE_FUNCTION ("ker_get_frame_desc()");

  frameDesc->Adr[0] = frame;
  frameDesc->Len[0] = rlp_data->ker.FrameSize;
  frameDesc->Adr[1] = (UBYTE *) NULL;
  frameDesc->Len[1] = 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_handle_prim_send    |
+--------------------------------------------------------------------+

  PURPOSE :

*/

LOCAL void ker_handle_prim_send(void)
{
  TRACE_FUNCTION ("ker_handle_prim_send()");

  if (rlp_data->ker.UL_Rcv_State EQ IW_WAIT)
  {
    T_RLP_DATA_IND *rlp_data_ind;

    rbm_get_prim(&rlp_data_ind, &rlp_data->ker.LRReady, &rlp_data->ker.LRFull);

    PSENDX (L2R, rlp_data_ind);
    rlp_data->ker.UL_Rcv_State = IW_IDLE;
    TIMERSTOP (TUL_RCV_HANDLE);
    rlp_data->tul_rcv_running = FALSE;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_stopTRCVS           |
+--------------------------------------------------------------------+

  PURPOSE : search the rcvs entry for a given timer handle. The
            function stops the timer and returns the corresponding
            slot.

*/

LOCAL void ker_stopTRCVS
            (
              T_FRAME_NUM slot
            )
{
  T_FRAME_NUM lookUp;

  TRACE_FUNCTION ("ker_stopTRCVS()");

  for (lookUp = 0; lookUp < MAX_SREJ_COUNT; lookUp++)
  {
    if (!rlp_data->ker.rcvsTab[lookUp].isFree AND rlp_data->ker.rcvsTab[lookUp].slot EQ slot)
    {
      TIMERSTOP(lookUp);
      rlp_data->ker.rcvsTab[lookUp].trcvs_running = FALSE;
      rlp_data->ker.rcvsTab[lookUp].isFree = TRUE;
      return;
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_i_handler           |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL BOOL ker_i_handler(T_FRAME_NUM ns)
{
  TRACE_FUNCTION ("ker_i_handler()");

  if (rlp_data->ker.LRFull)
  {
    /*
     * ignore improper frame
     * cBit and fBit are not considered
     * at this stage
     */
    return TRUE;
  }
  else
  {
    BOOL        valid, expected, resetTimer, primToSend;
    T_FRAME_NUM count;

    rbm_ns_check (ns, &valid, &expected);

    /*
     * ns not within window?
     */
    if (!valid)
    {
      /*
       * ignore improper frame
       */
      return TRUE;
    }

    TIMERSTOP (TUL_RCV_HANDLE); /* optimisation trial... */
    /*
    TRACE_EVENT("TIMERSTART TUL_RCV ker_i_handler");
    */
    TIMERSTART(TUL_RCV_HANDLE, rlp_data->ker.T_ul);

    if (expected)
    {
      rbm_accept_current_frame ();
      rbm_set_rslot_rcvd (ns, &resetTimer);

      if (resetTimer)
      {
        ker_stopTRCVS (ns);
        rlp_data->ker.SREJ_Count--;
        srm_clear (ns);
      }

      rbm_buffer_all_in_sequence_frames
      (
        ns,
        &primToSend,
        &rlp_data->ker.LRReady,
        &rlp_data->ker.LRFull
      );

      if (primToSend)
        ker_handle_prim_send ();

      rlp_data->ker.REJ_State = ISW_IDLE;
      TIMERSTOP (TRCVR_HANDLE);
      rlp_data->ker.Ackn_State = IS_SEND;
      return FALSE;
    }
    else
    {
      if (rlp_data->ker.REJ_State NEQ ISW_IDLE)
        return TRUE;
       /*
        * ignore out of sequence frames
        */

      rbm_count_missing_i_frames (ns, &count);

      if (rlp_data->ker.SREJ_Count + (count-1) <=MAX_SREJ_COUNT)
      {
        rbm_move_current_frame (ns);
        rbm_set_rslot_rcvd (ns, &resetTimer);

        if (resetTimer)
        {
          ker_stopTRCVS (ns);
          rlp_data->ker.SREJ_Count--;
          srm_clear (ns);
        }

        rbm_mark_missing_i_frames_srej (ns);

        return FALSE;
      }
      else
      {
        rlp_data->ker.REJ_State = ISW_SEND;
        return FALSE;
      }
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_init_link_vars      |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void ker_init_link_vars
            (
              void
            )
{
  TRACE_FUNCTION ("ker_init_link_vars()");

  rlp_data->ker.UL_Rcv_State = IW_IDLE;
  rlp_data->ker.UL_Snd_State = IW_IDLE;
  rlp_data->ker.Ackn_State   = IS_IDLE;
  rlp_data->ker.Poll_State   = ISW_IDLE;
  rlp_data->ker.Poll_Count   = 0;
  rlp_data->ker.Poll_xchg    = IW_IDLE;
  rlp_data->ker.REJ_State    = ISW_IDLE;
  rlp_data->ker.SABM_State   = ISW_IDLE;
  rlp_data->ker.DISC_State   = ISW_IDLE;
  rlp_data->ker.RRReady      = TRUE;
  rlp_data->ker.LRReady      = TRUE;
  rlp_data->ker.LRFull       = FALSE;
  rlp_data->ker.SREJ_Count   = 0;
  rlp_data->ker.Connected    = TRUE;
  rlp_data->ker.DTX_SF       = DTX_N;

  sbm_init
  (
    rlp_data->ker.K_ms_iwf,
    rlp_data->ker.FrameSize,
    rlp_data->ker.N2
  );

  rbm_init
  (
    rlp_data->ker.K_iwf_ms,
    rlp_data->ker.FrameSize,
    rlp_data->ker.FramesPerPrim
  );

  srm_init ();

}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_requestxid          |
+--------------------------------------------------------------------+

  PURPOSE : This function is invoked to request an XID command.
            Some variables are set, so that with next RA_READY_IND
            a XID command is sent.

*/

LOCAL void ker_requestxid(void)
{
  TRACE_FUNCTION ("ker_requestxid()");

  SET_STATE(KERXID_C, ISW_SEND);
  rlp_data->ker.XID_Count   = 0;
  rlp_data->ker.XID_C_PBit  = 1;

  ker_put_xid_data
  (
    rlp_data->ker.XID_C_Frame + HEADER_LEN,
    0,
    UF_ALL,
    TRUE,
    ORIG_RLP_VERS,
    rlp_data->ker.Orig_K_iwf_ms,
    rlp_data->ker.Orig_K_ms_iwf,
    rlp_data->ker.Orig_T1,
    rlp_data->ker.Orig_N2,
    rlp_data->ker.Orig_T2,
    rlp_data->ker.Orig_Pt,
    rlp_data->ker.Orig_P0,
    rlp_data->ker.Orig_P1,
    rlp_data->ker.Orig_P2
  );
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)            MODULE  : RLP_KERP             |
| STATE   : code                      ROUTINE : ker_init_xid_data    |
+--------------------------------------------------------------------+

  PURPOSE : initialize XID data from attach request

*/

GLOBAL void ker_init_xid_data
                  (
                    T_RLP_ATTACH_REQ *rlp_attach_req
                  )
{
  TRACE_FUNCTION ("ker_init_xid_data()");

  rlp_data->ker.Orig_K_ms_iwf = (U8)rlp_attach_req->k_ms_iwf;
  rlp_data->ker.K_ms_iwf      = DEF_K_MS_IWF;

  rlp_data->ker.Orig_K_iwf_ms = (U8)rlp_attach_req->k_iwf_ms;
  rlp_data->ker.K_iwf_ms      = DEF_K_IWF_MS;

  rlp_data->ker.Orig_T1       = TIME_EXT2INT (rlp_attach_req->t1);
  rlp_data->ker.T1            = rlp_data->ker.Def_T1;

  /*
   * >>>> According to GSM 11.10 V4.17.1 and V5.4.0 the default value must be used.
   * >>>> Later versions specify, that the value is used, which MS wants to negotiate.
   */
  /* use t1 immediately without negotiation, if it is greater than the default */
  /*
  if (rlp_data->ker.Orig_T1 > rlp_data->ker.T1)
  {
    rlp_data->ker.T1          = rlp_data->ker.Orig_T1;
  }
   */
  /*
  rlp_data->ker.Orig_T2       = TIME_EXT2INT (rlp_attach_req->t2);
  */
  rlp_data->ker.Orig_T2       = 200;    
  rlp_data->ker.T2            = DEF_T2;

  rlp_data->ker.Orig_N2       = rlp_attach_req->n2;
  rlp_data->ker.N2            = DEF_N2;

  rlp_data->ker.Orig_Pt       = rlp_attach_req->pt;
  rlp_data->ker.Pt            = DEF_PT;

  rlp_data->ker.Orig_P0       = rlp_attach_req->p0;
  rlp_data->ker.P0            = DEF_P0;

  rlp_data->ker.Orig_P1       = rlp_attach_req->p1;
  rlp_data->ker.P1            = DEF_P1;

  rlp_data->ker.Orig_P2       = rlp_attach_req->p2;
  rlp_data->ker.P2            = DEF_P2;

  rlp_data->ker.Rlp_Vers      = ORIG_RLP_VERS;


  if( !(rlp_data->ker.Orig_P0        EQ  RLP_COMP_DIR_NONE      AND
        rlp_data->ker.Orig_K_ms_iwf  EQ  DEF_K_MS_IWF           AND
        rlp_data->ker.Orig_K_iwf_ms  EQ  DEF_K_IWF_MS           AND
        rlp_data->ker.Orig_T1        EQ  rlp_data->ker.Def_T1   AND
        /*
        rlp_data->ker.Orig_T2        EQ  DEF_T2                 AND
        */
        rlp_data->ker.Orig_N2        EQ  DEF_N2                 AND
        rlp_data->ker.Orig_Pt        EQ  DEF_PT                 AND
        rlp_data->ker.Orig_P0        EQ  DEF_P0                 AND
        rlp_data->ker.Orig_P1        EQ  DEF_P1                 AND
        rlp_data->ker.Orig_P2        EQ  DEF_P2 ))
  {
    ker_requestxid ();
  }
  else
  {
    PALLOC (rlp_xid_ind, RLP_XID_IND);

    rlp_xid_ind->rlp_vers = rlp_data->ker.Rlp_Vers;
    rlp_xid_ind->k_ms_iwf = rlp_data->ker.K_ms_iwf;
    rlp_xid_ind->k_iwf_ms = rlp_data->ker.K_iwf_ms;
    rlp_xid_ind->t1 = TIME_INT2EXT (rlp_data->ker.T1);
    rlp_xid_ind->t2 = TIME_INT2EXT (rlp_data->ker.T2);
    rlp_xid_ind->n2 = rlp_data->ker.N2;
    rlp_xid_ind->pt = rlp_data->ker.Pt;
    rlp_xid_ind->p0 = rlp_data->ker.P0;
    rlp_xid_ind->p1 = rlp_data->ker.P1;
    rlp_xid_ind->p2 = rlp_data->ker.P2;

    PSENDX (L2R, rlp_xid_ind);
  }

}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_trace_rlp_frame     |
+--------------------------------------------------------------------+

  PURPOSE : traces the elements from the given RLP frame

*/

#ifdef TRACE_RLP_FRAME

GLOBAL void ker_trace_rlp_frame
            (
              T_RLP_FRAMEPTR  frame
            )
{
  T_BIT          cBit;
  T_BIT          pFBit;
  T_FRAME_NUM    nr;
  T_FRAME_NUM    ns;

  T_RLP_DEBUG *deb = &rlp_data->deb;

  TRACE_FUNCTION ("ker_trace_rlp_frame()");

  cBit  = (T_BIT) (frame[0] & 1);
  ns    = (T_FRAME_NUM) ((frame[0] >> 3) | ((frame[1] & 1)<<5));
  pFBit = (T_BIT) ((frame[1] & 2)>>1);

  
  deb->idx=0;

  deb->trc_buf[deb->idx++] = 'U';
  deb->trc_buf[deb->idx++] = ':';

  switch (ns)
  {
    case 62: /* S-Frame */
    {
      T_SF sFrame;

      nr = (T_FRAME_NUM) ((frame[1]>>2) & 0x3f);

      deb->trc_buf[deb->idx++] = 'S';
      deb->trc_buf[deb->idx++] = 'r';
      DEC_BYTE (nr, &deb->trc_buf[deb->idx]); deb->idx+=3;
      deb->trc_buf[deb->idx++] = ' ';

      sFrame = (T_SF) ((frame[0]>>1) & 0x03);

      /*
       * maybe optimize with a table!!
       */
      switch (sFrame)
      {
        case SF_RR:
          memcpy(&deb->trc_buf[deb->idx], "RR ",3);
          break;

        case SF_RNR:
          memcpy(&deb->trc_buf[deb->idx], "RNR",3);
          break;
        case SF_REJ:
          memcpy(&deb->trc_buf[deb->idx], "REJ",3);
          break;
        case SF_SREJ:
          memcpy(&deb->trc_buf[deb->idx], "SRJ",3);
          break;
        default:
          memcpy(&deb->trc_buf[deb->idx], "***",3);
          break;

      }
      deb->idx+=3;
      break;
    }

    case 63: /* U-Frame */
    {
      T_UF uFrame;
      uFrame = (T_UF) ((frame[1]>>2) & 0x1f);

      deb->trc_buf[deb->idx++] = 'U';
      deb->trc_buf[deb->idx++] = ' ';

      /*
       * maybe optimize with a table!!
       */
      switch (uFrame)
      {
        case UF_NULL:
          return;

        case UF_UI:
          memcpy(&deb->trc_buf[deb->idx], "UI  ",4);
          break;

        case UF_DM:
          memcpy(&deb->trc_buf[deb->idx], "DM  ",4);
          break;

        case UF_SABM:
          memcpy(&deb->trc_buf[deb->idx], "SABM",4);
          break;

        case UF_DISC:
          memcpy(&deb->trc_buf[deb->idx], "DISC",4);
          break;

        case UF_UA:
          memcpy(&deb->trc_buf[deb->idx], "UA  ",4);
          break;

        case UF_XID:
          memcpy(&deb->trc_buf[deb->idx], "XID ",4);
          break;

        case UF_TEST:
          memcpy(&deb->trc_buf[deb->idx], "TEST",4);
          break;

        case UF_REMAP:
          memcpy(&deb->trc_buf[deb->idx], "RMAP",4);
          break;

        default:
          memcpy(&deb->trc_buf[deb->idx], "****",4);
          break;
      }
      nr = 0;
      deb->idx += 4;
      break;
    }

    default: /* I+S-Frame */
    {
      T_SF sFrame;

      nr = (T_FRAME_NUM) ((frame[1]>>2) & 0x3f);

      sFrame = (T_SF) ((frame[0]>>1) & 0x03);

      deb->trc_buf[deb->idx++] = 'I';
      deb->trc_buf[deb->idx++] = 'r';
      DEC_BYTE (nr, &deb->trc_buf[deb->idx]); deb->idx+=3;
      deb->trc_buf[deb->idx++] = 's';
      DEC_BYTE (ns, &deb->trc_buf[deb->idx]); deb->idx+=3;
      deb->trc_buf[deb->idx++] = ' ';

      /*
       * maybe optimize with a table!!
       */
      switch (sFrame)
      {
        case SF_RR:
          memcpy(&deb->trc_buf[deb->idx], "RR ",3);
          break;

        case SF_RNR:
          memcpy(&deb->trc_buf[deb->idx], "RNR",3);
          break;
        case SF_REJ:
          memcpy(&deb->trc_buf[deb->idx], "REJ",3);
          break;
        case SF_SREJ:
          memcpy(&deb->trc_buf[deb->idx], "SRJ",3);
          break;
        default:
          memcpy(&deb->trc_buf[deb->idx], "***",3);
          break;
      }
      deb->idx += 3;
      break;
    }
  }
  deb->trc_buf[deb->idx++] = ' ';
  deb->trc_buf[deb->idx++] = ((cBit) ? 'C' : 'R');
  deb->trc_buf[deb->idx++] = ((cBit) ? 'P' : 'F');
  deb->trc_buf[deb->idx++] = ' ';
  HEX_BYTE (frame[1], &deb->trc_buf[deb->idx]); deb->idx+=2;
  HEX_BYTE (frame[0], &deb->trc_buf[deb->idx]); deb->idx+=2;
  deb->trc_buf[deb->idx++] = ' ';
  HEX_BYTE (frame[2], &deb->trc_buf[deb->idx]); deb->idx+=2;
  HEX_BYTE (frame[3], &deb->trc_buf[deb->idx]); deb->idx+=2;

  deb->trc_buf[deb->idx] = '\0';
  TRACE_EVENT (deb->trc_buf);
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_resetTRCVS          |
+--------------------------------------------------------------------+

  PURPOSE : Resets all running timer in the TCRVS table

*/

LOCAL void ker_resetTRCVS
            (
              void
            )
{
  T_FRAME_NUM lookUp;

  TRACE_FUNCTION ("ker_resetTRCVS()");

  for (lookUp = 0; lookUp < MAX_SREJ_COUNT; lookUp++)
  {
    if (!rlp_data->ker.rcvsTab[lookUp].isFree AND  rlp_data->ker.rcvsTab[lookUp].trcvs_running)
    {
      TIMERSTOP (lookUp);
      rlp_data->ker.rcvsTab[lookUp].trcvs_running = FALSE;
      rlp_data->ker.rcvsTab[lookUp].isFree       = TRUE;
      rlp_data->ker.rcvsTab[lookUp].slot         = INVALID_IDX;
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_reset_all_t_rcvs    |
+--------------------------------------------------------------------+

  PURPOSE : Resets the timer for each rcvs slot.

*/

GLOBAL void ker_reset_all_t_rcvs
            (
              void
            )
{
  TRACE_FUNCTION ("ker_reset_all_t_rcvs()");

  ker_resetTRCVS ();

  rlp_data->ker.SREJ_Count = 0;

  srm_reset ();
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_s_handler           |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void ker_s_handler
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_SF        sf,
              T_FRAME_NUM nr,
              BOOL        *retransError
            )
{
  BOOL allFramesAck = FALSE;

  TRACE_FUNCTION ("ker_s_handler()");

  *retransError = FALSE;

  if (cBit EQ 1 AND pFBit EQ 1)
  {
    rlp_data->ker.Ackn_State = IS_SEND;
    rlp_data->ker.Ackn_FBit  = 1;

    rbm_reset_all_r_states ();
    ker_reset_all_t_rcvs ();
    rlp_data->ker.REJ_State = ISW_IDLE;

    TIMERSTOP (TRCVR_HANDLE);
  }

  if (rlp_data->ker.Poll_State NEQ ISW_IDLE)
  {
    if (pFBit EQ 0 OR sf EQ SF_REJ OR sf EQ SF_SREJ )
      return;

    sbm_rej_from_n (nr, retransError);
    rlp_data->ker.Poll_State = ISW_IDLE;
    rlp_data->ker.Poll_xchg  = IW_IDLE;
    rlp_data->ker.Poll_Count = 0;

    if (*retransError)
    {
      ker_reset_all_t_rcvs ();
      rbm_reset_srej_slots ();

      TIMERSTOP (TRCVR_HANDLE);
      TIMERSTOP (TT_HANDLE);

      rlp_data->ker.DISC_State = ISW_SEND;
      rlp_data->ker.DISC_Count = 0;


      if (rlp_data->ker.Poll_xchg EQ IW_IDLE)
        rlp_data->ker.DISC_PBit = 1;
      else
        rlp_data->ker.DISC_PBit = 0;

      ker_deinit_link_vars();
      rlp_data->ker.DISC_Ind = TRUE;

      return;
    }
  }

  switch (sf)
  {
    case SF_RR:
      allFramesAck = sbm_ack_upto_n(nr);
      if (rlp_data->ker.RRReady EQ FALSE)
      {
        TRACE_EVENT ("Remote RR");
      }
      rlp_data->ker.RRReady = TRUE;
      break;

    case SF_RNR:
      allFramesAck = sbm_ack_upto_n(nr);
      if (rlp_data->ker.RRReady EQ TRUE)
      {
        TRACE_EVENT ("Remote RNR");
      }
      rlp_data->ker.RRReady = FALSE;
      break;

    case SF_REJ:
      allFramesAck = sbm_ack_upto_n(nr);
      sbm_rej_from_n (nr, retransError);

      if(*retransError)
      {
        ker_reset_all_t_rcvs ();
        rbm_reset_srej_slots ();

        TIMERSTOP (TRCVR_HANDLE);
        TIMERSTOP (TT_HANDLE);

        rlp_data->ker.DISC_State = ISW_SEND;
        rlp_data->ker.DISC_Count = 0;

        if (rlp_data->ker.Poll_xchg EQ IW_IDLE)
          rlp_data->ker.DISC_PBit = 1;
        else
          rlp_data->ker.DISC_PBit = 0;

        ker_deinit_link_vars();
        rlp_data->ker.DISC_Ind = TRUE;

        return;
      }

      if (rlp_data->ker.RRReady EQ FALSE)
      {
        TRACE_EVENT ("Remote RR");
      }
      rlp_data->ker.RRReady = TRUE;
      break;

    case SF_SREJ:
      sbm_srej_frame (nr);
      TIMERSTOP (TT_HANDLE);
      return;
  }

  if (allFramesAck)
  {
    TIMERSTOP (TT_HANDLE);
  }

}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_send_data           |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void ker_send_data(void)
{
  T_EXT_FRAME_NUM slot;
  T_FRAME_NUM     vr;

  TRACE_FUNCTION ("ker_send_data()");

  if (rlp_data->ker.UA_State EQ IS_SEND)
  {
    sig_ker_snd_ua_req (rlp_data->ker.UA_FBit);
    rlp_data->ker.UA_State = IS_IDLE;
  }
  else
  {
    vr = rbm_get_vr();
    if (rlp_data->ker.Ackn_FBit EQ 1)
    {
      if (rlp_data->ker.LRReady)
        ker_send_rr_rri_resp (vr);
      else
        ker_send_rnr_rnri_resp (vr);
    }
    else
    {
      switch (rlp_data->ker.REJ_State)
      {
        case ISW_SEND:
          ker_send_rej_reji_cmd (vr);
          break;

        default:
          slot = rbm_check_slots_srej();
          if (slot EQ INVALID_IDX)
          {
            if (rlp_data->ker.LRReady)
              ker_send_rr_rri_cmd (vr);
            else
              ker_send_rnr_rnri_cmd (vr);
          }
          else
            ker_send_srej_sreji_cmd (slot);
          break;
      }
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_send_txu            |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL BOOL ker_send_txu
            (
              void
            )
{
  TRACE_FUNCTION ("ker_send_txu()");

  if (rlp_data->ker.TEST_R_State EQ IS_SEND)
  {
    sig_ker_snd_test_req
    (
      0,
      rlp_data->ker.TEST_R_FBit,
      &rlp_data->ker.TEST_R_FrameDesc
    );
    rlp_data->ker.TEST_R_State = IS_IDLE;
    return TRUE;
  }

  if (rlp_data->ker.XID_R_State EQ IS_SEND)
  {
    TRACE_EVENT ("Snd XID R");
    sig_ker_snd_xid_req
    (
      0,
      1,
      &rlp_data->ker.XID_R_FrameDesc
    );

    rlp_data->ker.XID_R_State = IS_IDLE;
    return TRUE;
  }

  if (GET_STATE(KERXID_C) EQ ISW_SEND)
  {
    if (rlp_data->ker.Poll_xchg NEQ IW_WAIT)
    {
      TRACE_EVENT ("Snd XID C");
      sig_ker_snd_xid_req
      (
        1,
        1,
        &rlp_data->ker.XID_C_FrameDesc
      );
      SET_STATE(KERXID_C, ISW_WAIT);

      TIMERSTART(TXID_HANDLE, rlp_data->ker.T1);
      rlp_data->ker.Poll_xchg = IW_WAIT;
      return TRUE;
    }
  }

  if (rlp_data->ker.UI_State EQ IS_SEND)
  {
    sig_ker_snd_ui_req
    (
      1,
      0,
      &rlp_data->ker.UI_FrameDesc
    );
    rlp_data->ker.UI_State = IS_IDLE;
    return TRUE;
  }

  return FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_deinit_link_vars    |
+--------------------------------------------------------------------+

  PURPOSE : deinitialize send/receive buffer manager,

*/

GLOBAL void ker_deinit_link_vars
            (
              void
            )
{

  TRACE_FUNCTION ("ker_deinit_link_vars()");

  rlp_data->ker.Connected = FALSE;

  sbm_deinit ();

  rbm_deinit ();

  srm_deinit ();

}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_send_rlp_error_ind  |
+--------------------------------------------------------------------+

  PURPOSE : send a error cause to L2R

*/

GLOBAL void ker_send_rlp_error_ind(USHORT cause)
{
  PALLOC (rlp_error_ind, RLP_ERROR_IND);

  TRACE_FUNCTION ("ker_send_rlp_error_ind()");
  TRACE_EVENT_P1("RLP Error : %d", cause);

  rlp_error_ind->cause = cause;
  PSENDX (L2R, rlp_error_ind);
  ker_deinit_link_vars ();
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_getSlotTRCVS        |
+--------------------------------------------------------------------+

  PURPOSE : search the rcvs entry for a given timer handle. The
            function returns the corresponding slot. The entry
            is freed. This function is used in the timeout handling
            functions of the entity.

*/
GLOBAL T_FRAME_NUM ker_getSlotTRCVS (USHORT index)
{
  rlp_data->ker.rcvsTab[index].trcvs_running = FALSE;
  rlp_data->ker.rcvsTab[index].isFree = TRUE;

  return rlp_data->ker.rcvsTab[index].slot;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_fill_remap_frame    |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void               ker_fill_remap_frame
                          (
                            T_FRAME_NUM  vr
                          )
{

  TRACE_FUNCTION ("ker_fill_remap_frame()");

  rlp_data->ker.REMAP_Frame[HEADER_LEN + 0] = vr * 4;
  rlp_data->ker.REMAP_Frame[HEADER_LEN + 1] = 0;

  ker_put_xid_data
    (
    rlp_data->ker.REMAP_Frame + HEADER_LEN,
    2,
    UF_ALL,
    TRUE,
    ORIG_RLP_VERS,
    rlp_data->ker.Orig_K_iwf_ms,
    rlp_data->ker.Orig_K_ms_iwf,
    rlp_data->ker.Orig_T1,
    rlp_data->ker.Orig_N2,
    rlp_data->ker.Orig_T2,
    rlp_data->ker.Orig_Pt,
    rlp_data->ker.Orig_P0,
    rlp_data->ker.Orig_P1,
    rlp_data->ker.Orig_P2
    );

}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_send_remap_data     |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL BOOL               ker_send_remap_data
                          (
                            void
                          )
{
  T_RLP_DATA_REQ  *prim;

  TRACE_FUNCTION ("ker_send_remap_data()");

  if (sbm_recall_prim (&prim))
  {
    PPASS (prim, rlp_remap_data_ind, RLP_REMAP_DATA_IND);
    rlp_remap_data_ind->data_size = rlp_data->ker.OldFrameSize - HEADER_LEN - TRAILER_LEN;
    PSENDX (L2R, rlp_remap_data_ind);
    return TRUE;
  }
  return FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_set_frame_size      |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void               ker_set_frame_size
                          (
                            UBYTE rate
                          )
{

  TRACE_FUNCTION ("ker_set_frame_size()");

  switch(rate)
  {
  case RLP_FULLRATE_14400:
    rlp_data->ker.Def_T1 = DEF_T1_FR14400;
    rlp_data->ker.FrameSize = FRAME_SIZE_LONG;
    break;
  case RLP_FULLRATE_9600:
    rlp_data->ker.Def_T1 = DEF_T1_FR9600;
    rlp_data->ker.FrameSize = FRAME_SIZE_SHORT;
    break;
  case RLP_FULLRATE_4800:
    rlp_data->ker.Def_T1 = DEF_T1_FR4800;
    rlp_data->ker.FrameSize = FRAME_SIZE_SHORT;
    break;
  case RLP_HALFRATE_4800:
    rlp_data->ker.Def_T1 = DEF_T1_HR4800;
    rlp_data->ker.FrameSize = FRAME_SIZE_SHORT;
    break;
  }

  rlp_data->ker.FramesPerPrim =
    (rlp_data->ker.BytesPerPrim + rlp_data->ker.FrameSize - 1) /
    rlp_data->ker.FrameSize;

  sig_ker_snd_set_frame_size_req (rlp_data->ker.FrameSize);

}

