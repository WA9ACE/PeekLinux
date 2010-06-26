/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   VOICEMEMO
 $File:       MmiVoiceMemo.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       09/04/01

********************************************************************************

 Description:

    This provides the start code for the MMI

********************************************************************************
 $History: MmiVoiceMemo.c

	Mar 28, 2007  DR: OMAPS00122762 x0039928
	Description: MM: Deleting a PCM Voice Memo message in one particular memory, 
	delete them in all memories
	Solution: voice memo position and pcm voice memo position is provided for all the devices.
	
Mar 31, 2006   ER: OMAPS00067709  x0pleela
Description: Voice Buffering implementation on C+ for PTT via PCM API 
Solution: Added prototype for voice_buffering_stop_recording
		Updating voicebuffering data for play stopped in play callback 
			
Mar 15, 2006   ER: OMAPS00067709  x0pleela
Description: Voice Buffering implementation on C+ for PTT via PCM API 
Solution: Defined new macro "PCM_VOICE_MEMO_MAX_DURATION" to restrict the recording 
			timeout to 20 secs for PCM voice memo and voice buffering
		Added prototypes of new functions inputVoiceMemoNumberEditor, voiceMemoNumberCB, 
			voice_buffering_tim_cb, voice_buffering_data_reset
		Added following new functions:
			pcm_voice_memo_play: Sets voice memo type to PCM_VOICE_MEMO which indicates 
									PCM voice memo is active and start playing the voice
			pcm_voice_memo_record: Sets voice memo type to PCM_VOICE_MEMO which indicates 
									PCM voice memo is active and start recording the voice
			voice_buffering_edit_num: Opens an edtor for the user to enter PTT number. Also resets
									voice buffering data with default values
			voiceMemoNumberCB: call back for number entry
			inputVoiceMemoNumberEditor: Settings of Editor
			voice_buffering_record_start: Starts recording voice and conects to the PTT number if the
										call is not active. If in call, then start playing the voice
			voice_buffering_record_stop: Stops the timer and recording. If call is not active the 
										disconnects the call
			voice_buffering_playback_start: Starts playing the voice. If any error while playing, 
										stops timer and recording
			voice_buffering_stop_recording: A wrapper function for record stop to invoke from other 
										modules. Stops the timer and recording
			voice_buffering_stop_playing: A wrapper function for play stop to invoke from other modules
			voice_buffering_tim_cb: Timer callback. Stops the timer. If PTT call is not setup disconnect 
									the call, else call record stop
			voice_buffering_data_reset: Resets voice buffering data to default values

		Function: voice_memo_play, voice_memo_record, voice_memo_destroy
		Changes: Added code to set voice memo type

		Function: voice_memo
		Changes: Added code to handle record start, play start for PCM voice memo

		Function: voice_memo_tim_cb
		Changes: Added code to handle record stop, play stop for PCM voice memo

		Function: voice_memo_riv_play_cb
		Changes: Added code to allow user to accept ant incoming call after the buffering phase
			
Feb 27, 2006    ER: OMAPS00067709 x0pleela
Description: Voice Buffering implementation on C+ for PTT via PCM API 
Solution: Defined a global variable gPcm_voice_Memo which indicates which Voice memo is active 
		and corresponding audio APIs will be invoked and duplication of code is avoided

Feb 24, 2006    ER: OMAPS00067709 x0pleela
Description: Voice Buffering implementation on C+ for PTT via PCM API 
Solution: Defined new macro PCM_VOICE_MEMO_MAX_DURATION which defines maximum 
			recording and playing duration for PCM voice memo and voice buffering
		Implemented following functions to support PCM voice memo feature
			pcm_voice_memo_init: Initialization of PCM voice memo
			pcm_voice_memo_exit:Termination of PCM voice memo
			pcm_voice_memo_play:Starts PCM voice memo play functionality
			pcm_voice_memo_record:Starts PCM voice memo record functionality
			pcm_voice_memo_start: create a new voicememo dialog
			pcm_voice_memo_create: Creation of window
			pcm_voice_memo_destroy: Destruction of an dialog
			pcm_voice_memo: Dialog function for PCM voice memo dialog
			pcm_voice_memo_win_cb:Callback function for information dialog
			pcm_voice_memo_tim_cb: Callback function for the PCM voice_memo timer.
			pcm_voice_memo_kbd_cb: Keyboard event handler
			pcm_voice_memo_riv_record_cb: Callback that handles the return status for PCM voice memo recording
			pcm_voice_memo_riv_play_cb:Callback that handles the return status for PCM voice memo palying
			
  09/04/01      Original Condat(UK) BMI version.

 $End

*******************************************************************************/




/*******************************************************************************

                                Include files

*******************************************************************************/
#define ENTITY_MFW

/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#include "mfw_sys.h"

#include "prim.h"

#include "cus_aci.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_tim.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_sat.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_cphs.h"
#include "mfw_sat.h"

#include "dspl.h"


#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiIdle.h"
#include "MmiSoftKeys.h"
#include "Mmiicons.h"
#include "MmiMenu.h"
#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiSettings.h"

#include "gdi.h"
#include "audio.h"
#include "MmiSounds.h"
#include "MmiResources.h"
#include "mmiColours.h"

//#include "vmd.h"
#include "MmiVoiceMemo.h"
#include "mfw_ffs.h"

/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#ifndef NEPTUNE_BOARD
/* END ADD: Req ID: : Sumit : 14-Mar-05 */
#include "Audio/audio_api.h"
/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#endif /* NEPTUNE_BOARD*/
/* END ADD: Req ID: : Sumit : 14-Mar-05 */

#include "mfw_aud.h"

//x0pleela 09 Mar, 2006  ER:OMAPS00067709
//Including this header to support number editor
#ifdef FF_PCM_VM_VB
#include "MmiBookshared.h"
#endif
/*******************************************************************************

                                internal data

*******************************************************************************/

/*
 * Internal events
 */
#define VM_INIT           500
#define VM_DESTROY       501
#define VM_PLAY           502
#define VM_RECORD         503
#define VM_DELETE_BUFFER  504

#define VOICE_MEMO_MAX_DURATION 10					// 10 Secs

typedef struct
{
  T_MMI_CONTROL   mmi_control;
  T_MFW_HND       parent_win;
  T_MFW_HND       info_win;
  T_MFW_HND       info_tim;
  T_MFW_HND       info_kbd;
  USHORT          Identifier;
  USHORT          time;
  char         elapsed_time[7];
  int			  play_sk2;
} T_voice_memo;

//x0pleela 09 Mar, 2006   ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
EXTERN T_voice_buffering voice_buffering_data;
EXTERN T_call call_data; 
#endif
/*******************************************************************************

                                Local prototypes

*******************************************************************************/


T_MFW_HND       voice_memo_start  (T_MFW_HND  parent_win, SHORT ID);
static T_MFW_HND    voice_memo_create (T_MFW_HND  parent_win);
static void       voice_memo_destroy  (T_MFW_HND  own_window);
static void       voice_memo      (T_MFW_HND  win, USHORT event, SHORT value, void * parameter);
static int        voice_memo_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static T_MFW_CB     voice_memo_tim_cb (T_MFW_EVENT event, T_MFW_TIM * tc);
static int        voice_memo_kbd_cb (T_MFW_EVENT event, T_MFW_KBD * keyboard);
static void			voice_memo_dialog_create(T_MFW_HND win, int str1, int str2);
void          voice_memo_dialog_cb (T_MFW_HND win, UBYTE identifier, UBYTE reason);
GLOBAL void voice_buffering_playback_start(void );
/*
** Rivera Callback functions
*/
static void voice_memo_riv_record_cb(void *parameter);
static void voice_memo_riv_play_cb(void *parameter);

//x0pleela 02 Mar, 2006  ER:OMAPS00067709
//Including this header to support number editor
#ifdef FF_PCM_VM_VB
MfwHnd inputVoiceMemoNumberEditor( MfwHnd parent, void *buffer );
void voiceMemoNumberCB( T_MFW_HND win, USHORT Identifier, UBYTE reason );
static int voice_buffering_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc);
static void voice_buffering_data_reset( void );
GLOBAL void voice_buffering_stop_recording(void );
#endif
/*******************************************************************************

 $Function:     voice_memo_init

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL void voice_memo_init (void)
{
  TRACE_FUNCTION ("voice_memo_init()");
  
  #ifdef FF_PCM_VM_VB
    voice_buffering_data_reset();
  #endif
  
}

/*******************************************************************************

 $Function:     voice_memo_exit

 $Description:


 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL void voice_memo_exit (void)
{
  TRACE_FUNCTION ("voice_memo_exit()");
	
}




/*******************************************************************************

 $Function:     voice_memo_play

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int  voice_memo_play (MfwMnu* m, MfwMnuItem* i)
{
  T_MFW_HND win = mfw_parent(mfw_header());

  TRACE_FUNCTION ("voice_memo_play()");

  //x0pleela 27 Feb, 2006  ER: OMAPA00067709
  //Set this variable to VOICE_MEMOwhich indicates  voice memo is active
  #ifdef FF_PCM_VM_VB
   set_voice_memo_type( AMR_VOICE_MEMO);
  #endif
  
  //create a new voice memo dialog
  voice_memo_start (win, VM_PLAY);
  return 1;  // ADDED BY RAVI - 29-11-2005
}

/*******************************************************************************

 $Function:     voice_memo_storeage

 $Description:


 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int PCM_voice_memo_storeage (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	int txtId = TxtFFS;

	TRACE_FUNCTION("mmi_camera_set_storage()");

	switch (m->lCursor[m->level])
	{
		case 0:
			txtId = TxtFFS;
			FFS_flashData.PCM_voicememo_storage = SNAP_STG_FFS;
		break;
		
		case 1:
			txtId = TxtNORMS;
			FFS_flashData.PCM_voicememo_storage = SNAP_STG_NORMS;
		break;
		
		case 2:
			txtId = TxtNAND;
			FFS_flashData.PCM_voicememo_storage = SNAP_STG_NAND;
		break;
		
		case 3:
			txtId = TxtTflash;
			FFS_flashData.PCM_voicememo_storage = SNAP_STG_MMC;
		break;
	}

	flash_write();
	//Set the Quality Factor.
	TRACE_EVENT_P1("STorage set to %d ", FFS_flashData.PCM_voicememo_storage);
	//Display the dialgbox to the user.
	voice_memo_dialog_create(parent_win,txtId,TxtSelected);
	return MFW_EVENT_CONSUMED; 
}

/*******************************************************************************

 $Function:     voice_memo_storeage

 $Description:


 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int voice_buffer_storeage (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	int txtId = TxtFFS;

	TRACE_FUNCTION("mmi_camera_set_storage()");

	switch (m->lCursor[m->level])
	{
		case 0:
			txtId = TxtFFS;
			FFS_flashData.voicebuffer_storage = SNAP_STG_FFS;
		break;
		
		case 1:
			txtId = TxtNORMS;
			FFS_flashData.voicebuffer_storage = SNAP_STG_NORMS;
		break;

		case 2:
			txtId = TxtNAND;
			FFS_flashData.voicebuffer_storage = SNAP_STG_NAND;
		break;
		
		case 3:
			txtId = TxtTflash;
			FFS_flashData.voicebuffer_storage = SNAP_STG_MMC;
		break;
	}

	flash_write();
	//Set the Quality Factor.
	TRACE_EVENT_P1("STorage set to %d ", FFS_flashData.voicebuffer_storage);
	//Display the dialgbox to the user.
	voice_memo_dialog_create(parent_win,txtId,TxtSelected);
	return MFW_EVENT_CONSUMED; 
}

/*******************************************************************************

 $Function:     voice_memo_storeage

 $Description:


 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int voice_memo_storeage (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	int txtId = TxtFFS;

	TRACE_FUNCTION("mmi_camera_set_storage()");

	switch (m->lCursor[m->level])
	{
		case 0:
			txtId = TxtFFS;
			FFS_flashData.voicememo_storage = SNAP_STG_FFS;
		break;
		
		case 1:
			txtId = TxtNORMS;
			FFS_flashData.voicememo_storage = SNAP_STG_NORMS;
		break;
		
		case 2:
			txtId = TxtNAND;
			FFS_flashData.voicememo_storage = SNAP_STG_NAND;
		break;
		
		case 3:
			txtId = TxtTflash;
			FFS_flashData.voicememo_storage = SNAP_STG_MMC;
		break;
	}

	flash_write();
	//Set the Quality Factor.
	TRACE_EVENT_P1("STorage set to %d ", FFS_flashData.voicememo_storage);
	//Display the dialgbox to the user.
	voice_memo_dialog_create(parent_win,txtId,TxtSelected);
	return MFW_EVENT_CONSUMED; 
}


/*******************************************************************************

 $Function:     voice_memo_record

 $Description:


 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int voice_memo_record (MfwMnu* m, MfwMnuItem* i)
{

  T_MFW_HND win = mfw_parent(mfw_header());

  TRACE_FUNCTION ("voice_memo_record()");

  //x0pleela 27 Feb, 2006  ER: OMAPA00067709
  //Set this variable to VOICE_MEMOwhich indicates  voice memo is active
  #ifdef FF_PCM_VM_VB
    set_voice_memo_type( AMR_VOICE_MEMO);
  #endif
  //create a new voice memo dialog
  voice_memo_start (win, VM_RECORD);
  return 1;  // RAVI - 29-11-2005
}


/*******************************************************************************

 $Function:     voice_memo_start

 $Description:  create a new voicememo dialog

 $Returns:    Dialogue info win

 $Arguments:  parent win, display info

*******************************************************************************/
T_MFW_HND voice_memo_start (T_MFW_HND  parent_win, SHORT ID)
{
  T_MFW_HND win;

  win = voice_memo_create (parent_win);

  if (win NEQ NULL)
  {
    SEND_EVENT (win, VM_INIT, ID, 0);
  }
  return win;
}



/*******************************************************************************

 $Function:     vocie_memo_create

 $Description:  Creation of window

 $Returns:    Dialogue info win

 $Arguments:  parent win

*******************************************************************************/
static T_MFW_HND voice_memo_create (T_MFW_HND parent_win)
{
  T_MFW_WIN     * win_data;
  T_voice_memo *  data = (T_voice_memo *)ALLOC_MEMORY (sizeof (T_voice_memo));

  TRACE_FUNCTION ("vocie_memo_create()");

  data->info_win = win_create (parent_win, 0, E_WIN_VISIBLE, (T_MFW_CB)voice_memo_win_cb);

  if (data->info_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog   = (T_DIALOG_FUNC)voice_memo;
  data->mmi_control.data     = data;
  data->parent_win           = parent_win;
  win_data                   = ((T_MFW_HDR *)data->info_win)->data;
  win_data->user             = (void *)data;

  /*
  ** Initialise values to safe defualts
  */
  data->play_sk2 = TxtSoftBack;
	
  /*
   * return window handle
   */
  return data->info_win;
}


/*******************************************************************************

 $Function:     voice_memo_destroy

 $Description:  Destruction of an dialog

 $Returns:    void

 $Arguments:  win

*******************************************************************************/
void voice_memo_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win  = ((T_MFW_HDR *)own_window)->data;
  T_voice_memo * data   = (T_voice_memo *)win->user;

  TRACE_FUNCTION ("voice_memo_destroy()");

	if (own_window == NULL)
	{
		TRACE_EVENT ("Error : Called with NULL Pointer");
		return;
	}

  if (data)
  {
    /*
     * Exit TIMER & KEYBOARD Handle
     */
    kbd_delete (data->info_kbd);
    tim_delete (data->info_tim);

    /*
     * Delete WIN Handler
     */
    win_delete (data->info_win);
    /*
     * Free Memory
     */
    FREE_MEMORY ((void *)data, sizeof (T_voice_memo));
  }
  
  //x0pleela 27 Feb, 2006  ER: OMAPA00067709
  //Set gVoiceMemo to NONE while destroying voice memo dialog
#ifdef FF_PCM_VM_VB
  set_voice_memo_type( NONE);
#endif
}



/*******************************************************************************

 $Function:     voice_memo

 $Description:  Dialog function for voice memo dialog

 $Returns:    void

 $Arguments:  win, window handle event, value, parameter

*******************************************************************************/
void voice_memo (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
  T_MFW_WIN      * win_data     = ((T_MFW_HDR *)win)->data;
  T_voice_memo    * data         = (T_voice_memo *)win_data->user;

 // T_DISPLAY_DATA display_info;   // RAVI
  
  SHORT	mfw_aud_retVal = 0; /* x0039928 - Lint warning fix */
 #ifdef FF_PCM_VM_VB  
  UBYTE vm_type; //to store voice memo type
#endif


  data->Identifier = value;

  TRACE_FUNCTION ("voice_memo()");

  switch (event)
  {
    case VM_INIT:
    TRACE_EVENT ("VM_INIT()");
      /*
       * Initialize dialog
       */

      /*
       * Create timer and keyboard handler
       */
    data->info_tim = tim_create (win, 1000, (T_MFW_CB)voice_memo_tim_cb);
    data->info_kbd = kbd_create (win, KEY_ALL, (T_MFW_CB)voice_memo_kbd_cb);


    if (value EQ VM_PLAY)
    {
      TRACE_EVENT ("VM_PLAY()");
	  
	  mfw_aud_retVal = mfw_aud_vm_start_playback(voice_memo_riv_play_cb);
	  
	  if (mfw_aud_retVal == MFW_AUD_VM_OK)
      {
        //start the second timer
        data->time = 0;
	    tim_start (data->info_tim);
	    
	    //Set the Right Softkey Id
	    data->play_sk2 = TxtStop;
      }
	  else
	  {
	    if (mfw_aud_retVal == MFW_AUD_VM_MEM_EMPTY)
	    {
	    	voice_memo_dialog_create(win, TxtRecording, TxtEmpty);
	    }
	    else
	    {
	    	//Display a dialog and exit
	    	TRACE_EVENT_P1 ("ERROR : mfw_aud_vm_start_playback failed with return value : %d", mfw_aud_retVal);
	    
	    	voice_memo_dialog_create( win, TxtPlayback, TxtFailed);
	    }
	  }
    }
    else if (value EQ VM_RECORD)
    {
      TRACE_FUNCTION ("VM_RECORD()");
       //x0pleela 27 Feb, 2006  ER: OMAPA00067709
      //Pass 20 secs time out for PCM voice memo and 10 secs for AMR Voice memo
#ifdef FF_PCM_VM_VB
    //get the voice memo type
    vm_type = get_voice_memo_type();

      if(vm_type EQ PCM_VOICE_MEMO)
      	 mfw_aud_retVal = mfw_aud_vm_start_record(PCM_VOICE_MEMO_MAX_DURATION, 
												voice_memo_riv_record_cb);
      else
      	{
      	  if(vm_type EQ AMR_VOICE_MEMO )
#endif     
	  mfw_aud_retVal = mfw_aud_vm_start_record(VOICE_MEMO_MAX_DURATION, voice_memo_riv_record_cb);
#ifdef FF_PCM_VM_VB
      	}
#endif
	  
	  // If the Riviera call failed
	  if (mfw_aud_retVal == MFW_AUD_VM_OK)
	  {
	  	//start the second timer
	  	data->time = 0;
	  	tim_start (data->info_tim);
	  }
	  else
	  {
	  	if (mfw_aud_retVal == MFW_AUD_VM_MEM_FULL)
	    {
	      TRACE_EVENT ("memory full ");
/* RAVI - 20-1-2006 */
/* Silent Implementation */
#ifdef NEPTUNE_BOARD
             audio_PlaySoundID(0, TONES_KEYBEEP, getCurrentVoulmeSettings(), 
                                              AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else
	      /* play Keybeep */
	      audio_PlaySoundID(0, TONES_KEYBEEP, 200, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	  
	  	  voice_memo_dialog_create( win, TxtRecording, TxtFull );
	    }
	    else
	    {
	      //Display a dialog and exit
	      TRACE_EVENT_P1 ("ERROR : mfw_aud_vm_start_record failed with return value : %d", mfw_aud_retVal);
	    
	      voice_memo_dialog_create( win, TxtRecording, TxtFailed );
	    }
      }
    }
      win_show (win);
      break;


  //if the user selected to clear the Voicememo buffer
    case VM_DELETE_BUFFER:
	voice_memo_dialog_create( win, TxtDeleted, TxtNull );

	/*
	** Delete the voice memo file
	*/
	mfw_aud_vm_delete_file();
  break;

    case VM_DESTROY:
    voice_memo_destroy(win);
  break;
  }
  return;
}

/*******************************************************************************

 $Function:     voice_memo_win_cb

 $Description:  Callback function for information dialog

 $Returns:    void

 $Arguments:  window handle event, win
*******************************************************************************/
static int voice_memo_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
    T_voice_memo * data = (T_voice_memo *)win->user;

    TRACE_FUNCTION ("voice_memo_win_cb()");

    if (data EQ 0)
  return 1;

    switch (event)
  {
  case E_WIN_VISIBLE:
      if (win->flags & E_WIN_VISIBLE)
    {
        /*
         * Clear Screen
         */
        dspl_ClearAll();

      switch (data->Identifier)
      {
        case VM_PLAY:
            /*
             * Print the information screen
             */
					ALIGNED_PROMPT((T_MMI_ALIGN)LEFT,Mmi_layout_line(1),0, TxtPlayback);

            /*
             * Print the elapsed time
             */
				    displayAlignedText(LEFT, Mmi_layout_line(2), 0, data->elapsed_time);

            /*
             * Print softkeys
             */
					displaySoftKeys(TxtDelete, data->play_sk2);
          break;

        case VM_RECORD:
            /*
             * Print the information screen
             */
					ALIGNED_PROMPT((T_MMI_ALIGN)LEFT,Mmi_layout_line(1),0, TxtRecording);

            /*
             * Print the elapsed time
             */
				    displayAlignedText(LEFT, Mmi_layout_line(2), 0, data->elapsed_time);

            /*
             * Print softkeys
             */
            displaySoftKeys(TxtStop, '\0');
          break;
      }

    }
      break;

  default:
      return 0;
  }
    return 1;
}


/*******************************************************************************

 $Function:     voice_memo_tim_cb

 $Description:  Callback function for the voice_memo timer.


 $Returns:    MFW event handler

 $Arguments:  window handle event, timer control block

*******************************************************************************/
static T_MFW_CB voice_memo_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_voice_memo   * data = (T_voice_memo *)win_data->user;

  SHORT	mfw_aud_retVal;
  //x0pleela 27 Feb, 2006   ER:OMAPS00067709
  //to store PCM/AMR voice memo record/play duration
  USHORT vm_duration = 0; /* x0039928 - Lint warning fix */
//  char buf[40];   // RAVI

//x0pleela 08 Mar, 2006   ER:OMAPS00067709
#ifdef FF_PCM_VM_VB  
  UBYTE vm_type; //to store voice memo type
#endif

    TRACE_FUNCTION ("voice_memo_tim_cb()");

//x0pleela 27 Feb, 2006   ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
	//x0pleela 06 Mar, 2006  ER:OMAPS00067709
	//get the voice memo type
	vm_type = get_voice_memo_type();

	if(vm_type EQ PCM_VOICE_MEMO)
	  vm_duration =  PCM_VOICE_MEMO_MAX_DURATION;
	else 
	{
	  if (vm_type EQ AMR_VOICE_MEMO)
#endif
	 vm_duration = VOICE_MEMO_MAX_DURATION;
#ifdef FF_PCM_VM_VB 
	}
#endif

  switch (data->Identifier)
  {
    case VM_PLAY:
      //the max. playback time is 10 second
			if (data->time < mfw_aud_vm_get_duration())
      {
          TRACE_EVENT ("continue the second timer");

        data->time ++;
        // it shows here the elapsed time
        sprintf(data->elapsed_time,"%02d sec", data->time);

          TRACE_EVENT (data->elapsed_time);

        //continue the second timer
        tim_start (data->info_tim);
      }
      else
      {
		data->play_sk2 = TxtSoftBack;
#ifndef _SIMULATION_
		/*
		** Stop playing the Voice Memo.
		*/
		mfw_aud_retVal = mfw_aud_vm_stop_playback(voice_memo_riv_play_cb);

		if (mfw_aud_retVal != MFW_AUD_VM_OK)
			TRACE_EVENT_P1 ("ERROR : mfw_aud_vm_stop_playback failed with return value : %d", mfw_aud_retVal);
#else
		TRACE_EVENT("call mfw_aud_vm_stop_playback()");
#endif
      }
      break;


    case VM_RECORD:

	  data->time++;
 
      if (data->time < vm_duration)
      {
          TRACE_EVENT ("continue the second timer");
	//x0pleela 27 Feb, 2006   ER:OMAPS00067709
	//changed from Macro VOICE_MEMO_MAX_DURATION to local variable vm_duration
        // it shows here the remainig time
		sprintf(data->elapsed_time,"%02d sec", (vm_duration - data->time));

          TRACE_EVENT (data->elapsed_time);

        //continue the second timer
        tim_start (data->info_tim);
      }
      else
      {
//x0pleela 27 Feb, 2006   ER:OMAPS00067709
//changed from Macro VOICE_MEMO_MAX_DURATION to local variable vm_duration
		mfw_aud_vm_set_duration(vm_duration);
				
#ifndef _SIMULATION_
		/*
		** Stop recording the Voice Memo.
		*/
		mfw_aud_retVal = mfw_aud_vm_stop_record(voice_memo_riv_record_cb);

		if (mfw_aud_retVal != MFW_AUD_VM_OK)
			TRACE_EVENT_P1 ("ERROR : mfw_aud_vm_stop_record failed with return value : %d", mfw_aud_retVal);
/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
              audio_PlaySoundID(0, TONES_KEYBEEP, getCurrentVoulmeSettings(),
                                               AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else
		/*the voice memo is now full, show "memory full" */
		audio_PlaySoundID(0, TONES_KEYBEEP, 200, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */

		voice_memo_dialog_create( win, TxtRecording, TxtFull );
#else
		TRACE_EVENT("call mfw_aud_vm_stop_record()");
#endif
      }
      break;
  }

  //Update the screen
  winShow(win);

  return 0;
}


/*******************************************************************************

 $Function:     voice_memo_kbd_cb

 $Description:  Keyboard event handler

 $Returns:    status int

 $Arguments:  window handle event, keyboard control block

*******************************************************************************/

static int voice_memo_kbd_cb (T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_voice_memo   * data = (T_voice_memo *)win_data->user;

  USHORT          Identifier = data->Identifier;
	SHORT			mfw_aud_retVal;


  TRACE_FUNCTION("voice_memo_kbd_cb");

  switch (keyboard->code)
  {
  case KCD_HUP:
    case KCD_LEFT:
    if (Identifier EQ VM_PLAY)
    {
#ifndef _SIMULATION_
	  /*
	  ** Stop playing the Voice Memo and Delete it.
	  */
	  mfw_aud_retVal = mfw_aud_vm_stop_playback(voice_memo_riv_play_cb);
	  
	  if (mfw_aud_retVal != MFW_AUD_VM_OK)
	  	TRACE_EVENT_P1 ("ERROR : mfw_aud_vm_stop_playback failed with return value : %d", mfw_aud_retVal);
#else
	  TRACE_EVENT("call mfw_aud_vm_stop_playback()");
#endif
      //delete the voicememo buffer
     /* Mar 28, 2007  DR: OMAPS00122762 x0039928 */
     /* Fix: Commented duration reset */
/*	  mfw_aud_vm_set_duration(0); */

      //user selected the delete-key
      SEND_EVENT (win, VM_DELETE_BUFFER, 0, 0);
    }
    else if (Identifier EQ VM_RECORD)
    {
	  mfw_aud_vm_set_duration(data->time);
	  
#ifndef _SIMULATION_
	  /*
	  ** Stop recording the Voice Memo.
	  */
	  mfw_aud_retVal = mfw_aud_vm_stop_record(voice_memo_riv_record_cb);
	  
	  if (mfw_aud_retVal != MFW_AUD_VM_OK)
	  	TRACE_EVENT_P1 ("ERROR : mfw_aud_vm_stop_record failed with return value : %d", mfw_aud_retVal);
	  
	  //go back to the previous dialog
	  SEND_EVENT (win, VM_DESTROY, 0, 0);
#else
	  TRACE_EVENT("call mfw_aud_vm_stop_record()");
#endif
    }
       break;

    case KCD_RIGHT:
    if (Identifier EQ VM_PLAY)
    {

#ifndef _SIMULATION_
	  /*
	  ** Stop playing the Voice Memo.
	  */
	  mfw_aud_retVal = mfw_aud_vm_stop_playback(voice_memo_riv_play_cb);
	  
	  if (mfw_aud_retVal != MFW_AUD_VM_OK)
	  	TRACE_EVENT_P1 ("ERROR : mfw_aud_vm_stop_playback failed with return value : %d", mfw_aud_retVal);
#else
	  TRACE_EVENT("call mfw_aud_vm_stop_playback()");
#endif

      //go back to the previous dialog
      SEND_EVENT (win, VM_DESTROY, 0, 0);
    }
    else if (Identifier EQ VM_RECORD)
    {
      //nothing
    }
       break;

    default:
       break;
  }
  return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     voice_memo_dialog_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
void voice_memo_dialog_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
  TRACE_FUNCTION ("voice_memo_dialog_cb()");

      switch (reason)
      {
        case INFO_KCD_LEFT:
          /* no break; */
        case INFO_TIMEOUT:
          /* no break; */
        case INFO_KCD_HUP:
          /* no break; */
        case INFO_KCD_RIGHT:
          /* no break; */
        case INFO_KCD_CLEAR:

        //go back to the previous dialog
      SEND_EVENT (win, VM_DESTROY,0, 0);

          break;
      }
}


/*******************************************************************************

 $Function:     voice_memo_dialog_create

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
static void voice_memo_dialog_create(T_MFW_HND win, int str1, int str2)
{
	T_DISPLAY_DATA display_info;
	
	TRACE_FUNCTION ("voice_memo_dialog_create()");

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)voice_memo_dialog_cb, THREE_SECS, KEY_CLEAR | KEY_RIGHT | KEY_LEFT );
	/*
	* Call Info Screen
	*/
	info_dialog (win, &display_info);
}

static void voice_memo_riv_record_cb(void *parameter)
{
/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#ifndef NEPTUNE_BOARD
/* END ADD: Req ID: : Sumit : 14-Mar-05 */

//x0pleela 16 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
	UBYTE vm_type; //to store voice memo type
#endif


//x0pleela 16 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
  vm_type = get_voice_memo_type();
  switch( vm_type )
  {
    case AMR_VOICE_MEMO:
    {
#endif    /* FF_PCM_VM_VB */
      T_AUDIO_VM_RECORD_STATUS *record_status;
      record_status = (T_AUDIO_VM_RECORD_STATUS *)parameter;
      if (record_status->status != AUDIO_OK)
      {
        TRACE_EVENT_P1("ERROR : voice_memo_riv_record_cb received AUDIO_ERROR, recorded duration is %d",
        						record_status->recorded_duration);
      }
      else
      {
        TRACE_EVENT_P1("INFO : voice_memo_riv_record_cb received AUDIO_OK, recorded duration is %d",
        						record_status->recorded_duration);
        mfw_aud_vm_set_duration((UINT8)record_status->recorded_duration);
      }
 #ifdef FF_PCM_VM_VB   
    }
    break;

    case PCM_VOICE_MEMO:
    {
      T_AUDIO_VM_PCM_RECORD_STATUS *pcm_vm_record_status;
      
      pcm_vm_record_status = (T_AUDIO_VM_PCM_RECORD_STATUS *)parameter;
      if (pcm_vm_record_status->status != AUDIO_OK)
      {
        TRACE_EVENT_P1("ERROR : voice_memo_riv_record_cb received AUDIO_ERROR, recorded duration is %d",
        						pcm_vm_record_status->recorded_duration);
      }
      else
      {
        TRACE_EVENT_P1("INFO : voice_memo_riv_record_cb received AUDIO_OK, recorded duration is %d",
        						pcm_vm_record_status->recorded_duration);
        mfw_aud_vm_set_duration((UINT8)pcm_vm_record_status->recorded_duration);
      }
    }
    break;
    
    case VOICE_BUFFERING:
    {
    	T_AUDIO_VBUF_PCM_RECORD_STATUS *vbuf_pcm_record_status;

    	 vbuf_pcm_record_status = (T_AUDIO_VBUF_PCM_RECORD_STATUS *)parameter;
    	 
      if (vbuf_pcm_record_status->status != AUDIO_OK)
      {
        TRACE_EVENT_P1("ERROR : voice_memo_riv_record_cb received AUDIO_ERROR, recorded duration is %d",
        						vbuf_pcm_record_status->recorded_duration);
      }
      else
      {
        TRACE_EVENT_P1("INFO : voice_memo_riv_record_cb received AUDIO_OK, recorded duration is %d",
        						vbuf_pcm_record_status->recorded_duration);
        //x0pleela 13 Mar, 2006  ER:OMAPS00067709
       //check for reason to call record_stop
       switch( voice_buffering_data.rec_stop_reason)
       {
       case CALLING_PARTY_END_CALL: 		//calling party hangs up	
       case CALLED_PARTY_END_CALL: 		//called party hangs up
         voice_buffering_data.call_active = FALSE;
         break;
       
       case PLAY_ERROR:					//recording timeout while playing
         voice_buffering_data.buffering_phase = FALSE;
         break;
       
       case CALL_END_BEF_SETUP:			//calling party hangs up before call setup  	
       case USER_SEL_STOP: 				//user selected "STOP" option, then just return
       case RECORDING_TIMEOUT: 			//recording timeout
       case CALLED_PARTY_NOT_AVAILABLE: 	//not reachable
       default:
         break;		  		
       }
       voice_buffering_data.recording_possible = TRUE;
       voice_buffering_data.incoming_call_discon = FALSE;
       //destroy timer
       timDelete(voice_buffering_data.voice_buffering_tim);
      }
      //x0pleela 03 Apr, 2006  ER: OMAPS00067709
      //Set voice memo type to NONE 
      set_voice_memo_type(NONE);   
    }   
 
    break;
    
    default:
    break;
    }


	
#endif /* FF_PCM_VM_VB */
	
/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#endif /* NEPTUNE_BOARD */
/* END ADD: Req ID: : Sumit : 14-Mar-05 */

}

static void voice_memo_riv_play_cb(void *parameter)
{
/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#ifndef NEPTUNE_BOARD
/* END ADD: Req ID: : Sumit : 14-Mar-05 */

//x0pleela 16 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
  UBYTE vm_type; //to store voice memo type
#endif

//x0pleela 16 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
  vm_type = get_voice_memo_type();
  switch( vm_type )
  {
    case AMR_VOICE_MEMO:
    {
#endif    /* FF_PCM_VM_VB */

      T_AUDIO_VM_PLAY_STATUS *play_status;
      play_status = (T_AUDIO_VM_PLAY_STATUS *)parameter;
      	if (play_status->status != AUDIO_OK)
	{
		TRACE_EVENT("ERROR : voice_memo_riv_play_cb received AUDIO_ERROR");
	}
	else
	{
		TRACE_EVENT("INFO : voice_memo_riv_play_cb received AUDIO_OK");
	}
	
 #ifdef FF_PCM_VM_VB   
    }
    break;

    case PCM_VOICE_MEMO:
    {
      T_AUDIO_VM_PCM_PLAY_STATUS *pcm_vm_play_status;
      pcm_vm_play_status = (T_AUDIO_VM_PCM_PLAY_STATUS *)parameter;
      if (pcm_vm_play_status->status != AUDIO_OK)
      {
        TRACE_EVENT("ERROR : voice_memo_riv_play_cb received AUDIO_ERROR");
      }
      else
      {
        TRACE_EVENT("INFO : voice_memo_riv_play_cb received AUDIO_OK");
      }
    }
    break;
    
    case VOICE_BUFFERING:
    {
      T_AUDIO_VBUF_PCM_PLAY_STATUS *voice_buffering_play_status;
      voice_buffering_play_status = (T_AUDIO_VBUF_PCM_PLAY_STATUS *)parameter;
      
      if (voice_buffering_play_status->status != AUDIO_OK)
      {
        TRACE_EVENT("ERROR : voice_memo_riv_play_cb received AUDIO_ERROR");
      }
      else
      {
        TRACE_EVENT("INFO : voice_memo_riv_play_cb received AUDIO_OK");
        voice_buffering_data.buffering_phase = FALSE;
	 voice_buffering_data.play_stopped = TRUE;
      }

      //x0pleela 06 Jul, 2006  DR: OMAPS00067709
      //Fix from AS team
      vocoder_mute_ul(0);
	  
      //x0pleela 03 Apr, 2006  ER: OMAPS00067709
      //Set voice memo type to NONE 
      set_voice_memo_type(NONE); 
    }   
    
    break;
    
    default:
    break;
    }
	

	
#endif /* FF_PCM_VM_VB */

/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#endif /* NEPTUNE_BOARD*/
/* END ADD: Req ID: : Sumit : 14-Mar-05 */

}

//x0pleela 24 Feb, 2006  ER OMAPS00067709
#ifdef FF_PCM_VM_VB

/*******************************************************************************

 $Function:     pcm_voice_memo_play

 $Description: Sets voice memo type to PCM_VOICE_MEMO which indicates PCM voice memo is 
 			active and start playing the voice

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int  pcm_voice_memo_play (MfwMnu* m, MfwMnuItem* i)
{
  T_MFW_HND win = mfw_parent(mfw_header());

  TRACE_FUNCTION ("pcm_voice_memo_play()");

  //x0pleela 27 Feb, 2006  ER: OMAPA00067709
  //Set this variable to PCM_VOICE_MEMO which indicates PCM voice memo is active
    set_voice_memo_type( PCM_VOICE_MEMO);
  //create a new voice memo dialog
  voice_memo_start (win, VM_PLAY);
  return 1;  
}

/*******************************************************************************

 $Function:     pcm_voice_memo_record

 $Description: Sets voice memo type to PCM_VOICE_MEMO which indicates PCM voice memo is 
 			active and start recording the voice


 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int pcm_voice_memo_record (MfwMnu* m, MfwMnuItem* i)
{
  T_MFW_HND win = mfw_parent(mfw_header());

  TRACE_FUNCTION ("pcm_voice_memo_record()");

  //x0pleela 27 Feb, 2006  ER: OMAPA00067709
  //Set this variable to PCM_VOICE_MEMO which indicates PCM voice memo is active
  set_voice_memo_type( PCM_VOICE_MEMO);
  //create a new voice memo dialog
  voice_memo_start (win, VM_RECORD);
  return 1;
}

/*******************************************************************************

 $Function:     voice_buffering_edit_num

 $Description: Opens an edtor for the user to enter PTT number. Also resets voice buffering data 
 			with default values

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int  voice_buffering_edit_num (MfwMnu* m, MfwMnuItem* i)
{
  T_MFW_HND win = mfw_parent(mfw_header());
  T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
  tBookStandard   *data       = (tBookStandard *) win_data->user;
  T_phbk          *Phbk       = data->phbk;

  TRACE_FUNCTION ("voice_buffering_edit_num()");
  
  set_voice_memo_type( VOICE_BUFFERING );
  
  memset( Phbk->phbk->edt_buf_number, '\0', PHB_MAX_LEN );
  Phbk->input_number_win = inputVoiceMemoNumberEditor( win, Phbk->edt_buf_number );

  //Reset voice buffering data with default values
  voice_buffering_data_reset();
  
  /* Always return event consumed */
  return MFW_EVENT_CONSUMED;  
}

/*******************************************************************************

 $Function:     voiceMemoNumberCB

 $Description:  Call back for number entry

 $Returns:    None

 $Arguments:  win, parent window, reason, for call back

*******************************************************************************/

void voiceMemoNumberCB( T_MFW_HND win, USHORT Identifier, UBYTE reason )
{
  T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
  tBookStandard   *data       = (tBookStandard *) win_data->user;
  
  TRACE_FUNCTION("voiceMemoNumberCB()");

  switch ( reason )
  {
    case INFO_KCD_LEFT:
    {
      //copy phone number to global
      memset( voice_buffering_data.vb_PTTnum, 0, PHB_MAX_LEN );
      memcpy( voice_buffering_data.vb_PTTnum, data->phbk->edt_buf_number, strlen(data->phbk->edt_buf_number) );
      SEND_EVENT( data->phbk->input_number_win, E_ED_DESTROY, 0, 0 );
      data->phbk->input_number_win = 0;
    }
    break;
  
    case INFO_KCD_RIGHT:
    case INFO_KCD_HUP:
    {
      AUI_edit_Destroy( data->phbk->input_number_win );
      data->phbk->input_number_win = 0;
    }
    break;
      
    default:
    {
      /* otherwise no action to be performed
      */
    break;
    }
  }
}
/*******************************************************************************

 $Function:     inputVoiceMemoNumberEditor

 $Description: Settings of Editor

 $Returns:

 $Arguments:

*******************************************************************************/
MfwHnd inputVoiceMemoNumberEditor( MfwHnd parent, void *buffer )
{
  T_AUI_EDITOR_DATA editor_data;
  
  TRACE_FUNCTION ("inputVoiceMemoNumberEditor()");
  
  AUI_edit_SetDefault(&editor_data);
  AUI_edit_SetDisplay(&editor_data, NUMBER_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
  AUI_edit_SetEvents(&editor_data, 0, FALSE, FOREVER, (T_AUI_EDIT_CB)voiceMemoNumberCB);
  AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtEnterNumber, NULL);
  AUI_edit_SetAltTextStr(&editor_data, 0, NULL, TRUE, TxtSoftBack);
  AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)buffer, NUMBER_LENGTH);
  AUI_edit_SetMode(&editor_data, 0, ED_CURSOR_UNDERLINE);
  
  return AUI_edit_Start(parent, &editor_data);
}

/*******************************************************************************
 $Function:     voice_buffering_record_start

 $Description: Starts recording voice and conects to the PTT number if the call is not active. 
 			If in call, then start playing the voice

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int  voice_buffering_record_start (MfwMnu* m, MfwMnuItem* i)
{
  T_MFW_HND win = mfw_parent(mfw_header());
  SHORT	mfw_aud_retVal;

  TRACE_FUNCTION ("voice_buffering_record_start()");
  TRACE_EVENT_P1("PTT#: %s", voice_buffering_data.vb_PTTnum);

  set_voice_memo_type( VOICE_BUFFERING );

  //store window handler
  voice_buffering_data.win_hnd = win;

  if( voice_buffering_data.recording_possible )
  {
    //create timer
    voice_buffering_data.voice_buffering_tim= tim_create (win, 20000, 
      													  (T_MFW_CB)voice_buffering_tim_cb);
    //start timer
    timStart(voice_buffering_data.voice_buffering_tim);
    voice_buffering_data.buffering_phase = TRUE;
    voice_buffering_data.recording_possible = FALSE;

    mfw_aud_retVal = mfw_aud_vm_start_record(PCM_VOICE_MEMO_MAX_DURATION, 
  											voice_memo_riv_record_cb);  
    if(voice_buffering_data.call_active )
    {
      //start playing the recorded voice
      voice_buffering_playback_start(); 
    }
    else
    {
      //call PTT number
      callNumber((UBYTE*)voice_buffering_data.vb_PTTnum);
    }

    // If the Riviera call failed
    if (mfw_aud_retVal EQ MFW_AUD_VM_OK)
    {
       //no action to be performed
    }
    else
    {
      if (mfw_aud_retVal == MFW_AUD_VM_MEM_FULL)
      {
        TRACE_EVENT ("memory full ");
        /* Silent Implementation */
	 /* play Keybeep */
	 audio_PlaySoundID(0, TONES_KEYBEEP, 200, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */

        voice_memo_dialog_create( win, TxtRecording, TxtFull );
      }
      else
      {
        //Display a dialog and exit
        TRACE_EVENT_P1 ("ERROR : voice_buffering_record_start failed with return value : %d", mfw_aud_retVal);
	    
	 voice_memo_dialog_create( win, TxtRecording, TxtFailed );
      }
    }
  }
  else
  {
    //Display a dialog saying "Already Recording"
    voice_memo_dialog_create(win, TxtAlready, TxtRecording);    
  }
  win_show (win);
  return 1;  
}

/*******************************************************************************

 $Function:     voice_buffering_record_stop

 $Description: Stops the timer and recording. If call is not active the disconnects the call

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL int  voice_buffering_record_stop (MfwMnu* m, MfwMnuItem* i)
{
  T_MFW_HND win = mfw_parent(mfw_header());

  TRACE_FUNCTION ("voice_buffering_record_stop()");

  set_voice_memo_type ( VOICE_BUFFERING );

  //store window handler
  voice_buffering_data.win_hnd = win;
  
  voice_buffering_data.rec_stop_reason = USER_SEL_STOP;
  voice_buffering_data.recording_possible = TRUE;

  if(!voice_buffering_data.call_active )
  {
    //stop the timer, if recording
    timStop(voice_buffering_data.voice_buffering_tim);
    
    /* destroy the redial windows if exit*/
    cm_redial_abort();
    cm_disconnect(call_data.outCall);
    
    if (!call_data.calls.numCalls)
    {
      call_data.ignore_disconnect = TRUE;
      call_destroy(call_data.win);
    }
  }
  else
  {
    //Stop recording
    voice_buffering_stop_recording();
  }
     
  return 1;  
}

/*******************************************************************************

 $Function:     voice_buffering_playback_start

 $Description: Starts playing the voice. If any error while playing, stops timer and recording

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL void voice_buffering_playback_start(void )
{
  SHORT	mfw_aud_retVal;

  TRACE_FUNCTION ("voice_buffering_playback_start()");

  voice_buffering_data.buffering_phase = TRUE;
  mfw_aud_retVal = mfw_aud_vm_start_playback(voice_memo_riv_play_cb);
  if (mfw_aud_retVal == MFW_AUD_VM_OK)
  {
     //no action to be performed
  }
  else
  {
    if (mfw_aud_retVal == MFW_AUD_VM_MEM_EMPTY)
    {
      voice_memo_dialog_create(voice_buffering_data.win_hnd, TxtRecording, TxtEmpty);
    }
    else
    {
      //Display a dialog and exit
      TRACE_EVENT_P1 ("ERROR : mfw_aud_vm_start_playback failed with return value : %d", mfw_aud_retVal);
      voice_memo_dialog_create( voice_buffering_data.win_hnd, TxtPlayback, TxtFailed);

      //update the reason to stop recording
      voice_buffering_data.rec_stop_reason = PLAY_ERROR;

      //stop recording 
      voice_buffering_stop_recording();
    }
  }
  return;
}

/*******************************************************************************

 $Function:     voice_buffering_stop_recording

 $Description: A wrapper function for record stop to invoke from other modules. 
 			Stops the timer and recording

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL void voice_buffering_stop_recording(void )
{
  SHORT	mfw_aud_retVal;

  TRACE_FUNCTION ("voice_buffering_stop_recording()"); 

  //stop the timer, if recording
  timStop(voice_buffering_data.voice_buffering_tim);
  
  mfw_aud_retVal = mfw_aud_vm_stop_record(voice_memo_riv_record_cb); 

  if (mfw_aud_retVal != MFW_AUD_VM_OK)
  {
    TRACE_EVENT_P1 ("ERROR : voice_buffering_record_stop failed with return value : %d", mfw_aud_retVal);

    //display dialog saying "Recording Stop Failed"
    voice_memo_dialog_create( voice_buffering_data.win_hnd, TxtRecord, TxtStopFailed );
  }
  return;
}

/*******************************************************************************

 $Function:     voice_buffering_stop_playing

 $Description: A wrapper function for play stop to invoke from other modules. 

 $Returns:

 $Arguments:

*******************************************************************************/
GLOBAL void voice_buffering_stop_playing(void )
{
  SHORT	mfw_aud_retVal;

  TRACE_FUNCTION ("voice_buffering_stop_playing()"); 
  
  mfw_aud_retVal = mfw_aud_vm_stop_playback(voice_memo_riv_play_cb); 

  if (mfw_aud_retVal != MFW_AUD_VM_OK)
  {
    TRACE_EVENT_P1 ("ERROR : voice_buffering_record_stop failed with return value : %d", mfw_aud_retVal);

    //display dialog saying "Recording Stop Failed"
    voice_memo_dialog_create( voice_buffering_data.win_hnd, TxtPlay, TxtStopFailed );
  }
  return;
}


/*******************************************************************************

 $Function:     voice_memo_tim_cb

 $Description: Timer callback. Stops the timer. If PTT call is not setup disconnect the call, 
 			else call record stop

 $Returns:

 $Arguments:

*******************************************************************************/
static int voice_buffering_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_voice_buffering   * data = (T_voice_buffering *)win_data->user;
  SHORT	mfw_aud_retVal;

   TRACE_FUNCTION("voice_buffering_tim_cb()");
  //get the voice memo type
  if( ( get_voice_memo_type() EQ VOICE_BUFFERING ) AND ( data->voice_buffering_tim ) )
  {
    timStop(voice_buffering_data.voice_buffering_tim);
  }
  voice_buffering_data.rec_stop_reason = RECORDING_TIMEOUT;  
  
  if( !voice_buffering_data.call_active ) //timeout before call setup
  {
    cm_disconnect(call_data.outCall);
    
    if (!call_data.calls.numCalls)
    {
      call_data.ignore_disconnect = TRUE;
      call_destroy(call_data.win);
    }
  }
 else  //timeout after call setup
 {  
    mfw_aud_retVal = mfw_aud_vm_stop_record(voice_memo_riv_record_cb); 
    
    if (mfw_aud_retVal != MFW_AUD_VM_OK)
    {
      TRACE_EVENT_P1 ("ERROR : mfw_aud_vm_stop_record failed with return value : %d", mfw_aud_retVal);
  
      //display dialog saying "Recording Stop Failed"
      voice_memo_dialog_create( voice_buffering_data.win_hnd, TxtRecording, TxtStopFailed );
    }
  }

 return  MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     voice_buffering_data_reset

 $Description: Resets voice buffering data to default values

 $Returns:

 $Arguments:

*******************************************************************************/
static void voice_buffering_data_reset( void )
{
  TRACE_FUNCTION("voice_buffering_data_reset()");

  voice_buffering_data.win_hnd = 0;
  voice_buffering_data.voice_buffering_tim = 0;
  memset( voice_buffering_data.vb_PTTnum, 0, PTT_NUM_LEN );
  voice_buffering_data.rec_stop_reason = VB_NONE;
  voice_buffering_data.call_active = FALSE;
  voice_buffering_data.recording_possible = TRUE;
  voice_buffering_data.buffering_phase = FALSE;
  voice_buffering_data.incoming_call_discon = FALSE;
  voice_buffering_data.play_stopped = FALSE;
  
}
#endif
