/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  Rlp_sbm.c
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
|  Purpose :  This Modul defines the send buffer manager for 
|             the component Radio Link Protocol of the mobile station 
+----------------------------------------------------------------------------- 
*/ 

#ifndef RLP_SBM_C
#define RLP_SBM_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"
#include "cus_rlp.h"
#include "prim.h"
#include "tok.h"
#include "rlp.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : sbmi_srej_list_delete
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : lo -
|                hi -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void sbmi_srej_list_delete
           (
             T_SBM_BUFFER_INDEX lo,
             T_SBM_BUFFER_INDEX hi
           )
{
  T_SBM_BUFFER_EXTIDX p, p_last;

  TRACE_FUNCTION ("sbmi_srej_list_delete()");

  p      = rlp_data->sbm.SREJ_List;
  p_last = SBM_INVALID_IDX;

  if (lo <= hi)
  {
    while (p NEQ SBM_INVALID_IDX)
    {
      if (p >= lo AND p < hi)
      {
        if (p_last EQ SBM_INVALID_IDX)
          rlp_data->sbm.SREJ_List = rlp_data->sbm.Frame[p].NextSREJ;
        else
          rlp_data->sbm.Frame[p_last].NextSREJ
             = rlp_data->sbm.Frame[p].NextSREJ;
      }
      p_last = p;
      p = rlp_data->sbm.Frame[p].NextSREJ;
    }
  }
  else
  {
    while (p NEQ SBM_INVALID_IDX)
    {
      if (p >= lo OR p < hi)
      {
        if (p_last EQ SBM_INVALID_IDX)
          rlp_data->sbm.SREJ_List = rlp_data->sbm.Frame[p].NextSREJ;
        else
          rlp_data->sbm.Frame[p_last].NextSREJ
             = rlp_data->sbm.Frame[p].NextSREJ;
      }
      p_last = p;
      p = rlp_data->sbm.Frame[p].NextSREJ;
    }
  }
}


/*
+------------------------------------------------------------------------------
|  Function    : sbmi_srej_list_put
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : p -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void sbmi_srej_list_put
           (
             T_SBM_BUFFER_EXTIDX p
           )
{
  TRACE_FUNCTION ("sbmi_srej_list_put()");

  rlp_data->sbm.Frame[p].NextSREJ = rlp_data->sbm.SREJ_List;
  
  rlp_data->sbm.SREJ_List = p;
}

/*
+------------------------------------------------------------------------------
|  Function    : sbmi_srej_list_get
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : p
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void sbmi_srej_list_get
           (
             T_SBM_BUFFER_EXTIDX *p
           )
{
  TRACE_FUNCTION ("sbmi_srej_list_get()");

  if ((*p = rlp_data->sbm.SREJ_List) NEQ SBM_INVALID_IDX)
    rlp_data->sbm.SREJ_List = rlp_data->sbm.Frame[*p].NextSREJ;
}

/*
+------------------------------------------------------------------------------
|  Function    : sbmi_get_frame_desc
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : p          -
|                frameDesc  -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void sbmi_get_frame_desc
           (
             T_SBM_BUFFER_EXTIDX   p,
             T_FRAME_DESC         *frameDesc
           )
{
  TRACE_FUNCTION ("sbmi_get_frame_desc()");

  if (rlp_data->sbm.Frame[p].PrimSlot1 NEQ SBM_INVALID_IDX)
  {
    frameDesc->Adr[0]
      = rlp_data->sbm.Prim[rlp_data->sbm.Frame[p].PrimSlot1].Prim->sdu.buf
      + rlp_data->sbm.Frame[p].Offset1;

    frameDesc->Len[0] = rlp_data->sbm.Frame[p].Len1;
  
    if (rlp_data->sbm.Frame[p].PrimSlot2 NEQ SBM_INVALID_IDX)
    {
      frameDesc->Adr[1]
        = rlp_data->sbm.Prim[rlp_data->sbm.Frame[p].PrimSlot2].Prim->sdu.buf
        + rlp_data->sbm.Frame[p].Offset2;

      frameDesc->Len[1] = rlp_data->sbm.Frame[p].Len2;
    }
    else
    {
      frameDesc->Adr[1] = (UBYTE *) NULL;
      frameDesc->Len[1] = 0;
    }
  }
  else
    if (rlp_data->sbm.Frame[p].PrimSlot2 NEQ SBM_INVALID_IDX)
    {
      frameDesc->Adr[0]
        = rlp_data->sbm.Prim[rlp_data->sbm.Frame[p].PrimSlot2].Prim->sdu.buf
        + rlp_data->sbm.Frame[p].Offset2;

      frameDesc->Len[0] = rlp_data->sbm.Frame[p].Len2;

      frameDesc->Adr[1] = (UBYTE *) NULL;
      frameDesc->Len[1] = 0;
    }
}

/*
+------------------------------------------------------------------------------
|  Function    : sbmi_dec_ref_count
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : slot -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void sbmi_dec_ref_count
           (
             T_SBM_PRIM_EXTIDX slot
           )
{
  TRACE_FUNCTION ("sbmi_dec_ref_count()");

  if (slot NEQ SBM_INVALID_IDX)
  {
    if (--rlp_data->sbm.Prim[slot].RefCount EQ 0)
    {
      PFREE (rlp_data->sbm.Prim[slot].Prim);
      rlp_data->sbm.Prim[slot].Prim = NULL;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sbmi_data_to_transmit
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : p                  -
|                framesCouldBeSent  -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void sbmi_data_to_transmit
           (
             T_SBM_BUFFER_EXTIDX *p,
             BOOL                *framesCouldBeSent
           )
{
  TRACE_FUNCTION ("sbmi_data_to_transmit()");
  
  *framesCouldBeSent = FALSE;
  sbmi_srej_list_get (p);

  /*
   * SREJ slot found ?
   */
  if (*p EQ SBM_INVALID_IDX)
  {
    /*
     * No! - Is window open ?
     */
    if (rlp_data->sbm.VS NEQ (rlp_data->sbm.VA+rlp_data->sbm.K)
                              % SBM_BUF_SIZE)
    {
      *framesCouldBeSent = TRUE;
      /*
       * regular slot to send ?
       */
      if (rlp_data->sbm.VD NEQ rlp_data->sbm.VS)
      {
        *p = rlp_data->sbm.VS;
        rlp_data->sbm.VS = (rlp_data->sbm.VS+1) % SBM_BUF_SIZE;

      }
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_set_wind_size
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : windSize
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sbm_set_wind_size
            (
              T_SBM_BUFFER_INDEX windSize
            )
{
  T_RBM_BUFFER_INDEX n;
  T_RBM_BUFFER_INDEX nEnd;
  T_RBM_BUFFER_INDEX oldSize;

  TRACE_FUNCTION ("sbm_set_wind_size()");

  oldSize = rlp_data->sbm.K;
  rlp_data->sbm.K = windSize;

  if (windSize < oldSize)
  {
    n = rlp_data->sbm.VA + windSize;
    if (n >= SBM_BUF_SIZE)
    {
      n -= SBM_BUF_SIZE;
    }
    nEnd = rlp_data->sbm.VA + oldSize;
    if (nEnd >= SBM_BUF_SIZE)
    {
      nEnd -= SBM_BUF_SIZE;
    }
    sbmi_srej_list_delete (n, nEnd);
  }  
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_init
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : windSize   -
|                frameSize  -
|                n2         -
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sbm_init
            (
              T_SBM_BUFFER_INDEX windSize,
              USHORT             frameSize,
              UBYTE              n2
            )
{
  USHORT n;

  TRACE_FUNCTION ("sbm_init()");

  for (n = 0; n < SBM_BUF_SIZE; n++)
  {
    rlp_data->sbm.Frame[n].S_State   = SBM_IDLE;
    rlp_data->sbm.Frame[n].NextSREJ  = SBM_INVALID_IDX;
    rlp_data->sbm.Frame[n].PrimSlot1 = 0;
    rlp_data->sbm.Frame[n].PrimSlot2 = 0;
    rlp_data->sbm.Frame[n].Offset1   = 0;
    rlp_data->sbm.Frame[n].Offset2   = 0;
    rlp_data->sbm.Frame[n].Len1      = 0;
    rlp_data->sbm.Frame[n].Len2      = 0;
  }

  for (n=0; n < SBM_PRIMBUF_SIZE; n++)
  {
    rlp_data->sbm.Prim[n].Prim = NULL;
    rlp_data->sbm.Prim[n].RefCount = 0;
  }

  rlp_data->sbm.VA = 0;
  rlp_data->sbm.VS = 0;
  rlp_data->sbm.VD = 0;
  rlp_data->sbm.LastSentFrame = SBM_INVALID_IDX;
  rlp_data->sbm.CurrPrimSlot  = 0;
  rlp_data->sbm.CurrPrimOff   = 0;
  rlp_data->sbm.CurrPrimLen   = 0;
  rlp_data->sbm.SREJ_List     = SBM_INVALID_IDX;
  rlp_data->sbm.FrameSize     = frameSize;

  sbm_set_wind_size (windSize);
  sbm_set_retrans (n2);
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_store_prim
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : sendPrim -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sbm_store_prim
            (
              T_SBM_PRIM *sendPrim
            )
{

  TRACE_FUNCTION ("sbm_store_prim()");


  if (rlp_data->sbm.CurrPrimLen EQ 0)
  {
    rlp_data->sbm.Prim[rlp_data->sbm.CurrPrimSlot].Prim = sendPrim;
    rlp_data->sbm.CurrPrimOff = sendPrim->sdu.o_buf >> 3;
    rlp_data->sbm.CurrPrimLen = sendPrim->sdu.l_buf >> 3;
  }
  else if (rlp_data->sbm.CurrPrimLen > 0)
  {
    rlp_data->sbm.Frame[rlp_data->sbm.VD].PrimSlot1 = rlp_data->sbm.CurrPrimSlot;
    rlp_data->sbm.Frame[rlp_data->sbm.VD].Offset1   = rlp_data->sbm.CurrPrimOff;
    rlp_data->sbm.Frame[rlp_data->sbm.VD].Len1      = rlp_data->sbm.CurrPrimLen;

    rlp_data->sbm.CurrPrimSlot = (rlp_data->sbm.CurrPrimSlot+1)
                                 % SBM_PRIMBUF_SIZE;

    rlp_data->sbm.Prim[rlp_data->sbm.CurrPrimSlot].RefCount++;

    rlp_data->sbm.Prim[rlp_data->sbm.CurrPrimSlot].Prim = sendPrim;
    rlp_data->sbm.CurrPrimOff = sendPrim->sdu.o_buf >> 3;
    rlp_data->sbm.CurrPrimLen = sendPrim->sdu.l_buf >> 3;



    rlp_data->sbm.Frame[rlp_data->sbm.VD].PrimSlot2 = rlp_data->sbm.CurrPrimSlot;
    rlp_data->sbm.Frame[rlp_data->sbm.VD].Offset2   = rlp_data->sbm.CurrPrimOff;

    rlp_data->sbm.Frame[rlp_data->sbm.VD].Len2
      = (rlp_data->sbm.FrameSize - rlp_data->sbm.Frame[rlp_data->sbm.VD].Len1);

    if (rlp_data->sbm.Frame[rlp_data->sbm.VD].Len2 > rlp_data->sbm.CurrPrimLen)
    {
      rlp_data->sbm.Frame[rlp_data->sbm.VD].Len2 = rlp_data->sbm.CurrPrimLen;
    }

    rlp_data->sbm.Frame[rlp_data->sbm.VD].S_State = SBM_SEND;
  
    rlp_data->sbm.CurrPrimOff += rlp_data->sbm.Frame[rlp_data->sbm.VD].Len2;

    rlp_data->sbm.CurrPrimLen -= rlp_data->sbm.Frame[rlp_data->sbm.VD].Len2;

    rlp_data->sbm.VD = (rlp_data->sbm.VD+1) % SBM_BUF_SIZE;

  }

  
  while (rlp_data->sbm.CurrPrimLen >= rlp_data->sbm.FrameSize)
  {
    rlp_data->sbm.Frame[rlp_data->sbm.VD].PrimSlot1
      = rlp_data->sbm.CurrPrimSlot;

    rlp_data->sbm.Frame[rlp_data->sbm.VD].Offset1
      = rlp_data->sbm.CurrPrimOff;

    rlp_data->sbm.Frame[rlp_data->sbm.VD].Len1
      = rlp_data->sbm.FrameSize;

    rlp_data->sbm.CurrPrimOff += rlp_data->sbm.FrameSize;
    rlp_data->sbm.CurrPrimLen -= rlp_data->sbm.FrameSize;

    rlp_data->sbm.Prim[rlp_data->sbm.CurrPrimSlot].RefCount++;

    /*
     * the frame resides only in the first primitive
     * -> the seconds description is set to invalid
     */
    rlp_data->sbm.Frame[rlp_data->sbm.VD].Len2      = 0;
    rlp_data->sbm.Frame[rlp_data->sbm.VD].Offset2   = 0;
    rlp_data->sbm.Frame[rlp_data->sbm.VD].PrimSlot2 = SBM_INVALID_IDX;

    rlp_data->sbm.Frame[rlp_data->sbm.VD].S_State = SBM_SEND;

    rlp_data->sbm.VD = (rlp_data->sbm.VD+1) % SBM_BUF_SIZE;
  }

  if (rlp_data->sbm.CurrPrimLen > 0)
    rlp_data->sbm.Prim[rlp_data->sbm.CurrPrimSlot].RefCount++;

  else if (rlp_data->sbm.CurrPrimLen EQ 0)
  {
    rlp_data->sbm.CurrPrimSlot = (rlp_data->sbm.CurrPrimSlot+1)
                                 % SBM_PRIMBUF_SIZE;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_ack_upto_n
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : n            -
|                allFramesAck -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL BOOL sbm_ack_upto_n(T_SBM_BUFFER_EXTIDX n)
{
  TRACE_FUNCTION ("sbm_ack_upto_n()");
  
  if (n EQ SBM_INVALID_IDX)
  {
    if (rlp_data->sbm.LastSentFrame NEQ SBM_INVALID_IDX)
    {
      n = rlp_data->sbm.LastSentFrame;
      rlp_data->sbm.LastSentFrame = SBM_INVALID_IDX;
    }
    else
    {
      return (rlp_data->sbm.VA EQ rlp_data->sbm.VS);
    }
  } 
 
  sbmi_srej_list_delete (rlp_data->sbm.VA, n);

  while (rlp_data->sbm.VA NEQ n)
  {
    rlp_data->sbm.Frame[rlp_data->sbm.VA].S_State = SBM_IDLE;
    sbmi_dec_ref_count (rlp_data->sbm.Frame [rlp_data->sbm.VA].PrimSlot1);
    sbmi_dec_ref_count (rlp_data->sbm.Frame [rlp_data->sbm.VA].PrimSlot2);
    rlp_data->sbm.VA = (rlp_data->sbm.VA+1) % SBM_BUF_SIZE;
  }

  return (rlp_data->sbm.VA EQ rlp_data->sbm.VS);
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_srej_frame
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : n -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sbm_srej_frame
            (
              T_SBM_BUFFER_INDEX n
            )
{
  TRACE_FUNCTION ("sbm_srej_frame()");

  rlp_data->sbm.Frame[n].S_State = SBM_SEND;
  sbmi_srej_list_put (n);
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_get_frame
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : frameDesc         -
|                frameNo           -
|                framesCouldBeSent -
|                ok                -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sbm_get_frame
            (
              T_FRAME_DESC       *frameDesc,
              T_SBM_BUFFER_INDEX *frameNo,
              BOOL               *framesCouldBeSent,
              BOOL               *ok
            )
{
  T_SBM_BUFFER_EXTIDX p;

  TRACE_FUNCTION ("sbm_get_frame()");

  sbmi_data_to_transmit (&p, framesCouldBeSent);

  if (p NEQ SBM_INVALID_IDX)
  {
    sbmi_get_frame_desc (p, frameDesc);
    rlp_data->sbm.Frame[p].S_State = SBM_WAIT;
    *frameNo = rlp_data->sbm.LastSentFrame = p;

    *ok = TRUE;
  }
  else
    *ok = FALSE;
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_rej_from_n
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : n            -
|                retransError -
|
|  Return      : 
+------------------------------------------------------------------------------
*/


GLOBAL void sbm_rej_from_n
            (
              T_SBM_BUFFER_INDEX n,
              BOOL *retransError
            )
{

  T_SBM_BUFFER_INDEX ind;

  TRACE_FUNCTION ("sbm_rej_from_n()");

  while ( rlp_data->sbm.REJ_BufLo NEQ rlp_data->sbm.REJ_BufHi AND 
         !(
          XOR
           (
            XOR (
                  rlp_data->sbm.REJ_BufLastN > rlp_data->sbm.REJ_Buf[rlp_data->sbm.REJ_BufLo],
                  rlp_data->sbm.REJ_Buf[rlp_data->sbm.REJ_BufLo] > n 
                ),
                rlp_data->sbm.REJ_BufLastN > n
            )
           )
        )
        rlp_data->sbm.REJ_BufLo = (rlp_data->sbm.REJ_BufLo +1) % rlp_data->sbm.REJ_BufSize;

  
  ind = rlp_data->sbm.REJ_BufHi;

  while (ind NEQ rlp_data->sbm.REJ_BufLo)
  {
    if(ind EQ 0)
      ind = rlp_data->sbm.REJ_BufSize;

    ind--;

    if (! XOR(
            XOR(
                n > rlp_data->sbm.REJ_Buf[ind],
                rlp_data->sbm.REJ_Buf[ind] > rlp_data->sbm.VS
                ),
                n > rlp_data->sbm.VS
             )
        )
        break;

    rlp_data->sbm.REJ_Buf[ind] = rlp_data->sbm.VS;
  }

  rlp_data->sbm.REJ_Buf[rlp_data->sbm.REJ_BufHi] = rlp_data->sbm.VS;
  
  rlp_data->sbm.REJ_BufHi = (rlp_data->sbm.REJ_BufHi + 1) % rlp_data->sbm.REJ_BufSize;
  
  rlp_data->sbm.REJ_BufLastN = n;

  
  if (rlp_data->sbm.REJ_BufLo EQ rlp_data->sbm.REJ_BufHi)
  {    
    *retransError = TRUE;
    return;
  }
    
  *retransError = FALSE;
  
  sbmi_srej_list_delete (n, rlp_data->sbm.VS);

  while (rlp_data->sbm.VS NEQ n)
  {
    if (rlp_data->sbm.VS > 0)
      rlp_data->sbm.VS--;
    else
      rlp_data->sbm.VS = SBM_BUF_SIZE-1;

    rlp_data->sbm.Frame[rlp_data->sbm.VS].S_State = SBM_SEND;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_frame_in_range
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : n -
|                 
|
|  Return      : TRUE  -
|                FALSE -
+------------------------------------------------------------------------------
*/


GLOBAL BOOL sbm_frame_in_range
            (
              T_SBM_BUFFER_INDEX n
            )
{
  TRACE_FUNCTION ("sbm_frame_in_range()");

  return  !( XOR 
             (
               XOR 
               (
                 rlp_data->sbm.VA > n,
                 n > rlp_data->sbm.VS
               ),
               rlp_data->sbm.VA > rlp_data->sbm.VS
             )
            );
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_space_in_buf
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : -
|                 
|
|  Return      : ret
+------------------------------------------------------------------------------
*/


GLOBAL USHORT sbm_space_in_buf
              (
                void
              )
{
  SHORT ret;

  TRACE_FUNCTION ("sbm_space_in_buf()");

  ret = rlp_data->sbm.VA - rlp_data->sbm.VD - 1;

  if (ret < 0)
    return (USHORT) (ret + SBM_BUF_SIZE);
  else
    return (USHORT) ret;
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_deinit
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sbm_deinit
              (
                void
              )
{
  SHORT n;

  TRACE_FUNCTION ("sbm_deinit()");

  n = 0;

  while (n < SBM_PRIMBUF_SIZE)
  {
    if (rlp_data->sbm.Prim[n].Prim NEQ NULL)
    {
      PFREE(rlp_data->sbm.Prim[n].Prim);
      rlp_data->sbm.Prim[n].Prim = NULL;
    }
    n++;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_set_retrans
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : n2 -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sbm_set_retrans
            (
              UBYTE n2
            )
{
  TRACE_FUNCTION ("sbm_set_retrans()");

  rlp_data->sbm.REJ_BufSize = n2 + 1;
  rlp_data->sbm.REJ_BufLo   = 0;
  rlp_data->sbm.REJ_BufHi   = 0;
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_check_rej_count
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : retransError
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sbm_check_rej_count
            (
              BOOL *retransError
            )
{

  T_SBM_BUFFER_INDEX n;
  
  TRACE_FUNCTION ("sbm_check_rej_count()");

  n = rlp_data->sbm.REJ_BufHi +1;

  if (rlp_data->sbm.REJ_BufLo NEQ n)
  {
    if (n NEQ rlp_data->sbm.REJ_BufSize OR
          rlp_data->sbm.REJ_BufLo NEQ 0)
       *retransError = FALSE;
    else
       *retransError = TRUE;
  }
  else
      *retransError = TRUE;
}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_recall_prim
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : prim -
|                 
|
|  Return      : TRUE  -
|                FALSE -
+------------------------------------------------------------------------------
*/


GLOBAL BOOL sbm_recall_prim
             (
               T_SBM_PRIM **prim
             )
{
  T_SBM_PRIM_EXTIDX    primSlot;  
  T_SBM_PRIM_EXTIDX    endPrimSlot;  
  T_SBM_PRIM_EXTIDX    firstPrimSlot;  
  T_SBM_BUFFER_INDEX   vend;  

  TRACE_FUNCTION ("sbm_recall_prim()");

  if (rlp_data->sbm.VA EQ rlp_data->sbm.VD)
  {
    return FALSE;
  }

  primSlot = rlp_data->sbm.Frame[rlp_data->sbm.VA].PrimSlot1;
  firstPrimSlot = primSlot;

  if (rlp_data->sbm.VD EQ 0)
  {
    vend = SBM_BUF_SIZE - 1;
  }
  else
  {
    vend = rlp_data->sbm.VD - 1;
  }

  if (rlp_data->sbm.Frame[vend].Len2 EQ 0)
  {
    endPrimSlot = rlp_data->sbm.Frame[vend].PrimSlot1 + 1;
  }
  else
  {
    endPrimSlot = rlp_data->sbm.Frame[vend].PrimSlot2 + 1;
  }

  if (endPrimSlot >= SBM_PRIMBUF_SIZE)
  {
    endPrimSlot = 0;
  }

  while ((primSlot NEQ endPrimSlot) AND
         (rlp_data->sbm.Prim[primSlot].Prim EQ NULL))
  {
    primSlot += 1;
    if (primSlot >= SBM_PRIMBUF_SIZE)
    {
      primSlot = 0;
    }
  }

  if (primSlot EQ endPrimSlot)
  {
    return FALSE;
  }

  if (primSlot EQ firstPrimSlot)
  {
    /* in the first primitive there may be some frames sent already */
    rlp_data->sbm.Prim[primSlot].Prim->sdu.l_buf =
      rlp_data->sbm.Prim[primSlot].Prim->sdu.l_buf 
      - rlp_data->sbm.Frame[rlp_data->sbm.VA].Offset1 * 8
      + rlp_data->sbm.Prim[primSlot].Prim->sdu.o_buf;
    
    rlp_data->sbm.Prim[primSlot].Prim->sdu.o_buf = 
      rlp_data->sbm.Frame[rlp_data->sbm.VA].Offset1 * 8;
  }
  *prim = rlp_data->sbm.Prim[primSlot].Prim;
  rlp_data->sbm.Prim[primSlot].Prim = NULL;
  rlp_data->sbm.Prim[primSlot].RefCount = 0;
  return TRUE;

}

/*
+------------------------------------------------------------------------------
|  Function    : sbm_reset_after_remap
+------------------------------------------------------------------------------
|  Description : 
|
|  Parameters  : newFrameSize -
|                new_vs       -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sbm_reset_after_remap
             (
               USHORT             newFrameSize,
               T_SBM_BUFFER_INDEX new_vs
             )
{

  
  TRACE_FUNCTION ("sbm_reset_after_remap()");
  sbmi_srej_list_delete (0, SBM_BUF_SIZE);
  sbm_init
    (
    rlp_data->sbm.K, 
    newFrameSize, 
    (UBYTE)(rlp_data->sbm.REJ_BufSize - 1)
    );
  rlp_data->sbm.VA = new_vs;
  rlp_data->sbm.VS = new_vs;
  rlp_data->sbm.VD = new_vs;
}
