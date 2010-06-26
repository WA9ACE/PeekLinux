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

/* GPF Includes */
#include "typedefs.h"
#include "vsi.h"

#include "Mfw_Btips.h"
#include "Mfw_BtipsBmg.h"
#include "mmiBtips.h"
#include "mmiBtipsBmg.h"

#ifdef FF_MMI_BTIPS_APP
EXTERN char BTStr[20];

T_MFW_HND mmi_btips_app_win_create(T_MFW_HND parent);
static int mmi_btips_app_win_cb (MfwEvt evt, MfwWin *win);
static int mmi_btips_app_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static void mmi_btips_app_exit (T_MFW_HND win);
static void mmi_btips_win_exit (T_MFW_HND win);

MMI_Btips_data g_btipsData = {0};
T_MFW_HND g_win_infodialog;

void mmi_btips_init(void)
{
	TRACE_FUNCTION("mmiBtipsInit: Starting Bluetooth Initialization");
	if(0 == g_btipsData.ipBtipsHandle)
 		g_btipsData.ipBtipsHandle = aci_create(btipsPrimHandler,NULL);	
	mfw_btips_init();
}


/*******************************************************************************

 $Function:     mmi_btips_radioOnStatus

 $Description:  	Create the window which is going to have
 				 the viewfinder bitmaps displayed on top of it.

 $Returns:		Always returns MFW_EVENT_CONSUMED (1)

 $Arguments:		m : pointer to the mnu data for the menu which caused this function to be called.
 				i   : pointer to the item data from the menu.

*******************************************************************************/
USHORT	mmi_btipsRadioOnStatus(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi)
{
	if(mfw_btips_getRadioStatus())
		return TRUE;
	else
		return FALSE;
}

/*******************************************************************************

 $Function:     mmi_btips_radioOffStatus

 $Description:  	Create the window which is going to have
 				 the viewfinder bitmaps displayed on top of it.

 $Returns:		Always returns MFW_EVENT_CONSUMED (1)

 $Arguments:		m : pointer to the mnu data for the menu which caused this function to be called.
 				i   : pointer to the item data from the menu.

*******************************************************************************/
USHORT	mmi_btipsRadioOffStatus(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi)
{
	if(mfw_btips_getRadioStatus())
		return FALSE;
	else
		return TRUE;
}

/*******************************************************************************

 $Function:     mmi_btips_radioOn

 $Description:  	Create the window which is going to have
 				 the viewfinder bitmaps displayed on top of it.

 $Returns:		Always returns MFW_EVENT_CONSUMED (1)

 $Arguments:		m : pointer to the mnu data for the menu which caused this function to be called.
 				i   : pointer to the item data from the menu.

*******************************************************************************/
 int  mmi_btipsRadioOn(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());

	TRACE_FUNCTION ("mmi_btipsRadioOn()");
	return mfw_btips_radioOn();
}

/*******************************************************************************

 $Function:     mmi_btips_radioOff

 $Description:  	Create the window which is going to have
 				 the viewfinder bitmaps displayed on top of it.

 $Returns:		Always returns MFW_EVENT_CONSUMED (1)

 $Arguments:		m : pointer to the mnu data for the menu which caused this function to be called.
 				i   : pointer to the item data from the menu.

*******************************************************************************/
GLOBAL int  mmi_btipsRadioOff(void)
{
	TRACE_FUNCTION ("mmi_btipsRadioOff()");

	return mfw_btips_radioOff();
}


/******************************************************************************
		MFW Window Module (Creation, Deletion, Callbacks for MFW, Keyboard, etc)
******************************************************************************/


void mmi_btips_app_incoming_pairing_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{	
	
	TRACE_FUNCTION("mmi_btips_app_incoming_pairing_cb");

	switch (reason)
	{
	case INFO_KCD_LEFT:
			/*Launch the editor for taking pin-code request, with the parent as btips_main window, not this dialog*/
			g_btipsData.pinCode_editor = mmi_btips_bmg_pinCode_editor_create(win);
			SEND_EVENT(g_btipsData.pinCode_editor, BTIPS_BMG_LOCAL_EDITOR_INIT, 0, 0); 
	    	break;
	case INFO_KCD_HUP:
	case INFO_KCD_RIGHT:
		/*Reject the pairing operatio*/
		mfw_btips_bmgSendPin(&g_btipsData.bd_addr_incomingPinReq, NULL, 0);
		break;
	case INFO_KCD_CLEAR:
	    	break;
	default:
    	break;
    }
}
/******************************************************************************
		MFW Window Module (Creation, Deletion, Callbacks for MFW, Keyboard, etc)
******************************************************************************/
static void mmi_btips_app_push_pull_event_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{	
	TRACE_FUNCTION("mmi_btips_app_push_pull_event_cb");

	switch (reason)
	{
		case INFO_KCD_LEFT:
			mfw_btips_oppsAcceptObjectRequest(TRUE);
		break;
	
		case INFO_KCD_HUP:
		case INFO_KCD_RIGHT:
			mfw_btips_oppsAcceptObjectRequest(FALSE);
		break;
	
		case INFO_KCD_CLEAR:
		/* Do nothing */
		break;

		default:
			
    		break;
	}
}

static void mmi_btips_app_ftp_accept_cb (T_MFW_HND win, UBYTE identifier, UBYTE reason)
{	
	TRACE_FUNCTION("mmi_btips_app_ftp_accept_cb");

	switch (reason)
	{
	case INFO_KCD_LEFT:
		TRACE_FUNCTION("mmi_btips_app_ftp_accept_cb: KCD LEFT");
		mfw_btips_ftpsAcceptObjectRequest (TRUE);
	break;
	
	case INFO_KCD_HUP:
	case INFO_KCD_RIGHT:
		/*Reject the pairing operatio*/
		TRACE_FUNCTION("mmi_btips_app_ftp_accept_cb: KCD RIGHT");
		mfw_btips_ftpsAcceptObjectRequest (FALSE);
		break;
	case INFO_KCD_CLEAR:
	    	break;
	default:
    	break;
    }
}
	
static char ftpRequest[256];
	
/*******************************************************************************

 $Function:		mmi_btips_app_root_cb

 $Description:		This function is the notification from MFW to MMI for the BT Stack events that needs 
 				updation on the BTIPS Main Window

 $Returns:		None

 $Arguments:	
 
*******************************************************************************/
int mmi_btips_app_root_cb(T_MFW_EVENT evnt, void *para)
{
	T_MFW_HND win = mfw_parent(mfw_header());
	MfwBtBmgEvent bmgBtEvent;
	
    	TRACE_FUNCTION ("mmi_btips_app_root_cb()");
	TRACE_EVENT_P1("mmi_btips_app_root_cb - %d", evnt);


	if(para != NULL)
	{
		bmgBtEvent = ((T_BTIPS_MMI_IND * )para)->data.bmgBtEvent;
	}	
	switch (evnt)
	{
		case E_BTIPS_POWERON_SUCCESS:
			TRACE_EVENT("E_BTIPS_POWERON_SUCCESS");
			mmi_btips_app_show_info(0,TxtBtipsApp,TxtBtipsOn,ONE_SECS,NULL);
			break;
			
		case E_BTIPS_POWERON_FAILURE:
			TRACE_EVENT("E_BTIPS_POWERON_FAILURE");
			mmi_btips_app_show_info(0,TxtBtipsApp,TxtBtipsFailed,ONE_SECS,NULL);
			break;
			
		case E_BTIPS_BT_NOT_ON:
			TRACE_EVENT("E_BTIPS_BT_NOT_ON");
			mmi_btips_app_show_info(0,TxtBtipsApp,TxtBtipsNotOn,ONE_SECS,NULL);
			break;
			
		
		case E_BTIPS_POWEROFF_SUCCESS:
			TRACE_EVENT("E_BTIPS_POWEROFF_SUCCESS");
			mmi_btips_app_show_info(0,TxtBtipsApp,TxtBtipsOff,ONE_SECS,NULL);
			break;
			
		case E_BTIPS_BMG_NAME_RESULT:
			TRACE_EVENT("E_BTIPS_BMG_NAME_RESULT : mmibtips");
			if(TRUE == g_btipsData.incomingPinRequestState)
			{
			
				//if(OS_MemCmp((U8*)&g_btipsData.bd_addr_incomingPinReq, sizeof(BD_ADDR), (U8*)&bmgBtEvent.bdAddr, sizeof(BD_ADDR)))
				//{
					strcpy(g_btipsData.deviceName, bmgBtEvent.p.deviceName); 
					mmi_btips_app_ask_info(0, "Allow connection with", g_btipsData.deviceName, mmi_btips_app_incoming_pairing_cb);

				#if 0
				else
				{
					g_btipsData.incomingPinRequestState = FALSE;
					//This case should not happen.
					TRACE_ERROR("INCORRECT NAME RECEIVED FOR PIN_REQ");
					return MFW_EVENT_REJECTED;
				}
				#endif
			}
			else
			{
				return MFW_EVENT_REJECTED;
			}

			break;
		case E_BTIPS_INCOMING_PIN_REQ:
			{
				MfwBtBmgEvent bmgBtEvent = ((T_BTIPS_MMI_IND * )para)->data.bmgBtEvent;
				TRACE_EVENT("E_BTIPS_INCOMING_PIN_REQ");
				OS_MemSet(g_btipsData.deviceName, 0, BT_MAX_REM_DEV_NAME);
				OS_MemCopy((U8*)&g_btipsData.bd_addr_incomingPinReq, (U8*)&bmgBtEvent.bdAddr, sizeof(BD_ADDR));
				g_btipsData.incomingPinRequestState = TRUE;
			}
			break;
		case E_BTIPS_BMG_PAIRING_COMPLETE:
			{
				MfwBtBmgEvent bmgBtEvent = ((T_BTIPS_MMI_IND * )para)->data.bmgBtEvent;
				TRACE_EVENT("E_BTIPS_INCOMING_PIN_REQ");
				mfw_btips_bmgGetDeviceName(&bmgBtEvent.bdAddr, g_btipsData.deviceName);
				mmi_btips_app_show_text(0, g_btipsData.deviceName, "Paired",NULL);
				break;
			}
		
		case E_BTIPS_FTPS_PUT_EVENT:
			{
				MfwBtFtpsEvent event = ((T_BTIPS_MMI_IND * )para)->data.ftpsBtEvent;
				TRACE_EVENT("E_BTIPS_FTPS_PUT_EVENT");
//				sprintf (ftpRequest, "FTP Put %s", event.p.ftpPutRequest.objectName);
//				mmi_btips_app_ask_info(0, ftpRequest, event.p.ftpPutRequest.deviceName, mmi_btips_app_ftp_accept_cb);
				mmi_btips_app_ask_info(0, "FTP", "PUT", mmi_btips_app_ftp_accept_cb);
			}
			break;
			
		case E_BTIPS_FTPS_GET_EVENT:
			{
				MfwBtFtpsEvent event = ((T_BTIPS_MMI_IND * )para)->data.ftpsBtEvent;
				TRACE_EVENT("E_BTIPS_FTPS_GET_EVENT");
				//sprintf (ftpRequest, "FTP Get %s", event.p.ftpGetRequest.objectName);
				//mmi_btips_app_ask_info(0, ftpRequest, event.p.ftpGetRequest.deviceName, mmi_btips_app_ftp_accept_cb);
				mmi_btips_app_ask_info(0, "FTP", " Get", mmi_btips_app_ftp_accept_cb);//sundeep
			}
			break;

		case E_BTIPS_FTPS_DELETE_EVENT:
			{
				MfwBtFtpsEvent event = ((T_BTIPS_MMI_IND * )para)->data.ftpsBtEvent;
				TRACE_EVENT("E_BTIPS_FTPS_DELETE_EVENT");
//				sprintf (ftpRequest, "FTP Delete %s", event.p.ftpDeleteRequest.objectName);
//				mmi_btips_app_ask_info(0, ftpRequest, event.p.ftpDeleteRequest.deviceName, mmi_btips_app_ftp_accept_cb);
				mmi_btips_app_ask_info(0, "FTP", "DELETE", mmi_btips_app_ftp_accept_cb);
			}
			break;
		/*case E_BTIPS_FTPS_COMPLETE_EVENT:
			{
				MfwBtFtpsEvent event = ((T_BTIPS_MMI_IND * )para)->data.ftpsBtEvent;
				TRACE_EVENT("E_BTIPS_FTPS_COMPLETE_EVENT");
				//BTStr =GoepOpName1(event->ftpsEvent->oper);
				mmi_btips_app_show_text(0, BTStr, "Completed",NULL);
				break;
			}*/
		case E_BTIPS_FTPS_CONNECT_EVENT:
			{
				MfwBtFtpsEvent event = ((T_BTIPS_MMI_IND * )para)->data.ftpsBtEvent;
				TRACE_EVENT("E_BTIPS_FTPS_CONNECT_EVENT");
				mmi_btips_app_show_text(0, 0, "Connected",NULL);
			}
			break;
		case E_BTIPS_FTPS_DISCONNECT_EVENT:
			{
				MfwBtFtpsEvent event = ((T_BTIPS_MMI_IND * )para)->data.ftpsBtEvent;
				TRACE_EVENT("E_BTIPS_FTPS_DISCONNECT_EVENT");
				mmi_btips_app_show_text(0, 0, "Disconnected",NULL);
			}
			break;
		case E_BTIPS_FTPS_ABORTED_EVENT:
			{
				MfwBtFtpsEvent event = ((T_BTIPS_MMI_IND * )para)->data.ftpsBtEvent;
				TRACE_EVENT("E_BTIPS_FTPS_ABORTED_EVENT");
				mmi_btips_app_show_text(0, 0, "Aborted",NULL);
			}
			break;
		case E_BTIPS_FTPS_PUT_COMP_EVENT:
			{
				MfwBtFtpsEvent event = ((T_BTIPS_MMI_IND * )para)->data.ftpsBtEvent;
				TRACE_EVENT("E_BTIPS_FTPS_PUT_COMP_EVENT");
				mmi_btips_app_show_text(0, "PUT", "Completed",NULL);
			}
			break;
		/*case E_BTIPS_FTPS_GET_COMP_EVENT:
			{
				MfwBtFtpsEvent event = ((T_BTIPS_MMI_IND * )para)->data.ftpsBtEvent;
				TRACE_EVENT("E_BTIPS_FTPS_GET_COMP_EVENT");
				mmi_btips_app_show_text(0, "GET", "Completed",NULL);
			}
			break;*/
		case E_BTIPS_FTPS_DELETE_COMP_EVENT:
			{
				MfwBtFtpsEvent event = ((T_BTIPS_MMI_IND * )para)->data.ftpsBtEvent;
				TRACE_EVENT("E_BTIPS_FTPS_DELETE_COMP_EVENT");
				mmi_btips_app_show_text(0, "DELETE", "Completed",NULL);
			}
			break;
		case E_BTIPS_VG_CONNECT_EVENT:
			{
				MfwBtVgEvent event = ((T_BTIPS_MMI_IND * )para)->data.vgBtEvent;
				mmi_btips_app_show_text (win, event.deviceName, "connected", NULL);
			}
			break;
			
		case E_BTIPS_VG_DISCONNECT_EVENT:
			{
				MfwBtVgEvent event = ((T_BTIPS_MMI_IND * )para)->data.vgBtEvent;
				mmi_btips_app_show_text (win, "HF", "disconnected", NULL);
			}
			break;

		case E_BTIPS_VG_AUDIO_CONNECT_EVENT:
			{
				MfwBtVgEvent event = ((T_BTIPS_MMI_IND * )para)->data.vgBtEvent;
				mmi_btips_app_show_text (win, "Audio", "connected", NULL);
			}
			break;

		case E_BTIPS_VG_AUDIO_DISCONNECT_EVENT:
			{
				MfwBtVgEvent event = ((T_BTIPS_MMI_IND * )para)->data.vgBtEvent;
				mmi_btips_app_show_text (win, "Audio", "disconnected", NULL);
			}
			break;
		case E_BTIPS_OPPS_PUSH_EVENT:
			{
				MfwBtOppsEvent oppsBtEvent = ((T_BTIPS_MMI_IND * )para)->data.oppsBtEvent;
				U8	tempString1[256];
				TRACE_EVENT("E_BTIPS_OPPS_PUSH_EVENT");
				sprintf(tempString1, "Receive VCard %s from %s", oppsBtEvent.p.opps.oppObjectName, oppsBtEvent.p.opps.deviceName);	
				mmi_btips_app_ask_info(0, tempString1, oppsBtEvent.p.opps.deviceName, mmi_btips_app_push_pull_event_cb);
				break;
			}
		
		case E_BTIPS_OPPS_PULL_EVENT:
			{
				MfwBtOppsEvent oppsBtEvent = ((T_BTIPS_MMI_IND * )para)->data.oppsBtEvent;
				U8	tempString1[256];
				TRACE_EVENT("E_BTIPS_OPPS_PULL_EVENT");
				sprintf(tempString1, "Send default VCard to %s", oppsBtEvent.p.opps.deviceName);	
				mmi_btips_app_ask_info(0, tempString1, oppsBtEvent.p.opps.deviceName, mmi_btips_app_push_pull_event_cb);
				break;
			}

//#if 0
		case E_BTIPS_OPPS_PULL_COMP_EVENT:
			{
				MfwBtOppsEvent oppsBtEvent = ((T_BTIPS_MMI_IND * )para)->data.oppsBtEvent;
				//mmi_btips_app_show_text(0, oppsBtEvent.p.opps.deviceName, "PULL Complete",NULL);
				mmi_btips_app_show_text(0, "Sent", "VCard",NULL);
				break;
			}	

		case E_BTIPS_OPPS_PUSH_COMP_EVENT:
			{
				MfwBtOppsEvent oppsBtEvent = ((T_BTIPS_MMI_IND * )para)->data.oppsBtEvent;
				//mmi_btips_app_show_text(0, oppsBtEvent.p.opps.deviceName, "PUSH Complete",NULL);
				mmi_btips_app_show_text(0, "Recived", "VCard",NULL);
				break;
			}
		
//#endif
    		default:
			TRACE_EVENT_P1("E_BTIPS_DEFAULT %d", evnt);
			return MFW_EVENT_REJECTED;
	}
	dspl_Enable(1);
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     		mmi_btips_app_show_info

 $Description: 	Displays the dialog box

 $Returns: 		Dialog window

 $Arguments:		parent -parent window for dialog
 				str1- String to be displayed in dialog
 				str2 -String to be displayed in dialog
 				callback- Callback function

*******************************************************************************/
MfwHnd mmi_btips_app_show_info(T_MFW_HND parent, int str1, int str2, int timer,T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;
	T_MFW_HND win;

      TRACE_FUNCTION ("mmi_btips_app_show_info()");

	/*
	** Create a timed dialog to display the Message .
	*/
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, timer, KEY_LEFT|KEY_RIGHT | KEY_CLEAR | KEY_HUP);

	win = info_dialog(parent, &display_info);
	return win;
}


/*******************************************************************************

 $Function:     		mmi_fmRadio_app_show_info

 $Description: 	Displays the dialog box

 $Returns: 		Dialog window

 $Arguments:		parent -parent window for dialog
 				str1- String to be displayed in dialog
 				str2 -String to be displayed in dialog
 				callback- Callback function

*******************************************************************************/
MfwHnd mmi_btips_app_show_text(T_MFW_HND parent, char * str1, char * str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;
	T_MFW_HND win;
	/*
	** Create a timed dialog to display the Message .
	*/
	dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtNull, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, ONE_SECS, KEY_LEFT|KEY_RIGHT | KEY_CLEAR | KEY_HUP);

	win = info_dialog(parent, &display_info);
	return win;
}

T_MFW_HND mmi_btips_app_ask_info(T_MFW_HND parent, char * str1, char * str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION("mmi_btips_app_ask_info");
	dlg_initDisplayData_TextStr( &display_info, TxtYes, TxtNo, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, FOREVER, KEY_LEFT|KEY_RIGHT  | KEY_CLEAR | KEY_HUP);
	 return info_dialog(parent,&display_info);
}

GLOBAL int mmi_btips_services(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfw_parent(mfw_header());

	BOOL discoverServices = FALSE ;

	BOOL status;
			
	TRACE_EVENT_P1 ("mmi_btips_services()  %d",discoverServices);
	
	status = mmi_btips_bmgDeviceShowServices(parent_win, discoverServices);
	
	if(status == SERVICESMASK_EMPTY)
		{	
			discoverServices = TRUE;
			g_win_infodialog = mmi_btips_app_show_info(0,TxtBtipsRefreshing,NULL,TEN_SECS,NULL);
			mmi_btips_bmgDeviceShowServices(parent_win, discoverServices);
		}
	
	return MFW_EVENT_CONSUMED;
}

#endif
