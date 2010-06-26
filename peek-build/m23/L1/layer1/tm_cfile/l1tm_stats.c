/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1TM_STAT.C
 *
 *        Filename l1tm_stats.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#include "l1_macro.h"
#include "l1_confg.h"

#if TESTMODE

  #include <string.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_time.h"
  #include "l1_signa.h"

  #include "l1tm_defty.h"
  
  #if (AUDIO_TASK == 1)
    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_defty.h"
  #endif

  #if (L1_GTT == 1)
    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
  #endif

  #if (L1_MP3 == 1)
    #include "l1mp3_defty.h"
  #endif

  #if (L1_MIDI == 1)
    #include "l1midi_defty.h"
  #endif

  #include "l1_defty.h"
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "l1_varex.h"
  #include "l1_proto.h"

  #include "l1tm_msgty.h"
  #include "l1tm_varex.h"

/***********************************************************************/
/*                           TESTMODE 3.X                              */
/***********************************************************************/

/* External function prototypes */
/*------------------------------*/
void l1tm_stats_read(T_TM_RETURN *tm_return, WORD16 type, UWORD16 bitmask);
void l1a_tmode_send_ul_msg(T_TM_RETURN *tm_ret);

/* Internal function prototypes */
/*------------------------------*/
void l1tm_stats_compute(UWORD32 pm, UWORD32 toa, UWORD32 snr, WORD16 angle);
void l1tm_stats_auto_result_reset_loops(void);
void l1tm_reset_rx_stats(void);


void l1tm_stats_fb_confirm (T_TMODE_FB_CON *prim, WORD32 test)
// loop management done in L1A state machine
{

  l1tm.tmode_stats.loop_count++;
  l1tm.tmode_stats.flag_recent = prim->fb_flag;

  if (prim->fb_flag)
  {
    l1tm.tmode_stats.flag_count++;
  }

  if (prim->fb_flag)
    // We only save stats from a SUCCESS (including PM)
  {
    #if (TOA_ALGO == 2)
      l1tm_stats_compute(prim->pm_fullres, ((prim->toa)<<2), prim->snr, prim->angle);
    #else
      l1tm_stats_compute(prim->pm_fullres, prim->toa, prim->snr, prim->angle);      
    #endif
  } 

  l1tm_stats_auto_result_reset_loops();
}

void l1tm_stats_sb_confirm (T_TMODE_NCELL_SYNC_IND *prim, WORD32 test)
// loop management done in L1A state machine
{

  l1tm.tmode_stats.loop_count++;
  l1tm.tmode_stats.flag_recent = prim->sb_flag;

  if (prim->sb_flag)
  {
    l1tm.tmode_stats.flag_count++;
    l1tm.tmode_stats.bsic = prim->bsic;
  }

  if (prim->sb_flag)
    // we only save stats from a SUCCESS (including PM)
  {
    #if (TOA_ALGO == 2)
      l1tm_stats_compute(prim->pm_fullres, ((prim->toa)<<2), prim->snr, prim->angle);
    #else
      l1tm_stats_compute(prim->pm_fullres, prim->toa, prim->snr, prim->angle);      
    #endif
  } 

  l1tm_stats_auto_result_reset_loops();
}

void l1tm_stats_bcch_confirm (T_TMODE_BCCHS_CON *prim)
// loop management done here after stats management
// (the only stat is pass/fail)
{

  l1tm.tmode_stats.loop_count++;

  if (prim->error_flag == FALSE)
  {
    l1tm.tmode_stats.flag_count++;
    l1tm.tmode_stats.flag_recent = 1;
    l1tm.tmode_stats.bsic = l1a_l1s_com.Scell_info.bsic;  
  }
  else
  {
    l1tm.tmode_stats.flag_recent = 0;
  }

  l1tm_stats_auto_result_reset_loops(); 
}

void l1tm_stats_tch_confirm (T_TMODE_TCH_INFO *prim) 
// loop management and stats management done here
{

  // Update statistics only if downlink task enabled: DL-only or DL+UL
  if((l1_config.tmode.rf_params.down_up & TMODE_DOWNLINK) &&
    l1_config.tmode.rf_params.tmode_continuous == TM_NO_CONTINUOUS)
  {
    l1tm.tmode_stats.loop_count++;

    // TCH statistics are based on (accumulated) bit errors
    // Stats are computed independently from the RXQUAL
    // Discard first 2 blocks of TCH => always wrong if start not on block boundary
    if (l1tm.tmode_stats.loop_count > 2)
    {
      l1tm.tmode_stats.qual_acc_full      += prim->qual_full;
      l1tm.tmode_stats.qual_nbr_meas_full += prim->qual_nbr_meas_full;
      // fixed point unsigned F9.7 format
      l1tm.tmode_stats.flag_count = (WORD32)(((l1tm.tmode_stats.qual_acc_full * 100)<<8)/(l1tm.tmode_stats.qual_nbr_meas_full));
    }

    l1tm.tmode_stats.flag_recent = 1;
    l1tm.tmode_stats.bsic = l1a_l1s_com.Scell_info.bsic;  

    l1tm_stats_compute(prim->pm_fullres, prim->toa, prim->snr, prim->angle);

    l1tm_stats_auto_result_reset_loops();

  }  // end if DL-only or DL+UL
}

void l1tm_stats_mon_confirm (T_TMODE_FB_CON *prim)
// loop management done in L1A state machine
{

  l1tm.tmode_stats.loop_count++;
  l1tm.tmode_stats.flag_recent = prim->fb_flag;

  if (prim->fb_flag)
  {
    l1tm.tmode_stats.flag_count++;
  }

  if (prim->fb_flag)
    // we only save stats from a SUCCESS (including PM)
  {
    l1tm_stats_compute(prim->pm_fullres, prim->toa, prim->snr, prim->angle);
  }

  l1tm_stats_auto_result_reset_loops();
} 

/*void l1tm_stats_ra_confirm (T_TMODE_RA_DONE *prim)
// this function is only called when the RACH test is done
{
  //l1tm.tmode_stats.fn = prim->fn;

  l1tm.tmode_state.dedicated_active = 0;
}*/

void l1tm_stats_full_list_meas_confirm(T_TMODE_RXLEV_REQ *prim)
{
  T_TM_RETURN tm_ret;
  WORD16 stats_type = 2; // recent stats
  UWORD16 stats_bitmask = 0x0003; // for power measurement force rxlev and pm


  l1tm.tmode_stats.rssi_recent = prim->power_array->accum_power_result;
  l1tm_stats_read(&tm_ret, stats_type, stats_bitmask);
  // above function sets tm_ret.status, tm_ret.result[], tm_ret.index, tm_ret.size

#if (ETM_PROTOCOL == 1)
  tm_ret.mid = ETM_RF;
#endif

  tm_ret.cid = STATS_READ;
  l1a_tmode_send_ul_msg(&tm_ret);
}

#if L1_GPRS
  void l1tm_stats_pdtch_confirm (T_TMODE_PDTCH_INFO *prim) 
  // loop management and stats management done here
{
  UWORD8 ts;
  UWORD8 bit_map = 0x80;

  l1tm.tmode_stats.loop_count++;
  l1tm.tmode_stats.bsic = l1a_l1s_com.Scell_info.bsic;

  // count the number of successes over all time slots allocated for stats within one block
  for (ts=0;ts<8;ts++)
  {
    if (bit_map & l1_config.tmode.stats_config.stat_gprs_slots)
    {
      // if crc_error[ts]=FALSE the block is decoded successfully
      if (!prim->crc_error_tbl[ts])
        l1tm.tmode_stats.flag_count ++;
    }
    bit_map>>=1;
  }

  // TM stats are collected over one block (4 frames)
  l1tm_stats_compute(prim->pm_fullres, prim->toa, prim->snr, prim->angle);

  l1tm_stats_auto_result_reset_loops();

}
#endif

void l1tm_stats_compute(UWORD32 pm, UWORD32 toa, UWORD32 snr, WORD16 angle)
{
  // PM STATS
  l1tm.tmode_stats.pm_recent     = pm;
  l1tm.tmode_stats.pm_sum       += pm;
  l1tm.tmode_stats.pm_sq_sum    += (pm*pm);

  // TOA STATS
  l1tm.tmode_stats.toa_recent    = toa;
  l1tm.tmode_stats.toa_sum      += toa;
  l1tm.tmode_stats.toa_sq_sum   += (toa*toa);

  // ANGLE STATS
  l1tm.tmode_stats.angle_recent  = angle;
  l1tm.tmode_stats.angle_sum    += angle;
  l1tm.tmode_stats.angle_sq_sum += (angle*angle);
  if (angle < l1tm.tmode_stats.angle_min )
  {
    l1tm.tmode_stats.angle_min   = angle;
  }
  if (angle > l1tm.tmode_stats.angle_max )
  {
    l1tm.tmode_stats.angle_max   = angle;
  }

  // SNR STATS
  l1tm.tmode_stats.snr_recent    = snr;
  l1tm.tmode_stats.snr_sum      += snr;
  l1tm.tmode_stats.snr_sq_sum   += (snr*snr);
}

void l1tm_stats_auto_result_reset_loops(void)
{
  T_TM_RETURN tm_ret;

  if (l1_config.tmode.stats_config.auto_result_loops)
  {
    if ( ((l1tm.tmode_stats.loop_count) %((WORD32) l1_config.tmode.stats_config.auto_result_loops)) == 0) //omaps00090550
      // time to report current stats.  call l1tm_stats_read 
    {
      l1tm_stats_read(&tm_ret, l1_config.tmode.stats_config.stat_type, l1_config.tmode.stats_config.stat_bitmask);
      // above function sets tm_ret.status, tm_ret.result[], tm_ret.index, tm_ret.size

    #if (ETM_PROTOCOL == 1)
      tm_ret.mid = ETM_RF;
    #endif

      tm_ret.cid = STATS_READ;
      l1a_tmode_send_ul_msg(&tm_ret);
    }
  }

  if (l1_config.tmode.stats_config.auto_reset_loops)
  {
    if (((l1tm.tmode_stats.loop_count) %( (WORD32)l1_config.tmode.stats_config.auto_reset_loops)) == 0)  //omaps00090550
      // time to reset stats.
    {
      l1tm_reset_rx_stats();
      // resets stats accumulators and success/runs counters but not master loop counter for task.
    }
  }
}

/****************************************************************************
    Function:  l1tm_is_rx_counter_done
    Input:     none
    Output:    1 = done;
               0 = not done.

    Checks to see if the RX loop should be stopped, and increments counter.
    This counter is used in all RX functions.
*****************************************************************************/
BOOL l1tm_is_rx_counter_done(void)
// another T_TM_RETURN allocated here for auto acknowledge of rf_enable operation.
// to help save stack we could pass in a pointer to the tm_return structure,
// then save the current values
{
  T_TM_RETURN_ABBREV tm_ret;

  if (l1_config.tmode.stats_config.num_loops != 0) // 0 = infinite loop
  {
    l1tm.tmode_state.rx_counter++;
    if (l1tm.tmode_state.rx_counter >= l1_config.tmode.stats_config.num_loops)
    {
      // acknowledge end of RF_ENABLE operation

    #if (ETM_PROTOCOL == 1)
      tm_ret.mid = ETM_RF;
      tm_ret.status    = -ETM_FINISHED;
    #else
      tm_ret.status    = E_FINISHED;
    #endif
      tm_ret.cid       = RF_ENABLE;
      tm_ret.index     = 0;  //  No index value is sent

      tm_ret.size      = 1;
      tm_ret.result[0] = 1;
      l1a_tmode_send_ul_msg((T_TM_RETURN *) &tm_ret);

      return 1;
    }
    else
    {
      return 0; 
    }
  }
  else // infinite loop
  {
    return 0;
  }
}

void l1tm_reset_rx_state(void)
{
  l1tm.tmode_state.rx_counter = 0;
  l1tm.tmode_state.num_bcchs  = 0;
}

void l1tm_reset_rx_stats(void)
{
  UWORD32 i;

  l1tm.tmode_stats.toa_sum       = 0;
  l1tm.tmode_stats.toa_sq_sum    = 0;
  l1tm.tmode_stats.toa_recent    = 0;
  l1tm.tmode_stats.rssi_recent   = 0;
  l1tm.tmode_stats.pm_sum        = 0;
  l1tm.tmode_stats.pm_sq_sum     = 0;
  l1tm.tmode_stats.pm_recent     = 0;
  l1tm.tmode_stats.angle_sum     = 0;
  l1tm.tmode_stats.angle_sq_sum  = 0;
  l1tm.tmode_stats.angle_min     = +32767;
  l1tm.tmode_stats.angle_max     = -32768;
  l1tm.tmode_stats.angle_recent  = 0;
  l1tm.tmode_stats.snr_sum       = 0;
  l1tm.tmode_stats.snr_sq_sum    = 0;
  l1tm.tmode_stats.snr_recent    = 0;
  l1tm.tmode_stats.loop_count    = 0;
  l1tm.tmode_stats.flag_count    = 0;
  l1tm.tmode_stats.fn            = 0;
  l1tm.tmode_stats.bsic          = 0;
  l1tm.tmode_stats.qual_acc_full = 0;
  l1tm.tmode_stats.qual_nbr_meas_full = 0;

  for (i=0;i<4;i++)
    l1tm.tmode_stats.rssi_fifo[i] = 0;

#if L1_GPRS
 l1tm.tmode_stats.nb_dl_pdtch_slots = 0;
// Stats Bler
l1tm.tmode_stats.bler_total_blocks = 0;
  for (i=0;i<4;i++)
       l1tm.tmode_stats.bler_crc[i] = 0;
#endif
}


#endif

