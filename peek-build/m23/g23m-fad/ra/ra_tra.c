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
|  Purpose :  This Modul defines the functions for the transparent data model
|             for the component RA of the mobile station
+-----------------------------------------------------------------------------
*/

#ifndef RA_TRA_C
#define RA_TRA_C
#endif

#define ENTITY_RA

/*==== INCLUDES ===================================================*/

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
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_init                 |
+--------------------------------------------------------------------+

  PURPOSE : initialize the transparent mode data
*/

GLOBAL void tra_init (UBYTE tra_rate)
{
  T_TRA *dtra = ra_data->tra;

  TRACE_FUNCTION("tra_init()");

  dtra->ul_data_pending    = FALSE;
  dtra->ul_signals_pending = FALSE;
  dtra->ul_break_pending   = FALSE;

  dtra->break_pos = 0;
  dtra->break_len = 0;
  dtra->overspeed = 0;
  dtra->ul_status = 0;
  dtra->ul_sa     = 0;
  dtra->ul_sb     = 0;
  dtra->ul_x      = 0;

  dtra->dl_sa.pos   = ST_SA;  /* DSR */
  dtra->dl_sa.last  = 0xFF;
  dtra->dl_sa.timer = 0L;

#ifdef _SIMULATION_  /* according to GSM-TS07.01 */

  dtra->dl_sa.delay_OFF_ON = 0L;
  dtra->dl_sa.delay_ON_OFF = 0L;

#else /* to avoid spurious data at the beginning of the call */

  dtra->dl_sa.delay_OFF_ON = 5000L;
  dtra->dl_sa.delay_ON_OFF = 1000L;

#endif

  dtra->dl_sb.pos          = ST_SB;  /* DCD */
  dtra->dl_sb.last         = 0xFF;
  dtra->dl_sb.timer        = 0L;
  dtra->dl_sb.delay_OFF_ON =  200L;
  dtra->dl_sb.delay_ON_OFF = 5000L;

  dtra->dl_x.pos           = ST_X;   /* CTS */
  dtra->dl_x.last          = 0xFF;
  dtra->dl_x.timer         = 0L;
  dtra->dl_x.delay_OFF_ON  = 1000L;
  dtra->dl_x.delay_ON_OFF  = 1000L;

  ra_data->ra_data_ind.status = ST_SA + ST_SB + ST_X;

  ra_data->ra_data_ind.fr_type    = FR_TRANS;
  ra_data->ra_data_ind.sdu.o_buf  = 0;
  ra_data->ra_data_ind.sdu.l_buf  = 0;

  ra_data->ra_ready_ind[0].req_frames = 0; /* used, if not all data sent yet */
  dtra->ready_ind_idx = 1;

  switch (tra_rate)
  {
  case TRA_FULLRATE_14400:
    ra_data->ra_ready_ind[1].req_frames = RAU_DATA_14400;
    break;
  case TRA_FULLRATE_4800:
    ra_data->ra_ready_ind[1].req_frames = RAU_DATA_4800;
    break;
  case TRA_HALFRATE_4800:
  case TRA_FULLRATE_2400:
    ra_data->ra_ready_ind[1].req_frames = RAU_DATA_2400;
    break;
  case TRA_HALFRATE_2400:
    ra_data->ra_ready_ind[1].req_frames = RAU_DATA_1200;
    break;
  default:
    ra_data->ra_ready_ind[1].req_frames = RAU_DATA_9600;
    break;
  }

  cl_set_frame_desc_0(&dtra->ul_frame_desc, NULL, 0);

  hCommMMI = vsi_c_open ( VSI_CALLER ACI_NAME );
  hCommTRA = vsi_c_open ( VSI_CALLER TRA_NAME );
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_idle                 |
+--------------------------------------------------------------------+

  PURPOSE : IDLE processing fpr uplink and downlink transparent mode


*/

GLOBAL void tra_idle(void)
{
  TRACE_FUNCTION("tra_idle()");
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_ul_null              |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void tra_ul_null(void)
{
  TRACE_FUNCTION("tra_ul_null()");

  if (ra_data->activated)
  {
    /*
     * setup the communication parameters
     */
    shm_set_dsp_value (conf_b_itc, V_ITC_DATA);
    shm_set_dsp_value (test_b_t_dl_debug, 0);

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

    switch (ra_data->user_rate)
    {
      case URA_300:
        shm_set_dsp_value (conf_b_ur, V_UR_300);
        break;

      case URA_1200:
        shm_set_dsp_value (conf_b_ur, V_UR_1200);
        break;

      case URA_2400:
        shm_set_dsp_value (conf_b_ur, V_UR_2400);
        break;

      case URA_4800:
        shm_set_dsp_value (conf_b_ur, V_UR_4800);
        break;

      case URA_9600:
        /* hack for 14400: */
        if (ra_data->tra_rate EQ TRA_FULLRATE_14400)
        {
          shm_set_dsp_value (conf_b_ur, V_UR_14400);
        }
        else
        {
          shm_set_dsp_value (conf_b_ur, V_UR_9600);
        }
        break;

      case URA_1200_75:
        shm_set_dsp_value (conf_b_ur, V_UR_1200_75);
        break;

      case URA_14400:
        shm_set_dsp_value (conf_b_ur, V_UR_14400);
        break;

      default:
        break;
    }

    shm_set_dsp_value (conf_b_ce, V_CE_TRANSP);

    switch (ra_data->tra_rate)
    {
      case TRA_FULLRATE_14400:
      case TRA_FULLRATE_9600:
      case TRA_FULLRATE_4800:
      case TRA_FULLRATE_2400:
        shm_set_dsp_value (conf_b_ct, V_CT_FR);
        ra_data->cycle_time = 20;
        break;

      case TRA_HALFRATE_4800:
      case TRA_HALFRATE_2400:
        shm_set_dsp_value (conf_b_ct, V_CT_HR);
        ra_data->cycle_time = 40;
        break;

      default:
        break;
    }

    /*
     * set uplink buffer empty to empty values and no break
     */
    shm_set_dsp_value (rau_byte_cnt,  2);
    shm_set_dsp_value (rau_break_len, 0);
    shm_set_dsp_value (rau_break_pos, 0);

    /*
     * start initialisation process in F&D L1
     */
    shm_set_dsp_value (act_b_init,   1);
    shm_set_dsp_value (act_b_syncul, 1);
    shm_set_dsp_value (act_b_syncdl, 1);

    INIT_STATE (KER, TRA_INIT_L1);

#ifdef HO_WORKAROUND /* tra_ul_null */
    _act_d_ra_conf = *ra_data->shm.shm_addr[d_ra_conf];
    TRACE_EVENT("d_ra_conf saved");
#endif

  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_ul_init_l1           |
+--------------------------------------------------------------------+

  PURPOSE :


*/

GLOBAL void tra_ul_init_l1(void)
{
  TRACE_FUNCTION("tra_ul_init_l1()");

  if (shm_get_dsp_value (act_b_init) EQ 0)
  {
    /*
     * L1 F&D initialzed
     * start ul/dl synchronisation
     */

    SET_STATE (KER, TRA_SYNCH_TCH_START);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_dl_sync_tch_start    |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void tra_dl_sync_tch_start(void)
{
  TRACE_FUNCTION("tra_dl_sync_tch_start()");

  if (shm_get_dsp_value (statd_b_syncdet) EQ 1)
  {
    /*
     * Start a pseudo timer with 500 ms.
     * The value is decremented by cycle_time
     * each time the L1 calls the dll_data_ul function.
     */
    TRACE_EVENT("SYNC_TIMER: start");
    ra_data->sync_timer = 500L;
    shm_set_dsp_value (rad_byte_cnt,  2);
    SET_STATE (KER, TRA_SYNCH_TCH_FOUND);
  }
}

LOCAL void set_ul_status(T_TRA *tra)
{
shm_set_dsp_value (statu_b_sa, tra->ul_sa);
shm_set_dsp_value (statu_b_sb, tra->ul_sb);
shm_set_dsp_value (statu_b_x,  tra->ul_x);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_ul_sync_tch_found    |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void tra_ul_sync_tch_found(void)
{
  TRACE_FUNCTION("tra_ul_sync_tch_found()");

  if (TimeOut(&ra_data->sync_timer) NEQ TRUE)
    return;

  /*
   * timeout of the sync timer - reset the sync-detection flags
   */
  TRACE_EVENT("SYNC_TIMER: time out");

  shm_set_dsp_value (act_b_syncul,  0);
  shm_set_dsp_value (act_b_syncdl,  0);

  if (ra_data->tra_rate NEQ TRA_FULLRATE_14400)
  {
    ra_data->sync_timer = 10000L;         /* 10 secs timeout */
    SET_STATE (KER, TRA_WAIT_SYNC_LOST);
  }
  else
  {
    SET_STATE (KER, TRA_DATA_TRANS_PRE1);

#ifdef HO_WORKAROUND /* tra_ul_sync_tch_found */
  if (*ra_data->shm.shm_addr[d_ra_conf] NEQ _act_d_ra_conf)
  {
    *ra_data->shm.shm_addr[d_ra_conf] = _act_d_ra_conf; /* tra_dl_data_trans */
    TRACE_EVENT("HO_REFRESH tra_ul_sync_tch_found");
  }
#endif

  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_dl_wait_sync_lost    |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void tra_dl_wait_sync_lost(void)
{
  BOOL timeout;

  TRACE_FUNCTION("tra_dl_wait_sync_lost()");

  timeout = TimeOut(&ra_data->sync_timer);

  if (timeout EQ TRUE)
  {
    TRACE_EVENT("SYNC_TIMER: statd_b_syncdet remains HIGH");
  }

  if (shm_get_dsp_value (statd_b_syncdet) EQ 0 OR timeout EQ TRUE)
  {
    /*
     * sync detection finished -> enter the data transmission state
     * send ra_activate_cnf -> ACI
     */
    SET_STATE (KER, TRA_DATA_TRANS_PRE1);
    set_ul_status(ra_data->tra);

#ifdef HO_WORKAROUND /* tra_dl_wait_sync_lost */
  if (*ra_data->shm.shm_addr[d_ra_conf] NEQ _act_d_ra_conf)
  {
    *ra_data->shm.shm_addr[d_ra_conf] = _act_d_ra_conf; /* tra_dl_data_trans */
    TRACE_EVENT("HO_REFRESH tra_dl_wait_sync_lost");
  }
#endif

  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_ul_data_trans        |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void tra_ul_data_trans(void)
{
  T_TRA *dtra = ra_data->tra;

  TRACE_FUNCTION("tra_ul_data_trans()");

  if (dtra->ul_signals_pending)
  {
    /*
     * handle sa, sb, x signals
     */
    set_ul_status(dtra);

    shm_set_dsp_value (act_b_ovspul, dtra->overspeed);
  }

  if (dtra->ul_data_pending)
  {
    USHORT offs, new_offs;

    offs = shm_get_dsp_value (rau_byte_cnt);

    if (offs > 0)
    {
      offs -= 2;

      if (dtra->ul_break_pending)
      {
        shm_set_dsp_value (rau_break_pos, dtra->break_pos);
        shm_set_dsp_value (rau_break_len, dtra->break_len);
      }
      else
      {
        shm_set_dsp_value (rau_break_pos, 0);
        shm_set_dsp_value (rau_break_len, 0);
      }

      new_offs = shm_set_dsp_buffer(&dtra->ul_frame_desc,
                                     ra_data->shm.shm_addr[adr_rau_data]+wof_rau_data,
                                     offs,
                                     (USHORT)(ra_data->ra_ready_ind[1].req_frames - offs));
      if (new_offs NEQ offs)
      {
        shm_set_dsp_value (rau_byte_cnt, (USHORT)(new_offs+2));
      }

      if (dtra->ul_frame_desc.Len[0] EQ 0)
      {
        dtra->ul_data_pending = FALSE;
        dtra->ready_ind_idx = 1;
      }
      else
      {
        dtra->ul_frame_desc.Adr[0] += (new_offs-offs);
      }
    }
  }
  if (!dtra->ul_data_pending OR dtra->ul_signals_pending)
  {
    if (dtra->ul_signals_pending AND !dtra->ul_data_pending)
    {
      dtra->ready_ind_idx = 1;
    }
    if (dtra->data_req_rec) /* send only if RA_DATA_REQ received */
    {
      l1i_ra_ready_ind(hCommTRA, dtra->ready_ind_idx);
      dtra->data_req_rec = FALSE;
      dtra->ready_ind_idx = 0;
      dtra->ul_signals_pending = FALSE;
    }
  }
}

LOCAL UBYTE check_status_bit(T_STATUS_BIT *status)
{
  if (status->last NEQ status->current)
  {
    status->last = status->current;

    if (status->current) /* current status bit is OFF */
    {
      if (status->delay_ON_OFF)
      {
        status->timer = status->delay_ON_OFF; /* start ON to OFF timer */
        return 2+1;
      }
      else
      {
        ra_data->ra_data_ind.status |= status->pos; /* status bit OFF */
        return 4+1;
      }
    }
    else /* current status bit is ON */
    {
      if (status->delay_OFF_ON)
      {
        status->timer = status->delay_OFF_ON; /* start OFF to ON timer */
        return 2+0;
      }
      else
      {
        ra_data->ra_data_ind.status &= ~status->pos; /* status bit ON */
        return 4+0;
      }
    }
  }
  else /* status->last EQ status->current */
  {
    if (TimeOut(&status->timer) EQ TRUE)
    {
      if (status->current) /* current status bit is OFF */
      {
        ra_data->ra_data_ind.status |= status->pos; /* filtered status bit OFF */
        return 4+1;
      }
      else
      {
        ra_data->ra_data_ind.status &= ~status->pos; /* filtered status bit ON */
        return 4+0;
      }
    }
  }
  return 0;
}

LOCAL BOOL get_dl_status_bits(void)
{
  T_TRA *dtra = ra_data->tra;
  BOOL dl_status_changed = FALSE;
  UBYTE ret;

  /*** status bit SA ***/

  dtra->dl_sa.current = (UBYTE)shm_get_dsp_value(statd_b_sa);
  ret = check_status_bit(&dtra->dl_sa);
  if (ret > 3)
    dl_status_changed = TRUE;

#ifdef TRACE_RA_TRA_STATUS
  switch (ret)
  {
  case 2: TRACE_EVENT("SA bit timer started 1 -> 0"); break;
  case 3: TRACE_EVENT("SA bit timer started 0 -> 1"); break;
  case 4: TRACE_EVENT("CT107-DSR=ON"); break;
  case 5: TRACE_EVENT("CT107-DSR=OFF"); break;
  }
#endif

  if (!(ra_data->ra_data_ind.status & ST_SA)) /* DSR EQ ON */
  {
    /*** perform filtering of status bit SB ***/

    dtra->dl_sb.current = (UBYTE)shm_get_dsp_value(statd_b_sb);
    ret = check_status_bit(&dtra->dl_sb);
    if (ret > 3)
      dl_status_changed = TRUE;

#ifdef TRACE_RA_TRA_STATUS
    switch (ret)
    {
    case 2: TRACE_EVENT("SB bit timer started 1 -> 0"); break;
    case 3: TRACE_EVENT("SB bit timer started 0 -> 1"); break;
    case 4: TRACE_EVENT("CT109-DCD=ON"); break;
    case 5: TRACE_EVENT("CT109-DCD=OFF"); break;
    }
#endif

    /*** perform filtering of status bit X ***/

    dtra->dl_x.current = (UBYTE)shm_get_dsp_value(statd_b_x);
    ret = check_status_bit(&dtra->dl_x);
    if (ret > 3)
      dl_status_changed = TRUE;

#ifdef TRACE_RA_TRA_STATUS
    switch (ret)
    {
    case 2: TRACE_EVENT("X bit timer started 1 -> 0"); break;
    case 3: TRACE_EVENT("X bit timer started 0 -> 1"); break;
    case 4: TRACE_EVENT("CT106-CTS=ON"); break;
    case 5: TRACE_EVENT("CT106-CTS=OFF"); break;
    }
#endif

  }
  else if (dl_status_changed EQ TRUE) /* DSR EQ OFF */
  {
    ra_data->ra_data_ind.status |= ST_SB + ST_X;
    dtra->dl_sb.last = 0xFF;
    dtra->dl_x.last  = 0xFF;

#ifdef TRACE_RA_TRA_STATUS
    TRACE_EVENT("CT106-CTS=OFF");
    TRACE_EVENT("CT109-DCD=OFF");
#endif
  }

  /*
   * overwrite the downlink CTS bit;
   * allows UART the reception of escape sequence always;
   * no matter what the current call state may be
  ra_data->ra_data_ind.status &= ~ST_X;
   */
  return dl_status_changed;
}

LOCAL U8 get_frame_descriptor(U8 bytes_to_read)
{
  T_TRA *dtra = ra_data->tra;

#ifdef _SIMULATION_
  dtra->dl_pFD = cl_ribu_get_new_frame_desc(ra_data->dl_ribu);
#else
  dtra->dl_pFD = tra_get_next_FrameDesc();
#endif


  if (NULL EQ dtra->dl_pFD)
  {
    /*Race condition where there is a data from L1 and call is released parallely*/
    TRACE_EVENT("Call is Released when data arrived or Ribu write index is invalid");

    /*return as if no data is read from L1.*/
    return 0;
  }


  if (bytes_to_read)
  {
    dtra->dl_pFD->len = MAX_TRANS_BUFFER;
    return (shm_get_dsp_buffer_new(ra_data->shm.shm_addr[adr_rad_data]+wof_rad_data, bytes_to_read, dtra->dl_pFD));
  }
  else
  {
    dtra->dl_pFD->len = 0;
    return 0;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_dl_wait_for_connect  |
+--------------------------------------------------------------------+

  PURPOSE : wait for all filtered status bits set to 0
*/

GLOBAL void tra_dl_wait_for_connect(void)
{
  TRACE_FUNCTION("tra_dl_wait_for_connect()");

#ifdef HO_WORKAROUND /* tra_dl_wait_for_connect */
  if (*ra_data->shm.shm_addr[d_ra_conf] NEQ _act_d_ra_conf)
  {
    *ra_data->shm.shm_addr[d_ra_conf] = _act_d_ra_conf; /* tra_dl_data_trans */
    TRACE_EVENT("HO_REFRESH tra_dl_data_trans");
  }
#endif

  get_dl_status_bits();
  if ((UBYTE)(ra_data->ra_data_ind.status AND (ST_SA + ST_SB + ST_X)) EQ 0)
  {
    l1i_ra_activate_cnf();
    SET_STATE (KER, TRA_DATA_TRANS_PRE2);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_dl_wait_for_uart     |
+--------------------------------------------------------------------+

  PURPOSE : wait for DTI connection to UART
*/

GLOBAL void tra_dl_wait_for_uart(void)
{
  TRACE_FUNCTION("tra_dl_wait_for_uart()");

  get_dl_status_bits();
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_dl_got_uart          |
+--------------------------------------------------------------------+

  PURPOSE : send the current downlink status bits to UART
*/

GLOBAL void tra_dl_got_uart(void)
{
  TRACE_FUNCTION("tra_dl_got_uart()");

  get_dl_status_bits();
  get_frame_descriptor(0);
  l1i_ra_data_ind(hCommTRA);
  SET_STATE (KER, TRA_DATA_TRANS);
}

LOCAL void get_dl_bytes(UBYTE bytes_to_read)
{
  T_TRA *dtra = ra_data->tra;

  UBYTE bytes_read;

  bytes_read = get_frame_descriptor(bytes_to_read);
  shm_set_dsp_value (rad_byte_cnt, 2); /* confirm read operation */

  if ( bytes_read EQ 0 ){
    /*We couldn't read the data, in get_frame_descriptor() there is a possibiliy
     we get NULL frame description (race condition) hence no data is read. In this case we should not 
     access dtra->dl_pFD pointer and as well donot indicate ra_data_ind, hence return.
    */
    return;
  }


  if (ra_data->ra_data_ind.status & ST_SA OR ra_data->ra_data_ind.status & ST_SB)
  {
    /*
    DSR is OFF OR DCD is OFF
    */
    dtra->dl_pFD->len = 0;
#ifdef TRACE_RA_TRA_DATA
    TRACE_EVENT_P1("DATA_IND - len=0:%c", *dtra->dl_pFD->buf);
#endif
  }
  else
  {
    dtra->dl_pFD->len = bytes_read;
#ifdef TRACE_RA_TRA_DATA
    TRACE_EVENT_P2("DATA_IND - len=%d:%c", bytes_read, *dtra->dl_pFD->buf);
#endif
  }
  l1i_ra_data_ind(hCommTRA);
}

LOCAL BOOL dl_break_detected(BOOL *dl_status_changed, UBYTE *bytes_to_read)  /* BREAK handling */
{
  T_TRA *dtra = ra_data->tra;

  UBYTE brklen;
  UBYTE brkpos;
  UBYTE bytes_read;

  if ((brklen = (UBYTE)shm_get_dsp_value(rad_break_len)) > 0)
  {
    TRACE_EVENT_P1("RA_BREAK_IND-brklen=%d", brklen);

    /*** handling of 0xff break over multiple frames not implemented yet ***/

    if ((brkpos = (UBYTE)shm_get_dsp_value(rad_break_pos)) > 0)
    {
      TRACE_EVENT_P1("brkpos=%d", brkpos);
      /*
       * If the position > 0 read out the data until the break occurs.
       */
      if (*bytes_to_read > 0)
      {
        TRACE_EVENT_P1("bytes_to_read=%d", *bytes_to_read);

        bytes_read = get_frame_descriptor(brkpos);
        bytes_to_read -= bytes_read;
        dtra->dl_pFD->len = bytes_read;

        TRACE_EVENT ("DATA_IND -  break handling");
        dtra->dl_pFD->len = 0;
        l1i_ra_data_ind(hCommTRA);
      }
    }

    shm_set_dsp_value (rad_byte_cnt, 2); /* confirm read operation */

#ifdef HO_WORKAROUND /* tra_dl_data_trans - "BREAK" */

    /*
    enforce new DSR status bit filtering time
    */
    dtra->dl_sa.delay_OFF_ON = 1000L;
    dtra->dl_sa.delay_ON_OFF = 0L;

    dtra->dl_sa.last = 0xFF;
    dtra->dl_sb.last = 0xFF;
    dtra->dl_x.last  = 0xFF;

    ra_data->ra_data_ind.status = ST_SA + ST_SB + ST_X;

    *dl_status_changed = TRUE;

#else

    dtra->ra_break_ind.break_len = brklen;
    l1i_ra_break_ind();

#endif

    if (*dl_status_changed EQ TRUE)
    {
      get_frame_descriptor(0);
      l1i_ra_data_ind(hCommTRA);
    }

    return TRUE;
  } /* brklen > 0 */

  return FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_TRA                   |
| STATE   : code                  ROUTINE : tra_dl_data_trans        |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL void tra_dl_data_trans(void)
{
  UBYTE bytes_to_read;
  BOOL dl_status_changed;

  TRACE_FUNCTION("tra_dl_data_trans()");

#ifdef HO_WORKAROUND /* tra_dl_data_trans */
  if (*ra_data->shm.shm_addr[d_ra_conf] NEQ _act_d_ra_conf)
  {
    *ra_data->shm.shm_addr[d_ra_conf] = _act_d_ra_conf; /* tra_dl_data_trans */
    TRACE_EVENT("HO_REFRESH tra_dl_data_trans");
  }
#endif

  dl_status_changed = get_dl_status_bits();

  /*** read out the received downlink data ***/

  bytes_to_read = shm_get_dsp_value (rad_byte_cnt);

  if (bytes_to_read > 0)
  {
    bytes_to_read -= 2; /* ignore header */

    if (dl_break_detected(&dl_status_changed, &bytes_to_read))
      return;

    if (bytes_to_read > 0 AND bytes_to_read <= MAX_TRANS_BUFFER)
    {
      get_dl_bytes(bytes_to_read);
    }
    else  /* NO bytes_to_read */
    {
      if (dl_status_changed EQ TRUE)
      {
        get_frame_descriptor(0);
        l1i_ra_data_ind(hCommTRA);
      }
    }
  }
  else /* NO bytes_to_read -- will never happen */
  {
    if (dl_status_changed EQ TRUE)
    {
      get_frame_descriptor(0);
      l1i_ra_data_ind(hCommTRA);
    }
  }
}
