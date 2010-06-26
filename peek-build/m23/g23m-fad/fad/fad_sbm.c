/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_SBM
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
|             the component Fax Adaptation 3.45 of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef FAD_SBM_C
#define FAD_SBM_C
#endif

#define ENTITY_FAD

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"
#include "cnf_fad.h"
#include "mon_fad.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "ccdapi.h"
#include "cl_ribu.h"
#include "fad.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/
 
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SBM             |
| STATE   : code                       ROUTINE : sbm_init            |
+--------------------------------------------------------------------+

  PURPOSE : 

*/
GLOBAL void sbm_init(USHORT frameSize)
{
  T_SBM *sbm = &fad_data->sbm;
  USHORT n;

  TRACE_FUNCTION ("sbm_init()");

  cl_ribu_init(&sbm->ribu, SBM_PRIMBUF_SIZE);

  for (n = 0; n < sbm->ribu.depth; n++)
    sbm->Prim[n].Prim = NULL;

  sbm->PA = 0;
  sbm->CurrPrimOff = 0;
  sbm->CurrPrimLen = 0;
  sbm->FrameSize   = frameSize;

  fad_data->snd.FD.buf = fad_data->snd.FD_buf;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SBM             |
| STATE   : code                       ROUTINE : sbm_store_prim      |
+--------------------------------------------------------------------+

  PURPOSE : 

*/

GLOBAL void sbm_store_prim(T_SBM_PRIM *sendPrim)
{
  T_SBM *sbm = &fad_data->sbm;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("sbm_store_prim()");
#endif

  sbm->Prim[sbm->ribu.wi].Prim = sendPrim;

  if (!sbm->ribu.filled) /* Initialize new primitive for read out */
  {
    sbm->CurrPrimOff = sendPrim->sdu.o_buf >> 3;
    sbm->CurrPrimLen = sendPrim->sdu.l_buf >> 3;
  }
  cl_ribu_write_index(&sbm->ribu);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SBM             |
| STATE   : code                       ROUTINE : sbm_free_empty_prims|
+--------------------------------------------------------------------+

  PURPOSE : 

*/

GLOBAL void sbm_free_empty_prims(void)
{
  T_SBM *sbm = &fad_data->sbm;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("sbm_free_empty_prims()");
#endif
  
  while (sbm->PA NEQ sbm->ribu.ri)
  {
    PFREE (sbm->Prim[sbm->PA].Prim);
    sbm->PA++;
    if (sbm->PA EQ sbm->ribu.depth)
      sbm->PA = 0;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SBM             |
| STATE   : code                       ROUTINE : sbm_get_frame       |
+--------------------------------------------------------------------+

  PURPOSE : 

*/

GLOBAL BOOL sbm_get_frame(T_FRAME_DESC *ul_FD, UBYTE reqFrames)
{
  T_SBM *sbm = &fad_data->sbm;

  USHORT requiredData;
  BOOL   insertSync;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("sbm_get_frame()");
#endif

  cl_set_frame_desc_0(ul_FD, NULL, 0);

  if (!sbm->ribu.filled) /* There is no primitive */
  {
    return FALSE;
  }

  insertSync = FALSE;

  if (sbm->syncCycle NEQ SYNC_CYCLE_NONE)
  {
    if (reqFrames > sbm->framesUntilSync)
    {
      insertSync = TRUE;
      reqFrames--;
    }
    else
      sbm->framesUntilSync -= reqFrames;
  }

  requiredData = sbm->FrameSize * (USHORT)reqFrames;

  if (sbm->CurrPrimLen > requiredData)
  {
    /* Current primitive holds more than enough data */
    cl_set_frame_desc_0(ul_FD, sbm->Prim[sbm->ribu.ri].Prim->sdu.buf + sbm->CurrPrimOff, requiredData);

    sbm->CurrPrimOff += requiredData;
    sbm->CurrPrimLen -= requiredData;
  }
  else
  {
    /* 
     * Current primitive doesn't hold enough data
     * or just the right amount of data.
     * -> Switch to next primitive
     */

    /* Take all data from first primitive */
    cl_set_frame_desc_0(ul_FD, sbm->Prim[sbm->ribu.ri].Prim->sdu.buf + sbm->CurrPrimOff, sbm->CurrPrimLen);

    cl_ribu_read_index(&sbm->ribu); /* Switch to next primitive */

    if (!sbm->ribu.filled)          /* There is no second primitive */
    {
      ul_FD->Adr[1] = (UBYTE *) NULL;
      ul_FD->Len[1] = 0;
      sbm->CurrPrimOff = 0;
      sbm->CurrPrimLen = 0;
    }
    else                            /* Second primitive is available */
    {
      requiredData -= sbm->CurrPrimLen;

      sbm->CurrPrimOff = sbm->Prim[sbm->ribu.ri].Prim->sdu.o_buf >> 3;
      sbm->CurrPrimLen = sbm->Prim[sbm->ribu.ri].Prim->sdu.l_buf >> 3;

      if (requiredData EQ 0)
      {
        /*
         * First primitive was just used up.
         * We had to switch to next primitive, but don't need data.
         */
        ul_FD->Adr[1] = (UBYTE *) NULL;
        ul_FD->Len[1] = 0;
      }
      else
      {
        /* Data from next primitive is required */
        ul_FD->Adr[1] = sbm->Prim[sbm->ribu.ri].Prim->sdu.buf + sbm->CurrPrimOff;

        if (sbm->CurrPrimLen > requiredData)
        {
          /* Enough data found in second primitive */
          ul_FD->Len[1] = requiredData;
          sbm->CurrPrimOff += requiredData;
          sbm->CurrPrimLen -= requiredData;
        }
        else
        {
          /* Even with the second primitive there are not enough data! */
          ul_FD->Len[1] = sbm->CurrPrimLen;

          /* Second primitive is also emptied, switch to next primitve */
          cl_ribu_read_index(&sbm->ribu);

          if (sbm->ribu.filled)
          {
            sbm->CurrPrimOff = sbm->Prim[sbm->ribu.ri].Prim->sdu.o_buf >> 3;
            sbm->CurrPrimLen = sbm->Prim[sbm->ribu.ri].Prim->sdu.l_buf >> 3;

          }
          else
          {
            sbm->CurrPrimOff = 0;
            sbm->CurrPrimLen = 0;
          }
        }
      }
    }
  }

  if ((ul_FD->Len[0]+ul_FD->Len[1]) < requiredData)
  {
    /*
     * If there are not enough data available align the length to 64 bit 
     * align the length to 64 bit boundaries.
     * This alignment should normally occur at the end of the message phase.
     * The required data are always a multiple of 64 bits.
     */
    int i, k = 0;
    int diff = requiredData - (ul_FD->Len[0]+ul_FD->Len[1]);

    if (ul_FD->Len[1] > 0)
      k = 1;

    for (i = 0; i < ul_FD->Len[k]; i++)
      fad_data->snd.ul_FD_align[i] = ul_FD->Adr[k][i];

    for (i = ul_FD->Len[k]; i < ul_FD->Len[k] + diff; i++)
      fad_data->snd.ul_FD_align[i] = 0;

    ul_FD->Len[k] += diff;
    ul_FD->Adr[k] = fad_data->snd.ul_FD_align;
  }

  if (insertSync)
  {
    USHORT framesUntilSync = sbm->framesUntilSync;
    USHORT frame_off = 0, i, j;

    for (i=0; i<2; i++)
    {
      for (j=0; j<ul_FD->Len[i]; j+=FRAME_SIZE)
      {
        if (framesUntilSync-- EQ 0)
        {
          memcpy((char*)&sbm->frames[frame_off], (char*)FR_SEQ_SYNC, FRAME_SIZE);
          frame_off+=FRAME_SIZE;
        }
        memcpy((char*)&sbm->frames[frame_off], (char*)ul_FD->Adr[i], FRAME_SIZE);
        frame_off+=FRAME_SIZE;

        sbm->framesUntilSync = sbm->syncCycle - reqFrames + framesUntilSync;
      }
    }
    cl_set_frame_desc_0(ul_FD, sbm->frames, (USHORT)((reqFrames+1)*FRAME_SIZE));
  }
  return TRUE;
}

