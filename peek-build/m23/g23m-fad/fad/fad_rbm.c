/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_RBM
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
|  Purpose :  This Modul defines the receive buffer manager for
|             the component Fax Adaptation 3.45 of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef FAD_RBM_C
#define FAD_RBM_C
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
#include "fad.h"
#include "ra_l1int.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)            MODULE  : FAD_RBM              |
| STATE   : code                      ROUTINE : rbmi_alloc_curr_prim |
+--------------------------------------------------------------------+

  PURPOSE : 

*/

LOCAL BOOL rbmi_alloc_curr_prim(void)
{
  T_RBM_FAD *rbm = &fad_data->rbm;

  PALLOC_SDU (fad_data_ind, FAD_DATA_IND, (USHORT)(rbm->FramesPerPrim * FRAME_SIZE * 8));

  TRACE_FUNCTION ("rbmi_alloc_curr_prim()");

  fad_data_ind->sdu.l_buf = 0;
  fad_data_ind->sdu.o_buf = 0;

  rbm->CurrPrim         = fad_data_ind;
  rbm->FramesInCurrPrim = 0;

  return (rbm->CurrPrim NEQ NULL);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RBM             |
| STATE   : code                       ROUTINE : rbm_init            |
+--------------------------------------------------------------------+

  PURPOSE : 

*/

GLOBAL void rbm_init(USHORT framesPerPrim)
{
  T_RBM_FAD *rbm = &fad_data->rbm;

  TRACE_FUNCTION ("rbm_init()");

  cl_ribu_create(&rbm->ribu, MAX_SDU_SIZE, DL_RIBU_DEPTH);
  rbm->FramesPerPrim = framesPerPrim;
  rbm_deinit(FALSE);
  
  if (rbmi_alloc_curr_prim())
    rbm->Initialised = TRUE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RBM             |
| STATE   : code                       ROUTINE : rbm_deinit          |
+--------------------------------------------------------------------+

  PURPOSE : This function frees all allocated T_FAD_DATA_IND primitives
            of the receive buffer manager.

*/

GLOBAL void rbm_deinit(BOOL final)
{
  T_RBM_FAD *rbm = &fad_data->rbm;
  U8 n;

  TRACE_FUNCTION ("rbm_deinit()");

  rbm->Initialised = FALSE;

  if (rbm->CurrPrim NEQ (T_FAD_DATA_IND *)NULL)
  {
    PFREE (rbm->CurrPrim)
    rbm->CurrPrim = (T_FAD_DATA_IND *) NULL;
  }

  for (n = 0; n < RBM_PQ_SIZE; n++)
  {
    if (rbm->PQ_Array[rbm->PQ.ri] NEQ (T_FAD_DATA_IND *)NULL)
    {
      PFREE (rbm->PQ_Array[rbm->PQ.ri])
      rbm->PQ_Array[rbm->PQ.ri] = (T_FAD_DATA_IND *)NULL;
    }
  }
  cl_ribu_init(&rbm->PQ, RBM_PQ_SIZE);
  rbm->FramesInCurrPrim = 0;

  if (final EQ TRUE) /* during deactivation */
  {
    cl_ribu_release(&rbm->ribu); /* frees downlink FIFO */
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RBM             |
| STATE   : code                       ROUTINE : rbmi_pq_enq         |
+--------------------------------------------------------------------+

  PURPOSE : A primitive is put into the primitive queue.

*/

LOCAL BOOL rbmi_pq_enq(T_FAD_DATA_IND *prim)
{
  T_RBM_FAD *rbm = &fad_data->rbm;

  TRACE_FUNCTION ("rbmi_pq_enq()");
  
  if (rbm->PQ_Array[rbm->PQ.wi] EQ NULL)
  {
    /*
     * empty PQ entry found
     */
    prim->sdu.l_buf = rbm->FramesPerPrim * FRAME_SIZE * 8;

    rbm->PQ_Array[cl_ribu_write_index(&rbm->PQ)] = prim;
    return TRUE;
  }
  else
    return FALSE; /* no available entry in PQ */
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)    MODULE  : FAD_RBM                      |
| STATE   : code              ROUTINE : rbmi_copy_frame_to_curr_prim |
+--------------------------------------------------------------------+

  PURPOSE : 

*/

LOCAL BOOL rbmi_copy_frame_to_curr_prim(UBYTE *frame)
{
  T_RBM_FAD *rbm = &fad_data->rbm;
    
  TRACE_FUNCTION ("rbmi_copy_frame_to_curr_prim()");

  if (rbm->CurrPrim EQ NULL)
    if (!rbmi_alloc_curr_prim())
      return FALSE;

  if (rbm->FramesInCurrPrim < rbm->FramesPerPrim)
  {
    memcpy(&rbm->CurrPrim->sdu.buf[rbm->FramesInCurrPrim * FRAME_SIZE], frame, FRAME_SIZE);
    rbm->FramesInCurrPrim++;
  }

  if (rbm->FramesInCurrPrim >= rbm->FramesPerPrim)
  {
    /*
     * Current primitive is full.
     * Store it in the PQ and allocate a new current prim
     */
    if (rbmi_pq_enq(rbm->CurrPrim))
    {
      /*
       * Current prim is in PQ -> allocate a new current prim
       */
      return rbmi_alloc_curr_prim();
    }
    else
    {
      return FALSE;
    }
  }
  return TRUE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RBM             |
| STATE   : code                       ROUTINE : rbmi_pq_deq         |
+--------------------------------------------------------------------+

  PURPOSE : A fad_data_ind primitive is taken from the queue. This
            is done when the queue is not empty and a FAD_GETDATA_REQ
            is executed. Of after storing a primitive in the queue,
            when the upper layer is waiting for data.

*/

LOCAL T_FAD_DATA_IND *rbmi_pq_deq(void)
{
  T_RBM_FAD *rbm = &fad_data->rbm;
  T_FAD_DATA_IND *prim = rbm->PQ_Array[rbm->PQ.ri];
  
  TRACE_FUNCTION ("rbmi_pq_deq()");

  if (prim NEQ NULL)
  {
    rbm->PQ_Array[cl_ribu_read_index(&rbm->PQ)] = NULL;
  }
  return prim;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RBM             |
| STATE   : code                       ROUTINE : rbm_reset           |
+--------------------------------------------------------------------+

  PURPOSE : This function must be called once at startup before any
            other procedures of the RBM is invoked. (Coldstart)

*/

GLOBAL void rbm_reset(void)
{
  T_RBM_FAD *rbm = &fad_data->rbm;

  TRACE_FUNCTION ("rbm_reset()");

  rbm->CurrPrim         = (T_FAD_DATA_IND *) NULL;
  rbm->FramesInCurrPrim = 0;
  cl_ribu_init(&rbm->PQ, RBM_PQ_SIZE);
  rbm->FramesPerPrim    = 1;
  rbm->Initialised      = FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RBM             |
| STATE   : code                       ROUTINE : rbm_store_frames    |
+--------------------------------------------------------------------+

  PURPOSE : This procedure is only used in the simulation environment
            to store multiple frames in the receive buffer.
            In the implementation the copy is performed by the RA layer
            which have access to the shared memory area of the DSP.
*/

GLOBAL BOOL rbm_store_frames(T_FD *pFD, BOOL *primIsReady)
{
  T_RBM_FAD *rbm = &fad_data->rbm;

  if (rbm->Initialised)
  {
    USHORT n;
    BOOL flag = FALSE;

    for (n = 0; n < pFD->len/FRAME_SIZE; n++)
    {
      if (!rbmi_copy_frame_to_curr_prim(&pFD->buf[n * FRAME_SIZE]))
      {
		flag = TRUE;
		break;
      }
    }
    if (flag)
	  return FALSE;
  }
  *primIsReady = rbm->PQ.filled;
  return TRUE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RBM             |
| STATE   : code                       ROUTINE : rbm_get_prim        |
+--------------------------------------------------------------------+

  PURPOSE : This is just a wrapper for rbmi_pq_deq. The queu is an
            internal matter of the RBM, therefore the function
            rbmi_pq_deq should not be used directly by the user.

*/

GLOBAL T_FAD_DATA_IND *rbm_get_prim(void)
{
  TRACE_FUNCTION ("rbm_get_prim()");

  return rbmi_pq_deq();
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RBM             |
| STATE   : code                       ROUTINE : rbm_get_curr_prim   |
+--------------------------------------------------------------------+

  PURPOSE : 

*/

GLOBAL T_FAD_DATA_IND *rbm_get_curr_prim(void)
{
  T_RBM_FAD *rbm = &fad_data->rbm;
  T_FAD_DATA_IND *prim;

  TRACE_FUNCTION ("rbm_get_curr_prim()");

  prim = rbm->CurrPrim;
  if (prim NEQ NULL)
  {
    rbm->CurrPrim->sdu.l_buf = rbm->FramesInCurrPrim * FRAME_SIZE * 8;
    rbm->CurrPrim = (T_FAD_DATA_IND *) NULL;
  }
  return prim;
}

