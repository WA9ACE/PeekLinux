/************* Revision Control System Header *************
 *                  GSM Layer 1 software
 * MACS.C
 *
 *        Filename macs.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Control System Header *************/


//---Configuration flags---------------------------------------------------

#define TFI_FILTERING   1   // TFI FILTERING activated if set to 1
//-------------------------------------------------------------------------


#include <stdlib.h>
#include <stdio.h>

#include "l1_macro.h"
#include "l1_confg.h"

#if L1_GPRS

#include "l1_types.h"
#include "l1_const.h"

#if TESTMODE
  #include "l1tm_defty.h"
  #include "l1tm_varex.h"
#endif
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
#include "l1_varex.h"
#include "l1_signa.h"

#include "l1p_cons.h"
#include "l1p_msgt.h"
#include "l1p_deft.h"
#include "l1p_vare.h"
#include "l1p_sign.h"

#include "l1tm_proto.h"

#include "macs_def.h"
#include "macs_cst.h"
#include "macs_var.h"

#if FF_TBF
  #include "l1_trace.h"
  #if (CODE_VERSION == SIMULATION)
    #include "sim_cons.h"
    #include "sim_def.h"
    #include "sim_var.h"
  #endif
#endif
/**********************************************************/
/* MACS-S Prototypes                                      */
/**********************************************************/

void l1ps_macs_meas            (void);                // Measurement gap processing
void l1ps_macs_header_decoding (UWORD8 rx_no,
                                UWORD8 *tfi_result,
                                UWORD8 *pr);          // MAC header decoding
void l1ps_macs_read            (UWORD8 pr_table[8]);  // MAC-S control tasks processing
void l1ps_macs_ctrl            (void);                // MAC-S read tasks processing
void l1ps_macs_init            (void);                // MAC-S initialization

#if FF_TBF
void l1ps_macs_rlc_uplink_info (void);
#endif

/**********************************************************/
/* EXTERNAL Prototypes                                    */
/**********************************************************/

void l1pddsp_transfer_mslot_ctrl(UWORD8  burst_nb,
                                 UWORD8  dl_bitmap,
                                 UWORD8  ul_bitmap,
                                 UWORD8  *usf_table,
                                 UWORD8  mac_mode,
                                 UWORD8  *ul_buffer_index,
                                 UWORD8  tsc,
                                 UWORD16 radio_freq,
                                 UWORD8  synchro_timeslot,
                               #if FF_L1_IT_DSP_USF
                                 UWORD8  dsp_usf_interrupt
                               #else
                                 UWORD8  usf_vote_enable
                               #endif
                               );

/* RLC interface for uplink RLC/MAC blocks */
/*-----------------------------------------*/

void rlc_uplink(
                 UWORD8   assignment_id,
                 UWORD8   tx_data_no,            // Number of timeslot that can be used
                                                 //   for uplink data block transfer
                 UWORD32  fn,                    // Next frame number
                 UWORD8   timing_advance_value,  // Timing advance (255 if unknown)
                 API      *ul_poll_response,     // Pointer on a_pu_gprs (NDB): poll response blocks
                 API      *ul_data,              // Pointer on a_du_gprs (NDB): uplink data blocks
                 BOOL     allocation_exhausted   // Set to 1 if fixed allocation exhausted
               );

#if TESTMODE
void l1tm_rlc_uplink(UWORD8 tx, API *ul_data);
#endif

/* RLC interface for downlink RLC/MAC blocks */
/*-------------------------------------------*/
#if FF_TBF

void rlc_downlink_data(
                        UWORD8   assignment_id,
                        UWORD32  fn,             // Actual frame number
                        API      *dl             // Pointer on a_dd_gprs (NDB): downlink blocks
                      );
void rlc_uplink_info(
                     UWORD8   assignment_id,
                     UWORD32  fn,
                     UWORD8   rlc_blocks_sent,    // Number of uplink blocks that was transmitted
                                                  // during the last block period
                     UWORD8   last_poll_response  // Status of the poll responses of
                    );                            // the last block period

UWORD8 rlc_downlink_copy_buffer(UWORD8 isr);


#else

void rlc_downlink(
                   UWORD8   assignment_id,
                   UWORD32  fn,                  // Actual frame number
                   API      *dl,                 // Pointer on a_dd_gprs (NDB): downlink blocks
                   UWORD8   rlc_blocks_sent,     // Number of uplink blocks that was transmitted
                                                 //   during the last block period
                   UWORD8   last_poll_response   // Status of the poll responses of
                                                 //   the last block period
                 );

#endif

#if (TRACE_TYPE==1) || (TRACE_TYPE==4)
  #include "l1_trace.h"
#endif


#if FF_TBF
  #include <stddef.h>

  #if (CODE_VERSION == SIMULATION)
    API* const A_DD_XGPRS[1][4] =
    {
      {&buf.ndb_gprs.a_dd_gprs[0][0], &buf.ndb_gprs.a_dd_gprs[1][0], &buf.ndb_gprs.a_dd_gprs[2][0], &buf.ndb_gprs.a_dd_gprs[3][0]},
    };

    API* const A_DU_XGPRS[1][4] =
    {
      {&buf.ndb_gprs.a_du_gprs[0][0], &buf.ndb_gprs.a_du_gprs[1][0], &buf.ndb_gprs.a_du_gprs[2][0], &buf.ndb_gprs.a_du_gprs[3][0]}
    };

  #else
    API* const A_DD_XGPRS[1][4] =
    {
      {
        (API*)(NDB_ADR_GPRS + offsetof(T_NDB_MCU_DSP_GPRS, a_dd_gprs[0][0])),
        (API*)(NDB_ADR_GPRS + offsetof(T_NDB_MCU_DSP_GPRS, a_dd_gprs[1][0])),
        (API*)(NDB_ADR_GPRS + offsetof(T_NDB_MCU_DSP_GPRS, a_dd_gprs[2][0])),
        (API*)(NDB_ADR_GPRS + offsetof(T_NDB_MCU_DSP_GPRS, a_dd_gprs[3][0]))
      }
    };

    API* const  A_DU_XGPRS[1][4] =
    {
      {
        (API*)(NDB_ADR_GPRS + offsetof(T_NDB_MCU_DSP_GPRS,a_du_gprs[0][0])),
        (API*)(NDB_ADR_GPRS + offsetof(T_NDB_MCU_DSP_GPRS,a_du_gprs[1][0])),
        (API*)(NDB_ADR_GPRS + offsetof(T_NDB_MCU_DSP_GPRS,a_du_gprs[2][0])),
        (API*)(NDB_ADR_GPRS + offsetof(T_NDB_MCU_DSP_GPRS,a_du_gprs[3][0]))
      }
    };
  #endif

#endif
/*-----------------------------------------------------------*/
/* l1ps_macs_init()                                          */
/*-----------------------------------------------------------*/
/* Parameters:                                               */
/*                                                           */
/* Return:                                                   */
/*                                                           */
/* Description: This function initializes MAC-S variables    */
/*              and must be called before the first call     */
/*              of MAC-S                                     */
/*-----------------------------------------------------------*/
void l1ps_macs_init(void)
{
  UWORD8 i;

  #if FF_TBF
    macs.dl_buffer_index            = INVALID;
    for (i=0;i<NBR_SHARED_BUFFER_RLC;i++)
      macs.rlc_dbl_buffer[i].d_rlcmac_rx_no_gprs = 0xff;
  #endif
  /* General TBF parameters processed by MAC-S */
  /*********************************************/

  macs.sti_block_id           = 0;

  /* Ressources allocated by MAC-S             */
  /*********************************************/

  macs.rx_allocation          = 0;
  macs.tx_allocation          = 0;
  macs.tx_prach_allocation    = 0;
  macs.tx_data                = 0;
  macs.pwr_allocation         = 0xff;
  macs.last_rx_alloc          = 0;

  #if FF_L1_IT_DSP_USF
    macs.dsp_usf_interrupt    = 0;
  #endif

  macs.rx_blk_period          = NO_DL_BLK;
  macs.rlc_blocks_sent        = 0;
  macs.rx_no                  = 0;
  macs.last_poll_response     = 0;

  macs.usf_good               = 0;
  macs.usf_vote_enable        = 0;
  macs.tra_gap                = 8;
  macs.fix_alloc_exhaust      = FALSE;
  macs.next_usf_mon_block[0]  = macs.next_usf_mon_block[1] = 0;

  for (i = 0; i < TS_NUMBER; i ++)
  {
    macs.ul_buffer_index[i] = INVALID;
  }
  #if L1_EDA
    for (i = 0; i < 4; i++)
    {
      macs.monitored_ts[i] = INVALID;
    }
    macs.rx_monitored      = 0;
    macs.last_rx_monitored = 0;
    macs.lowest_poll_ts    = INVALID;
  #endif

  #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
    // Reset PDTCH trace structure
    for(i=0; i<8; i++)
    {
      trace_info.pdtch_trace.dl_status[i] = 0;
      trace_info.pdtch_trace.ul_status[i] = 0;
      trace_info.pdtch_trace.blk_status   = 0;
    }
  #endif

} /* End of l1ps_macs_init */

/*-----------------------------------------------------------*/
/* l1ps_macs_ctrl()                                          */
/*-----------------------------------------------------------*/
/* Parameters:global l1ps_macs_com           changed         */
/*            global l1s                     unchanged       */
/*            global l1a_l1s_com             unchanged       */
/*            global l1ps_dsp_com            changed         */
/*                                                           */
/* Return:                                                   */
/*                                                           */
/* Description: MAC_S manages the timeslot allocation for    */
/*              downlink and uplink transfer and assign a    */
/*              measurement gap according to the MS class,   */
/*              the frame number when it's called and the    */
/*              allocation information given by Layer 1.     */
/*              It also program the corresponding tasks on   */
/*              the DSP and asks the RLC layer for uplink    */
/*              blocks                                       */
/*-----------------------------------------------------------*/
void l1ps_macs_ctrl(void)
{
  #define NDB_PTR  l1ps_dsp_com.pdsp_ndb_ptr
  #define SET_PTR  l1pa_l1ps_com.transfer.aset

  #if MACS_STATUS
    // No error
    l1ps_macs_com.macs_status.nb = 0;
  #endif

  /***********************************************************/
  /* USF values updating in dynamic mode (first frames of    */
  /* block periods)                                          */
  /***********************************************************/

  if  ((SET_PTR->allocated_tbf == UL_TBF) || (SET_PTR->allocated_tbf == BOTH_TBF))
  {
  #if L1_EDA
    if ((SET_PTR->mac_mode == DYN_ALLOC) || (SET_PTR->mac_mode == EXT_DYN_ALLOC))
  #else
    if (SET_PTR->mac_mode == DYN_ALLOC)
  #endif
    {
      // Test if the USF must be read in the current frame
      // Concern the first frames of each block period (useful when
      // some USF values weren't valid the frame before)
      //                                                            FN 13
      //    0   1   2   3    4   5   6   7    8   9   10  11   12
      // ----------------------------------------------------------
      // ||       B0      ||       B1      ||       B2      || I ||
      // ||   |   |   |   ||   | X |   |   ||   | X |   |   ||   ||
      // ----------------------------------------------------------
      //                                                            X:USF Reading

    #if FF_L1_IT_DSP_USF
      if (l1ps_macs_com.usf_status == USF_IT_DSP)
    #else
      if (   (l1s.next_time.fn_mod13 == 5)
          || (l1s.next_time.fn_mod13 == 9))
    #endif
      {
        // USF values are read
        // Uplink timeslots whose USF was INVALID the frame before are de-allocated
        // if their USF value is now BAD

        UWORD8 tn;

        // Reading of the d_usf_updated_gprs value
        API    usf_updated = NDB_PTR->d_usf_updated_gprs;

        #if !L1_EDA
        // For each timeslot that can be allocated in uplink...
        for (tn = macs.first_monitored_ts; tn <= macs.last_monitored_ts; tn ++)
        {
          // If USF vote was enabled on this timeslot
          if(macs.usf_vote_enable & (MASK_SLOT0 >> tn))
          {
            // Clear the USF vote flag
            macs.usf_vote_enable &= ~(MASK_SLOT0 >> tn);

            // Read USF value
            if (((usf_updated >> ((MAX_TS_NB - tn) * 2)) & MASK_2SLOTS) == USF_GOOD)
            {
              // This timeslot is valid and good
              macs.usf_good |= (UWORD8) (MASK_SLOT0 >> (tn + 3));
              macs.next_usf_mon_block[tn - macs.first_monitored_ts] = l1s.next_time.block_id + USF_BLOCK_GRANULARITY[SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_granularity];
            }
            else
            {
              // This timeslot is bad or invalid
              // If the slot was allocated for data
              if (macs.tx_data & (MASK_SLOT0 >> (tn + RXTX_DELAY)))
              {
                // rlc_blocks_sent decremented
                macs.rlc_blocks_sent --;

              #if FF_L1_IT_DSP_USF
                    // If next timeslot is also a data block:
                    // Shift data block to next TX opportunity. For MS class 12
                    // with dynamic allocation, 2 TX data max and they are
                    // contiguous (optimization).
                    if (macs.tx_data & (MASK_SLOT0 >> (tn + 1 + RXTX_DELAY)))
                    {
                      macs.ul_buffer_index[tn + 1 + RXTX_DELAY] = macs.ul_buffer_index[tn + RXTX_DELAY];

                    }

                    // Cancel burst
                    macs.ul_buffer_index[tn + RXTX_DELAY] = INVALID;

              #endif // FF_L1_IT_DSP_USF

                #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
                  if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
                    trace_info.pdtch_trace.ul_status[tn + RXTX_DELAY] = 0;
                #endif

                #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
                  RTTL1_FILL_MACS_STATUS(TX_CANCELLED_USF, tn)
                #endif
              }
            }
          } // End if "USF vote enabled on this timeslot"
        } // End for

        /* Uplink resources de-allocated by the DSP are de-allocated by MAC-S */
        macs.tx_allocation &= (UWORD8)  (macs.usf_good | ~macs.tx_data);
        macs.tx_data       &= (UWORD8)  (macs.usf_good | ~macs.tx_data);
        #else //#if !L1_EDA
          UWORD8  i = 0;

          //for all timeslots that need to be monitored
          while ((macs.monitored_ts[i] != INVALID) && (i <= 3))
          {
            // If USF vote was enabled on this timeslot
            if(macs.usf_vote_enable & (MASK_SLOT0 >> macs.monitored_ts[i]))
            {
              // Clear the USF vote flag
              macs.usf_vote_enable &= ~(MASK_SLOT0 >> macs.monitored_ts[i]);

              // Read USF value
              if ((((usf_updated >> ((MAX_TS_NB - macs.monitored_ts[i]) * 2)) & MASK_2SLOTS) == USF_GOOD)
                  && (macs.last_rx_monitored & (MASK_SLOT0 >> macs.monitored_ts[i])))
              {
                if (SET_PTR->mac_mode == EXT_DYN_ALLOC)
                {
                  //RX timeslots to monitor have to be updated upon USF status receipt (only first ts with
                  //good USF has to be considered)
                  if ((macs.monitored_ts[i] <= macs.lowest_poll_ts) ||
                      ((MASK_SLOT0 >> macs.monitored_ts[i]) & macs.rx_allocation))
                    macs.rx_monitored |= (UWORD8) (MASK_SLOT0 >> macs.monitored_ts[i]);

                  // Clear the USF vote flag
                  macs.usf_vote_enable = 0;

                  //if the USF value is GOOD all remaining timelots that needed to be monitored
                  //have to be considered as having GOOD USFs
                  while ((macs.monitored_ts[i] != INVALID) && (i <= 3))
                  {
                    // Update good USFs bitmap
                    macs.usf_good |= (UWORD8) (MASK_SLOT0 >> (macs.monitored_ts[i] + RXTX_DELAY));
                    i++;
                  }
                  macs.next_usf_mon_block[0] = l1s.next_time.block_id + USF_BLOCK_GRANULARITY[SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_granularity];
                  break;
                }
                else
                {
                  // This timeslot is valid and good
                  macs.usf_good |= (UWORD8) (MASK_SLOT0 >> (macs.monitored_ts[i] + RXTX_DELAY));
                  macs.next_usf_mon_block[macs.monitored_ts[i] - macs.first_monitored_ts] = l1s.next_time.block_id + USF_BLOCK_GRANULARITY[SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_granularity];
                }
              }
              else //USF is BAD or INVALID
              {
                //The TDMA before USF status was not known so USF was supposed to be GOOD but
                // now it turns out to be BAD or INVALID so block is deallocated.
                if (macs.tx_data & (MASK_SLOT0 >> (macs.monitored_ts[i] + RXTX_DELAY)))
                {
                  UWORD8 j;

                  // rlc_blocks_sent decremented
                  macs.rlc_blocks_sent --;

                  tn = macs.monitored_ts[i];

                  #if FF_L1_IT_DSP_USF
                    //For all monitored ts (beginning with last one), if the timeslot is a data block then
                    //data block is shifted to next monitored ts.
                    j=3;
                    while (macs.monitored_ts[j] != tn)
                    {
                      if ((macs.monitored_ts[j] != INVALID) &&
                          (macs.tx_data & (MASK_SLOT0 >> (macs.monitored_ts[j] + RXTX_DELAY))))
                      {
                        macs.ul_buffer_index[macs.monitored_ts[j] + RXTX_DELAY] = macs.ul_buffer_index[macs.monitored_ts[j-1] + RXTX_DELAY];
                        #if L1_EGPRS
                          macs.tx_modulation &= ~(MASK_SLOT0 >> (macs.monitored_ts[j] + RXTX_DELAY));
                          macs.tx_modulation |= ((macs.tx_modulation &
                                                (MASK_SLOT0 >> (macs.monitored_ts[j-1] + RXTX_DELAY)))
                                               >> (macs.monitored_ts[j]-macs.monitored_ts[j-1]));
                        #endif // L1_EGPRS
                      }
                      j--;
                    }

                    // Cancel burst
                    macs.ul_buffer_index[tn + RXTX_DELAY] = INVALID;
                    #if L1_EGPRS
                      macs.tx_modulation &= ~(MASK_SLOT0 >> (tn + RXTX_DELAY));
                    #endif // L1_EGPRS
                  #endif // FF_L1_IT_DSP_USF
                }//if (macs.tx_data & (MASK_SLOT0 >> (macs.monitored_ts[i] + RXTX_DELAY)))
                if (SET_PTR->mac_mode == EXT_DYN_ALLOC)
                {
                  //USF for current timeslot is BAD so it has to be monitored for next USF
                  //period
                  if ((macs.monitored_ts[i] <= macs.lowest_poll_ts) ||
                      ((MASK_SLOT0 >> macs.monitored_ts[i]) & macs.rx_allocation))
                    macs.rx_monitored |= (UWORD8) (MASK_SLOT0 >> macs.monitored_ts[i]);
                }
              }
            }//if(macs.usf_vote_enable & (MASK_SLOT0 >> macs.monitored_ts[i]))
            i++;
          }//while ((macs.monitored_ts[i] != INVALID) && (i <= 3))

          if (SET_PTR->mac_mode == EXT_DYN_ALLOC)
          {
            // Downlink monitoring is updated depending on USF status
            macs.rx_allocation |= macs.rx_monitored;
          }

          // Uplink resources de-allocated by the DSP are de-allocated by MAC-S
          macs.tx_allocation &= (UWORD8)  (macs.usf_good | ~macs.tx_data);
          macs.tx_data       &= (UWORD8)  (macs.usf_good | ~macs.tx_data);
        #endif //#if !L1_EDA

        // Measurement gap processing
        l1ps_macs_meas();

      } // End if FN13 = 4 OR 8
    } // End if dynamic allocation mode
  } // End if uplink TBF


  #if FF_L1_IT_DSP_USF
    if (l1ps_macs_com.usf_status != USF_IT_DSP)
    {
  #endif

  /************************************************************/
  /*****  RESSOURCE ALLOCATION FOR THE NEXT BLOCK PERIOD  *****/
  /************************************************************/

  // If the next frame is the first of a block period
  //                                                           FN 13
  //    0   1   2   3    4   5   6   7    8   9   10  11   12
  // ----------------------------------------------------------
  // ||       B0      ||       B1      ||       B2      || I ||
  // || X |   |   |   || X |   |   |   || X |   |   |   ||   ||
  // ----------------------------------------------------------

  if (   (l1s.next_time.fn_mod13 == 4)
      || (l1s.next_time.fn_mod13 == 8)
      || (l1s.next_time.fn_mod13 == 0))
  {
    UWORD8   tx                  = 0;      // MS class Tx parameter checked
    UWORD8   rx                  = 0;      // MS class Rx parameter checked
    UWORD8   tx_no;                        // Number of allocated uplink timeslots
    UWORD8   highest_ul_ts;                // Highest numbered allocated uplink resource
    UWORD8   lowest_ul_ts;                 // Lowest numbered allocated uplink resource
    UWORD8   highest_dl_ts;                // Highest numbered allocated uplink resource
    UWORD8   lowest_dl_ts;                 // Lowest numbered allocated uplink resource
    UWORD8   tra_before_frame;             // Number of free Tx slots at the end of the
                                           // previous frame


    /***********************************************************/
    /* New allocated ressources                                */
    /***********************************************************/

    /*---------------------------------------------------------*/
    /* New assignment or synchronization change                */
    /*---------------------------------------------------------*/

    if (   (l1ps_macs_com.new_set != FALSE)
        || (l1a_l1s_com.dl_tn != macs.old_synchro_ts))
    {
      UWORD8  tn;
      UWORD8  fn_mod13;
      UWORD32 fn_div13;

      /* Fixed allocation mode initialization */
      /*--------------------------------------*/

      if (SET_PTR->mac_mode == FIX_ALLOC_NO_HALF)
      {
        if (((SET_PTR->assignment_command == UL_TBF) || (SET_PTR->assignment_command == BOTH_TBF)) &&
            (l1ps_macs_com.new_set))
        {
          // Starting time block ID processing
          fn_div13 = (UWORD32) (SET_PTR->tbf_sti.absolute_fn / 13);             // FN / 13
          fn_mod13 = (UWORD8) (SET_PTR->tbf_sti.absolute_fn - (fn_div13 * 13)); // FN mod 13
          macs.sti_block_id = (UWORD32) (  (3 * (UWORD32) fn_div13)             // Block ID
                                       + (fn_mod13 / 4));

          // Starting time not aligned on a block period
          if ((fn_mod13 != 0) && (fn_mod13 != 4) && (fn_mod13 != 8) && (fn_mod13 != 12))
            macs.sti_block_id ++;

          // Reset the fixed allocation bitmap exhaustion flag only in case of a new assignment
          macs.fix_alloc_exhaust      = FALSE;
        }

      } // End of fixed mode initialization
      else

        #if L1_EDA
          /*  Extended Dynamic/Dynamic allocation mode initialization  */
          /*------------------------------------------*/

          if ((SET_PTR->mac_mode == DYN_ALLOC) || (SET_PTR->mac_mode == EXT_DYN_ALLOC))
        #else
      /*  Dynamic allocation mode initialization  */
      /*------------------------------------------*/

      if (SET_PTR->mac_mode == DYN_ALLOC)
        #endif
      {
        if ((SET_PTR->assignment_command == UL_TBF) || (SET_PTR->assignment_command == BOTH_TBF) ||
            (l1a_l1s_com.dl_tn != macs.old_synchro_ts))
        {
          // USF value aren't kept
          macs.usf_good           = 0;
          macs.usf_vote_enable    = 0;         // No USF vote
          macs.rx_blk_period      = NO_DL_BLK;

          if (l1ps_macs_com.new_set)
          // USF monitoring block set to current block to immediately enable
          // the USF monitoring in case of new UL TBF
            macs.next_usf_mon_block[0] = macs.next_usf_mon_block[1] = l1s.next_time.block_id;

          // First and last allocated Tx number updating
          macs.first_monitored_ts = INVALID;
          macs.last_monitored_ts  = INVALID;

          tn = 0;
          // Search of the lowest timeslot allocated in uplink
          while (   !(SET_PTR->ul_tbf_alloc->timeslot_alloc & (MASK_SLOT0 >> tn))
                 && (tn < TS_NUMBER))
            tn ++;

          if (tn != TS_NUMBER)
          {
            macs.first_monitored_ts = tn - l1a_l1s_com.dl_tn;
            tn = MAX_TS_NB;

            // Search of the highest timeslot allocated in uplink
            while (!(SET_PTR->ul_tbf_alloc->timeslot_alloc & (MASK_SLOT0 >> tn)))
              tn --;
            macs.last_monitored_ts = tn - l1a_l1s_com.dl_tn;
                #if L1_EDA
                  //Extended Dynamic or Dynamic Allocation has been set
                  {
                    UWORD8  i=0;
                    macs.rx_monitored   = 0;
                    //Search among the timeslots allocated in uplink, the timeslots that are really
                    //allocated (macs.first_monitored_ts and macs.last_monitored_ts are for sure
                    //allocated but the allocation can have holes inbetween)
                    for (i = 0; i < 4; i++)
                    {
                      macs.monitored_ts[i] = INVALID;
                    }
                    i = 0;
                    for (tn = macs.first_monitored_ts; tn <= macs.last_monitored_ts; tn++)
                    {
                      //Find the ts that are allocated and need therefore to be monitored
                      if (((SET_PTR->ul_tbf_alloc->timeslot_alloc & (MASK_SLOT0 >> (tn + l1a_l1s_com.dl_tn))) && (SET_PTR->mac_mode == EXT_DYN_ALLOC))
                         || (SET_PTR->mac_mode == DYN_ALLOC))
                      {
                        macs.monitored_ts[i]= tn;
                        i++;
                      }
                    }
                  }
                #endif //#if L1_EDA
              }
            }
            #if L1_EDA
              if((SET_PTR->mac_mode == EXT_DYN_ALLOC))
                l1ps_macs_com.fb_sb_task_detect = TRUE;
              else
                l1ps_macs_com.fb_sb_task_detect = FALSE;
            #endif
      } // End of dynamic mode initialization

      /* Reset of new_set */
      /*------------------*/

      l1ps_macs_com.new_set = FALSE;

    } // End of new allocation

    /*---------------------------------------------------------*/
    /* Resource initialization                                 */
    /*---------------------------------------------------------*/

    macs.pwr_allocation      = 0;  // Power measurements
    macs.rx_allocation       = 0;  // Rx allocation
    macs.tx_allocation       = 0;  // Tx allocation
    macs.tx_data             = 0;  // Tx data allocation
    macs.tx_prach_allocation = 0;  // Tx PRACH allocation
    #if FF_L1_IT_DSP_USF
     macs.dsp_usf_interrupt = 0;  // DSP interrupt for USF decoding needed
    #endif

    /***********************************************************/
    /* Downlink TBF processing                                 */
    /***********************************************************/

    if ((SET_PTR->allocated_tbf == DL_TBF) || (SET_PTR->allocated_tbf == BOTH_TBF))
    {
      /* Downlink resources are allocated */
      macs.rx_allocation = (UWORD8) (   SET_PTR->dl_tbf_alloc.timeslot_alloc
                                     << l1a_l1s_com.dl_tn);
    } /* End if downlink TBF processing  */

    /***********************************************************/
    /* Uplink TBF processing                                   */
    /***********************************************************/

    if ((SET_PTR->allocated_tbf == UL_TBF) || (SET_PTR->allocated_tbf == BOTH_TBF))
    {

      /*---------------------------------------------------------*/
      /* Dynamic allocation mode                                 */
      /*---------------------------------------------------------*/

        #if L1_EDA
          if ((SET_PTR->mac_mode == DYN_ALLOC) || (SET_PTR->mac_mode == EXT_DYN_ALLOC))
        #else
          if (SET_PTR->mac_mode == DYN_ALLOC)
        #endif
      {
        UWORD8 i;
        UWORD8 tn;
        API    usf_updated;

            #if !L1_EDA
        /* Downlink resource monitoring */
        /*------------------------------*/

        macs.rx_allocation |= (UWORD8)  (   SET_PTR->ul_tbf_alloc->timeslot_alloc
                                        << l1a_l1s_com.dl_tn);

        /* USF values reading */
        /*--------------------*/

        // An uplink timeslot is allocated by MAC-S if it's allocated by the network and
        //    - USF is updated and good
        // OR - USF isn't updated

        // Reading of the d_usf_updated_gprs value
        usf_updated = NDB_PTR->d_usf_updated_gprs;

        // For each timeslot that can be allocated in uplink...
        for (tn = macs.first_monitored_ts; tn <= macs.last_monitored_ts; tn ++)
        {
          UWORD8 tn_usf;
          WORD32 delta;

          // Remaining blocks before a new USF reading
          delta = (WORD8) (macs.next_usf_mon_block[tn - macs.first_monitored_ts] - l1s.next_time.block_id);

          // MAX_FN modulo
          if (delta <= 0) delta += MAX_BLOCK_ID;

          if (delta >= USF_BLOCK_GRANULARITY[SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_granularity])
          {
            // USF no more usable
            // Clear USF in good USFs bitmap
            macs.usf_good &= (UWORD8) ~(MASK_SLOT0 >> (tn + 3));

            // Clear USF vote
            macs.usf_vote_enable &= (UWORD8) ~(MASK_SLOT0 >> tn);

            // If downlink blocks were entirely received during the last block period
            if (macs.rx_blk_period == l1s.next_time.block_id)
            {
              // Read USF
              tn_usf = (UWORD8) ((usf_updated >> ((MAX_TS_NB - tn) << 1)) & MASK_2SLOTS);

              if (tn_usf == USF_GOOD)
              {
                // Update good USFs bitmap
                macs.usf_good |= (UWORD8) (MASK_SLOT0 >> (tn + 3));
                macs.next_usf_mon_block[tn - macs.first_monitored_ts] = l1s.next_time.block_id + USF_BLOCK_GRANULARITY[SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_granularity];
              }
              else
              if ((tn_usf == USF_INVALID) && (l1s.next_time.fn_mod13 != 0))
              {
                // Unknown USF:
                //    - TPU is programmed to tranmit a block on timeslot tn
                //    - DSP will set the TX PWR to 0 for this timeslot is USF is bad
                //      (USF vote mechanism)

                macs.tx_allocation   |= (MASK_SLOT0 >> (tn + 3));
                macs.usf_vote_enable |= (MASK_SLOT0 >> tn);
              }
            } // End if "downlink block entirely received"
          } // End if "USF no more usable"
        } // End for

            #else  //#if !L1_EDA
              {
                UWORD8 tn_usf;
                WORD32 delta;
                UWORD8 i=0;

                if (SET_PTR->mac_mode == DYN_ALLOC)
                {
                  /* Downlink resource monitoring */
                  /*------------------------------*/

                  macs.rx_allocation |= (UWORD8)  (   SET_PTR->ul_tbf_alloc->timeslot_alloc
                                                << l1a_l1s_com.dl_tn);

                  macs.last_rx_monitored = macs.rx_allocation;
                }
                else
                {
                  // Remaining blocks before a new USF reading
                  delta = (WORD8) (macs.next_usf_mon_block[0] - l1s.next_time.block_id);

                  // MAX_FN modulo
                  if (delta <= 0) delta += MAX_BLOCK_ID;

                  //Save last rx_monitored timeslots
                  macs.last_rx_monitored = macs.rx_monitored;

                  //for all timeslots to monitor (timeslots allocated in UL)
                  for (tn = macs.first_monitored_ts; tn <= macs.last_monitored_ts; tn++)
                  {
                    //All possible timeslots to monitor have to be monitored
                    // (USF validity period exhausted)
                    if (macs.monitored_ts[i] != INVALID)
                      macs.rx_monitored |= (MASK_SLOT0 >> macs.monitored_ts[i]);
                    i++;
                  }

                  //USF validity period is exhausted
                  if (delta >= USF_BLOCK_GRANULARITY[SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_granularity])
                  {
                    //USF need to be evaluated for next block period
                    macs.usf_good = 0;
                  }
                  else
                  {
                    //Set monitored ts again if poll occured on one ts during a block granularity period = 4
                    //note: macs.usf_good is always different from 0
                    i=0;
                    while (!(macs.usf_good & (MASK_SLOT0 >> (macs.monitored_ts[i] + RXTX_DELAY))))
                      i++;
                    macs.rx_monitored &= (UWORD8) ~(MASK_ALL_SLOTS >> macs.monitored_ts[i+1]);
                  }
                }

                // Reading of the d_usf_updated_gprs value
                usf_updated = NDB_PTR->d_usf_updated_gprs;

                i=0;
                //for all possible timeslots to monitor
                while ((macs.monitored_ts[i] != INVALID) && (i <= 3))
                {
                  if (SET_PTR->mac_mode == DYN_ALLOC)
                  {
                    // Remaining blocks before a new USF reading
                    delta = (WORD8) (macs.next_usf_mon_block[macs.monitored_ts[i] - macs.first_monitored_ts] - l1s.next_time.block_id);

                    // MAX_FN modulo
                    if (delta <= 0) delta += MAX_BLOCK_ID;
                  }

                  //USF validity period is exhausted
                  if (delta >= USF_BLOCK_GRANULARITY[SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_granularity])
                  {
                    if (SET_PTR->mac_mode == DYN_ALLOC)
                    {
                      // USF no more usable
                      // Clear USF in good USFs bitmap
                      macs.usf_good &= (UWORD8) ~(MASK_SLOT0 >> (macs.monitored_ts[i] + RXTX_DELAY));

                      // Clear USF vote
                      macs.usf_vote_enable &= (UWORD8) ~(MASK_SLOT0 >> macs.monitored_ts[i]);
                    }

                    if ((macs.rx_blk_period == l1s.next_time.block_id) && (macs.last_rx_monitored & (MASK_SLOT0 >> macs.monitored_ts[i])))
                    {

                      #if L1_EGPRS
                        // EGPRS switched radio loopback sub mode on: dynamic allocation
                        // but USF are always deemed to be good
                        if (l1ps_macs_com.loop_param.sub_mode == TRUE)
                          tn_usf = USF_GOOD;
                        else
                      #endif
                          // Read USF
                         tn_usf = (UWORD8) ((usf_updated >> ((MAX_TS_NB - macs.monitored_ts[i]) << 1)) & MASK_2SLOTS);

                      if (tn_usf == USF_GOOD)
                      {
                        if (SET_PTR->mac_mode == EXT_DYN_ALLOC)
                        {
                          //Deallocate monitored ts
                          macs.rx_monitored &= (UWORD8) ~(MASK_ALL_SLOTS >> macs.monitored_ts[i+1]);

                          //As USF is good for current monitored ts, all subsequent monitored ts have
                          //to be deallocated and the associated USF set to USF_GOOD
                          while ((macs.monitored_ts[i] != INVALID) && (i <= 3))
                          {
                            //Update good USFs bitmap
                            macs.usf_good |= (UWORD8) (MASK_SLOT0 >> (macs.monitored_ts[i] + RXTX_DELAY));
                            i++;
                          }
                          macs.next_usf_mon_block[0] = l1s.next_time.block_id + USF_BLOCK_GRANULARITY[SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_granularity];
                          break;
                        }
                        else
                        {
                          // Update good USFs bitmap
                          macs.usf_good |= (UWORD8) (MASK_SLOT0 >> (macs.monitored_ts[i] + RXTX_DELAY));
                          macs.next_usf_mon_block[macs.monitored_ts[i] - macs.first_monitored_ts] = l1s.next_time.block_id + USF_BLOCK_GRANULARITY[SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_granularity];
                        }
                      }
                      else
                      if ((tn_usf == USF_INVALID) && (l1s.next_time.fn_mod13 != 0))
                      {
                        if (SET_PTR->mac_mode == EXT_DYN_ALLOC)
                        {
                          //Deallocate monitored ts
                          macs.rx_monitored &= (UWORD8) ~(MASK_ALL_SLOTS >> macs.monitored_ts[i+1]);

                          //As USF is invalid (status of USF not yet known) for current monitored ts, the USF is
                          //supposed to be good and therefore Tx is allocated and all subsequent monitored ts
                          //have to be deallocated. Vote mechanism is also enabled for these latter timeslots.
                          while ((macs.monitored_ts[i] != INVALID) && (i <= 3))
                          {
                            macs.tx_allocation |= (MASK_SLOT0 >> (macs.monitored_ts[i] + RXTX_DELAY));
                            macs.usf_vote_enable |= (MASK_SLOT0 >> macs.monitored_ts[i]);
                            i++;
                          }
                          break;
                        }
                        else
                        {
                          // Unknown USF:
                          //    - TPU is programmed to tranmit a block on timeslot tn
                          //    - DSP will set the TX PWR to 0 for this timeslot is USF is bad
                          //      (USF vote mechanism)

                          macs.tx_allocation   |= (MASK_SLOT0 >> (macs.monitored_ts[i] + RXTX_DELAY));
                          macs.usf_vote_enable |= (MASK_SLOT0 >> macs.monitored_ts[i]);
                        }
                      }
                    }//if ((macs.rx_blk_period == l1s.next_time.block_id) && (macs.last_rx_monitored & (MASK_SLOT0 >> macs.monitored_ts[i])))
                  }
                  i++;
                }//while ((tn != INVALID) && (i <= 3))
              }//if (SET_PTR->mac_mode == EXT_DYN_ALLOC)

              /* Downlink resource monitoring */
              /*------------------------------*/
              macs.rx_allocation |= macs.rx_monitored;
            #endif //#if !L1_EDA
        /* Uplink resources allocation according to USF values */
        /*-----------------------------------------------------*/

        i = l1a_l1s_com.dl_tn - RXTX_DELAY;
        if (i < TS_NUMBER)
        {
          macs.tx_allocation |= (UWORD8) (  macs.usf_good
                                          & (SET_PTR->ul_tbf_alloc->timeslot_alloc << i));
        }
        else
        {
          macs.tx_allocation |= (UWORD8) (  macs.usf_good
                                          & (SET_PTR->ul_tbf_alloc->timeslot_alloc >> (-i)));
        }

            #if L1_EDA
              //if FB/SB activity detected in comming idle frame, some TX burst have to be deallocated
              //to allow opening of FB/SB window (no TX activity should be scheduled in slots 6 and 7)
              if (l1ps_macs_com.fb_sb_task_enabled && l1ps_macs_com.fb_sb_task_detect)
              {
                macs.tx_allocation &= ~(MASK_SLOT0 >> 6);
              }
            #endif

       #if FF_L1_IT_DSP_USF
              // UL or BOTH TBF with dynamic allocation in use. DSP has to generate
              // an interrupt for USF validity for the block to be received if it
              // is either RBN (radio block number) % 3 = 0 or 1.

              if (   (l1s.next_time.fn_mod13 == 0)
                  || (l1s.next_time.fn_mod13 == 4))

                macs.dsp_usf_interrupt = 1;
       #endif

      } /* end if dynamic allocation mode */

      /*---------------------------------------------------------*/
      /* Fixed allocation mode                                   */
      /*---------------------------------------------------------*/

      if (SET_PTR->mac_mode == FIX_ALLOC_NO_HALF)
      {
        UWORD8   i;
        UWORD32  blk_id = l1s.next_time.block_id;

        /* Allocation bitmap isn't exhausted */
        /*-----------------------------------*/

        if(macs.fix_alloc_exhaust == FALSE)
        {

          // Allocation exhaustion detection
          //---------------------------------

          //
          // 0  current_fn  End of allocation                      STI  FN_MAX
          // |-----|---------|--------------------------------------|----||
          // |.....|.........|                                      |....||
          // |-----|---------|--------------------------------------|----||
          //
          // In this case, the Starting time is elapsed but current_fn < STI
          // ---> We must have (current block_ID - STI_block_ID) > 0
          //
          if (blk_id < macs.sti_block_id)
          {
            blk_id += MAX_BLOCK_ID;  // MAX_BLOCK_ID is the block ID obtained when fn = FN_MAX

          } // End of FN MAX modulo management

          #if TESTMODE
            // Never let exhaust the UL allocation in test mode packet transfer operation
            if (l1_config.TestMode)
              blk_id = macs.sti_block_id;
          #endif

          /* Allocation bitmap isn't exhausted */
          if (blk_id < (macs.sti_block_id + SET_PTR->ul_tbf_alloc->fixed_alloc.bitmap_length))
          {

            // Uplink allocation
            //------------------

            // Resources are allocated according to the allocation bitmap or ts_override
            macs.tx_allocation = (UWORD8)
                       (  SET_PTR->ul_tbf_alloc->timeslot_alloc
                        & (  SET_PTR->ul_tbf_alloc->fixed_alloc.bitmap[blk_id - macs.sti_block_id]
                           | SET_PTR->ts_override));
            // Delay
            i = l1a_l1s_com.dl_tn - RXTX_DELAY;
            if (i > MAX_TS_NB)
              macs.tx_allocation >>= (-i);
            else
              macs.tx_allocation <<= i;

            // Monitoring
            //-----------

            if ((l1s.next_time.fn_mod13 == 0) || (macs.rx_blk_period != l1s.next_time.block_id))
            {
              // Last frame was an idle frame or was used for another task --> considered as a free frame
              tra_before_frame = 8;
            }
            else
            {
              tra_before_frame = macs.tra_gap;  // Tra gap of last TDMA frame is saved
            }

            // DOWNLINK CONTROL TIMESLOT ALLOCATION

            if (  SET_PTR->ul_tbf_alloc->timeslot_alloc
                & (MASK_SLOT0 >> SET_PTR->ul_tbf_alloc->fixed_alloc.ctrl_timeslot))
            {
              // Tra and Ttb met --> allocates the downlink control timeslot
              macs.rx_allocation |= (UWORD8)
                               (MASK_SLOT0 >> (  SET_PTR->ul_tbf_alloc->fixed_alloc.ctrl_timeslot
                                               - l1a_l1s_com.dl_tn));
            }

            // UPLINK PDCH MONITORING
            else
            {
              // The control timeslot has been released

              // Allocates uplink TBF timeslots for monitoring
              macs.rx_allocation |= (UWORD8) (   SET_PTR->ul_tbf_alloc->timeslot_alloc
                                              << l1a_l1s_com.dl_tn);
            }

            // If Ttb or Tra not respected, the problem comes from uplink TBF
            // monitored timeslots
            // If Ttb not respected, downlink resources are removed
            for(i = 0; i <= MS_CLASS[SET_PTR->multislot_class].ttb; i++)
              macs.rx_allocation &=  (~((UWORD8)(macs.tx_allocation << i)));

            // Tra respect according to the current allocation
            for(i = 0; i <= MS_CLASS[SET_PTR->multislot_class].tra; i++)
              macs.rx_allocation &= (~((UWORD8)(macs.tx_allocation << (8-i))));

            // Tra respect according to the last allocation
            if (tra_before_frame < MS_CLASS[SET_PTR->multislot_class].tra)
            {
              macs.rx_allocation &= MASK_ALL_SLOTS >> (MS_CLASS[SET_PTR->multislot_class].tra - tra_before_frame);
            }

            #if MACS_STATUS
              if (macs.rx_allocation == 0)
              {
                l1ps_macs_com.macs_status.id[l1ps_macs_com.macs_status.nb] = NO_RX_MONITORED;
                l1ps_macs_com.macs_status.nb ++;
              }
            #endif

            // Last block of the allocation bitmap... next block will use new settings
            // for timeslot monitoring
            if ((blk_id + 1) == (macs.sti_block_id + SET_PTR->ul_tbf_alloc->fixed_alloc.bitmap_length))
            {
              macs.fix_alloc_exhaust               = TRUE;
              // Informs L1S
              l1ps_macs_com.fix_alloc_exhaust_flag = TRUE;
            }


          } // End if "allocation bitmap isn't exhausted"
          else

          // Allocation bitmap has exhausted
          {
            macs.fix_alloc_exhaust               = TRUE;
            // Informs L1S
            l1ps_macs_com.fix_alloc_exhaust_flag = TRUE;

            // Allocates uplink TBF timeslots for monitoring
            macs.rx_allocation |= (UWORD8) (   SET_PTR->ul_tbf_alloc->timeslot_alloc
                                            << l1a_l1s_com.dl_tn);

          }

        } // End if "allocation bitmap not exhausted"

        /* Allocation bitmap is exhausted */
        /*--------------------------------*/
        else
        {
          // Allocates uplink TBF timeslots for monitoring
          macs.rx_allocation |= (UWORD8) (   SET_PTR->ul_tbf_alloc->timeslot_alloc
                                          << l1a_l1s_com.dl_tn);

        } // End if fixed allocation exhausted

      } // End of fixed allocation processing

    } // End of uplink TBF processing

    /***********************************************************/
    /* Allocation parameters checking and updating             */
    /***********************************************************/
    {
      UWORD8 ts;
      BOOL   rx_ts;
      BOOL   tx_ts;

 #if MACS_STATUS
      UWORD8 time   = INVALID;  /* Timeslot counter */
 #endif

      tx_no         = 0;
      highest_ul_ts = INVALID;
      lowest_ul_ts  = INVALID;
      highest_dl_ts = INVALID;
      lowest_dl_ts  = INVALID;

      /*---------------------------------------------------------*/
      /* Trb, Ttb parameters verification and Rx, Tx number, Sum */
      /* and highest_ul_ts parameters processing                 */
      /*---------------------------------------------------------*/

      /* We verifies all allocated uplink and downlink timeslots */
      for (ts = 0; ts < TS_NUMBER; ts ++)
      {
        rx_ts = (UWORD8) (macs.rx_allocation & (MASK_SLOT0 >> ts));
        tx_ts = (UWORD8) (macs.tx_allocation & (MASK_SLOT0 >> ts));

      #if MACS_STATUS

        /* If Rx(ts) = 0 and Tx(ts) = 0 */
        /*------------------------------*/

        if ((!rx_ts) && (!tx_ts))
        {
          /* time is incremented */
          /* If time was invalid, it becomes active */
          if (time < TS_NUMBER)
            time ++;
          if ((time == RX_SLOT)||(time == TX_SLOT))
            time = 1;
        } /* End if Rx = 0 and Tx = 0 */

        /* If Rx(ts) = 1 and Tx(ts) = 1 */
        /*------------------------------*/

        if ((rx_ts) && (tx_ts))
        {
          /* error (only type 1 mobiles are supported) */
          l1ps_macs_com.macs_status.id[l1ps_macs_com.macs_status.nb] = MS_CLASS_TIME_ERROR;
          l1ps_macs_com.macs_status.nb ++;
        } /* End if Rx = 1 and Tx = 1 */
      #endif

        /* If Rx(ts) = 1 */
        /*---------------*/

        if (rx_ts)
        {
          highest_dl_ts = ts;
        #if MACS_STATUS
          /* If time is valid (invalid=0xFF) and time<Trb --> error */
          if ((time < MS_CLASS[SET_PTR->multislot_class].trb) || (time == TX_SLOT))
          {
            l1ps_macs_com.macs_status.id[l1ps_macs_com.macs_status.nb] = MS_CLASS_TIME_ERROR;
            l1ps_macs_com.macs_status.nb ++;
          }
          time = RX_SLOT;
        #endif
          /* First Rx updating */
          if (lowest_dl_ts == INVALID)
            lowest_dl_ts = ts;

        } /* End if Rx = 1 */

        /* If Tx(ts) = 1 */
        /*---------------*/

        if (tx_ts)
        {
          /* Number of Tx is incremented and highest_ul_ts is updated */
          tx_no ++;
          highest_ul_ts = ts;
        #if MACS_STATUS
          /* If time is valid (invalid=0xFF) and time<Ttb --> error */
          if (   (time < MS_CLASS[SET_PTR->multislot_class].ttb)
              || (time == RX_SLOT))
          {
            l1ps_macs_com.macs_status.id[l1ps_macs_com.macs_status.nb] = MS_CLASS_TIME_ERROR;
            l1ps_macs_com.macs_status.nb ++;
          }
          time = TX_SLOT;
        #endif
          /* First Tx updating */
          if (lowest_ul_ts == INVALID)
            lowest_ul_ts = ts;

          #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
            if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
              trace_info.pdtch_trace.ul_status[ts] |= 0x4;
          #endif
        } /* End if Tx = 1 */

      } /* End for */

      /* Rx and Tx MS class parameters are updated */
      if (lowest_dl_ts != INVALID)
        rx = (UWORD8) (highest_dl_ts - lowest_dl_ts + 1);

      if (lowest_ul_ts != INVALID)
        tx = (UWORD8) (highest_ul_ts - lowest_ul_ts + 1);

    #if MACS_STATUS
      // If the Trb parameter isn't respected at the end of
      // the TDMA frame --> MS Class isn't suported
      // Note: we considered that the first slot of the next TDMA is always a RX
      if (time < MS_CLASS[SET_PTR->multislot_class].trb)
      {
        l1ps_macs_com.macs_status.id[l1ps_macs_com.macs_status.nb] = MS_CLASS_TIME_ERROR;
        l1ps_macs_com.macs_status.nb ++;
      }

      /*---------------------------------------------------------*/
      /* Sum, Rx and Tx parameters verification                  */
      /*---------------------------------------------------------*/

      if (  ((rx + tx) > MS_CLASS[SET_PTR->multislot_class].sum)
          ||(rx > MS_CLASS[SET_PTR->multislot_class].rx)
          ||(tx > MS_CLASS[SET_PTR->multislot_class].tx))
      {
        l1ps_macs_com.macs_status.id[l1ps_macs_com.macs_status.nb] = MS_CLASS_SUM_ERROR;
        l1ps_macs_com.macs_status.nb ++;
      }

      // If all downlink timeslots are before the first uplink timeslot or after
      // the last uplink timeslot, Rx and Tx parameters are met
      if (   (   (highest_dl_ts > lowest_ul_ts)
              || (lowest_dl_ts > lowest_ul_ts))
          && (   (highest_dl_ts < highest_ul_ts)
              || (lowest_dl_ts < highest_ul_ts)))
      {
        l1ps_macs_com.macs_status.id[l1ps_macs_com.macs_status.nb] = MS_CLASS_SUM_ERROR;
        l1ps_macs_com.macs_status.nb ++;
      }

    #endif

    } /* End of allocation parameters checking and updating */


    /***********************************************************/
    /* Uplink RLC/MAC blocks management (RLC - DSP interfaces) */
    /* PACCH/U placement (Poll response processing)            */
    /***********************************************************/

    {
      BOOL   poll;              // TRUE if the poll response is processed
      UWORD8 highest_ul_data;   // Highest uplink timeslot assigned for data transfer
      UWORD8 tx_allocation_s;   // Used for saving of macs.tx_allocation
      UWORD8 rx_allocation_s;   // Used for saving of macs.rx_allocation
      UWORD8 tx_data_s;         // Used for saving of macs.tx_data
      UWORD8 highest_ul_ts_s;   // Used for saving of highest_ul_ts
      UWORD8 lowest_ul_ts_s;    // Used for saving of lowest_ul_ts
      UWORD8 poll_resp_ts;      // Timeslot on which the MS must transmit a poll response
      UWORD8 ts;
      UWORD8 i;
          #if L1_EDA
            UWORD8 rx_monitored_s; // Used for saving of rx_monitored
          #endif

      /*---------------------------------------------------------*/
      /* Uplink buffer indexes initialization                    */
      /*---------------------------------------------------------*/

      macs.ul_buffer_index[0] = macs.ul_buffer_index[1] = macs.ul_buffer_index[2] =
        macs.ul_buffer_index[3] = macs.ul_buffer_index[4] = macs.ul_buffer_index[5] =
        macs.ul_buffer_index[6] = macs.ul_buffer_index[7] = INVALID;

      // Reset all uplink blocks CS-TYPE in order to disable the validity of blocks not sent
      for(i=0; i<4; i++)
      {
        NDB_PTR->a_du_gprs[i][0] = CS_NONE_TYPE;
        NDB_PTR->a_pu_gprs[i][0] = CS_NONE_TYPE;
      }

      /*---------------------------------------------------------*/
      /* Uplink RLC/MAC blocks request to RLC (RLC_UPLINK)       */
      /*---------------------------------------------------------*/

      /* All allocated uplink resources are used for data */
      macs.tx_data = macs.tx_allocation;
      highest_ul_data = highest_ul_ts;

      /* RLC UPLINK CALL */
      /*-----------------*/

      #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_UL_NO_TA)
          if ((SET_PTR->packet_ta.ta == 255) && (macs.tx_allocation != 0))
            Trace_uplink_no_TA();
      #endif

      #if TESTMODE
        if (l1_config.TestMode)
        {
          l1tm_rlc_uplink (tx_no, (API*) NDB_PTR->a_du_gprs);
        }
        else
      #endif
      {
        rlc_uplink(SET_PTR->assignment_id,         // Assignment ID
                   tx_no,                          // Number of timeslot that can be used
                                                   //   for uplink data block transfer
                   l1s.next_time.fn,               // Next frame number
                   SET_PTR->packet_ta.ta,          // Timing advance value
                   (API*) NDB_PTR->a_pu_gprs,      // Pointer on poll response struct
                   (API*) NDB_PTR->a_du_gprs,      // Pointer on uplink block struct
                   macs.fix_alloc_exhaust          // Set to 1 if fixed allocation exhausted
        );
      }

          #if FF_TBF
            #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
              if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_RLC_PARAM)
              {
                UWORD32 cs_type = 0;
                //Get the cs_type from the UL buffer API header
                //The cs_type format: byte0 (LSByte) -indicates the CS type of TS0
                //byte1- CS type of TS1
                //byt2 - CS type of TS2
                //byte3(MSBye) - CS type of TS3
                for (i=0;i<tx_no;i++)
                {
                                        cs_type |= ((((UWORD8) NDB_PTR->a_du_gprs[i][0]) & CS_GPRS_MASK) << (8*i));

                }
                Trace_rlc_ul_param(SET_PTR->assignment_id,         // Assignment ID
                                   l1s.next_time.fn,               // Next frame number
                                   tx_no,                          // Number of UL timeslot that can be used
                                   SET_PTR->packet_ta.ta,          // Timing advance value
                                   macs.fix_alloc_exhaust,
                                   cs_type );
              }
            #endif
          #else
      #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_RLC_PARAM)
        {
          Trace_rlc_ul_param(SET_PTR->assignment_id,         // Assignment ID
                             tx_no,                          // Number of timeslot that can be used
                                                             //   for uplink data block transfer
                             l1s.next_time.fn,               // Next frame number
                             SET_PTR->packet_ta.ta,          // Timing advance value
                             (UWORD32) NDB_PTR->a_pu_gprs,      // Pointer on poll response struct
                             (UWORD32) NDB_PTR->a_du_gprs,      // Pointer on uplink block struct
                             macs.fix_alloc_exhaust);
        }
      #endif
          #endif

      //i = 0;//OMAPS00090550

      /*---------------------------------------------------------*/
      /* Poll responses processing                               */
      /*---------------------------------------------------------*/
          #if L1_EDA
            macs.lowest_poll_ts = INVALID;
          #endif

      /* While a poll response is requested */
      for(i=0;i<=3;i++) //OMAPS00090550
      {
      	if( ((((UWORD8) NDB_PTR->a_pu_gprs[i][0]) & 0xF) == CS1_TYPE_POLL)
           || ((((UWORD8) NDB_PTR->a_pu_gprs[i][0]) & 0xF) == CS_PAB8_TYPE)
           || ((((UWORD8) NDB_PTR->a_pu_gprs[i][0]) & 0xF) == CS_PAB11_TYPE))
      {
        poll = TRUE;

        // The number of the timeslot on which the poll response is requested is converted to
        // become relative to L1 synchronization
        poll_resp_ts =   (UWORD8) NDB_PTR->a_pu_gprs[i][1]
                      -  l1a_l1s_com.dl_tn
                      +  RXTX_DELAY;

        // All timeslots on which a poll is requested are set in last_poll_response.
        // last_poll_response will be updated in FN13 = 2, 6 and 10, when the 4th control
        // task will be processed for the current block period --> we'll be sure the poll
        // responses are entirely transmitted (no BCCH monitoring)
        macs.last_poll_response |= (UWORD8) (MASK_SLOT0 >> poll_resp_ts);

        // Allocations are saved: it's useful to restore uplink slots that were removed
        // for mapping a poll response that is finally not processed
        tx_allocation_s = macs.tx_allocation;
        rx_allocation_s = macs.rx_allocation;
        tx_data_s       = macs.tx_data;
        lowest_ul_ts_s  = lowest_ul_ts;
        highest_ul_ts_s = highest_ul_ts;
            #if L1_EDA
              rx_monitored_s  = macs.rx_monitored;
            #endif
          #if L1_EDA
            //In the case of concurrent TBFs in extended dynamic mode, poll response can be canceled
            //if response is not done on concurrent timeslots
            if (((SET_PTR->allocated_tbf == BOTH_TBF) && (SET_PTR->mac_mode == EXT_DYN_ALLOC)) &&
                (!((SET_PTR->dl_tbf_alloc.timeslot_alloc) & (SET_PTR->ul_tbf_alloc->timeslot_alloc)
                & (MASK_SLOT0 >> NDB_PTR->a_pu_gprs[i][1]))))
            {
              // Poll response not done
              poll = FALSE;
            }
            else
          #endif
        /* If the requested timeslot is allocated for data transfer */
        /*----------------------------------------------------------*/

        if (macs.tx_data & (MASK_SLOT0 >> poll_resp_ts))
        {
          /* The slot is removed in tx_data */
          /* No allocation modification */
          macs.tx_data &= (UWORD8) (~(MASK_SLOT0 >> poll_resp_ts));

          #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
            RTTL1_FILL_MACS_STATUS(TX_CANCELLED_POLL, poll_resp_ts)
          #endif

        } /* End if slot allocated for data */

        /* If the poll response is requested on an invalid timeslot    */
        /* i.e:                                                        */
        /*    - Timeslot > 7 or < 0                                    */
        /*    - Timeslot that avoid the RX on the first timeslot       */
        /*      according to Ttb                                       */
        /*-------------------------------------------------------------*/

        else
        if ((poll_resp_ts > 7) || (poll_resp_ts <= MS_CLASS[SET_PTR->multislot_class].ttb))
        {
          // Poll response not done
          poll = FALSE;
        }

        /* If the Tra parameter isn't respected */
        /*--------------------------------------*/

          #if L1_EDA
            //Tra does not always apply with EDA.
            else
            if ((  (MASK_SLOT0 >> poll_resp_ts)
                & (   macs.rx_allocation
                   >> (TS_NUMBER - MS_CLASS[SET_PTR->multislot_class].tra)) && (SET_PTR->mac_mode != EXT_DYN_ALLOC))
                || ((poll_resp_ts == 6) && (l1ps_macs_com.fb_sb_task_enabled)))
          #else
        else
        if (  (MASK_SLOT0 >> poll_resp_ts)
            & (   macs.rx_allocation
               >> (TS_NUMBER - MS_CLASS[SET_PTR->multislot_class].tra)))

          #endif
        {
          // Poll response not done
          poll = FALSE;
        }

        /* Ttb and Tra respected                                         */
        /* Poll on a slot not already allocated for uplink data transfer */
        /*---------------------------------------------------------------*/

        else
        {
          /* If Ttb parameter isn't respected  */
          /*-----------------------------------*/

          // If one or several downlink timeslots are allocated between:
          //    - Ttb timeslots before the timeslot to use for poll response
          //    - AND the last slot of the frame (optimization)
          //          --> Ttb parameter isn't respected if the poll response is transmitted
          //              so the RX resources are removed

          macs.rx_allocation &= ~((UWORD8) (  macs.rx_allocation
                                            & (   MASK_ALL_SLOTS
                                               >> (  poll_resp_ts
                                                   - MS_CLASS[SET_PTR->multislot_class].ttb))));

              #if L1_EDA
                if (SET_PTR->mac_mode == EXT_DYN_ALLOC)
                {
                  UWORD8 only_monitored_ts;

                  only_monitored_ts = ~((UWORD8)(SET_PTR->dl_tbf_alloc.timeslot_alloc << l1a_l1s_com.dl_tn)) & macs.rx_monitored;
                  macs.rx_allocation &= ~(only_monitored_ts & (MASK_ALL_SLOTS >> (poll_resp_ts + 1 - RXTX_DELAY)));
                  macs.rx_monitored &= macs.rx_allocation;
                }
              #endif
          /* The requested slot is allocated */
          macs.tx_allocation |= (UWORD8)  (MASK_SLOT0 >> poll_resp_ts);

          /* Lowest, highest numbered uplink timeslot and Tx parameter are updated */

          if (poll_resp_ts < lowest_ul_ts)
            lowest_ul_ts = poll_resp_ts;

          if ((poll_resp_ts > highest_ul_ts) || (highest_ul_ts == INVALID))
            highest_ul_ts = poll_resp_ts;

          tx = (UWORD8) (highest_ul_ts - lowest_ul_ts + 1);

          /* Tx and Sum parameters checking */
          /*--------------------------------*/

          /* While Tx or Sum parameter isn't respected and the poll response hasn't */
          /* already been removed */
          while (   (   (tx > MS_CLASS[SET_PTR->multislot_class].tx)
                     || ((rx + tx) > MS_CLASS[SET_PTR->multislot_class].sum))
                 && (poll == TRUE))
          {
            /* If no uplink timeslot is used for data */
            if (macs.tx_data == 0)
            {
              /* The poll response isn't processed */
              poll = FALSE;
            }
            else
            {
              /* Highest uplink PDTCH is removed */
              macs.tx_allocation &= (UWORD8) (~(MASK_SLOT0 >> highest_ul_data));
              macs.tx_data       &= (UWORD8) (~(MASK_SLOT0 >> highest_ul_data));

              #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
                RTTL1_FILL_MACS_STATUS(TX_CANCELLED_POLL, highest_ul_data)
              #endif

              /* Lowest, highest numbered uplink timeslot and Tx parameter are updated */

              lowest_ul_ts    = INVALID;
              highest_ul_ts   = INVALID;
              highest_ul_data = INVALID;

              for (ts = 0; ts < TS_NUMBER; ts ++)
              {
                if (macs.tx_allocation & (MASK_SLOT0 >> ts))
                {
                  if (lowest_ul_ts == INVALID)
                    lowest_ul_ts = ts;
                  highest_ul_ts = ts;
                  if (macs.tx_data & (MASK_SLOT0 >> ts))
                    highest_ul_data = ts;
                }
              }

              tx = (UWORD8) (highest_ul_ts - lowest_ul_ts + 1);

            }
          } /* End while Tx or Sum parameter not met */
        } /* End of case "poll on a timeslot not already allocated for uplink data transfer"
                         " Tra and Ttb respected " */

        /* If the poll response is done */
        /*------------------------------*/
        if (poll == TRUE)
        {
          // Note: Power measurement always found because Tra met

          UWORD8   cs_type;
          UWORD16  prach_info;
              #if L1_EDA
                UWORD8 only_monitored_ts;
              #endif

          cs_type    = (UWORD8)  ((NDB_PTR->a_pu_gprs[i][0]) & 0xF);
          prach_info = (UWORD16) l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[i][2];

          if(cs_type == CS_PAB8_TYPE)
          {
            l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[i][2] = ((API)(l1a_l1s_com.Scell_info.bsic << 2)) |
                                                         ((API)(prach_info) << 8);
            l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[i][3] = 0;

            // macs.tx_prach_allocation is updated
            macs.tx_prach_allocation |= (UWORD8) (MASK_SLOT0 >> poll_resp_ts);
          }
          else
          if(cs_type == CS_PAB11_TYPE)
          {
            l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[i][2] = ((API)(prach_info) << 5);
            l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[i][3] = ((API)(l1a_l1s_com.Scell_info.bsic << 10));

            // macs.tx_prach_allocation is updated
            macs.tx_prach_allocation |= (UWORD8) (MASK_SLOT0 >> poll_resp_ts);
          }

              #if L1_EDA
                only_monitored_ts = ~((UWORD8)(SET_PTR->dl_tbf_alloc.timeslot_alloc << l1a_l1s_com.dl_tn)) & (SET_PTR->ul_tbf_alloc->timeslot_alloc<< l1a_l1s_com.dl_tn);
                //lowest_poll_ts variable is used to remove only monitored ts above a ts
                //used for a poll.
                if (SET_PTR->mac_mode == EXT_DYN_ALLOC)
                {
                  if ((poll_resp_ts - RXTX_DELAY) < macs.lowest_poll_ts)
                    macs.lowest_poll_ts = (poll_resp_ts - RXTX_DELAY);
                }
              #endif
          #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
            RTTL1_FILL_UL_PDTCH(cs_type, tx_allocation_s & (0x80 >> poll_resp_ts), poll_resp_ts + l1a_l1s_com.dl_tn)
          #endif

          // a_ul_buffer_gprs updating
          if(poll_resp_ts < 8)
          macs.ul_buffer_index[poll_resp_ts] = i + 8;

          #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
            if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
            {
			  if(poll_resp_ts < 8)
			  {
              trace_info.pdtch_trace.ul_status[poll_resp_ts] |= cs_type << 4;
              trace_info.pdtch_trace.ul_status[poll_resp_ts] |= 1;
			  }
            }
          #endif
        } // End if the poll response is processed

        /* If the poll response isn't processed */
        /*--------------------------------------*/
        else
        {
          // All allocation parameters that may have been modified to map
          // this poll response are restored
          macs.tx_allocation = tx_allocation_s;
          macs.rx_allocation = rx_allocation_s;
          macs.tx_data       = tx_data_s;
          highest_ul_ts      = highest_ul_ts_s;
          lowest_ul_ts       = lowest_ul_ts_s;

              #if FF_TBF
                #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
                  if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
                    trace_info.pdtch_trace.blk_status |= 0x01;
                #endif
              #else
        #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
          if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
          	{
            trace_info.pdtch_trace.blk_status |= 0x01;
            if(poll_resp_ts < 8)
           trace_info.pdtch_trace.ul_status[poll_resp_ts] |= 0x08;
                //Set the poll reject bit (bit3) of ul_status to 1 to indicate that the
                //requested poll response has been rejected by MACS.
          	}
        #endif
        #endif

        #if MACS_STATUS
          l1ps_macs_com.macs_status.id[l1ps_macs_com.macs_status.nb]= POLL_ERROR_MS_CLASS;
          l1ps_macs_com.macs_status.nb ++;
        #endif

        #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          RTTL1_FILL_MACS_STATUS(POLL_REJECT, poll_resp_ts)
        #endif
        } // End if the poll response isn't processed
            //The trace for poll response
            #if FF_TBF
              #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
                if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_POLL_PARAM)
                {
                   Trace_rlc_poll_param ( poll, //Indicate whether L1 is going to transmit poll resp or not: 0-no, 1-yes
                                                     l1s.next_time.fn, //next frm on whih poll resp will be sent
                                                     poll_resp_ts, //Timseslot for poll response
                                                     macs.tx_allocation,
                                                     (UWORD32)macs.tx_data,
                                                     macs.rx_allocation,
                                                     (UWORD32)macs.last_poll_response,
                                                     ((NDB_PTR->a_pu_gprs[i][0]) & CS_GPRS_MASK));

                }
              #endif
            #endif
        i ++;

      } /* End while a poll response is requested and can be mapped */
  } // End of for loop

      /*---------------------------------------------------------*/
      /* Uplink RLC/MAC data blocks processing                   */
      /*---------------------------------------------------------*/

      i                          = 0;
      ts                         = 0;
      macs.rlc_blocks_sent       = 0;

      // tx_data_s represents here the remaining timeslots that must be associated
      // with a RLC/MAC data block
      tx_data_s                  = macs.tx_data;

      /* While a timeslot is available to transmit an uplink RLC/MAC data block */
      while (tx_data_s != 0)
      {
        /* If slot is allocated for data transfer */
        if (macs.tx_data & (MASK_SLOT0 >> ts))
        {
          UWORD8 cs_type = (((UWORD8) NDB_PTR->a_du_gprs[i][0]) & 0xF);

          /* If no RLC/MAC block is sent by RLC */
          /*------------------------------------*/

          if (cs_type == CS_NONE_TYPE)
          {
            // All uplink timeslots used for data and situated after this timeslot
            // (inluding this timeslot) are removed
            macs.tx_allocation &= (UWORD8) (~(  macs.tx_data  & (MASK_ALL_SLOTS >> ts)));
            macs.tx_data &= (UWORD8) (~(MASK_ALL_SLOTS >> ts));
            tx_data_s = 0;

            #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
              RTTL1_FILL_MACS_STATUS(TX_ALLOWED_NO_BLK, ts)
                  #if FF_TBF
                    //Update the ul_status fileds for cond PDTCH trace
                    //Since this blcok doesn't have any valid CS scheme, cs_type=0 and payload=11
                    if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
                    {
                      //Bits7,6,5,4 (cs_type)of ul_status should all be zeros
                      trace_info.pdtch_trace.ul_status[ts] &= 0x0f;
                      //Make the payload as NA
                      trace_info.pdtch_trace.ul_status[ts] |= 0x03;
                    }
                  #endif
            #endif
          }

          /* Else: uplink RLC/MAC data block transfer processing */
          /*-----------------------------------------------------*/
          else
          {
            #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
              if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
              {
                trace_info.pdtch_trace.ul_status[ts] |= ((UWORD8) cs_type) << 4;
                if (cs_type == CS1_TYPE_DATA)
                {
                  //If it is CS1 block, find out whether is it is DATA or CONTROL block.
                  UWORD8 payload = (((UWORD8) NDB_PTR->a_du_gprs[i][0]) & 0xc0);
                  if (payload == DATA_BLOCK)
                  {
                    //This is a CS1 DATA block. So fillup 10 for the payload field of ul_status
                trace_info.pdtch_trace.ul_status[ts] |= 2;
               }
                  else
                  {
                    //This is a CS1 CONTROL block. So fillup 01 for the payload field of ul_status
                    trace_info.pdtch_trace.ul_status[ts] |= 1;
                  }
                }
               }
            #endif

            /* A data block is assigned to timeslot ts */
            tx_data_s &= (UWORD8) (~(MASK_SLOT0 >> ts));
            /* rlc_blocks_sent value processed */
            macs.rlc_blocks_sent ++;
            /* Uplink buffer index stored in ul_buffer_index */
            macs.ul_buffer_index[ts] = i;

            /* Next data block */
            i ++;
          }

        } /* End if slot allocated for data transfer */

        /* Next timeslot */
        ts ++;
      } /* End while */

    } /* End of poll responses / uplink blocks processing */

    /***********************************************************/
    /* Measurement gap allocation                              */
    /***********************************************************/

    l1ps_macs_meas();

  #if MACS_STATUS
    if (macs.pwr_allocation == 0)
    {
      l1ps_macs_com.macs_status.id[l1ps_macs_com.macs_status.nb] = NO_MEAS_MAPPED;
      l1ps_macs_com.macs_status.nb ++;
    }
  #endif

    // Initialize the reception block period
    macs.rx_blk_period   = NO_DL_BLK;

  } /* End if next frame is the first of a block period */

  /***********************************************************/
  /* RLC_DOWNLINK call enabling for uplink PDCH status       */
  /***********************************************************/

  //                                                           FN 13
  //    0   1   2   3    4   5   6   7    8   9   10  11   12
  // ----------------------------------------------------------
  // ||       B0      ||       B1      ||       B2      || I ||
  // ||   |   |   |   || X |   |   |   || X |   |   |   || X ||
  // ----------------------------------------------------------

  if (   (l1s.actual_time.fn_mod13 == 4)
      || (l1s.actual_time.fn_mod13 == 8)
      || (l1s.actual_time.fn_mod13 == 12))
  {
        #if (FF_TBF)
          l1ps_macs_rlc_uplink_info();
        #else
    l1ps_macs_com.rlc_downlink_call = TRUE;
        #endif

    // RTT: trace UL PDTCH blocks
    #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
      if (SELECTED_BITMAP(RTTL1_ENABLE_UL_PDTCH))
      {
        UWORD8 i,j = 0;

        for(i=2; i < 8; i++)
        {
          if (macs.tx_data & (0x80 >> i))
          {
            RTTL1_FILL_UL_PDTCH((((UWORD8) NDB_PTR->a_du_gprs[j][0]) & 0xF), 1, i)
            j++;
          }
        }
      }
    #endif
  }

  #if FF_L1_IT_DSP_USF
    } // if (l1ps_macs_com.usf_status != USF_IT_DSP)
  #endif // FF_L1_IT_DSP_USF


  /***********************************************************/
  /* MAC-S control result for LAYER 1                        */
  /***********************************************************/

  /* We update allocation structures in Layer 1 - MAC-S interface */
  l1ps_macs_com.rx_allocation       = macs.rx_allocation;
  l1ps_macs_com.tx_nb_allocation    = macs.tx_allocation & (~macs.tx_prach_allocation);
  l1ps_macs_com.tx_prach_allocation = macs.tx_prach_allocation;
  l1ps_macs_com.pwr_allocation      = macs.pwr_allocation;


  #if FF_L1_IT_DSP_USF
    // When dynamic allocation is in use for uplink TBF, notifies L1S about
    // USF uncertainty for FN%13=3 and 7
    if (l1ps_macs_com.usf_status != USF_IT_DSP)
    {
      if (macs.usf_vote_enable)
        l1ps_macs_com.usf_status = USF_AWAITED;
      else
        l1ps_macs_com.usf_status = USF_AVAILABLE;
    }
  #endif // FF_L1_IT_DSP_USF

  /***********************************************************/
  /* DSP programming                                         */
  /***********************************************************/

  // Write uplink blocks - timeslots correspondance in a_ul_buffer_gprs
  // MAC mode in d_sched_mode_gprs and the USF table in a_usf_gprs (Each frame)

  #if FF_L1_IT_DSP_USF
    if (l1ps_macs_com.usf_status != USF_AWAITED)
  #endif // FF_L1_IT_DSP_USF

  l1pddsp_transfer_mslot_ctrl
      (l1s.next_time.fn_mod13_mod4,                   // Burst number (0 to 3)
       macs.rx_allocation,                            // DL Bitmap
       macs.tx_allocation,                            // UL Bitmap
       SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_table, // USF table
       SET_PTR->mac_mode,                             // MAC mode
       macs.ul_buffer_index,                          // UL buffer index
       SET_PTR->tsc,                                  // Training sequence code
       l1a_l1s_com.dedic_set.radio_freq,              // Radio Freq. used for I/Q swap.
       l1a_l1s_com.dl_tn,                             // DL Transfer Sync. TN.
         #if FF_L1_IT_DSP_USF
           macs.dsp_usf_interrupt                         // USF interrupt activation
         #else
           macs.usf_vote_enable                           // USF vote activation
         #endif
       );


  /*****************************************************************/
  /* TBF parameters saving and updating                            */
  /* last_rx_allocation, TFI, rlc_blocks_sent and last_poll_error  */
  /*****************************************************************/

  //                                                           FN 13
  //    0   1   2   3    4   5   6   7    8   9   10  11   12
  // ----------------------------------------------------------
  // ||       B0      ||       B1      ||       B2      || I ||
  // ||   |   |   | X ||   |   |   | X ||   |   |   | X ||   ||
  // ----------------------------------------------------------

  if (   (l1s.next_time.fn_mod13 == 3)
      || (l1s.next_time.fn_mod13 == 7)
      || (l1s.next_time.fn_mod13 == 11))
  {
    // Downlink blocks to report to RLC
    macs.last_rx_alloc  = macs.rx_allocation;
    macs.rx_blk_period  = l1s.next_time.block_id + 1;
    if (macs.rx_blk_period > MAX_BLOCK_ID)
      macs.rx_blk_period -= (UWORD32) (MAX_BLOCK_ID + 1);

    // Synchronization memorization for synchro. change detection
    macs.old_synchro_ts = l1a_l1s_com.dl_tn;

    macs.tx_allocation       = 0;
    macs.tx_prach_allocation = 0;
  } /* End if FN13 = 2, 6 OR 10 */

} /* END OF L1PS_MACS_CTRL() */

/*-----------------------------------------------------------*/
/* l1ps_macs_read()                                          */
/*-----------------------------------------------------------*/
/* Parameters: global l1ps_macs_com           unchanged      */
/*             global l1s                     unchanged      */
/*             global l1a_l1s_com             unchanged      */
/*             global l1ps_dsp_com            changed        */
/*                                                           */
/* Return:                                                   */
/*                                                           */
/* Description: l1ps_macs_read checks the last received      */
/*              downlink blocks. It checks if the TFI field  */
/*              is present and good in the block header and  */
/*              write in the NDB the number of received      */
/*              received blocks and on which timeslot was    */
/*              received each data block and how much block. */
/*              Then the RLC layer is called (rlc_downlink)  */
/*-----------------------------------------------------------*/
void l1ps_macs_read(UWORD8 pr_table[8])
{
  #define NDB_PTR  l1ps_dsp_com.pdsp_ndb_ptr

  /***********************************************************/
  /* Downlink RLC/MAC block management                       */
  /***********************************************************/

  // If we are in the first frame after a block period */
  //                                                           FN 13
  //    0   1   2   3    4   5   6   7    8   9   10  11   12
  // ----------------------------------------------------------
  // ||       B0      ||       B1      ||       B2      || I ||
  // ||   |   |   |   || X |   |   |   || X |   |   |   || X ||
  // ----------------------------------------------------------
  //                                         X: Received downlink RLC/MAC block management
  if (  (l1s.actual_time.fn_mod13 == 4)
      ||(l1s.actual_time.fn_mod13 == 8)
      ||(l1s.actual_time.fn_mod13 == 12))
  {
    UWORD8            ts;            // Timeslot pointer
    BOOL              tfi_result;    // Set to 1 if the TFI field is present and good
    #if FF_TBF
      UWORD8         cs_type;
      BOOL           crc_error = 0;
    #endif

    /* For each radio block allocated for downlink transfer in the last block period */
    for (ts = 0; ts < TS_NUMBER; ts ++)
    {
      if (macs.last_rx_alloc & (MASK_SLOT0 >> ts))
      {
        l1ps_macs_header_decoding(macs.rx_no, &tfi_result, &(pr_table[ts]));

        #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
          if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
          {
            trace_info.pdtch_trace.dl_status[macs.rx_no] |= tfi_result << 4;
            #if FF_TBF
                  crc_error = NDB_PTR->a_dd_gprs[macs.rx_no][0] & (1<<B_CRC_BLOCK_ERROR);
                  //In the case of GPRS b_cs_type is 4bit info.
                  cs_type = NDB_PTR->a_dd_gprs[macs.rx_no][0] & CS_GPRS_MASK;
              #if L1_EGPRS
                }
              #endif
              //If the blcok received is in CRC error, update the bit0 of dl_status as 1
              if (crc_error == (1<<B_CRC_BLOCK_ERROR))
              {
                trace_info.pdtch_trace.dl_status[macs.rx_no] |= 0x01;
              }
              else
              {
                // No CRC error. Good Block
                //dl_cs_type used only for BINARY TRACE. We put the following limitation so that
                //in the case of EGPRS with MCS we don't enter here.
                if ((cs_type > CS1_TYPE_DATA) && (cs_type <= CS4_TYPE))
                {
                  trace_info.pdtch_trace.dl_cs_type |= ((cs_type - 3) & 3) << (macs.rx_no * 2);
                }
              }
            #else
            if (l1ps_dsp_com.pdsp_ndb_ptr->a_dd_gprs[macs.rx_no][0] & 0x0100) // CRC error
              trace_info.pdtch_trace.dl_status[macs.rx_no] = 1;
            else
            {
              // CS type
              UWORD8 cs_type = NDB_PTR->a_dd_gprs[macs.rx_no][0] & 0xf;

              if (cs_type != CS1_TYPE_DATA)
                trace_info.pdtch_trace.dl_cs_type |= ((cs_type - 3) & 3) << macs.rx_no;
            }
            #endif
          }
        #endif

        // TFI filtering result stored in the downlink block buffer header
        NDB_PTR->a_dd_gprs[macs.rx_no][0] &= (API) (TFI_BIT_MASK);
        NDB_PTR->a_dd_gprs[macs.rx_no][0] |= (API) (tfi_result << TFI_BIT_SHIFT);

        /*---------------------------------------------------------*/
        /* Timeslot and Rx_no values updating                      */
        /*---------------------------------------------------------*/

        // Timeslot number (relative to the network) on which the block was received is
        // stored in the downlink block buffer header

        NDB_PTR->a_dd_gprs[macs.rx_no][1] = ts + l1ps.read_param.dl_tn;

        #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          RTTL1_FILL_DL_PDTCH((UWORD8) (NDB_PTR->a_dd_gprs[macs.rx_no][4]),    \
                              tfi_result,                                      \
                              NDB_PTR->a_dd_gprs[macs.rx_no][0] & 0x0100 >> 8, \
                              NDB_PTR->a_dd_gprs[macs.rx_no][0] & 0x000f,      \
                              ts + l1ps.read_param.dl_tn)
        #endif

        macs.rx_no ++;

      } /* End if timeslot was allocated in downlink for the last block period */
    } /* Next timeslot (FOR statement) */

    #if FF_TBF
      //The "rlc_downlink_bufferize_param" structure is used to memorize parameters
      //over a block period in order to cope with spreading issue.
      //
      // C|W R    |
      //  |C W R  |
      //  |  C W R|       TBF 1
      //  |    C W|R <------------
      //----------------------
      //  |      C|W R <--------------
      //  |       |C W R
      //  |       |  C W R       TBF 2
      //  |       |    C W R
      //                   ^
      //                   |
      //                   worst case where the rlc_downlink() function can be called
      //                   when spreading occurs

      //the case above depicts a new TBF assignment without change of the synchronization:
      //as the call to the rlc_downlink() function (that needs among other the assignment id parameter)
      //can be delayed due to spreading, the assignment id of TBF 1 should be memorized
      //until the call of the rlc_downlink() function that handles the last block of TBF 1

      l1ps_macs_com.rlc_downlink_bufferize_param.allocated_tbf = l1ps.read_param.allocated_tbf;
      l1ps_macs_com.rlc_downlink_bufferize_param.assignment_id = l1ps.read_param.assignment_id;
      #if L1_EGPRS
        l1ps_macs_com.rlc_downlink_bufferize_param.tbf_mode      = l1ps.read_param.tbf_mode;
      #endif

      // New buffer to be allocated
      macs.dl_buffer_index = INVALID;
      // Initialize spreading counter
      macs.tdma_delay = 0;

      // rlc_uplink_info() invokation in case of block skipped due to resynchro
      // See L1_MCU-CHG-17924
      if (l1s.task_status[PDTCH].current_status != RE_ENTERED)
        l1ps_macs_rlc_uplink_info();
    #endif
    /***********************************************************/
    /* RLC_DOWNLINK call enabling for downlink PDCH status     */
    /***********************************************************/
    l1ps_macs_com.rlc_downlink_call = TRUE;

    #if (TRACE_TYPE==1) || (TRACE_TYPE==4)
    #if L1_BINARY_TRACE == 0
      if (trace_info.current_config->l1_dyn_trace & (1<<L1_DYN_TRACE_DL_PDTCH_CRC))
      {
        BOOL    crc_error=0;
        UWORD8  i;

        for(i=0;i<macs.rx_no;i++)
          crc_error |= ((NDB_PTR->a_dd_gprs[i][0] & 0x0100) >> (1+i));

        Trace_Packet_Transfer(crc_error); // Previous RX blocks CRC_ERROR summary
      }
    #endif
    #endif

  } /* End if first frame after a block period */

} /* END OF L1PS_MACS_READ */

/*-----------------------------------------------------------*/
/* l1ps_macs_meas()                                          */
/*-----------------------------------------------------------*/
/* Parameters: global l1ps_macs_com           unchanged      */
/*             global l1a_l1s_com             unchanged      */
/*             static macs.rx_allocation      unchanged      */
/*             static macs.tx_allocation      unchanged      */
/*             static macs.pwr_allocation     changed        */
/* Return:                                                   */
/*                                                           */
/* Description: This function processes the power measurement*/
/*              gap according to the MS class and timeslots  */
/*              allocated in macs.tx_allocation and          */
/*              macs.rx_allocation fields.                   */
/*-----------------------------------------------------------*/
void l1ps_macs_meas()
{
  #define SET_PTR l1pa_l1ps_com.transfer.aset

  WORD8  ts                = 7;       // Timeslot pointer
  UWORD8 gap               = 0;       // Gap size counter
  UWORD8 meas              = 0;       // Temporary gap processing
  UWORD8 bitmap_rx, bitmap_tx;

  macs.pwr_allocation = 0;
  bitmap_rx = macs.rx_allocation;
  bitmap_tx = macs.tx_allocation;

  // Searching of the last allocated timeslot
  // Note: Layer 1 always synchronize on a RX event, so the Tra gap will always
  //       be found after the last allocated timeslot of a frame

  while (   (ts >= 0)
         && ((bitmap_rx & 0x01) == 0)
         && ((bitmap_tx & 0x01) == 0))
  {
    // Memorization of the timeslot
    meas |= (UWORD8) (MASK_SLOT0 >> ts);
    // Gap is incremented
    gap ++;

    bitmap_rx >>= 1;
    bitmap_tx >>= 1;
    ts --;
  }

  // Last allocated timeslot: ts
  // Power gap size: gap

  // Save the "tra gap" at the end of the frame
  macs.tra_gap = gap;

  // If Tra respected before the first Rx of the frame after
  // Here we consider that L1 is ALWAYS synchronized on a RX timeslot
  if (gap >= MS_CLASS[SET_PTR->multislot_class].tra)
  {
    // The gap is allocated
    macs.pwr_allocation |= meas;
  }
  else
  // If the first slot of the next frame is free and permit to respect the Tra parameter
  // in fixed mode
  // Notes:
  // - if Tra not respected and the current slot 0 isn't allocated --> the slot 0 of
  //   the next frame will not be allocated (only possible in Fixed mode)
  // - in all cases, only one timeslot need to be removed
  if (   (gap + 1 >= MS_CLASS[SET_PTR->multislot_class].tra)
      && (!(macs.rx_allocation & MASK_SLOT0)))
  {
    // The gap is allocated
    macs.pwr_allocation |= meas;
  }

#if L1_EDA
  //if in extended dynamic allocation and if no power measurement is set in Tra gap (Tra not fulfilled)
  //then power measurement is set in Tta gap if MS class supports it.
  if ((SET_PTR->mac_mode == EXT_DYN_ALLOC) && (!macs.pwr_allocation))
  {
    UWORD8  i = MAX_TS_NB;

    //compute tta
    while (!(macs.rx_allocation & (MASK_SLOT0 >> i)))
     i--;

    i++;
    gap = 0;
    meas = 0;
    while (!(macs.tx_allocation & (MASK_SLOT0 >> i)))
    {
     gap++;
     meas |= (UWORD8) (MASK_SLOT0 >> i);
     i++;
    }

    if (gap <= MS_CLASS[SET_PTR->multislot_class].tta)
      macs.pwr_allocation |= meas;
  }
#endif //#if L1_EDA
} /* End of l1ps_macs_meas */

/*-----------------------------------------------------------*/
/* l1ps_macs_header_decoding()                               */
/*-----------------------------------------------------------*/
/* Parameters:                                               */
/*                                                           */
/* Return:                                                   */
/*                                                           */
/* Description: This function process the TFI filtering and  */
/*              decode the PR value in the MAC header of the */
/*              block stored in buffer rx_no.                */
/*-----------------------------------------------------------*/
void l1ps_macs_header_decoding(UWORD8 rx_no, UWORD8 *tfi_result, UWORD8 *pr)
{
  UWORD8            payload;       // Payload type value in the RLC/MAC header
  UWORD8            tfi;           // TFI value
  UWORD16           mac_header[2]; // Downlink block MAC header

  *pr = 0;

  // DSP Driver
  // Downlink block MAC header reading

  mac_header[0] = NDB_PTR->a_dd_gprs[rx_no][4];
  mac_header[1] = NDB_PTR->a_dd_gprs[rx_no][5];

  *tfi_result = TFI_NOT_FILTERED;

#if TFI_FILTERING

  /*---------------------------------------------------------*/
  /* TFI Filtering                                           */
  /*---------------------------------------------------------*/

  *tfi_result = TFI_BAD;

  /* Payload reading in the block header */
  /*-------------------------------------*/

  payload = (UWORD8) (((mac_header[0]) >> PAYLOAD_SHIFT) & PAYLOAD_MASK);

  #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
    if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
    {
      trace_info.pdtch_trace.dl_status[rx_no] |= payload << 6;
      // RRBP + S/P trace
      trace_info.pdtch_trace.dl_status[rx_no] |= (mac_header[0] & (0x38)) >> 2;
    }
  #endif

  /* If the payload time isn't "RESERVED" */
  if (payload != RESERVED)
  {
    /* Data block case, processed if a downlink TBF is assigned */
    /*----------------------------------------------------------*/

    if (payload == DATA_BLOCK)
    {
      *pr = (UWORD8) ((mac_header[0] >> DATA_PR_SHIFT) & PR_MASK);

      if ((l1ps.read_param.allocated_tbf == DL_TBF) || (l1ps.read_param.allocated_tbf == BOTH_TBF))
      {
        // TFI value reading
        tfi = (UWORD8) ((mac_header[0] & DATA_TFI_MASK) >> DATA_TFI_SHIFT);

        // Downlink TFI control
        if (tfi == l1ps.read_param.dl_tfi)
        {
          *tfi_result    = TFI_GOOD;
        }
      } // End if "downlink TBF enabled"

    } /* End of data block case */

    /* Control block case */
    /*--------------------*/

    else
    {
      /* Optionnal field is no present */
      if (payload == CTRL_NO_OPTIONAL)
        *tfi_result = TFI_NOT_PRESENT;

      /* Optionnal field is present */
      if (payload == CTRL_OPTIONAL)
      {
        *pr = (UWORD8) ((mac_header[1] >> CTRL_PR_SHIFT) & PR_MASK);

        /* AC = 1 : TFI is present */
        if (mac_header[0] & AC_MASK)
        {
          // TFI value reading
          tfi = (UWORD8) ((mac_header[1] & CTRL_TFI_MASK) >> CTRL_TFI_SHIFT);

          /* If direction is downlink TBF (D = 1) and a downlink TBF is in progress */
          if ( mac_header[1] & MASK_D)
          {
            if (   (l1ps.read_param.allocated_tbf == DL_TBF)
                || (l1ps.read_param.allocated_tbf == BOTH_TBF))
            {
              // Downlink TFI value is checked
              if (tfi == l1ps.read_param.dl_tfi)
              {
                *tfi_result = TFI_GOOD;
              }
            }
          } /* End if direction is downlink */

          /* If direction is uplink TBF (D = 0) and an uplink TBF is in progress */
          else if (   (l1ps.read_param.allocated_tbf == UL_TBF)
                   || (l1ps.read_param.allocated_tbf == BOTH_TBF))
          {
            // Uplink TFI value is checked
            if (tfi == l1ps.read_param.ul_tfi)
            {
              *tfi_result = TFI_GOOD;
            }

          } /* End if direction is uplink */

        } /* End if AC = 1 */

        /* AC = 0 : TFI is no present */
        else
          *tfi_result = TFI_NOT_PRESENT;

      } // End if control block with optionnal field

    } // End of control block case

  } // End if PAYLOAD != "RESERVED"

#endif

  /*---------------------------------------------------------*/
  /* pr_table updating                                       */
  /*---------------------------------------------------------*/

  if(l1ps.read_param.dl_pwr_ctl.p0 == 255)
    *pr = 0; // PR unused in "No power control" mode
  else
    *pr = PR_CONVERSION[l1ps.read_param.dl_pwr_ctl.bts_pwr_ctl_mode][*pr];

  // If TFI isn't good
  if (*tfi_result != TFI_GOOD)
  {
    // Set bit 7 to 1
    *pr |= 0x80;
  }

}

/*-----------------------------------------------------------*/
/* l1ps_macs_rlc_downlink_call()                             */
/*-----------------------------------------------------------*/
/* Parameters:                                               */
/*                                                           */
/* Return:                                                   */
/*                                                           */
/* Description: This function is called at the end of L1S    */
/*              execution if RLC_DOWNLINK must be called.    */
/*                                                           */
/*-----------------------------------------------------------*/
void l1ps_macs_rlc_downlink_call(void)
{
  UWORD8 i;
  #if FF_TBF
    UWORD32         fn;
    BOOL            rlc_dl_call = FALSE;
    API*            rlc_buffer;
    API*            dummy_rlc_buffer = NULL;

    //correct reporting of FN to L3 should be TDMA 4, 8 or 12 of MF13
    fn=l1s.actual_time.fn-l1s.actual_time.fn_mod13_mod4;

    //when fn is in first block of the MF13 (which value is not a correct value
    //to report to upper layer) then fn should be decremented so that fn%13 = 12
    if(l1s.actual_time.fn_mod13 <= 3)
      fn--;

    //to cope with border case
//    if (fn < 0)  //OMAPS00090550
//      fn += MAX_FN - 1; //OMAPS00090550

    // Retrieve decoded blocks from API. All payload decoded check.
    if (!rlc_downlink_copy_buffer(FALSE))
    {
      // Flag RLC call
      rlc_dl_call = TRUE;

      // RLC buffer exhaustion check
      if ((macs.dl_buffer_index == INVALID) || (l1a_l1s_com.recovery_flag))
      {
        if (macs.tdma_delay >= 3)
        {
          // No block reported ever by DSP
      //    #if (TRACE_TYPE==1) || (TRACE_TYPE==4)
      //      l1_trace_egprs(NO_BLOCKS_PASSED_TO_L3);
       //   #endif

          #if (TRACE_TYPE==5)
            trace_fct_simu("MACS ERROR: No RLC blocks passed to L3 on current frame", 0);
            sprintf(errormsg,"MACS ERROR: No RLC blocks passed to L3 on current frame");
            log_sim_error(ERR);
          #endif
        }
        else
        {
          // No RLC buffer available

          #if (TRACE_TYPE==5)
            trace_fct_simu("MACS ERROR: No free buffer to copy RLC blocks on current frame", 0);
            //sprintf(errormsg,"MACS ERROR: No free buffer to copy RLC blocks on current frame");
            //log_sim_error(ERR);
          #endif
        }

        // Dummy buffer to be reported
        rlc_buffer = (API*) dummy_rlc_buffer;
      }
      // RLC buffer has been succesfully allocated
      else
      {
        // RLC buffer to be reported
        rlc_buffer = (API*) &(macs.rlc_dbl_buffer[macs.dl_buffer_index].d_rlcmac_rx_no_gprs);

        #if (TRACE_TYPE == 1)||(TRACE_TYPE == 4)
          #if L1_BINARY_TRACE == 0
            if (trace_info.current_config->l1_dyn_trace & (1<<L1_DYN_TRACE_DL_PDTCH_CRC))
            {
              BOOL    crc_error=0;
              UWORD8  i;
              #if L1_EGPRS
                if (l1ps_macs_com.rlc_downlink_bufferize_param.tbf_mode == TBF_MODE_EGPRS)
                {
                  for(i=0;i<macs.rlc_dbl_buffer[macs.dl_buffer_index].d_rlcmac_rx_no_gprs;i++)
                    crc_error |= ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & (1 << B_CRC_BLOCK_ERROR)) >> (1+i));
                }
                else
              #endif
                {
                  for(i=0;i<macs.rlc_dbl_buffer[macs.dl_buffer_index].d_rlcmac_rx_no_gprs;i++)
                    crc_error |= ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_gprs[i][0] & (1 << B_CRC_BLOCK_ERROR)) >> (1+i));
                }
              Trace_Packet_Transfer(crc_error); // Previous RX blocks CRC_ERROR summary
            }
          #endif
        #endif
        #if L1_EGPRS
          // IR testing specific trace
          #if (TRACE_TYPE == 1)||(TRACE_TYPE == 4)
            if ((trace_info.current_config->l1_dyn_trace & (1<<L1_DYN_TRACE_IR))
                && (l1ps_macs_com.rlc_downlink_bufferize_param.tbf_mode == TBF_MODE_EGPRS))
            {
              UWORD8 j ;
              // Clear ir trace variables
              trace_info.ir_trace.crc    = 0;
              trace_info.ir_trace.mcs    = 0;
		          trace_info.ir_trace.status_ir_tfi = 0;
              trace_info.ir_trace.puncturing = 0;

              for(j=0;j<MS_CLASS[MAX_CLASS].rx;j++)
              {
                trace_info.ir_trace.bsn[j] = 0;
                trace_info.ir_trace.cv_bep_egprs[j]= 0;
		            trace_info.ir_trace.mean_bep_egprs[j] = 0;
              }

              // Retrieve IR info from every PDCH
              for(i=0;i<macs.rlc_dbl_buffer[macs.dl_buffer_index].d_rlcmac_rx_no_gprs;i++)
              {
                UWORD16 crc;
                UWORD16 bsn1, bsn2;
                UWORD8  mcs;
                UWORD8  cps;
                UWORD8  k;

                // retrieve coding scheme
                mcs = (macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & CS_EGPRS_MASK);

                j = k = 0 ;

                // retrieve BSN
                switch (mcs)
                {
                  case CS1_TYPE_POLL:
                  case CS1_TYPE_DATA:
                  case CS2_TYPE:
                  case CS3_TYPE:
                  case CS4_TYPE:
                    // GPRS data block
                    if ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & (1 << B_CRC_BLOCK_ERROR)) || (mcs == CS1_TYPE_POLL))
                      bsn1 = 0xffff;
                    else
                      bsn1 = ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][SIZE_DSP_HEADER_EGPRS + 1] >> 1) & 0x7f);
                    bsn2 = 0;

                    crc = (macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & ((1 << B_CRC_HEADER_ERROR) + (1 << B_CRC_PAYLOAD1_ERROR) + (1 << B_CRC_BLOCK_ERROR)));
                    crc = (crc >> B_CRC_HEADER_ERROR);
                    break;

                  case MCS1_TYPE:
                  case MCS2_TYPE:
                  case MCS3_TYPE:
                  case MCS4_TYPE:
                    // rlc mac header type 3
                    cps = ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][SIZE_DSP_HEADER_EGPRS + 1]) >> 9) & 0x0F ;
                    j = 255 ; // cps is set
                  case MCS5_TYPE:
                  case MCS6_TYPE:
                      // rlc mac header type 2
                    if (j != 255)
                    {
                      cps = ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][SIZE_DSP_HEADER_EGPRS + 1]) >> 9) & 0x07 ;
                    }
                    do
                    {
                      if (cps == CPS_value1_6[k][mcs-MCS1_TYPE])
                      { // set puncturing scheme for payload 1 and 2: 0x01 PS1 0x10 PS2 0x11 PS3 related to time slot i
                        trace_info.ir_trace.puncturing |= (((k+1) << 2) << (4*(3-i))) ;
                        break ;
                      }
                      k++;
                    } while (k < 3) ;

                    // EGPRS data block, Header Type 2 and 3
                    if (macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & (1 << B_CRC_HEADER_ERROR))
                      bsn1 = 0xffff;
                    else
                    {
                      bsn1 = ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][SIZE_DSP_HEADER_EGPRS + 0] >> 14) & 0x03);
                      bsn1 |= ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][SIZE_DSP_HEADER_EGPRS + 1] & 0x1ff) << 2);
                    }
                    bsn2 = 0;

                    crc = (macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & ((1 << B_CRC_HEADER_ERROR) + (1 << B_CRC_PAYLOAD1_ERROR) + (1 << B_CRC_BLOCK_ERROR)));
                    crc = (crc >> B_CRC_HEADER_ERROR);
                    break;

                  case MCS7_TYPE:
                  case MCS8_TYPE:
                  case MCS9_TYPE:
                    // rlc mac header type 1
                    cps = ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][SIZE_DSP_HEADER_EGPRS + 2]) >> 3) & 0x1F ;
                    do
                    {
                      if (cps == CPS_value7_9[j][k][mcs-MCS7_TYPE])
                      {  // set puncturing scheme for payload 1 and 2: 0x01 PS1 0x10 PS2 0x11 PS3 related to time slot i
                        trace_info.ir_trace.puncturing |= ((((j+1) << 2) | (k+1)) << (4*(3-i))) ;
                        break ;
                      }
                      k++;
                      if (k == 3)
                      {
                        k = 0;
                        j++ ;
                      }
                    } while (j < 3) ;

                    // EGPRS data block, Header Type 1
                    if (macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & (1 << B_CRC_HEADER_ERROR))
                    {
                      bsn1 = 0xffff;
                      bsn2 = 0xffff;
                    }
                    else
                    {
                      bsn1 =  ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][SIZE_DSP_HEADER_EGPRS + 0] >> 14) & 0x03);
                      bsn1 |= ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][SIZE_DSP_HEADER_EGPRS + 1] & 0x1ff) << 2);

                      bsn2 =  ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][SIZE_DSP_HEADER_EGPRS + 1] >> 9) & 0x7f);
                      bsn2 |= ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][SIZE_DSP_HEADER_EGPRS + 2] & 0x07) << 7);
                      bsn2 += bsn1;
                      if (bsn2 >= 2048)
                        bsn2 -= 2048;
                    }

                    crc = (macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & ((1 << B_CRC_HEADER_ERROR) + (1 << B_CRC_PAYLOAD1_ERROR) + (1 << B_CRC_PAYLOAD2_ERROR) + (1 << B_CRC_BLOCK_ERROR)));
                    crc = (crc >> B_CRC_HEADER_ERROR);
                    break;

                  default:
                    bsn1 = 0xffff;
                    bsn2 = 0xffff;
                    crc = ((1 << B_CRC_HEADER_ERROR) + (1 << B_CRC_PAYLOAD1_ERROR) + (1 << B_CRC_PAYLOAD2_ERROR) + (1 << B_CRC_BLOCK_ERROR));
                    crc = (crc >> B_CRC_HEADER_ERROR);
                    break;
                }

                // Update IR info from current PDCH
                trace_info.ir_trace.crc    |= (crc << ((3-i)*8));
                trace_info.ir_trace.bsn[i] =  ((bsn1 << 16) | bsn2);
                trace_info.ir_trace.mcs    |= ((mcs << ((3-i)*8)));

                /* we take only the msb of cv_bep and mean_bep */
                trace_info.ir_trace.cv_bep_egprs[i] = ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][2]) >> 8) ;
                trace_info.ir_trace.mean_bep_egprs[i] = (macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][3]) ;
                if (((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & TFI_BIT_MASK) >> TFI_BIT_SHIFT) == TFI_BAD)
                  trace_info.ir_trace.status_ir_tfi |= (0x1 << (3-i)) ; /* set tfi flag to 1 if block is not for MS */
              }

              trace_info.ir_trace.fn = l1s.actual_time.fn ;
              trace_info.ir_trace.status_ir_tfi |= ((macs.rlc_dbl_buffer[macs.dl_buffer_index].dl_status & (1 << IR_OUT_OF_MEMORY))<<(7-IR_OUT_OF_MEMORY)) ;

             // Output trace
              Trace_IR (&trace_info.ir_trace) ;
            }
          #endif //(TRACE_TYPE == 1)||(TRACE_TYPE == 4)
        #endif //L1_EGPRS
        #if TESTMODE
          if (l1_config.TestMode)
          {
            BOOL    crc_error_bler; //Local var used for accumulating BLER
            UWORD8  i;

            l1tm.tmode_stats.bler_total_blocks++;
            #if L1_EGPRS
              if (l1ps_macs_com.rlc_downlink_bufferize_param.tbf_mode == TBF_MODE_EGPRS)
              {
                for(i=0;i<macs.rlc_dbl_buffer[macs.dl_buffer_index].d_rlcmac_rx_no_gprs;i++)
                {
                  //bler_total_blocks gives the total number of blocks for computing BLER
                  //The block error is assigned to crc_error_bler.
                  //If the block is in error bler_crc is incremented.
                  crc_error_bler = ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & (1 << B_CRC_BLOCK_ERROR)) >> B_CRC_BLOCK_ERROR);
                  if (crc_error_bler == TRUE)
                    l1tm.tmode_stats.bler_crc[i]++;
                }
              }
              else
            #endif
              {
                for(i=0;i<macs.rlc_dbl_buffer[macs.dl_buffer_index].d_rlcmac_rx_no_gprs;i++)
                {
                  //bler_total_blocks gives the total number of blocks for computing BLER
                  //The block error is assigned to crc_error_bler.
                  //If the block is in error bler_crc is incremented.
                  crc_error_bler = ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_gprs[i][0] & (1 << B_CRC_BLOCK_ERROR)) >> B_CRC_BLOCK_ERROR);
                  if (crc_error_bler == TRUE)
                    l1tm.tmode_stats.bler_crc[i]++;
                }
              }
          }
        #endif
      }
    }
    // Payload still awaited
    else // (!rlc_downlink_copy_buffer(FALSE)
    {
      // Check spreading limit not exceeded
      if ((macs.tdma_delay >= 3) || (l1a_l1s_com.recovery_flag))
      {
        // Free RLC buffer
        macs.rlc_dbl_buffer[macs.dl_buffer_index].d_rlcmac_rx_no_gprs = RLC_BLOCK_ACK;

        // Flag RLC call
        rlc_dl_call = TRUE;

        // Dummy buffer to be reported
        rlc_buffer = (API*) dummy_rlc_buffer;

        // No block reported ever by DSP

        #if (TRACE_TYPE==5)
          trace_fct_simu("MACS ERROR: No RLC blocks passed to L3 on current frame", 0);
          sprintf(errormsg,"MACS ERROR: No RLC blocks passed to L3 on current frame");
          log_sim_error(ERR);
        #endif
      }
      else
        // Increment spreading counter
        macs.tdma_delay++;
    } // (!rlc_downlink_copy_buffer(FALSE)

    // Function RLC_DOWNLINK_DATA() to be invoked
    if (rlc_dl_call)
    {

      rlc_downlink_data( l1ps_macs_com.rlc_downlink_bufferize_param.assignment_id, // Assignment ID
                         fn,                                                       // Frame number
                         rlc_buffer                                                // Pointer on the DL structure
                       );

      // Add the RLC_D traces in the case of EGPRS also

      #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
        if ((trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_RLC_PARAM)
           && (rlc_buffer != NULL))
        {
          UWORD8 i;
          UWORD16 dl_blk_status[4] = {0,0,0,0};
          for (i=0;i<macs.rx_no;i++)
          {
            #if (L1_EGPRS)
              if (l1ps_macs_com.rlc_downlink_bufferize_param.tbf_mode == TBF_MODE_EGPRS)
              {
                dl_blk_status[i] = A_DD_XGPRS[TBF_MODE_EGPRS][i][0];
              }
              else
              {
            #endif
                dl_blk_status[i] = (((UWORD8) NDB_PTR->a_dd_gprs[i][0]) & 0x070F);
            #if (L1_EGPRS)
              }
            #endif
          }
          Trace_rlc_dl_param(l1ps_macs_com.rlc_downlink_bufferize_param.assignment_id,
                       l1s.actual_time.fn,
                       macs.rx_no,
                       macs.rlc_blocks_sent,
                       macs.last_poll_response,
                       (dl_blk_status[1]<<16) |dl_blk_status[0], //dl_blk_status for TS1 and TS0
                       (dl_blk_status[3]<<16) |dl_blk_status[2]);//dl_blk_status for TS3 and TS2
        }
      #endif
      macs.rx_no                      = 0;
      l1ps_macs_com.rlc_downlink_call = FALSE;
    }

  #else

  #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
    if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
    {
      // If some RX have been received or some TX have been programmed
      if ((macs.last_rx_alloc != 0) || (macs.tx_allocation != 0))
      {
        // Send trace
        Trace_condensed_pdtch(macs.last_rx_alloc, macs.tx_allocation);
      }

      // Reset PDTCH trace structure
      for(i=0; i<8; i++)
      {
        trace_info.pdtch_trace.dl_status[i] = 0;
        trace_info.pdtch_trace.ul_status[i] = 0;
      }
      trace_info.pdtch_trace.dl_cs_type   = 0;
      trace_info.pdtch_trace.blk_status   = 0;
    }
  #endif


  // Last_poll_error processing
  //---------------------------

  // All slots allocated for poll response transmission (allocated in tx_allocation
  // but not in tx_data) are set to 0 (no error) in last_poll_response
  macs.last_poll_response &= (UWORD8) (~(macs.tx_allocation) | macs.tx_data);

  /* last_poll_response correspondance with network timeslot numbers */
  i = macs.old_synchro_ts - RXTX_DELAY;

  if (i > MAX_TS_NB)
  {
    macs.last_poll_response <<= (-i);
  }
  else
  {
    macs.last_poll_response >>= i;
  }

  // Store number of RX within NDB for RLC
  //--------------------------------------

  NDB_PTR->d_rlcmac_rx_no_gprs = macs.rx_no;

  #if L1_RECOVERY
    // blocks get a CRC error in case of COM error
    if (l1a_l1s_com.recovery_flag == TRUE)
    {
     // force bad CRC for 4 RX slots
      NDB_PTR->a_dd_gprs[0][0] |= 0x0100;
      NDB_PTR->a_dd_gprs[1][0] |= 0x0100;
      NDB_PTR->a_dd_gprs[2][0] |= 0x0100;
      NDB_PTR->a_dd_gprs[3][0] |= 0x0100;
    }
  #endif
  /******************/

  // Call RLC_DOWNLINK
  //------------------

  rlc_downlink( l1ps.read_param.assignment_id,            // Assignment ID
                l1s.actual_time.fn,                       // Frame number
                (API*) &(NDB_PTR->d_rlcmac_rx_no_gprs),   // Pointer on the DL structure
                 macs.rlc_blocks_sent,                    // ID of the last transmitted uplink
                                                          //   data block
                 macs.last_poll_response                  // Status of the poll responses of
              );                                          //   the last block period

  #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
    if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_RLC_PARAM)
    {
      Trace_rlc_dl_param(l1ps.read_param.assignment_id,
                         l1s.actual_time.fn,
                         (UWORD32) &(NDB_PTR->d_rlcmac_rx_no_gprs),
                         (UWORD8)  NDB_PTR->d_rlcmac_rx_no_gprs,
                         macs.rlc_blocks_sent,
                         macs.last_poll_response);
    }
  #endif

      #if TESTMODE
          if (l1_config.TestMode)
          {
            BOOL    crc_error_bler; //Local var used for accumulating BLER
            UWORD8  i;

            l1tm.tmode_stats.bler_total_blocks++;

            for(i=0; i < macs.rx_no; i++)
             {
                //bler_total_blocks gives the total number of blocks for computing BLER
                //The block error is assigned to crc_error_bler.
                //If the block is in error bler_crc is incremented.
               crc_error_bler = ( (NDB_PTR->a_dd_gprs[i][0] & (1 << B_CRC_BLOCK_ERROR)) >> B_CRC_BLOCK_ERROR);
               if (crc_error_bler == TRUE)
                 l1tm.tmode_stats.bler_crc[i]++;
              }
          }
        #endif

  // Clear parameters
  //-----------------

  /* All downlink blocks were processed */
  macs.last_rx_alloc              = 0;
  macs.rx_no                      = 0;
  macs.rlc_blocks_sent            = 0;
  macs.last_poll_response         = 0;

  l1ps_macs_com.rlc_downlink_call = FALSE;

  // Reset CS type.
  //---------------
  #if (DSP == 33)  || (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37) || (DSP == 38) || (DSP == 39)
    NDB_PTR->a_dd_gprs[0][0] = NDB_PTR->a_dd_gprs[1][0] = NDB_PTR->a_dd_gprs[2][0] =
    NDB_PTR->a_dd_gprs[3][0] = NDB_PTR->a_dd_gprs[4][0] = NDB_PTR->a_dd_gprs[5][0] =
    NDB_PTR->a_dd_gprs[6][0] = NDB_PTR->a_dd_gprs[7][0] = CS_NONE_TYPE;
  #else
    NDB_PTR->a_dd_gprs[0][0] = NDB_PTR->a_dd_gprs[1][0] = NDB_PTR->a_dd_gprs[2][0] =
    NDB_PTR->a_dd_gprs[3][0] = CS_NONE_TYPE;
  #endif
#endif
}//void l1ps_macs_rlc_downlink_call(void)
#if FF_TBF
/*-----------------------------------------------------------*/
/* l1ps_macs_rlc_uplink_info()                             */
/*-----------------------------------------------------------*/
/* Parameters:                                               */
/*                                                           */
/* Return:                                                   */
/*                                                           */
/* Description: This function is called in the               */
/*              l1ps_macs_ctrl() function on TDMA 4, 8 or 12 */
/*              of MF13                                      */
/*                                                           */
/*-----------------------------------------------------------*/
void l1ps_macs_rlc_uplink_info(void)
{
  UWORD8 i;

  #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
    if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
    {
      // If some RX have been received or some TX have been programmed
      if ((macs.last_rx_alloc != 0) || (macs.tx_allocation != 0))
      {
        // Send trace
        Trace_condensed_pdtch(macs.last_rx_alloc, macs.tx_allocation);
      }

      // Reset PDTCH trace structure
      for(i=0; i<8; i++)
      {
        trace_info.pdtch_trace.dl_status[i] = 0;
        trace_info.pdtch_trace.ul_status[i] = 0;
      }
      trace_info.pdtch_trace.dl_cs_type   = 0;
      trace_info.pdtch_trace.blk_status   = 0;
    }
  #endif

  // Last_poll_error processing
  //---------------------------

  /* All slots allocated for poll response transmission (allocated in tx_allocation
     but not in tx_data) are set to 0 (no error) in last_poll_response */
  macs.last_poll_response &= (UWORD8) (~(macs.tx_allocation) | macs.tx_data);

  /* last_poll_response correspondance with network timeslot numbers */
  i = macs.old_synchro_ts - RXTX_DELAY;

  if (i > MAX_TS_NB)
  {
    macs.last_poll_response <<= (-i);
  }
  else
  {
    macs.last_poll_response >>= i;
  }

  #if L1_EGPRS
    //sub_mode is ON
    if (l1ps_macs_com.loop_param.sub_mode == TRUE)
    {
      macs.rlc_blocks_sent    = 0;
      macs.last_poll_response = 0;
    }
  #endif

  rlc_uplink_info(l1ps.read_param.assignment_id,
                  l1s.actual_time.fn,
                  macs.rlc_blocks_sent,
                  macs.last_poll_response);

  //While the initialization of these variables is performed in the
  //l1ps_macs_rlc_downlink_call() for GPRS, for EGPRS the initialization
  //is done here below. Note that it is still performed on frame index 0
  //of MF13 in the l1s_end_manager() function whether in EGPRS or GPRS mode.
  //The variable below is set in the l1ps_macs_ctrl() function on frame index 2 of MF13
  //and stores the rx allocation. This allocation is used later in the l1ps_macs_read()
  //function to pass the blocks that were received in the previous block period.
  macs.last_rx_alloc              = 0;
  macs.rlc_blocks_sent            = 0;
  macs.last_poll_response         = 0;
}

/*-----------------------------------------------------------*/
/* rlc_downlink_copy_buffer()                                */
/*-----------------------------------------------------------*/
/* Parameters:  isr: flag that indicates whether the call    */
/*                   of this function is performed at the    */
/*                   beginning of the hisr() function        */
/*                   (hisr = TRUE) or not.                   */
/*                                                           */
/* Return:      missing_payload: flag that indicates if still*/
/*                               payloads are missing after  */
/*                               the copy                    */
/*                                                           */
/* Description: This function is called in the hisr()        */
/*              function with hisr = TRUE and in the         */
/*              l1ps_macs_rlc_downlink_call() function       */
/*              with hisr = FALSE                            */
/*-----------------------------------------------------------*/
  UWORD8 rlc_downlink_copy_buffer(UWORD8 isr)
  {
    BOOL missing_payload   = FALSE;
    BOOL allocation_needed = FALSE;

    UWORD32 i,j;

    // Downlink blocks expected
    if (l1ps_macs_com.rlc_downlink_call)
    {
      // Not in TDMA3 unless we are in ISR so we may have blocks to copy
      if ( ((macs.tdma_delay < 3) && (!isr)) // No logical XOR in C
           || ((macs.tdma_delay >= 3) && (isr))
         )

      {
        // Test buffer allocation requirement if not already allocated
        if (macs.dl_buffer_index == INVALID)
        {
            allocation_needed = TRUE;
        }

        // Look for an available buffer and initialize it
        if (allocation_needed)
        {
          for (i = 0; i < NBR_SHARED_BUFFER_RLC; i++)
          {
            //as soon as one free block is found
            if (macs.rlc_dbl_buffer[i].d_rlcmac_rx_no_gprs == RLC_BLOCK_ACK)
            {
              // Store buffer index
              macs.dl_buffer_index = i;
              // Store number of blocks in buffer passed to RLC
              macs.rlc_dbl_buffer[i].d_rlcmac_rx_no_gprs = macs.rx_no;
              break;
            }
          }
        }

        // Copy available blocks if buffer allocated
        if (macs.dl_buffer_index != INVALID)
        {
          // GPRS mode, no spreading
        #if L1_EGPRS
          if (l1ps_macs_com.rlc_downlink_bufferize_param.tbf_mode == TBF_MODE_GPRS)
        #endif
          {
            // Copy whole bunch of blocks (4 downlink, worst case)
            for (i=0;i<NBR_BUFFER_GPRS;i++)
            {
              memcpy((char*) &macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_gprs[i][0],
                     (char*) A_DD_XGPRS[TBF_MODE_GPRS][i],
                     SIZE_GPRS_DL_BUFF * sizeof(API));
            }
          }
          // EGPRS mode
        #if L1_EGPRS
          else
          {
            // Parse every expected block
            for (i = 0; i < macs.rx_no; i++)
            {
              // If not already copied
              if ((macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0] & CS_EGPRS_MASK) == CS_NONE_TYPE)
              {
                // New block available in API
                if (A_DD_XGPRS[TBF_MODE_EGPRS][i][0] & (1 << B_BLK_READY))
                {
                  // Copy block from API to SRAM
                  memcpy((char*) &macs.rlc_dbl_buffer[macs.dl_buffer_index].buffer.a_dd_egprs[i][0],
                         (char*) A_DD_XGPRS[TBF_MODE_EGPRS][i],
                         SIZE_EGPRS_DL_BUFF * sizeof(API));

                  // Acknowledge DSP
                  A_DD_XGPRS[TBF_MODE_EGPRS][i][0] &= ~(1 << B_BLK_READY);
                }
                else
                  missing_payload = TRUE;
              }
            }
          } // TBF mode
        #endif

          // Store "missing payload" flag used for TDMA3 limit case.
          macs.dl_missing_payload = missing_payload;

        } // Buffer is allocated
        else
        {
          // No buffer allocated yet
          if (allocation_needed)
            // RLC Buffer exhaustion - abort
            missing_payload = FALSE;
          else
           // No buffer allocated yet as no block present
           missing_payload = TRUE;

          // Still some buffer expected
          macs.dl_missing_payload = TRUE;
        }
      } // Not in ISR TDMA 0,1,2, or ISR from TDMA 3
      else if ((macs.tdma_delay >= 3) && (!isr))
      {
        // End of L1S in TDMA3, we need to report the status from ISR call.
        missing_payload = macs.dl_missing_payload;
      }

      // IR status reporting (relevant for EGPRS only)
      //----------------------------------------------

      // RLC buffer allocated, all blocks received
      if ((macs.dl_buffer_index != INVALID) && (!missing_payload))
      {
      #if L1_EGPRS
        if ( (l1ps_macs_com.rlc_downlink_bufferize_param.tbf_mode == TBF_MODE_EGPRS)
             && (l1ps_dsp_com.edsp_ndb_ptr->d_modem_status_egprs & (1 << B_IR_OUT_OF_MEM)) )
        {
          // EGPRS TBF mode, IR out of memory status flag is set
          macs.rlc_dbl_buffer[macs.dl_buffer_index].dl_status |= (1 << IR_OUT_OF_MEMORY);
        }
        else
      #endif
        {
          // GPRS TBF mode or EGPRS but IR out of memory not detected
          macs.rlc_dbl_buffer[macs.dl_buffer_index].dl_status &= (~(1 << IR_OUT_OF_MEMORY));
        }
      }
    } // if (l1ps_macs_com.rlc_downlink_call)

    // Return blocks receipt completion status
    return missing_payload;
}

#endif //FF_TBF

#if TESTMODE
  //===========================================================================
  // Function called instead of l1ps_macs_ctrl if CMU200 loop mode is selected
  //===========================================================================

  void l1ps_tmode_macs_ctrl(void)
  {
    #define NDB_PTR  l1ps_dsp_com.pdsp_ndb_ptr
    #define SET_PTR  l1pa_l1ps_com.transfer.aset

    NDB_PTR->a_du_gprs[0][0]     = l1_config.tmode.tx_params.coding_scheme;

    /* Enable loop */
    NDB_PTR->d_sched_mode_gprs |= (1<<6);

    // Force single slot allocation for CMU loop: 1RX, 1TX
    macs.rx_allocation = 0x80;
    macs.tx_allocation = 0x10;
    macs.tx_prach_allocation = 0;
    macs.pwr_allocation = 0;

    macs.ul_buffer_index[0] = 0xFF;                          // UL buffer index
    macs.ul_buffer_index[1] = 0xFF;
    macs.ul_buffer_index[2] = 0xFF;
    macs.ul_buffer_index[3] = 0;
    macs.ul_buffer_index[4] = 0xFF;
    macs.ul_buffer_index[5] = 0xFF;
    macs.ul_buffer_index[6] = 0xFF;
    macs.ul_buffer_index[7] = 0xFF;

    /* Disable USF management in the DSP */
    macs.usf_vote_enable = 0;

    /***********************************************************/
    /* MAC-S control result for LAYER 1                        */
    /***********************************************************/

    /* We update allocation structures in Layer 1 - MAC-S interface */
    l1ps_macs_com.rx_allocation       = macs.rx_allocation;
    l1ps_macs_com.tx_nb_allocation    = macs.tx_allocation & (~macs.tx_prach_allocation);
    l1ps_macs_com.tx_prach_allocation = macs.tx_prach_allocation;
    l1ps_macs_com.pwr_allocation      = macs.pwr_allocation;

    /***********************************************************/
    /* DSP programming                                         */
    /***********************************************************/

    // Write uplink blocks - timeslots correspondance in a_ul_buffer_gprs
    // MAC mode in d_sched_mode_gprs and the USF table in a_usf_gprs (Each frame)

    l1pddsp_transfer_mslot_ctrl
        (l1s.next_time.fn_mod13_mod4,                   // Burst number (0 to 3)
         macs.rx_allocation,                            // DL Bitmap
         macs.tx_allocation,                            // UL Bitmap
         SET_PTR->ul_tbf_alloc->dynamic_alloc.usf_table, // USF table
         SET_PTR->mac_mode,                             // MAC mode
         macs.ul_buffer_index,                          // UL buffer index
         SET_PTR->tsc,                                  // Training sequence code
         l1a_l1s_com.dedic_set.radio_freq,              // Radio Freq. used for I/Q swap.
         l1a_l1s_com.dl_tn,                             // DL Transfer Sync. TN.
       #if FF_L1_IT_DSP_USF
         macs.dsp_usf_interrupt                         // USF interrupt activation
       #else
         macs.usf_vote_enable                           // USF vote activation
       #endif
     );

   //NDB_PTR->a_ctrl_ched_gprs[0] = CS1_TYPE_DATA;
   NDB_PTR->a_ctrl_ched_gprs[0] = NDB_PTR->a_du_gprs[0][0];

  }
#endif
#endif // L1_GPRS
