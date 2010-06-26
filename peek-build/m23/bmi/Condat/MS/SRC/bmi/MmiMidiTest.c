/*******************************************************************************

          Texas Instruments

********************************************************************************

 This software product is the property of Texas Instruments and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Test MMI
 $Module:   MIDI TEST
 $File:       MmiMidiTest.c
 $Revision:   1.0

 $Author:   Elsa Armanet-Said Texas Instruments Nice
 $Date:       17/05/04

********************************************************************************

 Description:

    This provides the start code for the MMI

********************************************************************************
 $History: MmiMidiTest.c

 	Aug 01, 2007 REF:DRT OMAPS00137370 x0045876
   	Description: MM: MIDI/IMY file is not played correctly after listening a 
   				 MP3 in the Browse Midi list.
   	Solution: Param Voice_Limit is changed when MP3 or AAC file is played. So before 
   			  playing MIDI file, Voice_Limit should be initialized properly.
    Mar 30, 2007    REF: OMAPS00122691  x0039928
    Description: COMBO: No melody heard on parsing over different volums in tones menu
    Solution: Filename is passed with complete path to the audio service api.
    
 	Jan 30, 2007 DR: OMAPS00108892 x0pleela
 	Description: TCMIDITEST.23(Apps) - Play All File feature fails when different Configurations
 				like Mono/Stereo, Loop On/Off are selected
 	Solution: 	When Loop On is enabled, and when user selected to Play all files, an error message 
 			will be displayed saying Loop ON, cannot play all files. Changes aremade in functions
 			midi_test_play_all() and midi_test_set_loop_on()
 	

 	Sep 11, 2006 DR: OMAPS00094182 xrashmic
 	Description: midi and mp3 files is not being populated on accessing corresponding menu, but only during bootup
 	Solution: Populate files on accessing the "browse files" list
 	
    June 19, 2006    REF: DRT OMAPS00076378  x0012849 : Jagannatha
    Description: MIDI files should play while browsing
    Solution: The Menu up and down key event will be handled and the current 
     file will be played while browsing the files.

	Nov 23, 2005 REF : DR OMAPS00057378   x0039928
	Bug: All MIDI files are not displayed in MMI in locosto-lite
	Fix: Buffer memory allocated to copy the file contents is freed
	properly and copying file from FFS to LFS is not terminated if error is
	returned for just any one file, instead it tries to copy other files.
	
 	Nov 09, 2005    REF:DR OMAPS00049167  x0039928
	Description:   The midi files cannot be heard if the audio channel selected is 'Mono'
	Solution: Midi Player params are made global so that settings are retained across.

 	Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	Description:   Mp3 Ringer Implementation
	Solution: The Midi Ringer application has been enhanced to support mp3 as ringer.
	The interface  function names have been modified.
	
	Sep 29,2005 REF: SPR 34407 xdeepadh  
	Bug:Back soft key takes more time to return to the previous screen in Midi application
	Fix:The list creation for  Midi files has been modified
	
	Aug 29, 2005    REF: SPR 33999 xdeepadh
	Description:Midi Application - Play all files asks for a file to be selected
	Solution: When the files are not selected, the current playing index is set 0.
	
	Apr 05, 2005    REF: ENH 29994 xdeepadh
	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
	Solution: Generic Midi Ringer and Midi Test Application were implemented.
	
	Dec 08, 2004	REF: CRR MMI-SPR-27284
	Description: MIDI Player: Back Key malfunctions in MIDI application.
	Solution: A window is created to hold list window. When the user presses Back key, the newly 
		    created window is deleted along with list window, bringing the control back to Midi options
		    screen.

  17/05/04      

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

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "message.h" /* included for aci_cmh.h .... */
#include "aci_cmh.h" /* included for mfw_sat.h, included for MmiDummy.h, included for MmiMenu.h */

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_icn.h"	/* included for mfw_mnu.h */
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_kbd.h"
#include "mfw_sat.h" /* included for MmiDummy.h, included for MmiMenu.h */
#include "mfw_midi.h"

#include "kpd/kpd_api.h"

#include "MmiBlkLangDB.h"
#include "mmiColours.h"
#include "MmiDialogs.h"

#include "MmiDummy.h" /* included for MmiMenu.h */
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "AUIEditor.h"
#include "MmiMidiTest.h"
#include "MmiMain.h"
#include "Mmisounds.h"
#include "MmiBookShared.h" //Sep 11, 2006 DR: OMAPS00094182 xrashmic
/*******************************************************************************

                                internal data

*******************************************************************************/

/*
 * Internal events
 */
#define MT_CREATE_EDITOR         500
#define MT_EDITOR_CANCEL         501
#define MT_EDITOR_SELECT         502
#define MT_NAME_EDITOR           503
#define MT_MNU_INIT              504

/*local defines*/

#define MT_MAX_EDITOR_LENGTH	4
#define MT_INFO_SCRN_TIMEOUT  1500
#define MT_MAX_FILES             8

//	Dec 08, 2004	REF: CRR MMI-SPR-27284
//	Description: MIDI Player: Back Key malfunctions in MIDI application.
//	E_INIT event definition
#define E_INIT 1

#define TIME_HALFSECOND 500

/*editor data structure*/
typedef struct{
	T_MMI_CONTROL	mmi_control;
	MfwHnd			win;
	MfwHnd			mt_win_editor;
	UBYTE			mt_buffer[MT_MAX_EDITOR_LENGTH+1];
	USHORT                         mt_voice_number;
}	T_MMI_MT_Editor_Data;

/*menu data structure definition*/
typedef struct
{
	T_MMI_CONTROL          mmi_control;
	T_MFW_HND                 win;
	T_MFW_HND                 kbd;
	T_MFW_HND                 kbd_long;
	T_MFW_HND                 mt_menu;
	T_MFW_HND			parent;		// Dec 08, 2004	REF: CRR MMI-SPR-27284. Member "parent" added
	T_VOID_FUNC              func_cb;
	T_VOID_FUNC	            	cancel_cb;
	UBYTE				mt_num_file;
	UBYTE				mt_file_count;
	ListCbFunc			mt_list_cb;
	UBYTE				mt_list_idx;
	UBYTE                     	IsMfwInit;
	ListMenuData * 		menu_list_data; //Sep 29,2005 REF: SPR 34407 xdeepadh  
}T_mt_menu;

/*global menu data*/
T_mt_menu mt_menu_data = {0};
extern UBYTE mfw_player_exit_flag;//Flag to check playall is selected.
extern int mfw_player_currently_playing_idx;//Stores the index of the current selected file
extern UBYTE mfw_player_playback_loop_flag;//Flag to check whether loop is on /off
// June 19, 2006 REF: DRT OMAPS00076378  x0012849
// This timer is created so that a file is played only if the item is 
// selected for more than half a second
static MfwHnd  midiBrowseTimer;
// This variable is created to know Currently list of MIDI files are showed or not.
static BOOL isMidiListON = FALSE;
/*******************************************************************************

                                Local prototypes

*******************************************************************************/
static void midi_mt_menu_list_listmnu_cb(T_MFW_HND Parent, ListMenuData * ListData);
static void mmi_midi_test_editor_cb(T_MFW_HND win, USHORT identifier, SHORT reason);
//Sep 29,2005 REF: SPR 34407 xdeepadh  
static T_MFW_HND  mmi_midi_test_build_Menu( MfwHnd parent_window);


/*default static menu*/
static MfwMnuAttr menuMtSearchListAttr =
{
	&menuArea,
	MNU_LEFT | MNU_LIST | MNU_CUR_LINE,		/* centered page menu */
	(U8)-1,										/* use default font  */
	NULL,									/* with these items (filled in later) */
	0,										/* number of items (filled in leter)  */
	COLOUR_LIST_XX,							/* Colour */
	TxtNull,								/* Hdr Id */
	NULL,									/* Background */
	MNUATTRSPARE							/* unused */
};

/*******************************************************************************

 $Function:     mmi_midi_test_show_info

 $Description: Display the Dialog

 $Returns:

 $Arguments:

*******************************************************************************/
static MfwHnd mmi_midi_test_show_info(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION ("mmi_midi_test_show_info()");

	/*
	** Create a timed dialog to display the Message "Failed"
	*/
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, MT_INFO_SCRN_TIMEOUT, KEY_LEFT | KEY_CLEAR | KEY_HUP);

	return info_dialog(parent, &display_info);
}


/*******************************************************************************

 $Function:     midi_stop_player

 $Description:Stops the Midi file playing

 $Returns:

 $Arguments:m, menu being activated, i, item in menu

*******************************************************************************/
GLOBAL int midi_stop_player (MfwMnu* m, MfwMnuItem* i)
{

	TRACE_FUNCTION ("midi_stop_player()");
	if(mfw_player_exit_flag  == PLAYER_EXIT_FLAG_FALSE)
		mfw_player_exit_flag  = PLAYER_EXIT_FLAG_TRUE;  
	mfw_player_stop(sounds_midi_player_stop_cb);
	return 1;
}

/*******************************************************************************

 $Function:		mmi_midi_test_editor_cb

 $Description:		This function provides the callback functionality to an editor for Midi test

 $Returns:		None

 $Arguments:	
 
*******************************************************************************/

static void mmi_midi_test_editor_cb(T_MFW_HND win, USHORT identifier, SHORT reason)
{
	T_MFW_WIN		* win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_MT_Editor_Data* dev_name_data = (T_MMI_MT_Editor_Data *)win_data->user;
	UBYTE			* editor_buffer = NULL;
	SHORT			max_edit_size = 0;
	T_MFW_HND		parent;


	TRACE_FUNCTION ("mmi_midi_test_editor_cb()");


	if (identifier == MT_NAME_EDITOR)
	{
	editor_buffer = dev_name_data->mt_buffer;
	max_edit_size = MT_MAX_EDITOR_LENGTH;
	parent = dev_name_data->win;
	}
	else
	{
	TRACE_FUNCTION ("mmi_midi_test_editor_cb() : Unknown Identifier");
	/*
	** if this isn't one of the "recognised" editors leave now!
	*/
	return;
	}

	switch (reason)
	{
	case INFO_KCD_LEFT:
	{	
	/*
	** If there is no string entered, treat this as if it was a "Cancel"
	*/
	if (editor_buffer[0] == 0x00)
	{
		/*
		** Send a "Cancel" to the Midi test window
		*/
		SEND_EVENT(parent, MT_EDITOR_CANCEL, identifier, NULL);
	}
	else
	{
		/*
		** Send a "Select" to the Midi test window
		*/
		SEND_EVENT(parent, MT_EDITOR_SELECT, identifier, NULL);
	}
	}
	break;

	case INFO_KCD_RIGHT:
	if (strlen((char *)editor_buffer) == 0)
	{

		/*
		** Send a "Cancel" to the Midi test window
		*/
		SEND_EVENT(parent, MT_EDITOR_CANCEL, identifier, NULL);
	}
	break;

	case INFO_KCD_HUP:
	/*
	** Set the buffer to NULLS
	*/
	memset(editor_buffer, 0x00, max_edit_size);

	/*
	** Send a "Cancel" to the  Midi test window
	*/
	SEND_EVENT(parent, MT_EDITOR_CANCEL, identifier, NULL);
	break;

	default:
	/* otherwise no action to be performed
	*/
	break;
	}
}


/*******************************************************************************

 $Function:		mmi_midi_test_editor_destroy

 $Description:		This function performs the necessary steps to remove the window handling the
 				voice number editor, tidying up all allocated resources.

 $Returns:		MfwResOk : Success
 				MfwResIllHnd : Illegal Window Handler provided

 $Arguments:		win : Window to be destroyed

*******************************************************************************/
MfwRes mmi_midi_test_editor_destroy(T_MFW_HND win)
{
	T_MFW_WIN * win_data;
	T_MMI_MT_Editor_Data * data;
			
	TRACE_FUNCTION ("mmi_midi_test_editor_destroy");


	if (win == (void *)0)
		return MfwResIllHnd;

	win_data = ((T_MFW_HDR *) win)->data;
	data = (T_MMI_MT_Editor_Data *)win_data->user;

	/*
	** Destroy the Window
	*/
	win_delete(data->win);

	/*
	** Free the dynamically allocated memory
	*/
	mfwFree((U8 *)data, (U16)sizeof(T_MMI_MT_Editor_Data));

	return MfwResOk;
}


/*******************************************************************************

 $Function:		mmi_midi_test_editor_cntrl

 $Description:		This is the dialog control function for the voice number selection Window. It
 				receives the signals from the MFW and determines what action, if any, to take.

 $Returns:		None

 $Arguments:		win :		The pointer to the window handler, so the function can reference the
							dynamic user data, if any.
 				event :		The incoming event
 				value :		Generic incoming Parameter, it will be 'event' dependant
 				parameter :	Generic incoming Parameter, it will be 'event' dependant

*******************************************************************************/
void mmi_midi_test_editor_cntrl (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN			* win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_MT_Editor_Data * data = (T_MMI_MT_Editor_Data *)win_data->user;
	T_AUI_EDITOR_DATA	editor_data;
	SHORT	mfw_midi_retVal;
	int voice_limit;

	switch (event)
	{
	case MT_CREATE_EDITOR:
#ifdef NEW_EDITOR
	              //Clear the editor buffer
		memset(&editor_data,'\0',sizeof(editor_data));

		/*
		** Initialise the editor
		*/
		AUI_edit_SetDefault(&editor_data);
		AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, data->mt_buffer, MT_MAX_EDITOR_LENGTH);
		AUI_edit_SetTextStr(&editor_data, TxtSoftSelect, TxtSoftBack, TxtMidiEnterVoiceNumber, NULL);
		AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, TRUE, TxtCancel);
		AUI_edit_SetEvents(&editor_data, MT_NAME_EDITOR, TRUE, FOREVER, (T_AUI_EDIT_CB)mmi_midi_test_editor_cb);
		data->mt_win_editor = AUI_edit_Start(data->win, &editor_data);
#endif
		break;

	case MT_EDITOR_CANCEL:

		if (value == MT_NAME_EDITOR)
		{
			/*
			** The editor will have been destroyed. Reset the Window handler
			*/
			data->mt_win_editor = (void *)0;
		}
		else
		{
		      TRACE_FUNCTION ("mmi_midi_test_editor_cntrl(), MT_EDITOR_CANCEL event received with unexpected value" ); 
		}

		/*
		** Destroy the editor Window
		*/
		TRACE_FUNCTION ("mmi_midi_test_editor_cntrl cancel");
		mmi_midi_test_editor_destroy(data->win);
		break;

	case MT_EDITOR_SELECT:
		if (value == MT_NAME_EDITOR)
		{
			voice_limit=atoi((char*)data->mt_buffer);
			TRACE_EVENT_P1("voice_limit is %d",voice_limit);
			if(voice_limit >32 ||voice_limit <=0)
			{
				mmi_midi_test_show_info(0, TxtError, TxtNull, NULL);
			}
			else
			{
				//translate ASCII format into integer format
				mfw_midi_retVal = mfw_player_midi_set_voice_limit(voice_limit);

				/* Aug 01, 2007    DRT: OMAPS00137370  x0045876 */
				/* To display message, when Voice Limit is changed through Menu */
				if(mfw_midi_retVal == MFW_PLAYER_TEST_OK)
				{
					mmi_midi_test_show_info(0, TxtChConfigVoiceLimit, TxtSelected, NULL);
				}
			}
		}
	    else
		{
		      TRACE_FUNCTION ("mmi_midi_test_editor_cntrl(), MT_EDITOR_SELECT event received with unexpected value"); 
		}
		TRACE_FUNCTION ("mmi_midi_test_editor_cntrl select");

		/*
		** Destroy the Editor Window
		*/
		mmi_midi_test_editor_destroy(data->win);
		break;

	default:
		TRACE_FUNCTION ("mmi_midi_test_editor_cntrl(), Unexpected Event!"); 
		break;
	}

}



/*******************************************************************************

 $Function:		midi_test_editor_create

 $Description:		This function performs the necessary steps to create the a window to enter the number of voices.
 It will be removed, or rather remove itself on returning the
 				data to the Mfw.
 				
 $Returns:		T_MFW_HND : Window Handle to the New Window, Null if failed.

 $Arguments:		parent : pointer to the parent window.

*******************************************************************************/
T_MFW_HND midi_test_editor_create(T_MFW_HND parent)
{
	T_MMI_MT_Editor_Data *data;
	T_MFW_WIN * win;

	TRACE_FUNCTION ("midi_test_editor_create");

	data = (T_MMI_MT_Editor_Data *)mfwAlloc((U16)sizeof(T_MMI_MT_Editor_Data));

	if (data == (void *)0)
		return data;

	data->win = winCreate(parent, 0, E_WIN_VISIBLE, NULL);

	if (data->win == (void *)0)
	{
		/*
		** Failed to start : Free Memory, and exit
		*/
		mfwFree((U8 *)data, (U16)sizeof(T_MMI_MT_Editor_Data));
		data = (void *)0;
		return data;
	}
	else
	{
		/*
		** Setup the Dialog control functions
		*/
		data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_midi_test_editor_cntrl;
		data->mmi_control.data = data;
		
		win = ((T_MFW_HDR *)data->win)->data;
		win->user = (void *)data;
	}

	return data->win;

}

/*a0393213 lint warning removal - This function is not used currently. So it's put under the flag. If it is required just remove the flag.*/
#ifdef MMI_LINT_WARNING_REMOVAL
/*******************************************************************************

 $Function:     mt_menu_win_cb

 $Description: menu window callback (implementation of softkeys)

 $Returns:

 $Arguments:

*******************************************************************************/
static int mt_menu_win_cb (MfwEvt e, MfwWin *w)
{
	T_mt_menu * data = (T_mt_menu *)w->user;
	MfwMnu * mnu;

	TRACE_FUNCTION ("mt_menu_win_cb ");

	switch (e)
	{
		case MfwWinVisible:
			mnu = (MfwMnu *)mfwControl(data->mt_menu);
			softKeys_displayId(TxtSoftSelect, TxtSoftBack, 0, mnu->curAttr->mnuColour );
			break;

		default:
			return 0;
	}
	return 1;
}
#endif

/*******************************************************************************

 $Function:     mt_menu_destroy

 $Description: destroy menu window

 $Returns:

 $Arguments: window to be destroyed

*******************************************************************************/
static void mt_menu_destroy(MfwHnd window)
{
	T_MFW_WIN     * win_data  = ((T_MFW_HDR *)window)->data;
	T_mt_menu * data = (T_mt_menu *)win_data->user;

	TRACE_FUNCTION ("mt_menu_destroy");

	if (window == NULL)
	{
		return;
	}

	if (data)
	{
		win_delete(data->win);
		
		/*
		** Free Memory
		*/
		mfwFree((void *)data, sizeof (T_mt_menu));
	}
}



// June 19, 2006 REF: DRT OMAPS00076378  x0012849
/*******************************************************************************

 $Function:    midi_test_menu_status

 $Description: Returns TRUE if the list of MIDI files is showed currently on display.

 $Returns:     TRUE or FALSE

 $Arguments:   None

*******************************************************************************/
BOOL midi_test_menu_status()    
{
    return isMidiListON;
}



//Sep 29,2005 REF: SPR 34407 xdeepadh  
/*******************************************************************************

 $Function:     midi_mt_menu_list_listmnu_cb

 $Description: destroy lists and menu in case of back softkey or menu exit

 $Returns:

 $Arguments:

*******************************************************************************/
static void midi_mt_menu_list_listmnu_cb(T_MFW_HND Parent, ListMenuData * ListData)
{
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)Parent)->data;
	T_mt_menu	*data		= (T_mt_menu *)win_data->user;

	TRACE_FUNCTION ("midi_mt_menu_list_listmnu_cb()");


	if ((ListData->Reason EQ LISTS_REASON_BACK) ||
	     (ListData->Reason EQ LISTS_REASON_CLEAR) ||
	     (ListData->Reason EQ LISTS_REASON_HANGUP))
	{
		/*
		** Destroy the window --- Not sure if this is required! ... leave it out for now
		*/
		listsDestroy(ListData->win);

		/*
		** Free the memory set aside for ALL the devices to be listed.
		*/
		if (data->menu_list_data != NULL)
		{
			FREE_MEMORY ((void *)data->menu_list_data->List, (U16)(data->mt_num_file * sizeof (T_MFW_MNU_ITEM)));
			FREE_MEMORY ((void *)data->menu_list_data, sizeof (ListMenuData));
		}

		mt_menu_destroy(data->win);
        // June 19, 2006 REF: DRT OMAPS00076378  x0012849
        /*At this point the MIDI list is destroyed*/
        isMidiListON = FALSE;
        /* At this point the play need to be stopped. If user wants the file need to
        be played while going across menu then 'play' or 'play all' need to be selected */
        midi_stop_player (NULL, NULL);
        if (midiBrowseTimer != NULL)
        {
            // Delete the timer. It's not required any more.
            timDelete (midiBrowseTimer);
        }
	}
	//Sep 29,2005 REF: SPR 34407 xdeepadh  
	else if(ListData->Reason EQ LISTS_REASON_SELECT)
	  {
		TRACE_EVENT_P1("list position is %d",ListData->ListPosition);
		mt_menu_data.mt_list_idx = ListData->ListPosition;
		//call mfw to save selected index
		mfw_player_save_selected_file_idx(mt_menu_data.mt_list_idx);
		//display a "file selected" text here
		mmi_midi_test_show_info(0, TxtMidiFileSelected, TxtNull, NULL);
	  }

	return;
}



/*******************************************************************************

 $Function:     midi_test_set_loop_on

 $Description: set loop on

 $Returns:

 $Arguments:menu, menu item

*******************************************************************************/
GLOBAL int midi_test_set_loop_on (MfwMnu* m, MfwMnuItem* i)
{
	SHORT	mfw_midi_retVal;

	TRACE_FUNCTION ("midi_test_set_loop_on()");

	//x0pleela 29 Jan, 2007  DR: OMAPS00108892
	//Check if it is playing all files.
	//If playing, and user wants to activate Loop On, display error message. 
	//But keep playing all the files
	if( mfw_player_exit_flag == PLAYER_EXIT_FLAG_FALSE )
	{
	  //Display an error message
	  mmi_midi_test_show_info(0, TxtLoopOn, TxtCantBeActivated, NULL);
	  return 1;	  
	}

	mfw_midi_retVal =   mfw_player_set_loop_on();
	if(mfw_midi_retVal != MFW_PLAYER_TEST_OK)
	{
		TRACE_FUNCTION ("midi_test_set_loop_on(): Riviera Failed"); 
		mmi_midi_test_show_info(0, TxtMidiOptionNotImplemented, TxtNull, NULL);
	}
	else
	{
		mmi_midi_test_show_info(0, TxtMidiLoopOn, TxtNull, NULL);
	}
	return 1;
}

/*******************************************************************************

 $Function:     midi_test_set_loop_off

 $Description: set loop off

 $Returns:

 $Arguments:menu, menu item

*******************************************************************************/
GLOBAL int midi_test_set_loop_off (MfwMnu* m, MfwMnuItem* i)
{
	SHORT	mfw_midi_retVal;

	TRACE_FUNCTION ("midi_test_set_loop_off()");

	mfw_midi_retVal =   mfw_player_set_loop_off();
	if(mfw_midi_retVal != MFW_PLAYER_TEST_OK)
	{
		  TRACE_FUNCTION ("midi_test_set_loop_off(): Riviera Failed"); 
		  mmi_midi_test_show_info(0, TxtMidiOptionNotImplemented, TxtNull, NULL);
	}
	else
	{
	 	 mmi_midi_test_show_info(0, TxtMidiLoopOff, TxtNull, NULL);
	}
	return 1;
}


/*******************************************************************************

 $Function:     midi_test_set_channel_mono

 $Description: set channels to mono

 $Returns:

 $Arguments:menu, menu item

*******************************************************************************/
GLOBAL int midi_test_set_channel_mono (MfwMnu* m, MfwMnuItem* i)
{

	SHORT	mfw_midi_retVal;

	TRACE_FUNCTION ("midi_test_set_channel_mono()");
	mfw_midi_retVal = mfw_player_set_channel_mono();
	if(mfw_midi_retVal != MFW_PLAYER_TEST_OK)
	{
		  TRACE_FUNCTION ("mfw_player_set_channel_mono(): Riviera Failed"); 
		  mmi_midi_test_show_info(0, TxtMidiOptionNotImplemented, TxtNull, NULL);
	}
	else
	{
		 mmi_midi_test_show_info(0, TxtMidiMonoChannel, TxtNull, NULL);
	}

	return 1;
}

/*******************************************************************************

 $Function:     midi_test_set_channel_stereo

 $Description: set channels to stereo

 $Returns:

 $Arguments:menu, menu item

*******************************************************************************/
GLOBAL int midi_test_set_channel_stereo (MfwMnu* m, MfwMnuItem* i)
{

	SHORT	mfw_midi_retVal;

	TRACE_FUNCTION ("midi_test_set_channel_stereo()");
	mfw_midi_retVal = mfw_player_set_channel_stereo();
	if(mfw_midi_retVal != MFW_PLAYER_TEST_OK)
	{
		  TRACE_FUNCTION ("mfw_player_set_channel_stereo(): Riviera Failed"); 
		  mmi_midi_test_show_info(0, TxtMidiOptionNotImplemented, TxtNull, NULL);
	}
	else
	{
	  	mmi_midi_test_show_info(0, TxtMidiStereoChannel, TxtNull, NULL);
	}

	return 1;
}

/*******************************************************************************

 $Function:     midi_test_set_speaker

 $Description: set audiopath to speaker

 $Returns:

 $Arguments:	menu, menu item


*******************************************************************************/
GLOBAL int midi_test_set_speaker (MfwMnu* m, MfwMnuItem* i)
{

	SHORT	mfw_midi_retVal;
	TRACE_FUNCTION ("midi_test_set_speaker()");

	mfw_midi_retVal=mfw_player_set_speaker();
	if(mfw_midi_retVal != MFW_PLAYER_TEST_OK)
	{
		  TRACE_FUNCTION ("midi_test_set_speaker():  Failed"); 
		  mmi_midi_test_show_info(0, TxtMidiOptionNotImplemented, TxtNull, NULL);
	}
	else
	{
		mmi_midi_test_show_info(0, TxtMidiSpeaker, TxtNull, NULL);
	}

	return 1;
}



/*******************************************************************************

 $Function:     midi_test_set_headset

 $Description: set audiopath to headset

 $Returns:

 $Arguments:menu, menu item

*******************************************************************************/
GLOBAL int midi_test_set_headset (MfwMnu* m, MfwMnuItem* i)
{
	SHORT	mfw_midi_retVal;
	TRACE_FUNCTION ("midi_test_set_headset()");

	mfw_midi_retVal=mfw_player_set_headset();
	if(mfw_midi_retVal != MFW_PLAYER_TEST_OK)
	{
		  TRACE_FUNCTION ("midi_test_set_headset():  Failed"); 
		  mmi_midi_test_show_info(0, TxtMidiOptionNotImplemented, TxtNull, NULL);
	}
	else
	{
		mmi_midi_test_show_info(0, TxtMidiHeadset, TxtNull, NULL);
	}

	return 1;
}



/*******************************************************************************

 $Function:     midi_test_cc_voice_limit

 $Description:Creates the Editor for entering the voice limit number

 $Returns:

 $Arguments:menu, menu item

*******************************************************************************/
GLOBAL int midi_test_cc_voice_limit (MfwMnu* m, MfwMnuItem* i)
{

	T_MFW_HND win = mfw_parent(mfw_header());
	T_MFW_WIN		* win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_MT_Editor_Data * data = (T_MMI_MT_Editor_Data *)(win_data->user);

	TRACE_FUNCTION ("midi_test_cc_voice_limit()");

	data->mt_win_editor = midi_test_editor_create(win);

	if (data->mt_win_editor != (void *)0)
	{
	    SEND_EVENT(data->mt_win_editor, MT_CREATE_EDITOR, 0, (void *)NULL);
	}
	else
	{
	    TRACE_FUNCTION ("midi_test_cc_voice_limit() : Unable to create Editor Win!");
	}
	return 1;
}

/*******************************************************************************

 $Function:     midi_test_play

 $Description:Starts Playing a midi file

 $Returns:

 $Arguments:menu, menu item

*******************************************************************************/
GLOBAL int midi_test_play (MfwMnu* m, MfwMnuItem* i)
{
	T_AS_PLAYER_TYPE player_type;
#ifdef FF_MIDI_LOAD_FROM_MEM
	T_MELODY melody_data;
#else
	char* midi_file;
#ifdef FF_MMI_FILEMANAGER
	char temp[FILENAME_MAX_LEN];
	UINT16 midi_file_uc[FILENAME_MAX_LEN];
#endif
#endif
	TRACE_FUNCTION ("midi_test_play()");

	//If no file is selected inform the user
	if(mfw_player_currently_playing_idx < 0)
	{
		mmi_midi_test_show_info(0, TxtMidiSelectFile, TxtNull, NULL);
		return 1;
		
	}
	//flag set to 1 means we only play the currently selected file
	 mfw_player_exit_flag = PLAYER_EXIT_FLAG_TRUE;  
	//Play the midi player with high volume.
#ifdef FF_MIDI_RINGER  
#ifdef FF_MIDI_LOAD_FROM_MEM
	melody_data=sounds_midi_return_memory_location(mfw_player_currently_playing_idx);
	 player_type=mfw_ringer_deduce_player_type(melody_data.melody_name);
	mfw_player_start_memory(player_type,(UINT32*)melody_data.melody,melody_data.melody_size,(UBYTE)AS_VOLUME_HIGH,mfw_player_playback_loop_flag,sounds_midi_player_start_cb);
#else
	midi_file=sounds_midi_return_file_name(mfw_player_currently_playing_idx);
	strcpy(temp, "/FFS/mmi/tones/");
	strcat(temp, midi_file);
	 player_type=mfw_ringer_deduce_player_type(midi_file);
#ifdef FF_MMI_FILEMANAGER
	convert_u8_to_unicode(temp, midi_file_uc);
	mfw_player_start_file(player_type,midi_file_uc,(UBYTE)AS_VOLUME_HIGH,mfw_player_playback_loop_flag,sounds_midi_player_start_cb);
#else
	mfw_player_start_file(player_type,midi_file,(UBYTE)AS_VOLUME_HIGH,mfw_player_playback_loop_flag,sounds_midi_player_start_cb);
#endif
#endif 	
#endif
	return 1;
}

/*******************************************************************************

 $Function:     midi_test_play_all

 $Description:Plays all the midi files in the FFS one after the other

 $Returns:

 $Arguments:menu, menu item

*******************************************************************************/
GLOBAL int midi_test_play_all (MfwMnu* m, MfwMnuItem* i)
{
	T_AS_PLAYER_TYPE player_type;
	int numberOfMelodies=0;
#ifdef FF_MIDI_LOAD_FROM_MEM
	T_MELODY melody_data;
#else
	char* midi_file;
#ifdef FF_MMI_FILEMANAGER
	char temp[FILENAME_MAX_LEN];
	UINT16 midi_file_uc[FILENAME_MAX_LEN];
#endif
#endif

	TRACE_FUNCTION ("midi_test_play_all()");

	//x0pleela 31 Jan, 2007 DR: OMAPS00108892
	//Reset the index to -1 to avoid playing only the last file.
	mfw_player_currently_playing_idx = -1;

	//x0pleela 29 Jan, 2007  DR: OMAPS00108892
	//Check if Loop is On. If so, display error message. Else allow playing all files
	if( mfw_player_playback_loop_flag )
	{
	  //Display an error message
	  mmi_midi_test_show_info(0, TxtLoopOn, TxtCantPlayAllFiles, NULL);         
	  return 1 ;	  
	}
	//Sep 11, 2006 DR: OMAPS00094182 xrashmic
		 sounds_midi_init();

	numberOfMelodies = sounds_midi_return_file_number();
	TRACE_EVENT_P1("numberOfMelodies for midi_test_play_all is %d",numberOfMelodies);

	if(numberOfMelodies <= 0)
	{
		mmi_midi_test_show_info(0, TxtMidiFileNotLoaded, TxtNull, NULL);
		return 1 ;
		
	}
	if(mfw_player_currently_playing_idx < 0)
	{
		//Aug 29, 2005    REF: SPR 33999 xdeepadh
		// When the files are not selected, the current playing index is set 0.
		mfw_player_currently_playing_idx  = 0;
	}
	//set exit_flag to 0 to notify that we have to play all files
	mfw_player_exit_flag = PLAYER_EXIT_FLAG_FALSE;  
	//Play the midi player with high volume.	
#ifdef FF_MIDI_RINGER  
#ifdef FF_MIDI_LOAD_FROM_MEM
	melody_data=sounds_midi_return_memory_location(mfw_player_currently_playing_idx);
	player_type=mfw_ringer_deduce_player_type(melody_data.melody_name);
	mfw_player_start_memory(player_type,(UINT32*)melody_data.melody,melody_data.melody_size,(UBYTE)AS_VOLUME_HIGH,FALSE,sounds_midi_player_start_cb);
#else
	midi_file=sounds_midi_return_file_name(mfw_player_currently_playing_idx);
	strcpy(temp, "/FFS/mmi/tones/");
	strcat(temp, midi_file);
	player_type=mfw_ringer_deduce_player_type(midi_file);
#ifdef FF_MMI_FILEMANAGER
	convert_u8_to_unicode(temp, midi_file_uc);
	mfw_player_start_file(player_type,midi_file_uc,(UBYTE)AS_VOLUME_HIGH,FALSE,sounds_midi_player_start_cb);
#else
	   mfw_player_start_file(player_type,midi_file,(UBYTE)AS_VOLUME_HIGH,FALSE,sounds_midi_player_start_cb);
#endif
#endif 	
#endif		
	return 1;
}



// June 19, 2006 REF: DRT OMAPS00076378  x0012849
/*******************************************************************************

 $Function:    midi_test_play_browsing

 $Description: Play the currently choosen file.

 $Returns:    MFW_EVENT_CONSUMED always

 $Arguments:  e, event, tc timer context
*******************************************************************************/

static int midi_test_play_browsing( MfwEvt e, MfwTim *tc )
{
    midi_test_play(NULL, NULL); 
    if (midiBrowseTimer != NULL )
    {
        timStop(midiBrowseTimer);
    }
    return MFW_EVENT_CONSUMED;
}



// Dec 08, 2004	REF: CRR MMI-SPR-27284
// Description: MIDI Player: Back Key malfunctions in MIDI application.
// Solution: A window is created to hold list window. When the user presses Back key, the newly 
//		    created window is deleted along with list window, bringing the control back to Midi options
//		    screen.
/*******************************************************************************

 $Function:     midi_test_files_exec_cb

 $Description: Call back function

 $Returns:	none

 $Arguments:		

*******************************************************************************/
void midi_test_files_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_mt_menu   * data = (T_mt_menu *)win_data->user;
	int max_file_number;
	int i;

	 ListWinInfo      * mnu_data = (ListWinInfo *)parameter;

	TRACE_FUNCTION (">>>>> midi_test_files_exec_cb()");

	switch (event)
	{
    // June 19, 2006 REF: DRT OMAPS00076378  x0012849
    // To know whether the list is created or not.
	T_MFW MidiListCreationStatus;
	case E_INIT:

		TRACE_EVENT(">>>> midi_test_files_exec_cb() Event:E_INIT");

		 /* initialization of administrative data */

		data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));

		if(data->menu_list_data == 0)
		{
		  TRACE_EVENT("Failed memory alloc 1 ");
		  return;
		}

		  max_file_number = sounds_midi_return_file_number();
					//In case no files have been loaded display info message
		//In case no files have been loaded return info message
		if(max_file_number == 0)
		{
			  	     mmi_midi_test_show_info(0, TxtMidiFileNotLoaded, TxtNull, NULL);
					  return;
		}


		data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( max_file_number * sizeof(T_MFW_MNU_ITEM) );

		if(data->menu_list_data->List == 0)
		{
		  TRACE_EVENT("Failed memory alloc 2");
		  return;
		}
		//Sep 29,2005 REF: SPR 34407 xdeepadh  
		for (i = 0; i < max_file_number ; i++)
		{
			mnuInitDataItem(&data->menu_list_data->List[i]);
			data->menu_list_data->List[i].flagFunc = item_flag_none;
				//Display the midi file names in the list
		data->menu_list_data->List[i].str  = (char *)sounds_midi_return_file_name(data->mt_file_count++);
		}

		  data->menu_list_data->ListLength =max_file_number;
		data->menu_list_data->ListPosition = 1;
		data->menu_list_data->CursorPosition = 1;
		data->menu_list_data->SnapshotSize = max_file_number;
		data->menu_list_data->Font = 0;
		data->menu_list_data->LeftSoftKey = TxtSoftSelect;
		data->menu_list_data->RightSoftKey = TxtSoftBack;
		data->menu_list_data->KeyEvents = KEY_ALL;
		data->menu_list_data->Reason = 0;
		data->menu_list_data->Strings = TRUE;
		data->menu_list_data->Attr   = (MfwMnuAttr*)&menuMtSearchListAttr;
		//Apr 05, 2005    REF: ENH 29994 xdeepadh	
		data->menu_list_data->Attr->hdrId = TxtMidiFileTitle;
		data->menu_list_data->autoDestroy    = FALSE;
        // June 19, 2006 REF: DRT OMAPS00076378  x0012849
        MidiListCreationStatus = 
            listDisplayListMenu (win, data->menu_list_data, 
            (ListCbFunc)midi_mt_menu_list_listmnu_cb,0);
        if (LISTS_OK == MidiListCreationStatus)
        {
            /*If control reached this point then List of files are showed on display*/
            isMidiListON = TRUE;
            /*Create one timer and play the file only if that timer expires*/
            /*Play the first file if user hasn't pressed the up and down for half a second */
            mfw_player_currently_playing_idx = 0;
            midiBrowseTimer = timCreate( 0, TIME_HALFSECOND, (MfwCb)midi_test_play_browsing);
            timStart( midiBrowseTimer );
        }
    break;
    case E_START_MELODY:
        // June 19, 2006 REF: DRT OMAPS00076378  x0012849
        /*Stop playing the current file*/
        midi_stop_player (NULL, NULL);
        mfw_player_currently_playing_idx = mnu_data->MenuData.ListPosition;
        if (midiBrowseTimer != NULL )
        {
            timStop(midiBrowseTimer);
            timStart(midiBrowseTimer);
        }
        break;
	default:
	break;
	}

}

// Dec 08, 2004	REF: CRR MMI-SPR-27284
// Description: MIDI Player: Back Key malfunctions in MIDI application.
// Solution: A window is created to hold list window. When the user presses Back key, the newly 
//		    created window is deleted along with list window, bringing the control back to Midi options
//		    screen.
/*******************************************************************************

 $Function:     midi_test_files_win_cb

 $Description: Window event handler

 $Returns:	MFW_EVENT_REJECTED: 
 			MFW_EVENT_CONSUMED:

 $Arguments:	

*******************************************************************************/
static int midi_test_files_win_cb (MfwEvt e, MfwWin *w)    
{
  	TRACE_FUNCTION ("midi_test_files_win_cb()");
  	switch (e)
  	{
    		case MfwWinVisible:  /* window is visible  */
    		break;
    		case MfwWinFocussed: /* input focus / selected   */
    		case MfwWinDelete:   /* window will be deleted   */

    		default:
      		return MFW_EVENT_REJECTED;
  	}
  	return MFW_EVENT_CONSUMED;
}	


// Dec 08, 2004	REF: CRR MMI-SPR-27284
// Description: MIDI Player: Back Key malfunctions in MIDI application.
// Solution: A window is created to hold list window. When the user presses Back key, the newly 
//		    created window is deleted along with list window, bringing the control back to Midi options
//		    screen.
/*******************************************************************************

 $Function:     midi_test_files_browse

 $Description: This function is called when the user selects "Browse Midi Files" option

 $Returns:	MFW_EVENT_CONSUMED

 $Arguments:	
 			

*******************************************************************************/
GLOBAL int midi_test_files_browse( MfwMnu *m, MfwMnuItem *i )
{
		T_MFW_HND win;
		T_MFW_HND parent_win = mfw_parent(mfw_header());
	int numberOfMelodies=0;

	TRACE_FUNCTION ("midi_test_files_browse");
//Sep 11, 2006 DR: OMAPS00094182 xrashmic
     sounds_midi_init();
// Nov 09, 2005    REF:DR OMAPS00049167  x0039928
// Fix : Call the player initialize routine to set the default parameters.
	 mfw_player_init();

	numberOfMelodies = sounds_midi_return_file_number();
	TRACE_EVENT_P1("numberOfMelodies is %d",numberOfMelodies);

	if(numberOfMelodies <= 0)
	{
		mmi_midi_test_show_info(0, TxtMidiFileNotLoaded, TxtNull, NULL);
		
	}
	else
	{
		win = mmi_midi_test_build_Menu(parent_win);
		if (win NEQ NULL)
		{
		SEND_EVENT (win, E_INIT, 0, 0);
		}

	}
	return MFW_EVENT_CONSUMED;
}

//Sep 29,2005 REF: SPR 34407 xdeepadh  
/*******************************************************************************

 $Function:     mmi_midi_test_build_Menu

 $Description: To hold list window

 $Returns:	Window handle

 $Arguments:	
*******************************************************************************/

static T_MFW_HND  mmi_midi_test_build_Menu( MfwHnd parent_window)
{
	T_mt_menu *  data = (T_mt_menu *)mfwAlloc(sizeof (T_mt_menu));
  	T_MFW_WIN  * win;

    	TRACE_FUNCTION ("mmi_midi_test_build_Menu");

    	/*
     	* Create window handler
     	*/

    	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)midi_test_files_win_cb);
    	if (data->win EQ NULL)
   	{
      		return NULL;
   	}

  	TRACE_EVENT("list holder window created: " );
    	/*
     	* connect the dialog data to the MFW-window
     	*/

    	data->mmi_control.dialog = (T_DIALOG_FUNC)midi_test_files_exec_cb;
    	data->mmi_control.data   = data;
    	win                      = ((T_MFW_HDR *)data->win)->data;
    	win->user                = (void *)data;
    	data->parent             = parent_window;
    	winShow(data->win);
    	/*
     	* return window handle
     	*/

     	return data->win;
}
//Sep 11, 2006 DR: OMAPS00094182 xrashmic
/*******************************************************************************

 $Function:     mmi_midi_test_Application_cb

 $Description: Callback function called on exiting the midi appilcation

 $Returns:	None

 $Arguments:	parent_win : Parent window
 			identifier :    
 			reason:    
*******************************************************************************/
static void mmi_midi_test_Application_cb(T_MFW_HND parent_win, UBYTE identifier, UBYTE reason)
{
	TRACE_FUNCTION("mmi_midi_test_Application_cb");
	sounds_midi_exit();
}

/*******************************************************************************

 $Function:     mmi_midi_test_Application

 $Description: Launcing the Midi application menu

 $Returns:		Always returns MFW_EVENT_CONSUMED (1)

 $Arguments:		m : pointer to the mnu data for the menu which caused this function to be called.
 				i   : pointer to the item data from the menu.
*******************************************************************************/
GLOBAL int  mmi_midi_test_Application (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND 		win;
	T_MFW_HND 		parent_win = mfw_parent(mfw_header());
	TRACE_FUNCTION("mmi_midi_test_Application");
	win=bookMenuStart(parent_win, MidiTestAppMenuAttributes(),0);
	SEND_EVENT(win, ADD_CALLBACK, NULL, (void *)mmi_midi_test_Application_cb);
	return MFW_EVENT_CONSUMED;
}
