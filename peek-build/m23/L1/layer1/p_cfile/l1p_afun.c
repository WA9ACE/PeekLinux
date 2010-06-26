/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1P_AFUN.C
 *
 *        Filename l1p_afun.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#define  L1P_AFUN_C

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
  #include "l1_varex.h"
  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"

  #include "l1_msgty.h"
  #include "l1_proto.h"

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
  #include "l1_varex.h"
  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"

  #include "l1_msgty.h"
  #include "l1_proto.h"

#endif

// Internal prototypes
T_TRANSFER_SET    *l1pa_get_free_transfer_set (UWORD8  new_tbf);
int                l1pa_sort                  (const void *a, const void *b);
void               l1pa_reset_cr_freq_list    ();
T_CRES_LIST_PARAM *l1pa_get_free_cres_list_set();
void               l1pa_send_int_meas_report  (UWORD32               SignalCode, 
                                               T_L1P_ITMEAS_IND     *last_l1s_msg, 
                                               T_L1A_INT_MEAS_PARAM *first_meas_ptr);
void               l1pa_send_confirmation     (UWORD32 SignalCode, UWORD8 id);

/*-------------------------------------------------------*/
/* l1pa_get_free_transfer_set()                          */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/*                                                       */
/*-------------------------------------------------------*/
T_TRANSFER_SET  *l1pa_get_free_transfer_set(UWORD8  new_tbf)
{
  T_TRANSFER_SET  *transfer_set;

  #define clear_and_new(i, tbf) \
    switch(tbf) \
    { \
      case UL_TBF: \
        l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = DL_TBF; \
      break; \
      \
      case DL_TBF: \
        l1pa_l1ps_com.transfer.fset[i]->allocated_tbf = UL_TBF; \
      break; \
    }
  
  // If new_tbf is to replace an existing one, we must return
  // the considered free set.
  if(new_tbf == BOTH_TBF)
  {
    if(l1pa_l1ps_com.transfer.fset[0]->allocated_tbf == NO_TBF)
      transfer_set = l1pa_l1ps_com.transfer.fset[1];
    else
      transfer_set = l1pa_l1ps_com.transfer.fset[0];
  }
  else
  if(l1pa_l1ps_com.transfer.fset[0]->allocated_tbf == new_tbf)
    transfer_set = l1pa_l1ps_com.transfer.fset[0];
  else
  if(l1pa_l1ps_com.transfer.fset[1]->allocated_tbf == new_tbf)
    transfer_set = l1pa_l1ps_com.transfer.fset[1];
  else
  if(l1pa_l1ps_com.transfer.fset[0]->allocated_tbf == BOTH_TBF)
  {
    clear_and_new(0, new_tbf);
    transfer_set = l1pa_l1ps_com.transfer.fset[1];
  }
  else
  if(l1pa_l1ps_com.transfer.fset[1]->allocated_tbf == BOTH_TBF)
  {
    clear_and_new(1, new_tbf);
    transfer_set = l1pa_l1ps_com.transfer.fset[0];
  }
  else
    if(l1pa_l1ps_com.transfer.fset[0]->allocated_tbf == NO_TBF)
      transfer_set = l1pa_l1ps_com.transfer.fset[0];
    else
      transfer_set = l1pa_l1ps_com.transfer.fset[1];

  transfer_set->SignalCode                = NULL;
  transfer_set->dl_tbf_synchro_timeslot   = 0;
  transfer_set->ul_tbf_synchro_timeslot   = 0;
  transfer_set->transfer_synchro_timeslot = 0;

  // No init of "allocated_tbf"

  transfer_set->assignment_command = NO_TBF;
  transfer_set->multislot_class    = 0;
  transfer_set->packet_ta.ta       = 255;     // Not present
  transfer_set->packet_ta.ta_index = 255;     // Not present
  transfer_set->packet_ta.ta_tn    = 255;     // Not present
  transfer_set->tsc                = 0;

  transfer_set->freq_param.chan_sel.h                          = 0;
  transfer_set->freq_param.chan_sel.rf_channel.single_rf.radio_freq = 0;
  transfer_set->freq_param.chan_sel.rf_channel.hopping_rf.maio = 0;
  transfer_set->freq_param.chan_sel.rf_channel.hopping_rf.hsn  = 0;
  transfer_set->freq_param.freq_list.rf_chan_cnt               = 0;

  transfer_set->tbf_sti.present = FALSE;

  transfer_set->dl_pwr_ctl.p0                 = 255;
  transfer_set->dl_pwr_ctl.bts_pwr_ctl_mode   = 0;
  transfer_set->dl_pwr_ctl.pr_mode            = 0;

  transfer_set->ts_override                   = 0;
  transfer_set->pc_meas_chan                  = 0;
       
  // No init of "ul_tbf_alloc" and "ul_tbf_alloc", mac_mode is enought
  
  return(transfer_set);
}

/*-------------------------------------------------------*/
/* l1pa_sort()                                           */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/*                                                       */
/*-------------------------------------------------------*/
int l1pa_sort(const void *a, const void *b)
{
  UWORD8 entry1, entry2;

  entry1 = *((UWORD8*) a);
  entry2 = *((UWORD8*) b);

  if (entry1 < entry2 )
    return(-1);
  else if (entry1 == entry2)
         return(0);
       else 
         return(1);
}

/*-------------------------------------------------------*/
/* l1pa_reset_cr_freq_list()                             */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* This function resets parameters used in Frequency     */
/* list task                                             */
/*                                                       */
/*-------------------------------------------------------*/
void l1pa_reset_cr_freq_list()
{
  UWORD8 i;
 
  for(i=0; i<NB_MEAS_MAX_GPRS; i++)       
  {
    l1pa_l1ps_com.cr_freq_list.used_il_lna   [i].il = l1_config.params.il_min; // IL used in CTRL phase for AGC setting.  
    l1pa_l1ps_com.cr_freq_list.used_il_lna_d [i].il = l1_config.params.il_min; // ... 1 frame delay.
    l1pa_l1ps_com.cr_freq_list.used_il_lna_dd[i].il = l1_config.params.il_min; // ... 2 frames delay, used in READ phase.

    l1pa_l1ps_com.cr_freq_list.used_il_lna   [i].lna = FALSE; // LNA used in CTRL phase for AGC setting.  
    l1pa_l1ps_com.cr_freq_list.used_il_lna_d [i].lna = FALSE; // ... 1 frame delay.
    l1pa_l1ps_com.cr_freq_list.used_il_lna_dd[i].lna = FALSE; // ... 2 frames delay, used in READ phase.
  }
  
  // Initialize next power measurement control.
  l1pa_l1ps_com.cr_freq_list.next_to_ctrl = 0;
  
  // Initialize next power measurement result.
  l1pa_l1ps_com.cr_freq_list.next_to_read = 0; 

  // Reset PPCH burst ctrl indication
  l1pa_l1ps_com.cr_freq_list.pnp_ctrl = 0;
          
} 

/*-------------------------------------------------------*/
/* l1pa_get_free_cres_list_set()                         */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
T_CRES_LIST_PARAM *l1pa_get_free_cres_list_set()
{
  T_CRES_LIST_PARAM *flist;
  
  // Get free set pointer.
  if(l1pa_l1ps_com.cres_freq_list.alist == &(l1pa_l1ps_com.cres_freq_list.list[0]))
    flist = &(l1pa_l1ps_com.cres_freq_list.list[1]);
  else
    flist = &(l1pa_l1ps_com.cres_freq_list.list[0]);
     
  // Clear free set.
  flist->nb_carrier = 0;
  flist->list_id    = 0;

  // Reset fifo used in AGC algo => NEED MORE INVESTIGATION.


  return(flist);
}

/*-------------------------------------------------------*/
/* l1pa_send_int_meas_report()                           */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* This function builds and sends the Interf. Meas.      */
/* report to L3                                          */
/*                                                       */
/*-------------------------------------------------------*/
void l1pa_send_int_meas_report(UWORD32               SignalCode, 
                               T_L1P_ITMEAS_IND     *last_l1s_msg, 
                               T_L1A_INT_MEAS_PARAM *first_meas_ptr)
{
  xSignalHeaderRec *msg;
  UWORD8            i,j,k;
  UWORD8            bit_mask = 0x80;

  // Allocate report msg and fill signal code.
  msg = os_alloc_sig(sizeof(T_MPHP_INT_MEAS_IND));
  DEBUGMSG(status,NU_ALLOC_ERR)         
  msg->SignalCode = SignalCode;

  // Save measurement results
  //-------------------------
  for(i = 0, j = 0, k = 0; i < 8; i++, bit_mask >>= 1)
  {
    // Fill 1st measurement report
    //----------------------------
    // If a measurement was done on timeslot i
    if(first_meas_ptr->meas_bitmap & bit_mask)
    {
      // The result is stored in the msg. 
      ((T_MPHP_INT_MEAS_IND *) (msg->SigP))->int_meas[i].rxlev[0] = first_meas_ptr->rxlev[j];

      // Next measurement value
      j++;
    }
    else
    {
      // No measurement processed on timeslot i (0x80 value)
      ((T_MPHP_INT_MEAS_IND *) (msg->SigP))->int_meas[i].rxlev[0] = (WORD8)NO_MEAS; //omaps00090550
    }
    
    // Fill 2nd measurement report
    //----------------------------
    // If a measurement was done on timeslot i
    if(last_l1s_msg->meas_bitmap & bit_mask)
    {
      // The result is stored in the msg. 
      ((T_MPHP_INT_MEAS_IND *) (msg->SigP))->int_meas[i].rxlev[1] = last_l1s_msg->rxlev[k];

      // Next measurement value
      k++;
    }
    else
    {
      // No measurement processed on timeslot i (0x80 value)
      ((T_MPHP_INT_MEAS_IND *) (msg->SigP))->int_meas[i].rxlev[1] = (WORD8)NO_MEAS;
    }
  }

  // Set ID in the report message
  ((T_MPHP_INT_MEAS_IND *) (msg->SigP))->id = first_meas_ptr->id;
                                      
  #if (TRACE_TYPE==1) || (TRACE_TYPE==4)
    l1_trace_message(msg);
  #endif
   
  os_send_sig(msg, GRRM1_QUEUE);  
  DEBUGMSG(status,NU_SEND_QUEUE_ERR)
}

/*-------------------------------------------------------*/
/* l1pa_send_confirmation()                              */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pa_send_confirmation(UWORD32 SignalCode, UWORD8 id)
{
  xSignalHeaderRec *msg_con;

  msg_con = os_alloc_sig(sizeof(T_MPHP_ASSIGNMENT_CON));
  DEBUGMSG(status,NU_ALLOC_ERR)
  msg_con->SignalCode = SignalCode;

  ((T_MPHP_ASSIGNMENT_CON *) msg_con->SigP)->assignment_id = id;
  
  #if (TRACE_TYPE==1) || (TRACE_TYPE==4)
    l1_trace_message(msg_con);
  #endif
   
  os_send_sig(msg_con, GRRM1_QUEUE);  
  DEBUGMSG(status,NU_SEND_QUEUE_ERR)
}


/*-------------------------------------------------------*/
/* l1pa_send_tbf_release_con()                           */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pa_send_tbf_release_con(UWORD32 SignalCode, UWORD8 tbf_type)
{
    xSignalHeaderRec *conf_msg;

    // Send the confirmation message with the TBF type to L3
    // Allocate confirmation message...
    conf_msg = os_alloc_sig( sizeof(T_MPHP_TBF_RELEASE_CON));
    DEBUGMSG(status,NU_ALLOC_ERR);

    //Fill the message
    conf_msg->SignalCode = SignalCode;
    ((T_MPHP_TBF_RELEASE_CON *)(conf_msg->SigP))->tbf_type = tbf_type;

    // Send the confirmation message...
    os_send_sig(conf_msg, GRRM1_QUEUE);
    DEBUGMSG(status,NU_SEND_QUEUE_ERR)
}
#endif //L1_GPRS
