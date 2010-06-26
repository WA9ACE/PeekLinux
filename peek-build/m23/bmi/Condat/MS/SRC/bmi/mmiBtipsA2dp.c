/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */
/*
 $Project name: Basic Bluetooth MMI
 $Project code:
 $Module:   Bluetooth BMG APPlication
 $File:       MmiBtipsA2dp.c
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: MmiBtipsA2dp.c

  26/06/07 Sasken original version

 $End

*******************************************************************************/




/*******************************************************************************

                                Include files

*******************************************************************************/

#define ENTITY_MFW

#include <string.h>

/* BTIPS Includes */
#include "me.h"


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

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_icn.h"	/* included for mfw_mnu.h */
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_kbd.h"
#include "mfw_sat.h" /* included for MmiDummy.h, included for MmiMenu.h */
#include "MmiBlkLangDB.h"
#include "mmiColours.h"
#include "MmiDialogs.h"
#include "MmiDummy.h" /* included for MmiMenu.h */
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "AUIEditor.h"
#include "MmiMain.h"
#include "Mmiicons.h"
#include "MmiWindow.h"
#include "MmiCall.h"
#include "mmiBookShared.h"

#include "mmiBtips.h"
#include "mfw_btips.h"
#include "mfw_BtipsA2dp.h"
#include "mmiBtipsA2dp.h"
#include "mfw_BtipsBmg.h"
#include "mmiBtipsBmg.h"
#include "btl_a2dp.h"


int   a2dp_files_count;
extern BtlA2dpStreamId     streamId;
//Array to load fileNames in content directory
char* a2dp_file_names[A2DP_MAX_FILES];
char* szSelectedFileExt;
char  szSelectedFileName[256];
#define A2DP_INFO_SCRN_TIMEOUT  1500//Timeout for InfoDialog
T_MFW_HND hA2dpDeviceWnd;

#define A2DP_BMG_DEVICES_EXIT_WIN 5


#ifdef FF_MMI_BTIPS_APP

static T_MFW_HND  mmi_btips_a2dpBuildMenu( MfwHnd parent_window);
static int mmi_btips_a2dpWinCb (MfwEvt e, MfwWin *w)  ;

static void mmi_btips_a2dpListMenuCb(T_MFW_HND Parent, ListMenuData * ListData);
static void mmi_btips_a2dpListMenuDestroy(MfwHnd window);
static MfwHnd mmi_btips_a2dpShowInfoDlg(T_MFW_HND parent, int str1, int str2, 
	                                                                       T_VOID_FUNC callback);

 int  mmi_btips_a2dpHandler(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND win;
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	int numFiles=0;

	TRACE_FUNCTION ("mmi_btips_a2dpHandler");
	//mfw_btips_a2dpInit();
	a2dp_files_count = A2DP_MAX_FILES;
	a2dp_files_count=mfw_btips_a2dpPopulateFiles(a2dp_file_names,A2DP_MAX_FILES,
					  BT_DEMO_FOLDER_PATH);
	TRACE_FUNCTION_P1("mmi_btips_a2dpHandler: a2dp_files_count - %d", a2dp_files_count);
	numFiles = mfw_btips_a2dpGetNumFiles();

	TRACE_FUNCTION_P1("mmi_btips_a2dpHandler FileCount = %d",numFiles);

	if(numFiles <= 0)
	{
		mmi_btips_a2dpShowInfoDlg(0, TxtMidiFileNotLoaded, TxtNull, NULL);
		
	}
	else
	{
		win = mmi_btips_a2dpBuildMenu(parent_win);
		if (win NEQ NULL)
		{
			SEND_EVENT (win, BTIPS_A2DP_LIST_INIT, 0, 0);
		}

	}
	return MFW_EVENT_CONSUMED;
	
}

/*******************************************************************************

 $Function:     mmi_btips_a2dpBuildMenu

 $Description: To hold list window

 $Returns:	Window handle

 $Arguments:	
*******************************************************************************/

static T_MFW_HND  mmi_btips_a2dpBuildMenu( MfwHnd parent_window)
{
	T_MMI_Btips_A2dp_Win_data *  data = (T_MMI_Btips_A2dp_Win_data *)mfwAlloc(sizeof (T_MMI_Btips_A2dp_Win_data));
  	T_MFW_WIN  * win;

    	TRACE_FUNCTION ("mmi_btips_a2dpBuildMenu");

    	/*
     	* Create window handler
     	*/

    	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)mmi_btips_a2dpWinCb);
    	if (data->win EQ NULL)
   	{
      		return NULL;
   	}

  	TRACE_EVENT("list holder window created: " );
    	/*
     	* connect the dialog data to the MFW-window
     	*/

    	data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_btips_a2dpExecCb;
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


// Description: MIDI Player: Back Key malfunctions in MIDI application.
// Solution: A window is created to hold list window. When the user presses Back key, the newly 
//		    created window is deleted along with list window, bringing the control back to Midi options
//		    screen.
/*******************************************************************************

 $Function:     mmi_btips_a2dpWinCb

 $Description: Window event handler

 $Returns:	MFW_EVENT_REJECTED: 
 			MFW_EVENT_CONSUMED:

 $Arguments:	

*******************************************************************************/
static int mmi_btips_a2dpWinCb(MfwEvt e, MfwWin *win)
{
	T_MMI_Btips_A2dp_Win_data	*data = (T_MMI_Btips_A2dp_Win_data *)win->user;

  	TRACE_FUNCTION ("mmi_btips_a2dpWinCb()");
  	switch (e)
  	{
    		case MfwWinFocussed: /* input focus / selected   */   
			TRACE_EVENT("MfwWinFocussed");
    		case MfwWinDelete:   /* window will be deleted   */    
			TRACE_EVENT("MfwWinDelete");
		case MfwWinVisible:  /* window is visible  */	    
			TRACE_EVENT("MfwWinVisible");
		break;
		case MfwWinSuspend:  /* window is suspended  */
			TRACE_EVENT("MfwWinSuspend");
		break;
		case MfwWinResume: 	/* window is resumed */
			TRACE_EVENT("MfwWinResume");
    		default:
      		return MFW_EVENT_REJECTED;
  	}
  	return MFW_EVENT_CONSUMED;
}	

// Description: MIDI Player: Back Key malfunctions in MIDI application.
// Solution: A window is created to hold list window. When the user presses Back key, the newly 
//		    created window is deleted along with list window, bringing the control back to Midi options
//		    screen.
/*******************************************************************************

 $Function:     mmi_btips_a2dpExecCb

 $Description: Call back function

 $Returns:	none

 $Arguments:		

*******************************************************************************/
void mmi_btips_a2dpExecCb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_A2dp_Win_data   * data = (T_MMI_Btips_A2dp_Win_data *)win_data->user;
	int max_file_number;
	int i;

	 ListWinInfo      * mnu_data = (ListWinInfo *)parameter;

	TRACE_FUNCTION ("mmi_btips_a2dpExecCb()");
	switch (event)
	{
	   	T_MFW A2dpListCreationStatus;
		case BTIPS_A2DP_LIST_INIT:
		{

			TRACE_EVENT(" midi_test_files_exec_cb() Event:E_INIT");

			 /* initialization of administrative data */

			data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));

			if(data->menu_list_data == 0)
			{
			  	TRACE_EVENT("Failed memory alloc 1 ");
				 return;
			}

			  max_file_number = mfw_btips_a2dpGetNumFiles();
			//In case no files have been loaded display info message
			//In case no files have been loaded return info message
			if(max_file_number == 0)
			{
				  return;
			}


			data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( max_file_number * sizeof(T_MFW_MNU_ITEM) );

			if(data->menu_list_data->List == 0)
			{
			  	TRACE_EVENT("Failed memory alloc 2");
			  	return;
			}
			for (i = 0; i < max_file_number ; i++)
			{
				mnuInitDataItem(&data->menu_list_data->List[i]);
				data->menu_list_data->List[i].flagFunc = item_flag_none;
					//Display the A2DP file names in the list
				data->menu_list_data->List[i].str  = 
							(char *)mfw_btips_a2dpGetFileName(data->a2dp_file_count++);
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
			data->menu_list_data->Attr   = (MfwMnuAttr*)&BtipsA2dpList_Attrib;
			data->menu_list_data->Attr->hdrId = TxtPlayerSelectFile;
			data->menu_list_data->autoDestroy    = FALSE;
	              A2dpListCreationStatus = 
	             listDisplayListMenu (win, data->menu_list_data, 
	                                              (ListCbFunc)mmi_btips_a2dpListMenuCb,0);
			  if (LISTS_OK == A2dpListCreationStatus)
		        {
		        #if 0
		            /*If control reached this point then List of files are showed on display*/
		            isMidiListON = TRUE;
		            /*Create one timer and play the file only if that timer expires*/
		            /*Play the first file if user hasn't pressed the up and down for half a second */
		            mfw_player_currently_playing_idx = 0;
		            midiBrowseTimer = timCreate( 0, TIME_HALFSECOND, (MfwCb)midi_test_play_browsing);
		            timStart( midiBrowseTimer );
			#endif
		        }
		}
		break;
		default:
		break;
	}

}
/*******************************************************************************

 $Function:     midi_mt_menu_list_listmnu_cb

 $Description: destroy lists and menu in case of back softkey or menu exit

 $Returns:

 $Arguments:

*******************************************************************************/
static void mmi_btips_a2dpListMenuCb(T_MFW_HND Parent, ListMenuData * ListData)
{
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)Parent)->data;
	T_MMI_Btips_A2dp_Win_data	*data		= (T_MMI_Btips_A2dp_Win_data *)win_data->user;

	TRACE_FUNCTION ("mmi_btips_a2dpListMenuCb()");
	
	if ((ListData->Reason EQ LISTS_REASON_BACK) ||
	     (ListData->Reason EQ LISTS_REASON_CLEAR) ||
	     (ListData->Reason EQ LISTS_REASON_HANGUP))
	{
		mfw_btips_a2dpCloseStream();
		/*
		** Destroy the window --- Not sure if this is required! ... leave it out for now
		*/
		listsDestroy(ListData->win);

		/*
		** Free the memory set aside for ALL the devices to be listed.
		*/
		if (data->menu_list_data != NULL)
		{
			FREE_MEMORY ((void *)data->menu_list_data->List, (U16)(data->a2dp_num_file * sizeof (T_MFW_MNU_ITEM)));
			FREE_MEMORY ((void *)data->menu_list_data, sizeof (ListMenuData));
		}

		mmi_btips_a2dpListMenuDestroy(data->win);
		#if 0
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
		#endif
	}
	else if(ListData->Reason EQ LISTS_REASON_SELECT)
	{
		OS_MemSet(szSelectedFileName, 0, sizeof(szSelectedFileName));
		OS_StrCpy(szSelectedFileName, BT_DEMO_FOLDER_PATH);
		OS_StrCat(szSelectedFileName, "/");
		OS_StrCat(szSelectedFileName, mfw_btips_a2dpGetFileName(ListData->CursorPosition));
		TRACE_EVENT_P1("szSelectedFileName:  %s", szSelectedFileName);
		//szSelectedFileName = mfw_btips_a2dpGetFileName(ListData->CursorPosition);
		
		//OS_StrCpy(szSelectedFileName, "/MfwBtDemo/base_fadeout.wav");
		szSelectedFileExt = mfw_btips_a2dpGetExtension(szSelectedFileName);
		if(streamId >= 0)
		{
			TRACE_EVENT("Closing stream in  mmi_btips_a2dpListMenuCb After selecting new file");
			mfw_btips_a2dpCloseStream();
		}
		if(FALSE == data->deviceSearchStarted)
		{

			data->deviceSearchStarted = TRUE;
			if ( (strcmp(szSelectedFileExt, "mp3") == 0))
			{
				
				mmi_btips_a2dpDeviceSearchWnd();
			}
			else if ( (strcmp(szSelectedFileExt, "pcm") == 0))
			{
				
				mmi_btips_a2dpDeviceSearchWnd();
			}
			else if ( (strcmp(szSelectedFileExt, "wav") == 0))
			{
				
				mmi_btips_a2dpDeviceSearchWnd();
			}
			else
			{
				//It should never come here	
				TRACE_ERROR("invalid file name");
			}
		}
	  #if 0
		TRACE_EVENT_P1("list position is %d",ListData->ListPosition);
		mt_menu_data.mt_list_idx = ListData->ListPosition;
		//call mfw to save selected index
		mfw_player_save_selected_file_idx(mt_menu_data.mt_list_idx);
		//display a "file selected" text here
		mmi_midi_test_show_info(0, TxtMidiFileSelected, TxtNull, NULL);
	   #endif
	 }
	

	return;
}
/*******************************************************************************

 $Function:     mmi_btips_a2dpListMenuDestroy

 $Description: destroy menu window

 $Returns:

 $Arguments: window to be destroyed

*******************************************************************************/
static void mmi_btips_a2dpListMenuDestroy(MfwHnd window)
{
	T_MFW_WIN     * win_data  = ((T_MFW_HDR *)window)->data;
	T_MMI_Btips_A2dp_Win_data * data = (T_MMI_Btips_A2dp_Win_data *)win_data->user;

	TRACE_FUNCTION ("mmi_btips_a2dpListMenuDestroy");

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
		mfwFree((void *)data, sizeof (T_MMI_Btips_A2dp_Win_data));
	}
}
/*******************************************************************************

 $Function:     mmi_btips_a2dpShowInfoDlg

 $Description: Display the Dialog

 $Returns:

 $Arguments:

*******************************************************************************/
static MfwHnd mmi_btips_a2dpShowInfoDlg(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION ("mmi_btips_a2dpShowInfoDlg()");

	/*
	** Create a timed dialog to display the Message "Failed"
	*/
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, A2DP_INFO_SCRN_TIMEOUT, KEY_LEFT | KEY_CLEAR | KEY_HUP);

	return info_dialog(parent, &display_info);
}
/*******************************************************************************

 $Function:    	mmi_btips_a2dpPlayOnSingleDevice

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_a2dpPlayOnSingleDevice(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	TRACE_FUNCTION("mmi_btips_a2dpPlayOnSingleDevice");
	mfw_btips_bmgSearch(BTIPS_A2DP_DEVICE_SEARCH);
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	mmi_btips_a2dpPlayOnMultipleDevice

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_a2dpPlayOnMultipleDevice(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	//mmi_btips_app_show_info(parent_win,TxtBtipsShowPhone,TxtSelected,ONE_SECS,NULL);
	//mfw_BtipsBmgSetPhoneVisibility(True);
	return MFW_EVENT_CONSUMED;
}

int mmi_btips_a2dpDeviceSearchWnd()
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	hA2dpDeviceWnd = (T_MFW_HND)mmi_btips_bmg_devices_win_create(parent_win, BTIPS_A2DP_DEVICE_SEARCH, mmi_btips_a2dpDeviceSearchCallback);
	return MFW_EVENT_CONSUMED;
}

T_MFW_HND mmi_btips_a2dp_ask_cancelStreaming(T_MFW_HND parent, char * str1, char * str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION("mmi_btips_a2dp_ask_cancelStreaming");
	dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtStop, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, FOREVER, KEY_LEFT|KEY_RIGHT  | KEY_CLEAR | KEY_HUP);
	 return info_dialog(parent,&display_info);
}

void mmi_btips_a2dp_streaming_stop_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{	

	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_dialog_info      * data     = (T_dialog_info *)win_data->user;
	
	T_MFW_WIN *parent_win_data = ((T_MFW_HDR *) data->parent_win)->data;
   /*	T_MMI_Btips_Bmg_DevicesWin_data * parent_data     = (T_MMI_Btips_Bmg_DevicesWin_data *)parent_win_data->user;
	T_MFW_HND parent_win = parent_data->win;
*/	
	TRACE_FUNCTION_P1("mmi_btips_a2dp_streaming_stop_cb %d", reason);

	switch (reason)
	{
	case INFO_KCD_LEFT:
		TRACE_ERROR("No LSK");
	    	break;
	case INFO_KCD_HUP:
	case INFO_KCD_RIGHT:
		mfw_btips_a2dpCloseStream();
		break;
	case INFO_KCD_CLEAR:
	    	break;
	default:
    	break;
    }
}



int mmi_btips_a2dpDeviceSearchCallback(BD_ADDR bdAddr)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());

     	BtStatus status;
	static char szDevName[128];
	SEND_EVENT ( hA2dpDeviceWnd, BTIPS_BMG_DEVICES_EXIT_WIN, 0, (void *)NULL);
       vsi_t_sleep(0, 10);
  	mfw_btips_bmgGetDeviceName(&bdAddr, szDevName);
	TRACE_EVENT_P1("mmi_btips_a2dpDeviceSearchCallback with Dev Name %s",  szDevName);

	/*Now update the state machine so that when user comes back to this menu
	He should be able to STOP playing A2DP OR Select another file to Play*/
//	mmi_btips_a2dp_ask_cancelStreaming(NULL, "Streaming Audio to", szDevName, mmi_btips_a2dp_streaming_stop_cb);
	status = mfw_btips_a2dpConnectStream(bdAddr);
	return status;
}
#endif

