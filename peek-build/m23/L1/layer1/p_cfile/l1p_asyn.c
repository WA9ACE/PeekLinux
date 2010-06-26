/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1P_ASYN.C
 *
 *        Filename l1p_asyn.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

//#pragma DUPLICATE_FOR_INTERNAL_RAM_START
  #include "l1_macro.h"
  #include "l1_confg.h"
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END

#if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))  // MOVE TO INTERNAL MEM IN CASE GSM_IDLE_RAM enabled
//#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START         // KEEP IN EXTERNAL MEM otherwise

#define  L1P_ASYN_C

//#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END         // KEEP IN EXTERNAL MEM otherwise
#endif

//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

#include "l1_macro.h"
#include "l1_confg.h"

#if L1_GPRS

#if (CODE_VERSION == SIMULATION)
  #include <string.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
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
  #include "l1_time.h"
  #include "l1_ctl.h"

  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"
  #include "l1p_tabs.h"
  #include "l1p_sign.h"
  #include "l1p_mfta.h"
  #include "l1p_macr.h"

  #include "macs_def.h"
  #include "macs_cst.h"
#else
  #include <string.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"

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
  #include "l1_time.h"
  #include "l1_ctl.h"

  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"
  #include "l1p_tabs.h"
  #include "l1p_sign.h"
  #include "l1p_mfta.h"
  #include "l1p_macr.h"

  #include "macs_def.h"
  #include "macs_cst.h"
#endif

T_TRANSFER_SET   *l1pa_get_free_transfer_set              (UWORD8  new_tbf);
void              l1pa_transfer_process                   (xSignalHeaderRec *msg);
void              l1pa_access_process                     (xSignalHeaderRec *msg);
void              l1pa_idle_packet_polling_process        (xSignalHeaderRec *msg);
void              l1pa_idle_paging_process                (xSignalHeaderRec *msg);
void              l1pa_cr_meas_process                    (xSignalHeaderRec *msg);
void              l1pa_serving_cell_pbcch_read_process    (xSignalHeaderRec *msg);
void              l1pa_neighbor_cell_pbcch_read_process   (xSignalHeaderRec *msg);
int               l1pa_sort                               (const void *a, const void *b);
void              l1pa_reset_cr_freq_list                 (void);
void              l1pa_tcr_meas_process                   (xSignalHeaderRec *msg);
T_CRES_LIST_PARAM *l1pa_get_free_cres_list_set            (void);
void              l1pa_idle_interference_meas_process     (xSignalHeaderRec *msg);
void              l1pa_transfer_interference_meas_process (xSignalHeaderRec *msg);
void              l1pa_idle_smscb_process                 (xSignalHeaderRec *msg);

// External prototype
void l1pa_send_int_meas_report(UWORD32               SignalCode,
                               T_L1P_ITMEAS_IND     *last_l1s_msg,
                               T_L1A_INT_MEAS_PARAM *first_meas_ptr);
void l1pa_send_confirmation(UWORD32 SignalCode, UWORD8 id);
void l1pa_send_tbf_release_con(UWORD32 SignalCode, UWORD8 tbf_type);

//#pragma DUPLICATE_FOR_INTERNAL_RAM_END

#if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM > 1))  // MOVE TO INTERNAL MEM IN CASE GSM_IDLE_RAM == 2
//#pragma GSM_IDLE2_DUPLICATE_FOR_INTERNAL_RAM_START         // KEEP IN EXTERNAL MEM otherwise


/*-------------------------------------------------------*/
/* l1pa_task()                                           */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* L1PA (Layer 1 Asynchronous) task function. This       */
/* function manages the GPRS interface between L3 and L1.*/
/* It is composed with a set of state machine, each      */
/* machine handles a particular GSM functionality. When  */
/* a message is received in L1_C1 message queue, it is   */
/* submitted to every state machine. The one which are   */
/* impacted by the message process it. At the end of     */
/* "l1pa_task()" function, a balance routine is called,  */
/* it enables L1S tasks consequently to the state machine*/
/* requests.                                             */
/*                                                       */
/*-------------------------------------------------------*/
void  l1pa_task(xSignalHeaderRec *msg)
{
  UWORD8  process;

  // Clear L1PA "enable meas and tasks" variables.
  //---------------------------------------------
  for(process=0; process<NBR_L1PA_PROCESSES; process++)
  {
    l1pa.l1pa_en_meas[process] = NO_TASK;
  }

  #if (GSM_IDLE_RAM != 0)
    if ((msg->SignalCode != L1P_PNP_INFO) && (msg->SignalCode != L1P_PEP_INFO))

    #if (GSM_IDLE_RAM > 1) // GPF modified for GSM_IDLE_RAM -> SW still running in Internal RAM
          {
    #endif
            l1s.gsm_idle_ram_ctl.l1s_full_exec = TRUE;
  #endif // GSM_IDLE_RAM

  // Serving Cell Packet System Information Reading
  l1pa_serving_cell_pbcch_read_process(msg);

  // Neighbor Cell Packet System Information Reading
  l1pa_neighbor_cell_pbcch_read_process(msg);

  #if (GSM_IDLE_RAM <= 1)  // GPF modified for GSM_IDLE_RAM -> SW still running in Internal RAM
  {
    // Serving Cell Packet Paging Reading
    l1pa_idle_paging_process(msg);
  }
  #endif
  // Cell reselection measurement process
  l1pa_cr_meas_process(msg);

  // Packet access proccess
  l1pa_access_process(msg);

  // Packet polling process
  l1pa_idle_packet_polling_process(msg);

  // Packet transfer process.
  l1pa_transfer_process(msg);

  // Neighbour Cell Measurement in Packet Transfer mode
  l1pa_tcr_meas_process(msg);

  // Intererence measurements in packet idle mode
  l1pa_idle_interference_meas_process(msg);

  // Intererence measurements in packet transfer mode
  l1pa_transfer_interference_meas_process(msg);

  #if (GSM_IDLE_RAM > 1)  // GPF modified for GSM_IDLE_RAM -> SW still running in Internal RAM
  }else
  {
    // Serving Cell Packet Packet Idle Paging Reading
    l1pa_idle_paging_process(msg);
  }
  #endif
}

//#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END         // KEEP IN EXTERNAL MEM otherwise
#endif

/*-------------------------------------------------------*/
/* l1pa_access_process()                                 */
/*-------------------------------------------------------*/
/* Description : This state machine handles the packet   */
/* access to the network while in PACKET IDLE mode.      */
/*                                                       */
/* Starting messages:        MPHP_RA_REQ                 */
/*                                                       */
/* Subsequent messages:      MPHP_RA_REQ                 */
/*                                                       */
/* Result messages (input):  L1P_PRA_DONE                */
/*                                                       */
/* Result messages (output): MPHP_RA_CON                 */
/*                                                       */
/* Reset message (input): MPHP_RA_STOP_REQ               */
/*                                                       */
/* Reset message (input): MPHP_RA_STOP_CON               */
/*-------------------------------------------------------*/
void l1pa_access_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET       = 0,
    WAIT_INIT   = 1,
    WAIT_RESULT = 2
  };

  UWORD8   *state      = &l1pa.state[P_ACC];
  UWORD32   SignalCode = msg->SignalCode;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        // Reset PRACH process.
        l1a_l1s_com.l1s_en_task[PRACH]     = TASK_DISABLED;  // Clear PRACH task enable flag.
        l1pa_l1ps_com.pra_info.prach_alloc = 0;
      }
      break;


      case WAIT_INIT:
      {
        if(SignalCode == MPHP_RA_REQ)
        // 1st Random access request message.
        //-----------------------------------
        {

          // Download Transmit power configuration.
          // Given value must be used on 1st TX.
          // TXPWR value supplied by L3 is the max. TX power level the MS may use in the given band
          l1s.applied_txpwr = ((T_MPHP_RA_REQ *)(msg->SigP))->txpwr;


          #if L1_R99
          // Init PRACH process.
            // "rand" parameter chosen by protocol in range [1..4] for R99
            l1pa_l1ps_com.pra_info.rand                 = ((T_MPHP_RA_REQ *)(msg->SigP))->rand;
          #else
          // "rand" parameter from msg is not used for the 1st PRACH.
          l1pa_l1ps_com.pra_info.rand                 = 1; // First PRACH has to be sent immediately
          #endif
          l1pa_l1ps_com.pra_info.channel_request_data = ((T_MPHP_RA_REQ *)(msg->SigP))->channel_request_data;
          l1pa_l1ps_com.pra_info.bs_prach_blks        = ((T_MPHP_RA_REQ *)(msg->SigP))->bs_prach_blks;
          l1pa_l1ps_com.access_burst_type             = ((T_MPHP_RA_REQ *)(msg->SigP))->access_burst_type;

          // Increment rand parameter by 4 in order to avoid conflict between SYNCHRO and
          // PRACH tasks when MPHP_START_PCCCH_REQ and MPHP_RA_REQ are
          // sent at the same time by L3
          l1pa_l1ps_com.pra_info.rand+=4;

          if ((l1pa_l1ps_com.pra_info.bs_prach_blks == 0) || // no blocks allocated
              (l1pa_l1ps_com.pra_info.bs_prach_blks > 12))   // invalid number of blocks
            l1pa_l1ps_com.pra_info.prach_alloc = DYN_PRACH_ALLOC;

          // step in state machine.
          *state = WAIT_RESULT;

          // Change mode to connection establishment part 1.
          l1a_l1s_com.mode = CON_EST_MODE1;

          // Activate PRACH task (no semaphore for UL tasks).
          l1a_l1s_com.l1s_en_task[PRACH] = TASK_ENABLED; // Set PRACH task enable flag.

        }

        // end of process.
        end_process = 1;
      }
      break;

      case WAIT_RESULT:
      {
        if(SignalCode == L1P_RA_DONE)
        // Random access acknowledge message: PRACH sent.
        //-----------------------------------------------
        {
          // Forward result message to L3.
          l1a_send_result(MPHP_RA_CON, msg, GRRM1_QUEUE);

          // Change mode to connection establishment part 2.
          l1a_l1s_com.mode = CON_EST_MODE2;

          // end of process.
          return;
        }

        else
        if(SignalCode == MPHP_RA_REQ)
        // Random access message.
        //-----------------------
        {
          // REM: rand is added the msg content since its current content is the already
          // spent "slots" from the last PRACH sending.
          l1pa_l1ps_com.pra_info.rand                += ((T_MPHP_RA_REQ *)(msg->SigP))->rand + 1;
          l1pa_l1ps_com.pra_info.channel_request_data = ((T_MPHP_RA_REQ *)(msg->SigP))->channel_request_data;
          l1pa_l1ps_com.pra_info.bs_prach_blks        = ((T_MPHP_RA_REQ *)(msg->SigP))->bs_prach_blks;
          l1pa_l1ps_com.access_burst_type             = ((T_MPHP_RA_REQ *)(msg->SigP))->access_burst_type;

          if ((l1pa_l1ps_com.pra_info.bs_prach_blks == 0) || // no blocks allocated
              (l1pa_l1ps_com.pra_info.bs_prach_blks > 12))   // invalid number of blocks
            l1pa_l1ps_com.pra_info.prach_alloc = DYN_PRACH_ALLOC;
//          else
//            l1pa_l1ps_com.pra_info.prach_alloc = FIX_PRACH_ALLOC;
          else
            l1pa_l1ps_com.pra_info.prach_alloc = 0; //must be reset for each burst sent


          // Activate PRACH task (no semaphore for UL tasks).
          l1a_l1s_com.l1s_en_task[PRACH] = TASK_ENABLED;   // Set PRACH task enable flag.

          // end of process.
          return;
        }

        else
        if(SignalCode == MPHP_RA_STOP_REQ)
        // Request to STOP the LINK ACCESS procedure.
        //-------------------------------------------
        {
          UWORD8 i;

          // send confirmation
          l1a_send_confirmation(MPHP_RA_STOP_CON,GRRM1_QUEUE);

          // Store MAX TXPWR value to be used for first Tx PDCH blocks
          for(i = 0; i < 8; i++)
          {
            l1pa_l1ps_com.transfer.dl_pwr_ctrl.txpwr[i] = l1s.applied_txpwr;
          }

          // This process must be reset.
          *state = RESET;
        }

        else
        if(SignalCode == MPHP_POLLING_RESPONSE_REQ)
        // Stop packet access when packet polling initiated.
        //--------------------------------------------------
        {
          // Unacknowledged

          // This process must be reset.
          *state = RESET;
        }

        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          end_process = 1;
        }
      }
      break;
    } // end of "switch".
  } // end of "while"
} // end of procedure.

/*-------------------------------------------------------*/
/* l1pa_cr_meas_process()                                */
/*-------------------------------------------------------*/
/* Description : This state machine handles periodic     */
/* signal strength monitoring on carriers specified      */
/* in a frequency list: BA(GPRS), NC_FREQUENCY_LIST,     */
/* EXT_FREEQUENCY_LIST (respectively: cell reselection   */
/* Network Control and Extended measurements.            */
/*                                                       */
/* Starting messages:        MPHP_CR_MEAS_REQ            */
/* ------------------                                    */
/*  L1 starts then the periodic FREQUENCY list receive   */
/*  level monitoring.                                    */
/*                                                       */
/* Subsequent messages:      MPHP_CR_MEAS_REQ            */
/* --------------------                                  */
/*  The frequency list is updated only when measures     */
/*  on all the carriers of the current list are          */
/*  performed.                                           */
/*                                                       */
/* Result messages (input):  L1P_RXLEV_PERIODIC_DONE     */
/* ------------------------                              */
/*  This is a message reported to L1A from L1S.          */
/*  Reporting is done when last carrier of the frequency */
/*  list is read.                                        */
/*                                                       */
/* Result messages (output): MPHP_CR_MEAS_IND            */
/* -------------------------                             */
/*  This is the periodic reporting message to L3.        */
/*                                                       */
/* Reset messages (input):   MPHP_CR_MEAS_STOP_REQ       */
/* -----------------------                               */
/*  Frequency list measurement process is stopped by     */
/*  this message.                                        */
/*                                                       */
/*-------------------------------------------------------*/
void l1pa_cr_meas_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET       = 0,
    WAIT_INIT   = 1,
    WAIT_RESULT = 2

  };

  UWORD8   *state      = &l1pa.state[CR_MEAS];
  UWORD32   SignalCode = msg->SignalCode;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // step in state machine.
        *state = WAIT_INIT;

        // Reset P_CRMS_MEAS process.
        l1pa_l1ps_com.l1ps_en_meas &= P_CRMS_MEAS_MASK; // Reset Packet Cell Reselection Measurement enable flag.
      }
      break;

      case WAIT_INIT:
      {
        if(SignalCode == MPHP_CR_MEAS_REQ)
        // We receive the Frequency list to be monitored.
        //----------------------------------------------
        {
          UWORD8  i;
          T_CRES_LIST_PARAM  *free_list;

          // Set parameter synchro semaphore for P_CRMS_MEAS task.
          l1pa_l1ps_com.meas_param |= P_CRMS_MEAS;

          // Reset the frequency list structure.
          l1pa_reset_cr_freq_list();

          // Get Ptr to the free Neighbour meas list.
          // The number of carriers in the list and the list
          // identification are initialized.
          free_list = l1pa_get_free_cres_list_set();

          // Set number of carrier in the frequency list.
          free_list->nb_carrier = ((T_MPHP_CR_MEAS_REQ *)(msg->SigP))->nb_carrier;

          // Store ARFCN list in the Packet Cell Reselection structure.
          for(i=0;i<free_list->nb_carrier;i++)
            free_list->freq_list[i] = ((T_MPHP_CR_MEAS_REQ *)(msg->SigP))->radio_freq_no[i];

          // Download Frequency list identifier.
          free_list->list_id = ((T_MPHP_CR_MEAS_REQ *)(msg->SigP))->list_id;

          // Set "flist" with new set of frequency list parameter
          l1pa_l1ps_com.cres_freq_list.flist = free_list;

          // Enable Packet Cell Reselection measurement task.
          l1pa.l1pa_en_meas[CR_MEAS] |= P_CRMS_MEAS;

          // step in state machine.
          *state = WAIT_RESULT;
        }

        // End of process.
        end_process = 1;
      }
      break;

      case WAIT_RESULT:
      {
        if(SignalCode == L1P_CR_MEAS_DONE)
        // One set of measurement has been completed.
        //---------------------------------------------
        {
          // Forward result message to L3.
          l1a_send_result(MPHP_CR_MEAS_IND, msg, GRRM1_QUEUE);

          // End of process.
          end_process = 1;
        }

        else
        if((SignalCode == MPHP_CR_MEAS_STOP_REQ) ||
           (SignalCode == L1P_TRANSFER_DONE)     ||
           (SignalCode == L1C_DEDIC_DONE))
        // Request to STOP this activity.
        //-------------------------------
        {
          // send confirmation message
          l1a_send_confirmation(MPHP_CR_MEAS_STOP_CON,GRRM1_QUEUE);
          // This process must be reset.
          *state = RESET;
        }

        else
        if (SignalCode == MPHP_CR_MEAS_REQ)
        {
          // This process must be reset.
          *state = RESET;
        }

        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          end_process = 1;
        }
      }
      break;
    } // end of "switch".
  } // end of "while"
} // end of procedure.

#if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM > 1))  // MOVE TO INTERNAL MEM IN CASE GSM_IDLE_RAM == 2
//#pragma GSM_IDLE2_DUPLICATE_FOR_INTERNAL_RAM_START         // KEEP IN EXTERNAL MEM otherwise

/*-------------------------------------------------------*/
/* l1pa_idle_paging_process()                            */
/*-------------------------------------------------------*/
/*                                                       */
/*-------------------------------------------------------*/
void l1pa_idle_paging_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET             = 0,
    WAIT_INIT         = 1,
    WAIT_MSG          = 2
  };

  enum pg_mode
  {
    NORM_PG           = 0,
    EXT_PG            = 1,
    REORG_PG          = 2
  };

  UWORD8  *state     = &l1pa.state[PI_SCP];
  UWORD32 SignalCode = msg->SignalCode;
  UWORD16 imsimod;
  UWORD16 split_pg_cycle;
  UWORD16 kcn;
  UWORD8  page_mode;

  BOOL end_process = 0;

  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        // Disable serving cell tasks.
        l1a_l1s_com.l1s_en_task[PALLC]   = TASK_DISABLED;     // Reset PALLC (reorg) task enable flag.
        l1a_l1s_com.l1s_en_task[PNP]     = TASK_DISABLED;     // Reset PNP     task enable flag.
        l1a_l1s_com.l1s_en_task[PEP]     = TASK_DISABLED;     // Reset PEP     task enable flag.

        // No Paging  => no gauging => no Deep sleep
        l1s.pw_mgr.enough_gaug = FALSE;  // forbid Deep sleep

      }
      break;

      case WAIT_INIT:
      {
        if(SignalCode == MPHP_START_PCCCH_REQ)
        {
          // Set semaphores for any PCCCH reading tasks.
          l1a_l1s_com.task_param[PALLC] = SEMAPHORE_SET;
          l1a_l1s_com.task_param[PNP]   = SEMAPHORE_SET;
          l1a_l1s_com.task_param[PEP]   = SEMAPHORE_SET;

          // Request to enter in PACKET PAGING REORGANIZATION or NORMAL mode.
          //----------------------------------------------------------------
          // The initial page mode in the Mobile Station shall be set to paging Reorganization
          // cf 04.08 section 3.3.2.1.1. however current implementation allows to init the Paging
          // procedure either in Reorganization or in Normal paging mode.

          // Download the PAGING PARAMETERS from the command message.
          page_mode                           = ((T_MPHP_START_PCCCH_REQ *)(msg->SigP))->page_mode;
          imsimod                             = ((T_MPHP_START_PCCCH_REQ *)(msg->SigP))->imsimod;
          kcn                                 = ((T_MPHP_START_PCCCH_REQ *)(msg->SigP))->kcn;
          split_pg_cycle                      = ((T_MPHP_START_PCCCH_REQ *)(msg->SigP))->split_pg_cycle;
          l1pa_l1ps_com.pccch.bs_pag_blks_res = ((T_MPHP_START_PCCCH_REQ *)(msg->SigP))->bs_pag_blks_res;
          l1pa_l1ps_com.pccch.bs_pbcch_blks   = ((T_MPHP_START_PCCCH_REQ *)(msg->SigP))->bs_pbcch_blks;
          l1pa_l1ps_com.pccch.frequency_list  = ((T_MPHP_START_PCCCH_REQ *)(msg->SigP))->frequency_list;
          l1pa_l1ps_com.pccch.packet_chn_desc = ((T_MPHP_START_PCCCH_REQ *)(msg->SigP))->packet_chn_desc;
          l1a_l1s_com.Scell_info.pb           = ((T_MPHP_START_PCCCH_REQ *)(msg->SigP))->pb << 2; // Pb format 7.1

          // Number of paging blocks "available" on one PCCCH = number of ppch blocks per MF52 * 64
          // = (12 - BS_PAG_BLKS_RES - BS_PBCCH_BLKS)*64
          l1pa_l1ps_com.pccch.nb_ppch_per_mf52 = (12 - l1pa_l1ps_com.pccch.bs_pag_blks_res - (l1pa_l1ps_com.pccch.bs_pbcch_blks + 1));

          // Compute M.
          l1pa_l1ps_com.pccch.pg_blks_avail    =  l1pa_l1ps_com.pccch.nb_ppch_per_mf52 * 64;

          // (IMSI mod 1000) div (KC*N) (Note: N = 1 for PCCCH)
          l1pa_l1ps_com.pccch.pg_offset        = imsimod / kcn;

          // First Paging Group value: PAGING_GROUP = ((IMSI mod 1000) div ((KC*N)*N)) (for m = 0)
          l1pa_l1ps_com.pccch.first_pg_grp     = (l1pa_l1ps_com.pccch.pg_offset % l1pa_l1ps_com.pccch.pg_blks_avail);

          // Split Paging computation = min (pg_blks_avail, SPLIT_PG_CYCLE)
          l1pa_l1ps_com.pccch.split_pg_value = Min(l1pa_l1ps_com.pccch.pg_blks_avail, split_pg_cycle);

          // Paging Period computation
          l1pa_l1ps_com.pccch.pnp_period = (64*52) / l1pa_l1ps_com.pccch.split_pg_value;

          // Rem: changing the paging parameters changes the place where "Periodic Packet
          //      Measurement" task must be executed. It implies to set semaphore for P_CRMS task.
          l1pa_l1ps_com.meas_param |= P_CRMS_MEAS;

          // Layer 1 internal mode is set to IDLE MODE.
          l1a_l1s_com.mode = I_MODE;

          // In order to keep tn_difference and dl_tn consistent, we need to avoid
          // the execution of the SYNCHRO task with tn_difference updated and
          // dl_tn not yet updated (this can occur if we go in the HISR just after
          // the update of tn_difference). To do this the solution is to use the Semaphore
          // associated to the SYNCHRO task. SYNCHRO task will be schedule only if its
          // associated Semaphore is reset.
          // Note: Due to the specificity of the SYNCHRO task which can be enabled
          // by L1A state machines as by L1S processes, the semaphore can't followed
          // the generic rules of the Semaphore shared between L1A and L1S.
          // We must shift the mobile time setting to the timeslot provided by
          // ((T_MPHP_START_PCCCH_REQ *)(msg->SigP))->packet_chn_desc.timeslot_no parameter.
          //   tn_difference -> loaded with the number of timeslot to shift.
          //   dl_tn         -> loaded with the new timeslot.
          l1a_l1s_com.task_param[SYNCHRO] = SEMAPHORE_SET;
          {
            l1a_l1s_com.tn_difference += l1pa_l1ps_com.pccch.packet_chn_desc.timeslot_no - l1a_l1s_com.dl_tn;
            l1a_l1s_com.dl_tn          = l1pa_l1ps_com.pccch.packet_chn_desc.timeslot_no;

            // Select GPRS DSP Scheduler.
            l1a_l1s_com.dsp_scheduler_mode = GPRS_SCHEDULER;

            // Timing must be shifted to a new timeslot, enables SYNCHRO task..
            l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;
          }
          l1a_l1s_com.task_param[SYNCHRO] = SEMAPHORE_RESET;
          // Note: The using of the semaphore associated to the SYNCHRO task can't be done
          // as it is for the other semaphores. This is due to the specificity of the SYNCHRO
          // task both touch by L1A and L1S. Here above the semaphore is set prior to touching
          // the SYNCHRO parameters and reset after. In L1S this semaphore is checked. If it's
          // seen SET then L1S will not execute SYNCHRO task nor modify its parameters.

          // Step in state machine.
          *state = WAIT_MSG;

          if(page_mode == REORG_PG)
          // Paging Reorganization mode...
          {
            // Enable Packet Paging Reorganisation tasks.
            l1a_l1s_com.l1s_en_task[PALLC] = TASK_ENABLED;
            l1a_l1s_com.l1s_en_task[PNP]   = TASK_ENABLED;

            // End of process.
            end_process = 1;
          }
          else
          if(page_mode == NORM_PG)
          // Normal Paging mode...
          {
            // Enable Packet Paging tasks in mode "NORMAL".
            l1a_l1s_com.l1s_en_task[PNP]  = TASK_ENABLED;

            // End of process.
            end_process = 1;
          }
          else
          // Extended Paging mode...
          {
            // Initialize Paging State for PAGING_GROUP computation (L1S part)
            l1pa_l1ps_com.pccch.epg_computation = PPCH_POS_NOT_COMP;

            // Enable Packet Paging tasks in mode "EXTENDED".
            l1a_l1s_com.l1s_en_task[PNP]  = TASK_ENABLED;
            l1a_l1s_com.l1s_en_task[PEP]  = TASK_ENABLED;

            // End of process.
            end_process = 1;
          }

        } // end of test on SignalCode == MPHP_START_PCCCH_REQ

        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          end_process = 1;
        }

      } // end of case WAIT_INIT
      break;

      case WAIT_MSG:
      {
        if((SignalCode == L1P_PNP_INFO)   ||
           (SignalCode == L1P_PALLC_INFO) ||
           (SignalCode == L1P_PEP_INFO))
        // Paging Task results
        {
          // Forward result message to L3.
          l1a_send_result(MPHP_DATA_IND, msg, GRRM1_QUEUE);

          // End of process.
          return;
        }

        else
        if(SignalCode == MPHP_START_PCCCH_REQ)
        // New PCCCH configuration is provided.
        //--------------------------------------------------------
          {
          // Step in state machine
            *state = RESET;
          }

        else
        if((SignalCode == MPHP_STOP_PCCCH_REQ) ||
           (SignalCode == L1P_TRANSFER_DONE) || (SignalCode == L1C_DEDIC_DONE))
        // Request to STOP any serving cell Packet Paging activity, OR
        // Packet Transfer has just started.
        // In both cases, PCCCH reading must be stopped.
        //--------------------------------------------------------
        {
          // Send confirmation message to L3.
          l1a_send_confirmation(MPHP_STOP_PCCCH_CON,GRRM1_QUEUE);

          // This process must be reset.
          *state = RESET;
        }

        else
        if((SignalCode == L1P_SINGLE_BLOCK_CON) ||
           (SignalCode == MPHP_SINGLE_BLOCK_CON))
        // If Two Phase Access is ongoing: Packet Resource Request
        // msg has been sent to the network. PCCCH reading must be
        // stopped to let PDCH reading going.
        // REM: we must check both L1P/MPHP messages since an other
        // process could have renamed L1P into MPHP.
        //--------------------------------------------------------
        {
          if(((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->purpose == TWO_PHASE_ACCESS)
        {
          // This process must be reset.
          *state = RESET;
        }
        else
        {
            // End of process.
            return;
          }
        }

        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          return;
        }

      } // end of case WAIT_MSG
      break;
    } // end of switch
  } // end of while
} // end of procedure

//#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END
#endif

/*-------------------------------------------------------*/
/* l1pa_idle_packet_polling_process()                    */
/*-------------------------------------------------------*/
/* Description :                    .                    */
/* This state machine handles packet polling after       */
/* initiation of the packet acces procedure, when a      */
/* packet queuing notification is sent by the network    */
/*                                                       */
/* Starting messages:        MPHP_POLLING_RESPONSE_REQ   */
/*                                                       */
/* Subsequent messages:                                  */
/*                                                       */
/* Result messages (input):  L1P_PRA_DONE                */
/*                                                       */
/* Result messages (output): MPHP_POLLING_IND            */
/*                                                       */
/* Reset message (input):                                */
/*                                                       */
/* Reset message (input):                                */
/*-------------------------------------------------------*/
void l1pa_idle_packet_polling_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET       = 0,
    WAIT_INIT   = 1,
    WAIT_RESULT = 2
  };

  UWORD8   *state      = &l1pa.state[P_POLL];
  UWORD32   SignalCode = msg->SignalCode;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        // Reset POLL process.
        l1a_l1s_com.l1s_en_task[POLL] = TASK_DISABLED;  // Clear RAACC task enable flag.
      }
      break;


      case WAIT_INIT:
      {
        if(SignalCode == MPHP_POLLING_RESPONSE_REQ)
        // Polling response request for access procedure.
        //-----------------------------------------------
        {
          UWORD8 i;

          // Init POLL process.
          l1pa_l1ps_com.poll_info.pol_resp_type = ((T_MPHP_POLLING_RESPONSE_REQ *)(msg->SigP))->pol_resp_type;
          l1pa_l1ps_com.poll_info.fn            = ((T_MPHP_POLLING_RESPONSE_REQ *)(msg->SigP))->fn;

         // TXPWR value supplied by L3 is the max. TX power level the MS may use in the given band
          l1s.applied_txpwr                     = ((T_MPHP_POLLING_RESPONSE_REQ *)(msg->SigP))->txpwr;

          // 1 RLC/MAC block is sent:
          // This is a special case, only possible if the MS has a valid TA available from a pending
          // assignment and is pooled for an RLC/MAC block.
          if (l1pa_l1ps_com.poll_info.pol_resp_type == CS1_TYPE_POLL)
          {
            for (i=0; i<24; i++)
            {
              // download 24 bytes from message
              l1pa_l1ps_com.poll_info.chan_req.cs1_data[i] =
                ((T_MPHP_POLLING_RESPONSE_REQ *)(msg->SigP))->channel_request_data[i];
            }
            l1pa_l1ps_com.poll_info.timing_advance = ((T_MPHP_POLLING_RESPONSE_REQ *)(msg->SigP))->timing_advance;

            // Store MAX TXPWR value to be used for first POLL RESPONSE
            // if not already done by Packet Access process...
            for(i = 0; i < 8; i++)
            {
              l1pa_l1ps_com.transfer.dl_pwr_ctrl.txpwr[i] = l1s.applied_txpwr;
            }
          }
          // 4 identical PRACH are sent
          else
          {
            // UWORD16 = data[1]<<8 | data[0]
            l1pa_l1ps_com.poll_info.chan_req.prach_data[0] =
              ((T_MPHP_POLLING_RESPONSE_REQ *)(msg->SigP))->channel_request_data[0];
            l1pa_l1ps_com.poll_info.chan_req.prach_data[0] |=
              (((T_MPHP_POLLING_RESPONSE_REQ *)(msg->SigP))->channel_request_data[1] << 8);

            l1pa_l1ps_com.poll_info.timing_advance = 0;
          }

          // step in state machine.
          *state = WAIT_RESULT;

          // Change mode to connection establishment part 1.
          l1a_l1s_com.mode = CON_EST_MODE1;

          // Activate POLL task (no semaphore for UL tasks).
          // Enable Paging Reorg and Normal paging tasks.
          l1a_l1s_com.l1s_en_task[POLL] = TASK_ENABLED; // Set PRACH task enable flag.
        }

        // end of process.
        end_process = 1;
      }
      break;

      case WAIT_RESULT:
      {
        if(SignalCode == L1P_POLL_DONE)
        // Random access acqnowledge message.
        //-----------------------------------
        {
          // Forward result message to L3.
          l1a_send_result(MPHP_POLLING_IND, msg, GRRM1_QUEUE);

          // Change mode to connection establishment part 2.
          l1a_l1s_com.mode = CON_EST_MODE2;

          // This state machine has to be reset
          *state = RESET;

          // end of process.
          end_process = 1;
        }

        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          end_process = 1;
        }
      }
      break;
    } // end of "switch".
  } // end of "while"
} // end of procedure.


/*-------------------------------------------------------*/
/* l1pa_transfer_process()                               */
/*-------------------------------------------------------*/
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* Starting messages:                                    */
/* ------------------                                    */
/*                                                       */
/* Subsequent messages:                                  */
/* --------------------                                  */
/*                                                       */
/* Result messages (input):                              */
/* ------------------------                              */
/*                                                       */
/* Result messages (output):                             */
/* -------------------------                             */
/*                                                       */
/* Reset messages (input):                               */
/* -----------------------                               */
/*                                                       */
/*-------------------------------------------------------*/
void l1pa_transfer_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET       = 0,
    WAIT_MSG    = 1
  };

  UWORD8   *state     = &l1pa.state[TRANSFER];
  UWORD32  SignalCode = msg->SignalCode;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_MSG;

        // Rise transfert parameter semaphore.
        l1pa_l1ps_com.transfer.semaphore = TRUE;
      }
      break;

      case WAIT_MSG:
      {
        switch(SignalCode)
        // switch on input message.
        //-------------------------
        {
          case MPHP_SINGLE_BLOCK_REQ:
          // Repeat fixed allocation.
          //-------------------------
          {
            T_TRANSFER_SET  *free_set;
            UWORD8           purpose;
            UWORD8           i;

            // Rise transfert parameter semaphore to prevent L1S to use partial configuration.
            l1pa_l1ps_com.transfer.semaphore = TRUE;

            purpose = ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->purpose;

            // Get Ptr to the free dedicated parameter set.
            // All important fields are initialised.
            free_set = l1pa_get_free_transfer_set(purpose);

            // Fill Transfer mode generic parameters.
            free_set->assignment_id      = ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->assignment_id;
            free_set->assignment_command = purpose;
            free_set->allocated_tbf      = purpose;
            free_set->packet_ta          = ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->packet_ta;

            if((purpose == SINGLE_BLOCK_DL)||(purpose == SINGLE_BLOCK_UL))
            {
              free_set->packet_ta.ta_index = 255;
              free_set->packet_ta.ta_tn    = 255;
            }

            free_set->dl_pwr_ctl         = ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->dl_pwr_ctl;
            if (free_set->dl_pwr_ctl.p0 != 255)
              free_set->dl_pwr_ctl.p0    <<= 2;  // P0 format 7.1
            free_set->tsc                = ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->tsc;
            free_set->freq_param         = ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->freq_param;
            free_set->tbf_sti            = ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->tbf_sti;
            free_set->pc_meas_chan       = ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->pc_meas_chan;

            // Download access_burst_type
            l1pa_l1ps_com.transfer.psi_param.access_burst_type    = ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->access_burst_type;
            // Keep the same Pb factor
            l1pa_l1ps_com.transfer.psi_param.Scell_pb             = l1a_l1s_com.Scell_info.pb;
            // Enable PSI param updating in order to update access_burst_type in L1S
            l1pa_l1ps_com.transfer.psi_param.psi_param_update_cmd = TRUE;

            // Fill single block specific parameters.
            for(i=0;i<23;i++)
            {
              l1pa_l1ps_com.transfer.single_block.data_array[i] =
                ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->data_array[i];
            }
            l1pa_l1ps_com.transfer.single_block.tn =
              ((T_MPHP_SINGLE_BLOCK_REQ *)(msg->SigP))->timeslot_number;

            l1pa_l1ps_com.transfer.single_block.dl_tn_to_restore = l1a_l1s_com.dl_tn;

            // Fill "synchro_timeslot" which will be the frame synchro slot.
            free_set->ul_tbf_synchro_timeslot   = l1pa_l1ps_com.transfer.single_block.tn;
            free_set->transfer_synchro_timeslot = l1pa_l1ps_com.transfer.single_block.tn;

            // Step in state machine.
            *state = WAIT_MSG;

            // Store signalcode.
            free_set->SignalCode = MPHP_SINGLE_BLOCK_REQ;

            // Clear transfer parameter semaphore to let L1S use the new parameters.
            l1pa_l1ps_com.transfer.semaphore = FALSE;

            // end of process.
            end_process = 1;
          }
          break;

          case MPHP_ASSIGNMENT_REQ:
          // Assignement message.
          //---------------------
          {
            static int count =0;

            T_TRANSFER_SET  *free_set;
            UWORD8           assignment_command;
            UWORD8           timeslot_alloc;
            UWORD8           timeslot=0;

	// TBF_changes

            #if FF_TBF

              BOOL             pseudo_tbf_two_phase_acc;

              // Special case for two phase access (single or multi allocation):
              // It is handled as a pseudo UL TBF using a fixed allocation.
              // Still needs to be flagged to preempt TBF establishment switch in
              // transfer manager.
              if (((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->assignment_command == TWO_PHASE_ACCESS)
              {
                ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->assignment_command = UL_TBF;
                pseudo_tbf_two_phase_acc = TRUE;
              }
              else
                pseudo_tbf_two_phase_acc = FALSE;
            #endif
            count++ ;

            // Rise transfert parameter semaphore to prevent L1S to use partial configuration.
            l1pa_l1ps_com.transfer.semaphore = TRUE;

            assignment_command = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->assignment_command;

            // Get Ptr to the free dedicated parameter set.
            // All important fields are initialised.
            free_set = l1pa_get_free_transfer_set(assignment_command);

            // Download message containt.
            free_set->assignment_id      = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->assignment_id;
            free_set->assignment_command = assignment_command;
            free_set->multislot_class    = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->multislot_class;
            free_set->dl_pwr_ctl         = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->dl_pwr_ctl;
            if (free_set->dl_pwr_ctl.p0 != 255)
              free_set->dl_pwr_ctl.p0    <<= 2;  // P0 format 7.1
            free_set->packet_ta          = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->packet_ta;
            free_set->tsc                = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->tsc;
            free_set->freq_param         = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->freq_param;
            free_set->mac_mode           = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->mac_mode;
            free_set->tbf_sti            = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->tbf_sti;
            free_set->interf_meas_enable = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->interf_meas_enable;
            free_set->pc_meas_chan       = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->pc_meas_chan;

// TBF_changes

            #if FF_TBF
              // Two phase access condition is stored in FSET structure to be
              // transfered in ASET for transfer mode manager use.
              free_set->pseudo_tbf_two_phase_acc = pseudo_tbf_two_phase_acc;
            #endif

            // Download access_burst_type
            l1pa_l1ps_com.transfer.psi_param.access_burst_type    = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->access_burst_type;
            // Keep the same Pb factor
            l1pa_l1ps_com.transfer.psi_param.Scell_pb             = l1a_l1s_com.Scell_info.pb;
            // Enable PSI param updating in order to update access_burst_type in L1S
            l1pa_l1ps_com.transfer.psi_param.psi_param_update_cmd = TRUE;

            switch(assignment_command)
            {
              case DL_TBF:
              {
                free_set->dl_tbf_alloc = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->dl_ressource_alloc;

                if((free_set->allocated_tbf == UL_TBF) ||
                   (free_set->allocated_tbf == BOTH_TBF))
                  free_set->allocated_tbf = BOTH_TBF;
                else
                  free_set->allocated_tbf = DL_TBF;

                // Look for 1st allocated timeslot.
                // MSB=TS0...LSB=TS7
                timeslot_alloc = free_set->dl_tbf_alloc.timeslot_alloc;
                timeslot       = 0;
                while((timeslot<7) && !(timeslot_alloc & (0x80>>timeslot)))
                {
                  timeslot++;
                }

                // Fill "synchro_timeslot" which will be the frame synchro slot.
                free_set->dl_tbf_synchro_timeslot   = timeslot;
                free_set->transfer_synchro_timeslot = timeslot;
              }
              break;

              case UL_TBF:
              {
                *(free_set->ul_tbf_alloc) = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->ul_ressource_alloc;

                if((free_set->allocated_tbf == DL_TBF) ||
                   (free_set->allocated_tbf == BOTH_TBF))
                  free_set->allocated_tbf = BOTH_TBF;
                else
                  free_set->allocated_tbf = UL_TBF;

                // Look for 1st allocated timeslot.
                // MSB=TS0...LSB=TS7

                // Dynamic mode: the uplink PDCH are always monitored
                // The 1st allocated timeslot is a RX on the lowest numbered
                // timeslot allocated in uplink
              #if L1_EDA
                if((free_set->mac_mode == DYN_ALLOC) || (free_set->mac_mode == EXT_DYN_ALLOC))
              #else
                if(free_set->mac_mode == DYN_ALLOC)
              #endif
                {
                  timeslot_alloc = free_set->ul_tbf_alloc->timeslot_alloc;

                  timeslot = 0;
                  while((timeslot<7) && !(timeslot_alloc & (0x80>>timeslot)))
                  {
                    timeslot++;
                  }
                }
                else

                // Fixed mode: the 1st allocated timeslot is the downlink control
                // timeslot allocated by the network, which is a timeslot allocated
                // in uplink
                if(free_set->mac_mode == FIX_ALLOC_NO_HALF)
                {
                  timeslot = free_set->ul_tbf_alloc->fixed_alloc.ctrl_timeslot;
                }

                // Fill "synchro_timeslot" which will be the frame synchro slot.
                free_set->ul_tbf_synchro_timeslot   = timeslot;
                free_set->transfer_synchro_timeslot = timeslot;
              }
              break;

              case BOTH_TBF:
              {
                free_set->dl_tbf_alloc    = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->dl_ressource_alloc;
                *(free_set->ul_tbf_alloc) = ((T_MPHP_ASSIGNMENT_REQ *)(msg->SigP))->ul_ressource_alloc;
                free_set->allocated_tbf   = BOTH_TBF;

                // Process the downlink TBF first allocated timeslot
                timeslot_alloc = free_set->dl_tbf_alloc.timeslot_alloc;
                timeslot       = 0;

                while((timeslot<7) && !(timeslot_alloc & (0x80>>timeslot)))
                {
                  timeslot++;
                }

                free_set->dl_tbf_synchro_timeslot   = timeslot;

                // Process the uplink TBF first allocated timeslot

                // Dynamic mode: the uplink PDCH are always monitored
                // The 1st allocated timeslot is a RX on the lowest numbered
                // timeslot allocated in uplink
              #if L1_EDA
                if((free_set->mac_mode == DYN_ALLOC) || (free_set->mac_mode == EXT_DYN_ALLOC))
              #else
                if(free_set->mac_mode == DYN_ALLOC)
              #endif
                {
                  timeslot_alloc = free_set->ul_tbf_alloc->timeslot_alloc;

                  timeslot = 0;
                  while((timeslot<7) && !(timeslot_alloc & (0x80>>timeslot)))
                  {
                    timeslot++;
                  }
                }
                else

                // Fixed mode: the 1st allocated timeslot is the downlink control
                // timeslot allocated by the network, which is a timeslot allocated
                // in uplink
                if(free_set->mac_mode == FIX_ALLOC_NO_HALF)
                {
                  timeslot = free_set->ul_tbf_alloc->fixed_alloc.ctrl_timeslot;
                }

                free_set->ul_tbf_synchro_timeslot   = timeslot;

                // Fill "synchro_timeslot" which will be the frame synchro slot.
                if (free_set->dl_tbf_synchro_timeslot > free_set->ul_tbf_synchro_timeslot)
                {
                  free_set->transfer_synchro_timeslot = free_set->ul_tbf_synchro_timeslot;
                }
                else
                {
                  free_set->transfer_synchro_timeslot = free_set->dl_tbf_synchro_timeslot;
                }

              }
              break;
            }

            // Cancel any pending release on the assigned TBF
            if (l1pa_l1ps_com.transfer.tbf_release_param.tbf_release_cmd == TRUE)
            {
              // If pending released TBF = assigned TBF or assigned TBF = BOTH
              if ((assignment_command == l1pa_l1ps_com.transfer.tbf_release_param.released_tbf) ||
                  (assignment_command == BOTH_TBF))
              {
		// Send a "TBF Release confirmation" msg to L3  : CQ 46842
		l1pa_send_tbf_release_con(MPHP_TBF_RELEASE_CON,l1pa_l1ps_com.transfer.tbf_release_param.released_tbf);

                // Cancel the TBF release order
                l1pa_l1ps_com.transfer.tbf_release_param.tbf_release_cmd = FALSE;
                l1pa_l1ps_com.transfer.tbf_release_param.released_tbf    = NO_TBF;
              }
              // If BOTH TBF release order pending and no BOTH TBF assigned
              else if (l1pa_l1ps_com.transfer.tbf_release_param.released_tbf == BOTH_TBF)
              {
                // Keep the release of the TBF which is not assigned by this message
                if (assignment_command == DL_TBF)
                  l1pa_l1ps_com.transfer.tbf_release_param.released_tbf    = UL_TBF;
                else
                  l1pa_l1ps_com.transfer.tbf_release_param.released_tbf    = DL_TBF;
              }
            }

            // Step in state machine.
            *state = WAIT_MSG;

            // Store signalcode.
            free_set->SignalCode = MPHP_ASSIGNMENT_REQ;

            // Clear transfer parameter semaphore to let L1S use the new parameters.
            l1pa_l1ps_com.transfer.semaphore = FALSE;

            // end of process.
            end_process = 1;
          }
          break;

          case L1P_TRANSFER_DONE:
          // Switch to TRANSFER mode has been done. Send a Assignment confirmation
          // msg to L3.
          {
            l1pa_send_confirmation(MPHP_ASSIGNMENT_CON,
                                   ((T_L1P_TRANSFER_DONE *) msg->SigP)->assignment_id);

            // End of process.
            end_process = 1;
          }
          break;

          case MPHP_TBF_RELEASE_REQ:
          // TBF Release.
          //-------------
          {
            UWORD8 i;

            // Rise transfert parameter semaphore to prevent L1S to use partial configuration.
            l1pa_l1ps_com.transfer.semaphore = TRUE;

            // Cumulate with a possible TBF release request received during the same block period
            if (l1pa_l1ps_com.transfer.tbf_release_param.tbf_release_cmd == TRUE)
            {
              if (l1pa_l1ps_com.transfer.tbf_release_param.released_tbf != ((T_MPHP_TBF_RELEASE_REQ *)(msg->SigP))->tbf_type)
              {
                l1pa_l1ps_com.transfer.tbf_release_param.released_tbf = BOTH_TBF;
              }
            }
            else
            {
              // Enables the TBF release processing in L1S.
              l1pa_l1ps_com.transfer.tbf_release_param.tbf_release_cmd = TRUE;

              // Download msg info into L1PA_L1PS_COM.
              l1pa_l1ps_com.transfer.tbf_release_param.released_tbf = ((T_MPHP_TBF_RELEASE_REQ *)(msg->SigP))->tbf_type;
            }

            // Disable all pending TBFs those type is the same as the released TBF
            for(i = 0; i < 2; i++)
            {
              // Pending assignment
              if (l1pa_l1ps_com.transfer.fset[i]->SignalCode == MPHP_ASSIGNMENT_REQ)
              {
              #if !FF_TBF
                switch(l1pa_l1ps_com.transfer.tbf_release_param.released_tbf)
              #else
                UWORD8 released_tbf;

                // Special case if we got a request to release a two phase access TBF:
                // It is registered within FSET structure as an uplink TBF. If current
                // structure is pseudo TBF for two phase access, we process the request
                // like an uplink release, otherwise we skip it.

                released_tbf = l1pa_l1ps_com.transfer.tbf_release_param.released_tbf;

                if (released_tbf == TWO_PHASE_ACCESS)
                {
                  if (l1pa_l1ps_com.transfer.fset[i]->pseudo_tbf_two_phase_acc)
                    released_tbf = UL_TBF;
                  else
                    released_tbf = NO_TBF;
                }

                switch(released_tbf)
              #endif // FF_TBF
                {
                  case UL_TBF:
                  {
                    if (l1pa_l1ps_com.transfer.fset[i]->allocated_tbf == UL_TBF)
                    {
                      l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = NO_TBF;
                      l1pa_l1ps_com.transfer.fset[i]->SignalCode    = NULL;
                    }
                    if (l1pa_l1ps_com.transfer.fset[i]->allocated_tbf == BOTH_TBF)
                    {
                      l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = DL_TBF;
                    }
                  } break;

                  case DL_TBF:
                  {
                    if (l1pa_l1ps_com.transfer.fset[i]->allocated_tbf == DL_TBF)
                    {
                      l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = NO_TBF;
                      l1pa_l1ps_com.transfer.fset[i]->SignalCode    = NULL;
                    }
                    if (l1pa_l1ps_com.transfer.fset[i]->allocated_tbf == BOTH_TBF)
                    {
                      l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = UL_TBF;
                    }
                  } break;

                  case BOTH_TBF:
                  {
                    l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = NO_TBF;
                    l1pa_l1ps_com.transfer.fset[i]->SignalCode    = NULL;
                  } break;

                } // End of switch "tbf_release"
              } // End if SignalCode = MPHP_ASSIGNMENT_REQ or MPHP_REPEAT_UL_FIXED_ALLOC
            } // End "for"

            // Clear transfer parameter semaphore to let L1S use the new parameters.
            l1pa_l1ps_com.transfer.semaphore = FALSE;

            // end of process.
            end_process = 1;
          }
          break;

          case L1P_TBF_RELEASED:
          // TBF has been release by L1S. Send a "TBF Release confirmation"
          // msg to L3
          {
            // Send confirmation message to L3.
            l1pa_send_tbf_release_con(MPHP_TBF_RELEASE_CON,((T_L1P_TBF_RELEASED *)(msg->SigP))->tbf_type);

            // End of process.
            end_process = 1;
          }
          break;

          case MPHP_STOP_SINGLE_BLOCK_REQ:
          // Stop SINGLE block activity.
          //----------------------------
          {
            UWORD8 i;

            // Rise transfert parameter semaphore to prevent L1S to use partial configuration.
            l1pa_l1ps_com.transfer.semaphore = TRUE;

            // Disable SINGLE task.
            l1a_l1s_com.l1s_en_task[SINGLE] = TASK_DISABLED;

            // No more TBF...
            // Disable PDTCH task.
            l1a_l1s_com.l1s_en_task[PDTCH] = TASK_DISABLED;

            // Disable PTCCH task.
            l1a_l1s_com.l1s_en_task[PTCCH] = TASK_DISABLED;

            // Free the active set.
            l1pa_l1ps_com.transfer.aset->allocated_tbf = NO_TBF;

            // Send confirmation message to L3.
            l1a_send_confirmation(MPHP_STOP_SINGLE_BLOCK_CON,GRRM1_QUEUE);

            // disable all pending TBF those type is a single block
            for(i = 0; i < 2; i++)
            {
              // check for pending single block req (MPHP_SINGLE_BLOCK_REQ)
              if (l1pa_l1ps_com.transfer.fset[i]->SignalCode == MPHP_SINGLE_BLOCK_REQ)
              {
                // disable the fset corresponding to single blocks
                l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = NO_TBF;
                l1pa_l1ps_com.transfer.fset[i]->SignalCode    = NULL;
              }
            }

            // This process must be reset.
            *state = RESET;

            // end of process.
            end_process = 1;
          }
          break;

          case MPHP_PDCH_RELEASE_REQ:
          // PDCH Release.
          //--------------
          {
            // Rise transfert parameter semaphore to prevent L1S to use partial configuration.
            l1pa_l1ps_com.transfer.semaphore = TRUE;

            // Enables the PDCH release processing in L1S.
            l1pa_l1ps_com.transfer.pdch_release_param.pdch_release_cmd  = TRUE;

            // Download msg info into L1PA_L1PS_COM.
            l1pa_l1ps_com.transfer.pdch_release_param.timeslot_available = ((T_MPHP_PDCH_RELEASE_REQ *)(msg->SigP))->timeslot_available;
            l1pa_l1ps_com.transfer.pdch_release_param.assignment_id      = ((T_MPHP_PDCH_RELEASE_REQ *)(msg->SigP))->assignment_id;

            // Clear transfer parameter semaphore to let L1S use the new parameters.
            l1pa_l1ps_com.transfer.semaphore = FALSE;

            // end of process.
            end_process = 1;
          }
          break;

          case L1P_PDCH_RELEASED:
          // PDCHs have been release by L1S. Send a "PDCH Release confirmation"
          // msg to L3
          {
            // Send confirmation message to L3.
            l1pa_send_confirmation(MPHP_PDCH_RELEASE_CON,
                                   ((T_L1P_PDCH_RELEASE_CON *) msg->SigP)->assignment_id);

            // End of process.
            end_process = 1;
          }
          break;

          case MPHP_TIMING_ADVANCE_REQ:
          // TA configuration.
          //------------------
          {
            // Rise transfert parameter semaphore to prevent L1S to use partial configuration.
            l1pa_l1ps_com.transfer.semaphore = TRUE;

            // Enables the timing advance update in L1S.
            l1pa_l1ps_com.transfer.ptcch.ta_update_cmd = TRUE;

            // Download message content.
            l1pa_l1ps_com.transfer.ptcch.packet_ta     = ((T_MPHP_TIMING_ADVANCE_REQ *)(msg->SigP))->packet_ta;
            l1pa_l1ps_com.transfer.ptcch.assignment_id = ((T_MPHP_TIMING_ADVANCE_REQ *)(msg->SigP))->assignment_id;

            // Clear transfer parameter semaphore to let L1S use the new parameters.
            l1pa_l1ps_com.transfer.semaphore = FALSE;

            // end of process.
            end_process = 1;
          }
          break;

          case L1P_TA_CONFIG_DONE:
          // TA configuration done.
          //-----------------------
          {
            // Send confirmation message to L3.
            l1pa_send_confirmation(MPHP_TIMING_ADVANCE_CON,
                                   ((T_MPHP_TIMING_ADVANCE_CON *) msg->SigP)->assignment_id);

            // end of process.
            end_process = 1;
          }
          break;

          case MPHP_UPDATE_PSI_PARAM_REQ:
          // Update PSI Parameters.
          //----------------------
          {
            // Download msg content
            l1pa_l1ps_com.transfer.psi_param.Scell_pb          = ((T_MPHP_UPDATE_PSI_PARAM_REQ *)(msg->SigP))->pb << 2; // Pb format 7.1
            l1pa_l1ps_com.transfer.psi_param.access_burst_type = ((T_MPHP_UPDATE_PSI_PARAM_REQ *)(msg->SigP))->access_burst_type;

            l1pa_l1ps_com.transfer.psi_param.psi_param_update_cmd = TRUE;

            // send confirmation message
            l1a_send_confirmation(MPHP_UPDATE_PSI_PARAM_CON,GRRM1_QUEUE);

            // end of process.
            end_process = 1;
          }
          break;

          case L1P_PACCH_INFO:
          // Two Phase Access is ongoing: Packet Resource Request
          // msg has been sent to the network. CCCH reading must be
          // stopped to let PDCH reading going.
          //--------------------------------------------------------
          {
            // Forward result message to RR.
            l1a_send_result(MPHP_DATA_IND, msg, GRRM1_QUEUE);

            // end of process.
            end_process = 1;
          }
          break;

          case L1P_SINGLE_BLOCK_CON:
          // Two Phase Access is ongoing: Packet Resource Request
          // msg has been sent to the network. CCCH reading must be
          // stopped to let PDCH reading going.
          //--------------------------------------------------------
          {
            // Forward result message to RR.
            l1a_send_result(MPHP_SINGLE_BLOCK_CON, msg, GRRM1_QUEUE);

            // This process must be reset.
            *state = RESET;

            // end of process.
            end_process = 1;
          }
          break;

          case MPHP_REPEAT_UL_FIXED_ALLOC_REQ:
          // Repeat uplink fixed mode allocation bitmap
          //-------------------------------------------
          {
            // Rise transfert parameter semaphore to prevent L1S to use partial configuration.
            l1pa_l1ps_com.transfer.semaphore = TRUE;

            // If an UL TBF is running...
            if ((l1pa_l1ps_com.transfer.aset->allocated_tbf == UL_TBF) ||
                (l1pa_l1ps_com.transfer.aset->allocated_tbf == BOTH_TBF))
            {
              // Download info. from message
              l1pa_l1ps_com.transfer.repeat_alloc = *((T_MPHP_REPEAT_UL_FIXED_ALLOC_REQ *) msg->SigP);
            }

            // Send confirmation if this message was a repeat allocation cancelling
            if (!((T_MPHP_REPEAT_UL_FIXED_ALLOC_REQ *) msg->SigP)->repeat_allocation)
            {
              l1a_send_confirmation(MPHP_REPEAT_UL_FIXED_ALLOC_CON,GRRM1_QUEUE);
            }
            else
            {
              UWORD8 i;

              // Disable all pending UL TBF
              for(i = 0; i < 2; i++)
              {
                // Pending assignment
                if (l1pa_l1ps_com.transfer.fset[i]->SignalCode == MPHP_ASSIGNMENT_REQ)
                {
                  switch(l1pa_l1ps_com.transfer.fset[i]->allocated_tbf)
                  {
                    // Remove pending UL TBF
                    case UL_TBF:
                    {
                        l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = NO_TBF;
                        l1pa_l1ps_com.transfer.fset[i]->SignalCode    = NULL;
                    }
                    break;

                    // Change pending BOTH_TBF in pending DL_TBF
                    case BOTH_TBF:
                    {
                        l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = DL_TBF;
                    }
                    break;

                  } // End of switch "allocated_tbf"
                } // End if SignalCode = MPHP_ASSIGNMENT_REQ
              } // End "for"
            }

            // Clear transfer parameter semaphore to let L1S use the new parameters.
            l1pa_l1ps_com.transfer.semaphore = FALSE;

            // end of process.
            end_process = 1;
          }
          break;

          case L1P_REPEAT_ALLOC_DONE:
          {
            // Send confirmation message to L3.
            l1a_send_confirmation(MPHP_REPEAT_UL_FIXED_ALLOC_CON,GRRM1_QUEUE);

            // end of process.
            end_process = 1;
          }
          break;

          default:
          // End of process.
          //----------------
          {
            return;
          }
        } // end of switch(SignalCode)
      } // end of case WAIT_CONFIG.
    } // end of "switch".
  } // end of "while"
} // end of procedure.

/*-------------------------------------------------------*/
/* l1pa_serving_cell_pbcch_read_process()                */
/*-------------------------------------------------------*/
/* Description : This state machine handles Packet       */
/* serving cell PBCCH reading.                           */
/*                                                       */
/* Starting messages:        MPHP_SCELL_PBCCH_REQ        */
/* ------------------                                    */
/*                                                       */
/*  L1 continuously reads the serving cell PBCCH         */
/*  as requested by the scheduling info (PSI1 repeat     */
/*  period and relative position.                        */
/*                                                       */
/* Result messages (input):  L1C_PBCCHS_INFO             */
/* ------------------------                              */
/*  System information data block from L1S.              */
/*                                                       */
/* Reset messages (input):   MPHP_SCELL_PBCCH_STOP_REQ   */
/* -----------------------                               */
/*                                                       */
/*-------------------------------------------------------*/
void l1pa_serving_cell_pbcch_read_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET              = 0,
    WAIT_PBCCHS_CONFIG = 1,
    WAIT_PBCCHS_RESULT = 2,
    PBCCHS_CONFIG      = 3
  };

  UWORD8  *state      = &l1pa.state[SCPB];
  UWORD32  SignalCode = msg->SignalCode;

  #define  PbcchS  l1pa_l1ps_com.pbcchs

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_PBCCHS_CONFIG;

        // Reset PBCCHS process.
        l1a_l1s_com.l1s_en_task[PBCCHS] = TASK_DISABLED;  // Clear PBCCHS task enable flag.
      }
      break;

      case WAIT_PBCCHS_CONFIG:
      {
        // Request to read Normal BCCH from serving cell.
        if(SignalCode == MPHP_SCELL_PBCCH_REQ)
        {
          #define MAX_PSI1_PERIOD  16

          UWORD8  i;

          // Set semaphores for Serving Cell PBCCH reading task.
          l1a_l1s_com.task_param[PBCCHS] = SEMAPHORE_SET;

          // Download message content.
          //--------------------------
          PbcchS.nbr_psi                 = ((T_MPHP_SCELL_PBCCH_REQ *)(msg->SigP))->nbr_psi;
          PbcchS.bs_pbcch_blks           = ((T_MPHP_SCELL_PBCCH_REQ *)(msg->SigP))->bs_pbcch_blks;
          PbcchS.packet_chn_desc         = ((T_MPHP_SCELL_PBCCH_REQ *)(msg->SigP))->packet_chn_desc;
          PbcchS.frequency_list          = ((T_MPHP_SCELL_PBCCH_REQ *)(msg->SigP))->frequency_list;
          l1a_l1s_com.Scell_info.pb      = ((T_MPHP_SCELL_PBCCH_REQ *)(msg->SigP))->pb << 2;  // Pb format 7.1
          PbcchS.psi1_repeat_period      = ((T_MPHP_SCELL_PBCCH_REQ *)(msg->SigP))->psi1_repeat_period;

          // PBCCH Period is: MF52 * psi1_repeat_period
          PbcchS.pbcch_period = 52L * PbcchS.psi1_repeat_period;

          if(PbcchS.nbr_psi == 0)
          // Full PBCCH reading: Emulated throw "relative positions".
          //---------------------------------------------------------
          {
            // Emulate full PBCCH reading throw "relative positions" and a repeat period
            // of 1 MF52.
            //   bs_pbcch_blks= 0 -> Read B0
            //   bs_pbcch_blks= 1 -> Read B0,B6
            //   bs_pbcch_blks= 2 -> Read B0,B6,B3
            //   bs_pbcch_blks= 3 -> Read B0,B6,B3,B9

            PbcchS.nbr_psi      = PbcchS.bs_pbcch_blks+1;
            PbcchS.read_all_psi = TRUE;

            for(i=0;i<PbcchS.nbr_psi;i++)
            {
              ((T_MPHP_SCELL_PBCCH_REQ *)(msg->SigP))->relative_position_array[i] = i;
            }
          }

          else
          // PBCCH reading: use provided "relative positions".
          //--------------------------------------------------
          {
            PbcchS.read_all_psi = FALSE;
          }

          // Compute FN offset for each PSI.
          //--------------------------------
          for(i=0;i<l1pa_l1ps_com.pbcchs.nbr_psi;i++)
          {
            WORD8   nbr_mf52;          // Range 0..MAX_PSI1_PERIOD (can be negative along its estimation)
            UWORD8  nbr_rest;          // Range 0..3
            UWORD8  relative_position; // Range 0..4*MAX_PSI1_PERIOD
            UWORD8  psi_period;        // psi1_repeat_period = 1 if nbr_psi = 0

            relative_position = ((T_MPHP_SCELL_PBCCH_REQ *)(msg->SigP))->relative_position_array[i];

            nbr_mf52 = relative_position / (PbcchS.bs_pbcch_blks+1);
            nbr_rest = relative_position % (PbcchS.bs_pbcch_blks+1);

            // Block B0 is a special case since CTRL phase occurs during
            // the MF52 before.
            if(nbr_rest == 0)
            {
              nbr_mf52 -= 1;

              // Set psi_period to 1 when all PSI have to be read (nbr_psi = 0)
              if(PbcchS.read_all_psi)
                psi_period = 1;
              else
                psi_period = PbcchS.psi1_repeat_period;

              if(nbr_mf52 < 0)
                nbr_mf52 += psi_period;
            }

            PbcchS.offset_array[i]            = (nbr_mf52 * 52L) + PBCCH_POSITION[PbcchS.bs_pbcch_blks][nbr_rest];
            PbcchS.relative_position_array[i] = relative_position;
          }

          // Step in state machine.
          *state = PBCCHS_CONFIG;
        }

        // No action in this machine for other messages.
        else
        {
          // End of process.
          return;
        }
      }
      break;

      case PBCCHS_CONFIG:
      {
        WORD8   tn_pbcch;

        // If PBCCH TS is inferior to L1 synchronization TS, the PBCCH reading
        // control must be done one frame in advance
        if (PbcchS.packet_chn_desc.timeslot_no < l1a_l1s_com.dl_tn)
          PbcchS.control_offset = TRUE;
        else
          PbcchS.control_offset = FALSE;

        // Set "change_synchro" flag to trigger L1S to change the synchro on fly
        // within PBCCHS and to restore current synchro when PBCCHS task is completed.
        if(((PbcchS.packet_chn_desc.timeslot_no - l1a_l1s_com.dl_tn + 8) % 8) >=4)
        {
          // L1S will make a intra PBCCHS task synchro to current TS + 4.
          PbcchS.change_synchro = TRUE;
          tn_pbcch              = PbcchS.packet_chn_desc.timeslot_no - l1a_l1s_com.dl_tn - 4;
        }
        else
        {
          // L1S will NOT make the intra PBCCHS task synchro.
          PbcchS.change_synchro = FALSE;
          tn_pbcch              = PbcchS.packet_chn_desc.timeslot_no - l1a_l1s_com.dl_tn;
        }

        if(tn_pbcch < 0)
          PbcchS.tn_pbcch = tn_pbcch + 8;
        else
          PbcchS.tn_pbcch = tn_pbcch;

        // Enable PBCCHS task.
        l1a_l1s_com.l1s_en_task[PBCCHS] = TASK_ENABLED;


        // Step in state machine.
        *state = WAIT_PBCCHS_RESULT;

        // End of process.
        end_process = 1;
      }
      break;

      case WAIT_PBCCHS_RESULT:
      {
        if(SignalCode == L1P_PBCCHS_INFO)
        // Serving cell BCCH reading result.
        //----------------------------------
        {
          // Forward result message to L3.
          l1a_send_result(MPHP_DATA_IND, msg, GRRM1_QUEUE);

          // End of process.
          return;
        }

        else
        if(SignalCode == MPHP_SCELL_PBCCH_REQ)
        // Request to re-configure PBCCH reading.
        //--------------------------------------
        {
          // Step in state machine.
          *state = WAIT_PBCCHS_CONFIG;
        }

        else
        if(SignalCode == MPHP_SCELL_PBCCH_STOP_REQ)
        // Request to STOP any serving cell pbcch activity.
        //------------------------------------------------
        {
          // Send confirmation message to L3.
          l1a_send_confirmation(MPHP_SCELL_PBCCH_STOP_CON,GRRM1_QUEUE);

          // This process must be reset.
          *state = RESET;
        }

        else
        // End of packet transfer mode: test PDTCH to be sure that TBF downlink and uplink are released
        if((SignalCode == L1P_TBF_RELEASED) && (((T_L1P_TBF_RELEASED *)(msg->SigP))->released_all))
        {
           // This process must be reset.
          *state = RESET;
        }

        else
        if ((SignalCode == L1P_TRANSFER_DONE) || (SignalCode == L1P_TBF_RELEASED) ||           //change of Time Slot
            (SignalCode  == L1P_REPEAT_ALLOC_DONE) || (SignalCode == L1P_ALLOC_EXHAUST_DONE))
        {
          // Clear PBCCHS  task enable flag.
          l1a_l1s_com.l1s_en_task[PBCCHS] = TASK_DISABLED;

          // Set semaphores for Serving Cell PBCCH reading task.
          l1a_l1s_com.task_param[PBCCHS] = SEMAPHORE_SET;

          // l1a_l1s_com.dl_tn was changed. Check if a change synchro is needed
          *state = PBCCHS_CONFIG; // Step in state machine.
        }

        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          return;
        }
      }
      break;
    } // end of "switch".
  } // end of "while"
} // end of procedure.


/*-------------------------------------------------------*/
/* l1pa_neighbor_cell_pbcch_read_process()                */
/*-------------------------------------------------------*/
/* Description : This state machine handles Packet       */
/* neighbor cell PBCCH reading.                           */
/*                                                       */
/* Starting messages:        MPHP_NCELL_PBCCH_REQ        */
/* ------------------                                    */
/*                                                       */
/*  L1 continuously reads the neighbor cell PBCCH        */
/*  as requested by the scheduling info (PSI1 repeat     */
/*  period and relative position.                        */
/*                                                       */
/* Result messages (input):  L1C_PBCCHN_INFO             */
/* ------------------------                              */
/*  System information data block from L1S.              */
/*                                                       */
/* Reset messages (input):   MPHP_NCELL_PBCCH_STOP_REQ   */
/* -----------------------                               */
/*                                                       */
/*-------------------------------------------------------*/
void l1pa_neighbor_cell_pbcch_read_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET              = 0,
    WAIT_PBCCHN_CONFIG = 1,
    WAIT_PBCCHN_RESULT = 2,
    PBCCHN_CONFIG      = 3
  };

  UWORD8  *state      = &l1pa.state[NCPB];
  UWORD32  SignalCode = msg->SignalCode;

  #define  PbcchN  l1pa_l1ps_com.pbcchn

  static  WORD32  fn_offset_mem;
  static  WORD32  time_alignmt_mem;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_PBCCHN_CONFIG;

        // Reset PBCCHS process.
        l1a_l1s_com.l1s_en_task[PBCCHN_IDLE] = TASK_DISABLED;  // Clear PBCCHN task enable flag used in IDLE mode
        l1a_l1s_com.l1s_en_task[PBCCHN_TRAN] = TASK_DISABLED;  // Clear PBCCHN task enable flag used in Transfer Packet mode
      }
      break;

      case WAIT_PBCCHN_CONFIG:
      {
        // Request to read Normal PBCCH from neighbor cell.
        if(SignalCode == MPHP_NCELL_PBCCH_REQ)
        {
          #define MAX_PSI1_PERIOD  16

          // Set semaphores for Neighbor Cell PBCCH reading task.
          l1a_l1s_com.task_param[PBCCHN_IDLE] = SEMAPHORE_SET;
          l1a_l1s_com.task_param[PBCCHN_TRAN] = SEMAPHORE_SET;

          // Download message content.
          //--------------------------
          PbcchN.bcch_carrier       = ((T_MPHP_NCELL_PBCCH_REQ *)(msg->SigP))->bcch_carrier;
          PbcchN.bs_pbcch_blks      = ((T_MPHP_NCELL_PBCCH_REQ *)(msg->SigP))->bs_pbcch_blks;
          PbcchN.packet_chn_desc    = ((T_MPHP_NCELL_PBCCH_REQ *)(msg->SigP))->packet_chn_desc;
          PbcchN.frequency_list     = ((T_MPHP_NCELL_PBCCH_REQ *)(msg->SigP))->frequency_list;
          PbcchN.psi1_repeat_period = ((T_MPHP_NCELL_PBCCH_REQ *)(msg->SigP))->psi1_repeat_period;
          PbcchN.relative_position  = ((T_MPHP_NCELL_PBCCH_REQ *)(msg->SigP))->relative_position;
          fn_offset_mem             = ((T_MPHP_NCELL_PBCCH_REQ *)(msg->SigP))->fn_offset;
          time_alignmt_mem          = ((T_MPHP_NCELL_PBCCH_REQ *)(msg->SigP))->time_alignment;
          PbcchN.pb                 = ((T_MPHP_NCELL_PBCCH_REQ *)(msg->SigP))->pb  << 2; // Pb format 7.1

          // PBCCH Period is: MF52 * psi1_repeat_period
          PbcchN.pbcch_period = 52L * PbcchN.psi1_repeat_period;

          // Compute FN offset.
          //------------------
          {
            WORD8   nbr_mf52;          // Range 0..MAX_PSI1_PERIOD (can be negative along its estimation)
            UWORD8  nbr_rest;          // Range 0..3
            UWORD8  relative_position; // Range 0..4*MAX_PSI1_PERIOD

            relative_position = PbcchN.relative_position;

            // number of PBCCH blocs
            nbr_mf52 = relative_position / (PbcchN.bs_pbcch_blks+1);
            nbr_rest = relative_position % (PbcchN.bs_pbcch_blks+1);

            // Block B0 is a special case since CTRL phase occurs during the MF52 before.
            if(nbr_rest == 0)
            {
              nbr_mf52 -= 1;

              if(nbr_mf52 < 0)
                nbr_mf52 += PbcchN.psi1_repeat_period;
            }

            PbcchN.offset = (nbr_mf52 * 52L) + PBCCH_POSITION[PbcchN.bs_pbcch_blks][nbr_rest];

            // In case of idle mode
            if (l1a_l1s_com.mode != PACKET_TRANSFER_MODE)
            {
              PbcchN.offset -= 2 ;  // because of the 2 frames for the measurement windows
              // note: PbcchN.offset can not be negative (PbcchN.offset > 12).
            }
          }
          // Step in state machine.
          *state = PBCCHN_CONFIG;
        }

        // No action in this machine for other messages.
        else
        {
          // End of process.
          return;
        }
      }
      break;

      case PBCCHN_CONFIG:
      {
         //==================================================================================
         // choose a relative base time in the neighbor cell in order to simplify the L1S scheduling.
         //==================================================================================
         PbcchN.fn_offset    = fn_offset_mem;
         PbcchN.time_alignmt = time_alignmt_mem;

         //the new relative base time is set in order to have the Neighbor burst in position 0.
         //update with the burts position of the neighor cell     : PbcchN.packet_chn_desc.timeslot_no
         //update according to the current serving cell synchro   : l1a_l1s_com.dl_tn
         PbcchN.time_alignmt += (PbcchN.packet_chn_desc.timeslot_no - l1a_l1s_com.dl_tn ) * TN_WIDTH;

         // PbcchN.time_alignmt is in [-7TS..+16TS[.
         // more than 1 frame between the serving cell and the neighbor burst
         if ( PbcchN.time_alignmt >= 8*TN_WIDTH)
         {
           PbcchN.time_alignmt -= 8*TN_WIDTH;
           PbcchN.fn_offset --;
         }
         else
         if ( PbcchN.time_alignmt < 0)
         {
           PbcchN.time_alignmt += 8*TN_WIDTH;
           PbcchN.fn_offset ++;
         }

         // Set "change_synchro" flag to trigger L1S to change the synchro on fly
         // within PBCCHN and to restore current synchro when PBCCHN task is completed.
         if (PbcchN.time_alignmt >= 4 * TN_WIDTH)
         {
           PbcchN.time_alignmt -= 4 * TN_WIDTH;
           PbcchN.change_synchro = TRUE;
         }
         else
           PbcchN.change_synchro = FALSE;

         // In case of packet transfer mode
         if (l1a_l1s_com.mode == PACKET_TRANSFER_MODE)
         {
           // Enable Packet Transfer PBCCHN task
           l1a_l1s_com.l1s_en_task[PBCCHN_TRAN] = TASK_ENABLED;

         }
         // in case of Idle mode
         else
         {
           // Enable IDLE PBCCHN task
           l1a_l1s_com.l1s_en_task[PBCCHN_IDLE] = TASK_ENABLED;
         }

         // Step in state machine.
         *state = WAIT_PBCCHN_RESULT;

         // End of process.
         end_process = 1;
      }
      break;

      case WAIT_PBCCHN_RESULT:
      {
        if(SignalCode == L1P_PBCCHN_INFO)
        // Serving cell BCCH reading result.
        //----------------------------------
        {
          // Forward result message to L3.
         l1a_send_result(MPHP_NCELL_PBCCH_IND, msg, GRRM1_QUEUE);

          // This process must be reset.
          *state = RESET;
        }

        else
        if(SignalCode == MPHP_NCELL_PBCCH_REQ)
        // Request to re-configure PBCCH reading.
        //--------------------------------------
        {
          // Step in state machine.
          *state = WAIT_PBCCHN_CONFIG;
        }

        else
        if(SignalCode == MPHP_NCELL_PBCCH_STOP_REQ)
        // Request to STOP any serving cell pbcch activity.
        //------------------------------------------------
        {
          // Send confirmation message to L3.
          l1a_send_confirmation(MPHP_NCELL_PBCCH_STOP_CON,GRRM1_QUEUE);

          // This process must be reset.
          *state = RESET;
        }
        else
        // End of packet transfer mode: test PDTCH to be sure that TBF downlink and uplink are released
        if((SignalCode == L1P_TBF_RELEASED) && (((T_L1P_TBF_RELEASED *)(msg->SigP))->released_all))
        {
           // This process must be reset.
          *state = RESET;
        }
        else
        if ((SignalCode == L1P_TRANSFER_DONE) || (SignalCode == L1P_TBF_RELEASED) ||          //change of Time Slot
            (SignalCode  == L1P_REPEAT_ALLOC_DONE) || (SignalCode == L1P_ALLOC_EXHAUST_DONE))
        {
          // We consider only the case: packet Transfer => packet Transfer,the serving TS may be changed
          // For other cases such as Idle -> Transfer... decision not yet taken.

          // update the PBCCHN parameters
          // Clear PBCCHN_TRAN task disable flag.
          l1a_l1s_com.l1s_en_task[PBCCHN_TRAN] = TASK_DISABLED;

          // Set semaphores for Neighbor Cell PBCCH reading task.
          l1a_l1s_com.task_param[PBCCHN_TRAN] = SEMAPHORE_SET;

          // update the PBCCHN parameters
          *state = PBCCHN_CONFIG;// Step in state machine.
        }
        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          return;
        }
      }
      break;
    } // end of "switch".
  } // end of "while"
} // end of procedure.



/*-------------------------------------------------------*/
/* l1pa_tcr_meas_process()                               */
/*-------------------------------------------------------*/
/* Description : This state machine handles Neigbor cell */
/* measurement process in Packet Transfer mode.          */
/* Measurement are made on a specified frequency list.   */
/* The process is started automatically by L1 (on receipt*/
/* of a L1P_TRANSFER_DONE message from L1S) and doesn't  */
/* need to receive any message from L3.                  */
/* In order to update the frequency list, a              */
/* MHPC_TCR_MEAS_REQ msg will be sent from L3 to L1      */
/*                                                       */
/* Starting messages:        L1P_TRANSFER_DONE           */
/* ------------------                                    */
/*  L1 starts then measures on carriers specified in the */
/*  frequency list. Measures are performed on every      */
/*  frames with the occurence of 1 measure per frame     */
/*                                                       */
/* Subsequent messages:      MPHP_TCR_MEAS_REQ           */
/* --------------------                                  */
/*  The update is not done asap but postponed until the  */
/*  end of the reporting period. Frequency list is       */
/*  updated with the new list.                           */
/*                                                       */
/* Result messages (input):  L1P_TRANSFER_MEAS_DONE      */
/* ------------------------                              */
/*  This is the periodic reporting message sent by L1S.  */
/*  The reporting is done every "reporting period".      */
/*  The beguining of the reporting period is arbitrary   */
/*  and starts when the Neigh Meas task is enabled.      */
/*                                                       */
/* Result messages (output): MPHP_TCR_MEAS_IND           */
/* -------------------------                             */
/*  This is the periodic reporting message to L3.        */
/*                                                       */
/* Reset messages (input):   MPHP_TCR_MEAS_STOP_REQ      */
/* -----------------------                               */
/*  Frequency list neigbor cell measurement process in   */
/*  Packet Transfer mode is stopped by this message.     */
/*                                                       */
/*-------------------------------------------------------*/
void l1pa_tcr_meas_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET       = 0,
    WAIT_INIT   = 1,
    WAIT_RESULT = 3

  };

  UWORD8            *state      = &l1pa.state[TCR_MEAS];
  UWORD32           SignalCode  = msg->SignalCode;
  UWORD8            i;
  UWORD8            list_size;
  T_CRES_LIST_PARAM *free_list;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // step in state machine.
        *state = WAIT_INIT;

        // Reset TCR_MEAS process.
        l1pa_l1ps_com.l1ps_en_meas &= P_TCRMS_MEAS_MASK; // Disable Neighbour Measurement task.
      }
      break;

      case WAIT_INIT:
      {
        if(SignalCode == L1P_TRANSFER_DONE)
        // We enter in Packet Transfer mode.
        //---------------------------------
        {
        #if (CODE_VERSION != SIMULATION)
          //no meas when entering in Transfer if no BA list initialized
          //stay in this state and wait for a MPHP_TCR_MEAS_REQ from L3
          if((l1pa_l1ps_com.cres_freq_list.alist->nb_carrier == 0) &&
             (l1pa_l1ps_com.tcr_freq_list.new_list_present == FALSE) && (l1a_l1s_com.ba_list.nbr_carrier == 0))
            return;
        #endif
          // Set parameter synchro semaphore for P_TCRMS_MEAS task.
          l1pa_l1ps_com.meas_param |= P_TCRMS_MEAS;

          // Reset Neighbour Cell measurement parameters.
          l1pa_l1ps_com.tcr_freq_list.tcr_next_to_ctrl        = 0;
          l1pa_l1ps_com.tcr_freq_list.tcr_next_to_read        = 0;
          l1pa_l1ps_com.tcr_freq_list.last_stored_tcr_to_read = 0;
          l1pa_l1ps_com.tcr_freq_list.first_pass_flag  = TRUE;

          // Initialize counter used to report measurements
          l1pa_l1ps_com.tcr_freq_list.cres_meas_report = 0;


          // If no Packet Idle phase has been done and no BA(GPRS) list has been
          // downloaded, init BA(GPRS) list with BA list used in CS Idle mode.
          if((l1pa_l1ps_com.cres_freq_list.alist->nb_carrier == 0) &&
             (l1pa_l1ps_com.tcr_freq_list.new_list_present == FALSE))
          {
            // Get Ptr to the free Neighbour meas list.
            // The number of carriers in the list and the list
            // identification are initialized.
            free_list = l1pa_get_free_cres_list_set();

            // Download new list within T_CRES_LIST_PARAM structure.
            free_list->nb_carrier = l1a_l1s_com.ba_list.nbr_carrier;

            for(i = 0; i < free_list->nb_carrier; i++)
            {
              free_list->freq_list[i] = l1a_l1s_com.ba_list.A[i].radio_freq;
            }

            free_list->list_id = l1a_l1s_com.ba_list.ba_id;

            // Set "flist" with Circuit Swithed BA frequency list parameters
            l1pa_l1ps_com.cres_freq_list.alist = free_list;
          }

          // Reset flags.
          l1pa_l1ps_com.tcr_freq_list.ms_ctrl      = 0;
          l1pa_l1ps_com.tcr_freq_list.ms_ctrl_d    = 0;
          l1pa_l1ps_com.tcr_freq_list.ms_ctrl_dd   = 0;

          // Reset measures made on beacon frequency.
          l1pa_l1ps_com.tcr_freq_list.beacon_meas  = 0;

          // Enable Packet Transfer Neighbour Measurement task.
          l1pa.l1pa_en_meas[TCR_MEAS] |= P_TCRMS_MEAS;

          // step in state machine.
          *state = WAIT_RESULT;
        }
        else
        if(SignalCode == MPHC_RXLEV_PERIODIC_REQ)
        // We receive the BA list to be monitored in Idle mode
        //----------------------------------------------------
        {
          // When enter in Transfer the Idle list must be used
          // =>reset the packet Transfert list in order to use the Idle list
          l1pa_l1ps_com.cres_freq_list.alist->nb_carrier = 0;
          l1pa_l1ps_com.tcr_freq_list.new_list_present = FALSE;
        }
        else
        if(SignalCode == MPHP_TCR_MEAS_REQ)
        // Restart Packet Transfer measurement in TBF after
        // MPHP_TCR_MEAS_STOP_REQ
        // ### Check reason why was missing (s921_bis note)...
        //----------------------------------------------------
        {

          // Set parameter synchro semaphore for P_TCRMS_MEAS task.
          l1pa_l1ps_com.meas_param |= P_TCRMS_MEAS;

          free_list = l1pa_get_free_cres_list_set();


          // Download new list within T_CRES_LIST_PARAM structure.
          free_list->nb_carrier = ((T_MPHP_TCR_MEAS_REQ *)(msg->SigP))->nb_carrier;

          for(i = 0; i < free_list->nb_carrier; i++)
          {
            free_list->freq_list[i] = ((T_MPHP_TCR_MEAS_REQ *)(msg->SigP))->radio_freq_no[i];
          }

          free_list->list_id    = ((T_MPHP_TCR_MEAS_REQ *)(msg->SigP))->list_id;

          // Set "flist" with Circuit Swithed BA frequency list parameters
          l1pa_l1ps_com.cres_freq_list.alist = free_list;

          // Reset Neighbour Cell measurement parameters.
          l1pa_l1ps_com.tcr_freq_list.tcr_next_to_ctrl        = 0;
          l1pa_l1ps_com.tcr_freq_list.tcr_next_to_read        = 0;
          l1pa_l1ps_com.tcr_freq_list.last_stored_tcr_to_read = 0;
          l1pa_l1ps_com.tcr_freq_list.first_pass_flag  = TRUE;

          // Initialize counter used to report measurements
          l1pa_l1ps_com.tcr_freq_list.cres_meas_report = 0;

          // Reset flags.
          l1pa_l1ps_com.tcr_freq_list.ms_ctrl      = 0;
          l1pa_l1ps_com.tcr_freq_list.ms_ctrl_d    = 0;
          l1pa_l1ps_com.tcr_freq_list.ms_ctrl_dd   = 0;

          // Reset measures made on beacon frequency.
          l1pa_l1ps_com.tcr_freq_list.beacon_meas  = 0;

          // Enable Packet Transfer Neighbour Measurement task.
          l1pa.l1pa_en_meas[TCR_MEAS] |= P_TCRMS_MEAS;

          // step in state machine.
          *state = WAIT_RESULT;
        }

        // End of process.
        end_process = 1;
      }
      break;

      case WAIT_RESULT:
      {
        switch(SignalCode)
        {
          case L1P_TCR_MEAS_DONE:
          // One reporting period has been completed. A set of measures is forward to L3.
          //-----------------------------------------------------------------------------
          {
            // Forward result message to L3.
            l1a_send_result(MPHP_TCR_MEAS_IND, msg, GRRM1_QUEUE);

            // End of process.
            end_process = 1;
          }
          break;

          case MPHP_TCR_MEAS_REQ:
          // Update of the parameters are postponed until end of the reporting period.
          // Parameters are saved in a double buffer.
          {
            // Reset present flag to avoid to mix 2 updates in case of
            // an update already pending within "l1pa_l1ps_com.cres_freq_list.flist".
            l1pa_l1ps_com.tcr_freq_list.new_list_present = FALSE;

            // Get Ptr to the free Neighbour meas list.
            // The number of carriers in the list and the list
            // identification are initialized.
            free_list = l1pa_get_free_cres_list_set();


            // Download new list within T_CRES_LIST_PARAM structure.
            list_size             = ((T_MPHP_TCR_MEAS_REQ *)(msg->SigP))->nb_carrier;
            free_list->nb_carrier = list_size;

            for(i = 0; i < list_size; i++)
            {
              free_list->freq_list[i] = ((T_MPHP_TCR_MEAS_REQ *)(msg->SigP))->radio_freq_no[i];
            }

            free_list->list_id    = ((T_MPHP_TCR_MEAS_REQ *)(msg->SigP))->list_id;

            // Set "flist" with new set of frequency list parameter
            l1pa_l1ps_com.cres_freq_list.flist = free_list;

            // Set present flag only when the list has been downloaded.
            l1pa_l1ps_com.tcr_freq_list.new_list_present = TRUE;

            // End of process.
            end_process = 1;
          }
          break;

          case L1P_TBF_RELEASED:
          {
            // Test if all TBF have been released
            // Then stop Neighbour Measurement process
            if(((T_L1P_TBF_RELEASED *)(msg->SigP))->released_all)
            {
              // This process must be reset.
              *state = RESET;
            }
            else
            {
              return;
            }
          }
          break;

          case MPHP_TCR_MEAS_STOP_REQ:
          // Note: A TBF stop do not imply a stop of the Neighbour Measurement process.
          // L3 has to send a MPHP_TCR_MEAS_STOP message to stop the measurement process.
          {
            // send confirmation message
            l1a_send_confirmation(MPHP_TCR_MEAS_STOP_CON,GRRM1_QUEUE);

            // This process must be reset.
            *state = RESET;
          }
          break;

          default:
          // No action in this machine for other messages.
          // Note: No action is performed on receipt of a L1P_TRANSFER_DONE
          // message. However a SYNCHRO task is programmed, which implies
          // a reset of measures related to the Serving Cell (cf. L1S).
          //--------------------------------------------------------------
          {
            return;
          }
        } // end of switch(SignalCode)
      }
      break;
    } // end of "switch".
  } // end of "while"
} // end of procedure.

/*-------------------------------------------------------*/
/* l1pa_idle_interference_meas_process()                 */
/*-------------------------------------------------------*/
/* Description :                                         */
/*                                                       */
/* Starting messages:        MPHP_INT_MEAS_REQ           */
/*                                                       */
/* This message requests signal strength measurements    */
/* on several channels of a specific carrier.            */
/* Measurements must be done on one search frame and one */
/* PTCCH frame.                                          */
/*                                                       */
/* Result messages (input):  L1PS_ITMEAS_IND             */
/*                                                       */
/* This message is reported to L1A when signal strength  */
/* has been measured on one idle frame (PTCCH or search) */
/*                                                       */
/* Result messages (output): MPHP_INT_MEAS_IND           */
/*                                                       */
/* This message is reported to L3 when measurements have */
/* been done on two contiguous idle frames               */
/*                                                       */
/* Reset message (input):    MPHP_INT_MEAS_STOP_REQ      */
/*                                                       */
/* Interference measurement processing is stopped by     */
/* this message                                          */
/*-------------------------------------------------------*/
void l1pa_idle_interference_meas_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET             = 0,
    WAIT_INIT         = 1,
    WAIT_1ST_RESULT   = 2,
    WAIT_2ND_RESULT   = 3
  };

  static   T_L1A_INT_MEAS_PARAM int_meas_param;
  UWORD8   *state      = &l1pa.state[PI_INT_MEAS];
  UWORD32  SignalCode  = msg->SignalCode;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        // Reset ITMEAS process.
        l1a_l1s_com.l1s_en_task[ITMEAS]    = TASK_DISABLED;  // Clear ITMEAS task enable flag.
      }
      break;

      case WAIT_INIT:
      {
        // Interference measurement request
        //---------------------------------
        if (SignalCode == MPHP_INT_MEAS_REQ)
        {
          UWORD8 bitmap,i;

          // Set semaphore
          l1a_l1s_com.task_param[ITMEAS] = SEMAPHORE_SET;

          // Download message content
          l1pa_l1ps_com.itmeas.packet_intm_freq_param  = ((T_MPHP_INT_MEAS_REQ *)(msg->SigP))->packet_intm_freq_param;
          l1pa_l1ps_com.itmeas.multislot_class         = ((T_MPHP_INT_MEAS_REQ *)(msg->SigP))->multislot_class;
          int_meas_param.id                           = ((T_MPHP_INT_MEAS_REQ *)(msg->SigP))->carrier_id;

          // Processing of the 2 possible measurement bitmaps
          //-------------------------------------------------

          // 1- Without Rx on the frame before

          l1pa_l1ps_com.itmeas.idle_tn_no_rx = ((T_MPHP_INT_MEAS_REQ *)(msg->SigP))->tn;

          // Trb respect after measurements
          // We consider that the timeslot on which the Layer 1 is synchronized is
          // always allocated on the frame after the idle frame.
          // For the Trb multi-slot class parameter respect, we must clear the bits at
          // the right of the interference measurement bitmap
          l1pa_l1ps_com.itmeas.idle_tn_no_rx &= (UWORD8)
                           ~(   0xFF
                             >> (  8 + l1a_l1s_com.dl_tn
                                 - MS_CLASS[l1pa_l1ps_com.itmeas.multislot_class].trb));

          // 2- With a Rx programmed on the frame before
          // Note: This Rx is always on the dl_tn

          l1pa_l1ps_com.itmeas.idle_tn_rx = l1pa_l1ps_com.itmeas.idle_tn_no_rx;

          // Trb respect before measurements
          // The timeslot on which the Layer 1 is synchronized is allocated on the frame
          // before the idle frame.
          // For the Trb multi-slot class parameter respect, we must clear the bits at
          // the left of the interference measurement bitmap
          bitmap = 0x80;

          i = 8 - l1a_l1s_com.dl_tn - MS_CLASS[l1pa_l1ps_com.itmeas.multislot_class].trb;
          if (i > 8)
            bitmap >>= (-i);
          else
            bitmap <<= i;

          for (i = 1; i<= MS_CLASS[l1pa_l1ps_com.itmeas.multislot_class].trb; i++)
          {
            l1pa_l1ps_com.itmeas.idle_tn_rx &= (UWORD8) ~bitmap;
            bitmap <<= 1;
          }

          // Initialize parameters
          l1pa_l1ps_com.itmeas.position        = ANY_IDLE_FRAME;  // First measurement on any idle frame

          // Enable synchronous task
          l1a_l1s_com.l1s_en_task[ITMEAS]      = TASK_ENABLED;

          // Step in state machine
          *state = WAIT_1ST_RESULT;

          // End of process
          end_process = 1;
        }

        // No action in this machine for other messages.
        else
        {
          // End of process.
          end_process = 1;
        }
      }
      break;

      case WAIT_1ST_RESULT:
      {
        // Reporting of 1st measurement session
        //-------------------------------------
        if (SignalCode == L1P_ITMEAS_IND)
        {
          UWORD8  i;

          // Set semaphore
          l1a_l1s_com.task_param[ITMEAS] = SEMAPHORE_SET;

          // Save interference measurements
          for(i=0; i<8; i++)
            int_meas_param.rxlev[i]  = ((T_L1P_ITMEAS_IND *)(msg->SigP))->rxlev[i];

          // Save bitmap
          int_meas_param.meas_bitmap = ((T_L1P_ITMEAS_IND *)(msg->SigP))->meas_bitmap;

           // Save reported fn
          int_meas_param.fn          = ((T_L1P_ITMEAS_IND *)(msg->SigP))->fn;

          // Position = complement of reported position
          if (((T_L1P_ITMEAS_IND *)(msg->SigP))->position == PTCCH_FRAME)
            l1pa_l1ps_com.itmeas.position = SEARCH_FRAME;
          else
            l1pa_l1ps_com.itmeas.position = PTCCH_FRAME;

            // Enable ITMEAS
            l1a_l1s_com.l1s_en_task[ITMEAS] = TASK_ENABLED;

          // Step in state machine
          *state = WAIT_2ND_RESULT;

          // End of process
          return;
        }

          else
        if (SignalCode == MPHP_INT_MEAS_STOP_REQ)
          {
          // Send confirmation
          l1a_send_confirmation(MPHP_INT_MEAS_STOP_CON,GRRM1_QUEUE);

          // Reset process
          *state = RESET;
        }

        // No action in this machine for other messages.
        else
        {
          // End of process
          return;
        }
      }
      break;

      case WAIT_2ND_RESULT:
      {
        // Reporting subsequent measurement session
        //-----------------------------------------
        if (SignalCode == L1P_ITMEAS_IND)
        {
          // At least one measurement session has already been reported
          //-----------------------------------------------------------
          UWORD32 reported_fn;

          // Check fn
          // The two measurement sessions must be done in two contiguous idle frames

          // Modulo
          if (((T_L1P_ITMEAS_IND *)(msg->SigP))->fn < int_meas_param.fn)
          {
            reported_fn = ((T_L1P_ITMEAS_IND *)(msg->SigP))->fn + MAX_FN;
          }
          else
          {
            reported_fn = ((T_L1P_ITMEAS_IND *)(msg->SigP))->fn;
          }

          // The two last measurement sessions are enough close
          if ((reported_fn - int_meas_param.fn) == 13)
          {
            // Build and send result msg to L3.
            l1pa_send_int_meas_report(MPHP_INT_MEAS_IND,
                                      ((T_L1P_ITMEAS_IND *)(msg->SigP)),
                                      &int_meas_param);

            // Only one measurement session per request
            *state = RESET;
          }

          // The two last measurement sessions aren't enough close
          else
          {
            // 1st measurement result is no more valid, second result
            // must replace it: this is achieved by WAIT_1ST_RESULT state!!!

            // Step in state machine
            *state = WAIT_1ST_RESULT;
          }
        }

        else
        if (SignalCode == MPHP_INT_MEAS_STOP_REQ)
        {
          // Send confirmation
          l1a_send_confirmation(MPHP_INT_MEAS_STOP_CON,GRRM1_QUEUE);

          // Reset process
          *state = RESET;
        }

        // No action in this machine for other messages.
        else
        {
          // End of process
          return;
        }
      }
      break;

    } // End of "switch"
  } // End of "while"
} // End of "procedure"

/*-------------------------------------------------------*/
/* l1pa_transfer_interference_meas_process()             */
/*-------------------------------------------------------*/
/* Description :                                         */
/*                                                       */
/* Starting messages:        L1P_TRANFSER_DONE           */
/*                                                       */
/* Interference measurement processing starts each time  */
/* a new starting time occurs if the interference        */
/* measurements are enabled                              */
/* Measurements must be done on one search frame and one */
/* PTCCH frame.                                          */
/*                                                       */
/* Result messages (input):  L1PS_ITMEAS_IND             */
/*                                                       */
/* This message is reported to L1A when signal strength  */
/* has been measured on one idle frame (PTCCH or search) */
/*                                                       */
/* Result messages (output): MPHP_TINT_MEAS_IND          */
/*                                                       */
/* This message is reported to L3 when measurements have */
/* been done on two idle frames as close as possible     */
/*                                                       */
/* Reset message (input):    L1P_TBF_RELEASED            */
/*                                                       */
/* Interference measurement processing is stopped when   */
/* all TBF are released                                  */
/*-------------------------------------------------------*/
void l1pa_transfer_interference_meas_process(xSignalHeaderRec *msg)
{
  /* Bitmaps used for the processing of full_allocation */
  /*----------------------------------------------------*/
  const UWORD8 FULL_ALLOCATION[9]=
{
    0x00,
    0x80,
    0xc0,
    0xe0,
    0xf0,
    0xf8,
    0xfc,
    0xfe,
    0xff
  };

  enum states
  {
    RESET             = 0,
    WAIT_INIT         = 1,
    CONFIG            = 2,
    WAIT_1ST_RESULT   = 3,
    WAIT_2ND_RESULT   = 4
  };

  static   T_L1A_INT_MEAS_PARAM int_meas_param;
  UWORD8   *state      = &l1pa.state[PT_INT_MEAS];
  UWORD32  SignalCode  = msg->SignalCode;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        // Reset ITMEAS process.
        l1a_l1s_com.l1s_en_task[ITMEAS]    = TASK_DISABLED;  // Clear ITMEAS task enable flag.
      }
      break;

      case WAIT_INIT:
      {
        // New channel assignment
        //-----------------------
        if ((SignalCode == L1P_TRANSFER_DONE) || (SignalCode == L1P_REPEAT_ALLOC_DONE) ||
            (SignalCode == L1P_ALLOC_EXHAUST_DONE))
        {
          *state = CONFIG;
        }

        // No action in this machine for other messages.
        else
        {
          // End of process.
          return;
        }
      }
      break;

      case CONFIG:
      {
        // Rise transfert parameter semaphore to prevent L1S to use partial configuration.
        l1pa_l1ps_com.transfer.semaphore = TRUE;

        // If the interference measurements are disabled
        if (l1pa_l1ps_com.transfer.aset->interf_meas_enable == FALSE)
        {
          *state = WAIT_INIT;
        }
        else
        {
          // Set semaphore
          l1a_l1s_com.task_param[ITMEAS]   = SEMAPHORE_SET;

          // Initialize parameters
          l1pa_l1ps_com.itmeas.position    = ANY_IDLE_FRAME;  // First measurement on any idle frame

          // Save assignment ID for the interference measurements reporting message
          int_meas_param.id = l1pa_l1ps_com.transfer.aset->assignment_id;

          // Processing of the measurement bitmap
          l1pa_l1ps_com.itmeas.meas_bitmap = (UWORD8)
               FULL_ALLOCATION[MS_CLASS[l1pa_l1ps_com.transfer.aset->multislot_class].rx] >>
               l1a_l1s_com.dl_tn;

          // Enable synchronous task
          l1a_l1s_com.l1s_en_task[ITMEAS]  = TASK_ENABLED;

          // Step in state machine
          *state = WAIT_1ST_RESULT;
        }

        // Clear transfer parameter semaphore to let L1S use the new parameters.
        l1pa_l1ps_com.transfer.semaphore = FALSE;

        // End of process.
        end_process = 1;
      }
      break;

      case WAIT_1ST_RESULT:
      {
        // Reporting of 1st measurement session
        //-------------------------------------
        if (SignalCode == L1P_ITMEAS_IND)
        {
          UWORD8  i;

          // Set semaphore
          l1a_l1s_com.task_param[ITMEAS] = SEMAPHORE_SET;

          // Save interference measurements
          for(i=0; i<8; i++)
            int_meas_param.rxlev[i]  = ((T_L1P_ITMEAS_IND *)(msg->SigP))->rxlev[i];

          // Save bitmap
          int_meas_param.meas_bitmap = ((T_L1P_ITMEAS_IND *)(msg->SigP))->meas_bitmap;

            // Save reported fn
          int_meas_param.fn          = ((T_L1P_ITMEAS_IND *)(msg->SigP))->fn;

          // Position = complement of reported position
          if (((T_L1P_ITMEAS_IND *)(msg->SigP))->position == PTCCH_FRAME)
            l1pa_l1ps_com.itmeas.position = SEARCH_FRAME;
          else
            l1pa_l1ps_com.itmeas.position = PTCCH_FRAME;

            // Enable ITMEAS
            l1a_l1s_com.l1s_en_task[ITMEAS] = TASK_ENABLED;

          // Step in state machine
          *state = WAIT_2ND_RESULT;

            // End of process
            return;
        }

        else
        if (SignalCode == L1P_TRANSFER_DONE)
        // The starting time of a new TBF occurs
        {
          // Reset process
          *state = RESET;
        }

        else
        if (SignalCode == L1P_TBF_RELEASED)
        // A TBF has been released
        {
          // No remaining TBF
          if(((T_L1P_TBF_RELEASED *)(msg->SigP))->released_all)
          {
            // Reset process
            *state = RESET;
          }
          else
          {
            // Enable a new measurement session
            *state = CONFIG;
          }
        }

        else
        if (SignalCode == L1P_PDCH_RELEASED)
        // PDCH have been released
        {
          // Enable a new measurement session
          *state = CONFIG;
        }

        // No action in this machine for other messages.
          else
          {
          // End of process
          return;
        }
      }
      break;

      case WAIT_2ND_RESULT:
      {
        // Reporting subsequent measurement session
        //-----------------------------------------
        if (SignalCode == L1P_ITMEAS_IND)
        {
          // At least one measurement session has already been reported
          //-----------------------------------------------------------
          UWORD32 reported_fn;

          // Check fn
          // The two measurement sessions must be done in two contiguous idle frames

          // Modulo
          if (((T_L1P_ITMEAS_IND *)(msg->SigP))->fn < int_meas_param.fn)
          {
            reported_fn = ((T_L1P_ITMEAS_IND *)(msg->SigP))->fn + MAX_FN;
          }
          else
          {
            reported_fn = ((T_L1P_ITMEAS_IND *)(msg->SigP))->fn;
          }

            // The two last measurement sessions are enough close
          if ((reported_fn - int_meas_param.fn) <= 104)
          {
            // Build and send result msg to L3.
            l1pa_send_int_meas_report(MPHP_TINT_MEAS_IND,
                                      ((T_L1P_ITMEAS_IND *)(msg->SigP)),
                                      &int_meas_param);

              // Enable a new measurement session
              *state = CONFIG;
          }

          // The two last measurement sessions aren't enough close
          else
          {
            // 1st measurement result is no more valid, second result
            // must replace it: this is achieved by WAIT_1ST_RESULT state!!!

            // Step in state machine
            *state = WAIT_1ST_RESULT;
          }
        }

        else

        // New channel assignment
        //-----------------------
        if ((SignalCode == L1P_TRANSFER_DONE) || (SignalCode == L1P_REPEAT_ALLOC_DONE) ||
            (SignalCode == L1P_ALLOC_EXHAUST_DONE))
        {
          // Reset process
          *state = RESET;
        }

        else
        if (SignalCode == L1P_TBF_RELEASED)
        // A TBF has been released
        {
          // No remaining TBF
          if(((T_L1P_TBF_RELEASED *)(msg->SigP))->released_all)
          {
            // Reset process
            *state = RESET;
          }
          else
          {
            // Enable a new measurement session
            *state = CONFIG;
          }
        }

        else
        if (SignalCode == L1P_PDCH_RELEASED)
        // PDCH have been released
        {
          // Enable a new measurement session
          *state = CONFIG;
        }

        // No action in this machine for other messages.
        else
        {
          // End of process
          return;
        }
      }
      break;

    } // End of "switch"
  } // End of "while"
} // End of "procedure"
//#pragma DUPLICATE_FOR_INTERNAL_RAM_START
#endif
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END
