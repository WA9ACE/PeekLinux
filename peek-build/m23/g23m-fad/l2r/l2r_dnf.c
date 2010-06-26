/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  L2r_dnf.c
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

#ifndef L2R_DNF_C
#define L2R_DNF_C
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
|  Function    : dn_init
+------------------------------------------------------------------------------
|  Description : initialise the l2r data for the downlink process
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_init(T_DN *ddn)
{
  TRACE_FUNCTION ("dn_init()");

  ddn->FlowCtrlUsed = FALSE;
  ddn->DnFlow = FL_INACTIVE;
  ddn->UpFlow = FL_INACTIVE;
#ifdef L2R_TRACE_FLOW
  ddn->LastSentFlow = FL_INVALID;
#endif
  ddn->FlowThresh = MAX_DPRIM_RIBU_SIZE / 2;
  ddn->LastState = 0 << SO_SA_BIT | 0 << SO_SB_BIT;

  ddn->RiBu.idx.depth = MAX_DPRIM_RIBU_SIZE;
  INIT_STATE (DN_LL, IW_IDLE);
  INIT_STATE (DN_UL, IW_IDLE);
  INIT_STATE (DN, DN_DISCONNECTED);
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_check_flow
+------------------------------------------------------------------------------
|  Description : checks flow control staus
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_check_flow(void)
{
  T_DN *ddn = &l2r_data->dn;

  TRACE_FUNCTION ("dn_check_flow()");

  if (!ddn->FlowCtrlUsed)
  {
    return;
  }

  if (ddn->RiBu.idx.filled >= ddn->FlowThresh)
  {
    switch (ddn->DnFlow)
    {
    case FL_ACTIVE:
      return;

    case FL_INACTIVE:
      ddn->DnFlow = FL_ACTIVE;
      break;
    }
  }
  else
  {
    switch (ddn->DnFlow)
    {
    case FL_ACTIVE:
      ddn->DnFlow = FL_INACTIVE;
      break;

    case FL_INACTIVE:
      return;
    }
  }
  sig_dn_up_flow (ddn->DnFlow);
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_next_frame
+------------------------------------------------------------------------------
|  Description : Local function, which is used by up_copy_data_from_l2r 
|                to advance to the next frame in the primitive.
|                The variable primDesc of the calling function is updated.
|
|  Parameters  : primDesc   -
|
|  Return      : 1  -
|                0  -
+------------------------------------------------------------------------------
*/

LOCAL UBYTE dn_next_frame(T_P_DPRIM_DESCRIPTOR *primDesc)
{
  T_DN *ddn = &l2r_data->dn;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("dn_next_frame()");
#endif

  (*primDesc)->index++; /* next frame */

  if ((*primDesc)->index >= (*primDesc)->nFr)
  {
#ifdef _SIMULATION_
    TRACE_EVENT ("next primitive");
#endif
    cl_ribu_read_index(&ddn->RiBu.idx); /* point to next primitive */

    if (!ddn->RiBu.idx.filled) /* no primitive is ready */
    {
#ifdef _SIMULATION_
      TRACE_EVENT ("no primitive is ready");
#endif
      return (0);
    }

    *primDesc = ddn->RiBu._primDesc[ddn->RiBu.idx.ri]; /* point to next primitive descriptor */
    (*primDesc)->index = 0;
  }
  return (1);
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_copy_data_from_l2r
+------------------------------------------------------------------------------
|  Description : Copies data from l2r into ring buffer.
||               Moreover the return value of the calling function
|                bytesCopied are set

|  Parameters  : buf          -
|                len          -
|                sa           -
|                sb           -
|                flow         -
|                bytesCopied  -
|
|  Return      : -
+------------------------------------------------------------------------------
*/

LOCAL U16 dn_copy_data_from_l2r(U8 *buf, U16 len, U8 *sa, U8 *sb, U8 *flow)
{
  T_DN *ddn = &l2r_data->dn;

  T_P_DPRIM_DESCRIPTOR primDesc;
  T_P_L2R_FRAME frame;
  
  register T_P_UBYTE pFrame;
  register T_P_UBYTE pBuf;
  register T_P_UBYTE pStat;
  
  USHORT bytesToCopy;
  USHORT blocklen;
  UBYTE frameCount;
  UBYTE statOct;

#ifdef _SIMULATION_
  TRACE_EVENT ("dn_copy_data_from_l2r()");
#endif

  ddn->ReportMrgFlow = FALSE;

  if (!ddn->RiBu.idx.filled) /* don't copy into buffer if no primitive is ready */
  {
#ifdef _SIMULATION_
    TRACE_EVENT ("no primitive ready");
#endif
    switch (ddn->MrgFlow)
    {
    case FL_ACTIVE:
      *flow = DTI_FLOW_OFF;
      break;
    case FL_INACTIVE:
      *flow = DTI_FLOW_ON;
      break;
    }
    *sa = GET_SO_SA_BIT(ddn->LastState);
    *sb = GET_SO_SB_BIT(ddn->LastState);
    return 0;
  }

  if (ddn->ULFlow EQ FL_ACTIVE)
  {
    len = 0; /* upper layer has raised flow control; don't send data */
  }

  bytesToCopy = len;
  primDesc = ddn->RiBu._primDesc[ddn->RiBu.idx.ri]; /* point to current primitive descriptor */

  if (primDesc->nFr EQ 0) /* skip empty primitive */
  {
#ifdef _SIMULATION_
    TRACE_EVENT ("empty primitive");
#endif
    switch (ddn->MrgFlow)
    {
    case FL_ACTIVE:
      *flow = DTI_FLOW_OFF;
      break;
    case FL_INACTIVE:
      *flow = DTI_FLOW_ON;
      break;
    }
    *sa = GET_SO_SA_BIT(ddn->LastState);
    *sb = GET_SO_SB_BIT(ddn->LastState);
    return 0;
  }

#ifdef _TARGET_
  if (primDesc->prim->data_size > DATA_SIZE_SHORT)
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

  frame   = (*primDesc->dadr)[primDesc->index];   /* dn_copy_data_from_l2r: point to current l2r frame in primitive */
  pBuf    = buf;                                  /* point to destination buffer */
  pFrame  = &((*frame)[primDesc->offset]);
  pStat   = &((*frame)[primDesc->off_status]);

  if (pFrame EQ pStat)
  {
    /* current byte is status octet */
    ddn->LastState = *pFrame & SO_STATUS_BITS_MASK;
  }

  /* merge flow control conditions */

  switch (ddn->MrgFlow)
  {
  case FL_ACTIVE:
    *flow = DTI_FLOW_OFF;
    break;
  case FL_INACTIVE:
    *flow = DTI_FLOW_ON;
    break;
  }

  *sa = GET_SO_SA_BIT(ddn->LastState);
  *sb = GET_SO_SB_BIT(ddn->LastState);

  /************************************************************************************
   * loop until either
   *   -  no more data are available or
   *   -  status in L2R frame changes or
   *   -  buffer for data is full
   ************************************************************************************/

  for (;;)
  {
    blocklen = pStat - pFrame;

    if (blocklen EQ 0)
    {
      /*
       * current byte is status octet;
       * only in the first pass of the loop, there may be no status octet
       */

      /*****************************
       *  evaluate status bits
       *****************************/
      statOct = *pFrame;
      if (ddn->LastState NEQ (statOct & SO_STATUS_BITS_MASK))
      {
        /*
         *  Status has changed.
         *  We have to stop,
         *  since only one state can be transmitted to the upper layer.
         */
        primDesc->offset = primDesc->off_status = pFrame - (T_P_UBYTE)frame;

#ifdef _SIMULATION_
        TRACE_EVENT ("return because of status change");
#endif
        return len - bytesToCopy;
      }

      pFrame++;

      /************************************
       *  evaluate address bits
       ************************************/
      
      statOct &= SO_ADR_MASK;
      
      switch (statOct)
      {
      case SO_BREAK_ACK:
      case SO_BREAK_REQ:
      case SO_END_EMPTY:
        /*
         * no more data in this frame
         */
        if (dn_next_frame(&primDesc) EQ 0) /* no more data available */
        {
          primDesc->offset = 0;
          primDesc->off_status = 0;

#ifdef _SIMULATION_
          TRACE_EVENT ("return because no more data available");
#endif
          return len - bytesToCopy; /* this much data could be copied */
        }

        frameCount--;

        if (frameCount EQ 0)
        {
          primDesc->offset = 0;
          primDesc->off_status = 0;
#ifdef _SIMULATION_
          TRACE_EVENT ("return because number of frames reached");
#endif
          return len - bytesToCopy; /* this much data could be copied */
        }
        
        frame  = (*primDesc->dadr)[primDesc->index]; /* dn_copy_data_from_l2r */
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

      T_P_UBYTE pEnd = pFrame + bytesToCopy; /* save end mark */

      while (pFrame < pEnd)
      {
        *pBuf++ = *pFrame++;
      }

      if (pFrame >= &((*frame)[primDesc->prim->data_size]))
      {
        /*
         * end of frame reached 
         * actually this case can never occur, 
         * since bytesToCopy < blocklen
        */
        dn_next_frame(&primDesc);
        primDesc->offset = 0;
        primDesc->off_status = 0;
      }
      else
      {
        primDesc->offset = pFrame - (T_P_UBYTE)frame;
        primDesc->off_status = pStat  - (T_P_UBYTE)frame;
      }

#ifdef _SIMULATION_
      TRACE_EVENT ("return because buffer is full");
#endif
      return len; /* this much data could be copied */
    
    }
    else /* bytesToCopy >= blocklen */
    {
      /***************************************
       *  Copy the complete block
       ***************************************/
      
      bytesToCopy -= blocklen;

      while (pFrame < pStat)
      {
        *pBuf++ = *pFrame++;
      }

      if (pFrame >= &((*frame)[primDesc->prim->data_size])) /* end of frame reached */
      {
        if (dn_next_frame(&primDesc) EQ 0) /* no more data available */
        {
#ifdef _SIMULATION_
          TRACE_EVENT ("return because no more data available");
#endif
          return len - bytesToCopy; /* this much data could be copied */
        }
      
        frameCount--;

        if (frameCount EQ 0)
        {
          primDesc->offset = 0;
          primDesc->off_status = 0;

#ifdef _SIMULATION_
          TRACE_EVENT ("return because number of frames reached");
#endif
          return len - bytesToCopy; /* this much data could be copied */
        }
        
        frame  = (*primDesc->dadr)[primDesc->index]; /* dn_copy_data_from_l2r */
        pFrame = (T_P_UBYTE)frame;
        pStat  = (T_P_UBYTE)frame;
      }
      
      if (bytesToCopy EQ 0)
      {
        primDesc->offset = pFrame - (T_P_UBYTE)frame;
        primDesc->off_status = pStat - (T_P_UBYTE)frame;

#ifdef _SIMULATION_
        TRACE_EVENT ("return because all data are copied");
#endif
        return len; /* this much data could be copied */
      }

    } /* bytesToCopy >= blocklen */
  } /* for (;;) */
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_send_data_ind
+------------------------------------------------------------------------------
|  Description : This procedure copies data from the downlink ring buffer 
|                into a DTI_DATA_IND primitive
|                and sends this primitive to the relay entity.
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_send_data_ind(void)
{
  T_DN *ddn = &l2r_data->dn;

  TRACE_FUNCTION ("dn_send_data_ind()");

  if (ddn->DtiConnected EQ FALSE)
  {
    TRACE_EVENT("DTI not connected, but dn_send_data_ind() called");
    return;
  }

  {
  USHORT len = L2R_FRAMES_PER_PRIM_MAX * (RLP_FRAME_SIZE_SHORT - HT_LEN - 1);
  UBYTE sa;
  UBYTE sb;
  UBYTE flow;
  T_desc2* desc;

  PALLOC (dti_data_ind, DTI2_DATA_IND);

  MALLOC (desc, (USHORT)(sizeof(T_desc2) - 1 + len));
  desc->len  = dn_copy_data_from_l2r ((U8*)&desc->buffer[0], len, &sa, &sb, &flow);
  desc->size = desc->len;
  desc->offset = 0;
  desc->next = 0;

  dti_data_ind->desc_list2.first = (ULONG)desc;
  dti_data_ind->desc_list2.list_len = desc->len;
  
  dti_data_ind->parameters.st_lines.st_line_sa    = sa;
  dti_data_ind->parameters.st_lines.st_line_sb    = sb;
  dti_data_ind->parameters.st_lines.st_flow       = flow;
  dti_data_ind->parameters.st_lines.st_break_len  = DTI_BREAK_OFF;
  dti_data_ind->parameters.p_id                   = DTI_PID_UOS;
  dti_data_ind->link_id                           = ddn->link_id;

#ifdef L2R_TRACE_FLOW
  if (ddn->LastSentFlow NEQ dti_data_ind->parameters.st_lines.st_flow)
  {
    switch (dti_data_ind->parameters.st_lines.st_flow)
    {
    case FL_ACTIVE:
      TRACE_EVENT("DTI downlink: FL_ACTIVE");
      break;

    case FL_INACTIVE:
      TRACE_EVENT("DTI downlink: FL_INACTIVE");
      break;
    }
    ddn->LastSentFlow = dti_data_ind->parameters.st_lines.st_flow;
  }
#endif

  dti_send_data (
    l2r_hDTI,
    L2R_DTI_UP_DEF_INSTANCE,
    L2R_DTI_UP_INTERFACE,
    L2R_DTI_UP_CHANNEL,
    dti_data_ind
    );        
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_scan_break_req
+------------------------------------------------------------------------------
|  Description : This procedure scans a rlp_data_ind primitive for L2R BREAK
|                status octets. It returnes the index of the frame following
|                the last BREAK status octet in the primitive.
|                Moreover the total number of frames in the primitive 
|                as well as the status bits of the last BREAK are returned.
|                In addition the x bit of the last status octet is returned.
|                It is important to search the last BREAK in the primitive,
|                because the data following a break signal
|                are used in the case of no data compression.
|
|  Parameters  : data_ind -
|                found
|                index
|                frames
|                emptyfr
|                sa
|                sb
|                flow_brk
|                flow_gen
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_scan_break_req
            (
              T_P_RLP_DATA_IND  data_ind,
              BOOL             *found,
              T_PRIM_INDEX     *index,
              T_PRIM_INDEX     *frames,
              T_PRIM_INDEX     *emptyfr,
              T_BIT            *sa,
              T_BIT            *sb,
              T_FLOW           *flow_brk,
              T_FLOW           *flow_gen
            )

{
  T_PRIM_INDEX ind;
  UBYTE off;
  UBYTE statOct;
  UBYTE brkStatOct = (UBYTE)(0 << SO_SA_BIT | 0 << SO_SB_BIT | 0 << SO_X_BIT);
  UBYTE genStatOct = (UBYTE)(0 << SO_SA_BIT | 0 << SO_SB_BIT | 0 << SO_X_BIT);

  T_P_L2R_FRAME frame;

  TRACE_FUNCTION ("dn_scan_break_req()");

  *found   = FALSE;
  *frames  = data_ind->sdu.l_buf / (8 * data_ind->data_size + HT_LEN);
  *emptyfr = 0;

  frame = (T_P_L2R_FRAME)(data_ind->sdu.buf + (data_ind->sdu.o_buf>>3) + HEADER_LEN);
  off = 0;
  ind = 0;

  while (ind < *frames)
  {
    statOct = (*frame)[off];

    switch (statOct & SO_ADR_MASK)
    {
    case SO_BREAK_REQ:
      *found  = TRUE;
      *index  = ind + 1;
      brkStatOct = statOct;
      genStatOct = statOct;
      ind++;
      frame = (T_P_L2R_FRAME)((UBYTE*)frame + data_ind->data_size + HT_LEN);
      off = 0;
      break;

    case SO_END_EMPTY:
      if (off EQ 0)
      {
        (*emptyfr)++;
      }
      /* fall through!!! */

    case SO_BREAK_ACK:
    case SO_END_FULL:
      genStatOct = statOct;
      ind++;
      frame = (T_P_L2R_FRAME)((UBYTE*)frame + data_ind->data_size + HT_LEN);
      off = 0;
      break;

    case SO_TWO_OCTET:
      genStatOct = statOct;
      off += ((*frame)[off] & SO_ADR_MASK_TWO_OCT) + 2;
      if (off >= data_ind->data_size)
      {
        ind++;
        frame = (T_P_L2R_FRAME)((UBYTE*)frame + data_ind->data_size + HT_LEN);
        off = 0;
      }
      break;

    default:
      genStatOct = statOct;
      off += (statOct & SO_ADR_MASK) + 1;
      if (off >= data_ind->data_size)
      {
        ind++;
        frame = (T_P_L2R_FRAME)((UBYTE*)frame + data_ind->data_size + HT_LEN);
        off = 0;
      }
      break;
    }
  }

  if (*found)
  {
    *sa    = GET_SO_SA_BIT(brkStatOct);
    *sb    = GET_SO_SB_BIT(brkStatOct);
  }
  else
  {
    *sa    = GET_SO_SA_BIT(genStatOct);
    *sb    = GET_SO_SB_BIT(genStatOct);
  }

  if (l2r_data->dn.FlowCtrlUsed)
  {
    if (GET_SO_X_BIT (brkStatOct) EQ 0)
    {
      *flow_brk = FL_INACTIVE;
    }
    else
    {
      *flow_brk = FL_ACTIVE;
    }
    
    if (GET_SO_X_BIT (genStatOct) EQ 0)
    {
      *flow_gen = FL_INACTIVE;
    }
    else
    {
      *flow_gen = FL_ACTIVE;
    }
  }
  else
  {
    *flow_brk = FL_INACTIVE;
    *flow_gen = FL_INACTIVE;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_free_prim
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : primDesc -
|
|
|  Return      :
+------------------------------------------------------------------------------
*/

LOCAL void dn_free_prim(T_P_DPRIM_DESCRIPTOR primDesc)
{
  TRACE_FUNCTION ("dn_free_prim()");

  if (primDesc->prim NEQ NULL)
  {
    PFREE (primDesc->prim);
    primDesc->prim = NULL;
    primDesc->nFr = 0;
    primDesc->index = 0;
    primDesc->offset = 0;
    primDesc->off_status = 0;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_free_all_prims
+------------------------------------------------------------------------------
|  Description : frees all primititives
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_free_all_prims(void)
{
  T_DN *ddn = &l2r_data->dn;

  T_PRIM_DESC_RIBU_INDEX n;

  TRACE_FUNCTION ("dn_free_all_prims()");

  for (n = 0; n < ddn->RiBu.idx.depth; n++)
  {
    dn_free_prim (ddn->RiBu._primDesc[n]);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_cond_free_prims
+------------------------------------------------------------------------------
|  Description : frees all primitive if r
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_cond_free_prims(void)
{
  T_DN *ddn = &l2r_data->dn;

  TRACE_FUNCTION ("dn_cond_free_prims()");

  while (ddn->RiBu.free NEQ ddn->RiBu.idx.ri)
  {
    dn_free_prim (ddn->RiBu._primDesc[ddn->RiBu.free]);

    ddn->RiBu.free++;
    if (ddn->RiBu.free EQ ddn->RiBu.idx.depth)
    {
      ddn->RiBu.free = 0;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_store_prim
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : data_ind -
|                index    -
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_store_prim(T_P_RLP_DATA_IND data_ind, T_PRIM_INDEX index)
{
  T_DN *ddn = &l2r_data->dn;

  T_PRIM_INDEX m;
  T_P_DPRIM_DESCRIPTOR primDesc;

  T_PRIM_INDEX frames = data_ind->sdu.l_buf / (8 * data_ind->data_size + HT_LEN);
  UBYTE *pos = data_ind->sdu.buf + (data_ind->sdu.o_buf>>3) + HEADER_LEN;

  TRACE_FUNCTION ("dn_store_prim()");

  primDesc = ddn->RiBu._primDesc[cl_ribu_write_index(&ddn->RiBu.idx)];

  primDesc->prim = data_ind;
  primDesc->index = index;    /* if BREAK then not equal 0 */
  primDesc->offset = 0;
  primDesc->off_status = 0;
  primDesc->nFr = frames;

  for (m = 0; m < frames; m++)
  {
    (*primDesc->dadr)[m] = (T_P_L2R_FRAME)(pos + m * (data_ind->data_size + HT_LEN));
  }

  dn_check_flow();

  if (GET_STATE (DN_UL) EQ IW_WAIT AND ddn->ULFlow EQ FL_INACTIVE AND
      ddn->DtiConnected) /*jk: data send only when DTI connected*/
  {
    dn_send_data_ind();
    SET_STATE (DN_UL, IW_IDLE);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : L2R_DNF             |
| STATE   : code                       ROUTINE : dn_init_ribu        |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void dn_init_ribu(void)
{
  T_DN *ddn = &l2r_data->dn;

  T_PRIM_DESC_RIBU_INDEX n;
  T_PRIM_INDEX m;

  TRACE_FUNCTION ("dn_init_ribu()");

  cl_ribu_init(&ddn->RiBu.idx, ddn->RiBu.idx.depth);
  ddn->RiBu.free = 0;

  for (n = 0; n < ddn->RiBu.idx.depth; n++)
  {
    ddn->RiBu._primDesc[n]      = &(ddn->PrimDesc[n]);
    ddn->PrimDesc[n].nFr        = 0;
    ddn->PrimDesc[n].dadr       = (T_P_ADR_VECTOR)&(ddn->AdrVec[n]); /* dn_init_ribu */
    ddn->PrimDesc[n].index      = 0;
    ddn->PrimDesc[n].offset     = 0;
    ddn->PrimDesc[n].off_status = 0;
    ddn->PrimDesc[n].prim       = NULL;

    for (m = 0; m < L2R_FRAMES_PER_PRIM_MAX; m++)
    {
      ddn->AdrVec[n][m] = NULL;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_cond_req_data
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_cond_req_data(void)
{
  T_DN *ddn = &l2r_data->dn;

  TRACE_FUNCTION ("dn_cond_req_data()");

  /* ring buffer full? */

  if ((ddn->RiBu.idx.wi + 1) % ddn->RiBu.idx.depth EQ ddn->RiBu.free)
  {
    SET_STATE (DN_LL, IW_IDLE);
  }
  else
  {
    PALLOC (rlp_getdata_req, RLP_GETDATA_REQ);
    PSENDX (RLP, rlp_getdata_req);
    SET_STATE (DN_LL, IW_WAIT);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_store_status
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : flow -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_store_status(T_FLOW flow)
{
  T_DN *ddn = &l2r_data->dn;

  TRACE_FUNCTION ("dn_store_status()");

  if (flow EQ ddn->LLFlow)
  {
    return;
  }

  ddn->LLFlow = flow;
  dn_merge_flow();
  sig_dn_up_ll_flow(ddn->LLFlow);
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_merge_flow
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_merge_flow(void)
{
  T_DN *ddn = &l2r_data->dn;

  TRACE_FUNCTION ("dn_merge_flow()");

  if (ddn->FlowCtrlUsed AND (ddn->UpFlow EQ FL_ACTIVE OR ddn->LLFlow EQ FL_ACTIVE)     )
  {
    if (ddn->MrgFlow EQ FL_INACTIVE)
    {
      ddn->ReportMrgFlow = TRUE;
      /*
      TRACE_EVENT("DN: Merged flow set active");
      */
    }
    ddn->MrgFlow = FL_ACTIVE;
  }
  else
  {
    if (ddn->MrgFlow EQ FL_ACTIVE)
    {
      ddn->ReportMrgFlow = TRUE;
      /*
      TRACE_EVENT("DN: Merged flow set inactive");
      */
    }
    ddn->MrgFlow = FL_INACTIVE;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : dn_cond_report_status
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dn_cond_report_status(void)
{
  TRACE_FUNCTION ("dn_cond_report_status()");

  if (l2r_data->dn.ReportMrgFlow AND GET_STATE (DN_UL) EQ IW_WAIT)
  {
    if (l2r_data->dn.DtiConnected) /*jk: data send only when DTI connected*/
    {
      dn_send_data_ind();
      SET_STATE (DN_UL, IW_IDLE);
    }
  }
}

