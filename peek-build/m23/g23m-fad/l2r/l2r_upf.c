/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  L2r_upf.c
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
|             the component L2R of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef L2R_UPF_C
#define L2R_UPF_C
#endif

#define ENTITY_L2R

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "cus_l2r.h"
#include "cnf_l2r.h"
#include "mon_l2r.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"      /* functionality of the dti library */

#include "cl_ribu.h"
#include "l2r.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : up_init
+------------------------------------------------------------------------------
|  Description : initialise the l2r data for the uplink process
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_init(T_UP *dup)
{
  register ULONG i;

  TRACE_FUNCTION ("up_init()");

  dup->FlowCtrlUsed      = FALSE;
  dup->DnFlow            = FL_INACTIVE;
  dup->UpFlow            = FL_INACTIVE;
  dup->ULFlow            = FL_INACTIVE;
  dup->LLFlow            = FL_INACTIVE;
  dup->MrgFlow           = FL_INACTIVE;
  dup->FlowThreshLo      = MAX_UPRIM_RIBU_SIZE/2;
  dup->FlowThreshHi      = 3*MAX_UPRIM_RIBU_SIZE/4;
  dup->DataSize          = 0;
  dup->FrameSize         = 0;
  dup->OldFrameSize      = 0;
  dup->FramesPerPrim     = L2R_FRAMES_PER_PRIM_MAX;
  dup->LastRcvdSa        = 0;
  dup->LastRcvdSb        = 0;
  dup->LastSentSa        = 0;
  dup->LastSentSb        = 0;
  dup->LastSentFlow      = FL_INACTIVE;
  dup->StoreDataActive   = FALSE;
  dup->UrgentMsg         = FALSE;
  dup->DiscardRemapData  = FALSE;
  dup->QRemapRead        = 0;
  dup->QRemapWrite       = 0;
  dup->BRemapAdr         = NULL;
  dup->BRemapLen         = 0;
  dup->BRemapSa          = 0;
  dup->BRemapSb          = 0;
  dup->BRemapLastState   = 0;
  dup->QRemapPrimDesc.prim = NULL;

  for (i = 0; i < MAX_UP_REMAP_QUEUE_SIZE; i++)
  {
    dup->QRemap[i] = NULL;
  }

  for (i = 0; i < UP_REMAP_BUFFER_SIZE; i++)
  {
    dup->BRemap[i] = 0;
  }

  dup->RiBu.idx.depth = MAX_UPRIM_RIBU_SIZE;

  INIT_STATE (UP_UL, IW_IDLE);
  INIT_STATE (UP_LL, ISW_IDLE);
  INIT_STATE (UP, UP_DISCONNECTED);
}

/*
+------------------------------------------------------------------------------
|  Function    : up_alloc_prim
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void up_alloc_prim(void)
{
  T_UP *dup = &l2r_data->up;

  T_PRIM_INDEX          m;
  T_P_UPRIM_DESCRIPTOR  primDesc;
  USHORT                sduSize;

  TRACE_FUNCTION ("up_alloc_prim()");

  sduSize = dup->FramesPerPrim * dup->FrameSize * 8;
  primDesc = dup->RiBu.primDesc[dup->RiBu.alloc];
  {
    PALLOC_SDU(data_req, RLP_DATA_REQ, sduSize);
    primDesc->prim = data_req;
  }

#ifdef _SIMULATION_
  /* Clear SDU for test environment */
  {
  UBYTE *p;
  UBYTE *pend;
  p = &primDesc->prim->sdu.buf[0];
  pend = p + (sduSize >> 3);
  while (p < pend)
  {
    *p++ = 0;
  }
  }
#endif

  primDesc->prim->sdu.o_buf = 0;
  primDesc->prim->sdu.l_buf = sduSize;
  primDesc->nFr             = dup->FramesPerPrim;
  primDesc->index           = 0;
  primDesc->offset          = 0;
  primDesc->off_status      = 0;
  primDesc->full            = FALSE;
  primDesc->sa              = FL_INACTIVE;
  primDesc->sb              = FL_INACTIVE;
  /*lint -e416 (Warning -- Likely creation of out-of-bounds) */
  for (m = 0; m != dup->FramesPerPrim; m++)
  {
    (*primDesc->adr)[m] = (T_P_L2R_FRAME)(primDesc->prim->sdu.buf +L2R_ENCODING_OFFSET + m * dup->FrameSize);    
  }
   /*lint +e416 (Warning -- Likely creation of out-of-bounds) */	
  dup->RiBu.alloc++;
  if (dup->RiBu.alloc EQ dup->RiBu.idx.depth)
  {
    dup->RiBu.alloc = 0;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : up_send_prim_timeout
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void up_send_prim_timeout(void)
{
  T_UP *dup = &l2r_data->up;

  TRACE_FUNCTION ("up_send_prim_timeout()");

  if (!dup->RiBu.idx.filled)
  {
    if (dup->RiBu.alloc EQ dup->RiBu.idx.wi)
    {
      /* No primitive is there, so we allocate one */
      up_alloc_prim();
    }

    /*
    No primitive is ready, so we take one away from the relay entity
    */
    cl_ribu_write_index(&dup->RiBu.idx); /* point to next primitive */
  }
  up_send_current_prim();
}

/*
+------------------------------------------------------------------------------
|  Function    : up_next_remap_frame
+------------------------------------------------------------------------------
|  Description : Local function, which is used by up_copy_data_from_l2r 
|                to advance to the next frame in the primitive.
|                The variable primDesc of the calling function is updated.
|
|  Parameters  : primDesc -
|
|  Return      : 1 -
|                0 -
+------------------------------------------------------------------------------
*/

LOCAL UBYTE up_next_remap_frame(T_RPRIM_DESCRIPTOR *primDesc)
{
  T_UP *dup = &l2r_data->up;

  U8 i;
  T_P_RLP_REMAP_DATA_IND prim;

  /* next frame */
  primDesc->index++;

  if (primDesc->index >= primDesc->nFr)
  {
    /* primitive is completely read out */
    PFREE(dup->QRemap [dup->QRemapRead]);

    /* next primitive */
    (dup->QRemapRead)++;
    if (dup->QRemapRead >= MAX_UP_REMAP_QUEUE_SIZE)
    {
      dup->QRemapRead = 0;
    }
    if (dup->QRemapRead EQ dup->QRemapWrite)
    {
      /* no primitive is ready */
      primDesc->prim = NULL;
      return (1);
    }

    /* point to next primitive descriptor */

    prim            = dup->QRemap[dup->QRemapRead];
    primDesc->prim  = prim;
    primDesc->nFr   = prim->sdu.l_buf / (8 * prim->data_size + HT_LEN);
    primDesc->index = 0;

    for (i = 0; i < primDesc->nFr; i++)
    {
      primDesc->adr[i] = (T_P_L2R_FRAME)(prim->sdu.buf + (prim->sdu.o_buf>>3)
                          + HEADER_LEN + i * (prim->data_size + HT_LEN));
    }
  }
  primDesc->offset      = 0;
  primDesc->off_status  = 0;
  return (0);
}

/*
+------------------------------------------------------------------------------
|  Function    : up_copy_data_from_rq
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : buf         -
|                len         -
|                sa          -
|                sb          -
|                bytesCopied -
|
|  Return      : -
+------------------------------------------------------------------------------
*/

LOCAL void up_copy_data_from_rq(U8 *buf, U16 len, U8 *sa, U8 *sb, U16 *bytesCopied)
{
  T_UP *dup = &l2r_data->up;

  register ULONG          i;
  register T_P_UBYTE      pFrame;
  register T_P_UBYTE      pBuf;
  register T_P_UBYTE      pStat;
  T_P_RLP_REMAP_DATA_IND  prim;
  T_P_L2R_FRAME           frame;
  T_P_UBYTE               pEnd;
  USHORT                  bytesToCopy;
  USHORT                  blocklen;
  UBYTE                   statOct;
  UBYTE                   lastState;

  T_RPRIM_DESCRIPTOR *primDesc = &dup->QRemapPrimDesc;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("up_copy_data_from_rq()");
#endif

  bytesToCopy = len;

  if (primDesc->prim EQ NULL)
  {
    prim                  = dup->QRemap[dup->QRemapRead];
    primDesc->prim        = prim;
    primDesc->nFr         = prim->sdu.l_buf / ((prim->data_size<<3) + HT_LEN);
    primDesc->index       = 0;
    primDesc->offset      = 0;
    primDesc->off_status  = 0;

    for (i = 0; i < primDesc->nFr; i++)
    {
      primDesc->adr[i] = (T_P_L2R_FRAME)(prim->sdu.buf + (prim->sdu.o_buf>>3)
                                         + HEADER_LEN + i * (prim->data_size + HT_LEN));
    }
  }

  /* skip empty primitive */
  if (primDesc->nFr EQ 0)
  {
    *bytesCopied = 0;
    return;
  }

  frame  = primDesc->adr[primDesc->index];  /* point to current l2r frame in primitive */
  pBuf   = buf;                             /* point to destination buffer */
  pFrame = &((*frame)[primDesc->offset]);
  pStat  = &((*frame)[primDesc->off_status]);

  if (pFrame EQ pStat)
  {
    /* current byte is status octet */
    dup->BRemapLastState = *pFrame & SO_STATUS_BITS_MASK;
  }

  lastState = dup->BRemapLastState;
  *sa = GET_SO_SA_BIT(lastState);
  *sb = GET_SO_SB_BIT(lastState);

  /************************************************************************************
   * loop until either
   *   -  no more data are available or
   *   -  status in L2R frame changes or
   *   -  buffer for data is full
   ************************************************************************************/
  
  for (;;)
  {
#ifdef _SIMULATION_
    TRACE_FUNCTION ("loop");
#endif

    blocklen = pStat - pFrame;
    if (blocklen EQ 0)
    {
      /*
       * current byte is status octet
       * (only in the first pass of the loop, there may be no status octet)
       */

      /*****************************
       *  evaluate status bits
       *****************************/
      statOct = *pFrame;
      if (lastState NEQ (statOct & SO_STATUS_BITS_MASK))
      {
        /*
         *  status has changed. We have to stop,
         *  since only one state can be transmitted to the upper layer
         */
        primDesc->offset = primDesc->off_status = pFrame - (T_P_UBYTE)frame;

        *bytesCopied = len - bytesToCopy;
        return;
      }

      pFrame++;

      /************************************
       *  evaluate addrss bits
       ************************************/
      statOct &= SO_ADR_MASK;

      switch (statOct)
      {
      case SO_BREAK_ACK:
      case SO_BREAK_REQ:
      case SO_END_EMPTY:
        /* no more data in this frame */
        if (up_next_remap_frame(primDesc) EQ 1)
        {
          /* no more data available */
          *bytesCopied = len - bytesToCopy; /* this much data could be copied */
          return;
        }

        frame  = primDesc->adr[primDesc->index];
        pFrame = (T_P_UBYTE)frame;
        pStat  = (T_P_UBYTE)frame;
        continue;  /* continue with next frame */

      case SO_END_FULL:
        pStat = &((*frame)[primDesc->prim->data_size]);
        blocklen = pStat - pFrame;
        break;

      case SO_TWO_OCTET:
          blocklen = *pFrame++ & SO_ADR_MASK_TWO_OCT;
          pStat = pFrame + blocklen;
          break;

      default:
          blocklen = statOct;
          pStat = pFrame + blocklen;
          break;
      }
    }

    if (bytesToCopy < blocklen)
    {
      /***************************************
       *  There is not enough space in the
       *  buffer to copy the complete block
       ***************************************/

      pEnd = pFrame + bytesToCopy;

      while (pFrame < pEnd)
      {
        *pBuf++ = *pFrame++;
      }

      if (pFrame EQ &((*frame)[primDesc->prim->data_size]))
      {
        /* end of frame reached */
        up_next_remap_frame(primDesc);
      }
      else
      {
        primDesc->offset   = pFrame - (T_P_UBYTE)frame;
        primDesc->off_status = pStat  - (T_P_UBYTE)frame;
      }
      *bytesCopied = len; /* this much data could be copied */
      return;
    }
    else
    {
      /***************************************
       *  Copy the complete block
       ***************************************/
      bytesToCopy -= blocklen;

      while (pFrame < pStat)
      {
        *pBuf++ = *pFrame++;
      }

      if (pFrame EQ  &((*frame)[primDesc->prim->data_size]))
      {
        /* end of frame reached */
        if (up_next_remap_frame(primDesc) EQ 1)
        {
          /* no more data available */
          *bytesCopied = len - bytesToCopy; /* this much data could be copied */
          return;
        }
        frame  = primDesc->adr[primDesc->index];
        pFrame = (T_P_UBYTE)frame;
        pStat  = (T_P_UBYTE)frame;
      }
      if (bytesToCopy EQ 0)
      {
        primDesc->offset   = pFrame - (T_P_UBYTE)frame;
        primDesc->off_status = pStat  - (T_P_UBYTE)frame;
        *bytesCopied = len; /* this much data could be copied */
        return;
      }
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : up_fill_in_status_octet
+------------------------------------------------------------------------------
|  Description : local function, which is used patch a status octet into a
|                l2r frame. This function takes into acount the two octet
|                format for 14400.
|
|  Parameters  : pFrame   - points to begin of l2r frame
|                offstat1 - offset of status octet to be patched
|                offstat2 - offset of next status octet
|
|  Return      :
+------------------------------------------------------------------------------
*/

LOCAL BOOL up_fill_in_status_octet(T_P_UBYTE pFrame, UBYTE offstat1, UBYTE offstat2)
{
  register T_P_UBYTE pStat, pSrc, pDes, pEnd;
  register UBYTE dataLen;

  dataLen = offstat2 - offstat1 - 1;  /* number of data bytes between status octets */
  pStat = pFrame + offstat1;

  if (dataLen <= DATA_SIZE_SHORT-2)
  {
    *pStat |= dataLen;
    return (FALSE);
  }
  else
  {
    /* this is only possible with 14400 */
    if (offstat2 < DATA_SIZE_LONG - 1)
    {
      /* a two octet status must be inserted */
      pSrc = pFrame + offstat2;
      pDes = pSrc + 1;
      pEnd = pFrame + offstat1;
      while (pSrc > pEnd)
      {
        *--pDes = *--pSrc;
      }
      *pStat |= SO_TWO_OCTET;
      pStat++;
      *pStat = dataLen;
    }
    else
    {
      /* a additional status octet (FULL) must be inserted somewhere in the frame */
      pSrc = pFrame + offstat2;
      pDes = pSrc + 1;
      pEnd = pFrame + offstat1 + DATA_SIZE_SHORT - 1;
      while (pSrc > pEnd)
      {
        *--pDes = *--pSrc;
      }
      *pStat |= DATA_SIZE_SHORT - 2;
      *pEnd = *pStat & SO_STATUS_BITS_MASK | SO_END_FULL;
    }
    return (TRUE);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : up_next_frame
+------------------------------------------------------------------------------
|  Description : local function, which is used by up_copy_data_into_l2r to
|                advance to the next frame in the primitive
|                The variables primDesc, frame and pFrame of the calling function
|                are updated. Moreover the return values of the calling function
|                primToSend and bytesCopied are set.
|
|
|  Parameters  : primDesc   -
|                primToSend -
|
|  Return      :
+------------------------------------------------------------------------------
*/

LOCAL UBYTE up_next_frame(T_P_UPRIM_DESCRIPTOR *primDesc, BOOL *primToSend)
{
  T_UP *dup = &l2r_data->up;

  TRACE_FUNCTION("up_next_frame()");

  (*primDesc)->index++;
  (*primDesc)->offset = 0;

  if ((*primDesc)->index >= dup->FramesPerPrim) /* primitive is filled */
  {
    (*primDesc)->full = TRUE;
    *primToSend = TRUE;

    cl_ribu_write_index(&dup->RiBu.idx); /* point to next primitive */

    if (dup->RiBu.alloc EQ dup->RiBu.idx.wi)
    {
      /* no primitive is ready */
      return (1);
    }

    /* point to next primitive descriptor */
    *primDesc = dup->RiBu.primDesc[dup->RiBu.idx.wi];
    (*primDesc)->index = 0;
  }
  return (0);
}

LOCAL void up_copy_data_into_l2r(U8 *buf, U16 len, U8 sa, U8 sb, U8 x, BOOL *primToSend, U16 *bytesCopied)
{
  T_UP *dup = &l2r_data->up;

  T_P_UPRIM_DESCRIPTOR primDesc;
  T_P_L2R_FRAME frame;
  register T_P_UBYTE pFrame;
  register T_P_UBYTE pEnd;
  register T_P_UBYTE pBuf;
  USHORT bytesToCopy;
  UBYTE frameCount;
  T_FLOW flow = FL_INVALID;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("up_copy_data_into_l2r()");
#endif

  switch (x)
  {
  case DTI_FLOW_ON:
    flow = FL_INACTIVE;
    break;
  case DTI_FLOW_OFF:
    flow = FL_ACTIVE;
    break;
  }

#ifdef _TARGET_
  if (dup->DataSize > DATA_SIZE_SHORT)
  {
    frameCount = 1;
  }
  else
  {
    frameCount = 2;
  }
#else
  {
    frameCount = 100;
  }
#endif

  up_store_status(sa, sb, flow);

  if (dup->LastSentFlow EQ dup->MrgFlow)
  {
    *primToSend = FALSE;
  }
  else
  {
    *primToSend = TRUE;
  }

  /* don't copy into primitive if no primitive is ready */
  if (dup->RiBu.alloc EQ dup->RiBu.idx.wi)
  {
    *bytesCopied = 0;
    return;
  }

  bytesToCopy = len;

  primDesc = dup->RiBu.primDesc[dup->RiBu.idx.wi]; /* point to current primitive descriptor */
  frame = (*primDesc->adr)[primDesc->index];  /* point to current l2r frame in primitive */
  pFrame = &(*frame)[primDesc->offset];       /* point to current byte in frame */
  pBuf = buf;                                 /* point to source buffer */

  /************************************************************************************
   *
   * Handle a partly filled frame, which has been written in the primitive previously
   *
   ************************************************************************************/

  if (primDesc->offset NEQ 0)
  {
    /* partly filled frame is there */

    if (bytesToCopy EQ 0)
    /* no data to copy -> quit without writing status octet */
    {
      *bytesCopied = len;
      return;
    }

    if (sa NEQ primDesc->sa OR sb NEQ primDesc->sb)
    {
      /*
       * status has changed since last time
       */

      /* finish previous status octet */
      if (
          up_fill_in_status_octet(
            (T_P_UBYTE)frame,
            primDesc->off_status,
            primDesc->offset
            )
         )
      {
        /* frame data has been moved to make space for two octet status */
        pFrame++;
        (primDesc->offset)++;

        if (primDesc->offset >= (dup->DataSize))
        {
          /* frame is just filled by patching the previous status frame */

          if (up_next_frame(&primDesc, primToSend) EQ 1)
          {
            /* no more space for data */
            *bytesCopied = 0; /* no data could be copied */
            return;
          }
          frameCount--;
#ifdef _TARGET_
          if (frameCount EQ 0)
          {
            *bytesCopied = 0;  /* no data could be copied */
            return;
          }
#endif
          frame = (*primDesc->adr)[primDesc->index];  /* point to current l2r frame in primitive */
          pFrame = &(*frame)[primDesc->offset];       /* point to current byte in frame */
        }
      }

      if (primDesc->offset NEQ 0)
      {
        /* write status octet */
        *pFrame++ = (U8)(sa << SO_SA_BIT | sb << SO_SB_BIT | 0 << SO_X_BIT);    /* x is set to 0 (inactive) by default */

        /* store current status for next call */
        primDesc->sa = sa;
        primDesc->sb = sb;

        primDesc->off_status = primDesc->offset;
        (primDesc->offset)++;
        if (primDesc->offset >= (dup->DataSize))
        {
          /* frame is just filled by the status octet */

          /* finish previous status octet */
          (*frame)[primDesc->off_status] |= SO_END_EMPTY;

          if (up_next_frame(&primDesc, primToSend) EQ 1)
          {
            /* no more space for data */
            *bytesCopied = 0;         /* no data could be copied */
            return;
          }
          frameCount--;
          if (frameCount EQ 0)
          {
            *bytesCopied = 0;         /* no data could be copied */
            return;
          }
          frame = (*primDesc->adr)[primDesc->index];  /* point to current l2r frame in primitive */
          pFrame = &(*frame)[primDesc->offset];       /* point to current byte in frame */
        }
      }
    }

    /*
     * write data into partly filled frame
     */
    if (primDesc->offset NEQ 0)
    {
      if (bytesToCopy >= dup->DataSize - primDesc->offset)
      {
        /* enough data to fill frame completly */

        for (pEnd = pFrame + dup->DataSize - primDesc->offset; pFrame < pEnd; )
        {
          *pFrame++ = *pBuf++;
        }

        /* finish previous status octet */
        (*frame)[primDesc->off_status] |= SO_END_FULL;

        bytesToCopy -= dup->DataSize - primDesc->offset;

        if (up_next_frame(&primDesc, primToSend) EQ 1)
        {
          /* no more space for data */
          *bytesCopied = len - bytesToCopy; /* this much data could be copied */
          return;
        }
        frameCount--;
        if (frameCount EQ 0)
        {
          *bytesCopied = len - bytesToCopy; /* this much data could be copied */
          return;
        }
        frame = (*primDesc->adr)[primDesc->index];  /* point to current l2r frame in primitive */
        pFrame = &(*frame)[primDesc->offset];       /* point to current byte in frame */

      }
      else
      {
        /* not enough data to fill frame completly */

        for (pEnd = pFrame + bytesToCopy; pFrame < pEnd; )
        {
          *pFrame++ = *pBuf++;
        }
        primDesc->offset += bytesToCopy;
        *bytesCopied = len;
        return; /* Nothing else to do */
      }
    }
  }

  /************************************************************************************
   *
   * Handle consecutive frames, which are filled starting with byte 0
   *
   ************************************************************************************/

  while (bytesToCopy > 0)
  {
    if (bytesToCopy >= dup->DataSize - 1)
    {
      /*
       * There are enough data to fill a frame completely
       */
      *pFrame++ = sa << SO_SA_BIT | sb << SO_SB_BIT | 0 << SO_X_BIT | SO_END_FULL;    /* x is set to 0 (inactive) by default */

      /* store current status for next call */
      primDesc->sa = sa;
      primDesc->sb = sb;

      for (pEnd = pFrame + dup->DataSize - 1; pFrame < pEnd; )
      {
        *pFrame++ = *pBuf++;
      }
      bytesToCopy -= dup->DataSize - 1;

      /* advance to next frame */
      if (up_next_frame(&primDesc, primToSend) EQ 1)
      {
        /* running out of primitives */
        *bytesCopied = len - bytesToCopy; /* this much data could be copied */
        return;
      }
      frameCount--;
      if (frameCount EQ 0)
      {
        *bytesCopied = len - bytesToCopy; /* this much data could be copied */
        return;
      }
      frame = (*primDesc->adr)[primDesc->index];  /* point to current l2r frame in primitive */
      pFrame = &(*frame)[primDesc->offset];       /* point to current byte in frame */
    }
    else
    {
      /*
       * There are some data, but not enough to fill a frame completely
       */
      *pFrame++ = (U8)(sa << SO_SA_BIT | sb << SO_SB_BIT | 0 << SO_X_BIT);    /* x is set to 0 (inactive) by default */

      /* store current status for next call */
      primDesc->sa = sa;
      primDesc->sb = sb;

      for (pEnd = pFrame + bytesToCopy; pFrame < pEnd; )
      {
        *pFrame++ = *pBuf++;
      }
      primDesc->off_status = 0;
      primDesc->offset = bytesToCopy + 1;
      bytesToCopy = 0;
    }
  }
  *bytesCopied = len;
}

/*
+------------------------------------------------------------------------------
|  Function    : up_copy_remap_data
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

LOCAL void up_copy_remap_data(void)
{
  T_UP *dup = &l2r_data->up;

  USHORT bytesCopied;
  BOOL   dummy;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("up_copy_remap_data()");
#endif

  do
  {
    if (dup->BRemapLen EQ 0)
    {
      dup->BRemapAdr = dup->BRemap;
      up_copy_data_from_rq
        (
        dup->BRemapAdr,
        UP_REMAP_BUFFER_SIZE,
        &dup->BRemapSa,
        &dup->BRemapSb,
        &dup->BRemapLen
        );
      if (dup->BRemapLen EQ 0)
      {
        up_check_flow();
        return;
      }
    }
    up_copy_data_into_l2r
      (
      dup->BRemapAdr,
      dup->BRemapLen,
      dup->BRemapSa,
      dup->BRemapSb,
      DTI_FLOW_OFF,
      &dummy,
      &bytesCopied
      );
    dup->BRemapAdr += bytesCopied;
    dup->BRemapLen -= bytesCopied;

  } while (bytesCopied NEQ 0);
}

/*
+------------------------------------------------------------------------------
|  Function    : up_check_alloc
+------------------------------------------------------------------------------
|  Description : This procedure allocates new primitives until there are
|                UP_RIBU_PREALLOC primitives ready.
|
|                I.e.: alloc = write + UP_RIBU_PREALLOC
|
|                It may be less than this, if otherwise the alloc pointer
|                would reach the read pointer.
|
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_check_alloc(void)
{
  T_UP *dup = &l2r_data->up;

  S8 i_from, i_to;

  TRACE_FUNCTION ("up_check_alloc()");

  i_to = dup->RiBu.idx.ri - dup->RiBu.idx.wi - 1;

  if (i_to < 0)
  {
    i_to = i_to + dup->RiBu.idx.depth;
  }

  if (i_to > UP_RIBU_PREALLOC)
  {
    i_to = UP_RIBU_PREALLOC;
  }

  i_from = dup->RiBu.alloc - dup->RiBu.idx.wi;

  if (i_from < 0)
  {
    i_from = i_from + dup->RiBu.idx.depth;
  }

  if (i_from >= i_to)
  {
    return;
  }

  do
  {
    up_alloc_prim();
    i_from++;
  } while (i_from < i_to);

  if (dup->QRemapRead NEQ dup->QRemapWrite)
  {
    up_copy_remap_data();
    return;
  }

  switch (GET_STATE (UP_UL))
  {
  case IW_WAIT:
    break;

  case IW_IDLE:
    if (dup->StoreDataActive EQ FALSE AND dup->Prim NEQ NULL)
    {
      up_store_data();
    }
    break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : up_check_flow
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_check_flow(void)
{
  T_UP *dup = &l2r_data->up;
  
  U8 primCount;

  TRACE_FUNCTION ("up_check_flow()");

  if (!dup->FlowCtrlUsed)
  {
    return;
  }

  primCount = dup->RiBu.idx.filled;

  switch (dup->UpFlow)
  {
  case FL_ACTIVE:
    if (primCount < dup->FlowThreshLo)
    {
      dup->UpFlow = FL_INACTIVE;
    }
    else
    {
      return;
    }
    break;

  case FL_INACTIVE:
    if (primCount >= dup->FlowThreshHi)
    {
      dup->UpFlow = FL_ACTIVE;
    }
    else
    {
      return;
    }
    break;
  }
  sig_up_dn_flow(dup->UpFlow);
}

/*
+------------------------------------------------------------------------------
|  Function    : up_store_data
+------------------------------------------------------------------------------
|  Description : This procedure copies data from a dti_data_req primitive 
|                into the uplink ring buffer.
|
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_store_data(void)
{
  T_UP *dup = &l2r_data->up;

  T_P_UBYTE adr;
  USHORT len;
  BOOL primToSend;
  USHORT bytesCopied;
  USHORT i, j;
  T_desc2 *desc;

  TRACE_FUNCTION ("up_store_data()");

  dup->StoreDataActive = TRUE;

  do
  {
    desc = (T_desc2*)dup->Prim->desc_list2.first;
    if (desc)
    {
      len = desc->len;
      adr = desc->buffer;
    }
    else
    {
      len = 0;
      adr = NULL;
    }
    
    up_copy_data_into_l2r (adr,
      len,
      dup->Prim->parameters.st_lines.st_line_sa,
      dup->Prim->parameters.st_lines.st_line_sb,
      dup->Prim->parameters.st_lines.st_flow,
      &primToSend,
      &bytesCopied);

    dup->Prim->desc_list2.list_len  -= bytesCopied;
 
    if (desc)
    {
      if ((bytesCopied EQ len))
      {
        /*
         * Block has been copied successfully
         */
        dup->Prim->desc_list2.first = desc->next;
        MFREE (desc);
      }
      else if (bytesCopied > 0)
      {
       /*
        * remaining data must be copied to begin of block
        */
        /*lint -e{661} (Warning -- Likely creation of out-of-bounds) */        
        for (i=0, j = bytesCopied; j < len; i++, j++)
        {
	  desc->buffer[i] = desc->buffer[j];
	 }
        desc->len -= bytesCopied;
      }
    }
    up_check_alloc();

  } while (dup->Prim->desc_list2.first NEQ 0 AND dup->RiBu.alloc NEQ dup->RiBu.idx.wi);

  if (dup->Prim->desc_list2.first EQ 0)
  {
    PFREE (dup->Prim);
    dup->Prim = NULL;
    up_send_ready();
  }
  dup->StoreDataActive = FALSE;
}

/*
+------------------------------------------------------------------------------
|  Function    : up_send_ready
+------------------------------------------------------------------------------
|  Description : only used in test environment
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_send_ready(void)
{
  TRACE_FUNCTION ("up_send_ready()");

  if (GET_STATE (UP_UL) EQ IW_IDLE)
  {
    if (l2r_data->up.DtiConnected EQ FALSE)
    {
      TRACE_EVENT("DTI not connected, but up_send_ready() called -> break");
      return; /* jk: DTI_READY_IND primitive cannot be sent when DTI disconnected (!) */
    }

    dti_start (
      l2r_hDTI,
      L2R_DTI_UP_DEF_INSTANCE,
      L2R_DTI_UP_INTERFACE,
      L2R_DTI_UP_CHANNEL
      );        
    SET_STATE (UP_UL, IW_WAIT);
  }
}

LOCAL void clear_primDesc(T_P_UPRIM_DESCRIPTOR primDesc)
{
  primDesc->prim        = NULL;

  primDesc->nFr         = 0;
  primDesc->index       = 0;
  primDesc->offset      = 0;
  primDesc->off_status  = 0;
  primDesc->full        = FALSE;
  primDesc->sa          = FL_INACTIVE;
  primDesc->sb          = FL_INACTIVE;
}

/*
+------------------------------------------------------------------------------
|  Function    : up_free_prim
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : primDesc
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

LOCAL void up_free_prim(T_P_UPRIM_DESCRIPTOR primDesc)
{
  TRACE_FUNCTION ("up_free_prim()");

  if (primDesc->prim NEQ NULL)
  {
    PFREE (primDesc->prim);
    clear_primDesc(primDesc);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : up_deinit_ribu
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_deinit_ribu(void)
{
  T_UP *dup = &l2r_data->up;

  T_PRIM_DESC_RIBU_INDEX n;

  T_P_UPRIM_DESCRIPTOR primDesc;

  TRACE_FUNCTION ("up_deinit_ribu()");

  for (n = 0; n < dup->RiBu.idx.depth; n++)
  {
    primDesc = dup->RiBu.primDesc[n];
    if (primDesc->prim NEQ NULL)
    {
      up_free_prim(primDesc);
    }
  }
  if (GET_STATE (UP_LL) EQ ISW_SEND)
  {
    SET_STATE (UP_LL, ISW_IDLE);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : up_send_status
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : sa   -
|                sb   -
|                flow -
|                adr  -
|
|  Return      : -
+------------------------------------------------------------------------------
*/



GLOBAL void               up_send_status
                          (
                             T_BIT  sa,
                             T_BIT  sb,
                             T_FLOW flow,
                             UBYTE adr
                          )
{
  T_UP *dup = &l2r_data->up;

  T_P_UPRIM_DESCRIPTOR  primDesc;
  T_P_L2R_FRAME    frame;
  T_BIT x = 0;

  TRACE_FUNCTION ("up_send_status()");

  primDesc = dup->RiBu.primDesc[dup->RiBu.idx.wi];
  frame = (*primDesc->adr)[0];

  switch (flow)
  {
  case FL_ACTIVE:
    x = 1;
    break;
  case FL_INACTIVE:
    x = 0;
    break;
  }
  (*frame)[0] = sa << SO_SA_BIT | sb << SO_SB_BIT | x << SO_X_BIT | adr;

  /* jk:030501 - TEST CASE 251 -
  new funktion parameter 'adr' was introduced
  in order to cancel the older function 'up_send_break(...)'
  */
  primDesc->sa = sa; /* the status has to be saved in order to be correctly stored later */
  primDesc->sb = sb; /* on into "dup->LastSentSa/Sb" [up_send current_prim()] */

  primDesc->index = 1;
  primDesc->offset = 0;

  cl_ribu_write_index(&dup->RiBu.idx); /* point to next primitive */
}

/*
+------------------------------------------------------------------------------
|  Function    : up_set_flow_in_prim
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : prim -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

LOCAL void up_set_flow_in_prim(T_P_RLP_DATA_REQ prim)
{
  T_UP *dup = &l2r_data->up;

  if (dup->MrgFlow EQ FL_INACTIVE)
  {
    TRACE_FUNCTION ("up_set_flow_in_prim(INACTIVE)");
  }
  else
  {
    T_PRIM_INDEX frames;
    T_PRIM_INDEX ind;
    UBYTE off;
    register UBYTE statOct;
    T_P_L2R_FRAME frame;

    TRACE_FUNCTION ("up_set_flow_in_prim(ACTIVE)");

    frames = prim->sdu.l_buf / (8 * dup->FrameSize);
    frame = (T_P_L2R_FRAME) (prim->sdu.buf + (prim->sdu.o_buf / 8) + HEADER_LEN);
    off = 0;
    ind = 0;

    while (ind < frames)
    {
      statOct = (*frame)[off];
      SET_BIT(statOct, SO_X_BIT);
      (*frame)[off] = statOct;

      switch (statOct & SO_ADR_MASK)
      {
      case SO_BREAK_REQ:
      case SO_BREAK_ACK:
      case SO_END_EMPTY:
      case SO_END_FULL:
        ind++;
        frame = (T_P_L2R_FRAME)((UBYTE*)frame + dup->FrameSize);
        off = 0;
        break;

      case SO_TWO_OCTET:
        off++;
        off += ((*frame)[off] & SO_ADR_MASK_TWO_OCT) + 1;
        if (off >= dup->DataSize)
        {
          ind++;
          frame = (T_P_L2R_FRAME)((UBYTE*)frame + dup->FrameSize);
          off = 0;
        }
        break;

      default:
        off += (statOct & SO_ADR_MASK) + 1;
        if (off >= dup->DataSize)
        {
          ind++;
          frame = (T_P_L2R_FRAME)((UBYTE*)frame + dup->FrameSize);
          off = 0;
        }
        break;
      }
    }
  }
  dup->LastSentFlow = dup->MrgFlow;
}

/*
+------------------------------------------------------------------------------
|  Function    : up_send_current_prim
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_send_current_prim(void)
{
  T_UP *dup = &l2r_data->up;
  T_P_UPRIM_DESCRIPTOR primDesc = dup->RiBu.primDesc[dup->RiBu.idx.ri];

  TRACE_FUNCTION ("up_send_current_prim()");

  if (!primDesc->full AND primDesc->index < primDesc->nFr)
  {
    TRACE_FUNCTION ("prim is not full");
    if (primDesc->offset > 0)
    {
      if (up_fill_in_status_octet((T_P_UBYTE)(*primDesc->adr)[primDesc->index], primDesc->off_status, primDesc->offset))
      {
        primDesc->offset++;
        if (primDesc->offset >= dup->DataSize)
        {
          primDesc->offset = 0;
          primDesc->index++;
        }
      }
    }

    if (primDesc->offset > 0 OR primDesc->index EQ 0)
    {
      /*
       * Add status octet, if there is a partly filled frame
       * or if status has changed since last write
       */
      (*(*primDesc->adr)[primDesc->index])[primDesc->offset] =
        dup->LastRcvdSa << SO_SA_BIT |
        dup->LastRcvdSb << SO_SB_BIT |
                      0 << SO_X_BIT  |
        SO_END_EMPTY;
      primDesc->sa = dup->LastRcvdSa;
      primDesc->sb = dup->LastRcvdSb;
      primDesc->index++;
    }
    primDesc->prim->sdu.l_buf = primDesc->index * dup->FrameSize * 8;
  }

  dup->LastSentSa = primDesc->sa;
  dup->LastSentSb = primDesc->sb;
  up_set_flow_in_prim(primDesc->prim);

  PSENDX (RLP, primDesc->prim);

  clear_primDesc(primDesc);     /* this primitive doesn't belong to us any longer */

  cl_ribu_read_index(&dup->RiBu.idx); /* point to next primitive */

  dup->UrgentMsg = FALSE;
  up_check_alloc();
  up_check_flow();
}


/*
+------------------------------------------------------------------------------
|  Function    : up_send_prim_cond
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_send_prim_cond(void)
{
  TRACE_FUNCTION ("up_send_prim_cond()");

  switch (GET_STATE (UP_LL))
  {
  case ISW_WAIT:
    TIMERSTOP (TIMER_TUP_SND);
    up_send_current_prim();
    SET_STATE (UP_LL, ISW_IDLE);
    break;

  default:
    l2r_data->up.UrgentMsg = TRUE;
    up_check_flow();
    up_check_alloc();
    SET_STATE (UP_LL, ISW_SEND);
    break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : up_init_ribu
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void up_init_ribu(void)
{
  T_UP *dup = &l2r_data->up;

  T_PRIM_DESC_RIBU_INDEX n;
  T_PRIM_INDEX m;

  TRACE_FUNCTION ("up_init_ribu()");

  dup->RiBu.alloc = 0;

  cl_ribu_init(&dup->RiBu.idx, dup->RiBu.idx.depth);

  for (n = 0; n < dup->RiBu.idx.depth; n++)
  {
    dup->RiBu.primDesc[n] = &(dup->PrimDesc[n]);

    dup->PrimDesc[n].nFr = 0;
    dup->PrimDesc[n].adr = (T_P_ADR_VECTOR)&(dup->AdrVec[n]);
    dup->PrimDesc[n].index = 0;
    dup->PrimDesc[n].offset = 0;
    dup->PrimDesc[n].off_status = 0;
    dup->PrimDesc[n].full = FALSE;
    dup->PrimDesc[n].sa   = FL_INACTIVE;
    dup->PrimDesc[n].sb   = FL_INACTIVE;
    dup->PrimDesc[n].prim = NULL;

    for(m = 0;m < L2R_FRAMES_PER_PRIM_MAX; m++)
    {
      dup->AdrVec[n][m] = NULL;
    }

    if (dup->RiBu.alloc < UP_RIBU_PREALLOC)
    {
      up_alloc_prim();
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : up_store_status
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : sa
|                sb
|                flow
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void               up_store_status
                          (
                             T_BIT  sa,
                             T_BIT  sb,
                             T_FLOW flow
                          )
{
  T_UP *dup = &l2r_data->up;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("up_store_status()");
#endif

  dup->LastRcvdSa = sa;
  dup->LastRcvdSb = sb;

  if (dup->FlowCtrlUsed EQ FALSE OR flow EQ dup->ULFlow)
  {
    return;
  }

  dup->ULFlow = flow;

  up_merge_flow();
  sig_up_dn_ul_flow(dup->ULFlow);
}

/*
+------------------------------------------------------------------------------
|  Function    : up_send_empty_frame
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : sa   -
|                sb   -
|                flow -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void up_send_empty_frame(T_BIT sa, T_BIT sb, T_FLOW flow)
{
  T_UP *dup = &l2r_data->up;

  T_BIT x = 0;
  USHORT sduSize = dup->FrameSize << 3;

  PALLOC_SDU(data_req, RLP_DATA_REQ, sduSize);

  TRACE_FUNCTION ("up_send_empty_frame()");

#ifdef _SIMULATION_
  /* Clear SDU for test environment */
  {
  UBYTE *p;
  UBYTE *pend;
  p = &data_req->sdu.buf[0];
  pend = p + dup->FrameSize;
  while (p < pend)
  {
    *p++ = 0;
  }
  }
#endif

  data_req->sdu.o_buf = 0;
  data_req->sdu.l_buf = sduSize;

  switch (flow)
  {
  case FL_ACTIVE:
    x = 1;
    break;
  case FL_INACTIVE:
    x = 0;
    break;
  }
  /*lint -e416 (Warning -- Likely creation of out-of-bounds pointer) */
  /*lint -e415 (Warning -- Likely access of out-of-bounds pointer) */
  data_req->sdu.buf[L2R_ENCODING_OFFSET] = sa << SO_SA_BIT | sb << SO_SB_BIT | x << SO_X_BIT | SO_END_EMPTY;
  /*lint +e416 (Warning -- Likely creation of out-of-bounds pointer) */
  /*lint +e415 (Warning -- Likely access of out-of-bounds pointer) */  

  dup->LastSentFlow = flow;
  dup->LastSentSa   = sa;
  dup->LastSentSb   = sb;

  PSENDX (RLP, data_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : up_merge_flow
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void up_merge_flow(void)
{
  T_UP *dup = &l2r_data->up;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("up_merge_flow()");
#endif

  if (dup->FlowCtrlUsed AND (dup->DnFlow EQ FL_ACTIVE OR dup->ULFlow EQ FL_ACTIVE))
  {
    /* flow control active */
    dup->MrgFlow = FL_ACTIVE;
  }
  else
  {
    /* flow control inactive */
    dup->MrgFlow = FL_INACTIVE;
  }

}

/*
+------------------------------------------------------------------------------
|  Function    : up_rq_init
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_rq_init(void)
{
#ifdef _SIMULATION_
  TRACE_FUNCTION ("up_rq_init()");
#endif

  l2r_data->up.QRemapWrite = 0;
  l2r_data->up.QRemapRead  = 0;
}

/*
+------------------------------------------------------------------------------
|  Function    : up_some_data_to_send
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : TRUE  -
|                FALSE -
+------------------------------------------------------------------------------
*/

GLOBAL BOOL up_some_data_to_send(void)
{
  T_UP *dup = &l2r_data->up;

#ifdef _SIMULATION_
  TRACE_EVENT ("check if any data to send");
#endif

  if (
       dup->RiBu.idx.filled OR
      (dup->RiBu.primDesc[dup->RiBu.idx.ri]->index > 0) OR
      (dup->RiBu.primDesc[dup->RiBu.idx.ri]->offset > 0) OR
      (dup->LastRcvdSa NEQ dup->LastSentSa) OR
      (dup->LastRcvdSb NEQ dup->LastSentSb)
    )
  {
    return (TRUE);
  }
  return (FALSE);
}
