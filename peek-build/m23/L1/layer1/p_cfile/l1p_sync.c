/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1P_SYNC.C
 *
 *        Filename l1p_sync.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#define  L1P_SYNC_C

//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

#include "l1_macro.h"
#include "l1_confg.h"

#if L1_GPRS

#if (CODE_VERSION == SIMULATION)
  #include <string.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_time.h"
  #include <l1_trace.h>
  #if TESTMODE
    #include "l1tm_defty.h"
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
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "l1_varex.h"
  #include "l1_signa.h"
  #include "l1_proto.h"

  #if L2_L3_SIMUL
    #include "hw_debug.h"
  #endif

  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"
  #include "l1p_prot.h"
  #include "l1p_mfta.h"
  #include "l1p_sign.h"
  #include "l1p_macr.h"
  #include "l1p_proto.h"
#else
  #include <string.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_time.h"

  #if TESTMODE
    #include "l1tm_defty.h"
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
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "l1_varex.h"
  #include "l1_signa.h"
  #include "l1_proto.h"
  #include "l1_trace.h"

  #if L2_L3_SIMUL
    #include "hw_debug.h"
  #endif

  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"
  #include "l1p_prot.h"
  #include "l1p_mfta.h"
  #include "l1p_sign.h"
  #include "l1p_macr.h"
  #include "l1p_proto.h"
#endif

#if(RF_FAM == 61)
	#include "l1_rf61.h"
#endif

#if (GSM_IDLE_RAM !=0)
  #if (OP_L1_STANDALONE == 0)
    #include "csmi/sleep.h"
  #else
    #include "csmi_simul.h"
  #endif
#endif
#include "tpudrv61.h"
/*-------------------------------------------------------*/
/* Prototypes of external functions used in this file.   */
/*-------------------------------------------------------*/
void   l1ps_tcr_ctrl           (UWORD8 pm_position);
void   l1pddsp_meas_ctrl       (UWORD8 nbmeas, UWORD8 pm_pos);
void   l1pddsp_meas_read       (UWORD8 nbmeas, UWORD16 *pm_read);
void   l1pctl_transfer_agc_init();
UWORD8 l1pctl_pgc              (UWORD8 pm, UWORD8 last_known_il, UWORD8 lna_off, UWORD16 radio_freq);
void   l1ps_bcch_meas_ctrl     (UWORD8 ts);

//#pragma DUPLICATE_FOR_INTERNAL_RAM_END


#if (MOVE_IN_INTERNAL_RAM == 0) // Must be followed by the pragma used to duplicate the funtion in internal RAM
//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

/*-------------------------------------------------------*/
/* l1ps_transfer_mode_manager()                          */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1ps_transfer_mode_manager()
{
  BOOL   block_boundary = TRUE;
  UWORD8 current_assignment_command = NO_TBF;
  #if FF_TBF
    BOOL   tbf_update_synchro_forced = FALSE;
  #endif

  //====================================
  // NEW configuration management
  //====================================

  if(!l1pa_l1ps_com.transfer.semaphore)
  // IF Transfer parameter structure protected,
  //   No action within L1S.
  {
    WORD8  i;
    UWORD8 min_synchro_ts = 7;
    BOOL   new_tbf_installed = FALSE;
    T_PACKET_TA *current_ta_config = NULL;

    // In packet transfer mode, we only detect STI at block boundaries in order to udpate the
    // ASET structure after the last Control of the previous TBF
    if (l1a_l1s_com.l1s_en_task[PDTCH] == TASK_ENABLED)
    {
      if ((l1s.next_time.fn_mod13 != 0)&&(l1s.next_time.fn_mod13 != 4)&&
           (l1s.next_time.fn_mod13 != 8))
      {
        block_boundary = FALSE;
      }
    }
    // Delay STI detection when a poll response hasn't already been answered
    // for transition to Packet transfer mode
    else if (l1a_l1s_com.l1s_en_task[POLL] == TASK_ENABLED)
    {
      block_boundary = FALSE;
    }

    // LOOK FOR NEW ASSIGNMENT...
    //===========================

    // Consider both FREE SET...
    for(i=0;i<2;i++)
    {
      // Is there a new transfer channel provided in "fset[i]"?
      if(((l1pa_l1ps_com.transfer.fset[i]->SignalCode == MPHP_ASSIGNMENT_REQ) && (block_boundary == TRUE)) ||
         (l1pa_l1ps_com.transfer.fset[i]->SignalCode == MPHP_SINGLE_BLOCK_REQ))
      {
        if(l1pa_l1ps_com.transfer.fset[i]->tbf_sti.present)
        // Starting time present.
        // Rem: starting time detected 1 frame in advance, this frame is used by
        //      SYNCHRO task.
        {
          WORD32  time_diff;
          WORD8   frame_shift=0;
          WORD8   tn_diff;


          // In packet idle mode, L1 must detect if the synchronization change will happen
          // from a timeslot N to a timeslot M < N because in this case, a frame is skipped
          // and the MS is ready two frames after the synchronization change... a radio
          // block can be missed...
          // In packet transfer, the SYNCHRO task will always been executed on BURST 1, that let
          // 2 frames before the new TBF starts
          if (l1a_l1s_com.l1s_en_task[PDTCH] != TASK_ENABLED)
          {
            frame_shift -= 1;

            tn_diff = l1pa_l1ps_com.transfer.fset[i]->transfer_synchro_timeslot -
                      l1a_l1s_com.dl_tn;

            if(tn_diff < 0)
              frame_shift -= 1;
          }
//TBF_changes

          #if FF_TBF
            // PDTCH task is enabled
            else
            {
              // Pseudo TBF for Two phase access, new TBF configuration has to
              // be installed for Starting Time taking into account that a
              // SYNCHRO task has to be scheduled before.
              if ((l1pa_l1ps_com.transfer.aset->allocated_tbf == UL_TBF) &&
                  (l1pa_l1ps_com.transfer.aset->pseudo_tbf_two_phase_acc))
              {
                 // Note: We are at block boundary.
                 if ((l1s.next_time.fn_mod13 == 0) || (l1s.next_time.fn_mod13 == 4))
                   frame_shift -= 4;
                 else // i.e. (l1s.next_time.fn_mod13 == 8)
                   frame_shift -= 5;
              }

              // Two phase access establishment on PACCH (therefore there is an
              // ongoing uplink TBF).
              if ((l1pa_l1ps_com.transfer.fset[i]->allocated_tbf == UL_TBF) &&
                  (l1pa_l1ps_com.transfer.fset[i]->pseudo_tbf_two_phase_acc))
              {
                 // Make sure single/multi block allocation
                 // if a synchro task has to be inserted i.e. :
                 // -> we are currently in EGPRS mode
                 // -> and/or we change synchronization timeslot
                 if ( (l1pa_l1ps_com.transfer.fset[i]->transfer_synchro_timeslot != l1a_l1s_com.dl_tn)
                    #if L1_EGPRS
                      || (l1pa_l1ps_com.transfer.aset->egprs_param.tbf_mode == TBF_MODE_EGPRS)
                    #endif
                      )
                 {
                   // Note: We are at block boundary.
                   if ((l1s.next_time.fn_mod13 == 0) || (l1s.next_time.fn_mod13 == 4))
                     frame_shift -= 4;
                   else // i.e. (l1s.next_time.fn_mod13 == 8)
                     frame_shift -= 5;
                 }
              }
            }
          #endif


          time_diff = ( (l1pa_l1ps_com.transfer.fset[i]->tbf_sti.absolute_fn) +
                        frame_shift - (l1s.next_time.fn % 42432) + 2*42432) % 42432;

          if((time_diff >= (32024)) && (time_diff <= (42431)))
          // Starting time has been passed...
          //---------------------------------
          {
            // For SINGLE BLOCK case, an error must be reported to L3.
            if(l1pa_l1ps_com.transfer.fset[i]->SignalCode == MPHP_SINGLE_BLOCK_REQ)
            {
              xSignalHeaderRec *msg;

              // Send confirmation msg to L3/MACA.
              msg = os_alloc_sig(sizeof(T_MPHP_SINGLE_BLOCK_CON));
              DEBUGMSG(status,NU_ALLOC_ERR)

              ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->purpose       =
                l1pa_l1ps_com.transfer.fset[i]->assignment_command;
              ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->assignment_id =
                l1pa_l1ps_com.transfer.fset[i]->assignment_id;

              ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->status        = SINGLE_STI_PASSED;

              msg->SignalCode = L1P_SINGLE_BLOCK_CON;

              // send message...
              os_send_sig(msg, L1C1_QUEUE);
              DEBUGMSG(status,NU_SEND_QUEUE_ERR)

              // Do not reset "tbf_sti.present".

              // Ignore the SINGLE BLOCK requested.
              l1pa_l1ps_com.transfer.fset[i]->SignalCode = NULL;
            }
            else
            {
              // Reset "tbf_sti.present" flag to take into account the new
              // configuration.
              l1pa_l1ps_com.transfer.fset[i]->tbf_sti.present = FALSE;
            }
          }
          else
          if(time_diff == 0)
          // Starting time corresponds to current frame...
          //----------------------------------------------
          {
            // Reset "tbf_sti.present" flag to take into account the new
            // configuration.
            l1pa_l1ps_com.transfer.fset[i]->tbf_sti.present = FALSE;
          }
          else
          // Starting time hasn't already been reached...
          //---------------------------------------------
          {
            // time_to_next_l1s_task updated with time to sti
            Select_min_time(time_diff, l1a_l1s_com.time_to_next_l1s_task);
          }
        }

        // Do we switch to a new transfer configuration?
        // We have to switch if, new set STI is passed.
        if(!l1pa_l1ps_com.transfer.fset[i]->tbf_sti.present)
        {
          // Install new configuration immediately.
          // Rem: the new channel will start at a block boundary because by construction
          // PDTCH task is started every block boundary.
          T_TRANSFER_SET   *transfer_set;

          // STI has been passed, we must switch to the new config.

          #if (TRACE_TYPE!=0)
            // Trace "starting time" on log file and screen.
            trace_fct(CST_STI_PASSED, l1a_l1s_com.Scell_info.radio_freq);
          #endif

          #if FF_TBF
            // Forces a SYNCHRO task prior to new TBF configuration if
            // -> Coming from 2 phases access TBF
            // -> Coming from (Packet) Idle
            if (((l1pa_l1ps_com.transfer.aset->allocated_tbf == UL_TBF) &&
                 (l1pa_l1ps_com.transfer.aset->pseudo_tbf_two_phase_acc))
                || (l1a_l1s_com.l1s_en_task[PDTCH] == TASK_DISABLED)
               )
            {
               // Can not be merged with test above evaluated only when a
               // starting time is present in the message.
               tbf_update_synchro_forced = TRUE;
            }
          #endif
          if (current_assignment_command == NO_TBF)
            current_assignment_command = l1pa_l1ps_com.transfer.fset[i]->assignment_command;
          else if ((current_assignment_command == UL_TBF) ||
                   (current_assignment_command == DL_TBF))
            current_assignment_command = BOTH_TBF;

          // Check if anything to keep from previous configuration.
          // Select the best timeslot for timebase synchro.

          if(l1pa_l1ps_com.transfer.aset->allocated_tbf != NO_TBF)
          {
            UWORD8  synchro_ts;

            // Check if required to take TA from previous packet assignment
            // if TA not valid in new assignment command
            if ( l1pa_l1ps_com.transfer.fset[i]->packet_ta.ta == 255)
            {
              // new TA value to be taken from previous packet TA
              l1pa_l1ps_com.transfer.fset[i]->packet_ta.ta = l1pa_l1ps_com.transfer.aset->packet_ta.ta;
            }

            switch(l1pa_l1ps_com.transfer.fset[i]->assignment_command)
            {
              case DL_TBF:
              {
                // If any, keep the UL allocation from previous set.
                if((l1pa_l1ps_com.transfer.aset->allocated_tbf == UL_TBF) ||
                   (l1pa_l1ps_com.transfer.aset->allocated_tbf == BOTH_TBF))
                {
                  T_UL_RESSOURCE_ALLOC *ul_ptr;

                  // Swap the pointers on UL parameter structures
                  ul_ptr = l1pa_l1ps_com.transfer.fset[i]->ul_tbf_alloc;
                  l1pa_l1ps_com.transfer.fset[i]->ul_tbf_alloc  =
                    l1pa_l1ps_com.transfer.aset->ul_tbf_alloc;
                  l1pa_l1ps_com.transfer.aset->ul_tbf_alloc = ul_ptr;

                  l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = BOTH_TBF;
                  l1pa_l1ps_com.transfer.fset[i]->ul_tbf_synchro_timeslot  =
                    l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot;

                  // Chose min synchro timeslot from UL and DL TBFs.
                  if(l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot <
                     l1pa_l1ps_com.transfer.fset[i]->transfer_synchro_timeslot)
                  {
                    synchro_ts = l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot;
                  }
                  else
                  {
                    synchro_ts = l1pa_l1ps_com.transfer.fset[i]->transfer_synchro_timeslot;
                  }
                }

                // No UL TBF running, select the new DL TBF synchro timeslot.
                else
                {
                  synchro_ts = l1pa_l1ps_com.transfer.fset[i]->transfer_synchro_timeslot;
                }
              }
              break;

              case UL_TBF:
              {
                // If any, keep the DL allocation from previous set.

                if((l1pa_l1ps_com.transfer.aset->allocated_tbf == DL_TBF) ||
                   (l1pa_l1ps_com.transfer.aset->allocated_tbf == BOTH_TBF))
                {
                  l1pa_l1ps_com.transfer.fset[i]->dl_tbf_alloc  =
                    l1pa_l1ps_com.transfer.aset->dl_tbf_alloc;
                  l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = BOTH_TBF;
                  l1pa_l1ps_com.transfer.fset[i]->dl_tbf_synchro_timeslot  =
                    l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot;

                  // Chose min synchro timeslot from UL and DL TBFs.
                  if(l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot <
                     l1pa_l1ps_com.transfer.fset[i]->transfer_synchro_timeslot)
                  {
                    synchro_ts = l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot;
                  }
                  else
                  {
                    synchro_ts = l1pa_l1ps_com.transfer.fset[i]->transfer_synchro_timeslot;
                  }
                }

                // No DL TBF running, select the new UL TBF synchro timeslot.
                else
                {
                  synchro_ts = l1pa_l1ps_com.transfer.fset[i]->transfer_synchro_timeslot;
                }

                // Reset Repeat allocation starting time checking
                l1pa_l1ps_com.transfer.repeat_alloc.repeat_allocation = FALSE;
                // Reset Allocation Exhaustion detection flag
                l1ps_macs_com.fix_alloc_exhaust_flag                  = FALSE;
              }
              break;

              case BOTH_TBF:
              case NO_TBF:
              default:
              {
                // Nothing to keep, everything (UL & DL) is replaced.
                synchro_ts = l1pa_l1ps_com.transfer.fset[i]->transfer_synchro_timeslot;

                // Reset Repeat allocation starting time checking
                l1pa_l1ps_com.transfer.repeat_alloc.repeat_allocation = FALSE;
                // Reset Allocation Exhaustion detection flag
                l1ps_macs_com.fix_alloc_exhaust_flag                  = FALSE;
              }
              break;

            } // end of switch(...assignment_command)

            if(synchro_ts < min_synchro_ts)
              min_synchro_ts = synchro_ts;

          } // end of if(...allocated_tbf != NO_TBF)

          else
          {
            min_synchro_ts = l1pa_l1ps_com.transfer.fset[i]->transfer_synchro_timeslot;
          }

          // New set becomes active and old becomes free.

          // Save a pointer on currently used PTCCH parameters
          current_ta_config = &l1pa_l1ps_com.transfer.aset->packet_ta;

          transfer_set                                  = l1pa_l1ps_com.transfer.aset;
          l1pa_l1ps_com.transfer.aset                   = l1pa_l1ps_com.transfer.fset[i];
          l1pa_l1ps_com.transfer.fset[i]                = transfer_set;
          l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = NO_TBF;

          l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot = min_synchro_ts;

          // Set local flag.
          new_tbf_installed = TRUE;

          if(l1pa_l1ps_com.transfer.aset->SignalCode == MPHP_ASSIGNMENT_REQ)
          // Assignement confirmation message is sent
          {
            xSignalHeaderRec *msg;

            // Send confirmation msg to L3/MACA.
            msg = os_alloc_sig(sizeof(T_L1P_TRANSFER_DONE));
            DEBUGMSG(status,NU_ALLOC_ERR)

            msg->SignalCode = L1P_TRANSFER_DONE;

            ((T_L1P_TRANSFER_DONE *) (msg->SigP))->assignment_id =
              l1pa_l1ps_com.transfer.aset->assignment_id;

            // Insert "t_difference" information in L1P_TRANSFER_DONE msg
            // will be used in Ncell Dedic6 state machine to request a reset
            // or not of the state machine
            ((T_L1P_TRANSFER_DONE *) (msg->SigP))->tn_difference =
              min_synchro_ts - l1a_l1s_com.dl_tn;


            // detect the Transition IDLE -> Transfer
            if (l1a_l1s_com.l1s_en_task[PDTCH] == TASK_ENABLED)
               ((T_L1P_TRANSFER_DONE *) (msg->SigP))->Transfer_update = TRUE;
            else
               ((T_L1P_TRANSFER_DONE *) (msg->SigP))->Transfer_update = FALSE;


            os_send_sig(msg, L1C1_QUEUE);
            DEBUGMSG(status,NU_SEND_QUEUE_ERR)
          }

          // New config has been acknowledeged, clear SignalCode...
          l1pa_l1ps_com.transfer.fset[i]->SignalCode = NULL;
          l1pa_l1ps_com.transfer.aset->SignalCode    = NULL;

        } // end if(...tbf_sti.present)
      } // end of if(...SignalCode == MPHP_ASSIGNMENT_REQ)
    }

    if(new_tbf_installed == TRUE)
    {
      // Start the new configuration
      //----------------------------

      // Enable PACKET tasks.
      {
        // Set assignment_command to the last enabled TBF type.
        // This permits MAC-S to correctly manage TBF boundary conditions.
        l1pa_l1ps_com.transfer.aset->assignment_command = current_assignment_command;

        // Flag the new configuration to MACS.
        l1ps_macs_com.new_set = TRUE;

        // Flag the new configuration in order to update the Read set parameters
        // in the first Read phase of the new TBF
        // This permits to start using the new aset parameters for the first Control of
        // the first block of the new TBF and to keep the parameters needed for the
        // last read phase of the last block of the previous TBF.
        l1ps.read_param.new_set = TRUE;

// TBF_changes
        #if !FF_TBF
        // We need to detect that we just leaving CS/P Idle mode to enter
        // in Packet Transfer mode. Then we have to enable SYNCHRO task on dectection
        // of a mode change (Idle or Packet idle -> Packet transfer).
        // Note: This check can't be gathered with the one done a little bit later
        // on tn_difference and SINGLE task from the fact that the allocated_tbf
        // is checked and PDTCH enabled.
        if(l1a_l1s_com.task_param[SYNCHRO] == SEMAPHORE_RESET)
        {
          if(l1a_l1s_com.l1s_en_task[PDTCH] == TASK_DISABLED)
          {
            l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;
            l1a_l1s_com.dsp_scheduler_mode = GPRS_SCHEDULER;
          }
        }
        #endif

        // Disable interference measurements
        l1a_l1s_com.l1s_en_task[ITMEAS]  = TASK_DISABLED;

        // Check for Continuous Timing advance procedure.
        // Enable PTCCH task if required.
        if((l1pa_l1ps_com.transfer.aset->packet_ta.ta_index != 255) &&
           (l1pa_l1ps_com.transfer.aset->packet_ta.ta_tn    != 255))
        {
          // No action when the configuration is the same as the current one.
		if(current_ta_config != NULL)
		{
          if((l1pa_l1ps_com.transfer.aset->packet_ta.ta_index != current_ta_config->ta_index) ||
             (l1pa_l1ps_com.transfer.aset->packet_ta.ta_tn    != current_ta_config->ta_tn) ||
             (l1a_l1s_com.l1s_en_task[PTCCH] == TASK_DISABLED))
          // The configuration is different than the current one or no PTCCH is currently running
          // (for example in packet idle)
          {
            // Reset PTCCH execution variables.
            l1pa_l1ps_com.transfer.ptcch.activity   = 0;
            l1pa_l1ps_com.transfer.ptcch.request_dl = FALSE;

            // Enable PTCCH task.
            l1a_l1s_com.l1s_en_task[PTCCH] = TASK_ENABLED;
          }
	  }
        }
        else
        // PTCCH is not configured.
        {
          // Disable PTCCH task.
          l1a_l1s_com.l1s_en_task[PTCCH] = TASK_DISABLED;
        }

        // Transfer AGC initialization
        l1pctl_transfer_agc_init();

        switch(l1pa_l1ps_com.transfer.aset->allocated_tbf)
        {
          case SINGLE_BLOCK_UL:
          {
            // Set SINGLE execution variables.
            l1pa_l1ps_com.transfer.single_block.activity = SINGLE_UL; // UL enabled

            // Enable SINGLE task.
            l1a_l1s_com.l1s_en_task[SINGLE] = TASK_ENABLED;
          }
          break;

          case SINGLE_BLOCK_DL:
          {
            // Set SINGLE execution variables.
            l1pa_l1ps_com.transfer.single_block.activity = SINGLE_DL; // DL enabled

            // Enable SINGLE task.
            l1a_l1s_com.l1s_en_task[SINGLE] = TASK_ENABLED;
          }
          break;

          case TWO_PHASE_ACCESS:
          {
            // Set SINGLE execution variables.
            l1pa_l1ps_com.transfer.single_block.activity |= SINGLE_DL; // DL enabled
            l1pa_l1ps_com.transfer.single_block.activity |= SINGLE_UL; // UL enabled

            // Enable SINGLE task.
            l1a_l1s_com.l1s_en_task[SINGLE] = TASK_ENABLED;
          }
          break;

          default:
          {
            if(l1a_l1s_com.l1s_en_task[PDTCH] == TASK_ENABLED)
            //In case of transition idle to transfer the Packet transfer mode is set when synchro is executed
            // Layer 1 internal mode is set to PACKET TRANSFER MODE.
            l1a_l1s_com.mode = PACKET_TRANSFER_MODE;

            // Enable PDTCH task.
            l1a_l1s_com.l1s_en_task[PDTCH] = TASK_ENABLED;

            // Need to disable SINGLE task for two phase access case
            l1a_l1s_com.l1s_en_task[SINGLE] = TASK_DISABLED;
          }
          break;
        } // End switch()

        // SYNCHRO task is not schedule if we are in the specific case:
        // L1A is touching SYNCHRO parameters (tn_difference, dl_tn and dsp_scheduler_mode)
        // and leave L1A to go in HISR (L1S) in middle of the update (cf. BUG1339)
        // Note: tn_difference has to be accumulated in order to cope with the
        // specific case: L1A has just updated tn_difference, dl_tn and dsp_scheduler_mode
        // parameters and we enter in the HISR after the reset of the SYNCHRO Semaphore.
        if(l1a_l1s_com.task_param[SYNCHRO] == SEMAPHORE_RESET)
        {
          // Save the "timeslot difference" between new and old configuration
          // in "tn_difference".
          //   tn_difference -> loaded with the number of timeslot to shift.
          //   dl_tn         -> loaded with the new timeslot.
          l1a_l1s_com.tn_difference += min_synchro_ts - l1a_l1s_com.dl_tn;
          l1a_l1s_com.dl_tn          = min_synchro_ts;

          #if !FF_TBF
          // Enable SYNCHRO task only if lowest allocated timeslot changed
          // or if each time the SINGLE task is enabled
          // In the specific case of the SINGLE task, the GPRS_SCHEDULER
          // has to be selected.
          if((l1a_l1s_com.tn_difference != 0) ||
             (l1a_l1s_com.l1s_en_task[SINGLE] == TASK_ENABLED))
          {
            l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;
            l1a_l1s_com.dsp_scheduler_mode = GPRS_SCHEDULER;
          }
          #else
            // Enable SYNCHRO task if at least one of these conditions fulfilled:
            // -> Change in the timeslot synhronization
            // -> Change of the ongoing TBF mode (synchro_forced)
            // -> Exit of two phase access (synchro_forced)
            // -> Coming from (Packet) Idle (synchro forced)
            // -> SINGLE task enabled
            if((l1a_l1s_com.tn_difference != 0) ||
               (tbf_update_synchro_forced) ||
               (l1a_l1s_com.l1s_en_task[SINGLE] == TASK_ENABLED))
            {
              l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;

               l1a_l1s_com.dsp_scheduler_mode = GPRS_SCHEDULER;
            }
          #endif
        }
        else
        {}// L1A is touching dl_tn, tn_difference and dsp_scheduler_mode parameters...
      }
    }

    // LOOK FOR TIMING AVANCE UPDATE
    //===============================

    if(l1pa_l1ps_com.transfer.ptcch.ta_update_cmd == TRUE)
    {
      #define CURRENT_TA_CONFIG l1pa_l1ps_com.transfer.aset->packet_ta
      #define NEW_TA_CONFIG     l1pa_l1ps_com.transfer.ptcch.packet_ta

      // Only update if the assignment_id of the running TBF matches with the assignment_id
      // given in the MPHP_TIMING_ADVANCE_REQ message
      if (l1pa_l1ps_com.transfer.ptcch.assignment_id == l1pa_l1ps_com.transfer.aset->assignment_id)
      {
        xSignalHeaderRec *msg;

        // Immediate Update of PACKET TA structure.
        //-----------------------------------------

        // Update TA value only if a new value is provided.
        if(NEW_TA_CONFIG.ta != 255)
          CURRENT_TA_CONFIG.ta = NEW_TA_CONFIG.ta;

        if((NEW_TA_CONFIG.ta_index != 255) &&
           (NEW_TA_CONFIG.ta_tn    != 255))
        // There is a New PTCCH configuration.
        {
          // No action when the configuration is the same as the current one.

          if((NEW_TA_CONFIG.ta_index != CURRENT_TA_CONFIG.ta_index) ||
             (NEW_TA_CONFIG.ta_tn    != CURRENT_TA_CONFIG.ta_tn))
          // The configuration is different than the current one.
          {
            // Download the new configuration.
            CURRENT_TA_CONFIG.ta_index = NEW_TA_CONFIG.ta_index;
            CURRENT_TA_CONFIG.ta_tn    = NEW_TA_CONFIG.ta_tn;

            // Reset PTCCH execution variables.
            l1pa_l1ps_com.transfer.ptcch.activity   = 0;
            l1pa_l1ps_com.transfer.ptcch.request_dl = FALSE;

            // Enable PTCCH task.
            l1a_l1s_com.l1s_en_task[PTCCH] = TASK_ENABLED;
          }
        }
        else
        // PTCCH is not configured.
        {
          // Diable PTCCH task.
          l1a_l1s_com.l1s_en_task[PTCCH] = TASK_DISABLED;
        }

        // Send confirmation message to L3.
        msg = os_alloc_sig(sizeof(T_MPHP_TIMING_ADVANCE_CON));
        DEBUGMSG(status,NU_ALLOC_ERR)

        msg->SignalCode = L1P_TA_CONFIG_DONE;
        ((T_MPHP_TIMING_ADVANCE_CON *) msg->SigP)->assignment_id =
          l1pa_l1ps_com.transfer.aset->assignment_id;

        os_send_sig(msg, L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

      } // End if "assignment_id of the running TBF matches"

      // Reset Control code.
      l1pa_l1ps_com.transfer.ptcch.ta_update_cmd = FALSE;

    } // End if(...ta_update_cmd == TRUE)

    // LOOK FOR PSI PARAMETERS UPDATE
    //===============================

    if(l1pa_l1ps_com.transfer.psi_param.psi_param_update_cmd == TRUE)
    {
      // Update parameters
      l1a_l1s_com.Scell_info.pb        = l1pa_l1ps_com.transfer.psi_param.Scell_pb;
      l1pa_l1ps_com.access_burst_type  = l1pa_l1ps_com.transfer.psi_param.access_burst_type;

      // Reset Control code.
      l1pa_l1ps_com.transfer.psi_param.psi_param_update_cmd = FALSE;
    }

    /***********************************************************/
    /* TBF release, PDCH release, Repeat allocation, Fixed     */
    /* allocation exhaustion                                   */
    /***********************************************************/

    // These events are only taken into account on block boundaries
    // in order to keep the "aset" structure unchanged for all the control phases
    // of the last block before modification

    if(block_boundary)
    {
      // LOOK FOR TBF TO BE RELEASED...
      //===============================

      if(l1pa_l1ps_com.transfer.tbf_release_param.tbf_release_cmd == TRUE)
      {
        xSignalHeaderRec *msg;

//TBF_changes
        #if !FF_TBF
        switch(l1pa_l1ps_com.transfer.tbf_release_param.released_tbf)
        #else
          UWORD8 released_tbf;

          // Special case if we got a request to release a two phase access TBF:
          // It is registered within ASET structure as an uplink TBF. If we are
          // currently in pseudo TBF for two phase access, we process the request
          // like an uplink release, otherwise we skip it and just send the
          // L1P_TBF_RELEASED to L1A.

          released_tbf = l1pa_l1ps_com.transfer.tbf_release_param.released_tbf;

          if (released_tbf == TWO_PHASE_ACCESS)
          {
            if (l1pa_l1ps_com.transfer.aset->pseudo_tbf_two_phase_acc)
              released_tbf = UL_TBF;
            else
              released_tbf = NO_TBF;
          }

          switch(released_tbf)
       #endif
        {
          case UL_TBF:
          {
            if(l1pa_l1ps_com.transfer.aset->allocated_tbf == UL_TBF)
            {
              // Disable PDTCH task.
              l1a_l1s_com.l1s_en_task[PDTCH] = TASK_DISABLED;
              // Disable PTCCH task.
              l1a_l1s_com.l1s_en_task[PTCCH] = TASK_DISABLED;

              // Free the active set.
              l1pa_l1ps_com.transfer.aset->allocated_tbf = NO_TBF;
            }
            else
            if(l1pa_l1ps_com.transfer.aset->allocated_tbf == BOTH_TBF)
            {
              // Still DL_TBF running.
              // We must synchro to the 1st timeslot of DL_TBF.

              // REM: the new configuration is not flagged to MACS via "l1ps_macs_com.new_set"
              // since MACS will detect the alloc change.

              // Active set becomes DL TBF.
              l1pa_l1ps_com.transfer.aset->allocated_tbf = DL_TBF;

              // SYNCHRO task is not schedule if we are in the specific case:
              // L1A is touching SYNCHRO parameters (tn_difference, dl_tn and dsp_scheduler_mode)
              // and leave L1A to go in HISR (L1S) in middle of the update (cf. BUG1339)
              // Note: tn_difference has to be accumulated in order to cope with the
              // specific case: L1A has just updated tn_difference, dl_tn and dsp_scheduler_mode
              // parameters and we enter in the HISR after the reset of the SYNCHRO Semaphore.
              if(l1a_l1s_com.task_param[SYNCHRO] == SEMAPHORE_RESET)
              {
                l1a_l1s_com.tn_difference += l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot - l1a_l1s_com.dl_tn;
                l1a_l1s_com.dl_tn          = l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot;

                // Enable SYNCHRO task only when camp timeslot is changed.
                if(l1a_l1s_com.tn_difference != 0)
                {
                  l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;
                }
              }

              // Diable PTCCH if timeslot doesn't match with the remaining DL TBF allocation
              if (!((0x80 >> l1pa_l1ps_com.transfer.aset->packet_ta.ta_tn) &
                    l1pa_l1ps_com.transfer.aset->dl_tbf_alloc.timeslot_alloc))
              {
                // Disable PTCCH task.
                l1a_l1s_com.l1s_en_task[PTCCH] = TASK_DISABLED;

                #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
                  l1_trace_ptcch_disable();
                #endif
              }

            }

            // Reset Repeat allocation starting time checking
            l1pa_l1ps_com.transfer.repeat_alloc.repeat_allocation = FALSE;
            // Reset Allocation Exhaustion detection flag
            l1ps_macs_com.fix_alloc_exhaust_flag                  = FALSE;
            #if L1_EDA
              // Disable FB/SB task detection mechanism for MS class 12
              l1ps_macs_com.fb_sb_task_detect = FALSE;
            #endif
          }
          break;

          case DL_TBF:
          {
            if(l1pa_l1ps_com.transfer.aset->allocated_tbf == DL_TBF)
            {
              // Disable PDTCH task.
              l1a_l1s_com.l1s_en_task[PDTCH] = TASK_DISABLED;
              // Disable PTCCH task.
              l1a_l1s_com.l1s_en_task[PTCCH] = TASK_DISABLED;

              // Free the active set.
              l1pa_l1ps_com.transfer.aset->allocated_tbf = NO_TBF;
            }
            else
            if(l1pa_l1ps_com.transfer.aset->allocated_tbf == BOTH_TBF)
            {
              // Still UL_TBF running.
              // We must synchro to the 1st timeslot of UL_TBF.

              // REM: the new configuration is not flagged to MACS via "l1ps_macs_com.new_set"
              // since MACS will detect the alloc change.

              // Active set becomes UL TBF.
              l1pa_l1ps_com.transfer.aset->allocated_tbf = UL_TBF;

              // SYNCHRO task is not schedule if we are in the specific case:
              // L1A is touching SYNCHRO parameters (tn_difference, dl_tn and dsp_scheduler_mode)
              // and leave L1A to go in HISR (L1S) in middle of the update (cf. BUG1339)
              // Note: tn_difference has to be accumulated in order to cope with the
              // specific case: L1A has just updated tn_difference, dl_tn and dsp_scheduler_mode
              // parameters and we enter in the HISR after the reset of the SYNCHRO Semaphore.
              if(l1a_l1s_com.task_param[SYNCHRO] == SEMAPHORE_RESET)
              {
                l1a_l1s_com.tn_difference += l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot - l1a_l1s_com.dl_tn;
                l1a_l1s_com.dl_tn          = l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot;

                // Enable SYNCHRO task only when camp timeslot is changed.
                if(l1a_l1s_com.tn_difference != 0)
                {
                  l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;
                }
              }

              // Diable PTCCH if timeslot doesn't match with the remaining UL TBF allocation
              if (!((0x80 >> l1pa_l1ps_com.transfer.aset->packet_ta.ta_tn) &
                    l1pa_l1ps_com.transfer.aset->ul_tbf_alloc->timeslot_alloc))
              {
                // Disable PTCCH task.
                l1a_l1s_com.l1s_en_task[PTCCH] = TASK_DISABLED;

                #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
                  l1_trace_ptcch_disable();
                #endif
              }

            }
          }
          break;

          case BOTH_TBF:
          {
            // No more TBF...
            // Disable PDTCH task.
            l1a_l1s_com.l1s_en_task[PDTCH] = TASK_DISABLED;
            // Disable PTCCH task.
            l1a_l1s_com.l1s_en_task[PTCCH] = TASK_DISABLED;

            // Free the active set.
            l1pa_l1ps_com.transfer.aset->allocated_tbf = NO_TBF;

            // Reset Repeat allocation starting time checking
            l1pa_l1ps_com.transfer.repeat_alloc.repeat_allocation = FALSE;
            // Reset Allocation Exhaustion detection flag
            l1ps_macs_com.fix_alloc_exhaust_flag                  = FALSE;
            #if L1_EDA
              // Disable FB/SB task detection mechanism for MS class 12
              l1ps_macs_com.fb_sb_task_detect = FALSE;
            #endif
          }
          break;

        }

        // Send confirmation msg to L3/MACA.
        msg = os_alloc_sig(sizeof(T_L1P_TBF_RELEASED));
        DEBUGMSG(status,NU_ALLOC_ERR)

        msg->SignalCode = L1P_TBF_RELEASED;

        // initialize the TBF type for the confirmation msg
        ((T_L1P_TBF_RELEASED *) msg->SigP)->tbf_type = l1pa_l1ps_com.transfer.tbf_release_param.released_tbf;

        if (l1pa_l1ps_com.transfer.aset->allocated_tbf == NO_TBF)
        {
          l1ps.read_param.assignment_id = 0x01; /* default non initialised value for next tbf */

          ((T_L1P_TBF_RELEASED *) msg->SigP)->released_all = TRUE;

          #if (DSP == 33) || (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37)
            // Correction of BUG1041: reset of multislot bit in d_bbctrl_gprs
            // when leaving patcket transfer.
            l1ps_dsp_com.pdsp_ndb_ptr->d_bbctrl_gprs = l1_config.params.bbctrl;
          #endif
        }
        else
        {
          ((T_L1P_TBF_RELEASED *) msg->SigP)->released_all = FALSE;
        }

        // Insert "tn_difference" information in L1P_TBF_RELEASED msg
        // will be used in Ncell Dedic6 state machine to request a reset
        // or not of the state machine.
        ((T_L1P_TBF_RELEASED *) (msg->SigP))->tn_difference = l1a_l1s_com.tn_difference;

        os_send_sig(msg, L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Flag the new configuration in order to update the Read set parameters
        // with the "aset" structure modifications in the first PDTCH Read phase
        // after configuration change
        l1ps.read_param.new_set = TRUE;

        // Reset Control code.
        l1pa_l1ps_com.transfer.tbf_release_param.tbf_release_cmd = FALSE;
      }

      // LOOK FOR PDCH TO BE RELEASED...
      //================================

      if(l1pa_l1ps_com.transfer.pdch_release_param.pdch_release_cmd == TRUE)
      {
        xSignalHeaderRec *msg;
        UWORD8           timeslot,timeslot_alloc;

        // PDCH Release only apply if the assignement_id of the running TBF matches
        // with the assignment_id included in the MPHP_PDCH_RELEASE_REQ message
        if (l1pa_l1ps_com.transfer.pdch_release_param.assignment_id == l1pa_l1ps_com.transfer.aset->assignment_id)
        {

          // Update timeslot allocation bitmaps
          l1pa_l1ps_com.transfer.aset->ul_tbf_alloc->timeslot_alloc &= l1pa_l1ps_com.transfer.pdch_release_param.timeslot_available;
          l1pa_l1ps_com.transfer.aset->dl_tbf_alloc.timeslot_alloc  &= l1pa_l1ps_com.transfer.pdch_release_param.timeslot_available;

          // Process the downlink TBF first allocated timeslot
          timeslot_alloc = l1pa_l1ps_com.transfer.aset->dl_tbf_alloc.timeslot_alloc;
          timeslot       = 0;

          while((timeslot<7) && !(timeslot_alloc & (0x80>>timeslot)))
          {
            timeslot++;
          }

          l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot   = timeslot;

          // Process the uplink TBF first allocated timeslot
          timeslot_alloc = l1pa_l1ps_com.transfer.aset->ul_tbf_alloc->timeslot_alloc;
          timeslot = 0;
        #if L1_EDA
          // Dynamic allocation mode or Extended Dynamic allocation mode
          if((l1pa_l1ps_com.transfer.aset->mac_mode == DYN_ALLOC) || (l1pa_l1ps_com.transfer.aset->mac_mode == EXT_DYN_ALLOC))
        #else
          // Dynamic allocation mode
          if(l1pa_l1ps_com.transfer.aset->mac_mode == DYN_ALLOC)
        #endif
          {
            while((timeslot<7) && !(timeslot_alloc & (0x80>>timeslot)))
            {
              timeslot++;
            }
          }
          else
          // Fixed allocation mode
          if(l1pa_l1ps_com.transfer.aset->mac_mode == FIX_ALLOC_NO_HALF)
          {
            // If the control timeslot hasn't been released
            if (l1pa_l1ps_com.transfer.aset->ul_tbf_alloc->timeslot_alloc &
                 (0x80 >> l1pa_l1ps_com.transfer.aset->ul_tbf_alloc->fixed_alloc.ctrl_timeslot))
            {
              // The first allocated timeslot is the control timeslot
              timeslot = l1pa_l1ps_com.transfer.aset->ul_tbf_alloc->fixed_alloc.ctrl_timeslot;
            }
            else
            {
              // The first allocated timeslot is found in the allocation bitmap
              while((timeslot<7) && !(timeslot_alloc & (0x80>>timeslot)))
              {
                timeslot++;
              }
            }
          }

          l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot   = timeslot;

          // Fill "synchro_timeslot" which will be the frame synchro slot.
          switch(l1pa_l1ps_com.transfer.aset->allocated_tbf)
          {
            case(DL_TBF):
            {
              l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot = l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot;
            }
            break;

            case(UL_TBF):
            {
              l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot = l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot;
            }
            break;

            case(BOTH_TBF):
            {
              if (l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot > l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot)
              {
                l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot = l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot;
              }
              else
              {
                l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot = l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot;
              }
            }
            break;
          }

          // SYNCHRO task is not schedule if we are in the specific case:
          // L1A is touching SYNCHRO parameters (tn_difference, dl_tn and dsp_scheduler_mode)
          // and leave L1A to go in HISR (L1S) in middle of the update (cf. BUG1339)
          // Note: tn_difference has to be accumulated in order to cope with the
          // specific case: L1A has just updated tn_difference, dl_tn and dsp_scheduler_mode
          // parameters and we enter in the HISR after the reset of the SYNCHRO Semaphore.
          if(l1a_l1s_com.task_param[SYNCHRO] == SEMAPHORE_RESET)
          {
            // New synchronization
            l1a_l1s_com.tn_difference += l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot - l1a_l1s_com.dl_tn;
            l1a_l1s_com.dl_tn          = l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot;

            // REM: the new configuration is not flagged to MACS via "l1ps_macs_com.new_set"
            // since MACS will detect the alloc change.

            // Enable SYNCHRO task only when camp timeslot is changed.
            if(l1a_l1s_com.tn_difference != 0)
            {
              l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;
            }
          }

          // Disable PTCCH if timeslot doesn't match with the remaining PDCH allocation
          if (!((0x80 >> l1pa_l1ps_com.transfer.aset->packet_ta.ta_tn) &
                l1pa_l1ps_com.transfer.pdch_release_param.timeslot_available))
          {
            // Disable PTCCH task.
            l1a_l1s_com.l1s_en_task[PTCCH] = TASK_DISABLED;

            #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
              l1_trace_ptcch_disable();
            #endif
          }

          // Send confirmation msg to L3/MACA.
          msg = os_alloc_sig(sizeof(T_L1P_PDCH_RELEASE_CON));
          DEBUGMSG(status,NU_ALLOC_ERR)

          msg->SignalCode = L1P_PDCH_RELEASED;

          ((T_L1P_PDCH_RELEASE_CON *) msg->SigP)->assignment_id = l1pa_l1ps_com.transfer.pdch_release_param.assignment_id;

          // Insert "tn_difference" information in T_L1P_PDCH_RELEASE_CON msg
          // will be used in Ncell Dedic6 state machine to request a reset
          // or not of the state machine
          ((T_L1P_PDCH_RELEASE_CON *) (msg->SigP))->tn_difference = l1a_l1s_com.tn_difference;

          os_send_sig(msg, L1C1_QUEUE);
          DEBUGMSG(status,NU_SEND_QUEUE_ERR)

          // Flag the new configuration in order to update the Read set parameters
          // with the "aset" structure modifications in the first PDTCH Read phase
          // after configuration change
          l1ps.read_param.new_set = TRUE;

        } // End if "assignment_id matches with the running TBF"

        // Reset Control code.
        l1pa_l1ps_com.transfer.pdch_release_param.pdch_release_cmd = FALSE;
      }

      // LOOK FOR REPEAT ALLOCATION ...
      //================================

      if(l1pa_l1ps_com.transfer.repeat_alloc.repeat_allocation)
      {
        UWORD8           timeslot,timeslot_alloc;

        // Starting time checking...
        //--------------------------

        if(l1pa_l1ps_com.transfer.repeat_alloc.tbf_sti.present)
        // Starting time present.
        // Rem: starting time detected 1 frame in advance, this frame is used by
        //      SYNCHRO task.
        {
          WORD32  time_diff;

          // If synchro change occurs, it's always from a timeslot N to N + 1
          time_diff = ( (l1pa_l1ps_com.transfer.repeat_alloc.tbf_sti.absolute_fn - 1)
                         - (l1s.next_time.fn % 42432) + 2*42432) % 42432;

          // Starting time has been passed...
          if(((time_diff >= (32024)) && (time_diff <= (42431))) || (time_diff == 0))
          {
            l1pa_l1ps_com.transfer.repeat_alloc.tbf_sti.present = FALSE;
          }
        } // End if "starting time present"

        // Starting time passed...
        //------------------------

        // If the repeat allocation starts on this frame...
        if (!l1pa_l1ps_com.transfer.repeat_alloc.tbf_sti.present)
        {
          #if (TRACE_TYPE!=0)
            // Trace "starting time" on log file and screen.
            trace_fct(CST_STI_PASSED, l1a_l1s_com.Scell_info.radio_freq);
          #endif

          // Update ts_override and starting time
          l1pa_l1ps_com.transfer.aset->ts_override         = l1pa_l1ps_com.transfer.repeat_alloc.ts_override;
          l1pa_l1ps_com.transfer.aset->tbf_sti.absolute_fn = l1pa_l1ps_com.transfer.repeat_alloc.tbf_sti.absolute_fn;

          // Lowest allocated timeslot for the UL TBF

          // If the downlink control timeslot hasn't been released: it's the downlink control timeslot
          // Else no change
          if (  l1pa_l1ps_com.transfer.aset->ul_tbf_alloc->timeslot_alloc
                & (0x80 >> l1pa_l1ps_com.transfer.aset->ul_tbf_alloc->fixed_alloc.ctrl_timeslot))
          {
            // Synchronization on the downlink control timeslot
            l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot = l1pa_l1ps_com.transfer.aset->ul_tbf_alloc->fixed_alloc.ctrl_timeslot;
          }

          // Synchronization

          // If a downlink TBF is enabled
          if ( l1pa_l1ps_com.transfer.aset->allocated_tbf == BOTH_TBF)
          {
            // Synchronization on the downlink TBF lowest allocated timeslot ?
            if (l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot > l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot)
              l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot = l1pa_l1ps_com.transfer.aset->dl_tbf_synchro_timeslot;
            else
              l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot = l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot;
          }
          // Else: synchronization on the uplink TBF lowest allocated timeslot
          else
            l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot = l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot;

          // SYNCHRO task is not schedule if we are in the specific case:
          // L1A is touching SYNCHRO parameters (tn_difference, dl_tn and dsp_scheduler_mode)
          // and leave L1A to go in HISR (L1S) in middle of the update (cf. BUG1339)
          // Note: tn_difference has to be accumulated in order to cope with the
          // specific case: L1A has just updated tn_difference, dl_tn and dsp_scheduler_mode
          // parameters and we enter in the HISR after the reset of the SYNCHRO Semaphore.
          if(l1a_l1s_com.task_param[SYNCHRO] == SEMAPHORE_RESET)
          {
            l1a_l1s_com.tn_difference += l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot - l1a_l1s_com.dl_tn;
            l1a_l1s_com.dl_tn          = l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot;

            // Enable SYNCHRO task only when camp timeslot is changed.
            if(l1a_l1s_com.tn_difference != 0)
            {
              l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;
            }
          }

          // Set assignment_command to the last enabled TBF type.
          // This permits MAC-S to correctly manage TBF boundary conditions.
          if ((current_assignment_command == NO_TBF) || (current_assignment_command == UL_TBF))
            l1pa_l1ps_com.transfer.aset->assignment_command = UL_TBF;
          else
            l1pa_l1ps_com.transfer.aset->assignment_command = BOTH_TBF;

          // Flag the new configuration to MACS.
          l1ps_macs_com.new_set = TRUE;

          // Flag the new configuration in order to update the Read set parameters
          // with the "aset" structure modifications in the first PDTCH Read phase
          // after configuration change
          l1ps.read_param.new_set = TRUE;

          // Reset Repeat allocation starting time checking
          l1pa_l1ps_com.transfer.repeat_alloc.repeat_allocation = FALSE;
          // Reset Allocation Exhaustion detection flag
          l1ps_macs_com.fix_alloc_exhaust_flag                  = FALSE;

          // Send confirmation
          {
            xSignalHeaderRec *msg;

            // Send confirmation msg to L3/MACA.
            msg = os_alloc_sig(sizeof(T_L1P_REPEAT_ALLOC_DONE));
            DEBUGMSG(status,NU_ALLOC_ERR)

            msg->SignalCode = L1P_REPEAT_ALLOC_DONE;

            // Insert "tn_difference" information in T_L1P_REPEAT_ALLOC_DONE msg
            // will be used in Ncell Dedic6 state machine to request a reset
            // or not of the state machine
            ((T_L1P_REPEAT_ALLOC_DONE *) (msg->SigP))->tn_difference = l1a_l1s_com.tn_difference;

            os_send_sig(msg, L1C1_QUEUE);
            DEBUGMSG(status,NU_SEND_QUEUE_ERR)
          }

        } // End if "Repeat allocation starts this frame..."
      } // End of "Repeat allocation starting time checking"

      // LOOK FOR FIXED MODE ALLOCATION BITMAP EXHAUSTION ...
      //==================================================

      if(l1ps_macs_com.fix_alloc_exhaust_flag)
      {
        #if (TRACE_TYPE!=0)
          // Trace "starting time" on log file and screen.
          trace_fct(CST_ALLOC_EXHAUSTION, l1a_l1s_com.Scell_info.radio_freq);
        #endif

        // Update uplink TBF synchronization timeslot
        {
          UWORD8 timeslot = 0;
          UWORD8 bitmap   = 0x80;

          while (!(l1pa_l1ps_com.transfer.aset->ul_tbf_alloc->timeslot_alloc & bitmap))
          {
            timeslot ++;
            bitmap >>= 1;
          }

          // Synchronization on the lowest allocated timeslot for uplink tranfer
          l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot = timeslot;
        }

        // New synchronization only done on a timeslot inferior to current synchronization
        // - if a DL TBF is present: the DL TBF synchronization is taken into account
        // - an UL TBF is present: synchronization can only be done on a timeslot number inferior or
        //   equal to the current synchronization
        if (l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot <
            l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot)
        {
          l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot = l1pa_l1ps_com.transfer.aset->ul_tbf_synchro_timeslot;
        }

        // SYNCHRO task is not schedule if we are in the specific case:
        // L1A is touching SYNCHRO parameters (tn_difference, dl_tn and dsp_scheduler_mode)
        // and leave L1A to go in HISR (L1S) in middle of the update (cf. BUG1339)
        // Note: tn_difference has to be accumulated in order to cope with the
        // specific case: L1A has just updated tn_difference, dl_tn and dsp_scheduler_mode
        // parameters and we enter in the HISR after the reset of the SYNCHRO Semaphore.
        if(l1a_l1s_com.task_param[SYNCHRO] == SEMAPHORE_RESET)
        {
          l1a_l1s_com.tn_difference += l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot - l1a_l1s_com.dl_tn;
          l1a_l1s_com.dl_tn          = l1pa_l1ps_com.transfer.aset->transfer_synchro_timeslot;

          // Enable SYNCHRO task only when camp timeslot is changed.
          if(l1a_l1s_com.tn_difference != 0)
          {
            l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;
          }
        }

        // Send signal to L1A
        {
          xSignalHeaderRec *msg;

          // Send confirmation msg to L3/MACA.
          msg = os_alloc_sig(sizeof(T_L1P_ALLOC_EXHAUST_DONE));
          DEBUGMSG(status,NU_ALLOC_ERR)

          msg->SignalCode = L1P_ALLOC_EXHAUST_DONE;

          // Insert "tn_difference" information in T_L1P_ALLOC_EXHAUST_DONE msg
          // will be used in Ncell Dedic6 state machine to request a reset
          // or not of the state machine
          ((T_L1P_ALLOC_EXHAUST_DONE *) (msg->SigP))->tn_difference = l1a_l1s_com.tn_difference;

          os_send_sig(msg, L1C1_QUEUE);
          DEBUGMSG(status,NU_SEND_QUEUE_ERR)
        }

        // Flag the new configuration in order to update the Read set parameters
        // with the "aset" structure modifications in the first PDTCH Read phase
        // after configuration change
        l1ps.read_param.new_set = TRUE;

        // Reset flag
        l1ps_macs_com.fix_alloc_exhaust_flag = FALSE;

      } // End if "fixed mode allocation bitmap has exhausted"
    } // End of "block_boundary"
  } // end of if(!l1pa_l1ps_com.transfer.semaphore)
}
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END
#endif // MOVE_IN_INTERNAL_RAM

#if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))  // MOVE TO INTERNAL MEM IN CASE GSM_IDLE_RAM enabled
//#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START         // KEEP IN EXTERNAL MEM otherwise

/*-------------------------------------------------------*/
/* l1ps_meas_manager()                                   */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* This function is the tasks manager for Packet Cell    */
/* Reselection.                                          */
/* The followings tasks are handled:                     */
/*                                                       */
/* FSMS_MEAS:                                            */
/*                                                       */
/* P_CRMS_MEAS: Packet Cell Reselection measurement      */
/* in Idle mode. The task includes:                      */
/*    o BA(GPRS) measurement                             */
/*    o Network controlled meas.                         */
/*    o Extended measurement                             */
/* Occurrences are performed in order to satisfy the     */
/* following ETSI constraints:                           */
/* 1. At least one measure of each BA BCCH carrier shall */
/*    be taken for each paging block,                    */
/* 2. A minimum of one measure for each BA BCCH carrier  */
/*    for every 4 second must be performed,              */
/* 3. MS is not required to take more than one sample    */
/*    per second for each BCCH carrier,                  */
/* 4. At least 5 measures per BA BCCH carrier are        */
/*    required for a valid received level average value  */
/*    (RLA_P),                                           */
/* 5. RLA_P shall be based on samples collected over a   */
/*    period of 5s to Max{5s, five consecutive PPCH      */
/*    blocks dedicated to the MS},                       */
/* 6. Samples allocated to each carrier shall as far     */
/*    as possible be uniformly distributed over the      */
/*    evaluation period.                                 */
/*                                                       */
/* A TI condition is include:                            */
/* 7. In order to save power consumption, it will be     */
/*    necessary to use as far as possible the PPCH       */
/*    blocks to perform a maximum of measurements during */
/*    these frames.                                      */
/*                                                       */
/* From the previous constraints, it appears that        */
/* Paging block period needs to be considered to fit     */
/* ETSI consideration.                                   */
/* Tow case are considered:                              */
/*     o PPCH period >= 1s                               */
/*     o PPCH period < 1s                                */
/*                                                       */
/* Once all carriers of the frequency list have been     */
/* measured, a reporting message L1P_CR_MEAS_DONE is     */
/* built and sent to L1A.                                */
/*-------------------------------------------------------*/
void l1ps_meas_manager()
{
  enum states
  {
    NULL_MEAS  = 0,
    PCHTOTAL   = 1,
    TOTAL      = 2,
    MEAS       = 3,
    MEASTOTAL  = 4
  };

  UWORD8  IL_for_rxlev;
  BOOL    init_meas   = FALSE;
#if (RF_FAM == 61)
  UWORD16 dco_algo_ctl_pw = 0;
  UWORD16 dco_algo_ctl_pw_temp = 0;
  UWORD8 if_ctl = 0;
  UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GSM;
#endif
  #define  cr_list_size  l1pa_l1ps_com.cres_freq_list.alist->nb_carrier

  static WORD8            remain_carrier;
  static UWORD8           nbr_meas;
  static BOOL             schedule_trigger;
  static WORD16           time_to_wake_up;
  static WORD16           time_to_4s;
  static WORD16           time_to_1s;
  static UWORD32          fn_update;
  static UWORD16          session_done; // Did a session of measures performed in the 4s period
  static UWORD16          session_done_1s; // Did a session of measures performed in the 1s period
  static UWORD8           state;
  static UWORD32          reporting_period; // Parameter used in "reporting_period" computation

  static xSignalHeaderRec *cr_msg = NULL;

#if (FF_L1_FAST_DECODING == 1)
    if (l1a_apihisr_com.fast_decoding.deferred_control_req == TRUE)
    {
      /* Do not execute l1s_meas_manager if a fast decoding IT is scheduled */
      return;
    }
#endif /*#if (FF_L1_FAST_DECODING == 1)*/

  #if FF_L1_IT_DSP_USF
    if (l1ps_macs_com.usf_status != USF_IT_DSP)
    {
  #endif


  //====================================================
  //  RESET MEASUREMENT MACHINES WHEN ABORT EXECUTED.
  //====================================================
  if(l1s.dsp_ctrl_reg & CTRL_ABORT)
  // ABORT task has been controlled, which reset the MCU/DSP communication.
  // We must rewind any measurement activity.
  {
    // Aborted measurements have to be rescheduled
    nbr_meas += l1pa_l1ps_com.cr_freq_list.ms_ctrl_d + l1pa_l1ps_com.cr_freq_list.ms_ctrl_dd;

    // Rewind "next_to_ctrl" counter to come back to the next carrier to
    // measure.
    l1pa_l1ps_com.cr_freq_list.next_to_ctrl = l1pa_l1ps_com.cr_freq_list.next_to_read;

    // Reset flags.
    l1pa_l1ps_com.cr_freq_list.ms_ctrl    = 0;
    l1pa_l1ps_com.cr_freq_list.ms_ctrl_d  = 0;
    l1pa_l1ps_com.cr_freq_list.ms_ctrl_dd = 0;
  }

#if (GSM_IDLE_RAM != 1)
  // Test if task is disabled and cr_msg != NULL, then de-allocate memory
  if (!(l1pa_l1ps_com.l1ps_en_meas & P_CRMS_MEAS) && !(l1pa_l1ps_com.meas_param & P_CRMS_MEAS))
  {
    if(cr_msg != NULL)
    {
      // Cell reselection measurement process has been stopped by L3
      // Deallocate memory for the received message if msg not forwarded to L3.
      // ----------------------------------------------------------------------
      os_free_sig(cr_msg);
      DEBUGMSG(status,NU_DEALLOC_ERR)

      cr_msg = NULL;
    }
  }
#endif

  if((l1pa_l1ps_com.l1ps_en_meas & P_CRMS_MEAS) && (l1pa_l1ps_com.meas_param & P_CRMS_MEAS))
  // Some changes occured on the Frequency list or the PAGING PARAMETERS have
  // changed.
  {
    // Reset Packet Cell Reselection semaphore.
    l1pa_l1ps_com.meas_param &= P_CRMS_MEAS_MASK;

    // Paging process has been interrupted by a L3 message
    // Deallocate memory for the received message if msg not forwarded to L3.
    // ----------------------------------------------------------------------

    //Update Frequency list pointer
    l1pa_l1ps_com.cres_freq_list.alist = l1pa_l1ps_com.cres_freq_list.flist;

    // Rewind frequency list counters to come back to the first carrier of this
    // aborted session.
    l1pa_l1ps_com.cr_freq_list.next_to_read = 0;
    l1pa_l1ps_com.cr_freq_list.next_to_ctrl = 0;

    // Reset flags.
    l1pa_l1ps_com.cr_freq_list.ms_ctrl      = 0;
    l1pa_l1ps_com.cr_freq_list.ms_ctrl_d    = 0;
    l1pa_l1ps_com.cr_freq_list.ms_ctrl_dd   = 0;

    // Initialize timer
    time_to_wake_up  = 0;
    time_to_4s       = 866;
    time_to_1s       = 216;
    fn_update        = l1s.actual_time.fn;

    // Initialize Reporting Period
    reporting_period = l1s.actual_time.fn;

    // Initialize Cell reselection state machine and parameters
    state            = NULL_MEAS;
    session_done     = 0;
    session_done_1s  = 0;
    schedule_trigger = TRUE;
    nbr_meas         = 0;
    init_meas        = TRUE;
  }
  #if FF_L1_IT_DSP_USF
    } // if (l1ps_macs_com.usf_status != USF_IT_DSP)
  #endif

  // Packet Idle Cell Relselection Power Measurements fonction if P_CRMS_MEAS task still enabled.
  // In case L1S has switched in GPRS packet transfer mode and L1S_TRANSFER_DONE message hasn't been processed yet
  // by L1A, no control or read task shall be done.

  if ((l1pa_l1ps_com.l1ps_en_meas & P_CRMS_MEAS) && !(l1pa_l1ps_com.meas_param & P_CRMS_MEAS)
      && (l1a_l1s_com.l1s_en_task[PDTCH]  != TASK_ENABLED))
  {
    #if FF_L1_IT_DSP_USF
      if (l1ps_macs_com.usf_status != USF_IT_DSP)
      {
    #endif
    // Update P_CRMS_MEAS timers
    {
      UWORD16 fn_diff;
      #define current_fn l1s.actual_time.fn

      // Compute Frame Number increment since last L1S activity
      if(current_fn >= fn_update)
        fn_diff = current_fn - fn_update;
      else
        fn_diff = (current_fn + MAX_FN) - fn_update;

      // Update timer
      time_to_4s -= fn_diff;

      if(time_to_4s <= 0)
      {
        time_to_4s  += 866;
        session_done = 0;
      }

      time_to_1s -= fn_diff;

      if(time_to_1s <= 0)
      {
        time_to_1s      += 216;
        session_done_1s  = 0;
      }

      // Note: time to next meas position is negative during the meas.
      // session period
      time_to_wake_up -= fn_diff;

      fn_update = current_fn;
    }

    if(time_to_wake_up == 0)
    {
      // Schedule CR meas position for "PNP period >= 1s" case
      if(l1pa_l1ps_com.pccch.pnp_period >= 216)
      {
        switch (state)
        {
          case PCHTOTAL:
          case TOTAL:
          {
            nbr_meas = cr_list_size;

            // Measures is going to start, set "session_done" flag.
            // "session_done" flag must be set at the begining of a meas. session
            // in order to be able to detect crossing of the 4s boundary
            session_done = 1;
          }
          break;

          case NULL_MEAS:
          {
            nbr_meas = 0;
          }
          break;
        }
      }
      // Schedule CR meas position for "PNP period < 1s" case
      else
      {
        switch (state)
        {
          case NULL_MEAS:
          {
            nbr_meas = 0;
          }
          break;

          case MEAS:
          {
            UWORD8   max_nbmeas;
            WORD16   tpu_win_rest;
            UWORD16  power_meas_split;

            // Compute how many BP_SPLIT remains for cr list meas
            // Rem: we take into account the SYNTH load for 1st RX in next frame.
            // WARNING: only one RX activity is considered in next paging block !!!
            tpu_win_rest = FRAME_SPLIT - (RX_LOAD + l1_config.params.rx_synth_load_split);

            power_meas_split = (l1_config.params.rx_synth_load_split + PWR_LOAD);
            max_nbmeas = 0;

            while(tpu_win_rest >= power_meas_split)
            {
              max_nbmeas ++;
              tpu_win_rest -= power_meas_split;
            }

            if (max_nbmeas > NB_MEAS_MAX_GPRS) max_nbmeas = NB_MEAS_MAX_GPRS;

            // There is no more PPCH block before end of 1s period.
            // End remaining carriers.
            nbr_meas = Min(remain_carrier, 4*max_nbmeas);

            // Measures is going to start, set "session_done" flag.
            // "session_done_1s" flag must be set at the begining of a meas. session
            // in order to be able to detect crossing of the 1s boundary
            session_done_1s = 1;

          }
          break;

          case MEASTOTAL:
          {
            nbr_meas = remain_carrier;

            // Measures is going to start, set "session_done" flag.
            // "session_done_1s" flag must be set at the begining of a meas. session
            // in order to be able to detect crossing of the 1s boundary
            session_done_1s = 1;

          }
          break;
        }
      } // End of else ("PNP period < 1s" case)
    }

    /* --------------------------------------------------------------------*/
    /* CTRL and READ phase carrying out. "nbr_meas" measures are performed */
    /* --------------------------------------------------------------------*/

    // ********************
    // READ task if needed
    // ********************
    if(l1pa_l1ps_com.cr_freq_list.ms_ctrl_dd)
    // Background measurements....
    // A measure CTRL was performed 2 tdma earlier, read result now.
    {
      UWORD16  radio_freq_read;
      UWORD16  pm_read[NB_MEAS_MAX_GPRS];
      UWORD8   i;

      #define  next_to_read  l1pa_l1ps_com.cr_freq_list.next_to_read

      // When a READ is performed we set dsp_r_page_used flag to
      // switch the read page.
      l1s_dsp_com.dsp_r_page_used = TRUE;

      l1_check_com_mismatch(CR_MEAS_ID);

      // Read power measurement result from DSP/MCU GPRS interface
      l1pddsp_meas_read(l1pa_l1ps_com.cr_freq_list.ms_ctrl_dd, pm_read);

      for(i=0; i<l1pa_l1ps_com.cr_freq_list.ms_ctrl_dd; i++)
      {
        radio_freq_read = l1pa_l1ps_com.cres_freq_list.alist->freq_list[next_to_read];

        // Traces and debug.
        // ******************

        #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
          trace_fct(CST_READ_CR_MEAS, radio_freq_read);
        #endif

        l1_check_pm_error(pm_read[i], CR_MEAS_ID);
	pm_read[i] = (pm_read[i] >> 5);

        // Get Input level corresponding to the used IL and pm result.
        IL_for_rxlev = l1pctl_pgc(((UWORD8)(pm_read[i])),
                                  l1pa_l1ps_com.cr_freq_list.used_il_lna_dd[i].il,
                                  l1pa_l1ps_com.cr_freq_list.used_il_lna_dd[i].lna,
                                  radio_freq_read);

        #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          RTTL1_FILL_MON_MEAS(pm_read[i], IL_for_rxlev, CR_MEAS_ID, radio_freq_read)
        #endif

        // Check that cr_msg hasn't been erased.
        if (cr_msg == NULL)
        {
          cr_msg = os_alloc_sig(sizeof(T_L1P_CR_MEAS_DONE));
          DEBUGMSG(status,NU_ALLOC_ERR)
          cr_msg->SignalCode = L1P_CR_MEAS_DONE;
        }

        #if (GSM_IDLE_RAM != 1)
          // Fill reporting message.
          ((T_L1P_CR_MEAS_DONE*)(cr_msg->SigP))->
            ncell_meas[next_to_read].rxlev = l1s_encode_rxlev(IL_for_rxlev);
        #else
          // Fill reporting message.
          l1ps.ncell_meas_rxlev[next_to_read] = (WORD8) l1s_encode_rxlev(IL_for_rxlev);
        #endif
        // Increment "next_to_read" field for next measurement...
        if(++next_to_read >= cr_list_size)
          next_to_read = 0;
      }//end for

      // **********
      // Reporting
      // **********
      if(next_to_read == 0)
      {
        #if (GSM_IDLE_RAM == 1)
          // Check if memory for L1P_CR_MEAS_DONE msg is allocated
          if (cr_msg == NULL)
          {
            if (!READ_TRAFFIC_CONT_STATE)
            CSMI_TrafficControllerOn();
            cr_msg = os_alloc_sig(sizeof(T_L1P_CR_MEAS_DONE));
            DEBUGMSG(status,NU_ALLOC_ERR)
            cr_msg->SignalCode = L1P_CR_MEAS_DONE;
          }

          for(i=0; i<cr_list_size; i++)
          {
            ((T_L1P_CR_MEAS_DONE*)(cr_msg->SigP))->ncell_meas[i].rxlev = l1ps.ncell_meas_rxlev[i];
            // Fill reporting message.
          }
        #endif

        // Fill BA identifier field.
        ((T_L1P_CR_MEAS_DONE*)(cr_msg->SigP))->list_id = l1pa_l1ps_com.cres_freq_list.alist->list_id;

        ((T_L1P_CR_MEAS_DONE*)(cr_msg->SigP))->nmeas = cr_list_size;

        // Compute and Fill reporting period value
        if(l1s.actual_time.fn > reporting_period)
          reporting_period = l1s.actual_time.fn - reporting_period;
        else
          reporting_period = l1s.actual_time.fn - reporting_period + MAX_FN;

        ((T_L1P_CR_MEAS_DONE*)(cr_msg->SigP))->reporting_period = (UWORD16)reporting_period;

        reporting_period = l1s.actual_time.fn;

        // send L1P_CR_MEAS_DONE message...
        os_send_sig(cr_msg, L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Reset pointer for debugg.
        cr_msg = NULL;
      }

      // Trigger measurement scheduler when meas. session is completed.
      // Note: A meas. session includes all carriers of the list "PNP period >= 1s case"
      //       or only a sub-set of the CR freq list "PNP period < 1s case"
      if(!(l1pa_l1ps_com.cr_freq_list.ms_ctrl) && !(l1pa_l1ps_com.cr_freq_list.ms_ctrl_d))
        schedule_trigger = TRUE;
    }// end of READ

    #if FF_L1_IT_DSP_USF
      } // if (l1ps_macs_com.usf_status != USF_IT_DSP)
    #endif

    // **********
    // CTRL task
    // **********

    #if FF_L1_IT_DSP_USF
      if (l1ps_macs_com.usf_status != USF_AWAITED)
      {
    #endif

    if (nbr_meas > 0)
    {
      if(l1s.forbid_meas < 2)
      {
        UWORD8   max_nbmeas;
        UWORD8   nb_meas_to_perform;
        UWORD16  radio_freq_ctrl;
        UWORD8   i;
        UWORD8   pw_position = 0; // indicates first time slot in frame available for PM
        WORD16   tpu_win_rest;
        UWORD16  power_meas_split;

        #define next_to_ctrl  l1pa_l1ps_com.cr_freq_list.next_to_ctrl

        // Compute how many BP_SPLIT remains for cr list meas
        // Rem: we take into account the SYNTH load for 1st RX in next frame.
        tpu_win_rest = FRAME_SPLIT - l1s.tpu_win;
        power_meas_split = (l1_config.params.rx_synth_load_split + PWR_LOAD);
        max_nbmeas = 0;

        while(tpu_win_rest >= power_meas_split)
        {
          max_nbmeas ++;
          tpu_win_rest -= power_meas_split;
        }

        // Compute number of PM allowed in the Frame
        // Test if we are on a Paging frame
        if(l1pa_l1ps_com.cr_freq_list.pnp_ctrl > 0) pw_position = 1;

        // Test if PRACH controlled in the same frame
        if(l1s.tpu_win >= ((3 * BP_SPLIT) + l1_config.params.tx_ra_load_split
                                          + l1_config.params.rx_synth_load_split))
        {
          pw_position = 4;
        }

        // Compute Number of measures to perform
        nb_meas_to_perform = cr_list_size - next_to_ctrl;

        if(nb_meas_to_perform > max_nbmeas)
          nb_meas_to_perform = max_nbmeas;

        if(nb_meas_to_perform > NB_MEAS_MAX_GPRS)
          nb_meas_to_perform = NB_MEAS_MAX_GPRS;

        if (nb_meas_to_perform > nbr_meas)
          nb_meas_to_perform = nbr_meas;

        for(i=0; i<nb_meas_to_perform; i++)
        {
          UWORD8  lna_off;
          WORD8   agc;
          UWORD8 input_level;
#if (L1_FF_MULTIBAND == 1)
          UWORD16 operative_radio_freq;
#endif
          
          radio_freq_ctrl = l1pa_l1ps_com.cres_freq_list.alist->freq_list[next_to_ctrl];

#if (L1_FF_MULTIBAND == 0)

          // Get AGC according to the last known IL.
          input_level =  l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].input_level;
          lna_off = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].lna_off;
          agc     = Cust_get_agc_from_IL(radio_freq_ctrl,input_level >> 1, PWR_ID, lna_off);


          // Memorize the IL and LNA used for AGC setting.
          //l1pa_l1ps_com.cr_freq_list.used_il_lna[i]  = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset];
          l1pa_l1ps_com.cr_freq_list.used_il_lna[i].il  = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].input_level;
          l1pa_l1ps_com.cr_freq_list.used_il_lna[i].lna = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].lna_off;

#else // L1_FF_MULTIBAND = 1 below

          operative_radio_freq = 
            l1_multiband_radio_freq_convert_into_operative_radio_freq(radio_freq_ctrl);

          input_level =  
            l1a_l1s_com.last_input_level[operative_radio_freq].input_level;
          lna_off = 
            l1a_l1s_com.last_input_level[operative_radio_freq].lna_off;
          agc     = 
            Cust_get_agc_from_IL(radio_freq_ctrl,input_level >> 1, PWR_ID, lna_off);

          // Memorize the IL and LNA used for AGC setting.
          //l1pa_l1ps_com.cr_freq_list.used_il_lna[i]  = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset];
          l1pa_l1ps_com.cr_freq_list.used_il_lna[i].il  = 
            l1a_l1s_com.last_input_level[operative_radio_freq].input_level;
          l1pa_l1ps_com.cr_freq_list.used_il_lna[i].lna = 
            l1a_l1s_com.last_input_level[operative_radio_freq].lna_off;

#endif // #if (L1_FF_MULTIBAND == 0) else


          #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
            trace_fct(CST_CTRL_CR_MEAS, 1);//OMAPS00090550
          #endif

#if(RF_FAM == 61)   // Locosto DCO
#if (PWMEAS_IF_MODE_FORCE == 0)
          cust_get_if_dco_ctl_algo(&dco_algo_ctl_pw_temp, &if_ctl, (UWORD8) L1_IL_VALID ,
              input_level,
              radio_freq_ctrl, if_threshold);
        #else
          if_ctl = IF_120KHZ_DSP;
          dco_algo_ctl_pw_temp = DCO_IF_0KHZ;
        #endif

      dco_algo_ctl_pw |= ( (dco_algo_ctl_pw_temp & 0x03)<< (i*2));
#endif


          // tpu pgm: 1 measurement only.
          l1dtpu_meas(radio_freq_ctrl,
                      agc,
                      lna_off,
                      l1s.tpu_win,
                      l1s.tpu_offset,INACTIVE
               #if(RF_FAM == 61)
                     ,L1_AFC_SCRIPT_MODE
                     ,if_ctl
               #endif
		  );

          // increment carrier counter for next measurement...
          if(++next_to_ctrl >= cr_list_size)
            next_to_ctrl = 0;

          #if L2_L3_SIMUL
            #if (DEBUG_TRACE == BUFFER_TRACE_OFFSET_NEIGH)
              buffer_trace(4, l1s.actual_time.fn, radio_freq_ctrl,
                           l1s.tpu_win, 0);
            #endif
          #endif

          // Increment tpu window identifier.
          l1s.tpu_win += (l1_config.params.rx_synth_load_split + PWR_LOAD);

        } // End for(...nb_meas_to_perform)

  #if(RF_FAM == 61)
          l1ddsp_load_dco_ctl_algo_pw(dco_algo_ctl_pw);
  #endif


        // Program DSP, in order to performed nb_meas_to_perform measures
        // Second argument specifies if a Rx burst will be received in this frame
        l1pddsp_meas_ctrl(nb_meas_to_perform, pw_position);

        // Update d_debug timer
        l1s_dsp_com.dsp_db_w_ptr->d_debug = (l1s.debug_time + 2) ;


        // Flag measurement control.
        // **************************

        // Set flag "ms_ctrl" to nb_meas_to_perform.
        // It will be used as 2 tdma delayed to trigger Read phase.
        l1pa_l1ps_com.cr_freq_list.ms_ctrl = nb_meas_to_perform;

        // Flag DSP and TPU programmation.
        // ********************************

        // Set "CTRL_MS" flag in the controle flag register.
        l1s.tpu_ctrl_reg |= CTRL_MS;
        l1s.dsp_ctrl_reg |= CTRL_MS;

        // Update nbr_meas
        nbr_meas -= nb_meas_to_perform;

        // Update remainig measurements to performed according meas done
        //remain_carrier -= nb_meas_to_perform;

      } //  End of test on is PBCCHS, FB/SB or BCCHN task active
    } //end ctrl

    #if FF_L1_IT_DSP_USF
      } // if (l1ps_macs_com.usf_status != USF_AWAITED)

      if (l1ps_macs_com.usf_status != USF_IT_DSP)
      {
    #endif

    // If it's time, update time to next measures position and state
    // Two cases are considered: "pnp_period >= 1s" and "pnp_period < 1s"

    if(schedule_trigger)
    {
      BOOL condition = FALSE;

      schedule_trigger = FALSE;

      // Compute time to next session of measures for "PNP period >= 1s" case
      if(l1pa_l1ps_com.pccch.pnp_period >= 216)
      {
        while(!condition)
        {
          switch(state)
          {
            case NULL_MEAS:
            {
            #if (GSM_IDLE_RAM != 1)
              // Check if memory for L1P_CR_MEAS_DONE msg is allocated
              if (cr_msg == NULL)
              {
                cr_msg = os_alloc_sig(sizeof(T_L1P_CR_MEAS_DONE));
                DEBUGMSG(status,NU_ALLOC_ERR)
                cr_msg->SignalCode = L1P_CR_MEAS_DONE;
              }
            #endif

              if((l1pa_l1ps_com.pccch.time_to_pnp > ((866 * session_done) + time_to_4s)) &&
                 (l1pa_l1ps_com.pccch.pnp_period >= 866))
              {
                state = TOTAL;
                time_to_wake_up = (433 + time_to_4s * session_done);
                condition = TRUE;
              }
              else
              {
                state = PCHTOTAL;
                time_to_wake_up = l1pa_l1ps_com.pccch.time_to_pnp;
                condition = TRUE;
              }
            }
            break;

            case TOTAL:
            case PCHTOTAL:
              state = NULL_MEAS;
            break;
          }
        }
      } // End of "PNP period >= 1s" case
      else
      // Compute time to next session of measures for "PNP period < 1s" case
      {
        while(!condition)
        {
          switch(state)
          {
            case NULL_MEAS:
            {
              // Let's assume a small frequency list size
              // and a PNP period such that PM are performed
              // on first PPCH block and then stopped. PM activity
              // must be re-scheduled at the end of PPCH block.
              if((l1pa_l1ps_com.pccch.time_to_pnp < time_to_1s) && !(init_meas))
              {
                time_to_wake_up  = l1pa_l1ps_com.pccch.time_to_pnp;
                schedule_trigger = TRUE;
                condition        = TRUE;
              }
              else
              {
              #if (GSM_IDLE_RAM == 0)
                // Check if memory for L1P_CR_MEAS_DONE msg is allocated
                if (cr_msg == NULL)
                {
                  cr_msg = os_alloc_sig(sizeof(T_L1P_CR_MEAS_DONE));
                  DEBUGMSG(status,NU_ALLOC_ERR)
                  cr_msg->SignalCode = L1P_CR_MEAS_DONE;
                }
              #endif

                state = MEAS;
                time_to_wake_up = l1pa_l1ps_com.pccch.time_to_pnp;
                remain_carrier  = (WORD8)cr_list_size;
                condition = TRUE;
              }
            }
            break;

            case MEAS:
            {
              UWORD8   max_nbmeas;
              WORD16   tpu_win_rest;
              UWORD16  power_meas_split;

              // Compute how many BP_SPLIT remains for cr list meas
              // Rem: we take into account the SYNTH load for 1st RX in next frame.
              // WARNING: only one RX activity is considered in next paging block !!!
              tpu_win_rest = FRAME_SPLIT - (RX_LOAD + l1_config.params.rx_synth_load_split);

              power_meas_split = (l1_config.params.rx_synth_load_split + PWR_LOAD);
              max_nbmeas = 0;

              while(tpu_win_rest >= power_meas_split)
              {
                max_nbmeas ++;
                tpu_win_rest -= power_meas_split;
              }

              if (max_nbmeas > NB_MEAS_MAX_GPRS) max_nbmeas = NB_MEAS_MAX_GPRS;

              // Update number of remaining carrier to measure.
              // Note: std.nbmeas provides max nbr of PM / TDMA
              remain_carrier -= 4 * max_nbmeas;

              if(remain_carrier <= 0)
                state = NULL_MEAS;
              else
              {
                if((l1pa_l1ps_com.pccch.time_to_pnp >= time_to_1s) || !(session_done_1s))
                {
                  state = MEASTOTAL;
                  time_to_wake_up = 1;
                  condition = TRUE;
                }
                else
                {
                  time_to_wake_up = l1pa_l1ps_com.pccch.time_to_pnp;
                  condition = TRUE;
                }
              }
            }
            break;

            case MEASTOTAL:
            {
              #if (GSM_IDLE_RAM != 1)
                // Check if memory for L1P_CR_MEAS_DONE msg is allocated
                if (cr_msg == NULL)
                {
                  cr_msg = os_alloc_sig(sizeof(T_L1P_CR_MEAS_DONE));
                  DEBUGMSG(status,NU_ALLOC_ERR)
                  cr_msg->SignalCode = L1P_CR_MEAS_DONE;
                }
              #endif

                state = MEAS;
                time_to_wake_up = l1pa_l1ps_com.pccch.time_to_pnp;
                remain_carrier  = (WORD8)cr_list_size;
                condition = TRUE;
            }
            break;
          }
        } // End of while
      } // End of else ("PNP period < 1s" case)
    } // End of if(schedule_trigger)

    #if FF_L1_IT_DSP_USF
      } // if (l1ps_macs_com.usf_status != USF_IT_DSP)

      if (l1ps_macs_com.usf_status != USF_AWAITED)
      {
    #endif

    // Update time to next L1S task
    if((l1pa_l1ps_com.cr_freq_list.ms_ctrl) ||
       (l1pa_l1ps_com.cr_freq_list.ms_ctrl_d))
    {
      // Still some measurement results to get from DSP
      l1a_l1s_com.time_to_next_l1s_task = 0;
    }
    else
    {
      // No more measurements to read, next session of meas must
      // be at time_to_wake_up
      Select_min_time(time_to_wake_up, l1a_l1s_com.time_to_next_l1s_task);
    }

    // Clear controlled flag pnp_ctrl.
    //-------------------------------
    l1pa_l1ps_com.cr_freq_list.pnp_ctrl = 0;

    // C W R pipeline management.
    //---------------------------
    l1pa_l1ps_com.cr_freq_list.ms_ctrl_dd   = l1pa_l1ps_com.cr_freq_list.ms_ctrl_d;
    l1pa_l1ps_com.cr_freq_list.ms_ctrl_d    = l1pa_l1ps_com.cr_freq_list.ms_ctrl;
    l1pa_l1ps_com.cr_freq_list.ms_ctrl      = 0;

    // C W R pipeline management.
    //---------------------------
    {
      UWORD8 i;

      for(i=0; i<NB_MEAS_MAX_GPRS; i++)
      {
        l1pa_l1ps_com.cr_freq_list.used_il_lna_dd[i]  = l1pa_l1ps_com.cr_freq_list.used_il_lna_d[i];
        l1pa_l1ps_com.cr_freq_list.used_il_lna_d [i]  = l1pa_l1ps_com.cr_freq_list.used_il_lna  [i];
      }
    }

    #if FF_L1_IT_DSP_USF
      } // if (l1ps_macs_com.usf_status != USF_AWAITED)
    #endif
  } // End of if: P_CRMS_MEAS enable and associated semaphore = 0.

}

//#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
#endif


#if (MOVE_IN_INTERNAL_RAM == 0) // Must be followed by the pragma used to duplicate the funtion in internal RAM
//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

/*-------------------------------------------------------*/
/* l1ps_transfer_meas_manager()                          */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* Whilst in Packet Transfer mode, MS shall continuously */
/* monitor all the BCCH carriers as indicated by a       */
/* frequency list (BA(GPRS), Network Control frequency   */
/* list and Extended list) and the BCCH carrier of the   */
/* serving cell. Received signal level is used to monitor*/
/* the specified neighbour BCCH carriers.                */
/*                                                       */
/* Receive signal level measurement samples shall be     */
/* performed according to the following conditions:       */
/*                                                       */
/* 1) At least 1 measure shall be done every TDMA,       */
/* 2) Up to 2 TDMA frames per PDCH multiframe may be     */
/*    omitted if required for BSIC decoding,             */
/* 3) Running average value (RLA_P) is based on a 5s     */
/*    period and includes at least 5 measure samples,    */
/* 4) The same number of measures shall be taken for all */
/*    BCCH carriers except:                              */
/*    i) For the Serving Cell, where at least 6 measures */
/*       shall be taken per MF52,                        */
/*    ii) if PC_MEAS_CHAN indicates that power control   */
/*        measures shall be made on BCCH.                */
/* 5) Measures used to compute RLA_P shall as far as     */
/*    possible be uniformly distributed,                 */
/*-------------------------------------------------------*/
void l1ps_transfer_meas_manager()
{
  UWORD8   IL_for_rxlev;

  static xSignalHeaderRec *tcr_msg        = NULL;

  static BOOL  tcr_meas_removed           = FALSE;

  #define tcr_next_to_read        l1pa_l1ps_com.tcr_freq_list.tcr_next_to_read
  #define tcr_next_to_ctrl        l1pa_l1ps_com.tcr_freq_list.tcr_next_to_ctrl
  #define last_stored_tcr_to_read l1pa_l1ps_com.tcr_freq_list.last_stored_tcr_to_read


  #if FF_L1_IT_DSP_USF
    if (l1ps_macs_com.usf_status != USF_IT_DSP)
    {
  #endif

  // Test if task is disabled and cr_msg != NULL, then de-allocate memory
  if(tcr_msg != NULL)
  {
    if(!(l1pa_l1ps_com.l1ps_en_meas & P_TCRMS_MEAS) &&
       !(l1pa_l1ps_com.meas_param & P_TCRMS_MEAS))
    {
      // Neighbour measurement process has been stopped by L3
      // Deallocate memory for the received message if msg not forwarded to L3.
      // ----------------------------------------------------------------------
      os_free_sig(tcr_msg);
      DEBUGMSG(status,NU_DEALLOC_ERR)

      tcr_msg = NULL;
    }
  }

  #if FF_L1_IT_DSP_USF
    }  // if (l1ps_macs_com.usf_status != USF_IT_DSP)
  #endif

  if(l1pa_l1ps_com.l1ps_en_meas & P_TCRMS_MEAS)
  {

    #if FF_L1_IT_DSP_USF
      if (l1ps_macs_com.usf_status != USF_IT_DSP)
      {
    #endif

    // Increment l1pa_l1ps_com.tcr_freq_list.cres_meas_report
    if(++l1pa_l1ps_com.tcr_freq_list.cres_meas_report > 103)
      l1pa_l1ps_com.tcr_freq_list.cres_meas_report = 0;

    // Check if it's first time, Neighbour Measurement process is launched.
    // Then initialize fn_report counter and reset semaphore.
    if(l1pa_l1ps_com.meas_param & P_TCRMS_MEAS)
    {
       // Initialize counter used to report measurements
       l1pa_l1ps_com.tcr_freq_list.cres_meas_report = 0;

       // Reset Neighbour Measurements semaphore
       l1pa_l1ps_com.meas_param &= P_TCRMS_MEAS_MASK;
    }

    //====================================================
    //  RESET MEASUREMENT MACHINES WHEN SYNCHRO EXECUTED.
    //====================================================
    if(l1s.tpu_ctrl_reg & CTRL_SYNC)
    // SYNCHRO task has been executed.
    // -> Reset measures made on serving cell,
    // -> Rewind pointer used in Neighbour Cell measurement,
    // -> return.
    {

      // Reset Neighbour Cell measurement machine.
      // Rewind "next_to_ctrl" counter to come back to the next carrier to
      // measure.
      tcr_next_to_ctrl = tcr_next_to_read;

      // Reset flags.
      l1pa_l1ps_com.tcr_freq_list.ms_ctrl      = 0;
      l1pa_l1ps_com.tcr_freq_list.ms_ctrl_d    = 0;
      l1pa_l1ps_com.tcr_freq_list.ms_ctrl_dd   = 0;

      // Reset measures made on beacon frequency.
      l1pa_l1ps_com.tcr_freq_list.beacon_meas  = 0;
    }

    // *******************
    // Message Allocation
    // *******************
    // The reporting message must be allocated before READ phase.

    if(l1pa_l1ps_com.tcr_freq_list.cres_meas_report == 2)
    {
      // Memory allocation
      if (tcr_msg == NULL)
      {
         // alloc L1P_TCR_MEAS_DONE message...
        tcr_msg = os_alloc_sig(sizeof(T_MPHP_TCR_MEAS_IND));
        DEBUGMSG(status,NU_ALLOC_ERR)
        tcr_msg->SignalCode = L1P_TCR_MEAS_DONE;
      }

      l1pa_l1ps_com.tcr_freq_list.first_pass_flag = TRUE;
    }

    //------------------------------------------------------
    // READ and CTRL phase of the Neighbour Measurement task
    //------------------------------------------------------

    //-----------
    // READ phase
    //-----------

    // Test if Measurment has been removed (ms_ctrl_d forced to 0) during
    // previous frame, then switch DSP read page.
    if (tcr_meas_removed)
    {
      l1s_dsp_com.dsp_r_page_used = TRUE;
      tcr_meas_removed            = FALSE;
    }

    // Background measurements....
    // A measurement controle was performed 2 tdma earlier, read result now!!
    if(l1pa_l1ps_com.tcr_freq_list.ms_ctrl_dd != 0)
    {
      UWORD16  radio_freq_read;
      UWORD16  pm_read;

      #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
        trace_fct(CST_READ_TCR_MEAS, (UWORD32)(-1));
      #endif

      l1_check_com_mismatch(TCR_MEAS_ID);

      // When a read is performed, we set dsp_r_page_used flag to
      // switch the read page
      l1s_dsp_com.dsp_r_page_used = TRUE;

      // Read power measurement result from DSP/MCU GPRS interface
      l1pddsp_meas_read(l1pa_l1ps_com.tcr_freq_list.ms_ctrl_dd, &pm_read);

      l1_check_pm_error(pm_read, TCR_MEAS_ID);
      pm_read = pm_read >> 5;

      radio_freq_read = l1pa_l1ps_com.cres_freq_list.alist->freq_list[tcr_next_to_read];

      // Get Input level corresponding to the used IL and pm result.
      IL_for_rxlev = l1pctl_pgc(((UWORD8) (pm_read)),
                                l1pa_l1ps_com.tcr_freq_list.used_il_lna_dd.il,
                                l1pa_l1ps_com.tcr_freq_list.used_il_lna_dd.lna,
                                radio_freq_read);

      #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        RTTL1_FILL_MON_MEAS(pm_read, IL_for_rxlev, TCR_MEAS_ID, radio_freq_read)
      #endif


      if(l1pa_l1ps_com.tcr_freq_list.first_pass_flag)
      {
        // Fill reporting message: Store RXLEV
        ((T_L1P_TCR_MEAS_DONE*)(tcr_msg->SigP))->acc_level[tcr_next_to_read] =
         l1s_encode_rxlev(IL_for_rxlev);

        ((T_L1P_TCR_MEAS_DONE*)(tcr_msg->SigP))->acc_nbmeas[tcr_next_to_read] = 1;
      }
      else
      {
        // Fill reporting message: Accumulate RXLEV
        ((T_L1P_TCR_MEAS_DONE*)(tcr_msg->SigP))->acc_level[tcr_next_to_read] +=
         l1s_encode_rxlev(IL_for_rxlev);

        ((T_L1P_TCR_MEAS_DONE*)(tcr_msg->SigP))->acc_nbmeas[tcr_next_to_read] += 1;

      }

      // Increment "next_to_read" field for next measurement...
      if(++tcr_next_to_read >= l1pa_l1ps_com.cres_freq_list.alist->nb_carrier)
      {
        tcr_next_to_read    = 0;
      }

      // First pass has been completed on all BA list, reset "first_pass_flag"
      if(tcr_next_to_read == last_stored_tcr_to_read)
        l1pa_l1ps_com.tcr_freq_list.first_pass_flag = FALSE;

    } // End of READ phase


    // ************************
    // Reporting & List Update
    // ************************
    if(l1pa_l1ps_com.tcr_freq_list.cres_meas_report == 1)
    {
      if(tcr_msg != NULL)
      {
        // Fill TCR list identifier field.
        ((T_L1P_TCR_MEAS_DONE*)(tcr_msg->SigP))->list_id = l1pa_l1ps_com.cres_freq_list.alist->list_id;

        // send L1P_TCR_MEAS_DONE message...
        os_send_sig(tcr_msg, L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Reset pointer for debugg.
        tcr_msg = NULL;
      }

      // Update Frequency list pointer and reset new list flag
      if(l1pa_l1ps_com.tcr_freq_list.new_list_present)
      {
        //Update Frequency list pointer
        l1pa_l1ps_com.cres_freq_list.alist = l1pa_l1ps_com.cres_freq_list.flist;

        // Test if a Meas has been controlled in previous frame
        // Then set tcr_meas_removed flag in order to switch DSP read page in next frame
        if(l1pa_l1ps_com.tcr_freq_list.ms_ctrl_d != 0)
        {
          tcr_meas_removed = TRUE;
        }

        // Reset pointer
        tcr_next_to_ctrl                             = 0;
        tcr_next_to_read                             = 0;
        l1pa_l1ps_com.tcr_freq_list.ms_ctrl_d        = 0;

        // Reset New list flag
        l1pa_l1ps_com.tcr_freq_list.new_list_present = FALSE;
      }

      // While reporting, save Last "tcr_next_to_read" value to know when reset "first_pass_flag"
      last_stored_tcr_to_read = tcr_next_to_read;
    }

    //-----------
    // CTRL phase
    //-----------

    // CTRL phase is divided in two parts according measures allocated by MACS.
    // CTRL phase must then be exported in CTRL PDTCH function except for the Idle
    // frame where no PDTCH are programmed.
    // A measure can be performed during the idle frame, only if FB/SB/PTCCH
    // and Interference Measurement task is not active.
    if(!(l1pa_l1ps_com.meas_param & P_TCRMS_MEAS))
    {
      if((l1s.actual_time.t2 == 24) || (l1s.actual_time.t2 == 11))
      {
        if(l1s.forbid_meas == 0)
        {
          #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
            trace_fct(CST_CTRL_TCR_MEAS_2,(UWORD32)(-1));
          #endif

          l1ps_tcr_ctrl(0);
        }
      }
    }

    #if FF_L1_IT_DSP_USF
      } // if (l1ps_macs_com.usf_status != USF_IT_DSP)
    #endif

    #if FF_L1_IT_DSP_USF
      if (l1ps_macs_com.usf_status != USF_AWAITED)
      {
    #endif

    // Pipe Manager
    l1pa_l1ps_com.tcr_freq_list.ms_ctrl_dd = l1pa_l1ps_com.tcr_freq_list.ms_ctrl_d;
    l1pa_l1ps_com.tcr_freq_list.ms_ctrl_d  = l1pa_l1ps_com.tcr_freq_list.ms_ctrl;
    l1pa_l1ps_com.tcr_freq_list.ms_ctrl    = 0;

    #if FF_L1_IT_DSP_USF
      } // if (l1ps_macs_com.usf_status != USF_AWAITED)
    #endif

  } // End of if(l1pa_l1ps_com.l1ps_en_meas & P_TCRMS_MEAS)

  /*****************************/
  /* PC_MEAS_CHAN measurements */
  /*****************************/

  // If PC_MEAS_CHAN = 1, then BCCH serving cell carrier must be
  // measured at least 6 times per MF52.
  // CTRL of Serving Cell Carrier is performed two TDMA earlier.
  if(l1pa_l1ps_com.transfer.aset->pc_meas_chan == FALSE)
  {

    #if FF_L1_IT_DSP_USF
      if (l1ps_macs_com.usf_status != USF_IT_DSP)
      {
    #endif

    if(l1s.tpu_ctrl_reg & CTRL_SYNC)
    // SYNCHRO task has been executed.
    {
      l1ps.pc_meas_chan_ctrl = FALSE;
    }

    //-----------
    // READ phase
    //-----------

    if ((l1ps.pc_meas_chan_ctrl == TRUE) &&
        ((l1s.actual_time.t2 == 3) || (l1s.actual_time.t2 == 11)
          || (l1s.actual_time.t2 == 20)))
    {
      UWORD16   pm_read;

      l1_check_com_mismatch(PC_MEAS_CHAN_ID);

      // When a read is performed, we set dsp_r_page_used flag to
      // switch the read page
      l1s_dsp_com.dsp_r_page_used = TRUE;

      // Read power measurement result from DSP/MCU GPRS interface
      l1pddsp_meas_read(1, &pm_read);


      #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
        trace_fct(CST_READ_PC_MEAS_CHAN,(UWORD32)(-1));
      #endif

      l1_check_pm_error(pm_read, PC_MEAS_CHAN_ID);
      pm_read = pm_read >> 5;

      l1ps.pc_meas_chan_ctrl = FALSE;

      // Get Input level corresponding to the used IL and pm result.
      IL_for_rxlev = l1pctl_pgc(((UWORD8 )(pm_read)),
                                l1pa_l1ps_com.tcr_freq_list.used_il_lna_dd.il,
                                l1pa_l1ps_com.tcr_freq_list.used_il_lna_dd.lna,
                                l1a_l1s_com.Scell_info.radio_freq);

      #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        RTTL1_FILL_MON_MEAS(pm_read, IL_for_rxlev, PC_MEAS_CHAN_ID, l1a_l1s_com.Scell_info.radio_freq)
      #endif

      if (l1a_l1s_com.mode == PACKET_TRANSFER_MODE)
        // Store RXLEV, before to pass it to maca_power_control() function..
        l1pa_l1ps_com.tcr_freq_list.beacon_meas = l1s_encode_rxlev(IL_for_rxlev);
    }

    //-----------
    // CTRL phase
    //-----------

    // In two phase access, PC_MEAS_CHAN measurements can be done...
    if((l1a_l1s_com.l1s_en_task[SINGLE] == TASK_ENABLED) &&
       (l1pa_l1ps_com.transfer.aset->allocated_tbf == TWO_PHASE_ACCESS))
    if (l1s.task_status[NP].current_status != ACTIVE) // avoid conflict with Normal Paging
    if (l1s.task_status[EP].current_status != ACTIVE) // avoid conflict with Extended Paging
    {
      // Measurement on the beacon
      if((l1s.actual_time.t2 == 1) || (l1s.actual_time.t2 == 9) ||
         (l1s.actual_time.t2 == 18))
      {
        // Measurement programming
        // ts 4 is specified for DSP interface ONLY because the power activity
        // must be programmed after RX and/or TX activity (no multislot)
        #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
          trace_fct(CST_CTRL_PC_MEAS_CHAN, (UWORD32)(-1));
        #endif

        l1ps_bcch_meas_ctrl(4);
      }
    }

    #if FF_L1_IT_DSP_USF
      } // if (l1ps_macs_com.usf_status != USF_IT_DSP)
    #endif

  } // End of Meas made on BCCH serving cell

  #if FF_L1_IT_DSP_USF
    if (l1ps_macs_com.usf_status != USF_AWAITED)
    {
  #endif

  if((l1pa_l1ps_com.l1ps_en_meas & P_TCRMS_MEAS) || (l1ps.pc_meas_chan_ctrl == TRUE))
  {
    // C W R pipeline management.
    //---------------------------
    l1pa_l1ps_com.tcr_freq_list.used_il_lna_dd  = l1pa_l1ps_com.tcr_freq_list.used_il_lna_d;
    l1pa_l1ps_com.tcr_freq_list.used_il_lna_d   = l1pa_l1ps_com.tcr_freq_list.used_il_lna;
  }

  #if FF_L1_IT_DSP_USF
    } // if (l1ps_macs_com.usf_status != USF_AWAITED)
  #endif

}
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END
#endif // MOVE_IN_INTERNAL_RAM

//#pragma DUPLICATE_FOR_INTERNAL_RAM_START
#endif
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END
