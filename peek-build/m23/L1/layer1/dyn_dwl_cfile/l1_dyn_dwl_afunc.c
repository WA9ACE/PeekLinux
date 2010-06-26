/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1_DYN_DWL_AFUNC.C
 *
 *        Filename l1_dyn_dwl_afunc.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#include "l1_confg.h"
#include "l1_types.h"
#include "sys_types.h"
#include "cust_os.h"
#include "l1_macro.h"
#include "l1_const.h"
#if TESTMODE
  #include "l1tm_defty.h"
#endif
#if (AUDIO_TASK == 1)
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
  #include "l1audio_signa.h"
#endif
#if (L1_GTT == 1)
  #include "l1gtt_const.h"
  #include "l1gtt_defty.h"
  #include "l1gtt_signa.h"
#endif
#if (L1_DYN_DSP_DWNLD == 1)
  #include "l1_dyn_dwl_msgty.h"
  #include "l1_dyn_dwl_defty.h"
  #include "l1_dyn_dwl_proto.h"
  #include "l1_dyn_dwl_const.h"
#endif //L1_DYN_DSP_DWNLD
#if (L1_MP3 == 1)
  #include "l1mp3_signa.h"
  #include "l1mp3_defty.h"
#endif //L1_MP3
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#if (L1_AAC == 1)
  #include "l1aac_signa.h"
  #include "l1aac_defty.h"
#endif //L1_AAC
#include "l1_defty.h"
#include "l1_varex.h"
#include "l1_msgty.h"
#include "l1_proto.h"
#include "l1_signa.h"
#include <string.h>
#include <stdio.h>


#if (TRACE_TYPE == 1) ||(TRACE_TYPE == 4) || (TRACE_TYPE == 7) || (TESTMODE)
  #include "l1_trace.h"
#endif

#if (L1_DYN_DSP_DWNLD == 1)

extern const BOOL primitives_to_patch_matrix[][MAX_NUM_OF_PATCH_IDS];
extern const BOOL incompatibility_matrix[][MAX_NUM_OF_PATCH_IDS];
extern const BOOL semaphore_matrix[][MAX_NUM_OF_SEMAPHORES];

#if (CODE_VERSION == SIMULATION || ((CHIPSET == 12) || (CHIPSET == 15)))
const T_SIGNAL_PATCH signal_patch_array[NUM_OF_DYN_DWNLD_PRIMITIVES] =
{
#if (L1_GTT == 1)
    {MMI_GTT_START_REQ,                       0},
#endif // L1_GTT
#if (L1_VOCODER_IF_CHANGE == 1)
    {MMI_TCH_VOCODER_CFG_REQ,                1},
#endif // L1_VOCODER_IF_CHANGE
#if (AUDIO_TASK == 1)
#if (MELODY_E2 == 1)
    {MMI_MELODY0_E2_START_REQ,                2},
    {MMI_MELODY1_E2_START_REQ,                3},
#endif // AUDIO_TASK
#endif // MELODY_E2
#if (L1_MP3 == 1)
    {MMI_MP3_START_REQ,                       4},
#endif // L1_MP3
#if (AUDIO_TASK == 1)
#if (L1_VOICE_MEMO_AMR == 1)
    {MMI_VM_AMR_PLAY_START_REQ,               5},
    {MMI_VM_AMR_RECORD_START_REQ,             6},
#endif // L1_VOICE_MEMO_AMR
#endif // AUDIO_TASK
#if (L1_AAC == 1)
    {MMI_AAC_START_REQ,                       7},
#endif // L1_AAC
#if (L1_PCM_EXTRACTION == 1)
	{MMI_PCM_DOWNLOAD_START_REQ,              8},
	{MMI_PCM_UPLOAD_START_REQ,                9}
#endif
};
#elif (CHIPSET == 10 && BOARD == 35)
const T_SIGNAL_PATCH signal_patch_array[NUM_OF_DYN_DWNLD_PRIMITIVES] =
{
#if (L1_GTT == 1)
    {MMI_GTT_START_REQ,                       0},
#endif // L1_GTT
#if (L1_VOCODER_IF_CHANGE == 1)
    {MMI_TCH_VOCODER_CFG_REQ,               1},
#endif // L1_VOCODER_IF_CHANGE
#if (AUDIO_TASK == 1)
#if (L1_VOICE_MEMO_AMR == 1)
    {MMI_VM_AMR_PLAY_START_REQ,              2},
    {MMI_VM_AMR_RECORD_START_REQ,            3}
#endif // L1_VOICE_MEMO_AMR
#endif // AUDIO_TASK
};
#endif // CHIPSET


/* l1_check_flag_for_download_area                                                  */
/* Parameters : Signal Code of the message
// Return     :    none
// Description :  Modify flag if needed to handle different download area when melody E2 is playing                                   */


void l1_check_flag_for_download_area(UWORD32 msg_code)
{
  switch(msg_code)
  {
#if (AUDIO_TASK == 1)
#if (MELODY_E2 == 1)
    /* MELODY E2 case must be handled carefully: two melodies can be activated for E2: both trigger a dynamic download*/
    /* When one of the two is requested, the corresponding flag used afterwards when it is stopped is set */
    case MMI_MELODY0_E2_START_REQ:
    {

        l1a.dyn_dwnld.melody0_E2_flag_activated = TRUE;
    }
    break;
    case MMI_MELODY1_E2_START_REQ:
    {

        l1a.dyn_dwnld.melody1_E2_flag_activated = TRUE;
    }
    break;

    /* When E2 is stopped the activated flag must be reset */

    case L1_MELODY0_E2_STOP_CON:
    {

        l1a.dyn_dwnld.melody0_E2_flag_activated = FALSE;

    }
    break;
    case L1_MELODY1_E2_STOP_CON:
    {

        l1a.dyn_dwnld.melody1_E2_flag_activated = FALSE;

    }
    break;
#endif // MELODY_E2 == 1
#endif // AUDIO_TASK == 1
  }

}
/*------------------------------------------------------------------------------------------------------------------- */
/* l1_does_the_incoming_primitive_trigger_dynamic_dwnld                                                               */
/*------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                    */
/* Parameters : Signal Code of the message, delay flag                                                                */
/*                                                                                                                    */
/* Return     :    TRUE if the primitive triggers a dynamic download, FALSE in the other case                         */
/*                                                                                                                    */
/* Description :  Check if the primitive triggers a dynamic download; if yes it return TRUE, otherwise FALSE          */
/*                                                                                                                    */
/*                                                                                                                    */
/*------------------------------------------------------------------------------------------------------------------- */

BOOL l1_does_the_incoming_primitive_trigger_dynamic_dwnld(UWORD32 msg_code, BOOL delay_flag)
{
  BOOL return_flag = FALSE;
  UWORD16 i;
  for (i=0;i<l1a.dyn_dwnld.num_of_primitives;i++)
  {
    if(msg_code == signal_patch_array[i].primitive)
      return_flag = TRUE;

  }

  l1_check_flag_for_download_area(msg_code);

  return return_flag;
}

/*----------------------------------------------------------------------------------------   */
/* l1_lookup_primitive_patch_matrix                                                          */
/*----------------------------------------------------------------------------------------   */
/*                                                                                           */
/* Parameters : Signal Code of the message, patch_id vector                                  */
/*                                                                                           */
/* Return     :    number of patches to download, patch ids vectore (by reference)           */
/*                                                                                           */
/* Description :  Retrieves the patch ids to be downloaded and number of patches to download */
/*                     Patch ID is retrieved via look-up in primitives to patch ID matrix    */
/*                                                                                           */
/*----------------------------------------------------------------------------------------   */

UWORD16 l1_lookup_primitive_patch_matrix(UWORD32 msg_code, UWORD16* patch_id_p)
{
  UWORD16 j;
  UWORD16 counter=0;
  UWORD16 msg_code_id=0 ;//omaps00090550

  // Check if one of the messages belongs to the set of primitves which triggers a dynamic download and saves its index

 if (!((l1a.state[L1A_GTT_STATE] == 0) || (l1a.state[L1A_GTT_STATE] == 1))) // TTY ON
  {
      if(msg_code == MMI_TCH_VOCODER_CFG_REQ) // AND Current_message == VOCODER
      	{
      	    // Dont do anything
      	    return 0;
      	}
  }

  
  for (j=0;j<NUM_OF_DYN_DWNLD_PRIMITIVES;j++)
  {
    if (msg_code == signal_patch_array[j].primitive)
      msg_code_id = signal_patch_array[j].msg_id;
  }


  // Compute the number of elements to be downloaded and their ids
  for (j=0; j<MAX_NUM_OF_PATCH_IDS; j++)
  {
    patch_id_p[j]=0xFFFF; //omaps00090550 ;
    if(primitives_to_patch_matrix[msg_code_id][j] == 1)
    {
      patch_id_p[counter++]=j;
     }
  }
  return counter;
}

/*------------------------------------------------------------------------- */
/* l1_is_patch_already_installed                                            */
/*------------------------------------------------------------------------- */
/*                                                                          */
/* Parameters : patch id                                                    */
/*                                                                          */
/* Return     :    TRUE patch is already installed, FALSE otherwise         */
/*                                                                          */
/* Description :  Check if the selected patch id is already installed or not*/
/*                                                                          */
/*                                                                          */
/*------------------------------------------------------------------------- */

BOOL l1_is_patch_already_installed (UWORD16 patch_id)
{
  UWORD16 i=0;
  while(i<l1a.dyn_dwnld.num_patches_installed)
  {
    if(l1a.dyn_dwnld.patch_id[i]==patch_id)
      break;
    else
      i++;
  }
  if(i<l1a.dyn_dwnld.num_patches_installed)
    return TRUE;
  else
    return FALSE;
}

/*-----------------------------------------------------------------------------  */
/* l1_is_patch_id_in_uninstall_set                                               */
/*-----------------------------------------------------------------------------  */
/*                                                                               */
/* Parameters : patch id, uninstall address vector, num of uninstall elements    */
/*                                                                               */
/* Return     :    TRUE patch is in the set, FALSE otherwise                     */
/*                                                                               */
/* Description :  Check if the selected patch id is in the uninstall set or not  */
/*                                                                               */
/*                                                                               */
/*-----------------------------------------------------------------------------  */

BOOL l1_is_patch_id_in_uninstall_set (UWORD16 patch_id, UWORD16 *uninstall_patch_vect, UWORD16 num_of_uninstall_elem)
{
  UWORD16 i;
  for (i=0;i<num_of_uninstall_elem;i++)
  {
    if(patch_id==uninstall_patch_vect[i])
      return TRUE;
  }
  return FALSE;
}

/*--------------------------------------------------------------------------------------------------------------------------------------- */
/* l1_manage_patch_incompatibilty                                                                                                         */
/*--------------------------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                                        */
/* Parameters :    num of patch to download, num of uninstalled elements (pointer), uninstall vect                                        */
/* Return     :    TRUE if there is some patch to uninstall, FALSE otherwise. By reference, num of patches to uninstall and their IDs     */
/*                                                                                                                                        */
/* Description :  Check if some patch must be uninstalled: in case, retrieves the number of patch to uninstall and their IDs              */
/*                                                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------------------------- */

BOOL l1_manage_patch_incompatibilty(UWORD16 num_of_patch_id_to_dwnld, UWORD16 *num_of_uninstall_elem, UWORD16 *uninstall_patch_id_vect)
{
  UWORD16 i,j;
  UWORD16 patch_id;

  // Reset values passed by reference
  *num_of_uninstall_elem=0;
  for(j=0;j<MAX_NUM_OF_PATCH_IDS;j++)
    uninstall_patch_id_vect[j]=0xFFFF; //omaps00090550 ;

   // First cycle: for all the patches that must be downloaded
  for(i=0;i<num_of_patch_id_to_dwnld;i++)
  {
    patch_id=l1a.dyn_dwnld.next_patch_id[i];
    // Check all incompatibilities
    for(j=0;j<MAX_NUM_OF_PATCH_IDS;j++)
    {
      // If patch ID j is incompatible with selected patch id that must be downloaded
      if(incompatibility_matrix[patch_id][j]==1)
      {
        // If patch j is already installed and not yet in the uninstall set
        if((l1_is_patch_already_installed(j) == TRUE) && (l1_is_patch_id_in_uninstall_set(j,uninstall_patch_id_vect,(*num_of_uninstall_elem))==FALSE))
        {
          // Add patch j in the uninstall set and increase number of uninstall elements
          uninstall_patch_id_vect[*num_of_uninstall_elem]=j;
          (*num_of_uninstall_elem)++;
        }
    }
    }
  }
  if(*num_of_uninstall_elem==0)
    return TRUE;
  else
    return FALSE;
}

/*----------------------------------------------------------------- */
/* l1_set_semaphores_for_all_state_machines_involved                */
/*----------------------------------------------------------------- */
/*                                                                  */
/* Parameters :  num of patches to download,                        */
/* Return     :   none                                              */
/*                                                                  */
/* Description :  Set semaphores for the state machines impacted    */
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

void l1_set_semaphores_for_all_state_machines_involved(UWORD16 num_of_patch_id_to_dwnld, UWORD16* next_patch_id)
{
  UWORD16 i,j, patch_id;
  for (i=0;i<num_of_patch_id_to_dwnld;i++)
  {
    patch_id = next_patch_id[i];
    for(j=0;j<MAX_NUM_OF_SEMAPHORES;j++)
    {
      if(semaphore_matrix[patch_id][j]==1)
      {
        l1a.dyn_dwnld.semaphore_vect[j]=RED;
      }
    }
  }
}

/*----------------------------------------------------------------- */
/* l1_update_semaphores_for_all_state_machines                      */
/*----------------------------------------------------------------- */
/*                                                                  */
/* Parameters :  num of patches to download,                        */
/* Return     :   none                                              */
/*                                                                  */
/* Description :  Update semaphores for the state machines impacted */
/*                (DELAY case)                                      */
/*                                                                  */
/*----------------------------------------------------------------- */

void l1_update_semaphores_for_all_state_machines(UWORD16 num_of_patch_id_to_dwnld)
{
  /* In case of delay the patch which is DELAYED may have been be already installed*/
  /* The semaphores that have been raised during the previous dynamic download */
  /* for that patch must be updated */

  UWORD16 i,j, patch_id;
  for(j=0;j<MAX_NUM_OF_SEMAPHORES;j++)
    l1a.dyn_dwnld.semaphore_vect[j] = GREEN;
  for (i=0;i<num_of_patch_id_to_dwnld;i++)
  {
    patch_id = l1a.dyn_dwnld.next_patch_id[i];
    for(j=0;j<MAX_NUM_OF_SEMAPHORES;j++)
    {
      if(semaphore_matrix[patch_id][j]==1)
      {
        l1a.dyn_dwnld.semaphore_vect[j]=RED;
      }
    }
  }
}

/*-------------------------------------------------------*/
/* l1_reset_semaphores                                   */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters :  none                                    */
/* Return     :                                          */
/*                                                       */
/* Description :  Reset semaphores only for the state machines related to the currently downloaded patches                      */
/*                     In fact, because of delay, some semaphores can be set for patch that will be downloaded in the follwoing step                                  */
/*                                                       */
/*-------------------------------------------------------*/
void l1_reset_semaphores()
{
  UWORD16 i,j, patch_id;
  for (i=0;i<l1a.dyn_dwnld.num_of_elem_to_copy;i++)
  {
    patch_id = l1a.dyn_dwnld.next_patch_id[i];
    for(j=0;j<MAX_NUM_OF_SEMAPHORES;j++)
    {
      if(semaphore_matrix[patch_id][j]==1)
      {
        l1a.dyn_dwnld.semaphore_vect[j]=GREEN;
      }
    }
  }
}


/*---------------------------------------------------------- */
/* l1_push_Primitive                                         */
/*---------------------------------------------------------- */
/*                                                           */
/* Parameters :  primitive ID                                */
/* Return     :  TRUE if push is successful, FALSE otherwise */
/*                                                           */
/* Description :  Push primitive ID into DELAY FIFO          */
/*                                                           */
/*                                                           */
/*---------------------------------------------------------- */

BOOL l1_push_Primitive(UWORD32 primitive_id)
{
  UWORD32 num_elem = l1a.dyn_dwnld.waiting_patch_fifo.num_of_elem;
  BOOL return_flag = TRUE;

  if(num_elem >= MAX_NUM_OF_PATCH_IDS)
  {
    return_flag = FALSE;
  }
  else
  {
    l1a.dyn_dwnld.waiting_patch_fifo.signal_code_vect[num_elem++] = primitive_id;
    l1a.dyn_dwnld.waiting_patch_fifo.num_of_elem = num_elem;
  }
  return return_flag;
}

/*---------------------------------------------------------- */
/* l1_pop_Primitive                                          */
/*---------------------------------------------------------- */
/*                                                           */
/* Parameters :  primitive ID (pointer)                      */
/* Return     :  TRUE if pop is successful, FALSE otherwise  */
/*                                                           */
/* Description :  Pop primitive from DELAY FIFO              */
/*                                                           */
/*                                                           */
/*---------------------------------------------------------- */

BOOL l1_pop_Primitive(UWORD32 *p_primitive)
{
  UWORD32 primitive_id;
  UWORD8 ind;
  UWORD32 num_elem = l1a.dyn_dwnld.waiting_patch_fifo.num_of_elem;
  BOOL return_flag = TRUE;

  if(num_elem <= 0)
  {
    return_flag = FALSE;
  }
  else
  {
    primitive_id = l1a.dyn_dwnld.waiting_patch_fifo.signal_code_vect[0];
    for (ind = 0;ind<num_elem-1;ind++)
      l1a.dyn_dwnld.waiting_patch_fifo.signal_code_vect[ind] = l1a.dyn_dwnld.waiting_patch_fifo.signal_code_vect[ind+1];
    l1a.dyn_dwnld.waiting_patch_fifo.signal_code_vect[num_elem-1] = 0;
    l1a.dyn_dwnld.waiting_patch_fifo.num_of_elem = num_elem-1;
    *p_primitive = primitive_id;
  }
  return return_flag;
}
/*---------------------------------------------------------- */
/* l1_check_Fifo_Primitive                                   */
/*---------------------------------------------------------- */
/*                                                           */
/* Parameters :  none                                        */
/* Return     :  Number of elements in the FIFO              */
/*                                                           */
/* Description :  Return number of elements in the FIFO      */
/*                                                           */
/*                                                           */
/*---------------------------------------------------------- */

UWORD32 l1_check_Fifo_Primitive()
{
  return l1a.dyn_dwnld.waiting_patch_fifo.num_of_elem;
}
#endif // L1_DYN_DSP_DWNLD == 1
