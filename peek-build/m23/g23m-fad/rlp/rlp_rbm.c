/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  Rlp_rbm.c
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
|             the component Radio Link Protocol of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef RLP_RBM_C
#define RLP_RBM_C
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
|  Function    : rbmi_alloc_curr_prim
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void rbmi_alloc_curr_prim
           (
             void
           )
{
  USHORT sduSize = rlp_data->rbm.FramesPerPrim
                 * rlp_data->rbm.FrameSize * 8;

  PALLOC_SDU
  (
    rlp_data_ind,
    RLP_DATA_IND,
    sduSize
  );

  TRACE_FUNCTION ("rbmi_alloc_curr_prim()");

  rlp_data_ind->sdu.l_buf
    = rlp_data->rbm.FramesPerPrim * rlp_data->rbm.FrameSize * 8;

  rlp_data_ind->data_size = rlp_data->rbm.FrameSize - HEADER_LEN - TRAILER_LEN;

  rlp_data->rbm.CurrPrim    = rlp_data_ind;
  rlp_data->rbm.CurrPrimCou = 0;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbmi_pq_enq
+------------------------------------------------------------------------------
|  Description : A primitive is put into the primitive queue. This is done
|                in rbm_buffer_all_in_sequence_frames when the current
|                primitive is filled up.
|
|
|  Parameters  : prim -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void rbmi_pq_enq
           (
             T_RBM_PRIM *prim
           )
{
  TRACE_FUNCTION ("rbmi_pq_enq()");

  rlp_data->rbm.PQ_Array[rlp_data->rbm.PQ_Write] = prim;

  rlp_data->rbm.PQ_Write = (rlp_data->rbm.PQ_Write+1) % RBM_PQ_SIZE;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbmi_copy_frame_to_sdu
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : frame -
|                sdu   -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void rbmi_copy_frame_to_sdu(T_RBM_FRAMEPTR frame, T_sdu *sdu)
{
  TRACE_FUNCTION ("rbmi_copy_frame_to_sdu()");

  memcpy(sdu->buf+(sdu->o_buf>>3), frame, rlp_data->rbm.FrameSize);

  sdu->o_buf += rlp_data->rbm.FrameSize * 8;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbmi_buffer_frame
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : fromSlot     -
|                primIsReady  -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void rbmi_buffer_frame
           (
             T_RBM_BUFFER_EXTIDX  fromSlot,
             BOOL                *primIsReady
           )
{
  TRACE_FUNCTION ("rbmi_buffer_frame()");

  rbmi_copy_frame_to_sdu
  (
    rlp_data->rbm.Slot[fromSlot].Frame,
    &rlp_data->rbm.CurrPrim->sdu
  );

  if (++rlp_data->rbm.CurrPrimCou EQ rlp_data->rbm.FramesPerPrim)
  {
    rlp_data->rbm.CurrPrim->sdu.o_buf = 0;
    rbmi_pq_enq (rlp_data->rbm.CurrPrim);
    rbmi_alloc_curr_prim ();
    *primIsReady = TRUE;
  }
}


/*
+------------------------------------------------------------------------------
|  Function    : rbmi_pq_deq
+------------------------------------------------------------------------------
|  Description :  A rlp_data_ind primitive is taken from the queue. This
|                 is done when the queue is not empty and a RLP_GETDATA_REQ
|                 is executed. Of after storing a primitive in the queue,
|                 when the upper layer is waiting for data.
|
|  Parameters  : prim  -
|
|
|  Return      : TRUE  -
|                FALSE -
+------------------------------------------------------------------------------
*/


LOCAL BOOL rbmi_pq_deq
           (
             T_RBM_PRIM **prim
           )
{
  TRACE_FUNCTION ("rbmi_pq_deq()");

  if (rlp_data->rbm.PQ_Read NEQ rlp_data->rbm.PQ_Write)
  {
    *prim = rlp_data->rbm.PQ_Array[rlp_data->rbm.PQ_Read];
    rlp_data->rbm.PQ_Array[rlp_data->rbm.PQ_Read] = (T_RBM_PRIM *) NULL;

    rlp_data->rbm.PQ_Read = (rlp_data->rbm.PQ_Read+1) % RBM_PQ_SIZE;
    return TRUE;
  }
  else
  {
    /*
     * prim buffer is empty
     */
    return FALSE;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : rbmi_pq_check
+------------------------------------------------------------------------------
|  Description : The prim queue is checked.
|                rcvReady indicates that there is still more than the
|                minimum space in the queue.
|                rcvFull  indicates that the queue is completeley full.
|
|
|  Parameters  : rcvReady -
|                rcvFull  -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void rbmi_pq_check
           (
             BOOL *rcvReady,
             BOOL *rcvFull
           )
{
  T_RBM_PQ_INDEX writeMax, writeThresh;

  TRACE_FUNCTION ("rbmi_pq_check()");

  writeMax    = (rlp_data->rbm.PQ_Read + RBM_PQ_MAX_PRIM)
              % RBM_PQ_SIZE;

  writeThresh = (rlp_data->rbm.PQ_Read + RBM_PQ_THRE_RNR)
              % RBM_PQ_SIZE;

  /*
   * prim queue filled upto threshold?
   */

  if (!(XOR
        (
          XOR
          (
            writeThresh > rlp_data->rbm.PQ_Write,
            rlp_data->rbm.PQ_Write > writeMax
          ),
          writeThresh > writeMax
        )
       )
      )
  {
    *rcvReady = FALSE;

    *rcvFull  = (rlp_data->rbm.PQ_Write EQ writeMax);
  }
  else
  {
    *rcvFull = FALSE;
    *rcvReady = TRUE;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_set_wind_size
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : windSize -
|
|
|  Return      : TRUE  -
|                FALSE -
+------------------------------------------------------------------------------
*/


GLOBAL BOOL rbm_set_wind_size
            (
              T_RBM_BUFFER_INDEX windSize
            )
{
  BOOL resetTimers;
  T_RBM_BUFFER_INDEX n;
  T_RBM_BUFFER_INDEX nEnd;
  T_RBM_BUFFER_INDEX oldSize;

  TRACE_FUNCTION ("rbm_set_wind_size()");

  resetTimers = FALSE;
  oldSize = rlp_data->rbm.K;
  rlp_data->rbm.K = windSize;

  if (windSize < oldSize)
  {
    n = rlp_data->rbm.VR + windSize;
    if (n >= RBM_BUF_SIZE)
    {
      n -= RBM_BUF_SIZE;
    }
    nEnd = rlp_data->rbm.VR + oldSize;
    if (nEnd >= RBM_BUF_SIZE)
    {
      nEnd -= RBM_BUF_SIZE;
    }
    do
    {
      switch (rlp_data->rbm.Slot[n].R_State)
      {
      case RBM_SREJ:
      case RBM_WAIT:
        resetTimers = TRUE;
        rlp_data->rbm.Slot[n].R_State = RBM_IDLE;
        break;
      default:
        break;
      }
      n += 1;
      if (n >= RBM_BUF_SIZE)
      {
        n = 0;
      }

    } while (n NEQ nEnd);
  }
  return (resetTimers);
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_init
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : windSize      -
|                frameSize     -
|                framesPerPrim -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_init
            (
              T_RBM_BUFFER_INDEX windSize,
              USHORT             frameSize,
              USHORT             framesPerPrim
            )
{
  USHORT n;

  TRACE_FUNCTION ("rbm_init()");

  n = 0;

  rlp_data->rbm.FramesPerPrim = framesPerPrim;
  rlp_data->rbm.FrameSize     = frameSize;

  for (n = 0; n < RBM_BUF_SIZE; n++)
  {
    rlp_data->rbm.Slot[n].R_State = RBM_IDLE;

    memset
    (
      rlp_data->rbm.Slot[n].Frame,
      0,
      rlp_data->rbm.FrameSize
    );
  }

  rlp_data->rbm.VR          = 0;
  rlp_data->rbm.FrameInRiBu = FALSE;

  memset
  (
    rlp_data->rbm.Frame,
    0,
    rlp_data->rbm.FrameSize
  );

  for (n = 0; n < RBM_PQ_SIZE; n++)
    rlp_data->rbm.PQ_Array[n] = (T_RBM_PRIM *) NULL;


  rlp_data->rbm.PQ_Read  = 0;
  rlp_data->rbm.PQ_Write = 0;

  rbmi_alloc_curr_prim ();
  rlp_data->rbm.K = windSize;

  rlp_data->rbm.Initialised = TRUE;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_deinit
+------------------------------------------------------------------------------
|  Description : This function frees all allocated T_RBM_PRIM primitives
|                of the receive buffer manager.
|
|  Parameters  :
|
|
|  Return      :
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_deinit
            (
              void
            )
{
  TRACE_FUNCTION ("rbm_deinit()");

  rlp_data->rbm.Initialised = FALSE;

  if (rlp_data->rbm.CurrPrim NEQ (T_RBM_PRIM *) NULL)
  {
    PFREE (rlp_data->rbm.CurrPrim)
    rlp_data->rbm.CurrPrim = (T_RBM_PRIM *) NULL;
  }

  while (rlp_data->rbm.PQ_Read NEQ rlp_data->rbm.PQ_Write)
  {
    if (rlp_data->rbm.PQ_Array[rlp_data->rbm.PQ_Read]
    NEQ (T_RBM_PRIM *) NULL)
    {
      PFREE (rlp_data->rbm.PQ_Array[rlp_data->rbm.PQ_Read])
      rlp_data->rbm.PQ_Array[rlp_data->rbm.PQ_Read]
       = (T_RBM_PRIM *) NULL;
    }
    rlp_data->rbm.PQ_Read = (rlp_data->rbm.PQ_Read+1) % RBM_PQ_SIZE;
  }

}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_reset
+------------------------------------------------------------------------------
|  Description : This function must be called once at startup before any
|                other procedures of the RBM is invoked. (Coldstart)
|
|
|  Parameters  : rlp_data_ptr -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_reset(T_RBM *rbm)
{
  TRACE_FUNCTION ("rbm_reset()");

  rbm->CurrPrim    = (T_RBM_PRIM *) NULL;
  rbm->CurrPrimCou = 0;
  rbm->K           = 0;
  rbm->FrameInRiBu = FALSE;
  rbm->FrameSize   = FRAME_SIZE_SHORT;
  rbm->PQ_Read     = 0;
  rbm->PQ_Write    = 0;
  rbm->Initialised = FALSE;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_reset_srej_slots
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_reset_srej_slots
            (
              void
            )
{
  T_RBM_BUFFER_INDEX n;

  TRACE_FUNCTION ("rbm_reset_srej_slots()");

  for (n = 0; n < RBM_BUF_SIZE; n++)
  {
    switch (rlp_data->rbm.Slot[n].R_State)
    {
      case RBM_WAIT:
      case RBM_SREJ:
        rlp_data->rbm.Slot[n].R_State = RBM_IDLE;
        return;

      default:
        break;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_reset_all_r_states
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_reset_all_r_states
            (
              void
            )
{
  T_RBM_BUFFER_INDEX n;

  TRACE_FUNCTION ("rbm_reset_all_r_states()");

  for (n = 0; n < RBM_BUF_SIZE; n++)
    rlp_data->rbm.Slot[n].R_State = RBM_IDLE;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_check_slots_srej
+------------------------------------------------------------------------------
|  Description : Checks wheter a received slot is to be SREJted.
|                Returns RBM_INVALID: IDX if No SREJ pending
|                0..n       : Slot num to be SREJted
|
|  Parameters  : -
|
|
|  Return      : n                -
|                RBM_INVALID_IDX  -
+------------------------------------------------------------------------------
*/


GLOBAL T_RBM_BUFFER_EXTIDX rbm_check_slots_srej(void)
{
  T_RBM_BUFFER_INDEX n;

  TRACE_FUNCTION ("rbm_check_slots_srej()");

  n = rlp_data->rbm.VR;

  do
  {
    switch (rlp_data->rbm.Slot[n].R_State)
    {
      case RBM_IDLE:
        return RBM_INVALID_IDX;

      case RBM_SREJ:
        return n;

      default:
        n = (n+1) % RBM_BUF_SIZE;
        break;
    }
  }
  while (n NEQ rlp_data->rbm.VR);

  return RBM_INVALID_IDX;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_set_rslot_wait
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : slot -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_set_rslot_wait
            (
              T_RBM_BUFFER_INDEX slot
            )
{
  TRACE_FUNCTION ("rbm_set_rslot_wait()");

  rlp_data->rbm.Slot[slot].R_State = RBM_WAIT;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_set_rslot_wait2srej
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : slot -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_set_rslot_wait2srej
            (
              T_RBM_BUFFER_INDEX slot
            )
{
  TRACE_FUNCTION ("rbm_set_rslot_wait2srej()");

  rlp_data->rbm.Slot[slot].R_State = RBM_SREJ;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_set_rslot_rcvd
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : slot       -
|                resetTimer -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_set_rslot_rcvd
            (
              T_RBM_BUFFER_EXTIDX  slot,
              BOOL               *resetTimer
            )
{
  TRACE_FUNCTION ("rbm_set_rslot_rcvd()");

  if(slot EQ RBM_INVALID_IDX)
    slot = rlp_data->rbm.LastVR;

  switch (rlp_data->rbm.Slot[slot].R_State)
  {
    case RBM_WAIT:
      *resetTimer = TRUE;
      break;

    default:
      *resetTimer = FALSE;
      break;
  }

  rlp_data->rbm.Slot[slot].R_State = RBM_RCVD;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_get_current_frame
+------------------------------------------------------------------------------
|  Description : This function returns the current - i.e. last stored -
|                frame of the receive buffer. The frame may be in the ring
|                buffer or in the static buffer.
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL T_RBM_FRAMEPTR  rbm_get_current_frame
                       (
                         void
                       )
{

  T_RBM_FRAMEPTR frame;

  TRACE_FUNCTION ("rbm_get_current_frame()");

  if (rlp_data->rbm.FrameInRiBu)
    frame = rlp_data->rbm.Slot[rlp_data->rbm.VR].Frame;
  else
    frame = rlp_data->rbm.Frame;

  return frame;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_accept_current_frame
+------------------------------------------------------------------------------
|  Description : This function advances VR to make sure, that the next
|                frame is stored in the next free slot.
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_accept_current_frame
            (
              void
            )
{
  TRACE_FUNCTION ("rbm_accept_current_frame()");

  rlp_data->rbm.LastVR = rlp_data->rbm.VR;

  do
  {
    rlp_data->rbm.VR = (rlp_data->rbm.VR +1) % RBM_BUF_SIZE;
  } while (rlp_data->rbm.Slot[rlp_data->rbm.VR].R_State EQ RBM_RCVD);
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_mark_missing_i_frames_srej
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : ns -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_mark_missing_i_frames_srej
            (
              T_RBM_BUFFER_INDEX ns
            )
{
  T_RBM_BUFFER_INDEX n;

  TRACE_FUNCTION ("rbm_mark_missing_i_frames_srej()");

  n = rlp_data->rbm.VR;

  do
  {
    switch (rlp_data->rbm.Slot[n].R_State)
    {
      case RBM_IDLE:
        rlp_data->rbm.Slot[n].R_State = RBM_SREJ;
        break;

      default:
        break;
    }

    n = (n+1) % RBM_BUF_SIZE;
  }
  while (n NEQ ns);
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_count_missing_i_frames
+------------------------------------------------------------------------------
|  Description : Counts all slots in R buffer, which will be marked by
|                rbm_mark_missing_i_frames_srej
|
|  Parameters  : ns     -
|                count  -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_count_missing_i_frames
            (
              T_RBM_BUFFER_INDEX  ns,
              T_RBM_BUFFER_INDEX *count
            )
{
  T_RBM_BUFFER_INDEX n;

  TRACE_FUNCTION ("rbm_count_missing_i_frames()");

  *count = 0;
  n      = rlp_data->rbm.VR;

  do
  {
    switch (rlp_data->rbm.Slot[n].R_State)
    {
      case RBM_IDLE:
        (*count)++;
        break;

      default:
        break;
    }

    n = (n+1) % RBM_BUF_SIZE;
  }
  while (n NEQ ns);
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_buffer_all_in_sequence_frames
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : fromSlot     -
|                primIsReady  -
|                rcvReady     -
|                rcvFull      -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_buffer_all_in_sequence_frames
            (
              T_RBM_BUFFER_EXTIDX   fromSlot,
              BOOL                 *primIsReady,
              BOOL                 *rcvReady,
              BOOL                 *rcvFull
            )
{
  TRACE_FUNCTION ("rbm_buffer_all_in_sequence_frames()");

  if(fromSlot EQ RBM_INVALID_IDX)
    fromSlot = rlp_data->rbm.LastVR;

  *primIsReady = FALSE;

  do
  {
    rbmi_buffer_frame (fromSlot, primIsReady);
    rlp_data->rbm.Slot[fromSlot].R_State = RBM_IDLE;
    fromSlot = (fromSlot + 1) % RBM_BUF_SIZE;
  } while (rlp_data->rbm.Slot[fromSlot].R_State EQ RBM_RCVD);

  rbmi_pq_check (rcvReady, rcvFull);
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_ns_check
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : ns       -
|                valid    -
|                expected -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rbm_ns_check
            (
              T_RBM_BUFFER_INDEX  ns,
              BOOL               *valid,
              BOOL               *expected
            )
{
  USHORT max_ns;

  TRACE_FUNCTION ("rbm_ns_check()");

  if(rlp_data->rbm.K EQ 0)
  {
    *valid    = FALSE;
    *expected = FALSE;
    return;
  }


  max_ns = (rlp_data->rbm.VR + rlp_data->rbm.K -1) % RBM_BUF_SIZE;
  /*
   * ns within Window?
   */
  if (!(XOR
        (
          XOR
          (
            rlp_data->rbm.VR > ns,
            ns > max_ns
          ),
          rlp_data->rbm.VR > max_ns
        )
       )
     )
  {
    *valid    = TRUE;
    *expected = (ns EQ rlp_data->rbm.VR);
  }
  else
  {
    *valid    = FALSE;
    *expected = FALSE;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_get_vr
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : VR
+------------------------------------------------------------------------------
*/


GLOBAL T_RBM_BUFFER_INDEX rbm_get_vr
                          (
                            void
                          )
{
  TRACE_FUNCTION ("rbm_get_vr()");

  return rlp_data->rbm.VR;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_get_prim
+------------------------------------------------------------------------------
|  Description : This is just a wrapper for rbmi_pq_deq. The queu is an
|                internal matter of the RBM, therefore the function
|                rbmi_pq_deq should not be used directly by the user.
|
|  Parameters  : prim     -
|                rcvReady -
|                rcvFull  -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL BOOL rbm_get_prim
            (
              T_RBM_PRIM **prim,
              BOOL       *rcvReady,
              BOOL       *rcvFull
            )
{
  BOOL primFound;
  TRACE_FUNCTION ("rbm_get_prim()");

  primFound = rbmi_pq_deq (prim);
  if (primFound)
  {
    rbmi_pq_check(rcvReady, rcvFull);
  }
  return primFound;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_get_curr_prim
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : prim
+------------------------------------------------------------------------------
*/


GLOBAL T_RBM_PRIM *rbm_get_curr_prim
            (
              void
            )
{
  T_RBM_PRIM *prim;

  TRACE_FUNCTION ("rbm_get_curr_prim()");

  rlp_data->rbm.CurrPrim->sdu.l_buf
    = rlp_data->rbm.CurrPrimCou * rlp_data->rbm.FrameSize * 8;

  rlp_data->rbm.CurrPrim->sdu.o_buf  = 0;

  prim = rlp_data->rbm.CurrPrim;

  rbmi_alloc_curr_prim ();

  return prim;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_move_current_frame
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  :
|
|
|  Return      :
+------------------------------------------------------------------------------
*/



GLOBAL void rbm_move_current_frame
            (
              T_RBM_BUFFER_INDEX slot
            )
{
  TRACE_FUNCTION ("rbm_move_current_frame()");

  memcpy
  (
    rlp_data->rbm.Slot[slot].Frame,
    rlp_data->rbm.Slot[rlp_data->rbm.VR].Frame,
    rlp_data->rbm.FrameSize
  );
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_is_state_wait
+------------------------------------------------------------------------------
|  Description : This function is called from the HISR. It returns the
|                frame descriptor for the buffer, in which the HISR has to
|                write the received frame.
|
|
|  Parameters  : slot -
|
|
|  Return      : TRUE  -
|                FALSE -
+------------------------------------------------------------------------------
*/


GLOBAL BOOL rbm_is_state_wait
            (
              T_RBM_BUFFER_INDEX slot
            )
{
  TRACE_FUNCTION ("rbm_is_state_wait()");

  if (rlp_data->rbm.Slot[slot].R_State EQ RBM_WAIT)
    return TRUE;
  else
    return FALSE;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_prepare_remap
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : frameSize -
|
|
|  Return      :
+------------------------------------------------------------------------------
*/


GLOBAL T_FRAME_NUM rbm_prepare_remap
                   (
                     USHORT frameSize
                   )
{
  T_RBM_PRIM *prim;
  USHORT oldFrameSize;
  USHORT currPrimCou;

  TRACE_FUNCTION ("rbm_prepare_remap()");

  oldFrameSize = rlp_data->rbm.FrameSize;
  currPrimCou  = rlp_data->rbm.CurrPrimCou;

  rlp_data->rbm.FrameSize = frameSize;

  prim = rbm_get_curr_prim ();

  prim->sdu.l_buf = currPrimCou * oldFrameSize * 8;

  rbmi_pq_enq (prim);

  return (rbm_get_vr());
}

#ifdef _SIMULATION_
/*
+------------------------------------------------------------------------------
|  Function    : rbm_store_frame
+------------------------------------------------------------------------------
|  Description : This procedure is only used in the simulation environment
|                to store a frame in the receive buffer. 
|                In the implementation the copy is performed 
|                by the RA adaptation layer
|                which has access to the shared memory area of the DSP.
|
|  Parameters  : frame -
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void rbm_store_frame(T_RBM_FRAMEPTR frame)
{
  TRACE_FUNCTION ("rbm_store_frame()");

  if (rlp_data->rbm.Initialised)
  {
    memcpy
    (
      rlp_data->rbm.Slot[rlp_data->rbm.VR].Frame,
      frame,
      rlp_data->rbm.FrameSize
    );

    rlp_data->rbm.FrameInRiBu = TRUE;
  }
  else
  {
    memcpy
    (
      rlp_data->rbm.Frame,
      frame,
      rlp_data->rbm.FrameSize
    );

    rlp_data->rbm.FrameInRiBu = FALSE;
  }
}
#endif

#ifdef _TARGET_
/*
+------------------------------------------------------------------------------
|  Function    : rlp_rbm_get_next_buffer
+------------------------------------------------------------------------------
|  Description : This function is called from the HISR. It returns the
|                frame descriptor for the buffer, in which the HISR has to
|                write the received frame.
|
|
|  Parameters  : frameDesc -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rlp_rbm_get_next_buffer(T_FRAME_DESC *frameDesc)
{
  TRACE_FUNCTION ("rbm_get_next_buffer()");

  if (rlp_data->rbm.Initialised)
  {
    frameDesc->Adr[0] = rlp_data->rbm.Slot[rlp_data->rbm.VR].Frame;
    rlp_data->rbm.FrameInRiBu = TRUE;
  }
  else
  {
    frameDesc->Adr[0] = rlp_data->rbm.Frame;
    rlp_data->rbm.FrameInRiBu = FALSE;
  }

  frameDesc->Len[0] = rlp_data->rbm.FrameSize;
  frameDesc->Adr[1] = (UBYTE *) NULL;
  frameDesc->Len[1] = 0;
}
#endif

