/*
+-----------------------------------------------------------------------------
|  Project :  GSM-F&D (8411)
|  Modul   :  RA_TRA
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
|  Purpose :  This Modul defines the functions for the transparent
|             data model for the component RA of the mobile station
+-----------------------------------------------------------------------------
*/

#ifndef RA_L1INT_C
#define RA_L1INT_C
#endif

#define ENTITY_RA

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h"
#include "pconst.cdg"
#include "custom.h"
#include "gsm.h"
#include "cnf_ra.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "ccdapi.h"
#include "ra.h"
#include "ra_l1int.h"

/*==== CONST ======================================================*/
/*==== TYPES ======================================================*/
/*==== VAR EXPORT ==================================================*/
/*==== VAR LOCAL ===================================================*/
/*==== FUNCTIONS ==================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_activate_req      |
+--------------------------------------------------------------------+

  PURPOSE : Process the primitive RA_ACTIVATE_REQ.
            This function performs the TCH synchronisation
            if transparent data services are set up.
*/

GLOBAL void l1i_ra_activate_req (T_RA_ACTIVATE_REQ *ra_activate_req)
{
  TRACE_EVENT ("l1i_ra_activate_req()");

#ifdef _SIMULATION_
  PACCESS (ra_activate_req);
#else
  /*
   * Set ra_handle only for target
   * because RA is not a single task here like on PC simulation.
   * Handle is needed for stack check in frame.
   */
  ra_handle = vsi_gettaskhandle(VSI_CALLER L1_NAME);
#endif

  switch (ra_activate_req->model)
  {
  case RA_MODEL_TRANS:
    dll_init ();
    MALLOC(ra_data->tra, sizeof(T_TRA));

#ifdef _SIMULATION_
    /*
    Provide frame descriptors of L2R's downlink FIFO
    for the simulation
    */
    cl_ribu_create(&ra_data->dl_ribu, MAX_TRANS_BUFFER, RA_FIFO_DEPTH);
#endif

    tra_init (ra_activate_req->tra_rate);
    TRACE_EVENT ("RA MODE: Transparent");
    ra_data->model = MODEL_TRANS;
    break;

  case RA_MODEL_RLP:
    dll_init ();
    MALLOC(ra_data->rlp, sizeof(T_RLP));
    rlp_init ();
    TRACE_EVENT ("RA MODE: Non-Transparent");
    ra_data->model = MODEL_RLP;
    break;

#ifdef FF_FAX
  case RA_MODEL_FAX:
    dll_init ();
    MALLOC(ra_data->fax, sizeof(T_FAX));

#ifdef _SIMULATION_
    /*
    Provide frame descriptors of FAD's downlink FIFO
    for the simulation
    */
    cl_ribu_create(&ra_data->dl_ribu, FAD_DESCBUF_LEN, RA_FIFO_DEPTH);
#endif

    cl_ribu_create(&ra_data->fax->ul_ribu, FAD_DESCBUF_LEN, RA_FIFO_DEPTH);

    fax_init ();
    TRACE_EVENT ("RA MODE: FAX");
    ra_data->model = MODEL_FAX;
    break;
#endif /* FF_FAX */

  default:
    TRACE_EVENT ("RA MODE: INVALID (SPEECH?)");
#ifdef _SIMULATION_
    PFREE(ra_activate_req);
#endif
    return;
  }

  ra_data->tra_rate  = ra_activate_req->tra_rate;
  ra_data->user_rate = ra_activate_req->user_rate;
  ra_data->ndb       = ra_activate_req->ndb;
  ra_data->nsb       = ra_activate_req->nsb;
  ra_data->activated = TRUE;

#ifdef _SIMULATION_
  PFREE(ra_activate_req);
#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_datatrans_req     |
+--------------------------------------------------------------------+

  PURPOSE : Process the primitive RA_DATATRANS_REQ.
            This function is signalling
            the upper layer is ready to provide frame descriptors
            for downlink data transfer.
*/
GLOBAL void l1i_ra_datatrans_req (T_RA_DATATRANS_REQ *ra_datatrans_req)
{
  TRACE_EVENT ("l1i_ra_datatrans_req()");

#ifdef _SIMULATION_
  PACCESS (ra_datatrans_req);
#endif

  switch (ra_data->model)
  {
    case MODEL_TRANS:
      if (GET_STATE(KER) EQ TRA_DATA_TRANS_PRE2)
      {
        SET_STATE (KER, TRA_DATA_TRANS_PRE3);
      }
      break;

#ifdef FF_FAX
    case MODEL_FAX:
      if (GET_STATE(KER) EQ FAX_DATA_TRANS_PRE)
      {
        SET_STATE (KER, FAX_DATA_TRANS);
      }
      break;
#endif /* FF_FAX */

    case MODEL_RLP:
      break;
  }

#ifdef _SIMULATION_
  PFREE(ra_datatrans_req);
#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_activate_cnf      |
+--------------------------------------------------------------------+

  PURPOSE : Send the primitive RA_ACTIVATE_CNF.
*/

GLOBAL void l1i_ra_activate_cnf(void)
{
  TRACE_EVENT ("l1i_ra_activate_cnf()");

#ifndef _SIMULATION_

  PSIGNAL(hCommMMI, RA_ACTIVATE_CNF, NULL);

#else
  {
  PALLOC(ra_activate_cnf, RA_ACTIVATE_CNF);
  PSENDX(MMI, ra_activate_cnf);
  }
#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_modify_req        |
+--------------------------------------------------------------------+

  PURPOSE : Process the primitive RA_MODIFY_REQ.
            This function performs no TCH synchronisation.
            Only the communication parameters are changed in the shared memory
            and in case of fax the sequencer is synchronized.
            This function is only used in fax mode.
*/

GLOBAL void l1i_ra_modify_req (T_RA_MODIFY_REQ *ra_modify_req)
{
#ifdef FF_FAX
  TRACE_EVENT ("l1i_ra_modify_req()");

#ifdef _SIMULATION_
  PACCESS (ra_modify_req);
#endif

  if (ra_data->activated AND ra_data->model EQ MODEL_FAX)
  {

    ra_data->tra_rate   = ra_modify_req->tra_rate;
    ra_data->user_rate  = ra_modify_req->user_rate;

#ifdef TRACE_RA_FAX_STATUS
    TRACE_EVENT_P2("trans_rate=0x%02X & user_rate=0x%02X", ra_data->tra_rate, ra_data->user_rate);
#endif

    SET_STATE (KER, FAX_MODIFY);

#ifdef TRACE_RA_FAX_STATUS
    if (ra_data->fax->state_last NEQ ra_data->ker.state)
    {
      TRACE_EVENT_P2("KER:FAX_%d -> FAX_%d", ra_data->fax->state_last, ra_data->ker.state);
      ra_data->fax->state_last = ra_data->ker.state;
    }
#endif
  }

#ifdef _SIMULATION_
  PFREE(ra_modify_req);
#endif

#endif /* FF_FAX */
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_modify_cnf        |
+--------------------------------------------------------------------+

  PURPOSE : Send the primitive RA_MODIFY_CNF.
*/

GLOBAL void l1i_ra_modify_cnf(void)
{
#ifdef FF_FAX
  TRACE_EVENT ("l1i_ra_modify_cnf()");

#ifndef _SIMULATION_

  PSIGNAL ( hCommMMI, RA_MODIFY_CNF, NULL );

#else
  {
  PALLOC (ra_modify_cnf, RA_MODIFY_CNF);
  PSENDX (MMI, ra_modify_cnf);
  }
#endif

#endif /* FF_FAX */
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_deactivate_req    |
+--------------------------------------------------------------------+

  PURPOSE : Process the primitive RA_DEACTIVATE_REQ.
            This function stops the processing activities
            of the processes TRA, RLP or FAX.

*/

GLOBAL void l1i_ra_deactivate_req (T_RA_DEACTIVATE_REQ *ra_deactivate_req)
{
  TRACE_EVENT ("l1i_ra_deactivate_req()");

#ifdef _SIMULATION_
  PACCESS (ra_deactivate_req);
#endif

  if (ra_data->activated)
  {
    switch (ra_data->model)
    {
      case MODEL_TRANS:
        SET_STATE (KER, TRA_NULL);
        MFREE(ra_data->tra);
        break;

      case MODEL_RLP:
        SET_STATE (KER, RLP_NULL);
        MFREE(ra_data->rlp);
        break;

#ifdef FF_FAX
      case MODEL_FAX:
        SET_STATE (KER, FAX_NULL);
#ifdef TRACE_RA_FAX_STATUS
        if (ra_data->fax->state_last NEQ ra_data->ker.state)
        {
          TRACE_EVENT_P2("KER:FAX_%d -> FAX_%d", ra_data->fax->state_last, ra_data->ker.state);
          ra_data->fax->state_last = ra_data->ker.state;
        }
#endif
        if (ra_data->fax->ul_ribu)
          cl_ribu_release(&ra_data->fax->ul_ribu);

        MFREE(ra_data->fax);
        break;
#endif /* FF_FAX */

    }

#ifdef _SIMULATION_
    if (ra_data->dl_ribu)
      cl_ribu_release(&ra_data->dl_ribu);
#endif

    dll_init();

#ifdef _SIMULATION_
    {
    PALLOC (ra_deactivate_cnf, RA_DEACTIVATE_CNF);
    PSENDX (MMI, ra_deactivate_cnf);
    }
#else
    PSIGNAL (hCommMMI, RA_DEACTIVATE_CNF, &ra_data->ra_deactivate_cnf);
#endif

  }
ra_data->activated=FALSE;
#ifdef _SIMULATION_
  PFREE(ra_deactivate_req);
#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_data_req          |
+--------------------------------------------------------------------+

  PURPOSE : Process the primitive RA_DATA_REQ.
*/

GLOBAL void l1i_ra_data_req(T_RA_DATA_REQ *ra_data_req, T_FRAME_DESC *frame_desc)
{
  TRACE_FUNCTION ("l1i_ra_data_req()");

  if (ra_data->activated)
  {
    switch (ra_data->model)
    {
    case MODEL_RLP:

      if (frame_desc->Len[0] > 0)
      {
        U8 ribu_wi = cl_ribu_write_index(&ra_data->rlp->ulq);
        cl_set_frame_desc(&ra_data->rlp->ul_frame_desc[ribu_wi],
          frame_desc->Adr[0], frame_desc->Len[0], frame_desc->Adr[1], frame_desc->Len[1]);

#ifdef TRACE_RA_RLP
        TRACE_EVENT_P3("RLP_UL_L1:0x%02X%02X - %d",
            *(ra_data->rlp->ul_frame_desc[ribu_wi].Adr[0]+1),
            *(ra_data->rlp->ul_frame_desc[ribu_wi].Adr[0]+0),
              ra_data->rlp->ul_frame_desc[ribu_wi].Len[0]);
#endif
        ra_data->rlp->ul_data_pending = TRUE;
        ra_data->rlp->m1 = 0;
        ra_data->rlp->ul_48_first_half_sent = FALSE;
      }
      break; /* MODEL_RLP */

    default:
      break;
    }
  }
}

GLOBAL void l1i_ra_data_req_new(T_FD *pFD)
{
  TRACE_FUNCTION ("l1i_ra_data_req_new()");

  if (ra_data->activated)
  {
    switch (ra_data->model)
    {
    case MODEL_TRANS:
      if (ra_data->tra->ul_status NEQ pFD->status)
      {
        ra_data->tra->ul_sa     = ((pFD->status & ST_SA)   ? 1 : 0);
        ra_data->tra->ul_sb     = ((pFD->status & ST_SB)   ? 1 : 0);
        ra_data->tra->ul_x      = ((pFD->status & ST_X)    ? 1 : 0);
        ra_data->tra->overspeed = ((pFD->status & ST_OVER) ? 1 : 0);

        ra_data->tra->ul_status = pFD->status;
        ra_data->tra->ul_signals_pending = TRUE;
      }
      cl_set_frame_desc_0(&ra_data->tra->ul_frame_desc, pFD->buf, pFD->len);

      ra_data->tra->data_req_rec = TRUE;   /* ra_data_req received */

      if (ra_data->tra->ul_frame_desc.Len[0] > 0)
      {
        ra_data->tra->ul_data_pending = TRUE;
      }
      else
      {
        ra_data->tra->ul_signals_pending = TRUE;
      }
      break; /* MODEL_TRANS */

#ifdef FF_FAX
    case MODEL_FAX:
      cl_ribu_put(*pFD, ra_data->fax->ul_ribu);
      break; /* MODEL_FAX */
#endif

    default:
      break;
    }
  }
}

#ifdef _SIMULATION_

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_data_req_sim      |
+--------------------------------------------------------------------+

  PURPOSE : Process the primitive RA_DATA_REQ (SIMULATION wrapper)
*/

GLOBAL void l1i_ra_data_req_sim(T_RA_DATA_REQ *ra_data_req)
{
  static T_FRAME_DESC frame_desc;
  static U8 buf[MAX_FAX_BUFFER_UL];

  T_FD *pFD = &ra_data->ul_FD;

  TRACE_FUNCTION ("l1i_ra_data_req_sim()");

  PACCESS (ra_data_req);

  if (ra_data->activated)
  {
    switch (ra_data->model)
    {
    case MODEL_TRANS:
      pFD->status = ra_data_req->status;
      pFD->len = ra_data_req->sdu.l_buf>>3;
      pFD->buf = &ra_data->ul_buf[0];
      memcpy(pFD->buf, ra_data_req->sdu.buf, pFD->len);
      l1i_ra_data_req_new(pFD);
      break;

#ifdef FF_FAX
    case MODEL_FAX:
      pFD->type = ra_data_req->fr_type;
      pFD->len = ra_data_req->sdu.l_buf>>3;
      pFD->buf = ra_data_req->sdu.buf;
      l1i_ra_data_req_new(pFD);
      break;
#endif /* FF_FAX */

    default: /* serves old version */
      cl_set_frame_desc_0(&frame_desc, buf, (USHORT)(ra_data_req->sdu.l_buf >> 3));
      memcpy(frame_desc.Adr[0], ra_data_req->sdu.buf, frame_desc.Len[0]);
      l1i_ra_data_req(ra_data_req, &frame_desc);
      break;
    }
  }

  PFREE(ra_data_req);
}

#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_ready_ind         |
+--------------------------------------------------------------------+

  PURPOSE : Send the primitive RA_READY_IND.
*/

GLOBAL void l1i_ra_ready_ind(T_HANDLE handle, UBYTE idx)
{
  TRACE_FUNCTION ("l1i_ra_ready_ind()");

#ifdef _SIMULATION_
  {
  PALLOC (ra_ready_ind, RA_READY_IND);
  memcpy (ra_ready_ind, &ra_data->ra_ready_ind[idx], sizeof (T_RA_READY_IND));
  PSEND (handle, ra_ready_ind);
  }
#else

  PSIGNAL(handle, RA_READY_IND, &ra_data->ra_ready_ind[idx]);

#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_data_ind          |
+--------------------------------------------------------------------+

  PURPOSE : Send the primitive RA_DATA_IND
*/

GLOBAL void l1i_ra_data_ind(T_HANDLE handle)
{
  TRACE_FUNCTION ("l1i_ra_data_ind()");

#ifdef _TARGET_

  PSIGNAL(handle, RA_DATA_IND, &ra_data->ra_data_ind);

#else /* _SIMULATION_ */

  {
  PALLOC_SDU (ra_data_ind, RA_DATA_IND, (MAX_FAX_BUFFER_DL << 3));
  memcpy(ra_data_ind, &ra_data->ra_data_ind, sizeof (T_RA_DATA_IND));

  switch (ra_data->model)
  {
  case MODEL_TRANS:
    {
      T_TRA *dtra = ra_data->tra;

      dtra->dl_pFD = cl_ribu_get(ra_data->dl_ribu);

      if (dtra->dl_pFD AND dtra->dl_pFD->len)
      {
        ra_data_ind->sdu.l_buf = dtra->dl_pFD->len<<3;
        memcpy(&ra_data_ind->sdu.buf[0], dtra->dl_pFD->buf, dtra->dl_pFD->len);
        dtra->dl_pFD = NULL;
      }
    }
    break;

#ifdef FF_FAX
  case MODEL_FAX:
    {
      T_FAX *dfax = ra_data->fax;

      dfax->dl_pFD = cl_ribu_get(ra_data->dl_ribu);

      if (dfax->dl_pFD)
      {
        ra_data_ind->fr_type = dfax->dl_pFD->type;

        if (dfax->dl_pFD->len)
        {
          if (ra_data_ind->fr_type EQ FR_SYNC)
            ra_data_ind->sdu.l_buf = 0;
          else
          {
            ra_data_ind->sdu.l_buf = dfax->dl_pFD->len<<3;
            memcpy(&ra_data_ind->sdu.buf[0], dfax->dl_pFD->buf, ra_data_ind->sdu.l_buf>>3);
          }
        }
      }
    }
    break;
#endif /* FF_FAX */

  default:
    if (ra_data->ra_data_ind.sdu.l_buf)
    {
      memcpy(&ra_data_ind->sdu.buf[0], ra_data->rlp->dl_buf, ra_data->ra_data_ind.sdu.l_buf>>3);
    }
    break;
  }
  PSEND (handle, ra_data_ind);
  }

#endif /* _SIMULATION_ */
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_break_req         |
+--------------------------------------------------------------------+

  PURPOSE : Process the primitive RA_BREAK_REQ.
*/

GLOBAL void l1i_ra_break_req(T_RA_BREAK_REQ *ra_break_req)
{
  TRACE_EVENT ("l1i_ra_break_req()");

#ifdef _SIMULATION_
  PACCESS (ra_break_req);
#endif

  if (ra_data->activated)
  {
    /*
     * a break can only be requested if it is the
     * first request or the last is sent completeley
     * via the TCH.
     */
    if (ra_data->tra->break_len EQ 0)
    {
      ra_data->tra->break_len = ra_break_req->break_len;
    }
    else
      TRACE_EVENT ("break request too early");
  }

#ifdef _SIMULATION_
  PFREE(ra_break_req);
#endif
}

#ifndef HO_WORKAROUND
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_break_ind         |
+--------------------------------------------------------------------+

  PURPOSE : Send the primitive RA_BREAK_IND.
*/

GLOBAL void l1i_ra_break_ind(void)
{
  TRACE_EVENT ("l1i_ra_break_ind()");

#ifndef _SIMULATION_

  PSIGNAL (hCommTRA, RA_BREAK_IND, &ra_data->ra_break_ind);

#else
  {
  PALLOC (ra_break_ind, RA_BREAK_IND);
  memcpy (ra_break_ind, &ra_data->tra->ra_break_ind, sizeof(T_RA_BREAK_IND));
  PSENDX (TRA, ra_break_ind);
  }
#endif
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_L1INT                 |
| STATE   : code                  ROUTINE : l1i_ra_detect_req        |
+--------------------------------------------------------------------+

  PURPOSE : Process the primitive RA_DETECT_REQ.

*/

GLOBAL void l1i_ra_detect_req(T_RA_DETECT_REQ *ra_detect_req)
{
  TRACE_FUNCTION ("l1i_ra_detect_req()");

#ifdef _SIMULATION_
  PACCESS (ra_detect_req);
#endif

  if (ra_data->activated)
  {
    if (ra_detect_req->detect EQ DET_NO_STATUS)
    {
      shm_set_dsp_value (fax_b_status_detect, 0);
    }
    else if (ra_detect_req->detect EQ DET_STATUS)
    {
      shm_set_dsp_value (fax_b_status_detect, 1);
    }
  }

#ifdef _SIMULATION_
  PFREE(ra_detect_req);
#endif
}

