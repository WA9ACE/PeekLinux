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

#include "mfw_Btips.h"
#include "Mfw_BtipsBmg.h"

#include "mmiBtips.h"
#include "MmiBtipsBmg.h"
#include "AUIEditor_i.h"
CHAR noDeviceString[] = "No Devices";
CHAR FindNewDeviceString[] = "Find New Device";
CHAR SearchingDeviceString[] = "Searching Devices...";

extern MMI_Btips_data g_btipsData;
//UBYTE g_device_name[BT_MAX_REM_DEV_NAME] ;
T_MMI_BTIPS_SERVICES_STRUCT *services_data;
extern T_MFW_HND g_win_infodialog;

#ifdef FF_MMI_BTIPS_APP
//Global Function calls
static int mmi_btips_bmg_devices_root_cb(T_MFW_EVENT evnt, void *para);
//static int mmi_btips_bmg_connected_devices_root_cb(T_MFW_EVENT evnt, void *para);
static int mmi_btips_bmg_devices_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int mmi_btips_bmg_connected_devices_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int mmi_btips_bmg_devices_win_cb (MfwEvt evt, MfwWin *win);
static int mmi_btips_bmg_connected_devices_win_cb (MfwEvt evt, MfwWin *win)	;
static int mmi_btips_bmg_devices_kbd_cntrl (MfwEvt e, MfwKbd *k);
static void mmi_btips_bmg_devices_win_exit (T_MFW_HND win);
static void mmi_btips_bmg_connected_devices_win_exit (T_MFW_HND win);
static T_MFW_HND mmi_btips_bmg_phoneName_editor_create(MfwHnd parent_window);
static void mmi_btips_bmg_phoneName_editor_exec_cb(T_MFW_HND win, USHORT event, void *parameter);
static void mmi_btips_bmg_phoneName_editor_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);
static void mmi_btips_bmg_phoneName_editor_destroy(MfwHnd own_window);
T_MFW_HND mmi_btips_bmg_ShowPairedDeviceList(T_MFW_HND win);
T_MFW_HND mmi_btips_bmg_ShowConnectedDevicesList(T_MFW_HND win);
T_MFW_HND mmi_btips_bmg_ShowInquiredDeviceList(T_MFW_HND win);
static void mmi_btips_bmg_device_list_cb(T_MFW_HND * parent, ListMenuData * ListData);
static void mmi_btips_bmg_connected_device_list_cb(T_MFW_HND * parent, ListMenuData* ListConnectedData);
static void mmi_btips_bmg_pinCode_editor_exec_cb(T_MFW_HND win, USHORT event, void *parameter);
static void mmi_btips_bmg_pinCode_editor_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);
static void mmi_btips_bmg_pinCode_editor_destroy(MfwHnd own_window);
static int mmi_btips_bmg_SetConnectPermission(T_MFW_HND win, BOOL permission);
BOOL mmi_btips_bmgDeviceShowServices(T_MFW_HND win , BOOL discoverServices);
static void mmi_btips_bmg_services_editor_exec_cb(T_MFW_HND win, USHORT identifier, SHORT reason);
void mmi_btips_bmg_services_editor_destroy(void);
T_MFW_HND mmi_btips_bmg_ask_cancelBond(T_MFW_HND parent, char * str1, char * str2, T_VOID_FUNC callback);
void mmi_btips_bmg_bonding_cancelled_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
void mmi_btips_refreshScreen(T_MFW_HND win, UBYTE identifier, UBYTE reason);
void mmi_btips_ResumeBTMainMenu(T_MFW_HND win, UBYTE identifier, UBYTE reason);
extern void mmi_btips_app_incoming_pairing_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
/******************************************************************************
		MFW BTIPS BMG Window Module (Creation, Deletion, Callbacks for MFW, Keyboard, etc)
******************************************************************************/

/*******************************************************************************

 $Function:		mmi_btips_bmg_devices_win_create

 $Description:		This function performs the necessary steps to create the a window to handle the
 				viewfinder related softkeys and the viewfinder bitmaps. It will be removed, 
 				or rather remove itself on returning the data to the Mfw.

 $Returns:		T_MFW_HND : Window Handle to the New Window, Null if failed.

 $Arguments:		parent : pointer to the parent window.
 
*******************************************************************************/
T_MFW_HND mmi_btips_bmg_devices_win_create(T_MFW_HND parent, BOOL searchType, T_MFW_BTIPS_SEARCH_CB searchCB)
{
	T_MMI_Btips_Bmg_DevicesWin_data *data;
	T_MFW_WIN * win;
	
	TRACE_FUNCTION ("mmi_btips_bmg_devices_win_create()");
	data = (T_MMI_Btips_Bmg_DevicesWin_data *)mfwAlloc((U16)sizeof(T_MMI_Btips_Bmg_DevicesWin_data));

	data->win =(T_MFW_WIN * ) winCreate(parent, 0, MfwWinVisible|MfwWinSuspend|MfwWinResume|MfwWinDelete, 
									(MfwCb)mmi_btips_bmg_devices_win_cb);
	TRACE_EVENT_P2("New window - %x , parent = %x",parent,data->win);

	if (data->win == NULL)
	{
	
		//Failed to start : Free Memory, and exit
		mfwFree((U8 *)data, (U16)sizeof(T_MMI_Btips_Bmg_DevicesWin_data));
		data = NULL;
		return data;
	}
	else
	{

		/*
		** Setup the Dialog control functions
		*/
		data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_btips_bmg_devices_exec_cb;
		data->mmi_control.data = data;
		data->parent=parent;
		win = (T_MFW_WIN *)getWinData(data->win);
		win->user = (void *)data;
		
		//Create a BTIPS MFW component
		if (data->mfwwin == NULL)
		{
				data->mfwwin = (T_MFW_HND)mfw_btips_create(data->win, 
							E_BTIPS_BMG_NAME_RESULT|E_BTIPS_BMG_SEARCH_COMPLETE| 
							E_BTIPS_BMG_SEARCH_CANCELLED|E_BTIPS_BMG_BOND_COMPLETE|
							E_BTIPS_BMG_DEVICE_SERVICES|E_BTIPS_BMG_BOND_CANCELLED,
							MfwTyBtipsBmg, (MfwCb)mmi_btips_bmg_devices_root_cb);
		}
		if (data->mfwwin== NULL)
		{
			TRACE_EVENT ("mfwwin create failed");
			/*
			** BTIPS mfw Component Failed to start
			*/
			winDelete(data->win);
			return NULL;
		}

		data->bondState = BOND_NOT_STARTED;
		data->inquiryState = INQUIRY_NOT_STARTED;
		data->searchCB = searchCB;
		data->searchType = searchType;
		
		if (data->win NEQ NULL)
		{
				data->listID= SHOW_PAIRED_LIST;
			data->list_win = mmi_btips_bmg_ShowPairedDeviceList(data->win);
		}

	}

	/*Do the initialization of MMI BTIPS state-machine here, if any*/
	TRACE_EVENT_P1("mmi_btips_bmg_devices_win_create - new window %x", data->win);

	return data->win;

}
/*******************************************************************************

 $Function:		mmi_btips_bmg_connected_devices_win_create

 $Description:		This function performs the necessary steps to create the a window to handle the
 				viewfinder related softkeys and the viewfinder bitmaps. It will be removed, 
 				or rather remove itself on returning the data to the Mfw.

 $Returns:		T_MFW_HND : Window Handle to the New Window, Null if failed.

 $Arguments:		parent : pointer to the parent window.
 
*******************************************************************************/
T_MFW_HND mmi_btips_bmg_connected_devices_win_create(T_MFW_HND parent, BOOL searchType, T_MFW_BTIPS_SEARCH_CB searchCB)	//CONNECTED
{

	T_MMI_Btips_Bmg_DevicesWin_data *data;
	T_MFW_WIN * win;
	
	TRACE_FUNCTION ("mmi_btips_bmg_connected_devices_win_create()");
	data = (T_MMI_Btips_Bmg_DevicesWin_data *)mfwAlloc((U16)sizeof(T_MMI_Btips_Bmg_DevicesWin_data));

	data->win =(T_MFW_WIN * ) winCreate(parent, 0, MfwWinVisible|MfwWinSuspend|MfwWinResume|MfwWinDelete, (MfwCb)mmi_btips_bmg_connected_devices_win_cb);
	TRACE_EVENT_P2("New window - %x , parent = %x",parent,data->win);

	if (data->win == NULL)
	{
	
		//Failed to start : Free Memory, and exit
		mfwFree((U8 *)data, (U16)sizeof(T_MMI_Btips_Bmg_DevicesWin_data));
		data = NULL;
		return data;
	}
	else
	{

		/*
		** Setup the Dialog control functions
		*/
		data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_btips_bmg_connected_devices_exec_cb;
		data->mmi_control.data = data;
		data->parent=parent;
		win = (T_MFW_WIN *)getWinData(data->win);
		win->user = (void *)data;

		
		if (data->win NEQ NULL)
		{
				//data->listID= SHOW_PAIRED_LIST;
			TRACE_FUNCTION("data->win NEQ NULL");
			data->list_connected_win = mmi_btips_bmg_ShowConnectedDevicesList(data->win);
		}

	}

	/*Do the initialization of MMI BTIPS state-machine here, if any*/
	TRACE_EVENT_P1("mmi_btips_bmg_devices_win_create - new window %x", data->win);

	return data->win;
}
/*******************************************************************************

 $Function:		mmi_btips_bmg_devices_root_cb

 $Description:		This function is the notification from MFW to MMI for the FM Stack events that needs 
 				updation on the FM Radio Main Window

 $Returns:		None

 $Arguments:	
 
*******************************************************************************/
static int mmi_btips_bmg_devices_root_cb(T_MFW_EVENT evnt, void *para)
{
	T_MFW_HND win = mfw_parent(mfw_header());
	T_MFW_WIN * win_data;
	T_MMI_Btips_Bmg_DevicesWin_data *data;
	MfwBtBmgEvent bmgBtEvent;

	win_data = ((T_MFW_HDR *) win)->data;
	data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;
	if(para != NULL)
	{
		bmgBtEvent = ((T_BTIPS_MMI_IND * )para)->data.bmgBtEvent;
	}

	TRACE_FUNCTION_P1("mmi_btips_bmg_devices_root_cb - %d", evnt);	
	switch (evnt)
	{
		case E_BTIPS_BMG_NAME_RESULT:
			/*Copy BD_ADDR and DeviceName from the Para*/
			if(INQUIRY_STARTED == data->inquiryState)
			{		
				OS_MemCopy((U8*)&data->deviceList[data->noInquiredDevices].bdAddr, (U8*)&bmgBtEvent.bdAddr, sizeof(BD_ADDR));
				strcpy(data->deviceList[data->noInquiredDevices].name, bmgBtEvent.p.deviceName);
				data->deviceList[data->noInquiredDevices].myIndex = data->noInquiredDevices;
				data->noInquiredDevices++;
				
				/*Signal to create a new list with all newly found devices once the current list is deleted*/
				SEND_EVENT (data->win, BTIPS_BMG_INQUIRY_RESULT_DEVICES_INIT,0, (void *)NULL);
				/*Delete the current list whatever - previous window showing paired devices or
				the window showing the devices being discovered*/
				SEND_EVENT (data->win, BTIPS_BMG_DEVICES_LIST_WIN_DEINIT, 0, (void *)NULL);
			}
			else
				return MFW_EVENT_REJECTED;
			break;

		case E_BTIPS_BMG_SEARCH_COMPLETE:
			data->inquiryState = INQUIRY_COMPLETE;
			/*Signal to create a new list with all newly found devices once the current list is deleted*/
			SEND_EVENT (data->win, BTIPS_BMG_INQUIRY_RESULT_DEVICES_INIT,0, (void *)NULL);
			/*Delete the current list : window showing the devices being discovered
			Title should change to Find New Device*/
			SEND_EVENT (data->win, BTIPS_BMG_DEVICES_LIST_WIN_DEINIT, 0, (void *)NULL);
			break;
			
		case E_BTIPS_BMG_SEARCH_CANCELLED:
#if 0
/*We fail to get this event, hence commented for the time-being*/
		 	/*User might have requested to do a bonding while the inquiry was ongoing
			Now that the inquiry is cancelled, start the bond-process*/
			if(BOND_USER_REQUESTED == data->bondState)
				SEND_EVENT (data->win, BTIPS_BMG_BONDING_REQUEST_INIT, 0, (void *)NULL);
#endif
			break;
			
		case E_BTIPS_BMG_BOND_COMPLETE:
				{
				MfwBtBmgEvent bmgBtEvent = ((T_BTIPS_MMI_IND * )para)->data.bmgBtEvent;
				TRACE_EVENT_P1("E_BTIPS_BMG_BOND_COMPLETE: current state = %d", data->bondState);
				
				
				if(BOND_PIN_TAKEN == data->bondState)
				{
					data->bondState = BOND_NOT_STARTED;

					if (bmgBtEvent.errCode == 0) //Success
					{
						mmi_btips_app_show_text(win, "Bonded with",data->deviceList[data->currentListIndex-1].name, NULL);
					}
					else
					{
						mmi_btips_app_show_text(win, "Bond failed with",data->deviceList[data->currentListIndex-1].name, NULL);
					}
					//26-07-07
					mfw_btips_bmgSetDeviceNameInRecord(&data->deviceList[data->currentListIndex-1].bdAddr, data->deviceList[data->currentListIndex-1].name);

					dialog_info_destroy(data->bondCancellationWindow);
					data->bondCancellationWindow = NULL;
					
					if(BTIPS_GENERIC_DEVICE_SEARCH == data->searchType)
					{
						SEND_EVENT (data->win, BTIPS_BMG_PAIRED_DEVICES_INIT,0, (void *)NULL);
					}
					else
					{
						if (bmgBtEvent.errCode == 0) 
							data->searchCB(data->deviceList[data->currentListIndex-1].bdAddr);
						else
						{
							BD_ADDR bdAddr = {0};
							data->searchCB(bdAddr);
						}
					}
				}
				break;
			}
		case E_BTIPS_BMG_BOND_CANCELLED:
#if 0
				/*Signal to create a new list with all newly found devices once the current list is deleted*/
				SEND_EVENT (data->win, BTIPS_BMG_PAIRED_DEVICES_INIT,0, (void *)NULL);
				/*Delete the current list whatever - previous window showing paired devices or
				the window showing the devices being discovered*/
				SEND_EVENT (data->win, BTIPS_BMG_DEVICES_LIST_WIN_DEINIT, 0, (void *)NULL);
#endif
			break;
			
		case E_BTIPS_BMG_DEVICE_SERVICES : 
			
			TRACE_EVENT_P1(" mmi_btips_bmg_devices_root_cb services mask %x",bmgBtEvent.p.discoveredServicesMask);
			dialog_info_destroy (g_win_infodialog);
			if(bmgBtEvent.p.discoveredServicesMask)
				mfw_btips_bmgSetDeviceRecord(&bmgBtEvent.bdAddr, bmgBtEvent.p.discoveredServicesMask);
			
			mmi_btips_bmg_services_editor_create(win , bmgBtEvent.p.discoveredServicesMask);
			break;
	
    		default:
			TRACE_EVENT("E_BTIPS_BMG_DEFAULT");
			 return MFW_EVENT_REJECTED;
	}

	dspl_Enable(1);
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:		mmi_btips_bmg_devices_exec_cb

 $Description:		This is the dialog control function for the FMRadio Window. It
 				receives the signals from the MFW and determines what action, if any, to take.

 $Returns:		None

 $Arguments:		win :		The pointer to the window handler, so the function can reference the
							dynamic user data, if any.
 				event :		The incoming event
 				value :		Generic incoming Parameter, it will be 'event' dependant
 				parameter :	Generic incoming Parameter, it will be 'event' dependant

*******************************************************************************/
static int mmi_btips_bmg_devices_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN			* win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_Bmg_DevicesWin_data * data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;
	
	TRACE_FUNCTION("mmi_btips_bmg_devices_exec_cb()");
	TRACE_EVENT_P2("mmi_btips_bmg_devices_exec_cb - list-win - %x %d", data->list_win, event);

	switch (event)
	{
		case BTIPS_BMG_PAIRED_DEVICES_INIT:
			/*Create the Paired device List in Resume of mmi_btips_bmg_devices_win_cb*/
			data->listID = SHOW_PAIRED_LIST;
			break;
			
		case  BTIPS_BMG_INQUIRY_RESULT_DEVICES_INIT:
			/* create the list with the newly discovered devices Resume of mmi_btips_bmg_devices_win_cb*/
			data->listID = SHOW_SEARCH_LIST;
			break;

		case BTIPS_BMG_DEVICES_LIST_WIN_DEINIT:
			if(data->list_win)
			{
				listsDestroy(data->list_win);
			}
			break;
			
		case BTIPS_BMG_BONDING_REQUEST_INIT:
			TRACE_EVENT("BTIPS_BMG_BONDING_REQUEST_INIT");
			if(INQUIRY_NOT_STARTED == data->inquiryState || INQUIRY_COMPLETE == data->inquiryState)
			{
						
				data->inquiryState = INQUIRY_NOT_STARTED;
				/*Launch the editor for taking pin-code request*/
				data->pinCode_editor = mmi_btips_bmg_pinCode_editor_create(win);
				SEND_EVENT(data->pinCode_editor, BTIPS_BMG_LOCAL_EDITOR_INIT, 0, 0); 
			}
			break;
			
		case BTIPS_BMG_BONDING_REQUEST_SENT:
			data->bondCancellationWindow = mmi_btips_bmg_ask_cancelBond(win, "Bonding", "in Progress", mmi_btips_bmg_bonding_cancelled_cb);
			break;
			
		case BTIPS_BMG_DEVICES_EXIT_WIN:
			mmi_btips_bmg_devices_win_exit(data->win);
			break;
		case LISTS_REASON_RESUME:
			break;
		case LISTS_REASON_SUSPEND:
			break;
		case LISTS_REASON_VISIBLE:
			break;

		default:
			TRACE_EVENT("BTIPS BMG DEVICES DEFAULT ");
	}
		
		TRACE_EVENT_P2("EXEC CB LEAVE - list-win - %x %d", data->list_win, event);	

	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:		mmi_btips_bmg_connected_devices_exec_cb

 $Description:		This is the dialog control function for the FMRadio Window. It
 				receives the signals from the MFW and determines what action, if any, to take.

 $Returns:		None

 $Arguments:		win :		The pointer to the window handler, so the function can reference the
							dynamic user data, if any.
 				event :		The incoming event
 				value :		Generic incoming Parameter, it will be 'event' dependant
 				parameter :	Generic incoming Parameter, it will be 'event' dependant

*******************************************************************************/
static int mmi_btips_bmg_connected_devices_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)	//CONNECTED
{
	T_MFW_WIN			* win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_Bmg_DevicesWin_data * data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;
	
	TRACE_FUNCTION("mmi_btips_bmg_connected_devices_exec_cb()");
	TRACE_EVENT_P2("mmi_btips_bmg_connected_devices_exec_cb - list-win - %x %d", data->list_win, event);

	switch (event)
	{			
		case BTIPS_BMG_CONNECTED_DEVICES_EXIT_WIN:
			TRACE_FUNCTION("BTIPS_BMG_CONNECTED_DEVICES_EXIT_WIN");
			mmi_btips_bmg_connected_devices_win_exit(data->win);
			break;
		case LISTS_REASON_RESUME:
			break;
		case LISTS_REASON_SUSPEND:
			break;
		case LISTS_REASON_VISIBLE:
			break;

		default:
			TRACE_EVENT("BTIPS_BMG_CONNECTED_DEVICES_DEFAULT ");
	}
		
		TRACE_EVENT_P2("EXEC CB LEAVE - list-win - %x %d", data->list_win, event);	

	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:		mmi_btips_bmg_devices_win_cb

 $Description:		This function is the windows callback function for the mmi FMRadio Root window.
 				It has no display.

 $Returns:		MFW_EVENT_CONSUMED 

 $Arguments:		event		- window handle event
 				win			- FMRadio window

*******************************************************************************/
static int mmi_btips_bmg_devices_win_cb (MfwEvt evt, MfwWin *win)
{
	T_MMI_Btips_Bmg_DevicesWin_data	*data = (T_MMI_Btips_Bmg_DevicesWin_data *)win->user;

	TRACE_FUNCTION("mmi_btips_bmg_devices_win_cb()");
	TRACE_EVENT_P2("mmi_btips_bmg_devices_win_cb - list-data - %x %d",data->list_win, data->listID);

	switch (evt)
	{
		case MfwWinVisible:  /* window is visible  */	    
			TRACE_EVENT("MfwWinVisible");
		break;
		case MfwWinSuspend:  /* window is suspended  */
			TRACE_EVENT("MfwWinSuspend");
		break;
		case MfwWinResume: 	/* window is resumed */
			TRACE_EVENT("MfwWinResume");
			
			if(BOND_PIN_TAKEN != data->bondState )
			{
			/*Thsi window is resumed because 
			1. User navigated back from the Device List OR wanted to refresh the list
			2. After bonding process, the editor is destroyed. In this case, there is no 
			menu_list_data to be destroyed*/
				if (data->menu_list_data != NULL)
				{
					mfwFree( (U8 *)data->menu_list_data->List,((data->listCount) * sizeof(T_MFW_MNU_ITEM) ));
					mfwFree ((U8 *)data->menu_list_data, sizeof(ListMenuData));
				}
			}
			if(SHOW_PAIRED_LIST == data->listID)
				data->list_win = mmi_btips_bmg_ShowPairedDeviceList(data->win);
			else if(SHOW_SEARCH_LIST == data->listID)
				data->list_win = mmi_btips_bmg_ShowInquiredDeviceList(data->win);
			else if(SHOW_NONE_EXIT == data->listID)
				{}
			else if(SHOW_NO_LIST == data->listID)
				{/*No action*/}
				
		break;
		case MfwWinDelete:   /* window will be deleted   */        
			TRACE_EVENT("MfwWinDelete");
		break;
		case MfwWinFocussed: /* input focus / selected   */	
			TRACE_EVENT("MfwWinFocussed");
		break;
		default:
			TRACE_EVENT("DEFAULT");
			return MFW_EVENT_REJECTED;
	}
	dspl_Enable(1);
	
	return MFW_EVENT_CONSUMED;

}


/*******************************************************************************

 $Function:		mmi_btips_bmg_connected_devices_win_cb

 $Description:		This function is the windows callback function for the mmi FMRadio Root window.
 				It has no display.

 $Returns:		MFW_EVENT_CONSUMED 

 $Arguments:		event		- window handle event
 				win			- FMRadio window

*******************************************************************************/
static int mmi_btips_bmg_connected_devices_win_cb (MfwEvt evt, MfwWin *win)		//CONNECTED
{
	T_MMI_Btips_Bmg_DevicesWin_data	*data = (T_MMI_Btips_Bmg_DevicesWin_data *)win->user;

	TRACE_FUNCTION("mmi_btips_bmg_connected_devices_win_cb()");
	TRACE_EVENT_P1("mmi_btips_bmg_connected_devices_win_cb - list-data - %x ",data->list_connected_win);

	switch (evt)
	{
		case MfwWinVisible:  /* window is visible  */	    
			TRACE_EVENT("MfwWinVisible");
		break;
		case MfwWinSuspend:  /* window is suspended  */
			TRACE_EVENT("MfwWinSuspend");
		break;
		case MfwWinResume: 	/* window is resumed */
			TRACE_EVENT("MfwWinResume");
			if (data->menu_list_data != NULL)
			{
				mfwFree( (U8 *)data->menu_list_connected_data->List,((data->listCountConnectedDevices) * sizeof(T_MFW_MNU_ITEM) ));
				mfwFree ((U8 *)data->menu_list_connected_data, sizeof(ListMenuData));
			}
			/*noConnectedDevices:- This value was just now computed in the List Menu creation. 
			Now the control is here because the a device was disconnected and 
			the list may have to be recreated depending on noConnectedDevices */
			if(data->noConnectedDevices)
			{
				data->list_win = mmi_btips_bmg_ShowConnectedDevicesList(data->win);
			}
		break;
		case MfwWinDelete:   /* window will be deleted   */        
			TRACE_EVENT("MfwWinDelete");
		break;
		case MfwWinFocussed: /* input focus / selected   */	
			TRACE_EVENT("MfwWinFocussed");
		break;
		default:
			TRACE_EVENT("DEFAULT");
			return MFW_EVENT_REJECTED;
	}


	return MFW_EVENT_CONSUMED;

}
/*******************************************************************************

 $Function:     mmi_btips_bmg_devices_exit

 $Description:  exits the FM application
 				
 $Returns:    None

 $Arguments:  win

*******************************************************************************/
static void mmi_btips_bmg_devices_win_exit (T_MFW_HND win)
{
	T_MFW_WIN			* win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_Bmg_DevicesWin_data * data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;
	
	TRACE_FUNCTION("mmi_btips_bmg_devices_exit()");

	mfw_btips_delete(data->mfwwin);
	if(data->win != NULL)
	{
		winDelete (data->win);
		mfwFree((void *)data, sizeof (T_MMI_Btips_Bmg_DevicesWin_data));
	}
	return;
}

/*******************************************************************************

 $Function:     mmi_btips_bmg_connected_devices_win_exit

 $Description:  exits the FM application
 				
 $Returns:    None

 $Arguments:  win

*******************************************************************************/
static void mmi_btips_bmg_connected_devices_win_exit (T_MFW_HND win)
{
	T_MFW_WIN			* win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_Bmg_DevicesWin_data * data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;
	
	TRACE_FUNCTION("mmi_btips_bmg_connected_devices_win_exit()");

	mfw_btips_delete(data->mfwwin);
	if(data->win != NULL)
	{
		winDelete (data->win);
		mfwFree((void *)data, sizeof (T_MMI_Btips_Bmg_DevicesWin_data));
	}
	return;
}

/******************************************************************************
		BTIPS BMG EDITOR for Phone Name
******************************************************************************/

/*******************************************************************************

 $Function:    	mmi_btips_bmg_phoneName_editor_create

 $Description:	Create a window for entering the text
 
 $Returns:		window handle

 $Arguments:	parent_window - parent window.
 				
*******************************************************************************/

static T_MFW_HND mmi_btips_bmg_phoneName_editor_create(MfwHnd parent_window)
{

	T_MMI_Btips_Bmg_PhoneName_Win_data     * data = (T_MMI_Btips_Bmg_PhoneName_Win_data *)mfwAlloc (sizeof (T_MMI_Btips_Bmg_PhoneName_Win_data));
	T_MFW_WIN  * win;
	TRACE_FUNCTION ("mmi_btips_bmg_phoneName_editor_create()");
	
	// Create window handler
	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, NULL);
	if (data->win EQ NULL)
	{
		return NULL;
	}

	// connect the dialog data to the MFW-window
	data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_btips_bmg_phoneName_editor_exec_cb;
	data->mmi_control.data   = data;
	win                      = ((T_MFW_HDR *)data->win)->data;
	win->user                = (void *)data;
	data->parent             = parent_window;

	winShow(data->win);
	return data->win;
}

/*******************************************************************************

 $Function:    	mmi_btips_bmg_phoneName_editor_exec_cb

 $Description:	Callback handler for events sent by the CENTREEDIT editor

 $Returns:		none

 $Arguments:	win - current window
 				event - event id
 				parameter - optional data.
*******************************************************************************/

static void mmi_btips_bmg_phoneName_editor_exec_cb(T_MFW_HND win, USHORT event, void *parameter)
{	
	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_Bmg_PhoneName_Win_data         * data = (T_MMI_Btips_Bmg_PhoneName_Win_data *)win_data->user;

#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA editor_data;
#endif

	TRACE_FUNCTION ("mmi_btips_bmg_phoneName_editor_exec_cb()");

	switch (event)
	{
	case BTIPS_BMG_LOCAL_EDITOR_INIT:
			/* SPR#1428 - SH - New Editor changes */
			data->local_device = TRUE;	//Indicates we are setting a local device name
			memset(data->phoneNameBuffer,0,BT_MAX_REM_DEV_NAME);
			/*Display the current value*/
			mfw_btips_bmgGetLocalDeviceName(data->phoneNameBuffer);
#ifdef NEW_EDITOR
			AUI_edit_SetDefault(&editor_data);
			AUI_edit_SetDisplay(&editor_data, CENTRE_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
			AUI_edit_SetEvents(&editor_data, *(USHORT*)parameter, TRUE, FOREVER, (T_AUI_EDIT_CB)mmi_btips_bmg_phoneName_editor_edit_cb);
			AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtBtipsPhoneName, NULL);
			AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, TRUE, TxtSoftBack);
			AUI_edit_SetMode(&editor_data, ED_MODE_ALPHA, ED_CURSOR_UNDERLINE);
			AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, data->phoneNameBuffer, PHB_MAX_LEN);
			data->phoneName_editor = AUI_edit_Start(data->win, &editor_data);
#else /* NEW_EDITOR */

			SmsSend_loadEditDefault(&editor_data);
			bookSetEditAttributes( CENTRE_EDITOR, COLOUR_EDITOR_XX, 0, edtCurBar1, 0, 
			(char *) data->phoneNameBuffer, NUMBER_LENGTH, &editor_data.editor_attr);
			
			editor_data.TextId            = TxtBtipsPhoneName;  
			editor_data.editor_attr.text     = (char *)data->phoneNameBuffer;  
			editor_data.editor_attr.size     = PHB_MAX_LEN; 
			editor_data.LeftSoftKey        = TxtSoftOK;
			editor_data.RightSoftKey       = TxtDelete;
			editor_data.Identifier 	    = *(USHORT*)parameter;
			editor_data.Callback           = (T_EDIT_CB)mmi_btips_bmg_phoneName_editor_edit_cb;
			editor_data.mode		    = E_EDIT_ALPHA_MODE;
			editor_data.destroyEditor        = TRUE ; 
			data->phoneName_editor = editor_start(data->win,&editor_data);  
		/* start the Service Centre Number editor */ 
#endif /* NEW_EDITOR */
		break;
	case BTIPS_BMG_REMOTE_EDITOR_INIT:
		{		
			T_MMI_Btips_Bmg_DevicesWin_data 		*parent_data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;	

			/* SPR#1428 - SH - New Editor changes */
				/*T_MFW_WIN   * win_data_remote = ((T_MFW_HDR *) win)->data;
				tBookStandard      * data_remote    = (tBookStandard *)win_data_remote->user;
	
				T_MFW_WIN *parent_win_data = ((T_MFW_HDR *) data_remote->parent_win)->data;
   				T_MMI_Btips_Bmg_DevicesWin_data * parent_data     = (T_MMI_Btips_Bmg_DevicesWin_data *)parent_win_data->user;
				T_MFW_HND parent_win = parent_data->win;*/
			data->local_device = FALSE;	//Indicates we are setting a local device name
#ifdef NEW_EDITOR
			AUI_edit_SetDefault(&editor_data);
			AUI_edit_SetDisplay(&editor_data, CENTRE_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
			AUI_edit_SetEvents(&editor_data, *(USHORT*)parameter, TRUE, FOREVER, (T_AUI_EDIT_CB)mmi_btips_bmg_phoneName_editor_edit_cb);
			AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtBtipsPhoneName, NULL);
			AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, TRUE, TxtSoftBack);
			AUI_edit_SetMode(&editor_data, ED_MODE_ALPHA, ED_CURSOR_UNDERLINE);
			AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, data->phoneNameBuffer, PHB_MAX_LEN);
			data->phoneName_editor = AUI_edit_Start(data->win, &editor_data);
#else /* NEW_EDITOR */

			SmsSend_loadEditDefault(&editor_data);
			bookSetEditAttributes( CENTRE_EDITOR, COLOUR_EDITOR_XX, 0, edtCurBar1, 0, 
			(char *) data->phoneNameBuffer, NUMBER_LENGTH, &editor_data.editor_attr);
			
			editor_data.TextId            = TxtBtipsPhoneName;  
			editor_data.editor_attr.text     = (char *)data->phoneNameBuffer;  
			editor_data.editor_attr.size     = PHB_MAX_LEN; 
			editor_data.LeftSoftKey        = TxtSoftOK;
			editor_data.RightSoftKey       = TxtDelete;
			editor_data.Identifier 	    = *(USHORT*)parameter;
			editor_data.Callback           = (T_EDIT_CB)mmi_btips_bmg_phoneName_editor_edit_cb;
			editor_data.mode		    = E_EDIT_ALPHA_MODE;
			editor_data.destroyEditor        = TRUE ; 
			data->phoneName_editor = editor_start(data->win,&editor_data);  
		/* start the Service Centre Number editor */ 
#endif /* NEW_EDITOR */
			break;
		}
	case BTIPS_BMG_EDITOR_CANCEL:
		
		break;
	default:
		TRACE_EVENT("Err: Default");
		return;
	}
}

/*******************************************************************************

 $Function:    	mmi_btips_bmg_phoneName_editor_edit_cb

 $Description:	Callback function for the center number editor

 $Returns:		none

 $Arguments:	win - current window
 				Identifier - unique id
 				reason - event cause
*******************************************************************************/

static void mmi_btips_bmg_phoneName_editor_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_Bmg_PhoneName_Win_data      * data     = (T_MMI_Btips_Bmg_PhoneName_Win_data *)win_data->user;

	TRACE_FUNCTION ("mmi_btips_bmg_phoneName_editor_edit_cb()");

	TRACE_EVENT_P1("PhoneName Editor: %s", data->phoneNameBuffer);

	switch (reason)
	{
	case INFO_KCD_LEFT:
		TRACE_EVENT("mmi_btips_bmg_phoneName_editor_edit_cb,INFO_KCD_LEFT ");

		if(data->local_device)
			{
				if(!mfw_btips_bmgSetLocalDeviceName(data->phoneNameBuffer))
						mmi_btips_app_show_info(win,TxtBtipsRename,TxtFailed,ONE_SECS,NULL);
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				AUI_edit_Destroy(data->phoneName_editor);
#else /* NEW_EDITOR */
				editor_destroy(data->phoneName_editor);
#endif /* NEW_EDITOR */
			}
		else
			{
				T_MMI_Btips_Bmg_DevicesWin_data *parent_data= (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user	;
				if(!mfw_btips_bmgSetRemoteDeviceName(&parent_data->deviceList[parent_data->currentListIndex-1].bdAddr, data->phoneNameBuffer))
						mmi_btips_app_show_info(win,TxtBtipsRename,TxtFailed,ONE_SECS,NULL);
				#ifdef NEW_EDITOR
				AUI_edit_Destroy(data->phoneName_editor);
#else /* NEW_EDITOR */
				editor_destroy(data->phoneName_editor);
#endif /* NEW_EDITOR */
			}

				mmi_btips_bmg_phoneName_editor_destroy(win);
		break;
  
	case INFO_KCD_RIGHT:
	case INFO_KCD_HUP:
		#ifdef NEW_EDITOR
				AUI_edit_Destroy(data->phoneName_editor);
#else /* NEW_EDITOR */
				editor_destroy(data->phoneName_editor);
#endif /* NEW_EDITOR */

				mmi_btips_bmg_phoneName_editor_destroy(win);
				data->phoneName_editor= 0;
		break;
	default:
		
		break;
	}
}
/*******************************************************************************

 $Function:    	mmi_btips_bmg_phoneName_editor_destroy

 $Description:	Destroy the windows for entering the text
 
 $Returns:		none

 $Arguments:	own_window - window.
 				
*******************************************************************************/

static void mmi_btips_bmg_phoneName_editor_destroy(MfwHnd own_window)
{
  T_MFW_WIN * win_data;
  T_MMI_Btips_Bmg_PhoneName_Win_data    * data = NULL; 

  TRACE_FUNCTION ("mmi_btips_bmg_phoneName_editor_destroy()");

  if (own_window)
	{
	  win_data = ((T_MFW_HDR *)own_window)->data;
		if (win_data != NULL) 
			data = (T_MMI_Btips_Bmg_PhoneName_Win_data *)win_data->user;

	  if (data)
		{
			
			
			// Delete WIN handler 
		  win_delete (data->win);

		  // Free Memory
			mfwFree ((void *)data, sizeof (T_MMI_Btips_Bmg_PhoneName_Win_data));
			//win_data->user = NULL; 
		}
	  else
		{
			TRACE_EVENT ("mmi_btips_bmg_phoneName_editor_destroy() called twice");
		}
	}
}





/*******************************************************************************

 $Function:    	mmi_btips_bmg_pinCode_editor_create

 $Description:	Create a window for entering the text
 
 $Returns:		window handle

 $Arguments:	parent_window - parent window.
 				
*******************************************************************************/

T_MFW_HND mmi_btips_bmg_pinCode_editor_create(MfwHnd parent_window)
{

	T_MMI_Btips_Bmg_PinCode_Win_data     * data = (T_MMI_Btips_Bmg_PinCode_Win_data *)mfwAlloc (sizeof (T_MMI_Btips_Bmg_PinCode_Win_data));
	T_MFW_WIN  * win;
	TRACE_FUNCTION ("mmi_btips_bmg_pinCode_editor_create()");
	TRACE_EVENT_P1 ("mmi_btips_bmg_pinCode_editor_create - %x", parent_window);
	// Create window handler
	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, NULL);
	if (data->win EQ NULL)
	{
		return NULL;
	}

	// connect the dialog data to the MFW-window
	data->mmi_control.dialog = (T_DIALOG_FUNC)mmi_btips_bmg_pinCode_editor_exec_cb;
	data->mmi_control.data   = data;
	win                      = ((T_MFW_HDR *)data->win)->data;
	win->user                = (void *)data;
	data->parent             = parent_window;

	winShow(data->win);
	return data->win;
}


/*******************************************************************************

 $Function:    	mmi_btips_bmg_pinCode_editor_exec_cb

 $Description:	Callback handler for events sent by the CENTREEDIT editor

 $Returns:		none

 $Arguments:	win - current window
 				event - event id
 				parameter - optional data.
*******************************************************************************/

static void mmi_btips_bmg_pinCode_editor_exec_cb(T_MFW_HND win, USHORT event, void *parameter)
{	
	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_Bmg_PinCode_Win_data         * data = (T_MMI_Btips_Bmg_PinCode_Win_data *)win_data->user;

#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA editor_data;
#endif

	TRACE_FUNCTION ("mmi_btips_bmg_pinCode_editor_exec_cb()");
	memset(data->pinCodeBuffer,0,4);
	switch (event)
	{
	case BTIPS_BMG_LOCAL_EDITOR_INIT:
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_SetDefault(&editor_data);
			AUI_edit_SetDisplay(&editor_data, CENTRE_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
			AUI_edit_SetEvents(&editor_data, *(USHORT*)parameter, TRUE, FOREVER, (T_AUI_EDIT_CB)mmi_btips_bmg_pinCode_editor_edit_cb);
			AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtBtipsEnterPassKey, NULL);
			AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, TRUE, TxtSoftBack);
			AUI_edit_SetMode(&editor_data, 0, ED_CURSOR_UNDERLINE);
			AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, data->pinCodeBuffer,BTL_BMG_MAX_PIN_LENGTH );//PHB_MAX_LEN sundeep
			data->pinCode_editor = AUI_edit_Start(data->win, &editor_data);
#else /* NEW_EDITOR */

			SmsSend_loadEditDefault(&editor_data);
			bookSetEditAttributes( CENTRE_EDITOR, COLOUR_EDITOR_XX, 0, edtCurBar1, 0, 
			(char *) data->pinCodeBuffer, NUMBER_LENGTH, &editor_data.editor_attr);
			
			
			editor_data.TextId            = TxtBtipsEnterPassKey;  
			editor_data.editor_attr.text     = (char *)data->pinCodeBuffer;  
			editor_data.editor_attr.size     = PHB_MAX_LEN; 
			editor_data.LeftSoftKey        = TxtSoftOK;
			editor_data.RightSoftKey       = TxtDelete;
			editor_data.Identifier 	    = *(USHORT*)parameter;
			editor_data.Callback           = (T_EDIT_CB)mmi_btips_bmg_pinCode_editor_edit_cb;
			editor_data.mode		    = E_EDIT_ALPHA_MODE;
			editor_data.destroyEditor        = TRUE ; 
			data->phoneName_editor = editor_start(data->win,&editor_data);  
		/* start the Service Centre Number editor */ 
#endif /* NEW_EDITOR */
		break;
		
	case BTIPS_BMG_EDITOR_CANCEL:
		
		break;
	default:
		TRACE_EVENT("Err: Default");
		return;
	}
}

/*******************************************************************************

 $Function:    	mmi_btips_bmg_pinCode_editor_edit_cb

 $Description:	Callback function for the center number editor

 $Returns:		none

 $Arguments:	win - current window
 				Identifier - unique id
 				reason - event cause
*******************************************************************************/

static void mmi_btips_bmg_pinCode_editor_edit_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_Bmg_PinCode_Win_data      * data     = (T_MMI_Btips_Bmg_PinCode_Win_data *)win_data->user;
	T_MFW_HND parent_win = data->parent;
	T_MFW_WIN   * parent_win_data = ((T_MFW_HDR *) parent_win)->data;
	
	TRACE_FUNCTION_P1 ("mmi_btips_bmg_pinCode_editor_edit_cb: pincodeEditor = %s", data->pinCodeBuffer);
	
	switch (reason)
	{
	case INFO_KCD_LEFT:
		TRACE_EVENT("mmi_btips_bmg_pinCode_editor_edit_cb,INFO_KCD_LEFT ");
		
				if(strlen(data->pinCodeBuffer) > 16)// sundeep
				{
					//mmi_btips_app_show_info(win,TxtBtipsApp, TxtBtipsFailed,ONE_SECS, mmi_btips_refreshScreen);//sundeep
					mmi_btips_app_show_text(win,"Passkey Invalid", "Try Again", NULL);
					memset(data->pinCodeBuffer,0,16);
					SEND_EVENT (data->win, BTIPS_BMG_INQUIRY_RESULT_DEVICES_INIT,0, (void *)NULL);
					//SEND_EVENT (data->parent, BTIPS_BMG_PAIRED_DEVICES_INIT,0,0);

				}
				
				if(TRUE == g_btipsData.incomingPinRequestState)
				{
					mfw_btips_bmgSendPin((const BD_ADDR *) &(g_btipsData.bd_addr_incomingPinReq), data->pinCodeBuffer, strlen(data->pinCodeBuffer));	
					g_btipsData.incomingPinRequestState = FALSE;
				}
				else
				{
					T_MMI_Btips_Bmg_DevicesWin_data *parent_data = (T_MMI_Btips_Bmg_DevicesWin_data * )parent_win_data->user;	
					parent_data->bondState = BOND_PIN_TAKEN;
					/*When the Bond is cancelled or it completes successfully, we need to display the 
					Paired device list. The list gets actually created and displayed only when the WinResume
					event is received for the parent window*/
					SEND_EVENT (parent_data->win, BTIPS_BMG_INQUIRY_RESULT_DEVICES_INIT,0, (void *)NULL);

					if(mmi_btips_bmgBond(data->parent, data->pinCodeBuffer))
						SEND_EVENT(data->parent, BTIPS_BMG_BONDING_REQUEST_SENT, 0, 0); 
				}
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				AUI_edit_Destroy(data->pinCode_editor);
#else /* NEW_EDITOR */
				editor_destroy(data->pinCode_editor);
#endif /* NEW_EDITOR */

				mmi_btips_bmg_pinCode_editor_destroy(win);
		break;
  
	case INFO_KCD_RIGHT:
	case INFO_KCD_HUP:

				if(TRUE == g_btipsData.incomingPinRequestState)
				{
					g_btipsData.incomingPinRequestState = FALSE;
				}
				else
				{
					T_MMI_Btips_Bmg_DevicesWin_data *parent_data = (T_MMI_Btips_Bmg_DevicesWin_data * )parent_win_data->user;	
					parent_data->bondState = BOND_NOT_STARTED;
					
					/*We need to display the Paired device list. The list gets actually created and displayed only when the WinResume
					event is received for the parent window*/
					SEND_EVENT (parent_data->win, BTIPS_BMG_PAIRED_DEVICES_INIT,0, (void *)NULL);
				}
				
		#ifdef NEW_EDITOR
				AUI_edit_Destroy(data->pinCode_editor);
#else /* NEW_EDITOR */
				editor_destroy(data->pinCode_editor);
#endif /* NEW_EDITOR */

				mmi_btips_bmg_pinCode_editor_destroy(win);
				data->pinCode_editor= 0;

		break;
	default:
		
		break;
	}
}
/*******************************************************************************

 $Function:    	mmi_btips_bmg_pinCode_editor_destroy

 $Description:	Destroy the windows for entering the text
 
 $Returns:		none

 $Arguments:	own_window - window.
 				
*******************************************************************************/

static void mmi_btips_bmg_pinCode_editor_destroy(MfwHnd own_window)
{
  T_MFW_WIN * win_data;
  T_MMI_Btips_Bmg_PinCode_Win_data    * data = NULL; 

  TRACE_FUNCTION ("mmi_btips_bmg_pinCode_editor_destroy()");

  if (own_window)
	{
	  win_data = ((T_MFW_HDR *)own_window)->data;
		if (win_data != NULL) 
			data = (T_MMI_Btips_Bmg_PinCode_Win_data *)win_data->user;

	  if (data)
		{
			
			
			// Delete WIN handler 
		  win_delete (data->win);

		  // Free Memory
			mfwFree ((void *)data, sizeof (T_MMI_Btips_Bmg_PinCode_Win_data));
			//win_data->user = NULL; 
		}
	  else
		{
			TRACE_EVENT ("mmi_btips_bmg_pinCode_editor_destroy() called twice");
		}
	}
}

/*******************************************************************************
						BMG Editors END
*******************************************************************************/


/*******************************************************************************

 $Function:    	mmi_BtipsDevices

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgDevices(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	if(mfw_btips_getRadioStatus() == TRUE)//sundeep
		mmi_btips_bmg_devices_win_create(parent_win, BTIPS_GENERIC_DEVICE_SEARCH, NULL);
	else
		mmi_btips_app_show_info(parent_win,TxtBtipsApp, TxtBtipsNotOn,ONE_SECS, mmi_btips_refreshScreen);//sundeep
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	mmi_btips_bmgSetVisibility

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgSetVisibility(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	bookMenuStart(parent_win,  (MfwMnuAttr *)btipsBmgSetVisibilityMenuAttributes(),0);
	return MFW_EVENT_CONSUMED;
	
}

/*******************************************************************************

 $Function:    	mmi_btips_bmgConnectedDevices

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgConnectedDevices(MfwMnu* m,MfwMnuItem* i)	//CONNECTED
{
	T_MFW_HND parent_win = mfwParent(mfw_header());

	if(mfw_btips_bmg_getConnectedDevicesCount()!=0)
	{
		mmi_btips_bmg_connected_devices_win_create(parent_win, BTIPS_CONNECTED_DEVICES_SEARCH, NULL);
	}
	else
	{
		mmi_btips_app_show_info(parent_win, TxtNo, TxtBtipsDevices,ONE_SECS, mmi_btips_refreshScreen);
	}

	return MFW_EVENT_CONSUMED;
	
}
	
/*******************************************************************************

 $Function:    	mmi_BtipsPhoneName

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgPhoneName(MfwMnu* m, MfwMnuItem* i)
{
/*
Display current name
Give LSK - Change, RSK - Back
*/
	T_MFW_HND parent_win = mfwParent(mfw_header());
	T_MFW_HND phoneName_editor_win;
	phoneName_editor_win = mmi_btips_bmg_phoneName_editor_create(parent_win);
	SEND_EVENT(phoneName_editor_win, BTIPS_BMG_LOCAL_EDITOR_INIT,0,0); 
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	mmi_BtipsHidePhone

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgHidePhone(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	mfw_btips_bmgSetPhoneVisibility(FALSE);
	win_delete(parent_win);
	dspl_Enable(1);
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	mmi_btips_bmgSetConnectPermissionAllow

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgSetConnectPermissionAllow(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());

	TRACE_EVENT_P1("parent of  mmi_btips_bmgSetConnectPermissionAllow %x",parent_win);
	if(mmi_btips_bmg_SetConnectPermission(parent_win,TRUE))
		mmi_btips_app_show_info(0,TxtBtipsApp,TxtBtipsAlwaysAllow,ONE_SECS,NULL);
	else
		mmi_btips_app_show_info(0,TxtBtipsAlwaysAllow,TxtFailed,ONE_SECS,NULL);
		
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	mmi_btips_bmgSetConnectPermissionAsk

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgSetConnectPermissionAsk(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
//	mfw_btips_bmgDeviceSetConnectPermissionAsk();

	TRACE_EVENT_P1("parent of  mmi_btips_bmgSetConnectPermissionAllow %x",parent_win);

	if(mmi_btips_bmg_SetConnectPermission(parent_win,FALSE))
		mmi_btips_app_show_info(0,TxtBtipsApp,TxtBtipsAlwaysAsk,ONE_SECS,NULL);
	else
		mmi_btips_app_show_info(0,TxtBtipsAlwaysAsk,TxtFailed,ONE_SECS,NULL);
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	mmi_btips_bmgDeviceShowServices

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
BOOL mmi_btips_bmgDeviceShowServices(T_MFW_HND win , BOOL discoverServices)
{
	SdpServicesMask sdpServicesMask =0;
	
	BtStatus btStatus;
	char bdAddrstring[256];
	
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	tBookStandard      * data     = (tBookStandard *)win_data->user;
	
	T_MFW_WIN *parent_win_data = ((T_MFW_HDR *) data->parent_win)->data;
   	T_MMI_Btips_Bmg_DevicesWin_data * parent_data     = (T_MMI_Btips_Bmg_DevicesWin_data *)parent_win_data->user;
	T_MFW_HND parent_win = parent_data->win;

	TRACE_EVENT_P1("parent_win of  mmi_btips_bmgDeviceShowServices %x",parent_win);
	
	if(discoverServices==TRUE)
		{
			sdpServicesMask = (SDP_SERVICE_DUN|SDP_SERVICE_FTP|SDP_SERVICE_A2DP|SDP_SERVICE_HFP|SDP_SERVICE_HSP|SDP_SERVICE_OPP);
			//sdpServicesMask=	(SDP_SERVICE_SPP | SDP_SERVICE_DUN | SDP_SERVICE_OPP | SDP_SERVICE_FTP | SDP_SERVICE_HSP | SDP_SERVICE_A2DP | SDP_SERVICE_AVRCP | SDP_SERVICE_FAX | SDP_SERVICE_BPP | SDP_SERVICE_BIP | SDP_SERVICE_HFP | SDP_SERVICE_SAP | SDP_SERVICE_PBAP | SDP_SERVICE_HID);//sundeep
			bdaddr_ntoa(&parent_data->deviceList[parent_data->currentListIndex-1].bdAddr,bdAddrstring);
			TRACE_EVENT_P2("mmi_btips_bmgDeviceShowServices Discover  %x, addr = %s",sdpServicesMask,bdAddrstring);
			btStatus = mfw_btips_bmg_DiscoverServices(&parent_data->deviceList[parent_data->currentListIndex-1].bdAddr,sdpServicesMask);
			if(btStatus !=MFW_BTIPS_SUCCESS )
				{
					mmi_btips_app_show_info(0, TxtBtipsServices, TxtError, ONE_SECS,NULL);
					
				}
				
		}
	else if(discoverServices == FALSE)
		{
			 sdpServicesMask = mfw_btips_bmgDeviceGetServices(&parent_data->deviceList[parent_data->currentListIndex-1].bdAddr);
			 TRACE_EVENT_P1("sdpServicesMask of  mmi_btips_bmgDeviceShowServices %x",sdpServicesMask);

				if(sdpServicesMask)
					{
						TRACE_EVENT_P1("sdpServicesMask of  mmi_btips_bmgDeviceShowServices %x",sdpServicesMask);
						mmi_btips_bmg_services_editor_create(win,sdpServicesMask);
						 return  SERVICESMASK_NOT_EMPTY;
					}
				TRACE_FUNCTION("mmi_btips_bmgDeviceShowServices SERVICESMASK_EMPTY");
				return  SERVICESMASK_EMPTY;
			
		}
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	mmi_btips_bmgDeviceDelete

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgDeviceDelete(MfwMnu* m, MfwMnuItem* i)
{

	T_MFW_HND win = mfw_parent(mfw_header());
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	tBookStandard      * data     = (tBookStandard *)win_data->user;
	
	T_MFW_WIN *parent_win_data = ((T_MFW_HDR *) data->parent_win)->data;
   	T_MMI_Btips_Bmg_DevicesWin_data * parent_data     = (T_MMI_Btips_Bmg_DevicesWin_data *)parent_win_data->user;
	T_MFW_HND parent_win = parent_data->win;

	TRACE_EVENT_P2("current list index = %d 	, Parent Win = %x",parent_data->currentListIndex,parent_win);
	TRACE_EVENT_P1("BdAddr = %s",parent_data->deviceList[parent_data->currentListIndex-1].bdAddr);
	if(mfw_btips_bmgDeviceDelete(&parent_data->deviceList[parent_data->currentListIndex-1].bdAddr))
		mmi_btips_app_show_info(0,TxtRecord, TxtDeleted,ONE_SECS, NULL);
	else
		mmi_btips_app_show_info(0,TxtDelete,TxtFailed,ONE_SECS,NULL);
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************
 $Function:    	mmi_btips_bmgDeviceDisconnect

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgDeviceDisconnect(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	//mfw_btips_bmgDeviceDisconnect();
	mmi_btips_app_show_info(0,TxtBtipsApp,TxtBtipsDisconnect,ONE_SECS,NULL);
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	mmi_btips_bmgDeviceSetRemotename

 $Description:	Currently not being used

 $Returns:		

 $Arguments:	
 
*******************************************************************************/

int mmi_btips_bmgDeviceSetRemotename(MfwMnu* m, MfwMnuItem* i)
{	
	T_MFW_HND parent_win = mfwParent(mfw_header());
	T_MFW_HND phoneName_editor_win;
	//T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
	//T_MMI_Btips_Bmg_DevicesWin_data         * data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;

	

	TRACE_FUNCTION ("mmi_btips_bmgDeviceSetRemotename()");
		//memset(data->phoneNameBuffer,0,BT_MAX_REM_DEV_NAME);
	/*Display the current value*/
	//mfw_btips_bmgGetLocalDeviceName(data->phoneNameBuffer);
	phoneName_editor_win = mmi_btips_bmg_phoneName_editor_create(parent_win);
		
	SEND_EVENT(phoneName_editor_win, BTIPS_BMG_REMOTE_EDITOR_INIT,0,0); 
	
	//mmi_btips_app_show_info(0,TxtBtipsApp,TxtBtipsRename,ONE_SECS,NULL);
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	mmi_BtipsShowPhone

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgShowPhone(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	mfw_btips_bmgSetPhoneVisibility(TRUE);
	win_delete(parent_win);
	dspl_Enable(1);
	return MFW_EVENT_CONSUMED;
}

USHORT mmi_btips_bmgIsVisible( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	if(mfw_btips_bmgGetPhoneVisibility())
		return TRUE;
	
	return FALSE;
		
}

USHORT mmi_btips_bmgIsInvisible( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	if(mfw_btips_bmgGetPhoneVisibility())
		return FALSE;
	
	return TRUE;
		
}

/*******************************************************************************

 $Function:    	mmi_btips_bmg_pin_req

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmg_pin_req(const BD_ADDR *bdAddr)
{
	U8 	pin[] = "0000";
	U8  len = 4;
	/*
	o. Store the BD_ADDR in the context of the Window created to display the editor
	1. Create an editor
	2. Try to get the name of the peer device requesting pairing
	3. Take input from the user for the 4-digit Pass-Key
	*/
	mfw_btips_bmgSendPin(bdAddr, pin, len);	
	mmi_btips_app_show_info(0,TxtBtipsApp,TxtBtipsBond,ONE_SECS,NULL);

}


/*******************************************************************************

 $Function:    	mmi_btips_bmgBond

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_bmgBond(T_MFW_HND win, U8 pin[])
{

	U8  len = strlen(pin);
	T_MFW_WIN * win_data;
	T_MMI_Btips_Bmg_DevicesWin_data *data;

	win_data = ((T_MFW_HDR *) win)->data;
	data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;

	TRACE_FUNCTION ("mmi_btips_bmgBond()");
	return mfw_btips_bmgBond(&data->deviceList[data->currentListIndex-1].bdAddr, pin, len);	
}

/*******************************************************************************

 $Function:    	mmi_btips_bmg_ShowPairedDeviceList

 $Description:	Create a menu list of all paired devices; MMI => MFW  => BTL-BMG

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND mmi_btips_bmg_ShowPairedDeviceList(T_MFW_HND win)
{
 	T_MFW_WIN			* win_data = ((T_MFW_HDR *) win)->data;
 	T_MMI_Btips_Bmg_DevicesWin_data * data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;
 	USHORT  list_count=0;
 	//char  bdAddr[BDADDR_NTOA_SIZE];
 
        TRACE_FUNCTION ("mmi_btips_bmg_ShowPairedDeviceList()");
 	data->menu_list_data = (ListMenuData *)mfwAlloc(sizeof(ListMenuData));
 	if(data->menu_list_data == NULL )
 	{
 		TRACE_ERROR("Memory failure.");
 		return;
 	}
	 data->inquiryState = INQUIRY_NOT_STARTED;
	 	
 	/*Find out how many paired devices are present and allocate for the menu-list-data as
 	noPairedDevices + ONE for the top most entry ("Find new device")*/
 	//MFW has populated the (data->deviceList, &data->noInquiredDevices);
 
 	mfw_btips_bmg_getPairedDevices(data->deviceList, &data->noPairedDevices);	
 	data->listCount = data->noPairedDevices + 1;
 
 	data->menu_list_data->List = (T_MFW_MNU_ITEM *)mfwAlloc( (data->listCount) * sizeof(T_MFW_MNU_ITEM) );
 	if(data->menu_list_data->List == NULL)
 	{
 		TRACE_ERROR("BTIPS BMG Device List Memory failure.");
 		return;
 	}
 	/*First entry : if searching is complete - "Find More" OR it will display "searching"*/
 	mnuInitDataItem(&data->menu_list_data->List[list_count]);	
 	data->menu_list_data->CursorPosition = 1;	
 
 	data->menu_list_data->List[list_count].str  =  FindNewDeviceString;
 	if(data->noPairedDevices)
 	{
 		data->menu_list_data->CursorPosition = 2;
 	}
 	
 	data->menu_list_data->List[list_count].flagFunc = item_flag_none;
 
 
 	/*Second entry onwards : The newly found Device List*/
 	for (list_count=1; list_count<data->noPairedDevices+1; list_count++)
 	{
 			//bdaddr_ntoa(&data->deviceList[list_count-1].bdAddr, bdAddr);
 			mnuInitDataItem(&data->menu_list_data->List[list_count]);
 			data->menu_list_data->List[list_count].str  = (char *)data->deviceList[list_count-1].name;
 			data->menu_list_data->List[list_count].flagFunc = item_flag_none;
 	}
	
	data->menu_list_data->LeftSoftKey = TxtSelect;
 	data->menu_list_data->RightSoftKey = TxtSoftBack;
 	data->menu_list_data->ListLength = data->listCount; //(data->noPairedDevices + 1)
 	data->menu_list_data->ListPosition = 1;
 	data->menu_list_data->SnapshotSize =data->listCount; //(data->noPairedDevices + 1)
 	data->menu_list_data->Font = 1;
 	data->menu_list_data->KeyEvents = KEY_ALL;
 	data->menu_list_data->Reason = 0;
 	data->menu_list_data->Strings = TRUE;
 	data->menu_list_data->Attr   = (MfwMnuAttr*)&BtipsDeviceList_Attrib;
 	data->menu_list_data->autoDestroy    = TRUE;
 	data->currentListIndex = data->menu_list_data->CursorPosition;
 
 	//Display the List window that was created above
 	listDisplayListMenu(win, data->menu_list_data, (ListCbFunc)mmi_btips_bmg_device_list_cb, 0);
 	TRACE_EVENT_P2("Num of Paired Devices %d, CurrIndex %d",data->noPairedDevices, data->currentListIndex);
	displaySoftKeys(data->menu_list_data->LeftSoftKey,data->menu_list_data->RightSoftKey);	
	if(data->menu_list_data->ListLength > 1)
 	displayMenuKeys(MENU_KEY_UP_DOWN_CENTER);
	data->list_win = data->menu_list_data->win;
 	return data->menu_list_data->win;
}

/*******************************************************************************

 $Function:    	mmi_btips_bmg_ShowConnectedDevicesList

 $Description:	Create a menu list of all paired devices; MMI => MFW  => BTL-BMG

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND mmi_btips_bmg_ShowConnectedDevicesList(T_MFW_HND win)	//CONNECTED
{
 	T_MFW_WIN			* win_data = ((T_MFW_HDR *) win)->data;
 	T_MMI_Btips_Bmg_DevicesWin_data * data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;
 	USHORT  list_count=0;
 	//char  bdAddr[BDADDR_NTOA_SIZE];
 
        TRACE_FUNCTION ("mmi_btips_bmg_ShowConnectedDevicesList()");
 	data->menu_list_connected_data = (ListMenuData *)mfwAlloc(sizeof(ListMenuData));
 	if(data->menu_list_connected_data == NULL )
 	{
 		TRACE_ERROR("Memory failure.");
 		return NULL;
 	}
 
 	/*Find out how many Connected devices are present and allocate for the menu-list-data as
 	noConnectedDevices  */
 	
 	mfw_btips_bmg_getConnectedDevices(data->connected_deviceList, &data->noConnectedDevices);	
 	data->listCountConnectedDevices = data->noConnectedDevices;
	TRACE_EVENT_P1("Total Connected Devices in List = %d", data->noConnectedDevices);
	
	if(0 == data->noConnectedDevices)
	{
		return NULL;
	}

 	data->menu_list_connected_data->List = (T_MFW_MNU_ITEM *)mfwAlloc( (data->listCountConnectedDevices) * sizeof(T_MFW_MNU_ITEM) );
 	if(data->menu_list_connected_data->List == NULL)
 	{
 		TRACE_ERROR("BTIPS BMG Connected Device List Memory failure.");
 		return data->menu_list_connected_data->win;
 	}

  	data->menu_list_connected_data->CursorPosition = 1;
 	for (list_count=0; list_count<data->noConnectedDevices; list_count++)
 	{
 			mnuInitDataItem(&data->menu_list_connected_data->List[list_count]);
 			data->menu_list_connected_data->List[list_count].str  = (char *)data->connected_deviceList[list_count].name;
 			data->menu_list_connected_data->List[list_count].flagFunc = item_flag_none;
 	}
 
  	data->menu_list_connected_data->LeftSoftKey = TxtBtipsDisconnect;
 	data->menu_list_connected_data->RightSoftKey = TxtSoftBack;
 	data->menu_list_connected_data->ListLength = data->listCountConnectedDevices; 
 	data->menu_list_connected_data->ListPosition = 1;
 	data->menu_list_connected_data->SnapshotSize =data->listCountConnectedDevices;
 	data->menu_list_connected_data->Font = 1;
 	data->menu_list_connected_data->KeyEvents = KEY_ALL;
 	data->menu_list_connected_data->Reason = 0;
 	data->menu_list_connected_data->Strings = TRUE;
 	data->menu_list_connected_data->Attr   = (MfwMnuAttr*)&BtipsDeviceList_Attrib;
 	data->menu_list_connected_data->autoDestroy    = TRUE;
 	data->currentListIndex = data->menu_list_connected_data->CursorPosition;
 
 	//Display the List window that was created above
 	listDisplayListMenu(win, data->menu_list_connected_data, (ListCbFunc)mmi_btips_bmg_connected_device_list_cb, 0);
 	TRACE_EVENT_P2("Num of Connected Devices %d, CurrIndex %d",data->noConnectedDevices, data->currentListIndex);
	displaySoftKeys(data->menu_list_connected_data->LeftSoftKey,data->menu_list_connected_data->RightSoftKey);	
	if(data->menu_list_connected_data->ListLength > 1)
 	displayMenuKeys(MENU_KEY_UP_DOWN_CENTER);
	data->list_connected_win = data->menu_list_connected_data->win;
 	return data->menu_list_connected_data->win;
}

/*******************************************************************************

 $Function:    	mmi_btips_bmg_ShowInquiredDeviceList

 $Description:	Create a menu list of all paired devices; BTL-BMG => MFW (Context Switch)
 											  MFW => MMI (notification)
 											  MMI (gets data collected device list again from mfw)

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND mmi_btips_bmg_ShowInquiredDeviceList(T_MFW_HND win)
{
	T_MFW_WIN			* win_data = ((T_MFW_HDR *) win)->data;
	T_MMI_Btips_Bmg_DevicesWin_data * data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;
	USHORT  list_count=0;
	//char  bdAddr[BDADDR_NTOA_SIZE];

	TRACE_FUNCTION ("mmi_btips_bmg_ShowInquiredDeviceList()");	
	data->menu_list_data = (ListMenuData *)mfwAlloc(sizeof(ListMenuData));
	if(data->menu_list_data == NULL )
	{
		TRACE_ERROR("Memory failure.");
		return;
	}
	data->listCount = data->noInquiredDevices + 1;

	/*Find out how many paired devices are present and allocate for the menu-list-data as
	noPairedDevices + ONE for the top most entry ("Find new device")
	MFW has populated the (data->deviceList, &data->noInquiredDevices)*/;
	data->menu_list_data->List = (T_MFW_MNU_ITEM *)mfwAlloc( (data->listCount) * sizeof(T_MFW_MNU_ITEM) );
	if(data->menu_list_data->List == NULL)
	{
		TRACE_ERROR("BTIPS BMG Device List Memory failure.");
		return;
	}

	data->menu_list_data->LeftSoftKey = TxtSelect;
	data->menu_list_data->RightSoftKey= TxtCancel;
	data->menu_list_data->ListLength = data->listCount; 
	data->menu_list_data->ListPosition = 1;
	data->menu_list_data->CursorPosition = 1;
	data->menu_list_data->SnapshotSize =data->listCount; 
	data->menu_list_data->Font = 1;
	data->menu_list_data->KeyEvents = KEY_ALL;
	data->menu_list_data->Reason = 0;
	data->menu_list_data->Strings = TRUE;
	data->menu_list_data->Attr   = (MfwMnuAttr*)&BtipsDeviceList_Attrib;
	data->menu_list_data->autoDestroy    = TRUE;
	data->menu_list_data->CursorPosition = 1;	

	/*First entry : if searching is complete - "Find More" OR it will display "searching"*/
	mnuInitDataItem(&data->menu_list_data->List[list_count]);	
	switch(data->inquiryState)
	{
		case INQUIRY_NOT_STARTED:
			data->menu_list_data->List[list_count].str  =  FindNewDeviceString;
			break;
		case INQUIRY_STARTED:
			data->menu_list_data->List[list_count].str  =  SearchingDeviceString;
			if(data->noInquiredDevices)
			{
				data->menu_list_data->LeftSoftKey = TxtBtipsBond;
			}
			else
			{
				data->menu_list_data->LeftSoftKey = '\0';
			}
			break;
		case INQUIRY_CANCELLATION_STARTED:
		case INQUIRY_CANCELLATION_COMPLETE:
			data->menu_list_data->List[list_count].str  =  FindNewDeviceString;
			if(data->noInquiredDevices)
			{
				data->menu_list_data->LeftSoftKey = TxtBtipsBond;
			}
			else
			{
				data->menu_list_data->LeftSoftKey = '\0';
			}
			break;
		case INQUIRY_COMPLETE:
			data->menu_list_data->List[list_count].str  =  FindNewDeviceString;
			data->menu_list_data->RightSoftKey = TxtSoftBack;
			if(data->noInquiredDevices)
			{
				data->menu_list_data->ListPosition = 2;
				data->menu_list_data->LeftSoftKey = TxtBtipsBond;
			}
			break;
		}
	
	data->menu_list_data->List[list_count].flagFunc = item_flag_none;

	/*Second entry onwards : The newly found Device List*/
	for (list_count=1; list_count<data->noInquiredDevices+1; list_count++)
	{
			//bdaddr_ntoa(&data->deviceList[list_count-1].bdAddr, bdAddr);
			mnuInitDataItem(&data->menu_list_data->List[list_count]);
			data->menu_list_data->List[list_count].str  = (char *)data->deviceList[list_count-1].name;
			data->menu_list_data->List[list_count].flagFunc = item_flag_none;
	}

	data->currentListIndex = data->menu_list_data->CursorPosition;

	//Display the List window that was created above
	listDisplayListMenu(win, data->menu_list_data, (ListCbFunc)mmi_btips_bmg_device_list_cb, 0);
	TRACE_EVENT_P2("Num of Inquired Devices %d, CurrIndex %d",data->noInquiredDevices, data->currentListIndex);
	
	displaySoftKeys(data->menu_list_data->LeftSoftKey,data->menu_list_data->RightSoftKey);	
	if(data->menu_list_data->ListLength > 1)
	displayMenuKeys(MENU_KEY_UP_DOWN_CENTER);
	data->list_win =  data->menu_list_data->win;
	return data->menu_list_data->win;
}


static void mmi_btips_bmg_device_list_cb(T_MFW_HND * parent, ListMenuData * ListData)
{
	T_MFW_WIN			* win_data = ((T_MFW_HDR *) parent)->data;
	T_MMI_Btips_Bmg_DevicesWin_data * data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;

	TRACE_FUNCTION("mmi_btips_bmg_device_list_cb");
	TRACE_EVENT_P2("Reason %d, Current State = %d",ListData->Reason, data->inquiryState);
	switch(ListData->Reason)
	{
		case LISTS_REASON_SELECT:
			data->currentListIndex = ListData->CursorPosition;
			if(ListData->selectKey==TRUE)
			{				// Whats this value for			
			}
			TRACE_EVENT_P1("ListData->selectKey = %d", ListData->selectKey);
			if(0 == data->currentListIndex)
			{	
			/*User has selected "Find new device" or he just clicked on the display "searching" */
				TRACE_EVENT_P1("Find New Device: Current State = %d", data->inquiryState);
				if((INQUIRY_NOT_STARTED == data->inquiryState) || (INQUIRY_COMPLETE== data->inquiryState))
				{
					SEND_EVENT (parent, BTIPS_BMG_INQUIRY_RESULT_DEVICES_INIT, 0, (void *)NULL);
					data->noInquiredDevices = 0;
					data->inquiryState = INQUIRY_STARTED;
					mfw_btips_bmgSearch(data->searchType);
				}
			}
			else
			{
				if(INQUIRY_STARTED == data->inquiryState)
				{
					/*If the user has selected a particualr device thats currently discovered
					he now wishes to Bond/Connect to it. So Stop the inquiry now */
					data->inquiryState = INQUIRY_CANCELLATION_STARTED;
					mfw_btips_bmgCancelSearch();		
					/*We are not getting the search cancelled event, hence doing it this way*/
					data->inquiryState = INQUIRY_COMPLETE;
				}
				
				switch(data->inquiryState)
				{
					case INQUIRY_NOT_STARTED:
						if(BTIPS_GENERIC_DEVICE_SEARCH == data->searchType)
						{
							/*There is no Inquiry now and the user is viewing a list of
							previously paired devices. Now he has selected the options to 
							be exercised on paired devices: Services, Delete, Rename, etc..*/
							/* Create the  options menu*/
							bookMenuStart(parent, (MfwMnuAttr *)btipsBmgPairedDeviceMenuAttributes(),0);
						}
						else
						{
							data->searchCB(data->deviceList[data->currentListIndex-1].bdAddr);
						}
						break;
					case INQUIRY_CANCELLATION_STARTED:
							if(data->noInquiredDevices >= 1)
							{
							/*Mark that Bonding is requested on the currently selected device.
							Bonding will statr only when the inquiry cancellation event comes*/
							data->listID = SHOW_NO_LIST;
							data->bondState = BOND_USER_REQUESTED;
							}
						break;
					case INQUIRY_COMPLETE:
						/*Trigger Bonding on the currently selected device.*/
						data->listID = SHOW_NO_LIST;
						data->bondState = BOND_USER_REQUESTED;
						SEND_EVENT (parent, BTIPS_BMG_BONDING_REQUEST_INIT, 0, (void *)NULL);
						break;
				}
			}
			break;
					
		case LISTS_REASON_BACK:		
			TRACE_EVENT("Going back" );
			data->listID = SHOW_NONE_EXIT;
			if(INQUIRY_STARTED == data->inquiryState)
			{
					data->inquiryState = INQUIRY_CANCELLATION_STARTED;
					mfw_btips_bmgCancelSearch();
					SEND_EVENT (parent, BTIPS_BMG_PAIRED_DEVICES_INIT, 0, (void *)NULL);
			}
			else
				SEND_EVENT (data->win, BTIPS_BMG_DEVICES_EXIT_WIN, 0, (void *)NULL);
			break;
			
		case LISTS_REASON_DRAWCOMPLETE:
			TRACE_EVENT("LISTS_REASON_DRAWCOMPLETE " );
			break;
	}
}

//CONNECTED
static void mmi_btips_bmg_connected_device_list_cb(T_MFW_HND * parent, ListMenuData* ListConnectedData)
{
	T_MFW_WIN			* win_data = ((T_MFW_HDR *) parent)->data;
	T_MMI_Btips_Bmg_DevicesWin_data * data = (T_MMI_Btips_Bmg_DevicesWin_data *)win_data->user;

	TRACE_FUNCTION("mmi_btips_bmg_connected_device_list_cb");
	TRACE_EVENT_P2("Reason %d, Current Index = %d",ListConnectedData->Reason, ListConnectedData->CursorPosition);
	TRACE_EVENT_P3("List CB ++ ListWin = %x, %x, ListCount = %d",data->menu_list_connected_data->win, data->list_connected_win, data->listCountConnectedDevices);
	switch(ListConnectedData->Reason)
	{
		case LISTS_REASON_SELECT:
			TRACE_EVENT("LISTS_REASON_SELECT - Connected Device" );
			data->currentListIndex = ListConnectedData->CursorPosition;
			 data->noConnectedDevices--;
			if(data->connected_deviceList[data->currentListIndex].ServiceId == BTIPS_SDP_SERVICE_FTPS)
			{
				mfw_btips_ftpsDisconnect();
			}
			if(data->connected_deviceList[data->currentListIndex].ServiceId == BTIPS_SDP_SERVICE_OPPC)
			{
				mfw_btips_oppc_Disconnect();
			}
			if(data->connected_deviceList[data->currentListIndex].ServiceId == BTIPS_SDP_SERVICE_OPPS)
			{
				mfw_btips_opps_Disconnect();
			}
			if(data->connected_deviceList[data->currentListIndex].ServiceId == BTIPS_SDP_SERVICE_VG)
			{
				mfw_btips_vgDisconnect(data->connected_deviceList[data->currentListIndex].bdAddr);
			
			}
			if(data->connected_deviceList[data->currentListIndex].ServiceId == BTIPS_SDP_SERVICE_A2DP)
			{
				mfw_btips_a2dpDisconnect(data->connected_deviceList[data->currentListIndex].bdAddr);
			
			}
			
			if(0 == data->noConnectedDevices)
			{
				mmi_btips_app_show_info(data->win, TxtNo, TxtBtipsDevices,ONE_SECS, mmi_btips_ResumeBTMainMenu);
				SEND_EVENT (data->win, BTIPS_BMG_CONNECTED_DEVICES_EXIT_WIN, 0, (void *)NULL);
			}
			break;	
			
		case LISTS_REASON_BACK:		
			TRACE_EVENT("Going back" );
			SEND_EVENT (data->win, BTIPS_BMG_CONNECTED_DEVICES_EXIT_WIN, 0, (void *)NULL);
			break;
			
		case LISTS_REASON_DRAWCOMPLETE:
			TRACE_EVENT("LISTS_REASON_DRAWCOMPLETE " );
			break;
	
	}
}



static int mmi_btips_bmg_SetConnectPermission(T_MFW_HND win, BOOL permission)
{
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	tBookStandard      * data     = (tBookStandard *)win_data->user;
	
	T_MFW_WIN *parent_win_data = ((T_MFW_HDR *) data->parent_win)->data;
   	T_MMI_Btips_Bmg_DevicesWin_data * parent_data     = (T_MMI_Btips_Bmg_DevicesWin_data *)parent_win_data->user;
	T_MFW_HND parent_win = parent_data->win;

	TRACE_EVENT_P1("parent_win_data value  of  mmi_btips_bmg_SetConnectPermission %x",parent_win_data);
	TRACE_EVENT_P1("parent_win of  mmi_btips_bmg_SetConnectPermission %x",parent_win);
	TRACE_FUNCTION("mmi_btips_bmg_SetConnectPermission() ");
	
	if(permission)
		{
			return mfw_btips_bmgDeviceSetConnectPermissionAllow(&parent_data->deviceList[parent_data->currentListIndex-1].bdAddr, permission);
		}
	
	else
		{
			return mfw_btips_bmgDeviceSetConnectPermissionAsk(&parent_data->deviceList[parent_data->currentListIndex-1].bdAddr, permission);
		}

}

/*******************************************************************************
 $Function:    		mmi_btips_bmg_services_editor_create

 $Description:		Creates an editor for displaying the services supported by a device

 $Returns:		Execution status

 $Arguments:		parent_win		-Parent window
*******************************************************************************/
T_MFW_HND mmi_btips_bmg_services_editor_create(T_MFW_HND parent_win, SdpServicesMask sdpServicesMask)
{
	T_MFW_HND  win;
	T_AUI_EDITOR_DATA	editor_data;
	T_AUI_EDITOR_INFO 	*edtdata;
	 T_MFW_WIN	*win_data;

	TRACE_FUNCTION ("mmi_btips_bmg_services_editor_create");
	services_data = (T_MMI_BTIPS_SERVICES_STRUCT *)mfwAlloc(sizeof(T_MMI_BTIPS_SERVICES_STRUCT));
	services_data->edt_buf = (char *)mfwAlloc(BTIPS_MAX_SERVICES_BUFFER);
	

	memset(&editor_data,'\0',sizeof(editor_data));
	
	//Initialise the editor
	
	AUI_edit_SetDefault(&editor_data);
		
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII,(UBYTE *) services_data->edt_buf, BTIPS_MAX_SERVICES_BUFFER);

	AUI_edit_SetMode(&editor_data,ED_MODE_ALPHA,0);
		
	AUI_edit_SetTextStr(&editor_data, TxtNull,TxtSoftOK, TxtBtipsServices,NULL);
		
	AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, TRUE, TxtNull);
	
	AUI_edit_SetEvents(&editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)mmi_btips_bmg_services_editor_exec_cb);

	win = AUI_edit_Start(parent_win, &editor_data);
	
	
		TRACE_EVENT_P2("  sdpServicesMask = %d , SDP_SERVICE_OPP =%d",sdpServicesMask,sdpServicesMask& SDP_SERVICE_OPP);
		if(sdpServicesMask& SDP_SERVICE_OPP )
		{
			AUI_edit_InsertString(win , (UBYTE *) "Object Push",ATB_DCS_ASCII);
	
			AUI_edit_InsertChar(win , ctrlEnter);
		
		}

		TRACE_EVENT_P2("  sdpServicesMask = %d , SDP_SERVICE_HFP =%d",sdpServicesMask,sdpServicesMask& SDP_SERVICE_HFP);
		if(sdpServicesMask& SDP_SERVICE_HFP)
			{
				AUI_edit_InsertString(win , (UBYTE *) "Handsfree",ATB_DCS_ASCII);

				AUI_edit_InsertChar(win , ctrlEnter);
			}
		TRACE_EVENT_P2("  sdpServicesMask = %d , SDP_SERVICE_HSP =%d",sdpServicesMask,sdpServicesMask& SDP_SERVICE_HSP );
		if(sdpServicesMask& SDP_SERVICE_HSP )
			{
				AUI_edit_InsertString(win , (UBYTE *) "HeadSet",ATB_DCS_ASCII);

				AUI_edit_InsertChar(win , ctrlEnter);
		
			}
		TRACE_EVENT_P2("  sdpServicesMask = %d , SDP_SERVICE_FTP =%d",sdpServicesMask,sdpServicesMask& SDP_SERVICE_FTP );		
		if(sdpServicesMask& SDP_SERVICE_FTP )
			{
				AUI_edit_InsertString(win , (UBYTE *) "File Transfer",ATB_DCS_ASCII);
				AUI_edit_InsertChar(win , ctrlEnter);
			}
		TRACE_EVENT_P2("  sdpServicesMask = %d , SDP_SERVICE_DUN =%d",sdpServicesMask,sdpServicesMask& SDP_SERVICE_DUN);
		if(sdpServicesMask& SDP_SERVICE_DUN )
			{
				AUI_edit_InsertString(win , (UBYTE *) "Dial Up Networking",ATB_DCS_ASCII);

				AUI_edit_InsertChar(win , ctrlEnter);
			}
		TRACE_EVENT_P2("  sdpServicesMask = %d , SDP_SERVICE_A2DP =%d",sdpServicesMask,sdpServicesMask& SDP_SERVICE_A2DP);
		if(sdpServicesMask& SDP_SERVICE_A2DP)
			{
				AUI_edit_InsertString(win , (UBYTE *) "A2DP",ATB_DCS_ASCII);
	
				AUI_edit_InsertChar(win , ctrlEnter);
			}
		if(sdpServicesMask ==0)
			{

				AUI_edit_InsertString(win , (UBYTE *) "No Services",ATB_DCS_ASCII);
			}
		TRACE_EVENT_P1("editor_data->editor_attr.mode before %x",editor_data.editor_attr.mode);
			
		win_data	= ((T_MFW_HDR *)win)->data;
		edtdata      = (T_AUI_EDITOR_INFO *)win_data->user;
		edtdata->editor->attr->mode =  ED_MODE_READONLY;
			
		TRACE_EVENT_P1("editor_data->editor_attr.mode after %x",editor_data.editor_attr.mode);
		win_show(win);
		return win;
		
	}



/*******************************************************************************
 $Function:    		mmi_btips_bmg_services_editor_exec_cb

 $Description:		Event handler for the editor
 
 $Returns:		None

 $Arguments:		win			-current window
				identifier		-window id
				reason		-Event cause.
*******************************************************************************/
static void mmi_btips_bmg_services_editor_exec_cb(T_MFW_HND win, USHORT identifier, SHORT reason)
{
	TRACE_FUNCTION ("mmi_btips_bmg_services_editor_exec_cb()");
	switch (reason)
	{
		case INFO_KCD_LEFT:
		case INFO_KCD_HUP:	
		case INFO_KCD_RIGHT:
			mmi_btips_bmg_services_editor_destroy( );
			break;
		default:
			/* otherwise no action to be performed
			*/
			break;	
		
	}
}

/*******************************************************************************
 $Function:    		mmi_btips_bmg_services_editor_destroy

 $Description:		Frees the allocated memory

 $Returns:		None

 $Arguments:		None
*******************************************************************************/
void mmi_btips_bmg_services_editor_destroy(void)
{
	TRACE_FUNCTION ("mmi_btips_bmg_services_editor_destroy");
	mfwFree((U8 *)services_data->edt_buf, BTIPS_MAX_SERVICES_BUFFER);
	mfwFree((U8 *)services_data, sizeof(T_MMI_BTIPS_SERVICES_STRUCT));
}


T_MFW_HND mmi_btips_bmg_ask_cancelBond(T_MFW_HND parent, char * str1, char * str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION("mmi_btips_bmg_cancelBonding_ask");
	dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtCancel, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, FOREVER, KEY_LEFT|KEY_RIGHT  | KEY_CLEAR | KEY_HUP);
	 return info_dialog(parent,&display_info);
}

void mmi_btips_bmg_bonding_cancelled_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{	

	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_dialog_info      * data     = (T_dialog_info *)win_data->user;
	
	T_MFW_WIN *parent_win_data = ((T_MFW_HDR *) data->parent_win)->data;
   	T_MMI_Btips_Bmg_DevicesWin_data * parent_data     = (T_MMI_Btips_Bmg_DevicesWin_data *)parent_win_data->user;
	T_MFW_HND parent_win = parent_data->win;
	int oldBondState;
	
	TRACE_FUNCTION_P1("mmi_btips_bmg_bonding_cancelled_cb %d", reason);

	switch (reason)
	{
	case INFO_KCD_LEFT:
		TRACE_ERROR("No LSK for Bond Cancellation");
	    	break;
	case INFO_KCD_HUP:
	case INFO_KCD_RIGHT:
		/*Cancel the ongoing bond operation*/
		oldBondState = parent_data->bondState;
		parent_data->bondState == BOND_CANCELLED;
		TRACE_EVENT_P2("Bond State changed from %d to %d", oldBondState, parent_data->bondState);
		mfw_btips_bmgCancelBond(&parent_data->deviceList[parent_data->currentListIndex-1].bdAddr);
		break;
	case INFO_KCD_CLEAR:
	    	break;
	default:
    	break;
    }
}

void mmi_btips_ResumeBTMainMenu(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{	
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_dialog_info      * data     = (T_dialog_info *)win_data->user;
	
	TRACE_FUNCTION_P1("mmi_btips_ResumeBTMainMenu %d", reason);

	switch (reason)
	{
	case INFO_KCD_LEFT:
		TRACE_ERROR("No LSK - mmi_btips_refreshScreen");
	    	break;
	case INFO_KCD_HUP:
	case INFO_KCD_RIGHT:
		TRACE_ERROR("No RSK - mmi_btips_refreshScreen");
		break;
	case INFO_KCD_CLEAR:
	    	break;
	default:
    	break;
    }
	
dspl_Enable(1);
}


void mmi_btips_refreshScreen(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{	
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_dialog_info      * data     = (T_dialog_info *)win_data->user;
	
	TRACE_FUNCTION_P1("mmi_btips_refreshScreen %d", reason);

	switch (reason)
	{
	case INFO_KCD_LEFT:
		TRACE_ERROR("No LSK - mmi_btips_refreshScreen");
	    	break;
	case INFO_KCD_HUP:
	case INFO_KCD_RIGHT:
		TRACE_ERROR("No RSK - mmi_btips_refreshScreen");
		break;
	case INFO_KCD_CLEAR:
	    	break;
	default:
    	break;
    }
		dspl_Enable(1);
}


#endif
