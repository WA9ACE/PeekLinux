/*
+-----------------------------------------------------------------------------
|  Project :  GSM-F&D (8411)
|  Modul   :  RA_RLP
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
|  Purpose :  This Modul defines the functions for the non-transparent
|             data model (RLP) for the component RA of the mobile
|             station
+-----------------------------------------------------------------------------
*/

#ifndef RA_RLP_C
#define RA_RLP_C
#endif

#define ENTITY_RA

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
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

/*==== CONST ======================================================*/
/*==== TYPES ======================================================*/
/*==== VARIABLES ==================================================*/
/*==== FUNCTIONS ==================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_RLP                   |
| STATE   : code                  ROUTINE : rlp_init                 |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void rlp_init(void)
{
  TRACE_FUNCTION("rlp_init()");

  memset(ra_data->rlp, 0, sizeof(T_RLP));
  hCommRLP = vsi_c_open ( VSI_CALLER RLP_NAME );
  hCommMMI = vsi_c_open ( VSI_CALLER ACI_NAME );
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_RLP                   |
| STATE   : code                  ROUTINE : rlp_idle                 |
+--------------------------------------------------------------------+

  PURPOSE : IDLE processing fpr uplink and downlink non-transparent mode
*/

GLOBAL void rlp_idle(void)
{
  TRACE_FUNCTION("rlp_idle()");
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_RLP                   |
| STATE   : code                  ROUTINE : rlp_ul_null              |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void rlp_ul_null(void)
{
  T_RLP *drlp = ra_data->rlp;
  UBYTE i;

  TRACE_FUNCTION("rlp_ul_null()");

  if (ra_data->activated)
  {
#ifdef TRACE_RA_RLP
    drlp->fcs_error = 0L;
#endif
   /*
    * setup the communication parameters
    */
    shm_set_dsp_value (conf_b_itc, V_ITC_DATA);
    shm_set_dsp_value (conf_b_ce, V_CE_NON_TRANSP);

    switch (ra_data->tra_rate)
    {
      case TRA_FULLRATE_14400:
        shm_set_dsp_value (conf_b_ur, V_UR_14400);
        shm_set_dsp_value (conf_b_ct, V_CT_FR);
        break;

      case TRA_FULLRATE_9600:
        shm_set_dsp_value (conf_b_ur, V_UR_9600);
        shm_set_dsp_value (conf_b_ct, V_CT_FR);
        break;

      case TRA_FULLRATE_4800:
        shm_set_dsp_value (conf_b_ur, V_UR_4800);
        shm_set_dsp_value (conf_b_ct, V_CT_FR);
        break;

      case TRA_HALFRATE_4800:
        shm_set_dsp_value (conf_b_ur, V_UR_4800);
        shm_set_dsp_value (conf_b_ct, V_CT_HR);
        break;

      default:
        break;
    }

    shm_set_dsp_value (ntu_M1, 0);
    shm_set_dsp_value (ntu_M2, 0);
    shm_set_dsp_value (ntu_empty, 1);
    shm_set_dsp_value (ntu_byte_cnt, 2);

    /*
    * initialize the non-transparent mode data
    */
    drlp->ul_data_pending   = FALSE;
    drlp->m1                = 0;

    ra_data->ra_data_ind.fr_type   = FR_RLP;
    ra_data->ra_data_ind.sdu.o_buf = 0;
    ra_data->ra_data_ind.sdu.l_buf = 0;

    ra_data->ra_ready_ind[0].req_frames = 1;

    cl_ribu_init(&drlp->ulq, RLP_UL_QSIZE);

    for (i = 0; i < drlp->ulq.depth; i++)
    {
      cl_set_frame_desc_0(&drlp->ul_frame_desc[i], NULL, 0);
    }

    cl_set_frame_desc_0(&ra_data->rlp->dl_frame_desc, NULL, 0);
    shm_set_dsp_value (act_b_init, 1);

    INIT_STATE (KER, RLP_INIT_L1);

#ifdef HO_WORKAROUND /* rlp_ul_null */
    _act_d_ra_conf = *ra_data->shm.shm_addr[d_ra_conf];
    TRACE_EVENT("d_ra_conf saved");
#endif

  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_RLP                   |
| STATE   : code                  ROUTINE : rlp_ul_init_l1           |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void rlp_ul_init_l1(void)
{
  TRACE_FUNCTION("rlp_ul_init_l1()");

  if (shm_get_dsp_value (act_b_init) EQ 0)
  {
   /*
    * initialisation finished -> enter the data transmission state
    */
    l1i_ra_activate_cnf();
    SET_STATE (KER, RLP_DATA_TRANS);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_RLP                   |
| STATE   : code                  ROUTINE : rlp_ul_data_trans        |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void rlp_ul_data_trans(void)
{
  T_RLP *drlp = ra_data->rlp;

  TRACE_FUNCTION("rlp_ul_data_trans()");

  if (drlp->ul_data_pending) /* a frame is ready to be sent */
  {
    USHORT len = 0;

    switch (ra_data->tra_rate)
    {
      case TRA_FULLRATE_9600:
      case TRA_HALFRATE_4800:
        len = NT_DATA_SIZE;
        drlp->ul_data_pending = FALSE;
        break;

      case TRA_FULLRATE_4800:
        /* S844: 1=1st half, 0=2nd half */
        if (shm_get_dsp_value (act_b_f48blk_ul) EQ 1)
        {
          TRACE_FUNCTION("send 4800 first half");
          len = NT_DATA_SIZE_4800_FR_A;
          drlp->ul_48_first_half_sent = TRUE;
        }
        else
        {
          if (drlp->ul_48_first_half_sent)
          {
            TRACE_FUNCTION("send 4800 second half");
            len = NT_DATA_SIZE_4800_FR_B;
            drlp->ul_data_pending = FALSE;
            drlp->ul_frame_desc[drlp->ulq.ri].Adr[0] += NT_DATA_SIZE_4800_FR_A;
          }
          else
          {
            TRACE_FUNCTION("wait for 4800 first half");
            len = 0;
          }
        }
        break;

      case TRA_FULLRATE_14400:
        shm_set_dsp_value (ntu_M1, drlp->m1);
        /* S844: 0 = first half, 1 = second half */
        if (drlp->m1 EQ 0)
        {
          len = NT_DATA_SIZE_14400_A;
          drlp->m1 = 1;
        }
        else
        {
          len = NT_DATA_SIZE_14400_B;
          drlp->ul_data_pending = FALSE;
          drlp->ul_frame_desc[drlp->ulq.ri].Adr[0] += NT_DATA_SIZE_14400_A;
        }
        break;
    }
    if (len > 0)
    {
      shm_set_dsp_buffer(&drlp->ul_frame_desc[drlp->ulq.ri],
       ra_data->shm.shm_addr[adr_ntu_data]+wof_ntu_data, 0, len);
      shm_set_dsp_value (ntu_empty, 0);
      shm_set_dsp_value (ntu_byte_cnt, (len+2));
      if (!drlp->ul_data_pending)
      {
        cl_ribu_read_index(&drlp->ulq);
        /* PZ 22.7.99 */
        drlp->ul_data_pending = (drlp->ulq.ri NEQ drlp->ulq.wi);
        /* PZ 22.7.99 */
      }
    }
    else
    {
      shm_set_dsp_value (ntu_M1, 0);
      shm_set_dsp_value (ntu_empty, 1);
      shm_set_dsp_value (ntu_byte_cnt, 2);
    }
  }
  else
  {
    /* no frame is ready to be sent */
    shm_set_dsp_value (ntu_M1, 0);
    shm_set_dsp_value (ntu_empty, 1);
    shm_set_dsp_value (ntu_byte_cnt, 2);
  }

  if (!drlp->ul_data_pending)
  {
    l1i_ra_ready_ind(hCommRLP, 0);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_RLP                   |
| STATE   : code                  ROUTINE : rlp_dl_data_trans        |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void rlp_dl_data_trans(void)
{
  BOOL frame_complete = FALSE;

  TRACE_FUNCTION("rlp_dl_data_trans()");

#ifdef HO_WORKAROUND /* rlp_dl_data_trans */
  if (*ra_data->shm.shm_addr[d_ra_conf] NEQ _act_d_ra_conf)
  {
    *ra_data->shm.shm_addr[d_ra_conf] = _act_d_ra_conf; /* rlp_dl_data_trans */
    TRACE_EVENT("HO_REFRESH rlp_dl_data_trans");
  }
#endif

  /*
   * read out the received downlink data
   */
  if (shm_get_dsp_value (ntd_byte_cnt) > 0)
  {
    USHORT len = 0;

    switch (ra_data->tra_rate)
    {
    case TRA_FULLRATE_9600:
    case TRA_HALFRATE_4800:
      if (shm_get_dsp_value (ntd_fcs_ok) EQ 0)
      {
        frame_complete = TRUE;
        len = NT_DATA_SIZE;

#ifdef _SIMULATION_
        cl_set_frame_desc_0(&ra_data->rlp->dl_frame_desc, ra_data->rlp->dl_buf, NT_DATA_SIZE);
        ra_data->ra_data_ind.sdu.l_buf = NT_DATA_SIZE << 3;
#else
        rlp_rbm_get_next_buffer(&ra_data->rlp->dl_frame_desc);
#endif
      }
      else
      {
#ifdef TRACE_RA_RLP
        TRACE_EVENT("FCS-Error");
        ra_data->rlp->fcs_error++;
#endif
        frame_complete = FALSE;
        len = 0;
      }
      break;

    case TRA_FULLRATE_4800:
      /* S844: 0=1st half, 1=2nd half */
      if (shm_get_dsp_value (act_b_f48blk_dl) EQ 0)
      {
        len = NT_DATA_SIZE_4800_FR_A;
        frame_complete = FALSE;
#ifdef _SIMULATION_
        cl_set_frame_desc_0(&ra_data->rlp->dl_frame_desc, ra_data->rlp->dl_buf, NT_DATA_SIZE_4800_FR_A+NT_DATA_SIZE_4800_FR_B);
        ra_data->ra_data_ind.sdu.l_buf = (NT_DATA_SIZE_4800_FR_A+NT_DATA_SIZE_4800_FR_B) << 3;
#else
        rlp_rbm_get_next_buffer(&ra_data->rlp->dl_frame_desc);
#endif
      }
      else if (shm_get_dsp_value (ntd_fcs_ok) EQ 0)
      {
        frame_complete = TRUE;
        len = NT_DATA_SIZE_4800_FR_B;
        ra_data->rlp->dl_frame_desc.Adr[0] += NT_DATA_SIZE_4800_FR_A;
      }
      else
      {
#ifdef TRACE_RA_RLP
        TRACE_EVENT("FCS-Error");
        ra_data->rlp->fcs_error++;
#endif
        frame_complete = FALSE;
        len = 0;
      }
      break;

    case TRA_FULLRATE_14400:
      /* S844: 0 = first half, 1 = second half */
      if (shm_get_dsp_value (ntd_M1) EQ 0)
      {
        frame_complete = FALSE;
        len = NT_DATA_SIZE_14400_A;
#ifdef _SIMULATION_
        cl_set_frame_desc_0(&ra_data->rlp->dl_frame_desc, ra_data->rlp->dl_buf, NT_DATA_SIZE_14400_A + NT_DATA_SIZE_14400_B);
        ra_data->ra_data_ind.sdu.l_buf = (NT_DATA_SIZE_14400_A + NT_DATA_SIZE_14400_B) << 3;
#else
        rlp_rbm_get_next_buffer(&ra_data->rlp->dl_frame_desc);
#endif
      }
      else if (shm_get_dsp_value (ntd_fcs_ok) EQ 0)
      {
        frame_complete = TRUE;
        len = NT_DATA_SIZE_14400_B;
        ra_data->rlp->dl_frame_desc.Adr[0] += NT_DATA_SIZE_14400_A;
      }
      else
      {
#ifdef TRACE_RA_RLP
        TRACE_EVENT("FCS-Error");
        ra_data->rlp->fcs_error++;
#endif
        frame_complete = FALSE;
        len = 0;
      }
      break;
    }
    if (len > 0)
    {
      shm_get_dsp_buffer(&ra_data->rlp->dl_frame_desc, ra_data->shm.shm_addr[adr_ntd_data]+wof_ntd_data, len);
      shm_set_dsp_value (ntd_byte_cnt, 0);

      if (frame_complete)
      {
        l1i_ra_data_ind(hCommRLP);
      }
    }
  }
}

