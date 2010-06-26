/*
+-----------------------------------------------------------------------------
|  Project :  GSM-F&D (8411)
|  Modul   :  RA_FAX
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
|  Purpose :  This Modul defines the functions for the faxnsparent
|             data model for the component RA of the mobile station
+-----------------------------------------------------------------------------
*/

#ifndef RA_FAX_C
#define RA_FAX_C
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

LOCAL const T_SKIP skip_F14400 [4] =
{
  /* T4DATA same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* T4DATA other */  {0, 1, 0, 1, 0, 1, 0, 1}},
  /* STATUS same  */ {{0, 0, 1, 0, 0, 0, 1, 0},
  /* STATUS other */  {1, 1, 1, 0, 1, 1, 1, 0}},
  /* SYNC   same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* SYNC   other */  {1, 0, 1, 0, 1, 0, 1, 0}},
  /* TRAIN  same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* TRAIN  other */  {1, 1, 1, 0, 1, 1, 1, 0}}
};

LOCAL const UBYTE request_F14400 [4] =
{
  5,4,5,4
};

LOCAL const T_SKIP skip_F9600 [4] =
{
  /* T4DATA same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* T4DATA other */  {0, 0, 0, 0, 0, 0, 0, 0}},
  /* STATUS same  */ {{0, 0, 0, 1, 0, 0, 0, 1},
  /* STATUS other */  {0, 1, 1, 1, 0, 1, 1, 1}},
  /* SYNC   same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* SYNC   other */  {0, 0, 0, 0, 0, 0, 0, 0}},
  /* TRAIN  same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* TRAIN  other */  {0, 1, 0, 1, 0, 1, 0, 1}}
};

LOCAL const UBYTE request_F9600 [4] =
{
  3,3,3,3
};

LOCAL const T_SKIP skip_F4800 [4] =
{
  /* T4DATA same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* T4DATA other */  {0, 1, 0, 1, 0, 1, 0, 1}},
  /* STATUS same  */ {{0, 0, 0, 1, 0, 0, 0, 1},
  /* STATUS other */  {0, 1, 1, 1, 0, 1, 1, 1}},
  /* SYNC   same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* SYNC   other */  {0, 1, 0, 1, 0, 1, 0, 1}},
  /* TRAIN  same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* TRAIN  other */  {0, 1, 1, 1, 0, 1, 1, 1}}
};

LOCAL const UBYTE request_F4800 [4] =
{
  2,1,2,1
};

LOCAL const T_SKIP skip_F2400 [4] =
{
  /* T4DATA same  */ {{0, 0, 0, 1, 0, 0, 0, 1},
  /* T4DATA other */  {0, 1, 1, 1, 0, 1, 1, 1}},
  /* STATUS same  */ {{0, 0, 0, 1, 0, 0, 0, 1},
  /* STATUS other */  {0, 1, 1, 1, 0, 1, 1, 1}},
  /* SYNC   same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* SYNC   other */  {0, 1, 1, 1, 0, 1, 1, 1}},
  /* TRAIN  same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* TRAIN  other */  {0, 1, 1, 1, 1, 1, 1, 1}}
};

LOCAL const UBYTE request_F2400 [4] =
{
  1,1,1,1
};

LOCAL const T_SKIP skip_H4800 [4] =
{
  /* T4DATA same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* T4DATA other */  {0, 1, 0, 1, 0, 1, 0, 1}},
  /* STATUS same  */ {{0, 0, 0, 1, 0, 0, 0, 1},
  /* STATUS other */  {0, 1, 1, 1, 0, 1, 1, 1}},
  /* SYNC   same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* SYNC   other */  {0, 1, 0, 1, 0, 1, 0, 1}},
  /* TRAIN  same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* TRAIN  other */  {0, 1, 1, 1, 0, 1, 1, 1}}
};

LOCAL const UBYTE request_H4800 [4] =
{
  2,1,2,1
};

LOCAL const T_SKIP skip_H2400 [4] =
{
  /* T4DATA same  */ {{0, 0, 0, 1, 0, 0, 0, 1},
  /* T4DATA other */  {0, 1, 1, 1, 0, 1, 1, 1}},
  /* STATUS same  */ {{0, 0, 0, 1, 0, 0, 0, 1},
  /* STATUS other */  {0, 1, 1, 1, 0, 1, 1, 1}},
  /* SYNC   same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* SYNC   other */  {0, 1, 1, 1, 0, 1, 1, 1}},
  /* TRAIN  same  */ {{0, 0, 0, 0, 0, 0, 0, 0},
  /* TRAIN  other */  {0, 1, 1, 1, 1, 1, 1, 1}}
};

LOCAL const UBYTE request_H2400 [4] =
{
  1,1,1,1
};

/*==== TYPES ======================================================*/
/*==== VARIABLES ==================================================*/
/*==== FUNCTIONS ==================================================*/

#if defined TRACE_RA_FAX_UL_FRAME_TYPE || defined _SIMULATION_

LOCAL void trace_ra_fax_ul_frame_type(USHORT cus_frame_type, UBYTE* buf1, USHORT bytes_to_write)
{
  const char type[5] = "DSYT";
  char buf[15] = "TISx:0x00-0x00";

  if (cus_frame_type < 4)
    buf[3] = type[cus_frame_type];

  if (buf1)
    BYTE2HEXSTR(*(buf1+1), &buf[7]);

  BYTE2HEXSTR((UBYTE)bytes_to_write, &buf[12]);
  TRACE_EVENT (buf);
}

#endif

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_

LOCAL void trace_ra_fax_dl_frame_type(U8 *last, U8 current, USHORT bytes_read)
{
  void t30_ker_debug(CHAR *header, UBYTE *buf, USHORT len);

  const char type[5] = "DSYT";
  char buf[15] = "TIRx:0x00-0x__";

  if (*last NEQ current)
  {
    if (current < 4)
      buf[3] = type[current];

    TRACE_EVENT (buf);

   *last = current;
  }
  else if (bytes_read)
  {
    if (current EQ 0)
    {
      t30_ker_debug("TIRD", ra_data->fax->dl_pFD->buf, bytes_read);
    }
    else
    {
      if (current < 4)
        buf[3] = type[current];

      BYTE2HEXSTR((UBYTE)bytes_read, &buf[7]);

      if (current EQ 1)
        BYTE2HEXSTR(ra_data->fax->dl_pFD->buf[1], &buf[12]);

      TRACE_EVENT (buf);
    }
  }
}

#endif

LOCAL void fax_init_data(BOOL CMM_flag)
{
  T_FAX *dfax = ra_data->fax;
  /*
   * initialize the fax mode data
   */
  ra_data->sync_timer               = 0L;
  ra_data->cycle_time               = 0L;

  dfax->CMM_flag            = CMM_flag;
  dfax->ul_frame_type_last  = FR_SYNC;
  dfax->ul_frame_type       = FR_SYNC;
  dfax->ul_pFD = NULL;

  cl_ribu_init(&dfax->ul_ribu->idx, RA_FIFO_DEPTH);

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
  dfax->dl_frame_type_last  = 0xFF;
  dfax->dl_state_last = 0xFF;
#endif

  dfax->req_idx = 0;
  dfax->seq_idx = 0;

  dfax->ul_train_FD.buf = dfax->train_seq;
  dfax->ul_train_FD.len = 2;

  ra_data->ra_data_ind.fr_type        = FR_INVALID;
  ra_data->ra_data_ind.sdu.o_buf      = 0;
  ra_data->ra_data_ind.sdu.l_buf      = 0;
  ra_data->ra_ready_ind[0].req_frames = 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_FAX                   |
| STATE   : code                  ROUTINE : fax_init                 |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void fax_init(void)
{
  TRACE_FUNCTION("fax_init");

  fax_init_data(FALSE); /* no CMM */

  hCommFAD = vsi_c_open ( VSI_CALLER FAD_NAME );
  hCommMMI = vsi_c_open ( VSI_CALLER ACI_NAME );
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_FAX                   |
| STATE   : code                  ROUTINE : fax_ul_modify            |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void fax_ul_modify(void)
{
  TRACE_EVENT("fax_ul_modify()");

  fax_init_data(TRUE); /* CMM */
  fax_ul_null();
  return;
}
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_FAX                   |
| STATE   : code                  ROUTINE : fax_idle                 |
+--------------------------------------------------------------------+

  PURPOSE : IDLE processing for uplink and downlink fax mode
*/

GLOBAL void fax_idle(void)
{
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_FAX                   |
| STATE   : code                  ROUTINE : fax_ul_null              |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void fax_ul_null(void)
{
  T_FAX *dfax = ra_data->fax;

  TRACE_FUNCTION("fax_ul_null");

  if (ra_data->activated)
  {
    /*
     * setup the communication parameters
     */
    shm_set_dsp_value (conf_b_itc, V_ITC_FAX);

    switch (ra_data->nsb)
    {
    case 1:
      shm_set_dsp_value (conf_b_nsb, V_NSB_ONE_STOP);
      break;
    case 2:
      shm_set_dsp_value (conf_b_nsb, V_NSB_TWO_STOP);
      break;
    default:
      break;
    }

    switch (ra_data->ndb)
    {
    case 7:
      shm_set_dsp_value (conf_b_ndb, V_NDMSK_B_7_DBITS);
      break;
    case 8:
      shm_set_dsp_value (conf_b_ndb, V_NDMSK_B_8_DBITS);
      break;
    default:
      break;
    }

    shm_set_dsp_value (conf_b_ce, V_CE_TRANSP);

    switch (ra_data->tra_rate)
    {
    case TRA_FULLRATE_14400:
      dfax->skip_seq = &skip_F14400[0];
      dfax->req_seq  = &request_F14400[0];
      shm_set_dsp_value (conf_b_ct, V_CT_FR);
      shm_set_dsp_value (conf_b_ur, V_UR_14400);
      ra_data->cycle_time = 20;
      break;

    case TRA_FULLRATE_9600:
      dfax->skip_seq = &skip_F9600[0];
      dfax->req_seq  = &request_F9600[0];
      shm_set_dsp_value (conf_b_ur, V_UR_9600);
      shm_set_dsp_value (conf_b_ct, V_CT_FR);
      ra_data->cycle_time = 20;
      break;

    case TRA_FULLRATE_4800:
      dfax->skip_seq = &skip_F4800[0];
      dfax->req_seq  = &request_F4800[0];
      shm_set_dsp_value (conf_b_ur, V_UR_4800);
      shm_set_dsp_value (conf_b_ct, V_CT_FR);
      ra_data->cycle_time = 20;
      break;

    case TRA_FULLRATE_2400:
      dfax->skip_seq = &skip_F2400[0];
      dfax->req_seq  = &request_F2400[0];
      shm_set_dsp_value (conf_b_ur, V_UR_2400);
      shm_set_dsp_value (conf_b_ct, V_CT_FR);
      ra_data->cycle_time = 20;
      break;

    case TRA_HALFRATE_4800:
      dfax->skip_seq = &skip_H4800[0];
      dfax->req_seq  = &request_H4800[0];
      shm_set_dsp_value (conf_b_ur, V_UR_4800);
      shm_set_dsp_value (conf_b_ct, V_CT_HR);
      ra_data->cycle_time = 40;
      break;

    case TRA_HALFRATE_2400:
      dfax->skip_seq = &skip_H2400[0];
      dfax->req_seq  = &request_H2400[0];
      shm_set_dsp_value (conf_b_ur, V_UR_2400);
      shm_set_dsp_value (conf_b_ct, V_CT_HR);
      ra_data->cycle_time = 40;
      break;

    default:
      TRACE_EVENT ("Unknown transmission rate");

      ra_data->cycle_time = 20;
      break;
    }

    shm_set_dsp_value (fax_b_data_receive, 1);

    /*
     * set uplink buffer to empty values
     */
    shm_set_dsp_value (faxd_byte_cnt, 0);
    shm_set_dsp_value (faxu_byte_cnt, 0);
    shm_set_dsp_value (faxu_control, V_CTRL_SYNC);

    /*
     * start initialisation process in F&D L1
     */

    /*
     * enable status frame detection in L1
     * and set the biterror thresholds for the status frame detection
     */
    shm_set_dsp_value (fax_b_status_stop,    0);
    shm_set_dsp_value (fax_b_status_detect,  1);
    shm_set_dsp_value (fax_b_ident_thres,    3);
    shm_set_dsp_value (fax_b_info_thres,     3);
    shm_set_dsp_value (fax_b_status_receive, 0);
    shm_set_dsp_value (fax_b_byte_interface, 1);
    shm_set_dsp_value (faxu_byte_cnt, 0);

    shm_set_dsp_value (act_b_init,    1);
    shm_set_dsp_value (act_b_syncul,  1);
    shm_set_dsp_value (act_b_syncdl,  1);

    INIT_STATE (KER, FAX_INIT_L1);

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
    if (dfax->dl_state_last NEQ ra_data->ker.state)
    {
      TRACE_EVENT_P2("KER:FAX_%d -> FAX_%d", dfax->dl_state_last, ra_data->ker.state);
      dfax->dl_state_last = ra_data->ker.state;
    }
#endif

#ifdef HO_WORKAROUND /* fax_ul_null */
    _act_d_ra_conf = *ra_data->shm.shm_addr[d_ra_conf];
    TRACE_EVENT_P1("d_ra_conf saved; rate=%d", ra_data->tra_rate);
#endif
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_FAX                   |
| STATE   : code                  ROUTINE : fax_ul_init_l1           |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void fax_ul_init_l1(void)
{
  TRACE_FUNCTION("fax_ul_init_l1");

  if (shm_get_dsp_value(act_b_init) EQ 0) /* Indicates initialization of IDS module */
  {
    /*
     * L1 F&D initialized; start ul/dl synchronisation
     */
    SET_STATE (KER, FAX_SYNCH_TCH_START);

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
    if (ra_data->fax->dl_state_last NEQ ra_data->ker.state)
    {
      TRACE_EVENT_P2("KER:FAX_%d -> FAX_%d", ra_data->fax->dl_state_last, ra_data->ker.state);
      ra_data->fax->dl_state_last = ra_data->ker.state;
    }
#endif
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_FAX                   |
| STATE   : code                  ROUTINE : fax_dl_sync_tch_start    |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void fax_dl_sync_tch_start(void)
{
  TRACE_FUNCTION("fax_dl_sync_tch_start");

  if (shm_get_dsp_value (statd_b_syncdet) EQ 1)
  {
    /*
     * Detection of sync frames, synchro. on M1 sequence.
     * Start a pseudo timer with 500 ms.
     * The value is decremented by cycle_time
     * each time the L1 calls the dll_data_ul function
     */
    ra_data->sync_timer = 500L;

    SET_STATE (KER, FAX_SYNCH_TCH_FOUND);

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
    if (ra_data->fax->dl_state_last NEQ ra_data->ker.state)
    {
      TRACE_EVENT_P2("KER:FAX_%d -> FAX_%d", ra_data->fax->dl_state_last, ra_data->ker.state);
      ra_data->fax->dl_state_last = ra_data->ker.state;
    }
#endif

  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_FAX                   |
| STATE   : code                  ROUTINE : fax_ul_sync_tch_found    |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void fax_ul_sync_tch_found(void)
{
  T_FAX *dfax = ra_data->fax;

  TRACE_FUNCTION("fax_ul_sync_tch_found");

  if (TimeOut(&ra_data->sync_timer) NEQ TRUE)
    return;

  /*
   * timeout of the sync timer - reset the sync-detection flags
   */
  shm_set_dsp_value (act_b_syncul,  0);
  shm_set_dsp_value (act_b_syncdl,  0);

  if (ra_data->tra_rate NEQ TRA_FULLRATE_14400)
  {
    /*
     * setup uplink buffer to send SYNC frames
     */
    shm_set_dsp_value (faxu_byte_cnt, 0);
    shm_set_dsp_value (faxu_control, V_CTRL_SYNC);
    dfax->seq_idx = 0;
    SET_STATE (KER, FAX_WAIT_SYNC_LOST);

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
    if (dfax->dl_state_last NEQ ra_data->ker.state)
    {
      TRACE_EVENT_P2("KER:FAX_%d -> FAX_%d", dfax->dl_state_last, ra_data->ker.state);
      dfax->dl_state_last = ra_data->ker.state;
    }
#endif

  }
  else /* ra_data->tra_rate EQ TRA_FULLRATE_14400 */
  {
    /*
     * sync detection finished -> enter the data transmission (pre-) state
     */
    if (dfax->CMM_flag)
    {
      l1i_ra_modify_cnf();
      SET_STATE (KER, FAX_DATA_TRANS);
    }
    else
    {
      l1i_ra_activate_cnf();
      SET_STATE (KER, FAX_DATA_TRANS_PRE);
    }

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
    if (dfax->dl_state_last NEQ ra_data->ker.state)
    {
      TRACE_EVENT_P2("KER:FAX_%d -> FAX_%d", dfax->dl_state_last, ra_data->ker.state);
      dfax->dl_state_last = ra_data->ker.state;
    }
#endif

#ifdef HO_WORKAROUND /* fax_ul_sync_tch_found */
    if (*ra_data->shm.shm_addr[d_ra_conf] NEQ _act_d_ra_conf)
    {
      *ra_data->shm.shm_addr[d_ra_conf] = _act_d_ra_conf;
      TRACE_EVENT("HO_REFRESH fax_ul_sync_tch_found");
    }
#endif

  }
}

LOCAL void IncIndex(UBYTE *idx, UBYTE idxMax)
{
  if (++(*idx) EQ idxMax)
    *idx = 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_FAX                   |
| STATE   : code                  ROUTINE : fax_dl_wait_sync_lost    |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void fax_dl_wait_sync_lost(void)
{
  T_FAX *dfax = ra_data->fax;

  TRACE_FUNCTION("fax_dl_wait_sync_lost");

  IncIndex(&dfax->seq_idx, 8);

  if (shm_get_dsp_value (statd_b_syncdet) EQ 0)
  {
    /*
     * sync detection finished -> enter the data transmission (pre-) state
     */
    if (dfax->CMM_flag)
    {
      l1i_ra_modify_cnf();
      SET_STATE (KER, FAX_DATA_TRANS);
    }
    else
    {
      l1i_ra_activate_cnf();
      SET_STATE (KER, FAX_DATA_TRANS_PRE);
    }

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
    if (dfax->dl_state_last NEQ ra_data->ker.state)
    {
      TRACE_EVENT_P2("KER:FAX_%d -> FAX_%d", dfax->dl_state_last, ra_data->ker.state);
      dfax->dl_state_last = ra_data->ker.state;
    }
#endif

#ifdef HO_WORKAROUND /* fax_dl_wait_sync_lost */
    if (*ra_data->shm.shm_addr[d_ra_conf] NEQ _act_d_ra_conf)
    {
      *ra_data->shm.shm_addr[d_ra_conf] = _act_d_ra_conf;
      TRACE_EVENT("HO_REFRESH fax_dl_wait_sync_lost");
    }
#endif

  }
}

#ifndef _SIMULATION_
LOCAL void check_skip(USHORT fed_ul, BOOL *skip)
{
  if (*skip AND !fed_ul)
  {
    *skip = FALSE;
#ifdef TRACE_RA_FAX_UL_SKIP
    TRACE_EVENT_P1("fed_ul=%d -> skip -> FALSE", fed_ul);
#endif
  }
  else if (!*skip AND fed_ul)
  {
    *skip = TRUE;
#ifdef TRACE_RA_FAX_UL_SKIP
    TRACE_EVENT_P1("fed_ul=%d-> skip -> TRUE", fed_ul);
#endif
  }
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_FAX                   |
| STATE   : code                  ROUTINE : fax_ul_data_trans        |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void fax_ul_data_trans(void)
{
  T_FAX *dfax = ra_data->fax;
  USHORT bytes_used_after = 0;
  BOOL skip;

  USHORT fed_ul = shm_get_dsp_value(statu_b_fed_ul);

  TRACE_FUNCTION ("fax_ul_data_trans");

#if defined TRACE_RA_FAX_UL_FRAME_TYPE || defined _SIMULATION_
  if (dfax->ul_ribu->idx.filled > 1)
    TRACE_EVENT_P1("dfax->ul_ribu->idx.filled=%d", dfax->ul_ribu->idx.filled);
#endif

  if (dfax->ul_ribu->idx.filled)
  {
    dfax->ul_pFD = cl_ribu_get(dfax->ul_ribu);
    dfax->ul_frame_type = dfax->ul_pFD->type;
  }

#if defined TRACE_RA_FAX_UL_FRAME_TYPE || defined _SIMULATION_
  else
  {
    TRACE_EVENT_P1("dfax->ul_ribu->idx.filled=%d", dfax->ul_ribu->idx.filled);
  }
#endif

  /*
   * Check if we must skip this cycle.
   * For status frames this is done every third status frame,
   * because L1 sends more data (1,2,4,6) * 64 bit status frames.
   * for T4DATA frames this also happens in 2400 bps mode
   */
  /*
   * check if the frame type has changed
   */
  if (dfax->ul_frame_type_last NEQ dfax->ul_frame_type)
  {
    skip = (dfax->skip_seq[dfax->ul_frame_type_last].skip_seq_other[dfax->seq_idx] NEQ 0);

#ifndef _SIMULATION_
    check_skip(fed_ul, &skip);
#endif

    if ((dfax->ul_frame_type_last EQ FR_TRAIN) AND (dfax->train_len > 0))
    {
      /* Training is stopped only after a minimum length */
      dfax->train_len--;
      skip = TRUE;
    }

    if (!skip)
    {
      /*
       * we are on a 64 bit boundary so we can write into the new frame. Reset all sequencers.
       */
      dfax->ul_frame_type_last = dfax->ul_frame_type;
      dfax->seq_idx = 1;

      /*
       * start the request sequencer for T4DATA
       */
      if (dfax->ul_frame_type EQ FR_T4DATA)
      {
        dfax->req_idx = 1;
      }
      else
      {
        dfax->req_idx = 0;

        /*
         * Store training sequence. It will be used later, when changing
         * to T4 data, in order to avoid insertion of sync frames.
         */
        if (dfax->ul_frame_type EQ FR_TRAIN)
        {
          dfax->train_seq[0] = dfax->ul_pFD->buf[0];
          dfax->train_seq[1] = dfax->ul_pFD->buf[1];
          dfax->train_len = MIN_FR_TRAIN_LEN - 1; /* Minimum length of training */
        }
      }
    }
    else /* skip */
    {
      IncIndex(&dfax->seq_idx, 8);

      if (dfax->ul_frame_type_last EQ FR_T4DATA)
      {
        IncIndex(&dfax->req_idx, 4);
      }
    }
  }
  else /* frame type not changed */
  {
    skip = (dfax->skip_seq[dfax->ul_frame_type_last].skip_seq_same[dfax->seq_idx] NEQ 0);

#ifndef _SIMULATION_
    check_skip(fed_ul, &skip);
#endif

    IncIndex(&dfax->seq_idx, 8);

    if (dfax->ul_frame_type EQ FR_T4DATA)
    {
      IncIndex(&dfax->req_idx, 4);
    }
    else if ((dfax->ul_frame_type_last EQ FR_TRAIN) AND (dfax->train_len > 0))
    {
      /* Training is stopped only after a minimum length */
      dfax->train_len--;
    }
  }

  if (!skip)
  {
    switch (dfax->ul_frame_type)
    {
      case FR_T4DATA:
        shm_set_dsp_value (faxu_control, V_CTRL_DATA);
        break;

      case FR_STATUS:
        shm_set_dsp_value (faxu_control, V_CTRL_STATUS);
        break;

      case FR_TRAIN:
        shm_set_dsp_value (faxu_control, V_CTRL_TRAIN);
        break;

      case FR_SYNC:
        shm_set_dsp_value (faxu_control, V_CTRL_SYNC);
        shm_set_dsp_value (faxu_byte_cnt, 0);
        break;
    }
    if (dfax->ul_frame_type NEQ FR_SYNC)
    {
      bytes_used_after = shm_set_dsp_buffer_new(dfax->ul_pFD, ra_data->shm.shm_addr[adr_faxu_data]+wof_faxu_data, 0, (USHORT)MAX_FAX_BUFFER_UL);
      shm_set_dsp_value (faxu_byte_cnt, bytes_used_after);
    }

#if defined TRACE_RA_FAX_UL_FRAME_TYPE || defined _SIMULATION_
    if (dfax->ul_pFD)
      trace_ra_fax_ul_frame_type(dfax->ul_frame_type, dfax->ul_pFD->buf, bytes_used_after);
#endif

    ra_data->ra_ready_ind[0].req_frames = dfax->req_seq[dfax->req_idx];
    l1i_ra_ready_ind(hCommFAD, 0);
  }
  else if (dfax->ul_frame_type_last EQ FR_TRAIN) /* skip AND train*/
  {
    shm_set_dsp_value (faxu_control, V_CTRL_TRAIN);
    bytes_used_after = shm_set_dsp_buffer_new(&dfax->ul_train_FD, ra_data->shm.shm_addr[adr_faxu_data]+wof_faxu_data, 0, (USHORT)MAX_FAX_BUFFER_UL);
    shm_set_dsp_value (faxu_byte_cnt, bytes_used_after);

#if defined TRACE_RA_FAX_UL_FRAME_TYPE || defined _SIMULATION_
    trace_ra_fax_ul_frame_type(V_CTRL_TRAIN, dfax->ul_train_FD.buf, bytes_used_after);
#endif

  }
  else /* skip */
  {
    shm_set_dsp_value (faxu_byte_cnt, 0);

#if defined TRACE_RA_FAX_UL_FRAME_TYPE || defined _SIMULATION_
    trace_ra_fax_ul_frame_type(4, NULL, 0);
#endif

  }
}

LOCAL U8 get_frame_descriptor(U8 bytes_to_read)
{
  T_FAX *dfax = ra_data->fax;

  U8 bytes_read;

#ifdef _SIMULATION_
  dfax->dl_pFD = cl_ribu_get_new_frame_desc(ra_data->dl_ribu);
#else
  dfax->dl_pFD = fad_rbm_get_next_FrameDesc();
#endif

  dfax->dl_pFD->len = FAD_DESCBUF_LEN;
  bytes_read = shm_get_dsp_buffer_new(ra_data->shm.shm_addr[adr_faxd_data]+wof_faxd_data, bytes_to_read, dfax->dl_pFD);

  return bytes_read;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_FAX                   |
| STATE   : code                  ROUTINE : fax_dl_data_trans        |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void fax_dl_data_trans(void)
{
  UBYTE bytes_to_read, bytes_read = 0;
  UBYTE dl_frame_type;

  T_FAX *dfax = ra_data->fax;

  TRACE_FUNCTION ("fax_dl_data_trans");

#ifdef HO_WORKAROUND /* fax_dl_data_trans */
  if (*ra_data->shm.shm_addr[d_ra_conf] NEQ _act_d_ra_conf)
  {
    *ra_data->shm.shm_addr[d_ra_conf] = _act_d_ra_conf;
    TRACE_EVENT("HO_REFRESH fax_dl_data_trans");
  }
#endif

  dl_frame_type = shm_get_dsp_value (faxd_control);

  if (dl_frame_type EQ V_CTRL_SYNC)
  {
    bytes_read = get_frame_descriptor(0);
    dfax->dl_pFD->len = bytes_read;
    dfax->dl_pFD->type = dl_frame_type;
    l1i_ra_data_ind(hCommFAD);

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
    trace_ra_fax_dl_frame_type(&dfax->dl_frame_type_last, dl_frame_type, bytes_read);
#endif

  }
  else
  {
    bytes_to_read = shm_get_dsp_value (faxd_byte_cnt);

    if (bytes_to_read > 0)
    {
      bytes_read = get_frame_descriptor(bytes_to_read);
      shm_set_dsp_value (faxd_byte_cnt, 0);
    }

    if (bytes_read)
    {
      dfax->dl_pFD->len = bytes_read;
      dfax->dl_pFD->type = dl_frame_type;
      l1i_ra_data_ind(hCommFAD);

#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
      trace_ra_fax_dl_frame_type(&dfax->dl_frame_type_last, dl_frame_type, bytes_read);
#endif

    }
  }
#if defined TRACE_RA_FAX_DL_FRAME_TYPE || defined _SIMULATION_
  dfax->dl_frame_type_last = dl_frame_type;
#endif
}
