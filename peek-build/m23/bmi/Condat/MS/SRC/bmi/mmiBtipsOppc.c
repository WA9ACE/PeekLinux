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
 $File:       MmiBtipsBmg.c
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS BMG APPlication functionality.

********************************************************************************
 $History: MmiBtipsBmg.c

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
#include "mmiBtipsOppc.h"
#include "mfw_BtipsOppc.h"
#include "mfw_BtipsBmg.h"

#ifdef FF_MMI_BTIPS_APP

#endif

/*Function Declarations*/

#define OPPC_MAX_FILES 10


int   oppc_files_count;
char* oppc_file_names[OPPC_MAX_FILES];
#define OPPC_INFO_SCRN_TIMEOUT  1500//Timeout for InfoDialog
char push_file_name[256];
char pull_file_name[256];

int mmi_btips_oppcDeviceSearchCallback(BD_ADDR bdAddr);
int mmi_btips_oppcPullDeviceSearchCallback(BD_ADDR bdAddr);
int mmi_btips_oppcExchangeDeviceSearchCallback(BD_ADDR bdAddr);

 int  mmi_btips_oppcPullHandler(MfwMnu* m, MfwMnuItem* i)
{
	TRACE_FUNCTION("mmi_btips_oppcPullHandler");
	mmi_btips_oppcDeviceSearchWnd(1);
 }

  int  mmi_btips_oppcExchangeHandler(MfwMnu* m, MfwMnuItem* i)
{
	TRACE_FUNCTION("mmi_btips_oppcExchangeHandler");
	mmi_btips_oppcDeviceSearchWnd(2);
 }
 
 int  mmi_btips_oppcHandler(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND win;
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	int numFiles=0;
	
	TRACE_FUNCTION ("mmi_btips_oppcHandler");
	oppc_files_count = OPPC_MAX_FILES;
	oppc_files_count=mfw_btips_oppcPopulateFiles(oppc_file_names,OPPC_MAX_FILES,
					  BT_DEMO_FOLDER_PATH);
	TRACE_FUNCTION_P1("oppc_files_count - %d", oppc_files_count);
	numFiles = mfw_btips_oppcGetNumFiles();
	mmi_btips_oppcShowInfoDlg(0, TxtMidiFileNotLoaded, TxtNull, NULL);
	TRACE_EVENT_P1("mmi_btips_oppcHandler: numFiles is %d",numFiles);

	if(numFiles <= 0)
	{
		mmi_btips_oppcShowInfoDlg(0, TxtMidiFileNotLoaded, TxtNull, NULL);
		
	}
	else
	{
		win = mmi_btips_oppcBuildMenu(parent_win);
		if (win NEQ NULL)
		{
			SEND_EVENT (win, BTIPS_OPPC_LIST_INIT, 0, 0);
		}

	}

	return MFW_EVENT_CONSUMED;
}

 /*******************************************************************************

 $Function:     mmi_btips_oppcBuildMenu

 $Description: To hold list window

 $Returns:	Window handle

 $Arguments:	
*******************************************************************************/

static T_MFW_HND  mmi_btips_oppcBuildMenu( MfwHnd parent_window)
{
	T_MMI_Btips_Oppc_Win_data *  data = (T_MMI_Btips_Oppc_Win_data *)mfwAlloc(sizeof (T_MMI_Btips_Oppc_Win_data));
  	T_MFW_WIN  * win;

    	TRACE_FUNCTION ("mmi_btips_oppcBuildMenu");
    	/*
     	* Create window handler
     	*/

    	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)mmi_btips_oppcListWinCb);
    	if (data->win EQ NULL)
   	{
      		return NULL;
   	}

  	TRACE_EVENT("list holder window created: " );
    	/*
     	* connect the dialog data to the MFW-window
     	*/

    	data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_btips_oppcListWinExecCb;
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

/*******************************************************************************

 $Function:     mmi_btips_oppcListWinCb

 $Description: Window event handler

 $Returns:	MFW_EVENT_REJECTED: 
 			MFW_EVENT_CONSUMED:

 $Arguments:	

*******************************************************************************/
static int mmi_btips_oppcListWinCb (MfwEvt e, MfwWin *w)    
{
  	TRACE_FUNCTION ("mmi_btips_oppcListWinCb()");
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

/*******************************************************************************

 $Function:     mmi_btips_oppcListWinExecCb

 $Description: Call back function

 $Returns:	none

 $Arguments:		

*******************************************************************************/
void mmi_btips_oppcListWinExecCb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_Oppc_Win_data   * data = (T_MMI_Btips_Oppc_Win_data *)win_data->user;
	int max_file_number;
	int i;

	 ListWinInfo      * mnu_data = (ListWinInfo *)parameter;

	TRACE_FUNCTION ("mmi_btips_oppcListWinExecCb()");
	switch (event)
	{
	   	T_MFW OppcListCreationStatus;
		case BTIPS_OPPC_LIST_INIT:
		{

			TRACE_EVENT("mmi_btips_oppcListWinExecCb() Event:E_INIT");

			 /* initialization of administrative data */

			data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));

			if(data->menu_list_data == 0)
			{
			  	TRACE_EVENT("Failed memory alloc 1 ");
				 return;
			}

			  max_file_number = mfw_btips_oppcGetNumFiles();
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
					//Display the OPPC file names in the list
				data->menu_list_data->List[i].str  = 
							(char *)mfw_btips_oppcGetFileName(data->oppc_file_count++);
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
			data->menu_list_data->Attr   = (MfwMnuAttr*)&BtipsOppcList_Attrib;
			data->menu_list_data->Attr->hdrId = TxtPlayerSelectFile;
			data->menu_list_data->autoDestroy    = FALSE;
	              OppcListCreationStatus = 
	             listDisplayListMenu (win, data->menu_list_data, 
	                                              (ListCbFunc)mmi_btips_oppcListMenuCb,0);
		        if (LISTS_OK == OppcListCreationStatus)
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
static void mmi_btips_oppcListMenuCb(T_MFW_HND Parent, ListMenuData * ListData)
{
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)Parent)->data;
	T_MMI_Btips_Oppc_Win_data	*data		= (T_MMI_Btips_Oppc_Win_data *)win_data->user;
	char* szSelectedFileExt;
	T_MFW_HND parent_win = mfw_parent(mfw_header());
	TRACE_FUNCTION ("mmi_btips_oppcListMenuCb()");
	
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
			FREE_MEMORY ((void *)data->menu_list_data->List, (U16)(data->oppc_num_file * sizeof (T_MFW_MNU_ITEM)));
			FREE_MEMORY ((void *)data->menu_list_data, sizeof (ListMenuData));
		}

		mmi_btips_oppcListMenuDestroy(data->win);
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
		strcpy(push_file_name, OPPC_DEFAULT_PUSH_PULL_DIR);
		//strcat(push_file_name, ListData->List->str);
		strcat(push_file_name, ListData->List[ListData->ListPosition].str);
		mmi_btips_oppcDeviceSearchWnd(0);
		#if 0
		szSelectedFileExt = mfw_btips_oppcGetExtension(mfw_btips_oppcGetFileName(ListData->CursorPosition));
	
	        //This was for the Options menu
		//data->oppc_optionsMenu=(T_MFW_HND)bookMenuStart( parent_win, btipsOppcOptionsMenuAttributes(), 0);
		if ( (strcmp(szSelectedFileExt, "mp3") == 0))
		{
			mmi_btips_oppcDeviceSearchWnd();
		}
		else if ( (strcmp(szSelectedFileExt, "pcm") == 0))
		{
			mmi_btips_oppcDeviceSearchWnd();
		}
		else if ( (strcmp(szSelectedFileExt, "wav") == 0))
		{
			mmi_btips_oppcDeviceSearchWnd();
		}
		else
		{
			//It should never come here	
		}
		#endif

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

 $Function:     mmi_btips_oppcListMenuDestroy

 $Description: destroy menu window

 $Returns:

 $Arguments: window to be destroyed

*******************************************************************************/
static void mmi_btips_oppcListMenuDestroy(MfwHnd window)
{
	T_MFW_WIN     * win_data  = ((T_MFW_HDR *)window)->data;
	T_MMI_Btips_Oppc_Win_data * data = (T_MMI_Btips_Oppc_Win_data *)win_data->user;

	TRACE_FUNCTION ("mmi_btips_oppcListMenuDestroy");

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
		mfwFree((void *)data, sizeof (T_MMI_Btips_Oppc_Win_data));
	}
}
/*******************************************************************************

 $Function:     mmi_btips_oppcShowInfoDlg

 $Description: Display the Dialog

 $Returns:

 $Arguments:

*******************************************************************************/
static MfwHnd mmi_btips_oppcShowInfoDlg(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION ("mmi_btips_oppcShowInfoDlg()");

	/*
	** Create a timed dialog to display the Message "Failed"
	*/
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, OPPC_INFO_SCRN_TIMEOUT, KEY_LEFT | KEY_CLEAR | KEY_HUP);

	return info_dialog(parent, &display_info);
}

int mmi_btips_oppcDeviceSearchWnd(int type)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	//mmi_btips_bmg_devices_win_create(parent_win, BTIPS_GENERIC_DEVICE_SEARCH, NULL);
	switch (type)
	{
		case 0:
			mmi_btips_bmg_devices_win_create(parent_win, BTIPS_OPP_DEVICE_SEARCH, mmi_btips_oppcDeviceSearchCallback);
		break;

		case 1:
			mmi_btips_bmg_devices_win_create(parent_win, BTIPS_OPP_DEVICE_SEARCH, mmi_btips_oppcPullDeviceSearchCallback);
		break;

		case 2:
			mmi_btips_bmg_devices_win_create(parent_win, BTIPS_OPP_DEVICE_SEARCH, mmi_btips_oppcExchangeDeviceSearchCallback);
		break;	
	}
	
	return MFW_EVENT_CONSUMED;
}
int mmi_btips_oppcDeviceSearchCallback(BD_ADDR bdAddr)
{
	static char szDevName[128];
	mfw_btips_bmgGetDeviceName(&bdAddr, szDevName);
	TRACE_FUNCTION_P1("mmi_btips_oppcDeviceSearchCallback with Dev Name %s",  szDevName);
	mmi_btips_app_show_text(0, szDevName, "Selected Device",NULL);
	//mmi_btips_app_show_text(0, szBdAddr, TxtNull, NULL);
	//OPPCA_Connect(&bdAddr);
	mfw_btips_oppcEncapsulatePush(push_file_name, &bdAddr);
	return MFW_EVENT_CONSUMED;
}
int mmi_btips_oppcPullDeviceSearchCallback(BD_ADDR bdAddr)
{
	static char szDevName[128];
	mfw_btips_bmgGetDeviceName(&bdAddr, szDevName);
	TRACE_FUNCTION_P1("mmi_btips_oppcDeviceSearchCallback with Dev Name %s",  szDevName);
	mmi_btips_app_show_text(0, szDevName, "Selected Device",NULL);
	//mmi_btips_app_show_text(0, szBdAddr, TxtNull, NULL);
	//OPPCA_Connect(&bdAddr);
	strcpy(pull_file_name, OPPC_DEFAULT_PUSH_PULL_DIR);
	strcat(pull_file_name, szDevName);
	strcat(pull_file_name, OPPC_VCF_EXT);
	mfw_btips_oppcEncapsulatePull(pull_file_name, &bdAddr);
	return MFW_EVENT_CONSUMED;
}

int mmi_btips_oppcExchangeDeviceSearchCallback(BD_ADDR bdAddr)
{
	static char szDevName[128];
	mfw_btips_bmgGetDeviceName(&bdAddr, szDevName);
	TRACE_FUNCTION_P1("mmi_btips_oppcDeviceSearchCallback with Dev Name %s",  szDevName);
	mmi_btips_app_show_text(0, szDevName, "Selected Device",NULL);
	//mmi_btips_app_show_text(0, szBdAddr, TxtNull, NULL);
	//OPPCA_Connect(&bdAddr);
	strcpy(push_file_name, OPPC_DEFAULT_PUSH_CARD);
	strcpy(pull_file_name, OPPC_DEFAULT_PUSH_PULL_DIR);
	strcat(pull_file_name, szDevName);
	strcat(pull_file_name, OPPC_VCF_EXT);
	mfw_btips_oppcEncapsulateExchange(push_file_name, pull_file_name, &bdAddr);
	return MFW_EVENT_CONSUMED;
}


